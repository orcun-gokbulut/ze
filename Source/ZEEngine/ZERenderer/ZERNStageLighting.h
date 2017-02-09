//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageLighting.h
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

#include "ZERNStage.h"

#include "ZEDS/ZEFlags.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZERNShading.h"

class ZEGRShader;
class ZEGRSampler;
class ZEGRRenderStateData;
class ZEGRComputeRenderStateData;
class ZEGRBuffer;
class ZEGRRenderTarget;
class ZEGRTexture;
class ZELight;
class ZELightProjective;
class ZELightPoint;
class ZELightDirectional;
class ZELightOmniProjective;

class ZERNStageLighting : public ZERNStage
{
	ZE_OBJECT
	private:
		ZEFlags										DirtyFlags;
		ZEGRViewport								Viewport;

		ZEHolder<ZEGRShader>						ScreenCoverVertexShader;
		ZEHolder<ZEGRShader>						DeferredDirectionalLightVertexShader;
		ZEHolder<ZEGRShader>						DeferredSpotLightVertexShader;
		ZEHolder<ZEGRShader>						DeferredSpotLightShadowVertexShader;
		ZEHolder<ZEGRShader>						DeferredPointLightVertexShader;
		ZEHolder<ZEGRShader>						DeferredDirectionalLightPixelShader;
		ZEHolder<ZEGRShader>						DeferredSpotLightPixelShader;
		ZEHolder<ZEGRShader>						DeferredSpotLightShadowPixelShader;
		ZEHolder<ZEGRShader>						DeferredPointLightPixelShader;
		ZEHolder<ZEGRShader>						DeferredDirectionalLightPixelShaderPerSample;
		ZEHolder<ZEGRShader>						DeferredSpotLightPixelShaderPerSample;
		ZEHolder<ZEGRShader>						DeferredSpotLightShadowPixelShaderPerSample;
		ZEHolder<ZEGRShader>						DeferredPointLightPixelShaderPerSample;
		ZEHolder<ZEGRShader>						EdgeDetectionPixelShader;
		ZEHolder<ZEGRShader>						BlendPixelShader;
		ZEHolder<ZEGRShader>						BlendPixelShaderPerSample;
		ZEHolder<ZEGRShader>						TiledDeferredComputeShader;

		ZEHolder<ZEGRRenderStateData>				DeferredDirectionalLightRenderState;
		ZEHolder<ZEGRRenderStateData>				DeferredPointLightRenderState;
		ZEHolder<ZEGRRenderStateData>				DeferredSpotLightRenderState;
		ZEHolder<ZEGRRenderStateData>				DeferredSpotLightShadowRenderState;
		ZEHolder<ZEGRRenderStateData>				DeferredDirectionalLightRenderStatePerSample;
		ZEHolder<ZEGRRenderStateData>				DeferredPointLightRenderStatePerSample;
		ZEHolder<ZEGRRenderStateData>				DeferredSpotLightRenderStatePerSample;
		ZEHolder<ZEGRRenderStateData>				DeferredSpotLightShadowRenderStatePerSample;
		ZEHolder<ZEGRRenderStateData>				EdgeDetectionRenderState;
		ZEHolder<ZEGRRenderStateData>				BlendRenderState;
		ZEHolder<ZEGRRenderStateData>				BlendRenderStatePerSample;
		ZEHolder<ZEGRComputeRenderStateData>		TiledDeferredComputeRenderState;

		ZEHolder<ZEGRShader>						ProjectiveLightVertexShader;
		ZEHolder<ZEGRShader>						ProjectiveLightPixelShader;
		ZEHolder<ZEGRShader>						ProjectiveLightPixelShaderPerSample;

		ZEHolder<ZEGRRenderStateData>				ProjectiveLightRenderState;
		ZEHolder<ZEGRRenderStateData>				ProjectiveLightRenderStatePerSample;

		ZEHolder<ZEGRBuffer>						ConstantBuffer;
		ZEHolder<ZEGRBuffer>						PointLightStructuredBuffer;
		ZEHolder<ZEGRBuffer>						SpotLightStructuredBuffer;
		ZEHolder<ZEGRBuffer>						DeferredLightVertexBuffer;

		ZEHolder<ZEGRBuffer>						SpotLightShadowConstantBuffer;
		ZEHolder<ZEGRBuffer>						ProjectiveLightConstantBuffer;
		ZEHolder<ZEGRBuffer>						EdgeDetectionConstantBuffer;

		ZEHolder<ZEGRTexture>						RandomVectorsTexture;
		ZEHolder<ZEGRTexture>						TiledDeferredOutputTexture;

		ZEHolder<const ZEGRTexture>					AccumulationTexture;
		ZEHolder<const ZEGRTexture>					GBufferEmissive;
		ZEHolder<const ZEGRTexture>					GBufferDiffuse;
		ZEHolder<const ZEGRTexture>					GBufferNormal;
		ZEHolder<const ZEGRTexture>					DepthTexture;
		ZEHolder<const ZEGRTexture>					DirectionalShadowMaps;
		ZEHolder<const ZEGRTexture>					SpotShadowMaps;
		ZEHolder<const ZEGRTexture>					ProjectiveShadowMaps;

		bool										ShowCascades;
		bool										UseTiledDeferred;

		struct
		{
			ZERNShading_DirectionalLight			DirectionalLights[MAX_DIRECTIONAL_LIGHT];
			ZEUInt									DirectionalLightCount;
			ZEUInt									PointLightCount;
			ZEUInt									SpotLightCount;
			ZEUInt									ProjectiveLightCount;

			ZEBool32								AddTiledDeferredOutput;
			ZEUInt									TileCountX;
			ZEVector2								Reserved;
		} Constants;

		struct
		{
			float									DepthThreshold;
			float									NormalThreshold;
			ZEVector2								Reserved;
		} EdgeDetectionConstants;

		ZESmartArray<ZERNShading_PointLight>		PointLights;
		ZESmartArray<ZERNShading_SpotLight>			SpotLights;

		ZEArray<ZERNShading_SpotLightShadow>		SpotLightsShadow;
		ZEArray<ZERNShading_ProjectiveLight>		ProjectiveLights;
		ZEArray<ZEHolder<const ZEGRTexture>>		ProjectionTextures;

		void										CreateRandomVectors();
		void										CreateLightGeometries();

		bool										UpdateShaders();
		bool										UpdateRenderState();
		bool										UpdateLightBuffers();
		bool										Update();

		virtual bool								InitializeInternal();
		virtual bool								DeinitializeInternal();

		void										DrawLights(ZEGRContext* Context, bool PerSample);
		void										BlendTiledDeferred(ZEGRContext* Context, const ZEGRRenderTarget* RenderTarget, bool PerSample);

	public:
		virtual ZEInt								GetId() const;
		virtual const ZEString&						GetName() const;

		void										SetShowCascades(bool ShowCascades);
		bool										GetShowCascades() const;

		void										SetUseTiledDeferred(bool UseTiledDeferred);
		bool										GetUseTiledDeferred() const;

		void										SetMSAADepthThreshold(float MSAADepthThreshold);
		float										GetMSAADepthThreshold() const;

		void										SetMSAANormalThreshold(float MSAANormalThreshold);
		float										GetMSAANormalThreshold() const;

		virtual void								Resized(ZEUInt Width, ZEUInt Height);

		virtual bool								Setup(ZEGRContext* Context);
		virtual void								CleanUp(ZEGRContext* Context);

													ZERNStageLighting();
		virtual										~ZERNStageLighting();
};
