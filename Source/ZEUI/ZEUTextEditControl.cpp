//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUTextEditControl.cpp
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

#include "ZEUTextEditControl.h"
#include "ZEFontResource.h"
#include "ZEUIRenderer.h"
#include "ZEGraphics/ZEUIMaterial.h"

void ZEUITextEditControl::KeyPressed(unsigned char Key)
{
	ZEString Manupilator = TextDisplayer.GetText();

	if(Key != '\b')
	{
		//Manupilator.Append(tolower(Key));
		TextDisplayer.SetText(Manupilator);
	}
	else if (Key == '\b')
	{	
		if (Manupilator.GetLength() == 0)
			return;

		Manupilator.Remove(Manupilator.GetLength() - 1);
		TextDisplayer.SetText(Manupilator);
	}
}	

void ZEUITextEditControl::Tick(float ElapsedTime)
{
	TotalTime = TotalTime + ElapsedTime;

	Cursor.Positions.LeftUp = ZEVector2(TextDisplayer.Output.Positions.RightDown.x - 1 , TextDisplayer.Output.Positions.RightDown.y - GetHeight());
	Cursor.Positions.RightDown = TextDisplayer.Output.Positions.RightDown;

	if (TotalTime > CursorBlinkTime)
	{
		TotalTime = 0;
		if (CursorVisible == true)
			CursorVisible = false;

		else if (CursorVisible == false)
			CursorVisible= true;
	}
}

void ZEUITextEditControl::Draw(ZEUIRenderer* Renderer)
{
	ZEUIFrameControl::Draw(Renderer);

	if (CursorVisible && TextDisplayer.GetFocused())
		Renderer->AddRectangle(Cursor);
}

void ZEUITextEditControl::SetMaterial(ZEMaterial* Material)
{
	
}

ZEMaterial* ZEUITextEditControl::GetMaterial() const
{
	return NULL;
}

void ZEUITextEditControl::SetWidth(float Width)
{
	ZEUIFrameControl::SetWidth(Width);
	TextDisplayer.SetWidth(Width);
}

void ZEUITextEditControl::SetHeight(float Height)
{
	ZEUIFrameControl::SetHeight(Height);
	TextDisplayer.SetHeight(Height);
}

ZEUITextEditControl::ZEUITextEditControl()
{
	Cursor.Material = ZEUIMaterial::CreateInstance();
	Cursor.Color = ZEVector4::UnitY;

	TextDisplayer.SetSize(ZEVector2(400, 25));
	TextDisplayer.SetBackgroundColor(ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	TextDisplayer.SetTextColor(ZEVector4(1.0f, 1.0f, 1.0f, 1.0f));
	TextDisplayer.SetText("Single Line Text Edit");
	TextDisplayer.SetTextWrap(false);
	TextDisplayer.SetFontSize(ZEVector2::One);
	TextDisplayer.SetFont(ZEFontResource::LoadResource("OldEnglish.zeFont"));

	SetWidth(400);
	SetHeight(24);

	SetPosition(ZEVector2::Zero);
	TextDisplayer.SetPosition(ZEVector2(1,1));

	AddChildControl(&TextDisplayer);

	TextDisplayer.SetFocusable(true);

	TextDisplayer.SetKeyPressedEvent(BindDelegate(this, &ZEUITextEditControl::KeyPressed));

	CursorBlinkTime = 0.5f;
	TotalTime = 0;
	CursorVisible = false;
	Frame.Color = ZEVector4(0.6f, 0.6f, 0.6f, 1.0f);
}
