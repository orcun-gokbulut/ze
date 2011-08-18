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

#include "ZEWindow.h"
#include "ZECore.h"
#include "ZEError.h"
#include "ZEConsole.h"
#include "ZEInput\ZEInputModule.h"
#include "ZEGraphics\ZEGraphicsModule.h"
#include "ZESystemMessageManager.h"
#include "ZESystemMessageHandler.h"

#define WINDOWS_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <stdio.h>

#define ZEWINDOW_WINDOWNAME		"Zinek Engine"
#define ZEWINDOW_CLASSNAME		"ZINEKENGINE"

char*  Parameters;

ZEWindow* Window = NULL;
bool WindowInitialization;

class ZEWindowSystemMessageHandler : public ZESystemMessageHandler
{
	public:
		virtual bool Callback(MSG* Message);
};

bool ZEWindowSystemMessageHandler::Callback(MSG* Message)
{
	switch (Message->message)
	{
		case WM_CREATE:
			return true;

		case WM_SIZE:
			if (!WindowInitialization)
				Window->WindowResized(LOWORD(Message->lParam), HIWORD(Message->lParam));
			return true;

		case WM_PAINT:
			ValidateRect(Message->hwnd, NULL);
			return true;

		case WM_ACTIVATE:
			if (Message->wParam == WA_INACTIVE)
				Window->WindowLostFocus();
			else
				Window->WindowGainedFocus();
			return true;

		case WM_CLOSE:
			if (MessageBox(Message->hwnd, "Do you really want to exit Zinek Engine ?", "Zinek Engine", MB_ICONQUESTION | MB_YESNO) == IDYES)
				Window->WindowDestroyed();
			return true;

		case WM_DESTROY:
			Window->WindowDestroyed();
			return true;

		default:
			return false;
	}
}

LRESULT CALLBACK Callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_CREATE:
			return 0;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

void ShowWindowError()
{
    char szBuf[256]; 
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

	sprintf_s(szBuf, 256, "Win32API Error no : %d, Error text : \"%s\"", dw, lpMsgBuf); 
	zeError("Win32", szBuf); 
    LocalFree(lpMsgBuf);
}

void ZEWindow::WindowGainedFocus()
{
	zeLog("Window", "Main window gained focus.");
	ZEInputModule* Input = zeInput;
	if (Input != NULL)
		Input->Acquire();
}

void ZEWindow::WindowLostFocus()
{
	zeLog("Window", "Window lost focus.");
	if (zeInput != NULL)
		zeInput->UnAcquire();
}

void ZEWindow::WindowDestroyed()
{
	if (zeCore->GetCoreState() != ZE_CS_SHUTDOWN && zeCore->GetCoreState() != ZE_CS_CRITICALERROR)
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
			zeError("Window", "Wrong Window Type you can not create window with component type. Component windows are provided from the out size of Zinek Engine. Use SetComponentWindow function.");
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
		zeError("Window", "Could not register windows class.");
		ShowWindowError();
		return false;
	}

	ZESystemMessageManager::GetInstance()->RegisterMessageHandler(SystemMessageHandler);

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
	
	SystemMessageHandler->TargetWindow = (HWND)WindowHandle;

	if (!WindowHandle)
	{
		ShowWindowError();
		zeError("Win32Window Module", "Could not create window.");
		return false;
	}
	ShowWindow();

	if (WindowType != ZE_WT_FULLSCREEN)
		SetWindowSize(WindowWidth, WindowHeight);

	return true;
}

bool ZEWindow::DestroyMainWindow()
{
	if (DestroyWindow((HWND)WindowHandle) == 0)
	{
		zeError("Window", "Can not destroy window.");
		ShowWindowError();
		return false;
	}

	ZESystemMessageManager::GetInstance()->RegisterMessageHandler(SystemMessageHandler);
	 
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
	if (zeGraphics != NULL)
		zeGraphics->SetScreenSize(WindowWidth, WindowHeight);

	WindowType = ZE_WT_COMPONENT;
	return true;
}

void ZEWindow::WindowResized(int Width, int Height)
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

	if (zeGraphics != NULL)
		zeGraphics->SetScreenSize(Width, Height);

	WindowWidth = Width;
	WindowHeight = Height;
}

void ZEWindow::SetWindowPosition(int Left, int Top)
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

void ZEWindow::GetWindowPosition(int& Left, int& Top)
{
	RECT Rect;
	GetWindowRect((HWND)WindowHandle, &Rect);
	Left = Rect.left;
	Top = Rect.top;
}

void ZEWindow::SetWindowSize(int Width, int Height)
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

void ZEWindow::GetWindowSize(int& Width, int& Height)
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
	SystemMessageHandler = new ZEWindowSystemMessageHandler();
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
	delete SystemMessageHandler;
	Window = NULL;
}
