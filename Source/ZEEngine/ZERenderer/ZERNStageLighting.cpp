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

#define MAX_CULLABLE_LIGHT		1024
#define TILE_SIZE_IN_PIXELS		16

#define ZERN_SLDF_LIGHT_BUFFER	1
#define ZERN_SLDF_TILE_BUFFER	2
#define ZERN_SLDF_SHADERS		4
#define ZERN_SLDF_RENDER_STATE	8

void ZERNStageLighting::CreateRandomVectors()
{
	ZEUInt Size = 128 * 128 * 2;
	ZEArray<ZEUInt8> RandomVectors;
	RandomVectors.SetCount(Size);
	for(ZEUInt I = 0; I < Size; I += 2)
	{
		RandomVectors[I]		= (ZEUInt8)(ZERandom::GetFloatPositive() * 255.0f + 0.5f);
		RandomVectors[I + 1]	= (ZEUInt8)(ZERandom::GetFloatPositive() * 255.0f + 0.5f);
	}

	RandomVectorsTexture = ZEGRTexture2D::CreateInstance(128, 128, 1, ZEGR_TF_R8G8_UNORM, ZEGR_RU_GPU_READ_ONLY, ZEGR_RBF_SHADER_RESOURCE, 1, 1, RandomVectors.GetConstCArray());
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

	GraphicsRenderState.SetBlendState(BlendStateAdditive);

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

	GraphicsRenderState.SetBlendState(BlendStateAdditive);
	GraphicsRenderState.SetDepthStencilState(DepthStencilStateTestNoWriteLess);
	GraphicsRenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	GraphicsRenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);
	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, AccumulateAmbientEmissivePixelShader);

	AccumulateAmbientEmissiveRenderState = GraphicsRenderState.Compile();
	zeCheckError(AccumulateAmbientEmissiveRenderState == NULL, false, "Cannot set render state.");

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
	Options.EntryPoint = "ZERNDeferredShading_Accumulate_AmbientEmissive_PixelShader_Main";
	AccumulateAmbientEmissivePixelShader = ZEGRShader::Compile(Options);
	zeCheckError(AccumulateAmbientEmissivePixelShader == NULL, false, "Cannot set pixel shader.");

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
	Context->SetConstantBuffers(ZEGR_ST_VERTEX, 8, 1, DeferredLightConstantBuffer.GetPointerToPointer());
	Context->SetConstantBuffers(ZEGR_ST_PIXEL, 8, 1, DeferredLightConstantBuffer.GetPointerToPointer());

	Context->SetRenderState(DeferredRenderState);
	Context->SetVertexBuffers(0, 1, DeferredLightVertexBuffer.GetPointerToPointer());

	bool DirectionalLightExist = false;

	const ZERNView& View = GetRenderer()->GetView();

	ZERNStage* StageShadowing = GetRenderer()->GetStage(ZERN_STAGE_SHADOWING);
	bool ShadowingEnabled = (StageShadowing != NULL && StageShadowing->GetEnabled());

	ZEUInt Index = 0;
	DeferredLightConstantsStruct* LightConstants;
	DeferredLightConstantBuffer->Lock(reinterpret_cast<void**>(&LightConstants));
	ze_for_each(Link, DeferredLightList)
	{
		ZELight* Light = Link.GetPointer();
		if (Light->GetLightType() == ZE_LT_DIRECTIONAL)
		{
			DirectionalLightExist = true;

			ZELightDirectional* DirectionalLight = static_cast<ZELightDirectional*>(Light);
			LightStruct& DestLight = LightConstants->Light[Index++];

			DestLight.Color = DirectionalLight->GetColor() * DirectionalLight->GetIntensity();
			DestLight.CastShadow = static_cast<ZEBool32>(DirectionalLight->GetCastsShadow() & ShadowingEnabled);
			ZEMatrix4x4::Transform3x3(DestLight.DirectionView, View.ViewTransform, DirectionalLight->GetWorldRotation() * -ZEVector3::UnitZ);
			DestLight.DirectionView.NormalizeSelf();
			DestLight.Type = DirectionalLight->GetLightType();
			DestLight.ShadowSampleCount = ZELight::ConvertShadowSampleCount(DirectionalLight->GetShadowSampleCount());
			DestLight.ShadowSampleLength = DirectionalLight->GetShadowSampleLength();
			DestLight.ShadowDepthBias = DirectionalLight->GetShadowDepthBias();
			DestLight.ShadowNormalBias = DirectionalLight->GetShadowNormalBias();

			if (DirectionalLight->GetUseSunLight() || DirectionalLight->GetUseMoonLight())
				DestLight.Color = DirectionalLight->GetTerrestrialColor() * DirectionalLight->GetIntensity();

			if (DestLight.CastShadow)
			{
				DirectionalLight->BindCascades(GetRenderer(), Context);

				const ZEGRSampler* Samplers[] = {SamplerComparisonLinearPointClamp, SamplerPointWrap};
				Context->SetSamplers(ZEGR_ST_PIXEL, 1, 2, Samplers);
				
				const ZEGRTexture* Textures[] = {DirectionalLight->GetShadowMap(), RandomVectorsTexture};
				Context->SetTextures(ZEGR_ST_PIXEL, 6, 2, Textures);
			}
		}
	}
	DeferredLightConstantBuffer->Unlock();

	if (DirectionalLightExist)
		Context->Draw(6, 0);

	ze_for_each(Link, DeferredLightList)
	{
		ZELight* Light = Link.GetPointer();

		if (Light->GetLightType() == ZE_LT_PROJECTIVE)
			DrawProjectiveLight(static_cast<ZELightProjective*>(Light), Context);

		else if (Light->GetLightType() == ZE_LT_POINT)
			DrawPointLight(static_cast<ZELightPoint*>(Light), Context);
	}

	DeferredLightList.Clean();

	return true;
}

bool ZERNStageLighting::SetupComputeTiledDeferred(ZEGRContext* Context)
{
	const ZERNView& View = GetRenderer()->GetView();

	ZERNStage* StageShadowing = GetRenderer()->GetStage(ZERN_STAGE_SHADOWING);
	bool StageShadowingEnabled = (StageShadowing != NULL && StageShadowing->GetEnabled());

	ZEUInt CullableLightCount = 0;
	ZEUInt DirectionalLightCount = 0;
	TiledDeferredCullableLightStruct* Lights;
	TiledDeferredCullableLightStructuredBuffer->Lock(reinterpret_cast<void**>(&Lights));
	ze_for_each(Link, TiledDeferredLightList)
	{
		ZELight* SrcLight = Link.GetPointer();

		if (SrcLight->GetLightType() != ZE_LT_DIRECTIONAL)
		{
			TiledDeferredCullableLightStruct& CullableLight = Lights[CullableLightCount++];
			LightStruct& DestLight = CullableLight.Light;

			ZEMatrix4x4::Transform(DestLight.PositionView, View.ViewTransform, SrcLight->GetWorldPosition());
			DestLight.Range = SrcLight->GetRange();
			DestLight.Color = SrcLight->GetColor() * SrcLight->GetIntensity();
			DestLight.Attenuation = SrcLight->GetAttenuation();
			DestLight.CastShadow =  (ZEBool32)(SrcLight->GetCastsShadow() & StageShadowingEnabled);
			DestLight.Type = SrcLight->GetLightType();
			DestLight.ShadowSampleCount = ZELight::ConvertShadowSampleCount(SrcLight->GetShadowSampleCount());
			DestLight.ShadowSampleLength = SrcLight->GetShadowSampleLength();
			DestLight.ShadowDepthBias = SrcLight->GetShadowDepthBias();

			if (SrcLight->GetLightType() == ZE_LT_PROJECTIVE)
			{
				ZEAABBox BoundingBox = SrcLight->GetWorldBoundingBox();
				CullableLight.CullRange = BoundingBox.GetLength() * 0.5f;
				CullableLight.CullPositionView = View.ViewTransform * BoundingBox.GetCenter();
			}
			else
			{
				CullableLight.CullRange = DestLight.Range;
				CullableLight.CullPositionView = DestLight.PositionView;
			}

			CullableLight.ProjectionTransform = SrcLight->GetProjectionTransform() * SrcLight->GetViewTransform() * View.InvViewTransform;
		}
		else
		{
			LightStruct& DestLight = TiledDeferredConstants.DirectionalLights[DirectionalLightCount++];
		
			DestLight.Color = SrcLight->GetColor() * SrcLight->GetIntensity();
			DestLight.CastShadow = (ZEBool32)(SrcLight->GetCastsShadow() & StageShadowingEnabled);
			ZEMatrix4x4::Transform3x3(DestLight.DirectionView, View.ViewTransform, SrcLight->GetWorldRotation() * -ZEVector3::UnitZ);
			DestLight.DirectionView.NormalizeSelf();
			DestLight.Type = SrcLight->GetLightType();
			DestLight.ShadowSampleCount = ZELight::ConvertShadowSampleCount(SrcLight->GetShadowSampleCount());
			DestLight.ShadowSampleLength = SrcLight->GetShadowSampleLength();
			DestLight.ShadowDepthBias = SrcLight->GetShadowDepthBias();

			ZELightDirectional* DirectionalLight = static_cast<ZELightDirectional*>(SrcLight);

			if (DirectionalLight->GetUseSunLight() || DirectionalLight->GetUseMoonLight())
				DestLight.Color = DirectionalLight->GetTerrestrialColor() * DirectionalLight->GetIntensity();

			if (DestLight.CastShadow)
			{
				DirectionalLight->BindCascades(GetRenderer(), Context);

				const ZEGRSampler* Samplers[] = {SamplerComparisonLinearPointClamp, SamplerPointWrap};
				Context->SetSamplers(ZEGR_ST_COMPUTE, 1, 2, Samplers);

				const ZEGRTexture* Textures[] = {DirectionalLight->GetShadowMap(), RandomVectorsTexture};
				Context->SetTextures(ZEGR_ST_COMPUTE, 8, 2, Textures);
			}
		}
	}
	TiledDeferredCullableLightStructuredBuffer->Unlock();

	TiledDeferredLightList.Clean();

	TiledDeferredConstants.CullableLightCount = CullableLightCount;
	TiledDeferredConstants.DirectionalLightCount = DirectionalLightCount;
	TiledDeferredConstantBuffer->SetData(&TiledDeferredConstants);

	Context->SetComputeRenderState(TiledDeferredComputeRenderState);

	Context->SetConstantBuffers(ZEGR_ST_COMPUTE, 8, 1, TiledDeferredConstantBuffer.GetPointerToPointer());
	Context->SetStructuredBuffers(ZEGR_ST_COMPUTE, 10, 1, TiledDeferredCullableLightStructuredBuffer.GetPointerToPointer());
	Context->SetRWTextures(0, 1, reinterpret_cast<ZEGRTexture**>(&OutputTexture));

	ZEUInt TileCountX = (OutputTexture->GetWidth() + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;
	ZEUInt TileCountY = (OutputTexture->GetHeight() + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;

	Context->Dispatch(TileCountX, TileCountY, 1);

	return true;
}

void ZERNStageLighting::DrawDirectionalLight(ZELightDirectional* DirectionalLight, ZEGRContext* Context)
{
	DeferredLightConstantsStruct* LightConstants;
	DeferredLightConstantBuffer->Lock((void**)&LightConstants);
		LightStruct& Light = LightConstants->Light[0];
		Light.Color = DirectionalLight->GetColor() * DirectionalLight->GetIntensity();
		Light.Type = DirectionalLight->GetLightType();
		ZEVector3 Direction = DirectionalLight->GetWorldRotation() * -ZEVector3::UnitZ;
		ZEMatrix4x4::Transform3x3(Light.DirectionView, GetRenderer()->GetView().ViewTransform, Direction);
		Light.DirectionView.NormalizeSelf();
		ZERNStage* StageShadowing = GetRenderer()->GetStage(ZERN_STAGE_SHADOWING);
		bool ShadowingEnabled = (StageShadowing != NULL && StageShadowing->GetEnabled());
		Light.CastShadow = static_cast<ZEBool32>(DirectionalLight->GetCastsShadow() & ShadowingEnabled);
		Light.ShadowSampleCount = ZELight::ConvertShadowSampleCount(DirectionalLight->GetShadowSampleCount());
		Light.ShadowSampleLength = DirectionalLight->GetShadowSampleLength();
		Light.ShadowDepthBias = DirectionalLight->GetShadowDepthBias();

		if (DirectionalLight->GetUseSunLight() || DirectionalLight->GetUseMoonLight())
			Light.Color = DirectionalLight->GetTerrestrialColor() * DirectionalLight->GetIntensity();

		if (Light.CastShadow)
		{
			DirectionalLight->BindCascades(GetRenderer(), Context);

			const ZEGRSampler* Samplers[] = {SamplerComparisonLinearPointClamp, SamplerPointWrap};
			Context->SetSamplers(ZEGR_ST_PIXEL, 1, 2, Samplers);

			const ZEGRTexture* Textures[] = {DirectionalLight->GetShadowMap(), RandomVectorsTexture};
			Context->SetTextures(ZEGR_ST_PIXEL, 7, 2, Textures);
		}
	DeferredLightConstantBuffer->Unlock();

	Context->Draw(6, 0);

	Context->SetSamplers(ZEGR_ST_PIXEL, 1, 2, NULL);
	Context->SetTextures(ZEGR_ST_PIXEL, 7, 2, NULL);
}

void ZERNStageLighting::DrawProjectiveLight(ZELightProjective* ProjectiveLight, ZEGRContext* Context)
{
	ZERNStage* StageShadowing = GetRenderer()->GetStage(ZERN_STAGE_SHADOWING);
	bool ShadowingEnabled = (StageShadowing != NULL && StageShadowing->GetEnabled());

	DeferredLightConstantsStruct* LightConstants;
	DeferredLightConstantBuffer->Lock(reinterpret_cast<void**>(&LightConstants));
		LightStruct& Light = LightConstants->Light[0];
		ZEMatrix4x4::Transform(Light.PositionView, GetRenderer()->GetView().ViewTransform, ProjectiveLight->GetWorldPosition());
		Light.Range = ProjectiveLight->GetRange();
		Light.Color = ProjectiveLight->GetColor() * ProjectiveLight->GetIntensity();
		Light.Attenuation = ProjectiveLight->GetAttenuation();
		Light.CastShadow = (ZEBool32)(ProjectiveLight->GetCastsShadow() & ShadowingEnabled);
		Light.DirectionView = ProjectiveLight->GetWorldRotation() * ZEVector3::UnitZ;
		Light.Type = ProjectiveLight->GetLightType();
		Light.ShadowSampleCount = ZELight::ConvertShadowSampleCount(ProjectiveLight->GetShadowSampleCount());
		Light.ShadowSampleLength = ProjectiveLight->GetShadowSampleLength();
		Light.ShadowDepthBias = ProjectiveLight->GetShadowDepthBias();

		LightConstants->ProjectionMatrix = ProjectiveLight->GetProjectionTransform() * ProjectiveLight->GetViewTransform() * GetRenderer()->GetView().InvViewTransform;

		float TanFovRange = ZEAngle::Tan(ProjectiveLight->GetFOV() * 0.5f) * ProjectiveLight->GetRange();
		ZEMatrix4x4::CreateOrientation(LightConstants->WorldMatrix, 
										ProjectiveLight->GetWorldPosition(), 
										ProjectiveLight->GetWorldRotation(),
										ZEVector3(TanFovRange * ProjectiveLight->GetAspectRatio() * 2.0f, TanFovRange * 2.0f, ProjectiveLight->GetRange()));
	DeferredLightConstantBuffer->Unlock();

	const ZEGRSampler* Samplers[] = {SamplerLinearBorder, SamplerComparisonLinearPointClamp, SamplerPointWrap};
	Context->SetSamplers(ZEGR_ST_PIXEL, 0, 3, Samplers);
	const ZEGRTexture* Textures[] = {ProjectiveLight->GetProjectionTexture(), ProjectiveLight->GetShadowMap(), RandomVectorsTexture};
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 3, Textures);

	Context->Draw(18, 4542);
}

void ZERNStageLighting::DrawPointLight(ZELightPoint* PointLight, ZEGRContext* Context)
{
	DeferredLightConstantsStruct* LightConstants;
	DeferredLightConstantBuffer->Lock(reinterpret_cast<void**>(&LightConstants));
		LightStruct& Light = LightConstants->Light[0];
		ZEMatrix4x4::Transform(Light.PositionView, GetRenderer()->GetView().ViewTransform, PointLight->GetWorldPosition());
		Light.Range = PointLight->GetRange();
		Light.Color = PointLight->GetColor() * PointLight->GetIntensity();
		Light.Attenuation = PointLight->GetAttenuation();
		Light.CastShadow = false;
		Light.Type = PointLight->GetLightType();

		ZEMatrix4x4::CreateOrientation(LightConstants->WorldMatrix, PointLight->GetWorldPosition(), ZEQuaternion::Identity, ZEVector3(Light.Range));
	DeferredLightConstantBuffer->Unlock();

	Context->Draw(3600, 942);
}

bool ZERNStageLighting::InitializeSelf()
{
	if (!ZERNStage::InitializeSelf())
		return false;

	CreateSamplers();
	CreateRandomVectors();
	CreateLightGeometries();

	DeferredLightConstantBuffer = ZEGRConstantBuffer::Create(sizeof(DeferredLightConstantsStruct));
	TiledDeferredCullableLightStructuredBuffer = ZEGRStructuredBuffer::Create(MAX_CULLABLE_LIGHT, sizeof(TiledDeferredCullableLightStruct));
	TiledDeferredConstantBuffer = ZEGRConstantBuffer::Create(sizeof(TiledDeferredConstants));

	MultipleShadowMaps = ZEGRTexture2D::CreateInstance(1024, 1024, 1, ZEGR_TF_D32_FLOAT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_DEPTH_STENCIL, 10);

	return Update();
}

void ZERNStageLighting::DeinitializeSelf()
{
	DirtyFlags.RaiseAll();

	OutputTexture.Release();

	ScreenCoverVertexShader.Release();
	DeferredVertexShader.Release();
	DeferredPixelShader.Release();
	TiledDeferredComputeShader.Release();
	AccumulateAmbientEmissivePixelShader.Release();

	DeferredRenderState.Release();
	TiledDeferredComputeRenderState.Release();
	AccumulateAmbientEmissiveRenderState.Release();

	DeferredLightConstantBuffer.Release();
	DeferredLightVertexBuffer.Release();
	TiledDeferredCullableLightStructuredBuffer.Release();
	TiledDeferredConstantBuffer.Release();

	SamplerLinearBorder.Release();
	SamplerComparisonLinearPointClamp.Release();
	SamplerPointWrap.Release();

	MultipleShadowMaps.Release();
	RandomVectorsTexture.Release();

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

	if (!Update())
		return false;

	ze_for_each(Command, GetCommands())
	{
		ZELight* Light = static_cast<ZELight*>(Command->Entity);

		switch (Light->GetLightType())
		{
			case ZE_LT_DIRECTIONAL:
			case ZE_LT_PROJECTIVE:
			case ZE_LT_POINT:
			case ZE_LT_OMNIPROJECTIVE:
				DeferredLightList.AddEnd(new ZELink<ZELight>(Light));
				break;
			//case ZE_LT_DIRECTIONAL:
			//case ZE_LT_PROJECTIVE:
			//case ZE_LT_POINT:
			//case ZE_LT_OMNIPROJECTIVE:
			//	TiledDeferredLightList.AddEnd(new ZELink<ZELight>(Light));
			//	break;
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
		OutputTexture = ZEGRTexture2D::CreateInstance(Width, Height, 1, ZEGR_TF_R11G11B10_FLOAT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET | ZEGR_RBF_UNORDERED_ACCESS);
	
		Viewport.SetWidth((float)Width);
		Viewport.SetHeight((float)Height);
	}

	const ZEGRTexture2D* DepthStencilMap = GetPrevOutput(ZERN_SO_DEPTH);
	const ZEGRTexture* Textures[] = 
	{
		GetPrevOutput(ZERN_SO_ACCUMULATION), 
		GetPrevOutput(ZERN_SO_GBUFFER_EMISSIVE),
		GetPrevOutput(ZERN_SO_GBUFFER_DIFFUSE), 
		GetPrevOutput(ZERN_SO_NORMAL), 
		DepthStencilMap
	};

	Context->SetTextures(ZEGR_ST_PIXEL, 0, 5, Textures);

	RenderTarget = OutputTexture->GetRenderTarget();
	Context->ClearRenderTarget(RenderTarget, ZEVector4::Zero);

	if (TiledDeferredLightList.GetCount() > 0)
	{
		Context->SetTextures(ZEGR_ST_COMPUTE, 0, 5, Textures);

		SetupComputeTiledDeferred(Context);
	}
	Context->SetRenderTargets(1, &RenderTarget, DepthStencilMap->GetDepthStencilBuffer(true));
	Context->SetViewports(1, &Viewport);

	if (DeferredLightList.GetCount() > 0)
		SetupDeferred(Context);

	Context->SetRenderState(AccumulateAmbientEmissiveRenderState);
	Context->Draw(3, 0);

	CleanUp(Context);

	return false;
}

void ZERNStageLighting::CleanUp(ZEGRContext* Context)
{
	ZERNStage::CleanUp(Context);
}

ZERNStageLighting::ZERNStageLighting()
{
	DirtyFlags.RaiseAll();

	ShowCascades = false;
	memset(&TiledDeferredConstants, 0, sizeof(TiledDeferredConstants));
}
