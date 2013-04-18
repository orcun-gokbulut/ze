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
#include "ZED3D9ComponentBase.h"
#include "ZED3D9FrameRenderer.h"
#include "ZED3D9HDRProcessor.h"
#include "ZETexture/ZETexture2DResource.h"


#define GAUSSIAN_FILTER_WIDTH		13

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
/*
static void FillGaussianKernel1D(ZEArray<ZEVector4>& Filter, float StandartDeviation, float PixelSize)
{
	ZESSize FilterWidth = Filter.GetCount();
	ZESSize HalfWidth = (FilterWidth - 1) / 2;
	
	for (ZESize I = 0; I < FilterWidth; I++)
	{
		float TapCoord = (float)(I - HalfWidth);
		Filter[I].w = GausianFunction(TapCoord, StandartDeviation);
		
		Filter[I].x = PixelSize * TapCoord + Offset.x;
	}
}

static void ConvertGaussianKernel1DToBilinear(ZEArray<ZEVector4>& Filter)
{
	
}
*/
void ZED3D9HDRProcessor::UpdateBuffers(ZEUInt Width, ZEUInt Height)
{
	if (OutputWidth == Width || OutputHeight == Height)
		return;

	DestroyBuffers();

	//TestImage = (ZED3D9Texture2D*)ZETexture2DResource::LoadSharedResource("BlurTest.tga")->GetTexture();
	
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
	TestImage = NULL;

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
		Shaders.BrightPass = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSMainBrightPass", Components, "ps_3_0");
	}
	if(Shaders.Combine == NULL || Shaders.Recompile)
	{
		Shaders.Combine = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSMainCombine", Components, "ps_3_0");
	}
	if(Shaders.DebugPrint == NULL)
	{
		Shaders.DebugPrint = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSMainDebugPrint", Components, "ps_3_0");
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
}

void ZED3D9HDRProcessor::DebugPrint(const ZED3D9Texture2D* Input, ZED3D9ViewPort* Output)
{
	D3DPERF_BeginEvent(0, L"Debug Print");

	GetDevice()->SetTexture(0, Input->Texture);
	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

	GetDevice()->SetRenderTarget(0, Output->FrameBuffer);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_ALL);

	ZEVector4 OutputPixelSize = ZEVector4(1.0f / Output->GetWidth(), 1.0f / Output->GetHeight(), 0.0f, 0.0f);

	GetDevice()->SetVertexShaderConstantF(0, OutputPixelSize.M, 1);

	GetDevice()->SetPixelShader(Shaders.DebugPrint->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEHDRScreenAlignedQuad));

	D3DPERF_EndEvent();
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

// Input and output should be same size
void ZED3D9HDRProcessor::HorizontalBlur(ZED3D9Texture2D* Input, ZED3D9Texture2D* Output)
{
	D3DPERF_BeginEvent(0, L"Horizontal Blur");

	GetDevice()->SetTexture(0, Input->Texture);
	GetDevice()->SetRenderTarget(0, Output->ViewPort.FrameBuffer);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_ALL);

	ZEVector4 PixelSize = ZEVector4(1.0f / Output->GetWidth(), 1.0f / Output->GetHeight(), 0.0f, 0.0f);
	GetDevice()->SetVertexShaderConstantF(0, PixelSize.M, 1);

	ZEVector4 Filter[GAUSSIAN_FILTER_WIDTH];
	for (ZESize TapN = 0; TapN < GAUSSIAN_FILTER_WIDTH; ++TapN)
	{
		float TapOffset = (float)TapN - (float)(GAUSSIAN_FILTER_WIDTH / 2);

		// Sample Weight
		Filter[TapN].z = GausianFunction(TapOffset, BloomDeviation);

		// Sample Coords
		Filter[TapN].x = TapOffset * PixelSize.x;
		Filter[TapN].y = 0.0f;
		Filter[TapN].w = 0.0f;
	}
	GetDevice()->SetPixelShaderConstantF(15, Filter->M, GAUSSIAN_FILTER_WIDTH);

	GetDevice()->SetPixelShader(Shaders.BlurHorizontal->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEHDRScreenAlignedQuad));

	D3DPERF_EndEvent();
}

// Input and output should be same size
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

	ZEVector4 Filter[GAUSSIAN_FILTER_WIDTH];
	for (ZESize TapN = 0; TapN < GAUSSIAN_FILTER_WIDTH; ++TapN)
	{
		float TapOffset = (float)TapN - (float)(GAUSSIAN_FILTER_WIDTH / 2);

		// Sample Weight
		Filter[TapN].z = GausianFunction(TapOffset, BloomDeviation);

		// Sample Coords
		Filter[TapN].x = 0.0f;
		Filter[TapN].y = TapOffset * InputPixelSize.y;
		Filter[TapN].w = 0.0f;
	}
	GetDevice()->SetPixelShaderConstantF(15, Filter->M, GAUSSIAN_FILTER_WIDTH);

	ZEVector4 ParameterBloomWeight = ZEVector4(BloomWeight, 0.0f, 0.0f, 0.0f);
	GetDevice()->SetPixelShaderConstantF(10, ParameterBloomWeight.M, 1);

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

	ColorDownSample2x(BloomLevels[0], BloomLevels[1]);
	ColorDownSample2x(BloomLevels[1], BloomLevels[2]);
	ColorDownSample2x(BloomLevels[2], BloomLevels[3]);
	ColorDownSample2x(BloomLevels[3], BloomLevels[4]);
	ColorDownSample2x(BloomLevels[4], BloomLevels[5]);

	HorizontalBlur(BloomLevels[5], BloomLevelsTemp[5]);
	VerticalBlurAdditiveUpSample2x(BloomLevelsTemp[5], LargeBloomWeight, Black1x1, BloomLevels[5]);

	HorizontalBlur(BloomLevels[4], BloomLevelsTemp[4]);
	VerticalBlurAdditiveUpSample2x(BloomLevelsTemp[4], LargeBloomWeight, BloomLevels[5], BloomLevels[4]);

	HorizontalBlur(BloomLevels[3], BloomLevelsTemp[3]);
	VerticalBlurAdditiveUpSample2x(BloomLevelsTemp[3], MediumBloomWeight, BloomLevels[4], BloomLevels[3]);

	HorizontalBlur(BloomLevels[2], BloomLevelsTemp[2]);
	VerticalBlurAdditiveUpSample2x(BloomLevelsTemp[2], MediumBloomWeight, BloomLevels[3], BloomLevels[2]);

	HorizontalBlur(BloomLevels[1], BloomLevelsTemp[1]);
	VerticalBlurAdditiveUpSample2x(BloomLevelsTemp[1], SmallBloomWeight, BloomLevels[2], BloomLevels[1]);

	HorizontalBlur(BloomLevels[0], BloomLevelsTemp[0]);
	VerticalBlurAdditiveUpSample2x(BloomLevelsTemp[0], SmallBloomWeight, BloomLevels[1], BloomLevels[0]);

	//DebugPrint(InputOutput, OutputBuffer);

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

void ZED3D9HDRProcessor::LimitAndCommitConstants(float ElapsedTime)
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

	//! PACK THESE PARAMETERS
	ZEVector4 ParameterKey = ZEVector4(Key, 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterExposure = ZEVector4(Exposure, 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterBloomFactor = ZEVector4(BloomFactor, 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterWhiteLevel = ZEVector4(WhiteLevel, 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterSaturation = ZEVector4(Saturation, 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterBloomTreshold = ZEVector4(BloomTreshold, 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterElapsedTime = ZEVector4(ElapsedTime, 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterAdaptRate = ZEVector4(AdaptationRate, 0.0f, 0.0f, 0.0f);
		
	GetDevice()->SetPixelShaderConstantF(1, ParameterKey.M, 1);
	GetDevice()->SetPixelShaderConstantF(3, ParameterExposure.M, 1);
	GetDevice()->SetPixelShaderConstantF(4, ParameterAdaptRate.M, 1);
	GetDevice()->SetPixelShaderConstantF(5, ParameterBloomFactor.M, 1);
	GetDevice()->SetPixelShaderConstantF(6, ParameterBloomTreshold.M, 1);
	GetDevice()->SetPixelShaderConstantF(7, ParameterWhiteLevel.M, 1);
	GetDevice()->SetPixelShaderConstantF(8, ParameterSaturation.M, 1);
	GetDevice()->SetPixelShaderConstantF(9, ParameterElapsedTime.M, 1);

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
	
	
	float Filter[9];
	for (ZESize TapN = 0; TapN < 9; ++TapN)
	{
		float TapOffset = (float)TapN - (float)(9 / 2);

		// Sample Weight
		Filter[TapN] = GausianFunction(TapOffset, 4);
	}

	D3DPERF_BeginEvent(0, L"HDR Pass");
	
	UpdateShaders();
	UpdateBuffers(OutputBuffer->GetWidth(), OutputBuffer->GetHeight());
	
	// Update constants
	LimitAndCommitConstants(ElapsedTime);
	
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

void ZED3D9HDRProcessor::SetKey(float Key)
{
	this->Key = Key;
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

void ZED3D9HDRProcessor::SetLargeBloomWeight(float Value)
{
	LargeBloomWeight = Value;
}

float ZED3D9HDRProcessor::GetLargeBloomWeight() const
{
	return LargeBloomWeight;
}

void ZED3D9HDRProcessor::SetMediumBloomWeight(float Value)
{
	MediumBloomWeight = Value;
}

float ZED3D9HDRProcessor::GetMediumBloomWeight() const
{
	return MediumBloomWeight;
}
		
void ZED3D9HDRProcessor::SetSmallBloomWeight(float Value)
{
	SmallBloomWeight = Value;
}

float ZED3D9HDRProcessor::GetSmallBloomWeight() const
{
	return SmallBloomWeight;
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
	BloomFactor = ZEMath::Clamp(Value, 0.0f, 5.0f);
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
	BloomDeviation = ZEMath::Clamp(Value, 0.5f, 3.0f);
}

float ZED3D9HDRProcessor::GetBloomDeviation() const
{
	return BloomDeviation;
}

void ZED3D9HDRProcessor::SetWhiteLevel(float Value)
{
	WhiteLevel = ZEMath::Clamp(Value, 0.0f, 20.0f);
}

float ZED3D9HDRProcessor::GetWhiteLevel() const
{
	return WhiteLevel;
}

void ZED3D9HDRProcessor::SetSaturation(float Value)
{
	Saturation = ZEMath::Clamp(Value, 0.0f, 5.0f);
}

float ZED3D9HDRProcessor::GetSaturation() const
{
	return Saturation;
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
	OutputWidth = 0;
	OutputHeight = 0;

	AutoKey = false;
	AutoExposure = true;
	BloomEnabled = true;
	ToneMapOperator = ZE_HDR_TMO_FILMIC;
	
	Key = 0.3f;
	Exposure = 0.3f;
	AdaptationRate = 2.0f;
	BloomFactor = 3.0f;
	BloomTreshold = 5.0f;
	BloomDeviation = 2.0f;
	Saturation = 1.0f;
	WhiteLevel = 5.0f;
	BloomPassCount = 2;

	LargeBloomWeight = 1.0f;
	MediumBloomWeight = 0.2f;
	SmallBloomWeight = 0.1f;

	InputBuffer = NULL;
	OutputBuffer = NULL;

	TestImage = NULL;

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

	Shaders.DebugPrint = NULL;

	Black1x1 = NULL;
	CurrentLuminance = NULL;
	PreviousLuminance = NULL;
}

ZED3D9HDRProcessor::~ZED3D9HDRProcessor()
{
	Deinitialize();
}
