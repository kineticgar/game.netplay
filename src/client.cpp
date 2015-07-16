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

#include "interface/dll/DLLGame.h"
#include "interface/network/NetworkGame.h"
#include "interface/network/Server.h"
#include "log/Log.h"
#include "utils/PathUtils.h"

#include "kodi/kodi_game_dll.h"
#include "kodi/xbmc_addon_dll.h"

#include <cstring>

using namespace NETPLAY;

#ifndef SAFE_DELETE
  #define SAFE_DELETE(x)  do { delete x; x = NULL; } while (0)
#endif

namespace NETPLAY
{
  IFrontend*        FRONTEND  = NULL;
  CFrontendManager* CALLBACKS = NULL;
  IGame*            GAME      = NULL;
  CServer*          SERVER    = NULL;
}

// --- Helper functions --------------------------------------------------------

namespace NETPLAY
{
  /*!
   * \brief Remove the first item from the list of proxy DLLs
   */
  void PopProxyDLL(GameClientProperties& properties)
  {
    properties.proxy_dll_paths.erase(properties.proxy_dll_paths.begin());
  }

  IGame* GetGame(const GameClientProperties& properties)
  {
    IGame* game = NULL;

    std::string strNetplayDllPath;
    if (!properties.proxy_dll_paths.empty())
      strNetplayDllPath = properties.proxy_dll_paths[0];

    if (strNetplayDllPath.empty())
    {
      esyslog("Can't find Netplay DLL: list of proxy DLLs is empty (netplay should be first)");
    }
    else
    {
      const bool bLoadGameClient = !properties.game_client_dll_path.empty();
      if (bLoadGameClient)
      {
        // Remove netplay DLL from beginning of list
        GameClientProperties copy(properties);
        PopProxyDLL(copy);

        GAME = new CDLLGame(CALLBACKS, copy, PathUtils::GetHelperLibraryDir(PathUtils::GetParentDirectory(strNetplayDllPath)));
      }
      else
      {
        std::string address = properties.netplay_server;
        unsigned int port = properties.netplay_server_port;

        if (!address.empty())
        {
          GAME = new CNetworkGame(address, port);
        }
        else
        {
          // TODO
          esyslog("Network discovery not implemented");
        }
      }
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
    FRONTEND = new CDLLFrontend(callbacks);
    if (!FRONTEND->Initialize())
      throw ADDON_STATUS_PERMANENT_FAILURE;

    CALLBACKS = new CFrontendManager;
    CALLBACKS->RegisterFrontend(FRONTEND);

    GAME = GetGame(CDLLGame::TranslateProperties(*static_cast<game_client_properties*>(props)));
    if (!GAME)
      throw ADDON_STATUS_UNKNOWN;

    ADDON_STATUS status = GAME->Initialize();
    if (status == ADDON_STATUS_UNKNOWN || status == ADDON_STATUS_PERMANENT_FAILURE)
      throw status;

    SERVER = new CServer(GAME, CALLBACKS);
    if (!SERVER->Initialize())
      throw ADDON_STATUS_PERMANENT_FAILURE;

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
  {
    SERVER->Deinitialize();
    GAME->Deinitialize();
    CALLBACKS->UnregisterFrontend(FRONTEND);
    FRONTEND->Deinitialize();
  }

  SAFE_DELETE(SERVER);
  SAFE_DELETE(GAME);
  SAFE_DELETE(CALLBACKS);
  SAFE_DELETE(FRONTEND);
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
    return GAME->Announce(flag, sender, message, data);
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
  if (GAME)
    return GAME->UpdatePort(port, connected, controller);
}

bool InputEvent(unsigned int port, const game_input_event* event)
{
  if (GAME)
    return GAME->InputEvent(port, event);

  return false;
}

GAME_ERROR DiskSetEjectState(GAME_EJECT_STATE ejected)
{
  return GAME_ERROR_NOT_IMPLEMENTED;
}

GAME_EJECT_STATE DiskGetEjectState(void)
{
  return GAME_NOT_EJECTED;
}

unsigned DiskGetImageIndex(void)
{
  return 0;
}

GAME_ERROR DiskSetImageIndex(unsigned int index)
{
  return GAME_ERROR_NOT_IMPLEMENTED;
}

unsigned DiskGetNumImages(void)
{
  return 0;
}

GAME_ERROR DiskReplaceImageIndex(unsigned int index, const char* url)
{
  if (url == NULL)
    return GAME_ERROR_INVALID_PARAMETERS;

  return GAME_ERROR_NOT_IMPLEMENTED;
}

GAME_ERROR DiskAddImageIndex(void)
{
  return GAME_ERROR_NOT_IMPLEMENTED;
}

GAME_ERROR CameraInitialized(void)
{
  return GAME_ERROR_NOT_IMPLEMENTED;
}

GAME_ERROR CameraDeinitialized(void)
{
  return GAME_ERROR_NOT_IMPLEMENTED;
}

GAME_ERROR CameraFrameRawBuffer(const uint32_t* buffer, unsigned int width, unsigned int height, size_t stride)
{
  return GAME_ERROR_NOT_IMPLEMENTED;
}

GAME_ERROR CameraFrameOpenglTexture(unsigned int textureId, unsigned int textureTarget, const float* affine)
{
  return GAME_ERROR_NOT_IMPLEMENTED;
}

size_t SerializeSize(void)
{
  return 0;
}

GAME_ERROR Serialize(uint8_t* data, size_t size)
{
  if (data == NULL)
    return GAME_ERROR_INVALID_PARAMETERS;

  if (GAME)
    return GAME->Serialize(data, size);

  return GAME_ERROR_FAILED;
}

GAME_ERROR Deserialize(const uint8_t* data, size_t size)
{
  if (data == NULL)
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
  if (GAME)
    return GAME->SetCheat(index, enabled, code);

  return GAME_ERROR_FAILED;
}

} // extern "C"
