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
#include "ZEGraphics/ZEGRGraphicsModule.h"

#define MAX_LIGHT				512
#define TILE_DIMENSION			16

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

	ZEGRComputeRenderState ComputeRenderState;
	ComputeRenderState.SetComputeShader(TiledDeferredComputeShader);
	TiledDeferredComputeRenderState = ComputeRenderState.Compile();
	zeCheckError(TiledDeferredComputeRenderState == NULL, false, "Cannot set render state.");

	GraphicsRenderState = ZEGRRenderState();

	ZEGRDepthStencilState DepthStencilStateTestNoWriteLess;
	DepthStencilStateTestNoWriteLess.SetDepthTestEnable(true);
	DepthStencilStateTestNoWriteLess.SetDepthWriteEnable(false);
	DepthStencilStateTestNoWriteLess.SetDepthFunction(ZEGR_CF_LESS);

	GraphicsRenderState.SetBlendState(BlendStateAdditive);
	GraphicsRenderState.SetDepthStencilState(DepthStencilStateTestNoWriteLess);
	GraphicsRenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	GraphicsRenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);
	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, AccumulateEmissivePixelShader);

	AccumulateEmissiveRenderState = GraphicsRenderState.Compile();
	zeCheckError(AccumulateEmissiveRenderState == NULL, false, "Cannot set render state.");

	GraphicsRenderState = ZEGRRenderState();

	ZEGRDepthStencilState GenerateStencilState;
	GenerateStencilState.SetDepthTestEnable(false);
	GenerateStencilState.SetDepthWriteEnable(false);
	GenerateStencilState.SetStencilTestEnable(true);
	GenerateStencilState.SetFrontStencilPass(ZEGR_SO_REPLACE);
	GenerateStencilState.SetBackStencilPass(ZEGR_SO_REPLACE);

	GraphicsRenderState.SetDepthStencilState(GenerateStencilState);
	GraphicsRenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	GraphicsRenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);
	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, EdgeDetectionPixelShader);

	EdgeDetectionRenderState = GraphicsRenderState.Compile();
	zeCheckError(EdgeDetectionRenderState == NULL, false, "Cannot set render state.");

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
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString(ZEGRGraphicsModule::SAMPLE_COUNT)));

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


	Options.EntryPoint = "ZERNDeferredShading_Accumulate_Emissive_PixelShader_Main";
	AccumulateEmissivePixelShader = ZEGRShader::Compile(Options);
	zeCheckError(AccumulateEmissivePixelShader == NULL, false, "Cannot set pixel shader.");

	Options.EntryPoint = "ZERNDeferredShading_EdgeDetection_PixelShader_Main";
	EdgeDetectionPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(EdgeDetectionPixelShader == NULL, false, "Cannot set pixel shader.");

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

bool ZERNStageLighting::UpdateInputOutputs()
{
	AccumulationTexture = GetPrevOutput(ZERN_SO_ACCUMULATION);
	if (AccumulationTexture == NULL)
		return false;

	DepthTexture = GetPrevOutput(ZERN_SO_DEPTH);
	if (DepthTexture == NULL)
		return false;

	Viewport.SetWidth((float)AccumulationTexture->GetWidth());
	Viewport.SetHeight((float)AccumulationTexture->GetHeight());

	return true;
}

bool ZERNStageLighting::UpdateLightBuffers()
{
	const ZERNView& View = GetRenderer()->GetView();

	ZERNStage* StageShadowing = GetRenderer()->GetStage(ZERN_STAGE_SHADOWING);
	bool ShadowingEnabled = (StageShadowing != NULL && StageShadowing->GetEnabled());

	ZEUInt DirectionalLightCount = 0;

	ze_for_each(Command, GetCommands())
	{
		ZELight* Light = static_cast<ZELight*>(Command->Entity);
		switch (Light->GetLightType())
		{
			case ZE_LT_DIRECTIONAL:
			{
				ZELightDirectional* DirectionalLight = static_cast<ZELightDirectional*>(Light);

				DirectionalLightStruct& DestLight = Constants.DirectionalLights[DirectionalLightCount];
				ZEMatrix4x4::Transform3x3(DestLight.DirectionView, View.ViewTransform, DirectionalLight->GetWorldRotation() * -ZEVector3::UnitZ);
				DestLight.DirectionView.NormalizeSelf();
				DestLight.CastShadow = static_cast<ZEBool32>(DirectionalLight->GetCastsShadow() & ShadowingEnabled);

				if (DirectionalLight->GetUseSunLight() || DirectionalLight->GetUseMoonLight())
					DestLight.Color = DirectionalLight->GetTerrestrialColor() * DirectionalLight->GetIntensity();
				else
					DestLight.Color = DirectionalLight->GetColor() * DirectionalLight->GetIntensity();

				if (DestLight.CastShadow)
				{
					ZEMatrix4x4 TextureTransform;
					ZEMatrix4x4::Create(TextureTransform,
						0.5f, 0.0f, 0.0f, 0.5f,
						0.0f, -0.5f, 0.0f, 0.5f,
						0.0f, 0.0f, 1.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f);

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
				}
				DirectionalLightShadowMaps[DirectionalLightCount] = DirectionalLight->GetShadowMap();
				DirectionalLightCount++;
			}
			break;

			case ZE_LT_PROJECTIVE:
			{
				ZELightProjective* ProjectiveLight = static_cast<ZELightProjective*>(Light);

				ProjectiveLightStruct DestLight;
				ZEMatrix4x4::Transform(DestLight.PositionView, View.ViewTransform, ProjectiveLight->GetWorldPosition());
				DestLight.Range = ProjectiveLight->GetRange();
				DestLight.Color = ProjectiveLight->GetColor() * ProjectiveLight->GetIntensity();
				DestLight.Type = ProjectiveLight->GetLightType();
				DestLight.Attenuation = ProjectiveLight->GetAttenuation();
				DestLight.CastShadow = (ZEBool32)(ProjectiveLight->GetCastsShadow() & ShadowingEnabled);
				DestLight.ShadowSampleCount = ZELight::ConvertShadowSampleCount(ProjectiveLight->GetShadowSampleCount());
				DestLight.ShadowSampleLength = ProjectiveLight->GetShadowSampleLength();
				DestLight.ShadowDepthBias = ProjectiveLight->GetShadowDepthBias();

				DestLight.ProjectionTransform = ProjectiveLight->GetProjectionTransform() * ProjectiveLight->GetViewTransform() * View.InvViewTransform;

				float TanFovRange = ZEAngle::Tan(ProjectiveLight->GetFOV() * 0.5f) * ProjectiveLight->GetRange();
				ZEMatrix4x4::CreateOrientation(
					DestLight.WorldMatrix, 
					ProjectiveLight->GetWorldPosition(), 
					ProjectiveLight->GetWorldRotation(),
					ZEVector3(TanFovRange * ProjectiveLight->GetAspectRatio() * 2.0f, TanFovRange * 2.0f, ProjectiveLight->GetRange()));

				ProjectiveLights.AddByRef(DestLight);
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

	ConstantBuffer->SetData(&Constants);

	if (PointLights.GetCount() > 0)
	{
		PointLightStruct* DestPointLights;
		if (!PointLightStructuredBuffer->Lock(reinterpret_cast<void**>(&DestPointLights)))
			return false;
			memcpy(DestPointLights, PointLights.GetConstCArray(), PointLights.GetCount() * sizeof(PointLightStruct));
		PointLightStructuredBuffer->Unlock();
	}

	if (ProjectiveLights.GetCount() > 0)
	{
		ProjectiveLightStruct* DestProjectiveLights;
		if (!ProjectiveLightStructuredBuffer->Lock(reinterpret_cast<void**>(&DestProjectiveLights)))
			return false;
			memcpy(DestProjectiveLights, ProjectiveLights.GetConstCArray(), ProjectiveLights.GetCount() * sizeof(ProjectiveLightStruct));
		ProjectiveLightStructuredBuffer->Unlock();
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

	if (!UpdateInputOutputs())
		return false;

	if (!UpdateLightBuffers())
		return false;

	return true;
}

bool ZERNStageLighting::InitializeSelf()
{
	if (!ZERNStage::InitializeSelf())
		return false;

	CreateRandomVectors();
	CreateLightGeometries();

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));
	PointLightStructuredBuffer = ZEGRStructuredBuffer::Create(MAX_LIGHT, sizeof(PointLightStruct));
	ProjectiveLightStructuredBuffer = ZEGRStructuredBuffer::Create(MAX_LIGHT, sizeof(ProjectiveLightStruct));

	return Update();
}

void ZERNStageLighting::DeinitializeSelf()
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
	TiledDeferredComputeShader.Release();
	AccumulateEmissivePixelShader.Release();

	DeferredDirectionalLightRenderState.Release();
	DeferredPointLightRenderState.Release();
	DeferredProjectiveLightRenderState.Release();
	DeferredDirectionalLightRenderStatePerSample.Release();
	DeferredPointLightRenderStatePerSample.Release();
	DeferredProjectiveLightRenderStatePerSample.Release();
	TiledDeferredComputeRenderState.Release();
	AccumulateEmissiveRenderState.Release();
	EdgeDetectionRenderState.Release();

	ConstantBuffer.Release();
	PointLightStructuredBuffer.Release();
	ProjectiveLightStructuredBuffer.Release();
	DeferredLightVertexBuffer.Release();

	RandomVectorsTexture.Release();

	AccumulationTexture = NULL;
	DepthTexture = NULL;

	ZERNStage::DeinitializeSelf();
}

void ZERNStageLighting::DrawLights(ZEGRContext* Context, bool PerSample)
{
	if (Constants.DirectionalLightCount > 0)
	{
		Context->SetRenderState(PerSample ? DeferredDirectionalLightRenderStatePerSample: DeferredDirectionalLightRenderState);
		Context->DrawInstanced(6, 0, Constants.DirectionalLightCount, 0);
	}

	if (Constants.ProjectiveLightCount > 0)
	{
		Context->SetRenderState(PerSample ? DeferredProjectiveLightRenderStatePerSample : DeferredProjectiveLightRenderState);
		Context->DrawInstanced(18, 4542, Constants.ProjectiveLightCount, 0);
	}

	if (Constants.PointLightCount > 0)
	{
		Context->SetRenderState(PerSample ? DeferredPointLightRenderStatePerSample : DeferredPointLightRenderState);
		Context->DrawInstanced(3600, 942, Constants.PointLightCount, 0);
	}
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

void ZERNStageLighting::SetUseTiledDeferred(bool UseTiledDeferred)
{
	this->UseTiledDeferred = UseTiledDeferred;
}

bool ZERNStageLighting::GetUseTiledDeferred() const
{
	return UseTiledDeferred;
}

const ZEGRTexture2D* ZERNStageLighting::GetOutput(ZERNStageBuffer Output) const
{
	if (GetEnabled() && (Output == ZERN_SO_COLOR || Output == ZERN_SO_ACCUMULATION))
		return AccumulationTexture;
	
	return ZERNStage::GetOutput(Output);
}

bool ZERNStageLighting::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!Update())
		return false;

	if (GetCommands().GetCount() == 0)
		return false;

	Context->SetConstantBuffers(ZEGR_ST_VERTEX, 8, 1, ConstantBuffer.GetPointerToPointer());
	Context->SetConstantBuffers(ZEGR_ST_PIXEL, 8, 1, ConstantBuffer.GetPointerToPointer());
	Context->SetConstantBuffers(ZEGR_ST_COMPUTE, 8, 1, ConstantBuffer.GetPointerToPointer());

	const ZEGRStructuredBuffer* StructuredBuffers[] = {PointLightStructuredBuffer, ProjectiveLightStructuredBuffer};
	Context->SetStructuredBuffers(ZEGR_ST_VERTEX, 13, 2, StructuredBuffers);
	Context->SetStructuredBuffers(ZEGR_ST_PIXEL, 13, 2, StructuredBuffers);
	Context->SetStructuredBuffers(ZEGR_ST_COMPUTE, 13, 2, StructuredBuffers);

	const ZEGRTexture* GBuffers[] = {GetPrevOutput(ZERN_SO_GBUFFER_EMISSIVE), GetPrevOutput(ZERN_SO_GBUFFER_DIFFUSE), GetPrevOutput(ZERN_SO_NORMAL), DepthTexture, };
	Context->SetTextures(ZEGR_ST_PIXEL, 1, 4, GBuffers);
	const ZEGRTexture* ShadowRelatedTextures[] = {DirectionalLightShadowMaps[0], RandomVectorsTexture};
	Context->SetTextures(ZEGR_ST_PIXEL, 11, 2, ShadowRelatedTextures);

	if (UseTiledDeferred)
	{
		Context->SetComputeRenderState(TiledDeferredComputeRenderState);
		Context->SetRWTextures(0, 1, reinterpret_cast<const ZEGRTexture**>(&AccumulationTexture));	//Doesnt work with multi-sampled texture
		Context->SetTextures(ZEGR_ST_COMPUTE, 1, 4, GBuffers);

		ZEUInt TileCountX = (AccumulationTexture->GetWidth() + TILE_DIMENSION - 1) / TILE_DIMENSION;
		ZEUInt TileCountY = (AccumulationTexture->GetHeight() + TILE_DIMENSION - 1) / TILE_DIMENSION;

		Context->Dispatch(TileCountX, TileCountY, 1);
	}
	else
	{
		ZERNStage* StageGBuffer = GetRenderer()->GetStage(ZERN_STAGE_GBUFFER);
		if (StageGBuffer != NULL && StageGBuffer->GetEnabled() && StageGBuffer->GetCommands().GetCount() > 0)
		{
			Context->SetViewports(1, &Viewport);

			if (ZEGRGraphicsModule::SAMPLE_COUNT > 1)
			{
				Context->SetRenderState(EdgeDetectionRenderState);
				Context->SetRenderTargets(0, NULL, DepthTexture->GetDepthStencilBuffer(true));
				Context->SetStencilRef(1);
				Context->Draw(3, 0);
			}

			Context->SetVertexBuffers(0, 1, DeferredLightVertexBuffer.GetPointerToPointer());
			const ZEGRRenderTarget* OutputRenderTarget = AccumulationTexture->GetRenderTarget();
			Context->SetRenderTargets(1, &OutputRenderTarget, DepthTexture->GetDepthStencilBuffer(true));
			
			Context->SetStencilRef(0);
			DrawLights(Context, false);

			if (ZEGRGraphicsModule::SAMPLE_COUNT > 1)
			{
				Context->SetStencilRef(1);
				DrawLights(Context, true);
			}

			Context->SetRenderState(AccumulateEmissiveRenderState);
			Context->Draw(3, 0);
		}
	}

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

	AccumulationTexture = NULL;
	DepthTexture = NULL;

	ShowCascades = false;
	UseTiledDeferred = false;

	memset(&Constants, 0, sizeof(Constants));
	memset(&DirectionalLightShadowMaps, NULL, sizeof(ZEGRTexture*) * 2);
}