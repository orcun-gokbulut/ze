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

#include "ZEGraphics\ZEGRAdapter.h"
#include "ZEGraphics\ZEGROutput.h"
#include "ZEError.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include "ZEWindowResources.h"


static void ManageCursorVisibility(HWND hWnd, LPARAM lParam)
{
	ZEWindow* Window = (ZEWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	static bool CursorHidden = false;
	if (LOWORD(lParam) == HTCLIENT && !Window->GetHideCursor() && GetFocus() == hWnd)
	{
		ShowCursor(false);
	}
	else if (LOWORD(lParam) != HTCLIENT || Window->GetHideCursor() || GetFocus() != hWnd) 
	{
		ShowCursor(true);
	}
}

static void ManageCursorLock(HWND hWnd)
{
	ZEWindow* Window = (ZEWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (Window->GetLockCursor())
	{
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
	else
	{
		ClipCursor(NULL);
	}
}

static DWORD GetWindowStyle(ZEWindow* Window)
{
	DWORD Style = 0;
	if (Window->GetFullscreen())
	{
		Style = WS_POPUP | WS_EX_TOPMOST | WS_VISIBLE | WS_MAXIMIZE;
	}
	else
	{
		if (Window->GetBorderlessMode())
		{
			Style = WS_POPUP;
		}
		else 
		{
			Style |= WS_OVERLAPPED | WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_MINIMIZEBOX;
			if (Window->GetResizable())
				Style |= WS_SIZEBOX | WS_MAXIMIZEBOX;
		}

		if (Window->GetVisible())
			Style |= WS_VISIBLE;
	}

	return Style;
}

static void ShowWindowError()
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

class ZEWindowCallback
{
	public:
		static LRESULT CALLBACK Callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

LRESULT CALLBACK ZEWindowCallback::Callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ZEWindow* Window = (ZEWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	switch (msg)
	{
		case WM_CREATE:
		{
			LPCREATESTRUCT CreateStruct = (LPCREATESTRUCT)lParam;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)CreateStruct->lpCreateParams);
			break;
		}

		case WM_SIZE:
			Window->Width = (ZEUInt)LOWORD(lParam);
			Window->Height = (ZEUInt)HIWORD(lParam);
			if (Window->Output != NULL)
				Window->Output->Resize(Window->Width, Window->Height);
			break;

		case WM_PAINT:
			if (Window->GetOutput() != NULL)
			{
				ValidateRect(hWnd, NULL);
			}
			else
			{
				PAINTSTRUCT ps;
				RECT rc;
				HDC hdc = BeginPaint(hWnd, &ps);
				GetClientRect(hWnd, &rc);
				HBRUSH hBrush = (HBRUSH)GetStockObject(GRAY_BRUSH);
				FillRect(hdc, &rc, hBrush);
				EndPaint(hWnd, &ps);
			}
			break;

		case WM_SETCURSOR:
		{
			WORD Area = LOWORD(lParam);
			LRESULT Result =  DefWindowProc(hWnd, msg, wParam, lParam);
			if (Window->GetHideCursor() && LOWORD(lParam) == HTCLIENT && GetActiveWindow() == hWnd)
				SetCursor(NULL);
			return Result;
		}

		case WM_MOVE:
			Window->Left = (ZEInt)(short)LOWORD(lParam);
			Window->Top = (ZEInt)(short)HIWORD(lParam);
			break;

		case WM_MOVING:
			Window->Left = (ZEInt)(short)LOWORD(lParam);
			Window->Top = (ZEInt)(short)HIWORD(lParam);
			break;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			ManageCursorLock(hWnd);
			break;

		case WM_ACTIVATE:
			if (wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE)
				ManageCursorLock(hWnd);
			else if (wParam == WA_INACTIVE)
				ClipCursor(NULL);
			break;

		case WM_SHOWWINDOW:
			if (wParam == TRUE)
				ManageCursorLock(hWnd);
			else
				ClipCursor(NULL);
			break;

		case WM_CLOSE:
			ClipCursor(NULL);
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

ZEWindow::ZEWindow()
{
	Handle = NULL;
	Output = NULL;

	Title = "Zinek Engine";
	Left = 0;
	Top = 0;
	Width = 640;
	Height = 480;

	Visible = true;
	BorderlessMode = false;
	Resizable = true;
	Fullscreen = false;

	HideCursor = false;
	LockCursor = false;
}

ZEWindow::~ZEWindow()
{

}

void ZEWindow::ChangeWidowGeometry()
{
	if (Handle == NULL || Fullscreen)
		return;

	RECT ClientRectangle, WindowRectangle;
	POINT Difference;
	GetClientRect((HWND)Handle, &ClientRectangle);
	GetWindowRect((HWND)Handle, &WindowRectangle);
	Difference.x = (WindowRectangle.right - WindowRectangle.left) - ClientRectangle.right;
	Difference.y = (WindowRectangle.bottom - WindowRectangle.top) - ClientRectangle.bottom;
	MoveWindow((HWND)Handle, WindowRectangle.left, WindowRectangle.top, Width + Difference.x, Height + Difference.y, TRUE);
}

void ZEWindow::ChangeWindowConfiguration()
{
	if (Handle == NULL)
		return;

	SetWindowLong((HWND)Handle, GWL_STYLE, GetWindowStyle(this));

	if (Output == NULL)
		return;

	if (Fullscreen)
	{
		//MoveWindow((HWND)Handle, 0, 0, MonitorMode->GetWidth(), MonitorMode->GetHeight(), FALSE);
		//Output->SetMonitorMode(MonitorMode);
		Output->SetFullscreen(true);
	}
	else if (!Fullscreen && Output->GetFullscreen())
	{
		Output->SetFullscreen(false);
	}
}

bool ZEWindow::CreateWindow_()
{
	static bool ClassCreated = false;
	if (!ClassCreated)
	{
		WNDCLASSEX wc;
		wc.cbSize			= sizeof(WNDCLASSEX);
		wc.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc		= ZEWindowCallback::Callback;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= (HINSTANCE)GetModuleHandle(NULL);
		wc.hIcon			= NULL;
		wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground	= NULL;
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= "ZEWindow";
		wc.hIconSm			= NULL;

		if(RegisterClassEx(&wc) == 0)
		{
			zeError("Could not register windows class.");
			ShowWindowError();
			return false;
		}

		ClassCreated = true;
	}

	Handle = CreateWindowEx(WS_EX_APPWINDOW, 
        "ZEWindow", 
        Title.ToCString(), 
		GetWindowStyle(this), 
        CW_USEDEFAULT,
		CW_USEDEFAULT, 
		Width,
        Height, 
        NULL, 
        NULL, 
		(HINSTANCE)GetModuleHandle(NULL), 
        this);

	if (!Handle)
	{
		ShowWindowError();
		zeError("Can not create window.");
		return false;
	}

	RECT Rect;
	GetWindowRect((HWND)Handle, &Rect);
	Left = Rect.left;
	Top = Rect.top;
	
	ChangeWidowGeometry();
	SetVisible(GetVisible());
	
	if (Visible)
	{
		ShowWindow((HWND)Handle, SW_SHOW);
		UpdateWindow((HWND)Handle);
	}

	return true;
}

bool ZEWindow::DestroyWindow()
{
	if(!IsWindow((HWND)Handle))
		return false;

	if (::DestroyWindow((HWND)Handle) == 0)
	{
		ShowWindowError();
		zeError("Can not destroy window.");
		return false;
	}
	 
	return true;
}

bool ZEWindow::InitializeSelf()
{
	if (!ZEInitializable::InitializeSelf())
		return false;

	if (!CreateWindow_())
		return false;

	//Output = ZEGROutput::Create(this, ZEGR_TF_R8G8B8A8_UNORM);

	return true;
}

bool ZEWindow::DeinitializeSelf()
{
	Output = NULL;
	DestroyWindow();

	return ZEInitializable::DeinitializeSelf();
}

void* ZEWindow::GetHandle()
{
	return Handle;
}

ZEGROutput* ZEWindow::GetOutput()
{
	return Output;
}

void ZEWindow::SetTitle(const ZEString& Title)
{
	if (this->Title == Title)
		return;

	this->Title = Title;

	if (Handle == NULL)
		return;

	SetWindowText((HWND)Handle, Title.ToCString());
}

const ZEString& ZEWindow::GetTitle()
{
	return Title;
}

void ZEWindow::SetLeft(ZEInt Left)
{
	if (this->Left == Left)
		return;

	this->Left = Left;

	if (Handle == NULL || Fullscreen)
		return;

	RECT Rect;
	GetWindowRect((HWND)Handle, &Rect);
	MoveWindow((HWND)Handle, Left, Top, Rect.right - Rect.left, Rect.bottom - Rect.top, FALSE);
}

void ZEWindow::SetTop(ZEInt Top)
{
	if (this->Top == Top)
		return;

	this->Top = Top;

	if (Handle == NULL || Fullscreen)
		return;

	RECT Rect;
	GetWindowRect((HWND)Handle, &Rect);
	MoveWindow((HWND)Handle, Left, Top, Rect.right - Rect.left, Rect.bottom - Rect.top, FALSE);
}

void ZEWindow::SetWidth(ZEUInt Width)
{
	if (this->Height == Height)
		return;

	this->Height = Height;

	ChangeWidowGeometry();
}

ZEUInt ZEWindow::GetWidth()
{
	return Width;
}

void ZEWindow::SetHeight(ZEUInt Height)
{
	if (this->Height == Height)
		return;

	this->Height = Height;

	if (Handle == NULL || Fullscreen)
		return;

	ChangeWidowGeometry();
}

ZEUInt ZEWindow::GetHeigth()
{
	return Height;
}

void ZEWindow::SetVisible(bool Visible)
{
	if (this->Visible == Visible)
		return;

	this->Visible = Visible;

	if (Handle == NULL || Fullscreen)
		return;

	::ShowWindow((HWND)Handle, Visible ? SW_SHOWNORMAL : SW_HIDE);
	UpdateWindow((HWND)Handle);
}

bool ZEWindow::GetVisible()
{
	return Visible;
}

void ZEWindow::SetResizable(bool Enabled)
{
	if (Resizable == Enabled)
		return;

	Resizable = Enabled;
	ChangeWindowConfiguration();
}

bool ZEWindow::GetResizable()
{
	return Resizable;
}

void ZEWindow::SetBorderlessMode(bool Enabled)
{
	if (BorderlessMode == Enabled)
		return;

	BorderlessMode = Enabled;
	ChangeWindowConfiguration();
}

bool ZEWindow::GetBorderlessMode()
{
	return BorderlessMode;
}

void ZEWindow::SetFullScreen(bool Enabled)
{
	if (Fullscreen == Enabled)
		return;

	Fullscreen = Enabled;

	ChangeWindowConfiguration();
}

bool ZEWindow::GetFullscreen()
{
	return Fullscreen;
}

ZEVector2 ZEWindow::GetCursorPosition()
{
	POINT CursorPosition;
	GetCursorPos(&CursorPosition);
	
	return ZEVector2((float)(CursorPosition.x - Left), (float)(CursorPosition.y - Top));
}

void ZEWindow::Minimize()
{
	if (!Visible)
		return;

	ShowWindow((HWND)Handle, SW_MINIMIZE);
}

void ZEWindow::Restore()
{
	if (!Visible)
		return;

	ShowWindow((HWND)Handle, SW_NORMAL);
}

void ZEWindow::Maximize()
{
	if (!Visible)
		return;

	if (Fullscreen)
		ShowWindow((HWND)Handle, SW_MAXIMIZE);
	else if (Resizable)
		ShowWindow((HWND)Handle, SW_MAXIMIZE);
	else
		ShowWindow((HWND)Handle, SW_NORMAL);
}

void ZEWindow::SetHideCursor(bool Visibility)
{
	HideCursor = Visibility;
}

bool ZEWindow::GetHideCursor()
{
	return HideCursor;
}

void ZEWindow::SetLockCursor(bool Enabled)
{
	LockCursor = Enabled;
}

bool ZEWindow::GetLockCursor()
{
	return LockCursor;
}

void ZEWindow::Destroy()
{
	delete this;
}

ZEWindow* ZEWindow::CreateInstance()
{
	return new ZEWindow();
}
