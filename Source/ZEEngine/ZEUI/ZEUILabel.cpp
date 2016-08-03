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
#include "ZEUIFont.h"
#include "ZEGraphics/ZEGRTexture2D.h"

void ZEUILabel::UpdateCharacters()
{
	ZEInt64 KerningDistance = 0;
	ZEUIFontCharacter Character;
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
	TextCharacter.RenderableCharacter.ZOrder = GetZOrder() + 1;
	TextCharacter.RenderableCharacter.Texcoords = Character.CoordinateRectangle;
	TextCharacter.RenderableCharacter.Color = FontColor;
	TextCharacter.RenderableCharacter.Positions = Output.Positions;
	TextCharacter.RenderableCharacter.Texture = Character.Texture;
	TextCharacter.Line = 0;

	if (IsWordWrapping)
	{
		if((LineTextWidths.GetLastItem() + TextCharacter.RenderableCharacter.Positions.GetWidth()) >= TextRenderingArea.GetWidth())
		{
			LineTextWidths.Add(0);
			CurrentLine++;
			TextCharacter.RenderableCharacter.Positions.LeftUp.y += Characters.GetLastItem().FontCharacter.CharacterMetric.VerticalAdvance;
			TextCharacter.RenderableCharacter.Positions.RightDown.y += Characters.GetLastItem().FontCharacter.CharacterMetric.VerticalAdvance;
			TextCharacter.RenderableCharacter.Positions.LeftUp.x = TextRenderingArea.LeftUp.x ;
			TextCharacter.RenderableCharacter.Positions.RightDown.x = TextCharacter.RenderableCharacter.Positions.LeftUp.x + CharacterSize.x;
		}
	}

	LineTextWidths.GetLastItem() += TextCharacter.RenderableCharacter.Positions.GetWidth();
	TextCharacter.Line = CurrentLine;
	Characters.Add(TextCharacter);
}

void ZEUILabel::UpdateTextRenderingArea()
{
	TextRenderingArea.SetSize(GetSize());
	SetTextMargins(GetTextMargins());
}

ZEVector2 ZEUILabel::CalculateLineStartPoint(ZESize LineIndex)
{
	ZEVector2 StartPosition = TextRenderingArea.GetPosition();

	if(TextAlignment == ZE_UI_TA_RIGHT)
		StartPosition.x = TextRenderingArea.RightDown.x - (LineTextWidths[LineIndex]);

	else if(TextAlignment == ZE_UI_TA_CENTER)
		StartPosition.x += (ZEInt32)(((TextRenderingArea.GetWidth() - LineTextWidths[LineIndex]) / 2.0f) + 0.5f);

	else
		StartPosition = StartPosition;

	return StartPosition;
}

void ZEUILabel::Draw(ZEUIRenderer* Renderer)
{
	ZEUIFrameControl::Draw(Renderer);

	if(FontResource == NULL)
		return;

	if(Characters.GetCount() == 0)
		return;

	ZEUIRectangle OutputCharacter, TempCharacter;
	ZESSize DrawLineIndex = -1;
	ZERectangle TempRenderRect = TextRenderingArea;
	ZEVector2 CharacterPosIncrement, LineStartPosition, ScreenPos = GetScreenPosition();
	TempRenderRect.SetPosition(ScreenPos);
	ZERectangle::IntersectionTest(GetVisibleRectangle(), TextRenderingArea, TempRenderRect);
	ZEInt32 TempTextZOrder = GetZOrder() + 1;

	LineStartPosition = CalculateLineStartPoint(0);

	for (ZESize I = 0; I < Characters.GetCount(); I++)
	{
		if(Characters[I].Line != DrawLineIndex)
		{
			DrawLineIndex = Characters[I].Line;
			LineStartPosition = CalculateLineStartPoint(DrawLineIndex);
			CharacterPosIncrement = LineStartPosition + ScreenPos;
		}
		
		TempCharacter = Characters[I].RenderableCharacter;	
		TempCharacter.Positions.LeftUp += CharacterPosIncrement;
		TempCharacter.Positions.RightDown += CharacterPosIncrement;
		TempCharacter.ZOrder = TempTextZOrder;

		ZERectangle Intersection;
		if(ZERectangle::IntersectionTest(TempRenderRect, GetVisibleRectangle(), Intersection))
		{
			if(!ZEUIRectangle::Clip(OutputCharacter, TempCharacter, Intersection))
				Renderer->AddRectangle(&OutputCharacter);
		}
	}
}

ZEUILabel::ZEUILabel()
{
	TextRenderingArea.SetPosition(ZEVector2::Zero);
	TextRenderingArea.SetSize(ZEVector2::Zero);
	TextMargins = ZEVector4::Zero;
	SetFontColor(ZEUIManager::GetDefaultForegroundColor());
	SetBackgroundColor(ZEVector4::Zero);
	SetFontResource(ZEUIManager::GetDefaultFontResource());
	Characters.Clear();
	Text.Clear();
	SetFocusable(false);
	SetMoveable(false);
	CurrentLine = 0;
	SetWordWrapping(false);
	SetSize(ZEVector2::One * 100);
	SetTextMargins();
	LineTextWidths.Add(0);
	TextAlignment = ZE_UI_TA_LEFT;
}

ZEUILabel::~ZEUILabel()
{
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

void ZEUILabel::SetText(ZEString NewText)
{
	if(Text == NewText)
		return;

	this->Text = "";
	Characters.Clear();
	LineTextWidths.Clear();
	LineTextWidths.Add(0);
	CurrentLine = 0;

	for (ZESize I = 0; I < NewText.GetLength(); I++)
	{
		ZEString NewCharacter = NewText[I];
		this->Text.Append(NewCharacter);
		UpdateCharacters();
	}
}

const ZEString& ZEUILabel::GetText() const
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

void ZEUILabel::SetWordWrapping(bool Enabled)
{
	IsWordWrapping = Enabled;
	SetText(Text);
}

bool ZEUILabel::GetWordWrapping() const
{
	return IsWordWrapping;
}

void ZEUILabel::SetTextMargins(ZEUInt32 Top, ZEUInt32 Left, ZEUInt32 Bottom, ZEUInt32 Right)
{
	TextRenderingArea.LeftUp.y -= TextMargins.x;
	TextRenderingArea.LeftUp.x -= TextMargins.y;
	TextRenderingArea.RightDown.y += TextMargins.z;
	TextRenderingArea.RightDown.x += TextMargins.w;

	TextMargins = ZEVector4(Top, Left, Bottom, Right);

	TextRenderingArea.LeftUp.y += TextMargins.x;
	TextRenderingArea.LeftUp.x += TextMargins.y;
	TextRenderingArea.RightDown.y -= TextMargins.z;
	TextRenderingArea.RightDown.x -= TextMargins.w;
}

void ZEUILabel::SetTextMargins(const ZEVector4& Margins)
{
	SetTextMargins(Margins.x, Margins.y, Margins.z, Margins.w);
}

const ZEVector4& ZEUILabel::GetTextMargins() const
{
	return TextMargins;
}

void ZEUILabel::SetPosition(const ZEVector2& Position)
{
	ZEUIFrameControl::SetPosition(Position);
}

void ZEUILabel::SetSize(const ZEVector2& Size)
{
	ZEUIFrameControl::SetSize(Size);
	UpdateTextRenderingArea();
}

void ZEUILabel::SetWidth(float Width)
{
	ZEUIFrameControl::SetWidth(Width);
	UpdateTextRenderingArea();
}

void ZEUILabel::SetHeight(float Height)
{
	ZEUIFrameControl::SetHeight(Height);
	UpdateTextRenderingArea();
}

ZEUILabel* ZEUILabel::CreateInstance()
{
	return new ZEUILabel();
}

void ZEUILabel::SetFontResource(ZEHolder<const ZEUIFont> Resource)
{
	FontResource = Resource;
	SetText(Text);
}

ZEHolder<const ZEUIFont> ZEUILabel::GetFontResource()
{
	return FontResource;
}
