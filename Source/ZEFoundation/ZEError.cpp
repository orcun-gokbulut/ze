//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEError.cpp
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

#include "ZEError.h"
#include "ZEPlatform.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if defined(ZE_PLATFORM_WINDOWS) && defined(ZE_DEBUG_ENABLE)
#define WINDOWS_MEAN_AND_LEAN
#include <windows.h>
#endif

ZELock ZE_BREAK_LOCK_INTERNAL;

static void DefaultErrorCallback(ZEErrorType Level)
{
	if (Level >= ZE_ET_CRITICAL_ERROR)
		exit(EXIT_FAILURE);
}

ZEError::ZEError()
{
	BreakOnAssertEnabled = true;
	BreakOnErrorEnabled = true;
	BreakOnWarningEnabled = true;
	ErrorCallback = ZEErrorCallback::Create<&DefaultErrorCallback>();
}

void ZEError::SetBreakOnDebugCheckEnabled(bool Enabled)
{
	BreakOnAssertEnabled = Enabled;
}

bool ZEError::GetBreakOnDebugCheckEnabled()
{
	return BreakOnAssertEnabled;
}

void ZEError::SetBreakOnErrorEnabled(bool Enabled)
{
	BreakOnErrorEnabled = Enabled;
}

bool ZEError::GetBreakOnErrorEnabled()
{
	return BreakOnErrorEnabled;
}

void ZEError::SetBreakOnWarningEnabled(bool Enabled)
{
	BreakOnWarningEnabled = Enabled;
}

bool ZEError::GetBreakOnWarningEnabled()
{
	return BreakOnWarningEnabled;
}

const char* ZEError::GetErrorTypeString(ZEErrorType Type)
{
	switch(Type)
	{
		case ZE_ET_CRITICAL_ERROR:
			return "Critical Error";

		case ZE_ET_ERROR:
			return "Error";

		case ZE_ET_WARNING:
			return "Warning";

		case ZE_ET_DEBUG_CHECK_FAILED:
			return "Debug Check Failed";

		case ZE_ET_DEBUG_CHECK_WARNING:
			return "Debug Check Warning";

		default:
			return "Unknown";
	}
}

void ZEError::SetCallback(ZEErrorCallback Callback)
{
	ErrorCallback = Callback;
}

ZEErrorCallback ZEError::GetCallback()
{
	return ErrorCallback;
}

void ZEError::RaiseError(ZEErrorType Type)
{
	static ZELock Lock;
	Lock.LockNested();
	
	if (ErrorCallback != NULL)
		ErrorCallback(Type);

	Lock.Unlock();
}

ZEError* ZEError::GetInstance()
{
	static ZEError Instance;
	return &Instance;
}
