//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELog.cpp
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

#include "ZELog.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <memory.h>

#ifdef ZE_PLATFORM_WINDOWS
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#endif

static void DefaultCallback(const char* Module, ZELogType Level, const char* LogText);

static void SetColor(int Type)
{
	#ifdef PLATFORM_WIN32
		switch(Type)
		{
			case LOG_ERROR:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

				break;

			case LOG_WARNING:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				break;

			case LOG_INFO:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;

			case LOG_SUCCESS:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				break;

			default:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				break;
		}
	#else
		switch(Type)
		{
            case ZE_LOG_ERROR:
				printf("\033[01;31m");
				break;

            case ZE_LOG_WARNING:
				printf("\033[01;33m");
				break;

            case ZE_LOG_INFO:
				printf("\033[01;37m");
				break;

            case ZE_LOG_SUCCESS:
				printf("\033[01;32m");
				break;

			default:
				printf("\033[0;37m");
		}
	#endif
}

static void DefaultCallback(const char* Module, ZELogType Type, const char* LogText)
{
	time_t Temp = time(NULL);                          
	tm TimeStamp;
	memcpy(&TimeStamp, localtime(&Temp), sizeof(tm));

	SetColor(-1);
	printf("%02d-%02d-%04d %02d:%02d:%02d [",
		TimeStamp.tm_mday, TimeStamp.tm_mon + 1, 1900 + TimeStamp.tm_year,
		TimeStamp.tm_hour, TimeStamp.tm_min, TimeStamp.tm_sec);

	SetColor(Type);
	printf("%s", ZELog::GetLogTypeString(Type));

	SetColor(-1);
	printf("] %s : %s\n", Module, LogText);
}

void ZELog::GetModuleName(char* Output, const char* FileName, const char* Function)
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

const char* ZELog::GetLogTypeString(ZELogType Type)
{
	switch(Type)
	{
		case ZE_LOG_CRITICAL_ERROR:
			return "CRITICAL ERROR";
			break;

        case ZE_LOG_ERROR:
			return "ERROR";
			break;

        case ZE_LOG_WARNING:
			return "Warning";
			break;

        case ZE_LOG_INFO:
			return "Info";
			break;
		
        case ZE_LOG_SUCCESS:
			return "Success";
			break;

		case ZE_LOG_DEBUG:
			return "Debug";
			break;

		default:
			return "Unknown";
	}
}

void ZELog::SetMinimumLogLevel(ZELogType Level)
{
	if (Level > ZE_LOG_CRITICAL_ERROR)
		Level = ZE_LOG_CRITICAL_ERROR;

	MinimumLogLevel = Level;
}

ZELogType ZELog::GetMinimumLogLevel()
{
	return MinimumLogLevel;

}

void ZELog::SetLogFileEnabled(bool Enabled)
{
	LogFileEnabled = Enabled;

	tm FileTimeStamp;
	time_t Temp = time(NULL);                          
	memcpy(&FileTimeStamp, localtime(&Temp), sizeof(tm));

	if (LogFileEnabled && LogFile == NULL)
		LogFile = fopen(LogFileName, "a");

	if (LogFile != NULL)
	{
		fprintf((FILE*)LogFile, "\n\n##########################################################\n");
		fprintf((FILE*)LogFile, " Log Session Start Date/Time : %02d-%02d-%04d %2d:%2d:%2d\n", 
			FileTimeStamp.tm_mday, FileTimeStamp.tm_mon + 1, 1900 + FileTimeStamp.tm_year,
			FileTimeStamp.tm_hour, FileTimeStamp.tm_min, FileTimeStamp.tm_sec);
		fprintf((FILE*)LogFile, "##########################################################\n");
	}
}

bool ZELog::GetLogFileEnabled()
{
	return LogFileEnabled;
}

void ZELog::SetLogFileName(const ZEString& FileName)
{
	LogFileName = FileName;
	if (LogFile != NULL)
	{
		fclose((FILE*)LogFile);
		LogFile = fopen(FileName, "a");
	}
}

const char* ZELog::GetLogFileName()
{
	return LogFileName;
}

void ZELog::SetCallback(ZELogCallback Callback)
{
	LogCallback = Callback;
}

ZELogCallback ZELog::GetCallback()
{
	return LogCallback;
}

void ZELog::Log(const char* Module, ZELogType Type, const char* Format, ...)
{
	char Buffer[4096];

	va_list VList;
	va_start(VList, Format);
	vsprintf(Buffer, Format, VList);
	va_end(VList);

	#if defined(ZE_PLATFORM_WINDOWS) && defined(ZE_DEBUG_ENABLE)
		char DebugBuffer[4096];
		sprintf(DebugBuffer, "[%s] %s : %s \r\n", Module, ZELog::GetLogTypeString(ZE_LOG_INFO), Buffer);
		OutputDebugString(DebugBuffer);
	#endif

	if (LogFile != NULL)
	{
		time_t Temp = time(NULL);                          
		tm TimeStamp;
		memcpy(&TimeStamp, localtime(&Temp), sizeof(tm));

		fprintf((FILE*)LogFile, "%02d-%02d-%04d %02d:%02d:%02d [%s] %s : %s\n",
			TimeStamp.tm_mday, TimeStamp.tm_mon + 1, 1900 + TimeStamp.tm_year,
			TimeStamp.tm_hour, TimeStamp.tm_min, TimeStamp.tm_sec,
			GetLogTypeString(Type), Module, Buffer);
		fflush((FILE*)LogFile);
	}

	if (LogCallback != NULL)
		LogCallback(Module, Type, Buffer);
}

void ZELog::Log(const char* Module, const char* Format, ...)
{
	char Buffer[4096];
	va_list VList;
	va_start(VList, Format);
	vsprintf(Buffer, Format, VList);
	va_end(VList);

	#if defined(ZE_PLATFORM_WINDOWS) && defined(ZE_DEBUG_ENABLE)
		char DebugBuffer[4096];
		sprintf(DebugBuffer, "[%s] %s : %s \r\n", Module, ZELog::GetLogTypeString(ZE_LOG_INFO), Buffer);
		OutputDebugString(DebugBuffer);
	#endif

	if (LogFile != NULL)
	{
		time_t Temp = time(NULL);                          
		tm TimeStamp;
		memcpy(&TimeStamp, localtime(&Temp), sizeof(tm));

		fprintf((FILE*)LogFile, "%02d-%02d-%04d %02d:%02d:%02d [%s] %s : %s\n",
			TimeStamp.tm_mday, TimeStamp.tm_mon + 1, 1900 + TimeStamp.tm_year,
			TimeStamp.tm_hour, TimeStamp.tm_min, TimeStamp.tm_sec,
			GetLogTypeString(ZE_LOG_INFO), Module, Buffer);
		fflush((FILE*)LogFile);
	}

	if (LogCallback != NULL)
		LogCallback(Module, ZE_LOG_INFO, Buffer);

}

ZELog::ZELog()
{
	LogFile = NULL;
	LogFileEnabled = false;
	LogCallback = DefaultCallback;
	MinimumLogLevel = ZE_LOG_INFO;
}

ZELog* ZELog::GetInstance()
{
	static ZELog Instance;
	return &Instance;
}
