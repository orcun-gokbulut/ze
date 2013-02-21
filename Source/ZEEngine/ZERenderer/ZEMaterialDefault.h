//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterialDefault.h
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
#include "ZEMath/ZEMatrix.h"
#include "ZEMeta/ZEObject.h"
#include "ZEGraphics/ZESamplerState.h"

class ZEFile;
class ZEShader;
class ZETexture;
class ZETexture2D;
class ZETextureCube;
class ZEConstantBuffer;
class ZETextureResource;
class ZETexture2DResource;
class ZETextureCubeResource;

ZE_META_OBJECT_DESCRIPTION(ZEMaterialDefault)

class ZEMaterialDefault : public ZEMaterial
{
	ZE_META_OBJECT(ZEMaterialDefault)

	protected:
		bool						Recompile;

		struct 
		{
			ZEMatrix4x4				WorldView;
			ZEMatrix4x4				WorldViewProj;

		} Transformations;

		struct ZEMaterialDefaultComponents
		{
			ZEUInt64				Ambient			: 1;
			ZEUInt64				Diffuse			: 1;
			ZEUInt64				BaseMap			: 1;
			ZEUInt64				NormalMap		: 1;
			ZEUInt64				ParallaxMap		: 1;
			ZEUInt64				Specular		: 1;
			ZEUInt64				SpecularMap		: 1;
			ZEUInt64				Emissive		: 1;
			ZEUInt64				EmissiveMap		: 1;
			ZEUInt64				Opacity			: 1;
			ZEUInt64				OpacityBaseAlpha : 1;
			ZEUInt64				OpacityConstant : 1;
			ZEUInt64				OpacityMap		: 1;
			ZEUInt64				DetailMap		: 1;
			ZEUInt64				DetailBaseMap	: 1;
			ZEUInt64				DetailNormalMap : 1;
			ZEUInt64				Reflection		: 1;
			ZEUInt64				Refraction		: 1;
			ZEUInt64				LightMap		: 1;
			ZEUInt64				DistortionMap	: 1;
			ZEUInt64				AlphaCull		: 1;
			ZEUInt64				VertexColor		: 1;
			
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
			

		struct ZEFixedMaterialSamplers
		{
			ZESamplerState			BaseMap;
			ZESamplerState			NormalMap;
			ZESamplerState			ParallaxMap;
			ZESamplerState			SpecularMap;
			ZESamplerState			EmissiveMap;
			ZESamplerState			OpacityMap;
			ZESamplerState			DetailBaseMap;
			ZESamplerState			DetailNormalMap;
			ZESamplerState			EnvironmentMap;
			ZESamplerState			LightMap;
			ZESamplerState			DistortionMap;

		} Samplers;

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

		bool						SetupShadowPass(const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);
		bool						SetupGeometryPass(const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);
		bool						SetupForwardPass(const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);
		bool						SetupTransparentPass(const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);

		void						UpdateShaders();
		void						DestroyShaders();

									ZEMaterialDefault();
		virtual						~ZEMaterialDefault();

	public:
		ZESize						GetHash() const;

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
		void						SetBaseMapSampler(const ZESamplerState& Sampler);
		const ZESamplerState&		GetBaseMapSampler() const;

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
		void						SetSpecularMapSampler(const ZESamplerState& Sampler);
		const ZESamplerState&		GetSpecularMapSampler() const;

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
		void						SetEmissiveMapSampler(const ZESamplerState& Sampler);
		const ZESamplerState&		GetEmissiveMapSampler() const;

		// Normal Map
		void						SetNormalMapEnabled(bool Value);
		bool						GetNormalMapEnabled() const;
		void						SetNormalMap(ZETexture2D*	Texture);
		ZETexture2D*				GetNormalMap() const;
		void						SetNormalMapFile(const char* Filename);
		const char*					GetNormalMapFile() const;
		void						SetNormalMapSampler(const ZESamplerState& Sampler);
		const ZESamplerState&		GetNormalMapSampler() const;

		// ParallaxMap
		void						SetParallaxMapEnabled(bool Value);
		bool						GetParallaxMapEnabled() const;
		void						SetParallaxMap(ZETexture2D* Texture);
		ZETexture2D*				GetParallaxMap() const;
		void						SetParallaxMapFile(const char* Filename);
		const char*					GetParallaxMapFile() const;
		void						SetParallaxMapSampler(const ZESamplerState& Sampler);
		const ZESamplerState&		GetParallaxMapSampler() const;

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
		void						SetOpacityMapSampler(const ZESamplerState& Sampler);
		const ZESamplerState&		GetOpacityMapSampler() const;

		// Environment Map
		void						SetEnvironmentMap(ZETextureCube* Texture);
		ZETextureCube*				GetEnvironmentMap() const;
		void						SetEnvironmentMapFile(const char* Filename);
		const char*					GetEnvironmentMapFile() const;
		void						SetEnvironmentMapSampler(const ZESamplerState& Sampler);
		const ZESamplerState&		GetEnvironmentMapSampler() const;

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
		void						SetDetailBaseMapSampler(const ZESamplerState& Sampler);
		const ZESamplerState&		GetDetailBaseMapSampler() const;
		void						SetDetailNormalMapEnabled(bool Value); 
		bool						GetDetailNormalMapEnabled() const;
		void						SetDetailNormalMap(ZETexture2D* Texture);
		ZETexture2D*				GetDetailNormalMap() const;
		void						SetDetailNormalMapFile(const char* Filename);
		const char*					GetDetailNormalMapFile() const;
		void						SetDetailNormalMapSampler(const ZESamplerState& Sampler);
		const ZESamplerState&		GetDetailNormalMapSampler() const;

		// LightMap/Ambient Occlusion Map
		void						SetLightMapEnabled(bool Value);
		bool						GetLightMapEnabled() const;
		void						SetLightMapFactor(float Factor);
		float						GetLightMapFactor() const;
		void						SetLightMap(ZETexture2D* Texture);
		ZETexture2D*				GetLightMap() const;
		void						SetLightMapFile(const char* Filename);
		const char*					GetLightMapFile() const;
		void						SetLightMapSampler(const ZESamplerState& Sampler);
		const ZESamplerState&		GetLightMapSampler() const;

		// Distortion
		void						SetDistortionEnabled(bool Value);
		bool						GetDistortionEnabled() const;
		void						SetDistortionFactor(float Factor);
		float						GetDistortionFactor() const;
		void						SetDistortionMap(ZETexture2D* Texture);
		ZETexture2D*				GetDistortionMap() const;
		void						SetDistortionMapFile(const char* Filename);
		const char*					GetDistortionMapFile() const;
		void						SetDistortionMapSampler(const ZESamplerState& Sampler);
		const ZESamplerState&		GetDistortionMapSampler() const;

		void						SetVertexColorEnabled(bool Value);
		bool						GetVertexColorEnabled();

		void						WriteToFile(const ZEString& FilePath);
		void						ReadFromFile(const ZEString& FilePath);

		virtual void				UpdateMaterial();

		virtual void				Tick(float ElapsedTime);
		virtual bool				SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);

		static ZEMaterialDefault*	CreateInstance();
};

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZEMaterialDefault" noinstance="true">
			<description>Material System</description>
			<property name="TwoSided" groupname="Shading" type="boolean" autogetset="yes"/>
			<property name="Wireframe" groupname="Shading" type="boolean" autogetset="yes"/>
			<property name="AlphaCullEnabled" groupname="Transparancy" type="boolean" autogetset="yes"/>
			<property name="AlphaCullLimit" groupname="Transparancy" type="float" autogetset="yes"/>
			<property name="TransparancyMode" groupname="Transparancy" type="integer32" autogetset="yes">
				<enumurator name="ZEMaterialTransparancyMode">
					<item name="No Transparancy" value="ZE_MTM_NONE"/>
					<item name="Regular" value="ZE_MTM_REGULAR"/>
					<item name="Adaptive" value="ZE_MTM_ADDAPTIVE"/>
					<item name="Subtractive" value="ZE_MTM_SUBTRACTIVE"/>
				</enumurator>
			</property>		

			<property name="AmbientEnabled" groupname="Ambient" type="boolean" autogetset="yes"/>
			<property name="AmbientFactor" groupname="Ambient" type="float" autogetset="yes"/>
			<property name="AmbientColor" groupname="Ambient" semantic="ZE_PS_COLOR" type="ZEVector3" autogetset="yes"/>
			  

			<property name="DiffuseEnabled" groupname="Diffuse" type="boolean" autogetset="yes"/>
			<property name="DiffuseColor" groupname="Diffuse" semantic="ZE_PS_COLOR" type="ZEVector3" autogetset="yes"/>
			<property name="DiffuseFactor" groupname="Diffuse" type="float" autogetset="yes"/>
			<property name="BaseMap" groupname="Diffuse" type="string" semantic="ZE_PS_FILENAME" setfunction="SetBaseMapFile" getfunction="GetBaseMapFile"/>

			<property name="SpecularEnabled" groupname="Specular" type="boolean" autogetset="yes"/>
			<property name="SpecularFactor" groupname="Specular" type="float" autogetset="yes"/>
			<property name="SpecularShininess" groupname="Specular" type="float" autogetset="yes"/>
			<property name="SpecularColor" groupname="Specular" semantic="ZE_PS_COLOR" type="ZEVector3" autogetset="yes"/>
			<property name="SpecularMap" groupname="Specular" type="string" semantic="ZE_PS_FILENAME" setfunction="SetSpecularMapFile" getfunction="GetSpecularMapFile"/>


			<property name="EmissiveEnabled" groupname="Emissive" type="boolean" autogetset="yes"/>
			<property name="EmissiveFactor" groupname="Emissive" type="float" autogetset="yes"/>
			<property name="EmissiveColor" groupname="Emissive" semantic="ZE_PS_COLOR" type="ZEVector3" autogetset="yes"/>
			<property name="EmissiveMap" groupname="Emissive" type="string" semantic="ZE_PS_FILENAME" setfunction="SetEmissiveMapFile" getfunction="GetEmissiveMapFile"/>


			<property name="NormalMapEnabled" groupname="Normal" type="boolean" autogetset="yes"/>
			<property name="NormalMap" groupname="Normal" type="string" semantic="ZE_PS_FILENAME" setfunction="SetNormalMapFile" getfunction="GetNormalMapFile"/>


			<property name="ParallaxMapEnabled" groupname="Parallax" type="boolean" autogetset="yes"/>
			<property name="ParallaxMap" groupname="Parallax" type="string" semantic="ZE_PS_FILENAME" setfunction="SetParallaxMapFile" getfunction="GetParallaxMapFile"/>


			<property name="OpacityEnabled" groupname="Opacity" type="boolean" autogetset="yes"/>
			<property name="Opacity" groupname="Opacity" type="float" autogetset="yes"/>
			<property name="OpacityComponent" groupname="Opacity" type="integer32" autogetset="yes">
				<enumurator name = "ZEMaterialOpacityComponent">
					<item name="Constant" value="ZE_MOC_CONSTANT"/>
					<item name="Diffuse Map Alpha" value="ZE_MOC_BASE_MAP_ALPHA"/>
					<item name="Opacity Map" value="ZE_MOC_OPACITY_MAP"/>
				</enumurator>
			</property>
			<property name="OpacityMap" groupname="Opacity" type="string" semantic="ZE_PS_FILENAME" setfunction="SetOpacityMapFile" getfunction="GetOpacityMapFile"/>


			<property name="EnvironmentMap" groupname="Environment" type="string" semantic="ZE_PS_FILENAME" getfunction="GetEnvironmentMapFile" setfunction="SetEnvironmentMapFile"/>

			<property name = "ReflectionEnabled" groupname="Reflection" type="boolean" autogetset="yes" />
			<property name = "ReflectionFactor" groupname="Reflection" type="float" autogetset="yes" />
			<property name = "RefractionEnabled" groupname="Refraction" type="boolean" autogetset="yes" />
			<property name = "RefractionFactor" groupname="Refraction" type="float" autogetset="yes" />
			<property name = "RefractionIndex" groupname="Refraction" type="float" autogetset="yes" />

			<property name="DetailMapTiling" groupname="Detail" type="ZEVector2" autogetset="yes" />
			<property name="DetailBaseMap" groupname="Detail" type="string" semantic="ZE_PS_FILENAME" getfunction="GetDetailBaseMapFile" setfunction="SetDetailBaseMapFile" />			
			<property name="DetailNormalMap" groupname="Detail" type="string" semantic="ZE_PS_FILENAME" getfunction="GetDetailNormalMapFile" setfunction="SetDetailNormalMapFile" />

			<property name="LightMapEnabled" groupname="Light Map" type="boolean" autogetset="yes" />
			<property name="LightMapFactor" groupname="Light Map" type="float" autogetset="yes" />
			<property name="LightMap" groupname="Light Map" type="string" semantic="ZE_PS_FILENAME" getfunction="GetLightMapFile" setfunction="SetLightMapFile" />

			<property name="DistortionEnabled" groupname="Distortion" type="boolean" autogetset="yes" />
			<property name="DistortionFactor" groupname="Distortion" type="float" autogetset="yes" />
			<property name="DistortionMap" groupname="Distortion" type="string" semantic="ZE_PS_FILENAME" getfunction="GetDistortionMapFile" setfunction="SetDistortionMapFile" />
			
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/

#endif
