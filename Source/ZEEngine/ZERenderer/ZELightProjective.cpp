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
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"

#define ZE_LDF_VIEW_TRANSFORM			1
#define ZE_LDF_PROJECTION_TRANSFORM		2
#define ZE_LDF_SHADOW_MAP				4
#define ZE_LDF_VIEW_VOLUME				8

void ZELightProjective::LoadProjectionTexture()
{
	if (ProjectionTextureFileName.IsEmpty())
		return;

	ZEGRTextureOptions TextureOptions;
	TextureOptions.Type = ZEGR_TT_2D;
	TextureOptions.CompressionFormat = ZEGR_TF_BC1_UNORM_SRGB;
	TextureOptions.GenerateMipMaps = true;
	TextureOptions.MaximumMipmapLevel = 0;
	TextureOptions.sRGB = true;
	ProjectionTexture = ZEGRTexture::LoadResourceShared(ProjectionTextureFileName, TextureOptions);
}

ZEEntityResult ZELightProjective::LoadInternal()
{
	ZE_ENTITY_LOAD_CHAIN(ZEEntity);

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
	FalloffExponent = 2.0f;
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

void ZELightProjective::SetProjectionTexture(const ZEGRTexture* Texture)
{
	if (ProjectionTexture == Texture)
		return;

	ProjectionTexture = Texture;
	ProjectionTextureFileName = Texture->GetFileName();
}

const ZEGRTexture* ZELightProjective::GetProjectionTexture() const
{
	return ProjectionTexture;
}

void ZELightProjective::SetFalloffExponent(float Exponent)
{
	FalloffExponent = Exponent;
}

float ZELightProjective::GetFalloffExponent() const
{
	return FalloffExponent;
}

const ZEViewVolume& ZELightProjective::GetViewVolume(ZESize Index) const
{
	if (DirtyFlags.GetFlags(ZE_LDF_VIEW_VOLUME))
	{
		ZEViewFrustum::Create(ViewVolume, GetWorldPosition(), GetWorldRotation(), FOV, AspectRatio, 0.1f, Range);
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

bool ZELightProjective::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!ZELight::PreRender(Parameters))
		return false;

	Command.Scene = GetScene();
	Command.PositionWorld = GetWorldPosition();
	Command.RotationWorld = GetWorldRotation();
	Command.Range = GetRange();
	Command.Color = GetColor() * GetIntensity();
	Command.FalloffExponent = GetFalloffExponent();
	Command.ProjectionTexture = GetProjectionTexture();
	Command.ViewProjectionTransform = GetProjectionTransform() * GetViewTransform();
	Command.CastShadow = GetCastsShadow();
	Command.ViewFrustum = static_cast<const ZEViewFrustum&>(GetViewVolume());
	Command.ShadowSampleCount = GetShadowSampleCount();
	Command.ShadowSampleLength = GetShadowSampleLength();
	Command.ShadowDepthBias = GetShadowDepthBias();
	Command.ShadowNormalBias = GetShadowNormalBias();
	Command.StageMask = ZERN_STAGE_LIGHTING | (GetCastsShadow() ? ZERN_STAGE_SHADOWING : 0);
	
	Parameters->Renderer->AddCommand(&Command);

	return true;
}

ZELightProjective* ZELightProjective::CreateInstance()
{
	return new ZELightProjective();
}
