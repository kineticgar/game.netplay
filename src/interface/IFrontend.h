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

#include "kodi/xbmc_addon_types.h"
#include "kodi/kodi_game_types.h"
#include "kodi/libXBMC_addon.h"

#include <string>

namespace NETPLAY
{
  class IFrontend
  {
  public:
    virtual ~IFrontend(void) { }

    virtual bool Initialize(void) = 0;
    virtual void Deinitialize(void) = 0;

    // --- Add-on callbacks ----------------------------------------------------

    virtual void Log(const ADDON::addon_log_t loglevel, const std::string& msg) = 0;
    virtual bool GetSetting(const std::string& settingName, void* settingValue) = 0;
    virtual void QueueNotification(const ADDON::queue_msg_t type, const std::string& msg) = 0;
    virtual bool WakeOnLan(const std::string& mac) = 0;
    virtual std::string UnknownToUTF8(const std::string& str) = 0;
    virtual std::string GetLocalizedString(int dwCode, const std::string& strDefault = "") = 0;
    virtual std::string GetDVDMenuLanguage(void) = 0;
    virtual void* OpenFile(const std::string& strFileName, unsigned int flags) = 0;
    virtual void* OpenFileForWrite(const std::string& strFileName, bool bOverWrite) = 0;
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
    virtual bool FileExists(const std::string& strFileName, bool bUseCache) = 0;
    //virtual int StatFile(const std::string& strFileName, struct __stat64* buffer) = 0; // TODO
    virtual bool DeleteFile(const std::string& strFileName) = 0;
    virtual bool CanOpenDirectory(const std::string& strUrl) = 0;
    virtual bool CreateDirectory(const std::string& strPath) = 0;
    virtual bool DirectoryExists(const std::string& strPath) = 0;
    virtual bool RemoveDirectory(const std::string& strPath) = 0;

    // --- Game callbacks ------------------------------------------------------

    virtual void CloseGame(void) = 0;
    virtual void VideoFrame(const uint8_t* data, unsigned int width, unsigned int height, GAME_RENDER_FORMAT format) = 0;
    virtual void AudioFrames(const uint8_t* data, unsigned int frames, GAME_AUDIO_FORMAT format) = 0;
    virtual void HwSetInfo(const game_hw_info* hw_info) = 0;
    virtual uintptr_t HwGetCurrentFramebuffer(void) = 0;
    virtual game_proc_address_t HwGetProcAddress(const char* symbol) = 0;
    virtual bool OpenPort(unsigned int port) = 0;
    virtual void ClosePort(unsigned int port) = 0;
    virtual void RumbleSetState(unsigned int port, GAME_RUMBLE_EFFECT effect, float strength) = 0;
    virtual void SetCameraInfo(unsigned int width, unsigned int height, GAME_CAMERA_BUFFER caps) = 0;
    virtual bool StartCamera(void) = 0;
    virtual void StopCamera(void) = 0;
    virtual bool StartLocation(void) = 0;
    virtual void StopLocation(void) = 0;
    virtual bool GetLocation(double* lat, double* lon, double* horizAccuracy, double* vertAccuracy) = 0;
    virtual void SetLocationInterval(unsigned int intervalMs, unsigned int intervalDistance) = 0;
  };
}
