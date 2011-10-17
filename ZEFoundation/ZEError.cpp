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

#include <stdio.h>
#include <stdarg.h>

static void DefaultErrorCallback(const char* Module, ZEErrorType Type, const char* Error, void* Args)
{
	const char* TypeString = "Unknown";
	switch(Type)
	{
		case ZE_ET_CRITICAL_ERROR:
			TypeString = "Critical Error";
			break;
		case ZE_ET_ERROR:
			TypeString = "Error";
			break;
		case ZE_ET_WARNING:
			TypeString = "Warning";
			break;
		case ZE_ET_NOTICE:
			TypeString = "Notice";
			break;
		case ZE_ET_LOG:
			TypeString = "Log";
			break;
	}

	printf("[%s] %s : ", Module, TypeString);
	vprintf(Error, *(va_list*)Args);
	printf("\r\n");
}

static void DefaultAssertCallback(ZEAssertType Type, const char* Function, const char* File, int Line, const char* Error, void* Args)
{
	printf("[%sAssert] : ", (Type == ZE_AT_WARNING_ASSERT ? "Warning " : ""));
	vprintf(Error, *(va_list*)Args);
	printf(" (Function : \"%s\", File : \"%s\", Line : %d)\r\n", Error, Function, File, Line);
}

static ZEErrorCallback ErrorCallback = &DefaultErrorCallback;
static ZEAssertCallback AssertCallback = &DefaultAssertCallback;

void zeSetErrorCallback(ZEErrorCallback Callback)
{
	ErrorCallback = Callback;
}

void zeSetAssertCallback(ZEAssertCallback Callback)
{
	AssertCallback = Callback;
}

void zeRaiseError(const char* Module, ZEErrorType Type, const char* Error, ...)
{
	va_list Args;
	va_start(Args, Error);

	if (ErrorCallback != NULL)
		ErrorCallback(Module, Type, Error, &Args);

	va_end(Args);
}

void zeRaiseAssert(ZEAssertType Type, const char* Function, const char* File, int Line, const char* Error, ...)
{
	va_list Args;
	va_start(Args, Error);

	if (AssertCallback != NULL)
		AssertCallback(Type, Function, File, Line, Error, &Args);

	va_end(Args);
}
