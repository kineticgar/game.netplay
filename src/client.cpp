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
#include "Netplay.h"

#include "kodi/kodi_game_dll.h"
#include "kodi/xbmc_addon_dll.h"

#include <cstring>

using namespace NETPLAY;

#ifndef SAFE_DELETE
  #define SAFE_DELETE(x)  do { delete x; x = NULL; } while (0)
#endif

namespace NETPLAY
{
  IGame*     GAME     = NULL;
  IFrontend* FRONTEND = NULL;
  CNetplay*  SESSION  = NULL;
}

extern "C"
{

ADDON_STATUS ADDON_Create(void* callbacks, void* props)
{
  if (callbacks == NULL || props == NULL)
    return ADDON_STATUS_UNKNOWN;

  game_client_properties* gameProps = static_cast<game_client_properties*>(props);

  try
  {
    SESSION = new CNetplay;

    const bool bLoadGame = (gameProps->library_path && std::strlen(gameProps->library_path) > 0);
    if (bLoadGame)
    {
      GAME = new CDLLGame(gameProps->library_path);
      if (!GAME->Initialize())
        throw ADDON_STATUS_PERMANENT_FAILURE;

      SESSION->RegisterGame(GAME);
    }

    FRONTEND = new CDLLFrontend(callbacks);
    if (!FRONTEND->Initialize())
      throw ADDON_STATUS_PERMANENT_FAILURE;

    SESSION->RegisterFrontend(FRONTEND);
  }
  catch (const ADDON_STATUS& status)
  {
    ADDON_Destroy();
    return status;
  }

  return SESSION->Create(callbacks, props);
}

void ADDON_Stop()
{
  if (SESSION)
    return SESSION->Stop();
}

void ADDON_Destroy()
{
  if (SESSION)
  {
    if (GAME)
      SESSION->UnregisterGame();
    SESSION->UnregisterFrontend(FRONTEND);
  }

  SAFE_DELETE(GAME);
  SAFE_DELETE(FRONTEND);
  SAFE_DELETE(SESSION);
}

ADDON_STATUS ADDON_GetStatus()
{
  if (SESSION)
    return SESSION->GetStatus();

  return ADDON_STATUS_UNKNOWN;
}

bool ADDON_HasSettings()
{
  if (SESSION)
    return SESSION->HasSettings();

  return false;
}

unsigned int ADDON_GetSettings(ADDON_StructSetting*** sSet)
{
  if (SESSION)
    return SESSION->GetSettings(sSet);

  return false;
}

ADDON_STATUS ADDON_SetSetting(const char* settingName, const void* settingValue)
{
  if (!settingName || !settingValue)
    return ADDON_STATUS_UNKNOWN;

  if (SESSION)
    return SESSION->SetSetting(settingName, settingValue);

  return ADDON_STATUS_OK;
}

void ADDON_FreeSettings()
{
  if (SESSION)
    return SESSION->FreeSettings();
}

void ADDON_Announce(const char* flag, const char* sender, const char* message, const void* data)
{
  if (SESSION)
    return SESSION->Announce(flag, sender, message, data);
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

  if (SESSION)
    return SESSION->LoadGame(url);

  return GAME_ERROR_FAILED;
}

GAME_ERROR LoadGameSpecial(SPECIAL_GAME_TYPE type, const char** urls, size_t urlCount)
{
  if (urls == NULL || urlCount == 0)
    return GAME_ERROR_INVALID_PARAMETERS;

  if (SESSION)
    return SESSION->LoadGameSpecial(type, urls, urlCount);

  return GAME_ERROR_FAILED;
}

GAME_ERROR LoadStandalone(void)
{
  if (SESSION)
    return SESSION->LoadStandalone();

  return GAME_ERROR_FAILED;
}

GAME_ERROR UnloadGame(void)
{
  if (SESSION)
    return SESSION->UnloadGame();

  return GAME_ERROR_FAILED;
}

GAME_ERROR GetGameInfo(game_system_av_info* info)
{
  if (info == NULL)
    return GAME_ERROR_INVALID_PARAMETERS;

  if (SESSION)
    return SESSION->GetGameInfo(info);

  return GAME_ERROR_FAILED;
}

GAME_REGION GetRegion(void)
{
  if (SESSION)
    return SESSION->GetRegion();

  return GAME_REGION_UNKNOWN;
}

void FrameEvent(void)
{
  if (SESSION)
    return SESSION->FrameEvent();
}

GAME_ERROR Reset(void)
{
  if (SESSION)
    return SESSION->Reset();

  return GAME_ERROR_FAILED;
}

GAME_ERROR HwContextReset()
{
  if (SESSION)
    return SESSION->HwContextReset();

  return GAME_ERROR_FAILED;
}

GAME_ERROR HwContextDestroy()
{
  if (SESSION)
    return SESSION->HwContextDestroy();

  return GAME_ERROR_FAILED;
}

void UpdatePort(unsigned int port, bool connected, const game_controller* controller)
{
  if (SESSION)
    return SESSION->UpdatePort(port, connected, controller);
}

bool InputEvent(unsigned int port, const game_input_event* event)
{
  if (SESSION)
    return SESSION->InputEvent(port, event);

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

  if (SESSION)
    return SESSION->Serialize(data, size);

  return GAME_ERROR_FAILED;
}

GAME_ERROR Deserialize(const uint8_t* data, size_t size)
{
  if (data == NULL)
    return GAME_ERROR_INVALID_PARAMETERS;

  if (SESSION)
    return SESSION->Deserialize(data, size);

  return GAME_ERROR_FAILED;
}

GAME_ERROR CheatReset(void)
{
  if (SESSION)
    return SESSION->CheatReset();

  return GAME_ERROR_FAILED;
}

GAME_ERROR GetMemory(GAME_MEMORY type, const uint8_t** data, size_t* size)
{
  if (data == NULL || size == NULL)
    return GAME_ERROR_INVALID_PARAMETERS;

  if (SESSION)
    return SESSION->GetMemory(type, data, size);

  return GAME_ERROR_FAILED;
}

GAME_ERROR SetCheat(unsigned int index, bool enabled, const char* code)
{
  if (SESSION)
    return SESSION->SetCheat(index, enabled, code);

  return GAME_ERROR_FAILED;
}

} // extern "C"
