//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRCollectorSystemInformation.cpp
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

#include "ZECRCollectorSystemInformation.h"

#include "ZEDS/ZEFormat.h"
#include "ZEFile/ZEFileInfo.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>


ZECRDataProviderType ZECRCollectorSystemInformation::GetCollectorType()
{
	return ZECR_DPT_TEXT;
}

const char* ZECRCollectorSystemInformation::GetName()
{
	return "System Information";
}

const char* ZECRCollectorSystemInformation::GetExtension()
{
	return ".nfo";
}

bool ZECRCollectorSystemInformation::Generate(ZEMLWriterNode* CollectorNode, const ZECRReportParameters* Parameters)
{	
	char TempFolder[MAX_PATH];
	if (GetTempPath (MAX_PATH, TempFolder) == 0)
		return false;

	SetFileName(ZEFormat::Format("{0}{1}.nfo", TempFolder, ZEGUID::Generate().ToString()));

	STARTUPINFO StartupInfo;
	memset(&StartupInfo, 0, sizeof(STARTUPINFO));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION ProcessInfo;
	memset(&ProcessInfo, 0, sizeof(PROCESS_INFORMATION));

	ZEString FileRealPath = ZEFileInfo(GetFileName()).GetRealPath().Path;

	BOOL Result = CreateProcessA(NULL, 
		(char*)ZEFormat::Format("msinfo32 /nfo \"{0}\"", FileRealPath).ToCString(), 
		NULL, NULL, false, NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &ProcessInfo);
	if (!Result)
		return false;

	DWORD ExitCode;
	while (GetExitCodeProcess(ProcessInfo.hProcess, &ExitCode))
	{
		if (ExitCode == STILL_ACTIVE)
		{
			WaitForSingleObject(ProcessInfo.hProcess, 1000);
			continue;
		}
		else if (ExitCode == EXIT_FAILURE)
		{
			ZEFileInfo(GetFileName()).Delete();
			return false;
		}
		else
		{
			break;
		}
	}

	return ZECRCollectorFile::Generate(CollectorNode, Parameters);
}

ZECRCollectorSystemInformation::ZECRCollectorSystemInformation()
{
	SetBinary(false);
	SetDeleteOnExit(true);
}
