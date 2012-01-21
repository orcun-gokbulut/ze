//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETMaterialFile.cpp
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

#include "ZETMaterialFile.h"
#include <memory.h>
#include <string.h>

#ifndef NULL
#define NULL 0
#endif

void ZEFixedMaterialFile::SetZero()
{
	TwoSided = false;
	LightningEnabled = true;
	Wireframe = false;
	TransparancyMode = ZE_MTM_NOTRANSPARACY;
	TransparancyCullLimit = 0;
	Opacity = 1.0f;
	RecivesShadow = false;

	AmbientEnabled = false;
	AmbientColor = ZEVector3(0.0f, 0.0f, 0.0f);
	AmbientFactor = 1.0f;

	DiffuseEnabled = false;
	DiffuseColor = ZEVector3(1.0f, 1.0f, 1.0f);
	DiffuseFactor = 1.0f;
	DiffuseMap.FileName[0] = '\0';
	DiffuseMap.Internal = false;
	DiffuseMapAddressModeU = ZE_TAM_WRAP;
	DiffuseMapAddressModeV = ZE_TAM_WRAP;

	NormalMapEnabled = false;
	NormalMap.FileName[0] = '\0';
	NormalMap.Internal = false;
	NormalMapAddressModeU = ZE_TAM_WRAP;
	NormalMapAddressModeV = ZE_TAM_WRAP;
	
	ParallaxMapEnabled = false;
	ParallaxMap.FileName[0] = '\0';
	ParallaxMap.Internal = false;
	ParallaxMapAddressModeU = ZE_TAM_WRAP;
	ParallaxMapAddressModeV = ZE_TAM_WRAP;
	
	SpecularEnabled = false;
	SpecularColor = ZEVector3(1.0f, 1.0f, 1.0f);
	SpecularFactor = 1.0f;
	SpecularShininess = 64.0f;
	SpecularMap.FileName[0] = '\0';
	SpecularMap.Internal = false;
	SpecularMapAddressModeU = ZE_TAM_WRAP;
	SpecularMapAddressModeV = ZE_TAM_WRAP;

	EmmisiveEnabled = false;
	EmmisiveColor = ZEVector3(0.0f, 0.0f, 0.0f);
	EmmisiveFactor = 1.0f;
	EmmisiveMap.FileName[0] = '\0';
	EmmisiveMap.Internal = false;
	EmmisiveMapAddressModeU = ZE_TAM_WRAP;
	EmmisiveMapAddressModeV = ZE_TAM_WRAP;

	OpacityEnabled = false;
	Opacity = 1.0f;
	OpacityMap.FileName[0] = '\0';
	OpacityMap.Internal = false;
	OpacityMapAddressModeU = ZE_TAM_WRAP;
	OpacityMapAddressModeV = ZE_TAM_WRAP;

	DetailMapTiling = ZEVector2(1.0f, 1.0f);
	DetailDiffuseMapEnabled = false;
	DetailDiffuseMap.FileName[0] = '\0';
	DetailDiffuseMap.Internal = false;
	DetailDiffuseMapAddressModeU = ZE_TAM_WRAP;
	DetailDiffuseMapAddressModeV = ZE_TAM_WRAP;

	DetailNormalMapEnabled = false;
	DetailNormalMap.FileName[0] = '\0';
	DetailNormalMap.Internal = false;
	DetailNormalMapAddressModeU = ZE_TAM_WRAP;
	DetailNormalMapAddressModeV = ZE_TAM_WRAP;

	ReflectionEnabled = false;
	ReflectionFactor = 1.0f;
	ReflectionMap.FileName[0] = '\0';
	ReflectionMap.Internal = false;
	ReflectionMapAddressModeU = ZE_TAM_WRAP;
	ReflectionMapAddressModeV = ZE_TAM_WRAP;
	ReflectionMapAddressModeW = ZE_TAM_WRAP;

	RefractionEnabled = false; 
	RefractionFactor = 1.0f;
	RefractionIndex = 1.0f;
	RefractionMap.FileName[0] = '\0';
	RefractionMap.Internal = false;
	RefractionMapAddressModeU = ZE_TAM_WRAP;
	RefractionMapAddressModeV = ZE_TAM_WRAP;
	RefractionMapAddressModeW = ZE_TAM_WRAP;

	LightMapEnabled = false;
	LightMapFactor = 1.0f;		
	LightMap.FileName[0] = '\0';
	LightMap.Internal = false;
	LightMapAddressModeU = ZE_TAM_WRAP;
	LightMapAddressModeV = ZE_TAM_WRAP;


	DistortionEnabled = false;
	DistortionFactor = 1.0f;
	DistortionMap.FileName[0] = '\0';
	DistortionMap.Internal = false;
	DistortionMapAddressModeU = ZE_TAM_WRAP;
	DistortionMapAddressModeV = ZE_TAM_WRAP;
}

ZEFixedMaterialFile::ZEFixedMaterialFile()
{
	SetZero();
}

ZEFixedMaterialFile::~ZEFixedMaterialFile()
{

}

void ZEFixedMaterialFile::SetTwoSided(bool Enable)
{
	TwoSided = Enable;
}

bool ZEFixedMaterialFile::GetTwoSided()
{
	return TwoSided;
}

void ZEFixedMaterialFile::SetLightningEnabled(bool Enable)
{
	LightningEnabled = Enable;
}

bool ZEFixedMaterialFile::GetLightningEnabled()
{
	return LightningEnabled;
}

void ZEFixedMaterialFile::SetWireframe(bool Enable)
{
	Wireframe = Enable;
}

bool ZEFixedMaterialFile::GetWireframe()
{
	return Wireframe;
}

void ZEFixedMaterialFile::SetTransparancyMode(ZEMaterialTransparancyMode Mode)
{
	TransparancyMode = Mode;
}

ZEMaterialTransparancyMode ZEFixedMaterialFile::GetTransparancyMode()
{
	return TransparancyMode;
}

void ZEFixedMaterialFile::SetTransparancyCullLimit(ZEUInt Limit)
{
	Limit = TransparancyCullLimit;
}

ZEUInt ZEFixedMaterialFile::GetTransparancyCullLimit()
{
	return TransparancyCullLimit;
}

void ZEFixedMaterialFile::SetRecivesShadow(bool Enable)
{
	RecivesShadow = Enable;
}

bool ZEFixedMaterialFile::GetRecivesShadow()
{
	return RecivesShadow;
}

void ZEFixedMaterialFile::SetAmbientEnabled(bool Enabled)
{
	AmbientEnabled = Enabled;
}

bool ZEFixedMaterialFile::GetAmbientEnabled()
{
	return AmbientEnabled;
}

void ZEFixedMaterialFile::SetAmbientColor(const ZEVector3& Color)
{
	AmbientColor = Color;
}

const ZEVector3& ZEFixedMaterialFile::GetAmbientColor()
{
	return AmbientColor;
}

void ZEFixedMaterialFile::SetAmbientFactor(float Factor)
{
	AmbientFactor = Factor;
}

float ZEFixedMaterialFile::GetAmbientFactor()
{
	return AmbientFactor;
}

void ZEFixedMaterialFile::SetDiffuseEnabled(bool Enabled)
{
	DiffuseEnabled = Enabled;
}

bool ZEFixedMaterialFile::GetDiffuseEnabled()
{
	return DiffuseEnabled;
}

void ZEFixedMaterialFile::SetDiffuseColor(const ZEVector3& Color)
{
	DiffuseColor = Color;
}

const ZEVector3& ZEFixedMaterialFile::GetDiffuseColor()
{
	return DiffuseColor;
}

void ZEFixedMaterialFile::SetDiffuseFactor(float Factor)
{
	DiffuseFactor = Factor;
}

float ZEFixedMaterialFile::GetDiffuseFactor()
{
	return DiffuseFactor;
}

void ZEFixedMaterialFile::SetDiffuseMap(const char* FileName, bool Internal)
{
	strncpy(DiffuseMap.FileName, FileName, ZE_MAX_FILE_NAME_SIZE);
	DiffuseMap.Internal = Internal;
}

const char* ZEFixedMaterialFile::GetDiffuseMap()
{
	return DiffuseMap.FileName;
}

void ZEFixedMaterialFile::SetDiffuseMapAddressModeU(ZETextureAddressMode Mode)
{
	DiffuseMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetDiffuseMapAddressModeU()
{
	return DiffuseMapAddressModeU;
}

void ZEFixedMaterialFile::SetDiffuseMapAddressModeV(ZETextureAddressMode Mode)
{
	DiffuseMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetDiffuseMapAddressModeV()
{
	return DiffuseMapAddressModeV;
}

void ZEFixedMaterialFile::SetSpecularEnabled(bool Enabled)
{
	SpecularEnabled = Enabled;
}

bool ZEFixedMaterialFile::GetSpecularEnabled()
{
	return SpecularEnabled;
}

void ZEFixedMaterialFile::SetSpecularColor(const ZEVector3& Color)
{
	SpecularColor = Color;
}

const ZEVector3& ZEFixedMaterialFile::GetSpecularColor()
{
	return SpecularColor;
}

void ZEFixedMaterialFile::SetSpecularFactor(float Factor)
{
	SpecularFactor = Factor;
}

float ZEFixedMaterialFile::GetSpecularFactor()
{
	return SpecularFactor;
}

void ZEFixedMaterialFile::SetSpecularShininess(float Shininess)
{
	SpecularShininess = Shininess;
}

float ZEFixedMaterialFile::GetSpecularShininess()
{
	return SpecularShininess;
}

void ZEFixedMaterialFile::SetSpecularMap(const char* FileName, bool Internal)
{
	strncpy(SpecularMap.FileName, FileName, ZE_MAX_FILE_NAME_SIZE);
	SpecularMap.Internal = Internal;
}

const char* ZEFixedMaterialFile::GetSpecularMap()
{
	return SpecularMap.FileName;
}

void ZEFixedMaterialFile::SetSpecularMapAddressModeU(ZETextureAddressMode Mode)
{
	SpecularMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetSpecularMapAddressModeU()
{
	return SpecularMapAddressModeU;
}

void ZEFixedMaterialFile::SetSpecularMapAddressModeV(ZETextureAddressMode Mode)
{
	SpecularMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetSpecularMapAddressModeV()
{
	return SpecularMapAddressModeV;
}

void ZEFixedMaterialFile::SetEmmisiveEnabled(bool Enabled)
{
	EmmisiveEnabled = Enabled;
}

bool ZEFixedMaterialFile::GetEmmisiveEnabled()
{
	return EmmisiveEnabled;
}

void ZEFixedMaterialFile::SetEmmisiveFactor(float Factor)
{
	EmmisiveFactor = Factor;
}

float ZEFixedMaterialFile::GetEmmisiveFactor()
{
	return EmmisiveFactor;
}

void ZEFixedMaterialFile::SetEmmisiveColor(const ZEVector3& Color)
{
	EmmisiveColor = Color;
}

const ZEVector3& ZEFixedMaterialFile::GetEmmisiveColor()
{
	return EmmisiveColor;
}

void ZEFixedMaterialFile::SetEmmisiveMap(const char* FileName, bool Internal)
{
	strncpy(EmmisiveMap.FileName, FileName, ZE_MAX_FILE_NAME_SIZE);
	EmmisiveMap.Internal = Internal;
}

const char* ZEFixedMaterialFile::GetEmmisiveMap()
{
	return EmmisiveMap.FileName;
}

void ZEFixedMaterialFile::SetEmmisiveMapAddressModeU(ZETextureAddressMode Mode)
{
	EmmisiveMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetEmmisiveMapAddressModeU()
{
	return EmmisiveMapAddressModeU;
}

void ZEFixedMaterialFile::SetEmmisiveMapAddressModeV(ZETextureAddressMode Mode)
{
	EmmisiveMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetEmmisiveMapAddressModeV()
{
	return EmmisiveMapAddressModeV;
}

void ZEFixedMaterialFile::SetNormalMapEnabled(bool Enabled)
{
	NormalMapEnabled = Enabled;
}

bool ZEFixedMaterialFile::GetNormalMapEnabled()
{
	return NormalMapEnabled;
}

void ZEFixedMaterialFile::SetNormalMap(const char* FileName, bool Internal)
{
	strncpy(NormalMap.FileName, FileName, ZE_MAX_FILE_NAME_SIZE);
	NormalMap.Internal = Internal;
}

const char* ZEFixedMaterialFile::GetNormalMap()
{
	return NormalMap.FileName;
}

void ZEFixedMaterialFile::SetNormalMapAddressModeU(ZETextureAddressMode Mode)
{
	NormalMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetNormalMapAddressModeU()
{
	return NormalMapAddressModeU;
}

void ZEFixedMaterialFile::SetNormalMapAddressModeV(ZETextureAddressMode Mode)
{
	NormalMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetNormalMapAddressModeV()
{
	return NormalMapAddressModeV;
}

void ZEFixedMaterialFile::SetParallaxMapEnabled(bool Enabled)
{
	ParallaxMapEnabled = Enabled;	
}

bool ZEFixedMaterialFile::GetParallaxMapEnabled()
{
	return ParallaxMapEnabled;
}

void ZEFixedMaterialFile::SetParallaxMap(const char* FileName, bool Internal)
{
	strncpy(ParallaxMap.FileName, FileName, ZE_MAX_FILE_NAME_SIZE);
	ParallaxMap.Internal = Internal;
}

const char* ZEFixedMaterialFile::GetParallaxMap()
{
	return ParallaxMap.FileName;
}

void ZEFixedMaterialFile::SetParallaxMapAddressModeU(ZETextureAddressMode Mode)
{
	ParallaxMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetParallaxMapAddressModeU()
{
	return ParallaxMapAddressModeU;
}

void ZEFixedMaterialFile::SetParallaxMapAddressModeV(ZETextureAddressMode Mode)
{
	ParallaxMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetParallaxMapAddressModeV()
{
	return ParallaxMapAddressModeV;
}

void ZEFixedMaterialFile::SetOpacityEnabled(bool Enabled)
{
	OpacityEnabled = Enabled;
}

bool ZEFixedMaterialFile::GetOpacityEnabled()
{
	return OpacityEnabled;
}

void ZEFixedMaterialFile::SetOpacity(float Value)
{
	Opacity = Value;
}

float ZEFixedMaterialFile::GetOpacity()
{
	return Opacity;
}

void ZEFixedMaterialFile::SetOpacityComponent(ZEMaterialOpacityComponent Component)
{
	#pragma message("Task: Implament Fixed Material Ocapacity Component")
}

void ZEFixedMaterialFile::SetOpacityMap(const char* FileName, bool Internal)
{
	strncpy(OpacityMap.FileName, FileName, ZE_MAX_FILE_NAME_SIZE);
	OpacityMap.Internal = Internal;
}

const char* ZEFixedMaterialFile::GetOpacityMap()
{
	return OpacityMap.FileName;
}

void ZEFixedMaterialFile::SetOpacityMapAddressModeU(ZETextureAddressMode Mode)
{
	OpacityMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetOpacityMapAddressModeU()
{
	return OpacityMapAddressModeU;
}

void ZEFixedMaterialFile::SetOpacityMapAddressModeV(ZETextureAddressMode Mode)
{
	OpacityMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetOpacityMapAddressModeV()
{
	return OpacityMapAddressModeV;
}

void ZEFixedMaterialFile::SetReflectionEnabled(bool Enabled)
{
	ReflectionEnabled = Enabled;
}

bool ZEFixedMaterialFile::GetReflectionEnabled()
{
	return ReflectionEnabled;
}

void ZEFixedMaterialFile::SetReflectionFactor(float Factor)
{
	ReflectionFactor = Factor;
}

float ZEFixedMaterialFile::GetReflectionFactor()
{
	return ReflectionFactor;
}

void ZEFixedMaterialFile::SetReflectionMap(const char* FileName, bool Internal)
{
	strncpy(ReflectionMap.FileName, FileName, ZE_MAX_FILE_NAME_SIZE);
	ReflectionMap.Internal = Internal;
}

const char* ZEFixedMaterialFile::GetReflectionMap()
{
	return ReflectionMap.FileName;
}

void ZEFixedMaterialFile::SetReflectionMapAddressModeU(ZETextureAddressMode Mode)
{
	ReflectionMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetReflectionMapAddressModeU()
{
	return ReflectionMapAddressModeU;
}

void ZEFixedMaterialFile::SetReflectionMapAddressModeV(ZETextureAddressMode Mode)
{
	ReflectionMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetReflectionMapAddressModeV()
{
	return ReflectionMapAddressModeV;
}

void ZEFixedMaterialFile::SetReflectionMapAddressModeW(ZETextureAddressMode Mode)
{
	ReflectionMapAddressModeW = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetReflectionMapAddressModeW()
{
	return ReflectionMapAddressModeW;
}

void ZEFixedMaterialFile::SetRefractionEnabled(bool Enabled)
{
	RefractionEnabled = Enabled;
}

bool ZEFixedMaterialFile::GetRefractionEnabled()
{
	return RefractionEnabled;
}

void ZEFixedMaterialFile::SetRefractionIndex(float Index)
{
	RefractionIndex = Index;
}

float ZEFixedMaterialFile::GetRefractionIndex()
{
	return RefractionIndex;
}

void ZEFixedMaterialFile::SetRefractionFactor(float Factor)
{
	RefractionFactor = Factor;
}

float ZEFixedMaterialFile::GetRefractionFactor()
{
	return RefractionFactor;
}

void ZEFixedMaterialFile::SetRefractionMap(const char* FileName, bool Internal)
{
	strncpy(RefractionMap.FileName, FileName, ZE_MAX_FILE_NAME_SIZE);
	RefractionMap.Internal = Internal;
}

const char* ZEFixedMaterialFile::GetRefractionMap()
{
	return RefractionMap.FileName;
}

void ZEFixedMaterialFile::SetRefractionMapAddressModeU(ZETextureAddressMode Mode)
{
	RefractionMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetRefractionMapAddressModeU()
{
	return RefractionMapAddressModeU;
}

void ZEFixedMaterialFile::SetRefractionMapAddressModeV(ZETextureAddressMode Mode)
{
	RefractionMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetRefractionMapAddressModeV()
{
	return RefractionMapAddressModeV;
}

void ZEFixedMaterialFile::SetRefractionMapAddressModeW(ZETextureAddressMode Mode)
{
	RefractionMapAddressModeW = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetRefractionMapAddressModeW()
{
	return RefractionMapAddressModeW;
}

void ZEFixedMaterialFile::SetDetailMapUScale(float u)
{
	DetailMapTiling.x = u;
}

float ZEFixedMaterialFile::GetDetailMapUScale()
{
	return DetailMapTiling.x;
}

void ZEFixedMaterialFile::SetDetailMapVScale(float v)
{
	DetailMapTiling.y = v;
}

float ZEFixedMaterialFile::GetDetailMapVScale()
{
	return DetailMapTiling.y;
}

void ZEFixedMaterialFile::SetDetailDiffuseMapEnabled(bool Enabled)
{
	DetailDiffuseMapEnabled = true;
}

bool ZEFixedMaterialFile::GetDetailDiffuseMapEnabled()
{
	return DetailDiffuseMapEnabled;
}

void ZEFixedMaterialFile::SetDetailDiffuseMap(const char* FileName, bool Internal)
{
	strncpy(DetailDiffuseMap.FileName, FileName, ZE_MAX_FILE_NAME_SIZE);
	DetailDiffuseMap.Internal = Internal;
}

const char* ZEFixedMaterialFile::GetDetailDiffuseMap()
{
	return DetailDiffuseMap.FileName;
}

void ZEFixedMaterialFile::SetDetailDiffuseMapAddressModeU(ZETextureAddressMode Mode)
{
	DetailDiffuseMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetDetailDiffuseMapAddressModeU()
{
	return DetailDiffuseMapAddressModeU;
}

void ZEFixedMaterialFile::SetDetailDiffuseMapAddressModeV(ZETextureAddressMode Mode)
{
	DetailDiffuseMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetDetailDiffuseMapAddressModeV()
{
	return DetailDiffuseMapAddressModeV;
}


void ZEFixedMaterialFile::SetDetailNormalMapEnabled(bool Enabled)
{
	DetailNormalMapEnabled = true;
}

bool ZEFixedMaterialFile::GetDetailNormalMapEnabled()
{
	return DetailNormalMapEnabled;
}

void ZEFixedMaterialFile::SetDetailNormalMap(const char* FileName, bool Internal)
{
	strncpy(DetailNormalMap.FileName, FileName, ZE_MAX_FILE_NAME_SIZE);
	DetailNormalMap.Internal = Internal;
}

const char* ZEFixedMaterialFile::GetDetailNormalMap()
{
	return DetailNormalMap.FileName;
}

void ZEFixedMaterialFile::SetDetailNormalMapAddressModeU(ZETextureAddressMode Mode)
{
	DetailNormalMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetDetailNormalMapAddressModeU()
{
	return DetailNormalMapAddressModeU;
}

void ZEFixedMaterialFile::SetDetailNormalMapAddressModeV(ZETextureAddressMode Mode)
{
	DetailNormalMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetDetailNormalMapAddressModeV()
{
	return DetailNormalMapAddressModeV;
}

void ZEFixedMaterialFile::SetLightMapEnabled(bool Enabled)
{
	LightMapEnabled = Enabled;
}

bool ZEFixedMaterialFile::GetLightMapEnabled()
{
	return LightMapEnabled;
}

void ZEFixedMaterialFile::SetLightMapFactor(float Factor)
{
	LightMapFactor = Factor;
}

float ZEFixedMaterialFile::GetLightMapFactor()
{
	return LightMapFactor;
}

void ZEFixedMaterialFile::SetLightMap(const char* FileName, bool Internal)
{
	strncpy(LightMap.FileName, FileName, ZE_MAX_FILE_NAME_SIZE);
	LightMap.Internal = Internal;
}

const char* ZEFixedMaterialFile::GetLightMap()
{
	return LightMap.FileName;
}

void ZEFixedMaterialFile::SetLightMapAddressModeU(ZETextureAddressMode Mode)
{
	LightMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetLightMapAddressModeU()
{
	return LightMapAddressModeU;
}

void ZEFixedMaterialFile::SetLightMapAddressModeV(ZETextureAddressMode Mode)
{
	LightMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetLightMapAddressModeV()
{
	return LightMapAddressModeV;
}

void ZEFixedMaterialFile::SetDistortionEnabled(bool Enabled)
{
	DistortionEnabled = Enabled;
}

bool ZEFixedMaterialFile::GetDistortionEnabled()
{
	return DistortionEnabled;
}

void ZEFixedMaterialFile::SetDistortionFactor(float Factor)
{
	DistortionFactor = Factor;
}

float ZEFixedMaterialFile::GetDistortionFactor()
{
	return DistortionFactor;
}

void ZEFixedMaterialFile::SetDistortionMap(const char* FileName, bool Internal)
{
	strncpy(DistortionMap.FileName, FileName, ZE_MAX_FILE_NAME_SIZE);
	DistortionMap.Internal = Internal;
}

const char* ZEFixedMaterialFile::GetDistortionMap()
{
	return DistortionMap.FileName; 
}

void ZEFixedMaterialFile::SetDistortionMapAddressModeU(ZETextureAddressMode Mode)
{
	DistortionMapAddressModeU = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetDistortionMapAddressModeU()
{
	return DistortionMapAddressModeU;
}

void ZEFixedMaterialFile::SetDistortionMapAddressModeV(ZETextureAddressMode Mode)
{
	DistortionMapAddressModeV = Mode;
}

ZETextureAddressMode ZEFixedMaterialFile::GetDistortionMapAddressModeV()
{
	return DistortionMapAddressModeV;
}
