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

static ZEOptionSection ErrorOptions; 

bool ZEErrorManager::OptionCallback_General(ZEOption* Option, ZETypedVariant* Value)
{
	if (_stricmp(Option->GetName(), "FileLogging") == 0)
		LogFileEnabled = Option->GetValue().GetBoolean();
	else if (_stricmp(Option->GetName(), "LogFile") == 0)
		SetLogFileName(Option->GetValue().GetString());

	return true;
}

void ZEErrorManager::SetLogFileEnabled(bool Enabled)
{
	LogFileEnabled = Enabled;
}

bool ZEErrorManager::GetLogFileEnabled()
{
	return LogFileEnabled;
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

void ZEErrorManager::LogToFile(const char* Module, ZEErrorType ErrorType, const char* Error)
{
	if (LogFileEnabled == true)
	{
		FILE* LogFile;
		if (fopen_s(&LogFile, LogFileName, "a") != NULL)
		{
			SetLogFileEnabled(false);
			zeError("Can not open log file. File logging is disabled.", ZE_ET_CRITICAL_ERROR);
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

void ZEErrorManager::ErrorCallback(const char* From, ZEErrorType Level, const char* ErrorText)
{
	ZEErrorManager::GetInstance()->LogToFile(From, Level, ErrorText);
	if (zeCore->GetConsole() != NULL)
		zeOutput("[%s] %s : %s\r\n", From, ErrorLevelToString(Level), ErrorText);
	
	if (Level == ZE_ET_ERROR)
		ZEConsole::GetInstance()->ShowConsole();

	if (Level == ZE_ET_CRITICAL_ERROR)
	{
		zeCore->SetCoreState(ZE_CS_CRITICAL_ERROR);
		ZEConsoleWindow ConsoleWindow;
		ZECore::GetInstance()->GetConsole()->SetConsoleInterface(&ConsoleWindow);
		ConsoleWindow.TermiantionState();
		ZECore::GetInstance()->GetConsole()->SetConsoleInterface(NULL);
		exit(0);
	}
}

void ZEErrorManager::AssertCallback(ZEAssertType AssertType, const char* AssertText, const char* Function, const char* File, int Line)
{
	if (zeCore->GetConsole() != NULL)
		zeOutput("%s : %s (Function : %s, File : %s, Line : %d)\r\n", (AssertType == ZE_AT_ASSERT ? "Assert" : "Warning"),  AssertText,  Function, File, Line);
}

ZEErrorManager* ZEErrorManager::GetInstance()
{
	return ZECore::GetInstance()->GetError();
}

ZEErrorManager::ZEErrorManager()
{
	LogFileEnabled = false;
	ErrorOptions.SetName("Error");
	ErrorOptions.AddOption(new ZEOption("LogFileEnabled", false, ZE_OA_NORMAL));
	ErrorOptions.AddOption(new ZEOption("LogFileName", "error.log", ZE_OA_NORMAL));
	ZEOptionManager::GetInstance()->RegisterSection(&ErrorOptions);
	ZEError::SetAssertCallback(&ZEErrorManager::AssertCallback);
	ZEError::SetErrorCallback(&ZEErrorManager::ErrorCallback);
}

ZEErrorManager::~ZEErrorManager()
{
	ZEOptionManager::GetInstance()->UnregisterSection(&ErrorOptions);
}





