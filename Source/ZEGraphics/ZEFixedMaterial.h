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
#include "ZEDS/ZEVariant.h"
#include "ZEMeta/ZEClass.h"

ZE_META_CLASS_DESCRIPTION(ZEFixedMaterial)

class ZETextureCube;
class ZETextureCubeResource;
class ZETexture2D;
class ZETexture2DResource;
class ZEFixedMaterial : public ZEMaterial
{
	ZE_META_CLASS(ZEFixedMaterial)
	protected:
		unsigned int					OldMaterialComponents;
		unsigned int					MaterialComponents;

		ZEMaterialOpacityComponent		OpacityComponent;

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

				float					SubSurfaceScatteringFactor;
				float					AlphaCullLimit;
				float					Reserved1;
				float					Reserved2;
			};
			float						PixelShaderConstants[24];
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
				bool					ShadowReciver;
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
			ZETexture2D*					Textures[12];
		};

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
		float							GetDiffuseSubSurfaceScatteringFactor();

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

		// Detail Map  

		void							SetDetailMapTiling(const ZEVector2& Tiling);
		const ZEVector2&				GetDetailMapTiling() const;
		void							SetDetailBaseMapEnabled(bool Enabled); 
		bool							GetDetailBaseMapEnabled() const;
		void							SetDetailBaseMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetDetailBaseMap() const;
		void							SetDetailBaseMapFile(const char* Filename);
		const char*						GetDetailBaseMapFile() const;
		void							SetDetailBaseMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDetailBaseMapAddressModeU() const;
		void							SetDetailBaseMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDetailBaseMapAddressModeV() const;
		void							SetDetailNormalMapEnabled(bool Enabled); 
		bool							GetDetailNormalMapEnabled() const;
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

		virtual void					SetZero();

		void							Tick(float ElapsedTime);

		static ZEFixedMaterial*			CreateInstance();
};

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZEFixedMaterial" noinstance="true">
			<description>Material System</description>
			<property name="TwoSided" groupname="Shading" type="boolean" autogetset="yes"/>
			<property name="Wireframe" groupname="Shading" type="boolean" autogetset="yes"/>
			<property name="AlphaCullEnabled" groupname="Transparancy" type="boolean" autogetset="yes"/>
			<property name="AlphaCullLimit" groupname="Transparancy" type="integer" autogetset="yes"/>
			<property name="TransparancyMode" groupname="Transparancy" type="integer" autogetset="yes">
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
			<property name="BaseMapAddressModeU" groupname="Diffuse" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
					<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<property name="BaseMapAddressModeV" groupname="Diffuse" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
					<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>

			<property name="SpecularEnabled" groupname="Specular" type="boolean" autogetset="yes"/>
			<property name="SpecularFactor" groupname="Specular" type="float" autogetset="yes"/>
			<property name="SpecularShininess" groupname="Specular" type="float" autogetset="yes"/>
			<property name="SpecularColor" groupname="Specular" semantic="ZE_PS_COLOR" type="ZEVector3" autogetset="yes"/>
			<property name="SpecularMap" groupname="Specular" type="string" semantic="ZE_PS_FILENAME" setfunction="SetSpecularMapFile" getfunction="GetSpecularMapFile"/>
			<property name="SpecularMapAddressModeU" groupname="Specular" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
					<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<property name="SpecularMapAddressModeV" groupname="Specular" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
					<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>


			<property name="EmmisiveEnabled" groupname="Emmisive" type="boolean" autogetset="yes"/>
			<property name="EmmisiveFactor" groupname="Emmisive" type="float" autogetset="yes"/>
			<property name="EmmisiveColor" groupname="Emmisive" semantic="ZE_PS_COLOR" type="ZEVector3" autogetset="yes"/>
			<property name="EmmisiveMap" groupname="Emmisive" type="string" semantic="ZE_PS_FILENAME" setfunction="SetEmmisiveMapFile" getfunction="GetEmmisiveMapFile"/>
			<property name="EmmisiveMapAddressModeU" groupname="Emmisive" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
					<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<property name="EmmisiveMapAddressModeV" groupname="Emmisive" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
					<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>


			<property name="NormalMapEnabled" groupname="Normal" type="boolean" autogetset="yes"/>
			<property name="NormalMap" groupname="Normal" type="string" semantic="ZE_PS_FILENAME" setfunction="SetNormalMapFile" getfunction="GetNormalMapFile"/>
			<property name="NormalMapAddressModeU" groupname="Normal" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
					<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<property name="NormalMapAddressModeV" groupname="Normal" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
					<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>


			<property name="ParallaxMapEnabled" groupname="Parallax" type="boolean" autogetset="yes"/>
			<property name="ParallaxMap" groupname="Parallax" type="string" semantic="ZE_PS_FILENAME" setfunction="SetParallaxMapFile" getfunction="GetParallaxMapFile"/>
			<property name="ParallaxMapAddressModeU" groupname="Parallax" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
					<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property> 
			<property name="ParallaxMapAddressModeV" groupname="Parallax" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
					<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>


			<property name="OpacityEnabled" groupname="Opacity" type="boolean" autogetset="yes"/>
			<property name="Opacity" groupname="Opacity" type="float" autogetset="yes"/>
			<property name="OpacityComponent" groupname="Opacity" type="integer" autogetset="yes">
				<enumurator name = "ZEMaterialOpacityComponent">
					<item name="Constant" value="ZE_MOC_CONSTANT"/>
					<item name="Diffuse Map Alpha" value="ZE_MOC_BASE_MAP_ALPHA"/>
					<item name="Opacity Map" value="ZE_MOC_OPACITY_MAP"/>
				</enumurator>
			</property>
			<property name="OpacityMap" groupname="Opacity" type="string" semantic="ZE_PS_FILENAME" setfunction="SetOpacityMapFile" getfunction="GetOpacityMapFile"/>
			<property name="OpacityMapAddressModeU" groupname="Opacity" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
					<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>  
			<property name="OpacityMapAddressModeV" groupname="Opacity" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>

			<property name="EnvironmentMap" groupname="Environment" type="string" semantic="ZE_PS_FILENAME" getfunction="GetEnvironmentMapFile" setfunction="SetEnvironmentMapFile"/>
			<property name = "EnvironmentMapAddressModeU" groupname="Environment" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<property name = "EnvironmentMapAddressModeV" groupname="Environment" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<property name = "EnvironmentMapAddressModeW" groupname="Environment" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<property name = "ReflectionEnabled" groupname="Environment" type="boolean" autogetset="yes" />
			<property name = "ReflectionFactor" groupname="Environment" type="float" autogetset="yes" />
			<property name = "RefractionEnabled" groupname="Environment" type="boolean" autogetset="yes" />
			<property name = "RefractionFactor" groupname="Environment" type="float" autogetset="yes" />
			<property name = "RefractionIndex" groupname="Environment" type="float" autogetset="yes" />

			<property name="DetailMapTiling" groupname="Detail" type="ZEVector2" autogetset="yes" />
			<property name="DetailBaseMap" groupname="Detail" type="string" semantic="ZE_PS_FILENAME" getfunction="GetDetailBaseMapFile" setfunction="SetDetailBaseMapFile" />
			<property name="DetailBaseMapAddressModeU" groupname="Detail" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<property name="DetailBaseMapAddressModeV" groupname="Detail" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<property name="DetailNormalMap" groupname="Detail" type="string" semantic="ZE_PS_FILENAME" getfunction="GetDetailNormalMapFile" setfunction="SetDetailNormalMapFile" />
			<property name="DetailNormalMapAddressModeU" groupname="Detail" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<property name="DetailNormalMapAddressModeV" groupname="Detail" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>


			<property name="LightMapEnabled" groupname="Light Map" type="boolean" autogetset="yes" />
			<property name="LightMapFactor" groupname="Light Map" type="float" autogetset="yes" />
			<property name="LightMap" groupname="Light Map" type="string" semantic="ZE_PS_FILENAME" getfunction="GetLightMapFile" setfunction="SetLightMapFile" />
			<property name="LightMapAddressModeU" groupname="Light Map" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<property name="LightMapAddressModeU" groupname="Light Map" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>


			<property name="DistortionEnabled" groupname="Distortion" type="boolean" autogetset="yes" />
			<property name="DistortionFactor" groupname="Distortion" type="float" autogetset="yes" />
			<property name="DistortionMap" groupname="Distortion" type="string" semantic="ZE_PS_FILENAME" getfunction="GetDistortionMapFile" setfunction="SetDistortionMapFile" />
			<property name="DistortionMapAddressModeU" groupname="Distortion" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<property name="DistortionMapAddressModeU" groupname="Distortion" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/
#endif




