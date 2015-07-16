/*
 *      Copyright (C) 2015 Garrett Brown
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
 *  along with this Program; see the file COPYING. If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "PathUtils.h"
#include "StringUtils.h"

#include <limits.h>

#if defined(_WIN32)
  #include <winbase.h>
#else
  #include <unistd.h>
#endif

using namespace NETPLAY;

#define HELPER_LIBRARY_DIR  "resources"

#if defined(_WIN32)
  #define PATH_SEPARATOR    "\\"
#else
  #define PATH_SEPARATOR    "/"
#endif

std::string PathUtils::GetProcessPath(void)
{
  char buff[PATH_MAX] = { };

#if defined(_WIN32)
  GetModuleFileName(NULL, buff, MAX_PATH);
#else
  readlink("/proc/self/exe", buff, sizeof(buff) - 1);
#endif

  return buff;
}

std::string PathUtils::GetFileName(const std::string& strPath)
{
  size_t pos = strPath.find_last_of("/\\");
  if (pos != std::string::npos)
    return strPath.substr(pos + 1);

  return strPath;
}

std::string PathUtils::GetParentDirectory(const std::string& strPath)
{
  size_t pos = strPath.find_last_of("/\\");
  if (pos != std::string::npos)
    return strPath.substr(0, pos);

  return strPath;
}

std::string PathUtils::RemoveSlashAtEnd(const std::string& strPath)
{
  std::string strDeslashed;

  if (!strPath.empty())
  {
    char last = strPath[strPath.length() - 1];
    if (last == '/' || last == '\\')
      strDeslashed = strPath.substr(0, strPath.length() - 1);
  }

  return strDeslashed;
}

std::string PathUtils::GetHelperLibraryDir(const std::string& strBasePath)
{
  return strBasePath + PATH_SEPARATOR + HELPER_LIBRARY_DIR;
}
