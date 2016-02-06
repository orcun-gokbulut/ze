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
#include "ZERandom.h"

#define ZEAT_ASDF_SHADERS			1
#define ZEAT_ASDF_RENDER_STATE		2
#define ZEAT_ASDF_CONSTANT_BUFFER	4

void ZEATAtmosphericScattering::CreateRandomVectors()
{
	ZEArray<ZEVector4> SphereSamples;

	float ZenithStepAngle = ZE_PI / 8.0f;
	float AzimuthStepAngle = ZE_PIx2 / 16.0f;
	for(float I = 0.0f; I < ZE_PI; I += ZenithStepAngle)
	{
		float Y = ZEAngle::Cos(I);
		float ProjXZ = ZEAngle::Sin(I);
		for(float J = 0.0f; J < ZE_PIx2; J += AzimuthStepAngle)
		{
			float X = ProjXZ * ZEAngle::Sin(J);
			float Z = ProjXZ * ZEAngle::Cos(J);

			SphereSamples.Add(ZEVector4(X, Y, Z, 0.0f));
		}
	}

	RandomVectorsTexture.Release();
	RandomVectorsTexture = ZEGRTexture2D::CreateInstance(SphereSamples.GetCount(), 1, 1, 1, 1, ZEGR_TF_R32G32B32A32_FLOAT);
	RandomVectorsTexture->UpdateSubResource(0, 0, &SphereSamples[0], SphereSamples.GetCount() * sizeof(ZEVector4));
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

bool ZEATAtmosphericScattering::UpdateShaders()
{
	if(!DirtyFlags.GetFlags(ZEAT_ASDF_SHADERS))
		return true;

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

	DirtyFlags.UnraiseFlags(ZEAT_ASDF_SHADERS);
	DirtyFlags.RaiseFlags(ZEAT_ASDF_RENDER_STATE);

	return true;
}

bool ZEATAtmosphericScattering::UpdateRenderState()
{
	if(!DirtyFlags.GetFlags(ZEAT_ASDF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState;
	
	ZEGRDepthStencilState DepthStencilState;
	DepthStencilState.SetDepthTestEnable(false);
	DepthStencilState.SetDepthWriteEnable(false);

	RenderState.SetDepthStencilState(DepthStencilState);

	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PixelShader);

	RenderStateData = RenderState.Compile();
	zeCheckError(RenderStateData == NULL, false, "Cannot set render state.");

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

	ZEGRBlendState BlendState;
	BlendState.SetBlendEnable(true);
	ZEGRBlendRenderTarget BlendRenderTarget = BlendState.GetRenderTarget(0);
	BlendRenderTarget.SetBlendEnable(true);
	BlendRenderTarget.SetSource(ZEGR_BO_ONE);
	BlendRenderTarget.SetDestination(ZEGR_BO_ONE);
	BlendRenderTarget.SetOperation(ZEGR_BE_ADD);

	BlendState.SetRenderTargetBlend(0, BlendRenderTarget);

	RenderState.SetBlendState(BlendState);
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
	if(!UpdateShaders())
		return false;

	if(!UpdateRenderState())
		return false;

	if(!UpdateConstantBuffers())
		return false;

	return true;
}

bool ZEATAtmosphericScattering::InitializeSelf()
{
	CreateRandomVectors();

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));
	PrecomputeConstantBuffer = ZEGRConstantBuffer::Create(sizeof(PrecomputeConstants));

	PrecomputedDensityBuffer = ZEGRTexture2D::CreateInstance(32, 128, 1, 1, 1, ZEGR_TF_R16G16_FLOAT, true);
	PrecomputedSingleScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R16G16B16A16_FLOAT, true, false);
	PrecomputedHighOrderScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R32G32B32A32_FLOAT, true, false);
	PrecomputedHighOrderInScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R32G32B32A32_FLOAT, true, false);
	PrecomputedMultipleScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R16G16B16A16_FLOAT, true, false);
	PrecomputedSkyAmbientBuffer = ZEGRTexture2D::CreateInstance(128, 1, 1, 1, 1, ZEGR_TF_R16G16B16A16_FLOAT, true);

	DirtyFlags.RaiseAll();

	SamplerLinearClamp = ZEGRSampler::GetSampler(ZEGRSamplerDescription());

	return true;
}

void ZEATAtmosphericScattering::DeinitializeSelf()
{
	VertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
	ConstantBuffer.Release();

	PrecomputedDensityBuffer.Release();
	PrecomputedSingleScatteringBuffer.Release();
	PrecomputedMultipleScatteringBuffer.Release();
	PrecomputedSkyAmbientBuffer.Release();

	ReleasePrecomputeResources();
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
	ComputeRayleighMiePhaseFactors();

	ZEGRRenderTarget* PreviousRenderTarget;
	Context->GetRenderTargets(1, &PreviousRenderTarget, NULL);

	ZEGRTexture* PreviousTexture0;
	ZEGRTexture* PreviousTexture1;
	ZEGRTexture* PreviousTexture2;
	Context->GetTexture(ZEGR_ST_PIXEL, 0, &PreviousTexture0);
	Context->GetTexture(ZEGR_ST_PIXEL, 1, &PreviousTexture1);
	Context->GetTexture(ZEGR_ST_PIXEL, 2, &PreviousTexture2);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearClamp.GetPointer());
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, 32.0f, 128.0f));
	Context->SetVertexBuffers(0, 0, NULL);
	
	ZEArray<ZEHolder<const ZEGRRenderTarget>> MultipleScatteringRenderTargets;
	MultipleScatteringRenderTargets.Resize(1024);	//64 * 16

	ZEHolder<const ZEGRRenderTarget> DensityRenderTarget = PrecomputedDensityBuffer->GetRenderTarget();
	PrecomputeConstantBuffer->SetData(&PrecomputeConstants);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
	Context->SetRenderTargets(1, &DensityRenderTarget, NULL);
	Context->SetRenderState(PrecomputeDensityRenderStateData);

	Context->Draw(3, 0);

	for(ZEUInt I = 0; I < 1024; I++)
	{
		ZEHolder<const ZEGRRenderTarget> RenderTarget = PrecomputedSingleScatteringBuffer->GetRenderTarget(I);
		MultipleScatteringRenderTargets[I] = PrecomputedMultipleScatteringBuffer->GetRenderTarget(I);
		Context->ClearRenderTarget(MultipleScatteringRenderTargets[I], ZEVector4::Zero);

		PrecomputeConstants.IndexZ = ((float)(I % 64) + 0.5f) / 64.0f;
		PrecomputeConstants.IndexW = ((float)(I / 64) + 0.5f) / 16.0f;
		PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

		Context->SetTexture(ZEGR_ST_PIXEL, 2, PrecomputedDensityBuffer);
		Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
		Context->SetRenderTargets(1, &RenderTarget, NULL);
		Context->SetRenderState(PrecomputeSingleScatteringRenderStateData);

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

			Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
			Context->SetTexture(ZEGR_ST_PIXEL, 0, (S == 1) ? PrecomputedSingleScatteringBuffer : PrecomputedHighOrderInScatteringBuffer);
			Context->SetTexture(ZEGR_ST_PIXEL, 3, RandomVectorsTexture);
			Context->SetRenderTargets(1, &RenderTarget, NULL);
			Context->SetRenderState(PrecomputeHighOrderScatteringRenderStateData);
		
			Context->Draw(3, 0);
		}

		for(ZEUInt J = 0; J < 1024; J++)
		{
			ZEHolder<const ZEGRRenderTarget> RenderTarget = PrecomputedHighOrderInScatteringBuffer->GetRenderTarget(J);
			PrecomputeConstants.IndexZ = ((float)(J % 64) + 0.5f) / 64.0f;
			PrecomputeConstants.IndexW = ((float)(J / 64) + 0.5f) / 16.0f;
			PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

			Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
			Context->SetTexture(ZEGR_ST_PIXEL, 1, PrecomputedHighOrderScatteringBuffer);
			Context->SetRenderTargets(1, &RenderTarget, NULL);
			Context->SetRenderState(PrecomputeHighOrderInScatteringRenderStateData);

			Context->Draw(3, 0);
		}

		for(ZEUInt J = 0; J < 1024; J++)
		{
			ZEHolder<const ZEGRRenderTarget> RenderTarget = MultipleScatteringRenderTargets[J];
			PrecomputeConstants.IndexZ = (float)J;
			PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

			Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
			Context->SetTexture(ZEGR_ST_PIXEL, 0, PrecomputedHighOrderInScatteringBuffer);
			Context->SetRenderTargets(1, &RenderTarget, NULL);
			Context->SetRenderState(AddOrdersRenderStateData);

			Context->Draw(3, 0);
		}
	}

	for(ZEUInt K = 0; K < 1024; K++)
	{
		ZEHolder<const ZEGRRenderTarget> RenderTarget = MultipleScatteringRenderTargets[K];
		PrecomputeConstants.IndexZ = (float)K;
		PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

		Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
		Context->SetTexture(ZEGR_ST_PIXEL, 0, PrecomputedSingleScatteringBuffer);
		Context->SetRenderTargets(1, &RenderTarget, NULL);
		Context->SetRenderState(AddOrdersRenderStateData);

		Context->Draw(3, 0);
	}

	ZEHolder<const ZEGRRenderTarget> SkyAmbientRenderTarget = PrecomputedSkyAmbientBuffer->GetRenderTarget();

	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, 128.0f, 1.0f));
	Context->SetTexture(ZEGR_ST_PIXEL, 0, PrecomputedMultipleScatteringBuffer);
	Context->SetTexture(ZEGR_ST_PIXEL, 3, RandomVectorsTexture);
	Context->SetRenderTargets(1, &SkyAmbientRenderTarget, NULL);
	Context->SetRenderState(PrecomputeSkyAmbientRenderStateData);

	Context->Draw(3, 0);


	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, PreviousTexture0);
	Context->SetTexture(ZEGR_ST_PIXEL, 1, PreviousTexture1);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, PreviousTexture2);

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

void ZEATAtmosphericScattering::Process(ZEGRContext* Context)
{
	if(!Update())
		return;

	static bool Precomputed = false;
	if(!Precomputed)
	{
		Precomputed = true;
		PrecomputeWithPixelShader(Context);
	}

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, ConstantBuffer);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearClamp.GetPointer());
	Context->SetVertexBuffers(0, 0, NULL);

	Context->SetTexture(ZEGR_ST_PIXEL, 5, UseMultipleScattering ? PrecomputedMultipleScatteringBuffer : PrecomputedSingleScatteringBuffer);
	Context->SetTexture(ZEGR_ST_PIXEL, 6, PrecomputedDensityBuffer);
	Context->SetTexture(ZEGR_ST_PIXEL, 7, PrecomputedSkyAmbientBuffer);
	Context->SetRenderState(RenderStateData);

	Context->Draw(3, 0);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
}

ZEATAtmosphericScattering::ZEATAtmosphericScattering()
{
	OrderCount = 4;
	UseMultipleScattering = false;

	PrecomputeConstants.RayleighScatteringFactor = ZEVector4(5.8f, 13.5f, 33.1f, 0.0f) * 0.000001f;
	PrecomputeConstants.MieScatteringFactor = ZEVector4(2.0f, 2.0f, 2.0f, 0.0f) * 0.00001f;
}

ZEATAtmosphericScattering::~ZEATAtmosphericScattering()
{
	DeinitializeSelf();
}
