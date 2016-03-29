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
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNCuller.h"
#include "ZERenderer/ZERNStageGBuffer.h"
#include "ZERenderer/ZELightDirectional.h"
#include "ZEATAstronomy.h"
#include "ZEATSun.h"
#include "ZEATMoon.h"

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
	RandomVectorsTexture = ZEGRTexture2D::CreateInstance(128, 1, 1, 1, 1, ZEGR_TF_R32G32B32A32_FLOAT);
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

	for (ZEUInt S = 1; S < OrderCount; S++)
	{
		for(ZEUInt J = 0; J < 1024; J++)
		{
			ZEHolder<const ZEGRRenderTarget> RenderTarget = PrecomputedHighOrderScatteringBuffer->GetRenderTarget(J);
			PrecomputeConstants.IndexZ = ((float)(J % 64) + 0.5f) / 64.0f;
			PrecomputeConstants.IndexW = ((float)(J / 64) + 0.5f) / 16.0f;
			PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

			Context->SetTexture(ZEGR_ST_PIXEL, 0, (S == 1) ? PrecomputedSingleScatteringBuffer : PrecomputedHighOrderInScatteringBuffer);
			Context->SetTexture(ZEGR_ST_PIXEL, 2, RandomVectorsTexture);

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

	ZEHolder<const ZEGRRenderTarget> SkyAmbientRenderTarget = PrecomputedSkyAmbientBuffer->GetRenderTarget();

	Context->SetRenderState(PrecomputeSkyAmbientRenderStateData);
	Context->SetRenderTargets(1, SkyAmbientRenderTarget.GetPointerToPointer(), NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, UseMultipleScattering ? PrecomputedMultipleScatteringBuffer : PrecomputedSingleScatteringBuffer);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, 1024.0f, 1.0f));

	Context->Draw(3, 0);

	Context->SetSampler(ZEGR_ST_PIXEL, 0, NULL);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 1, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, NULL);
	Context->SetRenderTargets(0, NULL, NULL);

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

	RandomVectorsTexture.Release();
}

bool ZEATAtmosphere::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	CreateRandomVectors();

	SkyConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));
	PrecomputeConstantBuffer = ZEGRConstantBuffer::Create(sizeof(PrecomputeConstants));

	PrecomputedDensityBuffer = ZEGRTexture2D::CreateInstance(1024, 1024, 1, 1, 1, ZEGR_TF_R32G32_FLOAT, true);
	PrecomputedSingleScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R16G16B16A16_FLOAT, true, false);
	PrecomputedHighOrderScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R32G32B32A32_FLOAT, true, false);
	PrecomputedHighOrderInScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R32G32B32A32_FLOAT, true, false);
	PrecomputedMultipleScatteringBuffer = ZEGRTexture3D::Create(32, 128, 64 * 16, 1, ZEGR_TF_R16G16B16A16_FLOAT, true, false);
	PrecomputedSkyAmbientBuffer = ZEGRTexture2D::CreateInstance(1024, 1, 1, 1, 1, ZEGR_TF_R16G16B16A16_FLOAT, true);

	SamplerLinearClamp = ZEGRSampler::GetDefaultSampler();

	Sun = new ZEATSun();
	zeScene->AddEntity(Sun);

	Moon = ZEATMoon::CreateInstance();
	zeScene->AddEntity(Moon);
	Moon->SetTextureFile("#R:/ZEEngine/ZEAtmosphere/Textures/MoonFrame.png", 53, 1);

	return true;
}

bool ZEATAtmosphere::DeinitializeSelf()
{
	DirtyFlags.RaiseAll();

	ScreenCoverVertexShader.Release();

	PrecomputedDensityBuffer.Release();
	PrecomputedSingleScatteringBuffer.Release();
	PrecomputedMultipleScatteringBuffer.Release();
	PrecomputedSkyAmbientBuffer.Release();

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

void ZEATAtmosphere::Tick(float ElapsedTime)
{
	ZEVector3 SunDirection = ZEATAstronomy::GetSunDirection(Observer);

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
	if (SunLight != NULL)
	{
		SunLight->SetAmbientBuffer(PrecomputedSkyAmbientBuffer);
		SunLight->SetDensityBuffer(PrecomputedDensityBuffer);

		Constants.SunColor = SunLight->GetColor() * SunLight->GetIntensity() * 5.0f;
		Constants.SunDirection = SunLight->GetWorldRotation() * -ZEVector3::UnitZ;
		Constants.SunDirection.NormalizeSelf();

		DirtyFlags.RaiseFlags(ZEAT_ADF_CONSTANT_BUFFER);
	}

	if (MoonLight != NULL)
	{
		MoonLight->SetAmbientBuffer(PrecomputedSkyAmbientBuffer);
		MoonLight->SetDensityBuffer(PrecomputedDensityBuffer);

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

		Sun->SetDensityBuffer(PrecomputedDensityBuffer);
		Moon->SetDensityBuffer(PrecomputedDensityBuffer);
	}

	ZERNStage* Stage = Parameters->Stage;

	const ZEGRRenderTarget* RenderTarget = Stage->GetProvidedInput(ZERN_SO_COLOR);

	const ZEGRTexture2D* DepthTexture = Stage->GetOutput(ZERN_SO_DEPTH);
	const ZEGRTexture2D* ColorTexture = Stage->GetPrevStage()->GetOutput(ZERN_SO_COLOR);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, SkyConstantBuffer);
	Context->SetRenderState(SkyRenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, DepthTexture->GetDepthStencilBuffer(true));
	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearClamp);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, DepthTexture);
	Context->SetTexture(ZEGR_ST_PIXEL, 1, ColorTexture);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, UseMultipleScattering ? PrecomputedMultipleScatteringBuffer : PrecomputedSingleScatteringBuffer);
	Context->SetVertexBuffers(0, 0, NULL);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, RenderTarget->GetWidth(), RenderTarget->GetHeight()));

	Context->Draw(3, 0);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);
	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 1, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
}

ZEATAtmosphere* ZEATAtmosphere::CreateInstance()
{
	return new ZEATAtmosphere();
}
