//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEErrorManager.cpp
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
#include "ZEErrorManager.h"
#include "ZECore.h"
#include "ZEConsole.h"
#include "ZEConsoleWindow.h"
#include "ZEOptionManager.h"

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include <crtdbg.h>

static void ZEErrorManagerErrorCallback(const char* Module, ZEErrorType Type, const char* Error, void* Args)
{
	ZEErrorManager::GetInstance()->RaiseErrorInternal(Module, Type, Error, Args);
}

static void ZEErrorManagerAssertCallback(ZEAssertType Type, const char* Function, const char* File, int Line, const char* Error, void* Args)
{
	ZEErrorManager::GetInstance()->RaiseAssertInternal(Type, Function, File, Line, Error, Args);
}

static ZEOptionSection ErrorOptions; 

bool ZEErrorManager::OptionCallback_General(ZEOption* Option, ZETypedVariant* Value)
{
	if (_stricmp(Option->GetName(), "FileLogging") == 0)
		FileLogging = Option->GetValue().GetBoolean();
	else if (_stricmp(Option->GetName(), "LogFile") == 0)
		SetLogFileName(Option->GetValue().GetString());

	return true;
}

void ZEErrorManager::EnableFileLogging()
{
	FileLogging = true;
}

void ZEErrorManager::DisableFileLogging()
{
	FileLogging = false;
}

void ZEErrorManager::SetLogFileName(const ZEString& NewLogFile)
{
	LogFileName = NewLogFile;
}

const ZEString& ZEErrorManager::GetLogFileName()
{
	return LogFileName;
}

char* ZEErrorManager::ErrorLevelToString(ZEErrorType ErrorLevel)
{
	switch(ErrorLevel)
	{
		case ZE_ET_CRITICAL_ERROR:
			return "Critical Error";
		default:
		case ZE_ET_ERROR:
			return "Error";
		case ZE_ET_WARNING:
			return "Warning";
		case ZE_ET_NOTICE:
			return "Notice";
	}
}

void ZEErrorManager::LogToFile(const char* Module, ZEErrorType ErrorType, const char* Error)
{
	if (FileLogging == true)
	{
		FILE* LogFile;
		if (fopen_s(&LogFile, LogFileName, "a") != NULL)
		{
			DisableFileLogging();
			RaiseError("Can not open log file. File logging is disabled.", ZE_ET_CRITICAL_ERROR, "Error");
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

void ZEErrorManager::RaiseError(const char* From, ZEErrorType Level, const char* ErrorFormat, ...)
{
	va_list VList;
	va_start(VList, ErrorFormat);
	RaiseErrorInternal(From, Level, ErrorFormat, &VList);
	va_end(VList);
}

void ZEErrorManager::RaiseErrorInternal(const char* From, ZEErrorType Level, const char* ErrorFormat, void* Args)
{
	char Buffer[4096];

	va_list VList = *(va_list*)Args;
	vsprintf_s(Buffer, 4095, ErrorFormat, VList);

	LogToFile(From, Level, Buffer);
	if (zeCore->GetConsole() != NULL)
		zeOutput("[%s] %s : %s\r\n", From, ErrorLevelToString(Level), Buffer);
	
	if (Level == ZE_ET_ERROR)
		ZEConsole::GetInstance()->ShowConsole();

	if (Level == ZE_ET_CRITICAL_ERROR)
	{
		zeCore->SetCoreState(ZE_CS_CRITICAL_ERROR);
		//ZEConsoleWindow ConsoleWindow;
		//ZECore::GetInstance()->GetConsole()->SetConsoleInterface(&ConsoleWindow);
		//ConsoleWindow.TermiantionState();
		//ZECore::GetInstance()->GetConsole()->SetConsoleInterface(NULL);
		exit(0);
	}
}

void ZEErrorManager::RaiseAssert(ZEAssertType AssertType, const char* Function, const char* File, int Line, const char* Message, ...)
{
	va_list VList;
	va_start(VList, Line);
	RaiseAssertInternal(AssertType, Function, File, Line, Message, &VList);
	va_end(VList);
}

void ZEErrorManager::RaiseAssertInternal(ZEAssertType AssertType, const char* Function, const char* File, int Line, const char* Message, void* Args)
{
	char Buffer[4096];

	va_list& VList = *(va_list*)Args;
	vsprintf_s(Buffer, 4095, Message, VList);

	if (zeCore->GetConsole() != NULL)
		zeOutput("%s : %s (Function : %s, File : %s, Line : %d)\r\n", (AssertType == ZE_AT_ASSERT ? "Assert" : "Warning"),  Buffer,  Function, File, Line);
}

ZEErrorManager* ZEErrorManager::GetInstance()
{
	return ZECore::GetInstance()->GetError();
}

ZEErrorManager::ZEErrorManager()
{
	FileLogging = false;
	ErrorOptions.SetName("Error");
	ErrorOptions.AddOption(new ZEOption("FileLogging", false, ZE_OA_NORMAL));
	ErrorOptions.AddOption(new ZEOption("LogFile", "error.log", ZE_OA_NORMAL));
	ZEOptionManager::GetInstance()->RegisterSection(&ErrorOptions);
}

ZEErrorManager::~ZEErrorManager()
{
	ZEOptionManager::GetInstance()->UnregisterSection(&ErrorOptions);
}





