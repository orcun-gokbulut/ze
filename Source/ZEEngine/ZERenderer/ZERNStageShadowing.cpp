//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageShadowing.cpp
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

#include "ZERNStageShadowing.h"

#include "ZEDS/ZEString.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEFrustum.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZERNStageID.h"
#include "ZERNRenderParameters.h"
#include "ZERNShaderSlots.h"
#include "ZERNStageShadowmapGeneration.h"
#include "ZELightSpot.h"
#include "ZELightDirectional.h"
#include "ZELightProjective.h"
#include "ZERNShading.h"

#define ZERN_SSDF_OUTPUT0		1
#define ZERN_SSDF_OUTPUT1		2
#define ZERN_SSDF_OUTPUT2		4
#define ZERN_SSDF_OUTPUT_ALL	(ZERN_SSDF_OUTPUT0 | ZERN_SSDF_OUTPUT1 | ZERN_SSDF_OUTPUT2)

bool ZERNStageShadowing::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;

	ShadowRenderer.AddStage(new ZERNStageShadowmapGeneration());
	ShadowRenderer.SetContext(ZEGRGraphicsModule::GetInstance()->GetMainContext());
	ShadowRenderer.Initialize();

	return true;
}

bool ZERNStageShadowing::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();
	ShadowRenderer.Deinitialize();

	DirectionalShadowMaps.Release();
	SpotShadowMaps.Release();
	ProjectiveShadowMaps.Release();

	return ZERNStage::DeinitializeInternal();
}

void ZERNStageShadowing::GenerateDirectionalShadowMaps(ZEGRContext* Context, const ZERNCommandDirectionalLight& DirectionalLight)
{
	if ((DirectionalShadowMapIndex + DirectionalLight.Cascades.GetCount()) > (DirectionalShadowMapCount * CascadeCount))
		return;

	ZERNView View;
	View.Position = GetRenderer()->GetView().Position;

	ze_for_each(Cascade, DirectionalLight.Cascades)
	{
		View.ViewVolume = &Cascade->ViewVolume;
		View.ViewProjectionTransform = Cascade->ProjectionTransform * Cascade->ViewTransform;
		ShadowRenderer.SetView(View);

		ZERNPreRenderParameters PreRenderParameters;
		PreRenderParameters.Renderer = &ShadowRenderer;
		PreRenderParameters.View = &View;
		PreRenderParameters.Type = ZERN_RT_SHADOW;

		GetRenderer()->BeginNestedRenderer();
		DirectionalLight.Scene->PreRender(&PreRenderParameters);

		const ZEGRDepthStencilBuffer* DepthBuffer = DirectionalShadowMaps->GetDepthStencilBuffer(false, DirectionalShadowMapIndex + Cascade.GetIndex());
		Context->ClearDepthStencilBuffer(DepthBuffer, true, true, 0.0f, 0x00);
		Context->SetRenderTargets(0, NULL, DepthBuffer);
		Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, DepthBuffer->GetWidth(), DepthBuffer->GetHeight()));

		ShadowRenderer.Render();
		GetRenderer()->EndNestedRenderer();
	}

	DirectionalShadowMapIndex += DirectionalLight.Cascades.GetCount();
}

void ZERNStageShadowing::GenerateSpotShadowMaps(ZEGRContext* Context, const ZERNCommandSpotLightShadow& SpotLight)
{
	if (SpotShadowMapIndex >= SpotShadowMapCount)
		return;

	ZERNView View;
	View.Position = GetRenderer()->GetView().Position;
	View.ViewVolume = &SpotLight.ViewFrustum;
	View.ViewProjectionTransform = SpotLight.ViewProjectionTransform;
	ShadowRenderer.SetView(View);

	ZERNPreRenderParameters PreRenderParameters;
	PreRenderParameters.Renderer = &ShadowRenderer;
	PreRenderParameters.View = &View;
	PreRenderParameters.Type = ZERN_RT_SHADOW;

	GetRenderer()->BeginNestedRenderer();
	SpotLight.Scene->PreRender(&PreRenderParameters);

	const ZEGRDepthStencilBuffer* DepthBuffer = SpotShadowMaps->GetDepthStencilBuffer(false, SpotShadowMapIndex);
	Context->ClearDepthStencilBuffer(DepthBuffer, true, true, 0.0f, 0x00);
	Context->SetRenderTargets(0, NULL, DepthBuffer);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, DepthBuffer->GetWidth(), DepthBuffer->GetHeight()));

	ShadowRenderer.Render();
	GetRenderer()->EndNestedRenderer();

	SpotShadowMapIndex++;
}

void ZERNStageShadowing::GenerateProjectiveShadowMaps(ZEGRContext* Context, const ZERNCommandProjectiveLight& ProjectiveLight)
{
	if (ProjectiveShadowMapIndex >= ProjectiveShadowMapCount)
		return;

	ZERNView View;
	View.Position = GetRenderer()->GetView().Position;
	View.ViewVolume = &ProjectiveLight.ViewFrustum;
	View.ViewProjectionTransform = ProjectiveLight.ViewProjectionTransform;
	ShadowRenderer.SetView(View);

	ZERNPreRenderParameters PreRenderParameters;
	PreRenderParameters.Renderer = &ShadowRenderer;
	PreRenderParameters.View = &View;
	PreRenderParameters.Type = ZERN_RT_SHADOW;

	GetRenderer()->BeginNestedRenderer();
	ProjectiveLight.Scene->PreRender(&PreRenderParameters);

	const ZEGRDepthStencilBuffer* DepthBuffer = ProjectiveShadowMaps->GetDepthStencilBuffer(false, ProjectiveShadowMapIndex);
	Context->ClearDepthStencilBuffer(DepthBuffer, true, true, 0.0f, 0x00);
	Context->SetRenderTargets(0, NULL, DepthBuffer);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, DepthBuffer->GetWidth(), DepthBuffer->GetHeight()));

	ShadowRenderer.Render();
	GetRenderer()->EndNestedRenderer();

	ProjectiveShadowMapIndex++;
}

void ZERNStageShadowing::CreateOutput(const ZEString& Name)
{
	if (Name == "DirectionalShadowMaps")
	{
		if (DirtyFlags.GetFlags(ZERN_SSDF_OUTPUT0))
		{
			ZEUInt ShadowMapRes = ZELight::ConvertShadowResolution(DirectionalShadowMapsResolution);
			ZEUInt ArrayCount = DirectionalShadowMapCount * CascadeCount;
			DirectionalShadowMaps = ZEGRTexture::CreateResource(ZEGR_TT_2D, ShadowMapRes, ShadowMapRes, 1, ZEGR_TF_D32_FLOAT, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_DEPTH_STENCIL, ArrayCount);
			DirtyFlags.UnraiseFlags(ZERN_SSDF_OUTPUT0);
		}
	}
	else if (Name == "SpotShadowMaps")
	{
		if (DirtyFlags.GetFlags(ZERN_SSDF_OUTPUT1))
		{
			ZEUInt ShadowMapRes = ZELight::ConvertShadowResolution(SpotShadowMapsResolution);
			ZEUInt ArrayCount = SpotShadowMapCount;
			SpotShadowMaps = ZEGRTexture::CreateResource(ZEGR_TT_2D, ShadowMapRes, ShadowMapRes, 1, ZEGR_TF_D32_FLOAT, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_DEPTH_STENCIL, ArrayCount);
			DirtyFlags.UnraiseFlags(ZERN_SSDF_OUTPUT1);
		}
	}
	else if (Name == "ProjectiveShadowMaps")
	{
		if (DirtyFlags.GetFlags(ZERN_SSDF_OUTPUT2))
		{
			ZEUInt ShadowMapRes = ZELight::ConvertShadowResolution(ProjectiveShadowMapsResolution);
			ZEUInt ArrayCount = ProjectiveShadowMapCount;
			ProjectiveShadowMaps = ZEGRTexture::CreateResource(ZEGR_TT_2D, ShadowMapRes, ShadowMapRes, 1, ZEGR_TF_D32_FLOAT, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_DEPTH_STENCIL, ArrayCount);
			DirtyFlags.UnraiseFlags(ZERN_SSDF_OUTPUT2);
		}
	}
}

ZEInt ZERNStageShadowing::GetId() const
{
	return ZERN_STAGE_SHADOWING;
}

const ZEString& ZERNStageShadowing::GetName() const
{
	static const ZEString Name = "Stage Shadowing";
	return Name;
}

void ZERNStageShadowing::SetCascadeCount(ZEUInt Count)
{
	if (CascadeCount == Count)
		return;
	
	CascadeCount = ZEMath::Min(Count, (ZEUInt)MAX_CASCADE);

	DirtyFlags.RaiseFlags(ZERN_SSDF_OUTPUT0);
}

ZEUInt ZERNStageShadowing::GetCascadeCount() const
{
	return CascadeCount;
}

void ZERNStageShadowing::SetDirectionalShadowMapCount(ZEUInt Count)
{
	if (this->DirectionalShadowMapCount == Count)
		return;

	this->DirectionalShadowMapCount = ZEMath::Min(Count, (ZEUInt)MAX_DIRECTIONAL_LIGHT_SHADOW);

	DirtyFlags.RaiseFlags(ZERN_SSDF_OUTPUT0);
}

ZEUInt ZERNStageShadowing::GetDirectionalShadowMapCount() const
{
	return DirectionalShadowMapCount;
}

void ZERNStageShadowing::SetSpotShadowMapCount(ZEUInt Count)
{
	if (this->SpotShadowMapCount == Count)
		return;

	this->SpotShadowMapCount = ZEMath::Min(Count, (ZEUInt)MAX_SPOT_LIGHT_SHADOW);

	DirtyFlags.RaiseFlags(ZERN_SSDF_OUTPUT1);
}

ZEUInt ZERNStageShadowing::GetSpotShadowMapCount() const
{
	return SpotShadowMapCount;
}

void ZERNStageShadowing::SetProjectiveShadowMapCount(ZEUInt Count)
{
	if (ProjectiveShadowMapCount == Count)
		return;

	ProjectiveShadowMapCount = ZEMath::Min(Count, (ZEUInt)MAX_PROJECTIVE_LIGHT_SHADOW);

	DirtyFlags.RaiseFlags(ZERN_SSDF_OUTPUT2);
}

ZEUInt ZERNStageShadowing::GetProjectiveShadowMapCount() const
{
	return ProjectiveShadowMapCount;
}

void ZERNStageShadowing::SetDirectionalShadowMapsResolution(ZERNLightShadowResolution Resolution)
{
	if (DirectionalShadowMapsResolution == Resolution)
		return;

	DirectionalShadowMapsResolution = Resolution;

	DirtyFlags.RaiseFlags(ZERN_SSDF_OUTPUT0);
}

ZERNLightShadowResolution ZERNStageShadowing::GetDirectionalShadowMapsResolution() const
{
	return DirectionalShadowMapsResolution;
}

void ZERNStageShadowing::SetSpotShadowMapsResolution(ZERNLightShadowResolution Resolution)
{
	if (SpotShadowMapsResolution == Resolution)
		return;

	SpotShadowMapsResolution = Resolution;

	DirtyFlags.RaiseFlags(ZERN_SSDF_OUTPUT1);
}

ZERNLightShadowResolution ZERNStageShadowing::GetSpotShadowMapsResolution() const
{
	return SpotShadowMapsResolution;
}

void ZERNStageShadowing::SetProjectiveShadowMapsResolution(ZERNLightShadowResolution Resolution)
{
	if (ProjectiveShadowMapsResolution == Resolution)
		return;

	ProjectiveShadowMapsResolution = Resolution;

	DirtyFlags.RaiseFlags(ZERN_SSDF_OUTPUT2);
}

ZERNLightShadowResolution ZERNStageShadowing::GetProjectiveShadowMapsResolution() const
{
	return ProjectiveShadowMapsResolution;
}

bool ZERNStageShadowing::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (GetCommands().GetCount() == 0)
		return false;

	ze_for_each(Command, GetCommands())
	{
		if (Command->Scene == NULL)
			continue;
		
		Context->SetConstantBuffer(ZEGR_ST_ALL, ZERN_SHADER_CONSTANT_SCENE, Command->Scene->GetConstantBuffer());

		if (Command->GetClass() == ZERNCommandDirectionalLight::Class())
			GenerateDirectionalShadowMaps(Context, static_cast<const ZERNCommandDirectionalLight&>(Command.GetItem()));
		else if (Command->GetClass() == ZERNCommandSpotLightShadow::Class())
			GenerateSpotShadowMaps(Context, static_cast<const ZERNCommandSpotLightShadow&>(Command.GetItem()));
		else if (Command->GetClass() == ZERNCommandProjectiveLight::Class())
			GenerateProjectiveShadowMaps(Context, static_cast<const ZERNCommandProjectiveLight&>(Command.GetItem()));
	}


	return false;
}

void ZERNStageShadowing::CleanUp(ZEGRContext* Context)
{
	DirectionalShadowMapIndex = 0;
	SpotShadowMapIndex = 0;
	ProjectiveShadowMapIndex = 0;

	ZERNStage::CleanUp(Context);
}

ZERNStageShadowing::ZERNStageShadowing()
{
	DirtyFlags.RaiseAll();
	
	SetCascadeCount(4);
	SetDirectionalShadowMapCount(1);
	SetSpotShadowMapCount(4);
	SetProjectiveShadowMapCount(4);

	DirectionalShadowMapIndex = 0;
	SpotShadowMapIndex = 0;
	ProjectiveShadowMapIndex = 0;

	DirectionalShadowMapsResolution = ZERN_LSR_HIGH;
	SpotShadowMapsResolution = ZERN_LSR_MEDIUM;
	ProjectiveShadowMapsResolution = ZERN_LSR_MEDIUM;

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&DirectionalShadowMaps), "DirectionalShadowMaps", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN);
	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&SpotShadowMaps), "SpotShadowMaps", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN);
	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&ProjectiveShadowMaps), "ProjectiveShadowMaps", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN);
}

ZERNStageShadowing::~ZERNStageShadowing()
{
	Deinitialize();
}
