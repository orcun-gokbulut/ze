//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETMaterialFile.h
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

#pragma once
#ifndef __ZE_MATERIAL_FILE_H__
#define __ZE_MATERIAL_FILE_H__

#include "Common/Definitions.h"
#include "Common/ZEDS/Array.h"
#include "Common/ZEDS/Variant.h"

enum ZETextureFilterMode
{
	ZE_TF_AUTO					= 0,
	ZE_TF_FORCE_POINT			= 1,
	ZE_TF_FORCE_LINEAR			= 2,
	ZE_TF_FORCE_TRILINEAR		= 3,
	ZE_TF_FORCE_ANISOTROPHIC	= 4,
};

enum ZETextureAddressMode
{
	ZE_TAM_WRAP					= 0,
	ZE_TAM_CLAMP				= 1,
	ZE_TAM_MIRROR				= 2,
	ZE_TAM_BORDER				= 3,
};

enum ZEMaterialTransparancyMode
{
	ZE_MTM_NOTRANSPARACY		= 0,
	ZE_MTM_ALPHACULL			= 1,
	ZE_MTM_REGULAR				= 2,
	ZE_MTM_ADDAPTIVE			= 3,
	ZE_MTM_SUBTRACTIVE			= 4,
};

enum ZEMaterialOpacityComponent
{
	ZE_MOC_CONSTANT				= 0,
	ZE_MOC_DIFFUSE_ALPHA		= 1,
	ZE_MOC_OPACITYMAP			= 2,
};

struct ZEMaterialTexture
{	
	private:
		char							FileName[ZE_MAX_FILE_NAME_SIZE];
		bool							Internal;
		size_t							Size;
		char*							Data;

	public:
		void							SetInternal();
		bool							IsInternal();

		void							SetFileName();
		void							GetFileName();
		
		void							SetSize(size_t Size);
		size_t							GetSize();

		char*							GetData();

		void							LoadInternalTexture(const char* FileName);
};

struct ZEMaterialAnimationKey
{
	unsigned int						PropertyIndex;
	ZEVariant							Value;
};

struct ZEMaterialAnimationFrame
{

	ZEArray<ZEMaterialAnimationKey>		Keys;
};

struct ZEMaterialAnimation
{

	ZEArray<ZEMaterialAnimationFrame>	Frames;
	char								Name[ZE_MAX_NAME_SIZE];
};

class ZEMaterialFile
{
	public:
		ZEArray<ZEMaterialTexture>		Textures;
		ZEArray<ZEMaterialAnimation>	Animation;

};

class ZEFixedMaterialFile : public ZEMaterialFile
{
	private:
		bool							TwoSided;
		bool							LightningEnabled;
		bool							Wireframe;
		ZEMaterialTransparancyMode		TransparancyMode;
		bool							RecivesShadow;
		unsigned int					TransparancyCullLimit;

		bool							AmbientEnabled;
		ZEVector3						AmbientColor;
		float							AmbientFactor;

		bool							DiffuseEnabled;
		ZEVector3						DiffuseColor;
		float							DiffuseFactor;
		ZEMaterialTexture				DiffuseMap;
		ZETextureAddressMode			DiffuseMapAddressModeU;
		ZETextureAddressMode			DiffuseMapAddressModeV;

		bool							NormalMapEnabled;
		ZEMaterialTexture				NormalMap;
		ZETextureAddressMode			NormalMapAddressModeU;
		ZETextureAddressMode			NormalMapAddressModeV;

		bool							ParallaxMapEnabled;
		ZEMaterialTexture				ParallaxMap;
		ZETextureAddressMode			ParallaxMapAddressModeU;
		ZETextureAddressMode			ParallaxMapAddressModeV;

		bool							SpecularEnabled;
		ZEVector3						SpecularColor;
		float							SpecularFactor;
		float							SpecularShininess;
		ZEMaterialTexture				SpecularMap;
		ZETextureAddressMode			SpecularMapAddressModeU;
		ZETextureAddressMode			SpecularMapAddressModeV;

		bool							EmmisiveEnabled;
		ZEVector3						EmmisiveColor;
		float							EmmisiveFactor;
		ZEMaterialTexture				EmmisiveMap;
		ZETextureAddressMode			EmmisiveMapAddressModeU;
		ZETextureAddressMode			EmmisiveMapAddressModeV;

		bool							OpacityEnabled;
		float							Opacity;
		ZEMaterialTexture				OpacityMap;
		ZETextureAddressMode			OpacityMapAddressModeU;
		ZETextureAddressMode			OpacityMapAddressModeV;

		ZEVector2						DetailMapTiling;

		bool							DetailDiffuseMapEnabled;
		ZEMaterialTexture				DetailDiffuseMap;
		ZETextureAddressMode			DetailDiffuseMapAddressModeU;
		ZETextureAddressMode			DetailDiffuseMapAddressModeV;

		bool							DetailNormalMapEnabled;
		ZEMaterialTexture				DetailNormalMap;
		ZETextureAddressMode			DetailNormalMapAddressModeU;
		ZETextureAddressMode			DetailNormalMapAddressModeV;

		bool							ReflectionEnabled;
		float							ReflectionFactor;
		ZEMaterialTexture				ReflectionMap;
		ZETextureAddressMode			ReflectionMapAddressModeU;
		ZETextureAddressMode			ReflectionMapAddressModeV;
		ZETextureAddressMode			ReflectionMapAddressModeW;

		bool							RefractionEnabled;
		float							RefractionFactor;
		float							RefractionIndex;
		ZEMaterialTexture				RefractionMap;
		ZETextureAddressMode			RefractionMapAddressModeU;
		ZETextureAddressMode			RefractionMapAddressModeV;
		ZETextureAddressMode			RefractionMapAddressModeW;

		bool							LightMapEnabled;
		ZEMaterialTexture				LightMap;
		ZETextureAddressMode			LightMapAddressModeU;
		ZETextureAddressMode			LightMapAddressModeV;
		float							LightMapFactor;

		bool							DistortionEnabled;
		ZEMaterialTexture				DistortionMap;
		float							DistortionFactor;
		ZETextureAddressMode			DistortionMapAddressModeU;
		ZETextureAddressMode			DistortionMapAddressModeV;

	public:
		void							SetZero();
		
		// Material Options
		void							SetTwoSided(bool Enable);
		bool							GetTwoSided();
		void							SetLightningEnabled(bool Enable);
		bool							GetLightningEnabled();
		void							SetWireframe(bool Enable);
		bool							GetWireframe();
		void							SetTransparancyMode(ZEMaterialTransparancyMode Mode);
		ZEMaterialTransparancyMode		GetTransparancyMode();
		void							SetTransparancyCullLimit(unsigned int Limit);
		unsigned int					GetTransparancyCullLimit();
		void							SetRecivesShadow(bool Enable);
		bool							GetRecivesShadow();

		// Ambient
		void							SetAmbientEnabled(bool Enabled);
		bool							GetAmbientEnabled();
		void							SetAmbientFactor(float Factor);
		float							GetAmbientFactor();
		void							SetAmbientColor(const ZEVector3& Color);
		const ZEVector3&				GetAmbientColor();

		// Diffuse
		void							SetDiffuseEnabled(bool Enabled);
		bool							GetDiffuseEnabled();
		void							SetDiffuseColor(const ZEVector3& Color);
		const ZEVector3&				GetDiffuseColor();
		void							SetDiffuseFactor(float Factor);
		float							GetDiffuseFactor();
		void							SetDiffuseMap(const char* FileName, bool Internal);
		const char*						GetDiffuseMap();
		void							SetDiffuseMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDiffuseMapAddressModeU();
		void							SetDiffuseMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDiffuseMapAddressModeV();

		// Specular
		void							SetSpecularEnabled(bool Enabled);
		bool							GetSpecularEnabled();
		void							SetSpecularFactor(float Factor);
		float							GetSpecularFactor();
		void							SetSpecularShininess(float Shininess);
		float							GetSpecularShininess();
		void							SetSpecularColor(const ZEVector3& Color);
		const ZEVector3&				GetSpecularColor();
		void							SetSpecularMap(const char* FileName, bool Internal);
		const char*						GetSpecularMap();
		void							SetSpecularMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetSpecularMapAddressModeU();
		void							SetSpecularMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetSpecularMapAddressModeV();

		// Emmisive
		void							SetEmmisiveEnabled(bool Enabled);
		bool							GetEmmisiveEnabled();
		void							SetEmmisiveFactor(float Factor);
		float							GetEmmisiveFactor();
		void							SetEmmisiveColor(const ZEVector3& Color);
		const ZEVector3&				GetEmmisiveColor();
		void							SetEmmisiveMap(const char* FileName, bool Internal);
		const char*						GetEmmisiveMap();
		void							SetEmmisiveMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetEmmisiveMapAddressModeU();
		void							SetEmmisiveMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetEmmisiveMapAddressModeV();

		// Normal Map
		void							SetNormalMapEnabled(bool Enabled);
		bool							GetNormalMapEnabled();
		void							SetNormalMap(const char* FileName, bool Internal);
		const char*						GetNormalMap();
		void							SetNormalMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetNormalMapAddressModeU();
		void							SetNormalMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetNormalMapAddressModeV();

		// ParallaxMap
		void							SetParallaxMapEnabled(bool Enabled);
		bool							GetParallaxMapEnabled();
		void							SetParallaxMap(const char* FileName, bool Internal);
		const char*						GetParallaxMap();
		void							SetParallaxMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetParallaxMapAddressModeU();
		void							SetParallaxMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetParallaxMapAddressModeV();

		// Opacity
		void							SetOpacityEnabled(bool Enabled);
		bool							GetOpacityEnabled();
		void							SetOpacity(float Value);
		float							GetOpacity();
		void							SetOpacityComponent(ZEMaterialOpacityComponent Component);
		ZEMaterialOpacityComponent		GetOpacityComponent();
		void							SetOpacityMap(const char* FileName, bool Internal);
		const char*						GetOpacityMap();
		void							SetOpacityMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetOpacityMapAddressModeU();
		void							SetOpacityMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetOpacityMapAddressModeV();

		// Reflection
		void							SetReflectionEnabled(bool Enabled);
		bool							GetReflectionEnabled();
		void							SetReflectionFactor(float Factor);
		float							GetReflectionFactor();
		void							SetReflectionMap(const char* Texture, bool Internal);
		const char*						GetReflectionMap();
		void							SetReflectionMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetReflectionMapAddressModeU();
		void							SetReflectionMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetReflectionMapAddressModeV();
		void							SetReflectionMapAddressModeW(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetReflectionMapAddressModeW();

		// Refraction
		void							SetRefractionEnabled(bool Enabled);
		bool							GetRefractionEnabled();
		void							SetRefractionIndex(float Index);
		float							GetRefractionIndex();
		void							SetRefractionFactor(float Factor);
		float							GetRefractionFactor();
		void							SetRefractionMap(const char* Texture, bool Internal);
		const char*						GetRefractionMap();
		void							SetRefractionMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetRefractionMapAddressModeU();
		void							SetRefractionMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetRefractionMapAddressModeV();
		void							SetRefractionMapAddressModeW(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetRefractionMapAddressModeW();
		// Detail Map
		void							SetDetailMapUScale(float u);
		float							GetDetailMapUScale();
		void							SetDetailMapVScale(float v);
		float							GetDetailMapVScale();

		void							SetDetailDiffuseMapEnabled(bool Enabled);
		bool							GetDetailDiffuseMapEnabled();
		void							SetDetailDiffuseMap(const char* FileName, bool Internal);
		const char*						GetDetailDiffuseMap();
		void							SetDetailDiffuseMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDetailDiffuseMapAddressModeU();
		void							SetDetailDiffuseMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDetailDiffuseMapAddressModeV();

		void							SetDetailNormalMapEnabled(bool Enabled);
		bool							GetDetailNormalMapEnabled();
		void							SetDetailNormalMap(const char* FileName, bool Internal);
		const char*						GetDetailNormalMap();
		void							SetDetailNormalMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDetailNormalMapAddressModeU();
		void							SetDetailNormalMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDetailNormalMapAddressModeV();

		// LightMap/Ambient Occlusion Map
		void							SetLightMapEnabled(bool Enabled);
		bool							GetLightMapEnabled();
		void							SetLightMapFactor(float Factor);
		float							GetLightMapFactor();
		void							SetLightMap(const char* FileName, bool Internal);
		const char*						GetLightMap();
		void							SetLightMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetLightMapAddressModeU();
		void							SetLightMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetLightMapAddressModeV();

		// Distortion
		void							SetDistortionEnabled(bool Enabled);
		bool							GetDistortionEnabled();
		void							SetDistortionFactor(float Factor);
		float							GetDistortionFactor();
		void							SetDistortionMap(const char* FileName, bool Internal);
		const char*						GetDistortionMap();
		void							SetDistortionMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDistortionMapAddressModeU();
		void							SetDistortionMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDistortionMapAddressModeV();

		bool							Validate();
		void							WriteToFile(const char* FileName);
		void							WriteToFile(FILE* File);

		void							ReadFromFile(const char* FileName);
		void							ReadFromFile(FILE* File);

										ZEFixedMaterialFile();
		virtual							~ZEFixedMaterialFile();

};

#endif
