//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESplashWindow.cpp
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

#include "ZESplashWindow.h"

#include "ZESplashWindow_Logo.bmp.h"
#include "ZEError.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static HBITMAP LogoBitmap;
static const DWORD WindowWidth = 400;
static const DWORD WindowHeight = 500;

struct ZESplashWindowData
{
	ZESplashWindow* Window;
	HWND Handle;
	HINSTANCE Instance;
};

struct ZESplashWindowMessage
{
	HWND hwnd;
	UINT uMsg;
	WPARAM wParam;
	LPARAM lParam;
	LRESULT Return;
};

static void HandleWin32Error(DWORD ErrorCode)
{
	char ErrorString[4096] = {0};

	DWORD Flags = FORMAT_MESSAGE_FROM_SYSTEM;
	DWORD Result = ::FormatMessage(Flags, NULL, ErrorCode, 0, ErrorString, 4096, NULL);
	if (Result == 0)
	{
		zeError("Unknwon Win32 error with code %u.", ErrorCode);
		return;
	}

	zeError("%s", ErrorString);
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ZESplashWindowData* Data = NULL;
	if (uMsg == WM_CREATE)
	{
		LPCREATESTRUCT CreationParams = (LPCREATESTRUCT)lParam;
		Data = (ZESplashWindowData*)CreationParams->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(CreationParams->lpCreateParams));
	}
	else if (uMsg == WM_DESTROY)
	{
		SetWindowLongPtr(hwnd, GWLP_USERDATA, NULL);
	}
	else
	{
		Data = (ZESplashWindowData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}

	// IMPORTANT !!! Route all messages to DefWindowProc until WM_CREATE received
	if (Data == NULL)
		return DefWindowProc(hwnd, uMsg, wParam, lParam);

	ZESplashWindowMessage Message;
	Message.hwnd = hwnd;
	Message.uMsg = uMsg;
	Message.wParam = wParam;
	Message.lParam = lParam;
	Data->Window->HandleMessage(&Message);
	return Message.Return;
}

void ZESplashWindow::HandleMessage(ZESplashWindowMessage* Message)
{
	Message->Return = 0;
	if (Message->uMsg != WM_PAINT)
	{
		Message->Return = DefWindowProc(Message->hwnd, Message->uMsg, Message->wParam, Message->lParam);
		return;
	}

	PAINTSTRUCT Ps;
	HDC hDC = BeginPaint(Message->hwnd, &Ps);
	HDC MemDCExercising = CreateCompatibleDC(hDC);
	SelectObject(MemDCExercising, LogoBitmap);
	BitBlt(hDC, 0, 0, WindowWidth, WindowHeight, MemDCExercising, 0, 0, SRCCOPY);
	DeleteDC(MemDCExercising);
	EndPaint(Message->hwnd, &Ps);

	ValidateRect(Data->Handle, NULL);
}

bool ZESplashWindow::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	Data = new ZESplashWindowData();
	Data->Window = this;
	Data->Instance = GetModuleHandle(NULL);

	WNDCLASSEX WindowClass = {};
	WindowClass.cbSize			= sizeof(WNDCLASSEX);
	WindowClass.style			= CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc		= &WindowProc;
	WindowClass.cbClsExtra		= 0;
	WindowClass.cbWndExtra		= 0;
	WindowClass.hInstance		= Data->Instance;
	WindowClass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	WindowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);
	WindowClass.hbrBackground	= CreateSolidBrush(RGB(128, 128, 128));
	WindowClass.lpszMenuName	= NULL;
	WindowClass.lpszClassName	= "ZESplashWindow";
	WindowClass.hIconSm			= NULL;

	BOOL Result = RegisterClassEx(&WindowClass);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	DWORD ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	DWORD ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	Data->Handle = CreateWindowEx(
		WS_EX_TOOLWINDOW | WS_EX_LAYERED, "ZESplashWindow", "Zinek Engine", WS_POPUP, 
		(ScreenWidth - WindowWidth) / 2, (ScreenHeight - WindowHeight) / 2, WindowWidth, WindowHeight,
		NULL, NULL,	Data->Instance,	Data);

	if (Data->Handle == NULL)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	ZESplashWindow_Logo_bmp Logo;
	BITMAPFILEHEADER* BMPHeader = (BITMAPFILEHEADER*)Logo.GetData();
	BITMAPINFOHEADER* BMPInfoHeader = (BITMAPINFOHEADER*)((ZEBYTE*)Logo.GetData() + sizeof(BITMAPFILEHEADER));
	BITMAPINFO* BMPInfo = (BITMAPINFO*)BMPInfoHeader;
	void* BMPData = (void*)((ZEBYTE*)Logo.GetData() + BMPHeader->bfOffBits);
	void* BMPBuffer = 0;
	LogoBitmap = CreateDIBSection(NULL, BMPInfo, DIB_RGB_COLORS, &BMPBuffer, NULL, NULL);
	memcpy(BMPBuffer, BMPData, BMPHeader->bfSize - BMPHeader->bfOffBits); // this line should be added!!
	
	::EnableWindow(Data->Handle, true);
	::ShowWindow(Data->Handle, true);

	SetLayeredWindowAttributes(Data->Handle, RGB(0, 0, 0), 255, 0);
	UpdateWindow(Data->Handle);

	return true;
}

bool ZESplashWindow::DeinitializeInternal()
{
	Hide();
	DeleteObject(LogoBitmap);
	DestroyWindow(Data->Handle);
	UnregisterClass("ZESplashWindow", Data->Instance);
	
	delete Data;
	Data = NULL;

	return ZEInitializable::DeinitializeInternal();
}

ZESplashWindow::ZESplashWindow()
{
	Data = NULL;
}

ZESplashWindow::~ZESplashWindow()
{
	Deinitialize();
}

void ZESplashWindow::SetStatusText(const ZEString& Text)
{
	StatusText = Text;
}

const ZEString& ZESplashWindow::GetStatusText() const
{
	return StatusText;
}

void ZESplashWindow::Show()
{
	Initialize();

	if (Data == NULL)
		return;

	::ShowWindow(Data->Handle, true);
}

void ZESplashWindow::Hide()
{
	if (Data == NULL)
		return;

	::ShowWindow(Data->Handle, false);
}

ZESplashWindow* ZESplashWindow::CreateInstance()
{
	return new ZESplashWindow();
}
