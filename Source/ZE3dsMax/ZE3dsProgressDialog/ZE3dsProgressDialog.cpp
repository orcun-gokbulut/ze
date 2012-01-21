//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsProgressDialog.cpp
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

#include "ZE3dsProgressDialog.h"
#include "ZE3dsProgressDialogResources.h"


#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

bool CloseDialog = false;
INT_PTR CALLBACK ZEProgDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{
	//static ZESModelExporter *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			//imp = (ZESModelExporter *)lParam;
			//CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			CloseDialog = true;
			EndDialog(hWnd, 0);
			return TRUE;
		case WM_COMMAND:
			if (LOWORD(wParam) == IDC_ACTION)
			{
				CloseDialog = true;
				EndDialog(hWnd, 0);
				return TRUE;
			}
			break;
	}	return 0;
}

ZE3dsProgressDialog* ZE3dsProgressDialog::Instance = NULL;
void ZE3dsProgressDialog::Create(void* Instance)
{
	CloseDialog = false;
	handle = CreateDialogParam((HINSTANCE)Instance, 
				MAKEINTRESOURCE(IDD_PANEL), 
				GetActiveWindow(), 
				ZEProgDlgProc, (LPARAM)this);
	
	SendMessage(GetDlgItem((HWND)handle, IDC_LOG), EM_SETLIMITTEXT, 0, 0);
}

void ZE3dsProgressDialog::ProcessMessages()
{
	MSG msg;
	while(true)
	{
		if (PeekMessage(&msg, (HWND)handle, 0, 0, PM_REMOVE) == 0)
			break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void ZE3dsProgressDialog::Show()
{
	ShowWindow((HWND)handle, TRUE);
}

void ZE3dsProgressDialog::Hide()
{
	ShowWindow((HWND)handle, FALSE);
}

void ZE3dsProgressDialog::Update()
{
	UpdateWindow((HWND)handle);
	ProcessMessages();
}

bool ZE3dsProgressDialog::IsCanceled()
{
	return CloseDialog;
}

void ZE3dsProgressDialog::SetOutputLevel(ZE3dsProgressDialogOutputLevel OutputLevel)
{
	this->OutputLevel = OutputLevel;
}

void ZE3dsProgressDialog::StartFileLogging(TCHAR* Filename)
{
	LogFile = fopen(Filename, "w");
	if (LogFile == NULL)
		Output(_T("Can not open \"%s\" log file for writing.\r\n"), Filename);
}

void ZE3dsProgressDialog::StopFileLogging()
{
	if (LogFile != NULL)
		fclose(LogFile);
}

#define PBM_SETPOS              (WM_USER+2)
#define PBM_SETRANGE32          (WM_USER+6)

void ZE3dsProgressDialog::SetProgress(ZEInt Value)
{
	SendMessage(GetDlgItem((HWND)handle, IDC_TOTALPROGRESS), PBM_SETPOS, (WPARAM) Value, 0); 
	Update();
}

void ZE3dsProgressDialog::SetProgressRange(ZEInt Min, ZEInt Max)
{
	SendMessage(GetDlgItem((HWND)handle, IDC_TOTALPROGRESS), PBM_SETRANGE32, (WPARAM)Min, (LPARAM)Max);
}

void ZE3dsProgressDialog::SetPartialProgress(ZEInt Value)
{
	if ((UpdateTreshold != 0) && (Value % UpdateTreshold == 0))
	{
		SendMessage(GetDlgItem((HWND)handle, IDC_PARTIALPROGRESS), PBM_SETPOS, (WPARAM) Value, 0); 
		Update();
	}
}

void ZE3dsProgressDialog::SetPartialProgressRange(ZEInt Min, ZEInt Max)
{
	UpdateTreshold = (Min - Max) / 100;
	if (UpdateTreshold == 0)
		UpdateTreshold = 1;

	SendMessage(GetDlgItem((HWND)handle, IDC_PARTIALPROGRESS), PBM_SETRANGE32, (WPARAM)Min, (LPARAM)Max);
}

static char* OutputLevelToString(ZE3dsProgressDialogOutputLevel OutputLevel)
{
	switch(OutputLevel)
	{
		case ZE3DS_PDOL_ERRORS:
			return "ERROR : ";
		case ZE3DS_PDOL_WARNINGS:
			return "Warning : ";
		case ZE3DS_PDOL_NOTICE:
			return "Notice : ";
		case ZE3DS_PDOL_LOG:
		default:
			return "";
	}
}

void ZE3dsProgressDialog::Output(TCHAR* Format, ...)
{
	TCHAR Buffer[2048];

	va_list vlist;
	va_start(vlist, Format);
	_vstprintf_s(Buffer, 1024, Format, vlist);
	va_end(vlist);
		
	if (LogFile != NULL)
		fputs(Buffer, LogFile);

	ZEInt Length = GetWindowTextLength(GetDlgItem((HWND)handle, IDC_LOG)); 
	SendMessage(GetDlgItem((HWND)handle, IDC_LOG), EM_SETSEL, (WPARAM)Length, (LPARAM)Length);
	SendMessage(GetDlgItem((HWND)handle, IDC_LOG) ,EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)Buffer);
	Update();
}

void ZE3dsProgressDialog::Output(ZE3dsProgressDialogOutputLevel OutputLevel, TCHAR* Format, ...)
{
	TCHAR Buffer[2048];

	va_list vlist;
	va_start(vlist, Format);
	_vstprintf_s(Buffer, 1024, Format, vlist);
	va_end(vlist);
	
	TCHAR Buffer2[2100];
	_stprintf(Buffer2, "%s%s\r\n", OutputLevelToString(OutputLevel), Buffer);

	if (LogFile != NULL)
		fputs(Buffer2, LogFile);

	ZEInt Length = GetWindowTextLength(GetDlgItem((HWND)handle, IDC_LOG)); 
	SendMessage(GetDlgItem((HWND)handle, IDC_LOG), EM_SETSEL, (WPARAM)Length, (LPARAM)Length);
	SendMessage(GetDlgItem((HWND)handle, IDC_LOG) ,EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)Buffer2);
	Update();
}
void ZE3dsProgressDialog::SetExitMode(bool Exit)
{
	if (Exit)
	{
		SendMessage(GetDlgItem((HWND)handle, IDC_ACTION), WM_SETTEXT, -1, (LPARAM)_T("Close"));
		MSG msg;
		CloseDialog = false;
		while(CloseDialog == false)
		{
			GetMessage(&msg, (HWND)handle, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	else
		SendMessage(GetDlgItem((HWND)handle, IDC_ACTION), WM_SETTEXT, -1, (LPARAM)_T("Cancel"));

	Update();
}

ZE3dsProgressDialog* ZE3dsProgressDialog::GetInstance()
{
	return Instance;
}

ZE3dsProgressDialog::ZE3dsProgressDialog()
{
	Instance = this;
	LogFile = NULL;
	handle = NULL;
}

ZE3dsProgressDialog::~ZE3dsProgressDialog()
{
	Instance = NULL;
	LogFile = NULL;
}
