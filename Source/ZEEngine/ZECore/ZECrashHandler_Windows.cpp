//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECrashHandler_Windows.cpp
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

#include "ZECrashHandler.h"
#include "ZECore.h"
#include "ZEDS/ZEFormat.h"
#include "ZEGUID.h"
#include "ZEBase64.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "ZEVersion.h"
#include "ZELog.h"

ZECrashHandler::ZECrashHandler()
{

	Enable = true;
	Initialized = false;
	CrashDumpType = ZE_CDT_NORMAL;

	#if(_DEBUG)
	{
		CrashReport = true;
		CreateCrashDump = true;
	}
	#else
	{
		CreateCrashDump = true;
		CrashReport = true;
	}
	#endif
}

ZECrashHandler::~ZECrashHandler()
{
	Deinitialize();
}

LONG WINAPI ZEUnhandledExceptionHandler(EXCEPTION_POINTERS* Ex)
{
	if(zeCore->GetCrashHandler()->GetEnable())
	{
		zeCore->GetCrashHandler()->SetCrashDumpType(ZE_CDT_FULL);
		zeCore->GetCrashHandler()->SetCreateCrashDump(true);
		zeCore->GetCrashHandler()->SetCrashReport(true);
		zeCore->GetCrashHandler()->Crashed();		
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void ZECrashHandler::Crashed()
{
	if (!Enable)
		return;

	if (CrashReport)
	{		
		HMODULE CrashReportDLL = LoadLibrary("C:/Users/onur.babaoglu/Desktop/ZE/trunk/Build/x64/Source/ZETools/ZECrashReport/Debug/ZECrashReport.dll");		
		if (CrashReportDLL == NULL)
			return;

		void* FunctionPointer = GetProcAddress(CrashReportDLL, "CrashReportMain");
		if (FunctionPointer == NULL)
			return;

		ZECrashReportParameters Data;
		Data.ProcessId = GetCurrentProcessId();
		Data.CreateDump = GetCreateCrashDump();
		Data.DumpType = GetCrashDumpType();
		Data.LogFile = ZELog::GetInstance()->GetLogFileEnabled();
		//strcpy(Data.LogFilePath, ZELog::GetInstance()->GetLogFileName());
		strcpy(Data.URL, "http://localhost:8080/puttest/test.dat");
		Data.Version = ZEVersion::GetZinekVersion();

		ZEString NamedPipeName = "\\\\.\\pipe\\fsdf";//+ ZEGUID::Generate().ToString();		

		HANDLE NamedPipeHandle = CreateNamedPipe(NamedPipeName.ToCString(), PIPE_ACCESS_DUPLEX,	PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, NULL, NULL, INFINITE, NULL);
		
		ZEString CommandArgument = "rundll32.exe C:/Users/onur.babaoglu/Desktop/ZE/trunk/Build/x64/Source/ZETools/ZECrashReport/Release/ZECrashReport.dll, CrashReportMain fsdf";
		
		if(WinExec(CommandArgument.ToCString(), SW_NORMAL) < 32)
			return;

		ConnectNamedPipe(NamedPipeHandle, NULL);
		DWORD Temp;
		WriteFile(NamedPipeHandle, &Data, sizeof(ZECrashReportParameters), &Temp, NULL);
		DWORD Result;
		ReadFile(NamedPipeHandle, &Result, sizeof(DWORD), &Temp, NULL);
	}
}

void ZECrashHandler::Initialize()
{
	if (Initialized)
		return;

	SetUnhandledExceptionFilter(ZEUnhandledExceptionHandler);
}

void ZECrashHandler::Deinitialize()
{
	if (!Initialized)
		return;

	SetUnhandledExceptionFilter(NULL);
}
