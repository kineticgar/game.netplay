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

#include "kodi/xbmc_addon_types.h"
#include "kodi/kodi_game_types.h"

namespace NETPLAY
{
  class IGame
  {
  public:
    virtual ~IGame(void) { }

    virtual bool Initialize(void) = 0;
    virtual void Deinitialize(void) = 0;

    virtual ADDON_STATUS Create(void *callbacks, void* props) = 0;
    virtual void         Stop(void) = 0;
    virtual void         Destroy(void) = 0;
    virtual ADDON_STATUS GetStatus(void) = 0;
    virtual bool         HasSettings(void) = 0;
    virtual unsigned int GetSettings(ADDON_StructSetting*** sSet) = 0;
    virtual ADDON_STATUS SetSetting(const char* settingName, const void* settingValue) = 0;
    virtual void         FreeSettings(void) = 0;
    virtual void         Announce(const char* flag, const char* sender, const char* message, const void* data) = 0;

    virtual const char* GetGameAPIVersion(void) = 0;
    virtual const char* GetMininumGameAPIVersion(void) = 0;
    virtual GAME_ERROR LoadGame(const char* url);
    virtual GAME_ERROR LoadGameSpecial(SPECIAL_GAME_TYPE type, const char** urls, size_t urlCount) = 0;
    virtual GAME_ERROR LoadStandalone(void) = 0;
    virtual GAME_ERROR UnloadGame(void) = 0;
    virtual GAME_ERROR GetGameInfo(game_system_av_info* info) = 0;
    virtual GAME_REGION GetRegion(void) = 0;
    virtual void FrameEvent(void) = 0;
    virtual GAME_ERROR Reset(void) = 0;
    virtual GAME_ERROR HwContextReset(void) = 0;
    virtual GAME_ERROR HwContextDestroy(void) = 0;
    virtual void UpdatePort(unsigned int port, bool connected, const game_controller* controller) = 0;
    virtual bool InputEvent(unsigned int port, const game_input_event* event) = 0;
    virtual size_t SerializeSize(void) = 0;
    virtual GAME_ERROR Serialize(uint8_t* data, size_t size) = 0;
    virtual GAME_ERROR Deserialize(const uint8_t* data, size_t size) = 0;
    virtual GAME_ERROR CheatReset(void) = 0;
    virtual GAME_ERROR GetMemory(GAME_MEMORY type, const uint8_t** data, size_t* size) = 0;
    virtual GAME_ERROR SetCheat(unsigned int index, bool enabled, const char* code) = 0;
  };
}
