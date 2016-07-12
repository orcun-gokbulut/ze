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
#include "ZEGraphics/ZEGRViewport.h"
#include "ZERNFilter.h"

class ZEGRShader;
class ZEGRSampler;
class ZEGRTexture2D;
class ZEGRConstantBuffer;
class ZEGRContext;
class ZEGRRenderStateData;
class ZERNRenderer;
class ZEGRRenderTarget;

ZE_ENUM(ZERNHDRToneMapOperator)
{
	ZERN_HTMO_LOGARITMIC			= 0,
	ZERN_HTMO_EXPONENTIAL			= 1,
	ZERN_HTMO_REINHARD				= 2,
	ZERN_HTMO_MODIFIED_REINHARD		= 3,
	ZERN_HTMO_FILMIC				= 4,
	ZERN_HTMO_UNCHARTED				= 5
};

ZE_ENUM(ZERNHDRBlurTextureSize)
{
	ZERN_HBTS_EXACT					= 0,
	ZERN_HBTS_HALF					= 1,
	ZERN_HBTS_QUARTER				= 2
};

class ZERNStageHDR : public ZERNStage
{
	ZE_OBJECT
	private:
		ZEFlags								DirtyFlags;
		ZERNFilter							Filter;
		ZEGRViewport						Viewport;

		ZEHolder<ZEGRShader>				ScreenCoverPositionTexcoordVertexShader;
		ZEHolder<ZEGRShader>				CalculateLuminancePixelShader;
		ZEHolder<ZEGRShader>				CalculateAdaptedLuminancePixelShader;
		ZEHolder<ZEGRShader>				CalculateBrightnessPixelShader;
		ZEHolder<ZEGRShader>				ToneMappingPixelShader;

		ZEHolder<ZEGRRenderStateData>		CalculateLuminanceRenderStateData;
		ZEHolder<ZEGRRenderStateData>		CalculateAdaptedLuminanceRenderStateData;
		ZEHolder<ZEGRRenderStateData>		CalculateBrightnessRenderStateData;
		ZEHolder<ZEGRRenderStateData>		ToneMappingRenderStateData;

		ZEHolder<ZEGRConstantBuffer>		ConstantBuffer;

		ZEHolder<ZEGRTexture2D>				OutputTexture;
		ZEHolder<ZEGRTexture2D>				BrightTexture;
		ZEHolder<ZEGRTexture2D>				DownScaledTexture4x;
		ZEHolder<ZEGRTexture2D>				DownScaledTexture8x;
		ZEArray<ZEHolder<ZEGRTexture2D>>	LuminanceTextures;
		ZEHolder<ZEGRTexture2D>				CurrentAdaptedLuminance;
		ZEHolder<ZEGRTexture2D>				PreviousAdaptedLuminance;

		ZERNHDRBlurTextureSize				BlurTextureSize;

		const ZEGRTexture2D*				InputTexture;
		const ZEGRRenderTarget*				OutputRenderTarget;

		struct
		{	
			float							Key;
			float							WhiteLevel;
			float							BloomFactor;
			float							BloomThreshold;

			ZEBool32						AutoKeyEnabled;
			ZEUInt							ToneMapOperator;
			ZEBool32						BloomEnabled;
			float							Saturation;

			float							LuminanceMin;
			float							LuminanceMax;
			float							Reserved1;
			float							Reserved2;
		} Constants;

		virtual bool						InitializeInternal();
		virtual bool						DeinitializeInternal();

		bool								UpdateInputOutput();
		bool								UpdateTextures();
		bool								UpdateShaders();
		bool								UpdateRenderStates();
		bool								UpdateConstantBuffer();
		bool								Update();

		void								CalculateAverageLuminance(ZEGRContext* Context);
		void								CalculateAdaptedLuminance(ZEGRContext* Context);
		const ZEGRTexture2D*				CalculateBloom(ZEGRContext* Context);
		void								ToneMapping(ZEGRContext* Context, const ZEGRTexture2D* BloomTexture);

	public:
		virtual ZEInt						GetId() const;
		virtual const ZEString&				GetName() const;

		void								SetKey(float Key);
		float								GetKey() const;

		void								SetAutoKey(bool AutoKeyEnabled);
		bool								GetAutoKey() const;

		void								SetWhiteLevel(float WhiteLevel);
		float								GetWhiteLevel() const;

		void								SetBloomEnabled(bool BloomEnabled);
		bool								GetBloomEnabled() const;

		void								SetBloomFactor(float BloomFactor);
		float								GetBloomFactor() const;

		void								SetBloomThreshold(float BloomThreshold);
		float								GetBloomThreshold() const;

		void								SetLuminanceMin(float LuminanceMin);
		float								GetLuminanceMin() const;

		void								SetLuminanceMax(float LuminanceMax);
		float								GetLuminanceMax() const;

		void								SetSaturation(float Saturation);
		float								GetSaturation() const;

		void								SetToneMapOperator(ZERNHDRToneMapOperator Operator);
		ZERNHDRToneMapOperator				GetToneMapOperator() const;

		void								SetBlurTextureSize(ZERNHDRBlurTextureSize Value);
		ZERNHDRBlurTextureSize				GetBlurTextureSize() const;

		virtual const ZEGRRenderTarget*		GetProvidedInput(ZERNStageBuffer Output) const;
		virtual const ZEGRTexture2D*		GetOutput(ZERNStageBuffer Output) const;

		virtual bool						Setup(ZEGRContext* Context);
		virtual void						CleanUp(ZEGRContext* Context);

											ZERNStageHDR();
};
