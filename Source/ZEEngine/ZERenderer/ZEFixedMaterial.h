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

#include "ZETypes.h"
#include "ZEMaterial.h"
#include "ZEMath/ZEVector.h"
#include "ZEGraphics/ZESamplerState.h"
#include "ZEMath/ZEMatrix.h"

class ZEFile;
class ZEShader;
class ZETexture;
class ZETexture2D;
class ZETextureCube;
class ZEConstantBuffer;
class ZETextureResource;
class ZETexture2DResource;
class ZETextureCubeResource;

class ZEFixedMaterial : public ZEMaterial
{
	protected:
		bool						Recompile;

		struct 
		{
			ZEMatrix4x4				WorldView;
			ZEMatrix4x4				WorldViewProj;

		} Transformations;

		struct ZEFixedMaterialComponents
		{
			ZEUInt32				Ambient : 1;
			ZEUInt32				Diffuse	: 1;
			ZEUInt32				BaseMap	: 1;
			ZEUInt32				NormalMap : 1;
			ZEUInt32				ParallaxMap	: 1;
			ZEUInt32				Specular : 1;
			ZEUInt32				SpecularMap	: 1;
			ZEUInt32				Emissive : 1;
			ZEUInt32				EmissiveMap	: 1;
			ZEUInt32				Opacity	: 1;
			ZEUInt32				OpacityBaseAlpha : 1;
			ZEUInt32				OpacityConstant : 1;
			ZEUInt32				OpacityMap : 1;
			ZEUInt32				DetailMap : 1;
			ZEUInt32				DetailBaseMap : 1;
			ZEUInt32				DetailNormalMap : 1;
			ZEUInt32				Reflection : 1;
			ZEUInt32				Refraction : 1;
			ZEUInt32				LightMap : 1;
			ZEUInt32				DistortionMap : 1;
			ZEUInt32				AlphaCull : 1;
			ZEUInt32				VertexColor : 1;
			
		} Components;
		
		ZEMaterialOpacityComponent	OpacityComponent;
		ZEMaterialTransparancyMode	TransparancyMode;

		ZEVector3					AmbientColor;
		float						AmbientFactor;
		
		ZEVector3					SpecularColor;
		float						SpecularFactor;
		
		ZEVector3					DiffuseColor;
		float						DiffuseFactor;
		
		ZEVector3					EmissiveColor;
		float						EmissiveFactor;
		
		bool						TwoSided;
		bool						Wireframe;

		ZEVector3					MaterialAmbientColor;
		ZEVector3					MaterialSpecularColor;
		ZEVector3					MaterialEmissiveColor;
		ZEVector3					MaterialDiffuseColor;
		ZEVector2					DetailMapTiling;
		float						DetailDistance;
		float						DetailFadeDistance;
		float						Opacity;
		float						DistortionFactor;
		float						SpecularShininess;
		float						LightMapFactor;
		float						ReflectionFactor;
		float						RefractionFactor;
		float						RefractionIndex;
		float						SubSurfaceScatteringFactor;
		float						AlphaCullLimit;
		bool						SkinTransform;
		bool						ShadowReciver;

		struct ZEFixedMaterialTextures
		{
			ZETexture2D*			BaseMap;
			ZETexture2D*			NormalMap;
			ZETexture2D*			ParallaxMap;
			ZETexture2D*			SpecularMap;
			ZETexture2D*			EmissiveMap;
			ZETexture2D*			OpacityMap;
			ZETexture2D*			DetailBaseMap;
			ZETexture2D*			DetailNormalMap;
			ZETextureCube*			EnvironmentMap;
			ZETexture2D*			LightMap;
			ZETexture2D*			DistortionMap;

		} Textures;
			
		struct ZEFixedMaterialAdressingModes
		{
			ZETextureAddressMode	BaseMapU;
			ZETextureAddressMode	BaseMapV;
			ZETextureAddressMode	NormalMapU;
			ZETextureAddressMode	NormalMapV;
			ZETextureAddressMode	ParallaxMapU;
			ZETextureAddressMode	ParallaxMapV;
			ZETextureAddressMode	SpecularMapU;
			ZETextureAddressMode	SpecularMapV;
			ZETextureAddressMode	EmissiveMapU;
			ZETextureAddressMode	EmissiveMapV;
			ZETextureAddressMode	OpacityMapU;
			ZETextureAddressMode	OpacityMapV;
			ZETextureAddressMode	DetailBaseMapU;
			ZETextureAddressMode	DetailBaseMapV;
			ZETextureAddressMode	DetailNormalMapU;
			ZETextureAddressMode	DetailNormalMapV;
			ZETextureAddressMode	EnvironmentMapU;
			ZETextureAddressMode	EnvironmentMapV;
			ZETextureAddressMode	EnvironmentMapW;
			ZETextureAddressMode	LightMapU;
			ZETextureAddressMode	LightMapV;
			ZETextureAddressMode	DistortionMapU;
			ZETextureAddressMode	DistortionMapV;

		} AdressingModes;

		struct ZEFixedMaterialResources
		{
			ZETextureResource*		BaseMap;
			ZETextureResource*		NormalMap;
			ZETextureResource*		ParallaxMap;
			ZETextureResource*		SpecularMap;
			ZETextureResource*		EmissiveMap;
			ZETextureResource*		OpacityMap;
			ZETextureResource*		DetailBaseMap;
			ZETextureResource*		DetailNormalMap;
			ZETextureResource*		EnvironmentMap;
			ZETextureResource*		LightMap;
			ZETextureResource*		DistortionMap;

		} Resources;

		struct ZEFixedMaterialShaders
		{
			ZEShader*				GBufferPassVertexShader;
			ZEShader*				GBufferPassPixelShader;
			ZEShader*				ForwardPassVertexShader;
			ZEShader*				ForwardPassPixelShader;
			ZEShader*				ShadowPassVertexShader;
			ZEShader*				ShadowPassPixelShader;
		
		} Shaders;

		struct ZEFixedMaterialBuffers
		{
			ZEConstantBuffer*		GBufferPassVertexShader;
			ZEConstantBuffer*		GBufferPassPixelShader;
			ZEConstantBuffer*		ForwardPassVertexShader;
			ZEConstantBuffer*		ForwardPassPixelShader;
			ZEConstantBuffer*		ShadowPassVertexShader;
			ZEConstantBuffer*		ShadowPassPixelShader;

		}Buffers;

		void						UpdateTransformations(const ZERenderCommand* RenderCommand);

		bool						SetupGeometryPass(const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);
		bool						SetupForwardPass(const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);
		bool						SetupTransparentPass(const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);

		void						CreateShaders();
		void						DestroyShaders();

									ZEFixedMaterial();
		virtual						~ZEFixedMaterial();

	public:
		ZEUInt32					GetHash() const;

		virtual ZEMaterialFlags		GetMaterialFlags() const;

		// Material Options
		void						SetTwoSided(bool Enable);
		bool						GetTwoSided() const;

		void						SetWireframe(bool Enable);
		bool						GetWireframe() const;

		void						SetTransparancyMode(ZEMaterialTransparancyMode Mode);
		ZEMaterialTransparancyMode	GetTransparancyMode() const;

		void						SetAlphaCullEnabled(bool Enabled);
		bool						GetAlphaCullEnabled() const;

		void						SetAlphaCullLimit(float Limit);
		float						GetAlphaCullLimit() const;

		// Ambient
		void						SetAmbientEnabled(bool Value);
		bool						GetAmbientEnabled() const;
		void						SetAmbientFactor(float Factor);
		float						GetAmbientFactor() const;
		void						SetAmbientColor(const ZEVector3& Color);
		const ZEVector3&			GetAmbientColor() const;

		void						SetBaseMapEnabled(bool Value);
		bool						GetBaseMapEnabled() const;
		void						SetBaseMap(ZETexture2D* Texture);
		ZETexture2D*				GetBaseMap() const;
		void						SetBaseMapFile(const char* Filename);
		const char*					GetBaseMapFile() const;
		void						SetBaseMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetBaseMapAddressModeU() const;
		void						SetBaseMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetBaseMapAddressModeV() const;

		// Diffuse
		void						SetDiffuseEnabled(bool Value);
		bool						GetDiffuseEnabled() const;
		void						SetDiffuseColor(const ZEVector3& Color);
		const ZEVector3&			GetDiffuseColor() const;
		void						SetDiffuseFactor(float Factor);
		float						GetDiffuseFactor() const;
		void						SetDiffuseSubSurfaceScatteringFactor(float Factor);
		float						GetDiffuseSubSurfaceScatteringFactor();

		// Specular
		void						SetSpecularEnabled(bool Value);
		bool						GetSpecularEnabled() const;
		void						SetSpecularFactor(float Factor);
		float						GetSpecularFactor() const;
		void						SetSpecularShininess(float Shininess);
		float						GetSpecularShininess() const;
		void						SetSpecularColor(const ZEVector3& Color);
		const ZEVector3&			GetSpecularColor() const;
		void						SetSpecularMapEnabled(bool Value);
		bool						GetSpecularMapEnabled() const;
		void						SetSpecularMap(ZETexture2D* Texture);
		ZETexture2D*				GetSpecularMap() const;
		void						SetSpecularMapFile(const char* Filename);
		const char*					GetSpecularMapFile() const;
		void						SetSpecularMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetSpecularMapAddressModeU() const;
		void						SetSpecularMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetSpecularMapAddressModeV() const;

		// Emissive
		void						SetEmissiveEnabled(bool Value);
		bool						GetEmissiveEnabled() const;
		void						SetEmissiveFactor(float Factor);
		float						GetEmissiveFactor() const;
		void						SetEmissiveColor(const ZEVector3& Color);
		const ZEVector3&			GetEmissiveColor() const;
		void						SetEmissiveMapEnabled(bool Value);
		bool						GetEmissiveMapEnabled() const;
		void						SetEmissiveMap(ZETexture2D* Texture);
		ZETexture2D*				GetEmissiveMap() const;
		void						SetEmissiveMapFile(const char* Filename);
		const char*					GetEmissiveMapFile() const;
		void						SetEmissiveMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetEmissiveMapAddressModeU() const;
		void						SetEmissiveMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetEmissiveMapAddressModeV() const;

		// Normal Map
		void						SetNormalMapEnabled(bool Value);
		bool						GetNormalMapEnabled() const;
		void						SetNormalMap(ZETexture2D*	Texture);
		ZETexture2D*				GetNormalMap() const;
		void						SetNormalMapFile(const char* Filename);
		const char*					GetNormalMapFile() const;
		void						SetNormalMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetNormalMapAddressModeU() const;
		void						SetNormalMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetNormalMapAddressModeV() const;

		// ParallaxMap
		void						SetParallaxMapEnabled(bool Value);
		bool						GetParallaxMapEnabled() const;
		void						SetParallaxMap(ZETexture2D* Texture);
		ZETexture2D*				GetParallaxMap() const;
		void						SetParallaxMapFile(const char* Filename);
		const char*					GetParallaxMapFile() const;
		void						SetParallaxMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetParallaxMapAddressModeU() const;
		void						SetParallaxMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetParallaxMapAddressModeV() const;

		// Opacity
		void						SetOpacityEnabled(bool Value);
		bool						GetOpacityEnabled() const;
		void						SetOpacity(float Value);
		float						GetOpacity() const;
		void						SetOpacityComponent(ZEMaterialOpacityComponent Component);
		ZEMaterialOpacityComponent	GetOpacityComponent() const;
		void						SetOpacityMapEnabled(bool Value);
		bool						GetOpacityMapEnabled() const;
		void						SetOpacityMap(ZETexture2D* Texture);
		ZETexture2D*				GetOpacityMap() const;
		void						SetOpacityMapFile(const char* Filename);
		const char*					GetOpacityMapFile() const;
		void						SetOpacityMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetOpacityMapAddressModeU() const;
		void						SetOpacityMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetOpacityMapAddressModeV() const;

		// Environment Map
		void						SetEnvironmentMap(ZETextureCube* Texture);
		ZETextureCube*				GetEnvironmentMap() const;
		void						SetEnvironmentMapFile(const char* Filename);
		const char*					GetEnvironmentMapFile() const;
		void						SetEnvironmentMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetEnvironmentMapAddressModeU() const;
		void						SetEnvironmentMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetEnvironmentMapAddressModeV() const;
		void						SetEnvironmentMapAddressModeW(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetEnvironmentMapAddressModeW() const;

		// Reflection
		void						SetReflectionEnabled(bool Value);
		bool						GetReflectionEnabled() const;
		void						SetReflectionFactor(float Factor);
		float						GetReflectionFactor() const;

		// Refraction
		void						SetRefractionEnabled(bool Value);
		bool						GetRefractionEnabled() const;
		void						SetRefractionIndex(float Index);
		float						GetRefractionIndex() const;
		void						SetRefractionFactor(float Factor);
		float						GetRefractionFactor() const;

		// Detail Map
		void						SetDetailMapTiling(const ZEVector2& Tiling);
		const ZEVector2&			GetDetailMapTiling() const;
		void						SetDetailDistance(float Factor);
		float						GetDetailDistance() const;
		void						SetDetailFadeDistance(float Factor);
		float						GetDetailFadeDistance() const;
		void						SetDetailBaseMapEnabled(bool Value); 
		bool						GetDetailBaseMapEnabled() const;
		void						SetDetailBaseMap(ZETexture2D* Texture);
		ZETexture2D*				GetDetailBaseMap() const;
		void						SetDetailBaseMapFile(const char* Filename);
		const char*					GetDetailBaseMapFile() const;
		void						SetDetailBaseMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetDetailBaseMapAddressModeU() const;
		void						SetDetailBaseMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetDetailBaseMapAddressModeV() const;
		void						SetDetailNormalMapEnabled(bool Value); 
		bool						GetDetailNormalMapEnabled() const;
		void						SetDetailNormalMap(ZETexture2D* Texture);
		ZETexture2D*				GetDetailNormalMap() const;
		void						SetDetailNormalMapFile(const char* Filename);
		const char*					GetDetailNormalMapFile() const;
		void						SetDetailNormalMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetDetailNormalMapAddressModeU() const;
		void						SetDetailNormalMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetDetailNormalMapAddressModeV() const;

		// LightMap/Ambient Occlusion Map
		void						SetLightMapEnabled(bool Value);
		bool						GetLightMapEnabled() const;
		void						SetLightMapFactor(float Factor);
		float						GetLightMapFactor() const;
		void						SetLightMap(ZETexture2D* Texture);
		ZETexture2D*				GetLightMap() const;
		void						SetLightMapFile(const char* Filename);
		const char*					GetLightMapFile() const;
		void						SetLightMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetLightMapAddressModeU() const;
		void						SetLightMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetLightMapAddressModeV() const;

		// Distortion
		void						SetDistortionEnabled(bool Value);
		bool						GetDistortionEnabled() const;
		void						SetDistortionFactor(float Factor);
		float						GetDistortionFactor() const;
		void						SetDistortionMap(ZETexture2D* Texture);
		ZETexture2D*				GetDistortionMap() const;
		void						SetDistortionMapFile(const char* Filename);
		const char*					GetDistortionMapFile() const;
		void						SetDistortionMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetDistortionMapAddressModeU() const;
		void						SetDistortionMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode		GetDistortionMapAddressModeV() const;

		void						SetVertexColorEnabled(bool Value);
		bool						GetVertexColorEnabled();

		void						Tick(float ElapsedTime);

		void						WriteToFile(ZEFile* File);
		void						ReadFromFile(ZEFile* File);

		virtual bool				SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);

		virtual void				UpdateMaterial();

		static ZEFixedMaterial*		CreateInstance();
};

#endif
