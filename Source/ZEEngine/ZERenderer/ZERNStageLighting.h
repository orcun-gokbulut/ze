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
#include "ZEMath/ZEBSphere.h"

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
	friend class ZERNStageForward;
	friend class ZERNStageParticleRendering;
	private:
		ZEFlags										DirtyFlags;
		ZEGRViewport								Viewport;

		ZEHolder<ZEGRShader>						ScreenCoverVertexShader;
		ZEHolder<ZEGRShader>						DeferredLightsVertexShader;
		ZEHolder<ZEGRShader>						DeferredirectionalLightPixelShader;
		ZEHolder<ZEGRShader>						DeferredSpotLightPixelShader;

		ZEHolder<ZEGRShader>						DeferredPointLightPixelShader;

		ZEHolder<ZEGRShader>						DeferredSpotLightPixelShaderPerSample;
		ZEHolder<ZEGRShader>						DeferredPointLightPixelShaderPerSample;
		ZEHolder<ZEGRShader>						EdgeDetectionPixelShader;
		ZEHolder<ZEGRShader>						DebugEdgeDetectionPixelShader;
		ZEHolder<ZEGRShader>						TiledDeferredComputeShader;
		ZEHolder<ZEGRShader>						TiledDeferredTransparencyComputeShader;
		ZEHolder<ZEGRShader>						TiledDeferredPixelShader;
		ZEHolder<ZEGRShader>						TiledDeferredPixelShaderPerSample;

		ZEHolder<ZEGRRenderStateData>				DeferredPointLightRenderState;
		ZEHolder<ZEGRRenderStateData>				DeferredSpotLightRenderState;

		ZEHolder<ZEGRRenderStateData>				DeferredPointLightRenderStatePerSample;
		ZEHolder<ZEGRRenderStateData>				DeferredSpotLightRenderStatePerSample;
		ZEHolder<ZEGRRenderStateData>				EdgeDetectionRenderState;
		ZEHolder<ZEGRRenderStateData>				DebugEdgeDetectionRenderState;
		ZEHolder<ZEGRComputeRenderStateData>		TiledDeferredComputeRenderState;
		ZEHolder<ZEGRComputeRenderStateData>		TiledDeferredTransparencyComputeRenderState;
		ZEHolder<ZEGRRenderStateData>				TiledDeferredGraphicsRenderState;
		ZEHolder<ZEGRRenderStateData>				TiledDeferredGraphicsRenderStatePerSample;

		ZEHolder<ZEGRShader>						ProjectiveLightPixelShader;
		ZEHolder<ZEGRShader>						ProjectiveLightPixelShaderPerSample;

		ZEHolder<ZEGRRenderStateData>				ProjectiveLightRenderState;
		ZEHolder<ZEGRRenderStateData>				ProjectiveLightRenderStatePerSample;
		
		ZEHolder<ZEGRBuffer>						DeferredLightsVertexBuffer;
		ZEHolder<ZEGRBuffer>						DeferredLightsTransformsConstantBuffer;
		
		ZEHolder<ZEGRBuffer>						PointLightsBoundingSpheresStructuredBuffer;
		ZEHolder<ZEGRBuffer>						SpotLightsBoundingSpheresStructuredBuffer;

		ZEHolder<ZEGRBuffer>						EdgeDetectionConstantBuffer;

		ZEHolder<ZEGRTexture>						RandomVectorsTexture;

		ZEHolder<ZEGRBuffer>						LightsConstantBuffer;
		ZEHolder<ZEGRBuffer>						TileLightStructuredBuffer;

		ZEHolder<const ZEGRTexture>					AccumulationTexture;
		ZEHolder<const ZEGRTexture>					GBufferEmissive;
		ZEHolder<const ZEGRTexture>					GBufferDiffuse;
		ZEHolder<const ZEGRTexture>					GBufferNormal;
		ZEHolder<const ZEGRTexture>					DepthTexture;
		ZEHolder<const ZEGRTexture>					DirectionalShadowMaps;
		ZEHolder<const ZEGRTexture>					SpotShadowMaps;
		ZEHolder<const ZEGRTexture>					ProjectiveShadowMaps;

		bool										ShowCascades;
		bool										ShowMSAAEdges;
		bool										UseTiledDeferred;

		ZERNShading_Constants						Constants;
		ZERNShading_EdgeDetectionConstants			EdgeDetectionConstants;

		ZESmartArray<ZERNShading_PointLight>		PointLights;
		ZESmartArray<ZERNShading_SpotLight>			SpotLights;

		ZESmartArray<ZEMatrix4x4>					PointLightsTransforms;
		ZESmartArray<ZEMatrix4x4>					SpotLightsTransforms;
		
		ZESmartArray<ZEBSphere>						PointLightsBoundingSpheres;
		ZESmartArray<ZEBSphere>						SpotLightsBoundingSpheres;

		ZEArray<ZERNShading_SpotLightShadow>		SpotLightsShadow;
		
		ZEArray<ZERNShading_ProjectiveLight>		ProjectiveLights;
		ZEArray<ZEMatrix4x4>						ProjectiveLightsTransforms;
		ZEArray<ZEHolder<const ZEGRTexture>>		ProjectionTextures;

		struct ZERNBufferRange
		{
			ZEUInt	Offset;
			ZEUInt	Count;
		};
		
		ZERNBufferRange								CommonParamsRange;
		ZERNBufferRange								PointLightRange;
		ZERNBufferRange								SpotLightRange;
		ZERNBufferRange								SpotLightShadowRange;
		ZERNBufferRange								ProjectiveLightRange;

		ZERNBufferRange								PointLightVertexRange;
		ZERNBufferRange								SpotLightVertexRange;
		ZERNBufferRange								ProjectiveLightVertexRange;

		void										CreateRandomVectors();
		void										CreateLightGeometries();

		bool										UpdateShaders();
		bool										UpdateRenderState();
		bool										UpdateBuffers();
		bool										Update();

		virtual bool								InitializeInternal();
		virtual bool								DeinitializeInternal();

		bool										SetupLights(ZEGRContext* Context);
		void										DrawLights(ZEGRContext* Context, bool PerSample);

		virtual void								CreateOutput(const ZEString& Name);

	public:
		virtual ZEInt								GetId() const;
		virtual const ZEString&						GetName() const;

		void										SetShowCascades(bool ShowCascades);
		bool										GetShowCascades() const;

		void										SetShowMSAAEdges(bool ShowMSAAEdges);
		bool										GetShowMSAAEdges() const;

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
