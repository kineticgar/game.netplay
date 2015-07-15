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

#include "StringUtils.h"

#include <errno.h>
#include <stdlib.h>

using namespace NETPLAY;

long StringUtils::IntVal(const std::string& str, long iDefault /* = 0 */)
{
  if (!str.empty())
  {
    const char *s = str.c_str();
    char *p = NULL;
    errno = 0;
    long n = strtol(s, &p, 10);
    if (!errno && s != p)
      return n;
  }
  return iDefault;
}

double StringUtils::DoubleVal(const std::string& str, double fDefault /* = 0 */)
{
  if (!str.empty())
  {
    const char *s = str.c_str();
    char *p = NULL;
    errno = 0;
    double d = strtod(s, &p);
    if (!errno && s != p)
      return d;
  }
  return fDefault;
}
