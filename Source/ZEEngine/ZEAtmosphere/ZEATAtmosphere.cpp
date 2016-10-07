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
#include "ZEMath/ZEViewVolume.h"

#include "ZEATAstronomy.h"
#include "ZEATSun.h"
#include "ZEATMoon.h"
#include "ZEATFog.h"
#include "ZEATCloud.h"
#include "ZEATSkyBox.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNStageAtmosphere.h"
#include "ZERenderer/ZELightDirectional.h"

#define ZEAT_ADF_SHADERS			1
#define ZEAT_ADF_RENDER_STATE		2
#define ZEAT_ADF_CONSTANT_BUFFER	4

static ZEVector2 CalculateDensity(const ZEVector3& Position)
{
	float Height = (Position - ZEVector3(0.0f, -EARTH_RADIUS, 0.0f)).Length() - EARTH_RADIUS;

	float RayleighDensity = ZEMath::Exp(-Height / RAYLEIGH_MIE_HEIGHT.x);
	float MieDensity = ZEMath::Exp(-Height / RAYLEIGH_MIE_HEIGHT.y);

	return ZEVector2(RayleighDensity, MieDensity);
}

void ZEATAtmosphere::CreateRandomVectors()
{
	ZEArray<ZEVector4> SphereSamples;
	SphereSamples.SetCount(128);
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

	RandomVectorsTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, 128, 1, 1, ZEGR_TF_R32G32B32A32_FLOAT, ZEGR_RU_IMMUTABLE, ZEGR_RBF_SHADER_RESOURCE, 1, 1, SphereSamples.GetConstCArray());
}

bool ZEATAtmosphere::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZEAT_ADF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNFastLightScattering.hlsl";
	Options.Model = ZEGR_SM_5_0;
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString(ZEGRGraphicsModule::SAMPLE_COUNT)));

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

	ZEGRRenderState RenderState = ZERNStageAtmosphere::GetRenderState();
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);

	ZEGRBlendState BlendState;
	BlendState.SetBlendEnable(true);
	ZEGRBlendRenderTarget RenderTarge0_BlendAdditive = BlendState.GetRenderTarget(0);
	RenderTarge0_BlendAdditive.SetSource(ZEGRBlend::ZEGR_BO_ONE);
	RenderTarge0_BlendAdditive.SetDestination(ZEGRBlend::ZEGR_BO_ONE);
	RenderTarge0_BlendAdditive.SetOperation(ZEGRBlendOperation::ZEGR_BE_ADD);
	RenderTarge0_BlendAdditive.SetBlendEnable(true);
	BlendState.SetRenderTargetBlend(0, RenderTarge0_BlendAdditive);

	RenderState.SetBlendState(BlendState);

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

ZEVector3 ZEATAtmosphere::GetAmbientColorFromLUT(float CosLightZenith)
{
	float U = CosLightZenith * 0.5f + 0.5f;
	float Index = U * 1024.0f;
	ZEUInt Index0 = ZEMath::Floor(Index);
	ZEUInt Index1 = ZEMath::Ceil(Index);
	float Weight = Index - Index0;

	ZEVector4 Color0 = (Index0 >= 0 && Index0 < 1024) ? SkyAmbient[Index0] : ZEVector4::Zero;
	ZEVector4 Color1 = (Index1 >= 0 && Index1 < 1024) ? SkyAmbient[Index1] : ZEVector4::Zero;

	ZEVector4 InterpolatedAmbient = (1.0f - Weight) * Color0 + Weight * Color1;

	return InterpolatedAmbient.ToVector3();
}

void ZEATAtmosphere::PrecomputeBuffers(ZEGRContext* Context)
{
	ZEHolder<ZEGRBuffer> PrecomputeConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(PrecomputeConstants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

	ZEArray<const ZEGRRenderTarget*> MultipleScatteringRenderTargets;
	MultipleScatteringRenderTargets.Resize(1024);	//64 * 16

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, PrecomputeConstantBuffer);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, RandomVectorsTexture);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, 32.0f, 128.0f));

	for (ZEUInt I = 0; I < 1024; I++)
	{
		const ZEGRRenderTarget* RenderTarget = PrecomputedSingleScatteringBuffer->GetRenderTarget(I);
		MultipleScatteringRenderTargets[I] = PrecomputedMultipleScatteringBuffer->GetRenderTarget(I);
		Context->ClearRenderTarget(MultipleScatteringRenderTargets[I], ZEVector4::Zero);

		PrecomputeConstants.IndexZ = ((float)(I % 64) + 0.5f) / 64.0f;
		PrecomputeConstants.IndexW = ((float)(I / 64) + 0.5f) / 16.0f;
		PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

		Context->SetRenderState(PrecomputeSingleScatteringRenderStateData);
		Context->SetRenderTargets(1, &RenderTarget, NULL);

		Context->Draw(3, 0);
	}

	ZEHolder<ZEGRTexture> PrecomputedHighOrderScatteringBuffer = ZEGRTexture::CreateResource(ZEGR_TT_3D, 32, 128, 1, ZEGR_TF_R32G32B32A32_FLOAT, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 64 * 16);
	ZEHolder<ZEGRTexture> PrecomputedHighOrderInScatteringBuffer = ZEGRTexture::CreateResource(ZEGR_TT_3D, 32, 128, 1, ZEGR_TF_R32G32B32A32_FLOAT, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 64 * 16);

	for (ZEUInt S = 1; S < OrderCount; S++)
	{
		for(ZEUInt J = 0; J < 1024; J++)
		{
			const ZEGRRenderTarget* RenderTarget = PrecomputedHighOrderScatteringBuffer->GetRenderTarget(J);
			PrecomputeConstants.IndexZ = ((float)(J % 64) + 0.5f) / 64.0f;
			PrecomputeConstants.IndexW = ((float)(J / 64) + 0.5f) / 16.0f;
			PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

			Context->SetTexture(ZEGR_ST_PIXEL, 0, (S == 1) ? PrecomputedSingleScatteringBuffer : PrecomputedHighOrderInScatteringBuffer);

			Context->SetRenderTargets(1, &RenderTarget, NULL);
			Context->SetRenderState(PrecomputeHighOrderScatteringRenderStateData);

			Context->Draw(3, 0);
		}

		for (ZEUInt J = 0; J < 1024; J++)
		{
			const ZEGRRenderTarget* RenderTarget = PrecomputedHighOrderInScatteringBuffer->GetRenderTarget(J);
			PrecomputeConstants.IndexZ = ((float)(J % 64) + 0.5f) / 64.0f;
			PrecomputeConstants.IndexW = ((float)(J / 64) + 0.5f) / 16.0f;
			PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

			Context->SetTexture(ZEGR_ST_PIXEL, 1, PrecomputedHighOrderScatteringBuffer);
			Context->SetRenderTargets(1, &RenderTarget, NULL);
			Context->SetRenderState(PrecomputeHighOrderInScatteringRenderStateData);

			Context->Draw(3, 0);
		}

		for (ZEUInt J = 0; J < 1024; J++)
		{
			const ZEGRRenderTarget* RenderTarget = MultipleScatteringRenderTargets[J];
			PrecomputeConstants.IndexZ = (float)J;
			PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

			Context->SetTexture(ZEGR_ST_PIXEL, 0, PrecomputedHighOrderInScatteringBuffer);
			Context->SetRenderTargets(1, &RenderTarget, NULL);
			Context->SetRenderState(AddOrdersRenderStateData);

			Context->Draw(3, 0);
		}
	}

	for (ZEUInt K = 0; K < 1024; K++)
	{
		const ZEGRRenderTarget* RenderTarget = MultipleScatteringRenderTargets[K];
		PrecomputeConstants.IndexZ = (float)K;
		PrecomputeConstantBuffer->SetData(&PrecomputeConstants);

		Context->SetTexture(ZEGR_ST_PIXEL, 0, PrecomputedSingleScatteringBuffer);
		Context->SetRenderTargets(1, &RenderTarget, NULL);
		Context->SetRenderState(AddOrdersRenderStateData);

		Context->Draw(3, 0);
	}

	ZEHolder<ZEGRTexture> PrecomputedSkyAmbientBuffer = ZEGRTexture::CreateResource(ZEGR_TT_2D, 1024, 1, 1, ZEGR_TF_R32G32B32A32_FLOAT, ZEGR_RU_STATIC);
	const ZEGRRenderTarget* SkyAmbientRenderTarget = PrecomputedSkyAmbientBuffer->GetRenderTarget();

	Context->SetRenderState(PrecomputeSkyAmbientRenderStateData);
	Context->SetRenderTargets(1, &SkyAmbientRenderTarget, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, UseMultipleScattering ? PrecomputedMultipleScatteringBuffer : PrecomputedSingleScatteringBuffer);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, 1024.0f, 1.0f));

	Context->Draw(3, 0);

	ZEHolder<ZEGRTexture> AmbientBuffer = ZEGRTexture::CreateResource(ZEGR_TT_2D, 1024, 1, 1, ZEGR_TF_R32G32B32A32_FLOAT, ZEGR_RU_STAGING, ZEGR_RBF_NONE);

	Context->CopyResource(AmbientBuffer, PrecomputedSkyAmbientBuffer);

	float* Data;
	AmbientBuffer->Map(ZEGR_RMT_WRITE, reinterpret_cast<void**>(&Data));
		memcpy(&SkyAmbient, Data, sizeof(ZEVector4) * 1024);
	AmbientBuffer->Unmap();

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

void ZEATAtmosphere::ComputeTerrestrialColors(const ZEVector3& SunDirection, const ZEVector3& MoonDirection)
{
	static const float STEPCOUNT = 20;

	ZEVector3 StartPositionWorld = ZEVector3(0.0f, Observer.Space.Elevation, 0.0f);
	ZEBSphere Sphere(ZEVector3(0.0f, -EARTH_RADIUS, 0.0f), TOTAL_RADIUS);

	ZERay Ray(SunDirection, StartPositionWorld);
	float Length;
	ZEBSphere::IntersectionTest(Sphere, Ray, Length);
	ZEVector3 SunEndPositionWorld = StartPositionWorld + SunDirection * Length;

	Ray.v = MoonDirection;
	ZEBSphere::IntersectionTest(Sphere, Ray, Length);
	ZEVector3 MoonEndPositionWorld = StartPositionWorld + MoonDirection * Length;


	ZEVector2 SunPrevRayleighMieDensity = CalculateDensity(StartPositionWorld);
	ZEVector2 SunTotalRayleighMieDensity = ZEVector2::Zero;
	ZEVector2 MoonPrevRayleighMieDensity = SunPrevRayleighMieDensity;
	ZEVector2 MoonTotalRayleighMieDensity = ZEVector2::Zero;

	ZEVector3 SunStartToEnd = SunEndPositionWorld - StartPositionWorld;
	ZEVector3 SunDirectionStep = SunStartToEnd / STEPCOUNT;
	float SunStepLength = SunDirectionStep.Length();

	ZEVector3 MoonStartToEnd = MoonEndPositionWorld - StartPositionWorld;
	ZEVector3 MoonDirectionStep = MoonStartToEnd / STEPCOUNT;
	float MoonStepLength = MoonDirectionStep.Length();

	for(float S = 1.0f; S <= STEPCOUNT; S += 1.0f)
	{
		ZEVector3 SunPositionStep = StartPositionWorld + SunDirectionStep * S;
		ZEVector3 MoonPositionStep = StartPositionWorld + MoonDirectionStep * S;

		ZEVector2 SunCurRayleighMieDensity = CalculateDensity(SunPositionStep);
		ZEVector2 MoonCurRayleighMieDensity = CalculateDensity(MoonPositionStep);

		SunTotalRayleighMieDensity += (SunCurRayleighMieDensity + SunPrevRayleighMieDensity) * 0.5f * SunStepLength;
		MoonTotalRayleighMieDensity += (MoonCurRayleighMieDensity + MoonPrevRayleighMieDensity) * 0.5f * MoonStepLength;

		SunPrevRayleighMieDensity = SunCurRayleighMieDensity;
		MoonPrevRayleighMieDensity = MoonCurRayleighMieDensity;
	}

	ZEVector3 SunExtinction = ZEATRayleighScatteringFactor * SunTotalRayleighMieDensity.x + ZEATMieScatteringFactor * SunTotalRayleighMieDensity.y;
	ZEVector3 MoonExtinction = ZEATRayleighScatteringFactor * MoonTotalRayleighMieDensity.x + ZEATMieScatteringFactor * MoonTotalRayleighMieDensity.y;

	float SunR = ZEMath::Exp(-SunExtinction.x);
	float SunG = ZEMath::Exp(-SunExtinction.y);
	float SunB = ZEMath::Exp(-SunExtinction.z);

	float MoonR = ZEMath::Exp(-MoonExtinction.x);
	float MoonG = ZEMath::Exp(-MoonExtinction.y);
	float MoonB = ZEMath::Exp(-MoonExtinction.z);

	TerrestrialSunColor = ZEVector3(SunR, SunG, SunB);
	TerrestrialMoonColor = ZEVector3(MoonR, MoonG, MoonB);
}

void ZEATAtmosphere::ComputeAmbientColors(float CosSunZenith, float CosMoonZenith)
{
	TerrestrialSunAmbientColor = GetAmbientColorFromLUT(CosSunZenith);
	TerrestrialMoonAmbientColor = GetAmbientColorFromLUT(CosMoonZenith);
}

ZEEntityResult ZEATAtmosphere::LoadInternal()
{
	ZE_ENTITY_LOAD_CHAIN(ZEEntity);

	CreateRandomVectors();

	SkyConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

	ZEGRTextureOptions TextureOptions;
	TextureOptions.Type = ZEGR_TT_3D;
	PrecomputedMultipleScatteringBuffer = ZEGRTexture::LoadResource("#R:/ZEEngine/ZEATAtmosphere/MultipleScatteringLUT.dds", TextureOptions);

	Sun = ZEATSun::CreateInstance();
	AddComponent(Sun);

	Moon = ZEATMoon::CreateInstance();
	Moon->SetTextureFile("#R:/ZEEngine/ZEATAtmosphere/MoonPhases.dds", 53, 1);
	AddComponent(Moon);

	Fog = ZEATFog::CreateInstance();
	Fog->SetDensity(0.0f);
	Fog->SetStartDistance(0.0f);
	Fog->SetColor(ZEVector3(0.5f));
	AddComponent(Fog);

	Cloud = ZEATCloud::CreateInstance();
	Cloud->SetCloudTexture("#R:/ZEEngine/ZEATAtmosphere/Clouds.dds");
	AddComponent(Cloud);

	Stars = ZEATSkyBox::CreateInstance();
	Stars->SetName("StarMap");
	Stars->SetVisible(true);
	Stars->SetEnabled(true);
	Stars->SetTextureFile("#R:/ZEEngine/ZEATAtmosphere/StarMap.dds");
	Stars->SetColor(ZEVector3::One);
	Stars->SetBrightness(0.1f);
	AddComponent(Stars);

	if (!Update())
		return ZE_ER_FAILED_CLEANUP;

	return ZE_ER_DONE;
}

ZEEntityResult ZEATAtmosphere::UnloadInternal()
{
	DirtyFlags.RaiseAll();

	ScreenCoverVertexShader.Release();
	SkyPixelShader.Release();
	SkyRenderStateData.Release();
	SkyConstantBuffer.Release();
	PrecomputedSingleScatteringBuffer.Release();
	PrecomputedMultipleScatteringBuffer.Release();
	SunLight = NULL;
	MoonLight = NULL;

	ZE_ENTITY_UNLOAD_CHAIN(ZEEntity);
	return ZE_ER_DONE;
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
	Command.StageMask = ZERN_STAGE_ATMOSPHERE;
	Command.Priority = 3;

	TerrestrialSunColor = ZEVector3::Zero;
	TerrestrialSunAmbientColor = ZEVector3::Zero;
	TerrestrialMoonColor = ZEVector3::Zero;
	TerrestrialMoonAmbientColor = ZEVector3::Zero;

	Sun = NULL;
	Moon = NULL;
	Fog = NULL;
	Cloud = NULL;
	Stars = NULL;

	OrderCount = 4;
	UseMultipleScattering = true;

	SunLight = NULL;
	MoonLight = NULL;

	memset(&Constants, 0, sizeof(Constants));

	SetEntityFlags(ZE_EF_RENDERABLE | ZE_EF_TICKABLE);
}

ZEATAtmosphere::~ZEATAtmosphere()
{

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
	Fog->SetDensity(ZEMath::Saturate(FogDensity));
}

float ZEATAtmosphere::GetFogDensity() const
{
	return Fog->GetDensity();
}

ZEVector3 ZEATAtmosphere::GetTerrestrialSunColor()
{
	return TerrestrialSunColor;
}

ZEVector3 ZEATAtmosphere::GetTerrestrialSunAmbientColor()
{
	return TerrestrialSunAmbientColor;
}

ZEVector3 ZEATAtmosphere::GetTerrestrialMoonColor()
{
	return TerrestrialMoonColor;
}

ZEVector3 ZEATAtmosphere::GetTerrestrialMoonAmbientColor()
{
	return TerrestrialMoonAmbientColor;
}

void ZEATAtmosphere::Tick(float ElapsedTime)
{
	ZEVector3 SunDirection = ZEATAstronomy::GetSunDirection(Observer);
	SunDirection.NormalizeSelf();
	ZEVector3 MoonDirection = ZEATAstronomy::GetMoonDirection(Observer);
	MoonDirection.NormalizeSelf();

	ComputeTerrestrialColors(SunDirection, MoonDirection);

	float CosSunZenith = ZEVector3::DotProduct(ZEVector3::UnitY, SunDirection);
	float CosMoonZenith = ZEVector3::DotProduct(ZEVector3::UnitY, MoonDirection);

	ComputeAmbientColors(CosSunZenith, CosMoonZenith);

	bool SunVisible = CosSunZenith >= 0.0f;
	bool MoonVisible = CosMoonZenith >= 0.0f;

	if (SunLight != NULL)
	{
		ZEQuaternion SunRotation;
		ZEQuaternion::CreateFromDirection(SunRotation, -SunDirection);

		SunLight->SetWorldRotation(SunRotation);
		SunLight->SetVisible(SunVisible);
		SunLight->SetTerrestrialColor(TerrestrialSunColor);
		SunLight->SetTerrestrialIntensity(ZEMath::Exp(-Cloud->GetCloudDensity()) * SunLight->GetIntensity());

		TerrestrialSunAmbientColor *= SunLight->GetIntensity();
	}

	if (MoonLight != NULL)
	{
		ZEQuaternion MoonRotation;
		ZEQuaternion::CreateFromDirection(MoonRotation, -MoonDirection);

		MoonLight->SetWorldRotation(MoonRotation);
		MoonLight->SetVisible(MoonVisible);
		MoonLight->SetTerrestrialColor(TerrestrialMoonColor);
		MoonLight->SetTerrestrialIntensity(ZEMath::Exp(-Cloud->GetCloudDensity()) * MoonLight->GetIntensity());

		TerrestrialMoonAmbientColor *= MoonLight->GetIntensity();
	}

	ZEVector3 AmbientColor = ZEVector3(0.005f) + TerrestrialSunAmbientColor + TerrestrialMoonAmbientColor;
	//GetScene()->SetAmbientColor(AmbientColor);
	//GetScene()->SetAmbientFactor(1.0f);

	float HeightFromEarthCenter = (Observer.Space.Elevation + EARTH_RADIUS) * 1e-6f;

	float SunDiskRadiusDegree = ZEATAstronomy::GetSunDiskRadius(Observer);
	float SunDiskRadiusFromObserver = ZEAngle::Tan(ZEAngle::ToRadian(SunDiskRadiusDegree)) * HeightFromEarthCenter;
	Sun->SetColor(TerrestrialSunColor * 50.0f);
	Sun->SetDirection(SunDirection);
	Sun->SetDiskRadius(SunDiskRadiusFromObserver);
	Sun->SetVisible(SunVisible);

	float MoonDiskRadiusDegree = ZEATAstronomy::GetMoonDiskRadius(Observer);
	float MoonDiskRadiusFromObserver = ZEAngle::Tan(ZEAngle::ToRadian(MoonDiskRadiusDegree)) * HeightFromEarthCenter;
	Moon->SetColor(TerrestrialMoonColor * 1.0f);
	Moon->SetDirection(MoonDirection);
	Moon->SetDiskRadius(MoonDiskRadiusFromObserver);
	Moon->SetVisible(MoonVisible);

	if (SunVisible)
	{
		float SunInscattering = (CosSunZenith * 0.5f + 0.5f);

		Cloud->SetLightDirection(-SunDirection);
		Cloud->SetLightColor(TerrestrialSunColor * 10.0f);
		Cloud->SetInscattering(SunInscattering * SunLight->GetTerrestrialIntensity());
		Stars->SetBrightness(0.0f);
		Fog->SetColor(ZEVector3(SunInscattering));
	}
	else
	{
		float MoonInscattering = (CosMoonZenith * 0.5f + 0.5f);

		Cloud->SetLightDirection(-MoonDirection);
		Cloud->SetLightColor(TerrestrialMoonColor * 1.0f);
		Cloud->SetInscattering(MoonInscattering * MoonLight->GetTerrestrialIntensity());
		Stars->SetBrightness(0.5f);
		Fog->SetColor(ZEVector3(MoonInscattering));
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

bool ZEATAtmosphere::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!ZEEntity::PreRender(Parameters))
		return false;

	if (SunLight != NULL)
	{
		ZEVector3 SunDirection = SunLight->GetWorldRotation() * -ZEVector3::UnitZ;
		float CosSunZenith = ZEVector3::DotProduct(ZEVector3::UnitY, SunDirection);
		float SunIntensity = (CosSunZenith > -0.2f) ? 5.0f : 0.5f;

		Constants.SunColor = SunLight->GetColor() * SunLight->GetIntensity() * SunIntensity;
		Constants.SunDirection = SunLight->GetWorldRotation() * -ZEVector3::UnitZ;
		Constants.SunDirection.NormalizeSelf();

		DirtyFlags.RaiseFlags(ZEAT_ADF_CONSTANT_BUFFER);
	}
	else
	{
		Constants.SunColor = ZEVector3::One * 10.0f;
		ZEVector3::Normalize(Constants.SunDirection, ZEVector3::One);

		DirtyFlags.RaiseFlags(ZEAT_ADF_CONSTANT_BUFFER);
	}

	if (MoonLight != NULL)
	{
		Constants.MoonColor = MoonLight->GetColor() * MoonLight->GetIntensity();
		Constants.MoonDirection = MoonLight->GetWorldRotation() * -ZEVector3::UnitZ;
		Constants.MoonDirection.NormalizeSelf();

		DirtyFlags.RaiseFlags(ZEAT_ADF_CONSTANT_BUFFER);
	}
	else
	{
		Constants.MoonColor = ZEVector3::Zero;
		ZEVector3::Normalize(Constants.MoonDirection, ZEVector3::One);

		DirtyFlags.RaiseFlags(ZEAT_ADF_CONSTANT_BUFFER);
	}

	Parameters->Renderer->AddCommand(&Command);

	return true;
}

void ZEATAtmosphere::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (!Update())
		return;

	ZEGRContext* Context = Parameters->Context;
	const ZERNStage* Stage = Parameters->Stage;

	const ZEGRTexture* DepthTexture = static_cast<const ZEGRTexture*>(Stage->GetInput("DepthTexture"));

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, SkyConstantBuffer);
	Context->SetRenderState(SkyRenderStateData);

	const ZEGRTexture* Textures[2];
	Textures[0] = DepthTexture;
	Textures[1] = UseMultipleScattering ? PrecomputedMultipleScatteringBuffer : PrecomputedSingleScatteringBuffer;
	Context->SetTextures(ZEGR_ST_PIXEL, 4, 2, Textures);

	Context->Draw(3, 0);
}

ZEATAtmosphere* ZEATAtmosphere::CreateInstance()
{
	return new ZEATAtmosphere();
}
