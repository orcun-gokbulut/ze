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
#include "ZELightPoint.h"
#include "ZELightDirectional.h"
#include "ZELightProjective.h"
#include "ZELightOmniProjective.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRStructuredBuffer.h"

#define MAX_LIGHT					512
#define TILE_DIMENSION				16

#define ZERN_SFDF_SHADERS			1
#define ZERN_SFDF_RENDER_STATES		2

bool ZERNStageForward::InitializeSelf()
{
	if (!ZERNStage::InitializeSelf())
		return false;

	return Update();
}

void ZERNStageForward::DeinitializeSelf()
{
	DirtyFlags.RaiseAll();

	TiledForwardComputeShader.Release();
	TiledForwardComputeRenderState.Release();

	TileLightStructuredBuffer.Release();

	OutputTexture.Release();

	AccumulationTexture = NULL;
	DepthTexture = NULL;
	TransparentDepthTexture = NULL;

	ZERNStage::DeinitializeSelf();
}

bool ZERNStageForward::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_SFDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNTiledDeferredShadingCompute.hlsl";
	Options.Model = ZEGR_SM_5_0;
	Options.Type = ZEGR_ST_COMPUTE;
	Options.EntryPoint = "ZERNTiledDeferredShadingCompute_ComputeShader_Main";

	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString(ZEGRGraphicsModule::SAMPLE_COUNT)));
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_TILED_FORWARD"));
	TiledForwardComputeShader = ZEGRShader::Compile(Options);
	zeCheckError(TiledForwardComputeShader == NULL, false, "Cannot set compute shader.");

	DirtyFlags.UnraiseFlags(ZERN_SFDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_SFDF_RENDER_STATES);

	return true;
}

bool ZERNStageForward::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZERN_SFDF_RENDER_STATES))
		return true;

	ZEGRComputeRenderState ComputeRenderState;
	ComputeRenderState.SetComputeShader(TiledForwardComputeShader);
	TiledForwardComputeRenderState = ComputeRenderState.Compile();
	zeCheckError(TiledForwardComputeRenderState == NULL, false, "Cannot set render state.");

	DirtyFlags.UnraiseFlags(ZERN_SFDF_RENDER_STATES);

	return true;
}

bool ZERNStageForward::UpdateInputOutputs()
{
	AccumulationTexture = GetPrevOutput(ZERN_SO_ACCUMULATION);
	if (AccumulationTexture == NULL)
	{
		const ZEGRRenderTarget* MainRenderTarget = GetRenderer()->GetOutputRenderTarget();
		if (MainRenderTarget == NULL)
			return false;

		if (OutputTexture == NULL || 
			OutputTexture->GetWidth() != MainRenderTarget->GetWidth() || OutputTexture->GetHeight() != MainRenderTarget->GetHeight())
		{
			OutputTexture = ZEGRTexture2D::CreateInstance(
														MainRenderTarget->GetWidth(), 
														MainRenderTarget->GetHeight(), 
														1, 
														ZEGR_TF_R11G11B10_FLOAT, 
														ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, 
														ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 
														1, 
														ZEGRGraphicsModule::SAMPLE_COUNT);
		}

		AccumulationTexture = OutputTexture;
	}
	else
	{
		OutputTexture.Release();
	}

	TransparentDepthTexture = GetPrevOutput(ZERN_SO_TRANSPARENT_DEPTH);
	if (TransparentDepthTexture == NULL)
		return false;

	DepthTexture = GetPrevOutput(ZERN_SO_DEPTH);
	if (DepthTexture == NULL)
		return false;

	Viewport.SetWidth((float)AccumulationTexture->GetWidth());
	Viewport.SetHeight((float)AccumulationTexture->GetHeight());

	ZEUInt TileCountX = (AccumulationTexture->GetWidth() + TILE_DIMENSION - 1) / TILE_DIMENSION;
	ZEUInt TileCountY = (AccumulationTexture->GetHeight() + TILE_DIMENSION - 1) / TILE_DIMENSION;
	ZESize Size = TileCountX * TileCountY * (MAX_LIGHT + 2) * sizeof(ZEUInt);

	if (TileLightStructuredBuffer == NULL || TileLightStructuredBuffer->GetSize() != Size)
		TileLightStructuredBuffer = ZEGRStructuredBuffer::Create(TileCountX * TileCountY * (MAX_LIGHT + 2), sizeof(ZEUInt), ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_UNORDERED_ACCESS);

	return true;
}

bool ZERNStageForward::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateInputOutputs())
		return false;

	return true;
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

const ZEGRTexture2D* ZERNStageForward::GetOutput(ZERNStageBuffer Output) const
{
	if (GetEnabled())
	{
		if ((Output == ZERN_SO_COLOR || Output == ZERN_SO_ACCUMULATION))
			return AccumulationTexture;
	}

	return ZERNStage::GetOutput(Output);
}

bool ZERNStageForward::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!Update())
		return false;

	if (GetCommands().GetCount() == 0)
		return false;


	Context->SetComputeRenderState(TiledForwardComputeRenderState);
	Context->SetRWStructuredBuffers(0, 1, TileLightStructuredBuffer.GetPointerToPointer());
	Context->SetTextures(ZEGR_ST_COMPUTE, 4, 1, reinterpret_cast<const ZEGRTexture**>(&TransparentDepthTexture));

	ZEUInt TileCountX = (AccumulationTexture->GetWidth() + TILE_DIMENSION - 1) / TILE_DIMENSION;
	ZEUInt TileCountY = (AccumulationTexture->GetHeight() + TILE_DIMENSION - 1) / TILE_DIMENSION;

	Context->Dispatch(TileCountX, TileCountY, 1);


	const ZEGRRenderTarget* RenderTarget = AccumulationTexture->GetRenderTarget();
	if (OutputTexture != NULL)
		Context->ClearRenderTarget(RenderTarget, ZEVector4::Zero);

	Context->SetStructuredBuffers(ZEGR_ST_PIXEL, 16, 1, TileLightStructuredBuffer.GetPointerToPointer());
	Context->SetRenderTargets(1, &RenderTarget, DepthTexture->GetDepthStencilBuffer(true));
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

	AccumulationTexture = NULL;
	DepthTexture = NULL;
	TransparentDepthTexture = NULL;
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
	return "StageForwardPostHDR";
}
