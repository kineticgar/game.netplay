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
#include "ConnectionFactory.h"
#include "IConnection.h"
#include "filesystem/StatStructure.h"

// clash between platform lib and protobuf
#if defined(MutexLock)
  #undef MutexLock
#endif

#include "addon.pb.h"
#include "game.pb.h"

#include <assert.h>
#include <cstring>

using namespace NETPLAY;

CNetworkFrontend::CNetworkFrontend(int fd) :
  m_rpc(CConnectionFactory::CreateConnection(fd))
{
  assert(m_rpc);
  m_rpc->RegisterObserver(this);
}

CNetworkFrontend::~CNetworkFrontend(void)
{
  Deinitialize();
  m_rpc->UnregisterObserver(this);
  delete m_rpc;
}

bool CNetworkFrontend::Initialize(void)
{
  return m_rpc->Open();
}

void CNetworkFrontend::Deinitialize(void)
{
  m_rpc->Close();
}

void CNetworkFrontend::Log(const ADDON::addon_log_t loglevel, const char* msg)
{
  addon::LogRequest request;
  request.set_level(loglevel);
  request.set_msg(msg);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc->Send(RPC_METHOD::Log, strRequest, strResponse);
  }
}

bool CNetworkFrontend::GetSetting(const char* settingName, void* settingValue)
{
  return false; // TODO
}

void CNetworkFrontend::QueueNotification(const ADDON::queue_msg_t type, const char* msg)
{
  addon::QueueNotificationRequest request;
  request.set_type(type);
  request.set_msg(msg);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc->Send(RPC_METHOD::QueueNotification, strRequest, strResponse);
  }
}

bool CNetworkFrontend::WakeOnLan(const char* mac)
{
  addon::WakeOnLanRequest request;
  request.set_mac_address(mac);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc->Send(RPC_METHOD::WakeOnLan, strRequest, strResponse))
    {
      addon::WakeOnLanResponse response;
      if (response.ParseFromString(strResponse))
        return response.result();
    }
  }

  return false;
}

std::string CNetworkFrontend::UnknownToUTF8(const char* str)
{
  addon::UnknownToUTF8Request request;
  request.set_str(str);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc->Send(RPC_METHOD::UnknownToUTF8, strRequest, strResponse))
    {
      addon::UnknownToUTF8Response response;
      if (response.ParseFromString(strResponse))
        return response.result();
    }
  }

  return "";
}

std::string CNetworkFrontend::GetLocalizedString(int dwCode, const char* strDefault /* = "" */)
{
  std::string result(strDefault);

  addon::GetLocalizedStringRequest request;
  request.set_code(dwCode);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc->Send(RPC_METHOD::GetLocalizedString, strRequest, strResponse))
    {
      addon::GetLocalizedStringResponse response;
      if (response.ParseFromString(strResponse))
        result = response.result();
    }
  }

  return result;
}

std::string CNetworkFrontend::GetDVDMenuLanguage()
{
  addon::GetDVDMenuLanguageRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc->Send(RPC_METHOD::GetDVDMenuLanguage, strRequest, strResponse))
    {
      addon::GetDVDMenuLanguageResponse response;
      if (response.ParseFromString(strResponse))
        return response.result();
    }
  }

  return "";
}

void* CNetworkFrontend::OpenFile(const char* strFileName, unsigned int flags)
{
  return NULL; // TODO
}

void* CNetworkFrontend::OpenFileForWrite(const char* strFileName, bool bOverWrite)
{
  return NULL; // TODO
}

ssize_t CNetworkFrontend::ReadFile(void* file, void* lpBuf, size_t uiBufSize)
{
  return 0; // TODO
}

bool CNetworkFrontend::ReadFileString(void* file, char* szLine, int iLineLength)
{
  return false; // TODO
}

ssize_t CNetworkFrontend::WriteFile(void* file, const void* lpBuf, size_t uiBufSize)
{
  return 0; // TODO
}

void CNetworkFrontend::FlushFile(void* file)
{
  // TODO
}

int64_t CNetworkFrontend::SeekFile(void* file, int64_t iFilePosition, int iWhence)
{
  return -1; // TODO
}

int CNetworkFrontend::TruncateFile(void* file, int64_t iSize)
{
  return -1; // TODO
}

int64_t CNetworkFrontend::GetFilePosition(void* file)
{
  return -1; // TODO
}

int64_t CNetworkFrontend::GetFileLength(void* file)
{
  return -1; // TODO
}

void CNetworkFrontend::CloseFile(void* file)
{
  // TODO
}

int CNetworkFrontend::GetFileChunkSize(void* file)
{
  return -1; // TODO
}

bool CNetworkFrontend::FileExists(const char* strFileName, bool bUseCache)
{
  return false; // TODO
}

bool CNetworkFrontend::StatFile(const char* strFileName, STAT_STRUCTURE& buffer)
{
  return false; // TODO
}

bool CNetworkFrontend::DeleteFile(const char* strFileName)
{
  return false; // TODO
}

bool CNetworkFrontend::CanOpenDirectory(const char* strUrl)
{
  return false; // TODO
}

bool CNetworkFrontend::CreateDirectory(const char* strPath)
{
  return false; // TODO
}

bool CNetworkFrontend::DirectoryExists(const char* strPath)
{
  return false; // TODO
}

bool CNetworkFrontend::RemoveDirectory(const char* strPath)
{
  return false; // TODO
}

void CNetworkFrontend::CloseGame(void)
{
  game::CloseGameRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc->Send(RPC_METHOD::CloseGame, strRequest, strResponse);
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

void CNetworkFrontend::VideoFrame(const uint8_t* data, unsigned int width, unsigned int height, GAME_RENDER_FORMAT format)
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
      m_rpc->Send(RPC_METHOD::VideoFrame, strRequest, strResponse);
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

void CNetworkFrontend::AudioFrames(const uint8_t* data, unsigned int frames, GAME_AUDIO_FORMAT format)
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
      m_rpc->Send(RPC_METHOD::AudioFrames, strRequest, strResponse);
    }
  }
}

void CNetworkFrontend::HwSetInfo(const game_hw_info* hw_info)
{
}

uintptr_t CNetworkFrontend::HwGetCurrentFramebuffer(void)
{
  return 0;
}

game_proc_address_t CNetworkFrontend::HwGetProcAddress(const char* symbol)
{
  return NULL;
}

bool CNetworkFrontend::OpenPort(unsigned int port)
{
  game::OpenPortRequest request;
  request.set_port(port);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc->Send(RPC_METHOD::OpenPort, strRequest, strResponse))
    {
      game::OpenPortResponse response;
      if (response.ParseFromString(strResponse))
        return response.result();
    }
  }

  return false;
}

void CNetworkFrontend::ClosePort(unsigned int port)
{
  game::ClosePortRequest request;
  request.set_port(port);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc->Send(RPC_METHOD::ClosePort, strRequest, strResponse);
  }
}

void CNetworkFrontend::RumbleSetState(unsigned int port, GAME_RUMBLE_EFFECT effect, float strength)
{
  game::RumbleSetStateRequest request;
  request.set_port(port);
  request.set_effect(effect);
  request.set_strength(strength);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc->Send(RPC_METHOD::RumbleSetState, strRequest, strResponse);
  }
}

void CNetworkFrontend::SetCameraInfo(unsigned int width, unsigned int height, GAME_CAMERA_BUFFER caps)
{
  // tODO
}

bool CNetworkFrontend::StartCamera(void)
{
  return false; // TODO
}

void CNetworkFrontend::StopCamera(void)
{
  // TODO
}

bool CNetworkFrontend::StartLocation(void)
{
  game::StartLocationRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc->Send(RPC_METHOD::StartLocation, strRequest, strResponse))
    {
      game::StartLocationResponse response;
      if (response.ParseFromString(strResponse))
        return response.result();
    }
  }

  return false;
}

void CNetworkFrontend::StopLocation(void)
{
  game::StopLocationRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc->Send(RPC_METHOD::StopLocation, strRequest, strResponse);
  }
}

bool CNetworkFrontend::GetLocation(double* lat, double* lon, double* horizAccuracy, double* vertAccuracy)
{
  game::GetLocationRequest request;
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    if (m_rpc->Send(RPC_METHOD::GetLocation, strRequest, strResponse))
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

void CNetworkFrontend::SetLocationInterval(unsigned int intervalMs, unsigned int intervalDistance)
{
  game::SetLocationIntervalRequest request;
  request.set_intervalms(intervalMs);
  request.set_intervaldistance(intervalDistance);
  std::string strRequest;
  if (request.SerializeToString(&strRequest))
  {
    std::string strResponse;
    m_rpc->Send(RPC_METHOD::SetLocationInterval, strRequest, strResponse);
  }
}

void CNetworkFrontend::Notify(const Observable& obs, const ObservableMessage msg)
{
  switch (msg)
  {
    case ObservableMessageConnectionLost:
    {
      SetChanged();
      NotifyObservers(msg);
      break;
    }
    default:
      break;
  }
}
