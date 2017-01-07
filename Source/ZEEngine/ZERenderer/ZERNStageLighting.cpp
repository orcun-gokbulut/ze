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

#define MAX_LIGHT				512
#define TILE_DIMENSION			16

#define ZERN_SLDF_SHADERS		1
#define ZERN_SLDF_RENDER_STATE	2

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


	Options.EntryPoint = "ZERNDeferredShading_VertexShader_LightingStage";

	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_DIRECTIONAL_LIGHT"));
	DeferredDirectionalLightVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(DeferredDirectionalLightVertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_PROJECTIVE_LIGHT"));
	DeferredProjectiveLightVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(DeferredProjectiveLightVertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_POINT_LIGHT"));
	DeferredPointLightVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(DeferredPointLightVertexShader == NULL, false, "Cannot set vertex shader.");


	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNDeferredShading_PixelShader_LightingStage";

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_DIRECTIONAL_LIGHT"));
	DeferredDirectionalLightPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(DeferredDirectionalLightPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_PROJECTIVE_LIGHT"));
	DeferredProjectiveLightPixelShader= ZEGRShader::Compile(Options);
	zeCheckError(DeferredProjectiveLightPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_POINT_LIGHT"));
	DeferredPointLightPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(DeferredPointLightPixelShader == NULL, false, "Cannot set pixel shader.");


	Options.EntryPoint = "ZERNDeferredShading_PixelShader_PerSample_LightingStage";

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_DIRECTIONAL_LIGHT"));
	DeferredDirectionalLightPixelShaderPerSample = ZEGRShader::Compile(Options);
	zeCheckError(DeferredDirectionalLightPixelShaderPerSample == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_PROJECTIVE_LIGHT"));
	DeferredProjectiveLightPixelShaderPerSample = ZEGRShader::Compile(Options);
	zeCheckError(DeferredProjectiveLightPixelShaderPerSample == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_RENDER_POINT_LIGHT"));
	DeferredPointLightPixelShaderPerSample = ZEGRShader::Compile(Options);
	zeCheckError(DeferredPointLightPixelShaderPerSample == NULL, false, "Cannot set pixel shader.");


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

	GraphicsRenderState.SetShader(ZEGR_ST_VERTEX, DeferredDirectionalLightVertexShader);
	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DeferredDirectionalLightPixelShader);

	DeferredDirectionalLightRenderState = GraphicsRenderState.Compile();
	zeCheckError(DeferredDirectionalLightRenderState == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DeferredDirectionalLightPixelShaderPerSample);

	DeferredDirectionalLightRenderStatePerSample = GraphicsRenderState.Compile();
	zeCheckError(DeferredDirectionalLightRenderStatePerSample == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_VERTEX, DeferredProjectiveLightVertexShader);
	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DeferredProjectiveLightPixelShader);

	DeferredProjectiveLightRenderState = GraphicsRenderState.Compile();
	zeCheckError(DeferredProjectiveLightRenderState == NULL, false, "Cannot set render state.");

	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, DeferredProjectiveLightPixelShaderPerSample);

	DeferredProjectiveLightRenderStatePerSample = GraphicsRenderState.Compile();
	zeCheckError(DeferredProjectiveLightRenderStatePerSample == NULL, false, "Cannot set render state.");

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

	ZERNStage* StageShadowing = GetRenderer()->GetStage(ZERN_STAGE_SHADOWING);
	bool ShadowingEnabled = (StageShadowing != NULL && StageShadowing->GetEnabled());

	ZEUInt DirectionalLightCount = 0;

	ZEMatrix4x4 TextureTransform;
	ZEMatrix4x4::Create(TextureTransform,
		0.5f, 0.0f, 0.0f, 0.5f,
		0.0f, -0.5f, 0.0f, 0.5f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	ze_for_each(Command, GetCommands())
	{
		ZELight* Light = static_cast<ZELight*>(Command->Entity);
		switch (Light->GetLightType())
		{
			case ZE_LT_DIRECTIONAL:
			{
				ZELightDirectional* DirectionalLight = static_cast<ZELightDirectional*>(Light);

				DirectionalLightStruct& DestLight = Constants.DirectionalLights[DirectionalLightCount++];
				ZEMatrix4x4::Transform3x3(DestLight.DirectionView, View.ViewTransform, DirectionalLight->GetWorldRotation() * -ZEVector3::UnitZ);
				DestLight.DirectionView.NormalizeSelf();
				DestLight.CastShadow = (ZEBool32)(DirectionalLight->GetCastsShadow() & ShadowingEnabled);

				if (DirectionalLight->GetUseSunLight() || DirectionalLight->GetUseMoonLight())
					DestLight.Color = DirectionalLight->GetTerrestrialColor() * DirectionalLight->GetTerrestrialIntensity();
				else
					DestLight.Color = DirectionalLight->GetColor() * DirectionalLight->GetIntensity();

				if (DestLight.CastShadow)
				{
					ze_for_each(Cascade, DirectionalLight->GetCascades())
					{
						ZEUInt CascadeIndex = Cascade.GetIndex();
						DestLight.Cascades[CascadeIndex].ViewTransform = Cascade->ViewTransform * View.InvViewTransform;
						DestLight.Cascades[CascadeIndex].ProjectionTransform = TextureTransform * Cascade->ProjectionTransform;
						DestLight.Cascades[CascadeIndex].SampleCount = Cascade->SampleCount;
						DestLight.Cascades[CascadeIndex].SampleLength = Cascade->SampleLength;
						DestLight.Cascades[CascadeIndex].DepthBias = Cascade->DepthBias;
						DestLight.Cascades[CascadeIndex].NormalBias = Cascade->NormalBias;
					}
					DestLight.CascadeCount = DirectionalLight->GetCascadeCount();
					
					DirectionalLightShadowMap = DirectionalLight->GetShadowMap();
				}
			}
			break;

			case ZE_LT_PROJECTIVE:
			{
				ZELightProjective* ProjectiveLight = static_cast<ZELightProjective*>(Light);

				ProjectiveLightStruct DestLight;
				ZEMatrix4x4::Transform(DestLight.PositionView, View.ViewTransform, ProjectiveLight->GetWorldPosition());
				DestLight.Range = ProjectiveLight->GetRange();
				DestLight.Color = ProjectiveLight->GetColor() * ProjectiveLight->GetIntensity();
				DestLight.Attenuation = ProjectiveLight->GetAttenuation();
				DestLight.CastShadow = (ZEBool32)(ProjectiveLight->GetCastsShadow() & ShadowingEnabled);
				if (DestLight.CastShadow)
				{
					DestLight.ShadowMapIndex = ProjectiveLight->GetShadowMapIndex();
					DestLight.ShadowSampleCount = ZELight::ConvertShadowSampleCount(ProjectiveLight->GetShadowSampleCount());
					DestLight.ShadowSampleLength = ProjectiveLight->GetShadowSampleLength();
					DestLight.ShadowDepthBias = ProjectiveLight->GetShadowDepthBias();
					DestLight.ShadowNormalBias = ProjectiveLight->GetShadowNormalBias();
				}

				DestLight.ProjectionTransform = TextureTransform * ProjectiveLight->GetProjectionTransform() * ProjectiveLight->GetViewTransform() * View.InvViewTransform;

				float TanFovRange = ZEAngle::Tan(ProjectiveLight->GetFOV() * 0.5f) * ProjectiveLight->GetRange();
				ZEMatrix4x4::CreateOrientation(
					DestLight.WorldMatrix, 
					ProjectiveLight->GetWorldPosition(), 
					ProjectiveLight->GetWorldRotation(),
					ZEVector3(TanFovRange * ProjectiveLight->GetAspectRatio() * 2.0f, TanFovRange * 2.0f, ProjectiveLight->GetRange()));

				ZEVector3 Direction = ProjectiveLight->GetWorldRotation() * ZEVector3::UnitZ;
				Direction.NormalizeSelf();
				const ZEViewFrustum& ViewFrustum = static_cast<const ZEViewFrustum&>(ProjectiveLight->GetViewVolume());
				DestLight.CullRange = ZEVector3::Length(ViewFrustum.GetVertex(ZE_FV_RIGHT_TOP_FAR) - ViewFrustum.GetVertex(ZE_FV_LEFT_BOTTOM_NEAR)) * 0.5f;
				ZEMatrix4x4::Transform(DestLight.CullPositionView, View.ViewTransform, ProjectiveLight->GetWorldPosition() + DestLight.CullRange * Direction);

				ProjectiveLights.AddByRef(DestLight);

				ProjectiveLightTexture = ProjectiveLight->GetProjectionTexture();
			}
			break;

			case ZE_LT_POINT:
			{
				ZELightPoint* PointLight = static_cast<ZELightPoint*>(Light);

				PointLightStruct DestLight;
				ZEMatrix4x4::Transform(DestLight.PositionView, View.ViewTransform, PointLight->GetWorldPosition());
				DestLight.Range = PointLight->GetRange();
				DestLight.Color = PointLight->GetColor() * PointLight->GetIntensity();
				DestLight.Type = PointLight->GetLightType();
				DestLight.Attenuation = PointLight->GetAttenuation();
				DestLight.CastShadow = ZEGR_FALSE;

				ZEMatrix4x4::CreateOrientation(DestLight.WorldMatrix, PointLight->GetWorldPosition(), ZEQuaternion::Identity, ZEVector3(PointLight->GetRange()));

				PointLights.AddByRef(DestLight);
			}
			break;

			case ZE_LT_OMNIPROJECTIVE:
			break;
		}
	}

	Constants.DirectionalLightCount = DirectionalLightCount;
	Constants.PointLightCount = PointLights.GetCount();
	Constants.ProjectiveLightCount = ProjectiveLights.GetCount();
	Constants.TileCountX = (Viewport.GetWidth() + TILE_DIMENSION - 1) / TILE_DIMENSION;

	Constants.AddTiledDeferredOutput = (UseTiledDeferred && Constants.PointLightCount > 0) ? ZEGR_TRUE : ZEGR_FALSE;

	ConstantBuffer->SetData(&Constants);

	if (PointLights.GetCount() > 0)
	{
		PointLightStruct* DestPointLights;
		if (!PointLightStructuredBuffer->Map(ZEGR_RMT_WRITE_DISCARD, reinterpret_cast<void**>(&DestPointLights)))
			return false;
		
		memcpy(DestPointLights, PointLights.GetConstCArray(), ZEMath::Min((ZESize)MAX_LIGHT, PointLights.GetCount()) * sizeof(PointLightStruct));
		PointLightStructuredBuffer->Unmap();
	}

	if (ProjectiveLights.GetCount() > 0)
	{
		ProjectiveLightStruct* DestProjectiveLights;
		if (!ProjectiveLightStructuredBuffer->Map(ZEGR_RMT_WRITE_DISCARD, reinterpret_cast<void**>(&DestProjectiveLights)))
			return false;
		
		memcpy(DestProjectiveLights, ProjectiveLights.GetConstCArray(), ZEMath::Min((ZESize)MAX_LIGHT, ProjectiveLights.GetCount()) * sizeof(ProjectiveLightStruct));
		ProjectiveLightStructuredBuffer->Unmap();
	}

	PointLights.Clear();
	ProjectiveLights.Clear();

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
	PointLightStructuredBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_STRUCTURED_BUFFER, MAX_LIGHT * sizeof(PointLightStruct), sizeof(PointLightStruct), ZEGR_RU_DYNAMIC, ZEGR_RBF_SHADER_RESOURCE);
	ProjectiveLightStructuredBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_STRUCTURED_BUFFER, MAX_LIGHT * sizeof(ProjectiveLightStruct), sizeof(ProjectiveLightStruct), ZEGR_RU_DYNAMIC, ZEGR_RBF_SHADER_RESOURCE);

	return Update();
}

bool ZERNStageLighting::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	ScreenCoverVertexShader.Release();
	DeferredDirectionalLightVertexShader.Release();
	DeferredProjectiveLightVertexShader.Release();
	DeferredPointLightVertexShader.Release();
	DeferredDirectionalLightPixelShader.Release();
	DeferredProjectiveLightPixelShader.Release();
	DeferredPointLightPixelShader.Release();
	DeferredDirectionalLightPixelShaderPerSample.Release();
	DeferredProjectiveLightPixelShaderPerSample.Release();
	DeferredPointLightPixelShaderPerSample.Release();
	EdgeDetectionPixelShader.Release();
	TiledDeferredComputeShader.Release();

	DeferredDirectionalLightRenderState.Release();
	DeferredPointLightRenderState.Release();
	DeferredProjectiveLightRenderState.Release();
	DeferredDirectionalLightRenderStatePerSample.Release();
	DeferredPointLightRenderStatePerSample.Release();
	DeferredProjectiveLightRenderStatePerSample.Release();
	EdgeDetectionRenderState.Release();
	TiledDeferredComputeRenderState.Release();

	ConstantBuffer.Release();
	PointLightStructuredBuffer.Release();
	ProjectiveLightStructuredBuffer.Release();
	DeferredLightVertexBuffer.Release();

	RandomVectorsTexture.Release();
	TiledDeferredOutputTexture.Release();

	GBufferEmissive = NULL;
	GBufferDiffuse = NULL;
	GBufferNormal = NULL;
	DepthTexture = NULL;
	ProjectiveShadowMaps = NULL;
	AccumulationTexture = NULL;

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

	if (Constants.ProjectiveLightCount > 0)
	{
		Context->SetRenderState(PerSample ? DeferredProjectiveLightRenderStatePerSample : DeferredProjectiveLightRenderState);
		Context->DrawInstanced(18, 4542, Constants.ProjectiveLightCount, 0);
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

	const ZEGRBuffer* Buffers[] = {PointLightStructuredBuffer, ProjectiveLightStructuredBuffer};
	Context->SetBuffers(ZEGR_ST_VERTEX, 14, 2, Buffers);
	Context->SetBuffers(ZEGR_ST_PIXEL, 14, 2, Buffers);
	Context->SetBuffers(ZEGR_ST_COMPUTE, 14, 2, Buffers);

	const ZEGRTexture* GBuffers[] = {GBufferEmissive, GBufferDiffuse, GBufferNormal, DepthTexture};
	Context->SetTextures(ZEGR_ST_PIXEL, 1, 4, GBuffers);

	const ZEGRTexture* InputTextures[] = {ProjectiveLightTexture, DirectionalLightShadowMap, ProjectiveShadowMaps, RandomVectorsTexture};
	Context->SetTextures(ZEGR_ST_PIXEL, 10, 4, InputTextures);

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

	AccumulationTexture = NULL;
	DepthTexture = NULL;

	ShowCascades = false;
	UseTiledDeferred = false;

	memset(&Constants, 0, sizeof(Constants));

	DirectionalLightShadowMap = NULL;
	ProjectiveLightTexture = NULL;

	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&GBufferEmissive), "GBuffer1", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&GBufferDiffuse), "GBuffer2", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&GBufferNormal), "GBuffer3", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&DepthTexture), "DepthTexture", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&ProjectiveShadowMaps), "ProjectiveShadowMaps", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&AccumulationTexture), "ColorTexture", ZERN_SRUT_WRITE, ZERN_SRCF_GET_FROM_PREV);
}

ZERNStageLighting::~ZERNStageLighting()
{
	Deinitialize();
}
