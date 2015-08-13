/*
 *      Copyright (C) 2015 Garrett Brown
 *      Copyright (C) 2015 Team XBMC
 *
 *  This Program is free software you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "FrontendCallbacks.h"
#include "FrontendCallbackLib.h"
#include "filesystem/StatStructure.h"
#include "interface/IFrontend.h"

#include <assert.h>
#include <cstdio>

using namespace NETPLAY;

// --- CFrontendCallbacks ------------------------------------------------------

IFrontend* CFrontendCallbacks::m_globalFrontend = NULL;

CFrontendCallbacks::CFrontendCallbacks(IFrontend* frontend) :
  m_frontend(frontend)
{
  m_globalFrontend = m_frontend;
}

IFrontend* CFrontendCallbacks::GetFrontend(void* addonData)
{
  CFrontendCallbackLib* callbackLib = static_cast<CFrontendCallbackLib*>(addonData);
  if (callbackLib == NULL || callbackLib->GetHelperAddon() == NULL)
    return NULL;

  return callbackLib->GetHelperAddon()->GetFrontend();
}

char* CFrontendCallbacks::DuplicateString(const std::string& str)
{
  char* ret = new char[str.length() + 1];
  std::strcpy(ret, str.c_str());
  return ret;
}

void CFrontendCallbacks::UnduplicateString(char* str)
{
  delete[] str;
}

// --- CFrontendCallbacksAddon -------------------------------------------------

CFrontendCallbacksAddon::CFrontendCallbacksAddon(IFrontend* frontend) :
  CFrontendCallbacks(frontend)
{
  m_callbacks.Log                = Log;
  m_callbacks.QueueNotification  = QueueNotification;
  m_callbacks.WakeOnLan          = WakeOnLan;
  m_callbacks.GetSetting         = GetSetting;
  m_callbacks.UnknownToUTF8      = UnknownToUTF8;
  m_callbacks.GetLocalizedString = GetLocalizedString;
  m_callbacks.GetDVDMenuLanguage = GetDVDMenuLanguage;
  m_callbacks.FreeString         = FreeString;
  m_callbacks.OpenFile           = OpenFile;
  m_callbacks.OpenFileForWrite   = OpenFileForWrite;
  m_callbacks.ReadFile           = ReadFile;
  m_callbacks.ReadFileString     = ReadFileString;
  m_callbacks.WriteFile          = WriteFile;
  m_callbacks.FlushFile          = FlushFile;
  m_callbacks.SeekFile           = SeekFile;
  m_callbacks.TruncateFile       = TruncateFile;
  m_callbacks.GetFilePosition    = GetFilePosition;
  m_callbacks.GetFileLength      = GetFileLength;
  m_callbacks.CloseFile          = CloseFile;
  m_callbacks.GetFileChunkSize   = GetFileChunkSize;
  m_callbacks.FileExists         = FileExists;
  m_callbacks.StatFile           = StatFile;
  m_callbacks.DeleteFile         = DeleteFile;
  m_callbacks.CanOpenDirectory   = CanOpenDirectory;
  m_callbacks.CreateDirectory    = CreateDirectory;
  m_callbacks.DirectoryExists    = DirectoryExists;
  m_callbacks.RemoveDirectory    = RemoveDirectory;
}

void CFrontendCallbacksAddon::Log(void* addonData, const ADDON::addon_log_t loglevel, const char* msg)
{
  IFrontend* frontend = GetFrontend(addonData);
  if (frontend == NULL)
    return;

  if (msg == NULL)
    msg = "";

  return frontend->Log(loglevel, msg);
}

bool CFrontendCallbacksAddon::GetSetting(void* addonData, const char* settingName, void* settingValue)
{
  IFrontend* frontend = GetFrontend(addonData);
  if (frontend == NULL)
    return false;

  return frontend->GetSetting(settingName ? settingName : "", settingValue);
}

void CFrontendCallbacksAddon::QueueNotification(void* addonData, const ADDON::queue_msg_t type, const char* msg)
{
  IFrontend* frontend = GetFrontend(addonData);
  if (frontend == NULL)
    return;

  if (msg == NULL)
    return;

  return frontend->QueueNotification(type, msg);
}

bool CFrontendCallbacksAddon::WakeOnLan(const char* mac)
{
  IFrontend* frontend = GetStaticFrontend();
  if (frontend == NULL)
    return false;

  if (mac == NULL)
    return false;

  return frontend->WakeOnLan(mac);
}

char* CFrontendCallbacksAddon::UnknownToUTF8(const char* sourceDest)
{
  IFrontend* frontend = GetStaticFrontend();
  if (frontend == NULL)
    return NULL;

  if (sourceDest == NULL)
    return NULL;

  return DuplicateString(frontend->UnknownToUTF8(sourceDest));
}

char* CFrontendCallbacksAddon::GetLocalizedString(const void* addonData, long dwCode)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return NULL;

  return DuplicateString(frontend->GetLocalizedString(dwCode));
}

char* CFrontendCallbacksAddon::GetDVDMenuLanguage(const void* addonData)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return NULL;

  return DuplicateString(frontend->GetDVDMenuLanguage());
}

void CFrontendCallbacksAddon::FreeString(const void* addonData, char* str)
{
  return UnduplicateString(str);
}

void* CFrontendCallbacksAddon::OpenFile(const void* addonData, const char* strFileName, unsigned int flags)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return NULL;

  if (strFileName == NULL)
    return NULL;

  return frontend->OpenFile(strFileName, flags);
}

void* CFrontendCallbacksAddon::OpenFileForWrite(const void* addonData, const char* strFileName, bool bOverWrite)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return NULL;

  if (strFileName == NULL)
    return NULL;

  return frontend->OpenFileForWrite(strFileName, bOverWrite);
}

ssize_t CFrontendCallbacksAddon::ReadFile(const void* addonData, void* file, void* lpBuf, size_t uiBufSize)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return 0;

  if (file == NULL || lpBuf == NULL || uiBufSize == 0)
    return 0;

  return frontend->ReadFile(file, lpBuf, uiBufSize);
}

bool CFrontendCallbacksAddon::ReadFileString(const void* addonData, void* file, char* szLine, int iLineLength)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return false;

  if (file == NULL || szLine == NULL || iLineLength == 0)
    return false;

  return frontend->ReadFileString(file, szLine, iLineLength);
}

ssize_t CFrontendCallbacksAddon::WriteFile(const void* addonData, void* file, const void* lpBuf, size_t uiBufSize)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return 0;

  if (file == NULL || lpBuf == NULL || uiBufSize == 0)
    return false;

  return frontend->WriteFile(file, lpBuf, uiBufSize);
}

void CFrontendCallbacksAddon::FlushFile(const void* addonData, void* file)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return;

  if (file == NULL)
    return;

  return frontend->FlushFile(file);
}

int64_t CFrontendCallbacksAddon::SeekFile(const void* addonData, void* file, int64_t iFilePosition, int iWhence)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return -1;

  if (file == NULL || iFilePosition < 0)
    return -1;

  return frontend->SeekFile(file, iFilePosition, iWhence);
}

int CFrontendCallbacksAddon::TruncateFile(const void* addonData, void* file, int64_t iSize)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return -1;

  if (file == NULL || iSize < 0)
    return -1;

  return frontend->TruncateFile(file, iSize);
}

int64_t CFrontendCallbacksAddon::GetFilePosition(const void* addonData, void* file)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return -1;

  if (file == NULL)
    return -1;

  return frontend->GetFilePosition(file);
}

int64_t CFrontendCallbacksAddon::GetFileLength(const void* addonData, void* file)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return -1;

  if (file == NULL)
    return -1;

  return frontend->GetFileLength(file);
}

void CFrontendCallbacksAddon::CloseFile(const void* addonData, void* file)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return;

  if (file == NULL)
    return;

  return frontend->CloseFile(file);
}

int CFrontendCallbacksAddon::GetFileChunkSize(const void* addonData, void* file)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return -1;

  if (file == NULL)
    return -1;

  return frontend->GetFileChunkSize(file);
}

bool CFrontendCallbacksAddon::FileExists(const void* addonData, const char* strFileName, bool bUseCache)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return false;

  if (strFileName == NULL)
    return false;

  return frontend->FileExists(strFileName, bUseCache);
}

int CFrontendCallbacksAddon::StatFile(const void* addonData, const char* strFileName, struct __stat64* buffer)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return -1;

  if (strFileName == NULL || buffer == NULL)
    return -1;

  STAT_STRUCTURE statStruct;
  if (frontend->StatFile(strFileName, statStruct))
  {
    StatTranslator::TranslateToStruct64(statStruct, *buffer);
    return 0;
  }

  return -1;
}

bool CFrontendCallbacksAddon::DeleteFile(const void* addonData, const char* strFileName)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return false;

  if (strFileName == NULL)
    return false;

  return frontend->DeleteFile(strFileName);
}

bool CFrontendCallbacksAddon::CanOpenDirectory(const void* addonData, const char* strURL)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return false;

  if (strURL == NULL)
    return false;

  return frontend->CanOpenDirectory(strURL);
}

bool CFrontendCallbacksAddon::CreateDirectory(const void* addonData, const char* strPath)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return false;

  if (strPath == NULL)
    return false;

  return frontend->CreateDirectory(strPath);
}

bool CFrontendCallbacksAddon::DirectoryExists(const void* addonData, const char* strPath)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return false;

  if (strPath == NULL)
    return false;

  return frontend->DirectoryExists(strPath);
}

bool CFrontendCallbacksAddon::RemoveDirectory(const void* addonData, const char* strPath)
{
  IFrontend* frontend = GetFrontend(const_cast<void*>(addonData));
  if (frontend == NULL)
    return false;

  if (strPath == NULL)
    return false;

  return frontend->RemoveDirectory(strPath);
}

// --- CFrontendCallbacksGame --------------------------------------------------

CFrontendCallbacksGame::CFrontendCallbacksGame(IFrontend* frontend) :
  CFrontendCallbacks(frontend)
{
  m_callbacks.CloseGame                      = CloseGame;
  m_callbacks.VideoFrame                     = VideoFrame;
  m_callbacks.AudioFrames                    = AudioFrames;
  m_callbacks.HwSetInfo                      = HwSetInfo;
  m_callbacks.HwGetCurrentFramebuffer        = HwGetCurrentFramebuffer;
  m_callbacks.HwGetProcAddress               = HwGetProcAddress;
  m_callbacks.OpenPort                       = OpenPort;
  m_callbacks.ClosePort                      = ClosePort;
  m_callbacks.RumbleSetState                 = RumbleSetState;
}

void CFrontendCallbacksGame::CloseGame(void* addonData)
{
  IFrontend* frontend = GetFrontend(addonData);
  if (frontend == NULL)
    return;

  return frontend->CloseGame();
}

void CFrontendCallbacksGame::VideoFrame(void* addonData, const uint8_t* data, unsigned int size, unsigned int width, unsigned int height, GAME_RENDER_FORMAT format)
{
  IFrontend* frontend = GetFrontend(addonData);
  if (frontend == NULL)
    return;

  if (data == NULL || width == 0 || height == 0)
    return;

  return frontend->VideoFrame(data, size, width, height, format);
}

void CFrontendCallbacksGame::AudioFrames(void* addonData, const uint8_t* data, unsigned int size, unsigned int frames, GAME_AUDIO_FORMAT format)
{
  IFrontend* frontend = GetFrontend(addonData);
  if (frontend == NULL)
    return;

  if (data == NULL || frames == 0)
    return;

  return frontend->AudioFrames(data, size, frames, format);
}

void CFrontendCallbacksGame::HwSetInfo(void* addonData, const game_hw_info* hw_info)
{
  IFrontend* frontend = GetFrontend(addonData);
  if (frontend == NULL)
    return;

  if (hw_info == NULL)
    return;

  return frontend->HwSetInfo(hw_info);
}

uintptr_t CFrontendCallbacksGame::HwGetCurrentFramebuffer(void* addonData)
{
  IFrontend* frontend = GetFrontend(addonData);
  if (frontend == NULL)
    return 0;

  return frontend->HwGetCurrentFramebuffer();
}

game_proc_address_t CFrontendCallbacksGame::HwGetProcAddress(void* addonData, const char* symbol)
{
  IFrontend* frontend = GetFrontend(addonData);
  if (frontend == NULL)
    return NULL;

  if (symbol == NULL)
    return NULL;

  return frontend->HwGetProcAddress(symbol);
}

bool CFrontendCallbacksGame::OpenPort(void* addonData, unsigned int port)
{
  IFrontend* frontend = GetFrontend(addonData);
  if (frontend == NULL)
    return false;

  return frontend->OpenPort(port);
}

void CFrontendCallbacksGame::ClosePort(void* addonData, unsigned int port)
{
  IFrontend* frontend = GetFrontend(addonData);
  if (frontend == NULL)
    return;

  return frontend->ClosePort(port);
}

void CFrontendCallbacksGame::RumbleSetState(void* addonData, unsigned int port, GAME_RUMBLE_EFFECT effect, float strength)
{
  IFrontend* frontend = GetFrontend(addonData);
  if (frontend == NULL)
    return;

  return frontend->RumbleSetState(port, effect, strength);
}
