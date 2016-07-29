//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUILabel.h
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
#ifndef __ZE_UI_LABEL__
#define __ZE_UI_LABEL__

#include "ZEUIFrameControl.h"
#include "ZEUIFont.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"

enum ZEUITextAlignment
{
	ZE_UI_TA_LEFT,
	ZE_UI_TA_RIGHT,
	ZE_UI_TA_CENTER
};

struct ZEUITextCharacter
{
	char				CChar;
	ZEUIFontCharacter	FontCharacter;
	ZEUIRectangle		RenderableCharacter;
	ZEInt32				Line;
	bool				IsSelected;
};

class ZEUILabel : public ZEUIFrameControl
{
	ZE_OBJECT

	protected:
		ZEString							Text;
		ZEArray<ZEUITextCharacter>			Characters;
		ZEArray<ZEInt32>					LineTextWidths;

		ZEHolder<const ZEUIFont>			FontResource;
		ZEVector4							FontColor;

		ZERectangle							TextRenderingArea;

		ZEUITextAlignment					TextAlignment;
		bool								IsWordWrapping;

		ZEUInt32							CurrentLine;
		ZEVector4							TextMargins;

		void								UpdateCharacters();
		ZEVector2							CalculateLineStartPoint(ZESize LineIndex);
		void								UpdateTextRenderingArea();

	public:
		virtual void						Draw(ZEUIRenderer* Renderer);

		void								SetFontResource(ZEHolder<const ZEUIFont> Resource);
		ZEHolder<const ZEUIFont>			GetFontResource();

		void								SetFontColor(const ZEVector4& Color);
		const ZEVector4&					GetFontColor() const;

		void								SetText(ZEString NewText);
		const ZEString&						GetText() const;

		void								SetTextAlignment(ZEUITextAlignment Alignment);
		ZEUITextAlignment					GetTextAlignment() const;

		void								SetWordWrapping(bool Enabled);
		bool								GetWordWrapping() const;

		void								SetTextMargins(ZEUInt32 Top = 2, ZEUInt32 Left = 2, ZEUInt32 Bottom = 2, ZEUInt32 Right = 2);
		void								SetTextMargins(const ZEVector4& Margins);
		const ZEVector4&					GetTextMargins() const;

		virtual void						SetPosition(const ZEVector2& Position);
		virtual void						SetSize(const ZEVector2& Size);
		virtual void						SetWidth(float Width);
		virtual void						SetHeight(float Height);

											ZEUILabel();
											~ZEUILabel();
};

#endif
