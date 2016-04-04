//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageLighting.cpp
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

// Copyright 2010 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

#include "ZERNStageLighting.h"

#include "ZEError.h"
#include "ZERandom.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEMath.h"
#include "ZERNStageID.h"
#include "ZERNRenderer.h"
#include "ZERNStageGBuffer.h"
#include "ZECanvas.h"
#include "ZELightPoint.h"
#include "ZELightDirectional.h"
#include "ZELightProjective.h"
#include "ZELightOmniProjective.h"
#include "ZEGame/ZEScene.h"
#include "ZEGame/ZEEntity.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRTextureCube.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRStructuredBuffer.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"

#define ZERN_SLDF_LIGHT_BUFFER	1
#define ZERN_SLDF_TILE_BUFFER	2
#define ZERN_SLDF_SHADERS		4
#define ZERN_SLDF_RENDER_STATE	8

void ZERNStageLighting::CreateRandomVectors()
{
	const ZEUInt Size = 128 * 128 * 2;
	ZEUInt8 Vectors[Size];
	for(ZEUInt I = 0; I < Size; I += 2)
	{
		Vectors[I]		= (ZEUInt8)(ZERandom::GetFloatPositive() * 255.0f + 0.5f);
		Vectors[I + 1]	= (ZEUInt8)(ZERandom::GetFloatPositive() * 255.0f + 0.5f);
	}

	RandomVectorsTexture.Release();
	RandomVectorsTexture = ZEGRTexture2D::CreateInstance(128, 128, 1, ZEGR_TF_R8G8_UNORM, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE);
	RandomVectorsTexture->UpdateSubResource(0, 0, &Vectors[0], 128 * 2);
}

void ZERNStageLighting::CreateLightGeometries()
{
	/*
	Primitive	Start		Start(Byte)	Vertex Count	Prim. Count		Size (Bytes)
	Quad		0			0			6				2				288
	Sphere12	6			288			936				312				44928
	Sphere24	942			45216		3600			1200			172800
	Cone12		4542		218016		18				6				864
	End			4650				
	*/

	ZECanvas Canvas;

	// Directional
	Canvas.AddQuad(
		ZEVector3(-1.0f, 1.0f, 0.0f),
		ZEVector3(-1.0f, -1.0f, 0.0f),
		ZEVector3(1.0f, -1.0f, 0.0f),
		ZEVector3(1.0f, 1.0f, 0.0f));

	// Point/OmniProjective
	Canvas.AddSphere(1.0f, 12, 12);
	Canvas.AddSphere(1.0f, 24, 24);

	// Projective
	Canvas.SetRotation(ZEQuaternion(-ZE_PI_2, ZEVector3::UnitX));
	Canvas.SetTranslation(ZEVector3::UnitZ);
	Canvas.AddPyramid(1.0f, 1.0f, 1.0f);

	DeferredLightVertexBuffer = Canvas.CreateVertexBuffer();
}

void ZERNStageLighting::CreateSamplers()
{
	ZEGRSamplerDescription SamplerLinearBorderDescription;
	SamplerLinearBorderDescription.AddressU = ZEGR_TAM_BORDER;
	SamplerLinearBorderDescription.AddressV = ZEGR_TAM_BORDER;
	SamplerLinearBorderDescription.AddressW = ZEGR_TAM_BORDER;
	SamplerLinearBorderDescription.BorderColor = ZEVector4::Zero;
	SamplerLinearBorder = ZEGRSampler::GetSampler(SamplerLinearBorderDescription);

	ZEGRSamplerDescription SamplerComparisonLinearPointClampDescription;
	SamplerComparisonLinearPointClampDescription.MinFilter = ZEGR_TFM_LINEAR;
	SamplerComparisonLinearPointClampDescription.MagFilter = ZEGR_TFM_LINEAR;
	SamplerComparisonLinearPointClampDescription.MipFilter = ZEGR_TFM_POINT;
	SamplerComparisonLinearPointClampDescription.AddressU = ZEGR_TAM_CLAMP;
	SamplerComparisonLinearPointClampDescription.AddressV = ZEGR_TAM_CLAMP;
	SamplerComparisonLinearPointClampDescription.ComparisonFunction = ZEGR_CF_GREATER;
	SamplerComparisonLinearPointClamp = ZEGRSampler::GetSampler(SamplerComparisonLinearPointClampDescription);

	ZEGRSamplerDescription SamplerPointWrapDescription;
	SamplerPointWrapDescription.MinFilter = ZEGR_TFM_POINT;
	SamplerPointWrapDescription.MagFilter = ZEGR_TFM_POINT;
	SamplerPointWrapDescription.MipFilter = ZEGR_TFM_POINT;
	SamplerPointWrapDescription.AddressU = ZEGR_TAM_WRAP;
	SamplerPointWrapDescription.AddressV = ZEGR_TAM_WRAP;
	SamplerPointWrapDescription.AddressW = ZEGR_TAM_WRAP;
	SamplerPointWrap = ZEGRSampler::GetSampler(SamplerPointWrapDescription);

	ZEGRSamplerDescription SamplerPointBorderDescription;
	SamplerPointBorderDescription.MinFilter = ZEGR_TFM_POINT;
	SamplerPointBorderDescription.MagFilter = ZEGR_TFM_POINT;
	SamplerPointBorderDescription.MipFilter = ZEGR_TFM_POINT;
	SamplerPointBorderDescription.AddressU = ZEGR_TAM_BORDER;
	SamplerPointBorderDescription.AddressV = ZEGR_TAM_BORDER;
	SamplerPointBorderDescription.AddressW = ZEGR_TAM_BORDER;
	SamplerPointBorderDescription.BorderColor = ZEVector4::One;
	SamplerPointBorder = ZEGRSampler::GetSampler(SamplerPointBorderDescription);

	ZEGRSamplerDescription SamplerPointClampDescription;
	SamplerPointClampDescription.MinFilter = ZEGR_TFM_POINT;
	SamplerPointClampDescription.MagFilter = ZEGR_TFM_POINT;
	SamplerPointClampDescription.MipFilter = ZEGR_TFM_POINT;
	SamplerPointClamp = ZEGRSampler::GetSampler(SamplerPointClampDescription);
}

bool ZERNStageLighting::UpdateRenderState()
{
	if(!DirtyFlags.GetFlags(ZERN_SLDF_RENDER_STATE))
		return true;

	ZEGRRenderState GraphicsRenderState;
	GraphicsRenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);

	ZEGRBlendState BlendStateAdditive;
	BlendStateAdditive.SetBlendEnable(true);
	ZEGRBlendRenderTarget BlendRenderTargetAdditive = BlendStateAdditive.GetRenderTarget(0);
	BlendRenderTargetAdditive.SetSource(ZEGRBlend::ZEGR_BO_ONE);
	BlendRenderTargetAdditive.SetDestination(ZEGRBlend::ZEGR_BO_ONE);
	BlendRenderTargetAdditive.SetOperation(ZEGRBlendOperation::ZEGR_BE_ADD);
	BlendRenderTargetAdditive.SetBlendEnable(true);
	BlendStateAdditive.SetRenderTargetBlend(0, BlendRenderTargetAdditive);
	
	//GraphicsRenderState.SetBlendState(BlendStateAdditive);

	ZEGRDepthStencilState DepthStencilStateTestNoWriteLess;
	DepthStencilStateTestNoWriteLess.SetDepthTestEnable(true);
	DepthStencilStateTestNoWriteLess.SetDepthWriteEnable(false);
	DepthStencilStateTestNoWriteLess.SetDepthFunction(ZEGR_CF_LESS);

	GraphicsRenderState.SetDepthStencilState(DepthStencilStateTestNoWriteLess);

	ZEGRRasterizerState RasterizerStateCullFront;
	RasterizerStateCullFront.SetCullMode(ZEGR_CMD_FRONT);

	GraphicsRenderState.SetRasterizerState(RasterizerStateCullFront);
	GraphicsRenderState.SetVertexLayout(*ZECanvasVertex::GetVertexLayout());

	GraphicsRenderState.SetShader(ZEGR_ST_VERTEX, DeferredVertexShader);
	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DeferredPixelShader);

	DeferredRenderState = GraphicsRenderState.Compile();
	zeCheckError(DeferredRenderState == NULL, false, "Cannot set render state.");

	ZEGRComputeRenderState ComputeRenderState;

	ComputeRenderState.SetComputeShader(TiledDeferredComputeShader);
	TiledDeferredComputeRenderState = ComputeRenderState.Compile();
	zeCheckError(TiledDeferredComputeRenderState == NULL, false, "Cannot set render state.");

	GraphicsRenderState = ZEGRRenderState();

	ZEGRDepthStencilState DepthStencilStateNoTestWrite;
	DepthStencilStateNoTestWrite.SetDepthTestEnable(false);
	DepthStencilStateNoTestWrite.SetDepthWriteEnable(false);

	GraphicsRenderState.SetDepthStencilState(DepthStencilStateNoTestWrite);

	GraphicsRenderState.SetBlendState(BlendStateAdditive);
	GraphicsRenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	GraphicsRenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);
	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, AccumulatePixelShader);

	AccumulateRenderState = GraphicsRenderState.Compile();
	zeCheckError(AccumulateRenderState == NULL, false, "Cannot set render state.");

	DirtyFlags.UnraiseFlags(ZERN_SLDF_RENDER_STATE);

	return true;
}

bool ZERNStageLighting::UpdateShaders()
{
	if(!DirtyFlags.GetFlags(ZERN_SLDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.Model = ZEGR_SM_5_0;

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNDeferredShading.hlsl";

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_Position";
	ScreenCoverVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(ScreenCoverVertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNDeferredShading_VertexShader_LightingStage";
	DeferredVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(DeferredVertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNDeferredShading_PixelShader_LightingStage";
	DeferredPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(DeferredPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNDeferredShading_Accumulate_PixelShader_Main";
	AccumulatePixelShader = ZEGRShader::Compile(Options);
	zeCheckError(AccumulatePixelShader == NULL, false, "Cannot set pixel shader.");

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNTiledDeferredShadingCompute.hlsl";

	Options.Type = ZEGR_ST_COMPUTE;
	Options.EntryPoint = "ZERNTiledDeferredShadingCompute_ComputeShader_Main";
	TiledDeferredComputeShader = ZEGRShader::Compile(Options);
	zeCheckError(TiledDeferredComputeShader == NULL, false, "Cannot set compute shader.");

	DirtyFlags.UnraiseFlags(ZERN_SLDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_SLDF_RENDER_STATE);

	return true;
}

bool ZERNStageLighting::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderState())
		return false;

	return true;
}

bool ZERNStageLighting::SetupDeferred(ZEGRContext* Context)
{
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, 8, DeferredLightConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, DeferredLightConstantBuffer);

	Context->SetRenderState(DeferredRenderState);
	Context->SetVertexBuffers(0, 1, DeferredLightVertexBuffer.GetPointerToPointer());

	ZEUInt Index = 0;
	DeferredLightConstantsStruct* LightConstants;
	DeferredLightConstantBuffer->Lock((void**)&LightConstants);
	memset(LightConstants, 0, sizeof(DeferredLightConstantsStruct));
	ze_for_each(Link, DeferredLightList)
	{
		ZELight* Light = Link.GetPointer();
		if(Light->GetLightType() == ZE_LT_DIRECTIONAL)
		{
			ZELightDirectional* DirectionalLight = static_cast<ZELightDirectional*>(Light);

			LightStruct& Light = LightConstants->Light[Index++];
			Light.Color = DirectionalLight->GetColor() * DirectionalLight->GetIntensity();
			Light.Type = DirectionalLight->GetLightType();
			ZEVector3 Direction = DirectionalLight->GetWorldRotation() * -ZEVector3::UnitZ;
			ZEMatrix4x4::Transform3x3(Light.DirectionView, GetRenderer()->GetView().ViewTransform, Direction);
			Light.DirectionView.NormalizeSelf();
			ZERNStage* StageShadowing = GetRenderer()->GetStage(ZERN_STAGE_SHADOWING);
			bool ShadowingEnabled = (StageShadowing != NULL && StageShadowing->GetEnabled());
			Light.CastShadow = static_cast<ZEBool32>(DirectionalLight->GetCastsShadow() & ShadowingEnabled);
			Light.CosZenith = ZEVector3::DotProduct(Direction, ZEVector3::UnitY) * 0.5f + 0.5f;

			if (DirectionalLight->GetUseSunLight() || DirectionalLight->GetUseMoonLight())
				Light.Color = DirectionalLight->GetTerrestrialColor() * DirectionalLight->GetIntensity();

			if (Light.CastShadow)
			{
				LightConstants->ShowCascades = static_cast<ZEBool32>(ShowCascades);
				LightConstants->SampleCount = ZELight::ConvertShadowSampleCount(DirectionalLight->GetShadowSampleCount());
				LightConstants->SampleLength = DirectionalLight->GetShadowSampleLength();
				LightConstants->ShadowDepthBias = DirectionalLight->GetShadowDepthBias();

				DirectionalLight->BindCascades(GetRenderer(), Context);

				Context->SetSampler(ZEGR_ST_PIXEL, 2, SamplerComparisonLinearPointClamp);
				Context->SetSampler(ZEGR_ST_PIXEL, 3, SamplerPointWrap);
				
				Context->SetTexture(ZEGR_ST_PIXEL, 5, DirectionalLight->GetShadowMap());
				Context->SetTexture(ZEGR_ST_PIXEL, 8, RandomVectorsTexture);
			}
		}
	}

	DeferredLightConstantBuffer->Unlock();

	DeferredLightList.Clean();

	Context->SetSampler(ZEGR_ST_PIXEL, 1, SamplerLinearBorder);

	Context->Draw(6, 0);

	Context->SetSampler(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 1, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 2, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 3, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 8, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 10, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 11, NULL);

	Context->SetVertexBuffers(0, 0, NULL);
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, 8, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);

	return true;
}

bool ZERNStageLighting::SetupComputeTiledDeferred(ZEGRContext* Context)
{
	ZEUInt CurrentWidth = OutputTexture->GetWidth();
	ZEUInt CurrentHeight = OutputTexture->GetHeight();

	if(TiledDeferredComputeOutputTexture.IsNull() ||
		TiledDeferredComputeOutputTexture->GetWidth() != CurrentWidth || 
		TiledDeferredComputeOutputTexture->GetHeight() != CurrentHeight)
	{
		TiledDeferredComputeOutputTexture.Release();
		TiledDeferredComputeOutputTexture = ZEGRTexture2D::CreateInstance(CurrentWidth, CurrentHeight, 1, ZEGR_TF_R11G11B10_FLOAT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_UNORDERED_ACCESS);
	}

	const ZERNView& View = GetRenderer()->GetView();
	ZEUInt LightIndex = 0;
	ZELink<ZELight>* Link = TiledDeferredLightList.GetFirst();
	LightStruct* Lights;
	TiledDeferredLightStructuredBuffer->Lock((void**)&Lights);
	while(Link != NULL)
	{
		ZELight* SrcLight = Link->GetItem();
		LightStruct& DestLight = Lights[LightIndex];

		if (SrcLight->GetLightType() == ZE_LT_DIRECTIONAL)
		{
			ZELightDirectional* DirectionalLight = static_cast<ZELightDirectional*>(SrcLight);
			if (DirectionalLight->GetCastsShadow())
			{
				DirectionalLight->BindCascades(GetRenderer(), Context);

				Context->SetSampler(ZEGR_ST_COMPUTE, 2, SamplerComparisonLinearPointClamp);
				Context->SetSampler(ZEGR_ST_COMPUTE, 3, SamplerPointWrap);
				Context->SetTexture(ZEGR_ST_COMPUTE, 5, DirectionalLight->GetShadowMap());
				Context->SetTexture(ZEGR_ST_COMPUTE, 8, RandomVectorsTexture);
			}
		}

		ZEMatrix4x4::Transform(DestLight.PositionView, View.ViewTransform, SrcLight->GetWorldPosition());
		DestLight.Range = SrcLight->GetRange();
		DestLight.Color = SrcLight->GetColor() * SrcLight->GetIntensity();
		DestLight.Attenuation = SrcLight->GetAttenuation();
		DestLight.Type = SrcLight->GetLightType();
		ZEMatrix4x4::Transform3x3(DestLight.DirectionView, GetRenderer()->GetView().ViewTransform, -(SrcLight->GetWorldRotation() * ZEVector3::UnitZ));
		DestLight.CastShadow = (ZEBool32)SrcLight->GetCastsShadow();

		LightIndex++;
		Link = Link->GetNext();
	}
	TiledDeferredLightStructuredBuffer->Unlock();

	TiledDeferredLightConstants.LightCount = TiledDeferredLightList.GetCount();
	TiledDeferredLightList.Clean();
	TiledDeferredLightConstantBuffer->SetData(&TiledDeferredLightConstants);

	Context->SetComputeRenderState(TiledDeferredComputeRenderState);

	Context->SetConstantBuffer(ZEGR_ST_COMPUTE, 8, TiledDeferredLightConstantBuffer);
	Context->SetStructuredBuffer(ZEGR_ST_COMPUTE, 12, TiledDeferredLightStructuredBuffer);
	Context->SetRWTexture(ZEGR_ST_COMPUTE, 0, TiledDeferredComputeOutputTexture);

	ZEUInt TileCountX = (CurrentWidth + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;
	ZEUInt TileCountY = (CurrentHeight + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;

	Context->Dispatch(TileCountX, TileCountY, 1);

	Context->SetSampler(ZEGR_ST_COMPUTE, 0, NULL);
	Context->SetSampler(ZEGR_ST_COMPUTE, 2, NULL);
	Context->SetSampler(ZEGR_ST_COMPUTE, 3, NULL);
	Context->SetTexture(ZEGR_ST_COMPUTE, 10, NULL);
	Context->SetTexture(ZEGR_ST_COMPUTE, 11, NULL);
	Context->SetTexture(ZEGR_ST_COMPUTE, 5, NULL);
	Context->SetTexture(ZEGR_ST_COMPUTE, 8, NULL);

	Context->SetConstantBuffer(ZEGR_ST_COMPUTE, 8, NULL);
	Context->SetStructuredBuffer(ZEGR_ST_COMPUTE, 12, NULL);
	Context->SetRWTexture(ZEGR_ST_COMPUTE, 0, NULL);

	return true;
}

void ZERNStageLighting::DrawDirectionalLight(ZELightDirectional* DirectionalLight, ZEGRContext* Context)
{
	DeferredLightConstantsStruct* LightConstants;
	DeferredLightConstantBuffer->Lock((void**)&LightConstants);
		LightStruct& Light = LightConstants->Light[0];
		Light.Color = DirectionalLight->GetColor() * DirectionalLight->GetIntensity();
		Light.Type = DirectionalLight->GetLightType();
		ZEVector3 Direction = DirectionalLight->GetWorldRotation() * ZEVector3::UnitZ;
 		ZEMatrix4x4::Transform3x3(Light.DirectionView, GetRenderer()->GetView().ViewTransform, Direction);
		Light.DirectionView.NormalizeSelf();
		ZERNStage* StageShadowing = GetRenderer()->GetStage(ZERN_STAGE_SHADOWING);
		bool ShadowingEnabled = (StageShadowing != NULL && StageShadowing->GetEnabled());
		Light.CastShadow = static_cast<ZEBool32>(DirectionalLight->GetCastsShadow() & ShadowingEnabled);

		LightConstants->ShowCascades = static_cast<ZEBool32>(ShowCascades);
		LightConstants->SampleCount = ZELight::ConvertShadowSampleCount(DirectionalLight->GetShadowSampleCount());
		LightConstants->SampleLength = DirectionalLight->GetShadowSampleLength();
		LightConstants->ShadowDepthBias = DirectionalLight->GetShadowDepthBias();
	DeferredLightConstantBuffer->Unlock();

	DirectionalLight->BindCascades(GetRenderer(), Context); //TODO: Another way to do this?

	Context->SetSampler(ZEGR_ST_PIXEL, 1, SamplerLinearBorder);
	Context->SetSampler(ZEGR_ST_PIXEL, 2, SamplerComparisonLinearPointClamp);
	Context->SetSampler(ZEGR_ST_PIXEL, 3, SamplerPointWrap);
	Context->SetSampler(ZEGR_ST_PIXEL, 4, SamplerPointBorder);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, DirectionalLight->GetShadowMap());
	Context->SetTexture(ZEGR_ST_PIXEL, 8, RandomVectorsTexture);

	Context->Draw(6, 0);

	Context->SetSampler(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 1, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 2, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 3, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 4, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 8, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 10, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 11, NULL);
}

void ZERNStageLighting::DrawProjectiveLight(ZELightProjective* ProjectiveLight, ZEGRContext* Context)
{
	DeferredLightConstantsStruct* LightConstants;
	DeferredLightConstantBuffer->Lock((void**)&LightConstants);
		LightStruct& Light = LightConstants->Light[0];
		ZEMatrix4x4::Transform(Light.PositionView, GetRenderer()->GetView().ViewTransform, ProjectiveLight->GetWorldPosition());
		Light.Range = ProjectiveLight->GetRange();
		Light.Color = ProjectiveLight->GetColor() * ProjectiveLight->GetIntensity();
		Light.Attenuation = ProjectiveLight->GetAttenuation();
		Light.Type = ProjectiveLight->GetLightType();
		Light.DirectionView = ZEVector3::Zero;
		ZERNStage* StageShadowing = GetRenderer()->GetStage(ZERN_STAGE_SHADOWING);
		bool ShadowingEnabled = (StageShadowing != NULL && StageShadowing->GetEnabled());
		Light.CastShadow = static_cast<ZEBool32>(ProjectiveLight->GetCastsShadow() & ShadowingEnabled);

		LightConstants->SampleCount = ZELight::ConvertShadowSampleCount(ProjectiveLight->GetShadowSampleCount());
		LightConstants->SampleLength = ProjectiveLight->GetShadowSampleLength();
		LightConstants->ShadowDepthBias = ProjectiveLight->GetShadowDepthBias();

		LightConstants->ProjectionMatrix = ProjectiveLight->GetProjectionTransform() * ProjectiveLight->GetViewTransform() * GetRenderer()->GetView().InvViewTransform;

		float TanFovRange = ZEAngle::Tan(ProjectiveLight->GetFOV() * 0.5f) * ProjectiveLight->GetRange();
		ZEMatrix4x4::CreateOrientation(LightConstants->WorldMatrix, ProjectiveLight->GetWorldPosition(), ProjectiveLight->GetWorldRotation(),
			ZEVector3(TanFovRange * ProjectiveLight->GetAspectRatio() * 2.0f, TanFovRange * 2.0f, ProjectiveLight->GetRange()));
	DeferredLightConstantBuffer->Unlock();

	Context->SetSampler(ZEGR_ST_PIXEL, 1, SamplerLinearBorder);
	Context->SetSampler(ZEGR_ST_PIXEL, 2, SamplerComparisonLinearPointClamp);
	Context->SetSampler(ZEGR_ST_PIXEL, 3, SamplerPointWrap);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, ProjectiveLight->GetShadowMap());
	Context->SetTexture(ZEGR_ST_PIXEL, 6, ProjectiveLight->GetProjectionTexture());
	Context->SetTexture(ZEGR_ST_PIXEL, 8, RandomVectorsTexture);

	Context->Draw(18, 4542);

	Context->SetSampler(ZEGR_ST_PIXEL, 1, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 2, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 3, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 6, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 8, NULL);
}

bool ZERNStageLighting::InitializeSelf()
{
	if (!ZERNStage::InitializeSelf())
		return false;

	CreateRandomVectors();
	CreateLightGeometries();
	CreateSamplers();

	DeferredLightConstantBuffer = ZEGRConstantBuffer::Create(sizeof(DeferredLightConstantsStruct));
	TiledDeferredLightConstantBuffer = ZEGRConstantBuffer::Create(sizeof(TiledDeferredLightConstantsStruct));
	TiledDeferredLightStructuredBuffer = ZEGRStructuredBuffer::Create(MAX_LIGHT, sizeof(LightStruct));

	return true;
}

void ZERNStageLighting::DeinitializeSelf()
{
	DirtyFlags.RaiseAll();

	OutputTexture.Release();

	ScreenCoverVertexShader.Release();
	DeferredVertexShader.Release();
	DeferredPixelShader.Release();
	TiledDeferredComputeShader.Release();
	AccumulatePixelShader.Release();

	DeferredRenderState.Release();
	TiledDeferredComputeRenderState.Release();
	AccumulateRenderState.Release();

	DeferredLightConstantBuffer.Release();
	DeferredLightVertexBuffer.Release();
	TiledDeferredLightStructuredBuffer.Release();
	TiledDeferredLightConstantBuffer.Release();

	TiledDeferredComputeOutputTexture.Release();

	RandomVectorsTexture.Release();

	SamplerLinearBorder.Release();
	SamplerComparisonLinearPointClamp.Release();
	SamplerPointWrap.Release();
	SamplerPointBorder.Release();

	DeferredLightList.Clean();
	TiledDeferredLightList.Clean();

	ZERNStage::DeinitializeSelf();
}

ZEInt ZERNStageLighting::GetId() const
{
	return ZERN_STAGE_LIGHTING;
}

const ZEString& ZERNStageLighting::GetName() const
{
	static const ZEString Name = "Lighting";
	return Name;
}

void ZERNStageLighting::SetShowCascades(bool ShowCascades)
{
	this->ShowCascades = ShowCascades;
}

bool ZERNStageLighting::GetShowCascades() const
{
	return ShowCascades;
}

const ZEGRTexture2D* ZERNStageLighting::GetOutput(ZERNStageBuffer Output) const
{
	if (GetEnabled() && Output == ZERN_SO_COLOR)
		return OutputTexture;
	
	return ZERNStage::GetOutput(Output);
}

bool ZERNStageLighting::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	ze_for_each(Command, GetCommands())
	{
		ZELight* Light = static_cast<ZELight*>(Command->Entity);

		switch (Light->GetLightType())
		{
			case ZE_LT_DIRECTIONAL:
			case ZE_LT_PROJECTIVE:
				DeferredLightList.AddEnd(new ZELink<ZELight>(Light));
				break;
			case ZE_LT_POINT:
			case ZE_LT_OMNIPROJECTIVE:
				TiledDeferredLightList.AddEnd(new ZELink<ZELight>(Light));
				break;
		}
	}

	const ZEGRRenderTarget* RenderTarget = GetRenderer()->GetOutputRenderTarget();
	if (RenderTarget == NULL)
		return false;

	ZEUInt Width = RenderTarget->GetWidth();
	ZEUInt Height = RenderTarget->GetHeight();

	if (OutputTexture == NULL || 
		OutputTexture->GetWidth() != Width || OutputTexture->GetHeight() != Height)
	{
		OutputTexture.Release();
		OutputTexture = ZEGRTexture2D::CreateInstance(Width, Height, 1, ZEGR_TF_R11G11B10_FLOAT);
	}

	Viewport.SetWidth((float)Width);
	Viewport.SetHeight((float)Height);

	const ZEGRTexture2D* DepthStencilMap = GetPrevOutput(ZERN_SO_DEPTH);

	Context->SetSampler(ZEGR_ST_PIXEL, 4, SamplerPointClamp);
	Context->SetTexture(ZEGR_ST_ALL, 0, DepthStencilMap);
	Context->SetTexture(ZEGR_ST_ALL, 1, GetPrevOutput(ZERN_SO_ACCUMULATION));
	Context->SetTexture(ZEGR_ST_ALL, 2, GetPrevOutput(ZERN_SO_NORMAL));
	Context->SetTexture(ZEGR_ST_ALL, 3, GetPrevOutput(ZERN_SO_GBUFFER_DIFFUSE));
	Context->SetTexture(ZEGR_ST_ALL, 4, GetPrevOutput(ZERN_SO_GBUFFER_SPECULAR));
	Context->SetTexture(ZEGR_ST_PIXEL, 7, GetPrevOutput(ZERN_SO_AMBIENT_OCCLUSION));

	RenderTarget = OutputTexture->GetRenderTarget();
	Context->ClearRenderTarget(RenderTarget, ZEVector4::Zero);
	Context->SetRenderTargets(1, &RenderTarget, DepthStencilMap->GetDepthStencilBuffer(true));
	Context->SetViewports(1, &Viewport);

	if (!Update())
		return false;

	if(DeferredLightList.GetCount() > 0)
		SetupDeferred(Context);

	if(TiledDeferredLightList.GetCount() > 0)
	{
		if (SetupComputeTiledDeferred(Context))
		{	
			Context->SetRenderState(AccumulateRenderState);
			Context->SetTexture(ZEGR_ST_PIXEL, 9, TiledDeferredComputeOutputTexture);
			Context->SetVertexBuffers(0, 0, NULL);
			Context->Draw(3, 0);
			Context->SetTexture(ZEGR_ST_PIXEL, 9, NULL);
		}
	}

	CleanUp(Context);

	return false;
}

void ZERNStageLighting::CleanUp(ZEGRContext* Context)
{
	Context->SetSampler(ZEGR_ST_PIXEL, 4, NULL);
	Context->SetTexture(ZEGR_ST_ALL, 0, NULL);
	Context->SetTexture(ZEGR_ST_ALL, 1, NULL);
	Context->SetTexture(ZEGR_ST_ALL, 2, NULL);
	Context->SetTexture(ZEGR_ST_ALL, 3, NULL);
	Context->SetTexture(ZEGR_ST_ALL, 4, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 7, NULL);
	Context->SetRenderTargets(0, NULL, NULL);

	ZERNStage::CleanUp(Context);
}

ZERNStageLighting::ZERNStageLighting()
{
	DirtyFlags.RaiseAll();

	ShowCascades = false;
}
