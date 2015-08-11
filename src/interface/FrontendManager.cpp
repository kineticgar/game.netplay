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

#include "FrontendManager.h"
#include "interface/IFrontend.h"
#include "interface/IGame.h"

#include "kodi/libKODI_game.h"
#include "kodi/libXBMC_addon.h"

#include <algorithm>
#include <assert.h>

using namespace NETPLAY;

void CFrontendManager::RegisterFrontend(IFrontend* frontend)
{
  m_frontends.push_back(frontend);
}

bool CFrontendManager::UnregisterFrontend(IFrontend* frontend)
{
  const unsigned int oldSize = m_frontends.size();

  m_frontends.erase(std::remove(m_frontends.begin(), m_frontends.end(), frontend), m_frontends.end());

  return oldSize != m_frontends.size();
}

IFrontend* CFrontendManager::GetMaster(void)
{
  if (!m_frontends.empty())
    return *m_frontends.begin();

  return NULL;
}

void CFrontendManager::Log(const ADDON::addon_log_t loglevel, const char* msg)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->Log(loglevel, msg);
}

bool CFrontendManager::GetSetting(const char* settingName, void* settingValue)
{
  IFrontend* master = GetMaster();
  if (master)
    return master->GetSetting(settingName, settingValue);

  return false;
}

void CFrontendManager::QueueNotification(const ADDON::queue_msg_t type, const char* msg)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->QueueNotification(type, msg);
}

bool CFrontendManager::WakeOnLan(const char* mac)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->WakeOnLan(mac))
      return true;
  }

  return false;
}

std::string CFrontendManager::UnknownToUTF8(const char* str)
{
  IFrontend* master = GetMaster();
  if (master)
    return master->UnknownToUTF8(str);

  return "";
}

std::string CFrontendManager::GetLocalizedString(int dwCode, const char* strDefault /* = "" */)
{
  IFrontend* master = GetMaster();
  if (master)
    return master->GetLocalizedString(dwCode, strDefault);

  return "";
}

std::string CFrontendManager::GetDVDMenuLanguage(void)
{
  IFrontend* master = GetMaster();
  if (master)
    return master->GetDVDMenuLanguage();

  return "";
}

void* CFrontendManager::OpenFile(const char* strFileName, unsigned int flags)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    void* file = (*it)->OpenFile(strFileName, flags);
    if (file)
      return file;
  }

  return NULL;
}

void* CFrontendManager::OpenFileForWrite(const char* strFileName, bool bOverWrite)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    void* file = (*it)->OpenFileForWrite(strFileName, bOverWrite);
    if (file)
      return file;
  }

  return NULL;
}

ssize_t CFrontendManager::ReadFile(void* file, void* lpBuf, size_t uiBufSize)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    ssize_t result = (*it)->ReadFile(file, lpBuf, uiBufSize);
    if (result != -1)
      return result;
  }

  return -1;
}

bool CFrontendManager::ReadFileString(void* file, char* szLine, int iLineLength)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->ReadFileString(file, szLine, iLineLength))
      return true;
  }

  return false;
}

ssize_t CFrontendManager::WriteFile(void* file, const void* lpBuf, size_t uiBufSize)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    ssize_t result = (*it)->WriteFile(file, lpBuf, uiBufSize);
    if (result != -1)
      return result;
  }

  return -1;
}

void CFrontendManager::FlushFile(void* file)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->FlushFile(file);
}

int64_t CFrontendManager::SeekFile(void* file, int64_t iFilePosition, int iWhence)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    int64_t result = (*it)->SeekFile(file, iFilePosition, iWhence);
    if (result != -1)
      return result;
  }

  return -1;
}

int CFrontendManager::TruncateFile(void* file, int64_t iSize)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    int result = (*it)->TruncateFile(file, iSize);
    if (result != -1)
      return result;
  }

  return -1;
}

int64_t CFrontendManager::GetFilePosition(void* file)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    int64_t result = (*it)->GetFilePosition(file);
    if (result != -1)
      return result;
  }

  return -1;
}

int64_t CFrontendManager::GetFileLength(void* file)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    int64_t result = (*it)->GetFileLength(file);
    if (result != -1)
      return result;
  }

  return -1;
}

void CFrontendManager::CloseFile(void* file)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->CloseFile(file);
}

int CFrontendManager::GetFileChunkSize(void* file)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    int result = (*it)->GetFileChunkSize(file);
    if (result != -1)
      return result;
  }

  return -1;
}

bool CFrontendManager::FileExists(const char* strFileName, bool bUseCache)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->FileExists(strFileName, bUseCache))
      return true;
  }

  return false;
}

bool CFrontendManager::StatFile(const char* strFileName, STAT_STRUCTURE& buffer)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->StatFile(strFileName, buffer))
      return true;
  }

  return false;
}

bool CFrontendManager::DeleteFile(const char* strFileName)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->DeleteFile(strFileName))
      return true;
  }

  return false;
}

bool CFrontendManager::CanOpenDirectory(const char* strUrl)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->CanOpenDirectory(strUrl))
      return true;
  }

  return false;
}

bool CFrontendManager::CreateDirectory(const char* strPath)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->CreateDirectory(strPath))
      return true;
  }

  return false;
}

bool CFrontendManager::DirectoryExists(const char* strPath)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->DirectoryExists(strPath))
      return true;
  }

  return false;
}

bool CFrontendManager::RemoveDirectory(const char* strPath)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->RemoveDirectory(strPath))
      return true;
  }

  return false;
}

void CFrontendManager::CloseGame(void)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->CloseGame();
}

void CFrontendManager::VideoFrame(const uint8_t* data, unsigned int width, unsigned int height, GAME_RENDER_FORMAT format)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->VideoFrame(data, width, height, format);
}

void CFrontendManager::AudioFrames(const uint8_t* data, unsigned int frames, GAME_AUDIO_FORMAT format)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->AudioFrames(data, frames, format);
}

void CFrontendManager::HwSetInfo(const game_hw_info* hw_info)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->HwSetInfo(hw_info);
}

uintptr_t CFrontendManager::HwGetCurrentFramebuffer(void)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    uintptr_t framebuffer = (*it)->HwGetCurrentFramebuffer();
    if (framebuffer != 0)
      return framebuffer;
  }

  return 0;
}

game_proc_address_t CFrontendManager::HwGetProcAddress(const char* symbol)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    game_proc_address_t proc = (*it)->HwGetProcAddress(symbol);
    if (proc != NULL)
      return proc;
  }

  return NULL;
}

bool CFrontendManager::OpenPort(unsigned int port)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
  {
    if ((*it)->OpenPort(port))
      return true;
  }

  return false;
}

void CFrontendManager::ClosePort(unsigned int port)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->ClosePort(port);
}

void CFrontendManager::RumbleSetState(unsigned int port, GAME_RUMBLE_EFFECT effect, float strength)
{
  for (std::vector<IFrontend*>::iterator it = m_frontends.begin(); it != m_frontends.end(); ++it)
    (*it)->RumbleSetState(port, effect, strength);
}
