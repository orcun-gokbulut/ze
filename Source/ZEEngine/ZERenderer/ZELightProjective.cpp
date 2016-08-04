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

#include "ZERNStageShadowmapGeneration.h"
#include "ZERNRenderParameters.h"
#include "ZERNStageShadowing.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRTextureCube.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZETexture/ZETexture2DResource.h"

#define ZE_LDF_VIEW_TRANSFORM			1
#define ZE_LDF_PROJECTION_TRANSFORM		2
#define ZE_LDF_SHADOW_MAP				4
#define ZE_LDF_VIEW_VOLUME				8

void ZELightProjective::UpdateShadowMap()
{
	if (!DirtyFlags.GetFlags(ZE_LDF_SHADOW_MAP))
		return;

	ZEUInt Size = ZELight::ConvertShadowResolution(ShadowResolution);
	ShadowMap = ZEGRTexture2D::CreateResource(Size, Size, 1, ZEGR_TF_D32_FLOAT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_DEPTH_STENCIL);

	DirtyFlags.UnraiseFlags(ZE_LDF_SHADOW_MAP);
}

void ZELightProjective::LoadProjectionTexture()
{
	if (ProjectionTextureFileName.IsEmpty())
		return;

	ZEGRTexture2DOptions TextureOptions;
	TextureOptions.CompressionFormat = ZEGR_TF_BC1_UNORM_SRGB;
	TextureOptions.GenerateMipMaps = true;
	TextureOptions.MaximumMipmapLevel = 0;
	TextureOptions.sRGB = true;
	ProjectionTexture = ZEGRTexture2D::LoadResourceShared(ProjectionTextureFileName, TextureOptions);
}

ZEEntityResult ZELightProjective::LoadInternal()
{
	ZE_ENTITY_INITIALIZE_CHAIN(ZEEntity);

	LoadProjectionTexture();

	return ZE_ER_DONE;
}

ZEEntityResult ZELightProjective::UnloadInternal()
{
	ProjectionTexture.Release();
	ShadowMap.Release();

	ZE_ENTITY_UNLOAD_CHAIN(ZELight);
	return ZE_ER_DONE;
}

ZELightProjective::ZELightProjective()
{
	FOV = ZE_PI_4;
	AspectRatio = 1.0f;
	Command.Entity = this;
	Command.Priority = 1;
}

ZELightProjective::~ZELightProjective()
{

}

ZELightType ZELightProjective::GetLightType() const
{
	return ZE_LT_PROJECTIVE;
}

ZESize ZELightProjective::GetViewCount() const
{
	return 1;
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

void ZELightProjective::SetShadowMapIndex(ZEUInt ShadowMapIndex)
{
	this->ShadowMapIndex = ShadowMapIndex;
}

ZEUInt ZELightProjective::GetShadowMapIndex() const
{
	return ShadowMapIndex;
}

void ZELightProjective::SetProjectionTextureFile(const ZEString& FileName)
{
	if (FileName.IsEmpty() || ProjectionTextureFileName == FileName)
		return;

	ProjectionTextureFileName = FileName;

	if (IsLoadedOrLoading())
		LoadProjectionTexture();
}

const ZEString& ZELightProjective::GetProjectionTextureFile() const
{
	return ProjectionTextureFileName;
}

void ZELightProjective::SetProjectionTexture(const ZEGRTexture2D* Texture)
{
	if (ProjectionTexture == Texture)
		return;

	ProjectionTexture = Texture;
	ProjectionTextureFileName = Texture->GetFileName();
}

const ZEGRTexture2D* ZELightProjective::GetProjectionTexture() const
{
	return ProjectionTexture;
}

ZEGRTexture* ZELightProjective::GetShadowMap(ZESize Index) const
{
	return ShadowMap;
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

const ZEMatrix4x4& ZELightProjective::GetViewTransform(ZESize Index) const
{
	if (DirtyFlags.GetFlags(ZE_LDF_VIEW_TRANSFORM))
	{
		ZEMatrix4x4::CreateViewTransform(ViewTransform, GetWorldPosition(), GetWorldRotation());
		DirtyFlags.UnraiseFlags(ZE_LDF_VIEW_TRANSFORM);
	}

	return ViewTransform;
}

const ZEMatrix4x4& ZELightProjective::GetProjectionTransform(ZESize Index) const
{
	if (DirtyFlags.GetFlags(ZE_LDF_PROJECTION_TRANSFORM))
	{
		ZEMatrix4x4::CreatePerspectiveProjection(ProjectionTransform, FOV, AspectRatio, 0.1f, Range);
		DirtyFlags.UnraiseFlags(ZE_LDF_PROJECTION_TRANSFORM);
	}

	return ProjectionTransform;
}

void ZELightProjective::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (Parameters->Stage->GetId() != ZERN_STAGE_SHADOWING)
		return;

	if (GetScene() == NULL)
		return;

	UpdateShadowMap();

	ZERNView View = ShadowRenderer.GetView();
	View.Position = Parameters->View->Position;
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

	ZERNPreRenderParameters PreRenderParameters;
	PreRenderParameters.Renderer = &ShadowRenderer;
	PreRenderParameters.View = &View;
	PreRenderParameters.Type = ZERN_PRT_SHADOW;

	ZEGRContext* Context = Parameters->Context;
	ShadowRenderer.SetContext(Context);
	ShadowRenderer.SetView(View);
	ShadowRenderer.StartScene(GetScene()->GetConstantBuffer());
		GetScene()->PreRender(&PreRenderParameters);
	ShadowRenderer.EndScene();

	const ZERNStageShadowing* StageShadowing = static_cast<const ZERNStageShadowing*>(Parameters->Stage);
	this->ShadowMapIndex = StageShadowing->ShadowMapCount;
	const ZEGRDepthStencilBuffer* DepthBuffer = StageShadowing->ProjectiveShadowMaps->GetDepthStencilBuffer(false, const_cast<ZERNStageShadowing*>(StageShadowing)->ShadowMapCount++);

	Context->ClearDepthStencilBuffer(DepthBuffer, true, true, 0.0f, 0x00);
	Context->SetRenderTargets(0, NULL, DepthBuffer);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, DepthBuffer->GetWidth(), DepthBuffer->GetHeight()));
	
	ShadowRenderer.Render();
}

ZELightProjective* ZELightProjective::CreateInstance()
{
	return new ZELightProjective();
}
