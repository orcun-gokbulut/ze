//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRCursor_Windows.cpp
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

#include "ZEGRCursor.h"

#include "ZEError.h"
#include "ZEGRWindow.h"

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define ZE_WIN32_ERROR_STRING_LENGTH	1024

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

ZEGRCursor::ZEGRCursor()
{
	Visible = true;
	Locked = false;
}

ZEGRCursor::~ZEGRCursor()
{
	SetVisible(true);
	SetLockRectangle(NULL);
}

bool ZEGRCursor::SetVisible(bool Value)
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

bool ZEGRCursor::GetVisible()
{
	return Visible;
}

bool ZEGRCursor::SetPosition(ZEInt PosX, ZEInt PosY)
{
	BOOL Result = ::SetCursorPos(PosX, PosY);
	if (Result == 0)
	{
		HandleWin32Error(GetLastError());
		return false;
	}

	return true;
}

bool ZEGRCursor::GetPosition(ZEInt& PosX, ZEInt& PosY)
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

bool ZEGRCursor::GetPosition(ZEGRWindow* Window, ZEInt& PosX, ZEInt& PosY)
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

bool ZEGRCursor::CollisionCheck(const ZERectangle& Rectangle)
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

bool ZEGRCursor::CollisionCheck(const ZEGRWindow* Window)
{
	POINT Pt;
	GetPosition((ZEInt&)Pt.x, (ZEInt&)Pt.y);
	
	return Window->GetHandle() == WindowFromPoint(Pt);
}

bool ZEGRCursor::GetLocked()
{
	return Locked;
}

bool ZEGRCursor::SetLockRectangle(const ZERectangle* Rectangle)
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

bool ZEGRCursor::GetLockRectangle(ZERectangle& Rectangle)
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

ZEGRCursor*ZEGRCursor::GetInstance()
{
	static ZEGRCursor Cursor;
	return &Cursor;
}
