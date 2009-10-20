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

#include <Memory.h>
#include "FixedMaterial.h"
#include "Definitions.h"
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

bool ZEFixedMaterial::GetTwoSided()
{
	return TwoSided;
}

void ZEFixedMaterial::SetLightningEnabled(bool Enable)
{
	LightningEnabled = Enable;
}

bool ZEFixedMaterial::GetLightningEnabled()
{
	return LightningEnabled;
}

void ZEFixedMaterial::SetWireframe(bool Enable)
{
	Wireframe = Enable;
}

bool ZEFixedMaterial::GetWireframe()
{
	return Wireframe;
}

void ZEFixedMaterial::SetTransparancyMode(ZEMaterialTransparancyMode Mode)
{
	TransparancyMode = Mode;

}

ZEMaterialTransparancyMode ZEFixedMaterial::GetTransparancyMode()
{
	return TransparancyMode;
}

void ZEFixedMaterial::SetTransparancyCullLimit(unsigned int Limit)
{
	Limit = TransparancyCullLimit;
}

unsigned int ZEFixedMaterial::GetTransparancyCullLimit()
{
	return TransparancyCullLimit;
}

void ZEFixedMaterial::SetRecivesShadow(bool Enable)
{
	RecivesShadow = Enable;
}

bool ZEFixedMaterial::GetRecivesShadow()
{
	return RecivesShadow;
}

void ZEFixedMaterial::SetAmbientEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_AMBIENT;
	else
		MaterialComponents &= !ZESHADER_AMBIENT;

}

bool ZEFixedMaterial::GetAmbientEnabled()
{
	return MaterialComponents & ZESHADER_AMBIENT;
}

void ZEFixedMaterial::SetAmbientColor(const ZEVector3& Color)
{
	AmbientColor = Color;
	AmbientColorSC = AmbientColor * AmbientFactor;
}

const ZEVector3& ZEFixedMaterial::GetAmbientColor()
{
	return AmbientColor;
}

void ZEFixedMaterial::SetAmbientFactor(float Factor)
{
	AmbientFactor = Factor;
	AmbientColorSC = AmbientColor * AmbientFactor;
}

float ZEFixedMaterial::GetAmbientFactor()
{
	return AmbientFactor;
}

void ZEFixedMaterial::SetDiffuseEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_DIFFUSE;
	else
		MaterialComponents &= !ZESHADER_DIFFUSE;
}

bool ZEFixedMaterial::GetDiffuseEnabled()
{
	return MaterialComponents &= ZESHADER_DIFFUSE;
}

void ZEFixedMaterial::SetDiffuseColor(const ZEVector3& Color)
{
	DiffuseColor = Color;
	ZEVector3::Scale(DiffuseColorSC, DiffuseColor, DiffuseFactor);
}

const ZEVector3& ZEFixedMaterial::GetDiffuseColor()
{
	return DiffuseColor;
}

void ZEFixedMaterial::SetDiffuseFactor(float Factor)
{
	DiffuseFactor = Factor;
	ZEVector3::Scale(DiffuseColorSC, DiffuseColor, DiffuseFactor);
}

float ZEFixedMaterial::GetDiffuseFactor()
{
	return DiffuseFactor;
}

void ZEFixedMaterial::SetDiffuseMap(const ZETexture2D* Texture)
{
	if (Texture == NULL)
		MaterialComponents &= !ZESHADER_DIFFUSEMAP;
	else
		MaterialComponents |= ZESHADER_DIFFUSEMAP;

	DiffuseMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetDiffuseMap()
{
	return DiffuseMap;
}

void ZEFixedMaterial::SetDiffuseMapAddressModeU(ZETextureAddressMode Mode)
{
	DiffuseMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDiffuseMapAddressModeU()
{
	return DiffuseMapAddressModeU;
}

void ZEFixedMaterial::SetDiffuseMapAddressModeV(ZETextureAddressMode Mode)
{
	DiffuseMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDiffuseMapAddressModeV()
{
	return DiffuseMapAddressModeV;
}

void ZEFixedMaterial::SetSpecularEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_SPECULAR;
	else
		MaterialComponents &= !ZESHADER_SPECULAR;
}

bool ZEFixedMaterial::GetSpecularEnabled()
{
	return MaterialComponents & ZESHADER_SPECULAR;
}

void ZEFixedMaterial::SetSpecularColor(const ZEVector3& Color)
{
	SpecularColor = Color;
}

const ZEVector3& ZEFixedMaterial::GetSpecularColor()
{
	return SpecularColor;
}

void ZEFixedMaterial::SetSpecularFactor(float Factor)
{
	SpecularFactor = Factor;
	ZEVector3::Scale(SpecularColorSC, SpecularColor, SpecularFactor);
}

float ZEFixedMaterial::GetSpecularFactor()
{
	return SpecularFactor;
}

void ZEFixedMaterial::SetSpecularShininess(float Shininess)
{
	SpecularShininess = Shininess;
}

float ZEFixedMaterial::GetSpecularShininess()
{
	return SpecularShininess;
}

void ZEFixedMaterial::SetSpecularMap(const ZETexture2D* Texture)
{
	if (Texture == NULL)
		MaterialComponents &= !ZESHADER_SPECULARMAP;
	else
		MaterialComponents |= ZESHADER_SPECULARMAP;

	SpecularMap = Texture;

}

const ZETexture2D* ZEFixedMaterial::GetSpecularMap()
{
	return SpecularMap;
}

void ZEFixedMaterial::SetSpecularMapAddressModeU(ZETextureAddressMode Mode)
{
	SpecularMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetSpecularMapAddressModeU()
{
	return SpecularMapAddressModeU;
}

void ZEFixedMaterial::SetSpecularMapAddressModeV(ZETextureAddressMode Mode)
{
	SpecularMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetSpecularMapAddressModeV()
{
	return SpecularMapAddressModeV;
}

void ZEFixedMaterial::SetEmmisiveEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_EMMISIVE;
	else
		MaterialComponents &= !ZESHADER_EMMISIVE;


}

bool ZEFixedMaterial::GetEmmisiveEnabled()
{
	return MaterialComponents & ZESHADER_EMMISIVE;
}

void ZEFixedMaterial::SetEmmisiveFactor(float Factor)
{
	EmmisiveFactor = Factor;
	ZEVector3::Scale(EmmisiveColorSC, EmmisiveColor, EmmisiveFactor);
}

float ZEFixedMaterial::GetEmmisiveFactor()
{
	return EmmisiveFactor;
}

void ZEFixedMaterial::SetEmmisiveColor(const ZEVector3& Color)
{
	EmmisiveColor = Color;
	ZEVector3::Scale(EmmisiveColorSC, EmmisiveColor, EmmisiveFactor);
}

const ZEVector3& ZEFixedMaterial::GetEmmisiveColor()
{
	return EmmisiveColor;
}

void ZEFixedMaterial::SetEmmisiveMap(const ZETexture2D* Texture)
{
	EmmisiveMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetEmmisiveMap()
{
	return EmmisiveMap;
}

void ZEFixedMaterial::SetEmmisiveMapAddressModeU(ZETextureAddressMode Mode)
{
	EmmisiveMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetEmmisiveMapAddressModeU()
{
	return EmmisiveMapAddressModeU;
}

void ZEFixedMaterial::SetEmmisiveMapAddressModeV(ZETextureAddressMode Mode)
{
	EmmisiveMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetEmmisiveMapAddressModeV()
{
	return EmmisiveMapAddressModeV;
}

void ZEFixedMaterial::SetNormalMap(const ZETexture2D* Texture)
{
	if (Texture == NULL)
		MaterialComponents &= !ZESHADER_NORMALMAP;
	else
		MaterialComponents |= ZESHADER_NORMALMAP;

	NormalMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetNormalMap()
{
	return NormalMap;
}

void ZEFixedMaterial::SetNormalMapAddressModeU(ZETextureAddressMode Mode)
{
	NormalMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetNormalMapAddressModeU()
{
	return NormalMapAddressModeU;
}

void ZEFixedMaterial::SetNormalMapAddressModeV(ZETextureAddressMode Mode)
{
	NormalMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetNormalMapAddressModeV()
{
	return NormalMapAddressModeV;
}

void ZEFixedMaterial::SetParallaxMap(const ZETexture2D* Texture)
{
	if (Texture == NULL)
		MaterialComponents &= !ZESHADER_PARALLAXMAP;
	else
		MaterialComponents |= ZESHADER_PARALLAXMAP;

	ParallaxMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetParallaxMap()
{
	return ParallaxMap;
}

void ZEFixedMaterial::SetParallaxMapAddressModeU(ZETextureAddressMode Mode)
{
	ParallaxMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetParallaxMapAddressModeU()
{
	return ParallaxMapAddressModeU;
}

void ZEFixedMaterial::SetParallaxMapAddressModeV(ZETextureAddressMode Mode)
{
	ParallaxMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetParallaxMapAddressModeV()
{
	return ParallaxMapAddressModeV;
}

void ZEFixedMaterial::SetOpacityEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_OPACITY;
	else
		MaterialComponents &= !ZESHADER_OPACITY;
}

bool ZEFixedMaterial::GetOpacityEnabled()
{
	return MaterialComponents & ZESHADER_OPACITY;
}

void ZEFixedMaterial::SetOpacity(float Value)
{
	Opacity = Value;
}

float ZEFixedMaterial::GetOpacity()
{
	return Opacity;
}

void ZEFixedMaterial::SetOpacityComponent(ZEMaterialOpacityComponent Component)
{
	#pragma message("Task: Implament Fixed Material Ocapacity Component")
}

void ZEFixedMaterial::SetOpacityMap(const ZETexture2D* Texture)
{
	OpacityMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetOpacityMap()
{
	return OpacityMap;
}

void ZEFixedMaterial::SetOpacityMapAddressModeU(ZETextureAddressMode Mode)
{
	OpacityMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetOpacityMapAddressModeU()
{
	return OpacityMapAddressModeU;
}

void ZEFixedMaterial::SetOpacityMapAddressModeV(ZETextureAddressMode Mode)
{
	OpacityMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetOpacityMapAddressModeV()
{
	return OpacityMapAddressModeV;
}

void ZEFixedMaterial::SetReflectionEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents &= !ZESHADER_REFLECTION;
	else
		MaterialComponents |= ZESHADER_REFLECTION;
}

bool ZEFixedMaterial::GetReflectionEnabled()
{
	return MaterialComponents & ZESHADER_REFLECTION;
}

void ZEFixedMaterial::SetReflectionFactor(float Factor)
{
	ReflectionFactor = Factor;
}

float ZEFixedMaterial::GetReflectionFactor()
{
	return ReflectionFactor;
}

void ZEFixedMaterial::SetReflectionMap(const ZETextureCube* Texture)
{
	if (Texture == NULL)
		MaterialComponents &= !ZESHADER_REFLECTION;
	else
		MaterialComponents |= ZESHADER_REFLECTION;

	ReflectionMap = Texture;
}

const ZETextureCube* ZEFixedMaterial::GetReflectionMap()
{
	return ReflectionMap;
}

void ZEFixedMaterial::SetReflectionMapAddressModeU(ZETextureAddressMode Mode)
{
	ReflectionMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetReflectionMapAddressModeU()
{
	return ReflectionMapAddressModeU;
}

void ZEFixedMaterial::SetReflectionMapAddressModeV(ZETextureAddressMode Mode)
{
	ReflectionMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetReflectionMapAddressModeV()
{
	return ReflectionMapAddressModeV;
}

void ZEFixedMaterial::SetReflectionMapAddressModeW(ZETextureAddressMode Mode)
{
	ReflectionMapAddressModeW = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetReflectionMapAddressModeW()
{
	return ReflectionMapAddressModeW;
}

void ZEFixedMaterial::SetRefractionEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents &= !ZESHADER_REFRACTION;
	else
		MaterialComponents |= ZESHADER_REFRACTION;
}

bool ZEFixedMaterial::GetRefractionEnabled()
{
	return MaterialComponents & ZESHADER_REFRACTION;
}

void ZEFixedMaterial::SetRefractionIndex(float Index)
{
	RefractionIndex = Index;
}

float ZEFixedMaterial::GetRefractionIndex()
{
	return RefractionIndex;
}

void ZEFixedMaterial::SetRefractionFactor(float Factor)
{
	RefractionFactor = Factor;
}

float ZEFixedMaterial::GetRefractionFactor()
{
	return RefractionFactor;
}

void ZEFixedMaterial::SetRefractionMap(const ZETextureCube* Texture)
{
	if (Texture == NULL)
		MaterialComponents &= !ZESHADER_REFRACTION;
	else
		MaterialComponents |= ZESHADER_REFRACTION;

	RefractionMap = Texture;
}

const ZETextureCube* ZEFixedMaterial::GetRefractionMap()
{
	return RefractionMap;
}

void ZEFixedMaterial::SetRefractionMapAddressModeU(ZETextureAddressMode Mode)
{
	RefractionMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetRefractionMapAddressModeU()
{
	return RefractionMapAddressModeU;
}

void ZEFixedMaterial::SetRefractionMapAddressModeV(ZETextureAddressMode Mode)
{
	RefractionMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetRefractionMapAddressModeV()
{
	return RefractionMapAddressModeV;
}

void ZEFixedMaterial::SetRefractionMapAddressModeW(ZETextureAddressMode Mode)
{
	RefractionMapAddressModeW = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetRefractionMapAddressModeW()
{
	return RefractionMapAddressModeW;
}

void ZEFixedMaterial::SetDetailMapEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_DETAILMAP;
	else
		MaterialComponents &= !ZESHADER_DETAILMAP;
}

bool ZEFixedMaterial::GetDetailMapEnabled()
{
	return MaterialComponents & ZESHADER_DETAILMAP;
}

void ZEFixedMaterial::SetDetailMapUScale(float u)
{
	DetailMapTiling.x = u;
}

float ZEFixedMaterial::GetDetailMapUScale()
{
	return DetailMapTiling.x;
}

void ZEFixedMaterial::SetDetailMapVScale(float v)
{
	DetailMapTiling.y = v;
}

float ZEFixedMaterial::GetDetailMapVScale()
{
	return DetailMapTiling.y;
}

void ZEFixedMaterial::SetDetailDiffuseMap(const ZETexture2D* Texture)
{
	if (Texture == NULL)
		MaterialComponents &= !ZESHADER_DETAILDIFFUSEMAP;
	else
		MaterialComponents |= ZESHADER_DETAILDIFFUSEMAP;

	DetailDiffuseMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetDetailDiffuseMap()
{
	return DetailDiffuseMap;
}

void ZEFixedMaterial::SetDetailDiffuseMapAddressModeU(ZETextureAddressMode Mode)
{
	DetailDiffuseMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDetailDiffuseMapAddressModeU()
{
	return DetailDiffuseMapAddressModeU;
}

void ZEFixedMaterial::SetDetailDiffuseMapAddressModeV(ZETextureAddressMode Mode)
{
	DetailDiffuseMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDetailDiffuseMapAddressModeV()
{
	return DetailDiffuseMapAddressModeV;
}

void ZEFixedMaterial::SetDetailNormalMap(const ZETexture2D* Texture)
{
	if (Texture == NULL)
		MaterialComponents &= !ZESHADER_DETAILNORMALMAP;
	else
		MaterialComponents |= ZESHADER_DETAILNORMALMAP;

	DetailNormalMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetDetailNormalMap()
{
	return DetailNormalMap;
}

void ZEFixedMaterial::SetDetailNormalMapAddressModeU(ZETextureAddressMode Mode)
{
	DetailNormalMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDetailNormalMapAddressModeU()
{
	return DetailNormalMapAddressModeU;
}

void ZEFixedMaterial::SetDetailNormalMapAddressModeV(ZETextureAddressMode Mode)
{
	DetailNormalMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDetailNormalMapAddressModeV()
{
	return DetailNormalMapAddressModeV;
}

void ZEFixedMaterial::SetLightMapEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_LIGHTMAP;
	else
		MaterialComponents &= !ZESHADER_LIGHTMAP;
}

bool ZEFixedMaterial::GetLightMapEnabled()
{
	return MaterialComponents & ZESHADER_LIGHTMAP;
}

void ZEFixedMaterial::SetLightMapFactor(float Factor)
{
	LightMapFactor = Factor;
}

float ZEFixedMaterial::GetLightMapFactor()
{
	return LightMapFactor;
}

void ZEFixedMaterial::SetLightMap(const ZETexture2D* Texture)
{
	LightMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetLightMap()
{
	return LightMap;
}

void ZEFixedMaterial::SetLightMapAddressModeU(ZETextureAddressMode Mode)
{
	LightMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetLightMapAddressModeU()
{
	return LightMapAddressModeU;
}

void ZEFixedMaterial::SetLightMapAddressModeV(ZETextureAddressMode Mode)
{
	LightMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetLightMapAddressModeV()
{
	return LightMapAddressModeV;
}

void ZEFixedMaterial::SetDistortionEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZESHADER_DISTORTIONMAP;
	else
		MaterialComponents &= !ZESHADER_DISTORTIONMAP;
}

bool ZEFixedMaterial::GetDistortionEnabled()
{
	return MaterialComponents & ZESHADER_DISTORTIONMAP;
}

void ZEFixedMaterial::SetDistortionFactor(float Factor)
{
	DistortionFactor = Factor;
}

float ZEFixedMaterial::GetDistortionFactor()
{
	return DistortionFactor;
}

void ZEFixedMaterial::SetDistortionMap(const ZETexture2D* Texture)
{
	DistortionMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetDistortionMap()
{
	return DistortionMap; 
}

void ZEFixedMaterial::SetDistortionMapAddressModeU(ZETextureAddressMode Mode)
{
	DistortionMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDistortionMapAddressModeU()
{
	return DistortionMapAddressModeU;
}

void ZEFixedMaterial::SetDistortionMapAddressModeV(ZETextureAddressMode Mode)
{
	DistortionMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDistortionMapAddressModeV()
{
	return DistortionMapAddressModeV;
}

ZEFixedMaterial* ZEFixedMaterial::CreateInstance()
{
	return zeGraphics->CreateFixedMaterial();
}
