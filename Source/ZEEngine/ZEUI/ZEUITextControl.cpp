//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUITextControl.cpp
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

#include "ZEUITextControl.h"
#include "ZEUIRenderer.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGraphics/ZEUIMaterial.h"
#include "ZEFontResourceDynamic.h"

void ZEUITextControl::SetDisplayTextCharacters()
{
	if (FontResource == NULL)
		return;

	DisplayTextCharacters.Clear();
	int Line = 1;

	const char* Narrow = (const char*)Text;
	size_t Size = Text.GetLength();

	const ZERectangle& ControlRectangle = GetRectangle();

	ZEUIRectangle Temporary;
	Temporary.Material = NULL;
	Temporary.Color = TextColor;

	ZEVector2 CharacterSize;

	Temporary.Positions.LeftUp = ControlRectangle.LeftUp;

	for (size_t I = 0; I < Text.GetLength(); I++)
	{
		if(FontResource->GetFontResourceType() == ZE_FRT_BITMAP)
		{
			if (SpecialCharacters.Exists(Text[I]))
			{
				Temporary.Positions.LeftUp = SpecialCharacter('\n', I, Temporary);
			}
			else
			{
				const ZEFontCharacter& CurrCharacter = FontResource->GetCharacter(Narrow[I]);

				ZEVector2::Substution(CharacterSize, CurrCharacter.CoordinateRectangle.RightDown, CurrCharacter.CoordinateRectangle.LeftUp);
				ZEVector2::Multiply(CharacterSize, CharacterSize, FontSize);
				ZEVector2::Multiply(CharacterSize, CharacterSize, ZEVector2((float)CurrCharacter.Texture->GetWidth(), (float)CurrCharacter.Texture->GetHeight()));

				if (TextWrap)
				{
					if (Temporary.Positions.LeftUp.x + CharacterSize.x > ControlRectangle.RightDown.x)
					{
						Temporary.Positions.LeftUp.y += CharacterSize.y;
						Temporary.Positions.LeftUp.x = ControlRectangle.LeftUp.x;
						Line++;
					}
				}

				ZEVector2::Add(Temporary.Positions.RightDown, Temporary.Positions.LeftUp, CharacterSize);
				Temporary.Texcoords = CurrCharacter.CoordinateRectangle;

				ZEUITextCharacter TextControlCharacter;
				TextControlCharacter.CChar = Text[I];
				TextControlCharacter.RenderableCharacter = Temporary;
				TextControlCharacter.FontCharacter = CurrCharacter;
				TextControlCharacter.Line = Line;

				DisplayTextCharacters.Add(TextControlCharacter);

				Temporary.Positions.LeftUp.x = Temporary.Positions.RightDown.x;
			}
		}
		else if(FontResource->GetFontResourceType() == ZE_FRT_DYNAMIC)
		{
			ZEInt64 KerningDistance = 0;
			const ZEFontCharacter CurrCharacter = (I < Size - 1) ? FontResource->GetCharacter(Narrow[I], Narrow[I + 1], KerningDistance) : FontResource->GetCharacter(Narrow[I]);
			const ZEFontCharacter NextChar = (I < Size - 1) ? FontResource->GetCharacter(Narrow[I + 1]) : FontResource->GetCharacter(Narrow[I]);

			ZEVector2::Substution(CharacterSize, CurrCharacter.CoordinateRectangle.RightDown, CurrCharacter.CoordinateRectangle.LeftUp);
			ZEVector2::Multiply(CharacterSize, CharacterSize, ZEVector2((float)CurrCharacter.Texture->GetWidth(), (float)CurrCharacter.Texture->GetHeight()));

			if (TextWrap)
			{
				if (Temporary.Positions.LeftUp.x + CharacterSize.x > ControlRectangle.RightDown.x)
				{
					Temporary.Positions.LeftUp.y += CharacterSize.y;
					Temporary.Positions.LeftUp.x = ControlRectangle.LeftUp.x;
					Line++;
				}
			}

			ZEVector2::Add(Temporary.Positions.RightDown, Temporary.Positions.LeftUp, CharacterSize);

			float HorizontalBearingY = CurrCharacter.CharacterMetric.FontSize - CurrCharacter.CharacterMetric.HorizontalBearingY - (CurrCharacter.CharacterMetric.FontSize - CurrCharacter.CharacterMetric.MaximumHeight);

			Temporary.Positions.LeftUp.y += HorizontalBearingY;
			Temporary.Positions.RightDown.y += HorizontalBearingY;

			Temporary.Texcoords = CurrCharacter.CoordinateRectangle;

			ZEUITextCharacter TextControlCharacter;
			TextControlCharacter.CChar = Text[I];
			TextControlCharacter.RenderableCharacter = Temporary;
			TextControlCharacter.FontCharacter = CurrCharacter;
			TextControlCharacter.Line = Line;

			DisplayTextCharacters.Add(TextControlCharacter);

			Temporary.Positions.LeftUp.y -= HorizontalBearingY;
			Temporary.Positions.RightDown.y -= HorizontalBearingY;
			Temporary.Positions.RightDown.x += KerningDistance - CurrCharacter.CharacterMetric.HorizontalBearingX + NextChar.CharacterMetric.HorizontalBearingX;

			Temporary.Positions.LeftUp.x = Temporary.Positions.RightDown.x;
		}
	}
	RearrangeDimensions();
}

const ZEArray<ZEUITextCharacter>& ZEUITextControl::GetTextControlCharacters()
{
	return DisplayTextCharacters;
}


ZEVector2 ZEUITextControl::SpecialCharacter(char Character, size_t Position, ZEUIRectangle Rectangle)
{
	ZEVector2 CharacterSize;
	ZEUIRectangle Temporary;

	const ZEFontCharacter& CurrCharacter = FontResource->GetCharacter(' ');

	ZEVector2::Substution(CharacterSize, CurrCharacter.CoordinateRectangle.RightDown, CurrCharacter.CoordinateRectangle.LeftUp);
	ZEVector2::Multiply(CharacterSize, CharacterSize, FontSize);
	ZEVector2::Multiply(CharacterSize, CharacterSize, ZEVector2((float)CurrCharacter.Texture->GetWidth(), (float)CurrCharacter.Texture->GetHeight()));

	Temporary.Positions.LeftUp.x = GetRectangle().GetPosition().x;
	Temporary.Positions.LeftUp.y = Rectangle.Positions.LeftUp.y + CharacterSize.y;
	Temporary.Positions.RightDown.x = GetRectangle().GetPosition().x;
	Temporary.Positions.RightDown.y = Temporary.Positions.LeftUp.y + CharacterSize.y;

	Temporary.Texcoords = CurrCharacter.CoordinateRectangle;

	ZEUITextCharacter TextControlCharacter;
	TextControlCharacter.CChar = Text[Position];
	TextControlCharacter.RenderableCharacter = Temporary;
	TextControlCharacter.FontCharacter = CurrCharacter;

	DisplayTextCharacters.Insert(Position, TextControlCharacter);

	return ZEVector2(DisplayTextCharacters[Position].RenderableCharacter.Positions.RightDown.x, DisplayTextCharacters[Position].RenderableCharacter.Positions.LeftUp.y);
}

void ZEUITextControl::RearrangeDimensions()
{
	const ZERectangle& ControlRectangle = GetVisibleRectangle();

	if (DisplayTextCharacters.GetCount() == 0)
	{
		SetHeight(ControlRectangle.GetHeight());
		SetWidth(ControlRectangle.GetWidth());
		return;
	}

	if (DisplayTextCharacters[DisplayTextCharacters.GetCount() - 1].RenderableCharacter.Positions.RightDown.y - DisplayTextCharacters[0].RenderableCharacter.Positions.LeftUp.y < ControlRectangle.GetHeight())
	{
		SetHeight(ControlRectangle.GetHeight());
	}
	else
		SetHeight(DisplayTextCharacters[DisplayTextCharacters.GetCount() - 1].RenderableCharacter.Positions.RightDown.y - DisplayTextCharacters[0].RenderableCharacter.Positions.LeftUp.y);

	if (TextWrap)
		SetWidth(ControlRectangle.GetWidth());

	else
	{
		SetWidth(ControlRectangle.GetWidth());
		float TextWidth;
		for (size_t I = 0; I < DisplayTextCharacters.GetCount(); I++)
		{	
			TextWidth = DisplayTextCharacters[I].RenderableCharacter.Positions.RightDown.x - DisplayTextCharacters[0].RenderableCharacter.Positions.LeftUp.x + 1;

			if (GetWidth() < TextWidth)
				SetWidth(TextWidth);
		}
	}
}

void ZEUITextControl::AddText(size_t Position, const ZEString& Value)
{
	if (Value.GetLength() != 0)
	{
		if (FontResource == NULL)
			return;

		int Line = 0;		

		size_t Size = Text.GetLength();

		const ZERectangle& ControlRectangle = GetRectangle();

		ZEUIRectangle Temporary;
		Temporary.Material = NULL;
		Temporary.Color = TextColor;

		ZEVector2 CharacterSize;

		Temporary.Positions.LeftUp = ControlRectangle.LeftUp;

		if (Size != 0)
		{
			Temporary.Positions.LeftUp.y = DisplayTextCharacters[Position - 1].RenderableCharacter.Positions.LeftUp.y;
			Temporary.Positions.LeftUp.x = DisplayTextCharacters[Position - 1].RenderableCharacter.Positions.RightDown.x;
			Line = DisplayTextCharacters[DisplayTextCharacters.GetCount() - 1].Line;
		}

		Text.Insert(Position, Value);
		const char* Narrow = (const char*)Text;

		for (size_t I = Position; I < Position + Value.GetLength(); I++)
		{
			if (SpecialCharacters.Exists(Text[I]))
				Temporary.Positions.LeftUp = SpecialCharacter('\n', I, Temporary);

			else
			{
				const ZEFontCharacter& CurrCharacter = FontResource->GetCharacter(Narrow[I]);

				ZEVector2::Substution(CharacterSize, CurrCharacter.CoordinateRectangle.RightDown, CurrCharacter.CoordinateRectangle.LeftUp);
				ZEVector2::Multiply(CharacterSize, CharacterSize, FontSize);
				ZEVector2::Multiply(CharacterSize, CharacterSize, ZEVector2((float)CurrCharacter.Texture->GetWidth(), (float)CurrCharacter.Texture->GetHeight()));

				if (TextWrap)
				{
					if (Temporary.Positions.LeftUp.x + CharacterSize.x > ControlRectangle.RightDown.x)
					{
						Temporary.Positions.LeftUp.y += CharacterSize.y;
						Temporary.Positions.LeftUp.x = ControlRectangle.LeftUp.x;
						Line++;
					}
				}

				ZEVector2::Add(Temporary.Positions.RightDown, Temporary.Positions.LeftUp, CharacterSize);
				Temporary.Texcoords = CurrCharacter.CoordinateRectangle;

				ZEUITextCharacter TextControlCharacter;
				TextControlCharacter.CChar = Text[I];
				TextControlCharacter.RenderableCharacter = Temporary;
				TextControlCharacter.FontCharacter = CurrCharacter;
				TextControlCharacter.Line = Line;

				DisplayTextCharacters.Insert(I, TextControlCharacter);

				Temporary.Positions.LeftUp.x = Temporary.Positions.RightDown.x;
			}
		}

		Temporary.Positions.LeftUp = ZEVector2(DisplayTextCharacters[Position + Value.GetLength() - 1].RenderableCharacter.Positions.RightDown.x, DisplayTextCharacters[Position + Value.GetLength() - 1].RenderableCharacter.Positions.LeftUp.y);
		for (size_t I = Position + Value.GetLength(); I < Text.GetLength(); I++)
		{
			if (Text[I] == '\n' || Text[I] == '\r')
			{
				Temporary.Positions.LeftUp.x = ControlRectangle.LeftUp.x;
				Temporary.Positions.LeftUp.y = DisplayTextCharacters[I - 1].RenderableCharacter.Positions.RightDown.y;
			}

			if (TextWrap)
			{
				if (Temporary.Positions.LeftUp.x + DisplayTextCharacters[I].RenderableCharacter.Positions.GetWidth() > ControlRectangle.RightDown.x)
				{
					Temporary.Positions.LeftUp.y += DisplayTextCharacters[I].RenderableCharacter.Positions.GetHeight();
					Temporary.Positions.LeftUp.x = ControlRectangle.LeftUp.x;
					Line++;
				}
			}

			DisplayTextCharacters[I].RenderableCharacter.Positions.SetPosition(Temporary.Positions.LeftUp);

			Temporary.Positions.LeftUp.x = DisplayTextCharacters[I].RenderableCharacter.Positions.RightDown.x;
		}

		RearrangeDimensions();
	}
}

void ZEUITextControl::AddCharacter(size_t Position, char Character)
{
	if (FontResource == NULL)
		return;

	int Line = 0;		

	size_t Size = Text.GetLength();

	const ZERectangle& ControlRectangle = GetRectangle();

	ZEUIRectangle Temporary;
	Temporary.Material = NULL;
	Temporary.Color = TextColor;

	ZEVector2 CharacterSize;

	Temporary.Positions.LeftUp = ControlRectangle.LeftUp;

	if (Size != 0 && Position != 0)
	{
		Temporary.Positions.LeftUp.y = DisplayTextCharacters[Position - 1].RenderableCharacter.Positions.LeftUp.y;
		Temporary.Positions.LeftUp.x = DisplayTextCharacters[Position - 1].RenderableCharacter.Positions.RightDown.x;
		Line = DisplayTextCharacters[DisplayTextCharacters.GetCount() - 1].Line;
	}

	Text.Insert(Position, ZEString(ZECharacter(Character)));
	const char* Narrow = Text.ToCString();

	if (SpecialCharacters.Exists(Text[Position]))
	{
		Temporary.Positions.LeftUp = SpecialCharacter(Text[Position], Position, Temporary);
	}
	else
	{
		const ZEFontCharacter& CurrCharacter = FontResource->GetCharacter(Narrow[Position]);

		ZEVector2::Substution(CharacterSize, CurrCharacter.CoordinateRectangle.RightDown, CurrCharacter.CoordinateRectangle.LeftUp);
		ZEVector2::Multiply(CharacterSize, CharacterSize, FontSize);
		ZEVector2::Multiply(CharacterSize, CharacterSize, ZEVector2((float)CurrCharacter.Texture->GetWidth(), (float)CurrCharacter.Texture->GetHeight()));

		if (TextWrap)
		{
			if (Temporary.Positions.LeftUp.x + CharacterSize.x > ControlRectangle.RightDown.x)
			{
				Temporary.Positions.LeftUp.y += CharacterSize.y;
				Temporary.Positions.LeftUp.x = ControlRectangle.LeftUp.x;
				Line++;
			}
		}

		ZEVector2::Add(Temporary.Positions.RightDown, Temporary.Positions.LeftUp, CharacterSize);
		Temporary.Texcoords = CurrCharacter.CoordinateRectangle;

		ZEUITextCharacter TextControlCharacter;
		TextControlCharacter.CChar = Text[Position];
		TextControlCharacter.RenderableCharacter = Temporary;
		TextControlCharacter.FontCharacter = CurrCharacter;
		TextControlCharacter.Line = Line;

		DisplayTextCharacters.Insert(Position, TextControlCharacter);

		Temporary.Positions.LeftUp = ZEVector2(DisplayTextCharacters[Position].RenderableCharacter.Positions.RightDown.x, DisplayTextCharacters[Position].RenderableCharacter.Positions.LeftUp.y);
	}

	for (size_t I = Position + 1; I < Text.GetLength(); I++)
	{
		if (Text[I] == '\n' || Text[I] == '\r')
		{
			Temporary.Positions.LeftUp.x = ControlRectangle.LeftUp.x;
			Temporary.Positions.LeftUp.y = DisplayTextCharacters[I - 1].RenderableCharacter.Positions.RightDown.y;
		}

		if (TextWrap)
		{
			if (Temporary.Positions.LeftUp.x + DisplayTextCharacters[I].RenderableCharacter.Positions.GetWidth() > ControlRectangle.RightDown.x)
			{
				Temporary.Positions.LeftUp.y += DisplayTextCharacters[I].RenderableCharacter.Positions.GetHeight();
				Temporary.Positions.LeftUp.x = ControlRectangle.LeftUp.x;
				Line++;
			}
		}

		DisplayTextCharacters[I].RenderableCharacter.Positions.SetPosition(Temporary.Positions.LeftUp);

		Temporary.Positions.LeftUp.x = DisplayTextCharacters[I].RenderableCharacter.Positions.RightDown.x;
	}

	RearrangeDimensions();
}


void ZEUITextControl::DeleteCharacters(ZEArray<size_t> Positions)
{
	if (Positions.GetCount() == 0)
		return;

	const ZERectangle& ControlRectangle = GetRectangle();
	ZEUIRectangle Temporary;

	if (Positions[0] != 0)
		Temporary.Positions.LeftUp = ZEVector2(DisplayTextCharacters[Positions[0] - 1].RenderableCharacter.Positions.RightDown.x, DisplayTextCharacters[Positions[0] - 1].RenderableCharacter.Positions.LeftUp.y);	

	else
		Temporary.Positions.LeftUp = ControlRectangle.LeftUp;


	for (int I = Positions.GetCount() - 1; I >= 0; I--)
	{
		Text.Remove(Positions[I]);
		DisplayTextCharacters.DeleteAt(Positions[I]);
	}

	for (size_t I = Positions[0]; I < Text.GetLength(); I++)
	{
		if (Text[I] == '\n' || Text[I] == '\r')
		{
			if (I != 0)
			{
				Temporary.Positions.LeftUp.x = ControlRectangle.LeftUp.x;
				Temporary.Positions.LeftUp.y = DisplayTextCharacters[I - 1].RenderableCharacter.Positions.RightDown.y;
			}	
		}
		if (TextWrap)
		{
			if (Temporary.Positions.LeftUp.x + DisplayTextCharacters[I].RenderableCharacter.Positions.GetWidth() > ControlRectangle.RightDown.x)
			{
				Temporary.Positions.LeftUp.y += DisplayTextCharacters[I].RenderableCharacter.Positions.GetHeight();
				Temporary.Positions.LeftUp.x = ControlRectangle.LeftUp.x;
			}
		}

		DisplayTextCharacters[I].RenderableCharacter.Positions.SetPosition(Temporary.Positions.LeftUp);

		Temporary.Positions.LeftUp.x = DisplayTextCharacters[I].RenderableCharacter.Positions.RightDown.x;
	}

	RearrangeDimensions();
}

void ZEUITextControl::DeleteCharacter(size_t Position)
{
	const ZERectangle& ControlRectangle = GetRectangle();
	ZEUIRectangle Temporary;

	if (Text.GetLength()  == 0)
		return;

	if (Position != 0)
		Temporary.Positions.LeftUp = ZEVector2(DisplayTextCharacters[Position - 1].RenderableCharacter.Positions.RightDown.x, DisplayTextCharacters[Position - 1].RenderableCharacter.Positions.LeftUp.y);

	else
		Temporary.Positions.LeftUp = ControlRectangle.LeftUp;

	Text.Remove(Position);
	DisplayTextCharacters.DeleteAt(Position);

	for (size_t I = Position; I < Text.GetLength(); I++)
	{
		if (Text[I] == '\n' || Text[I] == '\r')
		{
			if (I != 0)
			{
				Temporary.Positions.LeftUp.x = ControlRectangle.LeftUp.x;
				Temporary.Positions.LeftUp.y = DisplayTextCharacters[I - 1].RenderableCharacter.Positions.RightDown.y;
			}			
		}

		if (TextWrap)
		{
			if (Temporary.Positions.LeftUp.x + DisplayTextCharacters[I].RenderableCharacter.Positions.GetWidth() > ControlRectangle.RightDown.x)
			{
				Temporary.Positions.LeftUp.y += DisplayTextCharacters[I].RenderableCharacter.Positions.GetHeight();
				Temporary.Positions.LeftUp.x = ControlRectangle.LeftUp.x;
			}
		}

		DisplayTextCharacters[I].RenderableCharacter.Positions.SetPosition(Temporary.Positions.LeftUp);

		Temporary.Positions.LeftUp.x = DisplayTextCharacters[I].RenderableCharacter.Positions.RightDown.x;
	}

	RearrangeDimensions();
}

void ZEUITextControl::SetText(const ZEString& Value)
{
	this->Text = Value;
	SetDisplayTextCharacters();
}

const ZEString& ZEUITextControl::GetText()
{
	return Text;
}

void ZEUITextControl::SetPosition(float X, float Y)
{
	SetPosition(ZEVector2(X, Y));
}

void ZEUITextControl::SetPosition(const ZEVector2& Position)
{
	ZEVector2 MoveAmount = Position - GetPosition();

	for (int I = 0; I < DisplayTextCharacters.GetCount(); I++)
	{
		DisplayTextCharacters[I].RenderableCharacter.Positions.SetPosition(DisplayTextCharacters[I].RenderableCharacter.Positions.GetPosition() + MoveAmount);
	}

	ZEUIControl::SetPosition(Position);
}

void ZEUITextControl::SetFont(ZEFontResource* FontResource)
{
	if (this->FontResource != NULL)
		this->FontResource->Release();
		
	this->FontResource = FontResource;
	
	if (FontResource != NULL)
		FontResource->AddReferance();
}

ZEFontResource* ZEUITextControl::GetFont()
{
	return FontResource;
}

void ZEUITextControl::SetFontSize(const ZEVector2& FontSize)
{
	this->FontSize = FontSize;
}

const ZEVector2& ZEUITextControl::GetFontSize()
{
	return FontSize;
}

void ZEUITextControl::SetTextColor(const ZEVector4& Color)
{
	TextColor = Color;
}

const ZEVector4& ZEUITextControl::GetTextColor()
{
	return TextColor;
}

void ZEUITextControl::SetTextWrap(bool Enabled)
{
	TextWrap = Enabled;
}

bool ZEUITextControl::GetTextWrap()
{
	return TextWrap;
}

void ZEUITextControl::Draw(ZEUIRenderer* Renderer)
{
	if (FontResource == NULL)
		return;

	ZERectangle ControlVisibleRectangle = GetVisibleRectangle();

	if (GetParentControl() != NULL)
		ControlVisibleRectangle = GetParentControl()->GetVisibleRectangle();

	for (size_t I = 0; I < DisplayTextCharacters.GetCount(); I++)
	{
		if (!ZEUIRectangle::Clip(Output, DisplayTextCharacters[I].RenderableCharacter, ControlVisibleRectangle)) // Muhtemel bug
		{
			if (DisplayTextCharacters[I].IsSelected == true)
			{
				ZEUIRectangle Highlight = Output;
				Highlight.Material = HightlightMaterial;
				Highlight.Color = ZEVector4(0.0f, 0.0f, 1.0f , 0.5f);
				Renderer->AddRectangle(Highlight);
			}

			Material->SetTexture(DisplayTextCharacters[I].FontCharacter.Texture);
			Output.Material = Material;
			Output.ZOrder = GetZOrder();
			Renderer->AddRectangle(Output);
		}
	}
}

void ZEUITextControl::SetMaterial(ZEMaterial* Material)
{
	
}

ZEMaterial* ZEUITextControl::GetMaterial() const
{
	return NULL;
}

ZEUITextControl::ZEUITextControl()
{
	Material = ZEUIMaterial::CreateInstance();
	Material->SetTexture(NULL);
	Material->SetWireFrame(false);
	Material->UpdateMaterial();
	FontResource = NULL;
	FontSize = ZEVector2::One;
	TextColor = ZEVector4(1.0f, 1.0f, 1.0f, 1.0f);
	TextWrap = false;
	HightlightMaterial = ZEUIMaterial::CreateInstance();
	SetText("");
	SpecialCharacters.Add('\n');
	SpecialCharacters.Add('\r');
	SetPosition(ZEVector2::Zero);
}

ZEUITextControl::~ZEUITextControl()
{
	if (FontResource != NULL)
		FontResource->Release();
}
