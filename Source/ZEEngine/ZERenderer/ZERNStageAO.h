//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageAO.h
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

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEFlags.h"
#include "ZEMath/ZEVector.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZERNFilter.h"

class ZEGRShader;
class ZEGRSampler;
class ZEGRTexture2D;
class ZEGRRenderTarget;
class ZEGRConstantBuffer;
class ZEGRRenderStateData;
class ZEGRDepthStencilBuffer;

ZE_ENUM(ZERNSSAOSampleCount)
{
	ZERN_AOSC_LOW		= 0,
	ZERN_AOSC_MEDIUM	= 1,
	ZERN_AOSC_HIGH		= 2
};

class ZERNStageAO : public ZERNStage
{
	ZE_OBJECT
	private:
		ZEFlags								DirtyFlags;
		ZERNFilter							Filter;
		ZEGRViewport						Viewport;
		ZERNSSAOSampleCount					SampleCount;
		ZEArray<ZEVector4>					RandomVectors;

		ZEHolder<ZEGRShader>				ScreenCoverPositionTexCoordVertexShader;

		ZEHolder<ZEGRShader>				ResolveAndClampDepthPixelShader;
		ZEHolder<ZEGRShader>				SSAOPixelShader;

		ZEHolder<ZEGRShader>				ResolveAndLinearizeDepthPixelShader;
		ZEHolder<ZEGRShader>				DeinterleaveDepthPixelShader;
		ZEHolder<ZEGRShader>				DeinterleaveSSAOPixelShader;
		ZEHolder<ZEGRShader>				ReinterleaveSSAOPixelShader;

		ZEHolder<ZEGRShader>				CrossBilateralBlurXPixelShader;
		ZEHolder<ZEGRShader>				CrossBilateralBlurYPixelShader;

		ZEHolder<ZEGRRenderStateData>		ResolveAndClampDepthRenderStateData;
		ZEHolder<ZEGRRenderStateData>		SSAORenderStateData;

		ZEHolder<ZEGRRenderStateData>		ResolveAndLinearizeDepthRenderStateData;
		ZEHolder<ZEGRRenderStateData>		DeinterleaveDepthRenderStateData;
		ZEHolder<ZEGRRenderStateData>		DeinterleaveSSAORenderStateData;
		ZEHolder<ZEGRRenderStateData>		ReinterleaveSSAORenderStateData;

		ZEHolder<ZEGRRenderStateData>		CrossBilateralBlurXRenderStateData;
		ZEHolder<ZEGRRenderStateData>		CrossBilateralBlurYRenderStateData;

		ZEHolder<ZEGRConstantBuffer>		ConstantBuffer;
		ZEHolder<ZEGRConstantBuffer>		DeinterleavedConstantBuffer;

		ZEHolder<ZEGRTexture2D>				AmbientOcclusionTexture;
		ZEHolder<ZEGRTexture2D>				BlurTempTexture;
		ZEHolder<ZEGRTexture2D>				RandomVectorsTexture;
		ZEHolder<ZEGRTexture2D>				ResolvedDepthTexture;
		ZEHolder<ZEGRTexture2D>				DeinterleavedDepthtexture;
		ZEHolder<ZEGRTexture2D>				DeinterleavedAmbientOcclusionTexture;

		bool								UseDeinterleavedTexturing;

		const ZEGRTexture2D*				DepthTexture;
		const ZEGRTexture2D*				NormalTexture;
		const ZEGRTexture2D*				AccumulationTexture;

		struct
		{
			ZEVector4						SphereSamples[32];
			ZEUInt							SampleCount;
			float							OcclusionRadius;
			float							NormalBias;
			float							Intensity;

			ZEVector2						WidthHeight;
			ZEVector2						InvWidthHeight;

			ZEUInt							KernelRadius;
			float							BlurSharpness;
			float							DistanceThreshold;
			float							Reserved0;
		} Constants;

		struct
		{
			ZEVector3						RandomVector;
			ZEUInt							DepthArrayIndex;

			ZEVector2						Offset;
			ZEVector2						Reserved;
		} DeinterleavedConstants;

		void								CreateRandomVectors();
		void								CreateSphereSamples();

		virtual bool						InitializeSelf();
		virtual bool						DeinitializeSelf();

		bool								UpdateShaders();
		bool								UpdateRenderStates();
		bool								UpdateConstantBuffers();
		bool								UpdateInputOutputs();
		bool								UpdateTextures();
		bool								Update();

		void								ResolveAndClampDepth(ZEGRContext* Context);
		void								AmbientOcclusion(ZEGRContext* Context, const ZEGRDepthStencilBuffer* DepthStencilBuffer);
		void								ResolveAndLinearizeDepth(ZEGRContext* Context);
		void								DeinterleaveDepth(ZEGRContext* Context);
		void								DeinterleaveAmbientOcclusion(ZEGRContext* Context);
		void								ReinterleaveAmbientOcclusion(ZEGRContext* Context);
		void								ApplyBlur(ZEGRContext* Context, const ZEGRTexture2D* InputTexture, const ZEGRDepthStencilBuffer* DepthStencilBuffer);

	public:
		virtual ZEInt						GetId() const;
		virtual const ZEString&				GetName() const;

		void								SetSampleCount(ZERNSSAOSampleCount SampleCount);
		ZERNSSAOSampleCount					GetSampleCount() const;

		void								SetOcclusionRadius(float Radius);
		float								GetOcclusionRadius() const;

		void								SetNormalBias(float NormalBias);
		float								GetNormalBias() const;

		void								SetIntensity(float Intensity);
		float								GetIntensity() const;

		void								SetDistanceThreshold(float DistanceThreshold);
		float								GetDistanceThreshold() const;

		void								SetDeinterleavedTexturing(bool UseDeinterleavedTexturing);
		bool								GetDeinterleavedTexturing() const;

		virtual const ZEGRTexture2D*		GetOutput(ZERNStageBuffer Output) const;

		virtual bool						Setup(ZEGRContext* Context);
		virtual void						CleanUp(ZEGRContext* Context);

											ZERNStageAO();
};
