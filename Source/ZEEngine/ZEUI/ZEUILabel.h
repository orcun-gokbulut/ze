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
#include "ZEFontResource.h"
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
	ZEFontCharacter		FontCharacter;
	ZEUIRectangle		RenderableCharacter;
	int					Line;
	bool				IsSelected;
};

class ZEUILabel : public ZEUIFrameControl
{
	protected:
		ZEString							Text;
		ZEArray<ZEUITextCharacter>			Characters;

		ZEFontResource*						FontResource;
		ZEMaterial*							FontMaterial;
		ZEVector4							FontColor;

		ZESize								TextWidth;
		ZERectangle							TextRenderingArea;
		ZEVector2							TextStartPosition;

		ZEUITextAlignment					TextAlignment;
		ZEVector2							TextMargin;
		bool								IsWordWrapping;

		ZEUInt32							CurrentLine;
		ZEUInt32							CurrentLineWidth;

		void								UpdateCharacters();
		ZEVector2							CalculateTextStartPoint();

	public:
		virtual void						Draw(ZEUIRenderer* Renderer);

		void								SetFontResource(ZEFontResource* Resource);

		void								SetFontColor(const ZEVector4& Color);
		const ZEVector4&					GetFontColor() const;

		void								SetText(ZEString Text);
		const ZEString&						GetText();

		void								SetTextAlignment(ZEUITextAlignment Alignment);
		ZEUITextAlignment					GetTextAlignment() const;

		void								SetTextMargin(const ZEVector2& Margin);
		const ZEVector2&					GetTextMargin() const;

		void								SetWordWrapping(bool Enabled);
		bool								GetWordWrapping() const;

		virtual void						SetPosition(const ZEVector2& Position);
		virtual void						SetSize(const ZEVector2& Size);
		virtual void						SetWidth(float Width);
		virtual void						SetHeight(float Height);

											ZEUILabel();
											~ZEUILabel();
};

#endif
