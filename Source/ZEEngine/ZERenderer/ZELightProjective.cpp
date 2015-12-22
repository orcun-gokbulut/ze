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
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZERNStageShadowmapGeneration.h"
#include "ZERNRenderParameters.h"

#define ZE_LDF_VIEW_TRANSFORM			1
#define ZE_LDF_PROJECTION_TRANSFORM		2
#define ZE_LDF_SHADOW_MAP				4
#define ZE_LDF_VIEW_VOLUME				8

ZELightType ZELightProjective::GetLightType() const
{
	return ZE_LT_PROJECTIVE;
}

void ZELightProjective::SetFOV(float FOV)
{
	if(this->FOV == FOV)
		return;

	this->FOV = FOV;
	
	DirtyFlags.RaiseFlags(ZE_LDF_PROJECTION_TRANSFORM | ZE_LDF_VIEW_VOLUME);
}

float ZELightProjective::GetFOV() const
{
	return FOV;
}

void ZELightProjective::SetAspectRatio(float AspectRatio)
{
	if(this->AspectRatio == AspectRatio)
		return;

	this->AspectRatio = AspectRatio;

	DirtyFlags.RaiseFlags(ZE_LDF_PROJECTION_TRANSFORM | ZE_LDF_VIEW_VOLUME);
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
	if(FileName.GetLength() == 0 || ProjectionTextureFile == FileName)
		return;

	ProjectionTextureFile = FileName;

	if (ProjectionTextureResource != NULL)
		ProjectionTextureResource->Release();

	ProjectionTextureResource = ZETexture2DResource::LoadSharedResource(ProjectionTextureFile);
	if (ProjectionTextureResource != NULL)
		ProjectionTexture = ProjectionTextureResource->GetTexture2D();
	else
		zeError("Can not load projection texture.");
}

const ZEString& ZELightProjective::GetProjectionTextureFile() const
{
	return ProjectionTextureFile;
}

ZEGRTexture2D* ZELightProjective::GetShadowMap() const
{
	return ShadowMap;
}

ZESize ZELightProjective::GetViewCount()
{
	return 1;
}

const ZEViewVolume& ZELightProjective::GetViewVolume(ZESize Index)
{
	if (DirtyFlags.GetFlags(ZE_LDF_VIEW_VOLUME))
	{
		ViewVolume.Create(GetWorldPosition(), GetWorldRotation(), FOV, AspectRatio, 0.1f, Range);
		DirtyFlags.UnraiseFlags(ZE_LDF_VIEW_VOLUME);
	}

	return ViewVolume;
}

const ZEMatrix4x4& ZELightProjective::GetViewTransform(ZESize CascadeIndex)
{
	if(DirtyFlags.GetFlags(ZE_LDF_VIEW_TRANSFORM))
	{
		ZEMatrix4x4::CreateViewTransform(ViewTransform, GetWorldPosition(), GetWorldRotation());
		DirtyFlags.UnraiseFlags(ZE_LDF_VIEW_TRANSFORM);
	}

	return ViewTransform;
}

const ZEMatrix4x4& ZELightProjective::GetProjectionTransform(ZESize Index)
{
	if(DirtyFlags.GetFlags(ZE_LDF_PROJECTION_TRANSFORM))
	{
		ZEMatrix4x4::CreatePerspectiveProjection(ProjectionTransform, FOV, AspectRatio, 0.1f, Range);
		DirtyFlags.UnraiseFlags(ZE_LDF_PROJECTION_TRANSFORM);
	}

	return ProjectionTransform;
}

bool ZELightProjective::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	SetProjectionTextureFile(ProjectionTextureFile);

	ShadowMap = ZEGRTexture2D::CreateInstance(1024, 1024, 1, ZEGR_TF_R16_FLOAT, true);

	ShadowRenderer.SetOutputRenderTarget(ShadowMap->GetRenderTarget());
	ShadowRenderer.AddStage(new ZERNStageShadowmapGeneration());
	ShadowRenderer.Initialize();

	return true;
}

bool ZELightProjective::DeinitializeSelf()
{
	ProjectionTexture.Release();
	ShadowMap.Release();

	ShadowRenderer.Deinitialize();

	return ZEEntity::DeinitializeSelf();
}

ZELightProjective::ZELightProjective()
{
	FOV = ZE_PI_4;
	AspectRatio = 1.0f;
	ProjectionTextureResource = NULL;

	Command.Entity = this;
	Command.StageMask = ZERN_STAGE_SHADOWING;
}

ZELightProjective::~ZELightProjective()
{
	DeinitializeSelf();
}

bool ZELightProjective::PreRender(const ZERNCullParameters* CullParameters)
{
	return ZELight::PreRender(CullParameters);
}

void ZELightProjective::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	static bool Rendered = false;
	if(!Rendered)
	{
		Rendered = true;
	
		ZERNView View = ShadowRenderer.GetView();
		View.Position = GetWorldPosition();
		View.Rotation = GetWorldRotation();
		View.Direction = GetWorldFront();
		View.U = GetWorldRight();
		View.V = GetWorldUp();
		View.N = GetWorldFront();

		View.VerticalFOV = FOV;
		View.HorizontalFOV = FOV;
		View.AspectRatio = AspectRatio;

		View.NearZ = 0.1f;
		View.FarZ = GetRange();
		View.ViewVolume = &GetViewVolume();
		View.ViewTransform = GetViewTransform();
		View.ViewProjectionTransform = GetProjectionTransform() * View.ViewTransform;

		ShadowRenderer.SetView(View);
		ShadowRenderer.SetOutputRenderTarget(ShadowMap->GetRenderTarget());

		ZELight::Render(Parameters, Command);
	}
}

ZELightProjective* ZELightProjective::CreateInstance()
{
	return new ZELightProjective();
}
