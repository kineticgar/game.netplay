/*
 *      Copyright (C) 2013-2015 Garrett Brown
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

#include "Version.h"
#include "StringUtils.h"

using namespace NETPLAY;

Version::Version(std::string strVersion) :
  version_major(0),
  version_minor(0),
  version_point(0)
{
  size_t pos = strVersion.find(".");
  if (pos != std::string::npos)
  {
    version_major = StringUtils::IntVal(strVersion.substr(0, pos));
    strVersion = strVersion.substr(pos + 1);
    pos = strVersion.find(".");
    if (pos != std::string::npos)
    {
      version_minor = StringUtils::IntVal(strVersion.substr(0, pos));
      strVersion = strVersion.substr(pos + 1);
      version_point = StringUtils::IntVal(strVersion);

    }
  }
}

Version::Version(unsigned int version_major,
                 unsigned int version_minor,
                 unsigned int version_point) :
  version_major(version_major),
  version_minor(version_minor),
  version_point(version_point)
{
}

std::string Version::ToString(void) const
{
  return StringUtils::Format("%d.%d.%d", version_major, version_minor, version_point);
}

bool Version::operator<(const Version& rhs) const
{
  if (version_major < rhs.version_major) return true;
  if (version_major > rhs.version_major) return false;

  if (version_major < rhs.version_minor) return true;
  if (version_minor > rhs.version_minor) return false;

  if (version_point < rhs.version_point) return true;
  if (version_point > rhs.version_point) return false;

  return false;
}

bool Version::operator==(const Version& rhs) const
{
  return version_major == rhs.version_major &&
         version_minor == rhs.version_minor &&
         version_point == rhs.version_point;
}
