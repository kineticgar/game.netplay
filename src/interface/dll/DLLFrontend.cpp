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

#include "DLLFrontend.h"

#include "kodi/libXBMC_addon.h"
#include "kodi/libKODI_game.h"
#include "kodi/kodi_addon_utils.hpp"

using namespace ADDON;
using namespace NETPLAY;

CDLLFrontend::CDLLFrontend(void* callbacks)
{
  try
  {
    m_addon = new CHelper_libXBMC_addon;
    if (!m_addon->RegisterMe(callbacks))
      throw false;

    m_game = new CHelper_libKODI_game;
    if (!m_game->RegisterMe(callbacks))
      throw false;
  }
  catch (const bool& bSuccess)
  {
    if (!bSuccess)
      Deinitialize();
  }
}

bool CDLLFrontend::Initialize(void)
{
  return m_addon != NULL &&
         m_game  != NULL;
}

void CDLLFrontend::Deinitialize(void)
{
  delete m_addon;
  delete m_game;

  m_addon = NULL;
  m_game  = NULL;
}

void CDLLFrontend::Log(const addon_log_t loglevel, const std::string& msg)
{
  return m_addon->Log(loglevel, msg.c_str());
}

bool CDLLFrontend::GetSetting(const std::string& settingName, void* settingValue)
{
  return m_addon->GetSetting(settingName.c_str(), settingValue);
}

void CDLLFrontend::QueueNotification(const queue_msg_t type, const std::string& msg)
{
  return m_addon->QueueNotification(type, msg.c_str());
}

bool CDLLFrontend::WakeOnLan(const std::string& mac)
{
  return m_addon->WakeOnLan(mac.c_str());
}

std::string CDLLFrontend::UnknownToUTF8(const std::string& str)
{
  return AddonUtils::UnknownToUTF8(m_addon, str);
}

std::string CDLLFrontend::GetLocalizedString(int dwCode, const std::string& strDefault /* = "" */)
{
  return AddonUtils::GetLocalizedString(m_addon, dwCode, strDefault);
}

std::string CDLLFrontend::GetDVDMenuLanguage()
{
  return AddonUtils::GetDVDMenuLanguage(m_addon);
}

void* CDLLFrontend::OpenFile(const std::string& strFileName, unsigned int flags)
{
  return m_addon->OpenFile(strFileName.c_str(), flags);
}

void* CDLLFrontend::OpenFileForWrite(const std::string& strFileName, bool bOverWrite)
{
  return m_addon->OpenFileForWrite(strFileName.c_str(), bOverWrite);
}

ssize_t CDLLFrontend::ReadFile(void* file, void* lpBuf, size_t uiBufSize)
{
  return m_addon->ReadFile(file, lpBuf, uiBufSize);
}

bool CDLLFrontend::ReadFileString(void* file, char* szLine, int iLineLength)
{
  return m_addon->ReadFileString(file, szLine, iLineLength);
}

ssize_t CDLLFrontend::WriteFile(void* file, const void* lpBuf, size_t uiBufSize)
{
  return m_addon->WriteFile(file, lpBuf, uiBufSize);
}

void CDLLFrontend::FlushFile(void* file)
{
   return m_addon->FlushFile(file);
}

int64_t CDLLFrontend::SeekFile(void* file, int64_t iFilePosition, int iWhence)
{
  return m_addon->SeekFile(file, iFilePosition, iWhence);
}

int CDLLFrontend::TruncateFile(void* file, int64_t iSize)
{
  return m_addon->TruncateFile(file, iSize);
}

int64_t CDLLFrontend::GetFilePosition(void* file)
{
  return m_addon->GetFilePosition(file);
}

int64_t CDLLFrontend::GetFileLength(void* file)
{
  return m_addon->GetFileLength(file);
}

void CDLLFrontend::CloseFile(void* file)
{
  return m_addon->CloseFile(file);
}

int CDLLFrontend::GetFileChunkSize(void* file)
{
  return m_addon->GetFileChunkSize(file);
}

bool CDLLFrontend::FileExists(const std::string& strFileName, bool bUseCache)
{
  return m_addon->FileExists(strFileName.c_str(), bUseCache);
}

/* TODO
int CDLLFrontend::StatFile(const std::string& strFileName, struct __stat64* buffer)
{
  return m_addon->StatFile(strFileName.c_str(), buffer);
}
*/

bool CDLLFrontend::DeleteFile(const std::string& strFileName)
{
  return m_addon->DeleteFile(strFileName.c_str());
}

bool CDLLFrontend::CanOpenDirectory(const std::string& strUrl)
{
  return m_addon->CanOpenDirectory(strUrl.c_str());
}

bool CDLLFrontend::CreateDirectory(const std::string& strPath)
{
  return m_addon->CreateDirectory(strPath.c_str());
}

bool CDLLFrontend::DirectoryExists(const std::string& strPath)
{
  return m_addon->DirectoryExists(strPath.c_str());
}

bool CDLLFrontend::RemoveDirectory(const std::string& strPath)
{
  return m_addon->RemoveDirectory(strPath.c_str());
}

void CDLLFrontend::CloseGame(void)
{
  return m_game->CloseGame();
}

void CDLLFrontend::VideoFrame(const uint8_t* data, unsigned int width, unsigned int height, GAME_RENDER_FORMAT format)
{
  return m_game->VideoFrame(data, width, height, format);
}

void CDLLFrontend::AudioFrames(const uint8_t* data, unsigned int frames, GAME_AUDIO_FORMAT format)
{
  return m_game->AudioFrames(data, frames, format);
}

void CDLLFrontend::HwSetInfo(const game_hw_info* hw_info)
{
  return m_game->HwSetInfo(hw_info);
}

uintptr_t CDLLFrontend::HwGetCurrentFramebuffer(void)
{
  return m_game->HwGetCurrentFramebuffer();
}

game_proc_address_t CDLLFrontend::HwGetProcAddress(const char* symbol)
{
  return m_game->HwGetProcAddress(symbol);
}

bool CDLLFrontend::OpenPort(unsigned int port)
{
  return m_game->OpenPort(port);
}

void CDLLFrontend::ClosePort(unsigned int port)
{
  return m_game->ClosePort(port);
}

void CDLLFrontend::RumbleSetState(unsigned int port, GAME_RUMBLE_EFFECT effect, float strength)
{
  return m_game->RumbleSetState(port, effect, strength);
}

void CDLLFrontend::SetCameraInfo(unsigned int width, unsigned int height, GAME_CAMERA_BUFFER caps)
{
  return m_game->SetCameraInfo(width, height, caps);
}

bool CDLLFrontend::StartCamera(void)
{
  return m_game->StartCamera();
}

void CDLLFrontend::StopCamera(void)
{
  return m_game->StopCamera();
}

bool CDLLFrontend::StartLocation(void)
{
  return m_game->StartLocation();
}

void CDLLFrontend::StopLocation(void)
{
  return m_game->StopLocation();
}

bool CDLLFrontend::GetLocation(double* lat, double* lon, double* horizAccuracy, double* vertAccuracy)
{
  return m_game->GetLocation(lat, lon, horizAccuracy, vertAccuracy);
}

void CDLLFrontend::SetLocationInterval(unsigned int intervalMs, unsigned int intervalDistance)
{
  return m_game->SetLocationInterval(intervalMs, intervalDistance);
}
