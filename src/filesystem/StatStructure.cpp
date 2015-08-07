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

#include "StatStructure.h"

#include <fcntl.h>
#include <sys/types.h>

using namespace NETPLAY;

#ifndef S_ISDIR
  #define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif

void StatTranslator::TranslateToStruct(const struct __stat64& buffer, STAT_STRUCTURE& output)
{
  output.deviceId         = buffer.st_dev;
  output.size             = buffer.st_size;
#if defined(_WIN32)
  output.accessTime       = buffer.st_atime;
  output.modificationTime = buffer.st_mtime;
  output.statusTime       = buffer.st_ctime;
#elif defined(__APPLE__)
  output.accessTime       = buffer.st_atimespec;
  output.modificationTime = buffer.st_mtimespec;
  output.statusTime       = buffer.st_ctimespec;
#else
  output.accessTime       = buffer.st_atim;
  output.modificationTime = buffer.st_mtim;
  output.statusTime       = buffer.st_ctim;
#endif
  output.isDirectory      = S_ISDIR(buffer.st_mode);
#if defined(_WIN32)
  output.isSymLink        = false;
#else
  output.isSymLink        = S_ISLNK(buffer.st_mode);
#endif
  output.isHidden         = false; // TODO
}

void StatTranslator::TranslateToStruct64(const STAT_STRUCTURE& output, struct __stat64& buffer)
{
  buffer.st_dev          = output.deviceId;
  buffer.st_size         = output.size;
#if defined(_WIN32)
  buffer.st_atime        = output.accessTime;
  buffer.st_mtime        = output.modificationTime;
  buffer.st_ctime        = output.statusTime;
#elif defined(__APPLE__)
  buffer.st_atimespec    = output.accessTime;
  buffer.st_mtimespec    = output.modificationTime;
  buffer.st_ctimespec    = output.statusTime;
#else
  buffer.st_atim         = output.accessTime;
  buffer.st_mtim         = output.modificationTime;
  buffer.st_ctim         = output.statusTime;
#endif
  buffer.st_mode = 0;
  if (output.isDirectory)
    buffer.st_mode       |= S_IFDIR;
#if !defined(_WIN32)
  if (output.isSymLink)
    buffer.st_mode       |= S_IFLNK;
#endif
  // TODO
  //if (output.isHidden)
  //  buffer.st_mode |= __S_IFDIR;
}
