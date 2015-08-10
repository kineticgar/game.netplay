/*
 *      Copyright (C) 2014-2015 Garrett Brown
 *      Copyright (C) 2014-2015 Team XBMC
 *      Portions Copyright (C) 2013-2014 Lars Op den Kamp
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
#pragma once

#include "ILog.h"

#include "platform/threads/mutex.h"

#include <string>

// --- Shorthand logging -------------------------------------------------------

#ifndef esyslog
  #define esyslog(...) NETPLAY::CLog::Get().Log(SYS_LOG_ERROR, __VA_ARGS__)
#endif

#ifndef isyslog
  #define isyslog(...) NETPLAY::CLog::Get().Log(SYS_LOG_INFO, __VA_ARGS__)
#endif

#ifndef dsyslog
  #define dsyslog(...) NETPLAY::CLog::Get().Log(SYS_LOG_DEBUG, __VA_ARGS__)
#endif

#define LOG_ERROR_STR(str)  esyslog("ERROR (%s, %d): %s: %m", __FILE__, __LINE__, str)

// --- CLog --------------------------------------------------------------------

namespace NETPLAY
{
  class CLog
  {
  private:
    CLog(ILog* pipe);

  public:
    static CLog& Get(void);
    ~CLog(void);

    bool SetType(SYS_LOG_TYPE type);
    void SetPipe(ILog* pipe);
    void SetLevel(SYS_LOG_LEVEL level);
    void SetLogPrefix(const std::string& strLogPrefix);

    void Log(SYS_LOG_LEVEL level, const char* format, ...);

    static const char* TypeToString(SYS_LOG_TYPE type);

  private:
    static const char* GetLogPrefix(SYS_LOG_LEVEL level);

    ILog*            m_pipe;
    SYS_LOG_LEVEL    m_level;
    std::string      m_strLogPrefix;
    PLATFORM::CMutex m_mutex;
  };
}
