//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIConsole.cpp
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

#include "ZEUIConsole.h"
#include "ZECore\ZECore.h"
#include "ZECore\ZEWindow.h"
#include "ZEFontResourceBitmap.h"
#include "ZEFontResourceDynamic.h"

#define  MOVETIME 0.5f
#define  NOTIFICATIONTIME 5.0f

void ZEUIConsole::ExecuteCommand(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	if (InputField->GetText().GetLength() == 0)
		return;

	Input(InputField->GetText());
	InputField->SetText("");
}
void ZEUIConsole::ExecuteCommandReturn(ZEUIInputKey Key)
{
	ZEUIControl::KeyPressed(Key);
	if (Key != ZE_UI_IKB_RETURN || InputField->GetText().GetLength() == 0)
		return;

	Input(InputField->GetText());
	InputField->SetText("");
}

void ZEUIConsole::Tick(float ElapsedTime)
{
	TotalTime += ElapsedTime;
// CONSOLE SHOW / HIDE
	if(IsMoveUp)
	{
		ZEVector2 UpPos(0.0f, -GetHeight() + NotificationArea->GetHeight());

		if(TotalTime < MOVETIME)
		{
			ZEVector2 NewPos;
			ZEVector2::Lerp(NewPos, MotionStartPosition, UpPos, TotalTime * 2);
			SetPosition(NewPos);
		}
		else
		{
			SetPosition(UpPos);
			IsMoveUp = false;
			IsConsoleShown = false;
			//SetVisiblity(false);
		}
	}
	else if(IsMoveDown)
	{
		ZEVector2 DownPos(0.0f, 0.0f);
		NotificationArea->SetText("");

		if(TotalTime < MOVETIME)
		{
			ZEVector2 NewPos;
			ZEVector2::Lerp(NewPos, MotionStartPosition, DownPos, TotalTime * 2);
			SetPosition(NewPos);
		}
		else
		{
			SetPosition(DownPos);
			IsConsoleShown = true;
			IsMoveDown = false;
		}
	}

// NOTIFICATION AREA
	if (IncomingOutputArrived)
	{
		ZEVector4 InvisibilityColor(0.0f, 1.0f, 0.0f, 0.0f);
		if (TotalTime < NOTIFICATIONTIME)
		{
			ZEVector4 NewColor;
			ZEVector4::Lerp(NewColor, NotificationArea->GetTextColor(), InvisibilityColor, TotalTime / 500);
			NotificationArea->SetTextColor(NewColor);
			NotificationArea->SetText(NotificationArea->GetText());
		}
		else
		{
			NotificationArea->SetTextColor(InvisibilityColor);
			IncomingOutputArrived = false;
			NotificationArea->SetText("");
		}
	}

	ZEUIControl::Tick(ElapsedTime);
}

bool ZEUIConsole::Initialize()
{
	return true;
}

void ZEUIConsole::Deinitialize()
{

}

void ZEUIConsole::ShowConsole()
{
	MotionStartPosition = GetPosition();
	IsMoveDown = true;
	IsMoveUp = false;
	TotalTime = 0.0f;
	SetVisiblity(true);
}

void ZEUIConsole::HideConsole()
{
	MotionStartPosition = GetPosition();
	IsMoveUp = true;
	IsMoveDown = false;
	TotalTime = 0.0f;
}

void ZEUIConsole::EnableInput()
{
	InputField->SetReadOnly(false);
}

void ZEUIConsole::DisableInput()
{
	InputField->SetReadOnly(true);
}

void ZEUIConsole::Output(const char* Output)
{
	OutputField->AppendText(ZEString(Output));
	
	if(strlen(Output) > 2 && IsConsoleShown == false &&  NotificationEnabled == true)
	{
		ZEString OutputString = ZEString(Output);
		OutputString.Remove(OutputString.GetLength() - 2, 2);
		NotificationArea->SetText(OutputString);
		NotificationArea->SetTextColor(ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
		TotalTime = 0.0f;
		IncomingOutputArrived = true;
	}
}

ZEMaterial* ZEUIConsole::GetMaterial() const
{
	return NULL;
}

void ZEUIConsole::SetMaterial(ZEMaterial* Material)
{

}

void ZEUIConsole::SetSize(const ZEVector2& Size)
{
	SetWidth(Size.x);
	SetHeight(Size.y);
}

void ZEUIConsole::SetHeight(float Height)
{
	ZEUIControl::SetHeight(Height);

	OutputField->SetHeight(Height - InputField->GetHeight() - NotificationArea->GetHeight());
	InputField->SetPosition(ZEVector2(InputField->GetPosition().x, OutputField->GetPosition().y + OutputField->GetHeight()));
	ExecuteButton->SetPosition(ZEVector2(ExecuteButton->GetPosition().x, OutputField->GetPosition().y + OutputField->GetHeight()));
	NotificationArea->SetPosition(ZEVector2(NotificationArea->GetPosition().x, InputField->GetPosition().y + InputField->GetHeight()));
}

void ZEUIConsole::SetWidth(float Width)
{
	ZEUIControl::SetWidth(Width);

	OutputField->SetWidth(Width);
	InputField->SetWidth(Width - ExecuteButton->GetWidth());
	ExecuteButton->SetPosition(ZEVector2(InputField->GetPosition().x + InputField->GetWidth(), ExecuteButton->GetPosition().y));
	NotificationArea->SetWidth(InputField->GetWidth());
}

void ZEUIConsole::SetNotificationEnabled(bool Enabled)
{
	NotificationEnabled = Enabled;
}

bool ZEUIConsole::GetNotificationEnabled()
{
	return NotificationEnabled;
}

ZEUIConsole::ZEUIConsole()
{
	OutputField = new ZEUIMultiLineTextEdit();
	InputField = new ZEUITextEditControl();
	ExecuteButton = new ZEUIButtonControl();
	NotificationArea = new ZEUITextControl();

	OutputField->SetReadOnly(false);
	InputField->SetReadOnly(false);
	int WindowWidth, WindowHeight;
	zeCore->GetWindow()->GetWindowSize(WindowWidth, WindowHeight);
	OutputField->SetSize(ZEVector2(200, 100));
	InputField->SetSize(ZEVector2(130, 20));
	ExecuteButton->SetSize(ZEVector2(70, 20));
	NotificationArea->SetSize(ZEVector2(50, 20));
	OutputField->SetPosition(ZEVector2::Zero);
	InputField->SetPosition(ZEVector2(0, 100));
	ExecuteButton->SetPosition(ZEVector2(130, 100));
	NotificationArea->SetPosition(ZEVector2(0, 120));
	SetPosition(ZEVector2::Zero);

	NotificationArea->SetBackgroundColor(ZEVector4(1.0f, 1.0f, 1.0f, 1.0f));
	NotificationArea->SetTextColor(ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	NotificationArea->SetTextWrap(false);
	NotificationArea->SetFontSize(ZEVector2::One);
	NotificationArea->SetFont(ZEFontResourceDynamic::LoadResource("Arial.ttf", 12));
	NotificationArea->SetText("");
	NotificationArea->SetFocusable(false);

	AddChildControl(OutputField);
	AddChildControl(InputField);
	AddChildControl(ExecuteButton);
	AddChildControl(NotificationArea);

	SetSize(ZEVector2(WindowWidth, 400));

	ExecuteButton->SetMouseClickedEvent(BindDelegate(this, &ZEUIConsole::ExecuteCommand));
	InputField->SetKeyPressedEvent(BindDelegate(this, &ZEUIConsole::ExecuteCommandReturn));

	OutputField->SetTextColor(ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
//	OutputField->SetVerticalScrollbarPolicy(SCROLLBAR_AS_NEEDED);
// 	OutputField->SetHorizontalScrollbarPolicy(SCROLLBAR_AS_NEEDED);
	OutputField->SetTextWrap(false);

	ZEString Hist;

	for (size_t I = 0; I < zeCore->GetConsole()->OutputHistory.GetCount(); I++)
	{
		Hist.Append(ZEString(zeCore->GetConsole()->OutputHistory[I]));
	}

	OutputField->SetText(Hist);

	IsMoveDown = false;
	IsMoveUp = false;
	IncomingOutputArrived = false;
	IsConsoleShown = true;
	NotificationEnabled = true;
	TotalTime = 0.0f;
}

ZEUIConsole::~ZEUIConsole()
{

}
