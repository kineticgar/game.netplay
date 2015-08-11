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
 *  along with this Program; see the file COPYING. If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include "platform/threads/mutex.h"

namespace NETPLAY
{
  class CReadWriteLock
  {
  public:
    CReadWriteLock(void);

    void LockRead(void);
    void UnlockRead(void);

    void LockWrite(void);
    void UnlockWrite(void);

  private:
    PLATFORM::CMutex m_readLock;
    PLATFORM::CMutex m_writeLock;
    int              m_readCount;
  };

  class CReadLockObject
  {
  public:
    CReadLockObject(CReadWriteLock& mutex);
    ~CReadLockObject(void);

  private:
    CReadWriteLock& m_mutex;
  };

  class CWriteLockObject
  {
  public:
    CWriteLockObject(CReadWriteLock& mutex);
    ~CWriteLockObject(void);

  private:
    CReadWriteLock& m_mutex;
  };
}
