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

#include "NetplayUtils.h"
#include "Netplay.h"
#include "Socket.h"
#include "interface/IGame.h"
#include "log/Log.h"
#include "utils/retro_endianness.h"

#include "kodi/kodi_game_types.h"

/* TODO
#include "Network.h"
#include "Socket.h"
#include "interface/IGame.h"
#include "log/Log.h"
#include "utils/retro_endianness.h"

#include <net/net_compat.h>
#include "netplay.h"
#include "general.h"
#include "autosave.h"
#include "dynamic.h"
#include "msg_hash.h"
#include "system.h"

#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
*/

#include <netdb.h>
#include <string>
#include <sys/socket.h>

using namespace NETPLAY;

#define NETPLAY_API_VERSION  1
#define PACKAGE_VERSION      "1.0.0"

#define MAGIC_INDEX       0
#define SERIALIZER_INDEX  1
#define CRC_INDEX         2
#define STATE_SIZE_INDEX  3

#define RARCH_DEFAULT_PORT 55435

#define BSV_MAGIC         0x42535631


#ifndef PATH_MAX_LENGTH
  #define PATH_MAX_LENGTH 4096
#endif

namespace NETPLAY
{
  global_t *global_global = NULL; // TODO
  CNetplay* driver_netplay = NULL;
  std::string settings_username; // TODO
  const char* info_library_name = "library_name"; // TODO
  const char* info_library_version = "library_version"; // TODO
}

bool CNetplayUtils::init_netplay(void)
{
  struct retro_callbacks cbs = {0}; // TODO

  //retro_set_default_callbacks(&cbs); // TODO

  if (!global_global->netplay.server.empty())
  {
    dsyslog("Connecting to netplay host...");
    global_global->netplay.is_client = true;
  }
  else
    dsyslog("Waiting for client...");

  driver_netplay = new CNetplay(NULL, // TODO
         global_global->netplay.is_client ? global_global->netplay.server : NULL,
         global_global->netplay.port ? global_global->netplay.port : RARCH_DEFAULT_PORT,
         global_global->netplay.sync_frames, &cbs, global_global->netplay.is_spectate,
         settings_username);

  if (driver_netplay->Initialize())
    return true;

  global_global->netplay.is_client = false;
  esyslog("Failed to create netplay");

  return false;
}

void CNetplayUtils::deinit_netplay(void)
{
  if (driver_netplay)
  {
    delete driver_netplay;
    driver_netplay = NULL;
  }
}

void CNetplayUtils::input_poll_net(void)
{
  if (!driver_netplay->should_skip() && driver_netplay->CanPoll())
    driver_netplay->poll();
}

int16_t CNetplayUtils::input_state_net(unsigned port, unsigned device, unsigned idx, unsigned id)
{
  if (driver_netplay->is_alive())
    return driver_netplay->input_state(port, device, idx, id);

  return driver_netplay->cbs.state_cb(port, device, idx, id);
}

void CNetplayUtils::video_frame_net(const void *data, unsigned width, unsigned height, size_t pitch)
{
  if (!driver_netplay->should_skip())
    driver_netplay->cbs.frame_cb(data, width, height, pitch);
}

void CNetplayUtils::audio_sample_net(int16_t left, int16_t right)
{
  if (!driver_netplay->should_skip())
    driver_netplay->cbs.sample_cb(left, right);
}

size_t CNetplayUtils::audio_sample_batch_net(const int16_t *data, size_t frames)
{
  if (!driver_netplay->should_skip())
    return driver_netplay->cbs.sample_batch_cb(data, frames);

  return frames;
}

int16_t CNetplayUtils::input_state_spectate(unsigned port, unsigned device, unsigned idx, unsigned id)
{
  int16_t res = driver_netplay->cbs.state_cb(port, device, idx, id);

  driver_netplay->set_spectate_input(res);

  return res;
}

int16_t CNetplayUtils::input_state_spectate_client(unsigned port, unsigned device, unsigned idx, unsigned id)
{
  return driver_netplay->get_spectate_input(port, device, idx, id);
}

uint32_t CNetplayUtils::implementation_magic_value(void)
{
  std::string name = "Library name"; // TODO
  std::string version = "Library version"; // TOOD

  uint32_t res = 0;

  res |= NETPLAY_API_VERSION;

  for (unsigned i = 0; i < name.length(); i++)
    res ^= name[i] << (i & 0xf);

  for (unsigned i = 0; i < version.length(); i++)
    res ^= version[i] << (i & 0xf);

  static std::string pkgVersion(PACKAGE_VERSION);
  for (unsigned i = 0; i < pkgVersion.length(); i++)
    res ^= pkgVersion[i] << ((i & 0xf) + 16);

  return res;
}

uint32_t *CNetplayUtils::bsv_header_generate(size_t *size, uint32_t magic)
{
  uint32_t *header, bsv_header[4] = {0};
  size_t serialize_size = driver_netplay->m_game->SerializeSize();
  size_t header_size = sizeof(bsv_header) + serialize_size;

  *size = header_size;

  header = static_cast<uint32_t*>(malloc(header_size));
  if (!header)
    return NULL;

  bsv_header[MAGIC_INDEX]      = swap_if_little32(BSV_MAGIC);
  bsv_header[SERIALIZER_INDEX] = swap_if_big32(magic);
  bsv_header[CRC_INDEX]        = swap_if_big32(global_global->content_crc);
  bsv_header[STATE_SIZE_INDEX] = swap_if_big32(serialize_size);

  if (serialize_size && driver_netplay->m_game->Serialize(reinterpret_cast<uint8_t*>(header + 4), serialize_size) != GAME_ERROR_NO_ERROR)
  {
    free(header);
    return NULL;
  }

  memcpy(header, bsv_header, sizeof(bsv_header));
  return header;
}

bool CNetplayUtils::bsv_parse_header(const uint32_t *header, uint32_t magic)
{
  uint32_t in_crc;
  uint32_t in_magic;
  uint32_t in_state_size;
  uint32_t in_bsv = swap_if_little32(header[MAGIC_INDEX]);

  if (in_bsv != BSV_MAGIC)
  {
    esyslog("BSV magic mismatch, got 0x%x, expected 0x%x", in_bsv, BSV_MAGIC);
    return false;
  }

  in_magic = swap_if_big32(header[SERIALIZER_INDEX]);
  if (in_magic != magic)
  {
    esyslog("Magic mismatch, got 0x%x, expected 0x%x", in_magic, magic);
    return false;
  }

  in_crc = swap_if_big32(header[CRC_INDEX]);
  if (in_crc != global_global->content_crc)
  {
    esyslog("CRC32 mismatch, got 0x%x, expected 0x%x", in_crc, global_global->content_crc);
    return false;
  }

  in_state_size = swap_if_big32(header[STATE_SIZE_INDEX]);
  if (in_state_size != driver_netplay->m_game->SerializeSize())
  {
    esyslog("Serialization size mismatch, got 0x%x, expected 0x%x",
            (unsigned)in_state_size, (unsigned)driver_netplay->m_game->SerializeSize());
    return false;
  }

  return true;
}
