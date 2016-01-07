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
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRContext.h"
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
	/*const ZEUInt Size = 128 * 128 * 4;
	ZEUInt8 Vectors[Size];
	for(ZEUInt I = 0; I < Size; I += 4)
	{
		Vectors[I]		= (ZEUInt8)(ZERandom::GetFloatPositive() * 255.0f + 0.5f);
		Vectors[I + 1]	= (ZEUInt8)(ZERandom::GetFloatPositive() * 255.0f + 0.5f);
		Vectors[I + 2]	= (ZEUInt8)(ZERandom::GetFloatPositive() * 255.0f + 0.5f);
		Vectors[I + 3]	= 0;
	}

	RandomVectorsTexture.Release();
	RandomVectorsTexture = ZEGRTexture2D::CreateInstance(128, 128, 1, 1, ZEGR_TF_R8G8B8A8_UNORM);
	RandomVectorsTexture->UpdateSubResource(&Vectors[0], 0, 0, 128 * 4);*/

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
	RandomVectorsTexture = ZEGRTexture2D::CreateInstance(SphereSamples.GetCount(), 1, 1, 1, ZEGR_TF_R32G32B32A32_FLOAT);
	RandomVectorsTexture->UpdateSubResource(&SphereSamples[0], 0, 0, SphereSamples.GetCount() * sizeof(ZEVector4));
}

void ZEATAtmosphericScattering::CreateOffsetVectors()
{
	ZEVector4 Samples[] =
	{
		ZEVector4(-1.0f, -1.0f, -1.0f, 0.0f),
		ZEVector4(-1.0f, 1.0f, -1.0f, 0.0f),
		ZEVector4(1.0f, 1.0f, -1.0f, 0.0f),
		ZEVector4(1.0f, -1.0f, -1.0f, 0.0f),

		ZEVector4(-1.0f, -1.0f, 1.0f, 0.0f),
		ZEVector4(-1.0f, 1.0f, 1.0f, 0.0f),
		ZEVector4(1.0f, 1.0f, 1.0f, 0.0f),
		ZEVector4(1.0f, -1.0f, 1.0f, 0.0f),

		ZEVector4(-1.0f, 0.0f, 0.0f, 0.0f),
		ZEVector4(1.0f, 0.0f, 0.0f, 0.0f),
		ZEVector4(0.0f, -1.0f, 0.0f, 0.0f),
		ZEVector4(0.0f, 1.0f, 0.0f, 0.0f),
		ZEVector4(0.0f, 0.0f, -1.0f, 0.0f),
		ZEVector4(0.0f, 0.0f, 1.0f, 0.0f),
	};

	for (ZEUInt I = 0; I < 14; ++I)
	{
		Samples[I].NormalizeSelf();
		PrecomputeConstants.OffsetVectors[I] = Samples[I] * (ZERandom::GetFloatPositive() * 0.75f + 0.25f);
	}
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

void ZEATAtmosphericScattering::SetMieScatteringStrengh(float MieScatteringStrength)
{
	if(Constants.MieScatteringStrength == MieScatteringStrength)
		return;

	Constants.MieScatteringStrength = MieScatteringStrength;

	DirtyFlags.RaiseFlags(ZEAT_ASDF_CONSTANT_BUFFER);
}

float ZEATAtmosphericScattering::GetMieScatteringStrengh() const
{
	return Constants.MieScatteringStrength;
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
	Options.EntryPoint = "ZERNPrecomputingLightScattering_Extinction_PixelShader_Main";
	PrecomputeExtinctionPixelShader = ZEGRShader::Compile(Options);

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

	RenderState.SetShader(ZEGR_ST_PIXEL, PrecomputeExtinctionPixelShader);

	PrecomputeExtinctionRenderStateData = RenderState.Compile();
	zeCheckError(PrecomputeExtinctionRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, PrecomputeSingleScatteringPixelShader);

	PrecomputeSingleScatteringRenderStateData = RenderState.Compile();
	zeCheckError(PrecomputeSingleScatteringRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, PrecomputeHighOrderScatteringPixelShader);

	PrecomputeHighOrderScatteringRenderStateData = RenderState.Compile();
	zeCheckError(PrecomputeHighOrderScatteringRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, PrecomputeHighOrderInScatteringPixelShader);

	PrecomputeHighOrderInScatteringRenderStateData = RenderState.Compile();
	zeCheckError(PrecomputeHighOrderInScatteringRenderStateData == NULL, false, "Cannot set render state.");

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
	//CreateOffsetVectors();

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));
	PrecomputeConstantBuffer = ZEGRConstantBuffer::Create(sizeof(PrecomputeConstants));

	PrecomputedExtinctionBuffer = ZEGRTexture2D::CreateInstance(32, 128, 1, 1, ZEGR_TF_R16G16B16A16_FLOAT, true, false);
	PrecomputedSingleScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R16G16B16A16_FLOAT, true, false);
	PrecomputedHighOrderScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R32G32B32A32_FLOAT, true, false);
	PrecomputedHighOrderInScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R32G32B32A32_FLOAT, true, false);
	PrecomputedMultipleScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R16G16B16A16_FLOAT, true, false);

	DirtyFlags.RaiseAll();

	SamplerPointWrap.SetMinFilter(ZEGR_TFM_POINT);
	SamplerPointWrap.SetMagFilter(ZEGR_TFM_POINT);
	SamplerPointWrap.SetMipFilter(ZEGR_TFM_POINT);
	SamplerPointWrap.SetAddressU(ZEGR_TAM_WRAP);
	SamplerPointWrap.SetAddressV(ZEGR_TAM_WRAP);
	SamplerPointWrap.SetAddressW(ZEGR_TAM_WRAP);

	return true;
}

void ZEATAtmosphericScattering::DeinitializeSelf()
{
	VertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
	ConstantBuffer.Release();

	PrecomputedExtinctionBuffer.Release();
	PrecomputedSingleScatteringBuffer.Release();
	PrecomputedMultipleScatteringBuffer.Release();

	ReleasePrecomputeResources();
}

void ZEATAtmosphericScattering::PrecomputeWithPixelShader(ZEGRContext* Context)
{
	ZEGRRenderTarget* PreviousRenderTarget = NULL;
	Context->GetRenderTargets(1, &PreviousRenderTarget, NULL);

	ZEGRTexture* PreviousTexture0;
	ZEGRTexture* PreviousTexture1;
	Context->GetTexture(ZEGR_ST_PIXEL, 0, &PreviousTexture0);
	Context->GetTexture(ZEGR_ST_PIXEL, 1, &PreviousTexture1);

	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearClamp);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, 32.0f, 128.0f));
	Context->SetVertexBuffers(0, 0, NULL);
	
	ZEArray<ZEGRRenderTarget*> MultipleScatteringRenderTargets;
	MultipleScatteringRenderTargets.Resize(1024);	//64 * 16

	ZEGRRenderTarget* RenderTarget = PrecomputedExtinctionBuffer->GetRenderTarget();
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetRenderState(PrecomputeExtinctionRenderStateData);

	Context->Draw(3, 0);

	for(ZEUInt I = 0; I < 1024; I++)
	{
		ZEGRRenderTarget* RenderTarget = PrecomputedSingleScatteringBuffer->GetRenderTarget(I);
		MultipleScatteringRenderTargets[I] = PrecomputedMultipleScatteringBuffer->GetRenderTarget(I);
		Context->ClearRenderTarget(MultipleScatteringRenderTargets[I], ZEVector4::Zero);

		PrecomputeConstants.Index = ((float)(I % 64) + 0.5f) / 64.0f;
		PrecomputeConstants.Index2 = ((float)(I / 64) + 0.5f) / 16.0f;
		PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

		Context->SetTexture(ZEGR_ST_PIXEL, 2, PrecomputedExtinctionBuffer);
		Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
		Context->SetRenderTargets(1, &RenderTarget, NULL);
		Context->SetRenderState(PrecomputeSingleScatteringRenderStateData);

		Context->Draw(3, 0);
	}

	for(ZEUInt S = 1; S < OrderCount; S++)
	{
		for(ZEUInt J = 0; J < 1024; J++)
		{
			ZEGRRenderTarget* RenderTarget = PrecomputedHighOrderScatteringBuffer->GetRenderTarget(J);
			PrecomputeConstants.Index = ((float)(J % 64) + 0.5f) / 64.0f;
			PrecomputeConstants.Index2 = ((float)(J / 64) + 0.5f) / 16.0f;
			PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

			Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
			Context->SetSampler(ZEGR_ST_PIXEL, 1, SamplerPointWrap);
			Context->SetTexture(ZEGR_ST_PIXEL, 0, (S == 1) ? PrecomputedSingleScatteringBuffer : PrecomputedHighOrderInScatteringBuffer);
			Context->SetTexture(ZEGR_ST_PIXEL, 3, RandomVectorsTexture);
			Context->SetRenderTargets(1, &RenderTarget, NULL);
			Context->SetRenderState(PrecomputeHighOrderScatteringRenderStateData);
		
			Context->Draw(3, 0);
		}

		for(ZEUInt J = 0; J < 1024; J++)
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

		for(ZEUInt J = 0; J < 1024; J++)
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

	for(ZEUInt K = 0; K < 1024; K++)
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

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, PreviousTexture0);
	Context->SetTexture(ZEGR_ST_PIXEL, 1, PreviousTexture1);

	Context->SetRenderTargets(1, &PreviousRenderTarget, NULL);

	ReleasePrecomputeResources();
}

void ZEATAtmosphericScattering::ReleasePrecomputeResources()
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
	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearClamp);
	Context->SetVertexBuffers(0, 0, NULL);

	Context->SetTexture(ZEGR_ST_PIXEL, 5, UseMultipleScattering ? PrecomputedMultipleScatteringBuffer : PrecomputedSingleScatteringBuffer);
	Context->SetRenderState(RenderStateData);

	Context->Draw(3, 0);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
}

ZEATAtmosphericScattering::ZEATAtmosphericScattering()
{
	OrderCount = 4;
	UseMultipleScattering = false;
}

ZEATAtmosphericScattering::~ZEATAtmosphericScattering()
{
	DeinitializeSelf();
}
