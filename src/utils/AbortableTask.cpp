/*
 *      Copyright (C) 2015 Garrett Brown
 *      Copyright (C) 2015 Team XBMC
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

#include "AbortableTask.h"
#include "SignalHandler.h"

#include <signal.h>

using namespace NETPLAY;

// Mask the signals with this offset to get the appropriate exit codes
// E.g. SIGINT (0x02) should return 0x82 from main()
#define EXIT_CODE_OFFSET  0x80

CAbortableTask::CAbortableTask(IAbortable* callback) :
  m_callback(callback),
  m_exitCode(0)
{
  CSignalHandler::Get().SetSignalReceiver(SIGHUP, this);
  CSignalHandler::Get().SetSignalReceiver(SIGINT, this);
  CSignalHandler::Get().SetSignalReceiver(SIGKILL, this);
  CSignalHandler::Get().SetSignalReceiver(SIGTERM, this);
  CSignalHandler::Get().IgnoreSignal(SIGPIPE);
}

CAbortableTask::~CAbortableTask(void)
{
  CSignalHandler::Get().ResetSignalReceivers();
}

void CAbortableTask::OnSignal(int signum)
{
  switch (signum)
  {
    case SIGHUP:
    case SIGKILL:
    case SIGINT:
    case SIGTERM:
    {
      m_exitCode = EXIT_CODE_OFFSET + signum;
      m_callback->Deinitialize();
      break;
    }

    default:
      break;
  }
}
