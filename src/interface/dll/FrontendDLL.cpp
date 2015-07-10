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

#include "FrontendDLL.h"

#include "kodi/libXBMC_addon.h"
#include "kodi/libKODI_game.h"
#include "kodi/kodi_addon_utils.hpp"

using namespace ADDON;
using namespace NETPLAY;

CFrontendDLL::CFrontendDLL(void* callbacks)
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

bool CFrontendDLL::Initialize(void)
{
  return m_addon != NULL &&
         m_game  != NULL;
}

void CFrontendDLL::Deinitialize(void)
{
  delete m_addon;
  delete m_game;

  m_addon = NULL;
  m_game  = NULL;
}

void CFrontendDLL::Log(const addon_log_t loglevel, const std::string& msg)
{
  return m_addon->Log(loglevel, msg.c_str());
}

bool CFrontendDLL::GetSetting(const std::string& settingName, void* settingValue)
{
  return m_addon->GetSetting(settingName.c_str(), settingValue);
}

void CFrontendDLL::QueueNotification(const queue_msg_t type, const std::string& msg)
{
  return m_addon->QueueNotification(type, msg.c_str());
}

bool CFrontendDLL::WakeOnLan(const std::string& mac)
{
  return m_addon->WakeOnLan(mac.c_str());
}

std::string CFrontendDLL::UnknownToUTF8(const std::string& str)
{
  return AddonUtils::UnknownToUTF8(m_addon, str);
}

std::string CFrontendDLL::GetLocalizedString(int dwCode, const std::string& strDefault /* = "" */)
{
  return AddonUtils::GetLocalizedString(m_addon, dwCode, strDefault);
}

std::string CFrontendDLL::GetDVDMenuLanguage()
{
  return AddonUtils::GetDVDMenuLanguage(m_addon);
}

void* CFrontendDLL::OpenFile(const std::string& strFileName, unsigned int flags)
{
  return m_addon->OpenFile(strFileName.c_str(), flags);
}

void* CFrontendDLL::OpenFileForWrite(const std::string& strFileName, bool bOverWrite)
{
  return m_addon->OpenFileForWrite(strFileName.c_str(), bOverWrite);
}

ssize_t CFrontendDLL::ReadFile(void* file, void* lpBuf, size_t uiBufSize)
{
  return m_addon->ReadFile(file, lpBuf, uiBufSize);
}

bool CFrontendDLL::ReadFileString(void* file, char* szLine, int iLineLength)
{
  return m_addon->ReadFileString(file, szLine, iLineLength);
}

ssize_t CFrontendDLL::WriteFile(void* file, const void* lpBuf, size_t uiBufSize)
{
  return m_addon->WriteFile(file, lpBuf, uiBufSize);
}

void CFrontendDLL::FlushFile(void* file)
{
   return m_addon->FlushFile(file);
}

int64_t CFrontendDLL::SeekFile(void* file, int64_t iFilePosition, int iWhence)
{
  return m_addon->SeekFile(file, iFilePosition, iWhence);
}

int CFrontendDLL::TruncateFile(void* file, int64_t iSize)
{
  return m_addon->TruncateFile(file, iSize);
}

int64_t CFrontendDLL::GetFilePosition(void* file)
{
  return m_addon->GetFilePosition(file);
}

int64_t CFrontendDLL::GetFileLength(void* file)
{
  return m_addon->GetFileLength(file);
}

void CFrontendDLL::CloseFile(void* file)
{
  return m_addon->CloseFile(file);
}

int CFrontendDLL::GetFileChunkSize(void* file)
{
  return m_addon->GetFileChunkSize(file);
}

bool CFrontendDLL::FileExists(const std::string& strFileName, bool bUseCache)
{
  return m_addon->FileExists(strFileName.c_str(), bUseCache);
}

/* TODO
int CFrontendDLL::StatFile(const std::string& strFileName, struct __stat64* buffer)
{
  return m_addon->StatFile(strFileName.c_str(), buffer);
}
*/

bool CFrontendDLL::DeleteFile(const std::string& strFileName)
{
  return m_addon->DeleteFile(strFileName.c_str());
}

bool CFrontendDLL::CanOpenDirectory(const std::string& strUrl)
{
  return m_addon->CanOpenDirectory(strUrl.c_str());
}

bool CFrontendDLL::CreateDirectory(const std::string& strPath)
{
  return m_addon->CreateDirectory(strPath.c_str());
}

bool CFrontendDLL::DirectoryExists(const std::string& strPath)
{
  return m_addon->DirectoryExists(strPath.c_str());
}

bool CFrontendDLL::RemoveDirectory(const std::string& strPath)
{
  return m_addon->RemoveDirectory(strPath.c_str());
}
