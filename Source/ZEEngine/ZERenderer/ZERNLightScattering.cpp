//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNLightScattering.cpp
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

#include "ZERNLightScattering.h"
#include "ZEGraphics/ZEGRShaderCompileOptions.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRTexture3D.h"
#include "ZEGraphics/ZEGRRenderTarget.h"

void ZERNLightScattering::SetInputTexture(ZEGRTexture2D* InputTexture)
{
	this->InputTexture = InputTexture;
}

const ZEGRTexture2D* ZERNLightScattering::GetInputTexture() const
{
	return InputTexture;
}

void ZERNLightScattering::SetDepthTexture(ZEGRTexture2D* DepthTexture)
{
	this->DepthTexture = DepthTexture;
}

const ZEGRTexture2D* ZERNLightScattering::GetDepthTexture() const
{
	return DepthTexture;
}

void ZERNLightScattering::SetOutputRenderTarget(ZEGRRenderTarget* OutputRenderTarget)
{
	this->OutputRenderTarget = OutputRenderTarget;
}

const ZEGRRenderTarget* ZERNLightScattering::GetOutputRenderTarget() const
{
	return OutputRenderTarget;
}

void ZERNLightScattering::SetLightDirection(const ZEVector3& LightDirection)
{
	Constants.LightDirection = LightDirection;
}

const ZEVector3& ZERNLightScattering::GetLightDirection() const
{
	return Constants.LightDirection;
}

void ZERNLightScattering::SetLightIntensity(float LightIntensity)
{
	Constants.LightIntensity = LightIntensity;
}

float ZERNLightScattering::GetLightIntensity() const
{
	return Constants.LightIntensity;
}

void ZERNLightScattering::SetLightColor(const ZEVector3& LightColor)
{
	Constants.LightColor = LightColor;
}

const ZEVector3& ZERNLightScattering::GetLightColor() const
{
	return Constants.LightColor;
}

void ZERNLightScattering::SetMieScatteringStrengh(float MieScatteringStrength)
{
	Constants.MieScatteringStrength = MieScatteringStrength;
}

float ZERNLightScattering::GetMieScatteringStrengh() const
{
	return Constants.MieScatteringStrength;
}

void ZERNLightScattering::CreateShaders()
{
	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNFastLightScattering.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_Position";
	VertexShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFastLightScattering_PixelShader_Main";
	PixelShader = ZEGRShader::Compile(Options);

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNPrecomputingLightScattering.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNPrecomputingLightScattering_SingleScattering_PixelShader_Main";
	PrecomputeSingleScatteringPixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNPrecomputingLightScattering_HighOrderScattering_PixelShader_Main";
	PrecomputeHighOrderScatteringPixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNPrecomputingLightScattering_HighOrderInScattering_PixelShader_Main";
	PrecomputeHighOrderInScatteringPixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNPrecomputingLightScattering_AddOrders_PixelShader_Main";
	AddOrdersPixelShader = ZEGRShader::Compile(Options);
}

void ZERNLightScattering::CreateRenderState()
{
	ZEGRRenderState RenderState;
	
	ZEGRDepthStencilState DepthStencilState;
	DepthStencilState.SetDepthTestEnable(false);
	DepthStencilState.SetDepthWriteEnable(false);

	ZEGRBlendState BlendState;
	BlendState.SetBlendEnable(true);
	ZEGRBlendRenderTarget BlendRenderTarget = BlendState.GetRenderTarget(0);
	BlendRenderTarget.SetBlendEnable(true);
	BlendRenderTarget.SetSource(ZEGR_BO_ONE);
	BlendRenderTarget.SetDestination(ZEGR_BO_ONE);
	BlendRenderTarget.SetOperation(ZEGR_BE_ADD);

	BlendState.SetRenderTargetBlend(0, BlendRenderTarget);

	RenderState.SetDepthStencilState(DepthStencilState);
	RenderState.SetBlendState(BlendState);

	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PixelShader);

	RenderStateData = RenderState.Compile();
	zeCheckError(RenderStateData == NULL, , "Cannot set render state.");

	RenderState.SetBlendState(ZEGRBlendState());
	RenderState.SetShader(ZEGR_ST_PIXEL, PrecomputeSingleScatteringPixelShader);

	PrecomputeSingleScatteringRenderStateData = RenderState.Compile();
	zeCheckError(PrecomputeSingleScatteringRenderStateData == NULL, , "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, PrecomputeHighOrderScatteringPixelShader);

	PrecomputeHighOrderScatteringRenderStateData = RenderState.Compile();
	zeCheckError(PrecomputeHighOrderScatteringRenderStateData == NULL, , "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, PrecomputeHighOrderInScatteringPixelShader);

	PrecomputeHighOrderInScatteringRenderStateData = RenderState.Compile();
	zeCheckError(PrecomputeHighOrderInScatteringRenderStateData == NULL, , "Cannot set render state.");

	RenderState.SetBlendState(BlendState);
	RenderState.SetShader(ZEGR_ST_PIXEL, AddOrdersPixelShader);

	AddOrdersRenderStateData = RenderState.Compile();
	zeCheckError(AddOrdersRenderStateData == NULL, , "Cannot set render state.");
}

bool ZERNLightScattering::InitializeSelf()
{
	CreateShaders();
	CreateRenderState();

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(LightScatteringConstants));
	PrecomputeConstantBuffer = ZEGRConstantBuffer::Create(sizeof(PrecomputeScatteringConstants));

	PrecomputedSingleScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R16G16B16A16_FLOAT, true, false);
	PrecomputedHighOrderScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R32G32B32A32_FLOAT, true, false);
	PrecomputedHighOrderInScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R32G32B32A32_FLOAT, true, false);
	PrecomputedMultipleScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R16G16B16A16_FLOAT, true, false);

	return true;
}

void ZERNLightScattering::DeinitializeSelf()
{
	InputTexture = NULL;
	DepthTexture = NULL;
	OutputRenderTarget = NULL;

	VertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
	ConstantBuffer.Release();

	PrecomputedSingleScatteringBuffer.Release();
	PrecomputedMultipleScatteringBuffer.Release();

	ReleasePrecomputeResources();
}

void ZERNLightScattering::PrecomputeWithPixelShader(ZEGRContext* Context)
{
	ZEGRViewport Viewport;
	Viewport.SetWidth(32.0f);
	Viewport.SetHeight(128.0f);
	Viewport.SetX(0.0f);
	Viewport.SetY(0.0f);
	Viewport.SetMinDepth(0.0f);
	Viewport.SetMaxDepth(1.0f);

	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearClamp);
	Context->SetViewports(1, &Viewport);
	Context->SetVertexBuffers(0, 0, NULL);
	
	ZEArray<ZEGRRenderTarget*> MultipleScatteringRenderTargets;
	MultipleScatteringRenderTargets.Resize(1024);

	for(ZEUInt I = 0; I < 1024; ++I)
	{
		ZEGRRenderTarget* RenderTarget = PrecomputedSingleScatteringBuffer->GetRenderTarget(I);
		MultipleScatteringRenderTargets[I] = PrecomputedMultipleScatteringBuffer->GetRenderTarget(I);
		Context->ClearRenderTarget(MultipleScatteringRenderTargets[I], ZEVector4(0.0f, 0.0f, 0.0f, 0.0f));

		PrecomputeConstants.Index = ((float)(I % 64) + 0.5f) / 64.0f;
		PrecomputeConstants.Index2 = ((float)(I / 64) + 0.5f) / 16.0f;
		PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

		Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
		Context->SetRenderTargets(1, &RenderTarget, NULL);
		Context->SetRenderState(PrecomputeSingleScatteringRenderStateData);

		Context->Draw(3, 0);
	}

	for(ZEUInt S = 1; S < 4; S++)
	{
		for(ZEUInt J = 0; J < 1024; ++J)
		{
			ZEGRRenderTarget* RenderTarget = PrecomputedHighOrderScatteringBuffer->GetRenderTarget(J);
			PrecomputeConstants.Index = ((float)(J % 64) + 0.5f) / 64.0f;
			PrecomputeConstants.Index2 = ((float)(J / 64) + 0.5f) / 16.0f;
			PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

			Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
			Context->SetTexture(ZEGR_ST_PIXEL, 0, (S == 1) ? PrecomputedSingleScatteringBuffer : PrecomputedHighOrderInScatteringBuffer);
			Context->SetRenderTargets(1, &RenderTarget, NULL);
			Context->SetRenderState(PrecomputeHighOrderScatteringRenderStateData);
		
			Context->Draw(3, 0);
		}

		for(ZEUInt J = 0; J < 1024; ++J)
		{
			ZEGRRenderTarget* RenderTarget = PrecomputedHighOrderInScatteringBuffer->GetRenderTarget(J);
			PrecomputeConstants.Index = ((float)(J % 64) + 0.5f) / 64.0f;
			PrecomputeConstants.Index2 = ((float)(J / 64) + 0.5f) / 16.0f;
			PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

			Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
			Context->SetTexture(ZEGR_ST_PIXEL, 1, PrecomputedHighOrderScatteringBuffer);
			Context->SetRenderTargets(1, &RenderTarget, NULL);
			Context->SetRenderState(PrecomputeHighOrderInScatteringRenderStateData);

			Context->Draw(3, 0);
		}

		for(ZEUInt J = 0; J < 1024; ++J)
		{
			ZEGRRenderTarget* RenderTarget = MultipleScatteringRenderTargets[J];
			PrecomputeConstants.Index = (float)J;
			PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

			Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
			Context->SetTexture(ZEGR_ST_PIXEL, 0, PrecomputedHighOrderInScatteringBuffer);
			Context->SetRenderTargets(1, &RenderTarget, NULL);
			Context->SetRenderState(AddOrdersRenderStateData);

			Context->Draw(3, 0);
		}
	}

	for(ZEUInt K = 0; K < 1024; ++K)
	{
		ZEGRRenderTarget* RenderTarget = MultipleScatteringRenderTargets[K];
		PrecomputeConstants.Index = (float)K;
		PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

		Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
		Context->SetTexture(ZEGR_ST_PIXEL, 0, PrecomputedSingleScatteringBuffer);
		Context->SetRenderTargets(1, &RenderTarget, NULL);
		Context->SetRenderState(AddOrdersRenderStateData);

		Context->Draw(3, 0);
	}

	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 1, NULL);

	ReleasePrecomputeResources();
}

void ZERNLightScattering::ReleasePrecomputeResources()
{
	PrecomputeSingleScatteringPixelShader.Release();
	PrecomputeHighOrderScatteringPixelShader.Release();
	PrecomputeHighOrderInScatteringPixelShader.Release();
	AddOrdersPixelShader.Release();

	PrecomputeSingleScatteringRenderStateData.Release();
	PrecomputeHighOrderScatteringRenderStateData.Release();
	PrecomputeHighOrderInScatteringRenderStateData.Release();
	AddOrdersRenderStateData.Release();

	PrecomputedHighOrderScatteringBuffer.Release();
	PrecomputedHighOrderInScatteringBuffer.Release();

	PrecomputeConstantBuffer.Release();
}

void ZERNLightScattering::Process(ZEGRContext* Context, bool MultipleScattering)
{
	zeDebugCheck(InputTexture == NULL, "Input texture cannot be null");
	//zeDebugCheck(DepthTexture == NULL, "Depth texture cannot be null");
	zeDebugCheck(OutputRenderTarget == NULL, "Output render target cannot be null");

	static bool Precomputed = false;

	if(!Precomputed)
	{
		Precomputed = true;
		PrecomputeWithPixelShader(Context);
	}

	ConstantBuffer->SetData(&Constants);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, InputTexture);
	//Context->SetTexture(ZEGR_ST_PIXEL, 1, DepthTexture);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, MultipleScattering ? PrecomputedMultipleScatteringBuffer : PrecomputedSingleScatteringBuffer);
	Context->SetRenderTargets(1, &OutputRenderTarget, NULL);
	Context->SetRenderState(RenderStateData);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, ConstantBuffer);
	Context->SetVertexBuffers(0, 0, NULL);

	ZEGRViewport Viewport;
	Viewport.SetWidth((float)OutputRenderTarget->GetWidth());
	Viewport.SetHeight((float)OutputRenderTarget->GetHeight());
	Viewport.SetX(0.0f);
	Viewport.SetY(0.0f);
	Viewport.SetMinDepth(0.0f);
	Viewport.SetMaxDepth(1.0f);

	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 1, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, NULL);
	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);
}

ZERNLightScattering::ZERNLightScattering()
{
	InputTexture = NULL;
	DepthTexture = NULL;
	OutputRenderTarget = NULL;
}

ZERNLightScattering::~ZERNLightScattering()
{
	InputTexture = NULL;
	DepthTexture = NULL;
	OutputRenderTarget = NULL;
}

