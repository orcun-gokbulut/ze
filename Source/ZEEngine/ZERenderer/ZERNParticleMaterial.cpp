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

#include "ZERNStageID.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZERNStageParticleRendering.h"
#include "ZERNShaderSlots.h"

#define ZERN_PMDF_CONSTANT_BUFFER		1
#define ZERN_PMDF_RENDER_STATE			2
#define ZERN_PMDF_SHADERS				4

void ZERNParticleMaterial::UpdateShaderDefinitions(ZEGRShaderCompileOptions& Options) const
{
	Options.Definitions.Clear();

	if (AlphaCullEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_PM_ALPHA_CULL"));

	if (VertexColorEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_PM_VERTEX_COLOR"));

	if (DiffuseMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_PM_DIFFUSE_MAP"));

	if (EmissiveMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_PM_EMISSIVE_MAP"));

	if (NormalMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_PM_NORMAL_MAP"));

	if (OpacityMapEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_PM_OPACITY_MAP"));
}

bool ZERNParticleMaterial::UpdateShaders() const
{
	if (!DirtyFlags.GetFlags(ZERN_PMDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;

	UpdateShaderDefinitions(Options);
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString(ZEGRGraphicsModule::SAMPLE_COUNT)));
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

bool ZERNParticleMaterial::UpdateRenderState() const
{
	if (!DirtyFlags.GetFlags(ZERN_PMDF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState = ZERNStageParticleRendering::GetRenderState();
	RenderState.SetShader(ZEGR_ST_VERTEX, StageParticleRendering_VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, StageParticleRendering_PixelShader);

	StageParticleRendering_RenderState = RenderState.Compile();
	zeCheckError(StageParticleRendering_RenderState == NULL, false, "Cannot set particle rendering render state.");

	DirtyFlags.UnraiseFlags(ZERN_PMDF_RENDER_STATE);

	return true;
}

bool ZERNParticleMaterial::UpdateConstantBuffer() const
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

bool ZERNParticleMaterial::InitializeSelf()
{
	if (!ZERNMaterial::InitializeSelf())
		return false;

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));

	if (!UpdateShaders())
		return false;

	if (!UpdateRenderState())
		return false;

	if (!UpdateConstantBuffer())
		return false;

	return true;
}

void ZERNParticleMaterial::DeinitializeSelf()
{
	DirtyFlags.RaiseAll();

	StageParticleRendering_VertexShader.Release();
	StageParticleRendering_PixelShader.Release();
	StageParticleRendering_RenderState.Release();

	ConstantBuffer.Release();

	ZERNMaterial::DeinitializeSelf();
}

ZERNParticleMaterial::ZERNParticleMaterial()
{
	DirtyFlags.RaiseAll();

	ShadowCaster = false;
	AlphaCullEnabled = false;
	VertexColorEnabled = false;
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
	Constants.AlphaCullLimit = 1.0f;
	Constants.SoftParticle = false;
	Constants.DistanceMax = 2.0f;
	Constants.ContrastPower = 2.0f;

	ZEGRSamplerDescription SamplerLinearWrapDescription;
	SamplerLinearWrapDescription.AddressU = ZEGR_TAM_WRAP;
	SamplerLinearWrapDescription.AddressV = ZEGR_TAM_WRAP;
	SamplerLinearWrapDescription.AddressW = ZEGR_TAM_WRAP;
	Sampler = ZEGRSampler::GetSampler(SamplerLinearWrapDescription);
}

ZEUInt ZERNParticleMaterial::GetStageMask() const
{
	return ZERN_STAGE_PARTICLE_RENDERING;
}

void ZERNParticleMaterial::SetFileName(const ZEString& Filename)
{
	this->Filename = Filename;
}

const ZEString& ZERNParticleMaterial::GetFileName() const
{
	return Filename;
}

void ZERNParticleMaterial::SetSampler(ZEHolder<ZEGRSampler> Sampler)
{
	this->Sampler = Sampler;
}

ZEHolder<ZEGRSampler> ZERNParticleMaterial::GetSampler() const
{
	return Sampler;
}

void ZERNParticleMaterial::SetDiffuseMap(const ZERNMap& DiffuseMap)
{
	this->DiffuseMap = DiffuseMap;
}

const ZERNMap& ZERNParticleMaterial::GetDiffuseMap() const
{
	return DiffuseMap;
}

void ZERNParticleMaterial::SetDiffuseMapFile(const ZEString& Filename)
{
	if(Filename.IsEmpty())
		return;

	DiffuseMap.Load2D(Filename);
}

const ZEString& ZERNParticleMaterial::GetDiffuseMapFile() const
{
	return DiffuseMap.GetTextureFile();
}

void ZERNParticleMaterial::SetEmissiveMap(const ZERNMap& EmissiveMap)
{
	this->EmissiveMap = EmissiveMap;
}

const ZERNMap& ZERNParticleMaterial::GetEmissiveMap() const
{
	return EmissiveMap;
}

void ZERNParticleMaterial::SetEmissiveMapFile(const ZEString& Filename)
{
	if(Filename.IsEmpty())
		return;

	EmissiveMap.Load2D(Filename);
}

const ZEString& ZERNParticleMaterial::GetEmissiveMapFile() const
{
	return EmissiveMap.GetTextureFile();
}

void ZERNParticleMaterial::SetNormalMap(const ZERNMap& NormalMap)
{
	this->NormalMap = NormalMap;
}

const ZERNMap& ZERNParticleMaterial::GetNormalMap() const
{
	return NormalMap;
}

void ZERNParticleMaterial::SetNormalMapFile(const ZEString& Filename)
{
	if(Filename.IsEmpty())
		return;

	NormalMap.Load2D(Filename);
}

const ZEString& ZERNParticleMaterial::GetNormalMapFile() const
{
	return NormalMap.GetTextureFile();
}

void ZERNParticleMaterial::SetOpacityMap(const ZERNMap& OpacityMap)
{
	this->OpacityMap = OpacityMap;
}

const ZERNMap& ZERNParticleMaterial::GetOpacityMap() const
{
	return OpacityMap;
}

void ZERNParticleMaterial::SetOpacityMapFile(const ZEString& Filename)
{
	if(Filename.IsEmpty())
		return;

	OpacityMap.Load2D(Filename);
}

const ZEString& ZERNParticleMaterial::GetOpacityMapFile() const
{
	return OpacityMap.GetTextureFile();
}

void ZERNParticleMaterial::SetSoftParticle(bool SoftParticle)
{
	if (Constants.SoftParticle == SoftParticle)
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

bool ZERNParticleMaterial::GetDistanceMax() const
{
	return Constants.DistanceMax;
}

void ZERNParticleMaterial::SetContrastPower(bool ContrastPower)
{
	if (Constants.ContrastPower == ContrastPower)
		return;

	Constants.ContrastPower = ContrastPower;

	DirtyFlags.RaiseFlags(ZERN_PMDF_CONSTANT_BUFFER);
}

bool ZERNParticleMaterial::GetContrastPower() const
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

bool ZERNParticleMaterial::SetupMaterial(ZEGRContext* Context, const ZERNStage* Stage) const
{
	if (!ZERNMaterial::SetupMaterial(Context, Stage))
		return false;

	if (!Update())
		return false;

	Context->SetSamplers(ZEGR_ST_PIXEL, 0, 1, Sampler.GetPointerToPointer());
	const ZEGRTexture* Textures[] = {DiffuseMap.GetTexture(), EmissiveMap.GetTexture(), NormalMap.GetTexture(), OpacityMap.GetTexture()};
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 4, Textures);

	Context->SetConstantBuffers(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_MATERIAL, 1, ConstantBuffer.GetPointerToPointer());
	Context->SetRenderState(StageParticleRendering_RenderState);

	return true;
}

void ZERNParticleMaterial::CleanupMaterial(ZEGRContext* Context, const ZERNStage* Stage) const
{
	ZERNMaterial::CleanupMaterial(Context, Stage);
}

bool ZERNParticleMaterial::Update() const
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
