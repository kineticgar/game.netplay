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

#include "RequestHandler.h"
#include "log/Log.h"

#include <assert.h>

using namespace NETPLAY;
using namespace PLATFORM;

CRequestHandler::CRequestHandler(CClient* client) :
  m_client(client),
  m_request(NULL),
  m_nextHandler(NULL)
{
  assert(m_client);

  CreateThread();
}

CRequestHandler::~CRequestHandler(void)
{
  StopThread(-1);

  {
    CLockObject lock(m_requestMutex);
    delete m_request;
    m_request = NULL;
  }

  m_requestEvent.Signal();

  StopThread(0);

  delete m_nextHandler;
}

void* CRequestHandler::Process(void)
{
  Request* request;

  while (!IsStopped())
  {
    m_requestEvent.Wait();

    {
      CLockObject lock(m_requestMutex);
      request = m_request;
    }

    if (request == NULL)
      continue;

    HandleRequest(request->method, request->strRequest);

    {
      CLockObject lock(m_requestMutex);
      delete m_request;
      m_request = NULL;
    }
  }

  return NULL;
}

void CRequestHandler::ReceiveRequest(RPC_METHOD method, const std::string& strRequest)
{
  CLockObject lock(m_requestMutex);

  if (IsStopped())
    return;

  if (m_request == NULL)
  {
    m_request = new Request(method, strRequest);
    m_requestEvent.Signal();
  }
  else
  {
    if (!m_nextHandler)
    {
      dsyslog("Number of simultaneous requests exceeds number of threads, adding a thread");
      m_nextHandler = Clone();
    }
    m_nextHandler->ReceiveRequest(method, strRequest);
  }
}
