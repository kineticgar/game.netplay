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
#include "utils/Observer.h"

#include "kodi/xbmc_addon_types.h"
#include "kodi/kodi_game_types.h"
#include "kodi/libXBMC_addon.h"

#include <string>

namespace NETPLAY
{
  struct STAT_STRUCTURE;

  class IFrontend : public Observable
  {
  public:
    virtual ~IFrontend(void) { }

    virtual bool Initialize(void) = 0;
    virtual void Deinitialize(void) = 0;

    // --- Add-on callbacks ----------------------------------------------------

    virtual void Log(const ADDON::addon_log_t loglevel, const char* msg) = 0;
    virtual bool GetSetting(const char* settingName, void* settingValue) = 0;
    virtual void QueueNotification(const ADDON::queue_msg_t type, const char* msg) = 0;
    virtual bool WakeOnLan(const char* mac) = 0;
    virtual std::string UnknownToUTF8(const char* str) = 0;
    virtual std::string GetLocalizedString(int dwCode, const char* strDefault = "") = 0;
    virtual std::string GetDVDMenuLanguage(void) = 0;
    virtual void* OpenFile(const char* strFileName, unsigned int flags) = 0;
    virtual void* OpenFileForWrite(const char* strFileName, bool bOverWrite) = 0;
    virtual ssize_t ReadFile(void* file, void* lpBuf, size_t uiBufSize) = 0;
    virtual bool ReadFileString(void* file, char* szLine, int iLineLength) = 0;
    virtual ssize_t WriteFile(void* file, const void* lpBuf, size_t uiBufSize) = 0;
    virtual void FlushFile(void* file) = 0;
    virtual int64_t SeekFile(void* file, int64_t iFilePosition, int iWhence) = 0;
    virtual int TruncateFile(void* file, int64_t iSize) = 0;
    virtual int64_t GetFilePosition(void* file) = 0;
    virtual int64_t GetFileLength(void* file) = 0;
    virtual void CloseFile(void* file) = 0;
    virtual int GetFileChunkSize(void* file) = 0;
    virtual bool FileExists(const char* strFileName, bool bUseCache) = 0;
    virtual bool StatFile(const char* strFileName, STAT_STRUCTURE& buffer) = 0;
    virtual bool DeleteFile(const char* strFileName) = 0;
    virtual bool CanOpenDirectory(const char* strUrl) = 0;
    virtual bool CreateDirectory(const char* strPath) = 0;
    virtual bool DirectoryExists(const char* strPath) = 0;
    virtual bool RemoveDirectory(const char* strPath) = 0;

    // --- Game callbacks ------------------------------------------------------

    virtual void CloseGame(void) = 0;
    virtual void VideoFrame(const uint8_t* data, unsigned int size, unsigned int width, unsigned int height, GAME_RENDER_FORMAT format) = 0;
    virtual void AudioFrames(const uint8_t* data, unsigned int size, unsigned int frames, GAME_AUDIO_FORMAT format) = 0;
    virtual void HwSetInfo(const game_hw_info* hw_info) = 0;
    virtual uintptr_t HwGetCurrentFramebuffer(void) = 0;
    virtual game_proc_address_t HwGetProcAddress(const char* symbol) = 0;
    virtual bool OpenPort(unsigned int port) = 0;
    virtual void ClosePort(unsigned int port) = 0;
    virtual void RumbleSetState(unsigned int port, GAME_RUMBLE_EFFECT effect, float strength) = 0;
  };
}
