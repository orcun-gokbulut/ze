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
#include "ZEFontResource.h"
#include "ZEGraphics/ZETexture2D.h"

void ZEUITextControl::SetText(const ZEString& Value)
{
	this->Text = Value;
}

const ZEString& ZEUITextControl::GetText()
{
	return Text;
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

	const char* Narrow = (const char*)Text;
	ZESize Size = Text.GetLength();

	const ZERectangle& ControlRectangle = GetVisibleRectangle();
	ZEInt ZOrder = GetZOrder();

	ZEUIRectangle Temporary;
	Temporary.Material = NULL;
	Temporary.Color = TextColor;

	ZEVector2 CharacterSize;

	Temporary.Positions.LeftUp = ControlRectangle.LeftUp;

	Output.ZOrder = ZOrder;
	for (ZESize I = 0; I < Size; I++)
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
				if (Temporary.Positions.LeftUp.y > ControlRectangle.RightDown.y)
					return;

				Temporary.Positions.LeftUp.x = ControlRectangle.LeftUp.x;
			}
		}
		else
			if (Temporary.Positions.LeftUp.x > ControlRectangle.RightDown.x)
				return;

		ZEVector2::Add(Temporary.Positions.RightDown, Temporary.Positions.LeftUp, CharacterSize);
		Temporary.Texcoords = CurrCharacter.CoordinateRectangle;


	/*	if (Temporary.Positions.RightDown.y > ControlRectangle.RightDown.y)
		{*/
			if (!ZEUIRectangle::Clip(Output, Temporary, ControlRectangle)) // Muhtemel bug
			{
				Output.Material = CurrCharacter.Material;
				Output.ZOrder = ZOrder;
				Renderer->AddRectangle(Output);
			}
	/*	}
		else
		{
			Output.Material = CurrCharacter.Material;
			Output.ZOrder = ZOrder;
			Renderer->AddRectangle(Temporary);
		}*/

		Temporary.Positions.LeftUp.x = Temporary.Positions.RightDown.x;
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
	FontResource = NULL;
	FontSize = ZEVector2::One;
	TextColor = ZEVector4(1.0f, 1.0f, 1.0f, 1.0f);
	TextWrap = false;
}

ZEUITextControl::~ZEUITextControl()
{
	if (FontResource != NULL)
		FontResource->Release();
}





