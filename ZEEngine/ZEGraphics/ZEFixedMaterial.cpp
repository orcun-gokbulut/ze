//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFixedMaterial.cpp
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

#include "ZEFixedMaterial.h"
#include <Memory.h>

#ifndef NULL
#define NULL 0
#endif

#include "ZEGraphicsModule.h"
#include "ZEMaterialComponents.h"
#include "ZETexture2DResource.h"
#include "ZETextureCubeResource.h"
#include "ZETexture2D.h"
#include "ZETextureCube.h"


void ZEFixedMaterial::SetZero()
{
	memset(Textures, 0, sizeof(Textures));
	memset(PixelShaderConstants, 0, sizeof(PixelShaderConstants));
	memset(VertexShaderConstants, 0, sizeof(VertexShaderConstants));

	OldMaterialComponents = 0;
	MaterialComponents = 0;
	TwoSided = false;
	Wireframe = false;
	TransparancyMode = ZE_MTM_NONE;
	AlphaCullLimit = 1;
	OpacityComponent = ZE_MOC_CONSTANT;
	Opacity = 1.0f;
	BaseMapAddressModeU = ZE_TAM_WRAP;
	BaseMapAddressModeV = ZE_TAM_WRAP;
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
	DetailBaseMapAddressModeU = ZE_TAM_WRAP;
	DetailBaseMapAddressModeV = ZE_TAM_WRAP;
	DetailNormalMapAddressModeU = ZE_TAM_WRAP;
	DetailNormalMapAddressModeV = ZE_TAM_WRAP;
	EnvironmentMapAddressModeU = ZE_TAM_WRAP;
	EnvironmentMapAddressModeV = ZE_TAM_WRAP;
	EnvironmentMapAddressModeW = ZE_TAM_WRAP;
	LightMapAddressModeU = ZE_TAM_WRAP;
	LightMapAddressModeV = ZE_TAM_WRAP;
	DistortionMapAddressModeU = ZE_TAM_WRAP;
	DistortionMapAddressModeV = ZE_TAM_WRAP;

	AmbientFactor = 0.0f;
	DiffuseFactor = 1.0f;
	SpecularFactor = 1.0f;
	EmmisiveFactor = 1.0f;
	DistortionFactor = 1.0f;
	AmbientColor = ZEVector3(0.0f, 0.0f, 0.0f);
	DiffuseColor = ZEVector3(0.0f, 0.0f, 0.0f);
	SpecularColor = ZEVector3(0.0f, 0.0f, 0.0f);
	EmmisiveColor = ZEVector3(0.0f, 0.0f, 0.0f);
	SubSurfaceScatteringFactor = 0.0f;
}

ZEFixedMaterial::ZEFixedMaterial()
{
	BaseMapResource = NULL;
	NormalMapResource = NULL;
	ParallaxMapResource = NULL;
	SpecularMapResource = NULL;
	EmmisiveMapResource = NULL;
	OpacityMapResource = NULL;
	DetailBaseMapResource = NULL;
	DetailNormalMapResource = NULL;
	EnvironmentMapResource = NULL;
	LightMapResource = NULL;
	DistortionMapResource = NULL;
}

ZEFixedMaterial::~ZEFixedMaterial()
{

}

ZEMaterialFlags ZEFixedMaterial::GetMaterialFlags() const
{
	return ZE_MTF_G_BUFFER_PASS | ZE_MTF_PRE_Z_PASS | ZE_MTF_SUPPORTS_SKINNING;
}

void ZEFixedMaterial::SetTwoSided(bool Enable)
{
	TwoSided = Enable;
}

bool ZEFixedMaterial::GetTwoSided() const
{
	return TwoSided;
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

void ZEFixedMaterial::SetAlphaCullEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZE_SHADER_ALPHA_CULL;
	else
		MaterialComponents &= ~ZE_SHADER_ALPHA_CULL;
}

bool ZEFixedMaterial::GetAlphaCullEnabled() const
{
	return (MaterialComponents & ZE_SHADER_ALPHA_CULL) != 0;
}

void ZEFixedMaterial::SetAlphaCullLimit(float Limit)
{
	AlphaCullLimit = Limit;
}

float ZEFixedMaterial::GetAlphaCullLimit() const
{
	return AlphaCullLimit;
}

void ZEFixedMaterial::SetAmbientEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZE_SHADER_AMBIENT;
	else
		MaterialComponents &= ~ZE_SHADER_AMBIENT;

}

bool ZEFixedMaterial::GetAmbientEnabled() const
{
	return (MaterialComponents & ZE_SHADER_AMBIENT) != 0;
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
		MaterialComponents |= ZE_SHADER_DIFFUSE;
	else
		MaterialComponents &= ~ZE_SHADER_DIFFUSE;
}

bool ZEFixedMaterial::GetDiffuseEnabled() const
{
	return (MaterialComponents & ZE_SHADER_DIFFUSE) != 0;
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

void ZEFixedMaterial::SetDiffuseSubSurfaceScatteringFactor(float Factor)
{
	SubSurfaceScatteringFactor = Factor;
}

float ZEFixedMaterial::GetDiffuseSubSurfaceScatteringFactor()
{
	return SubSurfaceScatteringFactor;
}

void ZEFixedMaterial::SetBaseMap(const ZETexture2D* Texture)
{
	if (BaseMapResource != NULL)
	{
		BaseMapResource->Release();
		BaseMapResource = NULL;
	}

	if (Texture == NULL)
		MaterialComponents &= ~ZE_SHADER_BASE_MAP;
	else
		MaterialComponents |= ZE_SHADER_BASE_MAP;

	BaseMap = Texture;
}


const ZETexture2D* ZEFixedMaterial::GetBaseMap() const
{
	return BaseMap;
}

void ZEFixedMaterial::SetBaseMapFile(const char* Filename)
{
	if (BaseMapResource != NULL)
	{
		BaseMapResource->Release();
		BaseMapResource = NULL;
	}

	BaseMapResource = ZETexture2DResource::LoadSharedResource(Filename);

	if (BaseMapResource == NULL)
		MaterialComponents &= ~ZE_SHADER_BASE_MAP;
	else
		MaterialComponents |= ZE_SHADER_BASE_MAP;

	if (BaseMapResource != NULL)
		BaseMap = BaseMapResource->GetTexture();
	else
		BaseMap = NULL;
}

const char* ZEFixedMaterial::GetBaseMapFile() const
{
	if (BaseMapResource != NULL)
		return BaseMapResource->GetFileName();
	else
		return "";
}

void ZEFixedMaterial::SetBaseMapAddressModeU(ZETextureAddressMode Mode)
{
	BaseMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetBaseMapAddressModeU() const
{
	return BaseMapAddressModeU;
}

void ZEFixedMaterial::SetBaseMapAddressModeV(ZETextureAddressMode Mode)
{
	BaseMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetBaseMapAddressModeV() const
{
	return BaseMapAddressModeV;
}

void ZEFixedMaterial::SetSpecularEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZE_SHADER_SPECULAR;
	else
		MaterialComponents &= ~ZE_SHADER_SPECULAR;
}

bool ZEFixedMaterial::GetSpecularEnabled() const
{
	return (MaterialComponents & ZE_SHADER_SPECULAR) != 0;
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
	if (SpecularMapResource != NULL)
	{
		SpecularMapResource->Release();
		SpecularMapResource = NULL;
	}

	SpecularMap = Texture;

}

const ZETexture2D* ZEFixedMaterial::GetSpecularMap() const
{
	return SpecularMap;
}

void ZEFixedMaterial::SetSpecularMapFile(const char* Filename)
{
	if (SpecularMapResource != NULL)
	{
		SpecularMapResource->Release();
		SpecularMapResource = NULL;
	}

	SpecularMapResource = ZETexture2DResource::LoadSharedResource(Filename);

	if (SpecularMapResource != NULL)
		SpecularMap = SpecularMapResource->GetTexture();
	else
		SpecularMap = NULL;
}

const char* ZEFixedMaterial::GetSpecularMapFile() const
{
	if (SpecularMapResource != NULL)
		return SpecularMapResource->GetFileName();
	else
		return "";
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
		MaterialComponents |= ZE_SHADER_EMMISIVE;
	else
		MaterialComponents &= ~ZE_SHADER_EMMISIVE;


}

bool ZEFixedMaterial::GetEmmisiveEnabled() const
{
	return (MaterialComponents & ZE_SHADER_EMMISIVE) != 0;
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
	if (EmmisiveMapResource != NULL)
	{
		EmmisiveMapResource->Release();
		EmmisiveMapResource = NULL;
	}

	EmmisiveMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetEmmisiveMap() const
{
	return EmmisiveMap;
}

void ZEFixedMaterial::SetEmmisiveMapFile(const char* Filename)
{
	if (EmmisiveMapResource != NULL)
	{
		EmmisiveMapResource->Release();
		EmmisiveMapResource = NULL;
	}

	EmmisiveMapResource = ZETexture2DResource::LoadSharedResource(Filename);

	if (EmmisiveMapResource != NULL)
		EmmisiveMap = EmmisiveMapResource->GetTexture();
	else
		EmmisiveMap = NULL;
}

const char* ZEFixedMaterial::GetEmmisiveMapFile() const
{
	if (EmmisiveMapResource != NULL)
		return EmmisiveMapResource->GetFileName();
	else
		return "";
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
		MaterialComponents |= ZE_SHADER_NORMAL_MAP;
	else
		MaterialComponents &= ~ZE_SHADER_NORMAL_MAP;
}

bool ZEFixedMaterial::GetNormalMapEnabled() const
{
	return (MaterialComponents & ZE_SHADER_NORMAL_MAP) != 0;
}

void ZEFixedMaterial::SetNormalMap(const ZETexture2D* Texture)
{
	if (NormalMapResource != NULL)
	{
		NormalMapResource->Release();
		NormalMapResource = NULL;
	}

	NormalMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetNormalMap() const
{
	return NormalMap;
}

void ZEFixedMaterial::SetNormalMapFile(const char* Filename)
{
	if (NormalMapResource != NULL)
	{
		NormalMapResource->Release();
		NormalMapResource = NULL;
	}

	NormalMapResource = ZETexture2DResource::LoadSharedResource(Filename);

	if (NormalMapResource != NULL)
		NormalMap = NormalMapResource->GetTexture();
	else
		NormalMap = NULL;
}

const char* ZEFixedMaterial::GetNormalMapFile() const
{
	if (NormalMapResource != NULL)
		return NormalMapResource->GetFileName();
	else
		return "";
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
		MaterialComponents |= ZE_SHADER_PARALLAX_MAP;
	else
		MaterialComponents &= ~ZE_SHADER_PARALLAX_MAP;
}

bool ZEFixedMaterial::GetParallaxMapEnabled() const
{
	return (MaterialComponents & ZE_SHADER_PARALLAX_MAP) != 0;
}

void ZEFixedMaterial::SetParallaxMap(const ZETexture2D* Texture)
{
	if (ParallaxMapResource != NULL)
	{
		ParallaxMapResource->Release();
		ParallaxMapResource = NULL;
	}

	ParallaxMap = Texture;
}

void ZEFixedMaterial::SetParallaxMapFile(const char* Filename)
{
	if (ParallaxMapResource != NULL)
	{
		ParallaxMapResource->Release();
		ParallaxMapResource = NULL;
	}

	ParallaxMapResource = ZETexture2DResource::LoadSharedResource(Filename);

	if (ParallaxMapResource != NULL)
		ParallaxMap = ParallaxMapResource->GetTexture();
	else
		ParallaxMap = NULL;
}

const char* ZEFixedMaterial::GetParallaxMapFile() const
{
	if (ParallaxMapResource != NULL)
		return ParallaxMapResource->GetFileName();
	else
		return "";
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
		MaterialComponents |= ZE_SHADER_OPACITY;
	else
		MaterialComponents &= ~ZE_SHADER_OPACITY;
}

bool ZEFixedMaterial::GetOpacityEnabled() const
{
	return (MaterialComponents & ZE_SHADER_OPACITY) != 0;
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
	MaterialComponents &= ~(ZE_SHADER_OPACITY_BASE_ALPHA | ZE_SHADER_OPACITY_CONSTANT | ZE_SHADER_OPACITY_MAP);
	switch(Component)
	{
		case ZE_MOC_BASE_MAP_ALPHA:
			OpacityComponent = ZE_MOC_BASE_MAP_ALPHA;
			MaterialComponents |= ZE_SHADER_OPACITY_BASE_ALPHA;
			break;

		default:
		case ZE_MOC_CONSTANT:
			OpacityComponent = ZE_MOC_CONSTANT;
			MaterialComponents |= ZE_SHADER_OPACITY_CONSTANT;
			break;

		case ZE_MOC_OPACITY_MAP:
			OpacityComponent = ZE_MOC_OPACITY_MAP;
			MaterialComponents |= ZE_SHADER_OPACITY_MAP;
			break;
	}
}

ZEMaterialOpacityComponent ZEFixedMaterial::GetOpacityComponent() const
{
	return OpacityComponent;
}

void ZEFixedMaterial::SetOpacityMap(const ZETexture2D* Texture)
{
	if (OpacityMapResource != NULL)
	{
		OpacityMapResource->Release();
		OpacityMapResource = NULL;
	}

	OpacityMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetOpacityMap() const
{
	return OpacityMap;
}

void ZEFixedMaterial::SetOpacityMapFile(const char* Filename)
{
	if (OpacityMapResource != NULL)
	{
		OpacityMapResource->Release();
		OpacityMapResource = NULL;
	}

	OpacityMapResource = ZETexture2DResource::LoadSharedResource(Filename);

	if (OpacityMapResource != NULL)
		OpacityMap = OpacityMapResource->GetTexture();
	else
		OpacityMap = NULL;
}

const char* ZEFixedMaterial::GetOpacityMapFile() const
{
	if (OpacityMapResource != NULL)
		return OpacityMapResource->GetFileName();
	else
		return "";
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


void ZEFixedMaterial::SetEnvironmentMap(const ZETextureCube* Texture)
{
	if (EnvironmentMapResource != NULL)
	{
		EnvironmentMapResource->Release();
		EnvironmentMapResource = NULL;
	}

	EnvironmentMap = Texture;
}

const ZETextureCube* ZEFixedMaterial::GetEnvironmentMap() const
{
	return EnvironmentMap;
}

void ZEFixedMaterial::SetEnvironmentMapFile(const char* Filename)
{
	if (EnvironmentMapResource != NULL)
	{
		EnvironmentMapResource->Release();
		EnvironmentMapResource = NULL;
	}

	EnvironmentMapResource = ZETextureCubeResource::LoadSharedResource(Filename);

	if (EnvironmentMapResource != NULL)
		EnvironmentMap = EnvironmentMapResource->GetTexture();
	else
		EnvironmentMap = NULL;
}

const char* ZEFixedMaterial::GetEnvironmentMapFile() const
{
	if (EnvironmentMapResource != NULL)
		return EnvironmentMapResource->GetFileName();
	else
		return "";
}

void ZEFixedMaterial::SetEnvironmentMapAddressModeU(ZETextureAddressMode Mode)
{
	EnvironmentMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetEnvironmentMapAddressModeU() const
{
	return EnvironmentMapAddressModeU;
}

void ZEFixedMaterial::SetEnvironmentMapAddressModeV(ZETextureAddressMode Mode)
{
	EnvironmentMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetEnvironmentMapAddressModeV() const
{
	return EnvironmentMapAddressModeV;
}

void ZEFixedMaterial::SetEnvironmentMapAddressModeW(ZETextureAddressMode Mode)
{
	EnvironmentMapAddressModeW = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetEnvironmentMapAddressModeW() const
{
	return EnvironmentMapAddressModeW;
}

void ZEFixedMaterial::SetReflectionEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZE_SHADER_REFLECTION;
	else
		MaterialComponents &= ~ZE_SHADER_REFLECTION;
}

bool ZEFixedMaterial::GetReflectionEnabled() const
{
	return (MaterialComponents & ZE_SHADER_REFLECTION) != 0;
}

void ZEFixedMaterial::SetReflectionFactor(float Factor)
{
	ReflectionFactor = Factor;
}

float ZEFixedMaterial::GetReflectionFactor() const
{
	return ReflectionFactor;
}

void ZEFixedMaterial::SetRefractionEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZE_SHADER_REFRACTION;
	else
		MaterialComponents &= ~ZE_SHADER_REFRACTION;
}

bool ZEFixedMaterial::GetRefractionEnabled() const
{
	return (MaterialComponents & ZE_SHADER_REFRACTION) != 0;
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

void ZEFixedMaterial::SetDetailMapTiling(const ZEVector2& Tiling)
{
	DetailMapTiling = Tiling;
}

const ZEVector2& ZEFixedMaterial::GetDetailMapTiling() const
{
	return DetailMapTiling;
}

void ZEFixedMaterial::SetDetailBaseMapEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZE_SHADER_DETAIL_BASE_MAP;
	else
		MaterialComponents &= ~ZE_SHADER_DETAIL_BASE_MAP;
}

bool ZEFixedMaterial::GetDetailBaseMapEnabled() const
{
	return (MaterialComponents & ZE_SHADER_DETAIL_BASE_MAP) != 0;
}

void ZEFixedMaterial::SetDetailBaseMap(const ZETexture2D* Texture)
{
	if (DetailBaseMapResource != NULL)
	{
		DetailBaseMapResource->Release();
		DetailBaseMapResource = NULL;
	}

	DetailBaseMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetDetailBaseMap() const
{
	return DetailBaseMap;
}

void ZEFixedMaterial::SetDetailBaseMapFile(const char* Filename)
{
	if (DetailBaseMapResource != NULL)
	{
		DetailBaseMapResource->Release();
		DetailBaseMapResource = NULL;
	}

	DetailBaseMapResource = ZETexture2DResource::LoadSharedResource(Filename);

	if (DetailBaseMapResource != NULL)
		DetailBaseMap = DetailBaseMapResource->GetTexture();
	else
		DetailBaseMap = NULL;
}

const char* ZEFixedMaterial::GetDetailBaseMapFile() const
{
	if (DetailBaseMapResource != NULL)
		return DetailBaseMapResource->GetFileName();
	else
		return "";
}

void ZEFixedMaterial::SetDetailBaseMapAddressModeU(ZETextureAddressMode Mode)
{
	DetailBaseMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDetailBaseMapAddressModeU() const
{
	return DetailBaseMapAddressModeU;
}

void ZEFixedMaterial::SetDetailBaseMapAddressModeV(ZETextureAddressMode Mode)
{
	DetailBaseMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDetailBaseMapAddressModeV() const
{
	return DetailBaseMapAddressModeV;
}

void ZEFixedMaterial::SetDetailNormalMapEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZE_SHADER_DETAIL_NORMAL_MAP;
	else
		MaterialComponents &= ~ZE_SHADER_DETAIL_NORMAL_MAP;
}

bool ZEFixedMaterial::GetDetailNormalMapEnabled() const
{
	return (MaterialComponents & ZE_SHADER_DETAIL_NORMAL_MAP) != 0;
}

void ZEFixedMaterial::SetDetailNormalMap(const ZETexture2D* Texture)
{
	if (DetailNormalMapResource != NULL)
	{
		DetailNormalMapResource->Release();
		DetailNormalMapResource = NULL;
	}

	DetailNormalMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetDetailNormalMap() const
{
	return DetailNormalMap;
}

void ZEFixedMaterial::SetDetailNormalMapFile(const char* Filename)
{
	if (DetailNormalMapResource != NULL)
	{
		DetailNormalMapResource->Release();
		DetailNormalMapResource = NULL;
	}

	DetailNormalMapResource = ZETexture2DResource::LoadSharedResource(Filename);

	if (DetailNormalMapResource != NULL)
		DetailNormalMap = DetailNormalMapResource->GetTexture();
	else
		DetailNormalMap = NULL;
}

const char* ZEFixedMaterial::GetDetailNormalMapFile() const
{
	if (DetailNormalMapResource != NULL)
		return DetailNormalMapResource->GetFileName();
	else
		return "";
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
		MaterialComponents |= ZE_SHADER_LIGHT_MAP;
	else
		MaterialComponents &= ~ZE_SHADER_LIGHT_MAP;
}

bool ZEFixedMaterial::GetLightMapEnabled() const
{
	return (MaterialComponents & ZE_SHADER_LIGHT_MAP) != 0;
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

void ZEFixedMaterial::SetLightMapFile(const char* Filename)
{
	if (LightMapResource != NULL)
	{
		LightMapResource->Release();
		LightMapResource = NULL;
	}

	LightMapResource = ZETexture2DResource::LoadSharedResource(Filename);

	if (LightMapResource != NULL)
		LightMap = LightMapResource->GetTexture();
	else
		LightMap = NULL;
}

const char* ZEFixedMaterial::GetLightMapFile() const
{
	if (LightMapResource != NULL)
		return LightMapResource->GetFileName();
	else
		return "";
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
		MaterialComponents |= ZE_SHADER_DISTORTION_MAP;
	else
		MaterialComponents &= ~ZE_SHADER_DISTORTION_MAP;
}

bool ZEFixedMaterial::GetDistortionEnabled() const
{
	return (MaterialComponents & ZE_SHADER_DISTORTION_MAP) != 0;
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
	if (DistortionMapResource != NULL)
	{
		DistortionMapResource->Release();
		DistortionMapResource = NULL;
	}

	DistortionMap = Texture;
}

const ZETexture2D* ZEFixedMaterial::GetDistortionMap() const
{
	return DistortionMap; 
}

void ZEFixedMaterial::SetDistortionMapFile(const char* Filename)
{
	if (DistortionMapResource != NULL)
	{
		DistortionMapResource->Release();
		DistortionMapResource = NULL;
	}

	DistortionMapResource = ZETexture2DResource::LoadSharedResource(Filename);

	if (DistortionMapResource != NULL)
		DistortionMap = DistortionMapResource->GetTexture();
	else
		DistortionMap = NULL;
}

const char* ZEFixedMaterial::GetDistortionMapFile() const
{
	if (DistortionMapResource != NULL)
		return DistortionMapResource->GetFileName();
	else
		return "";
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