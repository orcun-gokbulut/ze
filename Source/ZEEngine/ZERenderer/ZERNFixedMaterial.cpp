//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNFixedMaterial.cpp
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

#include "ZERNFixedMaterial.h"

#include "ZEError.h"
#include "ZEML/ZEMLReader.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZETexture/ZETextureCubeResource.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRTextureCube.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRShaderCompileOptions.h"
#include "ZERNStageGBuffer.h"
#include "ZERNStageID.h"
#include "ZERNShaderSlots.h"
#include "ZEModel/ZEMDResource.h"
#include "ZECanvas.h"
#include "ZEInterior/ZEInterior.h"
#include "ZEInterior/ZEInteriorResource.h"
#include "ZERNStageShadowmapGeneration.h"
#include "ZERNStageParticleRendering.h"
#include "ZERNStageDebug.h"
#include "ZERNStageForward.h"
#include "ZERNStageRenderDepth.h"
#include "ZERNCommand.h"
#include "ZEModel/ZEMDVertex.h"

#define ZERN_FMDF_CONSTANT_BUFFER		1
#define ZERN_FMDF_RENDER_STATE			2
#define ZERN_FMDF_SHADERS				4
#define ZERN_FMDF_STAGE_MASK			8

void ZERNFixedMaterial::UpdateShaderDefinitions(ZEGRShaderCompileOptions& Options) const
{
	Options.Definitions.Clear();

	if (AlphaCullEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_ALPHA_CULL"));

	if (SkinningEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_SKIN_TRANSFORM"));

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

	if (ClippingPlanesEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_CLIPPING_PLANES"));

	if (TransparencyEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_FORWARD"));
	else
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_DEFERRED"));
}

bool ZERNFixedMaterial::UpdateShaders() const
{
	if (!DirtyFlags.GetFlags(ZERN_FMDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;

	UpdateShaderDefinitions(Options);

	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString(ZEGRGraphicsModule::SAMPLE_COUNT)));

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNFixedMaterial.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNFixedMaterial_VertexShader";
	StageGBuffer_Forward_VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(StageGBuffer_Forward_VertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFixedMaterial_PixelShader";
	StageGBuffer_Forward_PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(StageGBuffer_Forward_PixelShader == NULL, false, "Cannot set pixel shader.");

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNFixedMaterial_ShadowMapGenerationStage_VertexShader_Main";
	StageShadowmapGeneration_VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(StageShadowmapGeneration_VertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFixedMaterial_ShadowMapGenerationStage_PixelShader_Main";
	StageShadowmapGeneration_PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(StageShadowmapGeneration_PixelShader == NULL, false, "Cannot set pixel shader.");

	DirtyFlags.UnraiseFlags(ZERN_FMDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_FMDF_RENDER_STATE);

	return true;
}

bool ZERNFixedMaterial::UpdateRenderState() const
{
	if (!DirtyFlags.GetFlags(ZERN_FMDF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState;
	if (TransparencyEnabled)
		RenderState = ZERNStageForwardTransparent::GetRenderState();
	else
		RenderState = ZERNStageGBuffer::GetRenderState();

	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);

	if (SkinningEnabled)
		RenderState.SetVertexLayout(ZEMDVertexSkin::GetVertexLayout());
	else
		RenderState.SetVertexLayout(ZEMDVertex::GetVertexLayout());

	if (AlphaCullEnabled && !TransparencyEnabled)
	{
		ZEGRBlendState BlendStateAlphaToCoverage;
		BlendStateAlphaToCoverage.SetAlphaToCoverageEnable(true);

		RenderState.SetBlendState(BlendStateAlphaToCoverage);
	}

	ZEGRRasterizerState RasterizerState = RenderState.GetRasterizerState();
	RasterizerState.SetCullMode(TwoSided ? ZEGR_CMD_NONE : RasterizerState.GetCullMode());
	RasterizerState.SetFillMode(Wireframe ? ZEGR_FM_WIREFRAME : RasterizerState.GetFillMode());
	RenderState.SetRasterizerState(RasterizerState);
	
	RenderState.SetShader(ZEGR_ST_VERTEX, StageGBuffer_Forward_VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, StageGBuffer_Forward_PixelShader);

	StageGBuffer_Forward_RenderState = RenderState.Compile();
	zeCheckError(StageGBuffer_Forward_RenderState == NULL, false, "Cannot set gbuffer/forward render state.");

	RenderState = ZERNStageShadowmapGeneration::GetRenderState();
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetShader(ZEGR_ST_VERTEX, StageShadowmapGeneration_VertexShader);

	if (AlphaCullEnabled)
		RenderState.SetShader(ZEGR_ST_PIXEL, StageShadowmapGeneration_PixelShader);

	if (SkinningEnabled)
		RenderState.SetVertexLayout(ZEMDVertexSkin::GetVertexLayout());
	else
		RenderState.SetVertexLayout(ZEMDVertex::GetVertexLayout());

	RasterizerState = RenderState.GetRasterizerState();
	RasterizerState.SetCullMode(TwoSided ? ZEGR_CMD_NONE : RasterizerState.GetCullMode());
	RenderState.SetRasterizerState(RasterizerState);

	StageShadowmapGeneration_RenderState = RenderState.Compile();
	zeCheckError(StageShadowmapGeneration_RenderState == NULL, false, "Cannot set shadow map generation render state.");

	DirtyFlags.UnraiseFlags(ZERN_FMDF_RENDER_STATE);

	return true;
}

bool ZERNFixedMaterial::UpdateStageMask() const
{
	if (!DirtyFlags.GetFlags(ZERN_FMDF_STAGE_MASK))
		return true;

	StageMask = ZERN_STAGE_DEBUG;
	StageMask |= TransparencyEnabled ? (ZERN_STAGE_RENDER_DEPTH | ZERN_STAGE_FORWARD_TRANSPARENT) : ZERN_STAGE_GBUFFER;
	StageMask |= ShadowCaster ? ZERN_STAGE_SHADOW_MAP_GENERATION : 0;

	DirtyFlags.UnraiseFlags(ZERN_FMDF_STAGE_MASK);

	return true;
}

bool ZERNFixedMaterial::UpdateConstantBuffer() const
{
	if (!DirtyFlags.GetFlags(ZERN_FMDF_CONSTANT_BUFFER))
		return true;
	
	Constants.AmbientColor = (AmbientEnabled ? 1.0f : 0.0f) * AmbientFactor * AmbientColor;
	Constants.DiffuseColor = (DiffuseEnabled ? 1.0f : 0.0f) * DiffuseFactor * DiffuseColor;
	Constants.SpecularColor = (SpecularEnabled ? 1.0f : 0.0f) * SpecularFactor * SpecularColor;
	Constants.EmissiveColor = (EmissiveEnabled ? 1.0f : 0.0f) * EmissiveFactor * EmissiveColor;
	Constants.ReflectionColor = (ReflectionEnabled ? 1.0f : 0.0f) * ReflectionFactor * ReflectionColor;
	Constants.RefractionColor = (RefractionEnabled ? 1.0f : 0.0f) * RefractionFactor * RefractionColor;

	Constants.AlphaCullLimit = 0.1f;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_FMDF_CONSTANT_BUFFER);

	return true;
}

bool ZERNFixedMaterial::Update() const
{
	if (!ZERNMaterial::Update())
		return false;

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

bool ZERNFixedMaterial::InitializeInternal()
{
	if (!ZERNMaterial::InitializeInternal())
		return false;

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));

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

bool ZERNFixedMaterial::DeinitializeInternal()
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

	return ZERNMaterial::DeinitializeInternal();
}

ZERNFixedMaterial::ZERNFixedMaterial()
{
	StageMask = 0;
	DirtyFlags.RaiseAll();

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
	Constants.Reserved0 = 0.0f;
	Constants.EmissiveColor = ZEVector3::One;
	Constants.AlphaCullLimit = 1.0f;
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
}

ZEUInt ZERNFixedMaterial::GetStageMask() const
{
	return StageMask;
}

void ZERNFixedMaterial::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZERNFixedMaterial::GetName() const
{
	return Name;
}

const ZEString&	ZERNFixedMaterial::GetFileName() const
{
	return FileName;
}

void ZERNFixedMaterial::SetSampler(const ZEHolder<ZEGRSampler>& Sampler)
{
	this->Sampler = Sampler;
}

const ZEHolder<ZEGRSampler>& ZERNFixedMaterial::GetSampler() const
{
	return Sampler;
}

void ZERNFixedMaterial::SetShadowCaster(bool ShadowCaster)
{
	if (this->ShadowCaster == ShadowCaster)
		return;

	this->ShadowCaster = ShadowCaster;

	DirtyFlags.RaiseFlags(ZERN_FMDF_STAGE_MASK);
}

bool ZERNFixedMaterial::GetShadowCaster() const
{
	return ShadowCaster;
}

void ZERNFixedMaterial::SetTwoSided(bool Enable)
{
	if (TwoSided == Enable)
		return;

	TwoSided = Enable;

	DirtyFlags.RaiseFlags(ZERN_FMDF_RENDER_STATE);
}

bool ZERNFixedMaterial::GetTwoSided() const
{
	return TwoSided;
}

void ZERNFixedMaterial::SetWireframe(bool Enable)
{
	if (Wireframe == Enable)
		return;

	Wireframe = Enable;

	DirtyFlags.RaiseFlags(ZERN_FMDF_RENDER_STATE);
}

bool ZERNFixedMaterial::GetWireframe() const
{
	return Wireframe;
}

void ZERNFixedMaterial::SetSkinningEnabled(bool Enabled)
{
	if (SkinningEnabled == Enabled)
		return;

	SkinningEnabled = true;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNFixedMaterial::GetSkinningEnabled() const
{
	return SkinningEnabled;
}

void ZERNFixedMaterial::SetVertexColorEnabled(bool Enabled)
{
	if (VertexColorEnabled == Enabled)
		return;

	VertexColorEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNFixedMaterial::GetVertexColorEnabled() const
{
	return VertexColorEnabled;
}

void ZERNFixedMaterial::SetTransparencyEnabled(bool Enabled)
{
	if (TransparencyEnabled == Enabled)
		return;

	TransparencyEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS | ZERN_FMDF_STAGE_MASK);
}

bool ZERNFixedMaterial::GetTransparencyEnabled() const
{
	return TransparencyEnabled;
}

void ZERNFixedMaterial::SetTransparencyMode(ZERNTransparencyMode Mode)
{
	if (TransparencyMode == Mode)
		return;

	TransparencyMode = Mode;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

ZERNTransparencyMode ZERNFixedMaterial::GetTransparencyMode() const
{
	return TransparencyMode;
}

void ZERNFixedMaterial::SetAlphaCullEnabled(bool Enabled)
{
	if (AlphaCullEnabled == Enabled)
		return;

	AlphaCullEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNFixedMaterial::GetAlphaCullEnabled() const
{
	return AlphaCullEnabled;
}

void ZERNFixedMaterial::SetAlphaCullLimit(float Limit)
{
	if (Constants.AlphaCullLimit == Limit)
		return;

	Constants.AlphaCullLimit = Limit;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetAlphaCullLimit() const
{
	return Constants.AlphaCullLimit;
}

void ZERNFixedMaterial::SetSubSurfaceScatteringFactor(float Factor)
{
	if (Constants.SubSurfaceScatteringFactor == Factor)
		return;

	Constants.SubSurfaceScatteringFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetSubSurfaceScatteringFactor() const
{
	return Constants.SubSurfaceScatteringFactor;
}

void ZERNFixedMaterial::SetSubSurfaceScatteringMapEnabled(bool Enabled)
{
	if (SubSurfaceScatteringMapEnabled == Enabled)
		return;

	SubSurfaceScatteringMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNFixedMaterial::GetSubSurfaceScatteringMapEnabled() const
{
	return SubSurfaceScatteringMapEnabled;
}

void ZERNFixedMaterial::SetSubSurfaceScatteringMap(ZEGRTexture2D* Texture)
{
	SubSurfaceScatteringMap = Texture;
}

const ZEGRTexture2D* ZERNFixedMaterial::GetSubSurfaceScatteringMap() const
{
	return SubSurfaceScatteringMap;
}

void ZERNFixedMaterial::SetSubSurfaceScatteringMapFile(const ZEString& Filename)
{
	SubSurfaceScatteringMap = ZEGRTexture2D::CreateFromFile(Filename, false, false, true);
}

const ZEString& ZERNFixedMaterial::GetSubSurfaceScatteringMapFile() const
{
	return SubSurfaceScatteringMap->GetName();
}

void ZERNFixedMaterial::SetBaseMapEnabled(bool Enabled)
{
	if (BaseMapEnabled == Enabled)
		return;
	
	BaseMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNFixedMaterial::GetBaseMapEnabled() const
{
	return BaseMapEnabled;
}

void ZERNFixedMaterial::SetBaseMap(ZEGRTexture2D* Map)
{
	BaseMap = Map;
}

const ZEGRTexture2D* ZERNFixedMaterial::GetBaseMap() const
{
	return BaseMap;
}

void ZERNFixedMaterial::SetBaseMapFile(const ZEString& Filename)
{
	BaseMap = ZEGRTexture2D::CreateFromFile(Filename);
}

const ZEString& ZERNFixedMaterial::GetBaseMapFile() const
{
	return BaseMap->GetName();
}

void ZERNFixedMaterial::SetAmbientEnabled(bool Enabled)
{
	if (AmbientEnabled == Enabled)
		return;

	AmbientEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

bool ZERNFixedMaterial::GetAmbientEnabled() const
{
	return AmbientEnabled;
}

void ZERNFixedMaterial::SetSceneAmbientEnabled(bool Enabled)
{
	if ((Constants.SceneAmbientEnabled != 0) == Enabled)
		return;

	Constants.SceneAmbientEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

bool ZERNFixedMaterial::GetSceneAmbientEnabled() const
{
	return Constants.SceneAmbientEnabled != 0;
}

void ZERNFixedMaterial::SetAmbientFactor(float Factor)
{
	if (AmbientFactor == Factor)
		return;

	AmbientFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetAmbientFactor() const
{
	return AmbientFactor;
}

void ZERNFixedMaterial::SetAmbientColor(const ZEVector3& Color)
{
	if (AmbientColor == Color)
		return;

	AmbientColor = Color;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNFixedMaterial::GetAmbientColor() const
{
	return AmbientColor;
}

void ZERNFixedMaterial::SetDiffuseEnabled(bool Enabled)
{
	if (DiffuseEnabled == Enabled)
		return;

	DiffuseEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

bool ZERNFixedMaterial::GetDiffuseEnabled() const
{
	return DiffuseEnabled;
}

void ZERNFixedMaterial::SetDiffuseFactor(float Factor)
{
	if (DiffuseFactor == Factor)
		return;

	DiffuseFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetDiffuseFactor() const
{
	return DiffuseFactor;
}

void ZERNFixedMaterial::SetDiffuseColor(const ZEVector3& Color)
{
	if (DiffuseColor == Color)
		return;

	DiffuseColor = Color;
	
	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNFixedMaterial::GetDiffuseColor() const
{
	return DiffuseColor;
}

void ZERNFixedMaterial::SetSpecularEnabled(bool Enabled)
{
	if (SpecularEnabled == Enabled)
		return;

	SpecularEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

bool ZERNFixedMaterial::GetSpecularEnabled() const
{
	return SpecularEnabled;
}


void ZERNFixedMaterial::SetSpecularFactor(float Factor)
{
	if (SpecularFactor == Factor)
		return;

	SpecularFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetSpecularFactor() const
{
	return SpecularFactor;
}

void ZERNFixedMaterial::SetSpecularColor(const ZEVector3& Color)
{
	if (SpecularColor == Color)
		return;

	SpecularColor = Color;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNFixedMaterial::GetSpecularColor() const
{
	return SpecularColor;
}

void ZERNFixedMaterial::SetSpecularShininess(float Power)
{
	if (Constants.SpecularPower == Power)
		return;

	Constants.SpecularPower = Power;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetSpecularShininess() const
{
	return Constants.SpecularPower;
}

void ZERNFixedMaterial::SetSpecularMapEnabled(bool Enabled)
{
	if (SpecularMapEnabled == Enabled)
		return;

	SpecularMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNFixedMaterial::GetSpecularMapEnabled() const
{
	return SpecularMapEnabled;		
}

void ZERNFixedMaterial::SetSpecularMap(ZEGRTexture2D* Texture)
{
	SpecularMap = Texture;
}

const ZEGRTexture2D* ZERNFixedMaterial::GetSpecularMap() const
{
	return SpecularMap;
}

void ZERNFixedMaterial::SetSpecularMapFile(const ZEString& Filename)
{
	SpecularMap = ZEGRTexture2D::CreateFromFile(Filename, false, false, true);
}

const ZEString& ZERNFixedMaterial::GetSpecularMapFile() const
{
	return SpecularMap->GetName();
}

void ZERNFixedMaterial::SetSpecularGlossMapEnabled(bool Enabled)
{
	if (SpecularGlossMapEnabled == Enabled)
		return;

	SpecularGlossMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNFixedMaterial::GetSpecularGlossMapEnabled() const
{
	return SpecularGlossMapEnabled;
}

void ZERNFixedMaterial::SetSpecularGlossMap(ZEGRTexture2D* Texture)
{
	SpecularGlossMap = Texture;
}

const ZEGRTexture2D* ZERNFixedMaterial::GetSpecularGlossMap() const
{
	return SpecularGlossMap;
}

void ZERNFixedMaterial::SetSpecularGlossMapFile(const ZEString& Filename)
{
	SpecularGlossMap = ZEGRTexture2D::CreateFromFile(Filename, false, false, true);
}

const ZEString& ZERNFixedMaterial::GetSpecularGlossMapFile() const
{
	return SpecularGlossMap->GetName();
}

void ZERNFixedMaterial::SetEmissiveEnabled(bool Enabled)
{
	if (EmissiveEnabled == Enabled)
		return;

	EmissiveEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

bool ZERNFixedMaterial::GetEmissiveEnabled() const
{
	return EmissiveEnabled;
}

void ZERNFixedMaterial::SetEmissiveFactor(float Factor)
{
	if (EmissiveFactor == Factor)
		return;

	EmissiveFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetEmissiveFactor() const
{
	return EmissiveFactor;
}

void ZERNFixedMaterial::SetEmissiveColor(const ZEVector3& Color)
{
	if (EmissiveColor == Color)
		return;

	EmissiveColor = Color;
	
	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNFixedMaterial::GetEmissiveColor() const
{
	return EmissiveColor;
}

void ZERNFixedMaterial::SetEmissiveMapEnabled(bool Enabled)
{
	if (EmissiveMapEnabled == Enabled)
		return;

	EmissiveMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNFixedMaterial::GetEmissiveMapEnabled() const
{
	return EmissiveMapEnabled;
}

void ZERNFixedMaterial::SetEmissiveMap(ZEGRTexture2D* Texture)
{
	EmissiveMap = Texture;
}

const ZEGRTexture2D* ZERNFixedMaterial::GetEmissiveMap() const
{
	return EmissiveMap;
}

void ZERNFixedMaterial::SetEmissiveMapFile(const ZEString& Filename)
{
	EmissiveMap = ZEGRTexture2D::CreateFromFile(Filename);
}

const ZEString& ZERNFixedMaterial::GetEmissiveMapFile() const
{
	return EmissiveMap->GetName();
}

void ZERNFixedMaterial::SetNormalMapEnabled(bool Enabled)
{
	if (NormalMapEnabled == Enabled)
		return;

	NormalMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNFixedMaterial::GetNormalMapEnabled() const
{
	return NormalMapEnabled;
}

void ZERNFixedMaterial::SetNormalMap(ZEGRTexture2D* Texture)
{
	NormalMap = Texture;
}

const ZEGRTexture2D* ZERNFixedMaterial::GetNormalMap() const
{
	return NormalMap;
}

void ZERNFixedMaterial::SetNormalMapFile(const ZEString& Filename)
{
	NormalMap = ZEGRTexture2D::CreateFromFile(Filename, false, true);
}

const ZEString& ZERNFixedMaterial::GetNormalMapFile() const
{
	return NormalMap->GetName();
}

void ZERNFixedMaterial::SetHeightMapEnabled(bool Enabled)
{
	if (HeightMapEnabled == Enabled)
		return;

	HeightMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNFixedMaterial::GetHeightMapEnabled() const
{
	return HeightMapEnabled;
}

void ZERNFixedMaterial::SetHeightMapTechnique(ZERNHeightMapTechnique Technique)
{
	if (HeightMapTechnique == Technique)
		return;

	HeightMapTechnique = Technique;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

ZERNHeightMapTechnique ZERNFixedMaterial::GetHeightMapTechnique() const
{
	return HeightMapTechnique;
}

void ZERNFixedMaterial::SetHeightMapOffset(float Offset)
{
	if (Constants.HeightMapOffset == Offset)
		return;

	Constants.HeightMapOffset = Offset;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetHeightMapOffset() const
{
	return Constants.HeightMapOffset;
}

void ZERNFixedMaterial::SetHeightMapScale(float Scale)
{
	if (Constants.HeightMapScale == Scale)
		return;

	Constants.HeightMapScale = Scale;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetHeightMapScale() const
{
	return Constants.HeightMapScale;
}

void ZERNFixedMaterial::SetHeightMap(ZEGRTexture2D* Texture)
{
	HeightMap = Texture;
}

const ZEGRTexture2D* ZERNFixedMaterial::GetHeightMap() const
{
	return HeightMap;
}

void ZERNFixedMaterial::SetHeightMapFile(const ZEString& Filename)
{
	HeightMap = ZEGRTexture2D::CreateFromFile(Filename, false, false, true);
}

const ZEString& ZERNFixedMaterial::GetHeightMapFile() const
{
	return HeightMap->GetName();
}

void ZERNFixedMaterial::SetOpacity(float Value)
{
	if (Constants.Opacity == Value)
		return;

	Constants.Opacity = Value;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetOpacity() const
{
	return Constants.Opacity;
}

void ZERNFixedMaterial::SetOpacityMapEnabled(bool Enabled)
{
	if (OpacityMapEnabled == Enabled)
		return;

	OpacityMapEnabled = true;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS | ZERN_FMDF_STAGE_MASK);
}
bool ZERNFixedMaterial::GetOpacityMapEnabled() const
{
	return OpacityMapEnabled;
}

void ZERNFixedMaterial::SetOpacityMap(ZEGRTexture2D* Texture)
{
	OpacityMap = Texture;
}

const ZEGRTexture2D* ZERNFixedMaterial::GetOpacityMap() const
{
	return OpacityMap;
}

void ZERNFixedMaterial::SetOpacityMapFile(const ZEString& Filename)
{
	OpacityMap = ZEGRTexture2D::CreateFromFile(Filename, false, false, true);
}

const ZEString& ZERNFixedMaterial::GetOpacityMapFile() const
{
	return OpacityMap->GetName();
}

void ZERNFixedMaterial::SetEnvironmentMap(ZEGRTexture2D* Texture)
{
	EnvironmentMap = Texture;
}

const ZEGRTexture2D* ZERNFixedMaterial::GetEnvironmentMap() const
{
	return EnvironmentMap;
}

void ZERNFixedMaterial::SetEnvironmentMapFile(const ZEString& Filename)
{

}

const ZEString& ZERNFixedMaterial::GetEnvironmentMapFile() const
{
	return ZEString::Empty;
}

void ZERNFixedMaterial::SetReflectionEnabled(bool Enabled)
{
	if (ReflectionEnabled == Enabled)
		return;

	ReflectionEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS | ZERN_FMDF_CONSTANT_BUFFER);
}

bool ZERNFixedMaterial::GetReflectionEnabled() const
{
	return ReflectionEnabled;
}

void ZERNFixedMaterial::SetReflectionFactor(float Factor)
{
	if (ReflectionFactor == Factor)
		return;

	ReflectionFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetReflectionFactor() const
{
	return ReflectionFactor;
}

void ZERNFixedMaterial::SetReflectionColor(const ZEVector3& Color)
{
	if (ReflectionColor == Color)
		return;

	ReflectionColor = Color;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNFixedMaterial::GetReflectionColor() const
{
	return ReflectionColor;
}

void ZERNFixedMaterial::SetRefractionEnabled(bool Enabled)
{
	if (RefractionEnabled == Enabled)
		return;

	RefractionEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS | ZERN_FMDF_CONSTANT_BUFFER);
}

bool ZERNFixedMaterial::GetRefractionEnabled() const
{
	return RefractionEnabled;
}

void ZERNFixedMaterial::SetRefractionFactor(float Factor)
{
	if (RefractionFactor == Factor)
		return;

	RefractionFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetRefractionFactor() const
{
	return RefractionFactor;
}

void ZERNFixedMaterial::SetRefractionColor(const ZEVector3& Color)
{
	if (RefractionColor == Color)
		return;

	RefractionColor = Color;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNFixedMaterial::GetRefractionColor() const
{
	return RefractionColor;
}

void ZERNFixedMaterial::SetRefractionIndex(float Index)
{
	if (Constants.RefractionIndex == Index)
		return;

	Constants.RefractionIndex = Index;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetRefractionIndex() const
{
	return Constants.RefractionIndex;
}

void ZERNFixedMaterial::SetDetailMapTiling(const ZEVector2& Tiling)
{
	if (Constants.DetailBaseMapTiling == Tiling && Constants.DetailNormalMapTiling == Tiling)
		return;

	Constants.DetailBaseMapTiling = Tiling;
	Constants.DetailNormalMapTiling = Tiling;
	
	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNFixedMaterial::GetDetailMapTiling() const
{
	return Constants.DetailBaseMapTiling;
}

void ZERNFixedMaterial::SetDetailBaseMapEnabled(bool Enabled)
{
	if (DetailBaseMapEnabled == Enabled)
		return;

	DetailBaseMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNFixedMaterial::GetDetailBaseMapEnabled() const
{
	return DetailBaseMapEnabled;
}

void ZERNFixedMaterial::SetDetailBaseMapColor(const ZEVector3& Color)
{
	if (Constants.DetailBaseMapColor == Color)
		return;

	Constants.DetailBaseMapColor = Color;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNFixedMaterial::GetDetailBaseMapColor() const
{
	return Constants.DetailBaseMapColor;
}

void ZERNFixedMaterial::SetDetailBaseMapTiling(const ZEVector2& Tiling)
{
	if (Constants.DetailBaseMapTiling == Tiling)
		return;

	Constants.DetailBaseMapTiling = Tiling;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNFixedMaterial::GetDetailBaseMapTiling() const
{
	return Constants.DetailBaseMapTiling;
}

void ZERNFixedMaterial::SetDetailBaseMapAttenuationStart(float Start)
{
	if (Constants.DetailBaseMapAttenuationStart == Start)
		return;

	Constants.DetailBaseMapAttenuationStart = Start;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetDetailBaseMapAttenuationStart() const
{
	return Constants.DetailBaseMapAttenuationStart;
}

void ZERNFixedMaterial::SetDetailBaseMapAttenuationFactor(float Factor)
{
	if (Constants.DetailBaseMapAttenuationFactor == Factor)
		return;

	Constants.DetailBaseMapAttenuationFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetDetailBaseMapAttenuationFactor() const
{
	return Constants.DetailBaseMapAttenuationFactor;
}

void ZERNFixedMaterial::SetDetailBaseMapFile(const ZEString& Filename)
{
	DetailBaseMap = ZEGRTexture2D::CreateFromFile(Filename);
}

const ZEString& ZERNFixedMaterial::GetDetailBaseMapFile() const
{
	return DetailBaseMap->GetName();
}

void ZERNFixedMaterial::SetDetailNormalMapEnabled(bool Enabled)
{
	if (DetailNormalMapEnabled == Enabled)
		return;

	DetailNormalMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNFixedMaterial::GetDetailNormalMapEnabled() const
{
	return DetailNormalMapEnabled;
}

void ZERNFixedMaterial::SetDetailNormalMapFactor(float Factor)
{
	if (Constants.DetailNormalMapFactor == Factor)
		return;

	Constants.DetailNormalMapFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetDetailNormalMapFactor() const
{
	return Constants.DetailNormalMapFactor;
}

void ZERNFixedMaterial::SetDetailNormalMapTiling(const ZEVector2& Tiling)
{
	if (Constants.DetailNormalMapTiling == Tiling)
		return;

	Constants.DetailNormalMapTiling = Tiling;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNFixedMaterial::GetDetailNormalMapTiling() const
{
	return Constants.DetailNormalMapTiling;
}

void ZERNFixedMaterial::SetDetailNormalMapAttenuationStart(float Start)
{
	if (Constants.DetailNormalMapAttenuationStart == Start)
		return;

	Constants.DetailNormalMapAttenuationStart = Start;
	
	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetDetailNormalMapAttenuationStart() const
{
	return Constants.DetailNormalMapAttenuationStart;
}

void ZERNFixedMaterial::SetDetailNormalMapAttenuationFactor(float Factor)
{
	if (Constants.DetailNormalMapAttenuationFactor == Factor)
		return;

	Constants.DetailNormalMapAttenuationFactor = Factor;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

float ZERNFixedMaterial::GetDetailNormalMapAttenuationFactor() const
{
	return Constants.DetailNormalMapAttenuationFactor;
}

void ZERNFixedMaterial::SetDetailNormalMapFile(const ZEString& Filename)
{
	DetailNormalMap = ZEGRTexture2D::CreateFromFile(Filename, false, true);
}

const ZEString& ZERNFixedMaterial::GetDetailNormalMapFile() const
{
	return DetailNormalMap->GetName();
}

void ZERNFixedMaterial::SetClippingPlanesEnabled(bool Enabled)
{
	ClippingPlanesEnabled = Enabled;
}

bool ZERNFixedMaterial::GetClippingPlanesEnabled() const
{
	return ClippingPlanesEnabled;
}

bool ZERNFixedMaterial::PreRender(ZERNCommand& Command) const
{
	if (!ZERNMaterial::PreRender(Command))
		return false;

	if (GetTransparencyEnabled())
		Command.Order = -Command.Order;

	return true;
}

bool ZERNFixedMaterial::SetupMaterial(ZEGRContext* Context, const ZERNStage* Stage) const
{
	if (!ZERNMaterial::SetupMaterial(Context, Stage))
		return false;

	if (!Update())
		return false;

	Context->SetConstantBuffers(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_MATERIAL, 1, ConstantBuffer.GetPointerToPointer());

	ZEUInt StageID = Stage->GetId();
	if (StageID == ZERN_STAGE_GBUFFER || StageID == ZERN_STAGE_FORWARD_TRANSPARENT)
	{
		Context->SetRenderState(StageGBuffer_Forward_RenderState);

		const ZEGRSampler* Samplers[] = {Sampler, Sampler, Sampler, Sampler};
		Context->SetSamplers(ZEGR_ST_PIXEL, 0, 4, Samplers);

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
	else if (StageID == ZERN_STAGE_SHADOW_MAP_GENERATION || StageID == ZERN_STAGE_RENDER_DEPTH)
	{
		Context->SetRenderState(StageShadowmapGeneration_RenderState);

		if (AlphaCullEnabled)
		{
			if (OpacityMapEnabled)
			{
				const ZEGRTexture* Texture = OpacityMap;
				Context->SetSamplers(ZEGR_ST_PIXEL, 0, 1, Sampler.GetPointerToPointer());
				Context->SetTextures(ZEGR_ST_PIXEL, 5, 1, &Texture);
			}
		}
	}

	return true;
}

void ZERNFixedMaterial::CleanupMaterial(ZEGRContext* Context, const ZERNStage* Stage) const
{
	ZERNMaterial::CleanupMaterial(Context, Stage);
}

ZEHolder<ZERNFixedMaterial> ZERNFixedMaterial::CreateInstance()
{
	return new ZERNFixedMaterial;
}

void ZERNFixedMaterial::WriteToFile(const ZEString& FilePath)
{
	ZEFile File;
	if(!File.Open(FilePath, ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
		zeError("Can not open given file. File : %s", FilePath.ToCString());

	ZEMLWriter MaterialWriter;
	MaterialWriter.Open(&File);

	ZEMLWriterNode MaterialNode;
	MaterialWriter.OpenRootNode("ZEMaterial", MaterialNode);

	MaterialNode.WriteString("Name", GetName().ToCString());

	ZEMLWriterNode ConfigurationNode;
	MaterialNode.OpenNode("Configuration", ConfigurationNode);

	ConfigurationNode.WriteString("Name", "Default"); //Will be changed when configuration is implemented.
	ConfigurationNode.WriteBool("TwoSided", GetTwoSided());
	ConfigurationNode.WriteBool("Wireframe", GetWireframe());
	ConfigurationNode.WriteBool("VertexColorEnabled", GetVertexColorEnabled());
	ConfigurationNode.WriteBool("SkinningEnabled", GetSkinningEnabled());

	ConfigurationNode.WriteBool("TransparancyEnabled", GetTransparencyEnabled());
	ConfigurationNode.WriteInt32("TransparencyMode", (ZEInt32)GetTransparencyMode());
	ConfigurationNode.WriteBool("AlphaCullEnabled", GetAlphaCullEnabled());
	ConfigurationNode.WriteFloat("AlphaCullLimit", GetAlphaCullLimit());

	ConfigurationNode.WriteString("BaseMap", BaseMap->GetName());

	ConfigurationNode.WriteFloat("SubSurfaceScatteringFactor", GetSubSurfaceScatteringFactor());
	ConfigurationNode.WriteBool("SubSurfaceScatteringMapEnabled", GetSubSurfaceScatteringMapEnabled());
	ConfigurationNode.WriteString("SubSurfaceScatteringMap", SubSurfaceScatteringMap->GetName());

	ConfigurationNode.WriteBool("AmbientEnabled", GetAmbientEnabled());
	ConfigurationNode.WriteFloat("AmbientFactor", GetAmbientFactor());
	ConfigurationNode.WriteBool("SceneAmbientEnabled", GetSceneAmbientEnabled());
	ConfigurationNode.WriteVector3("AmbientColor", GetAmbientColor());

	ConfigurationNode.WriteBool("DiffuseEnabled", GetDiffuseEnabled());
	ConfigurationNode.WriteFloat("DiffuseFactor", GetDiffuseFactor());
	ConfigurationNode.WriteVector3("DiffuseColor", GetDiffuseColor());

	ConfigurationNode.WriteBool("SpecularEnabled", GetSpecularEnabled());
	ConfigurationNode.WriteFloat("SpecularFactor", GetSpecularFactor());
	ConfigurationNode.WriteVector3("SpecularColor", GetSpecularColor());
	ConfigurationNode.WriteBool("SpecularMapEnabled", GetSpecularMapEnabled());
	ConfigurationNode.WriteString("SpecularMap", SpecularMap->GetName());
	ConfigurationNode.WriteBool("SpecularGlossMapEnabled", GetSpecularGlossMapEnabled());
	ConfigurationNode.WriteString("SpecularGlossMap", SpecularGlossMap->GetName());

	ConfigurationNode.WriteBool("EmissiveEnabled", GetEmissiveEnabled());
	ConfigurationNode.WriteFloat("EmissiveFactor", GetEmissiveFactor());
	ConfigurationNode.WriteVector3("EmissiveColor", GetEmissiveColor());
	ConfigurationNode.WriteBool("EmissiveMapEnabled", GetEmissiveMapEnabled());
	ConfigurationNode.WriteString("EmissiveMap", EmissiveMap->GetName());

	ConfigurationNode.WriteBool("NormalMapEnabled", GetNormalMapEnabled());
	ConfigurationNode.WriteString("NormalMap", NormalMap->GetName());

	ConfigurationNode.WriteBool("HeightMapEnabled", GetHeightMapEnabled());
	ConfigurationNode.WriteUInt8("HeightMapTechnique", GetHeightMapEnabled());
	ConfigurationNode.WriteFloat("HeightMapOffset", GetHeightMapOffset());
	ConfigurationNode.WriteFloat("HeightMapScale", GetHeightMapScale());
	ConfigurationNode.WriteString("HeightMap", HeightMap->GetName());

	ConfigurationNode.WriteFloat("Opacity", GetOpacity());
	ConfigurationNode.WriteBool("OpacityMapEnabled", GetOpacityMapEnabled());
	ConfigurationNode.WriteString("OpacityMap", OpacityMap->GetName());

	ConfigurationNode.WriteString("EnvironmentMap", EnvironmentMap->GetName());

	ConfigurationNode.WriteBool("ReflectionEnabled", GetReflectionEnabled());
	ConfigurationNode.WriteFloat("ReflectionFactor", GetReflectionFactor());
	ConfigurationNode.WriteVector3("ReflectionColor", GetReflectionColor());
	ConfigurationNode.WriteBool("RefractionEnabled", GetRefractionEnabled());
	ConfigurationNode.WriteFloat("RefractionFactor", GetRefractionFactor());
	ConfigurationNode.WriteVector3("RefractionColor", GetRefractionColor());
	ConfigurationNode.WriteFloat("RefractionIndex", GetRefractionIndex());
	
	ConfigurationNode.WriteBool("DetailBaseMapEnabled", GetDetailBaseMapEnabled());
	ConfigurationNode.WriteVector3("DetailBaseMapColor", GetDetailBaseMapColor());
	ConfigurationNode.WriteVector2("DetailBaseMapTiling", GetDetailBaseMapTiling());
	ConfigurationNode.WriteFloat("DetailBaseMapAttenuationStart", GetDetailBaseMapAttenuationStart());
	ConfigurationNode.WriteFloat("DetailBaseMapAttenuationFactor", GetDetailBaseMapAttenuationFactor());
	ConfigurationNode.WriteString("DetailBaseMap", DetailBaseMap->GetName());

	ConfigurationNode.WriteBool("DetailNormalMapEnabled", GetDetailNormalMapEnabled());
	ConfigurationNode.WriteFloat("DetailNormalMapColor", GetDetailNormalMapFactor());
	ConfigurationNode.WriteVector2("DetailNormalMapTiling", GetDetailNormalMapTiling());
	ConfigurationNode.WriteFloat("DetailNormalMapAttenuationStart", GetDetailNormalMapAttenuationStart());
	ConfigurationNode.WriteFloat("DetailNormalMapAttenuationFactor", GetDetailNormalMapAttenuationFactor());
	ConfigurationNode.WriteString("DetailNormalMap", DetailNormalMap->GetName());

	ConfigurationNode.WriteBool("ClippingPlanesEnabled", GetClippingPlanesEnabled());

	ConfigurationNode.CloseNode();
	MaterialNode.CloseNode();
	MaterialWriter.Close();

	if(File.IsOpen())
		File.Close();
}

void ZERNFixedMaterial::ReadFromFile(const ZEString& FilePath)
{
	ZEFile File;
	if(!File.Open(FilePath, ZE_FOM_READ, ZE_FCM_NONE))
		zeError("Can not open given file. File : %s", FilePath.ToCString());

	ZEMLReader MaterialReader;
	if (!MaterialReader.Open(&File))
		return;

	ZEMLReaderNode MaterialNode = MaterialReader.GetRootNode();

	if(!MaterialNode.IsValid())
		zeError("Can not read material file.");

	if (MaterialNode.ReadUInt8("MajorVersion", 0) != 1)
		zeError("Old depricated ZEMaterial file version detected. Cannot read this material file. Current Version: 1.0. Detected Version: 0.0. File Name: \"%s\".", File.GetPath().ToCString());

	Load(MaterialNode);

	MaterialReader.Close();

	if(File.IsOpen())
		File.Close();

	FileName = FilePath;
}

void ZERNFixedMaterial::Load(const ZEMLReaderNode& MaterialNode)
{
	SetName(MaterialNode.ReadString("Name"));

	ZESize SubNodeCount = MaterialNode.GetNodeCount("Configuration");

	//When Material Configuration functionality is implemented this reading mechanism should be revised, now it just loads "Default" config.

	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode ConfigurationNode = MaterialNode.GetNode("Configuration", I);

		if (!ConfigurationNode.IsValid())
		{
			zeError("Cannot read material configuration");
			return;
		}

		SetWireframe(ConfigurationNode.ReadBoolean("Wireframe", false));
		SetTwoSided(ConfigurationNode.ReadBoolean("TwoSided", false));
		SetSkinningEnabled(ConfigurationNode.ReadBoolean("SkinningEnabled", false));
		SetVertexColorEnabled(ConfigurationNode.ReadBoolean("VertexColorEnabled", false));

		SetTransparencyEnabled(ConfigurationNode.ReadBoolean("TransparencyEnabled", ConfigurationNode.ReadBoolean("Transparant")));
		SetTransparencyMode((ZERNTransparencyMode)ConfigurationNode.ReadInt32("TransparencyMode", ZERN_TM_NONE));

		SetAlphaCullEnabled(ConfigurationNode.ReadBoolean("AlphaCullEnabled", false));
		SetAlphaCullLimit(ConfigurationNode.ReadFloat("AlphaCullLimit", 0.5f));

		SetSubSurfaceScatteringFactor(ConfigurationNode.ReadFloat("SubSurfaceScatteringFactor", 0.0f));
		SetSubSurfaceScatteringMapFile(ZEPathInfo::CombineRelativePath(ConfigurationNode.GetFile()->GetPath(), ConfigurationNode.ReadString("SubSurfaceScatteringMap")));
		SetSubSurfaceScatteringMapEnabled(ConfigurationNode.ReadBoolean("SubSurfaceScatteringMapEnabled", !SubSurfaceScatteringMap.IsNull()));

		SetBaseMapFile(ZEPathInfo::CombineRelativePath(ConfigurationNode.GetFile()->GetPath(), ConfigurationNode.ReadString("BaseMap")));
		SetBaseMapEnabled(ConfigurationNode.ReadBoolean("BaseMapEnabled", !BaseMap.IsNull()));

		SetAmbientEnabled(ConfigurationNode.ReadBoolean("AmbientEnabled", false));
		SetSceneAmbientEnabled(ConfigurationNode.ReadBoolean("SceneAmbientEnabled", true));
		SetAmbientFactor(ConfigurationNode.ReadFloat("AmbientFactor", 1.0f));
		SetAmbientColor(ConfigurationNode.ReadVector3("AmbientColor", ZEVector3::One));

		SetDiffuseEnabled(ConfigurationNode.ReadBoolean("DiffuseEnabled", true));
		SetDiffuseFactor(ConfigurationNode.ReadFloat("DiffuseFactor", 1.0f));
		SetDiffuseColor(ConfigurationNode.ReadVector3("DiffuseColor", ZEVector3::One));

		SetSpecularEnabled(ConfigurationNode.ReadBoolean("SpecularEnabled", true));
		SetSpecularFactor(ConfigurationNode.ReadFloat("SpecularFactor", 1.0f));
		SetSpecularShininess(ConfigurationNode.ReadFloat("SpecularShininess", 1.0f));
		SetSpecularColor(ConfigurationNode.ReadVector3("SpecularColor", ZEVector3::One));
		SetSpecularMapFile(ZEPathInfo::CombineRelativePath(ConfigurationNode.GetFile()->GetPath(), ConfigurationNode.ReadString("SpecularMap")));
		SetSpecularMapEnabled(ConfigurationNode.ReadBoolean("SpecularMapEnabled", !SpecularMap.IsNull()));
		SetSpecularGlossMapFile(ZEPathInfo::CombineRelativePath(ConfigurationNode.GetFile()->GetPath(), ConfigurationNode.ReadString("SpecularGlossMap")));
		SetSpecularGlossMapEnabled(ConfigurationNode.ReadBoolean("SpecularGlossMapEnabled", !SpecularGlossMap.IsNull()));

		SetEmissiveEnabled(ConfigurationNode.ReadBoolean("EmissiveEnabled", ConfigurationNode.ReadBoolean("EmmisiveEnabled")));
		SetEmissiveFactor(ConfigurationNode.ReadFloat("EmissiveFactor", ConfigurationNode.ReadFloat("EmmisiveFactor")));
		SetEmissiveColor(ConfigurationNode.ReadVector3("EmissiveColor", ConfigurationNode.ReadVector3("EmmisiveColor")));
		SetEmissiveMapFile(ZEPathInfo::CombineRelativePath(ConfigurationNode.GetFile()->GetPath(), ConfigurationNode.ReadString("EmissiveMap")));
		if (EmissiveMap.IsNull())
			SetEmissiveMapFile(ZEPathInfo::CombineRelativePath(ConfigurationNode.GetFile()->GetPath(), ConfigurationNode.ReadString("EmmisiveMap")));
		SetEmissiveMapEnabled(ConfigurationNode.ReadBoolean("EmissiveMapEnabled", !EmissiveMap.IsNull()));

		SetNormalMapFile(ZEPathInfo::CombineRelativePath(ConfigurationNode.GetFile()->GetPath(), ConfigurationNode.ReadString("NormalMap")));
		SetNormalMapEnabled(ConfigurationNode.ReadBoolean("NormalMapEnabled", !NormalMap.IsNull()));

		SetHeightMapEnabled(ConfigurationNode.ReadBoolean("HeightEnabled", false));
		SetHeightMapTechnique((ZERNHeightMapTechnique)ConfigurationNode.ReadUInt8("HeigthMapTechnique", ZERN_HMT_NONE));
		SetHeightMapOffset(ConfigurationNode.ReadFloat("HeightMapOffset"));
		SetHeightMapScale(ConfigurationNode.ReadFloat("HeightMapScale"));

		SetOpacity(ConfigurationNode.ReadFloat("Opacity", 1.0f));
		SetOpacityMapFile(ZEPathInfo::CombineRelativePath(ConfigurationNode.GetFile()->GetPath(), ConfigurationNode.ReadString("OpacityMap")));
		SetOpacityMapEnabled(ConfigurationNode.ReadBoolean("OpacityMapEnabled", !OpacityMap.IsNull()));

		SetEnvironmentMapFile(ZEPathInfo::CombineRelativePath(ConfigurationNode.GetFile()->GetPath(), ConfigurationNode.ReadString("EnvironmentMap")));
		SetReflectionEnabled(ConfigurationNode.ReadBoolean("ReflectionEnabled", false));
		SetReflectionFactor(ConfigurationNode.ReadFloat("ReflectionFactor", 1.0f));
		SetReflectionColor(ConfigurationNode.ReadVector3("ReflectionColor", ZEVector3::One));

		SetRefractionEnabled(ConfigurationNode.ReadBoolean("RefractionEnabled", false));
		SetRefractionFactor(ConfigurationNode.ReadFloat("RefractionFactor", 1.0f));
		SetRefractionColor(ConfigurationNode.ReadVector3("RefractionColor", ZEVector3::One));
		SetRefractionIndex(ConfigurationNode.ReadFloat("RefractionIndex", 1.0f));

		SetDetailBaseMapFile(ZEPathInfo::CombineRelativePath(ConfigurationNode.GetFile()->GetPath(), ConfigurationNode.ReadString("DetailBaseMap")));
		SetDetailBaseMapEnabled(ConfigurationNode.ReadBoolean("DetailBaseMapEnabled", !DetailBaseMap.IsNull()));
		SetDetailBaseMapColor(ConfigurationNode.ReadVector3("DetailBaseMapColor", ZEVector3::One));
		SetDetailMapTiling(ConfigurationNode.ReadVector2("DetailBaseMapTiling", ZEVector2::One));
		SetDetailBaseMapAttenuationStart(ConfigurationNode.ReadFloat("DetailBaseMapAttenuationStart", 10.0f));
		SetDetailBaseMapAttenuationFactor(ConfigurationNode.ReadFloat("DetailBaseMapAttenuationFactor", 0.01f));

		SetDetailNormalMapFile(ZEPathInfo::CombineRelativePath(ConfigurationNode.GetFile()->GetPath(), ConfigurationNode.ReadString("DetailNormalMap")));
		SetDetailNormalMapEnabled(ConfigurationNode.ReadBoolean("DetailNormalMapEnabled", !DetailNormalMap.IsNull()));
		SetDetailNormalMapFactor(ConfigurationNode.ReadFloat("DetailNormalMapFactor", 1.0f));
		SetDetailNormalMapTiling(ConfigurationNode.ReadVector2("DetailNormalMapTiling", ZEVector2::One));
		SetDetailNormalMapAttenuationStart(ConfigurationNode.ReadFloat("DetailNormalMapAttenuationStart", 10.0f));
		SetDetailNormalMapAttenuationFactor(ConfigurationNode.ReadFloat("DetailNormalMapAttenuationFactor", 0.01f));

		SetClippingPlanesEnabled(ConfigurationNode.ReadBoolean("ClippingPlanesEnabled", false));
	}
}
