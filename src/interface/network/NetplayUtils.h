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

struct addrinfo;

namespace NETPLAY
{
  typedef struct global
  {
    uint32_t content_crc;

    struct
    {
       std::string server;
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
       //char movie_path[PATH_MAX_LENGTH];
       bool movie_playback;
       bool eof_exit;

       /* Immediate playback/recording. */
       //char movie_start_path[PATH_MAX_LENGTH];
       bool movie_start_recording;
       bool movie_end;
    } bsv;
  } global_t;

  extern global_t *global_global;

  class CSocket;

  class CNetplayUtils
  {
  public:
    /**
     * init_netplay:
     *
     * Initializes netplay.
     *
     * If netplay is already initialized, will return false (0).
     *
     * Returns: true (1) if successful, otherwise false (0).
     **/
    static bool init_netplay(void);

    static void deinit_netplay(void);

    static void input_poll_net(void);

    static int16_t input_state_net(unsigned port, unsigned device, unsigned idx, unsigned id);

    static void video_frame_net(const void *data, unsigned width, unsigned height, size_t pitch);

    static void audio_sample_net(int16_t left, int16_t right);

    static size_t audio_sample_batch_net(const int16_t *data, size_t frames);

    static int16_t input_state_spectate(unsigned port, unsigned device, unsigned idx, unsigned id);

    static int16_t input_state_spectate_client(unsigned port, unsigned device, unsigned idx, unsigned id);

    /**
     * implementation_magic_value:
     *
     * Not really a hash, but should be enough to differentiate
     * implementations from each other.
     *
     * Subtle differences in the implementation will not be possible to spot.
     * The alternative would have been checking serialization sizes, but it
     * was troublesome for cross platform compat.
     **/
    static uint32_t implementation_magic_value(void);

    static uint32_t *bsv_header_generate(size_t *size, uint32_t magic);

    static bool bsv_parse_header(const uint32_t *header, uint32_t magic);
  };
}
