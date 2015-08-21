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

#include "ZERNMaterial.h"

#include "ZERNSampler.h"
#include "ZEDS/ZEString.h"
#include "ZEDefinitions.h"
#include "ZEMeta/ZEObject.h"
#include "ZEMath/ZEVector.h"
#include "ZEGraphics/ZEGRHolder.h"
#include "ZEGraphics/ZEGRSamplerState.h"

class ZEGRRenderStateData;
class ZEGRConstantBuffer;

class ZEFixedMaterial : public ZERNMaterial
{
	protected:
		ZEString						Name;
		ZEString						FileName;

		ZERNSampler						BaseTexture;
		ZERNSampler						NormalTexture;
		ZERNSampler						ParallaxTexture;
		ZERNSampler						SpecularTexture;
		ZERNSampler						EmmisiveTexture;
		ZERNSampler						OpacityTexture;
		ZERNSampler						DetailBaseTexture;
		ZERNSampler						DetailNormalTexture;
		ZERNSampler						EnvironmentTexture;
		ZERNSampler						DistortionTexture;

		ZEGRHolder<ZEGRRenderStateData*>	GBufferPass;
		ZEGRHolder<ZEGRRenderStateData*>	ForwardPass;
		ZEGRHolder<ZEGRRenderStateData*>	ShadowPass;

		struct
		{
			ZEVector3					AmbientColor;
			ZEVector3					DiffuseColor;
			ZEVector3					SpecularColor;
			ZEVector3					EmmisiveColor;
			ZEVector2					DetailMapTiling;
			float						Opacity;
			float						DistortionFactor;
			float						SpecularShininess;
			float						ReflectionFactor;
			float						RefractionIndex;
			float						RefractionFactor;
			float						LightMapFactor;
			float						SubSurfaceScatteringFactor;
			float						AlphaCullLimit;
			bool						TwoSided;
			bool						Wireframe;
			bool						GlobalAmbientEnabled;
		} Constants;
		ZEGRHolder<ZEGRConstantBuffer>	ConstantBuffer;
	
		ZEVector3						AmbientColor;
		float							AmbientFactor;
		ZEVector3						SpecularColor;
		float							SpecularFactor;
		ZEVector3						DiffuseColor;
		float							DiffuseFactor;
		ZEVector3						EmmisiveColor;
		float							EmmisiveFactor;

		bool							VertexColorEnabled;
		bool							AlphaCullEnabled;
		bool							AmbientEnabled;
		bool							DiffuseEnabled;
		bool							SpecularEnabled;
		bool							EmmisiveEnabled;
		bool							NormalMapEnabled;
		bool							DisplacementEnabled;
		bool							OpacityEnabled;
		bool							EnvironmentMapEnabled;
		bool							ReflectionEnabled;
		bool							RefractionEnabled;
		bool							DetailBaseMapEnabled;
		bool							DetailNormalMapEnabled;
		bool							DistortionEnabled;
		bool							SSAOEnabled;

		void							Update();

										ZEFixedMaterial();
		virtual							~ZEFixedMaterial();

	public:
		void							SetName(const ZEString& Name);
		const ZEString&					GetName() const;

		const ZEString&					GetFileName() const;

		void							SetTwoSided(bool Enable);
		bool							GetTwoSided() const;

		void							SetWireframe(bool Enable);
		bool							GetWireframe() const;

		void							SetSSAOEnabled(bool Enabled);
		bool							GetSSAOEnabled() const;

		void							SetVertexColorEnabled(bool Enabled);
		bool							GetVertexColorEnabled();

		void							SetAlphaCullEnabled(bool Enabled);
		bool							GetAlphaCullEnabled() const;
		void							SetAlphaCullLimit(float Limit);
		float							GetAlphaCullLimit() const;

		void							SetBaseMap(const ZERNSampler& Sampler);
		const ZERNSampler&				GetBaseMap() const;

		void							SetAmbientEnabled(bool Enabled);
		bool							GetAmbientEnabled() const;
		void							SetGlobalAmbientEnabled(bool Enabled);
		bool							GetGlobalAmbientEnabled() const;
		void							SetAmbientFactor(float Factor);
		float							GetAmbientFactor() const;
		void							SetAmbientColor(const ZEVector3& Color);
		const ZEVector3&				GetAmbientColor() const;
		
		void							SetDiffuseEnabled(bool Enabled);
		bool							GetDiffuseEnabled() const;
		void							SetDiffuseColor(const ZEVector3& Color);
		const ZEVector3&				GetDiffuseColor() const;
		void							SetDiffuseFactor(float Factor);
		float							GetDiffuseFactor() const;
		void							SetDiffuseSubSurfaceScatteringFactor(float Factor);
		float							GetDiffuseSubSurfaceScatteringFactor() const;

		void							SetSpecularEnabled(bool Enabled);
		bool							GetSpecularEnabled() const;
		void							SetSpecularFactor(float Factor);
		float							GetSpecularFactor() const;
		void							SetSpecularShininess(float Shininess);
		float							GetSpecularShininess() const;
		void							SetSpecularColor(const ZEVector3& Color);
		const ZEVector3&				GetSpecularColor() const;
		void							SetSpecularMap(const ZERNSampler& Sampler);
		const ZERNSampler&				GetSpecularMap() const;

		void							SetEmmisiveEnabled(bool Enabled);
		bool							GetEmmisiveEnabled() const;
		void							SetEmmisiveFactor(float Factor);
		float							GetEmmisiveFactor() const;
		void							SetEmmisiveColor(const ZEVector3& Color);
		const ZEVector3&				GetEmmisiveColor() const;
		void							SetEmmisiveMap(const ZERNSampler& Sampler);
		const ZERNSampler&				GetEmmisiveMap() const;

		void							SetNormalMapEnabled(bool Enabled);
		bool							GetNormalMapEnabled() const;
		void							SetNormalMap(const ZERNSampler& Sampler);
		const ZERNSampler&				GetNormalMap() const;

		void							SetDisplacementMapEnabled(bool Enabled);
		bool							GetDisplacementMapEnabled() const;
		void							SetDisplacementOffset(float Offset);
		float							GetDisplacementOffset() const;
		void							SetDisplacementScale(float Scale);
		float							GetDisplacementScale() const;
		void							SetDisplacementMap(const ZERNSampler& Sampler);
		const ZERNSampler&				GetDisplacementMap() const;

		void							SetOpacityEnabled(bool Enabled);
		bool							GetOpacityEnabled() const;
		void							SetOpacity(float Value);
		float							GetOpacity() const;
		void							SetOpacityMap(const ZERNSampler& Sampler);
		const ZERNSampler&				GetOpacityMap() const;

		void							SetEnvironmentMappingEnabled(bool Enabled);
		bool							GetEnvironmentMappingEnabled();
		void							SetEnvironmentMap(const ZERNSampler& Sampler);
		const ZERNSampler&				GetEnvironmentMap() const;

		void							SetReflectionEnabled(bool Enabled);
		bool							GetReflectionEnabled() const;
		void							SetReflectionFactor(float Factor);
		float							GetReflectionFactor() const;

		void							SetRefractionEnabled(bool Enabled);
		bool							GetRefractionEnabled() const;
		void							SetRefractionIndex(float Index);
		float							GetRefractionIndex() const;
		void							SetRefractionFactor(float Factor);
		float							GetRefractionFactor() const;

		void							SetDetailMapTiling(const ZEVector2& Tiling);
		const ZEVector2&				GetDetailMapTiling() const;
		void							SetDetailBaseMapEnabled(bool Enabled); 
		bool							GetDetailBaseMapEnabled() const;
		void							SetDetailBaseMap(const ZERNSampler& Sampler);
		const ZERNSampler&				GetDetailBaseMap() const;

		void							SetDetailNormalMapEnabled(bool Enabled); 
		bool							GetDetailNormalMapEnabled() const;
		void							SetDetailNormalMap(const ZERNSampler& Sampler);
		const ZERNSampler&				GetDetailNormalMap() const;

		void							SetDistortionMapEnabled(bool Enabled);
		bool							GetDistortionMapEnabled() const;
		void							SetDistortionFactor(float Factor);
		float							GetDistortionFactor() const;
		void							SetDistortionMap(const ZERNSampler& Sampler);
		const ZERNSampler&				GetDistortionMap() const;

		void							Tick(float ElapsedTime);

		void							WriteToFile(const ZEString& FileName);
		void							ReadFromFile(const ZEString& FileName);

		static ZEFixedMaterial*			CreateInstance();
};
