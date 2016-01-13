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

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "ZECore/ZECore.h"
#include "ZEMath/ZEMath.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGROutput.h"

#define ZE_WIN32_ERROR_STRING_LENGHT	1024
#define	ZE_WIN32_APP_WINDOW_CLASS_NAME	"ZE_APP_WINDOW_CLASS"

static void HandleWin32Error(DWORD ErrorCode)
{
	char ErrorString[ZE_WIN32_ERROR_STRING_LENGHT] = {0};

	DWORD Flags = FORMAT_MESSAGE_FROM_SYSTEM;
	DWORD Result = ::FormatMessage(Flags, NULL, ErrorCode, 0, ErrorString, ZE_WIN32_ERROR_STRING_LENGHT, NULL);
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
		PostQuitMessage(EXIT_SUCCESS);
		return false;
	}

	return true;
}

/************************************************************/
/*					ZEGraphicsCursorNew						*/
/************************************************************/
HCURSOR SNSizeIcon = LoadCursor(NULL, IDC_SIZENS);
HCURSOR WESizeIcon = LoadCursor(NULL, IDC_SIZEWE);
HCURSOR NESWSizeIcon = LoadCursor(NULL, IDC_SIZENESW);
HCURSOR NWSESizeIcon = LoadCursor(NULL, IDC_SIZENWSE);
HCURSOR DefaultCursor = LoadCursor(NULL, IDC_ARROW);

bool ZEGraphicsCursor::SetVisible(bool Value)
{
	if (Visible == Value)
		return true;

	if (Value) // Show
	{
		int Result = -1;
		while (Result < 0)
		{
			Result = ShowCursor(true);
		}
	}
	else // Hide
	{
		int Result = 1;
		while (Result >= 0)
		{
			Result = ShowCursor(false);
		}
	}

	Visible = Value;

	return true;
}

bool ZEGraphicsCursor::GetVisible()
{
	return Visible;
}

bool ZEGraphicsCursor::SetPosition(ZEInt PosX, ZEInt PosY)
{
	BOOL Result = ::SetCursorPos(PosX, PosY);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	return true;
}

bool ZEGraphicsCursor::GetPosition(ZEInt& PosX, ZEInt& PosY)
{
	POINT CursorPos;
	
	BOOL Result = GetCursorPos(&CursorPos);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	PosX = CursorPos.x;
	PosY = CursorPos.y;

	return true;
}

bool ZEGraphicsCursor::GetPosition(ZEGRWindow* Window, ZEInt& PosX, ZEInt& PosY)
{
	zeDebugCheck(Window == NULL, "NULL Pointer.");
	zeDebugCheck(Window->GetHandle() == NULL, "Window is not creted.");

	ZEInt ClientX, ClientY;
	Window->GetPosition(ClientX, ClientY);

	ZEInt CursorX, CursorY;
	GetPosition(CursorX, CursorY);

	PosX = ClientX - (ZEInt)CursorX;
	PosY = ClientY - (ZEInt)CursorY;

	return true;
}

bool ZEGraphicsCursor::CollisionCheck(const ZERectangle& Rectangle)
{
	ZEInt CursorX, CursorY;
	GetPosition(CursorX, CursorY);

	ZEInt Left = (ZEInt)(Rectangle.LeftUp.x + 0.5f);
	ZEInt Top = (ZEInt)(Rectangle.LeftUp.y + 0.5f);
	ZEInt Right = (ZEInt)(Rectangle.RightDown.x + 0.5f);
	ZEInt Bottom = (ZEInt)(Rectangle.RightDown.y + 0.5f);

	bool VerticalCheck = (CursorY >= Top) && (CursorY < Bottom);
	bool HorizontalCheck = (CursorX >= Left) && (CursorX < Right);

	return VerticalCheck && HorizontalCheck;
}

bool ZEGraphicsCursor::CollisionCheck(const ZEGRWindow* Window)
{
	POINT Pt;
	GetPosition((ZEInt&)Pt.x, (ZEInt&)Pt.y);
	
	return Window->GetHandle() == WindowFromPoint(Pt);
}

bool ZEGraphicsCursor::GetLocked()
{
	return Locked;
}

bool ZEGraphicsCursor::SetLockRectangle(const ZERectangle* Rectangle)
{
	RECT ClientRect = {0};
	RECT* FinalRectangle = NULL;
	if (Rectangle != NULL)
	{
		ClientRect.left = (ZEInt)(Rectangle->LeftUp.x + 0.5f);
		ClientRect.top = (ZEInt)(Rectangle->LeftUp.y + 0.5f);
		ClientRect.right = (ZEInt)(Rectangle->RightDown.x + 0.5f);
		ClientRect.bottom = (ZEInt)(Rectangle->RightDown.y + 0.5f);

		FinalRectangle = &ClientRect;
	}
	else
	{
		FinalRectangle = NULL;
	}

	BOOL Result = ClipCursor(FinalRectangle);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	Locked = Result != 0;

	return true;
}

bool ZEGraphicsCursor::GetLockRectangle(ZERectangle& Rectangle)
{
	RECT ClipRect = {0};
	
	BOOL Result = GetClipCursor(&ClipRect);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	Rectangle.LeftUp.x = (float)ClipRect.left;
	Rectangle.LeftUp.y = (float)ClipRect.top;
	Rectangle.RightDown.x = (float)ClipRect.right;
	Rectangle.RightDown.y = (float)ClipRect.bottom;

	return true;
}

bool ZEGraphicsCursor::Visible = true;
bool ZEGraphicsCursor::Locked = false;

ZEGraphicsCursor::ZEGraphicsCursor()
{
}

ZEGraphicsCursor::~ZEGraphicsCursor()
{
}

/************************************************************/
/*					ZEGraphicsWindowNew						*/
/************************************************************/
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

static void GetWin32Style(const ZEWindowStyle& Style, DWORD& Win32StyleExt, DWORD& Win32Style)
{
	switch (Style.Type)
	{
		case ZE_GWT_CAPTION:
			Win32Style |= WS_CAPTION | WS_SYSMENU;
			Win32Style |= Style.Caption.Resizable ? WS_SIZEBOX : 0;
			Win32Style |= Style.Caption.Maximizable ? WS_MAXIMIZEBOX : 0;
			Win32Style |= Style.Caption.Minimizable ? WS_MINIMIZEBOX : 0;
			
			Win32StyleExt |= WS_EX_APPWINDOW;
			Win32StyleExt |= Style.Caption.OnTop ? WS_EX_TOPMOST : 0;
			
			break;
		case ZE_GWT_POPUP:
			Win32Style |= WS_POPUP;
			Win32Style |= Style.Popup.Bordered ? WS_BORDER : 0;
			Win32Style |= Style.Popup.Resizable ? WS_SIZEBOX : 0;

			Win32StyleExt |= WS_EX_APPWINDOW;
			Win32StyleExt |= Style.Popup.OnTop ? WS_EX_TOPMOST : 0;
			break;

		case ZE_GWT_UNKNOWN:
			Win32Style = Style.Unknown.Properties0;
			Win32StyleExt = Style.Unknown.Properties1;
			break;
	};
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

bool ZEGRWindow::SetStyle(const ZEWindowStyle& Style)
{
	if (!IsInitialized())
	{
		this->Style = Style;
		return false;
	}

	DWORD Win32Style = 0;
	DWORD Win32StyleExt = 0;
	GetWin32Style(Style, Win32StyleExt, Win32Style);

	SetWindowLongPtr((HWND)Handle, GWL_STYLE, Win32Style);
	SetWindowLongPtr((HWND)Handle, GWL_EXSTYLE, Win32StyleExt);

	return true;
}

bool ZEGRWindow::SetTitle(const ZEString& Title)
{
	if (!IsInitialized())
	{
		this->Title = Title;
		return false;
	}

	BOOL Result = ::SetWindowText((HWND)Handle, Title);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	this->Title = Title;
	return true;
}

bool ZEGRWindow::SetPosition(ZEInt X, ZEInt Y)
{
	if (!IsInitialized())
	{
		PositionX = X;
		PositionY = Y;
		return false;
	}

	WINDOWINFO WindowInfo;
	BOOL Result = ::GetWindowInfo((HWND)Handle, &WindowInfo);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
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
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	return true;
}

bool ZEGRWindow::SetSize(ZEInt Width, ZEInt Height)
{
	if (!IsInitialized())
	{
		this->Width = Width;
		this->Height = Height;
		return false;
	}
	
	WINDOWINFO WindowInfo = {0};
	BOOL Result = ::GetWindowInfo((HWND)Handle, &WindowInfo);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
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
	{
		HandleWin32Error(GetLastError());
		return false;
	}
	
	return true;
}

bool ZEGRWindow::SetFullScreen(bool FullScreen)
{
	if (this->FullScreen == FullScreen)
		return true;

	this->FullScreen = FullScreen;

	if (FullScreen) // Going full screen
	{
		FlagLock.Lock();
		Flags.RaiseFlags(ZE_GWF_FULLSCREEN);
		FlagLock.Unlock();
	}
	else // Going windowed
	{
		FlagLock.Lock();
		Flags.RaiseFlags(ZE_GWF_WINDOWED);
		FlagLock.Unlock();
	}

	return IsInitialized();
}

bool ZEGRWindow::SetVSynchEnable(bool VSynchEnable)
{
	this->VSynchEnable = VSynchEnable;

	return IsInitialized();
}

void ZEGRWindow::SetEnable(bool Enable)
{
	::EnableWindow((HWND)Handle, Enable);
}

void ZEGRWindow::Focus()
{
	if (!IsInitialized())
	{
		Focused = true;
		return;
	}

	::SetFocus((HWND)Handle);
}

bool ZEGRWindow::Maximize()
{
	if (!IsInitialized())
	{
		Maximized = true;
		Minimized = false;
		return false;
	}
	
	::ShowWindow((HWND)Handle, SW_SHOWMAXIMIZED);
	::UpdateWindow((HWND)Handle);

	return true;
}

bool ZEGRWindow::Minimize()
{
	if (!IsInitialized())
	{
		Maximized = false;
		Minimized = true;
		return false;
	}

	BOOL Result =  CloseWindow((HWND)Handle);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	return true;
}

bool ZEGRWindow::Restore()
{
	if (!IsInitialized())
	{
		Minimized = false;
		Maximized = false;
		return false;
	}

	BOOL Result =  OpenIcon((HWND)Handle);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	return true;
}

static void ManageCursorLock(ZEGRWindow* Window, bool Focused)
{	
	if (!Window->GetCursorLock())
		return;

	if (Focused)
	{
		ZERectangle Rect;
		Window->GetRectangle(Rect);
		ZEGraphicsCursor::SetLockRectangle(&Rect);
	}
	else
	{
		ZEGraphicsCursor::SetLockRectangle(NULL);
	}
}

bool ZEGRWindow::SetCursorLock(bool Value)
{
	ZERectangle ClientRect;
	GetRectangle(ClientRect);

	bool Result = ZEGraphicsCursor::SetLockRectangle(Value ? &ClientRect : NULL);
	if (!Result)
	{
		return false;
	}

	LastCursorLock = Value ? this : NULL;
	return true;
}

ZESSize ZEGRWindow::HandleMessage(ZEUInt32 Message, ZESize wParam, ZESSize lParam)
{
	ZESSize Return = 0;

	switch (Message)
	{
		// window notification
		case WM_MOVE:
		{
			PositionX = (short)LOWORD(lParam);
			PositionY = (short)HIWORD(lParam);
			OnMove();

			break;
		}

		// window notification
		case WM_SIZE:
		{
			ZEInt TempWidth =  (short)LOWORD(lParam);
			ZEInt TempHeight = (short)HIWORD(lParam);

			if (TempWidth == 0 || TempHeight == 0)
				break;

			Width =  TempWidth;
			Height = TempHeight;

			FlagLock.Lock();
			Flags.RaiseFlags(ZE_GWF_RESIZED);
			FlagLock.Unlock();

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

		// window notification
		case WM_ENABLE:
			if (wParam)
			{
				Enabled = true;
				OnEnable();
			}
			else
			{
				Enabled = false;
				ManageCursorLock(this, false);
				OnDisable();
			}
			break;

		// paint/display message
		case WM_PAINT:
			ValidateRect((HWND)Handle, NULL);
			break;
			
		// cursor notification
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

		// window notification
		case WM_CLOSE:
			ExitValidation(this);
			break;

		// window notification
		case WM_CREATE:
			FlagLock.Lock();
			Flags.RaiseFlags(ZE_GWF_CREATED);
			FlagLock.Unlock();
			OnCreate();
			break;
		
		// window notification
		case WM_DESTROY:
			FlagLock.Lock();
			Flags.RaiseFlags(ZE_GWF_DESTROYED);
			FlagLock.Unlock();

			OnDestroy();
			break;
		
		default:
			Return = DefWindowProc((HWND)Handle, Message, wParam, lParam);
			break;
	};

	return Return;
}

void ZEGRWindow::Destroy()
{
	DeinitializeSelf();
}

bool ZEGRWindow::InitializeSelf()
{
	DWORD Win32Style = 0;
	DWORD Win32StyleExt = 0;
	GetWin32Style(Style, Win32StyleExt, Win32Style);
		
	//Win32Style |= !Enabled ? WS_DISABLED : 0;
	//Win32Style |= Maximized ? WS_MAXIMIZE : 0;
	//Win32Style |= Minimized ? WS_MINIMIZE : 0;

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

	ShowWindow((HWND)Handle, SW_SHOWNORMAL);

	return true;
}

//bool ZEGRWindow::Initialize(void* Handle)
//{
//	char TempString[128];
//	BOOL CharCount = ::GetWindowText((HWND)Handle, TempString, 128);
//	if (CharCount == 0)
//	{
//		HandleWin32Error(GetLastError());
//		return false;
//	}
//
//	LONG_PTR Win32Style = GetWindowLongPtr((HWND)Handle, GWL_STYLE);
//	if (Win32Style == 0)
//	{
//		HandleWin32Error(GetLastError());
//		return false;
//	}
//
//	LONG_PTR Win32StyleEx = GetWindowLongPtr((HWND)Handle, GWL_EXSTYLE);
//	if (Win32StyleEx == 0)
//	{
//		HandleWin32Error(GetLastError());
//		return false;
//	}
//
//	RECT Rectangle = {0};
//	BOOL Result = GetClientRect((HWND)Handle, &Rectangle);
//	if (Result == 0)
//	{
//		HandleWin32Error(GetLastError());
//		return false;
//	}
//	
//	Title = TempString;
//	
//	this->Handle = Handle;
//	
//	PositionX = Rectangle.left;
//	PositionY = Rectangle.top;
//	
//	Width = Rectangle.right - Rectangle.left;
//	Height = Rectangle.bottom - Rectangle.top;
//
//	Style.Type = ZE_GWT_UNKNOWN;
//	Style.Unknown.Properties0 = (ZEUInt32)Win32Style;
//	Style.Unknown.Properties1 = (ZEUInt32)Win32StyleEx;
//
//	return true;
//}

void ZEGRWindow::DeinitializeSelf()
{
	if (Handle == NULL)
		return;

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

	return;
}

bool ZEGRWindow::Update()
{
	::ShowWindow((HWND)Handle, SW_SHOW);
	::UpdateWindow((HWND)Handle);

	return true;
}
