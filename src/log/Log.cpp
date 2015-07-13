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

#include "Log.h"
#include "LogAddon.h"
#include "LogConsole.h"

#include "platform/threads/threads.h"

#include <stdarg.h>
#include <stdio.h>

using namespace NETPLAY;
using namespace PLATFORM;

#define SYS_LOG_BUFFER_SIZE  256 // bytes

CLog::CLog(ILog* pipe) :
  m_pipe(pipe),
  m_level(SYS_LOG_DEBUG)
{
}

CLog& CLog::Get(void)
{
  static CLog _instance(new CLogConsole);
  return _instance;
}

CLog::~CLog(void)
{
  SetPipe(NULL);
}

bool CLog::SetType(SYS_LOG_TYPE type)
{
  PLATFORM::CLockObject lock(m_mutex);
  if (m_pipe && m_pipe->Type() == type)
    return true; // Already set

  switch (type)
  {
  case SYS_LOG_TYPE_CONSOLE:
    SetPipe(new CLogConsole);
    break;
  case SYS_LOG_TYPE_NULL:
    SetPipe(NULL);
    break;
  case SYS_LOG_TYPE_ADDON: // Must be set through SetPipe() because CLogAddon has no default constructor
  default:
    Log(SYS_LOG_ERROR, "Failed to set log type to %s", TypeToString(type));
    return false;
  }

  return true;
}

void CLog::SetPipe(ILog* pipe)
{
  PLATFORM::CLockObject lock(m_mutex);

  delete m_pipe;
  m_pipe = pipe;
}

void CLog::SetLevel(SYS_LOG_LEVEL level)
{
  PLATFORM::CLockObject lock(m_mutex);

  m_level = level;
}

void CLog::Log(SYS_LOG_LEVEL level, const char* format, ...)
{
  char fmt[SYS_LOG_BUFFER_SIZE];
  char buf[SYS_LOG_BUFFER_SIZE];
  va_list ap;

  va_start(ap, format);
  snprintf(fmt, sizeof(fmt), "%s", format); // TODO: Prepend CThread::ThreadId()
  vsnprintf(buf, SYS_LOG_BUFFER_SIZE - 1, fmt, ap);
  va_end(ap);

  PLATFORM::CLockObject lock(m_mutex);

  if (level > m_level)
    return;

  if (m_pipe)
    m_pipe->Log(level, buf);
}

const char* CLog::TypeToString(SYS_LOG_TYPE type)
{
  switch (type)
  {
  case SYS_LOG_TYPE_NULL:
    return "null";
  case SYS_LOG_TYPE_CONSOLE:
    return "syslog";
  case SYS_LOG_TYPE_ADDON:
    return "addon";
  default:
    return "unknown";
  }
}

const char* CLog::LevelToString(SYS_LOG_LEVEL level)
{
  switch (level)
  {
  case SYS_LOG_NONE:
    return "none";
  case SYS_LOG_ERROR:
    return "error";
  case SYS_LOG_INFO:
    return "info";
  case SYS_LOG_DEBUG:
    return "debug";
  default:
    return "unknown";
  }
}
