//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageAntiAliasing.h
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
#include "ZEMath/ZEVector.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEGraphics/ZEGRViewport.h"

class ZEGRTexture;
class ZEGRRenderTarget;
class ZEGRRenderStateData;
class ZEGRShader;
class ZEGRSampler;
class ZEGRContext;
class ZEGRBuffer;
class ZERNRenderer;

class ZERNStageAntiAliasing  : public ZERNStage
{
	ZE_OBJECT
	private:
		ZEFlags								DirtyFlags;
		bool								TemporalEnabled;

		ZEHolder<ZEGRShader>				EdgeDetectionVertexShader;
		ZEHolder<ZEGRShader>				EdgeDetectionPixelShader;
		ZEHolder<ZEGRShader>				BlendingWeightCalculationVertexShader;
		ZEHolder<ZEGRShader>				BlendingWeightCalculationPixelShader;
		ZEHolder<ZEGRShader>				NeighborhoodBlendingVertexShader;
		ZEHolder<ZEGRShader>				NeighborhoodBlendingPixelShader;
		ZEHolder<ZEGRShader>				ScreenCoverPositionTexcoordVertexShader;
		ZEHolder<ZEGRShader>				GenerateVelocityBufferPixelShader;
		ZEHolder<ZEGRShader>				ReprojectPixelShader;

		ZEHolder<ZEGRRenderStateData>		EdgeDetectionPassRenderStateData;
		ZEHolder<ZEGRRenderStateData>		BlendingWeightCalculationPassRenderStateData;
		ZEHolder<ZEGRRenderStateData>		NeighborhoodBlendingPassRenderStateData;
		ZEHolder<ZEGRRenderStateData>		GenerateVelocityBufferRenderStateData;
		ZEHolder<ZEGRRenderStateData>		ReprojectRenderStateData;

		ZEHolder<ZEGRBuffer>				ConstantBuffer;

		ZEHolder<ZEGRTexture>				EdgeTexture;
		ZEHolder<ZEGRTexture>				BlendTexture;
		ZEHolder<ZEGRTexture>				AreaTexture;
		ZEHolder<ZEGRTexture>				SearchTexture;
		
		ZEHolder<ZEGRTexture>				ColorTextures[2];
		ZEHolder<ZEGRTexture>				VelocityBuffer;

		ZEHolder<ZEGRSampler>				SamplerLinearClamp;
		ZEHolder<ZEGRSampler>				SamplerPointClamp;

		ZEGRViewport						Viewport;

		struct SMAAConstants
		{
			ZEVector2						OutputSize;
			ZEVector2						InvOutputSize;

			ZEVector4						SubsampleIndices;
		} Constants;

		ZEHolder<const ZEGRTexture>			InputTexture;
		ZEHolder<const ZEGRTexture>			DepthTexture;
		ZEHolder<const ZEGRTexture>			OutputTexture;

		const ZEGRRenderTarget*				EdgeRenderTarget;
		const ZEGRRenderTarget*				BlendRenderTarget;

		bool								UpdateShaders();
		bool								UpdateRenderStates();
		bool								UpdateTextures();
		bool								UpdateConstantBuffers();
		bool								Update();

		void								ClearTextures(ZEGRContext* Context);
		void								DoEdgeDetection(ZEGRContext* Context);
		void								DoBlendingWeightCalculation(ZEGRContext* Context);
		void								DoNeighborhoodBlending(ZEGRContext* Context);
		void								GenerateVelocityBuffer(ZEGRContext* Context);
		void								DoReprojection(ZEGRContext* Context);

		virtual bool						InitializeInternal();						
		virtual bool						DeinitializeInternal();

		virtual void						CreateOutput(const ZEString& Name);

	public:
		virtual ZEInt						GetId() const;
		virtual const ZEString&				GetName() const;

		void								SetTemporalEnabled(bool Enabled);
		bool								GetTemporalEnabled() const;

		virtual void						Resized(ZEUInt Width, ZEUInt Height);

		virtual bool						Setup(ZEGRContext* Context);
		virtual void						CleanUp(ZEGRContext* Context);

											ZERNStageAntiAliasing();
		virtual								~ZERNStageAntiAliasing();
};
