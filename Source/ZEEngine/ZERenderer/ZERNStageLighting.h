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
#include "ZEDS/ZEList2.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEGraphics/ZEGRViewport.h"

class ZEGRShader;
class ZEGRSampler;
class ZEGRRenderStateData;
class ZEGRComputeRenderStateData;
class ZEGRConstantBuffer;
class ZEGRStructuredBuffer;
class ZEGRVertexBuffer;
class ZEGRRenderTarget;
class ZEGRTexture2D;
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
		ZEHolder<ZEGRShader>					DeferredVertexShader;
		ZEHolder<ZEGRShader>					DeferredPixelShader;
		ZEHolder<ZEGRShader>					DeferredPixelShaderPerSample;
		ZEHolder<ZEGRShader>					TiledDeferredComputeShader;
		ZEHolder<ZEGRShader>					AccumulateEmissivePixelShader;
		ZEHolder<ZEGRShader>					EdgeDetectionPixelShader;

		ZEHolder<ZEGRRenderStateData>			DeferredRenderState;
		ZEHolder<ZEGRRenderStateData>			DeferredRenderStatePerSample;
		ZEHolder<ZEGRComputeRenderStateData>	TiledDeferredComputeRenderState;
		ZEHolder<ZEGRRenderStateData>			AccumulateEmissiveRenderState;
		ZEHolder<ZEGRRenderStateData>			EdgeDetectionRenderState;

		ZEHolder<ZEGRConstantBuffer>			DeferredLightConstantBuffer;
		ZEHolder<ZEGRVertexBuffer>				DeferredLightVertexBuffer;
		ZEHolder<ZEGRStructuredBuffer>			TiledDeferredCullableLightStructuredBuffer;
		ZEHolder<ZEGRConstantBuffer>			TiledDeferredConstantBuffer;
		ZEHolder<ZEGRConstantBuffer>			CascadeConstantBuffer;

		ZEHolder<ZEGRSampler>					SamplerLinearBorder;
		ZEHolder<ZEGRSampler>					SamplerComparisonLinearPointClamp;
		ZEHolder<ZEGRSampler>					SamplerPointWrap;

		ZEHolder<ZEGRTexture2D>					MultipleShadowMaps;
		ZEHolder<ZEGRTexture2D>					RandomVectorsTexture;

		ZEList2<ZELight>						DeferredLightList;
		ZEList2<ZELight>						TiledDeferredLightList;

		const ZEGRTexture2D*					AccumulationTexture;
		const ZEGRTexture2D*					DepthTexture;

		bool									ShowCascades;

		struct LightStruct
		{
			ZEVector3							PositionView;
			float								Range;

			ZEVector3							Color;
			float								Reserved0;

			ZEVector3							Attenuation;
			ZEBool32							CastShadow;

			ZEVector3							DirectionView;
			ZEInt								Type;

			ZEUInt								ShadowSampleCount;
			float								ShadowSampleLength;
			float								ShadowDepthBias;
			float								ShadowNormalBias;
		};

		struct CascadeStruct
		{
			ZEMatrix4x4							ViewTransform;
			ZEMatrix4x4							ProjectionTransform;
			float								DepthBias;
			float								NormalBias;
			ZEVector2							Reserved;
		};

		struct DeferredLightCascadeConstansStruct
		{
			CascadeStruct						Cascades[4];
			ZEUInt								CascadeCount;
			ZEVector3							Reserved;
		} CascadeConstants;

		struct DeferredLightConstantsStruct
		{
			LightStruct							Light[2];
			ZEMatrix4x4							ProjectionMatrix;
			ZEMatrix4x4							WorldMatrix;
		};

		struct TiledDeferredCullableLightStruct
		{
			LightStruct							Light;
			float								CullRange;
			ZEVector3							CullPositionView;
			ZEMatrix4x4							ProjectionTransform;
		};

		struct
		{
			LightStruct							DirectionalLights[2];

			ZEUInt								CullableLightCount;
			ZEUInt								DirectionalLightCount;
			ZEVector2							Reserved;
		} TiledDeferredConstants;

		void									CreateRandomVectors();
		void									CreateLightGeometries();
		void									CreateSamplers();

		bool									UpdateRenderState();
		bool									UpdateShaders();
		bool									UpdateInputOutputs();
		bool									Update();

		bool									SetupDeferred(ZEGRContext* Context);
		bool									SetupComputeTiledDeferred(ZEGRContext* Context);

		void									DrawDirectionalLight(ZELightDirectional* DirectionalLight, ZEGRContext* Context);
		void									DrawProjectiveLight(ZELightProjective* ProjectiveLight, ZEGRContext* Context);
		void									DrawPointLight(ZELightPoint* PointLight, ZEGRContext* Context);

		virtual bool							InitializeSelf();
		virtual void							DeinitializeSelf();

	public:
		virtual ZEInt							GetId() const;
		virtual const ZEString&					GetName() const;

		void									SetShowCascades(bool ShowCascades);
		bool									GetShowCascades() const;

		virtual const ZEGRTexture2D*			GetOutput(ZERNStageBuffer Output) const;

		virtual bool							Setup(ZEGRContext* Context);
		virtual void							CleanUp(ZEGRContext* Context);

												ZERNStageLighting();
};
