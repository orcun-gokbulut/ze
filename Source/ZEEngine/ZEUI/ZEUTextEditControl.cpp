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
#include "ZEGraphics/ZETexture2D.h"

void ZEUITextEditControl::KeyPressed(ZEUIInputKey Key)
{
	ZEUIFrameControl::KeyPressed(Key);

	if(Key == ZE_UI_IKB_BACKSPACE)
	{
		if(Text.GetLength() != 0)
		{
			Text.Remove(Text.GetLength() - 1);
			ZEString TempText = Text;
			SetText(TempText);
			return;
		}
	}

	switch(Key)
	{
		case ZE_UI_IKB_1:
			Text.Append("1");
			break;
		case ZE_UI_IKB_2:
			Text.Append("2");
			break;
		case ZE_UI_IKB_3:
			Text.Append("3");
			break;
		case ZE_UI_IKB_4:
			Text.Append("4");
			break;
		case ZE_UI_IKB_5:
			Text.Append("5");
			break;
		case ZE_UI_IKB_6:
			Text.Append("6");
			break;
		case ZE_UI_IKB_7:
			Text.Append("7");
			break;
		case ZE_UI_IKB_8:
			Text.Append("8");
			break;
		case ZE_UI_IKB_9:
			Text.Append("9");
			break;
		case ZE_UI_IKB_0:
			Text.Append("0");
			break;
		case ZE_UI_IKB_MINUS:
			Text.Append("-");
			break;
		case ZE_UI_IKB_EQUALS:
			Text.Append("=");
			break;
// 		case ZE_UI_IKB_TAB:
// 			for (size_t I = 0; I < 4; I++)
// 				Text.Append(" ");
// 			break;
		case ZE_UI_IKB_Q:
			Text.Append("q");
			break;
		case ZE_UI_IKB_W:
			Text.Append("w");
			break;
		case ZE_UI_IKB_E:
			Text.Append("e");
			break;
		case ZE_UI_IKB_R:
			Text.Append("r");
			break;
		case ZE_UI_IKB_T:
			Text.Append("t");
			break;
		case ZE_UI_IKB_Y:
			Text.Append("y");
			break;
		case ZE_UI_IKB_U:
			Text.Append("u");
			break;
		case ZE_UI_IKB_I:
			Text.Append("i");
			break;
		case ZE_UI_IKB_O:
			Text.Append("o");
			break;
		case ZE_UI_IKB_P:
			Text.Append("p");
			break;
		case ZE_UI_IKB_LBRACKET:
			Text.Append("(");
			break;
		case ZE_UI_IKB_RBRACKET:
			Text.Append(")");
			break;
		case ZE_UI_IKB_A:
			Text.Append("a");
			break;
		case ZE_UI_IKB_S:
			Text.Append("s");
			break;
		case ZE_UI_IKB_D:
			Text.Append("d");
			break;
		case ZE_UI_IKB_F:
			Text.Append("f");
			break;
		case ZE_UI_IKB_G:
			Text.Append("g");
			break;
		case ZE_UI_IKB_H:
			Text.Append("h");
			break;
		case ZE_UI_IKB_J:
			Text.Append("j");
			break;
		case ZE_UI_IKB_K:
			Text.Append("k");
			break;
		case ZE_UI_IKB_L:
			Text.Append("l");
			break;
		case ZE_UI_IKB_SEMICOLON:
			Text.Append(";");
			break;
		case ZE_UI_IKB_APOSTROPHE:
			//Text.Append("");
			break;
		case ZE_UI_IKB_BACKSLASH:
			Text.Append("\\");
			break;
		case ZE_UI_IKB_Z:
			Text.Append("z");	
			break;
		case ZE_UI_IKB_X:
			Text.Append("x");
			break;
		case ZE_UI_IKB_C:
			Text.Append("c");
			break;
		case ZE_UI_IKB_V:
			Text.Append("v");
			break;
		case ZE_UI_IKB_B:
			Text.Append("b");
			break;
		case ZE_UI_IKB_N:
			Text.Append("n");
			break;
		case ZE_UI_IKB_M:
			Text.Append("m");
			break;
		case ZE_UI_IKB_COMMA:
			Text.Append(",");
			break;
		case ZE_UI_IKB_PERIOD:
			Text.Append(".");
			break;
		case ZE_UI_IKB_SLASH:
			Text.Append("/");
			break;
		case ZE_UI_IKB_MULTIPLY:
			Text.Append("*");
			break;
		case ZE_UI_IKB_SPACE:
			Text.Append(" ");
			break;
		case ZE_UI_IKB_AT:
			Text.Append("@");
			break;
		case ZE_UI_IKB_COLON:
			break;
		case ZE_UI_IKB_UNDERLINE:
			Text.Append("_");
			break;
		case ZE_UI_IKB_DIVIDE:
			Text.Append("/");
			break;
		default:
			return;
			break;
	}

	IsTextDirty = true;
}

void ZEUITextEditControl::Tick(float ElapsedTime)
{
	ZEUILabel::Tick(ElapsedTime);

	if(IsTextDirty)
	{
		UpdateCharacters();
		IsTextDirty = false;
	}
}

void ZEUITextEditControl::Draw(ZEUIRenderer* Renderer)
{
	ZEUILabel::Draw(Renderer);

	if (GetFocused() && Cursor.GetVisible())
	{
		ZEVector2 CursorPosition;
		Cursor.ZOrder = GetZOrder() + 2;

// 		if(Characters.GetCount() != 0)
// 		{
// 			ZEUIRectangle Temp;
// 			Temp = Characters.GetLastItem().RenderableCharacter;
// 			Temp.Positions.LeftUp += TextStartPosition + GetScreenPosition();
// 			Temp.Positions.RightDown += TextStartPosition + GetScreenPosition();
// 
// 			CursorPosition = Temp.Positions.RightDown;
// 			CursorPosition.y -= Characters.GetLastItem().FontCharacter.CharacterMetric.Height;
// 			Cursor.SetPostion(CursorPosition);
// 		}
// 		else
// 			Cursor.SetPostion(TextStartPosition + GetScreenPosition() + TextMargin);
	
		ZEUIRectangle Output;
	
		if(!ZEUIRectangle::Clip(Output, Cursor, GetVisibleRectangle()))
			Renderer->AddRectangle(Output);
	}
}

ZEUITextEditControl::ZEUITextEditControl()
{
	SetFocusable(true);
	SetMoveable(true);
	IsTextDirty = false;
}

const ZEMaterial* ZEUITextEditControl::GetCursorMaterial() const
{
	return Cursor.Material; 
}

void ZEUITextEditControl::SetCursorColor(const ZEVector4& Color)
{
	Cursor.Color = Color;
}

const ZEVector4& ZEUITextEditControl::GetCursorColor() const
{
	return Cursor.Color;
}

void ZEUITextEditControl::SetReadOnly(bool ReadOnly)
{
	IsReadOnly = ReadOnly;
	SetFocusable(!IsReadOnly);
}

bool ZEUITextEditControl::GetReadOnly()
{
	return IsReadOnly;
}
