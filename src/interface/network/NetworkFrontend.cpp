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

#include "NetworkFrontend.h"

#include "addon.pb.h"
#include "game.pb.h"

#include <cstring>

using namespace NETPLAY;

CFrontendNetwork::CFrontendNetwork(void)
{
}

bool CFrontendNetwork::Initialize(void)
{
  return false;
}

void CFrontendNetwork::Deinitialize(void)
{
}

void CFrontendNetwork::Log(const ADDON::addon_log_t loglevel, const std::string& msg)
{
  addon::LogRequest request;
  request.set_level(loglevel);
  request.set_message(msg);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc.Send(RPC_METHOD::Log, strRequest, strResponse);
  }
}

bool CFrontendNetwork::GetSetting(const std::string& settingName, void* settingValue)
{
  return false; // TODO
}

void CFrontendNetwork::QueueNotification(const ADDON::queue_msg_t type, const std::string& msg)
{
  addon::QueueNotificationRequest request;
  request.set_type(type);
  request.set_message(msg);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc.Send(RPC_METHOD::QueueNotification, strRequest, strResponse);
  }
}

bool CFrontendNetwork::WakeOnLan(const std::string& mac)
{
  addon::WakeOnLanRequest request;
  request.set_mac_address(mac);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::WakeOnLan, strRequest, strResponse))
    {
      addon::WakeOnLanResponse response;
      if (response.ParseFromString(strResponse))
        return response.result();
    }
  }

  return false;
}

std::string CFrontendNetwork::UnknownToUTF8(const std::string& str)
{
  addon::UnknownToUTF8Request request;
  request.set_str(str);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::UnknownToUTF8, strRequest, strResponse))
    {
      addon::UnknownToUTF8Response response;
      if (response.ParseFromString(strResponse))
        return response.result();
    }
  }

  return "";
}

std::string CFrontendNetwork::GetLocalizedString(int dwCode, const std::string& strDefault /* = "" */)
{
  std::string result(strDefault);

  addon::GetLocalizedStringRequest request;
  request.set_code(dwCode);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::GetLocalizedString, strRequest, strResponse))
    {
      addon::GetLocalizedStringResponse response;
      if (response.ParseFromString(strResponse))
        result = response.result();
    }
  }

  return result;
}

std::string CFrontendNetwork::GetDVDMenuLanguage()
{
  addon::GetDVDMenuLanguageRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::GetDVDMenuLanguage, strRequest, strResponse))
    {
      addon::GetDVDMenuLanguageResponse response;
      if (response.ParseFromString(strResponse))
        return response.result();
    }
  }

  return "";
}

void* CFrontendNetwork::OpenFile(const std::string& strFileName, unsigned int flags)
{
  return NULL; // TODO
}

void* CFrontendNetwork::OpenFileForWrite(const std::string& strFileName, bool bOverWrite)
{
  return NULL; // TODO
}

ssize_t CFrontendNetwork::ReadFile(void* file, void* lpBuf, size_t uiBufSize)
{
  return 0; // TODO
}

bool CFrontendNetwork::ReadFileString(void* file, char* szLine, int iLineLength)
{
  return false; // TODO
}

ssize_t CFrontendNetwork::WriteFile(void* file, const void* lpBuf, size_t uiBufSize)
{
  return 0; // TODO
}

void CFrontendNetwork::FlushFile(void* file)
{
  // TODO
}

int64_t CFrontendNetwork::SeekFile(void* file, int64_t iFilePosition, int iWhence)
{
  return -1; // TODO
}

int CFrontendNetwork::TruncateFile(void* file, int64_t iSize)
{
  return -1; // TODO
}

int64_t CFrontendNetwork::GetFilePosition(void* file)
{
  return -1; // TODO
}

int64_t CFrontendNetwork::GetFileLength(void* file)
{
  return -1; // TODO
}

void CFrontendNetwork::CloseFile(void* file)
{
  // TODO
}

int CFrontendNetwork::GetFileChunkSize(void* file)
{
  return -1; // TODO
}

bool CFrontendNetwork::FileExists(const std::string& strFileName, bool bUseCache)
{
  return false; // TODO
}

/* TODO
int CFrontendNetwork::StatFile(const std::string& strFileName, struct __stat64* buffer)
{
  return -1;
}
*/

bool CFrontendNetwork::DeleteFile(const std::string& strFileName)
{
  return false; // TODO
}

bool CFrontendNetwork::CanOpenDirectory(const std::string& strUrl)
{
  return false; // TODO
}

bool CFrontendNetwork::CreateDirectory(const std::string& strPath)
{
  return false; // TODO
}

bool CFrontendNetwork::DirectoryExists(const std::string& strPath)
{
  return false; // TODO
}

bool CFrontendNetwork::RemoveDirectory(const std::string& strPath)
{
  return false; // TODO
}

void CFrontendNetwork::CloseGame(void)
{
  game::CloseGameRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc.Send(RPC_METHOD::CloseGame, strRequest, strResponse);
  }
}

unsigned int GetStride(GAME_RENDER_FORMAT format, unsigned int width)
{
  switch (format)
  {
    case GAME_RENDER_FMT_0RGB8888:
      return width * 4;
    case GAME_RENDER_FMT_RGB565:
      return width * 2;
    case GAME_RENDER_FMT_0RGB1555:
      return width * 2;
    default:
      break;
  }
  return 0;
}

void CFrontendNetwork::VideoFrame(const uint8_t* data, unsigned int width, unsigned int height, GAME_RENDER_FORMAT format)
{
  game::VideoFrameRequest request;
  unsigned int size = GetStride(format, width) * height; // TODO
  if (size > 0)
  {
    request.mutable_data()->resize(size);
    std::memcpy(const_cast<char*>(request.mutable_data()->c_str()), data, size);
    request.set_width(width);
    request.set_height(height);
    request.set_format(format);
    std::string strRequest;
    if (request.SerializeToString(&strRequest))
    {
      std::string strResponse;
      m_rpc.Send(RPC_METHOD::VideoFrame, strRequest, strResponse);
    }
  }
}

unsigned int GetFrameSize(GAME_AUDIO_FORMAT format)
{
  switch (format)
  {
    case GAME_AUDIO_FMT_S16NE:
      return 4;
    default:
      break;
  }
  return 0;
}

void CFrontendNetwork::AudioFrames(const uint8_t* data, unsigned int frames, GAME_AUDIO_FORMAT format)
{
  game::AudioFramesRequest request;
  unsigned int size = GetFrameSize(format) * frames; // TODO
  if (size > 0)
  {
    request.mutable_data()->resize(size);
    std::memcpy(const_cast<char*>(request.mutable_data()->c_str()), data, size);
    request.set_frames(frames);
    request.set_format(format);
    std::string strRequest;
    if (request.SerializeToString(&strRequest))
    {
      std::string strResponse;
      m_rpc.Send(RPC_METHOD::AudioFrames, strRequest, strResponse);
    }
  }
}

void CFrontendNetwork::HwSetInfo(const game_hw_info* hw_info)
{
}

uintptr_t CFrontendNetwork::HwGetCurrentFramebuffer(void)
{
  return 0;
}

game_proc_address_t CFrontendNetwork::HwGetProcAddress(const char* symbol)
{
  return NULL;
}

bool CFrontendNetwork::OpenPort(unsigned int port)
{
  game::OpenPortRequest request;
  request.set_port(port);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::OpenPort, strRequest, strResponse))
    {
      game::OpenPortResponse response;
      if (response.ParseFromString(strResponse))
        return response.result();
    }
  }

  return false;
}

void CFrontendNetwork::ClosePort(unsigned int port)
{
  game::ClosePortRequest request;
  request.set_port(port);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc.Send(RPC_METHOD::ClosePort, strRequest, strResponse);
  }
}

void CFrontendNetwork::RumbleSetState(unsigned int port, GAME_RUMBLE_EFFECT effect, float strength)
{
  game::RumbleSetStateRequest request;
  request.set_port(port);
  request.set_effect(effect);
  request.set_strength(strength);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc.Send(RPC_METHOD::RumbleSetState, strRequest, strResponse);
  }
}

void CFrontendNetwork::SetCameraInfo(unsigned int width, unsigned int height, GAME_CAMERA_BUFFER caps)
{
  // tODO
}

bool CFrontendNetwork::StartCamera(void)
{
  return false; // TODO
}

void CFrontendNetwork::StopCamera(void)
{
  // TODO
}

bool CFrontendNetwork::StartLocation(void)
{
  game::StartLocationRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::StartLocation, strRequest, strResponse))
    {
      game::StartLocationResponse response;
      if (response.ParseFromString(strResponse))
        return response.result();
    }
  }

  return false;
}

void CFrontendNetwork::StopLocation(void)
{
  game::StopLocationRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc.Send(RPC_METHOD::StopLocation, strRequest, strResponse);
  }
}

bool CFrontendNetwork::GetLocation(double* lat, double* lon, double* horizAccuracy, double* vertAccuracy)
{
  game::GetLocationRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc.Send(RPC_METHOD::GetLocation, strRequest, strResponse))
    {
      game::GetLocationResponse response;
      if (response.ParseFromString(strResponse))
      {
        *lat = response.lat();
        *lon = response.lon();
        *horizAccuracy = response.horizaccuracy();
        *vertAccuracy = response.vertaccuracy();
        return response.result();
      }
    }
  }

  return false;
}

void CFrontendNetwork::SetLocationInterval(unsigned int intervalMs, unsigned int intervalDistance)
{
  game::SetLocationIntervalRequest request;
  request.set_intervalms(intervalMs);
  request.set_intervaldistance(intervalDistance);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc.Send(RPC_METHOD::SetLocationInterval, strRequest, strResponse);
  }
}
