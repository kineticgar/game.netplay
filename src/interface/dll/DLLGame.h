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

#include "kodi/xbmc_addon_types.h"
#include "kodi/kodi_game_types.h"
#include "platform/threads/mutex.h"

#include <string>
#include <vector>

namespace NETPLAY
{
  class IFrontend;
  class CFrontendCallbackLib;

  struct GameClientProperties
  {
    std::string              game_client_dll_path;
    std::vector<std::string> proxy_dll_paths;
    std::string              system_directory;
    std::string              content_directory;
    std::string              save_directory;
  };

  class CDLLGame : public IGame
  {
  public:
    CDLLGame(IFrontend* frontend, const GameClientProperties& properties, const std::string& strLibBasePath);
    virtual ~CDLLGame(void) { Deinitialize(); }

    // implementation of IGame
    virtual ADDON_STATUS Initialize(void);
    virtual bool IsInitialized(void) { return m_bInitialized; }
    virtual void Deinitialize(void);
    virtual void         Stop(void);
    virtual ADDON_STATUS GetStatus(void);
    virtual bool         HasSettings(void);
    virtual unsigned int GetSettings(ADDON_StructSetting*** sSet);
    virtual ADDON_STATUS SetSetting(const std::string& settingName, const void* settingValue);
    virtual void         FreeSettings(void);
    virtual void         Announce(const std::string& flag, const std::string& sender, const std::string& message, const void* data);
    virtual std::string GetGameAPIVersion(void);
    virtual std::string GetMininumGameAPIVersion(void);
    virtual GAME_ERROR LoadGame(const std::string& url);
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
    virtual GAME_ERROR SetCheat(unsigned int index, bool enabled, const std::string& code);

    static GameClientProperties TranslateProperties(const game_client_properties& props);

  private:
    static void TranslateProperties(const GameClientProperties& props, game_client_properties& propsStruct);
    static void FreeProperties(game_client_properties& propsStruct);

    IFrontend* const           m_callbacks;
    const GameClientProperties m_properties;
    const std::string          m_strLibBasePath;
    bool                       m_bInitialized;
    void*                      m_dll;
    CFrontendCallbackLib*      m_pHelper;
    PLATFORM::CMutex           m_mutex;

    ADDON_STATUS (*m_ADDON_Create)(void* callbacks, void* props);
    void         (*m_ADDON_Stop)(void);
    void         (*m_ADDON_Destroy)(void);
    ADDON_STATUS (*m_ADDON_GetStatus)(void);
    bool         (*m_ADDON_HasSettings)(void);
    unsigned int (*m_ADDON_GetSettings)(ADDON_StructSetting*** sSet);
    ADDON_STATUS (*m_ADDON_SetSetting)(const std::string& settingName, const void* settingValue);
    void         (*m_ADDON_FreeSettings)(void);
    void         (*m_ADDON_Announce)(const std::string& flag, const std::string& sender, const std::string& message, const void* data);
    const char* (*m_GetGameAPIVersion)(void);
    const char* (*m_GetMininumGameAPIVersion)(void);
    GAME_ERROR (*m_LoadGame)(const std::string& url);
    GAME_ERROR (*m_LoadGameSpecial)(SPECIAL_GAME_TYPE type, const char** urls, size_t urlCount);
    GAME_ERROR (*m_LoadStandalone)(void);
    GAME_ERROR (*m_UnloadGame)(void);
    GAME_ERROR (*m_GetGameInfo)(game_system_av_info* info);
    GAME_REGION (*m_GetRegion)(void);
    void (*m_FrameEvent)(void);
    GAME_ERROR (*m_Reset)(void);
    GAME_ERROR (*m_HwContextReset)(void);
    GAME_ERROR (*m_HwContextDestroy)(void);
    void (*m_UpdatePort)(unsigned int port, bool connected, const game_controller* controller);
    bool (*m_InputEvent)(unsigned int port, const game_input_event* event);
    size_t (*m_SerializeSize)(void);
    GAME_ERROR (*m_Serialize)(uint8_t* data, size_t size);
    GAME_ERROR (*m_Deserialize)(const uint8_t* data, size_t size);
    GAME_ERROR (*m_CheatReset)(void);
    GAME_ERROR (*m_GetMemory)(GAME_MEMORY type, const uint8_t** data, size_t* size);
    GAME_ERROR (*m_SetCheat)(unsigned int index, bool enabled, const std::string& code);
  };
}
