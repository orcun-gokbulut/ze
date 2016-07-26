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
#include "ZEResource/ZERSTemplates.h"

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

bool ZERNFixedMaterial::UpdateShaders()
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

bool ZERNFixedMaterial::UpdateRenderState()
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

bool ZERNFixedMaterial::UpdateStageMask()
{
	if (!DirtyFlags.GetFlags(ZERN_FMDF_STAGE_MASK))
		return true;

	StageMask = ZERN_STAGE_DEBUG;
	StageMask |= TransparencyEnabled ? (ZERN_STAGE_RENDER_DEPTH | ZERN_STAGE_FORWARD_TRANSPARENT) : ZERN_STAGE_GBUFFER;
	StageMask |= ShadowCaster ? ZERN_STAGE_SHADOW_MAP_GENERATION : 0;

	DirtyFlags.UnraiseFlags(ZERN_FMDF_STAGE_MASK);

	return true;
}

bool ZERNFixedMaterial::UpdateConstantBuffer()
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

bool ZERNFixedMaterial::Update()
{
	if (!ZERNMaterial::Update())
		return false;

	if (ConstantBuffer.IsNull())
		ConstantBuffer = ZEGRConstantBuffer::CreateResource(sizeof(Constants));

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

ZETaskResult ZERNFixedMaterial::LoadInternal()
{
	if (!GetFileName().IsEmpty())
	{
		ZEFile File;
		if(!File.Open(GetFileName(), ZE_FOM_READ, ZE_FCM_NONE))
			zeError("Can not open given file. File : %s", GetFileName().ToCString());

		ZEMLReader MaterialReader;
		if (!MaterialReader.Open(&File))
			return ZE_TR_FAILED;

		ZEMLReaderNode MaterialNode = MaterialReader.GetRootNode();
		if (!Unserialize(&MaterialNode))
			return ZE_TR_FAILED;
	}

	DirtyFlags.RaiseAll();
	if (!Update())
		return ZE_TR_FAILED;

	return ZE_TR_DONE;
}

ZETaskResult ZERNFixedMaterial::UnloadInternal()
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
	UnregisterExternalResource(Texture);
	SubSurfaceScatteringMap = Texture;
	RegisterExternalResource(Texture);
	SubSurfaceScatteringMapFile = "";
}

const ZEGRTexture2D* ZERNFixedMaterial::GetSubSurfaceScatteringMap() const
{
	return SubSurfaceScatteringMap;
}

void ZERNFixedMaterial::SetSubSurfaceScatteringMapFile(const ZEString& FileName)
{
	if (SubSurfaceScatteringMapFile == FileName)
		return;

	SubSurfaceScatteringMapFile = FileName;

	ZERSResourceState State = GetState();
	if (IsLoadedOrLoading())
	{
		ZEGRTexture2DOptions TextureOptions;
		TextureOptions.CompressionFormat = ZEGR_TF_BC4_UNORM;
		TextureOptions.GenerateMipMaps = true;
		TextureOptions.MaximumMipmapLevel = 0;
		TextureOptions.sRGB = false;
		UnregisterExternalResource(SubSurfaceScatteringMap);
		SubSurfaceScatteringMap = ZEGRTexture2D::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(SubSurfaceScatteringMap);
	}
}

const ZEString& ZERNFixedMaterial::GetSubSurfaceScatteringMapFile() const
{
	return SubSurfaceScatteringMapFile;
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
	UnregisterExternalResource(BaseMap);
	BaseMap = Map;
	RegisterExternalResource(BaseMap);
	BaseMapFile = "";
}

const ZEGRTexture2D* ZERNFixedMaterial::GetBaseMap() const
{
	return BaseMap;
}

void ZERNFixedMaterial::SetBaseMapFile(const ZEString& FileName)
{
	if (BaseMapFile == FileName)
		return;

	BaseMapFile = FileName;

	if (IsLoadedOrLoading())
	{
		ZEGRTexture2DOptions TextureOptions;
		TextureOptions.CompressionFormat = ZEGR_TF_BC1_UNORM_SRGB;
		TextureOptions.GenerateMipMaps = true;
		TextureOptions.MaximumMipmapLevel = 0;
		TextureOptions.sRGB = true;

		UnregisterExternalResource(BaseMap);
		BaseMap = ZEGRTexture2D::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(BaseMap);
	}
}

const ZEString& ZERNFixedMaterial::GetBaseMapFile() const
{
	return BaseMapFile;
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
	UnregisterExternalResource(SpecularMap);
	SpecularMap = Texture;
	RegisterExternalResource(SpecularMap);
	SpecularMapFile = "";
}

const ZEGRTexture2D* ZERNFixedMaterial::GetSpecularMap() const
{
	return SpecularMap;
}

void ZERNFixedMaterial::SetSpecularMapFile(const ZEString& FileName)
{
	if (SpecularMapFile == FileName)
		return;

	SpecularMapFile = FileName;

	ZERSResourceState State = GetState();
	if (IsLoadedOrLoading())
	{
		ZEGRTexture2DOptions TextureOptions;
		TextureOptions.CompressionFormat = ZEGR_TF_BC4_UNORM;
		TextureOptions.GenerateMipMaps = true;
		TextureOptions.MaximumMipmapLevel = 0;
		TextureOptions.sRGB = true;

		UnregisterExternalResource(SpecularMap);
		SpecularMap = ZEGRTexture2D::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(SpecularMap);
	}
}

const ZEString& ZERNFixedMaterial::GetSpecularMapFile() const
{
	return SpecularMapFile;
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
	UnregisterExternalResource(SpecularGlossMap);
	SpecularGlossMap = Texture;
	RegisterExternalResource(SpecularGlossMap);
	SpecularGlossMapFile = "";
}

const ZEGRTexture2D* ZERNFixedMaterial::GetSpecularGlossMap() const
{
	return SpecularGlossMap;
}

void ZERNFixedMaterial::SetSpecularGlossMapFile(const ZEString& FileName)
{
	if (SpecularGlossMapFile == FileName)
		return;

	SpecularGlossMapFile = FileName;

	ZERSResourceState State = GetState();
	if (IsLoadedOrLoading())
	{
		ZEGRTexture2DOptions TextureOptions;
		TextureOptions.CompressionFormat = ZEGR_TF_BC4_UNORM;
		TextureOptions.GenerateMipMaps = true;
		TextureOptions.MaximumMipmapLevel = 0;
		TextureOptions.sRGB = false;

		UnregisterExternalResource(SpecularGlossMap);
		SpecularGlossMap = ZEGRTexture2D::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(SpecularGlossMap);
	}
}

const ZEString& ZERNFixedMaterial::GetSpecularGlossMapFile() const
{
	return SpecularGlossMapFile;
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
	UnregisterExternalResource(EmissiveMap);
	EmissiveMap = Texture;
	RegisterExternalResource(EmissiveMap);
	EmissiveMapFile = "";
}

const ZEGRTexture2D* ZERNFixedMaterial::GetEmissiveMap() const
{
	return EmissiveMap;
}

void ZERNFixedMaterial::SetEmissiveMapFile(const ZEString& FileName)
{
	if (EmissiveMapFile == FileName)
		return;

	EmissiveMapFile = FileName;
	
	ZERSResourceState State = GetState();
	if (IsLoadedOrLoading())
	{
		ZEGRTexture2DOptions TextureOptions;
		TextureOptions.CompressionFormat = ZEGR_TF_BC1_UNORM_SRGB;
		TextureOptions.GenerateMipMaps = true;
		TextureOptions.MaximumMipmapLevel = 0;
		TextureOptions.sRGB = true;

		UnregisterExternalResource(EmissiveMap);
		EmissiveMap = ZEGRTexture2D::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(EmissiveMap);
	}
}

const ZEString& ZERNFixedMaterial::GetEmissiveMapFile() const
{
	return EmissiveMapFile;
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
	NormalMapFile = "";
}

const ZEGRTexture2D* ZERNFixedMaterial::GetNormalMap() const
{
	return NormalMap;
}

void ZERNFixedMaterial::SetNormalMapFile(const ZEString& FileName)
{
	if (NormalMapFile == FileName)
		return;
	
	NormalMapFile = FileName;

	ZERSResourceState State = GetState();
	if (IsLoadedOrLoading())
	{
		ZEGRTexture2DOptions TextureOptions;
		TextureOptions.CompressionFormat = ZEGR_TF_BC5_UNORM;
		TextureOptions.GenerateMipMaps = true;
		TextureOptions.MaximumMipmapLevel = 0;
		TextureOptions.sRGB = false;

		UnregisterExternalResource(NormalMap);
		NormalMap = ZEGRTexture2D::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(NormalMap);
	}
}

const ZEString& ZERNFixedMaterial::GetNormalMapFile() const
{
	return NormalMapFile;
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
	UnregisterExternalResource(HeightMap);
	HeightMap = Texture;
	RegisterExternalResource(HeightMap);
	HeightMapFile = "";
}

const ZEGRTexture2D* ZERNFixedMaterial::GetHeightMap() const
{
	return HeightMap;
}

void ZERNFixedMaterial::SetHeightMapFile(const ZEString& FileName)
{
	if (HeightMapFile == FileName)
		return;

	HeightMapFile = FileName;

	ZERSResourceState State = GetState();
	if (IsLoadedOrLoading())
	{
		ZEGRTexture2DOptions TextureOptions;
		TextureOptions.CompressionFormat = ZEGR_TF_BC4_UNORM;
		TextureOptions.GenerateMipMaps = true;
		TextureOptions.MaximumMipmapLevel = 0;
		TextureOptions.sRGB = false;

		UnregisterExternalResource(HeightMap);
		HeightMap = ZEGRTexture2D::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(HeightMap);
	}
}

const ZEString& ZERNFixedMaterial::GetHeightMapFile() const
{
	return HeightMapFile;
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
	OpacityMapFile = "";
}

const ZEGRTexture2D* ZERNFixedMaterial::GetOpacityMap() const
{
	return OpacityMap;
}

void ZERNFixedMaterial::SetOpacityMapFile(const ZEString& FileName)
{
	if (OpacityMapFile == FileName)
		return;

	OpacityMapFile = FileName;

	ZERSResourceState State = GetState();
	if (IsLoadedOrLoading())
	{
		ZEGRTexture2DOptions TextureOptions;
		TextureOptions.CompressionFormat = ZEGR_TF_BC4_UNORM;
		TextureOptions.GenerateMipMaps = true;
		TextureOptions.MaximumMipmapLevel = 0;
		TextureOptions.sRGB = true;

		UnregisterExternalResource(OpacityMap);
		OpacityMap = ZEGRTexture2D::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(OpacityMap);
	}
}

const ZEString& ZERNFixedMaterial::GetOpacityMapFile() const
{
	return OpacityMapFile;
}

void ZERNFixedMaterial::SetEnvironmentMapEnabled(bool Enabled)
{
	if (EnvironmentMapEnabled == Enabled)
		return;

	EnvironmentMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNFixedMaterial::GetEnvironmentMapEnabled() const
{
	return EnvironmentMapEnabled;
}

void ZERNFixedMaterial::SetEnvironmentMap(ZEGRTexture2D* Texture)
{
	EnvironmentMap = Texture;
	EnvironmentMapFile = "";
}

const ZEGRTexture2D* ZERNFixedMaterial::GetEnvironmentMap() const
{
	return EnvironmentMap;
}

void ZERNFixedMaterial::SetEnvironmentMapFile(const ZEString& FileName)
{
	if (EnvironmentMapFile == FileName)
		return;

	EnvironmentMapFile = FileName;

	if (IsLoadedOrLoading())
	{
		ZEGRTexture2DOptions TextureOptions;
		TextureOptions.CompressionFormat = ZEGR_TF_BC1_UNORM_SRGB;
		TextureOptions.GenerateMipMaps = true;
		TextureOptions.MaximumMipmapLevel = 0;
		TextureOptions.sRGB = true;

		UnregisterExternalResource(EnvironmentMap);
		EnvironmentMap = ZEGRTexture2D::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(EnvironmentMap);
	}
}

const ZEString& ZERNFixedMaterial::GetEnvironmentMapFile() const
{
	return EnvironmentMapFile;
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

void ZERNFixedMaterial::SetDetailBaseMap(ZEGRTexture2D* Texture)
{
	UnregisterExternalResource(DetailBaseMap);
	DetailBaseMap = Texture;
	RegisterExternalResource(DetailBaseMap);
	DetailBaseMapFile = "";
}

const ZEGRTexture2D* ZERNFixedMaterial::GetDetailBaseMap() const
{
	return DetailBaseMap;
}

void ZERNFixedMaterial::SetDetailBaseMapFile(const ZEString& FileName)
{
	if (DetailBaseMapFile == FileName)
		return;

	DetailBaseMapFile = FileName;

	if (IsLoadedOrLoading())
	{
		ZEGRTexture2DOptions TextureOptions;
		TextureOptions.CompressionFormat = ZEGR_TF_BC1_UNORM_SRGB;
		TextureOptions.GenerateMipMaps = true;
		TextureOptions.MaximumMipmapLevel = 0;
		TextureOptions.sRGB = true;

		UnregisterExternalResource(DetailBaseMap);
		DetailBaseMap = ZEGRTexture2D::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(DetailBaseMap);
	}
}

const ZEString& ZERNFixedMaterial::GetDetailBaseMapFile() const
{
	return DetailBaseMapFile;
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

void ZERNFixedMaterial::SetDetailNormalMap(ZEGRTexture2D* Texture)
{
	UnregisterExternalResource(DetailNormalMap);
	DetailNormalMap = Texture;
	RegisterExternalResource(DetailNormalMap);
	DetailNormalMapFile = "";
}

const ZEGRTexture2D* ZERNFixedMaterial::GetDetailNormalMap() const
{
	return DetailNormalMap;
}

void ZERNFixedMaterial::SetDetailNormalMapFile(const ZEString& FileName)
{
	if (DetailNormalMapFile == FileName)
		return;

	DetailNormalMapFile = FileName;

	if (IsLoadedOrLoading())
	{
		ZEGRTexture2DOptions TextureOptions;
		TextureOptions.CompressionFormat = ZEGR_TF_BC5_UNORM;
		TextureOptions.GenerateMipMaps = true;
		TextureOptions.MaximumMipmapLevel = 0;
		TextureOptions.sRGB = false;

		UnregisterExternalResource(DetailNormalMap);
		DetailNormalMap = ZEGRTexture2D::LoadResourceShared(FileName, TextureOptions);
		RegisterExternalResource(DetailNormalMap);
	}
}

const ZEString& ZERNFixedMaterial::GetDetailNormalMapFile() const
{
	return DetailNormalMapFile;
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

	if (!const_cast<ZERNFixedMaterial*>(this)->Update())
		return false;

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_MATERIAL, ConstantBuffer);

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
				Context->SetSampler(ZEGR_ST_PIXEL, 0, Sampler);
				Context->SetTexture(ZEGR_ST_PIXEL, 5, OpacityMap);
			}
		}
	}

	return true;
}

void ZERNFixedMaterial::CleanupMaterial(ZEGRContext* Context, const ZERNStage* Stage) const
{
	ZERNMaterial::CleanupMaterial(Context, Stage);
}

bool ZERNFixedMaterial::Serialize(ZEMLWriterNode* MaterialNode)
{
	MaterialNode->WriteUInt8("MajorVersion", 2);
	MaterialNode->WriteUInt8("MinorVersion", 2);
	MaterialNode->WriteString("GUID", GetGUID().ToString());
	MaterialNode->WriteString("Class", GetClass()->GetName());

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

bool ZERNFixedMaterial::Unserialize(ZEMLReaderNode* MaterialNode)
{
	ZEString FileName = MaterialNode->GetFile()->GetPath();

	zeCheckError(MaterialNode == NULL || !MaterialNode->IsValid(), false, "ZERNStandardMaterial loading failed. ZEML Root Node is not valid. File : \"%s\"", FileName.ToCString());

	ZEUInt8 FileMajorVersion = MaterialNode->ReadUInt8("MajorVersion", 0);
	if (FileMajorVersion < 2)
		zeWarning("ZERNStandardMaterial loading warning. Old depricated file version detected. Current Version: \"2.0\" Detected Version: \"%d.0\" File Name: \"%s\".", FileMajorVersion, FileName.ToCString());
	
	ZEString ClassName = MaterialNode->ReadString("Class");
	if (ClassName.IsEmpty())
		zeWarning("Class tag in ZERNMaterial is empty. Unknown material class. This can cause problems. File Name: \"%s\".", FileName.ToCString());
	else
		zeCheckError(ClassName != GetClass()->GetName(), false, "Cannot load ZERNStandardMaterial. Class is different than ZERNStandartMaterial. Class Name: \"%s\", File Name: \"%s\".", ClassName.ToCString(), FileName.ToCString());
	
	ZEMLReaderNode PropertiesNode;
	if (FileMajorVersion < 2)
		PropertiesNode = *MaterialNode;
	else
		PropertiesNode = MaterialNode->GetNode("Properties");
	
	zeCheckError(!PropertiesNode.IsValid(), false, "ZERNStandardMaterial loading failed. ZEML \"Properties\" Node is not valid. File : \"%s\"", FileName.ToCString());

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

ZEHolder<ZERNFixedMaterial> ZERNFixedMaterial::CreateInstance()
{
	return new ZERNFixedMaterial;
}

ZERSHolder<ZERNFixedMaterial> ZERNFixedMaterial::LoadResource(const ZEString& FileName)
{
	return ZERSTemplates::LoadResource<ZERNFixedMaterial>(FileName, ZERSTemplates::InstanciatorFunction<ZERNFixedMaterial>);
}

ZERSHolder<const ZERNFixedMaterial> ZERNFixedMaterial::LoadResourceShared(const ZEString& FileName)
{
	return ZERSTemplates::LoadResourceShared<ZERNFixedMaterial>(FileName, ZERSTemplates::InstanciatorFunction<ZERNFixedMaterial>);
}
