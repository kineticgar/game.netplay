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
#include "interface/IFrontend.h"
#include "interface/IGame.h"

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

IFrontend* CNetplay::GetMaster(void)
{
  if (!m_frontends.empty())
    return *m_frontends.begin();

  return NULL;
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

ADDON_STATUS CNetplay::SetSetting(const std::string& settingName, const void* settingValue)
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

void CNetplay::Announce(const std::string& flag, const std::string& sender, const std::string& message, const void* data)
{
  if (m_game)
    return m_game->Announce(flag, sender, message, data);
}

std::string CNetplay::GetGameAPIVersion(void)
{
  if (m_game)
    return m_game->GetGameAPIVersion();

  return GAME_API_VERSION;
}

std::string CNetplay::GetMininumGameAPIVersion(void)
{
  if (m_game)
    return m_game->GetMininumGameAPIVersion();

  return GAME_MIN_API_VERSION;
}

GAME_ERROR CNetplay::LoadGame(const std::string& url)
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

GAME_ERROR CNetplay::SetCheat(unsigned int index, bool enabled, const std::string& code)
{
  if (m_game)
    return m_game->SetCheat(index, enabled, code);

  return GAME_ERROR_FAILED;
}

void CNetplay::Log(const ADDON::addon_log_t loglevel, const std::string& msg)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->Log(loglevel, msg);
}

bool CNetplay::GetSetting(const std::string& settingName, void* settingValue)
{
  IFrontend* master = GetMaster();
  if (master)
    return master->GetSetting(settingName, settingValue);

  return false;
}

void CNetplay::QueueNotification(const ADDON::queue_msg_t type, const std::string& msg)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->QueueNotification(type, msg);
}

bool CNetplay::WakeOnLan(const std::string& mac)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->WakeOnLan(mac))
      return true;
  }

  return false;
}

std::string CNetplay::UnknownToUTF8(const std::string& str)
{
  IFrontend* master = GetMaster();
  if (master)
    return master->UnknownToUTF8(str);

  return "";
}

std::string CNetplay::GetLocalizedString(int dwCode, const std::string& strDefault /* = "" */)
{
  IFrontend* master = GetMaster();
  if (master)
    return master->GetLocalizedString(dwCode, strDefault);

  return "";
}

std::string CNetplay::GetDVDMenuLanguage(void)
{
  IFrontend* master = GetMaster();
  if (master)
    return master->GetDVDMenuLanguage();

  return "";
}

void* CNetplay::OpenFile(const std::string& strFileName, unsigned int flags)
{
  return NULL; // TODO
}

void* CNetplay::OpenFileForWrite(const std::string& strFileName, bool bOverWrite)
{
  return NULL; // TODO
}

ssize_t CNetplay::ReadFile(void* file, void* lpBuf, size_t uiBufSize)
{
  return 0; // TODO
}

bool CNetplay::ReadFileString(void* file, char* szLine, int iLineLength)
{
  return false; // TODO
}

ssize_t CNetplay::WriteFile(void* file, const void* lpBuf, size_t uiBufSize)
{
  return 0; // TODO
}

void CNetplay::FlushFile(void* file)
{
  // TODO
}

int64_t CNetplay::SeekFile(void* file, int64_t iFilePosition, int iWhence)
{
  return -1; // TODO
}

int CNetplay::TruncateFile(void* file, int64_t iSize)
{
  return -1; // TODO
}

int64_t CNetplay::GetFilePosition(void* file)
{
  return -1; // TODO
}

int64_t CNetplay::GetFileLength(void* file)
{
  return -1; // TODO
}

void CNetplay::CloseFile(void* file)
{
  // TODO
}

int CNetplay::GetFileChunkSize(void* file)
{
  return -1; // TODO
}

bool CNetplay::FileExists(const std::string& strFileName, bool bUseCache)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->FileExists(strFileName, bUseCache))
      return true;
  }

  return false;
}

/* TODO
int CNetplay::StatFile(const std::string& strFileName, struct __stat64* buffer)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->StatFile(strFileName, buffer) != -1)
      return 0;
  }

  return -1;
}
*/

bool CNetplay::DeleteFile(const std::string& strFileName)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->DeleteFile(strFileName))
      return true;
  }

  return false;
}

bool CNetplay::CanOpenDirectory(const std::string& strUrl)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->CanOpenDirectory(strUrl))
      return true;
  }

  return false;
}

bool CNetplay::CreateDirectory(const std::string& strPath)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->CreateDirectory(strPath))
      return true;
  }

  return false;
}

bool CNetplay::DirectoryExists(const std::string& strPath)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->DirectoryExists(strPath))
      return true;
  }

  return false;
}

bool CNetplay::RemoveDirectory(const std::string& strPath)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->RemoveDirectory(strPath))
      return true;
  }

  return false;
}

void CNetplay::CloseGame(void)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->CloseGame();
}

void CNetplay::VideoFrame(const uint8_t* data, unsigned int width, unsigned int height, GAME_RENDER_FORMAT format)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->VideoFrame(data, width, height, format);
}

unsigned int CNetplay::AudioFrames(const uint8_t* data, unsigned int frames, GAME_AUDIO_FORMAT format)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->AudioFrames(data, frames, format);

  return 0; // TODO
}

void CNetplay::HwSetInfo(const game_hw_info* hw_info)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->HwSetInfo(hw_info);
}

uintptr_t CNetplay::HwGetCurrentFramebuffer(void)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    uintptr_t framebuffer = (*it)->HwGetCurrentFramebuffer();
    if (framebuffer != 0)
      return framebuffer;
  }

  return 0;
}

game_proc_address_t CNetplay::HwGetProcAddress(const char* symbol)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    game_proc_address_t proc = (*it)->HwGetProcAddress(symbol);
    if (proc != NULL)
      return proc;
  }

  return NULL;
}

bool CNetplay::OpenPort(unsigned int port)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->OpenPort(port))
      return true;
  }

  return false;
}

void CNetplay::ClosePort(unsigned int port)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->ClosePort(port);
}

void CNetplay::RumbleSetState(unsigned int port, GAME_RUMBLE_EFFECT effect, float strength)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->RumbleSetState(port, effect, strength);
}

void CNetplay::SetCameraInfo(unsigned int width, unsigned int height, GAME_CAMERA_BUFFER caps)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->SetCameraInfo(width, height, caps);
}

bool CNetplay::StartCamera(void)
{
  bool bReturn = false;

  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    bReturn |= (*it)->StartCamera();

  return bReturn;
}

void CNetplay::StopCamera(void)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->StopCamera();
}

bool CNetplay::StartLocation(void)
{
  bool bReturn = false;

  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    bReturn |= (*it)->StartLocation();

  return bReturn;
}

void CNetplay::StopLocation(void)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->StopLocation();
}

bool CNetplay::GetLocation(double* lat, double* lon, double* horizAccuracy, double* vertAccuracy)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->GetLocation(lat, lon, horizAccuracy, vertAccuracy))
      return true;
  }

  return false;
}

void CNetplay::SetLocationInterval(unsigned int intervalMs, unsigned int intervalDistance)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->SetLocationInterval(intervalMs, intervalDistance);
}
