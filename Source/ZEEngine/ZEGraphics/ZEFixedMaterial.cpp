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
#include "ZETexture/ZETexture2DResource.h"
#include "ZETexture/ZETextureCubeResource.h"
#include "ZETexture2D.h"
#include "ZETextureCube.h"
#include "ZEML/ZEMLSerialReader.h"
#include "ZEFile/ZEPathUtils.h"
#include "ZEFile/ZEFileInfo.h"

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
	SpecularFactor = 0.0f;
	EmmisiveFactor = 0.0f;
	DistortionFactor = 1.0f;
	AmbientColor = ZEVector3::One;
	DiffuseColor = ZEVector3::One;
	SpecularColor = ZEVector3::One;
	EmmisiveColor = ZEVector3::One;
	SubSurfaceScatteringFactor = 0.0f;
	MaterialComponentMask = ~0;
	GlobalAmbientEnabled = true;
}

ZEFixedMaterial::~ZEFixedMaterial()
{

}

ZEMaterialFlags ZEFixedMaterial::GetMaterialFlags() const
{
	return (TransparancyMode == ZE_MTM_NONE ? ZE_MTF_G_BUFFER_PASS : 0) | ZE_MTF_PRE_Z_PASS | ZE_MTF_SUPPORTS_SKINNING;
}

void ZEFixedMaterial::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEFixedMaterial::GetName() const
{
	return Name;
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
	if (TransparancyMode == ZE_MTM_REGULAR)
		MaterialComponentMask = ~(ZE_SHADER_DIFFUSE | ZE_SHADER_SPECULAR | ZE_SHADER_DETAIL_NORMAL_MAP);
	else
		MaterialComponentMask = ~0;
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

void ZEFixedMaterial::SetGlobalAmbientEnabled(bool Enabled)
{
	GlobalAmbientEnabled = Enabled;
}

bool ZEFixedMaterial::GetGlobalAmbientEnabled()
{
	return GlobalAmbientEnabled;
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
	else
	{
		MaterialComponents &= ~ZE_SHADER_EMMISIVE_MAP;
	}

	EmmisiveMap = Texture;
	MaterialComponents |= ZE_SHADER_EMMISIVE_MAP;
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
	{
		EmmisiveMap = EmmisiveMapResource->GetTexture();
		MaterialComponents |= ZE_SHADER_EMMISIVE_MAP;
	}
	else
	{
		EmmisiveMap = NULL;
		MaterialComponents &= ~ZE_SHADER_EMMISIVE_MAP;
	}
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

		SetNormalMapEnabled(true);
		NormalMap = Texture;
	}
	else
	{
		SetNormalMapEnabled(false);
		NormalMap = NULL;
	}
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
	SetOpacityEnabled(true);
	SetOpacityComponent(ZE_MOC_OPACITY_MAP);

 	SetAlphaCullEnabled(true);
 	SetAlphaCullLimit(0.5f);
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
	{
		SetOpacityEnabled(true);
		SetOpacityComponent(ZE_MOC_OPACITY_MAP);
		OpacityMap = OpacityMapResource->GetTexture();

		SetAlphaCullEnabled(true);
		SetAlphaCullLimit(0.5f);
	}
	else
	{
		OpacityMap = NULL;
	}
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

void ZEFixedMaterial::SetVertexColorEnabled(bool Enabled)
{
	if (Enabled)
		MaterialComponents |= ZE_SHADER_VERTEX_COLOR;
	else
		MaterialComponents &= ~ZE_SHADER_VERTEX_COLOR;
}

bool ZEFixedMaterial::GetVertexColorEnabled()
{
	return (MaterialComponents & ZE_SHADER_VERTEX_COLOR) != 0;
}

void ZEFixedMaterial::Tick(float ElapsedTime)
{

}

ZEFixedMaterial* ZEFixedMaterial::CreateInstance()
{
	return zeGraphics->CreateFixedMaterial();
}

void ZEFixedMaterial::WriteToFile(const ZEString& FilePath)
{
	ZEFile File;
	if(!File.Open(FilePath, ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
		zeError("Can not open given file. File : %s", FilePath.ToCString());

	ZEMLNode* MaterialNode = new ZEMLNode("Material");

	MaterialNode->AddProperty("Name", GetName());

	ZEMLNode* ConfigurationNode = MaterialNode->AddSubNode("Configuration");

	ConfigurationNode->AddProperty("Name", "Default"); //Will be changed when configuration is implemented.
	ConfigurationNode->AddProperty("Wireframe", GetWireframe());
	ConfigurationNode->AddProperty("TwoSided", GetTwoSided());
	ConfigurationNode->AddProperty("TransparencyMode", (ZEInt32)GetTransparancyMode());
	ConfigurationNode->AddProperty("LightningEnabled", GetLightningEnabled());
	ConfigurationNode->AddProperty("ShadowReceiver", GetShadowReceiver());
	ConfigurationNode->AddProperty("ShadowCaster", GetShadowCaster());

	ConfigurationNode->AddProperty("AmbientEnabled", GetAmbientEnabled());
	ConfigurationNode->AddProperty("DiffuseEnabled", GetDiffuseEnabled());
	ConfigurationNode->AddProperty("NormalMapEnabled", GetNormalMapEnabled());
	ConfigurationNode->AddProperty("ParallaxMapEnabled", GetParallaxMapEnabled());
	ConfigurationNode->AddProperty("SpecularEnabled", GetSpecularEnabled());
	ConfigurationNode->AddProperty("EmmisiveEnabled", GetEmmisiveEnabled());
	ConfigurationNode->AddProperty("OpacityEnabled", GetOpacityEnabled());
	ConfigurationNode->AddProperty("DetailBaseMapEnabled", GetDetailBaseMapEnabled());
	ConfigurationNode->AddProperty("DetailNormalMapEnabled", GetDetailNormalMapEnabled());
	ConfigurationNode->AddProperty("ReflectionEnabled", GetReflectionEnabled());
	ConfigurationNode->AddProperty("RefractionEnabled", GetRefractionEnabled());
	ConfigurationNode->AddProperty("LightMapEnabled", GetLightMapEnabled());
	ConfigurationNode->AddProperty("AlphaCullEnabled", GetAlphaCullEnabled());
	ConfigurationNode->AddProperty("VertexColorEnabled", GetVertexColorEnabled());

	ConfigurationNode->AddProperty("Opacity", GetOpacity());
	ConfigurationNode->AddProperty("OpacityComponent", (ZEInt32)GetOpacityComponent());
	ConfigurationNode->AddProperty("AlphaCullLimit", GetAlphaCullLimit());
	ConfigurationNode->AddProperty("DetailMapTiling", GetDetailMapTiling());
	ConfigurationNode->AddProperty("SpecularShininess", GetSpecularShininess());
	ConfigurationNode->AddProperty("SubSurfaceScatteringFactor", GetDiffuseSubSurfaceScatteringFactor());

	ConfigurationNode->AddProperty("AmbientColor", GetAmbientColor());
	ConfigurationNode->AddProperty("AmbientFactor", GetAmbientFactor());
	ConfigurationNode->AddProperty("SpecularColor", GetSpecularColor());
	ConfigurationNode->AddProperty("SpecularFactor", GetSpecularFactor());
	ConfigurationNode->AddProperty("DiffuseColor", GetDiffuseColor());
	ConfigurationNode->AddProperty("DiffuseFactor", GetDiffuseFactor());
	ConfigurationNode->AddProperty("EmmisiveColor", GetEmmisiveColor());
	ConfigurationNode->AddProperty("EmmisiveFactor", GetEmmisiveFactor());
	ConfigurationNode->AddProperty("RefractionIndex", GetRefractionIndex());
	ConfigurationNode->AddProperty("RefractionFactor", GetRefractionFactor());
	ConfigurationNode->AddProperty("ReflectionFactor", GetReflectionFactor());

	ConfigurationNode->AddProperty("BaseMap", GetBaseMapFile());
	ConfigurationNode->AddProperty("BaseMapAddressModeU", (ZEInt32)GetBaseMapAddressModeU());
	ConfigurationNode->AddProperty("BaseMapAddressModeV", (ZEInt32)GetBaseMapAddressModeV());
	ConfigurationNode->AddProperty("NormalMap", GetNormalMapFile());
	ConfigurationNode->AddProperty("NormalMapAddressModeU", GetNormalMapAddressModeU());
	ConfigurationNode->AddProperty("NormalMapAddressModeV", GetNormalMapAddressModeV());
	ConfigurationNode->AddProperty("ParallaxMap", GetParallaxMapFile());
	ConfigurationNode->AddProperty("ParallaxMapAddressModeU", GetParallaxMapAddressModeU());
	ConfigurationNode->AddProperty("ParallaxMapAddressModeV", GetParallaxMapAddressModeV());
	ConfigurationNode->AddProperty("SpecularMap", GetSpecularMapFile());
	ConfigurationNode->AddProperty("SpecularMapAddressModeU", GetSpecularMapAddressModeU());
	ConfigurationNode->AddProperty("SpecularMapAddressModeV", GetSpecularMapAddressModeV());
	ConfigurationNode->AddProperty("EmmisiveMap", GetEmmisiveMapFile());
	ConfigurationNode->AddProperty("EmmisiveMapAddressModeU", GetEmmisiveMapAddressModeU());
	ConfigurationNode->AddProperty("EmmisiveMapAddressModeV", GetEmmisiveMapAddressModeV());
	ConfigurationNode->AddProperty("OpacityMap", GetOpacityMapFile());
	ConfigurationNode->AddProperty("OpacityMapAddressModeU", GetOpacityMapAddressModeU());
	ConfigurationNode->AddProperty("OpacityMapAddressModeV", GetOpacityMapAddressModeV());
	ConfigurationNode->AddProperty("DetailBaseMap", GetDetailBaseMap());
	ConfigurationNode->AddProperty("DetailBaseMapAddressModeU", GetDetailBaseMapAddressModeU());
	ConfigurationNode->AddProperty("DetailBaseMapAddressModeV", GetDetailBaseMapAddressModeV());
	ConfigurationNode->AddProperty("DetailNormalMap", GetDetailNormalMapFile());
	ConfigurationNode->AddProperty("DetailNormalMapAddressModeU", GetDetailNormalMapAddressModeU());
	ConfigurationNode->AddProperty("DetailNormalMapAddressModeV", GetDetailNormalMapAddressModeV());
	ConfigurationNode->AddProperty("EnvironmentMap", GetEnvironmentMapFile());
	ConfigurationNode->AddProperty("EnvironmentMapAddressModeU", GetEnvironmentMapAddressModeU());
	ConfigurationNode->AddProperty("EnvironmentMapAddressModeV", GetEnvironmentMapAddressModeV());
	ConfigurationNode->AddProperty("EnvironmentMapAddressModeW", GetEnvironmentMapAddressModeW());
	ConfigurationNode->AddProperty("LightMap", GetLightMapFile());
	ConfigurationNode->AddProperty("LightMapAddressModeU", GetLightMapAddressModeU());
	ConfigurationNode->AddProperty("LightMapAddressModeV", GetLightMapAddressModeV());

	MaterialNode->Write(&File);

	if(File.IsOpen())
		File.Close();

	delete MaterialNode;
}

void ZEFixedMaterial::ReadFromFile(const ZEString& FilePath)
{
	ZEFile File;
	if(!File.Open(FilePath, ZE_FOM_READ, ZE_FCM_NONE))
		zeError("Can not open given file. File : %s", FilePath.ToCString());

	ZEMLSerialReader Reader(&File);

	if(!Reader.Read())
		zeError("Can not read material file.");

	ZEVariant NameValue;
	ZEMLSerialPointer Configuration;

	ZEMLSerialListItem Material[] = 
	{
		ZEML_LIST_NODE("Configuration",		Configuration,							true),
		ZEML_LIST_PROPERTY("Name",			NameValue,			ZE_VRT_STRING,		true),
	};

	if (!Reader.ReadPropertyList(Material, 2))
		zeError("Cannot read material configuration");

	SetName(NameValue.GetString());

	Reader.SeekPointer(Configuration);

	// Simple rendering options
	ZEVariant ConfigurationNameValue, WireFrameValue, TwoSidedValue, TransparencyModeValue, LightingEnabledValue, ShadowCasterValue, ShadowReceiverValue;

	// Material properties
	ZEVariant OpacityValue, OpacityComponentValue, AlphaCullLimitValue, DetailMapTilingValue, DetailDistanceFullValue, 
		DetailDistanceFadeValue, SpecularShininessValue, SubSurfaceScatteringFactorValue,
		AmbientColorValue, AmbientFactorValue, SpecularColorValue, SpecularFactorValue, 
		DiffuseColorValue, DiffuseFactorValue, EmissiveColorValue, EmissiveFactorValue,
		RefractionIndexValue, RefractionFactorValue, ReflectionFactorValue;

	// Textures
	ZEVariant BaseMapValue, BaseMapAddressModeUValue, BaseMapAddressModeVValue, NormalMapValue, NormalMapAddressModeUValue, NormalMapAddressModeVValue, 
		ParallaxMapValue, ParallaxMapAddressModeUValue, ParallaxMapAddressModeVValue, SpecularMapValue, SpecularMapAddressModeUValue, SpecularMapAddressModeVValue, 
		EmissiveMapValue, EmissiveMapAddressModeUValue, EmissiveMapAddressModeVValue, OpacityMapValue, OpacityMapAddressModeUValue, OpacityMapAddressModeVValue,
		DetailBaseMapValue, DetailBaseMapAddressModeUValue, DetailBaseMapAddressModeVValue, DetailNormalMapValue, DetailNormalMapAddressModeUValue, DetailNormalMapAddressModeVValue, 
		EnvironmentMapValue, EnvironmentMapAddressModeUValue, EnvironmentMapAddressModeVValue, EnvironmentMapAddressModeWValue, LightMapValue, LightMapAddressModeUValue, LightMapAddressModeVValue;

	// Shader components
	ZEVariant AmbientEnabledValue, DiffuseEnabledValue, BaseMapEnabledValue, NormalMapEnabledValue,
		ParallaxMapEnabledValue, SpecularEnabledValue, SpecularMapEnabledValue, 
		EmissiveEnabledValue, EmissiveMapEnabledValue, OpacityEnabledValue,
		OpacityBaseAlphaEnabledValue, OpacityConstantEnabledValue, OpacityMapEnabledValue, 
		DetailEnabledValue, DetailBaseMapEnabledValue, DetailNormalMapEnabledValue, 
		ReflectionEnabledValue, RefractionEnabledValue, LightMapEnabledValue, 
		AlphaCullEnabledValue, VertexColorEnabledValue;

	ZEMLSerialListItem Properties[] = 
	{
		ZEML_LIST_PROPERTY("Name",							ConfigurationNameValue,				ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("Wireframe",						WireFrameValue,						ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("TwoSided",						TwoSidedValue,						ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("TransparencyMode",				TransparencyModeValue,				ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("LightningEnabled",				LightingEnabledValue,				ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("ShadowReceiver",				ShadowReceiverValue,				ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("ShadowCaster",					ShadowCasterValue,					ZE_VRT_BOOLEAN,		false),

		ZEML_LIST_PROPERTY("AmbientEnabled",				AmbientEnabledValue,				ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("DiffuseEnabled",				DiffuseEnabledValue,				ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("BaseMapEnabled",				BaseMapEnabledValue,				ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("NormalMapEnabled",				NormalMapEnabledValue,				ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("ParallaxMapEnabled",			ParallaxMapEnabledValue,			ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("SpecularEnabled",				SpecularEnabledValue,				ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("SpecularMapEnabled",			SpecularMapEnabledValue,			ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("EmmisiveEnabled",				EmissiveEnabledValue,				ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("EmmisiveMapEnabled",			EmissiveMapEnabledValue,			ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("OpacityEnabled",				OpacityEnabledValue,				ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("OpacityMapEnabled",				OpacityMapEnabledValue,				ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("OpacityConstantEnabled",		OpacityConstantEnabledValue,		ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("OpacityBaseAlphaEnabled",		OpacityBaseAlphaEnabledValue,		ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("DetailBaseMapEnabled",			DetailBaseMapEnabledValue,			ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("DetailNormalMapEnabled",		DetailNormalMapEnabledValue,		ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("ReflectionEnabled",				ReflectionEnabledValue,				ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("RefractionEnabled",				RefractionEnabledValue,				ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("LightMapEnabled",				LightMapEnabledValue,				ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("AlphaCullEnabled",				AlphaCullEnabledValue,				ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("VertexColorEnabled",			VertexColorEnabledValue,			ZE_VRT_BOOLEAN,		false),

		ZEML_LIST_PROPERTY("Opacity",						OpacityValue,						ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("OpacityComponent",				OpacityComponentValue,				ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("AlphaCullLimit",				AlphaCullLimitValue,				ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("DetailMapTiling",				DetailMapTilingValue,				ZE_VRT_VECTOR2,		false),
		ZEML_LIST_PROPERTY("DetailDistanceFull",			DetailDistanceFullValue,			ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("DetailDistanceFade",			DetailDistanceFadeValue,			ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("SpecularShininess",				SpecularShininessValue,				ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("SubSurfaceScatteringFactor",	SubSurfaceScatteringFactorValue,	ZE_VRT_FLOAT,		false),

		ZEML_LIST_PROPERTY("AmbientColor",					AmbientColorValue,					ZE_VRT_VECTOR3,		false),
		ZEML_LIST_PROPERTY("AmbientFactor",					AmbientFactorValue,					ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("SpecularColor",					SpecularColorValue,					ZE_VRT_VECTOR3,		false),
		ZEML_LIST_PROPERTY("SpecularFactor",				SpecularFactorValue,				ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("DiffuseColor",					DiffuseColorValue,					ZE_VRT_VECTOR3,		false),
		ZEML_LIST_PROPERTY("DiffuseFactor",					DiffuseFactorValue,					ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("EmmisiveColor",					EmissiveColorValue,					ZE_VRT_VECTOR3,		false),
		ZEML_LIST_PROPERTY("EmmisiveFactor",				EmissiveFactorValue,				ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("RefractionIndex",				RefractionIndexValue,				ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("RefractionFactor",				RefractionFactorValue,				ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("ReflectionFactor",				ReflectionFactorValue,				ZE_VRT_FLOAT,		false),

		ZEML_LIST_PROPERTY("BaseMap",						BaseMapValue,						ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("BaseMapAddressModeU",			BaseMapAddressModeUValue,			ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("BaseMapAddressModeV",			BaseMapAddressModeVValue,			ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("NormalMap",						NormalMapValue,						ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("NormalMapAddressModeU",			NormalMapAddressModeUValue,			ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("NormalMapAddressModeV",			NormalMapAddressModeVValue,			ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("ParallaxMap",					ParallaxMapValue,					ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("ParallaxMapAddressModeU",		ParallaxMapAddressModeUValue,		ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("ParallaxMapAddressModeV",		ParallaxMapAddressModeVValue,		ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("SpecularMap",					SpecularMapValue,					ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("SpecularMapAddressModeU",		SpecularMapAddressModeUValue,		ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("SpecularMapAddressModeV",		SpecularMapAddressModeVValue,		ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("EmmisiveMap",					EmissiveMapValue,					ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("EmmisiveMapAddressModeU",		EmissiveMapAddressModeUValue,		ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("EmmisiveMapAddressModeV",		EmissiveMapAddressModeVValue,		ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("OpacityMap",					OpacityMapValue,					ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("OpacityMapAddressModeU",		OpacityMapAddressModeUValue,		ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("OpacityMapAddressModeV",		OpacityMapAddressModeVValue,		ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("DetailBaseMap",					DetailBaseMapValue,					ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("DetailBaseMapAddressModeU",		DetailBaseMapAddressModeUValue,		ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("DetailBaseMapAddressModeV",		DetailBaseMapAddressModeVValue,		ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("DetailNormalMap",				DetailNormalMapValue,				ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("DetailNormalMapAddressModeU",	DetailNormalMapAddressModeUValue,	ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("DetailNormalMapAddressModeV",	DetailNormalMapAddressModeVValue,	ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("EnvironmentMap",				EnvironmentMapValue,				ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("EnvironmentMapAddressModeU",	EnvironmentMapAddressModeUValue,	ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("EnvironmentMapAddressModeV",	EnvironmentMapAddressModeVValue,	ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("EnvironmentMapAddressModeV",	EnvironmentMapAddressModeWValue,	ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("LightMap",						LightMapValue,						ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("LightMapAddressModeU",			LightMapAddressModeUValue,			ZE_VRT_INTEGER_32,	false),
		ZEML_LIST_PROPERTY("LightMapAddressModeV",			LightMapAddressModeVValue,			ZE_VRT_INTEGER_32,	false)
	};

	if (!Reader.ReadPropertyList(Properties, 76))
		zeError("Cannot read material properties");

	// Configure material

	SetWireframe((WireFrameValue.GetType() != ZE_VRT_UNDEFINED && WireFrameValue.GetBoolean() ? true : false));
	SetTwoSided((TwoSidedValue.GetType() != ZE_VRT_UNDEFINED && TwoSidedValue.GetBoolean() ? true : false));
	SetLightningEnabled((LightingEnabledValue.GetType() != ZE_VRT_UNDEFINED && LightingEnabledValue.GetBoolean() ? true : false));
	SetTransparancyMode(TransparencyModeValue.GetType() == ZE_VRT_UNDEFINED ? (ZEMaterialTransparancyMode)0 : (ZEMaterialTransparancyMode)TransparencyModeValue.GetInt32());

	SetAmbientEnabled((AmbientEnabledValue.GetType() != ZE_VRT_UNDEFINED && AmbientEnabledValue.GetBoolean() ? true : false));
	SetDiffuseEnabled((DiffuseEnabledValue.GetType() != ZE_VRT_UNDEFINED && DiffuseEnabledValue.GetBoolean() ? true : false));
	SetNormalMapEnabled((NormalMapEnabledValue.GetType() != ZE_VRT_UNDEFINED && NormalMapEnabledValue.GetBoolean() ? true : false));
	SetParallaxMapEnabled((ParallaxMapEnabledValue.GetType() != ZE_VRT_UNDEFINED && ParallaxMapEnabledValue.GetBoolean() ? true : false));
	SetSpecularEnabled((SpecularEnabledValue.GetType() != ZE_VRT_UNDEFINED && SpecularEnabledValue.GetBoolean() ? true : false));
	SetEmmisiveEnabled((EmissiveEnabledValue.GetType() != ZE_VRT_UNDEFINED && EmissiveEnabledValue.GetBoolean() ? true : false));
	SetOpacityEnabled((OpacityEnabledValue.GetType() != ZE_VRT_UNDEFINED && OpacityEnabledValue.GetBoolean() ? true : false));
	SetDetailBaseMapEnabled((DetailBaseMapEnabledValue.GetType() != ZE_VRT_UNDEFINED && DetailBaseMapEnabledValue.GetBoolean() ? true : false));
	SetDetailNormalMapEnabled((DetailNormalMapEnabledValue.GetType() != ZE_VRT_UNDEFINED && DetailNormalMapEnabledValue.GetBoolean() ? true : false));
	SetReflectionEnabled((ReflectionEnabledValue.GetType() != ZE_VRT_UNDEFINED && ReflectionEnabledValue.GetBoolean() ? true : false));
	SetRefractionEnabled((RefractionEnabledValue.GetType() != ZE_VRT_UNDEFINED && RefractionEnabledValue.GetBoolean() ? true : false));
	SetLightMapEnabled((LightMapEnabledValue.GetType() != ZE_VRT_UNDEFINED && LightMapEnabledValue.GetBoolean() ? true : false));
	SetAlphaCullEnabled((AlphaCullEnabledValue.GetType() != ZE_VRT_UNDEFINED && AlphaCullEnabledValue.GetBoolean() ? true : false));
	SetVertexColorEnabled((VertexColorEnabledValue.GetType() != ZE_VRT_UNDEFINED && VertexColorEnabledValue.GetBoolean() ? true : false));

	SetOpacity(OpacityValue.GetType() == ZE_VRT_UNDEFINED ? 1.0f : OpacityValue.GetFloat());
	SetOpacityComponent((ZEMaterialOpacityComponent)(OpacityComponentValue.GetType() == ZE_VRT_UNDEFINED ? 0 : OpacityComponentValue.GetInt32()));
	SetDetailMapTiling(DetailMapTilingValue.GetType() == ZE_VRT_UNDEFINED ? ZEVector2::One : DetailMapTilingValue.GetVector2());
	SetAmbientColor(AmbientColorValue.GetType() == ZE_VRT_UNDEFINED ? ZEVector3::One : AmbientColorValue.GetVector3());
	SetAmbientFactor(AmbientFactorValue.GetType() == ZE_VRT_UNDEFINED ? 1.0f : AmbientFactorValue.GetFloat());
	SetSpecularColor(SpecularColorValue.GetType() == ZE_VRT_UNDEFINED ? ZEVector3::One : SpecularColorValue.GetVector3());
	SetSpecularFactor(SpecularFactorValue.GetType() == ZE_VRT_UNDEFINED ? 1.0f : SpecularFactorValue.GetFloat());
	SetDiffuseColor(DiffuseColorValue.GetType() == ZE_VRT_UNDEFINED ? ZEVector3::One : DiffuseColorValue.GetVector3());
	SetDiffuseFactor(DiffuseFactorValue.GetType() == ZE_VRT_UNDEFINED ? 1.0f : DiffuseFactorValue.GetFloat());
	SetEmmisiveColor(EmissiveColorValue.GetType() == ZE_VRT_UNDEFINED ? ZEVector3::One : EmissiveColorValue.GetVector3());
	SetEmmisiveFactor(EmissiveFactorValue.GetType() == ZE_VRT_UNDEFINED ? 1.0f : EmissiveFactorValue.GetFloat());
	SetRefractionIndex(RefractionIndexValue.GetType() == ZE_VRT_UNDEFINED ? 1.0f : RefractionIndexValue.GetFloat());
	SetRefractionFactor(RefractionFactorValue.GetType() == ZE_VRT_UNDEFINED ? 1.0f : RefractionFactorValue.GetFloat());
	SetReflectionFactor(ReflectionFactorValue.GetType() == ZE_VRT_UNDEFINED ? 1.0f : ReflectionFactorValue.GetFloat());
	SetDiffuseSubSurfaceScatteringFactor(SubSurfaceScatteringFactorValue.GetType() == ZE_VRT_UNDEFINED ? 1.0f : SubSurfaceScatteringFactorValue.GetFloat());
	SetAlphaCullLimit(AlphaCullLimitValue.GetType() == ZE_VRT_UNDEFINED ? 1.0f : AlphaCullLimitValue.GetFloat());
	SetSpecularShininess(SpecularShininessValue.GetType() == ZE_VRT_UNDEFINED ? 1.0f : SpecularShininessValue.GetFloat());

	SetBaseMapAddressModeU((ZETextureAddressMode)(BaseMapAddressModeUValue.GetType() == ZE_VRT_UNDEFINED ? 0 : BaseMapAddressModeUValue.GetInt32()));
	SetBaseMapAddressModeV((ZETextureAddressMode)(BaseMapAddressModeVValue.GetType() == ZE_VRT_UNDEFINED ? 0 : BaseMapAddressModeVValue.GetInt32()));
	SetNormalMapAddressModeU((ZETextureAddressMode)(NormalMapAddressModeUValue.GetType() == ZE_VRT_UNDEFINED ? 0 : NormalMapAddressModeUValue.GetInt32()));
	SetNormalMapAddressModeV((ZETextureAddressMode)(NormalMapAddressModeVValue.GetType() == ZE_VRT_UNDEFINED ? 0 : NormalMapAddressModeVValue.GetInt32()));
	SetParallaxMapAddressModeU((ZETextureAddressMode)(ParallaxMapAddressModeUValue.GetType() == ZE_VRT_UNDEFINED ? 0 : ParallaxMapAddressModeUValue.GetInt32()));
	SetParallaxMapAddressModeV((ZETextureAddressMode)(ParallaxMapAddressModeVValue.GetType() == ZE_VRT_UNDEFINED ? 0 : ParallaxMapAddressModeVValue.GetInt32()));
	SetSpecularMapAddressModeU((ZETextureAddressMode)(SpecularMapAddressModeUValue.GetType() == ZE_VRT_UNDEFINED ? 0 : SpecularMapAddressModeUValue.GetInt32()));
	SetSpecularMapAddressModeV((ZETextureAddressMode)(SpecularMapAddressModeVValue.GetType() == ZE_VRT_UNDEFINED ? 0 : SpecularMapAddressModeVValue.GetInt32()));
	SetEmmisiveMapAddressModeU((ZETextureAddressMode)(EmissiveMapAddressModeUValue.GetType() == ZE_VRT_UNDEFINED ? 0 : EmissiveMapAddressModeUValue.GetInt32()));
	SetEmmisiveMapAddressModeV((ZETextureAddressMode)(EmissiveMapAddressModeVValue.GetType() == ZE_VRT_UNDEFINED ? 0 : EmissiveMapAddressModeVValue.GetInt32()));
	SetOpacityMapAddressModeU((ZETextureAddressMode)(OpacityMapAddressModeUValue.GetType() == ZE_VRT_UNDEFINED ? 0 : OpacityMapAddressModeUValue.GetInt32()));
	SetOpacityMapAddressModeV((ZETextureAddressMode)(OpacityMapAddressModeVValue.GetType() == ZE_VRT_UNDEFINED ? 0 : OpacityMapAddressModeVValue.GetInt32()));
	SetDetailBaseMapAddressModeU((ZETextureAddressMode)(DetailBaseMapAddressModeUValue.GetType() == ZE_VRT_UNDEFINED ? 0 : DetailBaseMapAddressModeUValue.GetInt32()));
	SetDetailBaseMapAddressModeV((ZETextureAddressMode)(DetailBaseMapAddressModeVValue.GetType() == ZE_VRT_UNDEFINED ? 0 : DetailBaseMapAddressModeVValue.GetInt32()));
	SetDetailNormalMapAddressModeU((ZETextureAddressMode)(DetailNormalMapAddressModeUValue.GetType() == ZE_VRT_UNDEFINED ? 0 : DetailNormalMapAddressModeUValue.GetInt32()));
	SetDetailNormalMapAddressModeV((ZETextureAddressMode)(DetailNormalMapAddressModeVValue.GetType() == ZE_VRT_UNDEFINED ? 0 : DetailNormalMapAddressModeVValue.GetInt32()));
	SetEnvironmentMapAddressModeU((ZETextureAddressMode)(EnvironmentMapAddressModeUValue.GetType() == ZE_VRT_UNDEFINED ? 0 : EnvironmentMapAddressModeUValue.GetInt32()));
	SetEnvironmentMapAddressModeV((ZETextureAddressMode)(EnvironmentMapAddressModeVValue.GetType() == ZE_VRT_UNDEFINED ? 0 : EnvironmentMapAddressModeVValue.GetInt32()));
	SetEnvironmentMapAddressModeW((ZETextureAddressMode)(EnvironmentMapAddressModeWValue.GetType() == ZE_VRT_UNDEFINED ? 0 : EnvironmentMapAddressModeWValue.GetInt32()));
	SetLightMapAddressModeU((ZETextureAddressMode)(LightMapAddressModeUValue.GetType() == ZE_VRT_UNDEFINED ? 0 : LightMapAddressModeUValue.GetInt32()));
	SetLightMapAddressModeV((ZETextureAddressMode)(LightMapAddressModeVValue.GetType() == ZE_VRT_UNDEFINED ? 0 : LightMapAddressModeVValue.GetInt32()));

	// Load resources
	ZEString ResourcePath;
	if (BaseMapValue.GetType() != ZE_VRT_UNDEFINED && !BaseMapValue.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + BaseMapValue.GetString();
		SetBaseMapFile(ResourcePath);
	}
	if (NormalMapValue.GetType() != ZE_VRT_UNDEFINED && !NormalMapValue.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + NormalMapValue.GetString();
		SetNormalMapFile(ResourcePath);
	}
	if (ParallaxMapValue.GetType() != ZE_VRT_UNDEFINED && !ParallaxMapValue.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + ParallaxMapValue.GetString();
		SetParallaxMapFile(ResourcePath);
	}
	if (SpecularMapValue.GetType() != ZE_VRT_UNDEFINED && !SpecularMapValue.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + SpecularMapValue.GetString();
		SetSpecularMapFile(ResourcePath);
	}
	if (EmissiveMapValue.GetType() != ZE_VRT_UNDEFINED && !EmissiveMapValue.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + EmissiveMapValue.GetString();
		SetEmmisiveMapFile(ResourcePath);
	}
	if (OpacityMapValue.GetType() != ZE_VRT_UNDEFINED && !OpacityMapValue.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + OpacityMapValue.GetString();
		SetOpacityMapFile(ResourcePath);
	}
	if (DetailBaseMapValue.GetType() != ZE_VRT_UNDEFINED && !DetailBaseMapValue.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + DetailBaseMapValue.GetString();
		SetDetailBaseMapFile(ResourcePath);
	}
	if (DetailNormalMapValue.GetType() != ZE_VRT_UNDEFINED && !DetailNormalMapValue.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + DetailNormalMapValue.GetString();
		SetDetailNormalMapFile(ResourcePath);
	}
	if (EnvironmentMapValue.GetType() != ZE_VRT_UNDEFINED && !EnvironmentMapValue.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + EnvironmentMapValue.GetString();
		SetEnvironmentMapFile(ResourcePath);
	}
	if (LightMapValue.GetType() != ZE_VRT_UNDEFINED && !LightMapValue.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + LightMapValue.GetString();
		SetLightMapFile(ResourcePath);
	}

	UpdateMaterial();
}
