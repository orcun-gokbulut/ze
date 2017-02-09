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
#include "ZEMath/ZEMatrix.h"
#include "ZERNStageID.h"
#include "ZERNRenderer.h"
#include "ZERNStage.h"
#include "ZELightPoint.h"
#include "ZELightDirectional.h"
#include "ZELightProjective.h"
#include "ZELightOmniProjective.h"
#include "ZEGame/ZEScene.h"
#include "ZEGame/ZEEntity.h"
#include "ZEGraphics/ZEGRCanvas.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZELightSpot.h"
#include "ZERNStageShadowing.h"

#define ZERN_SLDF_SHADERS						1
#define ZERN_SLDF_RENDER_STATE					2
#define ZERN_SLDF_EDGEDETECTION_CONSTANT_BUFFER	4

void ZERNStageLighting::CreateRandomVectors()
{
	ZEUInt Size = 128 * 128 * 2;
	ZEArray<ZEUInt8> RandomVectors;
	RandomVectors.SetCount(Size);
	for (ZEUInt I = 0; I < Size; I += 2)
	{
		RandomVectors[I]		= (ZEUInt8)(ZERandom::GetFloatPositive() * 255.0f + 0.5f);
		RandomVectors[I + 1]	= (ZEUInt8)(ZERandom::GetFloatPositive() * 255.0f + 0.5f);
	}
	
	RandomVectorsTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, 128, 128, 1, ZEGR_TF_R8G8_UNORM, ZEGR_RU_IMMUTABLE, ZEGR_RBF_SHADER_RESOURCE, 1, 1, RandomVectors.GetConstCArray());
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

	ZEGRCanvas Canvas;

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

	DeferredLightVertexBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_VERTEX_BUFFER, Canvas.GetBufferSize(), sizeof(ZECanvasVertex), ZEGR_RU_IMMUTABLE, ZEGR_RBF_VERTEX_BUFFER, ZEGR_TF_NONE, Canvas.GetBuffer());
}

bool ZERNStageLighting::UpdateShaders()
{
	if(!DirtyFlags.GetFlags(ZERN_SLDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.Model = ZEGR_SM_5_0;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNDeferredShading.hlsl";
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString::FromInt32(ZEGRGraphicsModule::SAMPLE_COUNT)));

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_Position";
	ScreenCoverVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(ScreenCoverVertexShader == NULL, false, "Cannot set vertex shader.");
	
	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNDeferredShading_VertexShader_LightingStage";

	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_DIRECTIONAL_LIGHT"));
	DeferredDirectionalLightVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(DeferredDirectionalLightVertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_SPOT_LIGHT"));
	DeferredSpotLightVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(DeferredSpotLightVertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_SPOT_LIGHT_SHADOW"));
	DeferredSpotLightShadowVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(DeferredSpotLightShadowVertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_POINT_LIGHT"));
	DeferredPointLightVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(DeferredPointLightVertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_PROJECTIVE_LIGHT"));
	Options.EntryPoint = "ZERNDeferredShading_VertexShader_LightingStage";
	ProjectiveLightVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(ProjectiveLightVertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNDeferredShading_PixelShader_LightingStage";

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_DIRECTIONAL_LIGHT"));
	DeferredDirectionalLightPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(DeferredDirectionalLightPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_SPOT_LIGHT"));
	DeferredSpotLightPixelShader= ZEGRShader::Compile(Options);
	zeCheckError(DeferredSpotLightPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_SPOT_LIGHT_SHADOW"));
	DeferredSpotLightShadowPixelShader= ZEGRShader::Compile(Options);
	zeCheckError(DeferredSpotLightShadowPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_POINT_LIGHT"));
	DeferredPointLightPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(DeferredPointLightPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_PROJECTIVE_LIGHT"));
	Options.EntryPoint = "ZERNDeferredShading_PixelShader_LightingStage";
	ProjectiveLightPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(ProjectiveLightPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.EntryPoint = "ZERNDeferredShading_PixelShader_PerSample_LightingStage";

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_DIRECTIONAL_LIGHT"));
	DeferredDirectionalLightPixelShaderPerSample = ZEGRShader::Compile(Options);
	zeCheckError(DeferredDirectionalLightPixelShaderPerSample == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_SPOT_LIGHT"));
	DeferredSpotLightPixelShaderPerSample = ZEGRShader::Compile(Options);
	zeCheckError(DeferredSpotLightPixelShaderPerSample == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_SPOT_LIGHT_SHADOW"));
	DeferredSpotLightShadowPixelShaderPerSample = ZEGRShader::Compile(Options);
	zeCheckError(DeferredSpotLightShadowPixelShaderPerSample == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_POINT_LIGHT"));
	DeferredPointLightPixelShaderPerSample = ZEGRShader::Compile(Options);
	zeCheckError(DeferredPointLightPixelShaderPerSample == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_PROJECTIVE_LIGHT"));
	Options.EntryPoint = "ZERNDeferredShading_PixelShader_LightingStage";
	ProjectiveLightPixelShaderPerSample = ZEGRShader::Compile(Options);
	zeCheckError(ProjectiveLightPixelShaderPerSample == NULL, false, "Cannot set pixel shader.");

	Options.EntryPoint = "ZERNDeferredShading_EdgeDetection_PixelShader_Main";
	EdgeDetectionPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(EdgeDetectionPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.EntryPoint = "ZERNDeferredShading_Blend_PixelShader_Main";
	BlendPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(BlendPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.EntryPoint = "ZERNDeferredShading_BlendPerSample_PixelShader_Main";
	BlendPixelShaderPerSample = ZEGRShader::Compile(Options);
	zeCheckError(BlendPixelShaderPerSample == NULL, false, "Cannot set pixel shader.");

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNTiledDeferredShadingCompute.hlsl";
	Options.Type = ZEGR_ST_COMPUTE;
	Options.EntryPoint = "ZERNTiledDeferredShadingCompute_ComputeShader_Main";

	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_TILED_DEFERRED"));
	TiledDeferredComputeShader = ZEGRShader::Compile(Options);
	zeCheckError(TiledDeferredComputeShader == NULL, false, "Cannot set compute shader.");

	DirtyFlags.UnraiseFlags(ZERN_SLDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_SLDF_RENDER_STATE);

	return true;
}

bool ZERNStageLighting::UpdateRenderState()
{
	if (!DirtyFlags.GetFlags(ZERN_SLDF_RENDER_STATE))
		return true;

	ZEGRRenderState GraphicsRenderState;

	ZEGRBlendState BlendStateAdditive;
	BlendStateAdditive.SetBlendEnable(true);
	ZEGRBlendRenderTarget BlendRenderTargetAdditive = BlendStateAdditive.GetRenderTarget(0);
	BlendRenderTargetAdditive.SetSource(ZEGRBlend::ZEGR_BO_ONE);
	BlendRenderTargetAdditive.SetDestination(ZEGRBlend::ZEGR_BO_ONE);
	BlendRenderTargetAdditive.SetOperation(ZEGRBlendOperation::ZEGR_BE_ADD);
	BlendRenderTargetAdditive.SetBlendEnable(true);
	BlendStateAdditive.SetRenderTargetBlend(0, BlendRenderTargetAdditive);

	GraphicsRenderState.SetBlendState(BlendStateAdditive);

	ZEGRDepthStencilState DepthStencilStateTestNoWriteLessTestStencil;
	DepthStencilStateTestNoWriteLessTestStencil.SetDepthTestEnable(true);
	DepthStencilStateTestNoWriteLessTestStencil.SetDepthWriteEnable(false);
	DepthStencilStateTestNoWriteLessTestStencil.SetDepthFunction(ZEGR_CF_LESS);
	DepthStencilStateTestNoWriteLessTestStencil.SetStencilTestEnable(true);
	DepthStencilStateTestNoWriteLessTestStencil.SetFrontStencilFunction(ZEGR_CF_EQUAL);
	DepthStencilStateTestNoWriteLessTestStencil.SetBackStencilFunction(ZEGR_CF_EQUAL);

	GraphicsRenderState.SetDepthStencilState(DepthStencilStateTestNoWriteLessTestStencil);

	ZEGRRasterizerState RasterizerStateCullFront;
	RasterizerStateCullFront.SetCullMode(ZEGR_CMD_FRONT);

	GraphicsRenderState.SetRasterizerState(RasterizerStateCullFront);
	GraphicsRenderState.SetVertexLayout(*ZECanvasVertex::GetVertexLayout());

	GraphicsRenderState.SetShader(ZEGR_ST_VERTEX, ProjectiveLightVertexShader);
	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, ProjectiveLightPixelShader);

	ProjectiveLightRenderState = GraphicsRenderState.Compile();
	zeCheckError(ProjectiveLightRenderState == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, ProjectiveLightPixelShaderPerSample);

	ProjectiveLightRenderStatePerSample = GraphicsRenderState.Compile();
	zeCheckError(ProjectiveLightRenderStatePerSample == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_VERTEX, DeferredDirectionalLightVertexShader);
	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DeferredDirectionalLightPixelShader);

	DeferredDirectionalLightRenderState = GraphicsRenderState.Compile();
	zeCheckError(DeferredDirectionalLightRenderState == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DeferredDirectionalLightPixelShaderPerSample);

	DeferredDirectionalLightRenderStatePerSample = GraphicsRenderState.Compile();
	zeCheckError(DeferredDirectionalLightRenderStatePerSample == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_VERTEX, DeferredSpotLightVertexShader);
	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DeferredSpotLightPixelShader);

	DeferredSpotLightRenderState = GraphicsRenderState.Compile();
	zeCheckError(DeferredSpotLightRenderState == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DeferredSpotLightPixelShaderPerSample);

	DeferredSpotLightRenderStatePerSample = GraphicsRenderState.Compile();
	zeCheckError(DeferredSpotLightRenderStatePerSample == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_VERTEX, DeferredSpotLightShadowVertexShader);
	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DeferredSpotLightShadowPixelShader);

	DeferredSpotLightShadowRenderState = GraphicsRenderState.Compile();
	zeCheckError(DeferredSpotLightShadowRenderState == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DeferredSpotLightShadowPixelShaderPerSample);

	DeferredSpotLightShadowRenderStatePerSample = GraphicsRenderState.Compile();
	zeCheckError(DeferredSpotLightShadowRenderStatePerSample == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_VERTEX, DeferredPointLightVertexShader);
	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DeferredPointLightPixelShader);

	DeferredPointLightRenderState = GraphicsRenderState.Compile();
	zeCheckError(DeferredPointLightRenderState == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DeferredPointLightPixelShaderPerSample);

	DeferredPointLightRenderStatePerSample = GraphicsRenderState.Compile();
	zeCheckError(DeferredPointLightRenderStatePerSample == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetRasterizerState(ZEGRRasterizerState());
	GraphicsRenderState.SetBlendState(BlendStateAdditive);
	GraphicsRenderState.SetDepthStencilState(DepthStencilStateTestNoWriteLessTestStencil);
	GraphicsRenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, BlendPixelShader);
	BlendRenderState = GraphicsRenderState.Compile();
	zeCheckError(BlendRenderState == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, BlendPixelShaderPerSample);
	BlendRenderStatePerSample = GraphicsRenderState.Compile();
	zeCheckError(BlendRenderStatePerSample == NULL, false, "Cannot set render state.");

	GraphicsRenderState = ZEGRRenderState();

	ZEGRDepthStencilState DepthStencilStateTestNoWriteLessWriteStencil;
	DepthStencilStateTestNoWriteLessWriteStencil.SetDepthTestEnable(true);
	DepthStencilStateTestNoWriteLessWriteStencil.SetDepthWriteEnable(false);
	DepthStencilStateTestNoWriteLessWriteStencil.SetDepthFunction(ZEGR_CF_LESS);
	DepthStencilStateTestNoWriteLessWriteStencil.SetStencilTestEnable(true);
	DepthStencilStateTestNoWriteLessWriteStencil.SetFrontStencilPass(ZEGR_SO_REPLACE);
	DepthStencilStateTestNoWriteLessWriteStencil.SetBackStencilPass(ZEGR_SO_REPLACE);

	GraphicsRenderState.SetDepthStencilState(DepthStencilStateTestNoWriteLessWriteStencil);
	GraphicsRenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);
	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, EdgeDetectionPixelShader);

	EdgeDetectionRenderState = GraphicsRenderState.Compile();
	zeCheckError(EdgeDetectionRenderState == NULL, false, "Cannot set render state.");

	ZEGRComputeRenderState ComputeRenderState;
	ComputeRenderState.SetComputeShader(TiledDeferredComputeShader);
	TiledDeferredComputeRenderState = ComputeRenderState.Compile();
	zeCheckError(TiledDeferredComputeRenderState == NULL, false, "Cannot set render state.");

	DirtyFlags.UnraiseFlags(ZERN_SLDF_RENDER_STATE);

	return true;
}

bool ZERNStageLighting::UpdateLightBuffers()
{
	const ZERNView& View = GetRenderer()->GetView();

	ZERNStageShadowing* StageShadowing = static_cast<ZERNStageShadowing*>(GetRenderer()->GetStage(ZERN_STAGE_SHADOWING));
	bool ShadowingEnabled = (StageShadowing != NULL && StageShadowing->GetEnabled());

	ZEUInt DirectionalLightCount = 0;
	ZEUInt DirectionalShadowMapIndex = 0;
	ZEUInt ProjectiveShadowMapIndex = 0;

	ZEMatrix4x4 TextureTransform;
	ZEMatrix4x4::Create(TextureTransform,
		0.5f, 0.0f, 0.0f, 0.5f,
		0.0f, -0.5f, 0.0f, 0.5f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	PointLights.Clear();
	SpotLights.Clear();
	SpotLightsShadow.Clear();
	ProjectiveLights.Clear();
	ProjectionTextures.Clear();

	ze_for_each(Command, GetCommands())
	{
		if (Command->GetClass() == ZERNCommandDirectionalLight::Class())
		{
			if (DirectionalLightCount > MAX_DIRECTIONAL_LIGHT)
				continue;

			const ZERNCommandDirectionalLight& DirectionalLight = static_cast<const ZERNCommandDirectionalLight&>(Command.GetItem());

			ZERNShading_DirectionalLight& DestLight = Constants.DirectionalLights[DirectionalLightCount++];

			ZEMatrix4x4::Transform3x3(DestLight.DirectionView, View.ViewTransform, DirectionalLight.RotationWorld * ZEVector3::NegUnitZ);
			DestLight.DirectionView.NormalizeSelf();
			DestLight.Color = DirectionalLight.Color;
			DestLight.CascadeCount = (ZEUInt)DirectionalLight.Cascades.GetCount();
			DestLight.CastShadow = DirectionalLight.CastShadow && ShadowingEnabled && (DirectionalShadowMapIndex + DestLight.CascadeCount) <= (StageShadowing->GetDirectionalShadowMapCount() * StageShadowing->GetCascadeCount());

			if (DestLight.CastShadow)
			{
				DestLight.ShadowMapIndex = DirectionalShadowMapIndex;

				DirectionalShadowMapIndex += DestLight.CascadeCount;

				ze_for_each(Cascade, DirectionalLight.Cascades)
				{
					ZEUInt CascadeIndex = Cascade.GetIndex();
					DestLight.Cascades[CascadeIndex].ViewTransform = Cascade->ViewTransform * View.InvViewTransform;
					DestLight.Cascades[CascadeIndex].ProjectionTransform = TextureTransform * Cascade->ProjectionTransform;
					DestLight.Cascades[CascadeIndex].SampleCount = Cascade->SampleCount;
					DestLight.Cascades[CascadeIndex].SampleLength = Cascade->SampleLength;
					DestLight.Cascades[CascadeIndex].DepthBias = Cascade->DepthBias;
					DestLight.Cascades[CascadeIndex].NormalBias = Cascade->NormalBias;
				}
			}
		}
		else if (Command->GetClass() == ZERNCommandPointLight::Class())
		{
			const ZERNCommandPointLight& PointLight = static_cast<const ZERNCommandPointLight&>(Command.GetItem());
			
			ZERNShading_PointLight DestLight;
			ZEMatrix4x4::Transform(DestLight.PositionView, View.ViewTransform, PointLight.PositionWorld);
			DestLight.Range = PointLight.Range;
			DestLight.Color = PointLight.Color;
			DestLight.FalloffExponent = PointLight.FalloffExponent;

			ZEMatrix4x4::CreateOrientation(DestLight.WorldMatrix, PointLight.PositionWorld, ZEQuaternion::Identity, ZEVector3(PointLight.Range));

			PointLights.AddByRef(DestLight);
		}
		else if (ZEClass::IsDerivedFrom(ZERNCommandSpotLight::Class(), Command->GetClass()))
		{
			const ZERNCommandSpotLight& SpotLight = static_cast<const ZERNCommandSpotLight&>(Command.GetItem());

			ZERNShading_SpotLight DestLight;
			ZEMatrix4x4::Transform(DestLight.PositionView, View.ViewTransform, SpotLight.PositionWorld);
			ZEMatrix4x4::Transform3x3(DestLight.DirectionView, View.ViewTransform, SpotLight.RotationWorld * ZEVector3::NegUnitZ);
			DestLight.DirectionView.NormalizeSelf();
			DestLight.Range = SpotLight.Range;
			DestLight.Color = SpotLight.Color;
			DestLight.FalloffExponent = SpotLight.FalloffExponent;
			DestLight.CosInnerConeAngle = ZEAngle::Cos(SpotLight.InnerConeAngle * 0.5f);
			DestLight.CosOuterConeAngle = ZEAngle::Cos(SpotLight.OuterConeAngle * 0.5f);

			float ScaleXY = 2.0f * ZEAngle::Tan(SpotLight.OuterConeAngle * 0.5f) * SpotLight.Range;
			ZEMatrix4x4::CreateOrientation(
				DestLight.WorldMatrix, 
				SpotLight.PositionWorld, 
				SpotLight.RotationWorld, 
				ZEVector3(ScaleXY, ScaleXY, SpotLight.Range));

			//ZEVector3 Center = (SpotLight.ViewFrustum.GetVertex(ZE_FV_RIGHT_TOP_FAR) - SpotLight.ViewFrustum.GetVertex(ZE_FV_LEFT_BOTTOM_NEAR)) * 0.5f;
			//DestLight.CullRange = ZEVector3::Length(Center);
			//ZEMatrix4x4::Transform(DestLight.CullPositionView, View.ViewTransform, Center);
			
			if (Command->GetClass() == ZERNCommandSpotLightShadow::Class() && ShadowingEnabled && SpotLightsShadow.GetCount() < StageShadowing->GetSpotShadowMapCount())
			{
				const ZERNCommandSpotLightShadow& SpotLightShadow = static_cast<const ZERNCommandSpotLightShadow&>(Command.GetItem());

				ZERNShading_SpotLightShadow DestLightShadow;
				DestLightShadow.SpotLight = DestLight;
				DestLightShadow.ProjectionTransform = TextureTransform * SpotLightShadow.ViewProjectionTransform * View.InvViewTransform;
				DestLightShadow.ShadowSampleCount = ZELight::ConvertShadowSampleCount(SpotLightShadow.ShadowSampleCount);
				DestLightShadow.ShadowSampleLength = SpotLightShadow.ShadowSampleLength;
				DestLightShadow.ShadowDepthBias = SpotLightShadow.ShadowDepthBias;
				DestLightShadow.ShadowNormalBias = SpotLightShadow.ShadowNormalBias;

				SpotLightsShadow.AddByRef(DestLightShadow);
			}
			else
			{
				SpotLights.AddByRef(DestLight);
			}
		}
		else if (Command->GetClass() == ZERNCommandProjectiveLight::Class())
		{
			const ZERNCommandProjectiveLight& ProjectiveLight = static_cast<const ZERNCommandProjectiveLight&>(Command.GetItem());
			ZERNShading_ProjectiveLight DestLight;
			ZEMatrix4x4::Transform(DestLight.PositionView, View.ViewTransform, ProjectiveLight.PositionWorld);
			DestLight.Range = ProjectiveLight.Range;
			DestLight.Color = ProjectiveLight.Color;
			DestLight.FalloffExponent = ProjectiveLight.FalloffExponent;
			
			ZEVector3 A = ProjectiveLight.ViewFrustum.GetVertex(ZE_FV_RIGHT_TOP_FAR);
			ZEVector3 B = ProjectiveLight.ViewFrustum.GetVertex(ZE_FV_LEFT_TOP_FAR);
			ZEVector3 C = ProjectiveLight.ViewFrustum.GetVertex(ZE_FV_RIGHT_TOP_FAR);
			ZEVector3 D = ProjectiveLight.ViewFrustum.GetVertex(ZE_FV_RIGHT_BOTTOM_FAR);
			float ScaleX = ZEVector3::Length(A - B);
			float ScaleY = ZEVector3::Length(C - D);
			ZEMatrix4x4::CreateOrientation(
				DestLight.WorldMatrix, 
				ProjectiveLight.PositionWorld, 
				ProjectiveLight.RotationWorld, 
				ZEVector3(ScaleX, ScaleY, ProjectiveLight.Range));
			
			DestLight.ProjectionTransform = TextureTransform * ProjectiveLight.ViewProjectionTransform * View.InvViewTransform;
			DestLight.CastShadow = ProjectiveLight.CastShadow && ShadowingEnabled && ProjectiveShadowMapIndex < StageShadowing->GetProjectiveShadowMapCount();

			if (DestLight.CastShadow)
			{
				DestLight.ShadowMapIndex = ProjectiveShadowMapIndex++;
				DestLight.ShadowSampleCount = ProjectiveLight.ShadowSampleCount;
				DestLight.ShadowSampleLength = ProjectiveLight.ShadowSampleLength;
				DestLight.ShadowDepthBias = ProjectiveLight.ShadowDepthBias;
				DestLight.ShadowNormalBias = ProjectiveLight.ShadowNormalBias;
			}
			
			ProjectiveLights.AddByRef(DestLight);
			ProjectionTextures.Add(ProjectiveLight.ProjectionTexture);
		}
	}

	Constants.DirectionalLightCount = DirectionalLightCount;
	Constants.PointLightCount = (ZEUInt)PointLights.GetCount();
	Constants.SpotLightCount = (ZEUInt)SpotLights.GetCount();
	Constants.ProjectiveLightCount = (ZEUInt)ProjectiveLights.GetCount();
	
	Constants.AddTiledDeferredOutput = (UseTiledDeferred && Constants.PointLightCount > 0) ? ZEGR_TRUE : ZEGR_FALSE;
	Constants.TileCountX = ((ZEUInt)Viewport.GetWidth() + TILE_DIMENSION - 1) / TILE_DIMENSION;

	ConstantBuffer->SetData(&Constants);

	if (PointLights.GetCount() > 0)
	{
		ZERNShading_PointLight* DestPointLights;
		if (!PointLightStructuredBuffer->Map(ZEGR_RMT_WRITE_DISCARD, reinterpret_cast<void**>(&DestPointLights)))
			return false;
		
		memcpy(DestPointLights, PointLights.GetConstCArray(), ZEMath::Min((ZESize)MAX_TILED_LIGHT, PointLights.GetCount()) * sizeof(ZERNShading_PointLight));
		PointLightStructuredBuffer->Unmap();
	}

	if (SpotLights.GetCount() > 0)
	{
		ZERNShading_SpotLight* DestSpotLights;
		if (!SpotLightStructuredBuffer->Map(ZEGR_RMT_WRITE_DISCARD, reinterpret_cast<void**>(&DestSpotLights)))
			return false;
		
		memcpy(DestSpotLights, SpotLights.GetConstCArray(), ZEMath::Min((ZESize)MAX_TILED_LIGHT, SpotLights.GetCount()) * sizeof(ZERNShading_SpotLight));
		SpotLightStructuredBuffer->Unmap();
	}

	if (SpotLightsShadow.GetCount() > 0)
	{
		if (SpotLightShadowConstantBuffer == NULL)
			SpotLightShadowConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, MAX_SPOT_LIGHT_SHADOW * sizeof(ZERNShading_SpotLightShadow), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);
	
		ZERNShading_SpotLightShadow* DestSpotLightsShadow;
		if (!SpotLightShadowConstantBuffer->Map(ZEGR_RMT_WRITE_DISCARD, reinterpret_cast<void**>(&DestSpotLightsShadow)))
			return false;

		memcpy(DestSpotLightsShadow, SpotLightsShadow.GetConstCArray(), SpotLightsShadow.GetCount() * sizeof(ZERNShading_SpotLightShadow));
		SpotLightShadowConstantBuffer->Unmap();

	}

	return true;
}

bool ZERNStageLighting::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderState())
		return false;

	if (!UpdateLightBuffers())
		return false;

	return true;
}

bool ZERNStageLighting::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;

	CreateRandomVectors();
	CreateLightGeometries();

	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);
	PointLightStructuredBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_STRUCTURED_BUFFER, MAX_TILED_LIGHT * sizeof(ZERNShading_PointLight), sizeof(ZERNShading_PointLight), ZEGR_RU_DYNAMIC, ZEGR_RBF_SHADER_RESOURCE);
	SpotLightStructuredBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_STRUCTURED_BUFFER, MAX_TILED_LIGHT * sizeof(ZERNShading_SpotLight), sizeof(ZERNShading_SpotLight), ZEGR_RU_DYNAMIC, ZEGR_RBF_SHADER_RESOURCE);

	EdgeDetectionConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(EdgeDetectionConstants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

	return Update();
}

bool ZERNStageLighting::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	ScreenCoverVertexShader.Release();
	DeferredDirectionalLightVertexShader.Release();
	DeferredSpotLightVertexShader.Release();
	DeferredSpotLightShadowVertexShader.Release();
	DeferredPointLightVertexShader.Release();
	DeferredDirectionalLightPixelShader.Release();
	DeferredSpotLightPixelShader.Release();
	DeferredSpotLightShadowPixelShader.Release();
	DeferredPointLightPixelShader.Release();
	DeferredDirectionalLightPixelShaderPerSample.Release();
	DeferredSpotLightPixelShaderPerSample.Release();
	DeferredSpotLightShadowPixelShaderPerSample.Release();
	DeferredPointLightPixelShaderPerSample.Release();
	EdgeDetectionPixelShader.Release();
	TiledDeferredComputeShader.Release();

	DeferredDirectionalLightRenderState.Release();
	DeferredPointLightRenderState.Release();
	DeferredSpotLightRenderState.Release();
	DeferredSpotLightShadowRenderState.Release();
	DeferredDirectionalLightRenderStatePerSample.Release();
	DeferredPointLightRenderStatePerSample.Release();
	DeferredSpotLightRenderStatePerSample.Release();
	DeferredSpotLightShadowRenderStatePerSample.Release();
	EdgeDetectionRenderState.Release();
	TiledDeferredComputeRenderState.Release();

	ProjectiveLightVertexShader.Release();
	ProjectiveLightPixelShader.Release();
	ProjectiveLightPixelShaderPerSample.Release();
	ProjectiveLightRenderState.Release();
	ProjectiveLightRenderStatePerSample.Release();

	ConstantBuffer.Release();
	PointLightStructuredBuffer.Release();
	SpotLightStructuredBuffer.Release();
	DeferredLightVertexBuffer.Release();

	SpotLightShadowConstantBuffer.Release();
	ProjectiveLightConstantBuffer.Release();
	EdgeDetectionConstantBuffer.Release();

	RandomVectorsTexture.Release();
	TiledDeferredOutputTexture.Release();

	AccumulationTexture = NULL;
	GBufferEmissive = NULL;
	GBufferDiffuse = NULL;
	GBufferNormal = NULL;
	DepthTexture = NULL;
	DirectionalShadowMaps = NULL;
	SpotShadowMaps = NULL;
	ProjectiveShadowMaps = NULL;

	return ZERNStage::DeinitializeInternal();
}

void ZERNStageLighting::DrawLights(ZEGRContext* Context, bool PerSample)
{
	if (!UseTiledDeferred)
	{
		if (Constants.PointLightCount > 0)
		{
			Context->SetRenderState(PerSample ? DeferredPointLightRenderStatePerSample : DeferredPointLightRenderState);
			Context->DrawInstanced(3600, 942, Constants.PointLightCount, 0);
		}
	}

	if (Constants.SpotLightCount > 0)
	{
		Context->SetRenderState(PerSample ? DeferredSpotLightRenderStatePerSample : DeferredSpotLightRenderState);
		Context->DrawInstanced(18, 4542, Constants.SpotLightCount, 0);
	}
	
	if (SpotLightsShadow.GetCount() > 0)
	{
		Context->SetConstantBuffer(ZEGR_ST_VERTEX, 9, SpotLightShadowConstantBuffer);
		Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, SpotLightShadowConstantBuffer);
		Context->SetRenderState(PerSample ? DeferredSpotLightShadowRenderStatePerSample : DeferredSpotLightShadowRenderState);
		Context->DrawInstanced(18, 4542, (ZEUInt)SpotLightsShadow.GetCount(), 0);
	}

	if (ProjectiveLights.GetCount() > 0)
	{
		if (ProjectiveLightConstantBuffer == NULL)
			ProjectiveLightConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(ZERNShading_ProjectiveLight), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

		Context->SetConstantBuffer(ZEGR_ST_VERTEX, 9, ProjectiveLightConstantBuffer);
		Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, ProjectiveLightConstantBuffer);
		Context->SetRenderState(PerSample ? ProjectiveLightRenderStatePerSample : ProjectiveLightRenderState);
		
		ze_for_each(ProjectiveLight, ProjectiveLights)
		{
			ProjectiveLightConstantBuffer->SetData(&ProjectiveLight.GetItem());
			Context->SetTexture(ZEGR_ST_PIXEL, 10, ProjectionTextures[ProjectiveLight.GetIndex()]);
			Context->Draw(18, 4542);
		}
	}

	if (Constants.DirectionalLightCount > 0)
	{
		Context->SetRenderState(PerSample ? DeferredDirectionalLightRenderStatePerSample: DeferredDirectionalLightRenderState);
		Context->Draw(6, 0);
	}
}

void ZERNStageLighting::BlendTiledDeferred(ZEGRContext* Context, const ZEGRRenderTarget* RenderTarget, bool PerSample)
{
	if (UseTiledDeferred && Constants.DirectionalLightCount == 0)
	{
		Context->SetRenderState(PerSample ? BlendRenderStatePerSample : BlendRenderState);
		Context->SetRenderTargets(1, &RenderTarget, DepthTexture->GetDepthStencilBuffer(true));
		Context->Draw(3, 0);
	}
}

ZEInt ZERNStageLighting::GetId() const
{
	return ZERN_STAGE_LIGHTING;
}

const ZEString& ZERNStageLighting::GetName() const
{
	static const ZEString Name = "Stage Lighting";
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

void ZERNStageLighting::SetUseTiledDeferred(bool UseTiledDeferred)
{
	this->UseTiledDeferred = UseTiledDeferred;
}

bool ZERNStageLighting::GetUseTiledDeferred() const
{
	return UseTiledDeferred;
}

void ZERNStageLighting::SetMSAADepthThreshold(float MSAADepthThreshold)
{
	if (EdgeDetectionConstants.DepthThreshold == MSAADepthThreshold)
		return;

	EdgeDetectionConstants.DepthThreshold = MSAADepthThreshold;

	DirtyFlags.RaiseFlags(ZERN_SLDF_EDGEDETECTION_CONSTANT_BUFFER);
}

float ZERNStageLighting::GetMSAADepthThreshold() const
{
	return EdgeDetectionConstants.DepthThreshold;
}

void ZERNStageLighting::SetMSAANormalThreshold(float MSAANormalThreshold)
{
	if (EdgeDetectionConstants.NormalThreshold == MSAANormalThreshold)
		return;

	EdgeDetectionConstants.NormalThreshold = MSAANormalThreshold;

	DirtyFlags.RaiseFlags(ZERN_SLDF_EDGEDETECTION_CONSTANT_BUFFER);
}

float ZERNStageLighting::GetMSAANormalThreshold() const
{
	return EdgeDetectionConstants.NormalThreshold;
}

void ZERNStageLighting::Resized(ZEUInt Width, ZEUInt Height)
{
	if (ZEGRGraphicsModule::SAMPLE_COUNT == 4)
	{
		Width *= 2;
		Height *= 2;
	}

	TiledDeferredOutputTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, ZEGR_TF_R11G11B10_FLOAT, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_UNORDERED_ACCESS);
}

bool ZERNStageLighting::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!Update())
		return false;

	if (GetCommands().GetCount() == 0)
		return false;

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, 8, ConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, ConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_COMPUTE, 8, ConstantBuffer);

	const ZEGRBuffer* Buffers[] = {PointLightStructuredBuffer, SpotLightStructuredBuffer};
	Context->SetBuffers(ZEGR_ST_VERTEX, 15, 2, Buffers);
	Context->SetBuffers(ZEGR_ST_PIXEL, 15, 2, Buffers);
	Context->SetBuffers(ZEGR_ST_COMPUTE, 15, 2, Buffers);

	const ZEGRTexture* GBuffers[] = {GBufferEmissive, GBufferDiffuse, GBufferNormal, DepthTexture};
	Context->SetTextures(ZEGR_ST_PIXEL, 1, 4, GBuffers);

	const ZEGRTexture* InputTextures[] = {DirectionalShadowMaps, SpotShadowMaps, ProjectiveShadowMaps, RandomVectorsTexture};
	Context->SetTextures(ZEGR_ST_PIXEL, 11, 4, InputTextures);

	ZERNStage* StageGBuffer = GetRenderer()->GetStage(ZERN_STAGE_GBUFFER);
	if (StageGBuffer == NULL || !StageGBuffer->GetEnabled() || StageGBuffer->GetCommands().GetCount() == 0)
		return false;

	if (UseTiledDeferred && Constants.PointLightCount > 0)
	{
		Context->ClearUnorderedAccessView(TiledDeferredOutputTexture, ZEVector4::Zero);

		Context->SetComputeRenderState(TiledDeferredComputeRenderState);
		Context->SetRWTexture(0, TiledDeferredOutputTexture);
		Context->SetTextures(ZEGR_ST_COMPUTE, 1, 4, GBuffers);

		ZEUInt TileCountX = (AccumulationTexture->GetWidth() + TILE_DIMENSION - 1) / TILE_DIMENSION;
		ZEUInt TileCountY = (AccumulationTexture->GetHeight() + TILE_DIMENSION - 1) / TILE_DIMENSION;

		Context->Dispatch(TileCountX, TileCountY, 1);

		Context->SetTexture(ZEGR_ST_PIXEL, 5, TiledDeferredOutputTexture);
	}

	Viewport.SetWidth((float)AccumulationTexture->GetWidth());
	Viewport.SetHeight((float)AccumulationTexture->GetHeight());

	Context->SetViewports(1, &Viewport);

	if (ZEGRGraphicsModule::SAMPLE_COUNT > 1)
	{
		if (DirtyFlags.GetFlags(ZERN_SLDF_EDGEDETECTION_CONSTANT_BUFFER))
		{
			EdgeDetectionConstantBuffer->SetData(&EdgeDetectionConstants);
			DirtyFlags.UnraiseFlags(ZERN_SLDF_EDGEDETECTION_CONSTANT_BUFFER);
		}

		Context->SetRenderState(EdgeDetectionRenderState);
		Context->SetRenderTargets(0, NULL, DepthTexture->GetDepthStencilBuffer(true));
		Context->SetStencilRef(1);
		Context->Draw(3, 0);
	}

	const ZEGRRenderTarget* RenderTarget = AccumulationTexture->GetRenderTarget();
	Context->SetRenderTargets(1, &RenderTarget, DepthTexture->GetDepthStencilBuffer(true));
	Context->SetVertexBuffer(0, DeferredLightVertexBuffer);

	Context->SetStencilRef(0);
	BlendTiledDeferred(Context, RenderTarget, false);
	DrawLights(Context, false);

	if (ZEGRGraphicsModule::SAMPLE_COUNT > 1)
	{
		Context->SetStencilRef(1);
		BlendTiledDeferred(Context, RenderTarget, true);
		DrawLights(Context, true);
	}

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
	UseTiledDeferred = false;

	memset(&Constants, 0, sizeof(Constants));

	EdgeDetectionConstants.DepthThreshold = 1.0f;
	EdgeDetectionConstants.NormalThreshold = 0.9f;

	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&GBufferEmissive), "GBuffer1", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&GBufferDiffuse), "GBuffer2", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&GBufferNormal), "GBuffer3", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&DepthTexture), "DepthTexture", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&DirectionalShadowMaps), "DirectionalShadowMaps", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&SpotShadowMaps), "SpotShadowMaps", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&ProjectiveShadowMaps), "ProjectiveShadowMaps", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&AccumulationTexture), "ColorTexture", ZERN_SRUT_WRITE, ZERN_SRCF_GET_FROM_PREV);
}

ZERNStageLighting::~ZERNStageLighting()
{
	Deinitialize();
}
