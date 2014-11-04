//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9HDRProcessor.cpp
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

#include "ZED3D9Module.h"
#include "ZED3D9Shader.h"
#include "ZECore/ZECore.h"
#include "ZEMath/ZEAngle.h"
#include "ZED3D9Texture2D.h"
#include "ZECore/ZEConsole.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9FrameRenderer.h"
#include "ZED3D9HDRProcessor.h"
#include "ZEMath/ZEMath.h"

#define GAUSSIAN_FILTER_WIDTH		21	//9 13 17 21 25 etc

#define COLOR_RED	D3DCOLORWRITEENABLE_RED
#define COLOR_ALL	D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA

static float GausianFunction(float x, float StandartDeviation)
{
	return (1.0f / (ZEMath::Sqrt(2.0f * ZE_PI) * StandartDeviation)) * ZEMath::Power(ZE_E, -((x * x) / (2.0f * StandartDeviation * StandartDeviation)));
}

static float GausianFunction(float x, float y, float StandartDeviation)
{
	return (1.0f / (2.0f * ZE_PI * StandartDeviation * StandartDeviation)) * ZEMath::Power(ZE_E, -((x * x + y * y) / (2.0f * StandartDeviation * StandartDeviation))); 
}

static void GetBilinearTap(const float Tap1, const float Coord1, const float Tap2, const float Coord2, float& TapOutput, float& CoordOutput)
{
	TapOutput = Tap1 + Tap2;
	CoordOutput = (Tap1 * Coord1 + Tap2 * Coord2) / TapOutput;
}

static void FillGaussianKernel(float* Weights, float* Coords, ZEUInt FilterWidth, float StandartDeviation, float PixelSize)
{
	ZESize Width = FilterWidth;
	ZESSize HalfWidth = (Width - 1) / 2;
	
	// Geneate discrete gaussian filter.
	for (ZESSize I = 0; I < FilterWidth; I++)
	{
		float TapCoord = (float)(I - HalfWidth);
		
		Coords[I] = PixelSize * TapCoord;
		Weights[I] = GausianFunction(TapCoord, StandartDeviation);
	}
}

static ZEUInt CalculateLinearFilterWidth(ZEUInt DiscreteFilterWidth)
{
	zeDebugCheck(DiscreteFilterWidth % 2 == 0, "DiscreteFilterWidth Cannot be odd");
	zeDebugCheck((DiscreteFilterWidth / 2) % 2 != 0, "Half filter width must be multiples of two");

	return (DiscreteFilterWidth / 2) + 1;
}

static void GenerateBilinearGaussianKernel(float* DiscreteWeights, float* DiscreteCoords, ZEUInt DiscreteFilterWidth, ZEUInt LinearFilterWidth, float* OutputLinearWeights, float* OutputLinearCoords)
{
	float TapCoord1 = 0.0f, TapWeight1 = 0.0f, TapCoord2 = 0.0f, TapWeight2 = 0.0f;

	ZESize OutputWidth = LinearFilterWidth;
	ZESSize OutputHalfWidth = (OutputWidth - 1) / 2;

	for (ZESSize I = 0; I < OutputHalfWidth; ++I)
	{
		ZESize Index = I * 2;

		float TapCoord1 = DiscreteCoords[Index];
		float TapWeight1 = DiscreteWeights[Index];
		
		float TapCoord2 = DiscreteCoords[Index + 1];
		float TapWeight2 = DiscreteWeights[Index + 1];
		
		GetBilinearTap(TapWeight1, TapCoord1, TapWeight2, TapCoord2, OutputLinearWeights[I], OutputLinearCoords[I]);

		ZESize OppositeIndex = DiscreteFilterWidth - 1 - Index;

		TapCoord1 = DiscreteCoords[OppositeIndex - 1];
		TapWeight1 = DiscreteWeights[OppositeIndex - 1];
		
		TapCoord2 = DiscreteCoords[OppositeIndex];
		TapWeight2 = DiscreteWeights[OppositeIndex];
		
		GetBilinearTap(TapWeight1, TapCoord1, TapWeight2, TapCoord2, OutputLinearWeights[LinearFilterWidth-1-I], OutputLinearCoords[LinearFilterWidth-1-I]);
	}

	ZESize InputWidth = DiscreteFilterWidth;
	ZESSize InputHalfWidth = (InputWidth - 1) / 2;
	
	OutputLinearCoords[OutputHalfWidth] = DiscreteCoords[InputHalfWidth];
	OutputLinearWeights[OutputHalfWidth] = DiscreteWeights[InputHalfWidth];
}

static void CropFilterEdge(float* Weights, float* Coords, ZEUInt FilterWidth, ZEUInt CropCount, float* OutputWeights, float* OutputCoords)
{
	ZESize IterationCount = FilterWidth - CropCount * 2;
	for (ZESize I = 0; I < IterationCount; ++I)
	{
		OutputCoords[I] = Coords[I + CropCount];
		OutputWeights[I] = Weights[I + CropCount];
	}
}

// Sould be applied as a final step
static void OffsetFilter(ZEVector4* Filter, ZEUInt FilterWidth, const ZEVector2& TapOffset)
{
	for (ZESize I = 0; I < (ZESize)FilterWidth; I++)
	{
		Filter[I].x += TapOffset.x;
		Filter[I].y += TapOffset.y;
	}
}

void ZED3D9HDRProcessor::UpdateBuffers(ZEUInt Width, ZEUInt Height)
{
	if (OutputWidth == Width || OutputHeight == Height)
		return;

	DestroyBuffers();
	
	// Create bloom textures
	for (ZEUInt32 I = 0; I < 6; ++I)
	{
		ZED3D9Texture2D* Texture = NULL;
		Texture = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Texture->Create(Width >> I, Height >> I, 1, ZE_TPF_F16_4, true);
		BloomLevels.Add(Texture);

		Texture = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Texture->Create(Width >> I, Height >> I, 1, ZE_TPF_F16_4, true);
		BloomLevelsTemp.Add(Texture);
	}

	// Create luminance textures
	for (ZEUInt Dimension = 3; Dimension < Width && Dimension < Height; Dimension *= 3)
	{
		ZED3D9Texture2D* Texture = NULL;
		Texture = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Texture->Create(Dimension, Dimension, 1, ZE_TPF_F16, true);
		
		LuminanceMips.Insert(0, Texture);
	}

	CurrentLuminance = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	CurrentLuminance->Create(1, 1, 1, ZE_TPF_F16, true);

	PreviousLuminance = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	PreviousLuminance->Create(1, 1, 1, ZE_TPF_F16, true);

	Black1x1 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Black1x1->Create(1, 1, 1, ZE_TPF_I8_4, false);
	
	// Fill in the blanks
	ZEUInt32* Target = NULL;
	ZESize Pitch = 0;
	Black1x1->Lock((void**)&Target, &Pitch ,0);
	*Target = 0x00000000;
	Black1x1->Unlock(0);

	OutputWidth = Width;
	OutputHeight = Height;
}

void ZED3D9HDRProcessor::DestroyBuffers()
{
	ZESize BloomLevelCount = BloomLevels.GetCount();
	for (ZESize I = 0; I < BloomLevelCount; ++I)
	{
		ZED3D_DESTROY(BloomLevels[I]);
		ZED3D_DESTROY(BloomLevelsTemp[I]);
	}
	BloomLevels.Clear(false);
	BloomLevelsTemp.Clear(false);

	ZESize LuminanceMipCount = LuminanceMips.GetCount();
	for (ZESize I = 0; I < LuminanceMipCount; ++I)
	{
		ZED3D_DESTROY(LuminanceMips[I]);
	}
	LuminanceMips.Clear(false);

	ZED3D_DESTROY(PreviousLuminance);
	ZED3D_DESTROY(CurrentLuminance);
}

void ZED3D9HDRProcessor::UpdateShaders()
{
	ZEUInt32 Components = 0;
	Components |= 1 << ToneMapOperator;	// Encode operator: components 0-5
	Components |= AutoKey ? 1 << 5 : 0;	// Encode auto key: component 5
	Components |= AutoExposure ? 1 << 6 : 0; // Encode auto key: component 6
	
	if (Shaders.Vertex == NULL)
	{
		Shaders.Vertex = ZED3D9VertexShader::CreateShader("HDRProcessor.hlsl", "VSMainCommon", Components, "vs_3_0");
	}
	if(Shaders.ConvertToLuminance == NULL)
	{
		Shaders.ConvertToLuminance = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSConvertToLuminance", Components, "ps_3_0");
	}
	if(Shaders.LuminanceAdaptation == NULL)
	{
		Shaders.LuminanceAdaptation = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSMainLuminanceAdaptation", Components, "ps_3_0");
	}
	if(Shaders.LuminanaceScale3x == NULL)
	{
		Shaders.LuminanaceScale3x = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSMainLuminanaceScale3x", Components, "ps_3_0");
	}
	if (Shaders.ColorDownSample2x == NULL)
	{
		Shaders.ColorDownSample2x = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSMainColorDownSample2x", Components, "ps_3_0");
	}
	if(Shaders.BlurVerticalUpSample2x == NULL)
	{
		Shaders.BlurVerticalUpSample2x = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSMainBlurVerticalUpSample2x", Components, "ps_3_0");
	}
	if(Shaders.BlurHorizontal == NULL)
	{
		Shaders.BlurHorizontal = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSMainBlurHorizontal", Components, "ps_3_0");
	}
	if(Shaders.BrightPass == NULL || Shaders.Recompile)
	{
		ZED3D_RELEASE(Shaders.BrightPass);
		Shaders.BrightPass = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSMainBrightPass", Components, "ps_3_0");
	}
	if(Shaders.Combine == NULL || Shaders.Recompile)
	{
		ZED3D_RELEASE(Shaders.Combine);
		Shaders.Combine = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSMainCombine", Components, "ps_3_0");
	}

	Shaders.Recompile = false;
}

void ZED3D9HDRProcessor::DestroyShaders()
{
	ZED3D_RELEASE(Shaders.Vertex);
	ZED3D_RELEASE(Shaders.ColorDownSample2x);
	ZED3D_RELEASE(Shaders.ConvertToLuminance);
	ZED3D_RELEASE(Shaders.LuminanaceScale3x);
	ZED3D_RELEASE(Shaders.BlurVerticalUpSample2x);
	ZED3D_RELEASE(Shaders.BlurHorizontal);
	ZED3D_RELEASE(Shaders.Combine);
	ZED3D_RELEASE(Shaders.LuminanceAdaptation);
	ZED3D_RELEASE(Shaders.BrightPass);
}

void ZED3D9HDRProcessor::ColorDownSample2x(const ZED3D9Texture2D* Input, ZED3D9Texture2D* Output)
{
	D3DPERF_BeginEvent(0, L"Color downsample 2x");

	GetDevice()->SetTexture(0, Input->Texture);
	GetDevice()->SetRenderTarget(0, Output->ViewPort.FrameBuffer);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_ALL);

	ZEVector4 PixelSize = ZEVector4(1.0f / Output->GetWidth(), 1.0f / Output->GetHeight(), 0.0f, 0.0f);

	GetDevice()->SetVertexShaderConstantF(0, PixelSize.M, 1);
	GetDevice()->SetPixelShaderConstantF(0, PixelSize.M, 1);

	GetDevice()->SetPixelShader(Shaders.ColorDownSample2x->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEHDRScreenAlignedQuad));

	D3DPERF_EndEvent();
}

void ZED3D9HDRProcessor::HorizontalBlur(ZED3D9Texture2D* Input, ZED3D9Texture2D* Output)
{
	D3DPERF_BeginEvent(0, L"Horizontal Blur");

	GetDevice()->SetTexture(0, Input->Texture);
	GetDevice()->SetRenderTarget(0, Output->ViewPort.FrameBuffer);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_ALL);

	ZEVector4 PixelSize = ZEVector4(1.0f / Output->GetWidth(), 1.0f / Output->GetHeight(), 0.0f, 0.0f);
	GetDevice()->SetVertexShaderConstantF(0, PixelSize.M, 1);

	float Taps[GAUSSIAN_FILTER_WIDTH] = {0.0f};
	float Coords[GAUSSIAN_FILTER_WIDTH] = {0.0f};
	FillGaussianKernel(Taps, Coords, GAUSSIAN_FILTER_WIDTH, BloomDeviation, PixelSize.x);
	
	float LinearTaps[GAUSSIAN_FILTER_WIDTH] = {0.0f};
	float LinearCoords[GAUSSIAN_FILTER_WIDTH] = {0.0f};
	ZEUInt NewWidth = CalculateLinearFilterWidth(GAUSSIAN_FILTER_WIDTH);
	GenerateBilinearGaussianKernel(Taps, Coords, GAUSSIAN_FILTER_WIDTH, NewWidth, LinearTaps, LinearCoords);

	ZEVector4 Filter[GAUSSIAN_FILTER_WIDTH];
	for (ZESize I = 0; I < NewWidth; ++I)
	{
		Filter[I].x = LinearCoords[I];
		Filter[I].y = 0.0f;

		Filter[I].z = LinearTaps[I];
	}
	// Crop first and last weights since they are too low
	GetDevice()->SetPixelShaderConstantF(15, Filter[1].M, NewWidth-2);

	GetDevice()->SetPixelShader(Shaders.BlurHorizontal->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEHDRScreenAlignedQuad));

	D3DPERF_EndEvent();
}

void ZED3D9HDRProcessor::VerticalBlurAdditiveUpSample2x(ZED3D9Texture2D* Input, float BloomWeight, ZED3D9Texture2D* Addition, ZED3D9Texture2D* Output)
{
	D3DPERF_BeginEvent(0, L"Vertical Blur");

	GetDevice()->SetTexture(0, Input->Texture);
	GetDevice()->SetTexture(5, Addition->Texture);
	GetDevice()->SetRenderTarget(0, Output->ViewPort.FrameBuffer);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_ALL);

	ZEVector4 PixelSize = ZEVector4(1.0f / Output->GetWidth(), 1.0f / Output->GetHeight(), 0.0f, 0.0f);
	GetDevice()->SetVertexShaderConstantF(0, PixelSize.M, 1);

	ZEVector4 InputPixelSize = ZEVector4(1.0f / Input->GetWidth(), 1.0f / Input->GetHeight(), 0.0f, 0.0f);

	float Taps[GAUSSIAN_FILTER_WIDTH] = {0.0f};
	float Coords[GAUSSIAN_FILTER_WIDTH] = {0.0f};
	FillGaussianKernel(Taps, Coords, GAUSSIAN_FILTER_WIDTH, BloomDeviation, InputPixelSize.y);
	
	float LinearTaps[GAUSSIAN_FILTER_WIDTH] = {0.0f};
	float LinearCoords[GAUSSIAN_FILTER_WIDTH] = {0.0f};
	ZEUInt NewWidth = CalculateLinearFilterWidth(GAUSSIAN_FILTER_WIDTH);
	GenerateBilinearGaussianKernel(Taps, Coords, GAUSSIAN_FILTER_WIDTH, NewWidth, LinearTaps, LinearCoords);

	ZEVector4 Filter[GAUSSIAN_FILTER_WIDTH];
	for (ZESize I = 0; I < NewWidth; ++I)
	{
		Filter[I].x = 0.0f;
		Filter[I].y = LinearCoords[I];

		Filter[I].z = LinearTaps[I];
	}
	OffsetFilter(Filter, NewWidth, ZEVector2(-PixelSize.x, 0.0f));
	
	// Crop first and last 2 weights sinde they are too low
	GetDevice()->SetPixelShaderConstantF(15, Filter[1].M, NewWidth-2);

	ZEVector4 ParameterBloomWeight = ZEVector4(BloomWeight, 0.0f, 0.0f, 0.0f);
	GetDevice()->SetPixelShaderConstantF(4, ParameterBloomWeight.M, 1);

	GetDevice()->SetPixelShader(Shaders.BlurVerticalUpSample2x->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEHDRScreenAlignedQuad));

	D3DPERF_EndEvent();
}

void ZED3D9HDRProcessor::LuminanaceScale3x(ZED3D9Texture2D* Input, ZED3D9Texture2D* Output)
{
	D3DPERF_BeginEvent(0, L"Luminance Scale 3x");

	GetDevice()->SetTexture(0, Input->Texture);
	GetDevice()->SetRenderTarget(0, Output->ViewPort.FrameBuffer);
	
	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_RED);

	ZEVector4 InputPixelSize = ZEVector4(1.0f / Input->GetWidth(), 1.0f / Input->GetHeight(), 0.0f, 0.0f);
	ZEVector4 OutputPixelSize = ZEVector4(1.0f / Output->GetWidth(), 1.0f / Output->GetHeight(), 0.0f, 0.0f);

	GetDevice()->SetVertexShaderConstantF(0, OutputPixelSize.M, 1);
	GetDevice()->SetPixelShaderConstantF(0, InputPixelSize.M, 1);

	GetDevice()->SetPixelShader(Shaders.LuminanaceScale3x->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEHDRScreenAlignedQuad));

	D3DPERF_EndEvent();
}

// Convert given color map to luminance map
void ZED3D9HDRProcessor::ConvertToLuminance(ZED3D9Texture2D* Input, ZED3D9Texture2D* Output)
{
	D3DPERF_BeginEvent(0, L"Luminance map generation");

	GetDevice()->SetTexture(0, Input->Texture);
	GetDevice()->SetRenderTarget(0, Output->ViewPort.FrameBuffer);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_RED);

	ZEVector4 InputPixelSize = ZEVector4(1.0f / Input->GetWidth(), 1.0f / Input->GetHeight(), 0.0f, 0.0f);
	ZEVector4 OutputPixelSize = ZEVector4(1.0f / Output->GetWidth(), 1.0f / Output->GetHeight(), 0.0f, 0.0f);

	GetDevice()->SetVertexShaderConstantF(0, OutputPixelSize.M, 1);
	GetDevice()->SetPixelShaderConstantF(0, InputPixelSize.M, 1);

	GetDevice()->SetPixelShader(Shaders.ConvertToLuminance->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEHDRScreenAlignedQuad));

	D3DPERF_EndEvent();
}

// Generates mipmaps of given luminance buffer
void ZED3D9HDRProcessor::MipMapLuminance(ZEArray<ZED3D9Texture2D*>& Input)
{
	D3DPERF_BeginEvent(0, L"Luminance Mipmap Generation");

	ZESize PassCount = Input.GetCount() - 1;
	for (ZESize I = 0; I < PassCount; ++I)
		LuminanaceScale3x(Input[I], Input[I + 1]);
		
	D3DPERF_EndEvent();
}

// Gets 3x3 measured luminance, adapts Final luminance based on measured luminance and previous luminance
void ZED3D9HDRProcessor::AdaptLuminance(ZED3D9Texture2D* MeasuredLum, ZED3D9Texture2D* PreviousLum, ZED3D9Texture2D* Output)
{
	D3DPERF_BeginEvent(0, L"Luminance Adaptation");

	GetDevice()->SetTexture(3, PreviousLum->Texture);
	GetDevice()->SetTexture(4, MeasuredLum->Texture);
	GetDevice()->SetRenderTarget(0, Output->ViewPort.FrameBuffer);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_RED);

	ZEVector4 InputPixelSize = ZEVector4(1.0f / MeasuredLum->GetWidth(), 1.0f / MeasuredLum->GetHeight(), 0.0f, 0.0f);
	ZEVector4 OutputPixelSize = ZEVector4(1.0f / Output->GetWidth(), 1.0f / Output->GetHeight(), 0.0f, 0.0f);

	GetDevice()->SetVertexShaderConstantF(0, OutputPixelSize.M, 1);
	GetDevice()->SetPixelShaderConstantF(0, InputPixelSize.M, 1);

	GetDevice()->SetPixelShader(Shaders.LuminanceAdaptation->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEHDRScreenAlignedQuad));

	D3DPERF_EndEvent();
}

// Tonemaps Color input buffer to a lower exposure before bloom textures are generated
void ZED3D9HDRProcessor::BrightPass(ZED3D9Texture2D* Color, ZED3D9Texture2D* CurrentLum, ZED3D9Texture2D* Output)
{
	if (!BloomEnabled)
		return;

	D3DPERF_BeginEvent(0, L"Bright Pass");

	GetDevice()->SetTexture(0, Color->Texture);
	GetDevice()->SetTexture(2, CurrentLum->Texture);
	GetDevice()->SetRenderTarget(0, Output->ViewPort.FrameBuffer);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_ALL);

	ZEVector4 OutputPixelSize = ZEVector4(1.0f / Output->GetWidth(), 1.0f / Output->GetHeight(), 0.0f, 0.0f);
	GetDevice()->SetVertexShaderConstantF(0, OutputPixelSize.M, 1);

	GetDevice()->SetPixelShader(Shaders.BrightPass->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEHDRScreenAlignedQuad));

	D3DPERF_EndEvent();
}

// Generates bloom texture with given PassCount
void ZED3D9HDRProcessor::GenerateBloom(ZED3D9Texture2D* InputOutput, ZEUInt PassCount)
{
	if (!BloomEnabled)
		return;

	D3DPERF_BeginEvent(0, L"Bloom Generation");

	ColorDownSample2x(InputOutput, BloomLevels[1]);
	ColorDownSample2x(BloomLevels[1], BloomLevels[2]);
	ColorDownSample2x(BloomLevels[2], BloomLevels[3]);
	ColorDownSample2x(BloomLevels[3], BloomLevels[4]);
	ColorDownSample2x(BloomLevels[4], BloomLevels[5]);

	HorizontalBlur(BloomLevels[5], BloomLevelsTemp[5]);
	VerticalBlurAdditiveUpSample2x(BloomLevelsTemp[5], BloomWeightLarge, Black1x1, BloomLevels[5]);

	HorizontalBlur(BloomLevels[4], BloomLevelsTemp[4]);
	VerticalBlurAdditiveUpSample2x(BloomLevelsTemp[4], BloomWeightLarge, BloomLevels[5], BloomLevels[4]);

	HorizontalBlur(BloomLevels[3], BloomLevelsTemp[3]);
	VerticalBlurAdditiveUpSample2x(BloomLevelsTemp[3], BloomWeightMedium, BloomLevels[4], BloomLevels[3]);

	HorizontalBlur(BloomLevels[2], BloomLevelsTemp[2]);
	VerticalBlurAdditiveUpSample2x(BloomLevelsTemp[2], BloomWeightMedium, BloomLevels[3], BloomLevels[2]);

	HorizontalBlur(BloomLevels[1], BloomLevelsTemp[1]);
	VerticalBlurAdditiveUpSample2x(BloomLevelsTemp[1], BloomWeightSmall, BloomLevels[2], BloomLevels[1]);

	HorizontalBlur(BloomLevels[0], BloomLevelsTemp[0]);
	VerticalBlurAdditiveUpSample2x(BloomLevelsTemp[0], BloomWeightSmall, BloomLevels[1], InputOutput);

	D3DPERF_EndEvent();
}

void ZED3D9HDRProcessor::Combine(ZED3D9Texture2D* Color, ZED3D9Texture2D* Bloom, ZED3D9Texture2D* CurrentLum, ZED3D9ViewPort* Output)
{
	D3DPERF_BeginEvent(0, L"ToneMap And Combine Bloom");

	GetDevice()->SetTexture(0, Color->Texture);
	GetDevice()->SetTexture(1, Bloom->Texture);
	GetDevice()->SetTexture(2, CurrentLum->Texture);
	GetDevice()->SetRenderTarget(0, Output->FrameBuffer);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_ALL);

	// Change color buffer sampler to point
	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

	ZEVector4 OutputPixelSize = ZEVector4(1.0f / Output->GetWidth(), 1.0f / Output->GetHeight(), 0.0f, 0.0f);
	GetDevice()->SetVertexShaderConstantF(0, OutputPixelSize.M, 1);

	GetDevice()->SetPixelShader(Shaders.Combine->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEHDRScreenAlignedQuad));

	D3DPERF_EndEvent();
}

void ZED3D9HDRProcessor::CommitConstants(float ElapsedTime)
{
	D3DPERF_BeginEvent(0, L"Update Shader Constants");
	
	//Set Depth Buffer
	GetDevice()->SetDepthStencilSurface(((ZED3D9ViewPort*)Renderer->GetViewPort())->ZBuffer);

	// Color Buffer sampler state
	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

	// Bloom buffer sampler state
	GetDevice()->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(1, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

	// Current Luminance sampler state
	GetDevice()->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(2, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

	// Previous Luminance sampler state
	GetDevice()->SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(3, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(3, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

	GetDevice()->SetSamplerState(4, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(4, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(4, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(4, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(4, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(4, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

	GetDevice()->SetSamplerState(5, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(5, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(5, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(5, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(5, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(5, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
	
	struct HDRParameters
	{
		float Key;
		float Exposure;
		float BloomFactor;
		float WhiteLevel;
		float Saturation;
		float BloomTreshold;
		float ElapsedTime;
		float AdaptationRate;
		float LuminanceUpper;
		float LuminanceLower;

		float Dummy1;
		float Dummy2;
	};

	HDRParameters Parameters = {0.0f};
	Parameters.Key = Key;
	Parameters.Exposure = Exposure;
	Parameters.BloomFactor = BloomFactor;
	Parameters.WhiteLevel = WhiteLevel;
	Parameters.Saturation = Saturation;
	Parameters.BloomTreshold = BloomTreshold;
	Parameters.ElapsedTime = ElapsedTime;
	Parameters.AdaptationRate = AdaptationRate;
	Parameters.LuminanceUpper = LuminanceUpper;
	Parameters.LuminanceLower = LuminanceLower;

	GetDevice()->SetPixelShaderConstantF(1, (const float*)&Parameters, 3);
	
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	GetDevice()->SetVertexShader(Shaders.Vertex->GetVertexShader());

	D3DPERF_EndEvent();
}

void ZED3D9HDRProcessor::SwitchLuminanceBuffers()
{
	ZED3D9Texture2D* TempPtr = PreviousLuminance;
	PreviousLuminance = CurrentLuminance;
	CurrentLuminance = TempPtr;
}

void ZED3D9HDRProcessor::Process(float ElapsedTime)
{
	if (Renderer == NULL)
		return;

	if (InputBuffer == NULL)
		return;

	if (OutputBuffer == NULL)
		return;

	D3DPERF_BeginEvent(0, L"HDR Pass");
	
	UpdateShaders();
	UpdateBuffers(OutputBuffer->GetWidth(), OutputBuffer->GetHeight());
	
	// Update constants
	CommitConstants(ElapsedTime);
	
	// Generate luminance texture
	ConvertToLuminance(InputBuffer, LuminanceMips[0]);

	// Create mipmaps
	MipMapLuminance(LuminanceMips);

	// Adapt current luminance based on previous one
	AdaptLuminance(LuminanceMips.GetLastItem(), PreviousLuminance, CurrentLuminance);

	// Tonemap input buffer to a lower exposure before bloom
	BrightPass(InputBuffer, CurrentLuminance, BloomLevels[0]);
	
	// Blur for Bloom
	GenerateBloom(BloomLevels[0], BloomPassCount);

	// Do final Combination pass
	Combine(InputBuffer, BloomEnabled ? BloomLevels[0] : Black1x1, CurrentLuminance, OutputBuffer);

	// Switch new and previous luminance
	SwitchLuminanceBuffers();
	
	D3DPERF_EndEvent();
}

void ZED3D9HDRProcessor::SetKey(float Value)
{
	Key = Value;
}

float ZED3D9HDRProcessor::GetKey() const
{
	return Key;
}

void ZED3D9HDRProcessor::SetAutoKey(bool Enabled)
{
	if (AutoKey != Enabled)
	{
		AutoKey = Enabled;
		Shaders.Recompile = true;
	}
}

bool ZED3D9HDRProcessor::GetAutoKey() const
{
	return AutoKey;
}

void ZED3D9HDRProcessor::SetAutoExposure(bool Enabled)
{
	if (AutoExposure != Enabled)
	{
		AutoExposure = Enabled;
		Shaders.Recompile = true;
	}
}

bool ZED3D9HDRProcessor::GetAutoExposure() const
{
	return AutoExposure;
}

void ZED3D9HDRProcessor::SetBloomEnabled(bool Enabled)
{
	BloomEnabled = Enabled;
}

bool ZED3D9HDRProcessor::GetBloomEnabled() const
{
	return BloomEnabled;
}

void ZED3D9HDRProcessor::SetBloomWeightLarge(float Value)
{
	BloomWeightLarge = Value;
}

float ZED3D9HDRProcessor::GetBloomWeightLarge() const
{
	return BloomWeightLarge;
}

void ZED3D9HDRProcessor::SetBloomWeightMedium(float Value)
{
	BloomWeightMedium = Value;
}

float ZED3D9HDRProcessor::GetBloomWeightMedium() const
{
	return BloomWeightMedium;
}
		
void ZED3D9HDRProcessor::SetBloomWeightSmall(float Value)
{
	BloomWeightSmall = Value;
}

float ZED3D9HDRProcessor::GetBloomWeightSmall() const
{
	return BloomWeightSmall;
}

void ZED3D9HDRProcessor::SetExposure(float Value)
{
	Exposure = Value;
}

float ZED3D9HDRProcessor::GetExposure() const
{
	return Exposure;
}

void ZED3D9HDRProcessor::SetAdaptationRate(float Value)
{
	AdaptationRate = Value;
}

float ZED3D9HDRProcessor::GetAdaptationRate() const
{
	return AdaptationRate;
}
		
void ZED3D9HDRProcessor::SetBloomFactor(float Value)
{
	BloomFactor = Value;
}

float ZED3D9HDRProcessor::GetBloomFactor() const
{
	return BloomFactor;
}

void ZED3D9HDRProcessor::SetBloomTreshold(float Value)
{
	BloomTreshold = ZEMath::Clamp(Value, 0.0f, 10.0f);
}

float ZED3D9HDRProcessor::GetBloomTreshold() const
{
	return BloomTreshold;
}

void ZED3D9HDRProcessor::SetBloomDeviation(float Value)
{
	BloomDeviation = Value;
}

float ZED3D9HDRProcessor::GetBloomDeviation() const
{
	return BloomDeviation;
}

void ZED3D9HDRProcessor::SetWhiteLevel(float Value)
{
	WhiteLevel = Value;
}

float ZED3D9HDRProcessor::GetWhiteLevel() const
{
	return WhiteLevel;
}

void ZED3D9HDRProcessor::SetSaturation(float Value)
{
	Saturation = Value;
}

float ZED3D9HDRProcessor::GetSaturation() const
{
	return Saturation;
}

void ZED3D9HDRProcessor::SetLuminanceUpper(float Value)
{
	LuminanceUpper = Value;
}

float ZED3D9HDRProcessor::GetLuminanceUpper() const
{
	return LuminanceUpper;
}

void ZED3D9HDRProcessor::SetLuminanceLower(float Value)
{
	LuminanceLower = Value;
}

float ZED3D9HDRProcessor::GetLuminanceLower() const
{
	return LuminanceLower;
}

void ZED3D9HDRProcessor::SetBloomPassCount(ZEUInt Count)
{
	BloomPassCount = Count;
}

ZEUInt ZED3D9HDRProcessor::GetBloomPassCount() const
{
	return BloomPassCount;
}

void ZED3D9HDRProcessor::SetToneMapOperator(ZEHDRToneMapOperator Operator)
{
	if (ToneMapOperator != Operator)
	{
		ToneMapOperator = Operator;
		Shaders.Recompile = true;
	}
}

ZEHDRToneMapOperator ZED3D9HDRProcessor::GetToneMapOperator() const
{
	return ToneMapOperator;
}

void ZED3D9HDRProcessor::SetInput(ZED3D9Texture2D* Input)
{
	this->InputBuffer = Input;
}

ZED3D9Texture2D* ZED3D9HDRProcessor::GetInput()
{
	return InputBuffer;
}

void ZED3D9HDRProcessor::SetOutput(ZED3D9ViewPort* Output)
{
	zeDebugCheck(Output == NULL, "Null Pointer");

	this->OutputBuffer = Output;
}

ZED3D9ViewPort* ZED3D9HDRProcessor::GetOutput()
{
	return OutputBuffer;
}

void ZED3D9HDRProcessor::SetRenderer(ZED3D9FrameRenderer* FrameRenderer)
{
	Renderer = FrameRenderer;
}

ZED3D9FrameRenderer* ZED3D9HDRProcessor::SetRenderer() const
{
	return Renderer;
}

void ZED3D9HDRProcessor::Initialize()
{
	// Vertex declaration for screen aligned quad
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0}, 
		D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);
}

void ZED3D9HDRProcessor::Deinitialize()
{
	DestroyBuffers();
	DestroyShaders();
}

ZEHDRScreenAlignedQuad ZED3D9HDRProcessor::Vertices[4] =
{
	{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}},
	{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}
};

ZED3D9HDRProcessor::ZED3D9HDRProcessor()
{
	Renderer = NULL;
	InputBuffer = NULL;
	OutputBuffer = NULL;

	OutputWidth = 0;
	OutputHeight = 0;

	Black1x1 = NULL;
	CurrentLuminance = NULL;
	PreviousLuminance = NULL;

	Shaders.Recompile = true;
	Shaders.Vertex = NULL;
	Shaders.ConvertToLuminance = NULL;
	Shaders.LuminanaceScale3x = NULL;
	Shaders.LuminanceAdaptation = NULL;
	Shaders.BrightPass = NULL;
	Shaders.BlurVerticalUpSample2x = NULL;
	Shaders.BlurHorizontal = NULL;
	Shaders.Combine = NULL;
	Shaders.ColorDownSample2x = NULL;
	
	VertexDeclaration = NULL;

	AutoKey = false;
	AutoExposure = true;
	BloomEnabled = true;

	Key = 0.35f;
	Exposure = 0.3f;
	Saturation = 1.0f;
	WhiteLevel = 5.0f;
	LuminanceUpper = 10.0f;
	LuminanceLower = 0.00f;
	AdaptationRate = 1.5f;
	ToneMapOperator = ZE_HDR_TMO_FILMIC;

	BloomFactor = 2.0f;
	BloomTreshold = 5.0f;
	BloomDeviation = 4.0f;
	BloomWeightLarge = 0.9f;
	BloomWeightMedium = 0.05f;
	BloomWeightSmall = 0.05f;
	BloomPassCount = 5;
}

ZED3D9HDRProcessor::~ZED3D9HDRProcessor()
{
	Deinitialize();
}
