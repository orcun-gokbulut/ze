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

#include "ZEMath/ZEMath.h"
#include "ZECore/ZECore.h"

#include "ZEGROutput.h"
#include "ZEGRGraphicsModule.h"

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define ZE_WIN32_ERROR_STRING_LENGTH	1024
#define	ZE_WIN32_APP_WINDOW_CLASS_NAME	"ZE_APP_WINDOW_CLASS"

struct ZEGRWindowData
{
	ZEGRWindow* Window;
	HINSTANCE Instance;
	HWND Handle;
};

struct ZEGRWindowMessage
{
	HWND hwnd;
	UINT uMsg;
	WPARAM wParam;
	LPARAM lParam;
	LRESULT Return;
};

static HCURSOR SNSizeIcon = LoadCursor(NULL, IDC_SIZENS);
static HCURSOR WESizeIcon = LoadCursor(NULL, IDC_SIZEWE);
static HCURSOR NESWSizeIcon = LoadCursor(NULL, IDC_SIZENESW);
static HCURSOR NWSESizeIcon = LoadCursor(NULL, IDC_SIZENWSE);
static HCURSOR DefaultCursor = LoadCursor(NULL, IDC_ARROW);
static HICON DefaultIcon = LoadIcon(NULL, IDI_APPLICATION);
static HBRUSH DefaultBackGround = CreateSolidBrush(RGB(128, 128, 128));

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

	if (!Window->GetValidateQuit())
	{
		DestroyWindow(Handle);
		return true;
	}

	int Result = ::MessageBox(Handle, "Do you really want to exit Zinek Engine ?", "Zinek Engine", MB_ICONQUESTION | MB_YESNO);
	if (Result == IDYES)
	{
		DestroyWindow(Handle);
		return true;
	}

	return false;
}

static void ManageCursorLock(ZEGRWindow* Window, bool Lock)
{	
	if (!Lock || !Window->GetCursorLocked())
	{
		ClipCursor(NULL);
		return;
	}

	RECT Rect;
	GetClientRect((HWND)Window->GetHandle(), &Rect);

	RECT ScreenRect;
	POINT Point;

	Point.x = Rect.left; 
	Point.y = Rect.top;
	ClientToScreen((HWND)Window->GetHandle(), &Point);
	ScreenRect.left = Point.x; 
	ScreenRect.top = Point.y;

	Point.x = Rect.right; 
	Point.y = Rect.bottom;
	ClientToScreen((HWND)Window->GetHandle(), &Point);
	ScreenRect.right = Point.x; 
	ScreenRect.bottom = Point.y;

	ClipCursor(&ScreenRect);
}

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

static void GetWin32Style(ZEGRWindow* Window, DWORD& Win32StyleExt, DWORD& Win32Style)
{
	Win32Style = 0;
	Win32StyleExt = 0;

	Win32Style |= !Window->GetTitleBar() && !Window->GetFullScreen() ? WS_POPUP : 0;
	Win32Style |= Window->GetTitleBar() ? WS_OVERLAPPED | WS_CAPTION : 0;
	Win32Style |= Window->GetTitleBar() && Window->GetIconVisible() && Window->GetCloseButton() ? WS_SYSMENU : 0;
	Win32Style |= Window->GetTitleBar() && Window->GetMinimizeButton() ? WS_MINIMIZEBOX : 0;
	Win32Style |= Window->GetTitleBar() && Window->GetMaximizeButton() ?  WS_MAXIMIZEBOX : 0;
	Win32Style |= Window->GetResizable() ? WS_SIZEBOX : 0;
	Win32Style |= Window->GetBordered() ? WS_BORDER : 0;
	Win32StyleExt |= Window->GetShowInTaskbar() ? WS_EX_APPWINDOW : 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ZEGRWindowData* Data = NULL;
	if (uMsg == WM_CREATE)
	{
		LPCREATESTRUCT CreationParams = (LPCREATESTRUCT)lParam;
		Data = (ZEGRWindowData*)CreationParams->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(CreationParams->lpCreateParams));
	}
	else if (uMsg == WM_QUIT)
	{
		SetWindowLongPtr(hwnd, GWLP_USERDATA, NULL);
	}
	else
	{
		Data = (ZEGRWindowData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}

	// IMPORTANT !!! Route all messages to DefWindowProc until WM_CREATE message received
	if (Data == NULL)
		return DefWindowProc(hwnd, uMsg, wParam, lParam);

	ZEGRWindowMessage Message;
	Message.hwnd = hwnd;
	Message.uMsg = uMsg;
	Message.wParam = wParam;
	Message.lParam = lParam;
	Data->Window->HandleMessage(&Message);
	return Message.Return;
}

static bool RegisterWindowClass(HINSTANCE Instance)
{
	if (ZEGRWindow::GetWindowCount() > 0)
		return true;

	WNDCLASSEX WindowClass = {};
	WindowClass.cbSize			= sizeof(WNDCLASSEX);
	WindowClass.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc		= &WindowProc;
	WindowClass.cbClsExtra		= 0;
	WindowClass.cbWndExtra		= 0;
	WindowClass.hInstance		= Instance;
	WindowClass.hIcon			= DefaultIcon;
	WindowClass.hCursor			= DefaultCursor;
	WindowClass.hbrBackground	= DefaultBackGround;
	WindowClass.lpszMenuName	= NULL;
	WindowClass.lpszClassName	= ZE_WIN32_APP_WINDOW_CLASS_NAME;
	WindowClass.hIconSm			= NULL;

	BOOL Result = RegisterClassEx(&WindowClass);
	if (Result == 0)
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

void* ZEGRWindow::GetHandle() const
{
	if (Data == NULL)
		return NULL;

	return Data->Handle;
}

void ZEGRWindow::UpdateStyle()
{
	if (!IsInitialized())
		return;

	DWORD Win32Style = 0;
	DWORD Win32StyleExt = 0;
	GetWin32Style(this, Win32StyleExt, Win32Style);

	SetWindowLongPtr(Data->Handle, GWL_STYLE, Win32Style);
	SetWindowLongPtr(Data->Handle, GWL_EXSTYLE, Win32StyleExt);
}

void ZEGRWindow::SetTitle(const ZEString& Title)
{
	this->Title = Title;

	if (!IsInitialized())
		return;

	BOOL Result = ::SetWindowText(Data->Handle, Title);
	if (Result == 0)
		HandleWin32Error(GetLastError());
}

void ZEGRWindow::SetPosition(ZEInt X, ZEInt Y)
{
	Left = X;
	Top = Y;

	if (!IsInitialized())
		return;

	WINDOWINFO WindowInfo = {0};
	WindowInfo.cbSize = sizeof(WINDOWINFO);
	BOOL Result = ::GetWindowInfo(Data->Handle, &WindowInfo);
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
	
	Result = ::MoveWindow(	Data->Handle, 
							WindowPosX, 
							WindowPosY, 
							WindowWidth, 
							WindowHeight, 
							TRUE);
	if (Result == 0)
		HandleWin32Error(GetLastError());
}

void ZEGRWindow::SetSize(ZEUInt Width, ZEUInt Height)
{
	this->Width = Width;
	this->Height = Height;

	if (!IsInitialized())
		return;

	WINDOWINFO WindowInfo = {0};
	WindowInfo.cbSize = sizeof(WINDOWINFO);
	BOOL Result = ::GetWindowInfo(Data->Handle, &WindowInfo);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
	}

	ZEInt WindowPosX, WindowPosY;
	ZEInt WindowWidth, WindowHeight;
	ConvertClientToWindowSize(	WindowInfo.dwStyle,
								WindowInfo.dwExStyle,
								Left,
								Top,
								Width,
								Height,
								WindowPosX,
								WindowPosY,
								WindowWidth,
								WindowHeight);
	
	Result = ::MoveWindow(	Data->Handle,
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

	if (!IsInitialized())
		return;

	Output->SetFullScreen(FullScreen);

	if (FullScreen)
		OnFullScreen();
	else
		OnWindowed();
}

void ZEGRWindow::SetVSyncEnabled(bool VSyncEnable)
{
	this->VSyncEnabled = VSyncEnable;
}

void ZEGRWindow::SetVisible(bool Visible)
{
	this->Visible = Visible;

	if (!IsInitialized())
		return;

	::ShowWindow(Data->Handle, (Visible ? SW_SHOWNA : SW_HIDE));
}

void ZEGRWindow::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;

	if (!IsInitialized())
		return;

	::EnableWindow(Data->Handle, (BOOL)Enabled);
}

void ZEGRWindow::Focus()
{
	Focused = true;

	if (!IsInitialized())
		return;

	::SetFocus(Data->Handle);
}

void ZEGRWindow::Maximize()
{
	Maximized = true;

	if (!IsInitialized())
		return;

	::ShowWindow(Data->Handle, SW_SHOWMAXIMIZED);
}

void ZEGRWindow::Minimize()
{
	Minimized = true;

	if (!IsInitialized())
		return;

	BOOL Result =  CloseWindow(Data->Handle);
	if (Result == 0)
		HandleWin32Error(GetLastError());
}

void ZEGRWindow::Restore()
{
	Minimized = false;
	Maximized = false;

	if (!IsInitialized())
		return;

	BOOL Result =  OpenIcon(Data->Handle);
	if (Result == 0)
		HandleWin32Error(GetLastError());
}

void ZEGRWindow::SetCursorLocked(bool Enabled)
{
	ManageCursorLock(this, Enabled);
	CursorLocked = Enabled;
}

void ZEGRWindow::HandleMessage(ZEGRWindowMessage* Message)
{
	Message->Return = 0;

	switch (Message->uMsg)
	{
		// Window notification
		case WM_MOVE:
		{
			Left = (short)LOWORD(Message->lParam);
			Top = (short)HIWORD(Message->lParam);
			OnMove();
			break;
		}

		// Window notification
		case WM_SIZE:
		{
			ZEInt NewWidth =  (short)LOWORD(Message->lParam);
			ZEInt NewHeight = (short)HIWORD(Message->lParam);

			if ((NewWidth != 0 && NewHeight != 0) && (Width != NewWidth || Height != NewHeight) && Message->wParam != SIZE_MINIMIZED)
			{
				Width = NewWidth;
				Height = NewHeight;

				if (Output != NULL)
					Output->Resize(Width, Height);
			}

			OnSize();

			switch (Message->wParam)
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
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			ManageCursorLock(this, true);
			break;

		// Window notification
		case WM_ENABLE:
			if (Message->wParam)
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
			ValidateRect(Message->hwnd, NULL);
			break;

		/*case WM_MOUSEMOVE:
			if (!CursorVisible)
			{
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = (HWND)GetHandle();
				TrackMouseEvent(&tme);
			}
			break;

		case WM_MOUSELEAVE:
			break;*/

		// Cursor notification
		case WM_SETCURSOR:
		{
			if (LOWORD(Message->lParam) == HTCLIENT && Focused && Message->hwnd == GetHandle()) 
				SetCursor(CursorVisible ? DefaultCursor : NULL);
			else
				Message->Return = DefWindowProc(Message->hwnd, Message->uMsg, Message->wParam, Message->lParam);
			break;
		}

		// keyboard notification
		case WM_SETFOCUS:
		{
			Focused = true;
			ManageCursorLock(this, GetCursorLocked());
			OnFocusGain();
			break;
		}
		
		case WM_KILLFOCUS:
		{
			Focused = false;
			ManageCursorLock(this, false);			
			OnFocusLoose();
			SetCursor(DefaultCursor);
			break;
		}

		case WM_ACTIVATE:
		{
			ZEInt HighOrder = HIWORD(Message->wParam);
			ZEInt LowOrder = LOWORD(Message->wParam);
			
			switch (LowOrder)
			{
				case WA_ACTIVE:
				case WA_CLICKACTIVE:
					if (Output != NULL && FullScreen && Minimized)
					{
						Restore();
						Output->SetFullScreen(true);
					}

					break;

				case WA_INACTIVE:
					ManageCursorLock(this, false);

					if (Output != NULL && FullScreen && !Minimized)
					{
						Output->SetFullScreen(false);
						Minimize();
					}

					break;
			};

			break;
		}

		// keyboard notification
		case WM_SYSKEYDOWN:
		{
			ZEInt HighOrder = HIWORD(Message->lParam);
			if (HighOrder & KF_ALTDOWN)
			{
				switch (Message->wParam)
				{
					case VK_RETURN:
						SetFullScreen(!GetFullScreen());
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
			Message->hwnd = NULL;
			if (QuitWhenClosed)
				PostQuitMessage(EXIT_SUCCESS);
			break;
		
		default:
			Message->Return = DefWindowProc(Message->hwnd, Message->uMsg, Message->wParam, Message->lParam);
			break;
	};
}

bool ZEGRWindow::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	Data = new ZEGRWindowData();
	Data->Window = this;
	Data->Instance = GetModuleHandle(NULL);
	
	if (!RegisterWindowClass(Data->Instance))
	{
		zeError("Cannot register window class.");
		delete Data;
		Data = NULL;
		return false;
	}

	Data->Handle = CreateWindowEx(
		0, ZE_WIN32_APP_WINDOW_CLASS_NAME, Title, 0,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, Data->Instance,	Data);

	if (Data->Handle == NULL)
	{
		delete Data;
		Data = NULL;
		HandleWin32Error(GetLastError());
		return false;
	}

	WindowCount++;

	DWORD Win32Style = 0;
	DWORD Win32StyleExt = 0;
	GetWin32Style(this, Win32StyleExt, Win32Style);

	ZEInt WindowPosX, WindowPosY, WindowWidth, WindowHeight;
	ConvertClientToWindowSize(
		Win32Style,	Win32StyleExt,
		Left, Top,
		Width, Height,
		WindowPosX,	WindowPosY,
		WindowWidth, WindowHeight);

	::SetWindowLongPtr(Data->Handle, GWL_STYLE, Win32Style);
	::SetWindowLongPtr(Data->Handle, GWL_EXSTYLE, Win32StyleExt);

	int ShowState = !Visible ? SW_HIDE : (Maximized ? SW_SHOWMAXIMIZED : (Minimized ? SW_SHOWMINIMIZED : SW_SHOW));
	::SetWindowPos(Data->Handle, AlwaysOnTop ? HWND_TOPMOST : HWND_TOP, WindowPosX, WindowPosY, WindowWidth, WindowHeight, SWP_FRAMECHANGED | SWP_NOACTIVATE);

	::EnableWindow(Data->Handle, Enabled);
	::ShowWindow(Data->Handle, ShowState);

	Output = ZEGROutput::CreateInstance(this, ZEGR_TF_R8G8B8A8_UNORM_SRGB);
	if (Output == NULL)
	{
		DestroyWindow(Data->Handle);
		delete Data;
		Data = NULL;
		return false;
	}

	Output->SetFullScreen(FullScreen);

	return true;
}

bool ZEGRWindow::DeinitializeInternal()
{
	Output.Release();

	if (Data != NULL)
	{
		BOOL Result = DestroyWindow(Data->Handle);
		if (Result == 0)
		{
			HandleWin32Error(GetLastError());
			return false;
		}

		WindowCount--;
		if (WindowCount == 0)
		{
			if (!UnRegisterWindowClass(Data->Instance))
			{
				zeError("Cannot unregister class.");
				return false;
			}
		}

		delete Data;
		Data = NULL;
	}

	return ZEInitializable::DeinitializeInternal();
}


ZEGRWindow* ZEGRWindow::WrapHandle(void* ExistingHandle)
{
	zeCheckError(ExistingHandle == NULL, NULL, "Cannot wrap window handle. ExistingHandle is NULL.");

	HWND Handle = (HWND)ExistingHandle;
	char TempString[128];
	BOOL CharCount = ::GetWindowText(Handle, TempString, 128);
	if (CharCount == 0)
	{
		HandleWin32Error(GetLastError());
		return NULL;
	}

	LONG_PTR Win32Style = GetWindowLongPtr(Handle, GWL_STYLE);
	LONG_PTR Win32StyleEx = GetWindowLongPtr(Handle, GWL_EXSTYLE);

	RECT Rectangle = {0};
	BOOL Result = GetClientRect(Handle, &Rectangle);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return NULL;
	}

	ZEGRWindow* Window = new ZEGRWindow();

	Window->Data = new ZEGRWindowData();
	Window->Data->Handle = Handle;
	Window->Data->Instance = GetModuleHandle(NULL);
	Window->Data->Window = Window;

	Window->Title = TempString;

	Window->Left = Rectangle.left;
	Window->Top = Rectangle.top;
	Window->Width = Rectangle.right - Rectangle.left;
	Window->Height = Rectangle.bottom - Rectangle.top;

	Window->TitleBar = (Win32Style & WS_CAPTION) == WS_CAPTION;
	Window->IconVisible = (Win32Style & WS_SYSMENU) == WS_SYSMENU;
	Window->MinimizeButton = (Win32Style & WS_MINIMIZEBOX) == WS_MINIMIZEBOX;
	Window->MaximizeButton = (Win32Style & WS_MAXIMIZEBOX) == WS_MAXIMIZEBOX;
	Window->Resizable = (Win32Style & WS_SIZEBOX) == WS_SIZEBOX;
	Window->Bordered = (Win32Style & WS_BORDER) == WS_BORDER;
	Window->ShowInTaskbar = (Win32StyleEx & WS_EX_APPWINDOW) == WS_EX_APPWINDOW;
	Window->AlwaysOnTop = (Win32StyleEx & WS_EX_TOPMOST) == WS_EX_TOPMOST;

	Window->Output = ZEGROutput::CreateInstance(Window, ZEGR_TF_R8G8B8A8_UNORM_SRGB);
	if (Window->Output == NULL)
	{
		delete Window;
		return NULL;
	}

	return Window;
}
