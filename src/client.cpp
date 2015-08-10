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

#include "interface/dll/DLLFrontend.h"
#include "interface/dll/DLLGame.h"
#include "interface/FrontendManager.h"
#include "interface/network/NetworkGame.h"
#include "interface/network/Server.h"
#include "keyboard/Keyboard.h"
#include "keyboard/KeyboardAddon.h"
#include "log/Log.h"
#include "utils/PathUtils.h"

#include "kodi/kodi_game_dll.h"
#include "kodi/xbmc_addon_dll.h"
#include "kodi/libKODI_guilib.h"

#include <cstring>

using namespace NETPLAY;

#define LOG_PREFIX  "NETPLAY: " // This gets prepended to log lines

#ifndef SAFE_DELETE
  #define SAFE_DELETE(x)  do { delete x; x = NULL; } while (0)
#endif

namespace NETPLAY
{
  CHelper_libKODI_guilib* GUI       = NULL;
  IFrontend*              FRONTEND  = NULL;
  CFrontendManager*       CALLBACKS = NULL;
  IGame*                  GAME      = NULL;
  CServer*                SERVER    = NULL;
}

// --- Helper functions --------------------------------------------------------

namespace NETPLAY
{
  /*!
   * \brief Remove the first item from the list of proxy DLLs
   */
  GameClientProperties PopProxyDLL(const GameClientProperties& properties)
  {
    GameClientProperties props(properties);
    props.proxy_dll_paths.erase(props.proxy_dll_paths.begin());
    return props;
  }

  bool IsStandalone(const GameClientProperties& properties)
  {
    // If no proxy DLL is given, then we're being created in standalone mode
    // and should load a game from the network
    return properties.proxy_dll_paths.empty();
  }

  IGame* GetGame(const GameClientProperties& properties, IFrontend* callbacks)
  {
    IGame* game = NULL;

    if (IsStandalone(properties))
    {
      game = new CNetworkGame(callbacks);
    }
    else
    {
      CLog::Get().SetLogPrefix(LOG_PREFIX);

      std::string myPath = properties.proxy_dll_paths[0];
      std::string myDir = PathUtils::GetParentDirectory(myPath);
      game = new CDLLGame(callbacks, PopProxyDLL(properties), PathUtils::GetHelperLibraryDir(myDir));
    }

    return game;
  }
}

// --- API functions -----------------------------------------------------------

extern "C"
{

ADDON_STATUS ADDON_Create(void* callbacks, void* props)
{
  if (callbacks == NULL || props == NULL)
    return ADDON_STATUS_UNKNOWN;

  ADDON_STATUS returnStatus(ADDON_STATUS_UNKNOWN);

  try
  {
    GUI = new CHelper_libKODI_guilib;
    if (!GUI->RegisterMe(callbacks))
      throw ADDON_STATUS_PERMANENT_FAILURE;

    CKeyboard::Get().SetPipe(new CKeyboardAddon(GUI));

    FRONTEND = new CDLLFrontend(callbacks);
    if (!FRONTEND->Initialize())
      throw ADDON_STATUS_PERMANENT_FAILURE;

    CALLBACKS = new CFrontendManager;
    if (!CALLBACKS->Initialize())
      throw ADDON_STATUS_PERMANENT_FAILURE;

    CALLBACKS->RegisterFrontend(FRONTEND);

    GameClientProperties gameProps = CDLLGame::TranslateProperties(*static_cast<game_client_properties*>(props));
    GAME = GetGame(gameProps, CALLBACKS);
    if (!GAME)
      throw ADDON_STATUS_UNKNOWN;

    ADDON_STATUS status = GAME->Initialize();
    if (status == ADDON_STATUS_UNKNOWN || status == ADDON_STATUS_PERMANENT_FAILURE)
      throw status;

    if (!IsStandalone(gameProps))
    {
      SERVER = new CServer(GAME, CALLBACKS);
      if (!SERVER->Initialize())
        throw ADDON_STATUS_PERMANENT_FAILURE;
    }

    returnStatus = ADDON_STATUS_OK;
  }
  catch (const ADDON_STATUS& status)
  {
    ADDON_Destroy();
    returnStatus = status;
  }

  return returnStatus;
}

void ADDON_Stop()
{
  if (GAME)
    GAME->Stop();
}

void ADDON_Destroy()
{
  if (SERVER)
    SERVER->Deinitialize();

  if (GAME)
    GAME->Deinitialize();

  if (CALLBACKS)
  {
    CALLBACKS->UnregisterFrontend(FRONTEND);
    CALLBACKS->Deinitialize();
  }

  if (FRONTEND)
    FRONTEND->Deinitialize();

  SAFE_DELETE(SERVER);
  SAFE_DELETE(GAME);
  SAFE_DELETE(CALLBACKS);
  SAFE_DELETE(FRONTEND);

  CKeyboard::Get().SetType(SYS_KEYBOARD_TYPE_CONSOLE);

  SAFE_DELETE(GUI);
}

ADDON_STATUS ADDON_GetStatus()
{
  if (GAME)
    return GAME->GetStatus();

  return ADDON_STATUS_UNKNOWN;
}

bool ADDON_HasSettings()
{
  if (GAME)
    return GAME->HasSettings();

  return false;
}

unsigned int ADDON_GetSettings(ADDON_StructSetting*** sSet)
{
  if (GAME)
    return GAME->GetSettings(sSet);

  return false;
}

ADDON_STATUS ADDON_SetSetting(const char* settingName, const void* settingValue)
{
  if (!settingName || !settingValue)
    return ADDON_STATUS_UNKNOWN;

  if (GAME)
    return GAME->SetSetting(settingName, settingValue);

  return ADDON_STATUS_OK;
}

void ADDON_FreeSettings()
{
  if (GAME)
    return GAME->FreeSettings();
}

void ADDON_Announce(const char* flag, const char* sender, const char* message, const void* data)
{
  if (GAME)
    return GAME->Announce(flag    ? flag    : "",
                          sender  ? sender  : "",
                          message ? message : "",
                          data    ? data    : NULL);
}

const char* GetGameAPIVersion(void)
{
  return GAME_API_VERSION;
}

const char* GetMininumGameAPIVersion(void)
{
  return GAME_MIN_API_VERSION;
}

GAME_ERROR LoadGame(const char* url)
{
  if (url == NULL)
    return GAME_ERROR_INVALID_PARAMETERS;

  if (GAME)
    return GAME->LoadGame(url);

  return GAME_ERROR_FAILED;
}

GAME_ERROR LoadGameSpecial(SPECIAL_GAME_TYPE type, const char** urls, size_t urlCount)
{
  if (urls == NULL || urlCount == 0)
    return GAME_ERROR_INVALID_PARAMETERS;

  if (GAME)
    return GAME->LoadGameSpecial(type, urls, urlCount);

  return GAME_ERROR_FAILED;
}

GAME_ERROR LoadStandalone(void)
{
  if (GAME)
    return GAME->LoadStandalone();

  return GAME_ERROR_FAILED;
}

GAME_ERROR UnloadGame(void)
{
  if (GAME)
    return GAME->UnloadGame();

  return GAME_ERROR_FAILED;
}

GAME_ERROR GetGameInfo(game_system_av_info* info)
{
  if (info == NULL)
    return GAME_ERROR_INVALID_PARAMETERS;

  if (GAME)
    return GAME->GetGameInfo(info);

  return GAME_ERROR_FAILED;
}

GAME_REGION GetRegion(void)
{
  if (GAME)
    return GAME->GetRegion();

  return GAME_REGION_UNKNOWN;
}

void FrameEvent(void)
{
  if (GAME)
    return GAME->FrameEvent();
}

GAME_ERROR Reset(void)
{
  if (GAME)
    return GAME->Reset();

  return GAME_ERROR_FAILED;
}

GAME_ERROR HwContextReset()
{
  if (GAME)
    return GAME->HwContextReset();

  return GAME_ERROR_FAILED;
}

GAME_ERROR HwContextDestroy()
{
  if (GAME)
    return GAME->HwContextDestroy();

  return GAME_ERROR_FAILED;
}

void UpdatePort(unsigned int port, bool connected, const game_controller* controller)
{
  if (controller == NULL)
    return;

  if (GAME)
    return GAME->UpdatePort(port, connected, controller);
}

bool InputEvent(unsigned int port, const game_input_event* event)
{
  if (event == NULL)
    return GAME_ERROR_INVALID_PARAMETERS;

  if (GAME)
    return GAME->InputEvent(port, event);

  return false;
}

size_t SerializeSize(void)
{
  return 0;
}

GAME_ERROR Serialize(uint8_t* data, size_t size)
{
  if (data == NULL || size == 0)
    return GAME_ERROR_INVALID_PARAMETERS;

  if (GAME)
    return GAME->Serialize(data, size);

  return GAME_ERROR_FAILED;
}

GAME_ERROR Deserialize(const uint8_t* data, size_t size)
{
  if (data == NULL || size == 0)
    return GAME_ERROR_INVALID_PARAMETERS;

  if (GAME)
    return GAME->Deserialize(data, size);

  return GAME_ERROR_FAILED;
}

GAME_ERROR CheatReset(void)
{
  if (GAME)
    return GAME->CheatReset();

  return GAME_ERROR_FAILED;
}

GAME_ERROR GetMemory(GAME_MEMORY type, const uint8_t** data, size_t* size)
{
  if (data == NULL || size == NULL)
    return GAME_ERROR_INVALID_PARAMETERS;

  if (GAME)
    return GAME->GetMemory(type, data, size);

  return GAME_ERROR_FAILED;
}

GAME_ERROR SetCheat(unsigned int index, bool enabled, const char* code)
{
  if (code == NULL)
    return GAME_ERROR_INVALID_PARAMETERS;

  if (GAME)
    return GAME->SetCheat(index, enabled, code);

  return GAME_ERROR_FAILED;
}

} // extern "C"
