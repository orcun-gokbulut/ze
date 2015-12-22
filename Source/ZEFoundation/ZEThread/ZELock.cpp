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
#include "ittnotify.h"
#endif

#ifdef ZE_PLATFORM_COMPILER_MSVC
	#include <intrin.h>
#endif

static inline ZEInt32 AtomicIncrement(volatile ZEInt32* NextNumber)
{
	#ifdef ZE_PLATFORM_COMPILER_MSVC
		return _InterlockedIncrement((long*)NextNumber);
    #endif

    #ifdef ZE_PLATFORM_COMPILER_GCC
		return __sync_add_and_fetch(NextNumber, 1);
	#endif
}

bool ZELock::IsLocked()
{
    return (CurrentNumber != NextNumber + 1);
}

void ZELock::Wait()
{
    while(IsLocked());
}

void ZELock::Lock()
{
	#ifdef ZE_VTUNE_ENABLED
	__itt_sync_prepare(this);
	#endif
    
	ZEInt32 MyNumber = AtomicIncrement(&NextNumber);
	while(MyNumber != CurrentNumber);

	#ifdef ZE_VTUNE_ENABLED
	__itt_sync_acquired(this);
	#endif
}

void ZELock::Unlock()
{
	#ifdef ZE_VTUNE_ENABLED
	__itt_sync_releasing(this);
	#endif

	CurrentNumber++;
}

ZELock ZELock::operator=(const ZELock& Lock)
{
	return ZELock();
}

ZELock::ZELock()
{
	CurrentNumber = 1;
	NextNumber = 0;
}

ZELock::ZELock(const ZELock& Lock)
{
	#ifdef ZE_VTUNE_ENABLED
	__itt_sync_create(this, "ZELock", "", 0);
	#endif

	CurrentNumber = 1;
	NextNumber = 0;
}

ZELock::~ZELock()
{
    zeDebugCheck(CurrentNumber != NextNumber + 1, "Destroying lock while it is still locked.");
	#ifdef ZE_VTUNE_ENABLED
	__itt_sync_destroy(this);
	#endif
}
