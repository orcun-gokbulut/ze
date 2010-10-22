//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEOmniProjectiveLight.cpp
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

#include "ZEOmniProjectiveLight.h"
#include "ZEGame\ZEScene.h"
#include "ZEShadowRenderer.h"
#include "ZETexture.h"
#include "ZETexture2D.h"

ZELightType ZEOmniProjectiveLight::GetLightType()
{
	return ZE_LT_OMNIPROJECTIVE;
}

const ZETextureCube* ZEOmniProjectiveLight::GetProjectionTexture()
{
	return ProjectionTexture;
}

void ZEOmniProjectiveLight::SetProjectionTexture(const ZETextureCube* Texture)
{
	ProjectionTexture = Texture;
}

void ZEOmniProjectiveLight::SetCastsShadow(bool NewValue)
{
	if (NewValue == false)
	{
		if (FrontShadowMap != NULL)
		{
			FrontShadowMap->Destroy();
			FrontShadowMap = NULL;
		}

		if (BackShadowMap != NULL)
		{
			BackShadowMap->Destroy();
			BackShadowMap = NULL;
		}
	}

	ZELight::SetCastsShadow(NewValue);
}

void ZEOmniProjectiveLight::Deinitialize()
{
	if (FrontShadowMap != NULL)
	{
		FrontShadowMap->Destroy();
		FrontShadowMap = NULL;
	}

	if (BackShadowMap != NULL)
	{
		BackShadowMap->Destroy();
		BackShadowMap = NULL;
	}
}

ZETexture2D* ZEOmniProjectiveLight::GetFrontShadowMap()
{
	return FrontShadowMap;
}

ZETexture2D* ZEOmniProjectiveLight::GetBackShadowMap()
{
	return BackShadowMap;
}

void ZEOmniProjectiveLight::RenderShadowMap(ZEScene* Scene, ZEShadowRenderer* ShadowRenderer)
{
	if (FrontShadowMap == NULL)
	{
		FrontShadowMap = ZETexture2D::CreateInstance();
		FrontShadowMap->Create(512, 512, ZE_TPF_SHADOW_MAP, false);
	}

	if (BackShadowMap == NULL)
	{
		BackShadowMap = ZETexture2D::CreateInstance();
		BackShadowMap->Create(512, 512, ZE_TPF_SHADOW_MAP, false);
	}
}

const ZEViewVolume& ZEOmniProjectiveLight::GetViewVolume()
{
	if (UpdateViewVolume)
	{
		ViewVolume.Create(GetWorldPosition(), GetRange(), 0.0f);
		UpdateViewVolume = false;
	}

	return ViewVolume;
}

ZEOmniProjectiveLight::ZEOmniProjectiveLight()
{
	ProjectionTexture = NULL;

	FrontShadowMap = NULL;
	BackShadowMap = NULL;
}

ZEOmniProjectiveLight::~ZEOmniProjectiveLight()
{
	Deinitialize();
}

#include "ZEOmniProjectiveLight.h.zpp"
