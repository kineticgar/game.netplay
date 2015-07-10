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

#include "FrontendNetwork.h"

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
  // TODO
}

bool CFrontendNetwork::GetSetting(const std::string& settingName, void* settingValue)
{
  return false; // TODO
}

void CFrontendNetwork::QueueNotification(const ADDON::queue_msg_t type, const std::string& msg)
{
  // TODO
}

bool CFrontendNetwork::WakeOnLan(const std::string& mac)
{
  return false; // TODO
}

std::string CFrontendNetwork::UnknownToUTF8(const std::string& str)
{
  return ""; // TODO
}

std::string CFrontendNetwork::GetLocalizedString(int dwCode, const std::string& strDefault /* = "" */)
{
  return strDefault; // TODO
}

std::string CFrontendNetwork::GetDVDMenuLanguage()
{
  return ""; // TODO
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

int CFrontendNetwork::StatFile(const std::string& strFileName, struct __stat64* buffer)
{
  return 0; // TODO
}

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
