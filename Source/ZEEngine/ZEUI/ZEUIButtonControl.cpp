//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIButtonControl.cpp
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

#include "ZEUIButtonControl.h"
#include "ZEGraphics/ZEFixedMaterial.h"
#include "zeui/ZEUIRenderer.h"
#include "ZEUILabel.h"
#include "ZEFontResourceDynamic.h"

void ZEUIButtonControl::Draw(ZEUIRenderer* Renderer)
{
	TextLabel->SetZOrder(GetZOrder() + 3);
	ZEUIFrameControl::Draw(Renderer);
}

void ZEUIButtonControl::SetWidth(float Width)
{
	ZEUIFrameControl::SetWidth(Width);
	TextLabel->SetWidth(GetWidth());
}

void ZEUIButtonControl::SetHeight(float Height)
{
	ZEUIFrameControl::SetHeight(Height);
	ZEVector2 TextLocalPosition = TextLabel->GetPosition() - GetPosition();
	ZEInt32 LocalY = (GetHeight() - TextLabel->GetHeight()) / 2;
	TextLocalPosition.y = LocalY;
	TextLabel->SetPosition(TextLocalPosition + GetPosition());
}

void ZEUIButtonControl::SetSize(const ZEVector2& Size)
{
	ZEUIFrameControl::SetSize(Size);
	SetWidth(Size.x);
	SetHeight(Size.y);
}

void ZEUIButtonControl::SetText(const ZEString& Text)
{
	TextLabel->SetText(Text);
}

const ZEString& ZEUIButtonControl::GetText() const
{
	return TextLabel->GetText();
}

ZEUIButtonControl::ZEUIButtonControl()
{
	TextLabel = new ZEUILabel();
	TextLabel->SetWordWrapping(false);
	TextLabel->SetFontResource(ZEFontResourceDynamic::LoadSharedResource("Arial.ttf", 18));
	TextLabel->SetFontColor(ZEVector4::UnitW);
	TextLabel->SetBackgroundColor(ZEVector4::Zero);
	TextLabel->SetHeight(22);
	TextLabel->SetTextAlignment(ZE_UI_TA_CENTER);
	///TextLabel->SetText("Button");
	TextLabel->SetMoveable(false);
	SetMoveable(true);
	AddChildControl(TextLabel);
	SetHeight(25);
	SetWidth(80);
}

ZEUIButtonControl::~ZEUIButtonControl()
{

}
