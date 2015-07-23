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
#include "utils/Observer.h"

namespace NETPLAY
{
  class IConnection;
  class IGame;

  class CNetworkFrontend : public IFrontend,
                           public Observer

  {
  public:
    CNetworkFrontend(IGame* game, int fd);
    virtual ~CNetworkFrontend(void);

    virtual bool Initialize(void);
    virtual void Deinitialize(void);

    // implementation of IFrontend
    virtual void Log(const ADDON::addon_log_t loglevel, const char* msg);
    virtual bool GetSetting(const char* settingName, void* settingValue);
    virtual void QueueNotification(const ADDON::queue_msg_t type, const char* msg);
    virtual bool WakeOnLan(const char* mac);
    virtual std::string UnknownToUTF8(const char* str);
    virtual std::string GetLocalizedString(int dwCode, const char* strDefault = "");
    virtual std::string GetDVDMenuLanguage(void);
    virtual void* OpenFile(const char* strFileName, unsigned int flags);
    virtual void* OpenFileForWrite(const char* strFileName, bool bOverWrite);
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
    virtual bool FileExists(const char* strFileName, bool bUseCache);
    virtual bool StatFile(const char* strFileName, STAT_STRUCTURE& buffer);
    virtual bool DeleteFile(const char* strFileName);
    virtual bool CanOpenDirectory(const char* strUrl);
    virtual bool CreateDirectory(const char* strPath);
    virtual bool DirectoryExists(const char* strPath);
    virtual bool RemoveDirectory(const char* strPath);
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

    // implementation of Observer
    virtual void Notify(const Observable& obs, const ObservableMessage msg);

  private:
    IConnection* const m_rpc;
  };
}
