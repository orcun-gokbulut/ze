//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRWindow.cpp
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

#include "ZECRWindow.h"

#include "ui_ZECRWindow.h"

#include "ZECRProviderApplicationInformation.h"
#include "ZECRProviderSystemInformation.h"
#include "ZECRProviderFile.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "ZECRProviderMemoryDump.h"

const ZECRCrashReporterParameters& ZECRWindow::GetParameters()
{
	return Parameters;
}

ZECRCrashReport* ZECRWindow::GetCrashReport()
{
	return &CrashReport;
}

void ZECRWindow::SetPage(ZECRWindowPageId Id)
{
	if (CurrentPage == Id)
		return;

	CurrentPage = Id;
	switch (CurrentPage)
	{
		case ZECR_WP_NONE:
			Form->stckWidgets->setCurrentIndex(-1);
			Form->widGeneratingReport->Deactivated();
			break;

		case ZECR_WP_GENERATING_REPORT:
			Form->stckWidgets->setCurrentIndex(1);
			Form->widGeneratingReport->Activated();
			break;

		case ZECR_WP_INFORMATION:
			Form->stckWidgets->setCurrentIndex(2);
			Form->widInformation->Deactivated();
			break;

		case ZECR_WP_USER_FEEDBACK:
			Form->stckWidgets->setCurrentIndex(3);
			Form->widUserFeedback->Deactivated();
			break;

		case ZECR_WP_TRANSFERING:
			Form->stckWidgets->setCurrentIndex(4);
			Form->widTransfering->Deactivated();
			break;

		case ZECR_WP_TRANSFE_COMPLETED:
			Form->stckWidgets->setCurrentIndex(5);
			Form->widTransferCompleted->Deactivated();
			break;

		default:
			break;
	}
}

void ZECRWindow::Process(const ZEString& CommandArguments)
{
	ApplicationPipe = CreateFile(CommandArguments.ToCString(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); 
	if (ApplicationPipe == INVALID_HANDLE_VALUE)
		abort();

	DWORD Temp = 0xEEFF0012;
	WriteFile(static_cast<HANDLE>(ApplicationPipe), &Temp, sizeof(Temp), NULL, NULL);

	ZECRCrashReporterParameters Parameters;
	if (!ReadFile(static_cast<HANDLE>(ApplicationPipe), &Parameters, sizeof(ZECRCrashReporterParameters), &Temp, NULL))
		abort();

	if (strlen(Parameters.LogFilePath) != 0)
	{
		ZECRProviderFile* FileProvider = new ZECRProviderFile();
		FileProvider->SetName("Log");
		FileProvider->SetFileName(Parameters.LogFilePath);
		FileProvider->SetBinary(false);
	}

	ZECRProviderMemoryDump*	DumpProvider = new ZECRProviderMemoryDump();
	DumpProvider->SetName("Dump");
	DumpProvider->SetBinary(true);
	DumpProvider->SetProcessId(Parameters.ProcessId);
	DumpProvider->SetDumpType(ZE_CDT_NORMAL);
	CrashReport.RegisterProvider(DumpProvider);

	ZECRProviderApplicationInformation* ApplicationProvider = new ZECRProviderApplicationInformation();
	ApplicationProvider->SetProcessId(Parameters.ProcessId);
	CrashReport.RegisterProvider(ApplicationProvider);

	ZECRProviderSystemInformation* SystemInformation = new ZECRProviderSystemInformation();
	CrashReport.RegisterProvider(SystemInformation);

	SetPage(ZECR_WP_GENERATING_REPORT);
}

void ZECRWindow::TerminateApplication()
{
	if (ApplicationPipe == NULL)
		return;

	DWORD Result = 1;
	DWORD Temp = 0xEEFF0012;
	WriteFile(static_cast<HANDLE>(ApplicationPipe), &Result, sizeof(DWORD), &Temp, NULL);
	CloseHandle(static_cast<HANDLE>(ApplicationPipe));

	ApplicationPipe = NULL;
}

ZECRWindow::ZECRWindow() : QDialog(NULL)
{
	ApplicationPipe = NULL;
	CurrentPage = ZECR_WP_NONE;

	Form = new Ui_ZECRWindow();	
	Form->setupUi(this);
}

ZECRWindow::~ZECRWindow()
{
	TerminateApplication();
	delete Form;
}
