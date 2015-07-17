/*
 *      Copyright (C) 2015 Garrett Brown
 *      Copyright (C) 2012-2015 Team XBMC
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

#include "interface/dll/FrontendCallbackLib.h"

#include "kodi/kodi_addon_callbacks.h"

#include <cstdio>

#ifdef _WIN32
  #include <windows.h>
  #define DLLEXPORT __declspec(dllexport)
#else
  #define DLLEXPORT
#endif

using namespace NETPLAY;

extern "C"
{

DLLEXPORT void* XBMC_register_me(AddonCB* handle)
{
  CB_AddOnLib* cbTable = NULL;

  if (handle == NULL)
  {
    std::fprintf(stderr, "libXBMC_addon-ERROR: XBMC_register_me is called with NULL handle !!!\n");
  }
  else
  {
    cbTable = handle->RegisterAddonLib(handle->addonData);
    if (handle == NULL || cbTable == NULL)
    {
      std::fprintf(stderr, "libXBMC_addon-ERROR: XBMC_register_me can't get callback table !!!\n");
    }
  }
  return cbTable;
}

DLLEXPORT void XBMC_unregister_me(AddonCB* handle, CB_AddOnLib* cbTable)
{
  if (handle == NULL || cbTable == NULL)
    return;

  handle->UnregisterAddonLib(handle->addonData, cbTable);
}

DLLEXPORT void XBMC_log(AddonCB* handle, CB_AddOnLib* cbTable, const ADDON::addon_log_t loglevel, const char* msg)
{
  if (handle == NULL || cbTable == NULL)
    return;

  cbTable->Log(handle->addonData, loglevel, msg);
}

DLLEXPORT bool XBMC_get_setting(AddonCB* handle, CB_AddOnLib* cbTable, const char* settingName, void* settingValue)
{
  if (handle == NULL || cbTable == NULL)
    return false;

  return cbTable->GetSetting(handle->addonData, settingName, settingValue);
}

DLLEXPORT void XBMC_queue_notification(AddonCB* handle, CB_AddOnLib* cbTable, const ADDON::queue_msg_t type, const char* msg)
{
  if (handle == NULL || cbTable == NULL)
    return;

  cbTable->QueueNotification(handle->addonData, type, msg);
}

DLLEXPORT bool XBMC_wake_on_lan(AddonCB* handle, CB_AddOnLib* cbTable, char* mac)
{
  if (handle == NULL || cbTable == NULL)
    return false;

  return cbTable->WakeOnLan(mac);
}

DLLEXPORT char* XBMC_unknown_to_utf8(AddonCB* handle, CB_AddOnLib* cbTable, const char* str)
{
  if (handle == NULL || cbTable == NULL)
    return NULL;

  return cbTable->UnknownToUTF8(str);
}

DLLEXPORT char* XBMC_get_localized_string(AddonCB* handle, CB_AddOnLib* cbTable, int dwCode)
{
  if (handle == NULL || cbTable == NULL)
    return NULL;

  return cbTable->GetLocalizedString(handle->addonData, dwCode);
}

DLLEXPORT char* XBMC_get_dvd_menu_language(AddonCB* handle, CB_AddOnLib* cbTable)
{
  if (handle == NULL || cbTable == NULL)
    return NULL;

  return cbTable->GetDVDMenuLanguage(handle->addonData);
}

DLLEXPORT void XBMC_free_string(AddonCB* handle, CB_AddOnLib* cbTable, char* str)
{
  if (handle == NULL || cbTable == NULL)
    return;

  cbTable->FreeString(handle->addonData, str);
}

DLLEXPORT void* XBMC_open_file(AddonCB* handle, CB_AddOnLib* cbTable, const char* strFileName, unsigned int flags)
{
  if (handle == NULL || cbTable == NULL)
    return NULL;

  return cbTable->OpenFile(handle->addonData, strFileName, flags);
}

DLLEXPORT void* XBMC_open_file_for_write(AddonCB* handle, CB_AddOnLib* cbTable, const char* strFileName, bool bOverWrite)
{
  if (handle == NULL || cbTable == NULL)
    return NULL;

  return cbTable->OpenFileForWrite(handle->addonData, strFileName, bOverWrite);
}

DLLEXPORT ssize_t XBMC_read_file(AddonCB* handle, CB_AddOnLib* cbTable, void* file, void* lpBuf, size_t uiBufSize)
{
  if (handle == NULL || cbTable == NULL)
    return -1;

  return cbTable->ReadFile(handle->addonData, file, lpBuf, uiBufSize);
}

DLLEXPORT bool XBMC_read_file_string(AddonCB* handle, CB_AddOnLib* cbTable, void* file, char* szLine, int iLineLength)
{
  if (handle == NULL || cbTable == NULL)
    return false;

  return cbTable->ReadFileString(handle->addonData, file, szLine, iLineLength);
}

DLLEXPORT ssize_t XBMC_write_file(AddonCB* handle, CB_AddOnLib* cbTable, void* file, const void* lpBuf, size_t uiBufSize)
{
  if (handle == NULL || cbTable == NULL)
    return -1;

  return cbTable->WriteFile(handle->addonData, file, lpBuf, uiBufSize);
}

DLLEXPORT void XBMC_flush_file(AddonCB* handle, CB_AddOnLib* cbTable, void* file)
{
  if (handle == NULL || cbTable == NULL)
    return;

  cbTable->FlushFile(handle->addonData, file);
}

DLLEXPORT int64_t XBMC_seek_file(AddonCB* handle, CB_AddOnLib* cbTable, void* file, int64_t iFilePosition, int iWhence)
{
  if (handle == NULL || cbTable == NULL)
    return 0;

  return cbTable->SeekFile(handle->addonData, file, iFilePosition, iWhence);
}

DLLEXPORT int XBMC_truncate_file(AddonCB* handle, CB_AddOnLib* cbTable, void* file, int64_t iSize)
{
  if (handle == NULL || cbTable == NULL)
    return 0;

  return cbTable->TruncateFile(handle->addonData, file, iSize);
}

DLLEXPORT int64_t XBMC_get_file_position(AddonCB* handle, CB_AddOnLib* cbTable, void* file)
{
  if (handle == NULL || cbTable == NULL)
    return 0;

  return cbTable->GetFilePosition(handle->addonData, file);
}

DLLEXPORT int64_t XBMC_get_file_length(AddonCB* handle, CB_AddOnLib* cbTable, void* file)
{
  if (handle == NULL || cbTable == NULL)
    return 0;

  return cbTable->GetFileLength(handle->addonData, file);
}

DLLEXPORT void XBMC_close_file(AddonCB* handle, CB_AddOnLib* cbTable, void* file)
{
  if (handle == NULL || cbTable == NULL)
    return;

  cbTable->CloseFile(handle->addonData, file);
}

DLLEXPORT int XBMC_get_file_chunk_size(AddonCB* handle, CB_AddOnLib* cbTable, void* file)
{
  if (handle == NULL || cbTable == NULL)
    return 0;

  return cbTable->GetFileChunkSize(handle->addonData, file);
}

DLLEXPORT bool XBMC_file_exists(AddonCB* handle, CB_AddOnLib* cbTable, const char* strFileName, bool bUseCache)
{
  if (handle == NULL || cbTable == NULL)
    return false;

  return cbTable->FileExists(handle->addonData, strFileName, bUseCache);
}

DLLEXPORT int XBMC_stat_file(AddonCB* handle, CB_AddOnLib* cbTable, const char* strFileName, struct __stat64* buffer)
{
  if (handle == NULL || cbTable == NULL)
    return -1;

  return cbTable->StatFile(handle->addonData, strFileName, buffer);
}

DLLEXPORT bool XBMC_delete_file(AddonCB* handle, CB_AddOnLib* cbTable, const char* strFileName)
{
  if (handle == NULL || cbTable == NULL)
    return false;

  return cbTable->DeleteFile(handle->addonData, strFileName);
}

DLLEXPORT bool XBMC_can_open_directory(AddonCB* handle, CB_AddOnLib* cbTable, const char* strURL)
{
  if (handle == NULL || cbTable == NULL)
    return false;

  return cbTable->CanOpenDirectory(handle->addonData, strURL);
}

DLLEXPORT bool XBMC_create_directory(AddonCB* handle, CB_AddOnLib* cbTable, const char* strPath)
{
  if (handle == NULL || cbTable == NULL)
    return false;

  return cbTable->CreateDirectory(handle->addonData, strPath);
}

DLLEXPORT bool XBMC_directory_exists(AddonCB* handle, CB_AddOnLib* cbTable, const char* strPath)
{
  if (handle == NULL || cbTable == NULL)
    return false;

  return cbTable->DirectoryExists(handle->addonData, strPath);
}

DLLEXPORT bool XBMC_remove_directory(AddonCB* handle, CB_AddOnLib* cbTable, const char* strPath)
{
  if (handle == NULL || cbTable == NULL)
    return false;

  return cbTable->RemoveDirectory(handle->addonData, strPath);
}

} // extern "C"
