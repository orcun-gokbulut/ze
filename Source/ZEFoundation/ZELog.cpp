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

#include "ZEPlatform.h"
#include "ZEFile\ZEPathManager.h"
#include "ZEDS\ZEFormat.h"
#include "ZETimeStamp.h"
#include "ZECore\ZECore.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <memory.h>

#ifdef ZE_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

static void DefaultCallback(const char* Module, ZELogType Level, const char* LogText);

static void SetColor(int Type)
{
	#ifdef ZE_PLATFORM_WINDOWS
		switch(Type)
		{
			case ZE_LOG_CRITICAL_ERROR:
			case ZE_LOG_ERROR:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

				break;

			case ZE_LOG_WARNING:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				break;

			case ZE_LOG_SUCCESS:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				break;

			case -2:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;

			default:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				break;
		}
	#elif defined(ZE_PLATFORM_UNIX)
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
	#else

	#endif
}

static void DefaultCallback(const char* Module, ZELogType Type, const char* LogText, void* ExtraParameters)
{
	time_t Temp = time(NULL);                          
	tm TimeStamp;
	memcpy(&TimeStamp, localtime(&Temp), sizeof(tm));

	SetColor(-1);
	printf("%04d-%02d-%02d %02d:%02d:%02d [",
		1900 + TimeStamp.tm_year, TimeStamp.tm_mon + 1,TimeStamp.tm_mday, 
		TimeStamp.tm_hour, TimeStamp.tm_min, TimeStamp.tm_sec);

	SetColor(-2);
	printf("%s",  Module);
	
	SetColor(-1);
	printf("] ");

	SetColor(Type);
	printf("%s", ZELog::UtilityGetLogTypeString(Type));

	SetColor(-1);
	printf(": %s\n", LogText);
}

void ZELog::OpenLogFile()
{
	Lock.Lock();

	if (LogFile != NULL)
	{
		fclose((FILE*)LogFile);
		LogFile = NULL;
	}

	if (!LogFileEnabled || LogFilePath.IsEmpty())
	{
		Lock.Unlock();
		return;
	}


	char ComputerName[256];
	DWORD Size = sizeof(ComputerName);
	GetComputerNameA(ComputerName, &Size);

	ZETimeStamp TimeStamp = ZETimeStamp::Now();
	ZEString Date = ZEFormat::Format("{0:d:04}{1:d:02}{2:d:02}", TimeStamp.GetYear(), TimeStamp.GetMonth(), TimeStamp.GetDay());
	ZERealPath Path = ZEPathManager::GetInstance()->TranslateToRealPath(ZEFormat::Format(LogFilePath, Date, ComputerName));
	if ((Path.Access & ZE_PA_WRITE) == 0)
	{
		Lock.Unlock();
		return;
	}

	LogFile = fopen(Path.Path, "a");
	if (LogFile == NULL)
	{
		Lock.Unlock();
		return;
	}

	if (ftell((FILE*)LogFile) != 0)
		fprintf((FILE*)LogFile, "\n\n");

	fprintf((FILE*)LogFile, 
		"##########################################################\n"
		"## Zinek Engine Log \n"
		"## Computer Name : %s\n"
		"## Start Date/Time : %04d-%02d-%02d %2d:%2d:%2d\n"
		"##########################################################\n",
		ComputerName,
		TimeStamp.GetYear(), TimeStamp.GetMonth(), TimeStamp.GetDay(),
		TimeStamp.GetHour(), TimeStamp.GetMinute(), TimeStamp.GetSecond());

	Lock.Unlock();
}


void ZELog::LogInternal(const char* Module, ZELogType Type, const char* Format, va_list args)
{
	#if !defined(ZE_PLATFORM_WINDOWS) || defined(ZE_DEBUG_ENABLE)
	if ((Type < GetMinimumLevel() || Callback == NULL) &&
		(Type < GetLogFileMinimumLevel() || LogFile == NULL))
		return;
	#endif

	Lock.Lock();
	
	char Buffer[4096];
	vsprintf(Buffer, Format, args);

	#if defined(ZE_PLATFORM_WINDOWS) && defined(ZE_DEBUG_ENABLE)
		char DebugBuffer[4096];
		sprintf(DebugBuffer, "[%s] %s : %s \r\n", Module, ZELog::UtilityGetLogTypeString(ZE_LOG_INFO), Buffer);
		OutputDebugString(DebugBuffer);
	#endif

	if (Type >= GetMinimumLevel() && Callback != NULL)
		Callback(Module, Type, Buffer,CallbackParameter);

	if (Type >= GetLogFileMinimumLevel() && LogFile != NULL)
	{
		time_t Temp = time(NULL);                          
		tm TimeStamp;
		memcpy(&TimeStamp, localtime(&Temp), sizeof(tm));

		fprintf((FILE*)LogFile, "%02d-%02d-%04d %02d:%02d:%02d [%s] %s : %s\n",
			1900 + TimeStamp.tm_year, TimeStamp.tm_mon + 1, TimeStamp.tm_mday, 
			TimeStamp.tm_hour, TimeStamp.tm_min, TimeStamp.tm_sec,
			Module, UtilityGetLogTypeString(ZE_LOG_INFO), Buffer);
		fflush((FILE*)LogFile);
	}

	Lock.Unlock();
}


ZELog::ZELog()
{
	MinimumLogLevel = ZE_LOG_INFO;
	Callback = DefaultCallback;
	CallbackParameter = NULL;
	LogFile = NULL;
	LogFileEnabled = false;
	LogFileMinimumLevel = ZE_LOG_INFO;
}

ZELog::~ZELog()
{
	if (LogFile != NULL)
	{
		fprintf((FILE*)LogFile, "END OF LOG\n");
		fflush((FILE*)LogFile);
		fclose((FILE*)LogFile);
	}
}

void ZELog::SetMinimumLevel(ZELogType Level)
{
	if (Level > ZE_LOG_CRITICAL_ERROR)
		Level = ZE_LOG_CRITICAL_ERROR;

	MinimumLogLevel = Level;
}

ZELogType ZELog::GetMinimumLevel()
{
	return MinimumLogLevel;
}

void ZELog::SetCallback(ZELogCallback Callback)
{
	this->Callback = Callback;
}

ZELogCallback ZELog::GetCallback()
{
	return Callback;
}

void ZELog::SetLogFileEnabled(bool Enabled)
{
	if (LogFileEnabled == Enabled)
		return;

	LogFileEnabled = Enabled;
	OpenLogFile();
}

bool ZELog::GetLogFileEnabled()
{
	return LogFileEnabled;
}

void ZELog::SetLogFilePath(const ZEString& FileName)
{
	if (LogFilePath == FileName)
		return;

	LogFilePath = FileName;
	OpenLogFile();
}

const char* ZELog::GetLogFilePath()
{
	return LogFilePath;
}


void ZELog::SetLogFileMinimumLevel(ZELogType Level)
{
	LogFileMinimumLevel = Level;
}

ZELogType ZELog::GetLogFileMinimumLevel()
{
	return LogFileMinimumLevel;
}


void ZELog::Log(const char* Module, ZELogType Type, const char* Format, ...)
{
	char Text[4096];
	va_list VList;
	va_start(VList, Format);
	LogInternal(Module, Type, Format, VList);
	va_end(VList);
}

void ZELog::Log(const char* Module, const char* Format, ...)
{
	va_list VList;
	va_start(VList, Format);
	LogInternal(Module, ZE_LOG_INFO, Format, VList);
	va_end(VList);
}

ZELog* ZELog::GetInstance()
{
	static ZELog Instance;
	return &Instance;
}


void ZELog::UtilityGetModuleName(char* Output, const char* FileName, const char* Function)
{
	ZESize Len = strlen(Function);
	ZESize Index = 0;
	for(ZESize I = 0; I <= Len; I++)
	{
		if (Function[I] == ' ')
		{
			Index = 0;
			continue;
		}

		if (Function[I] == ':' || Function[I] == '<')
		{
			Output[Index] = '\0';
			return;
		}
		else
		{
			Output[Index] = Function[I];
			Index++;
		}
	}

	Len = strlen(FileName);
	bool Found = false;
	ZESize N = Len - 1;
	for(; N != 0; N--)
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

const char* ZELog::UtilityGetLogTypeString(ZELogType Type)
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
