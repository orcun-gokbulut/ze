//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageHDR.h
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

#include "ZEDS\ZEArray.h"
#include "ZEDS\ZEFlags.h"
#include "ZEGraphics\ZEGRHolder.h"
#include "ZERNFilter.h"
#include "ZEGraphics/ZEGRSamplerState.h"

class ZEGRShader;
class ZEGRTexture2D;
class ZEGRConstantBuffer;
class ZEGRContext;
class ZEGRRenderStateData;
class ZEGRRenderState;
class ZERNRenderer;
class ZEGRRenderTarget;

enum ZERNHDRToneMapOperator
{
	ZERN_HTMO_LOGARITMIC			= 0,
	ZERN_HTMO_EXPONENTIAL			= 1,
	ZERN_HTMO_REINHARD				= 2,
	ZERN_HTMO_MODIFIED_REINHARD		= 3,
	ZERN_HTMO_FILMIC				= 4,
	ZERN_HTMO_UNCHARTED				= 5
};

enum ZERNHDRBlurTextureSize
{
	ZERN_HBTS_EXACT					= 0,
	ZERN_HBTS_HALF					= 1,
	ZERN_HBTS_QUARTER				= 2
};

class ZERNStageHDR : public ZERNStage
{
	private:
		ZEFlags								DirtyFlags;

		ZEGRHolder<ZEGRShader>				VertexShader;
		ZEGRHolder<ZEGRShader>				CalculateLuminance_PixelShader;
		ZEGRHolder<ZEGRShader>				CalculateAdaptedLuminance_PixelShader;
		ZEGRHolder<ZEGRShader>				DownSampling_PixelShader;
		ZEGRHolder<ZEGRShader>				CalculateBrightness_PixelShader;
		ZEGRHolder<ZEGRShader>				ToneMapping_PixelShader;

		ZEGRHolder<ZEGRRenderStateData>		CalculateLuminance_RenderState;
		ZEGRHolder<ZEGRRenderStateData>		CalculateAdaptedLuminance_RenderState;
		ZEGRHolder<ZEGRRenderStateData>		DownSampling_RenderState;
		ZEGRHolder<ZEGRRenderStateData>		CalculateBrightness_RenderState;
		ZEGRHolder<ZEGRRenderStateData>		ToneMapping_RenderState;

		ZEGRTexture2D*						InputTexture;
		ZEGRRenderTarget*					OutputRenderTarget;

		ZERNFilter							Filter;
		ZEArray<ZEVector4>					HorizontalValues;
		ZEArray<ZEVector4>					VerticalValues;

		ZEGRHolder<ZEGRTexture2D>			BlurTextureTemp1;
		ZEGRHolder<ZEGRTexture2D>			BlurTextureTemp2;
		ZEGRHolder<ZEGRTexture2D>			BlurTextureFinal;
		ZEGRHolder<ZEGRTexture2D>			BrightTexture;
		ZEArray<ZEGRHolder<ZEGRTexture2D>>	LuminanceMips;
		ZEGRHolder<ZEGRTexture2D>			CurrentAdaptedLuminance;
		ZEGRHolder<ZEGRTexture2D>			PreviousAdaptedLuminance;
		
		ZEGRSamplerState					SamplerLinearClamp;

		ZEUInt								PrevWidth;
		ZEUInt								PrevHeight;

		ZERNHDRBlurTextureSize				BlurTextureSize;

		struct HDRConstants
		{	
			float							Key;
			float							WhiteLevel;
			float							BloomFactor;
			float							BloomThreshold;

			bool							AutoKey;
			ZEUInt							ToneMapOperator;
			bool							BloomEnabled;
			ZEUInt							Reserved;
		} Constants;

		ZEGRHolder<ZEGRConstantBuffer>		ConstantBuffer;

	private:

		bool								UpdateTextures();
		bool								UpdateShaders();
		bool								UpdateRenderStates();
		bool								UpdateConstantBuffer();
		bool								Update();

		void								DownSample(ZEGRContext* Context, ZEGRTexture2D* Input, ZEGRRenderTarget* Output);
		void								CalculateLuminance(ZEGRContext* Context, ZEGRTexture2D* Input, ZEGRRenderTarget* Output);
		void								CalculateAdaptedLuminance(ZEGRContext* Context);
		void								GenerateMipMaps(ZEGRContext* Context);
		void								CalculateBrightness(ZEGRContext* Context, ZEGRTexture2D* Input, ZEGRRenderTarget* Output);
		void								ApplyBlur(ZEGRContext* Context, ZEGRTexture2D* Input, ZEGRRenderTarget* Output);
		void								ToneMapping(ZEGRContext* Context, ZEGRTexture2D* Input, ZEGRRenderTarget* Output);

		bool								SetupInputOutput(ZERNRenderer* Renderer);

		virtual bool						InitializeSelf();						
		virtual void						DeinitializeSelf();

	public:
		virtual ZEInt						GetId();
		virtual const ZEString&				GetName();

		void								SetKey(float Value);
		float								GetKey() const;

		void								SetAutoKey(bool Enabled);
		bool								GetAutoKey() const;

		void								SetWhiteLevel(float Value);
		float								GetWhiteLevel() const;

		void								SetBloomEnabled(bool Enabled);
		bool								GetBloomEnabled() const;

		void								SetBloomFactor(float Value);
		float								GetBloomFactor() const;

		void								SetBloomThreshold(float Value);
		float								GetBloomThreshold() const;

		void								SetToneMapOperator(ZERNHDRToneMapOperator Operator);
		ZERNHDRToneMapOperator				GetToneMapOperator() const;

		void								SetInput(ZEGRTexture2D* Input);
		ZEGRTexture2D*						GetInput() const;

		void								SetOutput(ZEGRRenderTarget* Output);
		ZEGRRenderTarget*					GetOutput() const;

		void								SetBlurTextureSize(ZERNHDRBlurTextureSize Value);
		ZERNHDRBlurTextureSize				GetBlurTextureSize() const;

		virtual bool						Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands);
		virtual void						CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context);

											ZERNStageHDR();
};
