//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELock.cpp
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 Yiğit Orçun GÖKBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "Yiğit  Orçun  GÖKBULUT") and
 provided with the Software  or, alternatively,  in  accordance with the  terms
 contained  in  a  written  agreement  between  you  and  copyright  owner. For
 licensing  terms  and conditions  please  contact  with  copyright owner.

                    GNU GENERAL PUBLIC LICENSE VERSION 3
 This program is free software: you can  redistribute it and/or modify it under
 the terms of the GNU General Public  License as published by the Free Software
 Foundation, either  version 3 of  the License, or  (at your option)  any later
 version. This program is  distributed in the hope that  it will be useful, but
 WITHOUT ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See  the GNU General Public License for more
 details. You  should have received  a copy of the  GNU General  Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>.

 Copyright Owner Information:
  Name: Yiğit Orçun GÖKBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#include "ZELock.h"
#include "ZEError.h"
#include "ZEPlatform.h"

#ifdef ZE_VTUNE_ENABLED
#include <ittnotify.h>
#endif

#ifdef ZE_PLATFORM_COMPILER_MSVC
	#include <intrin.h>
#endif
#include "ZEThread.h"


// ZELock
//////////////////////////////////////////////////////////////////////////////////////

ZE_COPY_NO_ACTION_IMP(ZELock)

static inline ZEUInt32 AtomicIncrement(volatile ZEUInt32* NextNumber)
{
	#ifdef ZE_PLATFORM_COMPILER_MSVC
		return _InterlockedIncrement((volatile long*)NextNumber);
    #endif

    #ifdef ZE_PLATFORM_COMPILER_GCC
		return __sync_add_and_fetch(NextNumber, 1);
	#endif
}

bool ZELock::IsLocked() const
{
    return (CurrentNumber != QueueNumber + 1);
}

ZEUInt32 ZELock::Queue()
{
	zeDebugCheck(OwnerThreadId == ZEThread::GetCurrentThreadId(), "Recursive lock detected.");

	return AtomicIncrement(&QueueNumber);
}

bool ZELock::Check(ZEUInt32 Number) const
{
	return (Number == CurrentNumber);
}

void ZELock::Wait(ZEUInt32 Number) const
{
	while(!Check(Number));
}

void ZELock::Release(ZEUInt32 Number)
{
	zeDebugCheck(CurrentNumber == QueueNumber + 1, "Lock cannot be release because it is not locked.");
	zeDebugCheck(CurrentNumber != Number, "Lock cannot be released because it hasn't acquired yet by given number.");
	zeDebugCheck(OwnerThreadId != 0 && OwnerThreadId != ZEThread::GetCurrentThreadId(), "Thread tries to release another thread's lock.");
	
	NestingCount--;

	if (NestingCount != 0)
		return;

	OwnerThreadId = 0;
	
	#ifdef ZE_VTUNE_ENABLED
	__itt_sync_releasing(this);
	#endif

	CurrentNumber++;
}

void ZELock::Wait() const
{
	while(IsLocked());
}

void ZELock::Lock()
{
	zeDebugCheck(OwnerThreadId == ZEThread::GetCurrentThreadId(), "Recursive lock detected.");

	#ifdef ZE_VTUNE_ENABLED
	__itt_sync_prepare(this);
	#endif

	ZEUInt32 MyNumber = Queue();
	Wait(MyNumber);

	NestingCount++;
	OwnerThreadId = ZEThread::GetCurrentThreadId();

	#ifdef ZE_VTUNE_ENABLED
	__itt_sync_acquired(this);
	#endif
}

void ZELock::LockNested()
{
	if (OwnerThreadId == ZEThread::GetCurrentThreadId())
	{
		NestingCount++;
		return;
	}

	#ifdef ZE_VTUNE_ENABLED
	__itt_sync_prepare(this);
	#endif

	ZEInt32 MyNumber = Queue();
	Wait(MyNumber);

	OwnerThreadId = ZEThread::GetCurrentThreadId();
	NestingCount++;

	#ifdef ZE_VTUNE_ENABLED
	__itt_sync_acquired(this);
	#endif
}

void ZELock::LockInterthreaded()
{
	zeDebugCheck(OwnerThreadId == ZEThread::GetCurrentThreadId(), "Recursive lock detected.");

	#ifdef ZE_VTUNE_ENABLED
	__itt_sync_prepare(this);
	#endif

	ZEInt32 MyNumber = Queue();
	Wait(MyNumber);

	OwnerThreadId = 0;
	NestingCount++;

	#ifdef ZE_VTUNE_ENABLED
	__itt_sync_acquired(this);
	#endif
}

void ZELock::Unlock()
{
	Release(CurrentNumber);
}

ZELock::ZELock()
{
	#ifdef ZE_VTUNE_ENABLED
	__itt_sync_create(this, "ZELock", "", 0);
	#endif

	zeDebugCheck(!ZE_CHECK_ALIGNMENT(&QueueNumber, 4), "ZELock memory alignmnet is not correct.");
	zeDebugCheck(!ZE_CHECK_ALIGNMENT(&CurrentNumber, 4), "ZELock memory alignmnet is not correct.");

	CurrentNumber = 1;
	QueueNumber = 0;
	NestingCount = 0;
	OwnerThreadId = 0;
}

ZELock::~ZELock()
{
    zeDebugCheck(CurrentNumber != QueueNumber + 1, "Destroying lock while it is still locked.");
	#ifdef ZE_VTUNE_ENABLED
	__itt_sync_destroy(this);
	#endif
}


// ZELockHolder
//////////////////////////////////////////////////////////////////////////////////////

ZE_COPY_NO_ACTION_IMP(ZELockHolder)

void ZELockHolder::Lock(ZELock& Lock)
{
	TargetLock = &Lock;
	TargetLock->Lock();
}

void ZELockHolder::Unlock()
{
	if (TargetLock == NULL)
		return;

	TargetLock->Unlock();
	TargetLock = NULL;
}

ZELockHolder::ZELockHolder()
{
	TargetLock = NULL;
}

ZELockHolder::ZELockHolder(ZELock& TargetLock)
{
	Lock(TargetLock);
}

ZELockHolder::~ZELockHolder()
{
	Unlock();
}

ZELockSectionLoopVariable::ZELockSectionLoopVariable(ZELock& Lock) : LockHolder(Lock), Control(true)
{

}
