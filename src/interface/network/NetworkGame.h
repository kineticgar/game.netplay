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

#include "interface/IGame.h"

#include <map>
#include <vector>

namespace NETPLAY
{
  class IFrontend;
  class CClient;

  class CNetworkGame : public IGame
  {
  public:
    CNetworkGame(IFrontend* callbacks);
    virtual ~CNetworkGame(void);

    // implementation of IGame
    virtual ADDON_STATUS Initialize(void);
    virtual void         Deinitialize(void);
    virtual void         Stop(void);
    virtual ADDON_STATUS GetStatus(void);
    virtual bool         HasSettings(void);
    virtual unsigned int GetSettings(ADDON_StructSetting*** sSet);
    virtual ADDON_STATUS SetSetting(const char* settingName, const void* settingValue);
    virtual void         FreeSettings(void);
    virtual void         Announce(const char* flag, const char* sender, const char* message, const void* data);
    virtual std::string GetGameAPIVersion(void);
    virtual std::string GetMininumGameAPIVersion(void);
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

  private:
    IFrontend* const              m_callbacks;
    CClient*                      m_rpc;

    std::map<GAME_MEMORY, std::vector<uint8_t> > m_memory;
  };
}
