/*
 *      Copyright (C) 2015 Garrett Brown
 *      Copyright (C) 2012-2015 Team XBMC
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

#include "interface/dll/FrontendCallbackLib.h"

#include "kodi/kodi_game_callbacks.h"

#include <cstdio>

#ifdef _WIN32
  #include <windows.h>
  #define DLLEXPORT __declspec(dllexport)
#else
  #define DLLEXPORT
#endif

using namespace NETPLAY;

extern "C"
{

DLLEXPORT CB_GameLib* GAME_register_me(AddonCB* handle)
{
  CB_GameLib* cbTable = NULL;
  if (handle == NULL)
  {
    std::fprintf(stderr, "ERROR: GAME_register_frontend is called with NULL handle !!!\n");
  }
  else
  {
    cbTable = handle->RegisterGameLib(handle->addonData);
    if (cbTable == NULL)
    {
      fprintf(stderr, "ERROR: GAME_register_frontend can't get callback table !!!\n");
    }
  }
  return cbTable;
}

DLLEXPORT void GAME_unregister_me(AddonCB* handle, CB_GameLib* cbTable)
{
  if (handle == NULL || cbTable == NULL)
    return;

  return handle->UnregisterGameLib(handle->addonData, cbTable);
}

DLLEXPORT void GAME_close_game(AddonCB* handle, CB_GameLib* cbTable)
{
  if (handle == NULL || cbTable == NULL)
    return;

  return cbTable->CloseGame(handle->addonData);
}

DLLEXPORT void GAME_video_frame(AddonCB* handle, CB_GameLib* cbTable, const uint8_t* data, unsigned int width, unsigned int height, GAME_RENDER_FORMAT format)
{
  if (handle == NULL || cbTable == NULL)
    return;

  return cbTable->VideoFrame(handle->addonData, data, width, height, format);
}

DLLEXPORT void GAME_audio_frames(AddonCB* handle, CB_GameLib* cbTable, const uint8_t* data, unsigned int frames, GAME_AUDIO_FORMAT format)
{
  if (handle == NULL || cbTable == NULL)
    return;

  return cbTable->AudioFrames(handle->addonData, data, frames, format);
}

DLLEXPORT void GAME_hw_set_info(AddonCB* handle, CB_GameLib* cbTable, game_hw_info* hw_info)
{
  if (handle == NULL || cbTable == NULL)
    return;

  return cbTable->HwSetInfo(handle->addonData, hw_info);
}

DLLEXPORT uintptr_t GAME_hw_get_current_framebuffer(AddonCB* handle, CB_GameLib* cbTable)
{
  if (handle == NULL || cbTable == NULL)
    return 0;

  return cbTable->HwGetCurrentFramebuffer(handle->addonData);
}

DLLEXPORT game_proc_address_t GAME_hw_get_proc_address(AddonCB* handle, CB_GameLib* cbTable, const char* sym)
{
  if (handle == NULL || cbTable == NULL)
    return NULL;

  return cbTable->HwGetProcAddress(handle->addonData, sym);
}

DLLEXPORT bool GAME_open_port(AddonCB* handle, CB_GameLib* cbTable, unsigned int port)
{
  if (handle == NULL || cbTable == NULL)
    return false;

  return cbTable->OpenPort(handle->addonData, port);
}

DLLEXPORT void GAME_close_port(AddonCB* handle, CB_GameLib* cbTable, unsigned int port)
{
  if (handle == NULL || cbTable == NULL)
    return;
  return cbTable->ClosePort(handle->addonData, port);
}

DLLEXPORT void GAME_rumble_set_state(AddonCB* handle, CB_GameLib* cbTable, unsigned int port, GAME_RUMBLE_EFFECT effect, float strength)
{
  if (handle == NULL || cbTable == NULL)
    return;

  return cbTable->RumbleSetState(handle->addonData, port, effect, strength);
}

} // extern "C"
