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
#include <string.h>

static void DefaultErrorCallback(const char* Module, ZEErrorType Type, const char* ErrorText)
{
	printf("[%s] %s : %s \r\n", Module, ZEError::GetErrorTypeString(Type), ErrorText);
}

static void DefaultAssertCallback(ZEAssertType Type, const char* AssertText, const char* Function, const char* File, int Line)
{
	printf("[%s] : %s (Function : \"%s\", File : \"%s\", Line : %d)\r\n", ZEError::GetAssertTypeString(Type), AssertText, Function, File, Line);
}

static ZEErrorCallback ErrorCallback = &DefaultErrorCallback;
static ZEAssertCallback AssertCallback = &DefaultAssertCallback;

void ZEError::GetModuleName(const char* Function, char* Output)
{
	size_t Len = strlen(Output);
	for(size_t I = 0; I < Len; I++)
		if (Function[I] == ':' || Function[I] == '<')
		{
			Output[I] ='\0';
			return;
		}
		else
			Output[I] = Function[I];

	Output[Len] = '\0';
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

		case ZE_ET_NOTICE:
			return "Notice";

		case ZE_ET_LOG:
			return "Log";

		default:
			return "Unknown";
	}
}

const char* ZEError::GetAssertTypeString(ZEAssertType Type)
{
	switch(Type)
	{
		case ZE_AT_ASSERT:
			return "Assert";

		case ZE_AT_WARNING_ASSERT:
			return "Warning Assert";

		default:
			return "Unknown";
	}
}

void ZEError::SetErrorCallback(ZEErrorCallback Callback)
{
	ErrorCallback = Callback;
}

void ZEError::SetAssertCallback(ZEAssertCallback Callback)
{
	AssertCallback = Callback;
}

void ZEError::RaiseError(const char* Module, ZEErrorType Type, const char* ErrorText, ...)
{
	if (ErrorCallback != NULL)
	{
		va_list Args;
		va_start(Args, ErrorText);

		char Buffer[4096];
		vsprintf(Buffer, ErrorText, Args);
		
		va_end(Args);

		ErrorCallback(Module, Type, Buffer);
	}
}

void ZEError::RaiseAssert(ZEAssertType Type, const char* Function, const char* File, int Line, const char* AssertText, ...)
{
	if (AssertCallback != NULL)
	{
		va_list Args;
		va_start(Args, AssertText);
		
		char Buffer[4096];
		vsprintf(Buffer, AssertText, Args);
		
		va_end(Args);
		
		AssertCallback(Type, Buffer, Function, File, Line);
	}
}
