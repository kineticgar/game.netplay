/*
 *      Copyright (C) 2015 Garrett Brown
 *      Copyright (C) 2012-2015 Team XBMC
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

/*!
 * \brief Callback table for Kodi's add-on callbacks
 *
 * This struct is imported from xbmc/addons/AddonCallbacks.h. The struct should
 * be relocated into kodi_addon_callbacks.h so that it may be available to
 * binary add-ons, similar to kodi_game_callbacks.h from the Game API.
 */

#include "kodi/libXBMC_addon.h"

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct CB_AddOnLib
{
  void (*Log)(void* addonData, const ADDON::addon_log_t loglevel, const char* msg);
  void (*QueueNotification)(void* addonData, const ADDON::queue_msg_t type, const char* msg);
  bool (*WakeOnLan)(const char* mac);
  bool (*GetSetting)(void* addonData, const char* settingName, void* settingValue);
  char* (*UnknownToUTF8)(const char* sourceDest);
  char* (*GetLocalizedString)(void* addonData, long dwCode);
  char* (*GetDVDMenuLanguage)(void* addonData);
  void (*FreeString)(void* addonData, char* str);

  void* (*OpenFile)(void* addonData, const char* strFileName, unsigned int flags);
  void* (*OpenFileForWrite)(void* addonData, const char* strFileName, bool bOverWrite);
  ssize_t (*ReadFile)(void* addonData, void* file, void* lpBuf, size_t uiBufSize);
  bool (*ReadFileString)(void* addonData, void* file, char* szLine, int iLineLength);
  ssize_t (*WriteFile)(void* addonData, void* file, const void* lpBuf, size_t uiBufSize);
  void (*FlushFile)(void* addonData, void* file);
  int64_t (*SeekFile)(void* addonData, void* file, int64_t iFilePosition, int iWhence);
  int (*TruncateFile)(void* addonData, void* file, int64_t iSize);
  int64_t (*GetFilePosition)(void* addonData, void* file);
  int64_t (*GetFileLength)(void* addonData, void* file);
  void (*CloseFile)(void* addonData, void* file);
  int (*GetFileChunkSize)(void* addonData, void* file);
  bool (*FileExists)(void* addonData, const char* strFileName, bool bUseCache);
  int (*StatFile)(void* addonData, const char* strFileName, struct __stat64* buffer);
  bool (*DeleteFile)(void* addonData, const char *strFileName);
  bool (*CanOpenDirectory)(void* addonData, const char* strURL);
  bool (*CreateDirectory)(void* addonData, const char* strPath);
  bool (*DirectoryExists)(void* addonData, const char* strPath);
  bool (*RemoveDirectory)(void* addonData, const char* strPath);
} CB_AddOnLib;
