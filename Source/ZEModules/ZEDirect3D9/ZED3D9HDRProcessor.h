//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9HDRProcessor.h
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
#ifndef __ZE_D3D9_HDR_PROCESSOR_H__
#define __ZE_D3D9_HDR_PROCESSOR_H__

#include "ZETypes.h"
#include "ZED3D9ComponentBase.h"
#include "ZEMeta/ZEObject.h"
#include "ZEDS/ZEArray.h"

#include <d3d9.h>

class ZETexture2D;
class ZED3D9ViewPort;
class ZED3D9Texture2D;
class ZED3D9PixelShader;
class ZED3D9VertexShader;
class ZED3D9FrameRenderer;

struct ZEHDRScreenAlignedQuad
{
	float	Position[3];
	float	TexCoord[2];
};

enum ZEHDRToneMapOperator
{
	ZE_HDR_TMO_LOGARITHMIC		= 0,
	ZE_HDR_TMO_EXPONENTIAL		= 1,
	ZE_HDR_TMO_REINHARD			= 2,
	ZE_HDR_TMO_REINHARD_MOD		= 3,
	ZE_HDR_TMO_FILMIC			= 4
};

class ZED3D9HDRProcessor : public ZED3D9ComponentBase, public ZEObject
{
	ZE_OBJECT

	private:
		ZED3D9FrameRenderer*			Renderer;
		ZED3D9Texture2D*				InputBuffer;
		ZED3D9ViewPort*					OutputBuffer;
		
		ZEUInt							OutputWidth;
		ZEUInt							OutputHeight;

		ZED3D9Texture2D*				Black1x1;
		ZED3D9Texture2D*				CurrentLuminance;
		ZED3D9Texture2D*				PreviousLuminance;

	public:
		ZEArray<ZED3D9Texture2D*>		BloomLevels;
		ZEArray<ZED3D9Texture2D*>		BloomLevelsTemp;
		ZEArray<ZED3D9Texture2D*>		LuminanceMips;
	
	private:
		struct
		{
			bool						Recompile;
			ZED3D9VertexShader*			Vertex;
			ZED3D9PixelShader*			ConvertToLuminance;
			ZED3D9PixelShader*			LuminanaceScale3x;
			ZED3D9PixelShader*			LuminanceAdaptation;
			ZED3D9PixelShader*			BrightPass;
			ZED3D9PixelShader*			ColorDownSample2x;
			ZED3D9PixelShader*			BlurHorizontal;
			ZED3D9PixelShader*			BlurVerticalUpSample2x;
			ZED3D9PixelShader*			Combine;
		
		} Shaders;

		static ZEHDRScreenAlignedQuad	Vertices[4];
		LPDIRECT3DVERTEXDECLARATION9	VertexDeclaration;


		bool							AutoKey;
		bool							AutoExposure;
		bool							BloomEnabled;

		float							Key;
		float							Exposure;
		float							WhiteLevel;
		float							Saturation;
		float							LuminanceUpper;
		float							LuminanceLower;
		float							AdaptationRate;
		ZEHDRToneMapOperator			ToneMapOperator;

		float							BloomFactor;
		float							BloomTreshold;
		float							BloomDeviation;
		float							BloomWeightLarge;
		float							BloomWeightMedium;
		float							BloomWeightSmall;
		ZEUInt							BloomPassCount; // Not active yet

		void							UpdateBuffers(ZEUInt Width, ZEUInt Height);
		void							DestroyBuffers();

		void							UpdateShaders();
		void							DestroyShaders();

		void							DebugPrint(const ZED3D9Texture2D* Input, ZED3D9ViewPort* Output);

		void							ColorDownSample2x(const ZED3D9Texture2D* Input, ZED3D9Texture2D* Output);
		void							LuminanaceScale3x(ZED3D9Texture2D* Input, ZED3D9Texture2D* Output);

		void							HorizontalBlur(ZED3D9Texture2D* Input, ZED3D9Texture2D* Output);
		void							VerticalBlurAdditiveUpSample2x(ZED3D9Texture2D* Input, float BloomWeight, ZED3D9Texture2D* Addition, ZED3D9Texture2D* Output);
		
		void							ConvertToLuminance(ZED3D9Texture2D* Input, ZED3D9Texture2D* Output);
		void							MipMapLuminance(ZEArray<ZED3D9Texture2D*>& Input);
		void							AdaptLuminance(ZED3D9Texture2D* MeasuredLum, ZED3D9Texture2D* PreviousLum, ZED3D9Texture2D* Output);

		void							BrightPass(ZED3D9Texture2D* Color, ZED3D9Texture2D* CurrentLum, ZED3D9Texture2D* Output);
		void							GenerateBloom(ZED3D9Texture2D* InputOutput, ZEUInt PassCount);

		void							Combine(ZED3D9Texture2D* Color, ZED3D9Texture2D* Bloom, ZED3D9Texture2D* CurrentLum, ZED3D9ViewPort* Output);
		
		void							CommitConstants(float ElapsedTime);
		void							SwitchLuminanceBuffers();

	public:
		void							Initialize();
		void							Deinitialize();

		void							SetKey(float Value);
		float							GetKey() const;

		void							SetAutoExposure(bool Enabled);
		bool							GetAutoExposure() const;

		void							SetBloomEnabled(bool Enabled);
		bool							GetBloomEnabled() const;

		void							SetBloomWeightLarge(float Value);
		float							GetBloomWeightLarge() const;

		void							SetBloomWeightMedium(float Value);
		float							GetBloomWeightMedium() const;
		
		void							SetBloomWeightSmall(float Value);
		float							GetBloomWeightSmall() const;

		void							SetAutoKey(bool Enabled);
		bool							GetAutoKey() const;

		void							SetExposure(float Value);
		float							GetExposure() const;

		void							SetAdaptationRate(float Value);
		float							GetAdaptationRate() const;
		
		void							SetBloomFactor(float Value);
		float							GetBloomFactor() const;

		void							SetBloomTreshold(float Value);
		float							GetBloomTreshold() const;

		void							SetBloomDeviation(float Value);
		float							GetBloomDeviation() const;

		void							SetWhiteLevel(float Value);
		float							GetWhiteLevel() const;

		void							SetSaturation(float Value);
		float							GetSaturation() const;

		void							SetLuminanceUpper(float Value);
		float							GetLuminanceUpper() const;

		void							SetLuminanceLower(float Value);
		float							GetLuminanceLower() const;

		void							SetBloomPassCount(ZEUInt Value);
		ZEUInt							GetBloomPassCount() const;

		void							SetToneMapOperator(ZEHDRToneMapOperator Operator);
		ZEHDRToneMapOperator			GetToneMapOperator() const;

		void							SetInput(ZED3D9Texture2D* Input);
		ZED3D9Texture2D*				GetInput();

		void							SetOutput(ZED3D9ViewPort* Output);
		ZED3D9ViewPort*					GetOutput();

		void							SetRenderer(ZED3D9FrameRenderer* FrameRenderer);
		ZED3D9FrameRenderer*			SetRenderer() const;

		void							Process(float ElapsedTime);

										ZED3D9HDRProcessor();
										~ZED3D9HDRProcessor();
};

#endif
