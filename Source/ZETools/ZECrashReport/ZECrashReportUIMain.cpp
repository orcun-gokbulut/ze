//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECrashReportUIMain.cpp
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
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <QtGui/QApplication>
#include "ZETypes.h"
#include "ZEFoundation/ZEBase64.h"
#include "ZECrashReportUIMainWindow.h"
#include "ZECrashReport/ZECrashReportSender.h"
#include "ZECrashReport/ZECrashReportFileProvider.h"
#include "ZECrashReport/ZECrashReportApplicationInformationProvider.h"
#include "ZECrashReport/ZECrashReportSystemInformationProvider.h"
#include "ZECrashReport/ZECrashReportDumpProvider.h"
#include "tinyxml.h"
#include "tinystr.h"
#include "ZECore/ZECrashHandler.h"
#include "ZEDS/ZEFormat.h"

ZEInt RunUI(ZECrashReport& CrashReport, const ZEString& UploadURL)
{
	ZEInt argc = 0;
	char** argv = NULL;

	QApplication a(argc, argv);
	ZECrashReportUIMainWindow* MainWindow = new ZECrashReportUIMainWindow(CrashReport, UploadURL);

	return a.exec();
}

static void Process(ZEString CommandArguments)
{	
	if(CommandArguments == NULL)
		return;
	
	HANDLE File = CreateFile(ZEFormat::Format("\\\\.\\pipe\\{0}", CommandArguments).ToCString(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); 
	if (File == INVALID_HANDLE_VALUE)
		return;
	
	DWORD Temp;
	ZECrashReportParameters Data;
	if (!ReadFile(File, &Data, sizeof(ZECrashReportParameters), &Temp, NULL))
		return;

	ZECrashReport CrashReport;

	if(Data.LogFile)
	{
		ZECrashReportFileProvider* FileProvider = new ZECrashReportFileProvider();
		FileProvider->SetName("Log");
		FileProvider->SetFileName(Data.LogFilePath);
		FileProvider->SetBinary(false);
	}

	if(Data.CreateDump)
	{
		ZECrashReportDumpProvider*	DumpProvider = new ZECrashReportDumpProvider();
		DumpProvider->SetName("Dump");
		DumpProvider->SetBinary(true);
		DumpProvider->SetProcessId(Data.ProcessId);
		DumpProvider->SetDumpType(Data.DumpType);
		CrashReport.RegisterProvider(DumpProvider);
	}

	ZECrashReportApplicationInformationProvider* ApplicationProvider = new ZECrashReportApplicationInformationProvider();
	ApplicationProvider->SetProcessId(Data.ProcessId);
	CrashReport.RegisterProvider(ApplicationProvider);

	ZECrashReportSystemInformationProvider* SystemInformation = new ZECrashReportSystemInformationProvider();
	CrashReport.RegisterProvider(SystemInformation);

	CrashReport.Generate();

	DWORD Result = 1;
	WriteFile(File, &Result, sizeof(DWORD), &Temp, NULL);

	CloseHandle(File);
	
	RunUI(CrashReport, Data.URL);
}

extern "C" __declspec(dllexport) void CALLBACK CrashReportMain(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{	
	Process(lpszCmdLine);
}

ZEInt __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, ZEInt nCmdShow)
{
	Process(lpCmdLine);
	return 0;
}
