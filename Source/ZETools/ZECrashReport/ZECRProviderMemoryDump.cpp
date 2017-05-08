//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRProviderMemoryDump.cpp
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

#include "ZECRProviderMemoryDump.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")
#include "ZEDS/ZEFormat.h"
#include "ZEGUID.h"

ZECRDataProviderType ZECRProviderMemoryDump::GetProviderType()
{
	return ZECR_DPT_BINARY;
}

const char* ZECRProviderMemoryDump::GetName()
{
	return "Crash Dump";
}

const char* ZECRProviderMemoryDump::GetExtension()
{
	return ".dmp";
}

void ZECRProviderMemoryDump::SetProcessId(ZEUInt32 ProcessId)
{
	this->ProcessId = ProcessId;
}

ZEUInt32 ZECRProviderMemoryDump::GetProcessId()
{
	return ProcessId;
}

void ZECRProviderMemoryDump::SetDumpType(ZECrashDumpType Type)
{
	DumpType = Type;
}
ZECrashDumpType ZECRProviderMemoryDump::GetDumpType()
{
	return DumpType;
}

bool ZECRProviderMemoryDump::Generate()
{
	MINIDUMP_TYPE DumpFlags;

	switch(DumpType)
	{
		case ZE_CDT_MINIMAL:		
			DumpFlags = (MINIDUMP_TYPE)(MiniDumpNormal);
			break;

		case ZE_CDT_NORMAL:
			DumpFlags = (MINIDUMP_TYPE)(MiniDumpWithDataSegs | MiniDumpWithHandleData | MiniDumpWithFullMemoryInfo | MiniDumpWithThreadInfo);
			break;

		case ZE_CDT_FULL:
			DumpFlags = (MINIDUMP_TYPE)(MiniDumpWithFullMemory | MiniDumpWithFullMemoryInfo | MiniDumpWithHandleData | MiniDumpWithThreadInfo);
			break;
	}

	char TempFolder[MAX_PATH];
	if (GetTempPath (MAX_PATH, TempFolder) == 0)
		return false;

	SetFileName(ZEFormat::Format("{0}{1}.zeDump", TempFolder, ZEGUID::Generate().ToString()));
	HANDLE hFile = CreateFileA(GetFileName(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, ProcessId);
	if (!MiniDumpWriteDump(hProcess, ProcessId, hFile, DumpFlags, NULL, NULL, NULL))
	{			
		CloseHandle(hFile);
		DeleteFileA(GetFileName());
		return false;
	}

	CloseHandle(hFile);		
	
	return ZECRProviderFile::Generate();
}

ZECRProviderMemoryDump::ZECRProviderMemoryDump()
{
	ProcessId = 0;
	DumpType = ZE_CDT_NORMAL;
}
