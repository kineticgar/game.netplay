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

#include "kodi/kodi_game_dll.h"
#include "kodi/xbmc_addon_dll.h"

//using namespace NETPLAY;

extern "C"
{

ADDON_STATUS ADDON_Create(void* callbacks, void* props)
{
  return ADDON_GetStatus();
}

void ADDON_Stop()
{
}

void ADDON_Destroy()
{
}

ADDON_STATUS ADDON_GetStatus()
{
  return ADDON_STATUS_OK;
}

bool ADDON_HasSettings()
{
  return false;
}

unsigned int ADDON_GetSettings(ADDON_StructSetting*** sSet)
{
  return 0;
}

ADDON_STATUS ADDON_SetSetting(const char* settingName, const void* settingValue)
{
  if (!settingName || !settingValue)
    return ADDON_STATUS_UNKNOWN;

  return ADDON_STATUS_OK;
}

void ADDON_FreeSettings()
{
}

void ADDON_Announce(const char *flag, const char *sender, const char *message, const void *data)
{
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

  return GAME_ERROR_FAILED;
}

GAME_ERROR LoadGameSpecial(SPECIAL_GAME_TYPE type, const char** urls, size_t urlCount)
{
  if (urls == NULL || urlCount == 0)
    return GAME_ERROR_INVALID_PARAMETERS;

  return GAME_ERROR_FAILED;
}

GAME_ERROR LoadStandalone(void)
{
  return GAME_ERROR_FAILED;
}

GAME_ERROR UnloadGame(void)
{
  return GAME_ERROR_FAILED;
}

GAME_ERROR GetGameInfo(game_system_av_info* info)
{
  if (info == NULL)
    return GAME_ERROR_INVALID_PARAMETERS;

  return GAME_ERROR_FAILED;
}

GAME_REGION GetRegion(void)
{
  return GAME_REGION_UNKNOWN;
}

void FrameEvent(void)
{
}

GAME_ERROR Reset(void)
{
  return GAME_ERROR_FAILED;
}

GAME_ERROR HwContextReset()
{
  return GAME_ERROR_FAILED;
}

GAME_ERROR HwContextDestroy()
{
  return GAME_ERROR_FAILED;
}

void UpdatePort(unsigned int port, bool connected, const game_controller* controller)
{
}

bool InputEvent(unsigned int port, const game_input_event* event)
{
  return false;
}

GAME_ERROR DiskSetEjectState(GAME_EJECT_STATE ejected)
{
  return GAME_ERROR_FAILED;
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
  return GAME_ERROR_FAILED;
}

unsigned DiskGetNumImages(void)
{
  return 0;
}

GAME_ERROR DiskReplaceImageIndex(unsigned int index, const char* url)
{
  if (url == NULL)
    return GAME_ERROR_INVALID_PARAMETERS;

  return GAME_ERROR_FAILED;
}

GAME_ERROR DiskAddImageIndex(void)
{
  return GAME_ERROR_FAILED;
}

GAME_ERROR CameraInitialized(void)
{
  return GAME_ERROR_FAILED;
}

GAME_ERROR CameraDeinitialized(void)
{
  return GAME_ERROR_FAILED;
}

GAME_ERROR CameraFrameRawBuffer(const uint32_t* buffer, unsigned int width, unsigned int height, size_t stride)
{
  return GAME_ERROR_FAILED;
}

GAME_ERROR CameraFrameOpenglTexture(unsigned int textureId, unsigned int textureTarget, const float* affine)
{
  return GAME_ERROR_FAILED;
}

size_t SerializeSize(void)
{
  return 0;
}

GAME_ERROR Serialize(uint8_t* data, size_t size)
{
  if (data == NULL)
    return GAME_ERROR_INVALID_PARAMETERS;

  return GAME_ERROR_FAILED;
}

GAME_ERROR Deserialize(const uint8_t* data, size_t size)
{
  if (data == NULL)
    return GAME_ERROR_INVALID_PARAMETERS;

  return GAME_ERROR_FAILED;
}

GAME_ERROR CheatReset(void)
{
  return GAME_ERROR_FAILED;
}

GAME_ERROR GetMemory(GAME_MEMORY type, const uint8_t** data, size_t* size)
{
  if (data == NULL || size == NULL)
    return GAME_ERROR_INVALID_PARAMETERS;

  return GAME_ERROR_FAILED;
}

GAME_ERROR SetCheat(unsigned int index, bool enabled, const char* code)
{
  return GAME_ERROR_FAILED;
}

} // extern "C"
