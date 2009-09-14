//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - FixedMaterial.h
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
#ifndef __ZE_FIXEDMATERIAL_H__ 
#define __ZE_FIXEDMATERIAL_H__

#include "Material.h"
#include "ZEMath/Vector.h"
#include "Definitions.h"
#include "Texture.h"
#include "ZEDS/Array.h"
#include "ZEDS/Variant.h"

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

struct ZEMaterialAnimationKey
{
	unsigned int							PropertyIndex;
	ZEVariant								Value;
};

struct ZEMaterialAnimationFrame
{

	ZEArray<ZEMaterialAnimationKey>			Keys;
};

struct ZEMaterialAnimation
{

	ZEArray<ZEMaterialAnimationFrame>		Frames;
	char									Name[ZE_MAX_NAME_SIZE];
};

class ZEFixedMaterial : public ZEMaterial
{
	protected:
		unsigned int					OldMaterialComponents;
		unsigned int					MaterialComponents;

		bool							TwoSided;
		bool							LightningEnabled;
		bool							Wireframe;
		ZEMaterialTransparancyMode		TransparancyMode;
		bool							RecivesShadow;
		unsigned int					TransparancyCullLimit;
		
		ZEVector3						AmbientColor;
		float							AmbientFactor;

		ZEVector3						SpecularColor;
		float							SpecularFactor;
		
		ZEVector3						DiffuseColor;
		float							DiffuseFactor;

		ZEVector3						EmmisiveColor;
		float							EmmisiveFactor;

		union
		{
			struct
			{
				ZEVector3				AmbientColorSC;
				float					Opacity;

				ZEVector3				DiffuseColorSC;
				float					DistortionFactor;

				ZEVector3				SpecularColorSC;
				float					SpecularShininess;

				ZEVector3				EmmisiveColorSC;
				float					LightMapFactor;

				float					ReflectionFactor;
				float					RefractionFactor;
				ZEVector2				DetailMapTiling;
			};
			float						PixelShaderConstants[20];
		};

		union
		{
			struct
			{
				float					RefractionIndex;
			};
			float						VertexShaderConstants[4];
		};

		union
		{
			struct
			{
				const ZETexture2D*		DiffuseMap;
				const ZETexture2D*		NormalMap;
				const ZETexture2D*		ParallaxMap;
				const ZETexture2D*		SpecularMap;
				const ZETexture2D*		EmmisiveMap;
				const ZETexture2D*		OpacityMap;
				const ZETexture2D*		DetailDiffuseMap;
				const ZETexture2D*		DetailNormalMap;
				const ZETextureCube*	ReflectionMap;
				const ZETextureCube*	RefractionMap;
				const ZETexture2D*		LightMap;
				const ZETexture2D*		DistortionMap;
			};
			ZETexture2D*					Textures[12];
		};

		ZETextureAddressMode			DiffuseMapAddressModeU;
		ZETextureAddressMode			DiffuseMapAddressModeV;

		ZETextureAddressMode			NormalMapAddressModeU;
		ZETextureAddressMode			NormalMapAddressModeV;

		ZETextureAddressMode			ParallaxMapAddressModeU;
		ZETextureAddressMode			ParallaxMapAddressModeV;

		ZETextureAddressMode			SpecularMapAddressModeU;
		ZETextureAddressMode			SpecularMapAddressModeV;

		ZETextureAddressMode			EmmisiveMapAddressModeU;
		ZETextureAddressMode			EmmisiveMapAddressModeV;

		ZETextureAddressMode			OpacityMapAddressModeU;
		ZETextureAddressMode			OpacityMapAddressModeV;

		ZETextureAddressMode			DetailDiffuseMapAddressModeU;
		ZETextureAddressMode			DetailDiffuseMapAddressModeV;

		ZETextureAddressMode			DetailNormalMapAddressModeU;
		ZETextureAddressMode			DetailNormalMapAddressModeV;

		ZETextureAddressMode			ReflectionMapAddressModeU;
		ZETextureAddressMode			ReflectionMapAddressModeV;
		ZETextureAddressMode			ReflectionMapAddressModeW;

		ZETextureAddressMode			RefractionMapAddressModeU;
		ZETextureAddressMode			RefractionMapAddressModeV;
		ZETextureAddressMode			RefractionMapAddressModeW;

		ZETextureAddressMode			LightMapAddressModeU;
		ZETextureAddressMode			LightMapAddressModeV;

		ZETextureAddressMode			DistortionMapAddressModeU;
		ZETextureAddressMode			DistortionMapAddressModeV;

										ZEFixedMaterial();
		virtual							~ZEFixedMaterial();

	public:
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
		void							SetDiffuseMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetDiffuseMap();
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
		void							SetSpecularMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetSpecularMap();
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
		void							SetEmmisiveMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetEmmisiveMap();
		void							SetEmmisiveMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetEmmisiveMapAddressModeU();
		void							SetEmmisiveMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetEmmisiveMapAddressModeV();

		// Normal Map
		void							SetNormalMapEnabled(bool Enabled);
		bool							GetNormalMapEnabled();
		void							SetNormalMap(const ZETexture2D*	Texture);
		const ZETexture2D*				GetNormalMap();
		void							SetNormalMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetNormalMapAddressModeU();
		void							SetNormalMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetNormalMapAddressModeV();

		// ParallaxMap
		void							SetParallaxMapEnabled(bool Enabled);
		bool							GetParallaxMapEnabled();
		void							SetParallaxMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetParallaxMap();
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
		void							SetOpacityMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetOpacityMap();
		void							SetOpacityMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetOpacityMapAddressModeU();
		void							SetOpacityMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetOpacityMapAddressModeV();

		// Reflection
		void							SetReflectionEnabled(bool Enabled);
		bool							GetReflectionEnabled();
		void							SetReflectionFactor(float Factor);
		float							GetReflectionFactor();
		void							SetReflectionMap(const ZETextureCube* Texture);
		const ZETextureCube*			GetReflectionMap();
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
		void							SetRefractionMap(const ZETextureCube* Texture);
		const ZETextureCube*			GetRefractionMap();
		void							SetRefractionMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetRefractionMapAddressModeU();
		void							SetRefractionMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetRefractionMapAddressModeV();
		void							SetRefractionMapAddressModeW(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetRefractionMapAddressModeW();
		// Detail Map
		void							SetDetailMapEnabled(bool Enabled);
		bool							GetDetailMapEnabled();
		void							SetDetailMapUScale(float u);
		float							GetDetailMapUScale();
		void							SetDetailMapVScale(float v);
		float							GetDetailMapVScale();
		void							SetDetailDiffuseMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetDetailDiffuseMap();
		void							SetDetailDiffuseMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDetailDiffuseMapAddressModeU();
		void							SetDetailDiffuseMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDetailDiffuseMapAddressModeV();
		void							SetDetailNormalMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetDetailNormalMap();
		void							SetDetailNormalMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDetailNormalMapAddressModeU();
		void							SetDetailNormalMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDetailNormalMapAddressModeV();

		// LightMap/Ambient Occlusion Map
		void							SetLightMapEnabled(bool Enabled);
		bool							GetLightMapEnabled();
		void							SetLightMapFactor(float Factor);
		float							GetLightMapFactor();
		void							SetLightMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetLightMap();
		void							SetLightMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetLightMapAddressModeU();
		void							SetLightMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetLightMapAddressModeV();

		// Distortion
		void							SetDistortionEnabled(bool Enabled);
		bool							GetDistortionEnabled();
		void							SetDistortionFactor(float Factor);
		float							GetDistortionFactor();
		void							SetDistortionMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetDistortionMap();
		void							SetDistortionMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDistortionMapAddressModeU();
		void							SetDistortionMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDistortionMapAddressModeV();

		virtual void					SetZero();

		void							PlayAnimation(ZEMaterialAnimation* Animation);
		void							PauseAnimation();
		void							StopAnimation();

		void							Tick(float ElapsedTime);

		static ZEFixedMaterial*			CreateInstance();
};

#endif
