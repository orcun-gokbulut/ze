//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNFixedMaterial.h
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

#include "ZERNMaterial.h"

#include "ZEDS/ZEString.h"
#include "ZEMath/ZEVector.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEResource/ZERSHolder.h"

enum ZEGRFormat;
class ZEGRRenderStateData;
class ZEGRConstantBuffer;
class ZEGRShader;
class ZEGRSampler;
class ZEGRTexture2D;
class ZEMLReaderNode;
class ZEGRShaderCompileOptions;

enum ZERNHeightMapTechnique : ZEUInt8
{
	ZERN_HMT_NONE,
	ZERN_HMT_PARALLAX,
	ZERN_HMT_PARALLAX_OCCLUSION,
	ZERN_HMT_VERTEX_DISPLACEMENT,
	ZERN_HMT_TESSELLATION
};

enum ZERNTransparencyMode : ZEUInt8
{
	ZERN_TM_NONE,
	ZERN_TM_ORDERED,
	ZERN_TM_ADDITIVE
};

class ZERNFixedMaterial : public ZERNMaterial
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZERNFixedMaterial)
	friend class ZERSTemplates;
	private:
		ZEString									Name;

		ZEUInt										StageMask;
		ZEFlags										DirtyFlags;
		
		ZEHolder<ZEGRShader>						StageGBuffer_Forward_VertexShader;
		ZEHolder<ZEGRShader>						StageGBuffer_Forward_PixelShader;
		ZEHolder<ZEGRRenderStateData>				StageGBuffer_Forward_RenderState;
		
		ZEHolder<ZEGRShader>						StageShadowmapGeneration_VertexShader;
		ZEHolder<ZEGRShader>						StageShadowmapGeneration_PixelShader;
		ZEHolder<ZEGRRenderStateData>				StageShadowmapGeneration_RenderState;

		ZEHolder<ZEGRConstantBuffer>				ConstantBuffer;
		ZEHolder<ZEGRSampler>						Sampler;

		ZEHolder<const ZEGRTexture2D>				BaseMap;
		ZEHolder<const ZEGRTexture2D>				SpecularMap;
		ZEHolder<const ZEGRTexture2D>				SpecularGlossMap;
		ZEHolder<const ZEGRTexture2D>				EmissiveMap;
		ZEHolder<const ZEGRTexture2D>				NormalMap;
		ZEHolder<const ZEGRTexture2D>				HeightMap;
		ZEHolder<const ZEGRTexture2D>				OpacityMap;
		ZEHolder<const ZEGRTexture2D>				SubSurfaceScatteringMap;
		ZEHolder<const ZEGRTexture2D>				EnvironmentMap;
		ZEHolder<const ZEGRTexture2D>				DetailBaseMap;
		ZEHolder<const ZEGRTexture2D>				DetailNormalMap;
		
		mutable struct
		{
			ZEVector3								AmbientColor;
			float									Opacity;
			
			ZEVector3								DiffuseColor;
			float									SubSurfaceScatteringFactor;
			
			ZEVector3								SpecularColor;
			float									SpecularPower;
			
			ZEUInt32								HeightMapTechnique;
			float									HeightMapScale;
			float									HeightMapOffset;
			float									Reserved0;
			
			ZEVector3								EmissiveColor;
			float									AlphaCullLimit;
			
			ZEVector3								ReflectionColor;
			ZEBool32								SceneAmbientEnabled;
			
			ZEVector3								RefractionColor;
			float									RefractionIndex;
			
			ZEVector3								DetailBaseMapColor;
			float									DetailNormalMapFactor;
			
			ZEVector2								DetailBaseMapTiling;
			ZEVector2								DetailNormalMapTiling;
			
			float									DetailBaseMapAttenuationStart;
			float									DetailBaseMapAttenuationFactor;
			float									DetailNormalMapAttenuationStart;
			float									DetailNormalMapAttenuationFactor;
		} Constants;

		bool										ShadowCaster;
		bool										TwoSided;
		bool										Wireframe;
		bool										AlphaCullEnabled;
		bool										SkinningEnabled;
		bool										VertexColorEnabled;
		bool										TransparencyEnabled;
		ZERNTransparencyMode						TransparencyMode;
		bool										BaseMapEnabled;
		ZEString									BaseMapFile;
		bool										AmbientEnabled;
		float										AmbientFactor;
		ZEVector3									AmbientColor;
		bool										DiffuseEnabled;
		float										DiffuseFactor;
		ZEVector3									DiffuseColor;
		bool										SpecularEnabled;
		bool										SpecularMapEnabled;
		ZEString									SpecularMapFile;
		bool										SpecularGlossMapEnabled;
		ZEString									SpecularGlossMapFile;
		float										SpecularFactor;
		ZEVector3									SpecularColor;
		bool										NormalMapEnabled;
		ZEString									NormalMapFile;
		bool										HeightMapEnabled;
		ZEString									HeightMapFile;
		ZERNHeightMapTechnique						HeightMapTechnique;
		bool										EmissiveEnabled;
		float										EmissiveFactor;
		ZEVector3									EmissiveColor; 
		bool										EmissiveMapEnabled;
		ZEString									EmissiveMapFile;
		bool										SubSurfaceScatteringMapEnabled;
		ZEString									SubSurfaceScatteringMapFile;
		bool										ReflectionEnabled;
		float										ReflectionFactor;
		ZEVector3									ReflectionColor;
		bool										RefractionEnabled;
		float										RefractionFactor;
		ZEVector3									RefractionColor;
		bool										OpacityMapEnabled;
		ZEString									OpacityMapFile;
		bool										EnvironmentMapEnabled;
		ZEString									EnvironmentMapFile;
		bool										DetailBaseMapEnabled;
		ZEString									DetailBaseMapFile;
		bool										DetailNormalMapEnabled;
		ZEString									DetailNormalMapFile;
		bool										ClippingPlanesEnabled;
		
		void										UpdateShaderDefinitions(ZEGRShaderCompileOptions& Options) const;
		bool										UpdateShaders();
		bool										UpdateConstantBuffer();
		bool										UpdateRenderState();
		bool										UpdateStageMask();

		virtual ZETaskResult						LoadInternal();
		virtual ZETaskResult						UnloadInternal();

													ZERNFixedMaterial();

	public:
		virtual ZEUInt								GetStageMask() const;

		void										SetName(const ZEString& Name);
		const ZEString&								GetName() const;

		void										SetSampler(const ZEHolder<ZEGRSampler>& Sampler);
		const ZEHolder<ZEGRSampler>&				GetSampler() const;

		void										SetShadowCaster(bool ShadowCaster);
		bool										GetShadowCaster() const;

		void										SetTwoSided(bool Enable);
		bool										GetTwoSided() const;

		void										SetWireframe(bool Enable);
		bool										GetWireframe() const;

		void										SetSkinningEnabled(bool SkinningEnabled);
		bool										GetSkinningEnabled() const;

		void										SetVertexColorEnabled(bool Enabled);
		bool										GetVertexColorEnabled() const;

		void										SetTransparencyEnabled(bool Enabled);
		bool										GetTransparencyEnabled() const;
		void										SetTransparencyMode(ZERNTransparencyMode Mode);
		ZERNTransparencyMode						GetTransparencyMode() const;

		void										SetAlphaCullEnabled(bool Enabled);
		bool										GetAlphaCullEnabled() const;
		void										SetAlphaCullLimit(float Limit);
		float										GetAlphaCullLimit() const;

		void										SetSubSurfaceScatteringFactor(float Factor);
		float										GetSubSurfaceScatteringFactor() const;
		void										SetSubSurfaceScatteringMapEnabled(bool Enabled);
		bool										GetSubSurfaceScatteringMapEnabled() const;
		void										SetSubSurfaceScatteringMap(ZEGRTexture2D* Texture);
		const ZEGRTexture2D*						GetSubSurfaceScatteringMap() const;
		void										SetSubSurfaceScatteringMapFile(const ZEString& Filename);
		const ZEString&								GetSubSurfaceScatteringMapFile() const;

		void										SetBaseMapEnabled(bool Enabled);
		bool										GetBaseMapEnabled() const;
		void										SetBaseMap(ZEGRTexture2D* Texture);
		const ZEGRTexture2D*						GetBaseMap() const;
		void										SetBaseMapFile(const ZEString& Filename);
		const ZEString&								GetBaseMapFile() const;

		void										SetAmbientEnabled(bool Enabled);
		bool										GetAmbientEnabled() const;
		void										SetSceneAmbientEnabled(bool Enabled);
		bool										GetSceneAmbientEnabled() const;
		void										SetAmbientFactor(float Factor);
		float										GetAmbientFactor() const;
		void										SetAmbientColor(const ZEVector3& Color);
		const ZEVector3&							GetAmbientColor() const;
		
		void										SetDiffuseEnabled(bool Enabled);
		bool										GetDiffuseEnabled() const;
		void										SetDiffuseFactor(float Factor);
		float										GetDiffuseFactor() const;
		void										SetDiffuseColor(const ZEVector3& Color);
		const ZEVector3&							GetDiffuseColor() const;

		void										SetSpecularEnabled(bool Enabled);
		bool										GetSpecularEnabled() const;
		void										SetSpecularFactor(float Factor);
		float										GetSpecularFactor() const;
		void										SetSpecularShininess(float Shininess);
		float										GetSpecularShininess() const;
		void										SetSpecularColor(const ZEVector3& Color);
		const ZEVector3&							GetSpecularColor() const;
		void										SetSpecularMapEnabled(bool Enabled);
		bool										GetSpecularMapEnabled() const;
		void										SetSpecularMap(ZEGRTexture2D* Texture);
		const ZEGRTexture2D*						GetSpecularMap() const;
		void										SetSpecularMapFile(const ZEString& Filename);
		const ZEString&								GetSpecularMapFile() const;
		void										SetSpecularGlossMapEnabled(bool Enabled);
		bool										GetSpecularGlossMapEnabled() const;
		void										SetSpecularGlossMap(ZEGRTexture2D* Texture);
		const ZEGRTexture2D*						GetSpecularGlossMap() const;
		void										SetSpecularGlossMapFile(const ZEString& Filename);
		const ZEString&								GetSpecularGlossMapFile() const;

		void										SetEmissiveEnabled(bool Enabled);
		bool										GetEmissiveEnabled() const;
		void										SetEmissiveFactor(float Factor);
		float										GetEmissiveFactor() const;
		void										SetEmissiveColor(const ZEVector3& Color);
		const ZEVector3&							GetEmissiveColor() const;
		void										SetEmissiveMapEnabled(bool Enabled);
		bool										GetEmissiveMapEnabled() const;
		void										SetEmissiveMap(ZEGRTexture2D* Texture);
		const ZEGRTexture2D*						GetEmissiveMap() const;
		void										SetEmissiveMapFile(const ZEString& Filename);
		const ZEString&								GetEmissiveMapFile() const;

		void										SetNormalMapEnabled(bool Enabled);
		bool										GetNormalMapEnabled() const;
		void										SetNormalMap(ZEGRTexture2D* Texture);
		const ZEGRTexture2D*						GetNormalMap() const;
		void										SetNormalMapFile(const ZEString& Filename);
		const ZEString&								GetNormalMapFile() const;

		void										SetHeightMapEnabled(bool Enabled);
		bool										GetHeightMapEnabled() const;
		void										SetHeightMapTechnique(ZERNHeightMapTechnique Technique);
		ZERNHeightMapTechnique						GetHeightMapTechnique() const;
		void										SetHeightMapOffset(float Offset);
		float										GetHeightMapOffset() const;
		void										SetHeightMapScale(float Scale);
		float										GetHeightMapScale() const;
		void										SetHeightMap(ZEGRTexture2D* Texture);
		const ZEGRTexture2D*						GetHeightMap() const;
		void										SetHeightMapFile(const ZEString& Filename);
		const ZEString&								GetHeightMapFile() const;

		void										SetOpacity(float Value);
		float										GetOpacity() const;
		void										SetOpacityMapEnabled(bool Enabled);
		bool										GetOpacityMapEnabled() const;
		void										SetOpacityMap(ZEGRTexture2D* Texture);
		const ZEGRTexture2D*						GetOpacityMap() const;
		void										SetOpacityMapFile(const ZEString& Filename);
		const ZEString&								GetOpacityMapFile() const;

		void										SetEnvironmentMapEnabled(bool Enabled);
		bool										GetEnvironmentMapEnabled() const;
		void										SetEnvironmentMap(ZEGRTexture2D* Texture);
		const ZEGRTexture2D*						GetEnvironmentMap() const;
		void										SetEnvironmentMapFile(const ZEString& Filename);
		const ZEString&								GetEnvironmentMapFile() const;

		void										SetReflectionEnabled(bool Enabled);
		bool										GetReflectionEnabled() const;
		void										SetReflectionFactor(float Factor);
		float										GetReflectionFactor() const;
		void										SetReflectionColor(const ZEVector3& Color);
		const ZEVector3&							GetReflectionColor() const;

		void										SetRefractionEnabled(bool Enabled);
		bool										GetRefractionEnabled() const;
		void										SetRefractionFactor(float Factor);
		float										GetRefractionFactor() const;
		void										SetRefractionColor(const ZEVector3& Color);
		const ZEVector3&							GetRefractionColor() const;
		void										SetRefractionIndex(float Index);
		float										GetRefractionIndex() const;

		void										SetDetailMapTiling(const ZEVector2& Tiling);
		const ZEVector2&							GetDetailMapTiling() const;

		void										SetDetailBaseMapEnabled(bool Enabled); 
		bool										GetDetailBaseMapEnabled() const;
		void										SetDetailBaseMapColor(const ZEVector3& Color);
		const ZEVector3&							GetDetailBaseMapColor() const;
		void										SetDetailBaseMapTiling(const ZEVector2& Tiling);
		const ZEVector2&							GetDetailBaseMapTiling() const;
		void										SetDetailBaseMapAttenuationStart(float Start);
		float										GetDetailBaseMapAttenuationStart() const;
		void										SetDetailBaseMapAttenuationFactor(float Factor);
		float										GetDetailBaseMapAttenuationFactor() const;
		void										SetDetailBaseMap(ZEGRTexture2D* Texture);
		const ZEGRTexture2D*						GetDetailBaseMap() const;
		void										SetDetailBaseMapFile(const ZEString& Filename);
		const ZEString&								GetDetailBaseMapFile() const;

		void										SetDetailNormalMapEnabled(bool Enabled);
		bool										GetDetailNormalMapEnabled() const;
		void										SetDetailNormalMapFactor(float Factor);
		float										GetDetailNormalMapFactor() const;
		void										SetDetailNormalMapTiling(const ZEVector2& Tiling);
		const ZEVector2&							GetDetailNormalMapTiling() const;
		void										SetDetailNormalMapAttenuationStart(float Start);
		float										GetDetailNormalMapAttenuationStart() const;
		void										SetDetailNormalMapAttenuationFactor(float Factor);
		float										GetDetailNormalMapAttenuationFactor() const;
		void										SetDetailNormalMap(ZEGRTexture2D* Texture);
		const ZEGRTexture2D*						GetDetailNormalMap() const;
		void										SetDetailNormalMapFile(const ZEString& Filename);
		const ZEString&								GetDetailNormalMapFile() const;

		void										SetClippingPlanesEnabled(bool Enabled);
		bool										GetClippingPlanesEnabled() const;

		virtual bool								PreRender(ZERNCommand& Command) const;
		virtual bool								SetupMaterial(ZEGRContext* Context, const ZERNStage* Stage) const;
		virtual void								CleanupMaterial(ZEGRContext* Context, const ZERNStage* Stage) const;

		virtual bool								Update();

		bool										Serialize(ZEMLWriterNode* Node);
		bool										Unserialize(ZEMLReaderNode* Node);

		static ZEHolder<ZERNFixedMaterial>			CreateInstance();
		static ZERSHolder<ZERNFixedMaterial>		LoadResource(const ZEString& FileName);
		static ZERSHolder<const ZERNFixedMaterial>	LoadResourceShared(const ZEString& FileName);
};
