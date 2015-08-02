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

#include "GameHandler.h"
#include "Client.h"
#include "interface/IGame.h"
#include "log/Log.h"
#include "utils/Version.h"

#include "kodi/kodi_addon_utils.hpp"
#include "kodi/kodi_game_types.h"

#include "MessageIncludes.h"

#include <assert.h>
#include <cstring>

using namespace NETPLAY;

CGameHandler::CGameHandler(IGame* gameCallback) :
  m_game(gameCallback)
{
  assert(m_game);
}

bool CGameHandler::HandleRequest(RPC_METHOD method, const std::string& strRequest, CClient* client)
{
  if (!client)
    return false;

  std::string strResponse;

  switch (method)
  {
    case RPC_METHOD::Login:
    {
      addon::LoginRequest request;
      if (request.ParseFromString(strRequest))
      {
        bool bLoginSuccess = false;

        Version networkVersion(request.game_version_major(),
                               request.game_version_minor(),
                               request.game_version_point());
        Version networkMinVersion(request.min_version_major(),
                                  request.min_version_minor(),
                                  request.min_version_point());

        static const Version myMinVersion(GAME_MIN_API_VERSION);
        static const Version myVersion(GAME_API_VERSION);

        const bool bCompatible = (myMinVersion <= networkVersion && networkMinVersion <= myVersion);
        if (!bCompatible)
        {
          esyslog("Client login rejected - incompatible version: %s", networkVersion.ToString().c_str());
        }
        else
        {
          dsyslog("Client logged in");
          bLoginSuccess = true;
        }

        addon::LoginResponse response;
        response.set_result(bLoginSuccess);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::Logout:
    {
      addon::LogoutRequest request;
      if (request.ParseFromString(strRequest))
      {
        addon::LogoutResponse response;
        if (response.SerializeToString(&strResponse))
        {
          if (client->SendResponse(method, strResponse))
          {
            client->Deinitialize();
            return true;
          }
        }

      }
      break;
    }
    case RPC_METHOD::GetStatus:
    {
      addon::GetStatusRequest request;
      if (request.ParseFromString(strRequest))
      {
        ADDON_STATUS result = m_game->GetStatus();

        addon::GetStatusResponse response;
        response.set_result(result);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::SetSetting:
    {
      /* TODO
      addon::SetSettingRequest request;
      if (request.ParseFromString(strRequest))
      {
        addon::SetSettingResponse response;
        response.set_result(result);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      */
      return true;
    }
    case RPC_METHOD::Announce:
    {
      addon::AnnounceRequest request;
      if (request.ParseFromString(strRequest))
      {
        m_game->Announce(request.flag(), request.sender(), request.msg(), NULL);

        addon::AnnounceResponse response;
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::GetGameInfo:
    {
      game::GetGameInfoRequest request;
      if (request.ParseFromString(strRequest))
      {
        game_system_av_info info = { };
        GAME_ERROR result = m_game->GetGameInfo(&info);

        game::GetGameInfoResponse response;
        response.set_result(result);
        response.mutable_info()->mutable_geometry()->set_base_width(info.geometry.base_width);
        response.mutable_info()->mutable_geometry()->set_base_height(info.geometry.base_height);
        response.mutable_info()->mutable_geometry()->set_max_width(info.geometry.max_width);
        response.mutable_info()->mutable_geometry()->set_max_height(info.geometry.max_height);
        response.mutable_info()->mutable_timing()->set_fps(info.timing.fps);
        response.mutable_info()->mutable_timing()->set_sample_rate(info.timing.sample_rate);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::GetRegion:
    {
      game::GetRegionRequest request;
      if (request.ParseFromString(strRequest))
      {
        GAME_REGION result = m_game->GetRegion();

        game::GetRegionResponse response;
        response.set_result(result);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::FrameEvent:
    {
      game::FrameEventRequest request;
      if (request.ParseFromString(strRequest))
      {
        m_game->FrameEvent();

        game::FrameEventResponse response;
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::Reset:
    {
      game::ResetRequest request;
      if (request.ParseFromString(strRequest))
      {
        GAME_ERROR result = m_game->Reset();

        game::ResetResponse response;
        response.set_result(result);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::UpdatePort:
    {
      game::UpdatePortRequest request;
      if (request.ParseFromString(strRequest))
      {
        game_controller controller;
        controller.controller_id        = request.controller().controller_id().c_str();
        controller.digital_button_count = request.controller().digital_button_count();
        controller.analog_button_count  = request.controller().analog_button_count();
        controller.analog_stick_count   = request.controller().analog_stick_count();
        controller.accelerometer_count  = request.controller().accelerometer_count();
        controller.key_count            = request.controller().key_count();
        controller.rel_pointer_count    = request.controller().rel_pointer_count();
        controller.abs_pointer_count    = request.controller().abs_pointer_count();

        m_game->UpdatePort(request.port(), request.connected(), &controller);

        game::UpdatePortResponse response;
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::InputEvent:
    {
      game::InputEventRequest request;
      if (request.ParseFromString(strRequest))
      {
        game_input_event event = { };
        event.type          = static_cast<GAME_INPUT_EVENT_SOURCE>(request.event().type());
        event.port          = request.event().port();
        event.controller_id = request.event().controller_id().c_str();
        event.feature_name  = request.event().feature_name().c_str();
        switch (event.type)
        {
          case GAME_INPUT_EVENT_DIGITAL_BUTTON:
            event.digital_button.pressed = request.event().digital_button().pressed();
            break;
          case GAME_INPUT_EVENT_ANALOG_BUTTON:
            event.analog_button.magnitude = request.event().analog_button().magnitude();
            break;
          case GAME_INPUT_EVENT_ANALOG_STICK:
            event.analog_stick.x = request.event().analog_stick().x();
            event.analog_stick.y = request.event().analog_stick().y();
            break;
          case GAME_INPUT_EVENT_ACCELEROMETER:
            event.accelerometer.x = request.event().accelerometer().x();
            event.accelerometer.y = request.event().accelerometer().y();
            event.accelerometer.z = request.event().accelerometer().z();
            break;
          case GAME_INPUT_EVENT_KEY:
            event.key.pressed   = request.event().key().pressed();
            event.key.character = request.event().key().character();
            event.key.modifiers = static_cast<GAME_KEY_MOD>(request.event().key().modifiers());
            break;
          case GAME_INPUT_EVENT_RELATIVE_POINTER:
            event.rel_pointer.x = request.event().rel_pointer().x();
            event.rel_pointer.y = request.event().rel_pointer().y();
            break;
          case GAME_INPUT_EVENT_ABSOLUTE_POINTER:
            event.abs_pointer.pressed = request.event().abs_pointer().pressed();
            event.abs_pointer.x       = request.event().abs_pointer().x();
            event.abs_pointer.y       = request.event().abs_pointer().y();
            break;
          default:
            break;
        }

        bool result = m_game->InputEvent(request.port(), &event);

        game::InputEventResponse response;
        response.set_result(result);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::SerializeSize:
    {
      game::SerializeSizeRequest request;
      if (request.ParseFromString(strRequest))
      {
        size_t result = m_game->SerializeSize();

        game::SerializeSizeResponse response;
        response.set_result(result);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::Serialize:
    {
      game::SerializeRequest request;
      if (request.ParseFromString(strRequest))
      {
        GAME_ERROR result(GAME_ERROR_FAILED);
        std::vector<uint8_t> data;

        const unsigned int size = m_game->SerializeSize();
        if (size > 0)
        {
          data.resize(size);
          result = m_game->Serialize(data.data(), size);
        }

        game::SerializeResponse response;
        response.set_result(result);
        if (size > 0)
          response.set_data(data.data(), size);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::Deserialize:
    {
      game::DeserializeRequest request;
      if (request.ParseFromString(strRequest))
      {
        std::string data = request.data();
        GAME_ERROR result = m_game->Deserialize(reinterpret_cast<const uint8_t*>(data.c_str()),
                                                data.size());

        game::DeserializeResponse response;
        response.set_result(result);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::CheatReset:
    {
      game::CheatResetRequest request;
      if (request.ParseFromString(strRequest))
      {
        GAME_ERROR result = m_game->CheatReset();

        game::CheatResetResponse response;
        response.set_result(result);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::GetMemory:
    {
      game::GetMemoryRequest request;
      if (request.ParseFromString(strRequest))
      {
        const uint8_t* data;
        size_t size;

        GAME_ERROR result = m_game->GetMemory(static_cast<GAME_MEMORY>(request.type()),
                                              &data,
                                              &size);

        game::GetMemoryResponse response;
        response.set_result(result);
        if (result == GAME_ERROR_NO_ERROR && size > 0)
          response.set_data(data, size);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::SetCheat:
    {
      game::SetCheatRequest request;
      if (request.ParseFromString(strRequest))
      {
        GAME_ERROR result = m_game->SetCheat(request.index(), request.enabled(), request.code().c_str());

        game::SetCheatResponse response;
        response.set_result(result);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    default:
      break;
  }

  esyslog("Invalid game method: %d", method);

  return false;
}
