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

#include "NetworkGame.h"

#include "addon.pb.h"
#include "game.pb.h"

#include <cstring>

using namespace NETPLAY;

CNetworkGame::CNetworkGame(const std::string& strAddress, unsigned int port) :
  m_rpc(strAddress, port)
{
}

ADDON_STATUS CNetworkGame::Initialize(void)
{
  if (!m_rpc.Open())
  {
    return ADDON_STATUS_UNKNOWN;
  }
  addon::LoginRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::Login, strRequest, strResponse))
    {
      addon::LoginResponse response;
      if (response.ParseFromString(strResponse))
        return static_cast<ADDON_STATUS>(response.result());
    }
  }

  return ADDON_STATUS_UNKNOWN;
}

void CNetworkGame::Deinitialize(void)
{
  addon::LogoutRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc.Send(RPC_METHOD::Logout, strRequest, strResponse);
  }
}

void CNetworkGame::Stop(void)
{
  // Removed from network API
}

ADDON_STATUS CNetworkGame::GetStatus(void)
{
  addon::GetStatusRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::GetStatus, strRequest, strResponse))
    {
      addon::GetStatusResponse response;
      if (response.ParseFromString(strResponse))
        return static_cast<ADDON_STATUS>(response.result());
    }
  }

  return ADDON_STATUS_UNKNOWN;
}

bool CNetworkGame::HasSettings(void)
{
  return false;
}

unsigned int CNetworkGame::GetSettings(ADDON_StructSetting*** sSet)
{
  return 0;
}

ADDON_STATUS CNetworkGame::SetSetting(const std::string& settingName, const void* settingValue)
{
  return ADDON_STATUS_UNKNOWN;
}

void CNetworkGame::FreeSettings(void)
{
}

void CNetworkGame::Announce(const std::string& flag, const std::string& sender, const std::string& message, const void* data)
{
  addon::AnnounceRequest request;
  request.set_flag(flag);
  request.set_sender(sender);
  request.set_msg(message);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc.Send(RPC_METHOD::Announce, strRequest, strResponse);
  }
}

std::string CNetworkGame::GetGameAPIVersion(void)
{
  game::GetGameAPIVersionRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::GetGameAPIVersion, strRequest, strResponse))
    {
      game::GetGameAPIVersionResponse response;
      if (response.ParseFromString(strResponse))
        return response.result();
    }
  }

  return GAME_API_VERSION;
}

std::string CNetworkGame::GetMininumGameAPIVersion(void)
{
  game::GetMininumGameAPIVersionRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::GetMininumGameAPIVersion, strRequest, strResponse))
    {
      game::GetMininumGameAPIVersionResponse response;
      if (response.ParseFromString(strResponse))
        return response.result();
    }
  }

  return GAME_MIN_API_VERSION;
}

GAME_ERROR CNetworkGame::LoadGame(const std::string& url)
{
  game::LoadGameRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::LoadGame, strRequest, strResponse))
    {
      game::LoadGameResponse response;
      if (response.ParseFromString(strResponse))
        return static_cast<GAME_ERROR>(response.result());
    }
  }

  return GAME_ERROR_FAILED;
}

GAME_ERROR CNetworkGame::LoadGameSpecial(SPECIAL_GAME_TYPE type, const char** urls, size_t urlCount)
{
  game::LoadGameSpecialRequest request;
  request.set_type(type);
  for (unsigned int i = 0; i < urlCount; i++)
    request.add_url(urls[i]);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::LoadGameSpecial, strRequest, strResponse))
    {
      game::LoadGameSpecialResponse response;
      if (response.ParseFromString(strResponse))
        return static_cast<GAME_ERROR>(response.result());
    }
  }

  return GAME_ERROR_FAILED;
}

GAME_ERROR CNetworkGame::LoadStandalone(void)
{
  game::LoadStandaloneRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::LoadStandalone, strRequest, strResponse))
    {
      game::LoadStandaloneResponse response;
      if (response.ParseFromString(strResponse))
        return static_cast<GAME_ERROR>(response.result());
    }
  }

  return GAME_ERROR_FAILED;
}

GAME_ERROR CNetworkGame::UnloadGame(void)
{
  game::UnloadGameRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::UnloadGame, strRequest, strResponse))
    {
      game::UnloadGameResponse response;
      if (response.ParseFromString(strResponse))
        return static_cast<GAME_ERROR>(response.result());
    }
  }

  return GAME_ERROR_FAILED;
}

GAME_ERROR CNetworkGame::GetGameInfo(game_system_av_info* info)
{
  GAME_ERROR result(GAME_ERROR_FAILED);

  game::GetGameInfoRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::GetGameInfo, strRequest, strResponse))
    {
      game::GetGameInfoResponse response;
      if (response.ParseFromString(strResponse))
      {
        result = static_cast<GAME_ERROR>(response.result());
        if (result == GAME_ERROR_NO_ERROR)
        {
          info->geometry.base_width = response.info().geometry().base_width();
          info->geometry.base_height = response.info().geometry().base_height();
          info->geometry.max_width = response.info().geometry().max_width();
          info->geometry.max_height = response.info().geometry().max_height();
          info->timing.fps = response.info().timing().fps();
          info->timing.sample_rate = response.info().timing().sample_rate();
        }
      }
    }
  }

  return result;
}

GAME_REGION CNetworkGame::GetRegion(void)
{
  game::GetRegionRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::GetRegion, strRequest, strResponse))
    {
      game::GetRegionResponse response;
      if (response.ParseFromString(strResponse))
        return static_cast<GAME_REGION>(response.result());
    }
  }

  return GAME_REGION_UNKNOWN;
}

void CNetworkGame::FrameEvent(void)
{
  game::FrameEventRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc.Send(RPC_METHOD::FrameEvent, strRequest, strResponse);
  }
}

GAME_ERROR CNetworkGame::Reset(void)
{
  game::ResetRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::Reset, strRequest, strResponse))
    {
      game::ResetResponse response;
      if (response.ParseFromString(strResponse))
        return static_cast<GAME_ERROR>(response.result());
    }
  }

  return GAME_ERROR_FAILED;
}

GAME_ERROR CNetworkGame::HwContextReset(void)
{
  return GAME_ERROR_NOT_IMPLEMENTED;
}

GAME_ERROR CNetworkGame::HwContextDestroy(void)
{
  return GAME_ERROR_NOT_IMPLEMENTED;
}

void CNetworkGame::UpdatePort(unsigned int port, bool connected, const game_controller* controller)
{
  game::UpdatePortRequest request;
  request.set_port(port);
  request.set_connected(connected);
  request.mutable_controller()->set_controller_id(controller->controller_id ? controller->controller_id : "");
  request.mutable_controller()->set_digital_button_count(controller->digital_button_count);
  request.mutable_controller()->set_analog_button_count(controller->analog_button_count);
  request.mutable_controller()->set_analog_stick_count(controller->analog_stick_count);
  request.mutable_controller()->set_accelerometer_count(controller->accelerometer_count);
  request.mutable_controller()->set_key_count(controller->key_count);
  request.mutable_controller()->set_rel_pointer_count(controller->rel_pointer_count);
  request.mutable_controller()->set_abs_pointer_count(controller->abs_pointer_count);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc.Send(RPC_METHOD::UpdatePort, strRequest, strResponse);
  }
}

bool CNetworkGame::InputEvent(unsigned int port, const game_input_event* event)
{
  game::InputEventRequest request;
  request.set_port(port);
  request.mutable_event()->set_type(event->type);
  request.mutable_event()->set_port(event->port);
  request.mutable_event()->set_controller_id(event->controller_id ? event->controller_id : "");
  request.mutable_event()->set_feature_name(event->feature_name ? event->feature_name : "");
  switch (event->type)
  {
    case GAME_INPUT_EVENT_DIGITAL_BUTTON:
      request.mutable_event()->mutable_digital_button()->set_pressed(event->digital_button.pressed);
      break;
    case GAME_INPUT_EVENT_ANALOG_BUTTON:
      request.mutable_event()->mutable_analog_button()->set_magnitude(event->analog_button.magnitude);
      break;
    case GAME_INPUT_EVENT_ANALOG_STICK:
      request.mutable_event()->mutable_analog_stick()->set_x(event->analog_stick.x);
      request.mutable_event()->mutable_analog_stick()->set_y(event->analog_stick.y);
      break;
    case GAME_INPUT_EVENT_ACCELEROMETER:
      request.mutable_event()->mutable_accelerometer()->set_x(event->accelerometer.x);
      request.mutable_event()->mutable_accelerometer()->set_y(event->accelerometer.y);
      request.mutable_event()->mutable_accelerometer()->set_z(event->accelerometer.z);
      break;
    case GAME_INPUT_EVENT_KEY:
      request.mutable_event()->mutable_key()->set_pressed(event->key.pressed);
      request.mutable_event()->mutable_key()->set_character(event->key.character);
      request.mutable_event()->mutable_key()->set_modifiers(event->key.modifiers);
      break;
    case GAME_INPUT_EVENT_RELATIVE_POINTER:
      request.mutable_event()->mutable_rel_pointer()->set_x(event->rel_pointer.x);
      request.mutable_event()->mutable_rel_pointer()->set_y(event->rel_pointer.y);
      break;
    case GAME_INPUT_EVENT_ABSOLUTE_POINTER:
      request.mutable_event()->mutable_abs_pointer()->set_pressed(event->abs_pointer.pressed);
      request.mutable_event()->mutable_abs_pointer()->set_x(event->abs_pointer.x);
      request.mutable_event()->mutable_abs_pointer()->set_y(event->abs_pointer.y);
      break;
    default:
      break;
  }
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::InputEvent, strRequest, strResponse))
    {
      game::InputEventResponse response;
      if (response.ParseFromString(strResponse))
        return response.result();
    }
  }

  return false;
}

size_t CNetworkGame::SerializeSize(void)
{
  game::SerializeSizeRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::SerializeSize, strRequest, strResponse))
    {
      game::SerializeSizeResponse response;
      if (response.ParseFromString(strResponse))
        return response.result();
    }
  }

  return 0;
}

GAME_ERROR CNetworkGame::Serialize(uint8_t* data, size_t size)
{
  GAME_ERROR result(GAME_ERROR_FAILED);

  game::SerializeRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::Serialize, strRequest, strResponse))
    {
      game::SerializeResponse response;
      if (response.ParseFromString(strResponse))
      {
        result = static_cast<GAME_ERROR>(response.result());
        if (result == GAME_ERROR_NO_ERROR)
        {
          size = std::min(size, response.data().length());
          std::memcpy(data, response.data().c_str(), size);
        }
      }
    }
  }

  return result;
}

GAME_ERROR CNetworkGame::Deserialize(const uint8_t* data, size_t size)
{
  game::DeserializeRequest request;
  request.mutable_data()->resize(size);
  std::memcpy(const_cast<char*>(request.mutable_data()->c_str()), data, size);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::Deserialize, strRequest, strResponse))
    {
      game::DeserializeResponse response;
      if (response.ParseFromString(strResponse))
        return static_cast<GAME_ERROR>(response.result());
    }
  }

  return GAME_ERROR_FAILED;
}

GAME_ERROR CNetworkGame::CheatReset(void)
{
  game::CheatResetRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::CheatReset, strRequest, strResponse))
    {
      game::CheatResetResponse response;
      if (response.ParseFromString(strResponse))
        return static_cast<GAME_ERROR>(response.result());
    }
  }

  return GAME_ERROR_FAILED;
}

GAME_ERROR CNetworkGame::GetMemory(GAME_MEMORY type, const uint8_t** data, size_t* size)
{
  GAME_ERROR result(GAME_ERROR_FAILED);

  game::GetMemoryRequest request;
  request.set_type(type);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::GetMemory, strRequest, strResponse))
    {
      game::GetMemoryResponse response;
      if (response.ParseFromString(strResponse))
      {
        result = static_cast<GAME_ERROR>(response.result());
        if (result == GAME_ERROR_NO_ERROR)
        {
          std::vector<uint8_t>& memory = m_memory[type];
          memory.resize(response.data().length());
          std::memcpy(memory.data(), response.data().c_str(), response.data().length());
          *data = memory.data();
          *size = response.data().length();
        }
      }
    }
  }

  return result;
}

GAME_ERROR CNetworkGame::SetCheat(unsigned int index, bool enabled, const std::string& code)
{
  game::SetCheatRequest request;
  request.set_index(index);
  request.set_enabled(enabled);
  request.set_code(code);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::SetCheat, strRequest, strResponse))
    {
      game::SetCheatResponse response;
      if (response.ParseFromString(strResponse))
        return static_cast<GAME_ERROR>(response.result());
    }
  }

  return GAME_ERROR_FAILED;
}
