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
#include "ZELogSession.h"
#include "ZEDS/ZEList2.h"
#include "ZEThread/ZEThread.h"

#include <stdio.h>
#include <stdarg.h>
#include <memory.h>
#include <time.h>

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

static void DefaultCallback(const ZELogSession* Session, const char* Module, ZELogType Type, const char* LogText, void* ExtraParameters)
{
	if (Session != NULL)
		printf("(%d) ", Session->GetSessionID());

	SetColor(-1);
	printf("[");

	SetColor(-2);
	printf("%s",  Module);
	
	SetColor(-1);
	printf("] ");

	SetColor(Type);
	printf("%s", ZELog::GetLogTypeString(Type));

	SetColor(-1);
	printf(": %s\n", LogText);
}

void ZELog::LogInternal(const char* Module, ZELogType Type, const char* Format, va_list args)
{
	char Buffer[4096];
	vsprintf(Buffer, Format, args);

	time_t Temp = time(NULL);
	tm TimeStamp;
	memcpy(&TimeStamp, localtime(&Temp), sizeof(tm));

	ZEUInt32 ThreadId = ZEThread::GetCurrentThreadId();
	ZELogSession* CurrentSession = NULL;
	ZELogSession* CurrentSinkSession = NULL;
	ze_for_each_reverse(Session, *Sessions)
	{
		if (Type < Session->MinimumLevel)
			continue;

		if (Session->Block)
			break;

		if (!Session->Sink && Session->ThreadID != ThreadId)
			continue;

		if (CurrentSession == NULL || CurrentSession->GetThreadID() != ThreadId)
			CurrentSession = Session.GetPointer();

		if (Session->Callback != NULL)
			Session->Callback(CurrentSession, Module, Type, Buffer, Session->CallbackParameter);

		if (Session->LogFile == NULL)
			continue;

		fprintf((FILE*)Session->LogFile, 
			"(%d) %04d-%02d-%02d %02d:%02d:%02d [%s] %s: %s\n",
			CurrentSession->SessionID,
			1900 + TimeStamp.tm_year, TimeStamp.tm_mon + 1, TimeStamp.tm_mday, 
			TimeStamp.tm_hour, TimeStamp.tm_min, TimeStamp.tm_sec,
			Module, GetLogTypeString(Type), Buffer);
		
		fflush((FILE*)Session->LogFile);
	}

	#if defined(ZE_PLATFORM_WINDOWS) && defined(ZE_DEBUG_ENABLE)
		char DebugBuffer[4096];
		sprintf(DebugBuffer, "(S:%d, T:%d) [%s] %s: %s \r\n", CurrentSession->GetSessionID(), ZEThread::GetCurrentThreadId(), Module, ZELog::GetLogTypeString(Type), Buffer);
		OutputDebugString(DebugBuffer);
	#endif
}

ZELog::ZELog()
{
	LastSessionId = 0;
	Sessions = new ZEList2<ZELogSession>();
	RootSession = new ZELogSession();
	RootSession->SetSink(true);
	RootSession->SetCallback(DefaultCallback);
	BeginSession(RootSession);
}

ZELog::~ZELog()
{
	EndSession(RootSession);
	delete RootSession;
	delete Sessions;
}

ZELogSession* ZELog::GetRootSession()
{
	return RootSession;
}

ZELogSession* ZELog::GetCurrentSession()
{
	Lock.Lock();

	ze_for_each_reverse(Session, *Sessions)
	{
		if (Session->ThreadID == ZEThread::GetCurrentThreadId())
		{
			Lock.Unlock();
			return Session.GetPointer();
		}
	}
	Lock.Unlock();

	return NULL;
}

ZESize ZELog::GetSessionCount()
{
	return Sessions->GetCount();
}

void ZELog::BeginSession(ZELogSession* Session)
{
	Lock.Lock();

	if (Session->Link.GetInUse())
	{
		Lock.Unlock();
		return;
	}

	Sessions->AddEnd(&Session->Link);
	Session->ThreadID = ZEThread::GetCurrentThreadId();
	Session->SessionID = LastSessionId++;

	Lock.Unlock();
}

void ZELog::EndSession(ZELogSession* Session)
{
	Lock.Lock();

	Session->CloseLogFile();

	if (Session->SessionID == -1)
	{
		Lock.Unlock();
		return;
	}

	Session->CloseLogFile();
	Sessions->Remove(&Session->Link);
	Session->SessionID = -1;
	Session->ThreadID = 0;

	Lock.Unlock();
}

void ZELog::Log(const char* Module, ZELogType Type, const char* Format, ...)
{
	Lock.Lock();

	va_list VList;
	va_start(VList, Format);
	LogInternal(Module, Type, Format, VList);
	va_end(VList);

	Lock.Unlock();
}

void ZELog::Log(const char* Module, const char* Format, ...)
{
	Lock.Lock();

	va_list VList;
	va_start(VList, Format);
	LogInternal(Module, ZE_LOG_INFO, Format, VList);
	va_end(VList);

	Lock.Unlock();
}

ZELog* ZELog::GetInstance()
{
	static ZELog Instance;
	return &Instance;
}

void ZELog::GetModuleName(char* Output, const char* FileName, const char* Function)
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
