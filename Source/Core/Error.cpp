//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Error.cpp
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

#include "error.h"
#include "core.h"
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include <crtdbg.h>

void _ZEAssert(char* Message, char* File, int Line, ...)
{
	char Buffer[4096];

	va_list VList;
	va_start(VList, Line);
	vsprintf_s(Buffer, 1024, Message, VList);

	if (zeCore->GetConsole() != NULL)
		zeLog("[Assert] : %s\r\n", Buffer);

#if defined(ZEDEBUG_ENABLED) && defined(ZEPLATFORM_WINDOWS)
	_CrtDbgReport(_CRT_ASSERT, File, Line, "Zinek Engine", Message, VList);
#else
	abort();
#endif
	va_end(VList);
}

void _ZEWarning(char* Message, char* File, int Line, ...)
{

}

ZEOptionSection ZEError::ErrorOptions; 

bool ZEError::OptionCallback_General(ZEOption* Option, ZETypedVariant* Value)
{
	if (_stricmp(Option->GetName(), "FileLogging") == 0)
		FileLogging = Option->GetValue().GetBoolean();
	else if (_stricmp(Option->GetName(), "LogFile") == 0)
		SetLogFileName(Option->GetValue().GetString());

	return true;
}

void ZEError::EnableFileLogging()
{
	FileLogging = true;
}

void ZEError::DisableFileLogging()
{
	FileLogging = false;
}

void ZEError::SetLogFileName(const char* NewLogFile)
{
	 strcpy_s(LogFileName, ZEERROR_MAX_FILENAME_SIZE, NewLogFile);
}

char* ZEError::GetLogFileName()
{
	return LogFileName;
}

char* ZEError::ErrorLevelToString(ZEErrorType ErrorLevel)
{
	switch(ErrorLevel)
	{
		case ZE_ERRORLEVEL_CRITICAL:
			return "CRITICAL ERROR";
		default:
		case ZE_ERRORLEVEL_NONCRITICAL:
			return "Error";
		case ZE_ERRORLEVEL_WARNING:
			return "Warning";
		case ZE_ERRORLEVEL_NOTICE:
			return "Notice";
	}
}

void ZEError::LogToFile(const char* Module, ZEErrorType ErrorType, const char* Error)
{
	if (FileLogging == true)
	{
		FILE* LogFile;
		if (fopen_s(&LogFile, LogFileName, "a") != NULL)
		{
			DisableFileLogging();
			RaiseError("Can not open log file. File logging is disabled.", ZE_ERRORLEVEL_NONCRITICAL, "Error");
			return;
		}
		else
		{
			char DateBuf[26], TimeBuf[26];
			_strdate_s(DateBuf, 26);
			_strtime_s(TimeBuf, 26);
			fprintf(LogFile, "(%s - %s) [%s] %s : %s\n", DateBuf, TimeBuf, Module, ErrorLevelToString(ErrorType), Error);
			fclose(LogFile);
		}
	}
}

void ZEError::RaiseError(const char* From, ZEErrorType Level, const char* ErrorFormat, ...)
{
	va_list VList;
	va_start(VList, ErrorFormat);
	RaiseError(From, Level, ErrorFormat, VList);
	va_end(VList);
}

void ZEError::RaiseError(const char* From, ZEErrorType Level, const char* ErrorFormat, va_list Parameters)
{
	char Buffer[4096];

	va_list VList;
	va_start(VList, ErrorFormat);
	vsprintf_s(Buffer, 4095, ErrorFormat, Parameters);
	va_end(VList);

	LogToFile(From, Level, Buffer);
	if (zeCore->GetConsole() != NULL)
		zeLog("[%s] %s : %s\r\n", From, ErrorLevelToString(Level), Buffer);
	
	if (Level == ZE_ERRORLEVEL_NONCRITICAL)
		zeConsole->ShowConsole();

	if (Level == ZE_ERRORLEVEL_CRITICAL)
		zeCore->SetCoreState(ZECORESTATE_CRITICALERROR);
}


void ZEError::RaiseAssert(ZEAssertType AssertType, const char* Function, const char* File, int Line, const char* Message, ...)
{
	va_list VList;
	va_start(VList, Message);
	RaiseAssert(AssertType, Function, File, Line, Message, VList);
	va_end(VList);
}

void ZEError::RaiseAssert(ZEAssertType AssertType, const char* Function, const char* File, int Line, const char* Message, va_list Parameters)
{
	char Buffer[4096];
	vsprintf_s(Buffer, 4095, Message, Parameters);

	if (zeCore->GetConsole() != NULL)
		zeLog("%s : %s (Function : %s, File : %s, Line : %d)\r\n", (AssertType == ZE_ASSERTTYPE_ASSERT ? "Assert" : "Warning"),  Buffer,  Function, File, Line);

	#if defined(ZEDEBUG_ENABLED) && defined(ZEPLATFORM_WINDOWS)
		if (AssertType == ZE_ASSERTTYPE_ASSERT)
		{
			_CrtDbgReport(_CRT_ASSERT, File, Line, "Zinek Engine", Message, Parameters); 
		}
	#else
		if (AssertType == ZE_ASSERTTYPE_ASSERT)
			abort();
	#endif
}

ZEError* ZEError::GetInstance()
{
	return ZECore::GetInstance()->GetError();
}

ZEError::ZEError()
{
	ErrorOptions.SetName("Error");
	ErrorOptions.AddOption(new ZEOption("FileLogging", false, ZEOPTIONATTRIBUTE_NORMAL));
	ErrorOptions.AddOption(new ZEOption("LogFile", "error.log", ZEOPTIONATTRIBUTE_NORMAL));
	zeOptions->RegisterSection(&ErrorOptions);
}

ZEError::~ZEError()
{
	zeOptions->UnregisterSection(&ErrorOptions);
}
