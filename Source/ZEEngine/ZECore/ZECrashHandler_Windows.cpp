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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")


ZECrashHandler::ZECrashHandler()
{
	char TempFolder[MAX_PATH];
	if (GetTempPath (MAX_PATH, TempFolder) != 0)
		CrashDumpFilePath = ZEFormat::Format("{0}/{1}.zeDump", TempFolder, ZEGUID::Generate().ToString());

	Enable = true;
	Initialized = false;
	CrashDumpType = ZE_CDT_NORMAL;

	#if(_DEBUG)
	{
		CrashReport = false;
		CreateCrashDump = false;
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
	zeCore->GetCrashHandler()->SetCrashReport(false);//test
	zeCore->GetCrashHandler()->SetCreateCrashDump(true);
	zeCore->GetCrashHandler()->Crashed();
	return EXCEPTION_EXECUTE_HANDLER;
}

void ZECrashHandler::Crashed()
{
	if (!Enable)
		return;

	if (CreateCrashDump)
	{
		MINIDUMP_TYPE DumpFlags;

		if(CrashDumpType == ZE_CDT_MINIMAL)
		{			
			DumpFlags = (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory);
		}

		if(CrashDumpType == ZE_CDT_NORMAL)
		{
			DumpFlags = (MINIDUMP_TYPE)(MiniDumpWithDataSegs | MiniDumpWithHandleData | MiniDumpWithFullMemoryInfo | MiniDumpWithThreadInfo | MiniDumpWithUnloadedModules);
		}

		if(CrashDumpType == ZE_CDT_FULL)
		{
			DumpFlags = (MINIDUMP_TYPE)(MiniDumpWithFullMemory | MiniDumpWithFullMemoryInfo | MiniDumpWithHandleData | MiniDumpWithThreadInfo | MiniDumpWithUnloadedModules);
		}

		HANDLE hFile = CreateFileA(CrashDumpFilePath, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if(hFile == INVALID_HANDLE_VALUE)
			return;

		HANDLE hProcess = GetCurrentProcess();
		DWORD ProcessId = GetProcessId(hProcess);

		if(!MiniDumpWriteDump(hProcess, ProcessId, hFile, DumpFlags, NULL, NULL, NULL))
		{			
			CloseHandle(hFile);
			DeleteFileA(CrashDumpFilePath);
			return;
		}

		CloseHandle(hFile);
		//CrashReport = true;
	}

	if (CrashReport)
	{		
		HMODULE CrashReportDLL = LoadLibrary("ZECrashReport.dll");		
		if (CrashReportDLL == NULL)
			return;

		void* FunctionPointer = GetProcAddress(CrashReportDLL, "CrashReport");
		if (FunctionPointer == NULL)
			return;

		((void (*)(ZECore*))FunctionPointer)(ZECore::GetInstance());
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
