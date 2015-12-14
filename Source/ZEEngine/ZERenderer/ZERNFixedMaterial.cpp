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
#include "ZEGraphics/ZEGRShaderCompileOptions.h"
#include "ZERNStageGBuffer.h"
#include "ZERNStageID.h"
#include "ZERNShaderSlots.h"
#include "ZEModel/ZEModelResource.h"
#include "ZECanvas.h"
#include "ZEInterior/ZEInterior.h"
#include "ZEInterior/ZEInteriorResource.h"

#define ZERN_FMDF_CONSTANT_BUFFER		1
#define ZERN_FMDF_RENDER_STATE			2
#define ZERN_FMDF_SHADERS				4

void ZERNFixedMaterial::UpdateShaderDefinitions(ZEGRShaderCompileOptions& Options)
{
	if (AlphaCullEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_ALPHA_CULL"));

	if (SkinningEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_SKIN_TRANSFORM"));

	if (VertexColorEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_VERTEX_COLOR"));

	if (TransparencyEnabled)
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_OPACITY_BASE_ALPHA"));

	if(BaseMap.IsAvailable())
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_BASE_MAP"));

	if(NormalMapEnabled && NormalMap.IsAvailable())
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_NORMAL_MAP"));

	if(EmissiveEnabled && EmissiveMap.IsAvailable())
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_EMISSIVE_MAP"));

	if(ReflectionEnabled && EnvironmentMap.IsAvailable())
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_REFLECTION"));

	if(RefractionEnabled && EnvironmentMap.IsAvailable())
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_REFRACTION"));

	if(OpacityMapEnabled && OpacityMap.IsAvailable())
		Options.Definitions.Add(ZEGRShaderDefinition("ZERN_FM_OPACITY_MAP"));

}

bool ZERNFixedMaterial::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_FMDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;

	UpdateShaderDefinitions(Options);

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNFixedMaterial.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNFixedMaterial_GBufferStage_VertexShader";
	GBufferStage_VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(GBufferStage_VertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFixedMaterial_GBufferStage_PixelShader";
	GBufferStage_PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(GBufferStage_PixelShader == NULL, false, "Cannot set pixel shader.");

	DirtyFlags.UnraiseFlags(ZERN_FMDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_FMDF_RENDER_STATE);

	return true;
}

bool ZERNFixedMaterial::UpdateRenderState()
{
	if (!DirtyFlags.GetFlags(ZERN_FMDF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState = ZERNStageGBuffer::GetRenderState();
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);

	if (SkinningEnabled)
		RenderState.SetVertexLayout(*ZESkinnedModelVertex::GetVertexLayout());
	else
		RenderState.SetVertexLayout(*ZEInteriorVertex::GetVertexLayout());

	ZEGRRasterizerState RasterizerState = RenderState.GetRasterizerState();
	RasterizerState.SetCullDirection(TwoSided ? ZEGR_CD_NONE : RasterizerState.GetCullDirection());
	RasterizerState.SetFillMode(Wireframe ? ZEGR_FM_WIREFRAME : ZEGR_FM_SOLID);
	RenderState.SetRasterizerState(RasterizerState);
	
	RenderState.SetShader(ZEGR_ST_VERTEX, GBufferStage_VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, GBufferStage_PixelShader);

	GBufferStage_RenderState = RenderState.Compile();
	zeCheckError(GBufferStage_RenderState == NULL, false, "Cannot set Gbuffer render state.");

	DirtyFlags.UnraiseFlags(ZERN_FMDF_RENDER_STATE);

	return true;
}

bool ZERNFixedMaterial::UpdateConstantBuffer()
{
	if (!DirtyFlags.GetFlags(ZERN_FMDF_CONSTANT_BUFFER))
		return true;

	Constants.AmbientColor = /*(AmbientEnabled ? 1.0f : 0.0f) * */AmbientFactor * AmbientColor;
	Constants.DiffuseColor = (DiffuseEnabled ? 1.0f : 0.0f) * DiffuseFactor * DiffuseColor;
	Constants.SpecularColor = (SpecularEnabled ? 1.0f : 0.0f) * SpecularFactor * SpecularColor;
	Constants.EmissiveColor = (EmissiveEnabled ? 1.0f : 0.0f) * EmissiveFactor * EmissiveColor;
	Constants.ReflectionColor = (ReflectionEnabled ? 1.0f : 0.0f) * ReflectionFactor * ReflectionColor;
	Constants.RefractionColor = (RefractionEnabled ? 1.0f : 0.0f) * RefractionFactor * RefractionColor;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_FMDF_CONSTANT_BUFFER);

	return true;
}

bool ZERNFixedMaterial::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderState())
		return false;

	if(!UpdateConstantBuffer())
		return false;

	return true;
}

bool ZERNFixedMaterial::InitializeSelf()
{
	if (!ZERNMaterial::InitializeSelf())
		return false;

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));
	DirtyFlags.RaiseAll();

	return Update();
}

void ZERNFixedMaterial::DeinitializeSelf()
{
	GBufferStage_VertexShader.Release();
	GBufferStage_PixelShader.Release();
	GBufferStage_RenderState.Release();
	ConstantBuffer.Release();

	DirtyFlags.RaiseAll();

	ZERNMaterial::DeinitializeSelf();
}

ZERNFixedMaterial::ZERNFixedMaterial()
{
	TwoSided = false;
	Wireframe = false;
	AlphaCullEnabled = false;
	SkinningEnabled = false;
	VertexColorEnabled = false;
	TransparencyEnabled = false;
	TransparencyMode = ZERN_TM_NONE;
	AmbientEnabled = true;
	AmbientFactor = 1.0f;
	AmbientColor = ZEVector3::One;
	DiffuseEnabled = true;
	DiffuseFactor = 1.0f;
	DiffuseColor = ZEVector3::One;
	SpecularEnabled = true;
	SpecularFactor = 1.0f;
	SpecularColor = ZEVector3::One;
	NormalMapEnabled = false;
	HeightMapEnabled = false;
	HeightMapTechnique = ZERN_HMT_NONE;
	EmissiveEnabled = false;
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

	Constants.AmbientColor = ZEVector3::One;
	Constants.Opacity = 1.0f;
	Constants.DiffuseColor = ZEVector3::One;
	Constants.SubSurfaceScatteringFactor = 1.0f;
	Constants.SpecularColor = ZEVector3::One;
	Constants.SpecularPower = 1.0f;
	Constants.HeightMapTechnique = ZERN_HMT_NONE;
	Constants.HeightMapScale = 1.0f;
	Constants.HeightMapOffset = 0.0f;
	Constants.Reserved0 = 0.0f;
	Constants.EmissiveColor = ZEVector3::One;
	Constants.AlphaCullLimit = 1.0f;
	Constants.ReflectionColor = ZEVector3::One;
	Constants.GlobalAmbientEnabled = false;
	Constants.RefractionColor = ZEVector3::One;
	Constants.RefractionIndex = 1.0f;
	Constants.DetailDiffuseMapColor = ZEVector3::One;
	Constants.DetailNormalMapFactor = 1.0f;
	Constants.DetailDiffuseMapTiling = ZEVector2::One;
	Constants.DetailNormalMapTiling = ZEVector2::One;
}

ZEUInt ZERNFixedMaterial::GetStageMask()
{
	return ZERN_STAGE_GBUFFER ;
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

bool ZERNFixedMaterial::GetSkinningEnabled()
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

bool ZERNFixedMaterial::GetVertexColorEnabled()
{
	return VertexColorEnabled;
}

void ZERNFixedMaterial::SetTransparencyEnabled(bool Enabled)
{
	if (TransparencyEnabled == Enabled)
		return;

	TransparencyEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNFixedMaterial::GetTransparencyEnabled()
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

ZERNTransparencyMode ZERNFixedMaterial::GetTransparencyMode()
{
	return TransparencyMode;
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

void ZERNFixedMaterial::SetSubSurfaceScatteringMap(const ZERNMap& Map)
{
	SubSurfaceScatteringMap = Map;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

const ZERNMap& ZERNFixedMaterial::GetSubSurfaceScatteringMap()
{
	return SubSurfaceScatteringMap;
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

void ZERNFixedMaterial::SetGlobalAmbientEnabled(bool Enabled)
{
	if (Constants.GlobalAmbientEnabled == Enabled)
		return;

	Constants.GlobalAmbientEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

bool ZERNFixedMaterial::GetGlobalAmbientEnabled() const
{
	return Constants.GlobalAmbientEnabled;
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

void ZERNFixedMaterial::SetSpecularMap(const ZERNMap& Map)
{
	SpecularMap = Map;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

const ZERNMap& ZERNFixedMaterial::GetSpecularMap() const
{
	return SpecularMap;
}

void ZERNFixedMaterial::SetSpecularPowerMap(const ZERNMap& Map)
{
	SpecularPowerMap = Map;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

const ZERNMap& ZERNFixedMaterial::GetSpecularPowerMap() const
{
	return SpecularPowerMap;
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

void ZERNFixedMaterial::SetEmissiveMap(const ZERNMap& Map)
{
	EmissiveMap = Map;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

const ZERNMap& ZERNFixedMaterial::GetEmissiveMap() const
{
	return EmissiveMap;
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

void ZERNFixedMaterial::SetNormalMap(const ZERNMap& Map)
{
	NormalMap = Map;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

const ZERNMap& ZERNFixedMaterial::GetNormalMap() const
{
	return NormalMap;
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

void ZERNFixedMaterial::SetHeightMap(const ZERNMap& Map)
{
	HeightMap = Map;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

const ZERNMap& ZERNFixedMaterial::GetHeightMap() const
{
	return HeightMap;
}

void ZERNFixedMaterial::SetOpacityMapEnabled(bool Enabled)
{
	if (OpacityMapEnabled == Enabled)
		return;

	OpacityMapEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

bool ZERNFixedMaterial::GetOpacityMapEnabled() const
{
	return OpacityMapEnabled;
}

void ZERNFixedMaterial::SetOpacityMap(const ZERNMap& Map)
{
	OpacityMap = Map;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

const ZERNMap& ZERNFixedMaterial::GetOpacityMap() const
{
	return OpacityMap;
}

void ZERNFixedMaterial::SetEnvironmentMap(const ZERNMap& Map)
{
	EnvironmentMap = Map;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

const ZERNMap& ZERNFixedMaterial::GetEnvironmentMap() const
{
	return EnvironmentMap;
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
	if (Constants.DetailDiffuseMapTiling == Tiling && Constants.DetailNormalMapTiling == Tiling)
		return;

	Constants.DetailDiffuseMapTiling = Tiling;
	Constants.DetailNormalMapTiling = Tiling;
	
	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNFixedMaterial::GetDetailMapTiling() const
{
	return Constants.DetailDiffuseMapTiling;
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
	if (Constants.DetailDiffuseMapColor == Color)
		return;

	Constants.DetailDiffuseMapColor = Color;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector3& ZERNFixedMaterial::GetDetailBaseMapColor()
{
	return Constants.DetailDiffuseMapColor;
}

void ZERNFixedMaterial::SetDetailBaseMapTiling(const ZEVector2& Tiling)
{
	if (Constants.DetailDiffuseMapTiling == Tiling)
		return;

	Constants.DetailDiffuseMapTiling = Tiling;

	DirtyFlags.RaiseFlags(ZERN_FMDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNFixedMaterial::GetDetailBaseMapTiling() const
{
	return Constants.DetailDiffuseMapTiling;
}

void ZERNFixedMaterial::SetDetailBaseMap(const ZERNMap& Map)
{
	DetailBaseMap = Map;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

const ZERNMap& ZERNFixedMaterial::GetDetailBaseMap() const
{
	return DetailBaseMap;
}

void ZERNFixedMaterial::SetDetailNormalMapEnabled(bool Enabled)
{
	if (DetailNormalMapEnabled == Enabled)
		return;

	DetailBaseMapEnabled = Enabled;

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

void ZERNFixedMaterial::SetDetailNormalMap(const ZERNMap& Map)
{
	DetailNormalMap = Map;

	DirtyFlags.RaiseFlags(ZERN_FMDF_SHADERS);
}

const ZERNMap& ZERNFixedMaterial::GetDetailNormalMap() const
{
	return DetailNormalMap;
}

void ZERNFixedMaterial::Tick(float ElapsedTime)
{

}

bool ZERNFixedMaterial::SetupMaterial(ZEGRContext* Context, ZERNStage* Stage)
{
	if (!Update())
		return false;

	Context->SetRenderState(GBufferStage_RenderState);
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_MATERIAL, ConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_MATERIAL, ConstantBuffer);

	bool TextureSampler = false;
	if (BaseMap.IsAvailable())
	{
		Context->SetTexture(ZEGR_ST_PIXEL, 0, BaseMap.GetTexture());
		TextureSampler = true;
	}

	if (NormalMapEnabled && NormalMap.IsAvailable())
	{
		Context->SetTexture(ZEGR_ST_PIXEL, 1, NormalMap.GetTexture());
		TextureSampler = true;
	}

	if (SpecularEnabled && SpecularMap.IsAvailable())
	{
		Context->SetTexture(ZEGR_ST_PIXEL, 2, SpecularMap.GetTexture());
		TextureSampler = true;
	}

	if (EmissiveEnabled && EmissiveMap.IsAvailable())
	{
		Context->SetTexture(ZEGR_ST_PIXEL, 3, EmissiveMap.GetTexture());
		TextureSampler = true;
	}

	if (HeightMapEnabled && HeightMap.IsAvailable())
	{
		Context->SetTexture(ZEGR_ST_PIXEL, 4, HeightMap.GetTexture());
		TextureSampler = true;
	}

	if (OpacityMapEnabled && OpacityMap.IsAvailable())
	{
		Context->SetTexture(ZEGR_ST_PIXEL, 5, OpacityMap.GetTexture());
		TextureSampler = true;
	}

	if ((ReflectionEnabled || RefractionEnabled) && EnvironmentMap.IsAvailable())
	{
		Context->SetTexture(ZEGR_ST_PIXEL, 7, EnvironmentMap.GetTexture());
		Context->SetSampler(ZEGR_ST_PIXEL, 1, EnvironmentMap.GetSamplerState());
	}

	if (GetDetailBaseMapEnabled() && DetailBaseMap.IsAvailable())
	{
		Context->SetTexture(ZEGR_ST_PIXEL, 8, DetailBaseMap.GetTexture());
		Context->SetSampler(ZEGR_ST_PIXEL, 2, DetailBaseMap.GetSamplerState());
	}

	if (GetDetailNormalMapEnabled() && DetailNormalMap.IsAvailable())
	{
		Context->SetTexture(ZEGR_ST_PIXEL, 9, DetailNormalMap.GetTexture());
		Context->SetSampler(ZEGR_ST_PIXEL, 3, DetailNormalMap.GetSamplerState());
	}

	if (TextureSampler)
		Context->SetSampler(ZEGR_ST_PIXEL, 0, BaseMap.GetSamplerState());

	return true;
}

void ZERNFixedMaterial::CleanupMaterial(ZEGRContext* Context, ZERNStage* Stage)
{
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_MATERIAL, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_MATERIAL, NULL);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 1, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 3, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 4, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 6, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 7, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 8, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 9, NULL);
}

ZERNFixedMaterial* ZERNFixedMaterial::CreateInstance()
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
	ConfigurationNode.WriteFloat("Opacity", GetOpacity());
	ConfigurationNode.WriteBool("AlphaCullEnabled", GetAlphaCullEnabled());
	ConfigurationNode.WriteFloat("AlphaCullLimit", GetAlphaCullLimit());

	BaseMap.Write(ConfigurationNode, "BaseMap");

	ConfigurationNode.WriteFloat("SubSurfaceScatteringFactor", GetSubSurfaceScatteringFactor());

	ConfigurationNode.WriteBool("AmbientEnabled", GetAmbientEnabled());
	ConfigurationNode.WriteFloat("AmbientFactor", GetAmbientFactor());
	ConfigurationNode.WriteBool("GlobalAmbientEnabled", GetGlobalAmbientEnabled());
	ConfigurationNode.WriteVector3("AmbientColor", GetAmbientColor());

	ConfigurationNode.WriteBool("DiffuseEnabled", GetDiffuseEnabled());
	ConfigurationNode.WriteFloat("DiffuseFactor", GetDiffuseFactor());
	ConfigurationNode.WriteVector3("DiffuseColor", GetDiffuseColor());
	SpecularMap.Write(ConfigurationNode, "SpecularMap");

	ConfigurationNode.WriteFloat("SpecularFactor", GetSpecularFactor());
	ConfigurationNode.WriteVector3("SpecularColor", GetSpecularColor());
	ConfigurationNode.WriteBool("SpecularEnabled", GetSpecularEnabled());

	ConfigurationNode.WriteBool("EmissiveEnabled", GetEmissiveEnabled());
	ConfigurationNode.WriteFloat("EmissiveFactor", GetEmissiveFactor());
	ConfigurationNode.WriteVector3("EmissiveColor", GetEmissiveColor());
	EmissiveMap.Write(ConfigurationNode, "EmissiveMap");

	ConfigurationNode.WriteBool("NormalMapEnabled", GetNormalMapEnabled());
	NormalMap.Write(ConfigurationNode, "NormalMap");

	ConfigurationNode.WriteBool("HeightMapEnabled", GetHeightMapEnabled());
	ConfigurationNode.WriteUInt8("HeightMapTechnique", GetHeightMapEnabled());
	ConfigurationNode.WriteFloat("HeightMapOffset", GetHeightMapOffset());
	ConfigurationNode.WriteFloat("HeightMapScale", GetHeightMapScale());
	HeightMap.Write(ConfigurationNode, "HeightMap");

	ConfigurationNode.WriteBool("OpacityMapEnabled", GetOpacityMapEnabled());
	OpacityMap.Write(ConfigurationNode, "OpacityMap");


	OpacityMap.Write(ConfigurationNode, "EnvironmentMap");
	
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
	DetailBaseMap.Write(ConfigurationNode, "DetailBaseMap");

	ConfigurationNode.WriteBool("DetailNormalMapEnabled", GetDetailNormalMapEnabled());
	ConfigurationNode.WriteFloat("DetailNormalMapColor", GetDetailNormalMapFactor());
	ConfigurationNode.WriteVector2("DetailNormalMapTiling", GetDetailNormalMapTiling());
	DetailNormalMap.Write(ConfigurationNode, "DetailNormalMap");
	
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

	if (MaterialNode.GetName() == "Material")
		zeWarning("Old depricated ZEMaterial file version detected. Please convert this file to new version for future compability. Current Version: 1.0. Detected Version: 0.0. File Name: \"%s\".", File.GetPath().ToCString());

	if(MaterialNode.ReadUInt8("MajorVersion", 0) == 1)
		ReadFromFileV1(MaterialNode);
	else
		ReadFromFileV0(MaterialNode);

	MaterialReader.Close();

	if(File.IsOpen())
		File.Close();

	FileName = FilePath;

	InitializeSelf();
}

void ZERNFixedMaterial::ReadFromFileV0(const ZEMLReaderNode& MaterialNode)
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

		SetTransparencyEnabled(ConfigurationNode.ReadBoolean("TransparencyEnabled", false));
		SetTransparencyMode((ZERNTransparencyMode)ConfigurationNode.ReadInt32("TransparencyMode", ZERN_TM_NONE));
		SetOpacity(ConfigurationNode.ReadFloat("Opacity", 1.0f));
		SetAlphaCullEnabled(ConfigurationNode.ReadBoolean("AlphaCullEnabled", false));
		SetAlphaCullLimit(ConfigurationNode.ReadFloat("AlphaCullLimit", 0.5f));

		BaseMap.ReadV0(ConfigurationNode, "BaseMap");

		SetAmbientEnabled(ConfigurationNode.ReadBoolean("AmbientEnabled", true));
		SetGlobalAmbientEnabled(ConfigurationNode.ReadBoolean("GlobalAmbientEnabled", false));
		SetAmbientFactor(ConfigurationNode.ReadFloat("AmbientFactor", 1.0f));
		SetAmbientColor(ConfigurationNode.ReadVector3("AmbientColor", ZEVector3::One));

		SetDiffuseEnabled(ConfigurationNode.ReadBoolean("DiffuseEnabled", true));
		SetDiffuseFactor(ConfigurationNode.ReadFloat("DiffuseFactor", 1.0f));
		SetDiffuseColor(ConfigurationNode.ReadVector3("DiffuseColor", ZEVector3::One));
		
		SetSpecularEnabled(ConfigurationNode.ReadBoolean("SpecularEnabled", true));
		SetSpecularFactor(ConfigurationNode.ReadFloat("SpecularFactor", 1.0f));
		SetSpecularShininess(ConfigurationNode.ReadFloat("SpecularShininess", 1.0f));
		SetSpecularColor(ConfigurationNode.ReadVector3("SpecularColor", ZEVector3::One));
		SpecularMap.ReadV0(ConfigurationNode, "SpecularMap");

		SetEmissiveEnabled(ConfigurationNode.ReadBoolean("EmissiveEnabled", false));
		SetEmissiveFactor(ConfigurationNode.ReadFloat("EmissiveFactor", 1.0f));
		SetEmissiveColor(ConfigurationNode.ReadVector3("EmissiveColor", ZEVector3::One));
		EmissiveMap.ReadV0(ConfigurationNode, "EmissiveMap");

		SetNormalMapEnabled(ConfigurationNode.ReadBoolean("NormalMapEnabled", false));
		NormalMap.ReadV0(ConfigurationNode, "NormalMap");

		SetHeightMapEnabled(ConfigurationNode.ReadBoolean("HeightEnabled", false));
		SetHeightMapTechnique((ZERNHeightMapTechnique)ConfigurationNode.ReadUInt8("HeigthMapTechnique", ZERN_HMT_NONE));
		SetHeightMapOffset(ConfigurationNode.ReadFloat("HeightMapOffset"));
		SetHeightMapScale(ConfigurationNode.ReadFloat("HeightMapScale"));

		SetOpacityMapEnabled(ConfigurationNode.ReadBoolean("OpacityMapEnabled", false));
		OpacityMap.ReadV0(ConfigurationNode, "OpacityMap");

		EnvironmentMap.ReadV0(ConfigurationNode, "EnvironmentMap");

		SetReflectionEnabled(ConfigurationNode.ReadBoolean("ReflectionEnabled", false));
		SetReflectionFactor(ConfigurationNode.ReadFloat("ReflectionFactor", 1.0f));
		SetReflectionColor(ConfigurationNode.ReadVector3("ReflectionColor", ZEVector3::One));

		SetRefractionEnabled(ConfigurationNode.ReadBoolean("RefractionEnabled", false));
		SetRefractionFactor(ConfigurationNode.ReadFloat("RefractionFactor", 1.0f));
		SetRefractionColor(ConfigurationNode.ReadVector3("RefractionColor", ZEVector3::One));
		SetRefractionIndex(ConfigurationNode.ReadFloat("RefractionIndex", 1.0f));


		SetDetailBaseMapEnabled(ConfigurationNode.ReadBoolean("DetailBaseMapEnabled", false));
		SetDetailBaseMapColor(ConfigurationNode.ReadVector3("DetailBaseMapColor", ZEVector3::One));
		SetDetailMapTiling(ConfigurationNode.ReadVector2("DetailBaseMapTiling", ZEVector2::One));
		DetailBaseMap.ReadV0(ConfigurationNode, "DetailBaseMap");

		SetDetailNormalMapEnabled(ConfigurationNode.ReadBoolean("DetailNormalMapEnabled", false));
		SetDetailNormalMapFactor(ConfigurationNode.ReadFloat("DetailNormalMapFactor", 1.0f));
		SetDetailNormalMapTiling(ConfigurationNode.ReadVector2("DetailNormalMapTiling", ZEVector2::One));
		DetailNormalMap.ReadV0(ConfigurationNode, "DetailNormalMap");
	}
}

void ZERNFixedMaterial::ReadFromFileV1(const ZEMLReaderNode& MaterialNode)
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

		SetTransparencyEnabled(ConfigurationNode.ReadBoolean("TransparencyEnabled", false));
		SetTransparencyMode((ZERNTransparencyMode)ConfigurationNode.ReadInt32("TransparencyMode", ZERN_TM_NONE));
		SetOpacity(ConfigurationNode.ReadFloat("Opacity", 1.0f));
		SetAlphaCullEnabled(ConfigurationNode.ReadBoolean("AlphaCullEnabled", false));
		SetAlphaCullLimit(ConfigurationNode.ReadFloat("AlphaCullLimit", 0.5f));

		BaseMap.ReadV1(ConfigurationNode, "BaseMap");

		SetAmbientEnabled(ConfigurationNode.ReadBoolean("AmbientEnabled", true));
		SetGlobalAmbientEnabled(ConfigurationNode.ReadBoolean("GlobalAmbientEnabled", false));
		SetAmbientFactor(ConfigurationNode.ReadFloat("AmbientFactor", 1.0f));
		SetAmbientColor(ConfigurationNode.ReadVector3("AmbientColor", ZEVector3::One));

		SetDiffuseEnabled(ConfigurationNode.ReadBoolean("DiffuseEnabled", true));
		SetDiffuseFactor(ConfigurationNode.ReadFloat("DiffuseFactor", 1.0f));
		SetDiffuseColor(ConfigurationNode.ReadVector3("DiffuseColor", ZEVector3::One));

		SetSpecularEnabled(ConfigurationNode.ReadBoolean("SpecularEnabled", true));
		SetSpecularFactor(ConfigurationNode.ReadFloat("SpecularFactor", 1.0f));
		SetSpecularShininess(ConfigurationNode.ReadFloat("SpecularShininess", 1.0f));
		SetSpecularColor(ConfigurationNode.ReadVector3("SpecularColor", ZEVector3::One));
		SpecularMap.ReadV1(ConfigurationNode, "SpecularMap");

		SetEmissiveEnabled(ConfigurationNode.ReadBoolean("EmissiveEnabled", false));
		SetEmissiveFactor(ConfigurationNode.ReadFloat("EmissiveFactor", 1.0f));
		SetEmissiveColor(ConfigurationNode.ReadVector3("EmissiveColor", ZEVector3::One));
		EmissiveMap.ReadV1(ConfigurationNode, "EmissiveMap");

		SetNormalMapEnabled(ConfigurationNode.ReadBoolean("NormalMapEnabled", false));
		NormalMap.ReadV1(ConfigurationNode, "NormalMap");

		SetHeightMapEnabled(ConfigurationNode.ReadBoolean("HeightEnabled", false));
		SetHeightMapTechnique((ZERNHeightMapTechnique)ConfigurationNode.ReadUInt8("HeigthMapTechnique", ZERN_HMT_NONE));
		SetHeightMapOffset(ConfigurationNode.ReadFloat("HeightMapOffset"));
		SetHeightMapScale(ConfigurationNode.ReadFloat("HeightMapScale"));

		SetOpacityMapEnabled(ConfigurationNode.ReadBoolean("OpacityMapEnabled", false));
		OpacityMap.ReadV1(ConfigurationNode, "OpacityMap");

		EnvironmentMap.ReadV1(ConfigurationNode, "EnvironmentMap");

		SetReflectionEnabled(ConfigurationNode.ReadBoolean("ReflectionEnabled", false));
		SetReflectionFactor(ConfigurationNode.ReadFloat("ReflectionFactor", 1.0f));
		SetReflectionColor(ConfigurationNode.ReadVector3("ReflectionColor", ZEVector3::One));

		SetRefractionEnabled(ConfigurationNode.ReadBoolean("RefractionEnabled", false));
		SetRefractionFactor(ConfigurationNode.ReadFloat("RefractionFactor", 1.0f));
		SetRefractionColor(ConfigurationNode.ReadVector3("RefractionColor", ZEVector3::One));
		SetRefractionIndex(ConfigurationNode.ReadFloat("RefractionIndex", 1.0f));


		SetDetailBaseMapEnabled(ConfigurationNode.ReadBoolean("DetailBaseMapEnabled", false));
		SetDetailBaseMapColor(ConfigurationNode.ReadVector3("DetailBaseMapColor", ZEVector3::One));
		SetDetailMapTiling(ConfigurationNode.ReadVector2("DetailBaseMapTiling", ZEVector2::One));
		DetailBaseMap.ReadV1(ConfigurationNode, "DetailBaseMap");

		SetDetailNormalMapEnabled(ConfigurationNode.ReadBoolean("DetailNormalMapEnabled", false));
		SetDetailNormalMapFactor(ConfigurationNode.ReadFloat("DetailNormalMapFactor", 1.0f));
		SetDetailNormalMapTiling(ConfigurationNode.ReadVector2("DetailNormalMapTiling", ZEVector2::One));
		DetailNormalMap.ReadV1(ConfigurationNode, "DetailNormalMap");
	}
}
