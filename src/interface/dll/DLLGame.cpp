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

#include "DLLGame.h"
#include "FrontendCallbackLib.h"
#include "log/Log.h"

#ifdef _WIN32
  #include "dlfcn-win32.h"
#else
  #include <dlfcn.h>
#endif

using namespace NETPLAY;
using namespace PLATFORM;

// --- REGISTER_SYMBOL() macro -------------------------------------------------

#define REGISTER_SYMBOL(dll, functionPtr)  NETPLAY::RegisterSymbol(dll, m_ ## functionPtr, #functionPtr)

namespace NETPLAY
{
  // Register symbols from DLL, cast to type T and store in member variable
  template <typename T>
  bool RegisterSymbol(void* dll, T& functionPtr, const char* strFunctionPtr)
  {
    return (functionPtr = reinterpret_cast<T>(dlsym(dll, strFunctionPtr))) != NULL;
  }
}

// --- CDLLGame ----------------------------------------------------------------

CDLLGame::CDLLGame(IFrontend* callbacks, const GameClientProperties& properties, const std::string& strLibBasePath) :
  m_callbacks(callbacks),
  m_properties(properties),
  m_strLibBasePath(strLibBasePath),
  m_bInitialized(false),
  m_dll(NULL),
  m_pHelper(NULL),
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

ADDON_STATUS CDLLGame::Initialize(void)
{
  if (m_bInitialized)
    return ADDON_STATUS_OK;

  std::string strDllPath;

  if (!m_properties.proxy_dll_paths.empty())
    strDllPath = m_properties.proxy_dll_paths[0];
  else
    strDllPath = m_properties.game_client_dll_path;

  m_dll = dlopen(strDllPath.c_str(), RTLD_LAZY);
  if (m_dll == NULL)
  {
    esyslog("Unable to load %s: %s", strDllPath.c_str(), dlerror());
    return ADDON_STATUS_PERMANENT_FAILURE;
  }

  try
  {
    if (!REGISTER_SYMBOL(m_dll, ADDON_Create)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, ADDON_Stop)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, ADDON_Destroy)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, ADDON_GetStatus)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, ADDON_HasSettings)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, ADDON_GetSettings)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, ADDON_SetSetting)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, ADDON_FreeSettings)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, ADDON_Announce)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, GetGameAPIVersion)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, GetMininumGameAPIVersion)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, LoadGame)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, LoadGameSpecial)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, LoadStandalone)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, UnloadGame)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, GetGameInfo)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, GetRegion)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, FrameEvent)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, Reset)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, HwContextReset)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, HwContextDestroy)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, UpdatePort)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, InputEvent)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, SerializeSize)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, Serialize)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, Deserialize)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, CheatReset)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, GetMemory)) throw ADDON_STATUS_PERMANENT_FAILURE;
    if (!REGISTER_SYMBOL(m_dll, SetCheat)) throw ADDON_STATUS_PERMANENT_FAILURE;
  }
  catch (const ADDON_STATUS& status)
  {
    esyslog("Unable to assign function %s", dlerror());
    return status;
  }

  m_pHelper = new CFrontendCallbackLib(m_callbacks, m_strLibBasePath);

  game_client_properties props = { };
  TranslateProperties(m_properties, props);

  ADDON_STATUS status = m_ADDON_Create(m_pHelper->GetCallbacks(), &props);

  FreeProperties(props);

  if (status != ADDON_STATUS_UNKNOWN || status != ADDON_STATUS_PERMANENT_FAILURE)
    m_bInitialized = true;

  return status;
}

void CDLLGame::Deinitialize(void)
{
  CLockObject lock(m_mutex);

  if (m_ADDON_Destroy)
    m_ADDON_Destroy();

  delete m_pHelper;
  m_pHelper = NULL;

  if (m_dll)
  {
    dlclose(m_dll);
    m_dll = NULL;
  }

  m_bInitialized = false;
}

void CDLLGame::Stop(void)
{
  CLockObject lock(m_mutex);
  return m_ADDON_Stop();
}

ADDON_STATUS CDLLGame::GetStatus(void)
{
  CLockObject lock(m_mutex);
  return m_ADDON_GetStatus();
}

bool CDLLGame::HasSettings(void)
{
  CLockObject lock(m_mutex);
  return m_ADDON_HasSettings();
}

unsigned int CDLLGame::GetSettings(ADDON_StructSetting*** sSet)
{
  CLockObject lock(m_mutex);
  return m_ADDON_GetSettings(sSet);
}

ADDON_STATUS CDLLGame::SetSetting(const char* settingName, const void* settingValue)
{
  CLockObject lock(m_mutex);
  return m_ADDON_SetSetting(settingName, settingValue);
}

void CDLLGame::FreeSettings(void)
{
  CLockObject lock(m_mutex);
  return m_ADDON_FreeSettings();
}

void CDLLGame::Announce(const char* flag, const char* sender, const char* message, const void* data)
{
  CLockObject lock(m_mutex);
  return m_ADDON_Announce(flag, sender, message, data);
}

std::string CDLLGame::GetGameAPIVersion(void)
{
  CLockObject lock(m_mutex);
  return m_GetGameAPIVersion();
}

std::string CDLLGame::GetMininumGameAPIVersion(void)
{
  CLockObject lock(m_mutex);
  return m_GetMininumGameAPIVersion();
}

GAME_ERROR CDLLGame::LoadGame(const char* url)
{
  CLockObject lock(m_mutex);
  return m_LoadGame(url);
}

GAME_ERROR CDLLGame::LoadGameSpecial(SPECIAL_GAME_TYPE type, const char** urls, size_t urlCount)
{
  CLockObject lock(m_mutex);
  return m_LoadGameSpecial(type, urls, urlCount);
}

GAME_ERROR CDLLGame::LoadStandalone(void)
{
  CLockObject lock(m_mutex);
  return m_LoadStandalone();
}

GAME_ERROR CDLLGame::UnloadGame(void)
{
  CLockObject lock(m_mutex);
  return m_UnloadGame();
}

GAME_ERROR CDLLGame::GetGameInfo(game_system_av_info* info)
{
  CLockObject lock(m_mutex);
  return m_GetGameInfo(info);
}

GAME_REGION CDLLGame::GetRegion(void)
{
  CLockObject lock(m_mutex);
  return m_GetRegion();
}

void CDLLGame::FrameEvent(void)
{
  CLockObject lock(m_mutex);
  return m_FrameEvent();
}

GAME_ERROR CDLLGame::Reset(void)
{
  CLockObject lock(m_mutex);
  return m_Reset();
}

GAME_ERROR CDLLGame::HwContextReset(void)
{
  CLockObject lock(m_mutex);
  return m_HwContextReset();
}

GAME_ERROR CDLLGame::HwContextDestroy(void)
{
  CLockObject lock(m_mutex);
  return m_HwContextDestroy();
}

void CDLLGame::UpdatePort(unsigned int port, bool connected, const game_controller* controller)
{
  CLockObject lock(m_mutex);
  return m_UpdatePort(port, connected, controller);
}

bool CDLLGame::InputEvent(unsigned int port, const game_input_event* event)
{
  CLockObject lock(m_mutex);
  return m_InputEvent(port, event);
}

size_t CDLLGame::SerializeSize(void)
{
  CLockObject lock(m_mutex);
  return m_SerializeSize();
}

GAME_ERROR CDLLGame::Serialize(uint8_t* data, size_t size)
{
  CLockObject lock(m_mutex);
  return m_Serialize(data, size);
}

GAME_ERROR CDLLGame::Deserialize(const uint8_t* data, size_t size)
{
  CLockObject lock(m_mutex);
  return m_Deserialize(data, size);
}

GAME_ERROR CDLLGame::CheatReset(void)
{
  CLockObject lock(m_mutex);
  return m_CheatReset();
}

GAME_ERROR CDLLGame::GetMemory(GAME_MEMORY type, const uint8_t** data, size_t* size)
{
  CLockObject lock(m_mutex);
  return m_GetMemory(type, data, size);
}

GAME_ERROR CDLLGame::SetCheat(unsigned int index, bool enabled, const char* code)
{
  CLockObject lock(m_mutex);
  return m_SetCheat(index, enabled, code);
}

GameClientProperties CDLLGame::TranslateProperties(const game_client_properties& props)
{
  GameClientProperties properties = { };

  properties.game_client_dll_path = props.game_client_dll_path ? props.game_client_dll_path : "";
  properties.system_directory     = props.system_directory     ? props.system_directory     : "";
  properties.content_directory    = props.content_directory    ? props.content_directory    : "";
  properties.save_directory       = props.save_directory       ? props.save_directory       : "";

  for (unsigned int i = 0; i < props.proxy_dll_count; i++)
    properties.proxy_dll_paths.push_back(props.proxy_dll_paths[i] ? props.proxy_dll_paths[i] : "");

  return properties;
}

void CDLLGame::TranslateProperties(const GameClientProperties& props, game_client_properties& propsStruct)
{
  propsStruct.game_client_dll_path = props.game_client_dll_path.c_str();
  propsStruct.proxy_dll_paths = NULL;
  propsStruct.proxy_dll_count = props.proxy_dll_paths.size();
  propsStruct.system_directory = props.system_directory.c_str();
  propsStruct.content_directory = props.content_directory.c_str();
  propsStruct.save_directory = props.save_directory.c_str();

  if (propsStruct.proxy_dll_count > 0)
  {
    const char** proxy_dll_paths = new const char*[propsStruct.proxy_dll_count];
    for (unsigned int i = 0; i < propsStruct.proxy_dll_count; i++)
      proxy_dll_paths[i] = props.proxy_dll_paths[i].c_str();
    propsStruct.proxy_dll_paths = proxy_dll_paths;
  }
}

void CDLLGame::FreeProperties(game_client_properties& propsStruct)
{
  delete[] propsStruct.proxy_dll_paths;
}
