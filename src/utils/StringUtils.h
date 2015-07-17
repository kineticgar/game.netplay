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
  class StringUtils
  {
  public:
    static std::string Format(const char* format, ...);

    /*!
     * \brief Convert a string to an integer
     * \param str The string
     * \param iDefault The value returned if parsing fails
     * \return The integer value, or iDefault if parsing fails
     */
    static long IntVal(const std::string& str, long iDefault = 0);

    /*!
     * \brief Convert a string to a double
     * \param str The string
     * \param fDefault The value returned if parsing fails
     * \return The double value, or fDefault if parsing fails
     */
    static double DoubleVal(const std::string& str, double fDefault = 0.0);
  };
}
