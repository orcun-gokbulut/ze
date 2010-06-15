//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - FixedMaterial.cpp
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

#include "FixedMaterial.h"
#include <Memory.h>

#ifndef NULL
#define NULL 0
#endif

#include "GraphicsModule.h"
#include "MaterialComponents.h"

void ZEFixedMaterial::SetZero()
{
	OldMaterialComponents = 0;
	MaterialComponents = 0;
	TwoSided = false;
	LightningEnabled = true;
	Wireframe = false;
	TransparancyMode = ZE_MTM_NOTRANSPARACY;
	TransparancyCullLimit = 0;
	Opacity = 1.0f;

	DiffuseMapAddressModeU = ZE_TAM_WRAP;
	DiffuseMapAddressModeV = ZE_TAM_WRAP;
	NormalMapAddressModeU = ZE_TAM_WRAP;
	NormalMapAddressModeV = ZE_TAM_WRAP;
	ParallaxMapAddressModeU = ZE_TAM_WRAP;
	ParallaxMapAddressModeV = ZE_TAM_WRAP;
	SpecularMapAddressModeU = ZE_TAM_WRAP;
	SpecularMapAddressModeV = ZE_TAM_WRAP;
	EmmisiveMapAddressModeU = ZE_TAM_WRAP;
	EmmisiveMapAddressModeV = ZE_TAM_WRAP;
	OpacityMapAddressModeU = ZE_TAM_WRAP;
	OpacityMapAddressModeV = ZE_TAM_WRAP;
	DetailDiffuseMapAddressModeU = ZE_TAM_WRAP;
	DetailDiffuseMapAddressModeV = ZE_TAM_WRAP;
	DetailNormalMapAddressModeU = ZE_TAM_WRAP;
	DetailNormalMapAddressModeV = ZE_TAM_WRAP;
	ReflectionMapAddressModeU = ZE_TAM_WRAP;
	ReflectionMapAddressModeV = ZE_TAM_WRAP;
	ReflectionMapAddressModeW = ZE_TAM_WRAP;
	RefractionMapAddressModeU = ZE_TAM_WRAP;
	RefractionMapAddressModeV = ZE_TAM_WRAP;
	RefractionMapAddressModeW = ZE_TAM_WRAP;
	LightMapAddressModeU = ZE_TAM_WRAP;
	LightMapAddressModeV = ZE_TAM_WRAP;
	DistortionMapAddressModeU = ZE_TAM_WRAP;
	DistortionMapAddressModeV = ZE_TAM_WRAP;

	AmbientFactor = 1.0f;
	DiffuseFactor = 1.0f;
	SpecularFactor = 1.0f;
	EmmisiveFactor = 1.0f;
	DistortionFactor = 1.0f;
	AmbientColor = ZEVector3(0.0f, 0.0f, 0.0f);
	DiffuseColor = ZEVector3(0.0f, 0.0f, 0.0f);
	SpecularColor = ZEVector3(0.0f, 0.0f, 0.0f);
	EmmisiveColor = ZEVector3(0.0f, 0.0f, 0.0f);

	memset(Textures, 0, sizeof(Textures));
	memset(PixelShaderConstants, 0, sizeof(PixelShaderConstants));
	memset(VertexShaderConstants, 0, sizeof(VertexShaderConstants));
}

ZEFixedMaterial::ZEFixedMaterial()
{

}

ZEFixedMaterial::~ZEFixedMaterial()
{

}

void ZEFixedMaterial::SetTwoSided(bool Enable)
{
	TwoSided = Enable;
}

bool ZEFixedMaterial::GetTwoSided() const
{
	return TwoSided;
}

void ZEFixedMaterial::SetLightningEnabled(bool Enable)
{
	LightningEnabled = Enable;
}

bool ZEFixedMaterial::GetLightningEnabled() const
{
	return LightningEnabled;
}

void ZEFixedMaterial::SetWireframe(bool Enable)
{
	Wireframe = Enable;
}

bool ZEFixedMaterial::GetWireframe() const
{
	return Wireframe;
}

void ZEFixedMaterial::SetTransparancyMode(ZEMaterialTransparancyMode Mode)
{
	TransparancyMode = Mode;

}

ZEMaterialTransparancyMode ZEFixedMaterial::GetTransparancyMode() const
{
	return TransparancyMode;
}

void ZEFixedMaterial::SetTransparancyCullLimit(unsigned int Limit)
{
	Limit = TransparancyCullLimit;
}

unsigned int ZEFixedMaterial::GetTransparancyCullLimit() const
{
	return TransparancyCullLimit;
}

void ZEFixedMaterial::SetRecivesShadow(bool Enable)
{
	RecivesShadow = Enable;
}

bool ZEFixedMaterial::GetRecivesShadow() const
{
	return RecivesShadow;
}

void ZEFixedMaterial::SetAmbientEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_AMBIENT;
	else
		MaterialComponents &= ~ZESHADER_AMBIENT;

}

bool ZEFixedMaterial::GetAmbientEnabled() const
{
	return MaterialComponents & ZESHADER_AMBIENT;
}

void ZEFixedMaterial::SetAmbientColor(const ZEVector3& Color)
{
	AmbientColor = Color;
	AmbientColorSC = AmbientColor * AmbientFactor;
}

const ZEVector3& ZEFixedMaterial::GetAmbientColor() const
{
	return AmbientColor;
}

void ZEFixedMaterial::SetAmbientFactor(float Factor)
{
	AmbientFactor = Factor;
	AmbientColorSC = AmbientColor * AmbientFactor;
}

float ZEFixedMaterial::GetAmbientFactor() const
{
	return AmbientFactor;
}

void ZEFixedMaterial::SetDiffuseEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_DIFFUSE;
	else
		MaterialComponents &= ~ZESHADER_DIFFUSE;
}

bool ZEFixedMaterial::GetDiffuseEnabled() const
{
	return MaterialComponents & ZESHADER_DIFFUSE;
}

void ZEFixedMaterial::SetDiffuseColor(const ZEVector3& Color)
{
	DiffuseColor = Color;
	ZEVector3::Scale(DiffuseColorSC, DiffuseColor, DiffuseFactor);
}

const ZEVector3& ZEFixedMaterial::GetDiffuseColor() const
{
	return DiffuseColor;
}

void ZEFixedMaterial::SetDiffuseFactor(float Factor)
{
	DiffuseFactor = Factor;
	ZEVector3::Scale(DiffuseColorSC, DiffuseColor, DiffuseFactor);
}

float ZEFixedMaterial::GetDiffuseFactor() const
{
	return DiffuseFactor;
}

void ZEFixedMaterial::SetDiffuseMap(const ZETexture2D* Texture)
{
	if (Texture == NULL)
		MaterialComponents &= ~ZESHADER_DIFFUSEMAP;
	else
		MaterialComponents |= ZESHADER_DIFFUSEMAP;

	DiffuseMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetDiffuseMap() const
{
	return DiffuseMap;
}

void ZEFixedMaterial::SetDiffuseMapAddressModeU(ZETextureAddressMode Mode)
{
	DiffuseMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDiffuseMapAddressModeU() const
{
	return DiffuseMapAddressModeU;
}

void ZEFixedMaterial::SetDiffuseMapAddressModeV(ZETextureAddressMode Mode)
{
	DiffuseMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDiffuseMapAddressModeV() const
{
	return DiffuseMapAddressModeV;
}

void ZEFixedMaterial::SetSpecularEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_SPECULAR;
	else
		MaterialComponents &= ~ZESHADER_SPECULAR;
}

bool ZEFixedMaterial::GetSpecularEnabled() const
{
	return MaterialComponents & ZESHADER_SPECULAR;
}

void ZEFixedMaterial::SetSpecularColor(const ZEVector3& Color)
{
	SpecularColor = Color;
}

const ZEVector3& ZEFixedMaterial::GetSpecularColor() const
{
	return SpecularColor;
}

void ZEFixedMaterial::SetSpecularFactor(float Factor)
{
	SpecularFactor = Factor;
	ZEVector3::Scale(SpecularColorSC, SpecularColor, SpecularFactor);
}

float ZEFixedMaterial::GetSpecularFactor() const
{
	return SpecularFactor;
}

void ZEFixedMaterial::SetSpecularShininess(float Shininess)
{
	SpecularShininess = Shininess;
}

float ZEFixedMaterial::GetSpecularShininess() const
{
	return SpecularShininess;
}

void ZEFixedMaterial::SetSpecularMap(const ZETexture2D* Texture)
{
	if (Texture == NULL)
		MaterialComponents &= ~ZESHADER_SPECULARMAP;
	else
		MaterialComponents |= ZESHADER_SPECULARMAP;

	SpecularMap = Texture;

}

const ZETexture2D* ZEFixedMaterial::GetSpecularMap() const
{
	return SpecularMap;
}

void ZEFixedMaterial::SetSpecularMapAddressModeU(ZETextureAddressMode Mode)
{
	SpecularMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetSpecularMapAddressModeU() const
{
	return SpecularMapAddressModeU;
}

void ZEFixedMaterial::SetSpecularMapAddressModeV(ZETextureAddressMode Mode)
{
	SpecularMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetSpecularMapAddressModeV() const
{
	return SpecularMapAddressModeV;
}

void ZEFixedMaterial::SetEmmisiveEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_EMMISIVE;
	else
		MaterialComponents &= ~ZESHADER_EMMISIVE;


}

bool ZEFixedMaterial::GetEmmisiveEnabled() const
{
	return MaterialComponents & ZESHADER_EMMISIVE;
}

void ZEFixedMaterial::SetEmmisiveFactor(float Factor)
{
	EmmisiveFactor = Factor;
	ZEVector3::Scale(EmmisiveColorSC, EmmisiveColor, EmmisiveFactor);
}

float ZEFixedMaterial::GetEmmisiveFactor() const
{
	return EmmisiveFactor;
}

void ZEFixedMaterial::SetEmmisiveColor(const ZEVector3& Color)
{
	EmmisiveColor = Color;
	ZEVector3::Scale(EmmisiveColorSC, EmmisiveColor, EmmisiveFactor);
}

const ZEVector3& ZEFixedMaterial::GetEmmisiveColor() const
{
	return EmmisiveColor;
}

void ZEFixedMaterial::SetEmmisiveMap(const ZETexture2D* Texture)
{
	EmmisiveMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetEmmisiveMap() const
{
	return EmmisiveMap;
}

void ZEFixedMaterial::SetEmmisiveMapAddressModeU(ZETextureAddressMode Mode)
{
	EmmisiveMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetEmmisiveMapAddressModeU() const
{
	return EmmisiveMapAddressModeU;
}

void ZEFixedMaterial::SetEmmisiveMapAddressModeV(ZETextureAddressMode Mode)
{
	EmmisiveMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetEmmisiveMapAddressModeV() const
{
	return EmmisiveMapAddressModeV;
}

void ZEFixedMaterial::SetNormalMapEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_NORMALMAP;
	else
		MaterialComponents &= ~ZESHADER_NORMALMAP;
}

bool ZEFixedMaterial::GetNormalMapEnabled() const
{
	return MaterialComponents & ZESHADER_NORMALMAP;
}

void ZEFixedMaterial::SetNormalMap(const ZETexture2D* Texture)
{
	NormalMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetNormalMap() const
{
	return NormalMap;
}

void ZEFixedMaterial::SetNormalMapAddressModeU(ZETextureAddressMode Mode)
{
	NormalMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetNormalMapAddressModeU() const
{
	return NormalMapAddressModeU;
}

void ZEFixedMaterial::SetNormalMapAddressModeV(ZETextureAddressMode Mode)
{
	NormalMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetNormalMapAddressModeV() const
{
	return NormalMapAddressModeV;
}

void ZEFixedMaterial::SetParallaxMapEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_PARALLAXMAP;
	else
		MaterialComponents &= ~ZESHADER_PARALLAXMAP;
}

void ZEFixedMaterial::SetParallaxMap(const ZETexture2D* Texture)
{

	ParallaxMap = Texture;
}

bool ZEFixedMaterial::GetParallaxMapEnabled() const
{
	return MaterialComponents & ZESHADER_PARALLAXMAP;
}

const ZETexture2D* ZEFixedMaterial::GetParallaxMap() const
{
	return ParallaxMap;
}

void ZEFixedMaterial::SetParallaxMapAddressModeU(ZETextureAddressMode Mode)
{
	ParallaxMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetParallaxMapAddressModeU() const
{
	return ParallaxMapAddressModeU;
}

void ZEFixedMaterial::SetParallaxMapAddressModeV(ZETextureAddressMode Mode)
{
	ParallaxMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetParallaxMapAddressModeV() const
{
	return ParallaxMapAddressModeV;
}

void ZEFixedMaterial::SetOpacityEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_OPACITY;
	else
		MaterialComponents &= ~ZESHADER_OPACITY;
}

bool ZEFixedMaterial::GetOpacityEnabled() const
{
	return MaterialComponents & ZESHADER_OPACITY;
}

void ZEFixedMaterial::SetOpacity(float Value)
{
	Opacity = Value;
}

float ZEFixedMaterial::GetOpacity() const
{
	return Opacity;
}

void ZEFixedMaterial::SetOpacityComponent(ZEMaterialOpacityComponent Component)
{
	#pragma message("Task: Implament Fixed Material Ocapacity Component")
}

ZEMaterialOpacityComponent  ZEFixedMaterial::GetOpacityComponent() const
{
	#pragma message("Task: Implament Fixed Material Ocapacity Component")
	return ZE_MOC_CONSTANT;
}

void ZEFixedMaterial::SetOpacityMap(const ZETexture2D* Texture)
{
	OpacityMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetOpacityMap() const
{
	return OpacityMap;
}

void ZEFixedMaterial::SetOpacityMapAddressModeU(ZETextureAddressMode Mode)
{
	OpacityMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetOpacityMapAddressModeU() const
{
	return OpacityMapAddressModeU;
}

void ZEFixedMaterial::SetOpacityMapAddressModeV(ZETextureAddressMode Mode)
{
	OpacityMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetOpacityMapAddressModeV() const
{
	return OpacityMapAddressModeV;
}

void ZEFixedMaterial::SetReflectionEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_REFLECTION;
	else
		MaterialComponents &= ~ZESHADER_REFLECTION;
}

bool ZEFixedMaterial::GetReflectionEnabled() const
{
	return MaterialComponents & ZESHADER_REFLECTION;
}

void ZEFixedMaterial::SetReflectionFactor(float Factor)
{
	ReflectionFactor = Factor;
}

float ZEFixedMaterial::GetReflectionFactor() const
{
	return ReflectionFactor;
}

void ZEFixedMaterial::SetReflectionMap(const ZETextureCube* Texture)
{
	if (Texture == NULL)
		MaterialComponents &= ~ZESHADER_REFLECTION;
	else
		MaterialComponents |= ZESHADER_REFLECTION;

	ReflectionMap = Texture;
}

const ZETextureCube* ZEFixedMaterial::GetReflectionMap() const
{
	return ReflectionMap;
}

void ZEFixedMaterial::SetReflectionMapAddressModeU(ZETextureAddressMode Mode)
{
	ReflectionMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetReflectionMapAddressModeU() const
{
	return ReflectionMapAddressModeU;
}

void ZEFixedMaterial::SetReflectionMapAddressModeV(ZETextureAddressMode Mode)
{
	ReflectionMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetReflectionMapAddressModeV() const
{
	return ReflectionMapAddressModeV;
}

void ZEFixedMaterial::SetReflectionMapAddressModeW(ZETextureAddressMode Mode)
{
	ReflectionMapAddressModeW = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetReflectionMapAddressModeW() const
{
	return ReflectionMapAddressModeW;
}

void ZEFixedMaterial::SetRefractionEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_REFRACTION;
	else
		MaterialComponents &= ~ZESHADER_REFRACTION;
}

bool ZEFixedMaterial::GetRefractionEnabled() const
{
	return MaterialComponents & ZESHADER_REFRACTION;
}

void ZEFixedMaterial::SetRefractionIndex(float Index)
{
	RefractionIndex = Index;
}

float ZEFixedMaterial::GetRefractionIndex() const
{
	return RefractionIndex;
}

void ZEFixedMaterial::SetRefractionFactor(float Factor)
{
	RefractionFactor = Factor;
}

float ZEFixedMaterial::GetRefractionFactor() const
{
	return RefractionFactor;
}

void ZEFixedMaterial::SetRefractionMap(const ZETextureCube* Texture)
{
	if (Texture == NULL)
		MaterialComponents &= ~ZESHADER_REFRACTION;
	else
		MaterialComponents |= ZESHADER_REFRACTION;

	RefractionMap = Texture;
}

const ZETextureCube* ZEFixedMaterial::GetRefractionMap() const
{
	return RefractionMap;
}

void ZEFixedMaterial::SetRefractionMapAddressModeU(ZETextureAddressMode Mode)
{
	RefractionMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetRefractionMapAddressModeU() const
{
	return RefractionMapAddressModeU;
}

void ZEFixedMaterial::SetRefractionMapAddressModeV(ZETextureAddressMode Mode)
{
	RefractionMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetRefractionMapAddressModeV() const
{
	return RefractionMapAddressModeV;
}

void ZEFixedMaterial::SetRefractionMapAddressModeW(ZETextureAddressMode Mode)
{
	RefractionMapAddressModeW = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetRefractionMapAddressModeW() const
{
	return RefractionMapAddressModeW;
}

void ZEFixedMaterial::SetDetailMapEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_DETAILMAP;
	else
		MaterialComponents &= ~ZESHADER_DETAILMAP;
}

bool ZEFixedMaterial::GetDetailMapEnabled() const
{
	return MaterialComponents & ZESHADER_DETAILMAP;
}

void ZEFixedMaterial::SetDetailMapTiling(const ZEVector2& Tiling)
{
	DetailMapTiling = Tiling;
}

const ZEVector2& ZEFixedMaterial::GetDetailMapTiling() const
{
	return DetailMapTiling;
}

void ZEFixedMaterial::SetDetailDiffuseMap(const ZETexture2D* Texture)
{
	if (Texture == NULL)
		MaterialComponents &= ~ZESHADER_DETAILDIFFUSEMAP;
	else
		MaterialComponents |= ZESHADER_DETAILDIFFUSEMAP;

	DetailDiffuseMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetDetailDiffuseMap() const
{
	return DetailDiffuseMap;
}

void ZEFixedMaterial::SetDetailDiffuseMapAddressModeU(ZETextureAddressMode Mode)
{
	DetailDiffuseMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDetailDiffuseMapAddressModeU() const
{
	return DetailDiffuseMapAddressModeU;
}

void ZEFixedMaterial::SetDetailDiffuseMapAddressModeV(ZETextureAddressMode Mode)
{
	DetailDiffuseMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDetailDiffuseMapAddressModeV() const
{
	return DetailDiffuseMapAddressModeV;
}

void ZEFixedMaterial::SetDetailNormalMap(const ZETexture2D* Texture)
{
	if (Texture == NULL)
		MaterialComponents &= ~ZESHADER_DETAILNORMALMAP;
	else
		MaterialComponents |= ZESHADER_DETAILNORMALMAP;

	DetailNormalMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetDetailNormalMap() const
{
	return DetailNormalMap;
}

void ZEFixedMaterial::SetDetailNormalMapAddressModeU(ZETextureAddressMode Mode)
{
	DetailNormalMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDetailNormalMapAddressModeU() const
{
	return DetailNormalMapAddressModeU;
}

void ZEFixedMaterial::SetDetailNormalMapAddressModeV(ZETextureAddressMode Mode)
{
	DetailNormalMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDetailNormalMapAddressModeV() const
{
	return DetailNormalMapAddressModeV;
}

void ZEFixedMaterial::SetLightMapEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_LIGHTMAP;
	else
		MaterialComponents &= ~ZESHADER_LIGHTMAP;
}

bool ZEFixedMaterial::GetLightMapEnabled() const
{
	return MaterialComponents & ZESHADER_LIGHTMAP;
}

void ZEFixedMaterial::SetLightMapFactor(float Factor)
{
	LightMapFactor = Factor;
}

float ZEFixedMaterial::GetLightMapFactor() const
{
	return LightMapFactor;
}

void ZEFixedMaterial::SetLightMap(const ZETexture2D* Texture)
{
	LightMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetLightMap() const
{
	return LightMap;
}

void ZEFixedMaterial::SetLightMapAddressModeU(ZETextureAddressMode Mode)
{
	LightMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetLightMapAddressModeU() const
{
	return LightMapAddressModeU;
}

void ZEFixedMaterial::SetLightMapAddressModeV(ZETextureAddressMode Mode)
{
	LightMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetLightMapAddressModeV() const
{
	return LightMapAddressModeV;
}

void ZEFixedMaterial::SetDistortionEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_DISTORTIONMAP;
	else
		MaterialComponents &= ~ZESHADER_DISTORTIONMAP;
}

bool ZEFixedMaterial::GetDistortionEnabled() const
{
	return MaterialComponents & ZESHADER_DISTORTIONMAP;
}

void ZEFixedMaterial::SetDistortionFactor(float Factor)
{
	DistortionFactor = Factor;
}

float ZEFixedMaterial::GetDistortionFactor() const
{
	return DistortionFactor;
}

void ZEFixedMaterial::SetDistortionMap(const ZETexture2D* Texture)
{
	DistortionMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetDistortionMap() const
{
	return DistortionMap; 
}

void ZEFixedMaterial::SetDistortionMapAddressModeU(ZETextureAddressMode Mode)
{
	DistortionMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDistortionMapAddressModeU() const
{
	return DistortionMapAddressModeU;
}

void ZEFixedMaterial::SetDistortionMapAddressModeV(ZETextureAddressMode Mode)
{
	DistortionMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDistortionMapAddressModeV() const
{
	return DistortionMapAddressModeV; 
}

ZEFixedMaterial* ZEFixedMaterial::CreateInstance()
{
	return zeGraphics->CreateFixedMaterial();
}

#include "FixedMaterial.h.zpp"

