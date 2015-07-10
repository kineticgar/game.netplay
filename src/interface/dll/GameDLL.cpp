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

#include "GameDLL.h"

#ifdef _WIN32
  #include "dlfcn-win32.h"
#else
  #include <dlfcn.h>
#endif

using namespace NETPLAY;

namespace NETPLAY
{
  // Register symbols from DLL, cast to type T and store in member variable
  template <typename T>
  bool RegisterSymbol(void* dll, T& functionPtr, const char* strFunctionPtr)
  {
    return (functionPtr = (T)dlsym(dll, strFunctionPtr)) != NULL;
  }

  // Convert functionPtr to a string literal
  #define REGISTER_SYMBOL(dll, functionPtr)  RegisterSymbol(dll, m_ ## functionPtr, #functionPtr)
}

CGameDLL::CGameDLL(const std::string& strDllPath) :
  m_strPath(strDllPath),
  m_dll(NULL),
  m_ADDON_Create(NULL),
  m_ADDON_Stop(NULL),
  m_ADDON_Destroy(NULL),
  m_ADDON_GetStatus(NULL),
  m_ADDON_HasSettings(NULL),
  m_ADDON_GetSettings(NULL),
  m_ADDON_SetSetting(NULL),
  m_ADDON_FreeSettings(NULL),
  m_ADDON_Announce(NULL),
  m_GetGameAPIVersion(NULL),
  m_GetMininumGameAPIVersion(NULL),
  m_LoadGame(NULL),
  m_LoadGameSpecial(NULL),
  m_LoadStandalone(NULL),
  m_UnloadGame(NULL),
  m_GetGameInfo(NULL),
  m_GetRegion(NULL),
  m_FrameEvent(NULL),
  m_Reset(NULL),
  m_HwContextReset(NULL),
  m_HwContextDestroy(NULL),
  m_UpdatePort(NULL),
  m_InputEvent(NULL),
  m_SerializeSize(NULL),
  m_Serialize(NULL),
  m_Deserialize(NULL),
  m_CheatReset(NULL),
  m_GetMemory(NULL),
  m_SetCheat(NULL)
{
}

bool CGameDLL::Initialize(void)
{
  Deinitialize();

  m_dll = dlopen(m_strPath.c_str(), RTLD_LAZY);
  if (m_dll == NULL)
  {
    //esyslog("Unable to load %s: %s", m_strPath.c_str(), dlerror());
    return false;
  }

  try
  {
    if (!REGISTER_SYMBOL(m_dll, ADDON_Create)) throw false;
    if (!REGISTER_SYMBOL(m_dll, ADDON_Stop)) throw false;
    if (!REGISTER_SYMBOL(m_dll, ADDON_Destroy)) throw false;
    if (!REGISTER_SYMBOL(m_dll, ADDON_GetStatus)) throw false;
    if (!REGISTER_SYMBOL(m_dll, ADDON_HasSettings)) throw false;
    if (!REGISTER_SYMBOL(m_dll, ADDON_GetSettings)) throw false;
    if (!REGISTER_SYMBOL(m_dll, ADDON_SetSetting)) throw false;
    if (!REGISTER_SYMBOL(m_dll, ADDON_FreeSettings)) throw false;
    if (!REGISTER_SYMBOL(m_dll, ADDON_Announce)) throw false;
    if (!REGISTER_SYMBOL(m_dll, GetGameAPIVersion)) throw false;
    if (!REGISTER_SYMBOL(m_dll, GetMininumGameAPIVersion)) throw false;
    if (!REGISTER_SYMBOL(m_dll, LoadGame)) throw false;
    if (!REGISTER_SYMBOL(m_dll, LoadGameSpecial)) throw false;
    if (!REGISTER_SYMBOL(m_dll, LoadStandalone)) throw false;
    if (!REGISTER_SYMBOL(m_dll, UnloadGame)) throw false;
    if (!REGISTER_SYMBOL(m_dll, GetGameInfo)) throw false;
    if (!REGISTER_SYMBOL(m_dll, GetRegion)) throw false;
    if (!REGISTER_SYMBOL(m_dll, FrameEvent)) throw false;
    if (!REGISTER_SYMBOL(m_dll, Reset)) throw false;
    if (!REGISTER_SYMBOL(m_dll, HwContextReset)) throw false;
    if (!REGISTER_SYMBOL(m_dll, HwContextDestroy)) throw false;
    if (!REGISTER_SYMBOL(m_dll, UpdatePort)) throw false;
    if (!REGISTER_SYMBOL(m_dll, InputEvent)) throw false;
    if (!REGISTER_SYMBOL(m_dll, SerializeSize)) throw false;
    if (!REGISTER_SYMBOL(m_dll, Serialize)) throw false;
    if (!REGISTER_SYMBOL(m_dll, Deserialize)) throw false;
    if (!REGISTER_SYMBOL(m_dll, CheatReset)) throw false;
    if (!REGISTER_SYMBOL(m_dll, GetMemory)) throw false;
    if (!REGISTER_SYMBOL(m_dll, SetCheat)) throw false;
  }
  catch (const bool& bSuccess)
  {
    //esyslog("Unable to assign function %s", dlerror());
    return bSuccess;
  }

  return true;
}

void CGameDLL::Deinitialize(void)
{
  if (m_dll)
  {
    dlclose(m_dll);
    m_dll = NULL;
  }
}

ADDON_STATUS CGameDLL::Create(void* callbacks, void* props)
{
  return m_ADDON_Create(callbacks, props);
}

void CGameDLL::Stop(void)
{
  return m_ADDON_Stop();
}

void CGameDLL::Destroy(void)
{
  return m_ADDON_Destroy();
}

ADDON_STATUS CGameDLL::GetStatus(void)
{
  return m_ADDON_GetStatus();
}

bool CGameDLL::HasSettings(void)
{
  return m_ADDON_HasSettings();
}

unsigned int CGameDLL::GetSettings(ADDON_StructSetting*** sSet)
{
  return m_ADDON_GetSettings(sSet);
}

ADDON_STATUS CGameDLL::SetSetting(const std::string& settingName, const void* settingValue)
{
  return m_ADDON_SetSetting(settingName, settingValue);
}

void CGameDLL::FreeSettings(void)
{
  return m_ADDON_FreeSettings();
}

void CGameDLL::Announce(const std::string& flag, const std::string& sender, const std::string& message, const void* data)
{
  return m_ADDON_Announce(flag, sender, message, data);
}

const char* CGameDLL::GetGameAPIVersion(void)
{
  return m_GetGameAPIVersion();
}

const char* CGameDLL::GetMininumGameAPIVersion(void)
{
  return m_GetMininumGameAPIVersion();
}

GAME_ERROR CGameDLL::LoadGame(const std::string& url)
{
  return m_LoadGame(url);
}

GAME_ERROR CGameDLL::LoadGameSpecial(SPECIAL_GAME_TYPE type, const char** urls, size_t urlCount)
{
  return m_LoadGameSpecial(type, urls, urlCount);
}

GAME_ERROR CGameDLL::LoadStandalone(void)
{
  return m_LoadStandalone();
}

GAME_ERROR CGameDLL::UnloadGame(void)
{
  return m_UnloadGame();
}

GAME_ERROR CGameDLL::GetGameInfo(game_system_av_info* info)
{
  return m_GetGameInfo(info);
}

GAME_REGION CGameDLL::GetRegion(void)
{
  return m_GetRegion();
}

void CGameDLL::FrameEvent(void)
{
  return m_FrameEvent();
}

GAME_ERROR CGameDLL::Reset(void)
{
  return m_Reset();
}

GAME_ERROR CGameDLL::HwContextReset(void)
{
  return m_HwContextReset();
}

GAME_ERROR CGameDLL::HwContextDestroy(void)
{
  return m_HwContextDestroy();
}

void CGameDLL::UpdatePort(unsigned int port, bool connected, const game_controller* controller)
{
  return m_UpdatePort(port, connected, controller);
}

bool CGameDLL::InputEvent(unsigned int port, const game_input_event* event)
{
  return m_InputEvent(port, event);
}

size_t CGameDLL::SerializeSize(void)
{
  return m_SerializeSize();
}

GAME_ERROR CGameDLL::Serialize(uint8_t* data, size_t size)
{
  return m_Serialize(data, size);
}

GAME_ERROR CGameDLL::Deserialize(const uint8_t* data, size_t size)
{
  return m_Deserialize(data, size);
}

GAME_ERROR CGameDLL::CheatReset(void)
{
  return m_CheatReset();
}

GAME_ERROR CGameDLL::GetMemory(GAME_MEMORY type, const uint8_t** data, size_t* size)
{
  return m_GetMemory(type, data, size);
}

GAME_ERROR CGameDLL::SetCheat(unsigned int index, bool enabled, const std::string& code)
{
  return m_SetCheat(index, enabled, code);
}
