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

#include "interface/IFrontend.h"

namespace ADDON { class CHelper_libXBMC_addon; }
class CHelper_libKODI_game;

namespace NETPLAY
{
  class CDLLFrontend : public IFrontend
  {
  public:
    CDLLFrontend(void* callbacks);
    virtual ~CDLLFrontend(void) { Deinitialize(); }

    virtual bool Initialize(void);
    virtual void Deinitialize(void);

    // implementation of IFrontend
    virtual void Log(const ADDON::addon_log_t loglevel, const std::string& msg);
    virtual bool GetSetting(const std::string& settingName, void* settingValue);
    virtual void QueueNotification(const ADDON::queue_msg_t type, const std::string& msg);
    virtual bool WakeOnLan(const std::string& mac);
    virtual std::string UnknownToUTF8(const std::string& str);
    virtual std::string GetLocalizedString(int dwCode, const std::string& strDefault = "");
    virtual std::string GetDVDMenuLanguage(void);
    virtual void* OpenFile(const std::string& strFileName, unsigned int flags);
    virtual void* OpenFileForWrite(const std::string& strFileName, bool bOverWrite);
    virtual ssize_t ReadFile(void* file, void* lpBuf, size_t uiBufSize);
    virtual bool ReadFileString(void* file, char* szLine, int iLineLength);
    virtual ssize_t WriteFile(void* file, const void* lpBuf, size_t uiBufSize);
    virtual void FlushFile(void* file);
    virtual int64_t SeekFile(void* file, int64_t iFilePosition, int iWhence);
    virtual int TruncateFile(void* file, int64_t iSize);
    virtual int64_t GetFilePosition(void* file);
    virtual int64_t GetFileLength(void* file);
    virtual void CloseFile(void* file);
    virtual int GetFileChunkSize(void* file);
    virtual bool FileExists(const std::string& strFileName, bool bUseCache);
    //virtual bool StatFile(const std::string& strFileName, STAT_STRUCTURE& buffer); // TODO
    virtual bool DeleteFile(const std::string& strFileName);
    virtual bool CanOpenDirectory(const std::string& strUrl);
    virtual bool CreateDirectory(const std::string& strPath);
    virtual bool DirectoryExists(const std::string& strPath);
    virtual bool RemoveDirectory(const std::string& strPath);
    virtual void CloseGame(void);
    virtual void VideoFrame(const uint8_t* data, unsigned int width, unsigned int height, GAME_RENDER_FORMAT format);
    virtual void AudioFrames(const uint8_t* data, unsigned int frames, GAME_AUDIO_FORMAT format);
    virtual void HwSetInfo(const game_hw_info* hw_info);
    virtual uintptr_t HwGetCurrentFramebuffer(void);
    virtual game_proc_address_t HwGetProcAddress(const char* symbol);
    virtual bool OpenPort(unsigned int port);
    virtual void ClosePort(unsigned int port);
    virtual void RumbleSetState(unsigned int port, GAME_RUMBLE_EFFECT effect, float strength);
    virtual void SetCameraInfo(unsigned int width, unsigned int height, GAME_CAMERA_BUFFER caps);
    virtual bool StartCamera(void);
    virtual void StopCamera(void);
    virtual bool StartLocation(void);
    virtual void StopLocation(void);
    virtual bool GetLocation(double* lat, double* lon, double* horizAccuracy, double* vertAccuracy);
    virtual void SetLocationInterval(unsigned int intervalMs, unsigned int intervalDistance);

  private:
    ADDON::CHelper_libXBMC_addon* m_addon;
    CHelper_libKODI_game*         m_game;
  };
}
