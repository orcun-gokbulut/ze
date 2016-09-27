//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELockRW.cpp
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

#include "ZELockRW.h"

#include "ZEError.h"

ZE_COPY_NO_ACTION_IMP(ZELockRW)

void ZELockRW::LockRead()
{
	ReaderLock.Lock();

	if (ReaderCount == 0)
		AccessLock.LockInterthreaded();

	ReaderCount++;
	
	ReaderLock.Unlock();
}

void ZELockRW::UnlockRead()
{
	ReaderLock.Lock();
	
	zeDebugCheck(ReaderCount == 0, "Lock is not locked for reading.");
	zeDebugCheck(WriterCount > 0, "ZELockRW::UnloadRead failed. Lock is not locked for reading.");
	
	ReaderCount--;
	if (ReaderCount == 0)
		AccessLock.Unlock();

	ReaderLock.Unlock();
}

void ZELockRW::LockWrite()
{
	AccessLock.Lock();
	WriterCount++;

	zeDebugCheck(ReaderCount != 0, "Reader count still not zero after write lock");
}

void ZELockRW::LockWriteNested()
{
	AccessLock.LockNested();
	WriterCount++;

	zeDebugCheck(ReaderCount != 0, "Reader count still not zero after write lock");
}

void ZELockRW::UnlockWrite()
{
	zeDebugCheck(ReaderCount > 0, "Lock is not locked for writing. It is locked for reading.");
	zeDebugCheck(WriterCount == 0, "Lock is not locked for writing.");
	
	WriterCount--;
	AccessLock.Unlock();
}

ZELockRW::ZELockRW()
{
	ReaderCount = 0;
	WriterCount = 0;
}

ZELockRW::~ZELockRW()
{
	zeDebugCheck(ReaderCount < 0, "Destroying ZELockRW. Lock is still locked for writing.");
	zeDebugCheck(ReaderCount > 0, "Destroying ZELockRW. Lock is still locked for reading.");
}
