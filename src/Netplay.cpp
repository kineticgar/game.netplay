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

#include "Netplay.h"

#include "kodi/libKODI_game.h"
#include "kodi/libXBMC_addon.h"

#include <algorithm>

using namespace NETPLAY;
using namespace PLATFORM;

CNetplay::CNetplay(void) :
  m_game(NULL)
{
}

bool CNetplay::Initialize(void)
{
  return true;
}

void CNetplay::Deinitialize(void)
{
}

void CNetplay::RegisterGame(IGame* game)
{
  CLockObject lock(m_mutex);
  m_game = game;
}

void CNetplay::UnregisterGame(void)
{
  CLockObject lock(m_mutex);
  m_game = NULL;
}

void CNetplay::RegisterFrontend(IFrontend* frontend)
{
  CLockObject lock(m_mutex);
  m_frontends.push_back(frontend);
}

void CNetplay::UnregisterFrontend(IFrontend* frontend)
{
  CLockObject lock(m_mutex);
  m_frontends.erase(std::remove(m_frontends.begin(), m_frontends.end(), frontend), m_frontends.end());
}

ADDON_STATUS CNetplay::Create(void* callbacks, void* props)
{
  if (m_game)
    return m_game->Create(callbacks, props);

  return ADDON_STATUS_PERMANENT_FAILURE;
}

void CNetplay::Stop(void)
{
  if (m_game)
    return m_game->Stop();
}

void CNetplay::Destroy(void)
{
  if (m_game)
    return m_game->Destroy();
}

ADDON_STATUS CNetplay::GetStatus(void)
{
  if (m_game)
    return m_game->GetStatus();

  return ADDON_STATUS_LOST_CONNECTION;
}

bool CNetplay::HasSettings(void)
{
  if (m_game)
    return m_game->HasSettings();

  return false;
}

unsigned int CNetplay::GetSettings(ADDON_StructSetting*** sSet)
{
  if (m_game)
    return m_game->GetSettings(sSet);

  return 0;
}

ADDON_STATUS CNetplay::SetSetting(const char* settingName, const void* settingValue)
{
  if (m_game)
    return m_game->SetSetting(settingName, settingValue);

  return ADDON_STATUS_LOST_CONNECTION;
}

void CNetplay::FreeSettings(void)
{
  if (m_game)
    return m_game->FreeSettings();
}

void CNetplay::Announce(const char* flag, const char* sender, const char* message, const void* data)
{
  if (m_game)
    return m_game->Announce(flag, sender, message, data);
}

const char* CNetplay::GetGameAPIVersion(void)
{
  if (m_game)
    return m_game->GetGameAPIVersion();

  return GAME_API_VERSION;
}

const char* CNetplay::GetMininumGameAPIVersion(void)
{
  if (m_game)
    return m_game->GetMininumGameAPIVersion();

  return GAME_MIN_API_VERSION;
}

GAME_ERROR CNetplay::LoadGame(const char* url)
{
  if (m_game)
    return m_game->LoadGame(url);

  return GAME_ERROR_FAILED;
}

GAME_ERROR CNetplay::LoadGameSpecial(SPECIAL_GAME_TYPE type, const char** urls, size_t urlCount)
{
  if (m_game)
    return m_game->LoadGameSpecial(type, urls, urlCount);

  return GAME_ERROR_FAILED;
}

GAME_ERROR CNetplay::LoadStandalone(void)
{
  if (m_game)
    return m_game->LoadStandalone();

  return GAME_ERROR_FAILED;
}

GAME_ERROR CNetplay::UnloadGame(void)
{
  if (m_game)
    return m_game->UnloadGame();

  return GAME_ERROR_FAILED;
}

GAME_ERROR CNetplay::GetGameInfo(game_system_av_info* info)
{
  if (m_game)
    return m_game->GetGameInfo(info);

  return GAME_ERROR_FAILED;
}

GAME_REGION CNetplay::GetRegion(void)
{
  if (m_game)
    return m_game->GetRegion();

  return GAME_REGION_UNKNOWN;
}

void CNetplay::FrameEvent(void)
{
  if (m_game)
    return m_game->FrameEvent();
}

GAME_ERROR CNetplay::Reset(void)
{
  if (m_game)
    return m_game->Reset();

  return GAME_ERROR_FAILED;
}

GAME_ERROR CNetplay::HwContextReset(void)
{
  if (m_game)
    return m_game->HwContextReset();

  return GAME_ERROR_FAILED;
}

GAME_ERROR CNetplay::HwContextDestroy(void)
{
  if (m_game)
    return m_game->HwContextDestroy();

  return GAME_ERROR_FAILED;
}

void CNetplay::UpdatePort(unsigned int port, bool connected, const game_controller* controller)
{
  if (m_game)
    return m_game->UpdatePort(port, connected, controller);
}

bool CNetplay::InputEvent(unsigned int port, const game_input_event* event)
{
  if (m_game)
    return m_game->InputEvent(port, event);

  return false;
}

size_t CNetplay::SerializeSize(void)
{
  if (m_game)
    return m_game->SerializeSize();

  return 0;
}

GAME_ERROR CNetplay::Serialize(uint8_t* data, size_t size)
{
  if (m_game)
    return m_game->Serialize(data, size);

  return GAME_ERROR_FAILED;
}

GAME_ERROR CNetplay::Deserialize(const uint8_t* data, size_t size)
{
  if (m_game)
    return m_game->Deserialize(data, size);

  return GAME_ERROR_FAILED;
}

GAME_ERROR CNetplay::CheatReset(void)
{
  if (m_game)
    return m_game->CheatReset();

  return GAME_ERROR_FAILED;
}

GAME_ERROR CNetplay::GetMemory(GAME_MEMORY type, const uint8_t** data, size_t* size)
{
  if (m_game)
    return m_game->GetMemory(type, data, size);

  return GAME_ERROR_FAILED;
}

GAME_ERROR CNetplay::SetCheat(unsigned int index, bool enabled, const char* code)
{
  if (m_game)
    return m_game->SetCheat(index, enabled, code);

  return GAME_ERROR_FAILED;
}