//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEWindow.cpp
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


#include "ZECore.h"
#include "ZEError.h"
#include "ZEWindow.h"
#include "ZEConsole.h"
#include "ZEInput/ZEInputModule.h"
#include "ZESystemMessageManager.h"
#include "ZESystemMessageHandler.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <stdio.h>

#define ZEWINDOW_WINDOWNAME		"Zinek Engine"
#define ZEWINDOW_CLASSNAME		"ZINEKENGINE"

char*  Parameters;

ZEWindow* Window = NULL;
bool WindowInitialization;


static void PaintBlack(HWND hwnd)
{
	PAINTSTRUCT ps;
	RECT rc;
	HDC hdc = BeginPaint(hwnd, &ps);
	GetClientRect(hwnd, &rc);
	HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
	FillRect(hdc, &rc, hBrush);
	EndPaint(hwnd, &ps);
	DeleteObject(hBrush);
}

static void ManageCursorVisibility(HWND hWnd, LPARAM lParam)
{
	WORD Handle = LOWORD(lParam);
	static bool CursorHidden = false;
	if (Handle == HTCLIENT && !CursorHidden && !Window->GetMouseCursorVisibility() && GetFocus() == hWnd)
	{
		CursorHidden = true;
		ShowCursor(false);
	}
	else if (Handle != HTCLIENT && CursorHidden) 
	{
		CursorHidden = false;
		ShowCursor(true);
	}
}

static void LockMousePosition(HWND hWnd)
{
	if (!Window->GetMouseCursorLockEnabled())
		return;

	RECT Rect;
	GetClientRect(hWnd, &Rect);

	RECT ScreenRect;
	POINT Point;
	Point.x = Rect.left; Point.y = Rect.top;
	ClientToScreen(hWnd, &Point);
	ScreenRect.left = Point.x; ScreenRect.top = Point.y;
	Point.x = Rect.right; Point.y = Rect.bottom;
	ClientToScreen(hWnd, &Point);
	ScreenRect.right = Point.x; ScreenRect.bottom = Point.y;

	ClipCursor(&ScreenRect);
}

static void UnlockMousePosition(HWND hWnd)
{
	ClipCursor(NULL);
}

LRESULT CALLBACK Callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_CREATE:
			break;

		case WM_SIZE:
			if (!WindowInitialization)
				Window->WindowResized(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_PAINT:
			if (ZECore::GetInstance()->GetGraphicsModule() != NULL)
				ValidateRect(hWnd, NULL);
			else
				PaintBlack(hWnd);
			break;

		case WM_SETCURSOR:
			ManageCursorVisibility(hWnd, lParam);
			break;

		/*case WM_ACTIVATEAPP:
			if (wParam == TRUE)
			{
				Window->WindowGainedFocus();
				LockMousePosition(hWnd);
			}
			else
			{
				Window->WindowLostFocus();
				UnlockMousePosition(hWnd);
			}
			break;*/
		
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			LockMousePosition(hWnd);
			break;

		case WM_ACTIVATE:
			if (wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE)
			{
				Window->WindowGainedFocus();
				LockMousePosition(hWnd);
			}
			else if (wParam == WA_INACTIVE)
			{
				Window->WindowLostFocus();
				UnlockMousePosition(hWnd);
			}
			break;

		case WM_CLOSE:
			if (MessageBox(hWnd, "Do you really want to exit Zinek Engine ?", "Zinek Engine", MB_ICONQUESTION | MB_YESNO) == IDYES)
			{
				Window->WindowDestroyed();
			}
			break;

		case WM_DESTROY:
			Window->WindowDestroyed();
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return true;
}

void ShowWindowError()
{
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

	zeError("Win32API Error no : %u, Error text : \"%s\"", dw, lpMsgBuf); 
    LocalFree(lpMsgBuf);
}

void ZEWindow::WindowGainedFocus()
{
	zeLog("Main window gained focus.");
	if (ZEInputModule::GetInstance() != NULL)
		ZEInputModule::GetInstance()->Acquire();
}

void ZEWindow::WindowLostFocus()
{
	zeLog("Window lost focus.");
	if (ZEInputModule::GetInstance() != NULL)
		ZEInputModule::GetInstance()->UnAcquire();
}

void ZEWindow::WindowDestroyed()
{
	exit(EXIT_SUCCESS);
	zeCore->ShutDown();
}

bool ZEWindow::CreateMainWindow(const char* WindowTitle)
{
	DWORD Style;
	switch(WindowType)
	{
		case ZE_WT_DEFAULT:
		case ZE_WT_FIXEDSIZE:
			Style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZE;
			break;
		case ZE_WT_RESIZABLE:
			Style = WS_SIZEBOX | WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZE;
			break;
		case ZE_WT_FULLSCREEN:
			Style = WS_EX_TOPMOST | WS_POPUP;
			break;
		case ZE_WT_COMPONENT:
			zeError("Wrong Window Type you can not create window with component type. Component windows are provided from the out size of Zinek Engine. Use SetComponentWindow function.");
			return false;
	}

	WNDCLASSEX wc;
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= Callback;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
	wc.hInstance		= (HINSTANCE)zeCore->GetApplicationInstance();
    wc.hIcon			= NULL;
    wc.hCursor			= NULL;
    wc.hbrBackground	= NULL;
    wc.lpszMenuName		= NULL;
    wc.lpszClassName	= "ZINEK ENGINE WINDOW";
    wc.hIconSm			= NULL;

	if(RegisterClassEx(&wc) == 0)
	{
		ShowWindowError();
		zeError("Could not register windows class.");
		ShowWindowError();
		return false;
	}

	WindowHandle = CreateWindowEx(WS_EX_APPWINDOW, 
        "ZINEK ENGINE WINDOW", 
        WindowTitle, 
		Style, 
        CW_USEDEFAULT,
		0, 
		WindowWidth,
        WindowHeight, 
        NULL, 
        NULL, 
		(HINSTANCE)zeCore->GetApplicationInstance(), 
        NULL);
	
	if (!WindowHandle)
	{
		ShowWindowError();
		zeError("Can not create window.");
		return false;
	}
	
	if (WindowType != ZE_WT_FULLSCREEN)
		SetWindowSize(WindowWidth, WindowHeight);

	ShowWindow();

	return true;
}

bool ZEWindow::DestroyMainWindow()
{
	if (DestroyWindow((HWND)WindowHandle) == 0)
	{
		zeError("Can not destroy window.");
		ShowWindowError();
		return false;
	}
	 
	return true;
}

void ZEWindow::SetWindowType(ZEWindowType WindowType)
{
	this->WindowType = WindowType;
}

ZEWindowType ZEWindow::GetWindowType()
{
	return WindowType;
}

bool ZEWindow::SetComponentWindowHandle(void* Handle)
{
	WindowHandle = Handle;
	RECT Rect;
	GetWindowRect((HWND)WindowHandle, &Rect);
	WindowWidth = Rect.right - Rect.left;
	WindowHeight = Rect.bottom - Rect.top;
	if (ZEGRGraphicsModule::GetInstance() != NULL)
		ZEGRGraphicsModule::GetInstance()->SetScreenSize(WindowWidth, WindowHeight);

	WindowType = ZE_WT_COMPONENT;
	return true;
}

void ZEWindow::WindowResized(ZEInt Width, ZEInt Height)
{

/*	if (WindowHandle != NULL && WindowType != ZE_WT_COMPONENT)
	{
		RECT ClientRectangle, WindowRectangle;
		POINT Difference;
		GetClientRect((HWND)WindowHandle, &ClientRectangle);
		GetWindowRect((HWND)WindowHandle, &WindowRectangle);
		Difference.x = (WindowRectangle.right - WindowRectangle.left) - ClientRectangle.right;
		Difference.y = (WindowRectangle.bottom - WindowRectangle.top) - ClientRectangle.bottom;
	}*/

	if (ZEGRGraphicsModule::GetInstance() != NULL)
		ZEGRGraphicsModule::GetInstance()->SetScreenSize(Width, Height);

	WindowWidth = Width;
	WindowHeight = Height;
}

void ZEWindow::SetWindowPosition(ZEInt Left, ZEInt Top)
{
	if (WindowType == ZE_WT_COMPONENT || WindowType == ZE_WT_FULLSCREEN)
		return;
	WindowPositionLeft = Left;
	WindowPositionTop = Top;
	if (WindowHandle != NULL)
	{
		RECT Rect;
		GetWindowRect((HWND)WindowHandle, &Rect);
		MoveWindow((HWND)WindowHandle, Left, Top, Rect.right - Rect.left, Rect.bottom - Rect.top, FALSE);
	}
}

void ZEWindow::GetWindowPosition(ZEInt& Left, ZEInt& Top)
{
	RECT Rect;
	GetWindowRect((HWND)WindowHandle, &Rect);
	Left = Rect.left;
	Top = Rect.top;
}

void ZEWindow::SetWindowSize(ZEInt Width, ZEInt Height)
{
	if (WindowType != ZE_WT_COMPONENT)
	{
		RECT ClientRectangle, WindowRectangle;
		POINT Difference;
		if (WindowHandle != NULL)
		{
			GetClientRect((HWND)WindowHandle, &ClientRectangle);
			GetWindowRect((HWND)WindowHandle, &WindowRectangle);
			Difference.x = (WindowRectangle.right - WindowRectangle.left) - ClientRectangle.right;
			Difference.y = (WindowRectangle.bottom - WindowRectangle.top) - ClientRectangle.bottom;
			MoveWindow((HWND)WindowHandle, WindowRectangle.left, WindowRectangle.top, Width + Difference.x, Height + Difference.y, TRUE);
		}
	}
	WindowResized(Width, Height);
}

void ZEWindow::GetWindowSize(ZEInt& Width, ZEInt& Height)
{
	RECT Rect;
	GetClientRect((HWND)WindowHandle, &Rect);
	Width = Rect.right - Rect.left;
	Height = Rect.bottom - Rect.top;
}

void* ZEWindow::GetHandle()
{
	return WindowHandle;
}

ZEVector2 ZEWindow::GetAbsoluteCursorPosition()
{
	POINT CursorPosition;
	GetCursorPos(&CursorPosition);

	return ZEVector2((float)CursorPosition.x, (float)CursorPosition.y);
}

ZEVector2 ZEWindow::GetRelativeCursorPosition()
{
	POINT CursorPosition;
	GetCursorPos(&CursorPosition);

	ZEInt LeftPosition;
	ZEInt TopPosition;

	GetWindowPosition(LeftPosition, TopPosition);

	return ZEVector2(((float)CursorPosition.x) - LeftPosition, ((float)CursorPosition.y) - TopPosition);
}

void ZEWindow::SetMouseCursorVisibility(bool Visibility)
{
	MouseCursorVisibility = Visibility;
}

bool ZEWindow::GetMouseCursorVisibility()
{
	return MouseCursorVisibility;
}

void ZEWindow::SetMouseCursorLockEnabled(bool Enabled)
{
	MouseCursorLockEnabled = Enabled;
}

bool ZEWindow::GetMouseCursorLockEnabled()
{
	return MouseCursorLockEnabled;
}

void ZEWindow::ShowWindow()
{
	::ShowWindow((HWND)WindowHandle, SW_SHOWNORMAL);
	UpdateWindow((HWND)WindowHandle);
}

void ZEWindow::HideWindow()
{
	::ShowWindow((HWND)WindowHandle, SW_HIDE);
	UpdateWindow((HWND)WindowHandle);
}

bool ZEWindow::Initialize()
{
	WindowInitialization = true;
	if (WindowType != ZE_WT_COMPONENT)
		if (!CreateMainWindow("Zinek Engine"))
			return false;

	WindowInitialization = false;
	return true;
}

void ZEWindow::Deinitialize()
{
	DestroyMainWindow();
	Window = NULL;
}

ZEWindow* ZEWindow::GetInstance()
{
	return ZECore::GetInstance()->GetWindow();
}

ZEWindow::ZEWindow()
{
	MouseCursorVisibility = false;
	MouseCursorLockEnabled = true;
	WindowType = ZE_WT_DEFAULT;
	WindowPositionLeft = CW_USEDEFAULT;
	WindowPositionTop = 0;
	WindowWidth = CW_USEDEFAULT;
	WindowHeight = 0;
	WindowHandle = NULL;
	Window = this;
}

ZEWindow::~ZEWindow()
{
	Window = NULL;
}
