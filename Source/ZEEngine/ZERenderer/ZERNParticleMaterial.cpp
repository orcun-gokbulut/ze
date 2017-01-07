//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNParticleMaterial.cpp
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

#include "ZERNParticleMaterial.h"

#include "ZEFile/ZEPathInfo.h"
#include "ZERNStageID.h"
#include "ZERNStageParticleRendering.h"
#include "ZERNShaderSlots.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"

#define ZERN_PMDF_CONSTANT_BUFFER		1
#define ZERN_PMDF_RENDER_STATE			2
#define ZERN_PMDF_SHADERS				4

void ZERNParticleMaterial::UpdateShaderDefinitions(ZEGRShaderCompileOptions& Options)
{
	Options.Definitions.Clear();

	if (AlphaCullEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_PM_ALPHA_CULL"));

	if (DiffuseMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_PM_DIFFUSE_MAP"));

	if (EmissiveMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_PM_EMISSIVE_MAP"));

	if (NormalMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_PM_NORMAL_MAP"));

	if (OpacityMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_PM_OPACITY_MAP"));
}

bool ZERNParticleMaterial::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_PMDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;

	UpdateShaderDefinitions(Options);

	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString::FromInt32(ZEGRGraphicsModule::SAMPLE_COUNT)));
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNParticleMaterial.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNParticleMaterial_VertexShader_Main";
	StageParticleRendering_VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(StageParticleRendering_VertexShader == NULL, false, "Can not compile vertex shader");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNParticleMaterial_PixelShader_Main";
	StageParticleRendering_PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(StageParticleRendering_PixelShader == NULL, false, "Can not compile pixel shader");

	DirtyFlags.UnraiseFlags(ZERN_PMDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_PMDF_RENDER_STATE);

	return true;
}

bool ZERNParticleMaterial::UpdateRenderState()
{
	if (!DirtyFlags.GetFlags(ZERN_PMDF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState = ZERNStageParticleRendering::GetRenderState();

	ZEGRRasterizerState RasterizerState = RenderState.GetRasterizerState();
	RasterizerState.SetCullMode(TwoSided ? ZEGR_CMD_NONE : RasterizerState.GetCullMode());
	RenderState.SetRasterizerState(RasterizerState);

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
	else if (TransparencyMode == ZERN_TM_ORDERED)
	{
		ZEGRBlendRenderTarget RenderTargetBlend;
		RenderTargetBlend.SetBlendEnable(true);
		RenderTargetBlend.SetSource(ZEGR_BO_SRC_ALPHA);
		RenderTargetBlend.SetDestination(ZEGR_BO_INV_SRC_ALPHA);
		RenderTargetBlend.SetOperation(ZEGR_BE_ADD);
		
		ZEGRBlendState BlendState;
		BlendState.SetBlendEnable(true);
		BlendState.SetRenderTargetBlend(0, RenderTargetBlend);	
		RenderState.SetBlendState(BlendState);
	}
	else
	{
		ZEGRBlendState BlendState;
		RenderState.SetBlendState(BlendState);
	}
	RenderState.SetShader(ZEGR_ST_VERTEX, StageParticleRendering_VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, StageParticleRendering_PixelShader);

	StageParticleRendering_RenderState = RenderState.Compile();
	zeCheckError(StageParticleRendering_RenderState == NULL, false, "Cannot set particle rendering render state.");

	DirtyFlags.UnraiseFlags(ZERN_PMDF_RENDER_STATE);

	return true;
}

bool ZERNParticleMaterial::UpdateConstantBuffer()
{
	if (!DirtyFlags.GetFlags(ZERN_PMDF_CONSTANT_BUFFER))
		return true;

	Constants.AmbientColor = (AmbientEnabled ? 1.0f : 0.0f) * AmbientFactor * AmbientColor;
	Constants.DiffuseColor = (DiffuseEnabled ? 1.0f : 0.0f) * DiffuseFactor * DiffuseColor;
	Constants.EmissiveColor = (EmissiveEnabled ? 1.0f : 0.0f) * EmissiveFactor * EmissiveColor;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_PMDF_CONSTANT_BUFFER);

	return true;
}

ZETaskResult ZERNParticleMaterial::LoadInternal()
{
	SetDiffuseMapFileName(DiffuseMapFileName);
	SetEmissiveMapFileName(EmissiveMapFileName);
	SetNormalMapFileName(NormalMapFileName);
	SetOpacityMapFileName(OpacityMapFileName);

	DirtyFlags.RaiseAll();

	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);
	if (ConstantBuffer == NULL)
		return ZE_TR_FAILED;

	if (!UpdateShaders())
		return ZE_TR_FAILED;

	if (!UpdateRenderState())
		return ZE_TR_FAILED;

	if (!UpdateConstantBuffer())
		return ZE_TR_FAILED;

	return ZE_TR_DONE;
}

ZETaskResult ZERNParticleMaterial::UnloadInternal()
{
	DirtyFlags.RaiseAll();

	StageParticleRendering_VertexShader.Release();
	StageParticleRendering_PixelShader.Release();
	StageParticleRendering_RenderState.Release();

	ConstantBuffer.Release();
	Sampler.Release();
	DiffuseMap.Release();
	EmissiveMap.Release();
	NormalMap.Release();
	OpacityMap.Release();

	return ZE_TR_DONE;
}

ZERNParticleMaterial::ZERNParticleMaterial()
{
	DirtyFlags.RaiseAll();

	TwoSided = false;
	ShadowCaster = false;
	AlphaCullEnabled = false;
	VertexColorEnabled = false;
	TransparencyMode = ZERN_TM_ADDITIVE;
	AmbientEnabled = true;
	AmbientFactor = 1.0f;
	AmbientColor = ZEVector3::One;
	DiffuseMapEnabled = false;
	DiffuseEnabled = false;
	DiffuseFactor = 1.0f;
	DiffuseColor = ZEVector3::One;
	EmissiveMapEnabled = false;
	EmissiveEnabled = false;
	EmissiveFactor = 1.0f;
	EmissiveColor = ZEVector3::One;
	NormalMapEnabled = false;
	OpacityMapEnabled = false;

	Constants.Opacity = 1.0f;
	Constants.SceneAmbientEnabled = false;
	Constants.AlphaCullLimit = 0.0f;
	Constants.SoftParticle = false;
	Constants.DistanceMax = 2.0f;
	Constants.ContrastPower = 2.0f;

	ZEGRSamplerDescription SamplerLinearWrapDescription;
	SamplerLinearWrapDescription.AddressU = ZEGR_TAM_WRAP;
	SamplerLinearWrapDescription.AddressV = ZEGR_TAM_WRAP;
	SamplerLinearWrapDescription.AddressW = ZEGR_TAM_WRAP;
	Sampler = ZEGRSampler::GetSampler(SamplerLinearWrapDescription);

	Register();
}

ZERNParticleMaterial::~ZERNParticleMaterial()
{
	Unregister();
}

ZEUInt ZERNParticleMaterial::GetStageMask() const
{
	return ZERN_STAGE_DEBUG | ZERN_STAGE_PARTICLE_RENDERING;
}

void ZERNParticleMaterial::SetTwoSided(bool TwoSided)
{
	if (this->TwoSided == TwoSided)
		return;

	this->TwoSided = TwoSided;

	DirtyFlags.RaiseFlags(ZERN_PMDF_RENDER_STATE);
}

bool ZERNParticleMaterial::GetTwoSided() const
{
	return TwoSided;
}

void ZERNParticleMaterial::SetTransparencyMode(ZERNTransparencyMode Mode)
{
	if (this->TransparencyMode == Mode)
		return;

	this->TransparencyMode = Mode;

	DirtyFlags.RaiseFlags(ZERN_PMDF_RENDER_STATE | ZERN_PMDF_SHADERS);
}

ZERNTransparencyMode ZERNParticleMaterial::GetTransparencyMode() const
{
	return TransparencyMode;
}

void ZERNParticleMaterial::SetSampler(const ZEGRSampler* Sampler)
{
	this->Sampler = Sampler;
}

const ZEGRSampler* ZERNParticleMaterial::GetSampler() const
{
	return Sampler;
}

void ZERNParticleMaterial::SetDiffuseMap(const ZEGRTexture* DiffuseMap)
{
	UnregisterExternalResource(DiffuseMap);
	this->DiffuseMap = DiffuseMap;
	RegisterExternalResource(DiffuseMap);

	DiffuseMapFileName = (DiffuseMap != NULL ? DiffuseMap->GetFileName() : "");
}

const ZEGRTexture* ZERNParticleMaterial::GetDiffuseMap() const
{
	return DiffuseMap;
}

void ZERNParticleMaterial::SetDiffuseMapFileName(const ZEString& FileName)
{
	if (!ZEPathInfo::Compare(DiffuseMapFileName, FileName))
	{
		DiffuseMapFileName = FileName;

		if (DiffuseMapFileName.IsEmpty())
			DiffuseMap.Release();
	}

	if (IsLoadedOrLoading() && !DiffuseMapFileName.IsEmpty())
	{
		ZEGRTextureOptions Options;
		Options.Type = ZEGR_TT_2D;

		UnregisterExternalResource(DiffuseMap);
		DiffuseMap = ZEGRTexture::LoadResourceShared(DiffuseMapFileName, Options);
		RegisterExternalResource(DiffuseMap);
	}
}

const ZEString& ZERNParticleMaterial::GetDiffuseMapFileName() const
{
	return DiffuseMapFileName;
}

void ZERNParticleMaterial::SetEmissiveMap(const ZEGRTexture* EmissiveMap)
{
	UnregisterExternalResource(EmissiveMap);
	this->EmissiveMap = EmissiveMap;
	RegisterExternalResource(EmissiveMap);

	EmissiveMapFileName = (EmissiveMap != NULL ? EmissiveMap->GetFileName() : "");
}

const ZEGRTexture* ZERNParticleMaterial::GetEmissiveMap() const
{
	return EmissiveMap;
}

void ZERNParticleMaterial::SetEmissiveMapFileName(const ZEString& FileName)
{
	if (!ZEPathInfo::Compare(EmissiveMapFileName, FileName))
	{
		EmissiveMapFileName = FileName;

		if (EmissiveMapFileName.IsEmpty())
			EmissiveMap.Release();
	}

	if (IsLoadedOrLoading() && !EmissiveMapFileName.IsEmpty())
	{
		ZEGRTextureOptions Options;
		Options.Type = ZEGR_TT_2D;

		UnregisterExternalResource(EmissiveMap);
		EmissiveMap = ZEGRTexture::LoadResourceShared(EmissiveMapFileName, Options);
		RegisterExternalResource(EmissiveMap);
	}
}

const ZEString& ZERNParticleMaterial::GetEmissiveMapFileName() const
{
	return EmissiveMapFileName;
}

void ZERNParticleMaterial::SetNormalMap(const ZEGRTexture* NormalMap)
{
	this->NormalMap = NormalMap;

	UnregisterExternalResource(NormalMap);
	NormalMapFileName = (NormalMap != NULL ? NormalMap->GetFileName() : "");
	RegisterExternalResource(NormalMap);
}

const ZEGRTexture* ZERNParticleMaterial::GetNormalMap() const
{
	return NormalMap;
}

void ZERNParticleMaterial::SetNormalMapFileName(const ZEString& FileName)
{
	if (!ZEPathInfo::Compare(EmissiveMapFileName, FileName))
	{
		EmissiveMapFileName = FileName;

		if (EmissiveMapFileName.IsEmpty())
			EmissiveMap.Release();
	}

	if (IsLoadedOrLoading() && !EmissiveMapFileName.IsEmpty())
	{
		ZEGRTextureOptions Options;
		Options.Type = ZEGR_TT_2D;

		UnregisterExternalResource(EmissiveMap);
		EmissiveMap = ZEGRTexture::LoadResourceShared(EmissiveMapFileName, Options);
		RegisterExternalResource(EmissiveMap);
	}
}

const ZEString& ZERNParticleMaterial::GetNormalMapFileName() const
{
	return NormalMapFileName;
}

void ZERNParticleMaterial::SetOpacityMap(const ZEGRTexture* Texture)
{
	UnregisterExternalResource(OpacityMap);
	OpacityMap = Texture;
	RegisterExternalResource(OpacityMap);

	OpacityMapFileName = (OpacityMap != NULL ? OpacityMap->GetFileName() : "");
}

const ZEGRTexture* ZERNParticleMaterial::GetOpacityMap() const
{
	return OpacityMap;
}

void ZERNParticleMaterial::SetOpacityMapFileName(const ZEString& FileName)
{
	if (!ZEPathInfo::Compare(OpacityMapFileName, FileName))
	{
		OpacityMapFileName = FileName;

		if (OpacityMapFileName.IsEmpty())
			OpacityMap.Release();
	}

	if (IsLoadedOrLoading() && !OpacityMapFileName.IsEmpty())
	{
		ZEGRTextureOptions Options;
		Options.Type = ZEGR_TT_2D;

		UnregisterExternalResource(OpacityMap);
		OpacityMap = ZEGRTexture::LoadResourceShared(OpacityMapFileName, Options);
		RegisterExternalResource(OpacityMap);
	}
}

const ZEString& ZERNParticleMaterial::GetOpacityMapFileName() const
{
	return OpacityMapFileName;
}

void ZERNParticleMaterial::SetSoftParticle(bool SoftParticle)
{
	if ((bool)Constants.SoftParticle == SoftParticle)
		return;

	Constants.SoftParticle = SoftParticle;

	DirtyFlags.RaiseFlags(ZERN_PMDF_CONSTANT_BUFFER);
}

bool ZERNParticleMaterial::GetSoftParticle() const
{
	return Constants.SoftParticle;
}

void ZERNParticleMaterial::SetDistanceMax(float DistanceMax)
{
	if (Constants.DistanceMax == DistanceMax)
		return;

	Constants.DistanceMax = DistanceMax;

	DirtyFlags.RaiseFlags(ZERN_PMDF_CONSTANT_BUFFER);
}

float ZERNParticleMaterial::GetDistanceMax() const
{
	return Constants.DistanceMax;
}

void ZERNParticleMaterial::SetContrastPower(float ContrastPower)
{
	if (Constants.ContrastPower == ContrastPower)
		return;

	Constants.ContrastPower = ContrastPower;

	DirtyFlags.RaiseFlags(ZERN_PMDF_CONSTANT_BUFFER);
}

float ZERNParticleMaterial::GetContrastPower() const
{
	return Constants.ContrastPower;
}

void ZERNParticleMaterial::SetShadowCaster(bool ShadowCaster)
{
	this->ShadowCaster = ShadowCaster;
}

bool ZERNParticleMaterial::GetShadowCaster() const
{
	return ShadowCaster;
}

void ZERNParticleMaterial::SetAlphaCullEnabled(bool AlphaCullEnabled)
{
	if (this->AlphaCullEnabled == AlphaCullEnabled)
		return;

	this->AlphaCullEnabled = AlphaCullEnabled;

	DirtyFlags.RaiseFlags(ZERN_PMDF_SHADERS);
}

bool ZERNParticleMaterial::GetAlphaCullEnabled() const
{
	return AlphaCullEnabled;
}

void ZERNParticleMaterial::SetVertexColorEnabled(bool VertexColorEnabled)
{
	if (this->VertexColorEnabled == VertexColorEnabled)
		return;

	this->VertexColorEnabled = VertexColorEnabled;

	DirtyFlags.RaiseFlags(ZERN_PMDF_SHADERS);
}

bool ZERNParticleMaterial::GetVertexColorEnabled() const
{
	return VertexColorEnabled;
}

void ZERNParticleMaterial::SetAmbientEnabled(bool AmbientEnabled)
{
	if (this->AmbientEnabled == AmbientEnabled)
		return;

	this->AmbientEnabled = AmbientEnabled;

	DirtyFlags.RaiseFlags(ZERN_PMDF_CONSTANT_BUFFER);
}

bool ZERNParticleMaterial::GetAmbientEnabled() const
{
	return AmbientEnabled;
}

void ZERNParticleMaterial::SetAmbientFactor(float AmbientFactor)
{
	if (this->AmbientFactor == AmbientFactor)
		return;

	this->AmbientFactor = AmbientFactor;

	DirtyFlags.RaiseFlags(ZERN_PMDF_CONSTANT_BUFFER);
}

float ZERNParticleMaterial::GetAmbientFactor() const
{
	return AmbientFactor;
}

void ZERNParticleMaterial::SetAmbientColor(const ZEVector3& AmbientColor)
{
	if (this->AmbientColor == AmbientColor)
		return;

	this->AmbientColor = AmbientColor;

	DirtyFlags.RaiseFlags(ZERN_PMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNParticleMaterial::GetAmbientColor() const
{
	return AmbientColor;
}

void ZERNParticleMaterial::SetDiffuseMapEnabled(bool DiffuseMapEnabled)
{
	if (this->DiffuseMapEnabled == DiffuseMapEnabled)
		return;

	this->DiffuseMapEnabled = DiffuseMapEnabled;

	DirtyFlags.RaiseFlags(ZERN_PMDF_SHADERS);
}

bool ZERNParticleMaterial::GetDiffuseMapEnabled() const
{
	return DiffuseMapEnabled;
}

void ZERNParticleMaterial::SetDiffuseEnabled(bool DiffuseEnabled)
{
	if (this->DiffuseEnabled == DiffuseEnabled)
		return;

	this->DiffuseEnabled = DiffuseEnabled;

	DirtyFlags.RaiseFlags(ZERN_PMDF_CONSTANT_BUFFER);
}

bool ZERNParticleMaterial::GetDiffuseEnabled() const
{
	return DiffuseEnabled;
}

void ZERNParticleMaterial::SetDiffuseFactor(float DiffuseFactor)
{
	if (this->DiffuseFactor == DiffuseFactor)
		return;

	this->DiffuseFactor = DiffuseFactor;

	DirtyFlags.RaiseFlags(ZERN_PMDF_CONSTANT_BUFFER);
}

float ZERNParticleMaterial::GetDiffuseFactor() const
{
	return DiffuseFactor;
}

void ZERNParticleMaterial::SetDiffuseColor(const ZEVector3& DiffuseColor)
{
	if (this->DiffuseColor == DiffuseColor)
		return;

	this->DiffuseColor = DiffuseColor;

	DirtyFlags.RaiseFlags(ZERN_PMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNParticleMaterial::GetDiffuseColor() const
{
	return DiffuseColor;
}

void ZERNParticleMaterial::SetEmissiveMapEnabled(bool EmissiveMapEnabled)
{
	if (this->EmissiveMapEnabled == EmissiveMapEnabled)
		return;

	this->EmissiveMapEnabled = EmissiveMapEnabled;

	DirtyFlags.RaiseFlags(ZERN_PMDF_SHADERS);
}

bool ZERNParticleMaterial::GetEmissiveMapEnabled() const
{
	return EmissiveMapEnabled;
}

void ZERNParticleMaterial::SetEmissiveEnabled(bool EmissiveEnabled)
{
	if (this->EmissiveEnabled == EmissiveEnabled)
		return;

	this->EmissiveEnabled = EmissiveEnabled;

	DirtyFlags.RaiseFlags(ZERN_PMDF_CONSTANT_BUFFER);
}

bool ZERNParticleMaterial::GetEmissiveEnabled() const
{
	return EmissiveEnabled;
}

void ZERNParticleMaterial::SetEmissiveFactor(float EmissiveFactor)
{
	if (this->EmissiveFactor == EmissiveFactor)
		return;

	this->EmissiveFactor = EmissiveFactor;

	DirtyFlags.RaiseFlags(ZERN_PMDF_CONSTANT_BUFFER);
}

float ZERNParticleMaterial::GetEmissiveFactor() const
{
	return EmissiveFactor;
}

void ZERNParticleMaterial::SetEmissiveColor(const ZEVector3& EmissiveColor)
{
	if (this->EmissiveColor == EmissiveColor)
		return;

	this->EmissiveColor = EmissiveColor;

	DirtyFlags.RaiseFlags(ZERN_PMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNParticleMaterial::GetEmissiveColor() const
{
	return EmissiveColor;
}

void ZERNParticleMaterial::SetNormalMapEnabled(bool NormalMapEnabled)
{
	if (this->NormalMapEnabled == NormalMapEnabled)
		return;

	this->NormalMapEnabled = NormalMapEnabled;

	DirtyFlags.RaiseFlags(ZERN_PMDF_SHADERS);
}

bool ZERNParticleMaterial::GetNormalMapEnabled() const
{
	return NormalMapEnabled;
}

void ZERNParticleMaterial::SetOpacityMapEnabled(bool OpacityMapEnabled)
{
	if (this->OpacityMapEnabled == OpacityMapEnabled)
		return;

	this->OpacityMapEnabled = OpacityMapEnabled;

	DirtyFlags.RaiseFlags(ZERN_PMDF_SHADERS);
}

bool ZERNParticleMaterial::GetOpacityMapEnabled() const
{
	return OpacityMapEnabled;
}

void ZERNParticleMaterial::SetOpacity(float Opacity)
{
	if (Constants.Opacity == Opacity)
		return;

	Constants.Opacity = Opacity;

	DirtyFlags.RaiseFlags(ZERN_PMDF_CONSTANT_BUFFER);
}

float ZERNParticleMaterial::GetOpacity() const
{
	return Constants.Opacity;
}

void ZERNParticleMaterial::SetSceneAmbientEnabled(bool SceneAmbientEnabled)
{
	if (Constants.SceneAmbientEnabled == (ZEBool32)SceneAmbientEnabled)
		return;

	Constants.SceneAmbientEnabled = (ZEBool32)SceneAmbientEnabled;

	DirtyFlags.RaiseFlags(ZERN_PMDF_CONSTANT_BUFFER);
}

bool ZERNParticleMaterial::GetSceneAmbientEnabled() const
{
	return (bool)Constants.SceneAmbientEnabled;
}

void ZERNParticleMaterial::SetAlphaCullLimit(float AlphaCullLimit)
{
	if (Constants.AlphaCullLimit == AlphaCullLimit)
		return;

	Constants.AlphaCullLimit = AlphaCullLimit;

	DirtyFlags.RaiseFlags(ZERN_PMDF_CONSTANT_BUFFER);
}

float ZERNParticleMaterial::GetAlphaCullLimit() const
{
	return Constants.AlphaCullLimit;
}

bool ZERNParticleMaterial::SetupMaterial(ZEGRContext* Context, const ZERNStage* Stage, bool Instanced) const
{
	if (!ZERNMaterial::SetupMaterial(Context, Stage))
		return false;

	if (!const_cast<ZERNParticleMaterial*>(this)->Update())
		return false;

	Context->SetSampler(ZEGR_ST_PIXEL, 0, Sampler);
	const ZEGRTexture* Textures[] = {DiffuseMap.GetPointer(), EmissiveMap.GetPointer(), NormalMap.GetPointer(), OpacityMap.GetPointer()};
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 4, Textures);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_MATERIAL, ConstantBuffer);
	Context->SetRenderState(StageParticleRendering_RenderState);

	return true;
}

void ZERNParticleMaterial::CleanupMaterial(ZEGRContext* Context, const ZERNStage* Stage, bool Instanced) const
{
	ZERNMaterial::CleanupMaterial(Context, Stage);
}

bool ZERNParticleMaterial::Update()
{
	if (!ZERNMaterial::Update())
		return false;

	if (!UpdateShaders())
		return false;

	if (!UpdateRenderState())
		return false;

	if (!UpdateConstantBuffer())
		return false;

	return true;
}

ZEHolder<ZERNParticleMaterial> ZERNParticleMaterial::CreateInstance()
{
	return new ZERNParticleMaterial();
}
