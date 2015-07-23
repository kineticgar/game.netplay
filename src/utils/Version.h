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
#pragma once

#include <string>

namespace NETPLAY
{
  struct Version
  {
  public:
    Version(std::string strVersion);

    Version(unsigned int version_major, unsigned int version_minor, unsigned int version_point);

    std::string ToString(void) const;

    bool operator<(const Version& rhs) const;

    bool operator==(const Version& rhs) const;

    bool operator<=(const Version& rhs) const { return  operator<(rhs) ||  operator==(rhs); }
    bool operator>(const Version& rhs) const  { return !operator<(rhs) && !operator==(rhs); }
    bool operator>=(const Version& rhs) const { return !operator<(rhs); }

    unsigned int version_major;
    unsigned int version_minor;
    unsigned int version_point;
  };
}
