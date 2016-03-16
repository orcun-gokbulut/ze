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
#include "ZERandom.h"
#include "ZEMath/ZEAngle.h"
#include "ZEGame/ZEScene.h"
#include "ZEGame/ZEEntityProvider.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRTextureCube.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZERNStageShadowmapGeneration.h"
#include "ZERNRenderParameters.h"
#include "ZETexture/ZETexture2DResource.h"

#define ZE_LDF_VIEW_TRANSFORM			1
#define ZE_LDF_PROJECTION_TRANSFORM		2
#define ZE_LDF_SHADOW_MAP				4
#define ZE_LDF_VIEW_VOLUME				8

void ZELightProjective::UpdateShadowMap()
{
	if(!DirtyFlags.GetFlags(ZE_LDF_SHADOW_MAP))
		return;

	ZEUInt Size = ZELight::ConvertShadowResolution(ShadowResolution);

	ShadowMap.Release();
	ShadowMap = ZEGRTexture2D::CreateInstance(Size, Size, 1, 1, 1, ZEGR_TF_D32_FLOAT, false, true);

	DirtyFlags.UnraiseFlags(ZE_LDF_SHADOW_MAP);
}

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

void ZELightProjective::SetProjectionTextureFilename(const ZEString& FileName)
{
	if(FileName.GetLength() == 0 || ProjectionTextureFilename == FileName)
		return;

	ProjectionTextureFilename = FileName;

	if (ProjectionTextureResource != NULL)
		ProjectionTextureResource->Release();

	ProjectionTextureResource = ZETexture2DResource::LoadSharedResource(ProjectionTextureFilename);
	if (ProjectionTextureResource != NULL)
		ProjectionTexture = ProjectionTextureResource->GetTexture2D();
	else
		zeError("Can not load projection texture.");
}

const ZEString& ZELightProjective::GetProjectionTextureFilename() const
{
	return ProjectionTextureFilename;
}

ZESize ZELightProjective::GetViewCount() const
{
	return 1;
}

const ZEViewVolume& ZELightProjective::GetViewVolume(ZESize Index) const
{
	if (DirtyFlags.GetFlags(ZE_LDF_VIEW_VOLUME))
	{
		ViewVolume.Create(GetWorldPosition(), GetWorldRotation(), FOV, AspectRatio, 0.1f, Range);
		DirtyFlags.UnraiseFlags(ZE_LDF_VIEW_VOLUME);
	}

	return ViewVolume;
}

ZEGRTexture* ZELightProjective::GetShadowMap(ZESize Index) const
{
	return ShadowMap;
}

const ZEMatrix4x4& ZELightProjective::GetViewTransform(ZESize Index) const
{
	if(DirtyFlags.GetFlags(ZE_LDF_VIEW_TRANSFORM))
	{
		ZEMatrix4x4::CreateViewTransform(ViewTransform, GetWorldPosition(), GetWorldRotation());
		DirtyFlags.UnraiseFlags(ZE_LDF_VIEW_TRANSFORM);
	}

	return ViewTransform;
}

const ZEMatrix4x4& ZELightProjective::GetProjectionTransform(ZESize Index) const
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

	SetProjectionTextureFilename(ProjectionTextureFilename);

	ShadowRenderer.AddStage(new ZERNStageShadowmapGeneration());
	ShadowRenderer.Initialize();

	DirtyFlags.RaiseAll();

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
	Command.Priority = 2;
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
	View.ViewProjectionTransform = GetProjectionTransform() * GetViewTransform();

	ShadowRenderer.SetView(View);

	UpdateShadowMap();

	ZEGRContext* Context = Parameters->Context;
	const ZEGRDepthStencilBuffer* DepthBuffer = ShadowMap->GetDepthStencilBuffer();

	Context->ClearDepthStencilBuffer(DepthBuffer, true, false, 1.0f, 0x00);
	Context->SetRenderTargets(0, NULL, DepthBuffer);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, DepthBuffer->GetWidth(), DepthBuffer->GetHeight()));

	ZELight::Render(Parameters, Command);

	Context->SetRenderTargets(0, NULL, NULL);
}

ZELightProjective* ZELightProjective::CreateInstance()
{
	return new ZELightProjective();
}
