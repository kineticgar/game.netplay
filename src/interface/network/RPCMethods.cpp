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

#include "RPCMethods.h"

using namespace NETPLAY;

const char* RPCMethods::TranslateMethod(RPC_METHOD method)
{
  switch (method)
  {
    case RPC_METHOD::Login:               return "Login";
    case RPC_METHOD::Logout:              return "Logout";
    case RPC_METHOD::GetStatus:           return "GetStatus";
    case RPC_METHOD::SetSetting:          return "SetSetting";
    case RPC_METHOD::Announce:            return "Announce";
    case RPC_METHOD::Log:                 return "Log";
    case RPC_METHOD::GetSetting:          return "GetSetting";
    case RPC_METHOD::QueueNotification:   return "QueueNotification";
    case RPC_METHOD::WakeOnLan:           return "WakeOnLan";
    case RPC_METHOD::UnknownToUTF8:       return "UnknownToUTF8";
    case RPC_METHOD::GetLocalizedString:  return "GetLocalizedString";
    case RPC_METHOD::GetDVDMenuLanguage:  return "GetDVDMenuLanguage";
    case RPC_METHOD::GetGameInfo:         return "GetGameInfo";
    case RPC_METHOD::GetRegion:           return "GetRegion";
    case RPC_METHOD::FrameEvent:          return "FrameEvent";
    case RPC_METHOD::Reset:               return "Reset";
    case RPC_METHOD::UpdatePort:          return "UpdatePort";
    case RPC_METHOD::InputEvent:          return "InputEvent";
    case RPC_METHOD::SerializeSize:       return "SerializeSize";
    case RPC_METHOD::Serialize:           return "Serialize";
    case RPC_METHOD::Deserialize:         return "Deserialize";
    case RPC_METHOD::CheatReset:          return "CheatReset";
    case RPC_METHOD::GetMemory:           return "GetMemory";
    case RPC_METHOD::SetCheat:            return "SetCheat";
    case RPC_METHOD::CloseGame:           return "CloseGame";
    case RPC_METHOD::VideoFrame:          return "VideoFrame";
    case RPC_METHOD::AudioFrames:         return "AudioFrames";
    case RPC_METHOD::OpenPort:            return "OpenPort";
    case RPC_METHOD::ClosePort:           return "ClosePort";
    case RPC_METHOD::RumbleSetState:      return "RumbleSetState";
    case RPC_METHOD::StartLocation:       return "StartLocation";
    case RPC_METHOD::StopLocation:        return "StopLocation";
    case RPC_METHOD::GetLocation:         return "GetLocation";
    case RPC_METHOD::SetLocationInterval: return "SetLocationInterval";
    case RPC_METHOD::Invalid:
    default:
      break;
  };
  return "Unknown/invalid";
}
