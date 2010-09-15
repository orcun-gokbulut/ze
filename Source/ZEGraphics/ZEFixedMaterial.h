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
#include "ZEMath\ZEVector.h"
#include "ZEDefinitions.h"
#include "ZEDS\ZEArray.h"
#include "ZEDS\ZEVariant.h"
#include "ZEMeta\ZEClass.h"

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

ZE_META_CLASS_DESCRIPTION(ZEFixedMaterial)

class ZETextureCube;
class ZETexture2D;
class ZEFixedMaterial : public ZEMaterial
{
	ZE_META_CLASS()
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
		bool							GetTwoSided() const;
		void							SetLightningEnabled(bool Enable);
		bool							GetLightningEnabled() const;
		void							SetWireframe(bool Enable);
		bool							GetWireframe() const;
		void							SetTransparancyMode(ZEMaterialTransparancyMode Mode);
		ZEMaterialTransparancyMode		GetTransparancyMode() const;
		void							SetTransparancyCullLimit(unsigned int Limit);
		unsigned int					GetTransparancyCullLimit() const;
		void							SetRecivesShadow(bool Enable);
		bool							GetRecivesShadow() const;

		// Ambient
		void							SetAmbientEnabled(bool Enabled);
		bool							GetAmbientEnabled() const;
		void							SetAmbientFactor(float Factor);
		float							GetAmbientFactor() const;
		void							SetAmbientColor(const ZEVector3& Color);
		const ZEVector3&				GetAmbientColor() const;

		// Diffuse
		void							SetDiffuseEnabled(bool Enabled);
		bool							GetDiffuseEnabled() const;
		void							SetDiffuseColor(const ZEVector3& Color);
		const ZEVector3&				GetDiffuseColor() const;
		void							SetDiffuseFactor(float Factor);
		float							GetDiffuseFactor() const;
		void							SetDiffuseMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetDiffuseMap() const;
		void							SetDiffuseMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDiffuseMapAddressModeU() const;
		void							SetDiffuseMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDiffuseMapAddressModeV() const;

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
		void							SetEmmisiveMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetEmmisiveMapAddressModeU() const;
		void							SetEmmisiveMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetEmmisiveMapAddressModeV() const;

		// Normal Map
		void							SetNormalMapEnabled(bool Enabled);
		bool							GetNormalMapEnabled() const;
		void							SetNormalMap(const ZETexture2D*	Texture);
		const ZETexture2D*				GetNormalMap() const;
		void							SetNormalMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetNormalMapAddressModeU() const;
		void							SetNormalMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetNormalMapAddressModeV() const;

		// ParallaxMap
		void							SetParallaxMapEnabled(bool Enabled);
		bool							GetParallaxMapEnabled() const;
		void							SetParallaxMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetParallaxMap() const;
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
		void							SetOpacityMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetOpacityMapAddressModeU() const;
		void							SetOpacityMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetOpacityMapAddressModeV() const;

		// Reflection
		void							SetReflectionEnabled(bool Enabled);
		bool							GetReflectionEnabled() const;
		void							SetReflectionFactor(float Factor);
		float							GetReflectionFactor() const;
		void							SetReflectionMap(const ZETextureCube* Texture);
		const ZETextureCube*			GetReflectionMap() const;
		void							SetReflectionMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetReflectionMapAddressModeU() const;
		void							SetReflectionMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetReflectionMapAddressModeV() const;
		void							SetReflectionMapAddressModeW(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetReflectionMapAddressModeW() const;

		// Refraction
		void							SetRefractionEnabled(bool Enabled);
		bool							GetRefractionEnabled() const;
		void							SetRefractionIndex(float Index);
		float							GetRefractionIndex() const;
		void							SetRefractionFactor(float Factor);
		float							GetRefractionFactor() const;
		void							SetRefractionMap(const ZETextureCube* Texture);
		const ZETextureCube*			GetRefractionMap() const;
		void							SetRefractionMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetRefractionMapAddressModeU() const;
		void							SetRefractionMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetRefractionMapAddressModeV() const;
		void							SetRefractionMapAddressModeW(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetRefractionMapAddressModeW() const;
		// Detail Map  
		void							SetDetailMapEnabled(bool Enabled); 
		bool							GetDetailMapEnabled() const;
		void							SetDetailMapTiling(const ZEVector2& Tiling);
		const ZEVector2&				GetDetailMapTiling() const;
		void							SetDetailDiffuseMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetDetailDiffuseMap() const;
		void							SetDetailDiffuseMapAddressModeU(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDetailDiffuseMapAddressModeU() const;
		void							SetDetailDiffuseMapAddressModeV(ZETextureAddressMode Mode);
		ZETextureAddressMode			GetDetailDiffuseMapAddressModeV() const;
		void							SetDetailNormalMap(const ZETexture2D* Texture);
		const ZETexture2D*				GetDetailNormalMap() const;
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
			<property name="LightningEnabled" groupname="Shading" type="boolean" autogetset="yes"/>
			<property name="RecivesShadow" groupname="Shading" type="boolean" autogetset="yes"/>
			<property name="Wireframe" groupname="Shading" type="boolean" autogetset="yes"/>
			<property name="TransparancyCullLimit" groupname="Transparancy" type="integer" autogetset="yes"/>
			<property name="TransparancyMode" groupname="Transparancy" type="integer" autogetset="yes">
				<enumurator name="ZEMaterialTransparancyMode">
					<item name="No Transparancy" value="ZE_MTM_NOTRANSPARACY"/>
					<item name="Alpha Test" value="ZE_MTM_ALPHACULL"/>
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
			<!--<property name="DiffuseMap" groupname="Diffuse" type="string" autogetset="yes"/>-->


			<property name="SpecularEnabled" groupname="Specular" type="boolean" autogetset="yes"/>
			<property name="SpecularFactor" groupname="Specular" type="float" autogetset="yes"/>
			<property name="SpecularShininess" groupname="Specular" type="float" autogetset="yes"/>
			<property name="SpecularColor" groupname="Specular" semantic="ZE_PS_COLOR" type="ZEVector3" autogetset="yes"/>
			<!--<property name="SpecularMap" groupname="Specular "type="string" autogetset="yes"/>-->
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
			<!--<property name="EmmisiveMap" groupname="Emmisive" type="string" autogetset="yes"/>-->
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
			<!--<property name="NormalMap" groupname="Normal" type="string" autogetset="yes"/>-->
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
			<!--<property name="ParallaxMap" groupname="Parallax" type="string" autogetset="yes"/>-->
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
					<item name="Diffuse Map Alpha" value="ZE_MOC_DIFFUSE_ALPHA"/>
					<item name="Opacity Map" value="ZE_MOC_OPACITYMAP"/>
				</enumurator>
			</property>
			<!--<property name="OpacityMap" groupname="Opacity" type="string" autogetset="yes"/>-->
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


			<property name = "ReflectionEnabled" groupname="Reflection" type="boolean" autogetset="yes" />
			<property name = "ReflectionFactor" groupname="Reflection" type="float" autogetset="yes" />
			<!--<property name="ReflectionMap" groupname="Reflection" type="string" autogetset="yes"/>-->
			<property name = "ReflectionMapAddressModeU" groupname="Reflection" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<property name = "ReflectionMapAddressModeV" groupname="Reflection" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<property name = "ReflectionMapAddressModeW" groupname="Reflection" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>


			<property name = "RefractionEnabled" groupname="Refraction" type="boolean" autogetset="yes" />
			<property name = "RefractionFactor" groupname="Refraction" type="float" autogetset="yes" />
			<property name = "RefractionIndex" groupname="Refraction" type="float" autogetset="yes" />
			<!--<property name="RefractionMap" groupname="Refraction" type="string" autogetset="yes"/>-->
			<property name = "RefractionMapAddressModeU" groupname="Refraction" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<property name = "RefractionMapAddressModeV" groupname="Refraction" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<property name = "RefractionMapAddressModeW" groupname="Refraction" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>

			<property name="DetailMapEnabled" groupname="Detail" type="boolean" autogetset="yes" />
			<property name="DetailMapTiling" groupname="Detail" type="ZEVector2" autogetset="yes" />
			<!--<property name="DetailDiffuseMap" groupname="Detail" type="string" autogetset="yes" />-->
			<property name="DetailDiffuseMapAddressModeU" groupname="Detail" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<property name="DetailDiffuseMapAddressModeV" groupname="Detail" type="integer" autogetset="yes">
				<enumurator name="ZETextureAddressMode">
					<item name="Wrap" value="ZE_TAM_WRAP"/>
					<item name="Clamp" value="ZE_TAM_CLAMP"/>
			 		<item name="Mirror" value="ZE_TAM_MIRROR"/>
					<item name="Border" value="ZE_TAM_BORDER"/>
				</enumurator>
			</property>
			<!--<property name="DetailNormalMap" groupname="Detail" type="string" autogetset="yes" />-->
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
			<!--<property name="LightMap" groupname="Light Map" type="string" autogetset="yes" />-->
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
			<!--<property name="DistortionMap" groupname="Distortion" type="string" autogetset="yes" />-->
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



