//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELock.h
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

#pragma once

#include "ZETypes.h"
#include "ZECommon.h"

#define ZE_LOCK_SECTION(Lock) for (ZELockSectionLoopVariable LoopVariable(Lock); LoopVariable.Control; LoopVariable.Control = false)
#define ZE_CRITICAL_SECTION_BEGIN(Name)	static ZELock Name##CriticalSection; Name##CriticalSection.Lock();
#define ZE_CRITICAL_SECTION_END(Name) Name##CriticalSection.Unlock();

class ZELock
{
	ZE_COPY_NO_ACTION(ZELock)
	friend class ZELockRW;
	private:
		volatile ZEInt32		CurrentNumber;
		volatile ZEInt32		NextNumber;
		volatile ZEUInt32		OwnerThreadId;
		volatile ZEInt32		NestingCount;

	public:
		bool					IsLocked() const;

		// Async Methods
		ZEUInt32				Queue();
		bool					Check(ZEUInt32 Number) const;
		void					Wait(ZEUInt32 Number) const;
		void					Release(ZEUInt32 Number);

		// Sync Methods
		void					Lock();
		void					LockNested();
		void					LockInterthreaded();
		void					Wait() const;
		void					Unlock();

								ZELock();
								~ZELock();
};

class ZELockHolder
{
	ZE_COPY_NO_ACTION(ZELockHolder)
	private:
		ZELock*					TargetLock;

	public:
		void					Lock(ZELock& Lock);
		void					Unlock();

								ZELockHolder();
								ZELockHolder(ZELock& Lock);
								~ZELockHolder();
};

class ZELockSectionLoopVariable 
{
	public:
		ZELockHolder			LockHolder;
		bool					Control;

								ZELockSectionLoopVariable(ZELock& Lock);
};
