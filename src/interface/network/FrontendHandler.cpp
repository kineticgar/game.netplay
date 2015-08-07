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

#include "FrontendHandler.h"
#include "Client.h"
#include "interface/IFrontend.h"
#include "log/Log.h"

#include "MessageIncludes.h"

#include <assert.h>
#include <cstring>

using namespace NETPLAY;

CFrontendHandler::CFrontendHandler(IFrontend* frontendCallback) :
  m_frontend(frontendCallback)
{
  assert(m_frontend);
}

bool CFrontendHandler::HandleRequest(RPC_METHOD method, const std::string& strRequest, CClient* client)
{
  if (!client)
    return false;

  std::string strResponse;

  switch (method)
  {
    case RPC_METHOD::Log:
    {
      addon::LogRequest request;
      if (request.ParseFromString(strRequest))
      {
        m_frontend->Log(static_cast<ADDON::addon_log_t>(request.level()), request.msg().c_str());

        addon::LogResponse response;
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::GetSetting:
    {
      addon::GetSettingRequest request;
      if (request.ParseFromString(strRequest))
      {
        bool result = m_frontend->GetSetting(request.setting_name().c_str(), const_cast<char*>(request.settings_value().c_str()));

        addon::GetSettingResponse response;
        response.set_result(result);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::QueueNotification:
    {
      addon::QueueNotificationRequest request;
      if (request.ParseFromString(strRequest))
      {
        m_frontend->QueueNotification(static_cast<ADDON::queue_msg_t>(request.type()), request.msg().c_str());

        addon::QueueNotificationResponse response;
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::WakeOnLan:
    {
      addon::WakeOnLanRequest request;
      if (request.ParseFromString(strRequest))
      {
        bool result = m_frontend->WakeOnLan(request.mac_address().c_str());

        addon::WakeOnLanResponse response;
        response.set_result(result);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::UnknownToUTF8:
    {
      addon::UnknownToUTF8Request request;
      if (request.ParseFromString(strRequest))
      {
        std::string result = m_frontend->UnknownToUTF8(request.str().c_str());

        addon::UnknownToUTF8Response response;
        response.set_result(result);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::GetLocalizedString:
    {
      addon::GetLocalizedStringRequest request;
      if (request.ParseFromString(strRequest))
      {
        std::string result = m_frontend->GetLocalizedString(request.code(), "");

        addon::GetLocalizedStringResponse response;
        response.set_result(result);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::GetDVDMenuLanguage:
    {
      addon::GetDVDMenuLanguageRequest request;
      if (request.ParseFromString(strRequest))
      {
        std::string result = m_frontend->GetDVDMenuLanguage();

        addon::GetDVDMenuLanguageResponse response;
        response.set_result(result);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::CloseGame:
    {
      game::CloseGameRequest request;
      if (request.ParseFromString(strRequest))
      {
        m_frontend->CloseGame();

        game::CloseGameResponse response;
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::VideoFrame:
    {
      game::VideoFrameRequest request;
      if (request.ParseFromString(strRequest))
      {
        m_frontend->VideoFrame(reinterpret_cast<const uint8_t*>(request.data().c_str()),
                               request.width(),
                               request.height(),
                               static_cast<GAME_RENDER_FORMAT>(request.format()));

        game::VideoFrameResponse response;
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::AudioFrames:
    {
      game::AudioFramesRequest request;
      if (request.ParseFromString(strRequest))
      {
        m_frontend->AudioFrames(reinterpret_cast<const uint8_t*>(request.data().c_str()),
                                request.frames(),
                                static_cast<GAME_AUDIO_FORMAT>(request.format()));

        game::AudioFramesResponse response;
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::OpenPort:
    {
      game::OpenPortRequest request;
      if (request.ParseFromString(strRequest))
      {
        bool result = m_frontend->OpenPort(request.port());

        game::OpenPortResponse response;
        response.set_result(result);
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::ClosePort:
    {
      game::ClosePortRequest request;
      if (request.ParseFromString(strRequest))
      {
        m_frontend->ClosePort(request.port());

        game::ClosePortResponse response;
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    case RPC_METHOD::RumbleSetState:
    {
      game::RumbleSetStateRequest request;
      if (request.ParseFromString(strRequest))
      {
        m_frontend->RumbleSetState(request.port(),
                                   static_cast<GAME_RUMBLE_EFFECT>(request.effect()),
                                   request.strength());

        game::RumbleSetStateResponse response;
        if (response.SerializeToString(&strResponse))
          return client->SendResponse(method, strResponse);
      }
      break;
    }
    default:
      break;
  }

  esyslog("Invalid frontend method: %d", method);

  return false;
}
