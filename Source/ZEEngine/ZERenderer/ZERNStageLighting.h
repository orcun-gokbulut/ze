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
		ZEFlags									DirtyFlags;
		ZEGRViewport							Viewport;

		ZEHolder<ZEGRShader>					ScreenCoverVertexShader;
		ZEHolder<ZEGRShader>					DeferredDirectionalLightVertexShader;
		ZEHolder<ZEGRShader>					DeferredProjectiveLightVertexShader;
		ZEHolder<ZEGRShader>					DeferredPointLightVertexShader;
		ZEHolder<ZEGRShader>					DeferredDirectionalLightPixelShader;
		ZEHolder<ZEGRShader>					DeferredProjectiveLightPixelShader;
		ZEHolder<ZEGRShader>					DeferredPointLightPixelShader;
		ZEHolder<ZEGRShader>					DeferredDirectionalLightPixelShaderPerSample;
		ZEHolder<ZEGRShader>					DeferredProjectiveLightPixelShaderPerSample;
		ZEHolder<ZEGRShader>					DeferredPointLightPixelShaderPerSample;
		ZEHolder<ZEGRShader>					EdgeDetectionPixelShader;
		ZEHolder<ZEGRShader>					BlendPixelShader;
		ZEHolder<ZEGRShader>					BlendPixelShaderPerSample;
		ZEHolder<ZEGRShader>					TiledDeferredComputeShader;

		ZEHolder<ZEGRRenderStateData>			DeferredDirectionalLightRenderState;
		ZEHolder<ZEGRRenderStateData>			DeferredPointLightRenderState;
		ZEHolder<ZEGRRenderStateData>			DeferredProjectiveLightRenderState;
		ZEHolder<ZEGRRenderStateData>			DeferredDirectionalLightRenderStatePerSample;
		ZEHolder<ZEGRRenderStateData>			DeferredPointLightRenderStatePerSample;
		ZEHolder<ZEGRRenderStateData>			DeferredProjectiveLightRenderStatePerSample;
		ZEHolder<ZEGRRenderStateData>			EdgeDetectionRenderState;
		ZEHolder<ZEGRRenderStateData>			BlendRenderState;
		ZEHolder<ZEGRRenderStateData>			BlendRenderStatePerSample;
		ZEHolder<ZEGRComputeRenderStateData>	TiledDeferredComputeRenderState;

		ZEHolder<ZEGRBuffer>					ConstantBuffer;
		ZEHolder<ZEGRBuffer>					PointLightStructuredBuffer;
		ZEHolder<ZEGRBuffer>					ProjectiveLightStructuredBuffer;
		ZEHolder<ZEGRBuffer>					DeferredLightVertexBuffer;

		ZEHolder<ZEGRTexture>					RandomVectorsTexture;
		ZEHolder<ZEGRTexture>					TiledDeferredOutputTexture;

		ZEHolder<const ZEGRTexture>				GBufferEmissive;
		ZEHolder<const ZEGRTexture>				GBufferDiffuse;
		ZEHolder<const ZEGRTexture>				GBufferNormal;
		ZEHolder<const ZEGRTexture>				DepthTexture;
		ZEHolder<const ZEGRTexture>				ProjectiveShadowMaps;
		ZEHolder<const ZEGRTexture>				AccumulationTexture;

		bool									ShowCascades;
		bool									UseTiledDeferred;

		struct PointLightStruct
		{
			ZEVector3							PositionView;
			float								Range;

			ZEVector3							Color;
			ZEInt								Type;

			ZEVector3							Attenuation;
			ZEBool32							CastShadow;

			ZEMatrix4x4							WorldMatrix;
		};

		struct ProjectiveLightStruct
		{
			ZEVector3							PositionView;
			float								Range;

			ZEVector3							CullPositionView;
			float								CullRange;

			ZEVector3							Color;
			ZEUInt								ShadowMapIndex;

			ZEVector3  							Attenuation;
			ZEBool32							CastShadow;

			ZEMatrix4x4							WorldMatrix;
			ZEMatrix4x4							ProjectionTransform;

			ZEUInt								ShadowSampleCount;
			float								ShadowSampleLength;
			float								ShadowDepthBias;
			float								ShadowNormalBias;
		};

		struct CascadeStruct
		{
			ZEMatrix4x4							ViewTransform;
			ZEMatrix4x4							ProjectionTransform;

			ZEUInt								SampleCount;
			float								SampleLength;
			float								DepthBias;
			float								NormalBias;
		};

		struct DirectionalLightStruct
		{
			ZEVector3							DirectionView;
			ZEBool32							CastShadow;

			ZEVector3							Color;
			ZEUInt								CascadeCount;

			CascadeStruct						Cascades[4];
		};

		struct
		{
			DirectionalLightStruct				DirectionalLights[2];
			ZEUInt								DirectionalLightCount;
			ZEUInt								PointLightCount;
			ZEUInt								ProjectiveLightCount;
			ZEUInt								TileCountX;

			ZEBool32							AddTiledDeferredOutput;
			ZEVector3							Reserved;
		} Constants;

		ZESmartArray<PointLightStruct>			PointLights;
		ZESmartArray<ProjectiveLightStruct>		ProjectiveLights;

		const ZEGRTexture*						DirectionalLightShadowMap;
		const ZEGRTexture*						ProjectiveLightTexture;

		void									CreateRandomVectors();
		void									CreateLightGeometries();

		bool									UpdateShaders();
		bool									UpdateRenderState();
		bool									UpdateLightBuffers();
		bool									Update();

		virtual bool							InitializeInternal();
		virtual bool							DeinitializeInternal();

		void									DrawLights(ZEGRContext* Context, bool PerSample);
		void									BlendTiledDeferred(ZEGRContext* Context, const ZEGRRenderTarget* RenderTarget, bool PerSample);

	public:
		virtual ZEInt							GetId() const;
		virtual const ZEString&					GetName() const;

		void									SetShowCascades(bool ShowCascades);
		bool									GetShowCascades() const;

		void									SetUseTiledDeferred(bool UseTiledDeferred);
		bool									GetUseTiledDeferred() const;

		virtual bool							Setup(ZEGRContext* Context);
		virtual void							CleanUp(ZEGRContext* Context);

												ZERNStageLighting();
		virtual									~ZERNStageLighting();
};
