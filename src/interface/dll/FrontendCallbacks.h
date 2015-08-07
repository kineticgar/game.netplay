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

#include "utils/CommonIncludes.h"

#include "kodi/kodi_addon_callbacks.h"
#include "kodi/kodi_game_callbacks.h"

namespace NETPLAY
{
  class IFrontend;
  struct STAT_STRUCTURE;

  class CFrontendCallbacks
  {
  public:
    CFrontendCallbacks(IFrontend* frontend);

  protected:
    IFrontend* GetFrontend(void) { return m_frontend; }

    static IFrontend* GetFrontend(void* addonData);

    static IFrontend* GetStaticFrontend(void) { return m_globalFrontend; }

    static char* DuplicateString(const std::string& str);
    static void UnduplicateString(char* str); // FreeString() is taken

  private:
    IFrontend* const  m_frontend;
    static IFrontend* m_globalFrontend;
  };

  class CFrontendCallbacksAddon : public CFrontendCallbacks
  {
  public:
    CFrontendCallbacksAddon(IFrontend* frontend);

    /*!
     * \return The callback table
     */
    CB_AddOnLib* GetCallbacks(void) { return &m_callbacks; }

    static void Log(void* addonData, const ADDON::addon_log_t loglevel, const char* msg);
    static bool GetSetting(void* addonData, const char* settingName, void* settingValue);
    static void QueueNotification(void* addonData, const ADDON::queue_msg_t type, const char* msg);
    static bool WakeOnLan(const char* mac);
    static char* UnknownToUTF8(const char* sourceDest);
    static char* GetLocalizedString(void* addonData, long dwCode);
    static char* GetDVDMenuLanguage(void* addonData);
    static void FreeString(void* addonData, char* str);
    static void* OpenFile(void* addonData, const char* strFileName, unsigned int flags);
    static void* OpenFileForWrite(void* addonData, const char* strFileName, bool bOverWrite);
    static ssize_t ReadFile(void* addonData, void* file, void* lpBuf, size_t uiBufSize);
    static bool ReadFileString(void* addonData, void* file, char* szLine, int iLineLength);
    static ssize_t WriteFile(void* addonData, void* file, const void* lpBuf, size_t uiBufSize);
    static void FlushFile(void* addonData, void* file);
    static int64_t SeekFile(void* addonData, void* file, int64_t iFilePosition, int iWhence);
    static int TruncateFile(void* addonData, void* file, int64_t iSize);
    static int64_t GetFilePosition(void* addonData, void* file);
    static int64_t GetFileLength(void* addonData, void* file);
    static void CloseFile(void* addonData, void* file);
    static int GetFileChunkSize(void* addonData, void* file);
    static bool FileExists(void* addonData, const char* strFileName, bool bUseCache);
    static int StatFile(void* addonData, const char* strFileName, struct __stat64* buffer);
    static bool DeleteFile(void* addonData, const char *strFileName);
    static bool CanOpenDirectory(void* addonData, const char* strURL);
    static bool CreateDirectory(void* addonData, const char* strPath);
    static bool DirectoryExists(void* addonData, const char* strPath);
    static bool RemoveDirectory(void* addonData, const char* strPath);

  private:
    CB_AddOnLib m_callbacks;
  };

  class CFrontendCallbacksGame : public CFrontendCallbacks
  {
  public:
    CFrontendCallbacksGame(IFrontend* frontend);

    /*!
     * \return The callback table
     */
    CB_GameLib* GetCallbacks(void) { return &m_callbacks; }

    static void CloseGame(void* addonData);
    static void VideoFrame(void* addonData, const uint8_t* data, unsigned int width, unsigned int height, GAME_RENDER_FORMAT format);
    static void AudioFrames(void* addonData, const uint8_t* data, unsigned int frames, GAME_AUDIO_FORMAT format);
    static void HwSetInfo(void* addonData, const game_hw_info* hw_info);
    static uintptr_t HwGetCurrentFramebuffer(void* addonData);
    static game_proc_address_t HwGetProcAddress(void* addonData, const char* symbol);
    static bool OpenPort(void* addonData, unsigned int port);
    static void ClosePort(void* addonData, unsigned int port);
    static void RumbleSetState(void* addonData, unsigned int port, GAME_RUMBLE_EFFECT effect, float strength);

  private:
    CB_GameLib m_callbacks;
  };
}
