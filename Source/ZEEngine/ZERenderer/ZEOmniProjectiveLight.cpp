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
#include "ZEGame/ZEScene.h"
#include "ZEShadowRenderer.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGame/ZEEntityProvider.h"

ZELightType ZEOmniProjectiveLight::GetLightType()
{
	return ZE_LT_OMNIPROJECTIVE;
}

const ZEGRTextureCube* ZEOmniProjectiveLight::GetProjectionTexture()
{
	return ProjectionTexture;
}

void ZEOmniProjectiveLight::SetProjectionTexture(const ZEGRTextureCube* Texture)
{
	ProjectionTexture = Texture;
}

void ZEOmniProjectiveLight::SetCastsShadow(bool NewValue)
{
	ZELight::SetCastsShadow(NewValue);
}

bool ZEOmniProjectiveLight::DeinitializeSelf()
{
	return ZELight::DeinitializeSelf();
}

ZESize ZEOmniProjectiveLight::GetViewCount()
{
	return 1;
}

const ZEViewVolume& ZEOmniProjectiveLight::GetViewVolume(ZESize Index)
{
	if (UpdateViewVolume)
	{
		ViewVolume.Create(GetWorldPosition(), GetRange(), 0.0f);
		UpdateViewVolume = false;
	}

	return ViewVolume;
}

const ZEMatrix4x4& ZEOmniProjectiveLight::GetViewTransform(ZESize Index)
{	
	return ViewProjectionMatrix;
}

void ZEOmniProjectiveLight::Draw(ZEDrawParameters* DrawParameters)
{
	if (DrawParameters->Pass != ZE_RP_COLOR)
		return;

	ZEBSphere LightBoundingSphere;
	LightBoundingSphere.Position = GetWorldPosition();
	LightBoundingSphere.Radius = GetRange();

	if (!DrawParameters->ViewVolume->CullTest(LightBoundingSphere))
		ZELight::Draw(DrawParameters);
}

ZEOmniProjectiveLight::ZEOmniProjectiveLight()
{
	ProjectionTexture = NULL;

	FrontShadowMap = NULL;
	BackShadowMap = NULL;
	ViewProjectionMatrix = ZEMatrix4x4::Identity;
}

ZEOmniProjectiveLight::~ZEOmniProjectiveLight()
{

}

ZEOmniProjectiveLight* ZEOmniProjectiveLight::CreateInstance()
{
	return new ZEOmniProjectiveLight();
}
