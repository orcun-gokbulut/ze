//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageForward.cpp
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

#include "ZERNStageForward.h"

#include "ZEMath/ZEAngle.h"
#include "ZERNRenderer.h"
#include "ZERNStageID.h"
#include "ZERNCommand.h"
#include "ZERNStageLighting.h"
#include "ZELightPoint.h"
#include "ZELightDirectional.h"
#include "ZELightProjective.h"
#include "ZELightOmniProjective.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"

#define ZERN_SFDF_SHADERS			1
#define ZERN_SFDF_RENDER_STATES		2
#define ZERN_SFDF_TILE_LIGHT_BUFFER	4
#define ZERN_SFDF_OUTPUT			8

bool ZERNStageForward::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;

	return true;
}

bool ZERNStageForward::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();
	
	LightsConstantBuffer = NULL;
	TileLightStructuredBuffer = NULL;
	AccumulationTexture = NULL;
	DepthTexture = NULL;

	return ZERNStage::DeinitializeInternal();
}

void ZERNStageForward::CreateOutput(const ZEString& Name)
{
	ZEUInt Width = GetRenderer()->GetOutputTexture()->GetWidth();
	ZEUInt Height = GetRenderer()->GetOutputTexture()->GetHeight();

	if (Name == "ColorTexture")
	{
		if (DirtyFlags.GetFlags(ZERN_SFDF_OUTPUT) || AccumulationTexture == GetRenderer()->GetOutputTexture())
		{
			AccumulationTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, ZEGR_TF_R11G11B10_FLOAT, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 1, ZEGRGraphicsModule::SAMPLE_COUNT).GetPointer();
			DirtyFlags.UnraiseFlags(ZERN_SFDF_OUTPUT);
		}
	}
}

ZEInt ZERNStageForward::GetId() const
{
	return ZERN_STAGE_FORWARD;
}

const ZEString& ZERNStageForward::GetName() const
{
	static const ZEString Name = "Stage Forward";
	return Name;
}

void ZERNStageForward::Resized(ZEUInt Width, ZEUInt Height)
{
	Viewport.SetWidth((float)Width);
	Viewport.SetHeight((float)Height);

	DirtyFlags.RaiseFlags(ZERN_SFDF_OUTPUT);
}

bool ZERNStageForward::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (GetCommands().GetCount() == 0)
		return false;
	
	const ZEGRRenderTarget* RenderTarget = AccumulationTexture->GetRenderTarget();
	if (!DirtyFlags.GetFlags(ZERN_SFDF_OUTPUT) && AccumulationTexture->GetSampleCount() > 1)
		Context->ClearRenderTarget(RenderTarget, ZEVector4::Zero);
	
	ZERNStageLighting* StageLighting = static_cast<ZERNStageLighting*>(GetRenderer()->GetStage(ZERN_STAGE_LIGHTING));
	if (StageLighting != NULL)
	{
		Context->SetBuffer(ZEGR_ST_PIXEL, 15, TileLightStructuredBuffer);

		const ZEGRBuffer* Buffers[] = {LightsConstantBuffer, LightsConstantBuffer, LightsConstantBuffer, LightsConstantBuffer};
		const ZEUInt Offsets[] = {StageLighting->CommonParamsRange.Offset, StageLighting->PointLightRange.Offset, StageLighting->SpotLightRange.Offset, StageLighting->SpotLightShadowRange.Offset};
		const ZEUInt Counts[] = {StageLighting->CommonParamsRange.Count, StageLighting->PointLightRange.Count, StageLighting->SpotLightRange.Count, StageLighting->SpotLightShadowRange.Count};
	
		Context->SetConstantBuffers(ZEGR_ST_PIXEL, 8, 4, Buffers, Offsets, Counts);
	}

	Context->SetRenderTargets(1, &RenderTarget, DepthTexture->GetDepthStencilBuffer());
	Context->SetViewports(1, &Viewport);
	
	return true;
}

void ZERNStageForward::CleanUp(ZEGRContext* Context)
{
	ZERNStage::CleanUp(Context);
}

ZERNStageForward::ZERNStageForward()
{
	DirtyFlags.RaiseAll();

	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&LightsConstantBuffer), "LightsConstantBuffer", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&TileLightStructuredBuffer), "TileLightBuffer", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&DepthTexture), "DepthTexture", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&AccumulationTexture), "ColorTexture", ZERN_SRUT_WRITE, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_CREATE_OWN | ZERN_SRCF_GET_OUTPUT);
}

ZERNStageForward::~ZERNStageForward()
{
	Deinitialize();
}

ZEGRRenderState ZERNStageForward::GetRenderState()
{
	static ZEGRRenderState RenderState;
	static bool Initialized = false;
	if (!Initialized)
	{
		Initialized = true;

		RenderState.SetRenderTargetFormat(0, ZEGR_TF_R11G11B10_FLOAT);
		RenderState.SetDepthStencilFormat(ZEGR_TF_D24_UNORM_S8_UINT);
	}

	return RenderState;
}

ZEInt ZERNStageForwardTransparent::GetId() const
{
	return ZERN_STAGE_FORWARD_TRANSPARENT;
}

const ZEString& ZERNStageForwardTransparent::GetName() const
{
	static const ZEString Name = "Stage ForwardTransparent";
	return Name;
}

bool ZERNStageForwardTransparent::DeinitializeInternal()
{
	FogConstantBuffer = NULL;
	
	return ZERNStageForward::DeinitializeInternal();
}

bool ZERNStageForwardTransparent::Setup(ZEGRContext* Context)
{
	if (!ZERNStageForward::Setup(Context))
		return false;

	const ZEGRRenderTarget* RenderTarget = AccumulationTexture->GetRenderTarget();

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 12, FogConstantBuffer);
	Context->SetRenderTargets(1, &RenderTarget, DepthTexture->GetDepthStencilBuffer(true));

	return true;
}

ZERNStageForwardTransparent::ZERNStageForwardTransparent()
{
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&FogConstantBuffer), "FogConstantBuffer", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);
}

ZEGRRenderState ZERNStageForwardTransparent::GetRenderState()
{
	static ZEGRRenderState RenderState;
	static bool Initialized = false;
	if (!Initialized)
	{
		Initialized = true;

		ZEGRDepthStencilState DepthStencilStateNoWrite;
		DepthStencilStateNoWrite.SetDepthTestEnable(true);
		DepthStencilStateNoWrite.SetDepthWriteEnable(false);
		
		RenderState.SetDepthStencilState(DepthStencilStateNoWrite);

		ZEGRBlendState BlendStateAlphaBlending;
		BlendStateAlphaBlending.SetBlendEnable(true);
		ZEGRBlendRenderTarget BlendRenderTargetAlphaBlending = BlendStateAlphaBlending.GetRenderTarget(0);
		BlendRenderTargetAlphaBlending.SetBlendEnable(true);
		BlendRenderTargetAlphaBlending.SetSource(ZEGRBlend::ZEGR_BO_SRC_ALPHA);
		BlendRenderTargetAlphaBlending.SetDestination(ZEGRBlend::ZEGR_BO_INV_SRC_ALPHA);
		BlendRenderTargetAlphaBlending.SetOperation(ZEGRBlendOperation::ZEGR_BE_ADD);
		BlendStateAlphaBlending.SetRenderTargetBlend(0, BlendRenderTargetAlphaBlending);
		
		RenderState.SetBlendState(BlendStateAlphaBlending);

		RenderState.SetRenderTargetFormat(0, ZEGR_TF_R11G11B10_FLOAT);
		RenderState.SetDepthStencilFormat(ZEGR_TF_D24_UNORM_S8_UINT);
	}

	return RenderState;
}

ZEInt ZERNStageForwardPostHDR::GetId() const
{
	return ZERN_STAGE_FORWARD_POST_HDR;
}

const ZEString& ZERNStageForwardPostHDR::GetName() const
{
	static const ZEString Name = "Stage ForwardPostHDR";
	return Name;
}

bool ZERNStageForwardPostHDR::Setup(ZEGRContext* Context)
{
	if (!ZERNStageForward::Setup(Context))
		return false;

	const ZEGRRenderTarget* RenderTarget = AccumulationTexture->GetRenderTarget();

	Context->SetRenderTargets(1, &RenderTarget, DepthTexture->GetDepthStencilBuffer(true));

	return true;
}
