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
 *  along with this Program; see the file COPYING. If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include <stdint.h>
#include <time.h>

struct stat64;

namespace NETPLAY
{
  struct STAT_STRUCTURE
  {
    uint32_t    deviceId;         // ID of device containing file
    uint64_t    size;             // Total size, in bytes
#if defined(_WIN32)
    __time64_t  accessTime;       // Time of last access
    __time64_t  modificationTime; // Time of last modification
    __time64_t  statusTime;       // Time of last status change
#else
    timespec    accessTime;       // Time of last access
    timespec    modificationTime; // Time of last modification
    timespec    statusTime;       // Time of last status change
#endif
    bool        isDirectory;      // The stat url is a directory
    bool        isSymLink;        // The stat url is a symbolic link
    bool        isHidden;         // The file is hidden
  };

  class StatTranslator
  {
  public:
    static void TranslateToStruct64(const STAT_STRUCTURE& output, struct stat64& buffer);
  };
}
