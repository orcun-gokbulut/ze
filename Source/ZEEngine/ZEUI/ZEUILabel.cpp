//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUILabel.cpp
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

#include "ZEUILabel.h"
#include "ZEFontResource.h"
#include "ZEGraphics/ZETexture2D.h"

void ZEUILabel::UpdateCharacters()
{
	ZEInt64 KerningDistance = 0;
	ZEFontCharacter Character;
	ZEUIRectangle Output;

	char AddedCharacter = Text.Right(1).GetValue()[0];

	if(Text.GetLength() == 1)
		Character = FontResource->GetCharacter(AddedCharacter);
	else
	{
		char PreviousCharacter = Text.Right(2).GetValue()[0];
		Character = FontResource->GetCharacter(AddedCharacter, PreviousCharacter, KerningDistance);		
	}

	ZEVector2 CharacterSize = Character.CoordinateRectangle.RightDown - Character.CoordinateRectangle.LeftUp;
	ZEVector2::Multiply(CharacterSize, CharacterSize, ZEVector2((float)Character.Texture->GetWidth(), (float)Character.Texture->GetHeight()));

	if(Characters.GetCount() == 0)
	{
		Output.Positions.LeftUp.x = 0;
		Output.Positions.LeftUp.y = Character.CharacterMetric.MaximumHeight - Character.CharacterMetric.HorizontalBearingY;
	}
	else
	{
		ZEUITextCharacter& LastCharacter = Characters.GetLastItem();
		ZEVector2 LeftUp = LastCharacter.RenderableCharacter.Positions.RightDown;
		LeftUp.y -= Character.CharacterMetric.HorizontalBearingY;
		LeftUp.y -= LastCharacter.FontCharacter.CharacterMetric.Height - LastCharacter.FontCharacter.CharacterMetric.HorizontalBearingY;
		LeftUp.x += KerningDistance;
		LeftUp.x += Character.CharacterMetric.HorizontalBearingX - LastCharacter.FontCharacter.CharacterMetric.HorizontalBearingX;
		Output.Positions.LeftUp = LeftUp;
	}

	Output.Positions.RightDown =  Output.Positions.LeftUp + CharacterSize;

	ZEUITextCharacter TextCharacter;
	TextCharacter.CChar = AddedCharacter;
	TextCharacter.FontCharacter = Character;
	TextCharacter.IsSelected = false;
	TextCharacter.RenderableCharacter.Material = FontMaterial;
	TextCharacter.RenderableCharacter.ZOrder = GetZOrder() + 1;
	TextCharacter.RenderableCharacter.Texcoords = Character.CoordinateRectangle;
	TextCharacter.RenderableCharacter.Color = FontColor;
	TextCharacter.RenderableCharacter.Positions = Output.Positions;
	((ZEUIMaterial*)TextCharacter.RenderableCharacter.Material)->SetTexture(Character.Texture);
	TextWidth += TextCharacter.RenderableCharacter.Positions.GetWidth();

	if (IsWordWrapping)
	{
		if((CurrentLineWidth + TextCharacter.RenderableCharacter.Positions.GetWidth()) > TextRenderingArea.GetWidth())
		{
			CurrentLine++;
			CurrentLineWidth = 0;
			TextCharacter.RenderableCharacter.Positions.LeftUp.y += Characters.GetLastItem().FontCharacter.CharacterMetric.VerticalAdvance;
			TextCharacter.RenderableCharacter.Positions.RightDown.y += Characters.GetLastItem().FontCharacter.CharacterMetric.VerticalAdvance;
 			TextCharacter.RenderableCharacter.Positions.LeftUp.x = TextRenderingArea.LeftUp.x - GetPosition().x;
 			TextCharacter.RenderableCharacter.Positions.RightDown.x = TextCharacter.RenderableCharacter.Positions.LeftUp.x + CharacterSize.x; 
		}
		
		CurrentLineWidth += TextCharacter.RenderableCharacter.Positions.GetWidth();
	}
	else
	{
		CurrentLine = 0;
		TextCharacter.Line = CurrentLine;
	}

	Characters.Add(TextCharacter);
}

ZEVector2 ZEUILabel::CalculateTextStartPoint()
{
	ZEVector2 StartPosition = TextRenderingArea.GetPosition();

	if(TextAlignment == ZE_UI_TA_RIGHT)
	{
		StartPosition.y = StartPosition.y;
		StartPosition.x = GetRectangle().RightDown.x - (TextWidth + TextMargin.x);
	}
	else if(TextAlignment == ZE_UI_TA_CENTER)
	{
		StartPosition.y = StartPosition.y;
		StartPosition.x += (int)((GetRectangle().GetWidth() - TextWidth) / 2.0f);
	}
	else
		StartPosition = StartPosition;

	return StartPosition;
}

void ZEUILabel::Draw(ZEUIRenderer* Renderer)
{
	ZEUIFrameControl::Draw(Renderer);

	if(FontResource == NULL)
		return;

	ZEUIRectangle Output, Temp;
	TextStartPosition = CalculateTextStartPoint();

	for (ZESize I = 0; I < Characters.GetCount(); I++)
	{
		Temp = Characters[I].RenderableCharacter;
		Temp.Positions.LeftUp += TextStartPosition;
		Temp.Positions.RightDown += TextStartPosition;

		if(!ZEUIRectangle::Clip(Output, Temp, TextRenderingArea))
			Renderer->AddRectangle(Output);
	}

	TextStartPosition = CalculateTextStartPoint();
}

ZEUILabel::ZEUILabel()
{
	FontResource = NULL;
	Characters.Clear();
	Text.Clear();
	SetFocusable(false);
	SetMoveable(true);
	FontMaterial = ZEUIMaterial::CreateInstance();
	SetFontColor(ZEVector4::UnitW);
	TextWidth = 0;
	SetTextMargin(ZEVector2::One * 2);
	CurrentLine = 0;
	CurrentLineWidth = 0;
	SetWordWrapping(false);
}

ZEUILabel::~ZEUILabel()
{
	if(FontMaterial != NULL)
	{
		FontMaterial->Destroy();
		FontMaterial = NULL;
	}

	Characters.Clear();
	Text.Clear();
}

void ZEUILabel::SetFontColor(const ZEVector4& Color)
{
	FontColor = Color;

	for (ZESize I = 0; I < Characters.GetCount(); I++)
		Characters[I].RenderableCharacter.Color = FontColor;
}

const ZEVector4& ZEUILabel::GetFontColor() const
{
	return FontColor;
}

void ZEUILabel::SetText(ZEString Text)
{
	this->Text = "";
	Characters.Clear();

	for (ZESize I = 0; I < Text.GetLength(); I++)
	{
		ZEString NewCharacter = Text[I];
		this->Text.Append(NewCharacter);
		UpdateCharacters();
	}
}

const ZEString& ZEUILabel::GetText()
{
	return Text;
}

void  ZEUILabel::SetTextAlignment(ZEUITextAlignment Alignment)
{
	TextAlignment = Alignment;
}

ZEUITextAlignment ZEUILabel::GetTextAlignment() const
{
	return TextAlignment;
}

void ZEUILabel::SetTextMargin(const ZEVector2& Margin)
{
	TextMargin = Margin;
}

const ZEVector2& ZEUILabel::GetTextMargin() const
{
	return TextMargin;
}

void ZEUILabel::SetWordWrapping(bool Enabled)
{
	IsWordWrapping = Enabled;
	SetText(Text);
}

bool ZEUILabel::GetWordWrapping() const
{
	return IsWordWrapping;
}

void ZEUILabel::SetPosition(const ZEVector2& Position)
{
	ZEUIFrameControl::SetPosition(Position);
	TextRenderingArea.SetPosition(Position + TextMargin);
	TextRenderingArea.SetSize(GetSize() - TextMargin * 2);
}

void ZEUILabel::SetSize(const ZEVector2& Size)
{
	ZEUIFrameControl::SetSize(Size);
	TextRenderingArea.SetPosition(GetPosition() + TextMargin);
	TextRenderingArea.SetSize(GetSize() - TextMargin * 2);
}

void ZEUILabel::SetWidth(float Width)
{
	ZEUIFrameControl::SetWidth(Width);
	TextRenderingArea.SetPosition(GetPosition() + TextMargin);
	TextRenderingArea.SetSize(GetSize() - TextMargin * 2);
}

void ZEUILabel::SetHeight(float Height)
{
	ZEUIFrameControl::SetHeight(Height);
	TextRenderingArea.SetPosition(GetPosition() + TextMargin);
	TextRenderingArea.SetSize(GetSize() - TextMargin * 2);
}

void ZEUILabel::SetFontResource(ZEFontResource* Resource)
{
	FontResource = Resource;
	SetText(Text);
}
