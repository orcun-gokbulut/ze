//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStandardMaterial.cpp
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

#include "ZERNStandardMaterial.h"

#include "ZEError.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEML/ZEMLReader.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRShaderCompileOptions.h"
#include "ZEGraphics/ZEGRCanvas.h"
#include "ZERNCommand.h"
#include "ZERNStageGBuffer.h"
#include "ZERNStageID.h"
#include "ZERNShaderSlots.h"
#include "ZERNStageShadowmapGeneration.h"
#include "ZERNStageParticleRendering.h"
#include "ZERNStageDebug.h"
#include "ZERNStageForward.h"
#include "ZERNStageRenderDepth.h"
#include "ZEResource/ZERSTemplates.h"
#include "ZEModel/ZEMDVertex.h"
#include "ZEInterior/ZEInterior.h"
#include "ZEInterior/ZEInteriorResource.h"

#define ZERN_FMDF_CONSTANT_BUFFER		1
#define ZERN_FMDF_RENDER_STATE			2
#define ZERN_FMDF_SHADERS				4
#define ZERN_FMDF_STAGE_MASK			8

void ZERNStandardMaterial::UpdateGBufferForwardVertexShaderDefinitions(ZEGRShaderCompileOptions& Options) const
{
	if (SkinningEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_SKIN_TRANSFORM"));

	if (VertexColorEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_VERTEX_COLOR"));

	if (ClippingPlanesEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_CLIPPING_PLANES"));

	if (TransparencyEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_FORWARD"));
	else
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_DEFERRED"));
}

void ZERNStandardMaterial::UpdateGBufferForwardPixelShaderDefinitions(ZEGRShaderCompileOptions& Options) const
{
	if (AlphaCullEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_ALPHA_CULL"));

	if (VertexColorEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_VERTEX_COLOR"));

	if (SubSurfaceScatteringMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_SUB_SURFACE_SCATTERING_MAP"));

	if (BaseMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_BASE_MAP"));

	if (NormalMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_NORMAL_MAP"));

	if (SpecularMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_SPECULAR_MAP"));

	if (SpecularGlossMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_SPECULAR_GLOSS_MAP"));

	if (EmissiveMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_EMISSIVE_MAP"));

	if (ReflectionEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_REFLECTION"));

	if (RefractionEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_REFRACTION"));

	if (OpacityMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_OPACITY_MAP"));

	if (DetailBaseMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_DETAIL_BASE_MAP"));

	if (DetailNormalMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_DETAIL_NORMAL_MAP"));

	if (TransparencyEnabled)
	{
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_FORWARD"));
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_SHADING_TRANSPARENT"));
	}
	else
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_DEFERRED"));
}

void ZERNStandardMaterial::UpdateShadowMapGenerationVertexShaderDefinitions(ZEGRShaderCompileOptions& Options) const
{
	if (SkinningEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_SKIN_TRANSFORM"));

	if (ClippingPlanesEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_CLIPPING_PLANES"));
}

void ZERNStandardMaterial::UpdateShadowMapGenerationPixelShaderDefinitions(ZEGRShaderCompileOptions& Options) const
{
	if (OpacityMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_OPACITY_MAP"));

	if (BaseMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_BASE_MAP"));

	if (AlphaCullEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_ALPHA_CULL"));
}

bool ZERNStandardMaterial::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_FMDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNFixedMaterial.hlsl";
	Options.Model = ZEGR_SM_5_0;

	UpdateGBufferForwardVertexShaderDefinitions(Options);

	if (!UseInteriorVertexLayout)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_MODEL"));

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNFixedMaterial_VertexShader";
	StageGBuffer_Forward_VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(StageGBuffer_Forward_VertexShader == NULL, false, "Cannot set vertex shader.");

	if (!SkinningEnabled)
	{
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_INSTANCING"));

		Options.Type = ZEGR_ST_VERTEX;
		Options.EntryPoint = "ZERNFixedMaterial_VertexShader";
		StageGBuffer_Forward_Instancing_VertexShader = ZEGRShader::Compile(Options);
		zeCheckError(StageGBuffer_Forward_Instancing_VertexShader == NULL, false, "Cannot set vertex shader.");
	}

	Options.Definitions.Clear();
	UpdateShadowMapGenerationVertexShaderDefinitions(Options);

	if (!UseInteriorVertexLayout)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_MODEL"));

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNFixedMaterial_ShadowMapGenerationStage_VertexShader_Main";
	StageShadowmapGeneration_VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(StageShadowmapGeneration_VertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_DEPTH_PREPASS"));

	if (!UseInteriorVertexLayout)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_MODEL"));

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNFixedMaterial_ShadowMapGenerationStage_VertexShader_Main";
	StageRenderDepth_VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(StageRenderDepth_VertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Definitions.Remove(Options.Definitions.GetCount() - 1);

	if (!SkinningEnabled)
	{
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_INSTANCING"));

		Options.Type = ZEGR_ST_VERTEX;
		Options.EntryPoint = "ZERNFixedMaterial_ShadowMapGenerationStage_VertexShader_Main";
		StageShadowmapGeneration_Instancing_VertexShader = ZEGRShader::Compile(Options);
		zeCheckError(StageShadowmapGeneration_Instancing_VertexShader == NULL, false, "Cannot set vertex shader.");

		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_DEPTH_PREPASS"));

		Options.Type = ZEGR_ST_VERTEX;
		Options.EntryPoint = "ZERNFixedMaterial_ShadowMapGenerationStage_VertexShader_Main";
		StageRenderDepth_Instancing_VertexShader = ZEGRShader::Compile(Options);
		zeCheckError(StageRenderDepth_Instancing_VertexShader == NULL, false, "Cannot set vertex shader.");
	}

	Options.Definitions.Clear();
	UpdateGBufferForwardPixelShaderDefinitions(Options);
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString::FromInt32(ZEGRGraphicsModule::SAMPLE_COUNT)));

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFixedMaterial_PixelShader";
	StageGBuffer_Forward_PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(StageGBuffer_Forward_PixelShader == NULL, false, "Cannot set pixel shader.");

	if (!SkinningEnabled)
	{
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_INSTANCING"));

		Options.Type = ZEGR_ST_PIXEL;
		Options.EntryPoint = "ZERNFixedMaterial_PixelShader";
		StageGBuffer_Forward_Instancing_PixelShader = ZEGRShader::Compile(Options);
		zeCheckError(StageGBuffer_Forward_Instancing_PixelShader == NULL, false, "Cannot set pixel shader.");
	}

	Options.Definitions.Clear();
	UpdateShadowMapGenerationPixelShaderDefinitions(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFixedMaterial_ShadowMapGenerationStage_PixelShader_Main";
	StageShadowmapGeneration_PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(StageShadowmapGeneration_PixelShader == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_DEPTH_PREPASS"));

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFixedMaterial_ShadowMapGenerationStage_PixelShader_Main";
	StageRenderDepth_PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(StageRenderDepth_PixelShader == NULL, false, "Cannot set pixel shader.");

	if (!SkinningEnabled)
	{
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_INSTANCING"));

		Options.Type = ZEGR_ST_PIXEL;
		Options.EntryPoint = "ZERNFixedMaterial_ShadowMapGenerationStage_PixelShader_Main";
		StageRenderDepth_Instancing_PixelShader = ZEGRShader::Compile(Options);
		zeCheckError(StageRenderDepth_Instancing_PixelShader == NULL, false, "Cannot set pixel shader.");
	}

	DirtyFlags.UnraiseFlags(ZERN_FMDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_FMDF_RENDER_STATE);

	return true;
}

bool ZERNStandardMaterial::UpdateRenderState()
{
	if (!DirtyFlags.GetFlags(ZERN_FMDF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState;
	if (TransparencyEnabled)
	{
		RenderState = ZERNStageForwardTransparent::GetRenderState();

		if (TransparencyMode == ZERN_TM_ADDITIVE)
		{
			ZEGRBlendRenderTarget RenderTargetBlend;
			RenderTargetBlend.SetBlendEnable(true);
			RenderTargetBlend.SetSource(ZEGR_BO_SRC_ALPHA);
			RenderTargetBlend.SetDestination(ZEGR_BO_ONE);
			RenderTargetBlend.SetOperation(ZEGR_BE_ADD);

			ZEGRBlendState State;
			State.SetBlendEnable(true);
			State.SetRenderTargetBlend(0, RenderTargetBlend);
			RenderState.SetBlendState(State);
		}
	}
	else
	{
		RenderState = ZERNStageGBuffer::GetRenderState();

		if (AlphaCullEnabled && AlphaToCoverageEnabled)
		{
			ZEGRBlendState BlendState;
			BlendState.SetAlphaToCoverageEnable(true);
			RenderState.SetBlendState(BlendState);
		}
	}

	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);

	if (SkinningEnabled)
		RenderState.SetVertexLayout(ZEMDVertexSkin::GetVertexLayout());
	else if (UseInteriorVertexLayout)
		RenderState.SetVertexLayout(*ZEInteriorVertex::GetVertexLayout());
	else
		RenderState.SetVertexLayout(ZEMDVertex::GetVertexLayout());

	ZEGRRasterizerState RasterizerState = RenderState.GetRasterizerState();
	RasterizerState.SetCullMode(TwoSided ? ZEGR_CMD_NONE : RasterizerState.GetCullMode());
	RasterizerState.SetFillMode(Wireframe ? ZEGR_FM_WIREFRAME : RasterizerState.GetFillMode());
	RenderState.SetRasterizerState(RasterizerState);
	
	RenderState.SetShader(ZEGR_ST_VERTEX, StageGBuffer_Forward_VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, StageGBuffer_Forward_PixelShader);

	StageGBuffer_Forward_RenderState = RenderState.Compile();
	zeCheckError(StageGBuffer_Forward_RenderState == NULL, false, "Cannot set gbuffer/forward render state.");

	if (!SkinningEnabled)
	{
		RenderState.SetVertexLayout(ZEMDVertexInstance::GetVertexLayout());
		RenderState.SetShader(ZEGR_ST_VERTEX, StageGBuffer_Forward_Instancing_VertexShader);
		RenderState.SetShader(ZEGR_ST_PIXEL, StageGBuffer_Forward_Instancing_PixelShader);
		StageGBuffer_Forward_Instancing_RenderState = RenderState.Compile();
		zeCheckError(StageGBuffer_Forward_Instancing_RenderState == NULL, false, "Cannot set gbuffer/forward instancing render state.");
	}

	RenderState = ZERNStageShadowmapGeneration::GetRenderState();
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);

	RenderState.SetShader(ZEGR_ST_VERTEX, StageShadowmapGeneration_VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, StageShadowmapGeneration_PixelShader);

	if (SkinningEnabled)
		RenderState.SetVertexLayout(ZEMDVertexSkin::GetVertexLayout());
	else if (UseInteriorVertexLayout)
		RenderState.SetVertexLayout(*ZEInteriorVertex::GetVertexLayout());
	else
		RenderState.SetVertexLayout(ZEMDVertex::GetVertexLayout());

	RasterizerState = RenderState.GetRasterizerState();
	RasterizerState.SetCullMode(TwoSided ? ZEGR_CMD_NONE : RasterizerState.GetCullMode());
	RenderState.SetRasterizerState(RasterizerState);

	ZEGRDepthStencilState DepthStencilState;
	DepthStencilState.SetDepthFunction(ZEGR_CF_GREATER);
	RenderState.SetDepthStencilState(DepthStencilState);

	StageShadowmapGeneration_RenderState = RenderState.Compile();
	zeCheckError(StageShadowmapGeneration_RenderState == NULL, false, "Cannot set shadow map generation render state.");

	if (!SkinningEnabled)
	{
		RenderState.SetVertexLayout(ZEMDVertexInstance::GetVertexLayout());
		RenderState.SetShader(ZEGR_ST_VERTEX, StageShadowmapGeneration_Instancing_VertexShader);

		StageShadowmapGeneration_Instancing_RenderState = RenderState.Compile();
		zeCheckError(StageShadowmapGeneration_Instancing_RenderState == NULL, false, "Cannot set shadow map generation instancing render state.");
	}
	
	RenderState = ZERNStageRenderDepth::GetRenderState();
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);

	if (SkinningEnabled)
		RenderState.SetVertexLayout(ZEMDVertexSkin::GetVertexLayout());
	else if (UseInteriorVertexLayout)
		RenderState.SetVertexLayout(*ZEInteriorVertex::GetVertexLayout());
	else
		RenderState.SetVertexLayout(ZEMDVertex::GetVertexLayout());

	if (AlphaCullEnabled && AlphaToCoverageEnabled)
	{
		ZEGRBlendState BlendState;
		BlendState.SetAlphaToCoverageEnable(true);
		RenderState.SetBlendState(BlendState);
	}

	RasterizerState = RenderState.GetRasterizerState();
	RasterizerState.SetCullMode(TwoSided ? ZEGR_CMD_NONE : RasterizerState.GetCullMode());
	RenderState.SetRasterizerState(RasterizerState);

	RenderState.SetShader(ZEGR_ST_VERTEX, StageRenderDepth_VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, StageRenderDepth_PixelShader);

	StageRenderDepth_RenderState = RenderState.Compile();
	zeCheckError(StageRenderDepth_RenderState == NULL, false, "Cannot set depth prepass render state.");

	if (!SkinningEnabled)
	{
		RenderState.SetVertexLayout(ZEMDVertexInstance::GetVertexLayout());
		RenderState.SetShader(ZEGR_ST_VERTEX, StageRenderDepth_Instancing_VertexShader);
		RenderState.SetShader(ZEGR_ST_PIXEL, StageRenderDepth_Instancing_PixelShader);

		StageRenderDepth_Instancing_RenderState = RenderState.Compile();
		zeCheckError(StageRenderDepth_Instancing_RenderState == NULL, false, "Cannot set depth prepass instancing render state.");
	}

	DirtyFlags.UnraiseFlags(ZERN_FMDF_RENDER_STATE);

	return true;
}

bool ZERNStandardMaterial::UpdateStageMask()
{
	if (!DirtyFlags.GetFlags(ZERN_FMDF_STAGE_MASK))
		return true;

	StageMask = ZERN_STAGE_DEBUG;
	StageMask |= ZERN_STAGE_RENDER_DEPTH | (TransparencyEnabled ? ZERN_STAGE_FORWARD_TRANSPARENT : ZERN_STAGE_GBUFFER);
	StageMask |= ShadowCaster ? ZERN_STAGE_SHADOW_MAP_GENERATION : 0;

	DirtyFlags.UnraiseFlags(ZERN_FMDF_STAGE_MASK);

	return true;
}

bool ZERNStandardMaterial::UpdateConstantBuffer()
{
	if (!DirtyFlags.GetFlags(ZERN_FMDF_CONSTANT_BUFFER))
		return true;
	
	Constants.AmbientColor = (AmbientEnabled ? 1.0f : 0.0f) * AmbientFactor * AmbientColor;
	Constants.DiffuseColor = (DiffuseEnabled ? 1.0f : 0.0f) * DiffuseFactor * DiffuseColor;
	Constants.SpecularColor = (SpecularEnabled ? 1.0f : 0.0f) * SpecularFactor * SpecularColor;
	Constants.EmissiveColor = (EmissiveEnabled ? 1.0f : 0.0f) * EmissiveFactor * EmissiveColor;
	Constants.ReflectionColor = (ReflectionEnabled ? 1.0f : 0.0f) * ReflectionFactor * ReflectionColor;
	Constants.RefractionColor = (RefractionEnabled ? 1.0f : 0.0f) * RefractionFactor * RefractionColor;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_FMDF_CONSTANT_BUFFER);

	return true;
}

bool ZERNStandardMaterial::Update()
{
	if (!ZERNMaterial::Update())
		return false;

	if (ConstantBuffer.IsNull())
		ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

	if (!UpdateStageMask())
		return false;

	if (!UpdateShaders())
		return false;

	if (!UpdateRenderState())
		return false;

	if (!UpdateConstantBuffer())
		return false;

	return true;
}

ZETaskResult ZERNStandardMaterial::LoadInternal()
{
	if (!GetFileName().IsEmpty())
	{
		ZEMLReader MaterialReader;
		if (!MaterialReader.Open(GetFileName()))
		{
			zeError("Cannot load ZERNStandardMaterial. Cannot open file. File Name: %s", GetFileName().ToCString());
			return ZE_TR_FAILED;
		}

		ZEMLReaderNode MaterialNode = MaterialReader.GetRootNode();
		if (!Unserialize(&MaterialNode))
			return ZE_TR_FAILED;
	}

	DirtyFlags.RaiseAll();
	
	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);
	if (ConstantBuffer == NULL)
		return ZE_TR_FAILED;

	if (!UpdateStageMask())
		return ZE_TR_FAILED;

	if (!UpdateShaders())
		return ZE_TR_FAILED;

	if (!UpdateRenderState())
		return ZE_TR_FAILED;

	return ZE_TR_DONE;
}

ZETaskResult ZERNStandardMaterial::UnloadInternal()
{
	StageMask = 0;
	DirtyFlags.RaiseAll();

	StageGBuffer_Forward_VertexShader.Release();
	StageGBuffer_Forward_PixelShader.Release();
	StageGBuffer_Forward_RenderState.Release();
	StageShadowmapGeneration_VertexShader.Release();
	StageShadowmapGeneration_RenderState.Release();
	StageShadowmapGeneration_RenderState.Release();
	ConstantBuffer.Release();
	Sampler.Release();
	BaseMap.Release();
	SpecularMap.Release();
	SpecularGlossMap.Release();
	EmissiveMap.Release();
	NormalMap.Release();
	HeightMap.Release();
	OpacityMap.Release();
	SubSurfaceScatteringMap.Release();
	EnvironmentMap.Release();
	DetailBaseMap.Release();
	DetailNormalMap.Release();

	return ZE_TR_DONE;
}

ZERNStandardMaterial::ZERNStandardMaterial()
{
	StageMask = 0;
	DirtyFlags.RaiseAll();

	UseInteriorVertexLayout = false;

	AlphaToCoverageEnabled = false;
	MaxTextureLOD = 0;
	ShadowCaster = true;
	TwoSided = false;
	Wireframe = false;
	AlphaCullEnabled = false;
	SkinningEnabled = false;
	VertexColorEnabled = false;
	TransparencyEnabled = false;
	TransparencyMode = ZERN_TM_NONE;
	SubSurfaceScatteringMapEnabled = false;	
	BaseMapEnabled = false;
	AmbientEnabled = false;
	AmbientFactor = 1.0f;
	AmbientColor = ZEVector3::One;
	DiffuseEnabled = true;
	DiffuseFactor = 1.0f;
	DiffuseColor = ZEVector3::One;
	SpecularEnabled = true;
	SpecularMapEnabled = false;
	SpecularGlossMapEnabled = false;
	SpecularFactor = 1.0f;
	SpecularColor = ZEVector3::One;
	NormalMapEnabled = false;
	HeightMapEnabled = false;
	HeightMapTechnique = ZERN_HMT_NONE;
	EmissiveEnabled = false;
	EmissiveMapEnabled = false;
	EmissiveFactor = 1.0f;
	EmissiveColor = ZEVector3::One;
	ReflectionEnabled = false;
	ReflectionFactor = 1.0f;
	ReflectionColor = ZEVector3::One;
	RefractionEnabled = false;
	RefractionFactor = 1.0f;
	RefractionColor = ZEVector3::One;
	OpacityMapEnabled = false;
	EnvironmentMapEnabled = false;
	DetailBaseMapEnabled = false;
	DetailNormalMapEnabled = false;
	ClippingPlanesEnabled = false;

	Constants.AmbientColor = ZEVector3::One;
	Constants.Opacity = 1.0f;
	Constants.DiffuseColor = ZEVector3::One;
	Constants.SubSurfaceScatteringFactor = 0.0f;
	Constants.SpecularColor = ZEVector3::One;
	Constants.SpecularPower = 1.0f;
	Constants.HeightMapTechnique = ZERN_HMT_NONE;
	Constants.HeightMapScale = 1.0f;
	Constants.HeightMapOffset = 0.0f;
	Constants.DitheredOpacityEnabled = ZEGR_FALSE;
	Constants.EmissiveColor = ZEVector3::One;
	Constants.AlphaCullLimit = 0.0f;
	Constants.ReflectionColor = ZEVector3::One;
	Constants.SceneAmbientEnabled = false;
	Constants.RefractionColor = ZEVector3::One;
	Constants.RefractionIndex = 1.0f;
	Constants.DetailBaseMapColor = ZEVector3::One;
	Constants.DetailNormalMapFactor = 1.0f;
	Constants.DetailBaseMapTiling = ZEVector2::One;
	Constants.DetailNormalMapTiling = ZEVector2::One;
	Constants.DetailBaseMapAttenuationStart = 10.0f;
	Constants.DetailBaseMapAttenuationFactor = 0.01f;
	Constants.DetailNormalMapAttenuationStart = 10.0f;
	Constants.DetailNormalMapAttenuationFactor = 0.01f;

	ZEGRSamplerDescription SamplerLinearWrapDescription;
	SamplerLinearWrapDescription.AddressU = ZEGR_TAM_WRAP;
	SamplerLinearWrapDescription.AddressV = ZEGR_TAM_WRAP;
	SamplerLinearWrapDescription.AddressW = ZEGR_TAM_WRAP;
	SamplerLinearWrapDescription.MinFilter = ZEGR_TFM_ANISOTROPIC;
	SamplerLinearWrapDescription.MipFilter = ZEGR_TFM_ANISOTROPIC;
	SamplerLinearWrapDescription.MagFilter = ZEGR_TFM_ANISOTROPIC;
	SamplerLinearWrapDescription.MaxAnisotropy = 4;
	Sampler = ZEGRSampler::GetSampler(SamplerLinearWrapDescription);

	Register();
}

ZERNStandardMaterial::~ZERNStandardMaterial()
{
	Unregister();
}

ZEUInt ZERNStandardMaterial::GetStageMask() const
{
	return StageMask;
}

void ZERNStandardMaterial::SetSampler(const ZEHolder<ZEGRSampler>& Sampler)
{
	this->Sampler = Sampler;
}

const ZEHolder<ZEGRSampler>& ZERNStandardMaterial::GetSampler() const
{
	return Sampler;
}

void ZERNStandardMaterial::SetAlphaToCoverageEnabled(bool Enabled)
{
	if (AlphaToCoverageEnabled == Enabled)
		return;

	AlphaToCoverageEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_RENDER_STATE);
}

bool ZERNStandardMaterial::GetAlphaToCoverageEnabled() const
{
	return AlphaToCoverageEnabled;
}

void ZERNStandardMaterial::SetMaxTextureLOD(ZEUInt8 MaxTextureLOD)
{
	this->MaxTextureLOD = MaxTextureLOD;
}

ZEUInt8 ZERNStandardMaterial::GetMaxTextureLOD() const
{
	return MaxTextureLOD;
}

void ZERNStandardMaterial::SetShadowCaster(bool ShadowCaster)
{
	if (this->ShadowCaster == ShadowCaster)
		return;

	this->ShadowCaster = ShadowCaster;

	DirtyFlags.RaiseFlags(ZERN_FMDF_STAGE_MASK);
}

bool ZERNStandardMaterial::GetShadowCaster() const
{
	return ShadowCaster;
}

void ZERNStandardMaterial::SetTwoSided(bool Enable)
{
	if (TwoSided == Enable)
		return;

	TwoSided = Enable;

	DirtyFlags.RaiseFlags(ZERN_FMDF_RENDER_STATE);
}

bool ZERNStandardMaterial::GetTwoSided() const
{
	return TwoSided;
}

void ZERNStandardMaterial::SetWireframe(bool Enable)
{
	if (Wireframe == Enable)
		return;

	Wireframe = Enable;

	DirtyFlags.RaiseFlags(ZERN_FMDF_RENDER_STATE);
}

bool ZERNStandardMaterial::GetWireframe() const
{
	return Wireframe;
}

void ZERNStandardMaterial::SetSkinningEnabled(bool Enabled)
{
	if (SkinningEnabled == Enabled)
		return;

	SkinningEnabled = true;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNStandardMaterial::GetSkinningEnabled() const
{
	return SkinningEnabled;
}

void ZERNStandardMaterial::SetVertexColorEnabled(bool Enabled)
{
	if (VertexColorEnabled == Enabled)
		return;

	VertexColorEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNStandardMaterial::GetVertexColorEnabled() const
{
	return VertexColorEnabled;
}

void ZERNStandardMaterial::SetTransparencyEnabled(bool Enabled)
{
	if (TransparencyEnabled == Enabled)
		return;

	TransparencyEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS | ZERN_FMDF_RENDER_STATE | ZERN_FMDF_STAGE_MASK);
}

bool ZERNStandardMaterial::GetTransparencyEnabled() const
{
	return TransparencyEnabled;
}

void ZERNStandardMaterial::SetTransparencyMode(ZERNTransparencyMode Mode)
{
	if (TransparencyMode == Mode)
		return;

	TransparencyMode = Mode;

	if (TransparencyEnabled)
		DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS | ZERN_FMDF_RENDER_STATE | ZERN_FMDF_STAGE_MASK);
}

ZERNTransparencyMode ZERNStandardMaterial::GetTransparencyMode() const
{
	return TransparencyMode;
}

void ZERNStandardMaterial::SetDitheredOpacityEnabled(bool Enabled)
{
	if (Constants.DitheredOpacityEnabled == Enabled)
		return;

	Constants.DitheredOpacityEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

bool ZERNStandardMaterial::GetDitheredOpacityEnabled() const
{
	return Constants.DitheredOpacityEnabled;
}

void ZERNStandardMaterial::SetAlphaCullEnabled(bool Enabled)
{
	if (AlphaCullEnabled == Enabled)
		return;

	AlphaCullEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNStandardMaterial::GetAlphaCullEnabled() const
{
	return AlphaCullEnabled;
}

void ZERNStandardMaterial::SetAlphaCullLimit(float Limit)
{
	if (Constants.AlphaCullLimit == Limit)
		return;

	Constants.AlphaCullLimit = Limit;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetAlphaCullLimit() const
{
	return Constants.AlphaCullLimit;
}

void ZERNStandardMaterial::SetSubSurfaceScatteringFactor(float Factor)
{
	if (Constants.SubSurfaceScatteringFactor == Factor)
		return;

	Constants.SubSurfaceScatteringFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetSubSurfaceScatteringFactor() const
{
	return Constants.SubSurfaceScatteringFactor;
}

void ZERNStandardMaterial::SetSubSurfaceScatteringMapEnabled(bool Enabled)
{
	if (SubSurfaceScatteringMapEnabled == Enabled)
		return;

	SubSurfaceScatteringMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNStandardMaterial::GetSubSurfaceScatteringMapEnabled() const
{
	return SubSurfaceScatteringMapEnabled;
}

void ZERNStandardMaterial::SetSubSurfaceScatteringMap(const ZEGRTexture* Texture)
{
	UnregisterExternalResource(Texture);
	SubSurfaceScatteringMap = Texture;
	RegisterExternalResource(Texture);
	
	SubSurfaceScatteringMapFileName = (SubSurfaceScatteringMap != NULL ? SubSurfaceScatteringMap->GetFileName() : "");
}

const ZEGRTexture* ZERNStandardMaterial::GetSubSurfaceScatteringMap() const
{
	return SubSurfaceScatteringMap;
}

void ZERNStandardMaterial::SetSubSurfaceScatteringMapFile(const ZEString& FileName)
{
	if (ZEPathInfo::Compare(SubSurfaceScatteringMapFileName, FileName))
		return;

	SubSurfaceScatteringMapFileName = FileName;

	if (SubSurfaceScatteringMapFileName.IsEmpty())
		SubSurfaceScatteringMap.Release();

	if (IsLoadedOrLoading() && !SubSurfaceScatteringMapFileName.IsEmpty())
	{
		ZEGRTextureOptions TextureOptions;
		TextureOptions.Type = ZEGR_TT_2D;
		TextureOptions.CompressionFormat = ZEGR_TF_BC4_UNORM;
		TextureOptions.GenerateMipMaps = true;
		TextureOptions.MaximumMipmapLevel = 0;
		TextureOptions.sRGB = false;
		UnregisterExternalResource(SubSurfaceScatteringMap);
		SubSurfaceScatteringMap = ZEGRTexture::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(SubSurfaceScatteringMap);
	}
}

const ZEString& ZERNStandardMaterial::GetSubSurfaceScatteringMapFile() const
{
	return SubSurfaceScatteringMapFileName;
}

void ZERNStandardMaterial::SetBaseMapEnabled(bool Enabled)
{
	if (BaseMapEnabled == Enabled)
		return;
	
	BaseMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNStandardMaterial::GetBaseMapEnabled() const
{
	return BaseMapEnabled;
}

void ZERNStandardMaterial::SetBaseMap(const ZEGRTexture* Texture)
{
	UnregisterExternalResource(BaseMap);
	BaseMap = Texture;
	RegisterExternalResource(BaseMap);
	
	BaseMapFileName = (BaseMap != NULL ? BaseMap->GetFileName() : "");
}

const ZEGRTexture* ZERNStandardMaterial::GetBaseMap() const
{
	return BaseMap;
}

void ZERNStandardMaterial::SetBaseMapFile(const ZEString& FileName)
{
	if (ZEPathInfo::Compare(BaseMapFileName, FileName))
		return;

	BaseMapFileName = FileName;

	if (BaseMapFileName.IsEmpty())
		BaseMap.Release();

	if (IsLoadedOrLoading() && !BaseMapFileName.IsEmpty())
	{
		ZEGRTextureOptions TextureOptions;
		TextureOptions.Type = ZEGR_TT_2D;
		TextureOptions.CompressionFormat = ZEGR_TF_BC1_UNORM_SRGB;
		TextureOptions.MaximumMipmapLevel = GetMaxTextureLOD();
		TextureOptions.GenerateMipMaps = (TextureOptions.MaximumMipmapLevel != 1) ? true : false;
		TextureOptions.sRGB = true;

		UnregisterExternalResource(BaseMap);
		BaseMap = ZEGRTexture::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(BaseMap);
	}
}

const ZEString& ZERNStandardMaterial::GetBaseMapFile() const
{
	return BaseMapFileName;
}

void ZERNStandardMaterial::SetAmbientEnabled(bool Enabled)
{
	if (AmbientEnabled == Enabled)
		return;

	AmbientEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

bool ZERNStandardMaterial::GetAmbientEnabled() const
{
	return AmbientEnabled;
}

void ZERNStandardMaterial::SetSceneAmbientEnabled(bool Enabled)
{
	if ((Constants.SceneAmbientEnabled != 0) == Enabled)
		return;

	Constants.SceneAmbientEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

bool ZERNStandardMaterial::GetSceneAmbientEnabled() const
{
	return Constants.SceneAmbientEnabled != 0;
}

void ZERNStandardMaterial::SetAmbientFactor(float Factor)
{
	if (AmbientFactor == Factor)
		return;

	AmbientFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetAmbientFactor() const
{
	return AmbientFactor;
}

void ZERNStandardMaterial::SetAmbientColor(const ZEVector3& Color)
{
	if (AmbientColor == Color)
		return;

	AmbientColor = Color;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNStandardMaterial::GetAmbientColor() const
{
	return AmbientColor;
}

void ZERNStandardMaterial::SetDiffuseEnabled(bool Enabled)
{
	if (DiffuseEnabled == Enabled)
		return;

	DiffuseEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

bool ZERNStandardMaterial::GetDiffuseEnabled() const
{
	return DiffuseEnabled;
}

void ZERNStandardMaterial::SetDiffuseFactor(float Factor)
{
	if (DiffuseFactor == Factor)
		return;

	DiffuseFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetDiffuseFactor() const
{
	return DiffuseFactor;
}

void ZERNStandardMaterial::SetDiffuseColor(const ZEVector3& Color)
{
	if (DiffuseColor == Color)
		return;

	DiffuseColor = Color;
	
	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNStandardMaterial::GetDiffuseColor() const
{
	return DiffuseColor;
}

void ZERNStandardMaterial::SetSpecularEnabled(bool Enabled)
{
	if (SpecularEnabled == Enabled)
		return;

	SpecularEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

bool ZERNStandardMaterial::GetSpecularEnabled() const
{
	return SpecularEnabled;
}

void ZERNStandardMaterial::SetSpecularFactor(float Factor)
{
	if (SpecularFactor == Factor)
		return;

	SpecularFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetSpecularFactor() const
{
	return SpecularFactor;
}

void ZERNStandardMaterial::SetSpecularColor(const ZEVector3& Color)
{
	if (SpecularColor == Color)
		return;

	SpecularColor = Color;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNStandardMaterial::GetSpecularColor() const
{
	return SpecularColor;
}

void ZERNStandardMaterial::SetSpecularShininess(float Power)
{
	if (Constants.SpecularPower == Power)
		return;

	Constants.SpecularPower = Power;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetSpecularShininess() const
{
	return Constants.SpecularPower;
}

void ZERNStandardMaterial::SetSpecularMapEnabled(bool Enabled)
{
	if (SpecularMapEnabled == Enabled)
		return;

	SpecularMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNStandardMaterial::GetSpecularMapEnabled() const
{
	return SpecularMapEnabled;		
}

void ZERNStandardMaterial::SetSpecularMap(const ZEGRTexture* Texture)
{
	UnregisterExternalResource(SpecularMap);
	SpecularMap = Texture;
	RegisterExternalResource(SpecularMap);
	
	SpecularMapFileName = (SpecularMap != NULL ? SpecularMap->GetFileName() : "");
}

const ZEGRTexture* ZERNStandardMaterial::GetSpecularMap() const
{
	return SpecularMap;
}

void ZERNStandardMaterial::SetSpecularMapFile(const ZEString& FileName)
{
	if (ZEPathInfo::Compare(SpecularMapFileName, FileName))
		return;

	SpecularMapFileName = FileName;

	if (SpecularMapFileName.IsEmpty())
		SpecularMap.Release();

	if (IsLoadedOrLoading() && !SpecularMapFileName.IsEmpty())
	{
		ZEGRTextureOptions TextureOptions;
		TextureOptions.Type = ZEGR_TT_2D;
		TextureOptions.CompressionFormat = ZEGR_TF_BC4_UNORM;
		TextureOptions.MaximumMipmapLevel = GetMaxTextureLOD();
		TextureOptions.GenerateMipMaps = (TextureOptions.MaximumMipmapLevel != 1) ? true : false;
		TextureOptions.sRGB = true;

		UnregisterExternalResource(SpecularMap);
		SpecularMap = ZEGRTexture::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(SpecularMap);
	}
}

const ZEString& ZERNStandardMaterial::GetSpecularMapFile() const
{
	return SpecularMapFileName;
}

void ZERNStandardMaterial::SetSpecularGlossMapEnabled(bool Enabled)
{
	if (SpecularGlossMapEnabled == Enabled)
		return;

	SpecularGlossMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNStandardMaterial::GetSpecularGlossMapEnabled() const
{
	return SpecularGlossMapEnabled;
}

void ZERNStandardMaterial::SetSpecularGlossMap(const ZEGRTexture* Texture)
{
	UnregisterExternalResource(SpecularGlossMap);
	SpecularGlossMap = Texture;
	RegisterExternalResource(SpecularGlossMap);

	SpecularGlossMapFile = (SpecularGlossMap != NULL ? SpecularGlossMap->GetFileName() : "");
}

const ZEGRTexture* ZERNStandardMaterial::GetSpecularGlossMap() const
{
	return SpecularGlossMap;
}

void ZERNStandardMaterial::SetSpecularGlossMapFile(const ZEString& FileName)
{
	if (ZEPathInfo::Compare(SpecularGlossMapFile, FileName))
		return;

	SpecularGlossMapFile = FileName;

	if (SpecularGlossMapFile.IsEmpty())
		SpecularGlossMap.Release();

	if (IsLoadedOrLoading() && !SpecularGlossMapFile.IsEmpty())
	{
		ZEGRTextureOptions TextureOptions;
		TextureOptions.Type = ZEGR_TT_2D;
		TextureOptions.CompressionFormat = ZEGR_TF_BC4_UNORM;
		TextureOptions.GenerateMipMaps = true;
		TextureOptions.MaximumMipmapLevel = 0;
		TextureOptions.sRGB = false;

		UnregisterExternalResource(SpecularGlossMap);
		SpecularGlossMap = ZEGRTexture::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(SpecularGlossMap);
	}
}

const ZEString& ZERNStandardMaterial::GetSpecularGlossMapFile() const
{
	return SpecularGlossMapFile;
}

void ZERNStandardMaterial::SetEmissiveEnabled(bool Enabled)
{
	if (EmissiveEnabled == Enabled)
		return;

	EmissiveEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

bool ZERNStandardMaterial::GetEmissiveEnabled() const
{
	return EmissiveEnabled;
}

void ZERNStandardMaterial::SetEmissiveFactor(float Factor)
{
	if (EmissiveFactor == Factor)
		return;

	EmissiveFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetEmissiveFactor() const
{
	return EmissiveFactor;
}

void ZERNStandardMaterial::SetEmissiveColor(const ZEVector3& Color)
{
	if (EmissiveColor == Color)
		return;

	EmissiveColor = Color;
	
	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNStandardMaterial::GetEmissiveColor() const
{
	return EmissiveColor;
}

void ZERNStandardMaterial::SetEmissiveMapEnabled(bool Enabled)
{
	if (EmissiveMapEnabled == Enabled)
		return;

	EmissiveMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNStandardMaterial::GetEmissiveMapEnabled() const
{
	return EmissiveMapEnabled;
}

void ZERNStandardMaterial::SetEmissiveMap(const ZEGRTexture* Texture)
{
	UnregisterExternalResource(EmissiveMap);
	EmissiveMap = Texture;
	RegisterExternalResource(EmissiveMap);

	EmissiveMapFileName = (EmissiveMap != NULL ? EmissiveMap->GetFileName() : "");
}

const ZEGRTexture* ZERNStandardMaterial::GetEmissiveMap() const
{
	return EmissiveMap;
}

void ZERNStandardMaterial::SetEmissiveMapFile(const ZEString& FileName)
{
	if (ZEPathInfo::Compare(EmissiveMapFileName, FileName))
		return;

	EmissiveMapFileName = FileName;
	
	if (EmissiveMapFileName.IsEmpty())
		EmissiveMap.Release();

	if (IsLoadedOrLoading() && !EmissiveMapFileName.IsEmpty())
	{
		ZEGRTextureOptions TextureOptions;
		TextureOptions.Type = ZEGR_TT_2D;
		TextureOptions.CompressionFormat = ZEGR_TF_BC1_UNORM_SRGB;
		TextureOptions.MaximumMipmapLevel = GetMaxTextureLOD();
		TextureOptions.GenerateMipMaps = (TextureOptions.MaximumMipmapLevel != 1) ? true : false;
		TextureOptions.sRGB = true;

		UnregisterExternalResource(EmissiveMap);
		EmissiveMap = ZEGRTexture::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(EmissiveMap);
	}
}

const ZEString& ZERNStandardMaterial::GetEmissiveMapFile() const
{
	return EmissiveMapFileName;
}

void ZERNStandardMaterial::SetNormalMapEnabled(bool Enabled)
{
	if (NormalMapEnabled == Enabled)
		return;

	NormalMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNStandardMaterial::GetNormalMapEnabled() const
{
	return NormalMapEnabled;
}

void ZERNStandardMaterial::SetNormalMap(const ZEGRTexture* Texture)
{
	UnregisterExternalResource(NormalMap);
	NormalMap = Texture;
	RegisterExternalResource(NormalMap);
	
	NormalMapFileName = (NormalMap != NULL ? NormalMap->GetFileName() : "");
}

const ZEGRTexture* ZERNStandardMaterial::GetNormalMap() const
{
	return NormalMap;
}

void ZERNStandardMaterial::SetNormalMapFile(const ZEString& FileName)
{
	if (ZEPathInfo::Compare(NormalMapFileName, FileName))
		return;
	
	NormalMapFileName = FileName;

	if (NormalMapFileName.IsEmpty())
		NormalMap.Release();

	if (IsLoadedOrLoading() && !NormalMapFileName.IsEmpty())
	{
		ZEGRTextureOptions TextureOptions;
		TextureOptions.Type = ZEGR_TT_2D;
		TextureOptions.CompressionFormat = ZEGR_TF_BC5_UNORM;
		TextureOptions.MaximumMipmapLevel = GetMaxTextureLOD();
		TextureOptions.GenerateMipMaps = (TextureOptions.MaximumMipmapLevel != 1) ? true : false;
		TextureOptions.sRGB = false;

		UnregisterExternalResource(NormalMap);
		NormalMap = ZEGRTexture::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(NormalMap);
	}
}

const ZEString& ZERNStandardMaterial::GetNormalMapFile() const
{
	return NormalMapFileName;
}

void ZERNStandardMaterial::SetHeightMapEnabled(bool Enabled)
{
	if (HeightMapEnabled == Enabled)
		return;

	HeightMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNStandardMaterial::GetHeightMapEnabled() const
{
	return HeightMapEnabled;
}

void ZERNStandardMaterial::SetHeightMapTechnique(ZERNHeightMapTechnique Technique)
{
	if (HeightMapTechnique == Technique)
		return;

	HeightMapTechnique = Technique;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

ZERNHeightMapTechnique ZERNStandardMaterial::GetHeightMapTechnique() const
{
	return HeightMapTechnique;
}

void ZERNStandardMaterial::SetHeightMapOffset(float Offset)
{
	if (Constants.HeightMapOffset == Offset)
		return;

	Constants.HeightMapOffset = Offset;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetHeightMapOffset() const
{
	return Constants.HeightMapOffset;
}

void ZERNStandardMaterial::SetHeightMapScale(float Scale)
{
	if (Constants.HeightMapScale == Scale)
		return;

	Constants.HeightMapScale = Scale;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetHeightMapScale() const
{
	return Constants.HeightMapScale;
}

void ZERNStandardMaterial::SetHeightMap(const ZEGRTexture* Texture)
{
	UnregisterExternalResource(HeightMap);
	HeightMap = Texture;
	RegisterExternalResource(HeightMap);

	HeightMapFileName = (HeightMap != NULL ? HeightMap->GetFileName() : "");
}

const ZEGRTexture* ZERNStandardMaterial::GetHeightMap() const
{
	return HeightMap;
}

void ZERNStandardMaterial::SetHeightMapFile(const ZEString& FileName)
{
	if (ZEPathInfo::Compare(HeightMapFileName, FileName))
		return;

	HeightMapFileName = FileName;

	if (HeightMapFileName.IsEmpty())
		HeightMap.Release();

	if (IsLoadedOrLoading() && !HeightMapFileName.IsEmpty())
	{
		ZEGRTextureOptions TextureOptions;
		TextureOptions.Type = ZEGR_TT_2D;
		TextureOptions.CompressionFormat = ZEGR_TF_BC4_UNORM;
		TextureOptions.GenerateMipMaps = true;
		TextureOptions.MaximumMipmapLevel = 0;
		TextureOptions.sRGB = false;

		UnregisterExternalResource(HeightMap);
		HeightMap = ZEGRTexture::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(HeightMap);
	}
}

const ZEString& ZERNStandardMaterial::GetHeightMapFile() const
{
	return HeightMapFileName;
}

void ZERNStandardMaterial::SetOpacity(float Value)
{
	if (Constants.Opacity == Value)
		return;

	Constants.Opacity = Value;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetOpacity() const
{
	return Constants.Opacity;
}

void ZERNStandardMaterial::SetOpacityMapEnabled(bool Enabled)
{
	if (OpacityMapEnabled == Enabled)
		return;

	OpacityMapEnabled = true;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS | ZERN_FMDF_STAGE_MASK);
}
bool ZERNStandardMaterial::GetOpacityMapEnabled() const
{
	return OpacityMapEnabled;
}

void ZERNStandardMaterial::SetOpacityMap(const ZEGRTexture* Texture)
{
	UnregisterExternalResource(OpacityMap);
	OpacityMap = Texture;
	RegisterExternalResource(OpacityMap);

	OpacityMapFileName = (OpacityMap != NULL ? OpacityMap->GetFileName() : "");
}

const ZEGRTexture* ZERNStandardMaterial::GetOpacityMap() const
{
	return OpacityMap;
}

void ZERNStandardMaterial::SetOpacityMapFile(const ZEString& FileName)
{
	if (ZEPathInfo::Compare(OpacityMapFileName, FileName))
		return;

	OpacityMapFileName = FileName;

	if (OpacityMapFileName.IsEmpty())
		OpacityMap.Release();

	if (IsLoadedOrLoading() && !OpacityMapFileName.IsEmpty())
	{
		ZEGRTextureOptions TextureOptions;
		TextureOptions.Type = ZEGR_TT_2D;
		TextureOptions.CompressionFormat = ZEGR_TF_BC4_UNORM;
		TextureOptions.MaximumMipmapLevel = GetMaxTextureLOD();
		TextureOptions.GenerateMipMaps = (TextureOptions.MaximumMipmapLevel != 1) ? true : false;
		TextureOptions.sRGB = true;

		UnregisterExternalResource(OpacityMap);
		OpacityMap = ZEGRTexture::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(OpacityMap);
	}
}

const ZEString& ZERNStandardMaterial::GetOpacityMapFile() const
{
	return OpacityMapFileName;
}

void ZERNStandardMaterial::SetEnvironmentMapEnabled(bool Enabled)
{
	if (EnvironmentMapEnabled == Enabled)
		return;

	EnvironmentMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNStandardMaterial::GetEnvironmentMapEnabled() const
{
	return EnvironmentMapEnabled;
}

void ZERNStandardMaterial::SetEnvironmentMap(const ZEGRTexture* Texture)
{
	UnregisterExternalResource(EnvironmentMap);
	EnvironmentMap = Texture;
	RegisterExternalResource(EnvironmentMap);

	EnvironmentMapFileName = (EnvironmentMap != NULL ? EnvironmentMap->GetFileName() : "");
}

const ZEGRTexture* ZERNStandardMaterial::GetEnvironmentMap() const
{
	return EnvironmentMap;
}

void ZERNStandardMaterial::SetEnvironmentMapFile(const ZEString& FileName)
{
	if (ZEPathInfo::Compare(EnvironmentMapFileName, FileName))
		return;

	EnvironmentMapFileName = FileName;

	if (EnvironmentMapFileName.IsEmpty())
		EnvironmentMap.Release();

	if (IsLoadedOrLoading() && !EnvironmentMapFileName.IsEmpty())
	{
		ZEGRTextureOptions TextureOptions;
		TextureOptions.Type = ZEGR_TT_2D;
		TextureOptions.CompressionFormat = ZEGR_TF_BC1_UNORM_SRGB;
		TextureOptions.GenerateMipMaps = true;
		TextureOptions.MaximumMipmapLevel = 0;
		TextureOptions.sRGB = true;

		UnregisterExternalResource(EnvironmentMap);
		EnvironmentMap = ZEGRTexture::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(EnvironmentMap);
	}
}

const ZEString& ZERNStandardMaterial::GetEnvironmentMapFile() const
{
	return EnvironmentMapFileName;
}

void ZERNStandardMaterial::SetReflectionEnabled(bool Enabled)
{
	if (ReflectionEnabled == Enabled)
		return;

	ReflectionEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS | ZERN_FMDF_CONSTANT_BUFFER);
}

bool ZERNStandardMaterial::GetReflectionEnabled() const
{
	return ReflectionEnabled;
}

void ZERNStandardMaterial::SetReflectionFactor(float Factor)
{
	if (ReflectionFactor == Factor)
		return;

	ReflectionFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetReflectionFactor() const
{
	return ReflectionFactor;
}

void ZERNStandardMaterial::SetReflectionColor(const ZEVector3& Color)
{
	if (ReflectionColor == Color)
		return;

	ReflectionColor = Color;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNStandardMaterial::GetReflectionColor() const
{
	return ReflectionColor;
}

void ZERNStandardMaterial::SetRefractionEnabled(bool Enabled)
{
	if (RefractionEnabled == Enabled)
		return;

	RefractionEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS | ZERN_FMDF_CONSTANT_BUFFER);
}

bool ZERNStandardMaterial::GetRefractionEnabled() const
{
	return RefractionEnabled;
}

void ZERNStandardMaterial::SetRefractionFactor(float Factor)
{
	if (RefractionFactor == Factor)
		return;

	RefractionFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetRefractionFactor() const
{
	return RefractionFactor;
}

void ZERNStandardMaterial::SetRefractionColor(const ZEVector3& Color)
{
	if (RefractionColor == Color)
		return;

	RefractionColor = Color;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNStandardMaterial::GetRefractionColor() const
{
	return RefractionColor;
}

void ZERNStandardMaterial::SetRefractionIndex(float Index)
{
	if (Constants.RefractionIndex == Index)
		return;

	Constants.RefractionIndex = Index;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetRefractionIndex() const
{
	return Constants.RefractionIndex;
}

void ZERNStandardMaterial::SetDetailMapTiling(const ZEVector2& Tiling)
{
	if (Constants.DetailBaseMapTiling == Tiling && Constants.DetailNormalMapTiling == Tiling)
		return;

	Constants.DetailBaseMapTiling = Tiling;
	Constants.DetailNormalMapTiling = Tiling;
	
	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNStandardMaterial::GetDetailMapTiling() const
{
	return Constants.DetailBaseMapTiling;
}

void ZERNStandardMaterial::SetDetailBaseMapEnabled(bool Enabled)
{
	if (DetailBaseMapEnabled == Enabled)
		return;

	DetailBaseMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNStandardMaterial::GetDetailBaseMapEnabled() const
{
	return DetailBaseMapEnabled;
}

void ZERNStandardMaterial::SetDetailBaseMapColor(const ZEVector3& Color)
{
	if (Constants.DetailBaseMapColor == Color)
		return;

	Constants.DetailBaseMapColor = Color;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNStandardMaterial::GetDetailBaseMapColor() const
{
	return Constants.DetailBaseMapColor;
}

void ZERNStandardMaterial::SetDetailBaseMapTiling(const ZEVector2& Tiling)
{
	if (Constants.DetailBaseMapTiling == Tiling)
		return;

	Constants.DetailBaseMapTiling = Tiling;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNStandardMaterial::GetDetailBaseMapTiling() const
{
	return Constants.DetailBaseMapTiling;
}

void ZERNStandardMaterial::SetDetailBaseMapAttenuationStart(float Start)
{
	if (Constants.DetailBaseMapAttenuationStart == Start)
		return;

	Constants.DetailBaseMapAttenuationStart = Start;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetDetailBaseMapAttenuationStart() const
{
	return Constants.DetailBaseMapAttenuationStart;
}

void ZERNStandardMaterial::SetDetailBaseMapAttenuationFactor(float Factor)
{
	if (Constants.DetailBaseMapAttenuationFactor == Factor)
		return;

	Constants.DetailBaseMapAttenuationFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetDetailBaseMapAttenuationFactor() const
{
	return Constants.DetailBaseMapAttenuationFactor;
}

void ZERNStandardMaterial::SetDetailBaseMap(const ZEGRTexture* Texture)
{
	UnregisterExternalResource(DetailBaseMap);
	DetailBaseMap = Texture;
	RegisterExternalResource(DetailBaseMap);

	DetailBaseMapFileName = (DetailBaseMap != NULL ? DetailBaseMap->GetFileName() : "");
}

const ZEGRTexture* ZERNStandardMaterial::GetDetailBaseMap() const
{
	return DetailBaseMap;
}

void ZERNStandardMaterial::SetDetailBaseMapFile(const ZEString& FileName)
{
	if (ZEPathInfo::Compare(DetailBaseMapFileName, FileName))
		return;

	DetailBaseMapFileName = FileName;

	if (DetailBaseMapFileName.IsEmpty())
		DetailBaseMap.Release();

	if (IsLoadedOrLoading() && !DetailBaseMapFileName.IsEmpty())
	{
		ZEGRTextureOptions TextureOptions;
		TextureOptions.Type = ZEGR_TT_2D;
		TextureOptions.CompressionFormat = ZEGR_TF_BC1_UNORM_SRGB;
		TextureOptions.GenerateMipMaps = true;
		TextureOptions.MaximumMipmapLevel = 0;
		TextureOptions.sRGB = true;

		UnregisterExternalResource(DetailBaseMap);
		DetailBaseMap = ZEGRTexture::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(DetailBaseMap);
	}
}

const ZEString& ZERNStandardMaterial::GetDetailBaseMapFile() const
{
	return DetailBaseMapFileName;
}

void ZERNStandardMaterial::SetDetailNormalMapEnabled(bool Enabled)
{
	if (DetailNormalMapEnabled == Enabled)
		return;

	DetailNormalMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNStandardMaterial::GetDetailNormalMapEnabled() const
{
	return DetailNormalMapEnabled;
}

void ZERNStandardMaterial::SetDetailNormalMapFactor(float Factor)
{
	if (Constants.DetailNormalMapFactor == Factor)
		return;

	Constants.DetailNormalMapFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetDetailNormalMapFactor() const
{
	return Constants.DetailNormalMapFactor;
}

void ZERNStandardMaterial::SetDetailNormalMapTiling(const ZEVector2& Tiling)
{
	if (Constants.DetailNormalMapTiling == Tiling)
		return;

	Constants.DetailNormalMapTiling = Tiling;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNStandardMaterial::GetDetailNormalMapTiling() const
{
	return Constants.DetailNormalMapTiling;
}

void ZERNStandardMaterial::SetDetailNormalMapAttenuationStart(float Start)
{
	if (Constants.DetailNormalMapAttenuationStart == Start)
		return;

	Constants.DetailNormalMapAttenuationStart = Start;
	
	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetDetailNormalMapAttenuationStart() const
{
	return Constants.DetailNormalMapAttenuationStart;
}

void ZERNStandardMaterial::SetDetailNormalMapAttenuationFactor(float Factor)
{
	if (Constants.DetailNormalMapAttenuationFactor == Factor)
		return;

	Constants.DetailNormalMapAttenuationFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNStandardMaterial::GetDetailNormalMapAttenuationFactor() const
{
	return Constants.DetailNormalMapAttenuationFactor;
}

void ZERNStandardMaterial::SetDetailNormalMap(const ZEGRTexture* Texture)
{
	UnregisterExternalResource(DetailNormalMap);
	DetailNormalMap = Texture;
	RegisterExternalResource(DetailNormalMap);

	DetailNormalMapFileName = (DetailNormalMap != NULL ? DetailNormalMap->GetFileName() : "");
}

const ZEGRTexture* ZERNStandardMaterial::GetDetailNormalMap() const
{
	return DetailNormalMap;
}

void ZERNStandardMaterial::SetDetailNormalMapFile(const ZEString& FileName)
{
	if (ZEPathInfo::Compare(DetailNormalMapFileName, FileName))
		return;

	DetailNormalMapFileName = FileName;

	if (DetailNormalMapFileName.IsEmpty())
		DetailNormalMap.Release();

	if (IsLoadedOrLoading() && !DetailNormalMapFileName.IsEmpty())
	{
		ZEGRTextureOptions TextureOptions;
		TextureOptions.Type = ZEGR_TT_2D;
		TextureOptions.CompressionFormat = ZEGR_TF_BC5_UNORM;
		TextureOptions.GenerateMipMaps = true;
		TextureOptions.MaximumMipmapLevel = 0;
		TextureOptions.sRGB = false;

		UnregisterExternalResource(DetailNormalMap);
		DetailNormalMap = ZEGRTexture::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(DetailNormalMap);
	}
}

const ZEString& ZERNStandardMaterial::GetDetailNormalMapFile() const
{
	return DetailNormalMapFileName;
}

void ZERNStandardMaterial::SetClippingPlanesEnabled(bool Enabled)
{
	ClippingPlanesEnabled = Enabled;
}

bool ZERNStandardMaterial::GetClippingPlanesEnabled() const
{
	return ClippingPlanesEnabled;
}

bool ZERNStandardMaterial::PreRender(ZERNCommand& Command) const
{
	if (!ZERNMaterial::PreRender(Command))
		return false;

	if (GetTransparencyEnabled())
		Command.Order = -Command.Order;

	if (Command.Entity->GetClass() == ZEInterior::Class())
	{
		if (!UseInteriorVertexLayout)
		{
			UseInteriorVertexLayout = true;
			DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
		}
	}

	return true;
}

bool ZERNStandardMaterial::SetupMaterial(ZEGRContext* Context, const ZERNStage* Stage, bool Instanced, bool LODTransitionEnabled) const
{
	zeDebugCheck(Instanced && SkinningEnabled, "Cannot setup skinned material for instanced draw");

	if (!ZERNMaterial::SetupMaterial(Context, Stage))
		return false;

	if (!const_cast<ZERNStandardMaterial*>(this)->Update())
		return false;

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_MATERIAL, ConstantBuffer);

	const ZEGRSampler* Samplers[] = {Sampler, Sampler, Sampler, Sampler};
	Context->SetSamplers(ZEGR_ST_PIXEL, 0, 4, Samplers);

	ZEUInt StageID = Stage->GetId();
	if (StageID == ZERN_STAGE_GBUFFER || StageID == ZERN_STAGE_FORWARD_TRANSPARENT)
	{
		Context->SetRenderState(Instanced ? StageGBuffer_Forward_Instancing_RenderState : StageGBuffer_Forward_RenderState);

		const ZEGRTexture* Textures[] = 
		{
			BaseMap,
			NormalMap,
			SpecularMap,
			EmissiveMap,
			HeightMap,
			OpacityMap,
			SubSurfaceScatteringMap,
			EnvironmentMap,
			DetailBaseMap,
			DetailNormalMap,
			SpecularGlossMap
		};
		Context->SetTextures(ZEGR_ST_PIXEL, 0, 11, Textures);
	}
	else if (StageID == ZERN_STAGE_SHADOW_MAP_GENERATION)
	{
		Context->SetRenderState(Instanced ? StageShadowmapGeneration_Instancing_RenderState : StageShadowmapGeneration_RenderState);

		if (AlphaCullEnabled || LODTransitionEnabled)
		{
			if (OpacityMapEnabled)
				Context->SetTexture(ZEGR_ST_PIXEL, 5, OpacityMap);
			else if (BaseMapEnabled)
				Context->SetTexture(ZEGR_ST_PIXEL, 0, BaseMap);
		}
	}
	else if (StageID == ZERN_STAGE_RENDER_DEPTH)
	{
		Context->SetRenderState(Instanced ? StageRenderDepth_Instancing_RenderState : StageRenderDepth_RenderState);

		if (AlphaCullEnabled || LODTransitionEnabled)
		{
			if (OpacityMapEnabled)
				Context->SetTexture(ZEGR_ST_PIXEL, 5, OpacityMap);
			else if (BaseMapEnabled)
				Context->SetTexture(ZEGR_ST_PIXEL, 0, BaseMap);
		}
	}

	return true;
}

void ZERNStandardMaterial::CleanupMaterial(ZEGRContext* Context, const ZERNStage* Stage, bool Instanced, bool LODTransitionEnabled) const
{
	ZERNMaterial::CleanupMaterial(Context, Stage);
}

bool ZERNStandardMaterial::Serialize(ZEMLWriterNode* MaterialNode)
{
	MaterialNode->WriteString("Class", GetClass()->GetName());
	MaterialNode->WriteString("GUID", GetGUID().ToString());
	MaterialNode->WriteUInt8("VersionMajor", 2);
	MaterialNode->WriteUInt8("VersionMinor", 2);

	ZEMLWriterNode PropertiesNode;
	MaterialNode->OpenNode("Properties", PropertiesNode);

	PropertiesNode.WriteString("Name", GetName().ToCString()); //Will be changed when configuration is implemented.
	PropertiesNode.WriteBool("TwoSided", GetTwoSided());
	PropertiesNode.WriteBool("Wireframe", GetWireframe());
	PropertiesNode.WriteBool("VertexColorEnabled", GetVertexColorEnabled());
	PropertiesNode.WriteBool("SkinningEnabled", GetSkinningEnabled());

	PropertiesNode.WriteBool("TransparencyEnabled", GetTransparencyEnabled());
	PropertiesNode.WriteInt32("TransparencyMode", (ZEInt32)GetTransparencyMode());
	PropertiesNode.WriteBool("AlphaCullEnabled", GetAlphaCullEnabled());
	PropertiesNode.WriteFloat("AlphaCullLimit", GetAlphaCullLimit());

	PropertiesNode.WriteBool("BaseMapEnabled", GetBaseMapEnabled());
	PropertiesNode.WriteString("BaseMap", BaseMap->GetName());

	PropertiesNode.WriteBool("DiffuseEnabled", GetDiffuseEnabled());
	PropertiesNode.WriteVector3("DiffuseColor", GetDiffuseColor());
	PropertiesNode.WriteFloat("DiffuseFactor", GetDiffuseFactor());

	PropertiesNode.WriteBool("NormalMapEnabled", GetNormalMapEnabled());
	PropertiesNode.WriteString("NormalMap", NormalMap->GetName());

	PropertiesNode.WriteBool("SpecularEnabled", GetSpecularEnabled());
	PropertiesNode.WriteVector3("SpecularColor", GetSpecularColor());
	PropertiesNode.WriteFloat("SpecularFactor", GetSpecularFactor());
	PropertiesNode.WriteBool("SpecularMapEnabled", GetSpecularMapEnabled());
	PropertiesNode.WriteString("SpecularMap", SpecularMap->GetName());
	PropertiesNode.WriteBool("SpecularGlossMapEnabled", GetSpecularGlossMapEnabled());
	PropertiesNode.WriteString("SpecularGlossMap", SpecularGlossMap->GetName());

	PropertiesNode.WriteBool("AmbientEnabled", GetAmbientEnabled());
	PropertiesNode.WriteBool("SceneAmbientEnabled", GetSceneAmbientEnabled());
	PropertiesNode.WriteVector3("AmbientColor", GetAmbientColor());
	PropertiesNode.WriteFloat("AmbientFactor", GetAmbientFactor());

	PropertiesNode.WriteBool("EmissiveEnabled", GetEmissiveEnabled());
	PropertiesNode.WriteVector3("EmissiveColor", GetEmissiveColor());
	PropertiesNode.WriteFloat("EmissiveFactor", GetEmissiveFactor());
	PropertiesNode.WriteBool("EmissiveMapEnabled", GetEmissiveMapEnabled());
	PropertiesNode.WriteString("EmissiveMap", EmissiveMap->GetName());

	PropertiesNode.WriteBool("HeightMapEnabled", GetHeightMapEnabled());
	PropertiesNode.WriteUInt8("HeightMapTechnique", GetHeightMapEnabled());
	PropertiesNode.WriteFloat("HeightMapOffset", GetHeightMapOffset());
	PropertiesNode.WriteFloat("HeightMapScale", GetHeightMapScale());
	PropertiesNode.WriteString("HeightMap", HeightMap->GetName());

	PropertiesNode.WriteBool("SubSurfaceScatteringMapEnabled", GetSubSurfaceScatteringMapEnabled());
	PropertiesNode.WriteString("SubSurfaceScatteringMap", SubSurfaceScatteringMap->GetName());
	PropertiesNode.WriteFloat("SubSurfaceScatteringFactor", GetSubSurfaceScatteringFactor());

	PropertiesNode.WriteFloat("Opacity", GetOpacity());
	PropertiesNode.WriteBool("OpacityMapEnabled", GetOpacityMapEnabled());
	PropertiesNode.WriteString("OpacityMap", OpacityMap->GetName());

	PropertiesNode.WriteBool("EnvironmentEnabled", GetOpacityMapEnabled());
	PropertiesNode.WriteString("EnvironmentMap", EnvironmentMap->GetName());

	PropertiesNode.WriteBool("ReflectionEnabled", GetReflectionEnabled());
	PropertiesNode.WriteVector3("ReflectionColor", GetReflectionColor());
	PropertiesNode.WriteFloat("ReflectionFactor", GetReflectionFactor());
	PropertiesNode.WriteBool("RefractionEnabled", GetRefractionEnabled());
	PropertiesNode.WriteVector3("RefractionColor", GetRefractionColor());
	PropertiesNode.WriteFloat("RefractionFactor", GetRefractionFactor());
	PropertiesNode.WriteFloat("RefractionIndex", GetRefractionIndex());
	
	PropertiesNode.WriteBool("DetailBaseMapEnabled", GetDetailBaseMapEnabled());
	PropertiesNode.WriteVector3("DetailBaseMapColor", GetDetailBaseMapColor());
	PropertiesNode.WriteVector2("DetailBaseMapTiling", GetDetailBaseMapTiling());
	PropertiesNode.WriteFloat("DetailBaseMapAttenuationStart", GetDetailBaseMapAttenuationStart());
	PropertiesNode.WriteFloat("DetailBaseMapAttenuationFactor", GetDetailBaseMapAttenuationFactor());
	PropertiesNode.WriteString("DetailBaseMap", DetailBaseMap->GetName());

	PropertiesNode.WriteBool("DetailNormalMapEnabled", GetDetailNormalMapEnabled());
	PropertiesNode.WriteFloat("DetailNormalMapColor", GetDetailNormalMapFactor());
	PropertiesNode.WriteVector2("DetailNormalMapTiling", GetDetailNormalMapTiling());
	PropertiesNode.WriteFloat("DetailNormalMapAttenuationStart", GetDetailNormalMapAttenuationStart());
	PropertiesNode.WriteFloat("DetailNormalMapAttenuationFactor", GetDetailNormalMapAttenuationFactor());
	PropertiesNode.WriteString("DetailNormalMap", DetailNormalMap->GetName());

	PropertiesNode.WriteBool("ClippingPlanesEnabled", GetClippingPlanesEnabled());

	PropertiesNode.CloseNode();

	return true;
}

bool ZERNStandardMaterial::Unserialize(ZEMLReaderNode* MaterialNode)
{
	ZEString FileName = MaterialNode->GetFile()->GetPath();

	zeCheckError(MaterialNode == NULL || !MaterialNode->IsValid(), false, "ZERNStandardMaterial loading failed. ZEML Root Node is not valid. File : \"%s\"", FileName.ToCString());

	ZEUInt8 FileMajorVersion = MaterialNode->ReadUInt8("VersionMajor", 0);
	if (FileMajorVersion < 2)
		zeWarning("ZERNStandardMaterial loading warning. Old depricated file version detected. Current Version: \"2.0\" Detected Version: \"%d.0\" File Name: \"%s\".", FileMajorVersion, FileName.ToCString());
	
	ZEString ClassName = MaterialNode->ReadString("Class");
	if (ClassName.IsEmpty())
		zeWarning("Class tag in ZERNMaterial is empty. Unknown material class. This can cause problems. File Name: \"%s\".", FileName.ToCString());
	else
		zeCheckError(ClassName != GetClass()->GetName(), false, "Cannot load ZERNStandardMaterial. Class is different than ZERNStandartMaterial. Class Name: \"%s\", File Name: \"%s\".", ClassName.ToCString(), FileName.ToCString());
	
	ZEMLReaderNode PropertiesNode;
	if (FileMajorVersion < 2)
		PropertiesNode = MaterialNode->GetNode("Configuration");
	else
		PropertiesNode = MaterialNode->GetNode("Properties");
	
	zeCheckError(!PropertiesNode.IsValid(), false, "ZERNStandardMaterial loading failed. ZEML \"Properties\" Node is not valid. File : \"%s\"", FileName.ToCString());
	
	SetAlphaToCoverageEnabled(PropertiesNode.ReadBoolean("AlphaToCoverageEnabled"));
	SetMaxTextureLOD(PropertiesNode.ReadUInt8("MaxTextureLOD", 0));
	SetInstancingEnabled(PropertiesNode.ReadBoolean("InstancingEnabled"));

	SetGUID(ZEGUID::FromString(MaterialNode->ReadString("GUID")));
	SetName(PropertiesNode.ReadString("Name"));
	SetShadowCaster(PropertiesNode.ReadBoolean("ShadowCaster", false));
	SetWireframe(PropertiesNode.ReadBoolean("Wireframe", false));
	SetTwoSided(PropertiesNode.ReadBoolean("TwoSided", false));
	SetSkinningEnabled(PropertiesNode.ReadBoolean("SkinningEnabled", false));
	SetVertexColorEnabled(PropertiesNode.ReadBoolean("VertexColorEnabled", false));

	SetTransparencyEnabled(PropertiesNode.ReadBoolean("TransparencyEnabled", false));
	SetTransparencyMode((ZERNTransparencyMode)PropertiesNode.ReadInt32("TransparencyMode", ZERN_TM_NONE));
	SetAlphaCullEnabled(PropertiesNode.ReadBoolean("AlphaCullEnabled", false));
	SetAlphaCullLimit(PropertiesNode.ReadFloat("AlphaCullLimit", 0.5f));

	SetBaseMapFile(ZEPathInfo::CombineRelativePath(PropertiesNode.GetFile()->GetPath(), PropertiesNode.ReadString("BaseMap")));
	SetBaseMapEnabled(PropertiesNode.ReadBoolean("BaseMapEnabled", !BaseMap.IsNull()));

	SetDiffuseEnabled(PropertiesNode.ReadBoolean("DiffuseEnabled", true));
	SetDiffuseFactor(PropertiesNode.ReadFloat("DiffuseFactor", 1.0f));
	SetDiffuseColor(PropertiesNode.ReadVector3("DiffuseColor", ZEVector3::One));

	SetNormalMapFile(ZEPathInfo::CombineRelativePath(PropertiesNode.GetFile()->GetPath(), PropertiesNode.ReadString("NormalMap")));
	SetNormalMapEnabled(PropertiesNode.ReadBoolean("NormalMapEnabled", !NormalMap.IsNull()));

	SetSpecularEnabled(PropertiesNode.ReadBoolean("SpecularEnabled", true));
	SetSpecularFactor(PropertiesNode.ReadFloat("SpecularFactor", 1.0f));
	SetSpecularShininess(PropertiesNode.ReadFloat("SpecularShininess", 1.0f));
	SetSpecularColor(PropertiesNode.ReadVector3("SpecularColor", ZEVector3::One));
	SetSpecularMapFile(ZEPathInfo::CombineRelativePath(PropertiesNode.GetFile()->GetPath(), PropertiesNode.ReadString("SpecularMap")));
	SetSpecularMapEnabled(PropertiesNode.ReadBoolean("SpecularMapEnabled", !SpecularMap.IsNull()));
	SetSpecularGlossMapFile(ZEPathInfo::CombineRelativePath(PropertiesNode.GetFile()->GetPath(), PropertiesNode.ReadString("SpecularGlossMap")));
	SetSpecularGlossMapEnabled(PropertiesNode.ReadBoolean("SpecularGlossMapEnabled", !SpecularGlossMap.IsNull()));

	SetAmbientEnabled(PropertiesNode.ReadBoolean("AmbientEnabled", false));
	SetSceneAmbientEnabled(PropertiesNode.ReadBoolean("SceneAmbientEnabled", true));
	SetAmbientFactor(PropertiesNode.ReadFloat("AmbientFactor", 1.0f));
	SetAmbientColor(PropertiesNode.ReadVector3("AmbientColor", ZEVector3::One));

	SetEmissiveEnabled(PropertiesNode.ReadBoolean("EmissiveEnabled", false));
	SetEmissiveFactor(PropertiesNode.ReadFloat("EmissiveFactor", 1.0f));
	SetEmissiveColor(PropertiesNode.ReadVector3("EmissiveColor", ZEVector3::One));
	SetEmissiveMapFile(ZEPathInfo::CombineRelativePath(PropertiesNode.GetFile()->GetPath(), PropertiesNode.ReadString("EmissiveMap")));
	SetEmissiveMapEnabled(PropertiesNode.ReadBoolean("EmissiveMapEnabled", !EmissiveMap.IsNull()));

	SetHeightMapEnabled(PropertiesNode.ReadBoolean("HeightEnabled", false));
	SetHeightMapTechnique((ZERNHeightMapTechnique)PropertiesNode.ReadUInt8("HeigthMapTechnique", ZERN_HMT_NONE));
	SetHeightMapOffset(PropertiesNode.ReadFloat("HeightMapOffset"));
	SetHeightMapScale(PropertiesNode.ReadFloat("HeightMapScale"));

	SetOpacity(PropertiesNode.ReadFloat("Opacity", 1.0f));
	SetOpacityMapFile(ZEPathInfo::CombineRelativePath(PropertiesNode.GetFile()->GetPath(), PropertiesNode.ReadString("OpacityMap")));
	SetOpacityMapEnabled(PropertiesNode.ReadBoolean("OpacityMapEnabled", !OpacityMap.IsNull()));

	SetSubSurfaceScatteringFactor(PropertiesNode.ReadFloat("SubSurfaceScatteringFactor", 0.0f));
	SetSubSurfaceScatteringMapFile(ZEPathInfo::CombineRelativePath(PropertiesNode.GetFile()->GetPath(), PropertiesNode.ReadString("SubSurfaceScatteringMap")));
	SetSubSurfaceScatteringMapEnabled(PropertiesNode.ReadBoolean("SubSurfaceScatteringMapEnabled", !SubSurfaceScatteringMap.IsNull()));

	SetEnvironmentMapFile(ZEPathInfo::CombineRelativePath(PropertiesNode.GetFile()->GetPath(), PropertiesNode.ReadString("EnvironmentMap")));
	SetEnvironmentMapEnabled(PropertiesNode.ReadBoolean("EnvironmentMapEnabled", !EnvironmentMap.IsNull()));
	SetReflectionEnabled(PropertiesNode.ReadBoolean("ReflectionEnabled", false));
	SetReflectionFactor(PropertiesNode.ReadFloat("ReflectionFactor", 1.0f));
	SetReflectionColor(PropertiesNode.ReadVector3("ReflectionColor", ZEVector3::One));

	SetRefractionEnabled(PropertiesNode.ReadBoolean("RefractionEnabled", false));
	SetRefractionFactor(PropertiesNode.ReadFloat("RefractionFactor", 1.0f));
	SetRefractionColor(PropertiesNode.ReadVector3("RefractionColor", ZEVector3::One));
	SetRefractionIndex(PropertiesNode.ReadFloat("RefractionIndex", 1.0f));

	SetDetailBaseMapFile(ZEPathInfo::CombineRelativePath(PropertiesNode.GetFile()->GetPath(), PropertiesNode.ReadString("DetailBaseMap")));
	SetDetailBaseMapEnabled(PropertiesNode.ReadBoolean("DetailBaseMapEnabled", !DetailBaseMap.IsNull()));
	SetDetailBaseMapColor(PropertiesNode.ReadVector3("DetailBaseMapColor", ZEVector3::One));
	SetDetailMapTiling(PropertiesNode.ReadVector2("DetailBaseMapTiling", ZEVector2::One));
	SetDetailBaseMapAttenuationStart(PropertiesNode.ReadFloat("DetailBaseMapAttenuationStart", 10.0f));
	SetDetailBaseMapAttenuationFactor(PropertiesNode.ReadFloat("DetailBaseMapAttenuationFactor", 0.01f));

	SetDetailNormalMapFile(ZEPathInfo::CombineRelativePath(PropertiesNode.GetFile()->GetPath(), PropertiesNode.ReadString("DetailNormalMap")));
	SetDetailNormalMapEnabled(PropertiesNode.ReadBoolean("DetailNormalMapEnabled", !DetailNormalMap.IsNull()));
	SetDetailNormalMapFactor(PropertiesNode.ReadFloat("DetailNormalMapFactor", 1.0f));
	SetDetailNormalMapTiling(PropertiesNode.ReadVector2("DetailNormalMapTiling", ZEVector2::One));
	SetDetailNormalMapAttenuationStart(PropertiesNode.ReadFloat("DetailNormalMapAttenuationStart", 10.0f));
	SetDetailNormalMapAttenuationFactor(PropertiesNode.ReadFloat("DetailNormalMapAttenuationFactor", 0.01f));

	SetClippingPlanesEnabled(PropertiesNode.ReadBoolean("ClippingPlanesEnabled", false));

	return true;
}

ZERNStandardMaterial* ZERNStandardMaterial::CreateInstance()
{
	return new ZERNStandardMaterial();
}

ZERSHolder<ZERNStandardMaterial> ZERNStandardMaterial::CreateResource()
{
	return ZERSTemplates::CreateResource<ZERNStandardMaterial>(ZERSTemplates::InstanciatorFunction<ZERNStandardMaterial>);
}

ZERSHolder<ZERNStandardMaterial> ZERNStandardMaterial::LoadResource(const ZEString& FileName)
{
	return ZERSTemplates::LoadResource<ZERNStandardMaterial>(FileName, ZERSTemplates::InstanciatorFunction<ZERNStandardMaterial>);
}

ZERSHolder<const ZERNStandardMaterial> ZERNStandardMaterial::LoadResourceShared(const ZEString& FileName)
{
	return ZERSTemplates::LoadResourceShared<ZERNStandardMaterial>(FileName, ZERSTemplates::InstanciatorFunction<ZERNStandardMaterial>);
}
