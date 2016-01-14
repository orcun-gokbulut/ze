//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRWindow_Windows.cpp
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

#include "ZEGRWindow.h"

#include "ZEGRCursor.h"
#include "ZECore/ZECore.h"
#include "ZEMath/ZEMath.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGROutput.h"

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define ZE_WIN32_ERROR_STRING_LENGTH	1024
#define	ZE_WIN32_APP_WINDOW_CLASS_NAME	"ZE_APP_WINDOW_CLASS"

static HCURSOR SNSizeIcon = LoadCursor(NULL, IDC_SIZENS);
static HCURSOR WESizeIcon = LoadCursor(NULL, IDC_SIZEWE);
static HCURSOR NESWSizeIcon = LoadCursor(NULL, IDC_SIZENESW);
static HCURSOR NWSESizeIcon = LoadCursor(NULL, IDC_SIZENWSE);
static HCURSOR DefaultCursor = LoadCursor(NULL, IDC_ARROW);

static void HandleWin32Error(DWORD ErrorCode)
{
	char ErrorString[ZE_WIN32_ERROR_STRING_LENGTH] = {0};

	DWORD Flags = FORMAT_MESSAGE_FROM_SYSTEM;
	DWORD Result = ::FormatMessage(Flags, NULL, ErrorCode, 0, ErrorString, ZE_WIN32_ERROR_STRING_LENGTH, NULL);
	if (Result == 0)
	{
		zeError("Unknwon Win32 error with code %u.", ErrorCode);
		return;
	}

	zeError("%s", ErrorString);
}

static bool ExitValidation(ZEGRWindow* Window)
{
	HWND Handle = (HWND)Window->GetHandle();
	int Result = ::MessageBox(Handle, "Do you really want to exit Zinek Engine ?", "Zinek Engine", MB_ICONQUESTION | MB_YESNO);
	
	if (Result == IDYES)
	{
		DestroyWindow(Handle);
		return true;
	}

	return false;
}

static void ManageCursorLock(ZEGRWindow* Window, bool Focused)
{	
	if (!Window->GetCursorLock())
		return;

	if (Focused)
	{
		ZERectangle Rect = Window->GetRectangle();
		ZEGRCursor::GetInstance()->SetLockRectangle(&Rect);
	}
	else
	{
		ZEGRCursor::GetInstance()->SetLockRectangle(NULL);
	}
}

HBRUSH DefaultBackGround = CreateSolidBrush(RGB(128, 128, 128));

static void SetWindowUserData(HWND Handle, void* DataPtr)
{
	LONG_PTR Result = SetWindowLongPtr(Handle, GWLP_USERDATA, (LONG_PTR)DataPtr);
}

static void GetWindowUserData(HWND Handle, void** DataPtr)
{	
	*DataPtr = (void*)GetWindowLongPtr(Handle, GWLP_USERDATA);
}

static bool ConvertClientToWindowSize(ZEUInt Style, ZEUInt StyleExt, ZEInt ClientX, ZEInt ClientY, ZEInt ClientWidth, ZEInt ClientHeight, 
									  ZEInt& WindowX, ZEInt& WindowY, ZEInt& WindowWidth, ZEInt& WindowHeight)
{
	RECT Viewport = {ClientX, ClientY, ClientX + ClientWidth, ClientY + ClientHeight};

	BOOL Result = ::AdjustWindowRectEx(&Viewport, Style, FALSE, StyleExt);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	WindowX = Viewport.left;
	WindowY = Viewport.top;
	WindowHeight = Viewport.bottom - Viewport.top;
	WindowWidth = Viewport.right -  Viewport.left;

	return true;
}

static void GetWin32Style(const ZEGRWindowStyle& Style, DWORD& Win32StyleExt, DWORD& Win32Style)
{
	if (Style.Type == ZEGR_WT_NORMAL)
		Win32Style |= WS_OVERLAPPED;
	else if (Style.Type == ZEGR_WT_NORMAL)
		Win32Style |= WS_POPUP;

	Win32Style |= 
		Win32Style |= Style.TitleBar ? WS_CAPTION : 0;
	Win32Style |= Style.TitleBar ? WS_SYSMENU : 0;
	Win32Style |= Style.Resizable ? WS_SIZEBOX : 0;
	Win32Style |= Style.MinimizeButton ? WS_MINIMIZEBOX : 0;
	Win32Style |= Style.MaximizeButton ? WS_MAXIMIZEBOX : 0;
	Win32Style |= Style.Bordered ? WS_BORDER : 0;
	Win32StyleExt |= Style.ShowInTaskbar ? WS_EX_APPWINDOW : 0;
	Win32StyleExt |= Style.AlwaysOnTop ? WS_EX_TOPMOST : 0;	
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ZEGRWindow* Window = NULL;

	if (uMsg == WM_CREATE)
	{
		LPCREATESTRUCT CreationParams = (LPCREATESTRUCT)lParam;
		Window = (ZEGRWindow*)CreationParams->lpCreateParams;

		SetWindowUserData(hwnd, (void*)Window);
	}
	else if (uMsg == WM_QUIT)
	{
		SetWindowUserData(hwnd, NULL);
	}
	else
	{
		GetWindowUserData(hwnd, (void**)&Window);
	}

	// Will skip the messages until WM_CREATE message comes
	return Window == NULL ? DefWindowProc(hwnd, uMsg, wParam, lParam) : 
		Window->HandleMessage(uMsg, wParam, lParam);						
}

static bool RegisterWindowClass(HINSTANCE Instance)
{
	if (ZEGRWindow::GetWindowCount() > 0)
		return true;

	WNDCLASSEX WindowClass;
	WindowClass.cbSize			= sizeof(WNDCLASSEX);
	WindowClass.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc		= &WindowProc;
	WindowClass.cbClsExtra		= 0;
	WindowClass.cbWndExtra		= 0;
	WindowClass.hInstance		= Instance;
	WindowClass.hIcon			= NULL;
	WindowClass.hCursor			= DefaultCursor;
	WindowClass.hbrBackground	= DefaultBackGround;
	WindowClass.lpszMenuName	= NULL;
	WindowClass.lpszClassName	= ZE_WIN32_APP_WINDOW_CLASS_NAME;
	WindowClass.hIconSm			= NULL;

	BOOL Result = RegisterClassEx(&WindowClass);
	if(Result == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	return true;
}

static bool UnRegisterWindowClass(HINSTANCE Instance)
{
	if (ZEGRWindow::GetWindowCount() > 0)
		return true;

	BOOL Result = UnregisterClass(ZE_WIN32_APP_WINDOW_CLASS_NAME, Instance);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	return true;
}

ZEGRWindowStyle::ZEGRWindowStyle()
{
	Type = ZEGR_WT_NORMAL;
	TitleBar = true;
	TitleBarIcon = true;
	MinimizeButton = true;
	MaximizeButton = true;
	Resizable = true;
	Bordered = true;
	ShowInTaskbar = false;
	AlwaysOnTop = false;
}

void ZEGRWindow::SetStyle(const ZEGRWindowStyle& Style)
{
	this->Style = Style;

	if (!IsInitialized())
		return;

	DWORD Win32Style = 0;
	DWORD Win32StyleExt = 0;
	GetWin32Style(Style, Win32StyleExt, Win32Style);

	SetWindowLongPtr((HWND)Handle, GWL_STYLE, Win32Style);
	SetWindowLongPtr((HWND)Handle, GWL_EXSTYLE, Win32StyleExt);
}

void ZEGRWindow::SetTitle(const ZEString& Title)
{
	this->Title = Title;

	if (!IsInitialized())
		return;

	BOOL Result = ::SetWindowText((HWND)Handle, Title);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return;
	}
}

void ZEGRWindow::SetPosition(ZEInt X, ZEInt Y)
{
	PositionX = X;
	PositionY = Y;

	if (!IsInitialized())
		return;

	WINDOWINFO WindowInfo = {0};
	WindowInfo.cbSize = sizeof(WINDOWINFO);
	BOOL Result = ::GetWindowInfo((HWND)Handle, &WindowInfo);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return;
	}

	ZEInt WindowPosX, WindowPosY;
	ZEInt WindowWidth, WindowHeight;
	ConvertClientToWindowSize(	WindowInfo.dwStyle,
								WindowInfo.dwExStyle,
								X,
								Y,
								Width,
								Height,
								WindowPosX,
								WindowPosY,
								WindowWidth,
								WindowHeight);
	
	Result = ::MoveWindow(	(HWND)Handle, 
							WindowPosX, 
							WindowPosY, 
							WindowWidth, 
							WindowHeight, 
							TRUE);
	if (Result == 0)
		HandleWin32Error(GetLastError());
}

void ZEGRWindow::SetSize(ZEInt Width, ZEInt Height)
{
	this->Width = Width;
	this->Height = Height;

	if (!IsInitialized())
		return;

	WINDOWINFO WindowInfo = {0};
	WindowInfo.cbSize = sizeof(WINDOWINFO);
	BOOL Result = ::GetWindowInfo((HWND)Handle, &WindowInfo);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
	}

	ZEInt WindowPosX, WindowPosY;
	ZEInt WindowWidth, WindowHeight;
	ConvertClientToWindowSize(	WindowInfo.dwStyle,
								WindowInfo.dwExStyle,
								PositionX,
								PositionY,
								Width,
								Height,
								WindowPosX,
								WindowPosY,
								WindowWidth,
								WindowHeight);
	
	Result = ::MoveWindow(	(HWND)Handle,
							WindowPosX,
							WindowPosY,
							WindowWidth,
							WindowHeight,
							TRUE);
	if (Result == 0)
		HandleWin32Error(GetLastError());
}

void ZEGRWindow::SetFullScreen(bool FullScreen)
{
	if (this->FullScreen == FullScreen)
		return;

	this->FullScreen = FullScreen;
}

void ZEGRWindow::SetVSynchEnable(bool VSynchEnable)
{
	this->VSynchEnable = VSynchEnable;
}

void ZEGRWindow::SetEnable(bool Enabled)
{
	this->Enabled = Enabled;

	if (!IsInitialized())
		return;

	::EnableWindow((HWND)Handle, (BOOL)Enabled);
}

void ZEGRWindow::Focus()
{
	Focused = true;

	if (!IsInitialized())
		return;

	::SetFocus((HWND)Handle);
}

void ZEGRWindow::Maximize()
{
	Maximized = true;
	Minimized = false;
	FullScreen = false;

	if (!IsInitialized())
		return;

	::ShowWindow((HWND)Handle, SW_SHOWMAXIMIZED);
	::UpdateWindow((HWND)Handle);
}

void ZEGRWindow::Minimize()
{
	Minimized = true;
	Maximized = false;
	FullScreen = false;

	if (!IsInitialized())
		return;

	BOOL Result =  CloseWindow((HWND)Handle);
	if (Result == 0)
		HandleWin32Error(GetLastError());
}

void ZEGRWindow::Restore()
{
	Minimized = false;
	Maximized = false;
	FullScreen = false;

	if (!IsInitialized())
		return;

	BOOL Result =  OpenIcon((HWND)Handle);
	if (Result == 0)
		HandleWin32Error(GetLastError());
}

void ZEGRWindow::SetCursorLock(bool CursorLock)
{
	ZERectangle ClientRect = GetRectangle();

	bool Result = ZEGRCursor::GetInstance()->SetLockRectangle(CursorLock ? &ClientRect : NULL);
	if (!Result)
		return;

	LastCursorLock = CursorLock ? this : NULL;
}

ZESSize ZEGRWindow::HandleMessage(ZEUInt32 Message, ZESize wParam, ZESSize lParam)
{
	ZESSize Return = 0;

	switch (Message)
	{
		// Window notification
		case WM_MOVE:
		{
			PositionX = (short)LOWORD(lParam);
			PositionY = (short)HIWORD(lParam);
			OnMove();

			break;
		}

		// Window notification
		case WM_SIZE:
		{
			ZEInt TempWidth =  (short)LOWORD(lParam);
			ZEInt TempHeight = (short)HIWORD(lParam);

			if (TempWidth == 0 || TempHeight == 0)
				break;

			Width =  TempWidth;
			Height = TempHeight;

			Output->Resize(Width, Height);
			OnSize();

			switch (wParam)
			{
				case SIZE_MAXIMIZED:
					Minimized = false;
					Maximized = true;
					OnMaximize();
					break;

				case SIZE_MINIMIZED:
					Minimized = true;
					Maximized = false;
					OnMinimize();
					break;

				case SIZE_RESTORED:
					Minimized = false;
					Maximized = false;
					OnRestore();
					break;
			};

			break;
		}

		case WM_MBUTTONDOWN:
			ManageCursorLock(this, false);
			break;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			ManageCursorLock(this, true);
			break;

		// Window notification
		case WM_ENABLE:
			if (wParam)
			{
				Enabled = true;
				OnEnabled();
			}
			else
			{
				Enabled = false;
				ManageCursorLock(this, false);
				OnDisabled();
			}
			break;

		// Paint/display message
		case WM_PAINT:
			ValidateRect((HWND)Handle, NULL);
			break;
			
		// Cursor notification
		case WM_SETCURSOR:
		{
			ZEInt32 HitCode = (ZEInt32)(short)LOWORD(lParam);
			
			switch (HitCode)
			{
				case HTCLIENT:
					// User defined cursor can be set here
					SetCursor(DefaultCursor);
					break;

				case HTTOP:
				case HTBOTTOM:
					SetCursor(SNSizeIcon);
					break;

				case HTLEFT:
				case HTRIGHT:
					SetCursor(WESizeIcon);
					break;

				case HTTOPRIGHT:
				case HTBOTTOMLEFT:
					SetCursor(NESWSizeIcon);
					break;

				case HTTOPLEFT:
				case HTBOTTOMRIGHT:
					SetCursor(NWSESizeIcon);
					break;

				default:
					SetCursor(DefaultCursor);
					break;
			}
			break;
		}

		// keyboard notification
		case WM_ACTIVATE:
		{
			ZEInt HighOrder = HIWORD(wParam);
			ZEInt LowOrder = LOWORD(wParam);
			switch(LowOrder)
			{
				case WA_ACTIVE:
				case WA_CLICKACTIVE:
					Focused = true;
					OnFocusGain();
					break;

				case WA_INACTIVE:
					Focused = false;
					ManageCursorLock(this, false);
					OnFocusLoose();
					break;
			};

			break;
		}

		// keyboard notification
		case WM_SYSKEYDOWN:
		{
			ZEInt HighOrder = HIWORD(lParam);
			if (HighOrder & KF_ALTDOWN)
			{
				switch (wParam)
				{
					case VK_RETURN:
						if (FullScreen)
						{
							SetFullScreen(false);
							OnWindowed();
						}
						else
						{
							SetFullScreen(true);
							OnFullScreen();
						}
						break;

					case VK_F4:
						ExitValidation(this);
						break;
				};
			}
			break;
		}

		// Window notification
		case WM_CLOSE:
			ExitValidation(this);
			break;

		// Window notification
		case WM_CREATE:
			OnCreate();
			break;
		
		// Window notification
		case WM_DESTROY:
			OnDestroy();
			PostQuitMessage(EXIT_SUCCESS);
			break;
		
		default:
			Return = DefWindowProc((HWND)Handle, Message, wParam, lParam);
			break;
	};

	return Return;
}

void ZEGRWindow::Destroy()
{
	delete this;
}

bool ZEGRWindow::InitializeSelf()
{
	DWORD Win32Style = 0;
	DWORD Win32StyleExt = 0;
	GetWin32Style(Style, Win32StyleExt, Win32Style);
	
	Win32Style |= !Enabled ? WS_DISABLED : 0;
	Win32Style |= Maximized ? WS_MAXIMIZE : 0;
	Win32Style |= Minimized ? WS_MINIMIZE : 0;

	HINSTANCE Instance = (HINSTANCE)zeCore->GetApplicationInstance();
	
	if (!RegisterWindowClass(Instance))
	{
		zeError("Cannot register class.");
		return false;
	}
	
	ZEInt WindowPosX, WindowPosY, WindowWidth, WindowHeight;
	ConvertClientToWindowSize(	Win32Style,
								Win32StyleExt,
								PositionX,
								PositionY,
								Width,
								Height,
								WindowPosX,
								WindowPosY,
								WindowWidth,
								WindowHeight);

	Handle = CreateWindowEx(Win32StyleExt,
							ZE_WIN32_APP_WINDOW_CLASS_NAME,
							Title,
							Win32Style,
							WindowPosX,
							WindowPosY,
							WindowWidth,
							WindowHeight,
							NULL,
							NULL,
							Instance,
							this);

	if (Handle == NULL)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	WindowCount++;

	Output = ZEGROutput::Create(this, ZEGR_TF_R8G8B8A8_UNORM);
	if(Output == NULL)
		return false;

	return true;
}

bool ZEGRWindow::InitializeEmbedded(void* ExistingHandle)
{
	HWND Handle = (HWND)ExistingHandle;

	char TempString[128];
	BOOL CharCount = ::GetWindowText(Handle, TempString, 128);
	if (CharCount == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	LONG_PTR Win32Style = GetWindowLongPtr(Handle, GWL_STYLE);
	if (Win32Style == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	LONG_PTR Win32StyleEx = GetWindowLongPtr(Handle, GWL_EXSTYLE);
	if (Win32StyleEx == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	RECT Rectangle = {0};
	BOOL Result = GetClientRect(Handle, &Rectangle);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	Title = TempString;

	this->Handle = Handle;

	PositionX = Rectangle.left;
	PositionY = Rectangle.top;

	Width = Rectangle.right - Rectangle.left;
	Height = Rectangle.bottom - Rectangle.top;

	Style.Type = (Win32Style & WS_POPUP) == 0 ? ZEGR_WT_NORMAL : ZEGR_WT_POPUP;
	Style.TitleBar = (Win32Style & WS_CAPTION) == 0;
	Style.TitleBarIcon = (Win32Style & WS_SYSMENU) == 0;
	Style.MinimizeButton = (Win32Style & WS_MINIMIZEBOX)  ==  0;
	Style.MaximizeButton = (Win32Style & WS_MAXIMIZEBOX) == 0;
	Style.Resizable = (Win32Style & WS_SIZEBOX) == 0;
	Style.Bordered = (Win32Style & WS_BORDER) == 0;
	Style.ShowInTaskbar =  (Win32StyleEx & WS_EX_APPWINDOW) == 0;	
	Style.AlwaysOnTop =  (Win32StyleEx & WS_EX_TOPMOST) == 0;	

	return true;
}

void ZEGRWindow::DeinitializeSelf()
{
	BOOL Result = DestroyWindow((HWND)Handle);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return;
	}
	
	WindowCount--;

	HINSTANCE Instance = (HINSTANCE)zeCore->GetApplicationInstance();

	if (!UnRegisterWindowClass(Instance))
	{
		zeError("Cannot unregister class.");
		return;
	}
}

void ZEGRWindow::Show()
{
	if(!IsInitialized())
		return;

	::ShowWindow((HWND)Handle, SW_SHOW);
}
