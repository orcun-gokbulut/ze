//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIDebugDrawTag.cpp
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

#include "ZEUIDebugDrawTag.h"
#include "ZEDS/ZEString.h"
#include "ZEUI/ZEFontResource.h"
#include "ZEGame/ZEScene.h"
#include "ZERenderer/ZECamera.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEWindow.h"
#include "ZEInput/ZEInputDefinitions.h"
#include "ZEInput/ZEInputModule.h"
#include "ZEDS/ZEFormat.h"
#include "ZETexture/ZETexture2DResource.h"


void ZEUIDebugDrawTag::SetFontColor(ZEVector4 Color)
{
	this->FontColor = Color;

	Name->SetFontColor(FontColor);
	Type->SetFontColor(FontColor);
}

ZEVector4 ZEUIDebugDrawTag::GetFontColor() const
{
	return FontColor;
}

void ZEUIDebugDrawTag::SetNameField(const ZEString& Name)
{
	this->Name->SetText(Name);
}

void ZEUIDebugDrawTag::SetTypeField(const ZEString& Type)
{
	this->Type->SetText(Type);
}

void ZEUIDebugDrawTag::SetIcon(const ZEString& IconPath)
{
	ZEUIMaterial* IconMaterial = (ZEUIMaterial*)Icon->GetMaterial();
	IconMaterial->SetTexture(ZETexture2DResource::LoadSharedResource(IconPath.ToCString())->GetTexture2D());
}

void ZEUIDebugDrawTag::SetPosition(const ZEVector2& Position)
{
	ZEUIFrameControl::SetPosition(ZEVector2((Position.x - Icon->GetWidth() / 2.0f - 4.0f), (Position.y - Icon->GetHeight() / 2.0f - 4.0f)));
}

void ZEUIDebugDrawTag::Draw(ZEUIRenderer* Renderer)
{
	ZEUIFrameControl::Draw(Renderer);
}

void ZEUIDebugDrawTag::Tick(float ElapsedTime)
{
	ZEUIFrameControl::Tick(ElapsedTime);
}

ZEUIDebugDrawTag::ZEUIDebugDrawTag()
{
	SetBackgroundColor(ZEVector4::Zero);

	Name = new ZEUILabel();
	Name->SetWidth(320.0f);
	Name->SetHeight(14.0f);
	Name->SetPosition(ZEVector2(40.0f, 4.0f));
	Name->SetFontResource(ZEUIManager::GetDefaultFontResource());
	Name->SetFontColor(ZEVector4::One);
	Name->SetZOrder(0);
	AddChildControl(Name);

	Type = new ZEUILabel();
	Type->SetWidth(320.0f);
	Type->SetHeight(14.0f);
	Type->SetPosition(ZEVector2(40.0f, 22.0f));
	Type->SetFontResource(ZEUIManager::GetDefaultFontResource());
	Type->SetFontColor(ZEVector4::One);
	Type->SetZOrder(0);
	AddChildControl(Type);

	Icon = new ZEUIFrameControl();
	Icon->SetHeight(32.0f);
	Icon->SetWidth(32.0f);
	Icon->SetPosition(ZEVector2(4.0f, 4.0f));
	Icon->SetZOrder(0);
	Icon->SetVisiblity(false);
	Icon->SetEnabled(false);
	Icon->SetMoveable(false);
	AddChildControl(Icon);

	this->SetHeight(40.0f);
	this->SetWidth(364.0f);

}

ZEUIDebugDrawTag* ZEUIDebugDrawTag::CreateInstance()
{
	return new ZEUIDebugDrawTag(); 
}

