//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDecalMaterial.cpp
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

#include "ZEDecalMaterial.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEMaterialComponents.h"
#include "ZEGraphics/ZETexture2D.h"

ZEDecalMaterial::ZEDecalMaterial()
{
	OldMaterialComponents = 0;
	MaterialComponents = 0;

	FadeAmount = 0.0f;
	
	ProjectionUp = ZEVector3::UnitY;
	ProjectionPoint = ZEVector3::Zero;
	ProjectionDirection = ZEVector3::UnitZ;

	ProjectionWidth = 1.0f;
	ProjectionHeight = 1.0f;
	ProjectionDepth = 1.0f;

	DiffuseColor = ZEVector3::One;
	AmbientColor = ZEVector3::One;
	SpecularColor = ZEVector3::One;
	EmissiveColor = ZEVector3::One;

	DiffuseFactor = 1.0f;
	AmbientFactor = 1.0f;
	SpecularFactor = 1.0f;
	EmissiveFactor = 1.0f;

	MaterialAmbientColor = ZEVector3(0.4f, 0.4f, 0.4f);
	MaterialDiffuseColor = ZEVector3::One;
	MaterialSpecularColor = ZEVector3(0.3f, 0.3f, 0.3f);
	MaterialEmissiveColor = ZEVector3::One;

	Opacity = 1.0f;
	AlphaCullLimit = 0.95f;
	SpecularShininess = 1.0f;

	OpacityComponent = ZE_MOC_CONSTANT;
	TransparancyMode = ZE_MTM_NONE;

	DiffuseMap = NULL;
	NormalMap = NULL;
	SpecularMap = NULL;
	OpacityMap = NULL;
	EmissiveMap = NULL;

	DiffuseMapAddressing = ZE_TAM_WRAP;
	NormalMapAddressing = ZE_TAM_WRAP;
	SpecularMapAddressing = ZE_TAM_WRAP;
	OpacityMapAddressing = ZE_TAM_WRAP;
	EmissiveMapAddressing = ZE_TAM_WRAP;

}

ZEDecalMaterial::~ZEDecalMaterial()
{
	
}

ZEMaterialFlags ZEDecalMaterial::GetMaterialFlags() const
{
	// @NOTE: Do not do g-buffer pass if normal mapping is not enabled
	return (MaterialComponents & ZE_SHADER_NORMAL_MAP) == ZE_SHADER_NORMAL_MAP ? ZE_MTF_G_BUFFER_PASS : ZE_MTF_NONE;
}

// Transparency
void ZEDecalMaterial::SetTransparancyMode(ZEMaterialTransparancyMode Mode)
{
	TransparancyMode = Mode;
}

ZEMaterialTransparancyMode ZEDecalMaterial::GetTransparancyMode() const
{
	return TransparancyMode;
}

void ZEDecalMaterial::SetAlphaCullEnabled(bool Enabled)
{
	MaterialComponents = Enabled ? MaterialComponents | ZE_SHADER_ALPHA_CULL : MaterialComponents & ~ZE_SHADER_ALPHA_CULL;
}

bool ZEDecalMaterial::GetAlphaCullEnabled() const
{
	return (MaterialComponents & ZE_SHADER_ALPHA_CULL) == ZE_SHADER_ALPHA_CULL;
}

void ZEDecalMaterial::SetAlphaCullLimit(float Value)
{
	AlphaCullLimit = Value;
}

float ZEDecalMaterial::GetAlphaCullLimit() const
{
	return AlphaCullLimit;
}

// Ambient
void ZEDecalMaterial::SetAmbientEnabled(bool Enabled)
{
	MaterialComponents = Enabled ? MaterialComponents | ZE_SHADER_AMBIENT : MaterialComponents & ~ZE_SHADER_AMBIENT;
}

bool ZEDecalMaterial::GetAmbientEnabled() const
{
	return (MaterialComponents & ZE_SHADER_AMBIENT) == ZE_SHADER_AMBIENT;
}

void ZEDecalMaterial::SetAmbientFactor(float Factor)
{
	AmbientFactor = Factor;
	MaterialAmbientColor = AmbientColor * AmbientFactor;
}

float ZEDecalMaterial::GetAmbientFactor() const
{
	return AmbientFactor;
}

void ZEDecalMaterial::SetAmbientColor(const ZEVector3& Color)
{
	AmbientColor = Color;
	MaterialAmbientColor = AmbientColor * AmbientFactor;
}

const ZEVector3& ZEDecalMaterial::GetAmbientColor() const
{
	return AmbientColor;
}

// Diffuse
void ZEDecalMaterial::SetDiffuseEnabled(bool Enabled)
{
	MaterialComponents = Enabled ? MaterialComponents | ZE_SHADER_DIFFUSE : MaterialComponents & ~ZE_SHADER_DIFFUSE;
}

bool ZEDecalMaterial::GetDiffuseEnabled() const
{
	return (MaterialComponents & ZE_SHADER_DIFFUSE) == ZE_SHADER_DIFFUSE;
}

void ZEDecalMaterial::SetDiffuseColor(const ZEVector3& Color)
{
	DiffuseColor = Color;
	MaterialDiffuseColor = DiffuseColor * DiffuseFactor;
}

const ZEVector3& ZEDecalMaterial::GetDiffuseColor() const
{
	return DiffuseColor;
}

void ZEDecalMaterial::SetDiffuseFactor(float Factor)
{
	DiffuseFactor = Factor;
	MaterialDiffuseColor = DiffuseColor * DiffuseFactor;
}

float ZEDecalMaterial::GetDiffuseFactor() const
{
	return DiffuseFactor;
}

void ZEDecalMaterial::SetDiffuseMapEnabled(bool Enabled)
{
	MaterialComponents = Enabled ? MaterialComponents | ZE_SHADER_BASE_MAP : MaterialComponents & ~ZE_SHADER_BASE_MAP;
}

bool ZEDecalMaterial::GetDiffuseMapEnabled() const
{
	return (MaterialComponents & ZE_SHADER_BASE_MAP) == ZE_SHADER_BASE_MAP;
}

void ZEDecalMaterial::SetDiffuseMap(const ZETexture2D* Texture)
{
	zeDebugCheck(Texture == NULL, "Null parameter.");

	DiffuseMap = Texture;
}

const ZETexture2D* ZEDecalMaterial::GetDiffuseMap() const
{
	return DiffuseMap;
}

void ZEDecalMaterial::SetDiffuseMapAddressing(ZETextureAddressMode Mode)
{
	DiffuseMapAddressing = Mode;
}

ZETextureAddressMode ZEDecalMaterial::GetDiffuseMapAddressing() const
{
	return DiffuseMapAddressing;
}

// Specular
void ZEDecalMaterial::SetSpecularEnabled(bool Enabled)
{
	MaterialComponents = Enabled ? MaterialComponents | ZE_SHADER_SPECULAR : MaterialComponents & ~ZE_SHADER_SPECULAR;
}

bool ZEDecalMaterial::GetSpecularEnabled() const
{
	return (MaterialComponents & ZE_SHADER_SPECULAR) == ZE_SHADER_SPECULAR;
}

void ZEDecalMaterial::SetSpecularColor(const ZEVector3& Color)
{
	SpecularColor = Color;
	MaterialSpecularColor = SpecularColor * SpecularFactor;
}

const ZEVector3& ZEDecalMaterial::GetSpecularColor() const
{
	return SpecularColor;
}

void ZEDecalMaterial::SetSpecularFactor(float Factor)
{
	SpecularFactor = Factor;
	MaterialSpecularColor = SpecularColor * SpecularFactor;
}

float ZEDecalMaterial::GetSpecularFactor() const
{
	return SpecularFactor;
}
void ZEDecalMaterial::SetSpecularShininess(float Shininess)
{
	SpecularShininess = Shininess;
}

float ZEDecalMaterial::GetSpecularShininess() const
{
	return SpecularShininess;
}

void ZEDecalMaterial::SetSpecularMapEnabled(bool Enabled)
{
	MaterialComponents = Enabled ? MaterialComponents | ZE_SHADER_SPECULAR_MAP : MaterialComponents & ~ZE_SHADER_SPECULAR_MAP;
}

bool ZEDecalMaterial::GetSpecularMapEnabled() const
{
	return (MaterialComponents & ZE_SHADER_SPECULAR_MAP) == ZE_SHADER_SPECULAR_MAP;
}

void ZEDecalMaterial::SetSpecularMap(const ZETexture2D* Texture)
{
	zeDebugCheck(Texture == NULL, "Null parameter.");

	SpecularMap = Texture;
}

const ZETexture2D* ZEDecalMaterial::GetSpecularMap() const
{
	return SpecularMap;
}

void ZEDecalMaterial::SetSpecularMapAddressing(ZETextureAddressMode Mode)
{
	SpecularMapAddressing = Mode;
}

ZETextureAddressMode ZEDecalMaterial::GetSpecularMapAddressing() const
{
	return SpecularMapAddressing;
}

// Emissive
void ZEDecalMaterial::SetEmissiveEnabled(bool Enabled)
{
	MaterialComponents = Enabled ? MaterialComponents | ZE_SHADER_EMMISIVE : MaterialComponents & ~ZE_SHADER_EMMISIVE;
}

bool ZEDecalMaterial::GetEmissiveEnabled() const
{
	return (MaterialComponents & ZE_SHADER_EMMISIVE) == ZE_SHADER_EMMISIVE;
}

void ZEDecalMaterial::SetEmissiveColor(const ZEVector3& Color)
{
	EmissiveColor = Color;
	MaterialEmissiveColor = EmissiveColor * EmissiveFactor;
}

const ZEVector3& ZEDecalMaterial::GetEmissiveColor() const
{
	return EmissiveColor;
}

void ZEDecalMaterial::SetEmissiveFactor(float Factor)
{
	EmissiveFactor = Factor;
	MaterialEmissiveColor = EmissiveColor * EmissiveFactor;
}

float ZEDecalMaterial::GetEmissiveFactor() const
{
	return EmissiveFactor;
}

void ZEDecalMaterial::SetEmissiveMapEnabled(bool Enabled)
{
	MaterialComponents = Enabled ? MaterialComponents | ZE_SHADER_EMMISIVE_MAP : MaterialComponents & ~ZE_SHADER_EMMISIVE_MAP;
}

bool ZEDecalMaterial::GetEmissiveMapEnabled() const
{
	return (MaterialComponents & ZE_SHADER_EMMISIVE_MAP) == ZE_SHADER_EMMISIVE_MAP;
}

void ZEDecalMaterial::SetEmissiveMap(const ZETexture2D* Texture)
{
	EmissiveMap = Texture;
}

const ZETexture2D* ZEDecalMaterial::GetEmissiveMap() const
{
	return EmissiveMap;
}

void ZEDecalMaterial::SetEmissiveMapAddressing(ZETextureAddressMode Mode)
{
	EmissiveMapAddressing = Mode;
}

ZETextureAddressMode ZEDecalMaterial::GetEmissiveMapAddressing() const
{
	return EmissiveMapAddressing;
}

// Normal Map
void ZEDecalMaterial::SetNormalMapEnabled(bool Enabled)
{
	MaterialComponents = Enabled ? MaterialComponents | ZE_SHADER_NORMAL_MAP : MaterialComponents & ~ZE_SHADER_NORMAL_MAP;
}

bool ZEDecalMaterial::GetNormalMapEnabled() const
{
	return (MaterialComponents & ZE_SHADER_NORMAL_MAP) == ZE_SHADER_NORMAL_MAP;
}

void ZEDecalMaterial::SetNormalMap(const ZETexture2D*	Texture)
{
	zeDebugCheck(Texture == NULL, "Null parameter.");

	NormalMap = Texture;
}

const ZETexture2D* ZEDecalMaterial::GetNormalMap() const
{
	return NormalMap;
}

void ZEDecalMaterial::SetNormalMapAddressing(ZETextureAddressMode Mode)
{
	NormalMapAddressing = Mode;
}

ZETextureAddressMode ZEDecalMaterial::GetNormalMapAddressing() const
{
	return NormalMapAddressing;
}

// Opacity
void ZEDecalMaterial::SetOpacityEnabled(bool Enabled)
{
	MaterialComponents = Enabled ? MaterialComponents | ZE_SHADER_OPACITY : MaterialComponents & ~ZE_SHADER_OPACITY;
}

bool ZEDecalMaterial::GetOpacityEnabled() const
{
	return (MaterialComponents & ZE_SHADER_OPACITY) == ZE_SHADER_OPACITY;
}

void ZEDecalMaterial::SetOpacity(float Value)
{
	Opacity = Value;
}

float ZEDecalMaterial::GetOpacity() const
{
	return Opacity;
}

void ZEDecalMaterial::SetOpacityComponent(ZEMaterialOpacityComponent Component)
{
	// Add new component
	// Remove others
	switch (Component)
	{
		case ZE_MOC_BASE_MAP_ALPHA:
			MaterialComponents |= ZE_SHADER_OPACITY_BASE_ALPHA;
			MaterialComponents &= ~(ZE_SHADER_OPACITY_CONSTANT | ZE_SHADER_OPACITY_MAP);
			break;

		default:
		case ZE_MOC_CONSTANT:
			MaterialComponents |= ZE_SHADER_OPACITY_CONSTANT;
			MaterialComponents &= ~(ZE_SHADER_OPACITY_BASE_ALPHA | ZE_SHADER_OPACITY_MAP);
			break;

		case ZE_MOC_OPACITY_MAP:
			MaterialComponents |= ZE_SHADER_OPACITY_MAP;
			MaterialComponents &= ~(ZE_SHADER_OPACITY_BASE_ALPHA | ZE_SHADER_OPACITY_CONSTANT);
			break;
	}

	OpacityComponent = Component;
}

ZEMaterialOpacityComponent ZEDecalMaterial::GetOpacityComponent() const
{
	return OpacityComponent;
}

void ZEDecalMaterial::SetOpacityMapEnabled(bool Enabled)
{
	MaterialComponents = Enabled ? MaterialComponents | ZE_SHADER_OPACITY_MAP : MaterialComponents & ~ZE_SHADER_OPACITY_MAP;
}

bool ZEDecalMaterial::GetOpacityMapEnabled() const
{
	return (MaterialComponents & ZE_SHADER_OPACITY_MAP) == ZE_SHADER_OPACITY_MAP;
}

void ZEDecalMaterial::SetOpacityMap(const ZETexture2D* Texture)
{
	zeDebugCheck(Texture == NULL, "Null parameter.");

	OpacityMap = Texture;
}

const ZETexture2D* ZEDecalMaterial::GetOpacityMap() const
{
	return OpacityMap;
}

void ZEDecalMaterial::SetOpacityMapAddressing(ZETextureAddressMode Mode)
{
	OpacityMapAddressing = Mode;
}

ZETextureAddressMode ZEDecalMaterial::GetOpacityMapAddressing() const
{
	return OpacityMapAddressing;
}

ZEDecalMaterial* ZEDecalMaterial::CreateInstance()
{
	return new ZEDecalMaterial();
}
