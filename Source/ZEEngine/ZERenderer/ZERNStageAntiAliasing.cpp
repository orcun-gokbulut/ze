//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageAntiAliasing.cpp
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

#include "ZERNStageAntiAliasing.h"

#include "ZERNStageID.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGame/ZEGizmo.h"
#include "ZERNRenderer.h"
#include "ZEGraphics/ZEGRRenderTarget.h"

#include "ZERNStageSMAAAreaTexture.h"
#include "ZERNStageSMAASearchTexture.h"
#include "ZERNShaderSlots.h"

#define ZERN_AADF_CONSTANT_BUFFER	1
#define ZERN_AADF_SHADER			2
#define ZERN_AADF_RENDER_STATE		4
#define ZERN_AADF_TEXTURE			8

bool ZERNStageAntiAliasing::UpdateInputOutput()
{
	InputTexture = GetPrevOutput(ZERN_SO_COLOR);
	if (InputTexture == NULL)
		return false;

	ZEHolder<const ZEGRRenderTarget> NewOutputRenderTarget = GetNextProvidedInput(ZERN_SO_COLOR);
	if (NewOutputRenderTarget == NULL)
	{
		// No Provided Output - Create Own Buffer
		if (OutputTexture == NULL || OutputTexture->GetWidth() != InputTexture->GetWidth() || OutputTexture->GetHeight() != InputTexture->GetHeight())
			OutputTexture = ZEGRTexture2D::CreateInstance(InputTexture->GetWidth(), InputTexture->GetHeight(), 1, 1, 1, ZEGR_TF_R11G11B10_FLOAT, true);

		NewOutputRenderTarget = OutputTexture->GetRenderTarget();
	}
	else
	{
		OutputTexture.Release();
	}

	if (NewOutputRenderTarget != OutputRenderTarget)
	{
		OutputRenderTarget = NewOutputRenderTarget;
		Viewport.SetWidth(OutputRenderTarget->GetWidth());
		Viewport.SetHeight(OutputRenderTarget->GetHeight());
		Constants.OutputSize = ZEVector2(Viewport.GetWidth(), Viewport.GetHeight());
		DirtyFlags.RaiseFlags(ZERN_AADF_CONSTANT_BUFFER);
	}

	return true;
}

bool ZERNStageAntiAliasing::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZERN_AADF_CONSTANT_BUFFER))
		return true;

	if (ConstantBuffer == NULL)
		ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_AADF_CONSTANT_BUFFER);

	return true;
}

bool ZERNStageAntiAliasing::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_AADF_SHADER))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNSMAA.hlsl";
	Options.Model = ZEGR_SM_4_1;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNSMAA_EdgeDetection_VertexShader";
	EdgeDetectionVertexShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNSMAA_BlendingWeightCalculation_VertexShader";
	BlendingWeightCalculationVertexShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNSMAA_NeighborhoodBlending_VertexShader";
	NeighborhoodBlendingVertexShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSMAA_EdgeDetection_PixelShader";
	EdgeDetectionPixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSMAA_BlendingWeightCalculation_PixelShader";
	BlendingWeightCalculationPixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSMAA_NeighborhoodBlending_PixelShader";
	NeighborhoodBlendingPixelShader = ZEGRShader::Compile(Options);

	DirtyFlags.UnraiseFlags(ZERN_AADF_SHADER);
	DirtyFlags.RaiseFlags(ZERN_AADF_RENDER_STATE);

	return true;
}

bool ZERNStageAntiAliasing::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZERN_AADF_RENDER_STATE))
		return true;

	ZEGRDepthStencilState EdgeDetectionDepthStencilState;
	EdgeDetectionDepthStencilState.SetDepthTestEnable(false);
	EdgeDetectionDepthStencilState.SetDepthWriteEnable(false);

	ZEGRRenderState RenderState;
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetDepthStencilState(EdgeDetectionDepthStencilState);

	RenderState.SetShader(ZEGR_ST_VERTEX, EdgeDetectionVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, EdgeDetectionPixelShader);
	EdgeDetectionPassRenderStateData = RenderState.Compile();
	zeCheckError(EdgeDetectionPassRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_VERTEX, BlendingWeightCalculationVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, BlendingWeightCalculationPixelShader);
	BlendingWeightCalculationPassRenderStateData = RenderState.Compile();
	zeCheckError(BlendingWeightCalculationPassRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_VERTEX, NeighborhoodBlendingVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, NeighborhoodBlendingPixelShader);
	NeighborhoodBlendingPassRenderStateData= RenderState.Compile();
	zeCheckError(NeighborhoodBlendingPassRenderStateData == NULL, false, "Cannot set render state.");
	
	DirtyFlags.UnraiseFlags(ZERN_AADF_RENDER_STATE);

	return true;
}

bool ZERNStageAntiAliasing::UpdateTextures()
{
	if (OutputRenderTarget == NULL)
		return false;

	if (EdgeTexture == NULL ||
		EdgeTexture->GetWidth() != OutputRenderTarget->GetWidth() ||
		EdgeTexture->GetHeight() != OutputRenderTarget->GetHeight())
	{
		EdgeTexture = ZEGRTexture2D::CreateInstance(OutputRenderTarget->GetWidth(), OutputRenderTarget->GetHeight(), 1, 1, 1,ZEGR_TF_R8G8B8A8_UNORM, true);
		EdgeRenderTarget = EdgeTexture->GetRenderTarget();

		BlendTexture = ZEGRTexture2D::CreateInstance(OutputRenderTarget->GetWidth(), OutputRenderTarget->GetHeight(), 1, 1, 1, ZEGR_TF_R8G8B8A8_UNORM, true);
		BlendRenderTarget = BlendTexture->GetRenderTarget();
	}

	DirtyFlags.UnraiseFlags(ZERN_AADF_TEXTURE);

	return true;
}


bool ZERNStageAntiAliasing::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateInputOutput())
		return false;

	if (!UpdateConstantBuffers())
		return false;

	if (!UpdateTextures())
		return false;

	return true;
}

void ZERNStageAntiAliasing::ClearTextures(ZEGRContext* Context)
{
	Context->ClearRenderTarget(EdgeRenderTarget, ZEVector4::Zero);
	Context->ClearRenderTarget(BlendRenderTarget, ZEVector4::Zero);
}

void ZERNStageAntiAliasing::DoEdgeDetection(ZEGRContext* Context)
{
	Context->SetRenderState(EdgeDetectionPassRenderStateData);
	Context->SetRenderTargets(1, EdgeRenderTarget.GetPointerToPointer(), NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, InputTexture);

	Context->Draw(3, 0);
}

void ZERNStageAntiAliasing::DoBlendingWeightCalculation(ZEGRContext* Context)
{
	Context->SetRenderState(BlendingWeightCalculationPassRenderStateData);
	Context->SetRenderTargets(1, BlendRenderTarget.GetPointerToPointer(), NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 1, EdgeTexture);

	Context->Draw(3, 0);
}

void ZERNStageAntiAliasing::DoNeighborhoodBlending(ZEGRContext* Context)
{
	Context->SetRenderState(NeighborhoodBlendingPassRenderStateData);
	Context->SetRenderTargets(1, OutputRenderTarget.GetPointerToPointer(), NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, BlendTexture);

	Context->Draw(3, 0);
}

bool ZERNStageAntiAliasing::InitializeSelf()
{
	if (!ZERNStage::InitializeSelf())
		return false;

	ZEGRSamplerDescription SamplerDescription;
	SamplerDescription.MinFilter = ZEGR_TFM_LINEAR;
	SamplerDescription.MagFilter = ZEGR_TFM_LINEAR;
	SamplerDescription.MipFilter = ZEGR_TFM_LINEAR;
	SamplerDescription.AddressU = ZEGR_TAM_CLAMP;
	SamplerDescription.AddressV = ZEGR_TAM_CLAMP;
	SamplerLinear = ZEGRSampler::GetSampler(SamplerDescription);

	SamplerDescription.MinFilter = ZEGR_TFM_POINT;
	SamplerDescription.MagFilter = ZEGR_TFM_POINT;
	SamplerDescription.MipFilter = ZEGR_TFM_POINT;
	SamplerPoint = ZEGRSampler::GetSampler(SamplerDescription);

	AreaTexture = ZEGRTexture2D::CreateInstance(AREATEX_WIDTH, AREATEX_HEIGHT, 1, 1, 1, ZEGR_TF_R8G8_UNORM);
	AreaTexture->UpdateSubResource(0, 0, areaTexBytes, AREATEX_PITCH);

	SearchTexture = ZEGRTexture2D::CreateInstance(SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT, 1, 1, 1, ZEGR_TF_R8_UNORM);
	SearchTexture->UpdateSubResource(0, 0, searchTexBytes, SEARCHTEX_PITCH);
	
	Update();

	return true;
}

void ZERNStageAntiAliasing::DeinitializeSelf()
{
	DirtyFlags.RaiseAll();

	InputTexture.Release();
	OutputTexture.Release();
	OutputRenderTarget.Release();

	EdgeRenderTarget.Release();
	EdgeTexture.Release();
	EdgeRenderTarget.Release();
	BlendTexture.Release();

	ConstantBuffer.Release();

	EdgeDetectionVertexShader.Release();
	EdgeDetectionPixelShader.Release();
	
	BlendingWeightCalculationVertexShader.Release();
	BlendingWeightCalculationPixelShader.Release();

	NeighborhoodBlendingVertexShader.Release();
	NeighborhoodBlendingPixelShader.Release();

	EdgeDetectionPassRenderStateData.Release();
	BlendingWeightCalculationPassRenderStateData.Release();
	NeighborhoodBlendingPassRenderStateData.Release();

	SamplerLinear.Release();
	SamplerPoint.Release();

	ZERNStage::DeinitializeSelf();
}

ZEInt ZERNStageAntiAliasing::GetId() const
{
	return ZERN_STAGE_ANTI_ALIASING;
}

const ZEString& ZERNStageAntiAliasing::GetName() const
{
	static const ZEString Name = "Anti Aliasing Stage";
	return Name;
}

const ZEGRRenderTarget* ZERNStageAntiAliasing::GetProvidedInput(ZERNStageBuffer Input) const
{
	if (GetEnabled() && (Input == ZERN_SO_COLOR))
		return NULL;

	return ZERNStage::GetProvidedInput(Input);
}

const ZEGRTexture2D* ZERNStageAntiAliasing::GetOutput(ZERNStageBuffer Output) const
{
	if (GetEnabled() && (Output == ZERN_SO_COLOR || Output == ZERN_SO_HDR))
		return OutputTexture;

	return ZERNStage::GetOutput(Output);
}

bool ZERNStageAntiAliasing::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!Update())
		return false;

	Context->SetViewports(1, &Viewport);
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_POST_PROCESSOR, ConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_POST_PROCESSOR, ConstantBuffer);
	Context->SetVertexBuffers(0, 0, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinear);
	Context->SetTexture(ZEGR_ST_PIXEL, 3, AreaTexture);
	Context->SetTexture(ZEGR_ST_PIXEL, 4, SearchTexture);

	ClearTextures(Context);
	DoEdgeDetection(Context);
	DoBlendingWeightCalculation(Context);
	DoNeighborhoodBlending(Context);

	return true;
}

void ZERNStageAntiAliasing::CleanUp(ZEGRContext* Context)
{
	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_POST_PROCESSOR, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_POST_PROCESSOR, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, NULL);


	ZERNStage::CleanUp(Context);
}

ZERNStageAntiAliasing::ZERNStageAntiAliasing()
{
	DirtyFlags.RaiseAll();
	memset(&Constants, 0, sizeof(Constants));
}
