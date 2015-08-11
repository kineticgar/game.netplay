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

#include "ReadWriteLock.h"

using namespace NETPLAY;
using namespace PLATFORM;

CReadWriteLock::CReadWriteLock(void) :
  m_readCount(0)
{
}

void CReadWriteLock::LockRead(void)
{
  CLockObject lock(m_readLock);

  m_readCount++;

  if (m_readCount == 1)
    LockWrite();
}

void CReadWriteLock::UnlockRead(void)
{
  CLockObject lock(m_readLock);

  m_readCount--;

  if (m_readCount == 0)
    UnlockWrite();
}

void CReadWriteLock::LockWrite(void)
{
  m_writeLock.Lock();
}

void CReadWriteLock::UnlockWrite(void)
{
  m_writeLock.Unlock();
}

CReadLockObject::CReadLockObject(CReadWriteLock& mutex) :
    m_mutex(mutex)
{
  m_mutex.LockRead();
}

CReadLockObject::~CReadLockObject(void)
{
  m_mutex.UnlockRead();
}

CWriteLockObject::CWriteLockObject(CReadWriteLock& mutex) :
    m_mutex(mutex)
{
  m_mutex.LockWrite();
}

CWriteLockObject::~CWriteLockObject(void)
{
  m_mutex.UnlockWrite();
}
