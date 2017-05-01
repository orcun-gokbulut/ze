//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRMain.cpp
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

#pragma once

#include "ZETypes.h"
#include "ZEBase64.h"
#include "ZEDS/ZEFormat.h"

#include "ZECore/ZECrashHandler.h"
#include "ZECRWindow.h"
#include "ZECRSender.h"
#include "ZECRCrashReport.h"
#include "ZECRProviderFile.h"
#include "ZECRProviderApplicationInformation.h"
#include "ZECRProviderSystemInformation.h"
#include "ZECRProviderMemoryDump.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <QApplication>
#include <tinyxml.h>
#include <tinystr.h>

ZEInt RunUI(ZECRCrashReport& CrashReport, const ZEString& UploadURL)
{
	ZEInt argc = 0;
	char** argv = NULL;

	QApplication a(argc, argv);
	ZECRWindow* MainWindow = new ZECRWindow(&CrashReport);

	return a.exec();
}

static void Process(ZEString CommandArguments)
{	
	if(CommandArguments == NULL)
		return;
	
	HANDLE File = CreateFile(CommandArguments, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); 
	if (File == INVALID_HANDLE_VALUE)
		return;

	DWORD Temp = 0xEEFF0012;

	WriteFile(File, &Temp, sizeof(Temp), NULL, NULL);

	ZECrashReportParameters Data;
	if (!ReadFile(File, &Data, sizeof(ZECrashReportParameters), &Temp, NULL))
		return;

	ZECRCrashReport CrashReport;

	if (strlen(Data.LogFilePath) != 0)
	{
		ZECRProviderFile* FileProvider = new ZECRProviderFile();
		FileProvider->SetName("Log");
		FileProvider->SetFileName(Data.LogFilePath);
		FileProvider->SetBinary(false);
	}

	ZECRProviderMemoryDump*	DumpProvider = new ZECRProviderMemoryDump();
	DumpProvider->SetName("Dump");
	DumpProvider->SetBinary(true);
	DumpProvider->SetProcessId(Data.ProcessId);
	DumpProvider->SetDumpType(ZE_CDT_NORMAL);
	CrashReport.RegisterProvider(DumpProvider);

	ZECRProviderApplicationInformation* ApplicationProvider = new ZECRProviderApplicationInformation();
	ApplicationProvider->SetProcessId(Data.ProcessId);
	CrashReport.RegisterProvider(ApplicationProvider);

	ZECRProviderSystemInformation* SystemInformation = new ZECRProviderSystemInformation();
	CrashReport.RegisterProvider(SystemInformation);

	CrashReport.Generate();

	DWORD Result = 1;
	WriteFile(File, &Result, sizeof(DWORD), &Temp, NULL);

	CloseHandle(File);
	
	RunUI(CrashReport, "https://support.zinek.xyz/crash-report/upload");
}

extern "C" __declspec(dllexport) void CALLBACK ReportCrash(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{	
	Process(lpszCmdLine);
}

ZEInt __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, ZEInt nCmdShow)
{
	Process(lpCmdLine);
	return 0;
}
