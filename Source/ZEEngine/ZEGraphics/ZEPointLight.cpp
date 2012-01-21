//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPointLight.cpp
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

#include "ZEPointLight.h"
#include "ZETexture2D.h"
#include "ZEShadowRenderer.h"
#include "ZEGame/ZEScene.h"
#include "ZEGame/ZEEntityProvider.h"

ZE_META_REGISTER_CLASS(ZEEntityProvider, ZEPointLight);

ZELightType ZEPointLight::GetLightType()
{
	return ZE_LT_POINT;
}

void ZEPointLight::SetCastShadows(bool NewValue)
{
	if (NewValue == false)
	{
		if (FrontShadowMap)
		{
			FrontShadowMap->Destroy();
			FrontShadowMap = NULL;
		}

		if (BackShadowMap)
		{
			BackShadowMap->Destroy();
			BackShadowMap = NULL;
		}
	}

	ZELight::SetCastsShadow(NewValue);
}

ZETexture2D* ZEPointLight::GetFrontShadowMap()
{
	return FrontShadowMap;
}

ZETexture2D* ZEPointLight::GetBackShadowMap()
{
	return FrontShadowMap;
}

void ZEPointLight::Deinitialize()
{
	if (FrontShadowMap)
	{
		FrontShadowMap->Destroy();
		FrontShadowMap = NULL;
	}

	if (BackShadowMap)
	{
		BackShadowMap->Destroy();
		BackShadowMap = NULL;
	}
}

void ZEPointLight::RenderShadowMap(ZEScene* Scene, ZEShadowRenderer* ShadowRenderer)
{
	if (!CastsShadows)
		return;

	if (FrontShadowMap == NULL)
	{
		FrontShadowMap = ZETexture2D::CreateInstance();
		FrontShadowMap->Create(512, 512, ZE_TPF_F32, true);
	}

	if (BackShadowMap == NULL)
	{
		BackShadowMap = ZETexture2D::CreateInstance();
		BackShadowMap->Create(512, 512, ZE_TPF_F32, true);
	}

	/*
	ShadowRenderer->SetLight(this);

	ShadowRenderer->SetFace(true);
	ShadowRenderer->SetViewPort(FrontShadowMap->GetViewPort());
	ShadowRenderer->ClearList();
	Scene->CullScene((ZERenderer*)ShadowRenderer, GetViewVolume(), false);
	ShadowRenderer->Render();

	ShadowRenderer->SetFace(false);
	ShadowRenderer->SetViewPort(BackShadowMap->GetViewPort());
	ShadowRenderer->ClearLists();
	Scene->CullScene((ZERenderer*)ShadowRenderer, GetViewVolume(), false);
	ShadowRenderer->Render();
	*/
}

const ZEViewVolume& ZEPointLight::GetViewVolume()
{
	if (UpdateViewVolume)
	{
		ViewVolume.Create(GetWorldPosition(), GetRange(), 0.0f);
		UpdateViewVolume = false;
	}
	
	return ViewVolume;
}

ZEPointLight::ZEPointLight()
{
	FrontShadowMap = NULL;
	BackShadowMap = NULL;
}

ZEPointLight::~ZEPointLight()
{
	Deinitialize();
}

ZEPointLight* ZEPointLight::CreateInstance()
{
	return new ZEPointLight();
}