//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFixedMaterial.h
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
#ifndef __ZE_FIXED_MATERIAL_H__ 
#define __ZE_FIXED_MATERIAL_H__

#include "ZEMaterial.h"
#include "ZEMath/ZEVector.h"
#include "ZEDefinitions.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEValue.h"

#include "ZEMeta/ZEObject.h"

ZE_META_FORWARD_DECLARE(ZETextureCube, "ZETextureCube.h")
ZE_META_FORWARD_DECLARE(ZETextureCubeResource, "ZETexture/ZETextureCubeResource.h")
ZE_META_FORWARD_DECLARE(ZETexture2D, "ZETexture2D.h")
ZE_META_FORWARD_DECLARE(ZETexture2DResource, "ZETexture/ZETexture2DResource.h")

class ZEFixedMaterial : public ZEMaterial
{
	ZE_OBJECT

	protected:
		ZEUInt32						OldMaterialComponents;
		ZEUInt32						MaterialComponents;
		ZEUInt32						MaterialComponentMask;

		ZEMaterialOpacityComponent		OpacityComponent;

		ZEString						Name;
		ZEString						FileName;

		bool							TwoSided;
		bool							Wireframe;
	
		ZEMaterialTransparancyMode		TransparancyMode;

		ZEVector3						AmbientColor;
		float							AmbientFactor;

		ZEVector3						SpecularColor;
		float							SpecularFactor;
		
		ZEVector3						DiffuseColor;
		float							DiffuseFactor;

		ZEVector3						EmmisiveColor;
		float							EmmisiveFactor;

		ZEVector3						DetailBaseMapColor;
		float							DetailBaseMapFactor;

		bool							GlobalAmbientEnabled;

		#ifndef ZE_META_COMPILER
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
				float					SubSurfaceScatteringFactor;
				float					AlphaCullLimit;

				ZEVector2				DetailBaseMapTiling;
				float					DetailBaseMapAttenuationStart;
				float					DetailBaseMapAttenuationFactor;

				ZEVector2				DetailNormalMapTiling;
				float					DetailNormalMapAttenuationStart;
				float					DetailNormalMapAttenuationFactor;

				ZEVector3				DetailBaseMapColorSC;
				float					DetailNormalMapFactor;

			};
			float						PixelShaderConstants[32];
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
				bool					SkinTransform;
				bool					ShadowReceiver;
			};
			float						PixelShaderBoolConstants[2];
		};

		union
		{
			struct
			{
				const ZETexture2D*		BaseMap;
				const ZETexture2D*		NormalMap;
				const ZETexture2D*		ParallaxMap;
				const ZETexture2D*		SpecularMap;
				const ZETexture2D*		EmmisiveMap;
				const ZETexture2D*		OpacityMap;
				const ZETexture2D*		DetailBaseMap;
				const ZETexture2D*		DetailNormalMap;
				const ZETextureCube*	EnvironmentMap;
				const ZETexture2D*		LightMap;
				const ZETexture2D*		DistortionMap;
			};
			ZETexture2D*				Textures[12];
		};
		#endif

		ZETexture2DResource*			BaseMapResource;
		ZETexture2DResource*			NormalMapResource;
		ZETexture2DResource*			ParallaxMapResource;
		ZETexture2DResource*			SpecularMapResource;
		ZETexture2DResource*			EmmisiveMapResource;
		ZETexture2DResource*			OpacityMapResource;
		ZETexture2DResource*			DetailBaseMapResource;
		ZETexture2DResource*			DetailNormalMapResource;
		ZETextureCubeResource*			EnvironmentMapResource;
		ZETexture2DResource*			LightMapResource;
		ZETexture2DResource*			DistortionMapResource;

		ZETextureAddressMode			BaseMapAddressModeU;
		ZETextureAddressMode			BaseMapAddressModeV;

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

		ZETextureAddressMode			DetailBaseMapAddressModeU;
		ZETextureAddressMode			DetailBaseMapAddressModeV;

		ZETextureAddressMode			DetailNormalMapAddressModeU;
		ZETextureAddressMode			DetailNormalMapAddressModeV;

		ZETextureAddressMode			EnvironmentMapAddressModeU;
		ZETextureAddressMode			EnvironmentMapAddressModeV;
		ZETextureAddressMode			EnvironmentMapAddressModeW;

		ZETextureAddressMode			LightMapAddressModeU;
		ZETextureAddressMode			LightMapAddressModeV;

		ZETextureAddressMode			DistortionMapAddressModeU;
		ZETextureAddressMode			DistortionMapAddressModeV;

										ZEFixedMaterial();
		virtual							~ZEFixedMaterial();

	public:
		virtual ZEMaterialFlags			GetMaterialFlags() const;

		// Material Options
		void							SetName(const ZEString& Name);
		const ZEString&					GetName() const;

		const ZEString&					GetFileName() const;

		void							SetTwoSided(bool Enable);
		bool							GetTwoSided() const;

		void							SetWireframe(bool Enable);
		bool							GetWireframe() const;

		void							SetTransparancyMode(ZEMaterialTransparancyMode Mode);
		ZEMaterialTransparancyMode		GetTransparancyMode() const;

		void							SetAlphaCullEnabled(bool Enabled);
		bool							GetAlphaCullEnabled() const;

		void							SetAlphaCullLimit(float Limit);
		float							GetAlphaCullLimit() const;

		// Ambient
		void							SetAmbientEnabled(bool Enabled);
		bool							GetAmbientEnabled() const;
		void							SetAmbientFactor(float Factor);
		float							GetAmbientFactor() const;
		void							SetAmbientColor(const ZEVector3& Color);
		const ZEVector3&				GetAmbientColor() const;

		void							SetGlobalAmbientEnabled(bool Enabled);
		bool							GetGlobalAmbientEnabled() const;

		void							SetBaseMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetBaseMap() const;
		void							SetBaseMapFile(const char* Filename);
		const char*						GetBaseMapFile() const;
		void							SetBaseMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetBaseMapAddressModeU() const;
		void							SetBaseMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetBaseMapAddressModeV() const;

		// Diffuse
		void							SetDiffuseEnabled(bool Enabled);
		bool							GetDiffuseEnabled() const;
		void							SetDiffuseColor(const ZEVector3& Color);
		const ZEVector3&				GetDiffuseColor() const;
		void							SetDiffuseFactor(float Factor);
		float							GetDiffuseFactor() const;
		void							SetDiffuseSubSurfaceScatteringFactor(float Factor);
		float							GetDiffuseSubSurfaceScatteringFactor() const;

		// Specular
		void							SetSpecularEnabled(bool Enabled);
		bool							GetSpecularEnabled() const;
		void							SetSpecularFactor(float Factor);
		float							GetSpecularFactor() const;
		void							SetSpecularShininess(float Shininess);
		float							GetSpecularShininess() const;
		void							SetSpecularColor(const ZEVector3& Color);
		const ZEVector3&				GetSpecularColor() const;
		void							SetSpecularMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetSpecularMap() const;
		void							SetSpecularMapFile(const char* Filename);
		const char*						GetSpecularMapFile() const;
		void							SetSpecularMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetSpecularMapAddressModeU() const;
		void							SetSpecularMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetSpecularMapAddressModeV() const;

		// Emmisive
		void							SetEmmisiveEnabled(bool Enabled);
		bool							GetEmmisiveEnabled() const;
		void							SetEmmisiveFactor(float Factor);
		float							GetEmmisiveFactor() const;
		void							SetEmmisiveColor(const ZEVector3& Color);
		const ZEVector3&				GetEmmisiveColor() const;
		void							SetEmmisiveMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetEmmisiveMap() const;
		void							SetEmmisiveMapFile(const char* Filename);
		const char*						GetEmmisiveMapFile() const;
		void							SetEmmisiveMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetEmmisiveMapAddressModeU() const;
		void							SetEmmisiveMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetEmmisiveMapAddressModeV() const;

		// Normal Map
		void							SetNormalMapEnabled(bool Enabled);
		bool							GetNormalMapEnabled() const;
		void							SetNormalMap(const ZETexture2D*	Texture);
		const ZETexture2D*				GetNormalMap() const;
		void							SetNormalMapFile(const char* Filename);
		const char*						GetNormalMapFile() const;
		void							SetNormalMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetNormalMapAddressModeU() const;
		void							SetNormalMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetNormalMapAddressModeV() const;

		// ParallaxMap
		void							SetParallaxMapEnabled(bool Enabled);
		bool							GetParallaxMapEnabled() const;
		void							SetParallaxMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetParallaxMap() const;
		void							SetParallaxMapFile(const char* Filename);
		const char*						GetParallaxMapFile() const;
		void							SetParallaxMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetParallaxMapAddressModeU() const;
		void							SetParallaxMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetParallaxMapAddressModeV() const;

		// Opacity
		void							SetOpacityEnabled(bool Enabled);
		bool							GetOpacityEnabled() const;
		void							SetOpacity(float Value);
		float							GetOpacity() const;
		void							SetOpacityComponent(ZEMaterialOpacityComponent Component);
		ZEMaterialOpacityComponent		GetOpacityComponent() const;
		void							SetOpacityMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetOpacityMap() const;
		void							SetOpacityMapFile(const char* Filename);
		const char*						GetOpacityMapFile() const;
		void							SetOpacityMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetOpacityMapAddressModeU() const;
		void							SetOpacityMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetOpacityMapAddressModeV() const;

		// Environment Map
		void							SetEnvironmentMap(const ZETextureCube* Texture);
		const ZETextureCube*			GetEnvironmentMap() const;
		void							SetEnvironmentMapFile(const char* Filename);
		const char*						GetEnvironmentMapFile() const;
		void							SetEnvironmentMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetEnvironmentMapAddressModeU() const;
		void							SetEnvironmentMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetEnvironmentMapAddressModeV() const;
		void							SetEnvironmentMapAddressModeW(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetEnvironmentMapAddressModeW() const;

		// Reflection
		void							SetReflectionEnabled(bool Enabled);
		bool							GetReflectionEnabled() const;
		void							SetReflectionFactor(float Factor);
		float							GetReflectionFactor() const;

		// Refraction
		void							SetRefractionEnabled(bool Enabled);
		bool							GetRefractionEnabled() const;
		void							SetRefractionIndex(float Index);
		float							GetRefractionIndex() const;
		void							SetRefractionFactor(float Factor);
		float							GetRefractionFactor() const;

		// Detail Base Map  
		void							SetDetailBaseMapEnabled(bool Enabled); 
		bool							GetDetailBaseMapEnabled() const;
		void							SetDetailBaseMapTiling(const ZEVector2& Tiling);
		const ZEVector2&				GetDetailBaseMapTiling() const;
		void							SetDetailBaseMapFactor(float Factor);
		float							GetDetailBaseMapFactor() const;
		void							SetDetailBaseMapColor(const ZEVector3& DiffuseColor);
		const ZEVector3&				GetDetailBaseMapColor() const;
		void							SetDetailBaseMapAttenuationStart(float Start);
		float							GetDetailBaseMapAttenuationStart() const;
		void							SetDetailBaseMapAttenuationFactor(float Factor);
		float							GetDetailBaseMapAttenuationFactor() const;
		void							SetDetailBaseMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetDetailBaseMap() const;
		void							SetDetailBaseMapFile(const char* Filename);
		const char*						GetDetailBaseMapFile() const;
		void							SetDetailBaseMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDetailBaseMapAddressModeU() const;
		void							SetDetailBaseMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDetailBaseMapAddressModeV() const;
		
		// Detail Normal Map
		void							SetDetailNormalMapEnabled(bool Enabled); 
		bool							GetDetailNormalMapEnabled() const;
		void							SetDetailNormalMapTiling(const ZEVector2& Tiling);
		const ZEVector2&				GetDetailNormalMapTiling() const;
		void							SetDetailNormalMapFactor(float Factor);
		float							GetDetailNormalMapFactor() const;
		void							SetDetailNormalMapAttenuationStart(float Start);
		float							GetDetailNormalMapAttenuationStart() const;
		void							SetDetailNormalMapAttenuationFactor(float Factor);
		float							GetDetailNormalMapAttenuationFactor() const;
		void							SetDetailNormalMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetDetailNormalMap() const;
		void							SetDetailNormalMapFile(const char* Filename);
		const char*						GetDetailNormalMapFile() const;
		void							SetDetailNormalMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDetailNormalMapAddressModeU() const;
		void							SetDetailNormalMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDetailNormalMapAddressModeV() const;


		// LightMap/Ambient Occlusion Map
		void							SetLightMapEnabled(bool Enabled);
		bool							GetLightMapEnabled() const;
		void							SetLightMapFactor(float Factor);
		float							GetLightMapFactor() const;
		void							SetLightMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetLightMap() const;
		void							SetLightMapFile(const char* Filename);
		const char*						GetLightMapFile() const;
		void							SetLightMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetLightMapAddressModeU() const;
		void							SetLightMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetLightMapAddressModeV() const;

		// Distortion
		void							SetDistortionEnabled(bool Enabled);
		bool							GetDistortionEnabled() const;
		void							SetDistortionFactor(float Factor);
		float							GetDistortionFactor() const;
		void							SetDistortionMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetDistortionMap() const;
		void							SetDistortionMapFile(const char* Filename);
		const char*						GetDistortionMapFile() const;
		void							SetDistortionMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDistortionMapAddressModeU() const;
		void							SetDistortionMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDistortionMapAddressModeV() const;

		void							SetSSAOEnabled(bool Enabled);
		bool							GetSSAOEnabled() const;

		void							SetVertexColorEnabled(bool Enabled);
		bool							GetVertexColorEnabled();

		void							Tick(float ElapsedTime);

		void							WriteToFile(const ZEString& FilePath);
		void							ReadFromFile(const ZEString& FilePath);

		static ZEFixedMaterial*			CreateInstance();
};
#endif
