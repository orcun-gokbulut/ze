//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELightProjective.cpp
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

#include "ZELightProjective.h"

#include "ZEError.h"
#include "ZEMath/ZEAngle.h"
#include "ZEGame/ZEScene.h"
#include "ZEGame/ZEEntityProvider.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGRTextureCube.h"
#include "ZETexture/ZETexture2DResource.h"

ZELightType ZELightProjective::GetLightType()
{
	return ZE_LT_PROJECTIVE;
}

void ZELightProjective::SetFOV(float FOV)
{
	this->FOV = FOV;
}

float ZELightProjective::GetFOV() const
{
	return FOV;
}

void ZELightProjective::SetAspectRatio(float AspectRatio)
{
	this->AspectRatio = AspectRatio;
}

float ZELightProjective::GetAspectRatio() const
{
	return AspectRatio;
}

void ZELightProjective::SetProjectionTexture(ZEGRTexture2D* Texture)
{
	ProjectionTexture = Texture;
}

ZEGRTexture2D* ZELightProjective::GetProjectionTexture() const
{
	return ProjectionTexture;
}

void ZELightProjective::SetProjectionTextureFile(const ZEString& FileName)
{
	ProjectionTextureFile = FileName;
	if (IsInitialized())
	{
		if (ProjectionTextureResource != NULL)
			ProjectionTextureResource->Release();

		ProjectionTextureResource = ZETexture2DResource::LoadSharedResource(ProjectionTextureFile);
		if (ProjectionTextureResource != NULL)
			ProjectionTexture = ProjectionTextureResource->GetTexture2D();
		else
			zeError("Can not load projection texture.");
	}
}

const ZEString& ZELightProjective::GetProjectionTextureFile() const
{
	return ProjectionTextureFile;
}

ZEGRTexture2D* ZELightProjective::GetShadowMap()
{
	return ShadowMap;
}

ZESize ZELightProjective::GetViewCount()
{
	return 1;
}

const ZEViewVolume& ZELightProjective::GetViewVolume(ZESize Index)
{
	if (UpdateViewVolume)
	{
		ViewVolume.Create(GetWorldPosition(), GetWorldRotation(), FOV, AspectRatio, 0.0f, GetRange());
		UpdateViewVolume = false;
	}

	return ViewVolume;
}

const ZEMatrix4x4& ZELightProjective::GetViewTransform(ZESize CascadeIndex)
{
	return ViewProjectionMatrix;
}

bool ZELightProjective::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	if (strlen(ProjectionTextureFile) != 0)
	{
		if (ProjectionTextureResource != NULL)
			ProjectionTextureResource->Release();

		ProjectionTextureResource = ZETexture2DResource::LoadSharedResource(ProjectionTextureFile);
		if (ProjectionTextureResource != NULL)
			ProjectionTexture = ProjectionTextureResource->GetTexture2D();
		else
			zeError("Can not load projection texture.");
	}

	return true;
}

bool ZELightProjective::DeinitializeSelf()
{
	if (ProjectionTextureResource != NULL)
	{
		ProjectionTextureResource->Release();
		ProjectionTextureResource = NULL;
	}

	return ZEEntity::DeinitializeSelf();
}

ZELightProjective::ZELightProjective()
{
	FOV = ZE_PI_2;
	AspectRatio = 1.0f;
	ShadowMap = NULL;
	ProjectionTexture = NULL;
	ProjectionTextureResource = NULL;
	ViewProjectionMatrix = ZEMatrix4x4::Identity;
}

ZELightProjective::~ZELightProjective()
{

}

ZELightProjective* ZELightProjective::CreateInstance()
{
	return new ZELightProjective();
}
