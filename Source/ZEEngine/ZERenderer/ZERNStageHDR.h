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

#include "ZEDS/ZEArray.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEPointer/ZESharedPointer.h"
#include "ZERNFilter.h"

class ZEGRShader;
class ZEGRSampler;
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

		ZEHolder<ZEGRShader>				VertexShader;
		ZEHolder<ZEGRShader>				CalculateLuminance_PixelShader;
		ZEHolder<ZEGRShader>				CalculateAdaptedLuminance_PixelShader;
		ZEHolder<ZEGRShader>				DownSampling_PixelShader;
		ZEHolder<ZEGRShader>				CalculateBrightness_PixelShader;
		ZEHolder<ZEGRShader>				ToneMapping_PixelShader;

		ZEHolder<ZEGRRenderStateData>		CalculateLuminance_RenderState;
		ZEHolder<ZEGRRenderStateData>		CalculateAdaptedLuminance_RenderState;
		ZEHolder<ZEGRRenderStateData>		DownSampling_RenderState;
		ZEHolder<ZEGRRenderStateData>		CalculateBrightness_RenderState;
		ZEHolder<ZEGRRenderStateData>		ToneMapping_RenderState;

		const ZEGRTexture2D*				InputTexture;
		const ZEGRRenderTarget*				OutputRenderTarget;

		ZERNFilter							Filter;
		ZEArray<ZEVector4>					HorizontalValues;
		ZEArray<ZEVector4>					VerticalValues;

		ZEHolder<ZEGRTexture2D>				BlurTextureTemp1;
		ZEHolder<ZEGRTexture2D>				BlurTextureTemp2;
		ZEHolder<ZEGRTexture2D>				BlurTextureFinal;
		ZEHolder<ZEGRTexture2D>				BrightTexture;
		ZEArray<ZEHolder<ZEGRTexture2D>>	LuminanceMips;
		ZEHolder<ZEGRTexture2D>				CurrentAdaptedLuminance;
		ZEHolder<ZEGRTexture2D>				PreviousAdaptedLuminance;
		
		ZEHolder<ZEGRSampler>			SamplerLinearClamp;

		ZEUInt								PrevWidth;
		ZEUInt								PrevHeight;

		ZERNHDRBlurTextureSize				BlurTextureSize;

		struct
		{	
			float							Key;
			float							WhiteLevel;
			float							BloomFactor;
			float							BloomThreshold;

			ZEBool32						AutoKey;
			ZEUInt							ToneMapOperator;
			ZEBool32						BloomEnabled;
			ZEUInt							Reserved0;

			float							LuminanceMin;
			float							LuminanceMax;
			float							Reserved1;
			float							Reserved2;
		} Constants;

		ZEHolder<ZEGRConstantBuffer>		ConstantBuffer;

	private:

		bool								UpdateTextures();
		bool								UpdateShaders();
		bool								UpdateRenderStates();
		bool								UpdateConstantBuffer();
		bool								Update();

		void								CalculateLuminance(ZEGRContext* Context, const ZEGRTexture2D* Input, const ZEGRRenderTarget* Output);
		void								DownSample(ZEGRContext* Context, const ZEGRTexture2D* Input, const ZEGRRenderTarget* Output);
		void								GenerateMipMaps(ZEGRContext* Context);
		void								CalculateAdaptedLuminance(ZEGRContext* Context);
		void								CalculateBrightness(ZEGRContext* Context, const ZEGRTexture2D* Input, const ZEGRRenderTarget* Output);
		void								ApplyBlur(ZEGRContext* Context, const ZEGRTexture2D* Input, const ZEGRRenderTarget* Output);
		void								ToneMapping(ZEGRContext* Context, const ZEGRTexture2D* Input, const ZEGRRenderTarget* Output);

		bool								SetupInputOutput(ZERNRenderer* Renderer);

		virtual bool						InitializeSelf();						
		virtual void						DeinitializeSelf();

	public:
		virtual ZEInt						GetId() const;
		virtual const ZEString&				GetName() const;

		void								SetKey(float Value);
		float								GetKey() const;

		void								SetAutoKey(ZEBool32 Enabled);
		ZEBool32							GetAutoKey() const;

		void								SetWhiteLevel(float Value);
		float								GetWhiteLevel() const;

		void								SetBloomEnabled(ZEBool32 Enabled);
		ZEBool32							GetBloomEnabled() const;

		void								SetBloomFactor(float Value);
		float								GetBloomFactor() const;

		void								SetBloomThreshold(float Value);
		float								GetBloomThreshold() const;

		void								SetLuminanceMin(float Value);
		float								GetLuminanceMin() const;

		void								SetLuminanceMax(float Value);
		float								GetLuminanceMax() const;

		void								SetToneMapOperator(ZERNHDRToneMapOperator Operator);
		ZERNHDRToneMapOperator				GetToneMapOperator() const;

		void								SetInputTexture(const ZEGRTexture2D* Input);
		const ZEGRTexture2D*				GetInputTexture() const;

		void								SetOutputRenderTarget(const ZEGRRenderTarget* Output);
		const ZEGRRenderTarget*				GetOutputRenderTarget() const;

		void								SetBlurTextureSize(ZERNHDRBlurTextureSize Value);
		ZERNHDRBlurTextureSize				GetBlurTextureSize() const;

		virtual bool						Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands);
		virtual void						CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context);

											ZERNStageHDR();
};
