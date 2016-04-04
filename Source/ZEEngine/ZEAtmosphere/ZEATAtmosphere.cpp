//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATAtmosphere.cpp
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

#include "ZEATAtmosphere.h"

#include "ZERandom.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEViewVolume.h""
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRTexture3D.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRStructuredBuffer.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNCuller.h"
#include "ZERenderer/ZERNStageGBuffer.h"
#include "ZERenderer/ZELightDirectional.h"
#include "ZEATAstronomy.h"
#include "ZEATSun.h"
#include "ZEATMoon.h"
#include "ZEATFog.h"
#include "ZEATCloud.h"
#include "ZEATSkyBox.h"

#define ZEAT_ADF_SHADERS			1
#define ZEAT_ADF_RENDER_STATE		2
#define ZEAT_ADF_CONSTANT_BUFFER	4

void ZEATAtmosphere::CreateRandomVectors()
{
	ZEVector4 SphereSamples[128];
	for (ZEUInt I = 0; I < 128; I++)
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
	RandomVectorsTexture = ZEGRTexture2D::CreateInstance(128, 1, 1, ZEGR_TF_R32G32B32A32_FLOAT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE);
	RandomVectorsTexture->UpdateSubResource(0, 0, &SphereSamples[0], 128 * sizeof(ZEVector4));
}

bool ZEATAtmosphere::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZEAT_ADF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNFastLightScattering.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFastLightScattering_PixelShader_Main";
	SkyPixelShader = ZEGRShader::Compile(Options);

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNPrecomputingLightScattering.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_Position";
	ScreenCoverVertexShader = ZEGRShader::Compile(Options);

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

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNPrecomputingLightScattering_SkyAmbient_PixelShader_Main";
	PrecomputeSkyAmbientPixelShader = ZEGRShader::Compile(Options);

	DirtyFlags.UnraiseFlags(ZEAT_ADF_SHADERS);
	DirtyFlags.RaiseFlags(ZEAT_ADF_RENDER_STATE);

	return true;
}

bool ZEATAtmosphere::UpdateRenderState()
{
	if (!DirtyFlags.GetFlags(ZEAT_ADF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState;

	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);

	ZEGRDepthStencilState DepthStencilStateNoTestWrite;
	DepthStencilStateNoTestWrite.SetDepthTestEnable(true);
	DepthStencilStateNoTestWrite.SetDepthWriteEnable(false);

	RenderState.SetDepthStencilState(DepthStencilStateNoTestWrite);

	ZEGRBlendState BlendStateAlphaBlending;
	BlendStateAlphaBlending.SetBlendEnable(true);
	ZEGRBlendRenderTarget BlendRenderTargetAlphaBlending = BlendStateAlphaBlending.GetRenderTarget(0);
	BlendRenderTargetAlphaBlending.SetSource(ZEGRBlend::ZEGR_BO_ONE);
	BlendRenderTargetAlphaBlending.SetDestination(ZEGRBlend::ZEGR_BO_ONE);
	BlendRenderTargetAlphaBlending.SetOperation(ZEGRBlendOperation::ZEGR_BE_ADD);
	BlendRenderTargetAlphaBlending.SetBlendEnable(true);
	BlendStateAlphaBlending.SetRenderTargetBlend(0, BlendRenderTargetAlphaBlending);

	RenderState.SetBlendState(BlendStateAlphaBlending);

	RenderState.SetShader(ZEGR_ST_PIXEL, SkyPixelShader);

	SkyRenderStateData = RenderState.Compile();
	zeCheckError(SkyRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetBlendState(ZEGRBlendState());

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

	RenderState.SetShader(ZEGR_ST_PIXEL, PrecomputeSkyAmbientPixelShader);

	PrecomputeSkyAmbientRenderStateData = RenderState.Compile();
	zeCheckError(PrecomputeSkyAmbientRenderStateData == NULL, false, "Cannot set render state.");

	ZEGRBlendState BlendStateAdditive;
	BlendStateAdditive.SetBlendEnable(true);
	ZEGRBlendRenderTarget BlendRenderTargetAdditive = BlendStateAdditive.GetRenderTarget(0);
	BlendRenderTargetAdditive.SetSource(ZEGRBlend::ZEGR_BO_ONE);
	BlendRenderTargetAdditive.SetDestination(ZEGRBlend::ZEGR_BO_ONE);
	BlendRenderTargetAdditive.SetOperation(ZEGRBlendOperation::ZEGR_BE_ADD);
	BlendRenderTargetAdditive.SetBlendEnable(true);
	BlendStateAdditive.SetRenderTargetBlend(0, BlendRenderTargetAdditive);

	RenderState.SetBlendState(BlendStateAdditive);
	RenderState.SetShader(ZEGR_ST_PIXEL, AddOrdersPixelShader);

	AddOrdersRenderStateData = RenderState.Compile();
	zeCheckError(AddOrdersRenderStateData == NULL, false, "Cannot set render state.");

	DirtyFlags.UnraiseFlags(ZEAT_ADF_RENDER_STATE);

	return true;
}

bool ZEATAtmosphere::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZEAT_ADF_CONSTANT_BUFFER))
		return true;

	SkyConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZEAT_ADF_CONSTANT_BUFFER);

	return true;
}

bool ZEATAtmosphere::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderState())
		return false;

	if (!UpdateConstantBuffers())
		return false;

	return true;
}

void ZEATAtmosphere::PrecomputeBuffers(ZEGRContext* Context)
{
	ZEHolder<ZEGRConstantBuffer> PrecomputeConstantBuffer = ZEGRConstantBuffer::Create(sizeof(PrecomputeConstants));

	ZEArray<ZEHolder<const ZEGRRenderTarget>> MultipleScatteringRenderTargets;
	MultipleScatteringRenderTargets.Resize(1024);	//64 * 16

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearClamp);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, RandomVectorsTexture);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, 32.0f, 128.0f));

	for (ZEUInt I = 0; I < 1024; I++)
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

	ZEHolder<ZEGRTexture3D> PrecomputedHighOrderScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R32G32B32A32_FLOAT, true, false);
	ZEHolder<ZEGRTexture3D> PrecomputedHighOrderInScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R32G32B32A32_FLOAT, true, false);

	for (ZEUInt S = 1; S < OrderCount; S++)
	{
		for(ZEUInt J = 0; J < 1024; J++)
		{
			ZEHolder<const ZEGRRenderTarget> RenderTarget = PrecomputedHighOrderScatteringBuffer->GetRenderTarget(J);
			PrecomputeConstants.IndexZ = ((float)(J % 64) + 0.5f) / 64.0f;
			PrecomputeConstants.IndexW = ((float)(J / 64) + 0.5f) / 16.0f;
			PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

			Context->SetTexture(ZEGR_ST_PIXEL, 0, (S == 1) ? PrecomputedSingleScatteringBuffer : PrecomputedHighOrderInScatteringBuffer);

			Context->SetRenderTargets(1, RenderTarget.GetPointerToPointer(), NULL);
			Context->SetRenderState(PrecomputeHighOrderScatteringRenderStateData);

			Context->Draw(3, 0);
		}

		for (ZEUInt J = 0; J < 1024; J++)
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

		for (ZEUInt J = 0; J < 1024; J++)
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

	for (ZEUInt K = 0; K < 1024; K++)
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
	Context->SetSampler(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 1, NULL);

	ZEHolder<ZEGRTexture2D> PrecomputedSkyAmbientBuffer = ZEGRTexture2D::CreateInstance(1024, 1, 1, ZEGR_TF_R32G32B32A32_FLOAT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE);
	ZEHolder<const ZEGRRenderTarget> SkyAmbientRenderTarget = PrecomputedSkyAmbientBuffer->GetRenderTarget();

	Context->SetRenderState(PrecomputeSkyAmbientRenderStateData);
	Context->SetRenderTargets(1, SkyAmbientRenderTarget.GetPointerToPointer(), NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, UseMultipleScattering ? PrecomputedMultipleScatteringBuffer : PrecomputedSingleScatteringBuffer);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, 1024.0f, 1.0f));

	Context->Draw(3, 0);

	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, NULL);

	ZEHolder<ZEGRTexture2D> AmbientBuffer = ZEGRTexture2D::CreateInstance(1024, 1, 1, ZEGR_TF_R32G32B32A32_FLOAT, ZEGR_RU_CPU_READ_WRITE, ZEGR_RBF_NONE);

	Context->CopyResource(AmbientBuffer, PrecomputedSkyAmbientBuffer);

	float* Data;
	AmbientBuffer->Lock(reinterpret_cast<void**>(&Data), NULL);
		memcpy(&SkyAmbient, Data, sizeof(ZEVector4) * 1024);
	AmbientBuffer->Unlock();

	//PrecomputedExtinctionBuffer = ZEGRTexture2D::CreateInstance(32, 128, 1,  ZEGR_TF_R32G32B32A32_FLOAT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE);
	//const ZEGRRenderTarget* ExtinctionRenderTarget = PrecomputedExtinctionBuffer->GetRenderTarget();
	//Context->SetRenderState(PrecomputeExtinctionRenderStateData);
	//Context->SetRenderTargets(1, &ExtinctionRenderTarget, NULL);
	//Context->SetVertexBuffers(0, 0, NULL);
	//Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, 32.0f, 128.0f));
	//
	//Context->Draw(3, 0);
	//
	//Context->SetRenderTargets(0, NULL, NULL);
	//
	//ZEHolder<ZEGRTexture2D> ExtinctionBuffer = ZEGRTexture2D::CreateInstance(32, 128, 1, ZEGR_TF_R32G32B32A32_FLOAT, ZEGR_RU_CPU_READ_WRITE, ZEGR_RBF_NONE);
	//
	//Context->CopyResource(ExtinctionBuffer, PrecomputedExtinctionBuffer);
	//
	//ZESize RowPitch;
	//ExtinctionBuffer->Lock(reinterpret_cast<void**>(&Data), &RowPitch);
	//	//for (ZEUInt I = 0; I < 1024; I++)
	//	//{
	//	//	memcpy(Extinction[I], Data + I * 1024, RowPitch);
	//	//}
	//memcpy(Extinction, Data, sizeof(ZEVector4) * 32 * 128);
	//ExtinctionBuffer->Unlock();

	PrecomputeExtinctionPixelShader.Release();
	PrecomputeSingleScatteringPixelShader.Release();
	PrecomputeHighOrderScatteringPixelShader.Release();
	PrecomputeHighOrderInScatteringPixelShader.Release();
	AddOrdersPixelShader.Release();
	PrecomputeSkyAmbientPixelShader.Release();

	PrecomputeExtinctionRenderStateData.Release();
	PrecomputeSingleScatteringRenderStateData.Release();
	PrecomputeHighOrderScatteringRenderStateData.Release();
	PrecomputeHighOrderInScatteringRenderStateData.Release();
	AddOrdersRenderStateData.Release();
	PrecomputeSkyAmbientRenderStateData.Release();

	RandomVectorsTexture.Release();
}

bool ZEATAtmosphere::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	CreateRandomVectors();

	SkyConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));

	PrecomputedSingleScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R16G16B16A16_FLOAT, true, false);
	PrecomputedMultipleScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R16G16B16A16_FLOAT, true, false);

	SamplerLinearClamp = ZEGRSampler::GetDefaultSampler();

	Sun = ZEATSun::CreateInstance();
	zeScene->AddEntity(Sun);

	Moon = ZEATMoon::CreateInstance();
	Moon->SetTextureFile("#R:/ZEEngine/ZEAtmosphere/Textures/MoonFrame.png", 53, 1);
	zeScene->AddEntity(Moon);

	Fog = ZEATFog::CreateInstance();
	Fog->SetDensity(0.0f);
	Fog->SetStartDistance(0.0f);
	Fog->SetColor(ZEVector3(0.5f));
	//zeScene->AddEntity(Fog);

	Cloud = ZEATCloud::CreateInstance();
	Cloud->SetCloudTexture("#R:/ZEEngine/ZEAtmosphere/Textures/Cloud.bmp");
	//zeScene->AddEntity(Cloud);

	Stars = ZEATSkyBox::CreateInstance();
	Stars->SetName("StarMap");
	Stars->SetVisible(true);
	Stars->SetEnabled(true);
	Stars->SetTexture("#R:/ZEEngine/ZEAtmosphere/Textures/StarMap.png");
	Stars->SetColor(ZEVector3::One);
	Stars->SetBrightness(0.1f);
	//zeScene->AddEntity(Stars);

	return true;
}

bool ZEATAtmosphere::DeinitializeSelf()
{
	DirtyFlags.RaiseAll();

	ScreenCoverVertexShader.Release();

	SkyPixelShader.Release();
	SkyRenderStateData.Release();
	SkyConstantBuffer.Release();

	PrecomputedSingleScatteringBuffer.Release();
	PrecomputedMultipleScatteringBuffer.Release();

	SamplerLinearClamp.Release();

	SunLight = NULL;
	MoonLight = NULL;

	return ZEEntity::DeinitializeSelf();
}

void ZEATAtmosphere::SetObserver(const ZEATObserver& Observer)
{
	this->Observer = Observer;
}

const ZEATObserver& ZEATAtmosphere::GetObserver() const
{
	return Observer;
}

void ZEATAtmosphere::SetMultipleScattering(bool MultipleScattering)
{
	UseMultipleScattering = MultipleScattering;
}

bool ZEATAtmosphere::GetMultipleScattering()
{
	return UseMultipleScattering;
}

ZEATAtmosphere::ZEATAtmosphere()
{
	DirtyFlags.RaiseAll();

	Command.Entity = this;
	Command.StageMask = ZERN_STAGE_POST_EFFECT;
	Command.Priority = 3;

	OrderCount = 4;
	UseMultipleScattering = true;

	SunLight = NULL;
	MoonLight = NULL;
}

ZEATAtmosphere::~ZEATAtmosphere()
{

}

ZEDrawFlags ZEATAtmosphere::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZEATAtmosphere::SetSunLight(ZELightDirectional* SunLight)
{
	if (SunLight != NULL && SunLight->GetUseSunLight())
		this->SunLight = SunLight;
	else
		this->SunLight = NULL;
}

ZELightDirectional* ZEATAtmosphere::GetSunLight()
{
	return SunLight;
}

void ZEATAtmosphere::SetMoonLight(ZELightDirectional* MoonLight)
{
	if (MoonLight != NULL && MoonLight->GetUseMoonLight())
		this->MoonLight = MoonLight;
	else
		this->MoonLight = NULL;
}

ZELightDirectional* ZEATAtmosphere::GetMoonLight()
{
	return MoonLight;
}

void ZEATAtmosphere::SetCloudCoverage(float CloudCoverage)
{
	Cloud->SetCloudCoverage(CloudCoverage);
}

float ZEATAtmosphere::GetCloudCoverage() const
{
	return Cloud->GetCloudCoverage();
}

void ZEATAtmosphere::SetCloudDensity(float CloudDensity)
{
	Cloud->SetCloudDensity(CloudDensity);
}

float ZEATAtmosphere::GetCloudDensity() const
{
	return Cloud->GetCloudDensity();
}

void ZEATAtmosphere::SetFogDensity(float FogDensity)
{
	Fog->SetDensity(FogDensity);
}

float ZEATAtmosphere::GetFogDensity() const
{
	return Fog->GetDensity();
}

static ZEVector2 CalculateDensity(const ZEVector3& Position)
{
	float Height = (Position - ZEVector3(0.0f, -EARTH_RADIUS, 0.0f)).Length() - EARTH_RADIUS;

	float RayleighDensity = ZEMath::Exp(-Height / RAYLEIGH_MIE_HEIGHT.x);
	float MieDensity = ZEMath::Exp(-Height / RAYLEIGH_MIE_HEIGHT.y);

	return ZEVector2(RayleighDensity, MieDensity);
}

static ZEVector2 IntegrateDensity(const ZEVector3& Start, const ZEVector3& End)
{
	static const float STEPCOUNT = 20;
	ZEVector2 PrevRayleighMieDensity = CalculateDensity(Start);
	ZEVector2 TotalRayleighMieDensity = ZEVector2::Zero;

	ZEVector3 StartToEnd = End - Start;
	ZEVector3 Direction = StartToEnd.Normalize();
	float StepLength = StartToEnd.Length() / STEPCOUNT;

	for(float S = 1.0f; S <= STEPCOUNT; S += 1.0f)
	{
		ZEVector3 Position = Start + Direction * S * StepLength;

		ZEVector2 CurRayleighMieDensity = CalculateDensity(Position);

		TotalRayleighMieDensity += (CurRayleighMieDensity + PrevRayleighMieDensity) * 0.5f * StepLength;

		PrevRayleighMieDensity = CurRayleighMieDensity;
	}

	return TotalRayleighMieDensity;
}

ZEVector3 ZEATAtmosphere::GetTerrestrialSunColor()
{
	if (SunLight == NULL)
		return ZEVector3::Zero;
	
	ZEVector3 SunDirection = SunLight->GetWorldRotation() * -ZEVector3::UnitZ;
	SunDirection.NormalizeSelf();

	//float CosSunZenith = ZEVector3::DotProduct(SunDirection, ZEVector3::UnitY) * 0.5f + 0.5f;
	//float HeightAboveEarth = zeScene->GetActiveCamera()->GetWorldPosition().y;
	//
	//ZEUInt Index1 = ((HeightAboveEarth - 20.0f) / (ATMOSPHERE_HEIGHT - 2.0f * 20.0f)) * 31.0f + 0.5f;
	//ZEUInt Index2 = CosSunZenith * 127.0f + 0.5f;
	//
	//ZEVector3 LUTResult = SunLight->GetColor() * Extinction[Index1][Index2].ToVector3();

	///////////////////////////////////////////////////////////////////////////////////////////////

	ZEVector3 StartPositionWorld = zeScene->GetActiveCamera()->GetWorldPosition();

	ZEBSphere Sphere(ZEVector3(0.0f, -EARTH_RADIUS, 0.0f), TOTAL_RADIUS);
	ZERay Ray(SunDirection, StartPositionWorld);
	float Length;
	ZEBSphere::IntersectionTest(Sphere, Ray, Length);
	ZEVector3 EndPositionWorld = StartPositionWorld + SunDirection * Length;

	ZEVector2 Density = IntegrateDensity(StartPositionWorld, EndPositionWorld);
	ZEVector3 Extinction = ZEATRayleighScatteringFactor * Density.x + ZEATMieScatteringFactor * Density.y;

	float R = ZEMath::Exp(-Extinction.x);
	float G = ZEMath::Exp(-Extinction.y);
	float B = ZEMath::Exp(-Extinction.z);

	ZEVector3 ComputeResult = SunLight->GetColor() * ZEVector3(R, G, B);

	return ComputeResult;
}

ZEVector3 ZEATAtmosphere::GetTerrestrialSunAmbientColor()
{
	if (SunLight == NULL)
		return ZEVector3::Zero;

	ZEVector3 SunDirection = SunLight->GetWorldRotation() * -ZEVector3::UnitZ;
	SunDirection.NormalizeSelf();
	float CosSunZenith = ZEVector3::DotProduct(SunDirection, ZEVector3::UnitY) * 0.5f + 0.5f;

	float TexelWidth = 1.0f / 1024.0f;
	float TexelIndex1 = CosSunZenith - TexelWidth;
	float TexelIndex2 = CosSunZenith + TexelWidth;
	ZEUInt Index1 = TexelIndex1 * 1023.0f + 0.5f;
	ZEUInt Index2 = TexelIndex2 * 1023.0f + 0.5f;
	float Weight1 = CosSunZenith - ((Index1 + 0.5f) / 1024.0f);
	float Weight2 = ((Index2 + 0.5f) / 1024.0f) - CosSunZenith;

	ZEVector4 InterpolatedAmbient = (Weight1 * SkyAmbient[Index1] + Weight2 * SkyAmbient[Index2]) / (Weight1 + Weight2);

	return SunLight->GetColor() * InterpolatedAmbient.ToVector3();
}

ZEVector3 ZEATAtmosphere::GetTerrestrialMoonColor()
{
	if (MoonLight == NULL)
		return ZEVector3::Zero;

	ZEVector3 MoonDirection = MoonLight->GetWorldRotation() * -ZEVector3::UnitZ;
	MoonDirection.NormalizeSelf();

	//float CosMoonZenith = ZEVector3::DotProduct(MoonDirection, ZEVector3::UnitY) * 0.5f + 0.5f;
	//float HeightAboveEarth = zeScene->GetActiveCamera()->GetWorldPosition().y;
	//
	//ZEUInt Index1 = ((HeightAboveEarth - 20.0f) / (ATMOSPHERE_HEIGHT - 2.0f * 20.0f)) * 1023.0f + 0.5f;
	//ZEUInt Index2 = CosMoonZenith * 1023.0f + 0.5f;
	//
	//ZEVector3 LUTResult = MoonLight->GetColor() * Extinction[Index1][Index2].ToVector3();

	ZEVector3 StartPositionWorld = zeScene->GetActiveCamera()->GetWorldPosition();

	ZEBSphere Sphere(ZEVector3(0.0f, -EARTH_RADIUS, 0.0f), TOTAL_RADIUS);
	ZERay Ray(MoonDirection, StartPositionWorld);
	float Length;
	ZEBSphere::IntersectionTest(Sphere, Ray, Length);
	ZEVector3 EndPositionWorld = StartPositionWorld + MoonDirection * Length;

	ZEVector2 Density = IntegrateDensity(StartPositionWorld, EndPositionWorld);
	ZEVector3 Extinction = ZEATRayleighScatteringFactor * Density.x + ZEATMieScatteringFactor * Density.y;

	float R = ZEMath::Exp(-Extinction.x);
	float G = ZEMath::Exp(-Extinction.y);
	float B = ZEMath::Exp(-Extinction.z);

	ZEVector3 ComputeResult = SunLight->GetColor() * ZEVector3(R, G, B);

	return ComputeResult;
}

ZEVector3 ZEATAtmosphere::GetTerrestrialMoonAmbientColor()
{
	if (MoonLight == NULL)
		return ZEVector3::Zero;

	ZEVector3 MoonDirection = MoonLight->GetWorldRotation() * -ZEVector3::UnitZ;
	MoonDirection.NormalizeSelf();
	float CosMoonZenith = ZEVector3::DotProduct(MoonDirection, ZEVector3::UnitY) * 0.5f + 0.5f;

	float TexelWidth = 1.0f / 1024.0f;
	float TexelIndex1 = CosMoonZenith - TexelWidth;
	float TexelIndex2 = CosMoonZenith + TexelWidth;
	ZEUInt Index1 = TexelIndex1 * 1023.0f + 0.5f;
	ZEUInt Index2 = TexelIndex2 * 1023.0f + 0.5f;
	float Weight1 = CosMoonZenith - ((Index1 + 0.5f) / 1024.0f);
	float Weight2 = ((Index2 + 0.5f) / 1024.0f) - CosMoonZenith;

	ZEVector4 InterpolatedAmbient = (Weight1 * SkyAmbient[Index1] + Weight2 * SkyAmbient[Index2]) / (Weight1 + Weight2);

	return MoonLight->GetColor() * InterpolatedAmbient.ToVector3();
}

void ZEATAtmosphere::Tick(float ElapsedTime)
{
	ZEVector3 SunDirection = ZEATAstronomy::GetSunDirection(Observer);
	SunDirection.NormalizeSelf();

	float HeightFromEarthCenter = (Observer.Space.Elevation + EARTH_RADIUS) * 1e-6f;
	float SunDiskRadiusDegree = ZEATAstronomy::GetSunDiskRadius(Observer);
	float SunDiskRadiusFromObserver = ZEAngle::Tan(ZEAngle::ToRadian(SunDiskRadiusDegree)) * HeightFromEarthCenter;

	Sun->SetDirection(SunDirection);
	Sun->SetDiskRadius(SunDiskRadiusFromObserver);

	float CosSunAltitude = ZEVector3::DotProduct(ZEVector3::UnitY, SunDirection);
	bool SunVisible = CosSunAltitude >= 0.0f;

	if (SunLight != NULL)
	{
		ZEQuaternion SunRotation;
		ZEQuaternion::CreateFromDirection(SunRotation, -SunDirection);
		
		SunLight->SetWorldRotation(SunRotation);
		SunLight->SetVisible(SunVisible);
	}

	ZEVector3 MoonDirection = ZEATAstronomy::GetMoonDirection(Observer);
	MoonDirection.NormalizeSelf();

	float MoonDiskRadiusDegree = ZEATAstronomy::GetMoonDiskRadius(Observer);
	float MoonDiskRadiusFromObserver = ZEAngle::Tan(ZEAngle::ToRadian(MoonDiskRadiusDegree)) * HeightFromEarthCenter;

	Moon->SetDirection(MoonDirection);
	Moon->SetDiskRadius(MoonDiskRadiusFromObserver);

	float CosMoonAltitude = ZEVector3::DotProduct(ZEVector3::UnitY, MoonDirection);
	bool MoonVisible = CosMoonAltitude >= 0.0f;

	if (MoonLight != NULL)
	{
		ZEQuaternion MoonRotation;
		ZEQuaternion::CreateFromDirection(MoonRotation, -MoonDirection);

		MoonLight->SetWorldRotation(MoonRotation);
		MoonLight->SetVisible(MoonVisible);
		MoonLight->SetCastsShadow(!SunVisible);
	}

	if (SunVisible)
	{
		float InscatteringSun = 0.5f * (ZEVector3::DotProduct(SunDirection, ZEVector3::UnitY) * 0.5f + 0.5f);
		Cloud->SetLightDirection(-SunDirection);
		Cloud->SetLightColor(ZEVector3(10));
		Cloud->SetInscattering(InscatteringSun);
		Stars->SetBrightness(0.0f);
	}
	else
	{
		float InscatteringMoon = 0.1f * (ZEVector3::DotProduct(MoonDirection, ZEVector3::UnitY) * 0.5f + 0.5f);
		Cloud->SetLightDirection(-MoonDirection);
		Cloud->SetLightColor(ZEVector3(1.0f));
		Cloud->SetInscattering(InscatteringMoon);
		Stars->SetBrightness(0.1f);
	}

	double SunTransit, Sunrise, Sunset;
	ZEATAstronomy::GetSunTransitSunriseSunsetTime(SunTransit, Sunrise, Sunset, Observer);

	ZEUInt SunTransitHour = (ZEUInt)SunTransit % 24;
	ZEUInt SunTransitMinute = (ZEUInt)(SunTransit * 60.0) % 60;

	ZEUInt SunriseHour = (ZEUInt)Sunrise % 24;
	ZEUInt SunriseMinute = (ZEUInt)(Sunrise * 60.0) % 60;

	ZEUInt SunsetHour = (ZEUInt)Sunset % 24;
	ZEUInt SunsetMinute = (ZEUInt)(Sunset * 60.0) % 60;
}

bool ZEATAtmosphere::PreRender(const ZERNCullParameters* CullParameters)
{
	if (!ZEEntity::PreRender(CullParameters))
		return false;

	if (SunLight != NULL)
	{
		Constants.SunColor = SunLight->GetColor() * SunLight->GetIntensity() * 5.0f;
		Constants.SunDirection = SunLight->GetWorldRotation() * -ZEVector3::UnitZ;
		Constants.SunDirection.NormalizeSelf();

		DirtyFlags.RaiseFlags(ZEAT_ADF_CONSTANT_BUFFER);
	}

	if (MoonLight != NULL)
	{
		Constants.MoonColor = MoonLight->GetColor() * MoonLight->GetIntensity() * 5.0f;
		Constants.MoonDirection = MoonLight->GetWorldRotation() * -ZEVector3::UnitZ;
		Constants.MoonDirection.NormalizeSelf();

		DirtyFlags.RaiseFlags(ZEAT_ADF_CONSTANT_BUFFER);
	}

	CullParameters->Renderer->AddCommand(&Command);

	return true;
}

void ZEATAtmosphere::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (!Update())
		return;

	ZEGRContext* Context = Parameters->Context;

	static bool Precomputed = false;
	if(!Precomputed)
	{
		Precomputed = true;
		PrecomputeBuffers(Context);
	}

	if (SunLight != NULL)
		SunLight->SetTerrestrialColor(GetTerrestrialSunColor());

	if (MoonLight != NULL)
		MoonLight->SetTerrestrialColor(GetTerrestrialMoonColor());

	ZEVector3 SunAmbient = GetTerrestrialSunAmbientColor() * SunLight->GetIntensity();
	ZEVector3 MoonAmbient = GetTerrestrialMoonAmbientColor() * MoonLight->GetIntensity();

	zeScene->SetAmbientColor(SunAmbient + MoonAmbient);
	zeScene->SetAmbientFactor(2.0f);

	ZERNStage* Stage = Parameters->Stage;

	const ZEGRRenderTarget* RenderTarget = Stage->GetProvidedInput(ZERN_SO_COLOR);
	const ZEGRTexture2D* DepthTexture = Stage->GetOutput(ZERN_SO_DEPTH);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, SkyConstantBuffer);
	Context->SetRenderState(SkyRenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, DepthTexture->GetDepthStencilBuffer(true));
	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearClamp);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, DepthTexture);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, UseMultipleScattering ? PrecomputedMultipleScatteringBuffer : PrecomputedSingleScatteringBuffer);
	Context->SetTexture(ZEGR_ST_PIXEL, 6, PrecomputedExtinctionBuffer);
	Context->SetVertexBuffers(0, 0, NULL);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, RenderTarget->GetWidth(), RenderTarget->GetHeight()));

	Context->Draw(3, 0);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);
	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
}

ZEATAtmosphere* ZEATAtmosphere::CreateInstance()
{
	return new ZEATAtmosphere();
}
