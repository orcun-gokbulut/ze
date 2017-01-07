//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNParticleMaterial.h
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

#include "ZEPointer/ZEHolder.h"
#include "ZEDS/ZEFlags.h"
#include "ZEDS/ZEString.h"
#include "ZEMath/ZEVector.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRShaderCompileOptions.h"
#include "ZERNStandardMaterial.h"

class ZEGRBuffer;
class ZEGRShader;
class ZEGRSampler;
class ZEGRRenderStateData;

class ZERNParticleMaterial : public ZERNMaterial
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZERNParticleMaterial)
	private:
		ZEFlags									DirtyFlags;

		ZEHolder<const ZEGRShader>				StageParticleRendering_VertexShader;
		ZEHolder<const ZEGRShader>				StageParticleRendering_PixelShader;
		ZEHolder<const ZEGRRenderStateData>		StageParticleRendering_RenderState;

		ZEHolder<ZEGRBuffer>					ConstantBuffer;

		ZERNTransparencyMode					TransparencyMode;

		ZEHolder<const ZEGRSampler>				Sampler;
		ZEHolder<const ZEGRTexture>				DiffuseMap;
		ZEHolder<const ZEGRTexture>				EmissiveMap;
		ZEHolder<const ZEGRTexture>				NormalMap;
		ZEHolder<const ZEGRTexture>				OpacityMap;

		bool									TwoSided;
		bool									ShadowCaster;
		bool									AlphaCullEnabled;
		bool									VertexColorEnabled;
		bool									AmbientEnabled;
		float									AmbientFactor;
		ZEVector3								AmbientColor;
		bool									DiffuseMapEnabled;
		ZEString								DiffuseMapFileName;
		bool									DiffuseEnabled;
		float									DiffuseFactor;
		ZEVector3								DiffuseColor;
		bool									EmissiveMapEnabled;
		ZEString								EmissiveMapFileName;
		bool									EmissiveEnabled;
		float									EmissiveFactor;
		ZEVector3								EmissiveColor;
		bool									NormalMapEnabled;
		ZEString								NormalMapFileName;
		bool									OpacityMapEnabled;
		ZEString								OpacityMapFileName;

		struct
		{
			ZEVector3							AmbientColor;
			float								Opacity;

			ZEVector3							DiffuseColor;
			ZEBool32							SceneAmbientEnabled;

			ZEVector3							EmissiveColor;
			float								AlphaCullLimit;

			ZEBool32							SoftParticle;
			float								DistanceMax;
			float								ContrastPower;
			float								Reserved0;

		} Constants;

		void									UpdateShaderDefinitions(ZEGRShaderCompileOptions& Options);
		bool									UpdateShaders();
		bool									UpdateRenderState();
		bool									UpdateConstantBuffer();

		virtual ZETaskResult					LoadInternal();
		virtual ZETaskResult					UnloadInternal();

												ZERNParticleMaterial();
		virtual									~ZERNParticleMaterial();

	public:
		virtual ZEUInt							GetStageMask() const;

		void									SetTwoSided(bool TwoSided);
		bool									GetTwoSided() const;

		void									SetTransparencyMode(ZERNTransparencyMode Mode);
		ZERNTransparencyMode					GetTransparencyMode() const;

		void									SetSampler(const ZEGRSampler* Sampler);
		const ZEGRSampler*						GetSampler() const;

		void									SetDiffuseMap(const ZEGRTexture* Texture);
		const ZEGRTexture*						GetDiffuseMap() const;

		void									SetDiffuseMapFileName(const ZEString& Filename);
		const ZEString&							GetDiffuseMapFileName() const;

		void									SetEmissiveMap(const ZEGRTexture* Texture);
		const ZEGRTexture*						GetEmissiveMap() const;

		void									SetEmissiveMapFileName(const ZEString& Filename);
		const ZEString&							GetEmissiveMapFileName() const;

		void									SetNormalMap(const ZEGRTexture* Texture);
		const ZEGRTexture*						GetNormalMap() const;

		void									SetNormalMapFileName(const ZEString& Filename);
		const ZEString&							GetNormalMapFileName() const;

		void									SetOpacityMap(const ZEGRTexture* Texture);
		const ZEGRTexture*						GetOpacityMap() const;

		void									SetOpacityMapFileName(const ZEString& Filename);
		const ZEString&							GetOpacityMapFileName() const;

		void									SetSoftParticle(bool SoftParticle);
		bool									GetSoftParticle() const;

		void									SetDistanceMax(float DistanceMax);
		float									GetDistanceMax() const;

		void									SetContrastPower(float ContrastPower);
		float									GetContrastPower() const;

		void									SetShadowCaster(bool ShadowCaster);
		bool									GetShadowCaster() const;

		void									SetAlphaCullEnabled(bool Enabled);
		bool									GetAlphaCullEnabled() const;

		void									SetVertexColorEnabled(bool Enabled);
		bool									GetVertexColorEnabled() const;

		void									SetAmbientEnabled(bool Enabled);
		bool									GetAmbientEnabled() const;

		void									SetAmbientFactor(float Factor);
		float									GetAmbientFactor() const;

		void									SetAmbientColor(const ZEVector3& Color);
		const ZEVector3&						GetAmbientColor() const;

		void									SetDiffuseMapEnabled(bool Enabled);
		bool									GetDiffuseMapEnabled() const;

		void									SetDiffuseEnabled(bool Enabled);
		bool									GetDiffuseEnabled() const;

		void									SetDiffuseFactor(float Factor);
		float									GetDiffuseFactor() const;

		void									SetDiffuseColor(const ZEVector3& Color);
		const ZEVector3&						GetDiffuseColor() const;

		void									SetEmissiveMapEnabled(bool Enabled);
		bool									GetEmissiveMapEnabled() const;

		void									SetEmissiveEnabled(bool Enabled);
		bool									GetEmissiveEnabled() const;

		void									SetEmissiveFactor(float Factor);
		float									GetEmissiveFactor() const;

		void									SetEmissiveColor(const ZEVector3& Color);
		const ZEVector3&						GetEmissiveColor() const;

		void									SetNormalMapEnabled(bool Enabled);
		bool									GetNormalMapEnabled() const;

		void									SetOpacityMapEnabled(bool Enabled);
		bool									GetOpacityMapEnabled() const;

		void									SetOpacity(float Value);
		float									GetOpacity() const;

		void									SetSceneAmbientEnabled(bool Enabled);
		bool									GetSceneAmbientEnabled() const;

		void									SetAlphaCullLimit(float Limit);
		float									GetAlphaCullLimit() const;

		virtual bool							SetupMaterial(ZEGRContext* Context, const ZERNStage* Stage, bool Instanced = false) const;
		virtual void							CleanupMaterial(ZEGRContext* Context, const ZERNStage* Stage, bool Instanced = false) const;

		virtual bool							Update();

		static ZEHolder<ZERNParticleMaterial>	CreateInstance();
};
