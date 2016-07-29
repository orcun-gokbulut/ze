//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIFont.h
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

#pragma once

#include "ZEResource/ZERSResource.h"

#include "ZEDS/ZEArray.h"
#include "ZEMath/ZERectangle.h"

ZE_META_FORWARD_DECLARE(ZEGRTexture2D, "ZEGraphics/ZEGRTexture2D.h");

ZE_ENUM(ZEUIFontType)
{
	ZEUI_FT_NONE,
	ZEUI_FT_BITMAP,
	ZEUI_FT_TRUE_TYPE
};

class ZEUIFontCharacterMetric : public ZEObject
{
	public:
		ZEUInt32							FontSize;
		ZEUInt32							MaximumHeight;

		ZEInt32								Height;
		ZEInt32								Width;

		ZEInt32								HorizontalAdvance;
		ZEInt32								VerticalAdvance;

		ZEInt32								HorizontalBearingX;
		ZEInt32								HorizontalBearingY;

		ZEInt32								VerticalBearingX;
		ZEInt32								VerticalBearingY;
};

class ZEUIFontCharacter : public ZEObject
{
	public:
		ZEHolder<const ZEGRTexture2D>		Texture;
		ZEUInt32							GlyphIndex;
		char								Character;
		ZEUIFontCharacterMetric				CharacterMetric;
		ZERectangle							CoordinateRectangle;
};

class ZEUIFont : public ZERSResource
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEUIFont)
	protected:
											ZEUIFont();
		virtual								~ZEUIFont();

	public:
		virtual ZEUIFontType				GetFontResourceType() const = 0;
		virtual ZEUIFontCharacter			GetCharacter(char Character) const = 0;
		virtual ZEUIFontCharacter			GetCharacter(char CurrentChar, char NextChar, ZEInt64& KerningDistance) const = 0;
};
