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
#define ZERN_SLDF_OUTPUT						8

static ZEGRVertexLayout GetVertexLayoutPosition()
{
	ZEGRVertexLayout VertexLayout;
	ZEGRVertexElement ElementArray[] = 
	{
		{ZEGR_VES_POSITION,	0, ZEGR_VET_FLOAT_3, 0, 0,  ZEGR_VU_PER_VERTEX, 0},
	};
	
	VertexLayout.SetElements(ElementArray, 1);

	return VertexLayout;
}

void ZERNStageLighting::CreateRandomVectors()
{
	const ZEUInt Length = 16;
	ZEUInt Size = Length * Length * 2;
	ZEArray<ZEUInt8> RandomVectors;
	RandomVectors.SetCount(Size);
	for (ZEUInt I = 0; I < Size; I += 2)
	{
		RandomVectors[I]		= (ZEUInt8)(ZERandom::GetFloatPositive() * 255.0f + 0.5f);
		RandomVectors[I + 1]	= (ZEUInt8)(ZERandom::GetFloatPositive() * 255.0f + 0.5f);
	}
	
	RandomVectorsTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Length, Length, 1, ZEGR_TF_R8G8_UNORM, ZEGR_RU_IMMUTABLE, ZEGR_RBF_SHADER_RESOURCE, 1, 1, RandomVectors.GetConstCArray());
}

void ZERNStageLighting::CreateLightGeometries()
{
	/*
	Primitive	Start		Start(Byte)	Vertex Count	Prim. Count		Size (Bytes)
	Sphere12	0			0			936				312				44928
	Sphere24	936			44928		3600			1200			172800
	Cone12		4536		217728		18				6				864
	End			4644				
	*/

	ZEGRCanvas Canvas;
	
	// Point/OmniProjective
	Canvas.AddSphere(1.0f, 12, 12);
	Canvas.AddSphere(1.0f, 24, 24);

	// Spot/Projective
	Canvas.SetRotation(ZEQuaternion(-ZE_PI_2, ZEVector3::UnitX));
	Canvas.SetTranslation(ZEVector3::UnitZ);
	Canvas.AddPyramid(1.0f, 1.0f, 1.0f);

	ZEArray<ZEVector3> Vertices;
	Vertices.SetCount(Canvas.GetVertexCount());
	for (ZESize I = 0; I < Canvas.GetVertexCount(); I++)
		Vertices[I] = Canvas.GetVertex(I).Position;

	DeferredLightsVertexBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_VERTEX_BUFFER, Vertices.GetCount() * sizeof(ZEVector3), sizeof(ZEVector3), ZEGR_RU_IMMUTABLE, ZEGR_RBF_VERTEX_BUFFER, ZEGR_TF_NONE, Vertices.GetConstCArray());
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
	
	DeferredLightsVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(DeferredLightsVertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNDeferredShading_PixelShader_LightingStage";

	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_SPOT_LIGHT"));
	DeferredSpotLightPixelShader= ZEGRShader::Compile(Options);
	zeCheckError(DeferredSpotLightPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.GetLastItem() = ZEGRShaderDefinition("ZERN_RENDER_POINT_LIGHT");
	DeferredPointLightPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(DeferredPointLightPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.GetLastItem() = ZEGRShaderDefinition("ZERN_RENDER_PROJECTIVE_LIGHT");
	ProjectiveLightPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(ProjectiveLightPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.GetLastItem() = ZEGRShaderDefinition("ZERN_TILED_DEFERRED");
	TiledDeferredPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(TiledDeferredPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.EntryPoint = "ZERNDeferredShading_PixelShader_PerSample_LightingStage";

	Options.Definitions.GetLastItem() = ZEGRShaderDefinition("ZERN_RENDER_SPOT_LIGHT");
	DeferredSpotLightPixelShaderPerSample = ZEGRShader::Compile(Options);
	zeCheckError(DeferredSpotLightPixelShaderPerSample == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.GetLastItem() = ZEGRShaderDefinition("ZERN_RENDER_POINT_LIGHT");
	DeferredPointLightPixelShaderPerSample = ZEGRShader::Compile(Options);
	zeCheckError(DeferredPointLightPixelShaderPerSample == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.GetLastItem() = ZEGRShaderDefinition("ZERN_RENDER_PROJECTIVE_LIGHT");
	ProjectiveLightPixelShaderPerSample = ZEGRShader::Compile(Options);
	zeCheckError(ProjectiveLightPixelShaderPerSample == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.GetLastItem() = ZEGRShaderDefinition("ZERN_TILED_DEFERRED");
	TiledDeferredPixelShaderPerSample = ZEGRShader::Compile(Options);
	zeCheckError(TiledDeferredPixelShaderPerSample == NULL, false, "Cannot set pixel shader.");

	Options.EntryPoint = "ZERNDeferredShading_EdgeDetection_PixelShader_Main";
	EdgeDetectionPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(EdgeDetectionPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.EntryPoint = "ZERNDeferredShading_DebugEdgeDetection_PixelShader_Main";
	DebugEdgeDetectionPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(DebugEdgeDetectionPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNTiledDeferredShadingCompute.hlsl";
	Options.Type = ZEGR_ST_COMPUTE;
	Options.EntryPoint = "ZERNTiledDeferredShadingCompute_ComputeShader_Main";

	TiledDeferredComputeShader = ZEGRShader::Compile(Options);
	zeCheckError(TiledDeferredComputeShader == NULL, false, "Cannot set compute shader.");

	Options.Definitions.GetLastItem() = ZEGRShaderDefinition("ZERN_TILED_TRANSPARENT");
	TiledDeferredTransparencyComputeShader = ZEGRShader::Compile(Options);
	zeCheckError(TiledDeferredTransparencyComputeShader == NULL, false, "Cannot set compute shader.");

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
	GraphicsRenderState.SetVertexLayout(GetVertexLayoutPosition());

	GraphicsRenderState.SetShader(ZEGR_ST_VERTEX, DeferredLightsVertexShader);

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, ProjectiveLightPixelShader);

	ProjectiveLightRenderState = GraphicsRenderState.Compile();
	zeCheckError(ProjectiveLightRenderState == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, ProjectiveLightPixelShaderPerSample);

	ProjectiveLightRenderStatePerSample = GraphicsRenderState.Compile();
	zeCheckError(ProjectiveLightRenderStatePerSample == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DeferredSpotLightPixelShader);

	DeferredSpotLightRenderState = GraphicsRenderState.Compile();
	zeCheckError(DeferredSpotLightRenderState == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DeferredSpotLightPixelShaderPerSample);

	DeferredSpotLightRenderStatePerSample = GraphicsRenderState.Compile();
	zeCheckError(DeferredSpotLightRenderStatePerSample == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DeferredPointLightPixelShader);

	DeferredPointLightRenderState = GraphicsRenderState.Compile();
	zeCheckError(DeferredPointLightRenderState == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DeferredPointLightPixelShaderPerSample);

	DeferredPointLightRenderStatePerSample = GraphicsRenderState.Compile();
	zeCheckError(DeferredPointLightRenderStatePerSample == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetRasterizerState(ZEGRRasterizerState());
	GraphicsRenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, TiledDeferredPixelShader);
	TiledDeferredGraphicsRenderState = GraphicsRenderState.Compile();
	zeCheckError(TiledDeferredGraphicsRenderState == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, TiledDeferredPixelShaderPerSample);

	TiledDeferredGraphicsRenderStatePerSample = GraphicsRenderState.Compile();
	zeCheckError(TiledDeferredGraphicsRenderStatePerSample == NULL, false, "Cannot set render state.");

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

	GraphicsRenderState.SetBlendState(BlendStateAdditive);
	GraphicsRenderState.SetDepthStencilState(DepthStencilStateTestNoWriteLessTestStencil);

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DebugEdgeDetectionPixelShader);

	DebugEdgeDetectionRenderState = GraphicsRenderState.Compile();
	zeCheckError(DebugEdgeDetectionRenderState == NULL, false, "Cannot set render state.");

	ZEGRComputeRenderState ComputeRenderState;
	ComputeRenderState.SetComputeShader(TiledDeferredComputeShader);
	TiledDeferredComputeRenderState = ComputeRenderState.Compile();
	zeCheckError(TiledDeferredComputeRenderState == NULL, false, "Cannot set render state.");

	ComputeRenderState.SetComputeShader(TiledDeferredTransparencyComputeShader);
	TiledDeferredTransparencyComputeRenderState = ComputeRenderState.Compile();
	zeCheckError(TiledDeferredTransparencyComputeRenderState == NULL, false, "Cannot set render state.");

	DirtyFlags.UnraiseFlags(ZERN_SLDF_RENDER_STATE);

	return true;
}

bool ZERNStageLighting::UpdateBuffers()
{
	if (DirtyFlags.GetFlags(ZERN_SLDF_EDGEDETECTION_CONSTANT_BUFFER))
	{
		if (EdgeDetectionConstantBuffer == NULL)
			EdgeDetectionConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(EdgeDetectionConstants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

		EdgeDetectionConstantBuffer->SetData(&EdgeDetectionConstants);
		DirtyFlags.UnraiseFlags(ZERN_SLDF_EDGEDETECTION_CONSTANT_BUFFER);
	}

	return true;
}

bool ZERNStageLighting::SetupLights(ZEGRContext* Context)
{
	const ZERNView& View = GetRenderer()->GetView();

	ZERNStageShadowing* StageShadowing = static_cast<ZERNStageShadowing*>(GetRenderer()->GetStage(ZERN_STAGE_SHADOWING));
	bool ShadowingEnabled = (StageShadowing != NULL && StageShadowing->GetEnabled());

	ZEUInt DirectionalLightCount = 0;
	ZEUInt DirectionalShadowMapIndex = 0;
	ZEUInt ProjectiveShadowMapIndex = 0;
	ZEUInt SpotShadowMapIndex = 0;

	ZEMatrix4x4 TextureTransform;
	ZEMatrix4x4::Create(TextureTransform,
		0.5f, 0.0f, 0.0f, 0.5f,
		0.0f, -0.5f, 0.0f, 0.5f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

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

			ZEMatrix4x4 WorldTransform;
			ZEMatrix4x4::CreateOrientation(WorldTransform, PointLight.PositionWorld, ZEQuaternion::Identity, ZEVector3(PointLight.Range));

			PointLights.AddByRef(DestLight);
			PointLightsTransforms.AddByRef(WorldTransform);
			PointLightsBoundingSpheres.AddByRef(ZEBSphere(DestLight.PositionView, DestLight.Range));
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

			ZEMatrix4x4 WorldTransform;
			float ScaleXY = 2.0f * ZEAngle::Tan(SpotLight.OuterConeAngle * 0.5f) * SpotLight.Range;
			ZEMatrix4x4::CreateOrientation(
				WorldTransform, 
				SpotLight.PositionWorld, 
				SpotLight.RotationWorld, 
				ZEVector3(ScaleXY, ScaleXY, SpotLight.Range));

			SpotLightsTransforms.AddByRef(WorldTransform);
			
			ZEBSphere BoundingSphere;
			BoundingSphere.Radius = DestLight.Range * 0.5f;
			BoundingSphere.Position = DestLight.PositionView - DestLight.DirectionView * BoundingSphere.Radius;

			SpotLightsBoundingSpheres.AddByRef(BoundingSphere);
			DestLight.CastShadow = ZEGR_FALSE;

			if (Command->GetClass() == ZERNCommandSpotLightShadow::Class() && ShadowingEnabled && SpotLightsShadow.GetCount() < StageShadowing->GetSpotShadowMapCount())
			{
				DestLight.CastShadow = ZEGR_TRUE;
				DestLight.ShadowIndex = SpotShadowMapIndex++;
				
				const ZERNCommandSpotLightShadow& SpotLightShadow = static_cast<const ZERNCommandSpotLightShadow&>(Command.GetItem());

				ZERNShading_SpotLightShadow DestLightShadow;
				DestLightShadow.ViewTransform = SpotLightShadow.ViewTransform * View.InvViewTransform;
				DestLightShadow.ProjectionTransform = TextureTransform * SpotLightShadow.ProjectionTransform;
				DestLightShadow.ShadowSampleCount = ZELight::ConvertShadowSampleCount(SpotLightShadow.ShadowSampleCount);
				DestLightShadow.ShadowSampleLength = SpotLightShadow.ShadowSampleLength;
				DestLightShadow.ShadowDepthBias = SpotLightShadow.ShadowDepthBias;
				DestLightShadow.ShadowNormalBias = SpotLightShadow.ShadowNormalBias;

				SpotLightsShadow.AddByRef(DestLightShadow);
			}
			
			SpotLights.AddByRef(DestLight);
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
			ZEMatrix4x4 WorldTransform;
			ZEMatrix4x4::CreateOrientation(
				WorldTransform, 
				ProjectiveLight.PositionWorld, 
				ProjectiveLight.RotationWorld, 
				ZEVector3(ScaleX, ScaleY, ProjectiveLight.Range));

			ProjectiveLightsTransforms.AddByRef(WorldTransform);

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
	
	Constants.TileCountX = ((ZEUInt)Viewport.GetWidth() + TILE_LIGHT_DIMENSION - 1) / TILE_LIGHT_DIMENSION;

	ZEUInt ConstantCount = 0;
	ZEBYTE* DestBuffer;
	LightsConstantBuffer->Map(ZEGR_RMT_WRITE_DISCARD, reinterpret_cast<void**>(&DestBuffer));

	ZESize CommonParamsStride = (sizeof(Constants) + 255) & ~255;
	CommonParamsRange.Offset = ConstantCount;
	CommonParamsRange.Count = CommonParamsStride / 16;

	memcpy(DestBuffer, &Constants, sizeof(Constants));
	ConstantCount += CommonParamsRange.Count;
	DestBuffer += CommonParamsStride;

	ZESize PointLightsStride = (Constants.PointLightCount * sizeof(ZERNShading_PointLight) + 255) & ~255;
	PointLightRange.Offset = ConstantCount;
	PointLightRange.Count = PointLightsStride / 16;

	memcpy(DestBuffer, PointLights.GetConstCArray(), PointLights.GetCount() * sizeof(ZERNShading_PointLight));
	ConstantCount += PointLightRange.Count;
	DestBuffer += PointLightsStride;

	ZESize SpotLightsStride = (Constants.SpotLightCount * sizeof(ZERNShading_SpotLight) + 255) & ~255;
	SpotLightRange.Offset = ConstantCount;
	SpotLightRange.Count = SpotLightsStride / 16;

	memcpy(DestBuffer, SpotLights.GetConstCArray(), SpotLights.GetCount() * sizeof(ZERNShading_SpotLight));
	ConstantCount += SpotLightRange.Count;
	DestBuffer += SpotLightsStride;

	ZESize SpotLightsShadowStride = (SpotLightsShadow.GetCount() * sizeof(ZERNShading_SpotLightShadow) + 255) & ~255;
	SpotLightShadowRange.Offset = ConstantCount;
	SpotLightShadowRange.Count = SpotLightsShadowStride / 16;

	memcpy(DestBuffer, SpotLightsShadow.GetConstCArray(), SpotLightsShadow.GetCount() * sizeof(ZERNShading_SpotLightShadow));
	ConstantCount += SpotLightShadowRange.Count;
	DestBuffer += SpotLightsShadowStride;

	ZESize ProjectiveLightStride = (sizeof(ZERNShading_ProjectiveLight) + 255) & ~255;
	ProjectiveLightRange.Offset = ConstantCount;
	ProjectiveLightRange.Count = ProjectiveLightStride / 16;

	ze_for_each(ProjectiveLight, ProjectiveLights)
	{
		memcpy(DestBuffer, &ProjectiveLight.GetItem(), sizeof(ZERNShading_ProjectiveLight));
		ConstantCount += ProjectiveLightRange.Count;
		DestBuffer += ProjectiveLightStride;
	}

	LightsConstantBuffer->Unmap();

	if ((!UseTiledDeferred && Constants.PointLightCount > 0) || Constants.SpotLightCount > 0 || Constants.ProjectiveLightCount > 0)
	{
		if (DeferredLightsTransformsConstantBuffer == NULL)
		{
			ZESize Size = sizeof(ZEMatrix4x4) * (MAX_DEFERRED_LIGHT * 2) + ((sizeof(ZEMatrix4x4) + 255) & ~255) * MAX_PROJECTIVE_LIGHT;
			DeferredLightsTransformsConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, Size, 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);
		}

		ZEUInt ConstantCount = 0;
		ZEBYTE* DestBuffer;
		DeferredLightsTransformsConstantBuffer->Map(ZEGR_RMT_WRITE_DISCARD, reinterpret_cast<void**>(&DestBuffer));

		ZESize PointLightsStride = ((Constants.PointLightCount * sizeof(ZEMatrix4x4)) + 255) & ~255;
		PointLightVertexRange.Offset = ConstantCount;
		PointLightVertexRange.Count = PointLightsStride / 16;

		memcpy(DestBuffer, PointLightsTransforms.GetConstCArray(), PointLightsTransforms.GetCount() * sizeof(ZEMatrix4x4));
		ConstantCount += PointLightVertexRange.Count;
		DestBuffer += PointLightsStride;

		ZESize SpotLightsStride = ((Constants.SpotLightCount * sizeof(ZEMatrix4x4)) + 255) & ~255;
		SpotLightVertexRange.Offset = ConstantCount;
		SpotLightVertexRange.Count = SpotLightsStride / 16;

		memcpy(DestBuffer, SpotLightsTransforms.GetConstCArray(), SpotLightsTransforms.GetCount() * sizeof(ZEMatrix4x4));
		ConstantCount += SpotLightVertexRange.Count;
		DestBuffer += SpotLightsStride;

		ZESize ProjectiveLightsStride = (sizeof(ZEMatrix4x4) + 255) & ~255;
		ProjectiveLightVertexRange.Offset = ConstantCount;
		ProjectiveLightVertexRange.Count = ProjectiveLightsStride / 16;

		ze_for_each(Transform, ProjectiveLightsTransforms)
		{
			memcpy(DestBuffer, &Transform.GetItem(), sizeof(ZEMatrix4x4));
			ConstantCount += ProjectiveLightVertexRange.Count;
			DestBuffer += ProjectiveLightsStride;
		}

		DeferredLightsTransformsConstantBuffer->Unmap();
	}
	
	return true;
}

bool ZERNStageLighting::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderState())
		return false;

	if (!UpdateBuffers())
		return false;

	return true;
}

bool ZERNStageLighting::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;

	CreateRandomVectors();
	CreateLightGeometries();

	return Update();
}

bool ZERNStageLighting::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	ScreenCoverVertexShader.Release();
	DeferredLightsVertexShader.Release();
	DeferredSpotLightPixelShader.Release();
	DeferredPointLightPixelShader.Release();
	DeferredSpotLightPixelShaderPerSample.Release();
	DeferredPointLightPixelShaderPerSample.Release();
	EdgeDetectionPixelShader.Release();
	TiledDeferredComputeShader.Release();
	TiledDeferredTransparencyComputeShader.Release();
	TiledDeferredPixelShader.Release();
	TiledDeferredPixelShaderPerSample.Release();

	DeferredPointLightRenderState.Release();
	DeferredSpotLightRenderState.Release();
	DeferredPointLightRenderStatePerSample.Release();
	DeferredSpotLightRenderStatePerSample.Release();
	EdgeDetectionRenderState.Release();
	DebugEdgeDetectionRenderState.Release();
	TiledDeferredComputeRenderState.Release();
	TiledDeferredTransparencyComputeRenderState.Release();
	TiledDeferredGraphicsRenderState.Release();
	TiledDeferredGraphicsRenderStatePerSample.Release();
	
	ProjectiveLightPixelShader.Release();
	ProjectiveLightPixelShaderPerSample.Release();
	ProjectiveLightRenderState.Release();
	ProjectiveLightRenderStatePerSample.Release();

	DeferredLightsVertexBuffer.Release();
	DeferredLightsTransformsConstantBuffer.Release();
	
	PointLightsBoundingSpheresStructuredBuffer.Release();
	SpotLightsBoundingSpheresStructuredBuffer.Release();
	
	EdgeDetectionConstantBuffer.Release();

	RandomVectorsTexture.Release();
	
	LightsConstantBuffer.Release();
	TileLightStructuredBuffer.Release();
	
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
	if ((!UseTiledDeferred && Constants.PointLightCount > 0) || Constants.SpotLightCount > 0 || Constants.ProjectiveLightCount > 0)
	{
		if (Constants.PointLightCount > 0)
		{
			Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, LightsConstantBuffer, &PointLightRange.Offset, &PointLightRange.Count);
			Context->SetConstantBuffer(ZEGR_ST_VERTEX, 11, DeferredLightsTransformsConstantBuffer, &PointLightVertexRange.Offset, &PointLightVertexRange.Count);
			Context->SetRenderState(PerSample ? DeferredPointLightRenderStatePerSample : DeferredPointLightRenderState);
			Context->DrawInstanced(3600, 936, Constants.PointLightCount, 0);
		}

		if (Constants.SpotLightCount > 0)
		{
			Context->SetConstantBuffer(ZEGR_ST_PIXEL, 10, LightsConstantBuffer, &SpotLightRange.Offset, &SpotLightRange.Count);
			Context->SetConstantBuffer(ZEGR_ST_PIXEL, 11, LightsConstantBuffer, &SpotLightShadowRange.Offset, &SpotLightShadowRange.Count);
			Context->SetConstantBuffer(ZEGR_ST_VERTEX, 11, DeferredLightsTransformsConstantBuffer, &SpotLightVertexRange.Offset, &SpotLightVertexRange.Count);
			Context->SetRenderState(PerSample ? DeferredSpotLightRenderStatePerSample : DeferredSpotLightRenderState);
			Context->DrawInstanced(18, 4536, Constants.SpotLightCount, 0);
		}
	
		if (Constants.ProjectiveLightCount > 0)
		{
			Context->SetRenderState(PerSample ? ProjectiveLightRenderStatePerSample : ProjectiveLightRenderState);

			ZEUInt Offset = ProjectiveLightRange.Offset;
			ZEUInt Count = ProjectiveLightRange.Count;

			ZEUInt OffsetTransform = ProjectiveLightVertexRange.Offset;
			ZEUInt CountTransform = ProjectiveLightVertexRange.Count;

			ze_for_each(ProjectiveLight, ProjectiveLights)
			{
				Context->SetConstantBuffer(ZEGR_ST_VERTEX, 11, DeferredLightsTransformsConstantBuffer, &OffsetTransform, &CountTransform);
				Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, LightsConstantBuffer, &Offset, &Count);
				Context->SetTexture(ZEGR_ST_PIXEL, 10, ProjectionTextures[ProjectiveLight.GetIndex()]);
				Context->DrawInstanced(18, 4536, 1, 0);

				Offset += Count;
				OffsetTransform += CountTransform;
			}
		}
	}

	if ((UseTiledDeferred && Constants.PointLightCount > 0) || Constants.DirectionalLightCount > 0)
	{
		Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, LightsConstantBuffer, &PointLightRange.Offset, &PointLightRange.Count);
		Context->SetRenderState(PerSample ? TiledDeferredGraphicsRenderStatePerSample : TiledDeferredGraphicsRenderState);
		Context->Draw(3, 0);
	}
}

void ZERNStageLighting::CreateOutput(const ZEString& Name)
{
	if (Name == "TileLightBuffer")
	{
		if (DirtyFlags.GetFlags(ZERN_SLDF_OUTPUT))
		{
			ZEUInt Width = GetRenderer()->GetOutputTexture()->GetWidth();
			ZEUInt Height = GetRenderer()->GetOutputTexture()->GetHeight();

			ZEUInt TileCountX = (Width + TILE_LIGHT_DIMENSION - 1) / TILE_LIGHT_DIMENSION;
			ZEUInt TileCountY = (Height + TILE_LIGHT_DIMENSION - 1) / TILE_LIGHT_DIMENSION;

			TileLightStructuredBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_STRUCTURED_BUFFER, TileCountX * TileCountY * TILE_LIGHT_TOTAL_COUNT * sizeof(ZEUInt), sizeof(ZEUInt), ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_UNORDERED_ACCESS);
			
			DirtyFlags.UnraiseFlags(ZERN_SLDF_OUTPUT);
		}
	}
	else if (Name == "LightsConstantBuffer")
	{
		if (LightsConstantBuffer == NULL)
		{
			const ZESize MaxAlignedCommonParamsSize = (sizeof(Constants) + 255) & ~255;
			const ZESize MaxAlignedPointLightSize = (sizeof(ZERNShading_PointLight) * MAX_TILED_LIGHT + 255) & ~255;
			const ZESize MaxAlignedSpotLightSize = (sizeof(ZERNShading_SpotLight) * MAX_TILED_LIGHT + 255) & ~255;
			const ZESize MaxAlignedSpotShadowParamsSize = (sizeof(ZERNShading_SpotLightShadow) * MAX_SPOT_LIGHT_SHADOW + 255) & ~255;
			const ZESize MaxAlignedProjectiveLightSize = (sizeof(ZERNShading_ProjectiveLight) * MAX_PROJECTIVE_LIGHT + 255) & ~255;

			ZESize TotalAlignedSize = MaxAlignedPointLightSize + MaxAlignedSpotLightSize + MaxAlignedSpotShadowParamsSize + MaxAlignedProjectiveLightSize;
			LightsConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, TotalAlignedSize, 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);
		}
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

void ZERNStageLighting::SetShowMSAAEdges(bool ShowMSAAEdges)
{
	this->ShowMSAAEdges = ShowMSAAEdges;
}

bool ZERNStageLighting::GetShowMSAAEdges() const
{
	return ShowMSAAEdges;
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
	DirtyFlags.RaiseFlags(ZERN_SLDF_OUTPUT);
}

bool ZERNStageLighting::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!Update())
		return false;

	if (!SetupLights(Context))
		return false;

	ZERNStage* StageForwardTransparent = GetRenderer()->GetStage(ZERN_STAGE_FORWARD_TRANSPARENT);
	ZERNStage* StageParticleRendering = GetRenderer()->GetStage(ZERN_STAGE_PARTICLE_RENDERING);	
	bool ForwardTransparencyEnabled = (StageForwardTransparent != NULL && StageForwardTransparent->GetEnabled() && StageForwardTransparent->GetCommands().GetCount() > 0);
	bool ParticleRenderingEnabled = (StageParticleRendering != NULL && StageParticleRendering->GetEnabled() && StageParticleRendering->GetCommands().GetCount() > 0);

	bool TransparencyEnabled = ForwardTransparencyEnabled || ParticleRenderingEnabled;
	
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, LightsConstantBuffer, &CommonParamsRange.Offset, &CommonParamsRange.Count);
	Context->SetConstantBuffer(ZEGR_ST_COMPUTE, 8, LightsConstantBuffer, &CommonParamsRange.Offset, &CommonParamsRange.Count);
	
	const ZEGRTexture* GBuffers[] = {GBufferEmissive, GBufferDiffuse, GBufferNormal, DepthTexture};
	Context->SetTextures(ZEGR_ST_PIXEL, 1, 4, GBuffers);
	Context->SetTextures(ZEGR_ST_COMPUTE, 1, 4, GBuffers);
	
	const ZEGRTexture* ShadowTextures[] = {DirectionalShadowMaps, SpotShadowMaps, ProjectiveShadowMaps, RandomVectorsTexture};
	Context->SetTextures(ZEGR_ST_PIXEL, 11, 4, ShadowTextures);

	if (UseTiledDeferred && (Constants.PointLightCount > 0 || (TransparencyEnabled && Constants.SpotLightCount > 0)))
	{
		if (Constants.PointLightCount > 0)
		{
			if (PointLightsBoundingSpheresStructuredBuffer == NULL)
				PointLightsBoundingSpheresStructuredBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_STRUCTURED_BUFFER, sizeof(ZEBSphere) * MAX_TILED_LIGHT, sizeof(ZEBSphere), ZEGR_RU_DYNAMIC, ZEGR_RBF_SHADER_RESOURCE);
		
			PointLightsBoundingSpheresStructuredBuffer->SetData(PointLightsBoundingSpheres.GetConstCArray(), PointLightsBoundingSpheres.GetCount() * sizeof(ZEBSphere));
		}

		if (TransparencyEnabled)
		{
			if (Constants.SpotLightCount > 0)
			{
				if (SpotLightsBoundingSpheresStructuredBuffer == NULL)
					SpotLightsBoundingSpheresStructuredBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_STRUCTURED_BUFFER, sizeof(ZEBSphere) * MAX_TILED_LIGHT, sizeof(ZEBSphere), ZEGR_RU_DYNAMIC, ZEGR_RBF_SHADER_RESOURCE);
				
				SpotLightsBoundingSpheresStructuredBuffer->SetData(SpotLightsBoundingSpheres.GetConstCArray(), SpotLightsBoundingSpheres.GetCount() * sizeof(ZEBSphere));
			}

			Context->SetComputeRenderState(TiledDeferredTransparencyComputeRenderState);
		}
		else
		{
			Context->SetComputeRenderState(TiledDeferredComputeRenderState);
		}
	
		const ZEGRBuffer* Buffers[] = {PointLightsBoundingSpheresStructuredBuffer, SpotLightsBoundingSpheresStructuredBuffer};
		Context->SetBuffers(ZEGR_ST_COMPUTE, 6, 2, Buffers);
	
		Context->SetRWBuffer(0, TileLightStructuredBuffer);

		ZEUInt TileCountX = (AccumulationTexture->GetWidth() + TILE_LIGHT_DIMENSION - 1) / TILE_LIGHT_DIMENSION;
		ZEUInt TileCountY = (AccumulationTexture->GetHeight() + TILE_LIGHT_DIMENSION - 1) / TILE_LIGHT_DIMENSION;
	
		Context->Dispatch(TileCountX, TileCountY, 1);
		
		Context->SetBuffer(ZEGR_ST_PIXEL, 15, TileLightStructuredBuffer);
	}
	
	ZERNStage* StageGBuffer = GetRenderer()->GetStage(ZERN_STAGE_GBUFFER);
	if (StageGBuffer == NULL || !StageGBuffer->GetEnabled() || StageGBuffer->GetCommands().GetCount() == 0)
		return false;
	
	Viewport.SetWidth((float)AccumulationTexture->GetWidth());
	Viewport.SetHeight((float)AccumulationTexture->GetHeight());
	
	Context->SetViewports(1, &Viewport);
	
	if (ZEGRGraphicsModule::SAMPLE_COUNT > 1)
	{
		Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, EdgeDetectionConstantBuffer);
		Context->SetRenderState(EdgeDetectionRenderState);
		Context->SetRenderTargets(0, NULL, DepthTexture->GetDepthStencilBuffer(true));
	
		Context->SetStencilRef(1);
		Context->Draw(3, 0);
	
		if (ShowMSAAEdges)
		{
			Context->SetRenderState(DebugEdgeDetectionRenderState);
			
			const ZEGRRenderTarget* RenderTarget = AccumulationTexture->GetRenderTarget();
			Context->SetRenderTargets(1, &RenderTarget, DepthTexture->GetDepthStencilBuffer(true));
			Context->Draw(3, 0);
		}
	}
	
	const ZEGRRenderTarget* RenderTarget = AccumulationTexture->GetRenderTarget();
	Context->SetRenderTargets(1, &RenderTarget, DepthTexture->GetDepthStencilBuffer(true));
	Context->SetVertexBuffer(0, DeferredLightsVertexBuffer);
	
	Context->SetStencilRef(0);
	DrawLights(Context, false);
	
	if (ZEGRGraphicsModule::SAMPLE_COUNT > 1)
	{
		Context->SetStencilRef(1);
		DrawLights(Context, true);
	}

	return false;
}

void ZERNStageLighting::CleanUp(ZEGRContext* Context)
{
	PointLights.Clear();
	SpotLights.Clear();
	PointLightsTransforms.Clear();
	SpotLightsTransforms.Clear();
	PointLightsBoundingSpheres.Clear();
	SpotLightsBoundingSpheres.Clear();
	SpotLightsShadow.Clear();
	ProjectiveLights.Clear();
	ProjectiveLightsTransforms.Clear();
	ProjectionTextures.Clear();
	
	ZERNStage::CleanUp(Context);
}

ZERNStageLighting::ZERNStageLighting()
{
	DirtyFlags.RaiseAll();

	ShowCascades = false;
	ShowMSAAEdges = false;
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
	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&LightsConstantBuffer), "LightsConstantBuffer", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN | ZERN_SRCF_REQUIRED);
	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&TileLightStructuredBuffer), "TileLightBuffer", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN | ZERN_SRCF_REQUIRED);
}

ZERNStageLighting::~ZERNStageLighting()
{
	Deinitialize();
}
