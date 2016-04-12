//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATAtmosphericScattering.cpp
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

#include "ZEATAtmosphericScattering.h"

#include "ZEGraphics/ZEGRShaderCompileOptions.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRTexture3D.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEMath.h"
#include "ZERandom.h"
#include "ZERenderer/ZERNStageGBuffer.h"

#define ZEAT_ASDF_SHADERS			1
#define ZEAT_ASDF_RENDER_STATE		2
#define ZEAT_ASDF_CONSTANT_BUFFER	4

void ZEATAtmosphericScattering::CreateRandomVectors()
{
	ZEVector4 SphereSamples[128];
	for(ZEUInt I = 0; I < 128; I++)
	{
		float Z = ZERandom::GetFloatPositive() * 2.0f - 1.0f;
		float Theta = ZERandom::GetFloatPositive() * ZE_PIx2;
		float Radius = ZEMath::Sqrt(ZEMath::Max(0.0f, 1.0f - Z * Z));

		SphereSamples[I].x = Radius * ZEAngle::Cos(Theta);
		SphereSamples[I].y = Radius * ZEAngle::Sin(Theta);
		SphereSamples[I].z = Z;
		SphereSamples[I].w = 0.0f;
	}

	RandomVectorsTexture.Release();
	RandomVectorsTexture = ZEGRTexture2D::CreateInstance(128, 1, 1, ZEGR_TF_R32G32B32A32_FLOAT, ZEGR_RU_GPU_READ_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE);
	RandomVectorsTexture->UpdateSubResource(0, 0, NULL, &SphereSamples[0], 128 * sizeof(ZEVector4));
}

bool ZEATAtmosphericScattering::UpdateShaders()
{
	if(!DirtyFlags.GetFlags(ZEAT_ASDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNFastLightScattering.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_Position";
	ScreenCoverVertexShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFastLightScattering_PixelShader_Main";
	SkyAndAerialPerspectivePixelShader = ZEGRShader::Compile(Options);

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNPrecomputingLightScattering.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNPrecomputingLightScattering_Density_PixelShader_Main";
	PrecomputeDensityPixelShader = ZEGRShader::Compile(Options);

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

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNPrecomputingLightScattering_SkyAmbient_PixelShader_Main";
	PrecomputeSkyAmbientPixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNPrecomputingLightScattering_SkyAmbientLighting_PixelShader_Main";
	SkyLightingPixelShader = ZEGRShader::Compile(Options);

	DirtyFlags.UnraiseFlags(ZEAT_ASDF_SHADERS);
	DirtyFlags.RaiseFlags(ZEAT_ASDF_RENDER_STATE);

	return true;
}

bool ZEATAtmosphericScattering::UpdateRenderState()
{
	if(!DirtyFlags.GetFlags(ZEAT_ASDF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState;
	
	ZEGRDepthStencilState DepthStencilStateNoTestWrite;
	DepthStencilStateNoTestWrite.SetDepthTestEnable(false);
	DepthStencilStateNoTestWrite.SetDepthWriteEnable(false);

	RenderState.SetDepthStencilState(DepthStencilStateNoTestWrite);

	ZEGRBlendState BlendStateAdditive;
	BlendStateAdditive.SetBlendEnable(true);
	ZEGRBlendRenderTarget BlendRenderTargetAdditive = BlendStateAdditive.GetRenderTarget(0);
	BlendRenderTargetAdditive.SetSource(ZEGRBlend::ZEGR_BO_ONE);
	BlendRenderTargetAdditive.SetDestination(ZEGRBlend::ZEGR_BO_ONE);
	BlendRenderTargetAdditive.SetOperation(ZEGRBlendOperation::ZEGR_BE_ADD);
	BlendRenderTargetAdditive.SetBlendEnable(true);
	BlendStateAdditive.SetRenderTargetBlend(0, BlendRenderTargetAdditive);

	RenderState.SetBlendState(BlendStateAdditive);

	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, SkyLightingPixelShader);

	SkyLightingRenderStateData = RenderState.Compile();
	zeCheckError(SkyLightingRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetBlendState(ZEGRBlendState());
	RenderState.SetShader(ZEGR_ST_PIXEL, SkyAndAerialPerspectivePixelShader);

	SkyAndAerialPerspectiveRenderStateData = RenderState.Compile();
	zeCheckError(SkyAndAerialPerspectiveRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, PrecomputeDensityPixelShader);

	PrecomputeDensityRenderStateData = RenderState.Compile();
	zeCheckError(PrecomputeDensityRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, PrecomputeSingleScatteringPixelShader);

	PrecomputeSingleScatteringRenderStateData = RenderState.Compile();
	zeCheckError(PrecomputeSingleScatteringRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, PrecomputeHighOrderScatteringPixelShader);

	PrecomputeHighOrderScatteringRenderStateData = RenderState.Compile();
	zeCheckError(PrecomputeHighOrderScatteringRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, PrecomputeHighOrderInScatteringPixelShader);

	PrecomputeHighOrderInScatteringRenderStateData = RenderState.Compile();
	zeCheckError(PrecomputeHighOrderInScatteringRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, PrecomputeSkyAmbientPixelShader);

	PrecomputeSkyAmbientRenderStateData = RenderState.Compile();
	zeCheckError(PrecomputeSkyAmbientRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetBlendState(BlendStateAdditive);
	RenderState.SetShader(ZEGR_ST_PIXEL, AddOrdersPixelShader);

	AddOrdersRenderStateData = RenderState.Compile();
	zeCheckError(AddOrdersRenderStateData == NULL, false, "Cannot set render state.");

	DirtyFlags.UnraiseFlags(ZEAT_ASDF_RENDER_STATE);

	return true;
}

bool ZEATAtmosphericScattering::UpdateConstantBuffers()
{
	if(!DirtyFlags.GetFlags(ZEAT_ASDF_CONSTANT_BUFFER))
		return true;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZEAT_ASDF_CONSTANT_BUFFER);

	return true;
}

bool ZEATAtmosphericScattering::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderState())
		return false;

	if (!UpdateConstantBuffers())
		return false;

	return true;
}

void ZEATAtmosphericScattering::ComputeRayleighMiePhaseFactors()
{
	float RayleighAngularFactor = (3.0f / (16.0f * ZE_PI));

	PrecomputeConstants.RayleighAngularFactor = RayleighAngularFactor * PrecomputeConstants.RayleighScatteringFactor;

	float CornetteShanksG = 0.76f;
	float CornetteShanksGsquare = CornetteShanksG * CornetteShanksG;

	float MieAngularFactor = 3.0f / (8.0f * ZE_PI);

	PrecomputeConstants.CornetteShanksG.x = (1.0f - CornetteShanksGsquare);
	PrecomputeConstants.CornetteShanksG.y = (2.0f + CornetteShanksGsquare);
	PrecomputeConstants.CornetteShanksG.z = 1.0f + CornetteShanksGsquare;
	PrecomputeConstants.CornetteShanksG.w = 2.0f * CornetteShanksG;

	PrecomputeConstants.MieAngularFactor = MieAngularFactor * PrecomputeConstants.MieScatteringFactor;

}

void ZEATAtmosphericScattering::PrecomputeWithPixelShader(ZEGRContext* Context)
{
	ZEGRRenderTarget* PreviousRenderTarget;
	Context->GetRenderTargets(1, &PreviousRenderTarget, NULL);

	ZEGRTexture* PreviousTexture0;
	ZEGRTexture* PreviousTexture2;
	ZEGRTexture* PreviousTexture3;
	ZEGRTexture* PreviousTexture4;
	Context->GetTexture(ZEGR_ST_PIXEL, 0, &PreviousTexture0);
	Context->GetTexture(ZEGR_ST_PIXEL, 2, &PreviousTexture2);
	Context->GetTexture(ZEGR_ST_PIXEL, 3, &PreviousTexture3);
	Context->GetTexture(ZEGR_ST_PIXEL, 4, &PreviousTexture4);

	ZEArray<ZEHolder<const ZEGRRenderTarget>> MultipleScatteringRenderTargets;
	MultipleScatteringRenderTargets.Resize(1024);	//64 * 16

	const ZEGRRenderTarget* DensityRenderTarget = PrecomputedDensityBuffer->GetRenderTarget();
	Context->SetRenderState(PrecomputeDensityRenderStateData);
	Context->SetRenderTargets(1, &DensityRenderTarget, NULL);
	Context->SetVertexBuffers(0, 0, NULL);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, 1024.0f, 1024.0f));

	Context->Draw(3, 0);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearClamp);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, 32.0f, 128.0f));
	
	for(ZEUInt I = 0; I < 1024; I++)
	{
		ZEHolder<const ZEGRRenderTarget> RenderTarget = PrecomputedSingleScatteringBuffer->GetRenderTarget(I);
		MultipleScatteringRenderTargets[I] = PrecomputedMultipleScatteringBuffer->GetRenderTarget(I);
		Context->ClearRenderTarget(MultipleScatteringRenderTargets[I], ZEVector4::Zero);

		PrecomputeConstants.IndexZ = ((float)(I % 64) + 0.5f) / 64.0f;
		PrecomputeConstants.IndexW = ((float)(I / 64) + 0.5f) / 16.0f;
		PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

		Context->SetRenderState(PrecomputeSingleScatteringRenderStateData);
		Context->SetRenderTargets(1, RenderTarget.GetPointerToPointer(), NULL);

		Context->Draw(3, 0);
	}

	for(ZEUInt S = 1; S < OrderCount; S++)
	{
		for(ZEUInt J = 0; J < 1024; J++)
		{
			ZEHolder<const ZEGRRenderTarget> RenderTarget = PrecomputedHighOrderScatteringBuffer->GetRenderTarget(J);
			PrecomputeConstants.IndexZ = ((float)(J % 64) + 0.5f) / 64.0f;
			PrecomputeConstants.IndexW = ((float)(J / 64) + 0.5f) / 16.0f;
			PrecomputeConstantBuffer->SetData(&PrecomputeConstants);
	
			Context->SetTexture(ZEGR_ST_PIXEL, 0, (S == 1) ? PrecomputedSingleScatteringBuffer : PrecomputedHighOrderInScatteringBuffer);
			Context->SetTexture(ZEGR_ST_PIXEL, 3, RandomVectorsTexture);
			const ZEGRRenderTarget* RenderTarget3 = RenderTarget.GetPointer();
			Context->SetRenderTargets(1, &RenderTarget3, NULL);
			Context->SetRenderState(PrecomputeHighOrderScatteringRenderStateData);
		
			Context->Draw(3, 0);
		}
	
		for(ZEUInt J = 0; J < 1024; J++)
		{
			ZEHolder<const ZEGRRenderTarget> RenderTarget = PrecomputedHighOrderInScatteringBuffer->GetRenderTarget(J);
			PrecomputeConstants.IndexZ = ((float)(J % 64) + 0.5f) / 64.0f;
			PrecomputeConstants.IndexW = ((float)(J / 64) + 0.5f) / 16.0f;
			PrecomputeConstantBuffer->SetData(&PrecomputeConstants);
	
			Context->SetTexture(ZEGR_ST_PIXEL, 1, PrecomputedHighOrderScatteringBuffer);
			Context->SetRenderTargets(1, RenderTarget.GetPointerToPointer(), NULL);
			Context->SetRenderState(PrecomputeHighOrderInScatteringRenderStateData);
	
			Context->Draw(3, 0);
		}
	
		for(ZEUInt J = 0; J < 1024; J++)
		{
			ZEHolder<const ZEGRRenderTarget> RenderTarget = MultipleScatteringRenderTargets[J];
			PrecomputeConstants.IndexZ = (float)J;
			PrecomputeConstantBuffer->SetData(&PrecomputeConstants);
	
			Context->SetTexture(ZEGR_ST_PIXEL, 0, PrecomputedHighOrderInScatteringBuffer);
			Context->SetRenderTargets(1, RenderTarget.GetPointerToPointer(), NULL);
			Context->SetRenderState(AddOrdersRenderStateData);
	
			Context->Draw(3, 0);
		}
	}
	
	for(ZEUInt K = 0; K < 1024; K++)
	{
		ZEHolder<const ZEGRRenderTarget> RenderTarget = MultipleScatteringRenderTargets[K];
		PrecomputeConstants.IndexZ = (float)K;
		PrecomputeConstantBuffer->SetData(&PrecomputeConstants);
	
		Context->SetTexture(ZEGR_ST_PIXEL, 0, PrecomputedSingleScatteringBuffer);
		Context->SetRenderTargets(1, RenderTarget.GetPointerToPointer(), NULL);
		Context->SetRenderState(AddOrdersRenderStateData);
	
		Context->Draw(3, 0);
	}

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);

	const ZEGRRenderTarget* SkyAmbientRenderTarget = PrecomputedSkyAmbientBuffer->GetRenderTarget();

	Context->SetRenderState(PrecomputeSkyAmbientRenderStateData);
	Context->SetRenderTargets(1, &SkyAmbientRenderTarget, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, UseMultipleScattering ? PrecomputedMultipleScatteringBuffer : PrecomputedSingleScatteringBuffer);
	Context->SetVertexBuffers(0, 0, NULL);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, 1024.0f, 1.0f));

	Context->Draw(3, 0);

	Context->SetSampler(ZEGR_ST_PIXEL, 0, NULL);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, PreviousTexture0);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, PreviousTexture2);
	Context->SetTexture(ZEGR_ST_PIXEL, 3, PreviousTexture3);
	Context->SetTexture(ZEGR_ST_PIXEL, 4, PreviousTexture4);

	Context->SetRenderTargets(1, &PreviousRenderTarget, NULL);

	ReleasePrecomputeResources();
}

void ZEATAtmosphericScattering::ReleasePrecomputeResources()
{
	PrecomputeDensityPixelShader.Release();
	PrecomputeSingleScatteringPixelShader.Release();
	PrecomputeHighOrderScatteringPixelShader.Release();
	PrecomputeHighOrderInScatteringPixelShader.Release();
	AddOrdersPixelShader.Release();
	PrecomputeSkyAmbientPixelShader.Release();

	PrecomputeDensityRenderStateData.Release();
	PrecomputeSingleScatteringRenderStateData.Release();
	PrecomputeHighOrderScatteringRenderStateData.Release();
	PrecomputeHighOrderInScatteringRenderStateData.Release();
	AddOrdersRenderStateData.Release();
	PrecomputeSkyAmbientRenderStateData.Release();

	PrecomputedHighOrderScatteringBuffer.Release();
	PrecomputedHighOrderInScatteringBuffer.Release();

	PrecomputeConstantBuffer.Release();
}

bool ZEATAtmosphericScattering::InitializeSelf()
{
	CreateRandomVectors();

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));
	PrecomputeConstantBuffer = ZEGRConstantBuffer::Create(sizeof(PrecomputeConstants));

	PrecomputedDensityBuffer = ZEGRTexture2D::CreateInstance(1024, 1024, 1, ZEGR_TF_R32G32_FLOAT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE);
	PrecomputedSingleScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R16G16B16A16_FLOAT, true, false);
	PrecomputedHighOrderScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R32G32B32A32_FLOAT, true, false);
	PrecomputedHighOrderInScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R32G32B32A32_FLOAT, true, false);
	PrecomputedMultipleScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R16G16B16A16_FLOAT, true, false);
	PrecomputedSkyAmbientBuffer = ZEGRTexture2D::CreateInstance(1024, 1, 1, ZEGR_TF_R16G16B16A16_FLOAT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE);

	SamplerLinearClamp = ZEGRSampler::GetSampler(ZEGRSamplerDescription());

	return true;
}

void ZEATAtmosphericScattering::DeinitializeSelf()
{
	DirtyFlags.RaiseAll();

	ScreenCoverVertexShader.Release();
	SkyLightingPixelShader.Release();
	SkyAndAerialPerspectivePixelShader.Release();
	SkyLightingRenderStateData.Release();
	SkyAndAerialPerspectiveRenderStateData.Release();
	ConstantBuffer.Release();

	PrecomputedDensityBuffer.Release();
	PrecomputedSingleScatteringBuffer.Release();
	PrecomputedMultipleScatteringBuffer.Release();
	PrecomputedSkyAmbientBuffer.Release();

	ReleasePrecomputeResources();
}

void ZEATAtmosphericScattering::SetLightDirection(const ZEVector3& LightDirection)
{
	if(Constants.LightDirection == LightDirection)
		return;

	Constants.LightDirection = LightDirection;

	DirtyFlags.RaiseFlags(ZEAT_ASDF_CONSTANT_BUFFER);
}

const ZEVector3& ZEATAtmosphericScattering::GetLightDirection() const
{
	return Constants.LightDirection;
}

void ZEATAtmosphericScattering::SetLightIntensity(float LightIntensity)
{
	if(Constants.LightIntensity == LightIntensity)
		return;

	Constants.LightIntensity = LightIntensity;

	DirtyFlags.RaiseFlags(ZEAT_ASDF_CONSTANT_BUFFER);
}

float ZEATAtmosphericScattering::GetLightIntensity() const
{
	return Constants.LightIntensity;
}

void ZEATAtmosphericScattering::SetLightColor(const ZEVector3& LightColor)
{
	if(Constants.LightColor == LightColor)
		return;

	Constants.LightColor = LightColor;

	DirtyFlags.RaiseFlags(ZEAT_ASDF_CONSTANT_BUFFER);
}

const ZEVector3& ZEATAtmosphericScattering::GetLightColor() const
{
	return Constants.LightColor;
}

void ZEATAtmosphericScattering::SetOrderCount(ZEUInt OrderCount)
{
	this->OrderCount = OrderCount;
}

ZEUInt ZEATAtmosphericScattering::GetOrderCount() const
{
	return OrderCount;
}

void ZEATAtmosphericScattering::SetMultipleScattering(bool UseMultipleScattering)
{
	this->UseMultipleScattering = UseMultipleScattering;
}

float ZEATAtmosphericScattering::GetMultipleScattering() const
{
	return UseMultipleScattering;
}

void ZEATAtmosphericScattering::PreProcess(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	if(!Update())
		return;

	static bool Precomputed = false;
	if(!Precomputed)
	{
		Precomputed = true;
		PrecomputeWithPixelShader(Context);
	}

	ZEGRRenderTarget* RenderTarget = Renderer->GetOutputRenderTarget();
	if (RenderTarget == NULL)
		return;

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, ConstantBuffer);
	Context->SetRenderState(SkyLightingRenderStateData);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearClamp);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, PrecomputedDensityBuffer);
	Context->SetTexture(ZEGR_ST_PIXEL, 6, PrecomputedSkyAmbientBuffer);
	Context->SetVertexBuffers(0, 0, NULL);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, RenderTarget->GetWidth(), RenderTarget->GetHeight()));

	Context->Draw(3, 0);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 6, NULL);
}

void ZEATAtmosphericScattering::PostProcess(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	if(!Update())
		return;

	ZEGRRenderTarget* RenderTarget = Renderer->GetOutputRenderTarget();
	if (RenderTarget == NULL)
		return;

	ZERNStageGBuffer* StageGBuffer = static_cast<ZERNStageGBuffer*>(Renderer->GetStage(ZERN_STAGE_GBUFFER));
	if (StageGBuffer == NULL)
		return;

	ZEGRRenderTarget* PrevRenderTarget;
	ZEGRDepthStencilBuffer* PrevDepthStencilBuffer;
	Context->GetRenderTargets(1, &PrevRenderTarget, &PrevDepthStencilBuffer);
	Context->SetRenderTargets(1, &PrevRenderTarget, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, StageGBuffer->GetDepthMap());

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, ConstantBuffer);
	Context->SetRenderState(SkyAndAerialPerspectiveRenderStateData);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearClamp);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, UseMultipleScattering ? PrecomputedMultipleScatteringBuffer : PrecomputedSingleScatteringBuffer);
	Context->SetVertexBuffers(0, 0, NULL);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, RenderTarget->GetWidth(), RenderTarget->GetHeight()));

	Context->Draw(3, 0);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetRenderTargets(1, &PrevRenderTarget, PrevDepthStencilBuffer);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, NULL);
}

ZEATAtmosphericScattering::ZEATAtmosphericScattering()
{
	DirtyFlags.RaiseAll();

	OrderCount = 4;
	UseMultipleScattering = false;

	PrecomputeConstants.RayleighScatteringFactor = ZEVector4(5.8f, 13.5f, 33.1f, 0.0f) * 0.000001f;
	PrecomputeConstants.MieScatteringFactor = ZEVector4(2.0f, 2.0f, 2.0f, 0.0f) * 0.00001f;
}

ZEATAtmosphericScattering::~ZEATAtmosphericScattering()
{

}
