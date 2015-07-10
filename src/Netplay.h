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
#include "interface/IGame.h"

#include "platform/threads/mutex.h"

#include <stddef.h>
#include <vector>

namespace NETPLAY
{
  class IFrontend;
  class IGame;

  class CNetplay : public IGame, IFrontend
  {
  public:
    CNetplay(void);
    ~CNetplay(void) { Deinitialize(); }

    bool Initialize(void);
    void Deinitialize(void);

    void RegisterGame(IGame* game);
    void UnregisterGame(void);

    void RegisterFrontend(IFrontend* frontend);
    void UnregisterFrontend(IFrontend* frontend);

    // implementation of IGame
    virtual ADDON_STATUS Create(void* callbacks, void* props);
    virtual void         Stop(void);
    virtual void         Destroy(void);
    virtual ADDON_STATUS GetStatus(void);
    virtual bool         HasSettings(void);
    virtual unsigned int GetSettings(ADDON_StructSetting*** sSet);
    virtual ADDON_STATUS SetSetting(const char* settingName, const void* settingValue);
    virtual void         FreeSettings(void);
    virtual void         Announce(const char* flag, const char* sender, const char* message, const void* data);
    virtual const char* GetGameAPIVersion(void);
    virtual const char* GetMininumGameAPIVersion(void);
    virtual GAME_ERROR LoadGame(const char* url);
    virtual GAME_ERROR LoadGameSpecial(SPECIAL_GAME_TYPE type, const char** urls, size_t urlCount);
    virtual GAME_ERROR LoadStandalone(void);
    virtual GAME_ERROR UnloadGame(void);
    virtual GAME_ERROR GetGameInfo(game_system_av_info* info);
    virtual GAME_REGION GetRegion(void);
    virtual void FrameEvent(void);
    virtual GAME_ERROR Reset(void);
    virtual GAME_ERROR HwContextReset(void);
    virtual GAME_ERROR HwContextDestroy(void);
    virtual void UpdatePort(unsigned int port, bool connected, const game_controller* controller);
    virtual bool InputEvent(unsigned int port, const game_input_event* event);
    virtual size_t SerializeSize(void);
    virtual GAME_ERROR Serialize(uint8_t* data, size_t size);
    virtual GAME_ERROR Deserialize(const uint8_t* data, size_t size);
    virtual GAME_ERROR CheatReset(void);
    virtual GAME_ERROR GetMemory(GAME_MEMORY type, const uint8_t** data, size_t* size);
    virtual GAME_ERROR SetCheat(unsigned int index, bool enabled, const char* code);

    // implementation of IFrontend
    virtual void Log(const ADDON::addon_log_t loglevel, const char* msg);
    virtual bool GetSetting(const char* settingName, void *settingValue);
    virtual void QueueNotification(const ADDON::queue_msg_t type, const char* msg);
    virtual bool WakeOnLan(const char* mac);
    virtual std::string UnknownToUTF8(const std::string& str);
    virtual std::string GetLocalizedString(int dwCode, const std::string& strDefault = "");
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
    //virtual int StatFile(const char* strFileName, struct __stat64* buffer); // TODO
    virtual bool DeleteFile(const char* strFileName);
    virtual bool CanOpenDirectory(const char* strUrl);
    virtual bool CreateDirectory(const char* strPath);
    virtual bool DirectoryExists(const char* strPath);
    virtual bool RemoveDirectory(const char* strPath);

  private:
    IFrontend* GetMaster(void);

    std::vector<IFrontend*> m_frontends;
    IGame*                  m_game;
    PLATFORM::CMutex        m_mutex;
  };
}
