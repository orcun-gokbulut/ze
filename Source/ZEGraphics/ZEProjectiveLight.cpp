//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEProjectiveLight.cpp
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

#include "ZEProjectiveLight.h"
#include "ZETexture2D.h"
#include "ZETextureCube.h"
#include "ZECore\ZEError.h"
#include "ZEGame\ZEScene.h"
#include "ZEShadowRenderer.h"
#include "ZEGraphicsModule.h"

ZELightType ZEProjectiveLight::GetLightType()
{
	return ZE_LT_PROJECTIVE;
}

void ZEProjectiveLight::SetFOV(float FOV)
{
	this->FOV = FOV;
}

float ZEProjectiveLight::GetFOV() const
{
	return FOV;
}

void ZEProjectiveLight::SetAspectRatio(float AspectRatio)
{
	this->AspectRatio = AspectRatio;
}

float ZEProjectiveLight::GetAspectRatio() const
{
	return AspectRatio;
}

void ZEProjectiveLight::SetProjectionTexture(const ZETexture2D* Texture)
{
	ProjectionMap = Texture;
}

const ZETexture2D* ZEProjectiveLight::GetProjectionTexture()
{
	return ProjectionMap;
}

const ZEMatrix4x4& ZEProjectiveLight::GetProjectionMatrix()
{
	return ZEMatrix4x4::Identity;
}

ZETexture2D* ZEProjectiveLight::GetShadowMap()
{
	return ShadowMap;
}

void ZEProjectiveLight::SetShadowMap(int Width, int Height)
{
	if (ShadowMap != NULL)
		ShadowMap->Release();
	else
		ShadowMap = ZETexture2D::CreateInstance();

	if (!ShadowMap->Create(Width, Height, ZE_TPF_SHADOW_MAP, true))
	{
		zeError("Projective Light", "Can not create shadow map texture.");
		return;
	}
}

void ZEProjectiveLight::RenderShadowMap(ZEScene* Scene, ZEShadowRenderer* ShadowRenderer)
{
	if (!GetCastsShadow() || ShadowMap == NULL || ShadowMap->IsEmpty())
		return;

	ShadowRenderer->SetLight(this);
	ShadowRenderer->SetViewPort(ShadowMap->GetViewPort());

	ShadowRenderer->ClearList();

	Scene->CullScene((ZERenderer*)ShadowRenderer, GetViewVolume(), false);
	ShadowRenderer->Render();
}

const ZEViewVolume& ZEProjectiveLight::GetViewVolume()
{
	if (UpdateViewVolume)
	{
		ViewVolume.Create(GetWorldPosition(), GetWorldRotation(), FOV, AspectRatio, 0.0f, GetRange());
		UpdateViewVolume = false;
	}

	return ViewVolume;
}

ZEProjectiveLight::ZEProjectiveLight()
{
	ShadowMap = NULL;
}

ZEProjectiveLight::~ZEProjectiveLight()
{
	if (ShadowMap != NULL)
		ShadowMap->Destroy();
}

#include "ZEProjectiveLight.h.zpp"
