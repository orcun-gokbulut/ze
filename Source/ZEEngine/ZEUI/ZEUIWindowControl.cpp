//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIWindowControl.cpp
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

#include "ZEUIWindowControl.h"
#include "ZETexture/ZETexture2DResource.h"

void ZEUIWindowTitleBarControl::MouseMoveEvent(const ZEVector2& MoveAmount)
{
	if (!GetMoveable())
		return;

	if (GetParentControl() != NULL)
		GetParentControl()->SetPosition(GetParentControl()->GetPosition() + MoveAmount);
}

void ZEUIWindowTitleBarControl::SetWidth(float Width)
{
	ZEUIFrameControl::SetWidth(Width);
	CloseButton.SetPosition(ZEVector2(GetRectangle().RightDown.x - CloseButton.GetWidth() - 2, GetPosition().y + 2));
	MinimizeButton.SetPosition(ZEVector2(CloseButton.GetPosition().x - 2 - MinimizeButton.GetWidth(), CloseButton.GetPosition().y));
}

void ZEUIWindowTitleBarControl::SetTitleText(const char* TitleText)
{
	Title.SetText(TitleText);
}

const char* ZEUIWindowTitleBarControl::GetTitleText()
{
	return Title.GetText();
}

ZEUIWindowTitleBarControl::ZEUIWindowTitleBarControl()
{
	CloseButton.SetWidth(20);
	CloseButton.SetHeight(20);

	MinimizeButton.SetWidth(20);
	MinimizeButton.SetHeight(20);

	((ZEUIMaterial*)(MinimizeButton.GetMaterial()))->SetTexture(ZETexture2DResource::LoadSharedResource("ZEEngine/ZEGUI/Textures/Minimize.png")->GetTexture());
	((ZEUIMaterial*)(CloseButton.GetMaterial()))->SetTexture(ZETexture2DResource::LoadSharedResource("ZEEngine/ZEGUI/Textures/Close.png")->GetTexture());
	((ZEUIMaterial*)(GetMaterial()))->SetTexture(ZETexture2DResource::LoadSharedResource("ZEEngine/ZEGUI/Textures/Frame.png")->GetTexture());

	Frame.Color = ZEVector4(0.8f, 0.8f, 0.8f, 1.0f);

	SetWidth(200);
	SetHeight(25);

	Title.SetPosition(GetPosition());

	AddChildControl(&Title);
	AddChildControl(&MinimizeButton);
	AddChildControl(&CloseButton);	
	SetMoveable(true);
}

void ZEUIWindowControl::ResizeWindow(const ZEVector2& ResizeAmount)
{
	if(GetFixedSized())
		return;

	SetWidth(GetWidth() + ResizeAmount.x);
	SetHeight(GetHeight() + ResizeAmount.y);
}

void ZEUIWindowControl::CloseWindow(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	SetVisiblity(false);
}

void ZEUIWindowControl::HideContentArea(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	if (ContentArea.GetVisiblity())
	{
		ZEUIControl::SetHeight(TitleBar.GetHeight());
		ContentArea.SetVisiblity(false);
		((ZEUIMaterial*)(TitleBar.MinimizeButton.GetMaterial()))->SetTexture(ZETexture2DResource::LoadSharedResource("ZEEngine/ZEGUI/Textures/Maximize.png")->GetTexture());
	}

	else
	{
		SetHeight(TitleBar.GetHeight() + ContentArea.GetHeight());
		ContentArea.SetVisiblity(true);
		((ZEUIMaterial*)(TitleBar.MinimizeButton.GetMaterial()))->SetTexture(ZETexture2DResource::LoadSharedResource("ZEEngine/ZEGUI/Textures/Minimize.png")->GetTexture());
	}
}

void ZEUIWindowControl::SetWidth(float Width)
{
	ZEUIControl::SetWidth(Width);
	TitleBar.SetWidth(GetWidth());
	ContentArea.SetWidth(GetWidth());
	ResizeButton.SetPosition(ZEVector2(GetRectangle().RightDown.x - ResizeButton.GetWidth(), GetRectangle().RightDown.y - ResizeButton.GetHeight()));
}

void ZEUIWindowControl::SetHeight(float Height)
{
	ZEUIControl::SetHeight(Height);
	ContentArea.SetHeight(GetHeight() - TitleBar.GetHeight());
	ResizeButton.SetPosition(ZEVector2(GetRectangle().RightDown.x - ResizeButton.GetWidth(), GetRectangle().RightDown.y - ResizeButton.GetHeight()));
}

void ZEUIWindowControl::AddChildControl(ZEUIControl* Control)
{
	ContentArea.AddChildControl(Control);
}

void ZEUIWindowControl::SetMoveable(bool Moveable)
{
	TitleBar.SetMoveable(Moveable);
	ZEUIControl::SetMoveable(Moveable);
}

void ZEUIWindowControl::SetMaterial(ZEMaterial* Material)
{
	
}

ZEMaterial* ZEUIWindowControl::GetMaterial() const
{
	return NULL;
}

ZEUIWindowControl::ZEUIWindowControl()
{
	SetPosition(ZEVector2::Zero);
	TitleBar.SetPosition(ZEVector2::Zero);
	ContentArea.SetPosition(ZEVector2(0.0f, TitleBar.GetHeight()));

	ZEUIControl::AddChildControl(&TitleBar);
	ZEUIControl::AddChildControl(&ContentArea);
	ContentArea.AddChildControl(&ResizeButton);
	((ZEUIMaterial*)(ContentArea.GetMaterial()))->SetTexture(ZETexture2DResource::LoadResource("ZEEngine/ZEGUI/Textures/Frame.png")->GetTexture());
	((ZEUIMaterial*)(ResizeButton.GetMaterial()))->SetTexture(ZETexture2DResource::LoadResource("ZEEngine/ZEGUI/Textures/Resize.png")->GetTexture());

	ResizeButton.SetWidth(10);
	ResizeButton.SetHeight(10);

	SetMoveable(true);
	ContentArea.SetMoveable(false);
	ResizeButton.SetMoveable(true);
	SetFixedSized(false);

	SetWidth(200);
	SetHeight(200);

	ResizeButton.SetMouseMovedEvent(BindDelegate(this, &ZEUIWindowControl::ResizeWindow));
	TitleBar.CloseButton.SetMouseButtonPressedEvent(BindDelegate(this, &ZEUIWindowControl::CloseWindow));
	TitleBar.MinimizeButton.SetMouseButtonPressedEvent(BindDelegate(this, &ZEUIWindowControl::HideContentArea));

	ZEUIButtonControl* TestB = new ZEUIButtonControl();
	TestB->SetPosition(ZEVector2(5,30));
	AddChildControl(TestB);
	SetMinimumSize(ZEVector2(50,50));
}
