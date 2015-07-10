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

#include "GameNetwork.h"

using namespace NETPLAY;

CGameNetwork::CGameNetwork(void)
{
}

bool CGameNetwork::Initialize(void)
{
  return false; // TODO
}

void CGameNetwork::Deinitialize(void)
{
}

ADDON_STATUS CGameNetwork::Create(void* callbacks, void* props)
{
  return ADDON_STATUS_UNKNOWN; // TODO
}

void CGameNetwork::Stop(void)
{
  // TODO
}

void CGameNetwork::Destroy(void)
{
  // TODO
}

ADDON_STATUS CGameNetwork::GetStatus(void)
{
  return ADDON_STATUS_UNKNOWN; // TODO
}

bool CGameNetwork::HasSettings(void)
{
  return false; // TODO
}

unsigned int CGameNetwork::GetSettings(ADDON_StructSetting*** sSet)
{
  return 0; // TODO
}

ADDON_STATUS CGameNetwork::SetSetting(const std::string& settingName, const void* settingValue)
{
  return ADDON_STATUS_UNKNOWN; // TODO
}

void CGameNetwork::FreeSettings(void)
{
  // TODO
}

void CGameNetwork::Announce(const std::string& flag, const std::string& sender, const std::string& message, const void* data)
{
  // TODO
}

const char* CGameNetwork::GetGameAPIVersion(void)
{
  return GAME_API_VERSION; // TODO
}

const char* CGameNetwork::GetMininumGameAPIVersion(void)
{
  return GAME_MIN_API_VERSION; // TODO
}

GAME_ERROR CGameNetwork::LoadGame(const std::string& url)
{
  return GAME_ERROR_NOT_IMPLEMENTED; // TODO
}

GAME_ERROR CGameNetwork::LoadGameSpecial(SPECIAL_GAME_TYPE type, const char** urls, size_t urlCount)
{
  return GAME_ERROR_NOT_IMPLEMENTED; // TODO
}

GAME_ERROR CGameNetwork::LoadStandalone(void)
{
  return GAME_ERROR_NOT_IMPLEMENTED; // TODO
}

GAME_ERROR CGameNetwork::UnloadGame(void)
{
  return GAME_ERROR_NOT_IMPLEMENTED; // TODO
}

GAME_ERROR CGameNetwork::GetGameInfo(game_system_av_info* info)
{
  return GAME_ERROR_NOT_IMPLEMENTED; // TODO
}

GAME_REGION CGameNetwork::GetRegion(void)
{
  return GAME_REGION_UNKNOWN; // TODO
}

void CGameNetwork::FrameEvent(void)
{
  // TODO
}

GAME_ERROR CGameNetwork::Reset(void)
{
  return GAME_ERROR_NOT_IMPLEMENTED; // TODO
}

GAME_ERROR CGameNetwork::HwContextReset(void)
{
  return GAME_ERROR_NOT_IMPLEMENTED; // TODO
}

GAME_ERROR CGameNetwork::HwContextDestroy(void)
{
  return GAME_ERROR_NOT_IMPLEMENTED; // TODO
}

void CGameNetwork::UpdatePort(unsigned int port, bool connected, const game_controller* controller)
{
  // TODO
}

bool CGameNetwork::InputEvent(unsigned int port, const game_input_event* event)
{
  return false; // TODO
}

size_t CGameNetwork::SerializeSize(void)
{
  return 0; // TODO
}

GAME_ERROR CGameNetwork::Serialize(uint8_t* data, size_t size)
{
  return GAME_ERROR_NOT_IMPLEMENTED; // TODO
}

GAME_ERROR CGameNetwork::Deserialize(const uint8_t* data, size_t size)
{
  return GAME_ERROR_NOT_IMPLEMENTED; // TODO
}

GAME_ERROR CGameNetwork::CheatReset(void)
{
  return GAME_ERROR_NOT_IMPLEMENTED; // TODO
}

GAME_ERROR CGameNetwork::GetMemory(GAME_MEMORY type, const uint8_t** data, size_t* size)
{
  return GAME_ERROR_NOT_IMPLEMENTED; // TODO
}

GAME_ERROR CGameNetwork::SetCheat(unsigned int index, bool enabled, const std::string& code)
{
  return GAME_ERROR_NOT_IMPLEMENTED; // TODO
}
