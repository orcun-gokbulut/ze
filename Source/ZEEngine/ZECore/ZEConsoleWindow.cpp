//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEConsoleWindow.cpp
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

#include "ZEConsoleWindow.h"

#include "ZECore.h"
#include "ZEError.h"
#include "ZETypes.h"
#include "ZEConsoleWindowResources.h"
#include "ZEPlatform.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdarg.h>
#include <winuser.h>

#pragma warning(push)
#pragma warning(disable:4996 4311 4312)

bool Wait;
WNDPROC OrgInputBoxCallback;

INT_PTR CALLBACK InputBoxCallback(HWND hwndEdit, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
			case WM_GETDLGCODE:
				return (DLGC_WANTALLKEYS | CallWindowProc(OrgInputBoxCallback, hwndEdit, uMsg, wParam, lParam));

			case WM_CHAR:
				if (wParam == VK_RETURN)
					return 0;
				else
					return CallWindowProc(OrgInputBoxCallback, hwndEdit, uMsg, wParam, lParam);

			case WM_KEYDOWN:

				if (wParam == VK_ESCAPE)
				{
					ZEConsole::GetInstance()->HideConsole();
					return 0;
				}
				if (wParam == VK_RETURN) 
				{
					char Buffer[100];
					SendMessage(hwndEdit, WM_GETTEXT, 100, (LPARAM)Buffer);
					SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM)"");
					ZEConsole::GetInstance()->Input(Buffer);
					 return 0;
				}
				return CallWindowProc(OrgInputBoxCallback, hwndEdit, uMsg, wParam, lParam);
				break;

			default:
               	return CallWindowProc(OrgInputBoxCallback, hwndEdit, uMsg, wParam, lParam);		
	}
}

INT_PTR CALLBACK ConsoleCallback(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			SendMessage(GetDlgItem(hwndDlg, IDC_OUTPUT), EM_SETLIMITTEXT, 0, 0);
			SendMessage(GetDlgItem(hwndDlg,IDC_INPUT), EM_SETLIMITTEXT, 99, 0);			
			SendMessage(GetDlgItem(hwndDlg, IDC_OUTPUT), WM_SETFONT, (WPARAM)GetStockObject(SYSTEM_FIXED_FONT), MAKELPARAM(TRUE, 0));
			#ifdef ZE_PLATFORM_ARCHITECTURE_X64
				OrgInputBoxCallback = (WNDPROC)SetWindowLongPtr(GetDlgItem(hwndDlg, IDC_INPUT), GWLP_WNDPROC, (LONG_PTR)InputBoxCallback);
			#else
				OrgInputBoxCallback = (WNDPROC)SetWindowLong(GetDlgItem(hwndDlg, IDC_INPUT), GWL_WNDPROC, (LONG)InputBoxCallback);
			#endif
			SetFocus(GetDlgItem(hwndDlg,IDC_INPUT));
			break;
		case WM_COMMAND:
			if (LOWORD(wParam) == IDEXECUTE)
			{
				if (Wait)
				{
					DestroyWindow(hwndDlg);
					Wait = false;
					break;
				}
				else
				{
					char Buffer[100];
					SendMessage(GetDlgItem(hwndDlg, IDC_INPUT), WM_GETTEXT, 100, (LPARAM)Buffer);
					SendMessage(GetDlgItem(hwndDlg, IDC_INPUT), WM_SETTEXT, 0, (LPARAM)"");
					ZEConsole::GetInstance()->Input(Buffer);
					break;
				}
			}
			break;

		case WM_CLOSE:
			if (Wait)
			{
				DestroyWindow(hwndDlg);
				Wait = false;
			}
			else
				ShowWindow(hwndDlg, SW_HIDE);

			return 0;
				
		case WM_DESTROY:
			#ifdef ZE_PLATFORM_ARCHITECTURE_X64
				SetWindowLongPtr(GetDlgItem(hwndDlg, IDC_INPUT), GWLP_WNDPROC, (LONG_PTR)OrgInputBoxCallback);
			#else
				SetWindowLong(GetDlgItem(hwndDlg, IDC_INPUT), GWL_WNDPROC, (LONG)OrgInputBoxCallback);
			#endif
			break;	
		default:
			return FALSE;
	}
	return TRUE;
}

bool ZEConsoleWindow::Initialize()
{
	#ifdef ZE_DEBUG_ENABLE
	const char* EngineDLLPath = "ZEEngine-Debug.dll";
	#else
	const char* EngineDLLPath = "ZEEngine.dll";
	#endif

 	HINSTANCE ModuleHandle = (HINSTANCE)GetModuleHandle(EngineDLLPath);
	Handle = CreateDialog(ModuleHandle, MAKEINTRESOURCE(IDD_CONSOLE), NULL, &ConsoleCallback);
	if (Handle == NULL)
	{
		zeError("Can not create console window.");
		return false;
	}

	ShowWindow((HWND)Handle, SW_HIDE);

	return true;
}

void ZEConsoleWindow::Deinitialize()
{
	DestroyWindow((HWND)Handle);
}

void ZEConsoleWindow::Process()
{
	BufferLock.Lock();
	if (!Buffer.IsEmpty())
	{
		SendMessage(GetDlgItem((HWND)Handle, IDC_OUTPUT), EM_SETSEL, (WPARAM)0, (LPARAM)-1);
		SendMessage(GetDlgItem((HWND)Handle, IDC_OUTPUT), EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
		SendMessage(GetDlgItem((HWND)Handle, IDC_OUTPUT), EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)Buffer.GetValue());

		Buffer.Clear();

		UpdateWindow((HWND)Handle);
	}
	BufferLock.Unlock();
}

void ZEConsoleWindow::EnableInput()
{
	EnableWindow(GetDlgItem((HWND)Handle, IDEXECUTE), TRUE);
	EnableWindow(GetDlgItem((HWND)Handle, IDC_INPUT), TRUE);
}

void ZEConsoleWindow::DisableInput()
{
	EnableWindow(GetDlgItem((HWND)Handle, IDEXECUTE), FALSE);
	EnableWindow(GetDlgItem((HWND)Handle, IDC_INPUT), FALSE);
}
void ZEConsoleWindow::ShowConsole()
{
	ShowWindow((HWND)Handle, SW_SHOW);
	SetFocus(GetDlgItem((HWND)Handle, IDC_INPUT));
}

void ZEConsoleWindow::HideConsole()
{
	ShowWindow((HWND)Handle, SW_HIDE);
}

void ZEConsoleWindow::TerminationState()
{
	ShowConsole();
	MSG msg;
	SetWindowText(GetDlgItem((HWND)Handle, IDEXECUTE), "Exit");
	EnableWindow(GetDlgItem((HWND)Handle, IDEXECUTE), TRUE);
	Wait = true;

	while(Wait == true)
	{
		if (GetMessage(&msg, NULL, 0, 0))
		{
			if (msg.message == WM_QUIT)
				return;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	} 
}

void ZEConsoleWindow::Output(const char* OutputText)
{
	BufferLock.Lock();
	const char* Current = OutputText;
	const char* Start = Current;

	while (*Current != '\0')
	{
		if (*Current == '\n')
		{
			if (Current != Start)
				Buffer.Append(Start, Current - Start - (Current != OutputText && Current[-1] == '\r' ? 1 : 0));

			Buffer.Append("\r\n");
			Start = Current + 1;
		}
		Current++;
	}

	if (Current != Start)
		Buffer.Append(Start);

	BufferLock.Unlock();
}

ZEConsoleWindow::ZEConsoleWindow(void)
{
	Wait = false;
	Handle = NULL;
}

ZEConsoleWindow::~ZEConsoleWindow(void)
{

}
#pragma warning(pop)
