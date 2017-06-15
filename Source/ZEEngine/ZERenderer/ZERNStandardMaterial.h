//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStandardMaterial.h
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
class ZEGRBuffer;
class ZEGRShader;
class ZEGRSampler;
class ZEGRTexture;
class ZEMLReaderNode;
class ZEGRShaderCompileOptions;

enum ZERNHeightMapTechnique : ZEUInt8
{
	ZERN_HMT_NONE					= 0,
	ZERN_HMT_PARALLAX				= 1,
	ZERN_HMT_PARALLAX_OCCLUSION		= 2,
	ZERN_HMT_VERTEX_DISPLACEMENT	= 3,
	ZERN_HMT_TESSELLATION			= 4
};

enum ZERNTransparencyMode : ZEUInt8
{
	ZERN_TM_NONE					= 0,
	ZERN_TM_ORDERED					= 1,
	ZERN_TM_ADDITIVE				= 2,
	ZERN_TM_DITHERING				= 3
};

class ZERNStandardMaterial : public ZERNMaterial
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZERNStandardMaterial)
	friend class ZERSTemplates;
	private:
		ZEUInt											StageMask;
		mutable ZEFlags									DirtyFlags;

		ZEHolder<ZEGRShader>							StageGBuffer_Forward_VertexShader;
		ZEHolder<ZEGRShader>							StageGBuffer_Forward_PixelShader;
		ZEHolder<ZEGRRenderStateData>					StageGBuffer_Forward_RenderState;
		
		ZEHolder<ZEGRShader>							StageGBuffer_Forward_Instancing_VertexShader;
		ZEHolder<ZEGRShader>							StageGBuffer_Forward_Instancing_PixelShader;
		ZEHolder<ZEGRRenderStateData>					StageGBuffer_Forward_Instancing_RenderState;

		ZEHolder<ZEGRShader>							StageShadowmapGeneration_VertexShader;
		ZEHolder<ZEGRShader>							StageShadowmapGeneration_PixelShader;
		ZEHolder<ZEGRRenderStateData>					StageShadowmapGeneration_RenderState;

		ZEHolder<ZEGRShader>							StageShadowmapGeneration_Instancing_VertexShader;
		ZEHolder<ZEGRRenderStateData>					StageShadowmapGeneration_Instancing_RenderState;

		ZEHolder<ZEGRShader>							StageRenderDepth_VertexShader;
		ZEHolder<ZEGRShader>							StageRenderDepth_PixelShader;
		ZEHolder<ZEGRRenderStateData>					StageRenderDepth_RenderState;

		ZEHolder<ZEGRShader>							StageRenderDepth_Instancing_VertexShader;
		ZEHolder<ZEGRShader>							StageRenderDepth_Instancing_PixelShader;
		ZEHolder<ZEGRRenderStateData>					StageRenderDepth_Instancing_RenderState;

		ZEHolder<ZEGRBuffer>							ConstantBuffer;
		ZEHolder<ZEGRSampler>							Sampler;

		ZEHolder<const ZEGRTexture>						BaseMap;
		ZEHolder<const ZEGRTexture>						SpecularMap;
		ZEHolder<const ZEGRTexture>						SpecularGlossMap;
		ZEHolder<const ZEGRTexture>						EmissiveMap;
		ZEHolder<const ZEGRTexture>						NormalMap;
		ZEHolder<const ZEGRTexture>						HeightMap;
		ZEHolder<const ZEGRTexture>						OpacityMap;
		ZEHolder<const ZEGRTexture>						SubSurfaceScatteringMap;
		ZEHolder<const ZEGRTexture>						EnvironmentMap;
		ZEHolder<const ZEGRTexture>						DetailBaseMap;
		ZEHolder<const ZEGRTexture>						DetailNormalMap;
		
		mutable bool									UseInteriorVertexLayout;

		mutable struct
		{
			ZEVector3									AmbientColor;
			float										Opacity;
			
			ZEVector3									DiffuseColor;
			float										SubSurfaceScatteringFactor;
			
			ZEVector3									SpecularColor;
			float										SpecularPower;
			
			ZEUInt32									HeightMapTechnique;
			float										HeightMapScale;
			float										HeightMapOffset;
			ZEBool32									DitheredOpacityEnabled;
			
			ZEVector3									EmissiveColor;
			float										AlphaCullLimit;
			
			ZEVector3									ReflectionColor;
			ZEBool32									SceneAmbientEnabled;
			
			ZEVector3									RefractionColor;
			float										RefractionIndex;
			
			ZEVector3									DetailBaseMapColor;
			float										DetailNormalMapFactor;
			
			ZEVector2									DetailBaseMapTiling;
			ZEVector2									DetailNormalMapTiling;
			
			float										DetailBaseMapAttenuationStart;
			float										DetailBaseMapAttenuationFactor;
			float										DetailNormalMapAttenuationStart;
			float										DetailNormalMapAttenuationFactor;
		} Constants;

		bool											AlphaToCoverageEnabled;
		ZEUInt8											MaxTextureLOD;
		bool											ShadowCaster;
		bool											TwoSided;
		bool											Wireframe;
		bool											AlphaCullEnabled;
		bool											SkinningEnabled;
		bool											VertexColorEnabled;
		bool											TransparencyEnabled;
		ZERNTransparencyMode							TransparencyMode;
		bool											BaseMapEnabled;
		ZEString										BaseMapFileName;
		bool											AmbientEnabled;
		float											AmbientFactor;
		ZEVector3										AmbientColor;
		bool											DiffuseEnabled;
		float											DiffuseFactor;
		ZEVector3										DiffuseColor;
		bool											SpecularEnabled;
		bool											SpecularMapEnabled;
		ZEString										SpecularMapFileName;
		bool											SpecularGlossMapEnabled;
		ZEString										SpecularGlossMapFile;
		float											SpecularFactor;
		ZEVector3										SpecularColor;
		bool											NormalMapEnabled;
		ZEString										NormalMapFileName;
		bool											HeightMapEnabled;
		ZEString										HeightMapFileName;
		ZERNHeightMapTechnique							HeightMapTechnique;
		bool											EmissiveEnabled;
		float											EmissiveFactor;
		ZEVector3										EmissiveColor; 
		bool											EmissiveMapEnabled;
		ZEString										EmissiveMapFileName;
		bool											SubSurfaceScatteringMapEnabled;
		ZEString										SubSurfaceScatteringMapFileName;
		bool											ReflectionEnabled;
		float											ReflectionFactor;
		ZEVector3										ReflectionColor;
		bool											RefractionEnabled;
		float											RefractionFactor;
		ZEVector3										RefractionColor;
		bool											OpacityMapEnabled;
		ZEString										OpacityMapFileName;
		bool											EnvironmentMapEnabled;
		ZEString										EnvironmentMapFileName;
		bool											DetailBaseMapEnabled;
		ZEString										DetailBaseMapFileName;
		bool											DetailNormalMapEnabled;
		ZEString										DetailNormalMapFileName;
		bool											ClippingPlanesEnabled;
		
		void											UpdateGBufferForwardVertexShaderDefinitions(ZEGRShaderCompileOptions& Options) const;
		void											UpdateGBufferForwardPixelShaderDefinitions(ZEGRShaderCompileOptions& Options) const;
		void											UpdateShadowMapGenerationVertexShaderDefinitions(ZEGRShaderCompileOptions& Options) const;
		void											UpdateShadowMapGenerationPixelShaderDefinitions(ZEGRShaderCompileOptions& Options) const;
		bool											UpdateShaders();
		bool											UpdateConstantBuffer();
		bool											UpdateRenderState();
		bool											UpdateStageMask();

		virtual ZETaskResult							LoadInternal();
		virtual ZETaskResult							UnloadInternal();

														ZERNStandardMaterial();
		virtual											~ZERNStandardMaterial();

	public:
		virtual ZEUInt									GetStageMask() const;

		void											SetSampler(const ZEHolder<ZEGRSampler>& Sampler);
		const ZEHolder<ZEGRSampler>&					GetSampler() const;

		void											SetAlphaToCoverageEnabled(bool Enabled);
		bool											GetAlphaToCoverageEnabled() const;

		void											SetMaxTextureLOD(ZEUInt8 MaxTextureLOD);
		ZEUInt8											GetMaxTextureLOD() const;

		void											SetShadowCaster(bool ShadowCaster);
		bool											GetShadowCaster() const;

		void											SetTwoSided(bool Enable);
		bool											GetTwoSided() const;

		void											SetWireframe(bool Enable);
		bool											GetWireframe() const;

		void											SetSkinningEnabled(bool SkinningEnabled);
		bool											GetSkinningEnabled() const;

		void											SetVertexColorEnabled(bool Enabled);
		bool											GetVertexColorEnabled() const;

		void											SetTransparencyEnabled(bool Enabled);
		bool											GetTransparencyEnabled() const;
		void											SetTransparencyMode(ZERNTransparencyMode Mode);
		ZERNTransparencyMode							GetTransparencyMode() const;

		void											SetDitheredOpacityEnabled(bool Enabled);
		bool											GetDitheredOpacityEnabled() const;

		void											SetAlphaCullEnabled(bool Enabled);
		bool											GetAlphaCullEnabled() const;
		void											SetAlphaCullLimit(float Limit);
		float											GetAlphaCullLimit() const;

		void											SetSubSurfaceScatteringFactor(float Factor);
		float											GetSubSurfaceScatteringFactor() const;
		void											SetSubSurfaceScatteringMapEnabled(bool Enabled);
		bool											GetSubSurfaceScatteringMapEnabled() const;
		void											SetSubSurfaceScatteringMap(const ZEGRTexture* Texture);
		const ZEGRTexture*								GetSubSurfaceScatteringMap() const;
		void											SetSubSurfaceScatteringMapFile(const ZEString& FileName);
		const ZEString&									GetSubSurfaceScatteringMapFile() const;

		void											SetBaseMapEnabled(bool Enabled);
		bool											GetBaseMapEnabled() const;
		void											SetBaseMap(const ZEGRTexture* Texture);
		const ZEGRTexture*								GetBaseMap() const;
		void											SetBaseMapFile(const ZEString& FileName);
		const ZEString&									GetBaseMapFile() const;

		void											SetAmbientEnabled(bool Enabled);
		bool											GetAmbientEnabled() const;
		void											SetSceneAmbientEnabled(bool Enabled);
		bool											GetSceneAmbientEnabled() const;
		void											SetAmbientFactor(float Factor);
		float											GetAmbientFactor() const;
		void											SetAmbientColor(const ZEVector3& Color);
		const ZEVector3&								GetAmbientColor() const;
		
		void											SetDiffuseEnabled(bool Enabled);
		bool											GetDiffuseEnabled() const;
		void											SetDiffuseFactor(float Factor);
		float											GetDiffuseFactor() const;
		void											SetDiffuseColor(const ZEVector3& Color);
		const ZEVector3&								GetDiffuseColor() const;

		void											SetSpecularEnabled(bool Enabled);
		bool											GetSpecularEnabled() const;
		void											SetSpecularFactor(float Factor);
		float											GetSpecularFactor() const;
		void											SetSpecularShininess(float Shininess);
		float											GetSpecularShininess() const;
		void											SetSpecularColor(const ZEVector3& Color);
		const ZEVector3&								GetSpecularColor() const;
		void											SetSpecularMapEnabled(bool Enabled);
		bool											GetSpecularMapEnabled() const;
		void											SetSpecularMap(const ZEGRTexture* Texture);
		const ZEGRTexture*								GetSpecularMap() const;
		void											SetSpecularMapFile(const ZEString& FileName);
		const ZEString&									GetSpecularMapFile() const;
		void											SetSpecularGlossMapEnabled(bool Enabled);
		bool											GetSpecularGlossMapEnabled() const;
		void											SetSpecularGlossMap(const ZEGRTexture* Texture);
		const ZEGRTexture*								GetSpecularGlossMap() const;
		void											SetSpecularGlossMapFile(const ZEString& FileName);
		const ZEString&									GetSpecularGlossMapFile() const;

		void											SetEmissiveEnabled(bool Enabled);
		bool											GetEmissiveEnabled() const;
		void											SetEmissiveFactor(float Factor);
		float											GetEmissiveFactor() const;
		void											SetEmissiveColor(const ZEVector3& Color);
		const ZEVector3&								GetEmissiveColor() const;
		void											SetEmissiveMapEnabled(bool Enabled);
		bool											GetEmissiveMapEnabled() const;
		void											SetEmissiveMap(const ZEGRTexture* Texture);
		const ZEGRTexture*								GetEmissiveMap() const;
		void											SetEmissiveMapFile(const ZEString& FileName);
		const ZEString&									GetEmissiveMapFile() const;

		void											SetNormalMapEnabled(bool Enabled);
		bool											GetNormalMapEnabled() const;
		void											SetNormalMap(const ZEGRTexture* Texture);
		const ZEGRTexture*								GetNormalMap() const;
		void											SetNormalMapFile(const ZEString& FileName);
		const ZEString&									GetNormalMapFile() const;

		void											SetHeightMapEnabled(bool Enabled);
		bool											GetHeightMapEnabled() const;
		void											SetHeightMapTechnique(ZERNHeightMapTechnique Technique);
		ZERNHeightMapTechnique							GetHeightMapTechnique() const;
		void											SetHeightMapOffset(float Offset);
		float											GetHeightMapOffset() const;
		void											SetHeightMapScale(float Scale);
		float											GetHeightMapScale() const;
		void											SetHeightMap(const ZEGRTexture* Texture);
		const ZEGRTexture*								GetHeightMap() const;
		void											SetHeightMapFile(const ZEString& FileName);
		const ZEString&									GetHeightMapFile() const;

		void											SetOpacity(float Value);
		float											GetOpacity() const;
		void											SetOpacityMapEnabled(bool Enabled);
		bool											GetOpacityMapEnabled() const;
		void											SetOpacityMap(const ZEGRTexture* Texture);
		const ZEGRTexture*								GetOpacityMap() const;
		void											SetOpacityMapFile(const ZEString& FileName);
		const ZEString&									GetOpacityMapFile() const;

		void											SetEnvironmentMapEnabled(bool Enabled);
		bool											GetEnvironmentMapEnabled() const;
		void											SetEnvironmentMap(const ZEGRTexture* Texture);
		const ZEGRTexture*								GetEnvironmentMap() const;
		void											SetEnvironmentMapFile(const ZEString& FileName);
		const ZEString&									GetEnvironmentMapFile() const;

		void											SetReflectionEnabled(bool Enabled);
		bool											GetReflectionEnabled() const;
		void											SetReflectionFactor(float Factor);
		float											GetReflectionFactor() const;
		void											SetReflectionColor(const ZEVector3& Color);
		const ZEVector3&								GetReflectionColor() const;

		void											SetRefractionEnabled(bool Enabled);
		bool											GetRefractionEnabled() const;
		void											SetRefractionFactor(float Factor);
		float											GetRefractionFactor() const;
		void											SetRefractionColor(const ZEVector3& Color);
		const ZEVector3&								GetRefractionColor() const;
		void											SetRefractionIndex(float Index);
		float											GetRefractionIndex() const;

		void											SetDetailMapTiling(const ZEVector2& Tiling);
		const ZEVector2&								GetDetailMapTiling() const;

		void											SetDetailBaseMapEnabled(bool Enabled); 
		bool											GetDetailBaseMapEnabled() const;
		void											SetDetailBaseMapColor(const ZEVector3& Color);
		const ZEVector3&								GetDetailBaseMapColor() const;
		void											SetDetailBaseMapTiling(const ZEVector2& Tiling);
		const ZEVector2&								GetDetailBaseMapTiling() const;
		void											SetDetailBaseMapAttenuationStart(float Start);
		float											GetDetailBaseMapAttenuationStart() const;
		void											SetDetailBaseMapAttenuationFactor(float Factor);
		float											GetDetailBaseMapAttenuationFactor() const;
		void											SetDetailBaseMap(const ZEGRTexture* Texture);
		const ZEGRTexture*								GetDetailBaseMap() const;
		void											SetDetailBaseMapFile(const ZEString& FileName);
		const ZEString&									GetDetailBaseMapFile() const;

		void											SetDetailNormalMapEnabled(bool Enabled);
		bool											GetDetailNormalMapEnabled() const;
		void											SetDetailNormalMapFactor(float Factor);
		float											GetDetailNormalMapFactor() const;
		void											SetDetailNormalMapTiling(const ZEVector2& Tiling);
		const ZEVector2&								GetDetailNormalMapTiling() const;
		void											SetDetailNormalMapAttenuationStart(float Start);
		float											GetDetailNormalMapAttenuationStart() const;
		void											SetDetailNormalMapAttenuationFactor(float Factor);
		float											GetDetailNormalMapAttenuationFactor() const;
		void											SetDetailNormalMap(const ZEGRTexture* Texture);
		const ZEGRTexture*								GetDetailNormalMap() const;
		void											SetDetailNormalMapFile(const ZEString& FileName);
		const ZEString&									GetDetailNormalMapFile() const;

		void											SetClippingPlanesEnabled(bool Enabled);
		bool											GetClippingPlanesEnabled() const;

		virtual bool									PreRender(ZERNCommand& Command) const;
		virtual bool									SetupMaterial(ZEGRContext* Context, const ZERNStage* Stage) const;
		virtual void									CleanupMaterial(ZEGRContext* Context, const ZERNStage* Stage) const;

		virtual bool									Update();

		bool											Serialize(ZEMLWriterNode* Node);
		bool											Unserialize(ZEMLReaderNode* Node);

		static ZERNStandardMaterial*					CreateInstance();
		static ZERSHolder<ZERNStandardMaterial>			CreateResource();
		static ZERSHolder<ZERNStandardMaterial>			LoadResource(const ZEString& FileName);
		static ZERSHolder<const ZERNStandardMaterial>	LoadResourceShared(const ZEString& FileName);
}
ZEMT_ATTRIBUTE(ZEMC.Instancable, true);
