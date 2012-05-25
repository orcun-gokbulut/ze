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

#if defined(ZE_PLATFORM_WINDOWS) && defined(ZE_DEBUG_ENABLE)
#define WINDOWS_MEAN_AND_LEAN
#include <windows.h>
#endif

static ZEErrorType ErrorOutputLevel = ZE_ET_LOG;

static bool BreakOnAssertEnabled = true;
static bool BreakOnErrorEnabled = true;
static bool BreakOnWarningEnabled = true;

static void DefaultErrorCallback(const char* Module, ZEErrorType Type, const char* ErrorText)
{
	printf("[%s] %s : %s \r\n", Module, ZEError::GetErrorTypeString(Type), ErrorText);
}

static void DefaultAssertCallback(ZEAssertType Type, const char* AssertText, const char* Function, const char* File, ZEInt Line)
{
	printf("[%s] : %s (Function : \"%s\", File : \"%s\", Line : %d)\r\n", ZEError::GetAssertTypeString(Type), AssertText, Function, File, Line);
}

static ZEErrorCallback ErrorCallback = &DefaultErrorCallback;
static ZEAssertCallback AssertCallback = &DefaultAssertCallback;

void ZEError::SetErrorOutputLevel(ZEErrorType Type)
{
	if (Type > ZE_ET_CRITICAL_ERROR)
		Type = ZE_ET_CRITICAL_ERROR;

	ErrorOutputLevel = Type;
}

ZEErrorType ZEError::GetErrorOutputLevel()
{
	return ErrorOutputLevel;
}

void ZEError::SetBreakOnAssertEnabled(bool Enabled)
{
	BreakOnAssertEnabled = Enabled;
}

bool ZEError::GetBreakOnAssertEnabled()
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

void ZEError::GetModuleName(char* Output, const char* Function, const char* FileName)
{
	ZESize Len = strlen(Function);
	for(ZESize I = 0; I <= Len; I++)
		if (Function[I] == ':' || Function[I] == '<')
		{
			Output[I] = '\0';
			return;
		}
		else
			Output[I] = Function[I];

	Len = strlen(FileName);
	bool Found = false;
	ZESize N;
	for(N = Len - 1; N >= 0; N--)
		if (FileName[N] == '\\' || FileName[N] == '/')
		{
			Found = true;
			N++;
			break;
		}

	if (Found)
		strcpy(Output, FileName + N);
	else
		strcpy(Output, FileName);
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
		
		if (Type >= ErrorOutputLevel)
		{
			#if defined(ZE_PLATFORM_WINDOWS) && defined(ZE_DEBUG_ENABLE)
				char DebugBuffer[4096];
				sprintf(DebugBuffer, "[%s] %s : %s \r\n", Module, ZEError::GetErrorTypeString(Type), Buffer);
				OutputDebugString(DebugBuffer);
			#endif

			ErrorCallback(Module, Type, Buffer);
		}
	}
}

void ZEError::RaiseAssert(ZEAssertType Type, const char* Function, const char* File, ZEInt Line, const char* AssertText, ...)
{
	if (AssertCallback != NULL)
	{
		va_list Args;
		va_start(Args, AssertText);
		
		char Buffer[4096];
		vsprintf(Buffer, AssertText, Args);
		
		va_end(Args);

		#if defined(ZE_PLATFORM_WINDOWS) && defined(ZE_DEBUG_ENABLE)
			char DebugBuffer[4096];
			sprintf(DebugBuffer, "[%s] : %s (Function : \"%s\", File : \"%s\", Line : %d)\r\n", ZEError::GetAssertTypeString(Type), Buffer, Function, File, Line);
			OutputDebugString(DebugBuffer);
		#endif
		
		AssertCallback(Type, Buffer, Function, File, Line);
	}
}
