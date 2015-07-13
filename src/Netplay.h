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

#include "kodi/libXBMC_addon.h"
#include "kodi/xbmc_addon_types.h"
#include "kodi/kodi_game_types.h"

#include <stddef.h>
#include <string>
#include <vector>

namespace NETPLAY
{
  class IFrontend;
  class IGame;

  class CNetplay
  {
  public:
    CNetplay(void);
    ~CNetplay(void) { }

    void RegisterGame(IGame* game);
    void UnregisterGame(void);

    void RegisterFrontend(IFrontend* frontend);
    void UnregisterFrontend(IFrontend* frontend);

    // partial implementation of IGame
    ADDON_STATUS Create(void* callbacks, void* props);
    void         Stop(void);
    void         Destroy(void);
    ADDON_STATUS GetStatus(void);
    bool         HasSettings(void);
    unsigned int GetSettings(ADDON_StructSetting*** sSet);
    ADDON_STATUS SetSetting(const std::string& settingName, const void* settingValue);
    void         FreeSettings(void);
    void         Announce(const std::string& flag, const std::string& sender, const std::string& message, const void* data);
    std::string GetGameAPIVersion(void);
    std::string GetMininumGameAPIVersion(void);
    GAME_ERROR LoadGame(const std::string& url);
    GAME_ERROR LoadGameSpecial(SPECIAL_GAME_TYPE type, const char** urls, size_t urlCount);
    GAME_ERROR LoadStandalone(void);
    GAME_ERROR UnloadGame(void);
    GAME_ERROR GetGameInfo(game_system_av_info* info);
    GAME_REGION GetRegion(void);
    void FrameEvent(void);
    GAME_ERROR Reset(void);
    GAME_ERROR HwContextReset(void);
    GAME_ERROR HwContextDestroy(void);
    void UpdatePort(unsigned int port, bool connected, const game_controller* controller);
    bool InputEvent(unsigned int port, const game_input_event* event);
    size_t SerializeSize(void);
    GAME_ERROR Serialize(uint8_t* data, size_t size);
    GAME_ERROR Deserialize(const uint8_t* data, size_t size);
    GAME_ERROR CheatReset(void);
    GAME_ERROR GetMemory(GAME_MEMORY type, const uint8_t** data, size_t* size);
    GAME_ERROR SetCheat(unsigned int index, bool enabled, const std::string& code);

    // partial implementation of IFrontend
    void Log(const ADDON::addon_log_t loglevel, const std::string& msg);
    bool GetSetting(const std::string& settingName, void *settingValue);
    void QueueNotification(const ADDON::queue_msg_t type, const std::string& msg);
    bool WakeOnLan(const std::string& mac);
    std::string UnknownToUTF8(const std::string& str);
    std::string GetLocalizedString(int dwCode, const std::string& strDefault = "");
    std::string GetDVDMenuLanguage(void);
    void* OpenFile(const std::string& strFileName, unsigned int flags);
    void* OpenFileForWrite(const std::string& strFileName, bool bOverWrite);
    ssize_t ReadFile(void* file, void* lpBuf, size_t uiBufSize);
    bool ReadFileString(void* file, char* szLine, int iLineLength);
    ssize_t WriteFile(void* file, const void* lpBuf, size_t uiBufSize);
    void FlushFile(void* file);
    int64_t SeekFile(void* file, int64_t iFilePosition, int iWhence);
    int TruncateFile(void* file, int64_t iSize);
    int64_t GetFilePosition(void* file);
    int64_t GetFileLength(void* file);
    void CloseFile(void* file);
    int GetFileChunkSize(void* file);
    bool FileExists(const std::string& strFileName, bool bUseCache);
    //int StatFile(const std::string& strFileName, struct __stat64* buffer); // TODO
    bool DeleteFile(const std::string& strFileName);
    bool CanOpenDirectory(const std::string& strUrl);
    bool CreateDirectory(const std::string& strPath);
    bool DirectoryExists(const std::string& strPath);
    bool RemoveDirectory(const std::string& strPath);
    void CloseGame(void);
    void VideoFrame(const uint8_t* data, unsigned int width, unsigned int height, GAME_RENDER_FORMAT format);
    void AudioFrames(const uint8_t* data, unsigned int frames, GAME_AUDIO_FORMAT format);
    void HwSetInfo(const game_hw_info* hw_info);
    uintptr_t HwGetCurrentFramebuffer(void);
    game_proc_address_t HwGetProcAddress(const char* symbol);
    bool OpenPort(unsigned int port);
    void ClosePort(unsigned int port);
    void RumbleSetState(unsigned int port, GAME_RUMBLE_EFFECT effect, float strength);
    void SetCameraInfo(unsigned int width, unsigned int height, GAME_CAMERA_BUFFER caps);
    bool StartCamera(void);
    void StopCamera(void);
    bool StartLocation(void);
    void StopLocation(void);
    bool GetLocation(double* lat, double* lon, double* horizAccuracy, double* vertAccuracy);
    void SetLocationInterval(unsigned int intervalMs, unsigned int intervalDistance);

  private:
    IFrontend* GetMaster(void);

    std::vector<IFrontend*> m_frontends;
    IGame*                  m_game;
  };
}
