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
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#define UDP_FRAME_PACKETS 16
#define MAX_SPECTATORS 16

struct addrinfo;

typedef void (*retro_video_refresh_t)(const void *data, unsigned width, unsigned height, size_t pitch);
typedef void (*retro_audio_sample_t)(int16_t left, int16_t right);
typedef size_t (*retro_audio_sample_batch_t)(const int16_t *data, size_t frames);
typedef int16_t (*retro_input_state_t)(unsigned port, unsigned device, unsigned index, unsigned id);
typedef void (*retro_input_poll_t)(void);

typedef struct retro_callbacks
{
   retro_video_refresh_t frame_cb;
   retro_audio_sample_t sample_cb;
   retro_audio_sample_batch_t sample_batch_cb;
   retro_input_state_t state_cb;
   retro_input_poll_t poll_cb;
} retro_callbacks_t;

namespace NETPLAY
{
  class CSocket;
  class IGame;

  class CNetplay
  {
  public:
    /**
     * @server               : IP address of server.
     * @port                 : Port of server.
     * @frames               : Amount of lag frames.
     * @cb                   : Libretro callbacks.
     * @spectate             : If true, enable spectator mode.
     * @nick                 : Nickname of user.
     *
     * Creates a new netplay handle. A NULL host means we're
     * hosting (user 1).
     **/
    CNetplay(IGame* game,
             const std::string& server,
             uint16_t port,
             unsigned frames,
             const struct retro_callbacks *cb,
             bool spectate,
             const std::string& nick);

    ~CNetplay(void);

    bool Initialize(void);

    bool IsInitialized(void) const { return m_bIsInitialized; }

    /**
     * flip_users:
     *
     * On regular netplay, flip who controls user 1 and 2.
     **/
    void flip_users(void);

    /**
     * pre_frame:
     *
     * Pre-frame for Netplay.
     * Call this before running retro_run().
     **/
    void pre_frame(void);

    /**
     * post_frame:
     *
     * Post-frame for Netplay.
     * We check if we have new input and replay from recorded input.
     * Call this after running retro_run().
     **/
    void post_frame(void);

    /**
     * should_skip:
     *
     * If we're fast-forward replaying to resync, check if we
     * should actually show frame.
     *
     * Returns: bool (1) if we should skip this frame, otherwise
     * false (0).
     **/
    bool should_skip(void);

    /**
     * poll:
     *
     * Polls network to see if we have anything new. If our
     * network buffer is full, we simply have to block
     * for new input data.
     *
     * Returns: true (1) if successful, otherwise false (0).
     **/
    bool poll(void);

    void set_spectate_input(int16_t input);

    int16_t get_spectate_input(bool port, unsigned device, unsigned idx, unsigned id);

    /**
     * is_alive:
     *
     * Checks if input port/index is controlled by netplay or not.
     *
     * Returns: true (1) if alive, otherwise false (0).
     **/
    bool is_alive(void);

    int16_t input_state(bool port, unsigned device, unsigned idx, unsigned id);

    bool CanPoll(void);

    IGame* const m_game;
    struct retro_callbacks cbs;

  private:
    struct delta_frame
    {
       uint8_t *state;

       uint16_t real_input_state;
       uint16_t simulated_input_state;
       uint16_t self_state;

       bool is_simulated;
       bool used_real;
    };

    bool send_chunk(void);

    /**
     * get_self_input_state:
     *
     * Grab our own input state and send this over the network.
     *
     * Returns: true (1) if successful, otherwise false (0).
     **/
    bool get_self_input_state(void);

    bool cmd_ack(void);

    bool cmd_nak(void);

    bool get_response(void);

    bool get_cmd(void);

    int poll_input(bool block);

    bool receive_data(uint8_t *buffer, size_t size);

    void parse_packet(uint32_t *buffer, unsigned size);

    void simulate_input(void);

    bool flip_port(bool port);

    bool init_socket(const std::string& server, uint16_t port);

    bool send_nickname(CSocket* fd);

    bool get_nickname(CSocket* fd);

    bool send_info(void);

    bool get_info(void);

    bool get_info_spectate(void);

    bool init_buffers(void);

    bool send_cmd(uint32_t cmd, const void *data, size_t size);

    void pre_frame_net(void);

    /**
     * pre_frame_spectate:
     *
     * Pre-frame for Netplay (spectate mode version).
     **/
    void pre_frame_spectate(void);

    /**
     * post_frame_net:
     *
     * Post-frame for Netplay (normal version).
     * We check if we have new input and replay from recorded input.
     **/
    void post_frame_net(void);

    /**
     * post_frame_spectate:
     *
     * Post-frame for Netplay (spectate mode version).
     * We check if we have new input and replay from recorded input.
     **/
    void post_frame_spectate(void);

    const std::string m_server;
    unsigned int m_frames;
    bool m_bIsInitialized;

    std::string nick;
    std::string other_nick;
    struct sockaddr_storage other_addr;

    /* TCP connection for state sending, etc. Also used for commands */
    CSocket *fd;
    /* UDP connection for game state updates. */
    CSocket *udp_fd;
    /* Which port is governed by netplay (other user)? */
    unsigned port;
    bool has_connection;

    struct delta_frame *buffer;
    size_t buffer_size;

    /* Pointer where we are now. */
    size_t self_ptr;
    /* Points to the last reliable state that self ever had. */
    size_t other_ptr;
    /* Pointer to where we are reading.
     * Generally, other_ptr <= read_ptr <= self_ptr. */
    size_t read_ptr;
    /* A temporary pointer used on replay. */
    size_t tmp_ptr;

    size_t state_size;

    /* Are we replaying old frames? */
    bool is_replay;
    /* We don't want to poll several times on a frame. */
    bool can_poll;

    /* To compat UDP packet loss we also send
     * old data along with the packets. */
    uint32_t packet_buffer[UDP_FRAME_PACKETS * 2];
    uint32_t frame_count;
    uint32_t read_frame_count;
    uint32_t other_frame_count;
    uint32_t tmp_frame_count;
    struct addrinfo *addr;
    struct sockaddr_storage their_addr;
    bool has_client_addr;

    unsigned timeout_cnt;

    /* Spectating. */
    bool spectate;
    bool spectate_client;
    CSocket* spectate_fds[MAX_SPECTATORS];
    uint16_t *spectate_input;
    size_t spectate_input_ptr;
    size_t spectate_input_size;

    /* User flipping
     * Flipping state. If ptr >= flip_frame, we apply the flip.
     * If not, we apply the opposite, effectively creating a trigger point.
     * To avoid collition we need to make sure our client/host is synced up
     * well after flip_frame before allowing another flip. */
    bool flip;
    uint32_t flip_frame;
  };
}
