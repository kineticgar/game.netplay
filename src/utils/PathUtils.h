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
#pragma once

#include <string>

namespace NETPLAY
{
  class PathUtils
  {
  public:
    /*!
     * \brief Get the path of the current process
     */
    static std::string GetProcessPath(void);

    /*!
     * \brief Get the name of the current process's executable
     */
    static std::string GetFileName(const std::string& strPath);

    /*!
     * \brief Get the parent directory for a given file/folder
     */
    static std::string GetParentDirectory(const std::string& strPath);

    /*!
     * \brief Remove trailing slashes from the given path
     */
    static std::string RemoveSlashAtEnd(const std::string& strPath);

    /*!
     * \brief Get the directory for helper libraries
     */
    static std::string GetHelperLibraryDir(const std::string& strBasePath);
  };
}
