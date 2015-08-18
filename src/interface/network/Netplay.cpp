/*
 *      Copyright (C) 2015 Garrett Brown
 *      Copyright (C) 2015 Team XBMC
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#if defined(_MSC_VER)
  #pragma comment(lib, "ws2_32")
#endif

#include "Netplay.h"
#include "NetplayUtils.h"
#include "Network.h"
#include "Socket.h"
#include "interface/IGame.h"
#include "log/Log.h"
#include "utils/retro_endianness.h"

#include "kodi/kodi_game_types.h"

/*
#include <net/net_compat.h>
#include "netplay.h"
#include "general.h"
#include "autosave.h"
#include "dynamic.h"
#include "msg_hash.h"
#include "system.h"
*/

#include <algorithm>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

using namespace NETPLAY;

#define RETRO_DEVICE_JOYPAD       1

#define NETPLAY_CMD_ACK 0
#define NETPLAY_CMD_NAK 1
#define NETPLAY_CMD_FLIP_PLAYERS 2

#define POLL_INPUT_MAX_RETRIES 16
#define POLL_INPUT_RETRY_MS    500

#ifndef PATH_MAX_LENGTH
  #define PATH_MAX_LENGTH 4096
#endif

#define PREV_PTR(x) ((x) == 0 ? this->buffer_size - 1 : (x) - 1)
#define NEXT_PTR(x) ((x + 1) % this->buffer_size)

// RetroArch specific bind IDs
#define RARCH_FIRST_CUSTOM_BIND 16

enum
{
   /* Custom binds that extend the scope of RETRO_DEVICE_JOYPAD for
    * RetroArch specifically.
    * Analogs (RETRO_DEVICE_ANALOG) */
   RARCH_ANALOG_LEFT_X_PLUS = RARCH_FIRST_CUSTOM_BIND,
   RARCH_ANALOG_LEFT_X_MINUS,
   RARCH_ANALOG_LEFT_Y_PLUS,
   RARCH_ANALOG_LEFT_Y_MINUS,
   RARCH_ANALOG_RIGHT_X_PLUS,
   RARCH_ANALOG_RIGHT_X_MINUS,
   RARCH_ANALOG_RIGHT_Y_PLUS,
   RARCH_ANALOG_RIGHT_Y_MINUS,

   /* Turbo */
   RARCH_TURBO_ENABLE,

   RARCH_CUSTOM_BIND_LIST_END,
};

#define RARCH_FIRST_META_KEY RARCH_CUSTOM_BIND_LIST_END

typedef struct global
{
  uint32_t content_crc;

  struct
  {
     char server[PATH_MAX_LENGTH];
     bool enable;
     bool is_client;
     bool is_spectate;
     unsigned sync_frames;
     unsigned port;
  } netplay;

  struct
  {
     /* Movie playback/recording support. */
     void *movie;
     char movie_path[PATH_MAX_LENGTH];
     bool movie_playback;
     bool eof_exit;

     /* Immediate playback/recording. */
     char movie_start_path[PATH_MAX_LENGTH];
     bool movie_start_recording;
     bool movie_end;
  } bsv;
} global_t;

namespace NETPLAY
{
  bool global_block_libretro_input = false; // TODO
  bool settings_input_netplay_client_swap_input = false; // TODO
}

/**
 * warn_hangup:
 *
 * Warns that netplay has disconnected.
 **/
static void warn_hangup(void)
{
   dsyslog("Netplay has disconnected. Will continue without connection...");
}

CNetplay::CNetplay(IGame* game,
                   const std::string& server,
                   uint16_t port,
                   unsigned frames,
                   const struct retro_callbacks *cb,
                   bool spectate,
                   const std::string& nick) :
    m_game(game),
    m_server(server),
    m_frames(std::min(frames, (unsigned)UDP_FRAME_PACKETS)),
    m_bIsInitialized(false)
{
  this->fd              = NULL;
  this->udp_fd          = NULL;
  this->cbs             = *cb;
  this->port            = !server.empty() ? 0 : 1;
  this->spectate        = spectate;
  this->spectate_client = !server.empty();
  this->nick            = nick;
}

bool CNetplay::Initialize(void)
{
  if (!init_socket(m_server, port))
    return false;

  if (spectate)
  {
    if (!m_server.empty())
    {
      if (!get_info_spectate())
        goto error;
    }

    for (unsigned i = 0; i < MAX_SPECTATORS; i++)
      this->spectate_fds[i] = NULL;
  }
  else
  {
    if (!m_server.empty())
    {
      if (!send_info())
        goto error;
    }
    else
    {
      if (!get_info())
        goto error;
    }

    this->buffer_size = m_frames + 1;

    if (!init_buffers())
      goto error;

    this->has_connection = true;
  }

  return true;

error:
  if (this->fd != NULL)
    this->fd->Shutdown();
  if (this->udp_fd != NULL)
    this->udp_fd->Shutdown();

  return false;
}

CNetplay::~CNetplay(void)
{
  unsigned i;

  if (this->fd)
  {
    this->fd->Shutdown();
    delete this->fd;
    this->fd = NULL;
  }

  if (this->spectate)
  {
    for (i = 0; i < MAX_SPECTATORS; i++)
    {
      if (this->spectate_fds[i])
      {
        this->spectate_fds[i]->Shutdown();
        delete this->spectate_fds[i];
        this->spectate_fds[i] = NULL;
      }
    }

    free(this->spectate_input);
  }
  else
  {
    if (this->udp_fd)
    {
      this->udp_fd->Shutdown();
      delete this->udp_fd;
      this->udp_fd = NULL;
    }

    for (i = 0; i < this->buffer_size; i++)
      free(this->buffer[i].state);

    free(this->buffer);
  }

  if (this->addr)
    freeaddrinfo(this->addr);
}

void CNetplay::flip_users(void)
{
  uint32_t flip_frame     = this->frame_count + 2 * UDP_FRAME_PACKETS;
  uint32_t flip_frame_net = htonl(flip_frame);

  if (this->spectate)
  {
    esyslog("Cannot flip users in spectate mode");
    return;
  }

  if (this->port == 0)
  {
    esyslog("Cannot flip users if you're not the host");
    return;
  }

  // Make sure both clients are definitely synced up
  if (this->frame_count < (this->flip_frame + 2 * UDP_FRAME_PACKETS))
  {
    esyslog("Cannot flip users yet. Wait a second or two before attempting flip");
    return;
  }

  if (send_cmd(NETPLAY_CMD_FLIP_PLAYERS, &flip_frame_net, sizeof(flip_frame_net)) && get_response())
  {
    dsyslog("Netplay users are flipped");

    // Queue up a flip well enough in the future
    this->flip ^= true;
    this->flip_frame = flip_frame;
  }
  else
  {
    esyslog("Failed to flip users");
  }
}

void CNetplay::pre_frame(void)
{
  if (this->spectate)
    pre_frame_spectate();
  else
    pre_frame_net();
}

void CNetplay::post_frame(void)
{
  if (this->spectate)
    post_frame_spectate();
  else
    post_frame_net();
}

bool CNetplay::should_skip(void)
{
  if (!IsInitialized())
    return false;

  return this->is_replay && this->has_connection;
}

bool CNetplay::poll(void)
{
  int res;

  if (!this->has_connection)
    return false;

  this->can_poll = false;

  if (!get_self_input_state())
    return false;

  // We skip reading the first frame so the host has a chance to grab our host
  // info so we don't block forever :')
  if (this->frame_count == 0)
  {
    this->buffer[0].used_real        = true;
    this->buffer[0].is_simulated     = false;
    this->buffer[0].real_input_state = 0;
    this->read_ptr                   = NEXT_PTR(this->read_ptr);
    this->read_frame_count++;
    return true;
  }

  // We might have reached the end of the buffer, where we simply have to block
  res = poll_input(this->other_ptr == this->self_ptr);
  if (res == -1)
  {
    this->has_connection = false;
    warn_hangup();
    return false;
  }

  if (res == 1)
  {
    uint32_t first_read = this->read_frame_count;
    do
    {
      uint32_t buffer[UDP_FRAME_PACKETS * 2];
      if (!receive_data(reinterpret_cast<uint8_t*>(buffer), sizeof(buffer)))
      {
        warn_hangup();
        this->has_connection = false;
        return false;
      }
      parse_packet(buffer, UDP_FRAME_PACKETS);

    } while ((this->read_frame_count <= this->frame_count) &&
             poll_input((this->other_ptr == this->self_ptr) &&
             (first_read == this->read_frame_count)) == 1);
  }
  else
  {
    // Cannot allow this. Should not happen though
    if (this->self_ptr == this->other_ptr)
    {
      warn_hangup();
      return false;
    }
  }

  if (this->read_ptr != this->self_ptr)
    simulate_input();
  else
    this->buffer[PREV_PTR(this->self_ptr)].used_real = true;

  return true;
}

void CNetplay::set_spectate_input(int16_t input)
{
  if (this->spectate_input_ptr >= this->spectate_input_size)
  {
    this->spectate_input_size++;
    this->spectate_input_size *= 2;
    this->spectate_input = (uint16_t*)realloc(this->spectate_input, this->spectate_input_size * sizeof(uint16_t));
  }

  this->spectate_input[this->spectate_input_ptr++] = swap_if_big16(input);
}

int16_t CNetplay::get_spectate_input(bool port, unsigned device, unsigned idx, unsigned id)
{
  int16_t inp;

  if (this->fd->receive_all_blocking(reinterpret_cast<uint8_t*>(&inp), sizeof(inp)))
    return swap_if_big16(inp);

  esyslog("Connection with host was cut");

  //pretro_set_input_state(this->cbs.state_cb); // TODO
  return this->cbs.state_cb(port, device, idx, id);
}

bool CNetplay::is_alive(void)
{
  if (!IsInitialized())
    return false;

  return this->has_connection;
}

int16_t CNetplay::input_state(bool port, unsigned device, unsigned idx, unsigned id)
{
  size_t ptr = this->is_replay ? this->tmp_ptr : PREV_PTR(this->self_ptr);
  uint16_t curr_input_state = this->buffer[ptr].self_state;

  if (this->port == (flip_port(port) ? 1 : 0))
  {
    if (this->buffer[ptr].is_simulated)
      curr_input_state = this->buffer[ptr].simulated_input_state;
    else
      curr_input_state = this->buffer[ptr].real_input_state;
  }

  return ((1 << id) & curr_input_state) ? 1 : 0;
}

bool CNetplay::CanPoll(void)
{
  if (!IsInitialized())
    return false;

  return this->can_poll;
}

bool CNetplay::send_chunk(void)
{
  const struct sockaddr *addr = NULL;

  if (this->addr)
    addr = this->addr->ai_addr;
  else if (this->has_client_addr)
    addr = (const struct sockaddr*)&this->their_addr;

  if (addr)
  {
    if (this->udp_fd->SendTo(reinterpret_cast<const uint8_t*>(this->packet_buffer),
               sizeof(this->packet_buffer), 0, addr,
               sizeof(struct sockaddr)))
    {
      warn_hangup();
      this->has_connection = false;
      return false;
    }
  }

  return true;
}

bool CNetplay::get_self_input_state(void)
{
  unsigned i;
  uint32_t state          = 0;
  struct delta_frame *ptr = &this->buffer[this->self_ptr];

  if (!global_block_libretro_input && this->frame_count > 0)
  {
    // First frame we always give zero input since relying on input from first
    // frame screws up when we use -F 0
    for (i = 0; i < RARCH_FIRST_META_KEY; i++)
    {
      int16_t tmp = this->cbs.state_cb(settings_input_netplay_client_swap_input ?
             0 : !this->port, RETRO_DEVICE_JOYPAD, 0, i);
      state |= tmp ? 1 << i : 0;
    }
  }

  memmove(this->packet_buffer, this->packet_buffer + 2, sizeof (this->packet_buffer) - 2 * sizeof(uint32_t));
  this->packet_buffer[(UDP_FRAME_PACKETS - 1) * 2] = htonl(this->frame_count);
  this->packet_buffer[(UDP_FRAME_PACKETS - 1) * 2 + 1] = htonl(state);

  if (!send_chunk())
  {
    warn_hangup();
    this->has_connection = false;
    return false;
  }

  ptr->self_state = state;
  this->self_ptr = NEXT_PTR(this->self_ptr);
  return true;
}

bool CNetplay::cmd_ack(void)
{
  uint32_t cmd = htonl(NETPLAY_CMD_ACK);
  return this->fd->send_all_blocking(&cmd, sizeof(cmd));
}

bool CNetplay::cmd_nak(void)
{
  uint32_t cmd = htonl(NETPLAY_CMD_NAK);
  return this->fd->send_all_blocking(&cmd, sizeof(cmd));
}

bool CNetplay::get_response(void)
{
  uint32_t response;
  if (!this->fd->receive_all_blocking(reinterpret_cast<uint8_t*>(&response), sizeof(response)))
    return false;

  return ntohl(response) == NETPLAY_CMD_ACK;
}

bool CNetplay::get_cmd(void)
{
  uint32_t cmd, flip_frame;
  size_t cmd_size;

  if (!this->fd->receive_all_blocking(reinterpret_cast<uint8_t*>(&cmd), sizeof(cmd)))
    return false;

  cmd = ntohl(cmd);

  cmd_size = cmd & 0xffff;
  cmd      = cmd >> 16;

  switch (cmd)
  {
    case NETPLAY_CMD_FLIP_PLAYERS:
      if (cmd_size != sizeof(uint32_t))
      {
        esyslog("CMD_FLIP_PLAYERS has unexpected command size");
        return cmd_nak();
      }

      if (!this->fd->receive_all_blocking(reinterpret_cast<uint8_t*>(&flip_frame), sizeof(flip_frame)))
      {
        esyslog("Failed to receive CMD_FLIP_PLAYERS argument");
        return cmd_nak();
      }

      flip_frame = ntohl(flip_frame);

      if (flip_frame < this->flip_frame)
      {
        esyslog("Host asked us to flip users in the past. Not possible ...");
        return cmd_nak();
      }

      this->flip ^= true;
      this->flip_frame = flip_frame;

      dsyslog("Netplay users are flipped");

      return cmd_ack();

    default:
      break;
  }

  esyslog("Unknown netplay command received");
  return cmd_nak();
}

int CNetplay::poll_input(bool block)
{
  int max_fd        = this->fd->max_fd(this->udp_fd) + 1;
  struct timeval tv = {0};
  tv.tv_sec         = 0;
  tv.tv_usec        = block ? (POLL_INPUT_RETRY_MS * 1000) : 0;

  do
  {
    fd_set fds;
    // select() does not take pointer to const struct timeval
    // Technically possible for select() to modify tmp_tv, so we go paranoia mode
    struct timeval tmp_tv = tv;

    this->timeout_cnt++;

    FD_ZERO(&fds);
    this->udp_fd->Set(&fds);
    this->fd->Set(&fds);

    if (CSocket::select_sockets(max_fd, &fds, NULL, NULL, &tmp_tv) < 0)
      return -1;

    // Somewhat hacky, but we aren't using the TCP connection for anything useful atm
    if (this->fd->IsSet(&fds) && !get_cmd())
      return -1;

    if (this->udp_fd->IsSet(&fds))
      return 1;

    if (!block)
      continue;

    if (!send_chunk())
    {
      warn_hangup();
      this->has_connection = false;
      return -1;
    }

    dsyslog("Network is stalling, resending packet... Count %u of %d ...",
            this->timeout_cnt, POLL_INPUT_MAX_RETRIES);
  } while ((this->timeout_cnt < POLL_INPUT_MAX_RETRIES) && block);

  if (block)
    return -1;

  return 0;
}

bool CNetplay::receive_data(uint8_t *buffer, size_t size)
{
  unsigned int addrlen = sizeof(this->their_addr);

  if (!this->udp_fd->ReceiveFrom(buffer, size, 0,
         reinterpret_cast<struct sockaddr*>(&this->their_addr), &addrlen))
    return false;

  this->has_client_addr = true;

  return true;
}

void CNetplay::parse_packet(uint32_t *buffer, unsigned size)
{
  unsigned i;

  for (i = 0; i < size * 2; i++)
    buffer[i] = ntohl(buffer[i]);

  for (i = 0; i < size && this->read_frame_count <= this->frame_count; i++)
  {
    uint32_t frame = buffer[2 * i + 0];
    uint32_t state = buffer[2 * i + 1];

    if (frame != this->read_frame_count)
      continue;

    this->buffer[this->read_ptr].is_simulated = false;
    this->buffer[this->read_ptr].real_input_state = state;
    this->read_ptr = NEXT_PTR(this->read_ptr);
    this->read_frame_count++;
    this->timeout_cnt = 0;
  }
}

/* TODO: Somewhat better prediction. :P */
void CNetplay::simulate_input(void)
{
  size_t ptr  = PREV_PTR(this->self_ptr);
  size_t prev = PREV_PTR(this->read_ptr);

  this->buffer[ptr].simulated_input_state = this->buffer[prev].real_input_state;
  this->buffer[ptr].is_simulated = true;
  this->buffer[ptr].used_real = false;
}

bool CNetplay::flip_port(bool port)
{
  size_t frame = this->frame_count;

  if (this->flip_frame == 0)
    return port;

  if (this->is_replay)
    frame = this->tmp_frame_count;

  return port ^ this->flip ^ (frame < this->flip_frame);
}

bool CNetplay::init_socket(const std::string& server, uint16_t port)
{
  if (!CNetwork::Get().Initialize())
    return false;

  this->fd = new CSocket(server, port, SOCKET_TYPE_TCP);
  if (!this->fd->Connect())
  {
    esyslog("Failed to initialize TCP socket");
    delete this->fd;
    this->fd = NULL;
    return false;
  }

  this->udp_fd = new CSocket(server, port, SOCKET_TYPE_UDP);
  if (!this->udp_fd->Connect())
  {
    esyslog("Failed to initialize UDP socket");
    delete this->udp_fd;
    this->udp_fd = NULL;
    return false;
  }

  return true;
}

bool CNetplay::send_nickname(CSocket* fd)
{
  uint8_t nick_size = this->nick.size();

  if (!fd->send_all_blocking(&nick_size, sizeof(nick_size)))
  {
    esyslog("Failed to send nick size");
    return false;
  }

  if (!fd->send_all_blocking(this->nick.c_str(), nick_size))
  {
    esyslog("Failed to send nick");
    return false;
  }

  return true;
}

bool CNetplay::get_nickname(CSocket* fd)
{
  uint8_t nick_size;

  if (!fd->receive_all_blocking(&nick_size, sizeof(nick_size)))
  {
    esyslog("Failed to receive nick size from host");
    return false;
  }

  if (nick_size >= 32)
  {
    esyslog("Invalid nick size");
    return false;
  }

  char buffer[32];

  if (!fd->receive_all_blocking(reinterpret_cast<uint8_t*>(buffer), nick_size))
  {
    esyslog("Failed to receive nick");
    return false;
  }

  this->other_nick = buffer;

  return true;
}

bool CNetplay::send_info(void)
{
  size_t sram_size;
  uint8_t *sram         = NULL;
  uint32_t header[3] = {0};

  m_game->GetMemory(GAME_MEMORY_SAVE_RAM, const_cast<const uint8_t**>(&sram), &sram_size); // TODO: constness

  header[0] = htonl(global_global->content_crc);
  header[1] = htonl(CNetplayUtils::implementation_magic_value());
  header[2] = htonl(sram_size);

  if (!this->fd->send_all_blocking(header, sizeof(header)))
    return false;

  if (!send_nickname(this->fd))
  {
    esyslog("Failed to send nick to host");
    return false;
  }

  // Get SRAM data from User 1
  if (!this->fd->receive_all_blocking(sram, sram_size))
  {
    esyslog("Failed to receive SRAM data from host");
    return false;
  }

  if (!get_nickname(this->fd))
  {
    esyslog("Failed to receive nick from host");
    return false;
  }

  dsyslog("Connected to: \"%s\"", this->other_nick.c_str());

  return true;
}

bool CNetplay::get_info(void)
{
  size_t sram_size = 0;
  uint32_t header[3];
  const uint8_t *sram = NULL;

  if (!this->fd->receive_all_blocking(reinterpret_cast<uint8_t*>(header), sizeof(header)))
  {
    esyslog("Failed to receive header from client");
    return false;
  }

  if (global_global->content_crc != ntohl(header[0]))
  {
    esyslog("Content CRC32s differ. Cannot use different games");
    return false;
  }

  if (CNetplayUtils::implementation_magic_value() != ntohl(header[1]))
  {
    esyslog("Implementations differ, make sure you're using exact same libretro implementations and RetroArch version");
    return false;
  }

  m_game->GetMemory(GAME_MEMORY_SAVE_RAM, &sram, &sram_size);

  if (sram_size != ntohl(header[2]))
  {
    esyslog("Content SRAM sizes do not correspond");
    return false;
  }

  if (!get_nickname(this->fd))
  {
    esyslog("Failed to get nickname from client");
    return false;
  }

  // Send SRAM data to our User 2
  if (!this->fd->send_all_blocking(sram, sram_size))
  {
    esyslog("Failed to send SRAM data to client");
    return false;
  }

  if (!send_nickname(this->fd))
  {
    esyslog("Failed to send nickname to client");
    return false;
  }

  return true;
}

bool CNetplay::get_info_spectate(void)
{
  size_t save_state_size, size;
  uint8_t *buf          = NULL;
  uint32_t header[4] = {0};
  bool ret           = true;

  if (!send_nickname(this->fd))
  {
    esyslog("Failed to send nickname to host");
    return false;
  }

  if (!get_nickname(this->fd))
  {
    esyslog("Failed to receive nickname from host");
    return false;
  }

  dsyslog("Connected to \"%s\"", this->other_nick.c_str());

  if (!this->fd->receive_all_blocking(reinterpret_cast<uint8_t*>(header), sizeof(header)))
  {
    esyslog("Cannot get header from host");
    return false;
  }

  save_state_size = m_game->SerializeSize();

  if (!CNetplayUtils::bsv_parse_header(header, CNetplayUtils::implementation_magic_value()))
  {
    esyslog("Received invalid BSV header from host");
    return false;
  }

  buf = static_cast<uint8_t*>(malloc(save_state_size));
  if (!buf)
    return false;

  size = save_state_size;

  if (!this->fd->receive_all_blocking(buf, size))
  {
    esyslog("Failed to receive save state from host");
    free(buf);
    return false;
  }

  if (save_state_size)
    ret = m_game->Deserialize(buf, save_state_size);

  free(buf);
  return ret;
}

bool CNetplay::init_buffers(void)
{
  unsigned i;

  if (!IsInitialized())
    return false;

  this->buffer = static_cast<struct delta_frame*>(calloc(this->buffer_size, sizeof(*this->buffer)));

  if (!this->buffer)
    return false;

  this->state_size = m_game->SerializeSize();

  for (i = 0; i < this->buffer_size; i++)
  {
    this->buffer[i].state = static_cast<uint8_t*>(malloc(this->state_size));

    if (!this->buffer[i].state)
      return false;

    this->buffer[i].is_simulated = true;
  }

  return true;
}

bool CNetplay::send_cmd(uint32_t cmd, const void *data, size_t size)
{
  cmd = (cmd << 16) | (size & 0xffff);
  cmd = htonl(cmd);

  if (!this->fd->send_all_blocking(&cmd, sizeof(cmd)))
    return false;

  if (!this->fd->send_all_blocking(data, size))
    return false;

  return true;
}

void CNetplay::pre_frame_net(void)
{
  m_game->Serialize(this->buffer[this->self_ptr].state, this->state_size);

  this->can_poll = true;

  CNetplayUtils::input_poll_net();
}

void CNetplay::pre_frame_spectate(void)
{
  unsigned i;
  uint32_t *header;
  int idx;
  size_t header_size;
  struct sockaddr_storage their_addr;
  unsigned int addr_size;
  fd_set fds;
  struct timeval tmp_tv = {0};

  if (this->spectate_client)
    return;

  FD_ZERO(&fds);
  this->fd->Set(&fds);

  if (CSocket::select_sockets(this->fd->max_fd(NULL) + 1, &fds, NULL, NULL, &tmp_tv) <= 0)
    return;

  if (!this->fd->IsSet(&fds))
    return;

  addr_size = sizeof(their_addr);
  CSocket* new_fd = this->fd->Accept(reinterpret_cast<struct sockaddr*>(&their_addr), &addr_size);
  if (new_fd == NULL)
  {
    esyslog("Failed to accept incoming spectator");
    return;
  }

  idx = -1;
  for (i = 0; i < MAX_SPECTATORS; i++)
  {
    if (this->spectate_fds[i] == NULL)
    {
      idx = i;
      break;
    }
  }

  // No vacant client streams :(
  if (idx == -1)
  {
    new_fd->Shutdown();
    delete new_fd;
    return;
  }

  if (!get_nickname(new_fd))
  {
    esyslog("Failed to get nickname from client");
    new_fd->Shutdown();
    delete new_fd;
    return;
  }

  if (!send_nickname(new_fd))
  {
    esyslog("Failed to send nickname to client");
    new_fd->Shutdown();
    delete new_fd;
    return;
  }

  header = CNetplayUtils::bsv_header_generate(&header_size, CNetplayUtils::implementation_magic_value());

  if (!header)
  {
    esyslog("Failed to generate BSV header");
    new_fd->Shutdown();
    delete new_fd;
    return;
  }

  new_fd->SetSendBuffer(header_size);

  if (!new_fd->send_all_blocking(header, header_size))
  {
    esyslog("Failed to send header to client");
    new_fd->Shutdown();
    delete new_fd;
    free(header);
    return;
  }

  free(header);
  this->spectate_fds[idx] = new_fd;
}

void CNetplay::post_frame_net(void)
{
  this->frame_count++;

  // Nothing to do...
  if (this->other_frame_count == this->read_frame_count)
    return;

  // Skip ahead if we predicted correctly. Skip until our simulation failed.
  while (this->other_frame_count < this->read_frame_count)
  {
    const struct delta_frame *ptr = &this->buffer[this->other_ptr];

    if ((ptr->simulated_input_state != ptr->real_input_state) && !ptr->used_real)
      break;
    this->other_ptr = NEXT_PTR(this->other_ptr);
    this->other_frame_count++;
  }

  if (this->other_frame_count < this->read_frame_count)
  {
    bool first = true;

    // Replay frames
    this->is_replay = true;
    this->tmp_ptr = this->other_ptr;
    this->tmp_frame_count = this->other_frame_count;

    m_game->Deserialize(this->buffer[this->other_ptr].state, this->state_size);

    while (first || (this->tmp_ptr != this->self_ptr))
    {
      m_game->Serialize(this->buffer[this->tmp_ptr].state, this->state_size);
#if defined(HAVE_THREADS) && !defined(RARCH_CONSOLE)
      lock_autosave();
#endif
      m_game->FrameEvent();
#if defined(HAVE_THREADS) && !defined(RARCH_CONSOLE)
      unlock_autosave();
#endif
      this->tmp_ptr = NEXT_PTR(this->tmp_ptr);
      this->tmp_frame_count++;
      first = false;
    }

    this->other_ptr = this->read_ptr;
    this->other_frame_count = this->read_frame_count;
    this->is_replay = false;
  }
}

void CNetplay::post_frame_spectate(void)
{
  unsigned i;

  if (this->spectate_client)
    return;

  for (i = 0; i < MAX_SPECTATORS; i++)
  {
    if (this->spectate_fds[i] == NULL)
      continue;

    if (this->spectate_fds[i]->send_all_blocking(this->spectate_input, this->spectate_input_ptr * sizeof(int16_t)))
      continue;

    dsyslog("Client (#%u) disconnected", i);

    this->spectate_fds[i]->Shutdown();
    delete this->spectate_fds[i];
    this->spectate_fds[i] = NULL;
    break;
  }

  this->spectate_input_ptr = 0;
}
