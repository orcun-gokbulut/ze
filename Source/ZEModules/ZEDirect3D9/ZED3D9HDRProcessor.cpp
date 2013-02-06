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

#include "ZED3D9HDRProcessor.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9ComponentBase.h"
#include "ZED3D9Module.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9Shader.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEConsole.h"
#include "ZEMath/ZEAngle.h"

static float GausianFunction(float x, float StandartDeviation)
{
	return (1.0f / (ZEMath::Sqrt(2.0f * ZE_PI) * StandartDeviation)) * ZEMath::Power(ZE_E, -((x * x) / (2.0f * StandartDeviation * StandartDeviation)));
}

static float GausianFunction(float x, float y, float StandartDeviation)
{
	return (1.0f / (2.0f * ZE_PI * StandartDeviation * StandartDeviation)) * ZEMath::Power(ZE_E, -((x * x + y * y) / (2.0f * StandartDeviation * StandartDeviation))); 
}

void FillGaussianKernel1D(float* Kernel, ZESize KernelSize, float StandartDeviation)
{
	ZESize HalfKernelSize = (KernelSize - 1) / 2;
	for (ZESize x = 0; x < KernelSize; x++)
	{
		Kernel[4 * x] = (float)(x - HalfKernelSize);
		Kernel[4 * x + 1] = GausianFunction((float)(x - HalfKernelSize), StandartDeviation);
	}
}

static void FillGaussianKernel2D(float* Kernel, ZESize KernelSize, float StandartDeviation)
{
	ZESize HalfKernelSize = (KernelSize - 1) / 2;
	for (ZESize y = 0; y < KernelSize; y++)
	{
		for (ZESize x = 0; x < KernelSize; x++)
		{
			float* CurrentSample = &Kernel[4 * KernelSize * y + x];
			CurrentSample[0] = (float)(x - HalfKernelSize);
			CurrentSample[1] = (float)(y - HalfKernelSize);
			CurrentSample[2] = GausianFunction((float)(x - HalfKernelSize), (float)(y - HalfKernelSize), StandartDeviation);
			zeOutput("%lf ", CurrentSample[2]);
		}
		zeOutput("\r\n");
	}
}

void ZED3D9HDRProcessor::CreateRenderTargets()
{
	if (Textures.Luminance5 == NULL)
		Textures.Luminance5 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.Luminance5->Create(243, 243, 1, ZE_TPF_F32, true);

	if (Textures.Luminance4 == NULL)
		Textures.Luminance4 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.Luminance4->Create(81, 81, 1, ZE_TPF_F32, true);

	if (Textures.Luminance3 == NULL)
		Textures.Luminance3 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.Luminance3->Create(27, 27, 1, ZE_TPF_F32, true);

	if (Textures.Luminance2 == NULL)
		Textures.Luminance2 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.Luminance2->Create(9, 9, 1, ZE_TPF_F32, true);

	if (Textures.Luminance1 == NULL)
		Textures.Luminance1 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.Luminance1->Create(3, 3, 1, ZE_TPF_F32, true);

	if (Textures.Luminance == NULL)
		Textures.Luminance = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.Luminance->Create(1, 1, 1, ZE_TPF_F32, true);

	if (Textures.OldLuminance == NULL)
		Textures.OldLuminance = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.OldLuminance->Create(1, 1, 1, ZE_TPF_F32, true);

	if (Textures.DownSampled2xA == NULL)
		Textures.DownSampled2xA = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.DownSampled2xA->Create(Input->GetWidth() / 2, Input->GetHeight() / 2, 1, ZE_TPF_I8_4, true);

	if (IntParameters.BloomPassCount > 0)
	{	
		if (Textures.DownSampled2xB == NULL)
			Textures.DownSampled2xB = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Textures.DownSampled2xB->Create(Input->GetWidth() / 2, Input->GetHeight() / 2, 1, ZE_TPF_I8_4, true);
	}

	if (IntParameters.BloomPassCount > 1)
	{
		if (Textures.DownSampled4xA == NULL)
			Textures.DownSampled4xA = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Textures.DownSampled4xA->Create(Input->GetWidth() / 4, Input->GetHeight() / 4, 1, ZE_TPF_I8_4, true);

		if (Textures.DownSampled4xB == NULL)
			Textures.DownSampled4xB = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Textures.DownSampled4xB->Create(Input->GetWidth() / 4, Input->GetHeight() / 4, 1, ZE_TPF_I8_4, true);
	}

	if (IntParameters.BloomPassCount > 2)
	{
		if (Textures.DownSampled8xA == NULL)
			Textures.DownSampled8xA = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Textures.DownSampled8xA->Create(Input->GetWidth() / 8, Input->GetHeight() / 8, 1, ZE_TPF_I8_4, true);

		if (Textures.DownSampled8xB == NULL)
			Textures.DownSampled8xB = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Textures.DownSampled8xB->Create(Input->GetWidth() / 8, Input->GetHeight() / 8, 1, ZE_TPF_I8_4, true);
	}

	if (IntParameters.BloomPassCount > 3)
	{
		if (Textures.DownSampled16xA == NULL)
			Textures.DownSampled16xA = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Textures.DownSampled16xA->Create(Input->GetWidth() / 16, Input->GetHeight() / 16, 1, ZE_TPF_I8_4, true);

		if (Textures.DownSampled16xB == NULL)
			Textures.DownSampled16xB = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Textures.DownSampled16xB->Create(Input->GetWidth() / 16, Input->GetHeight() / 16, 1, ZE_TPF_I8_4, true);
	}
}


void ZED3D9HDRProcessor::ReleaseRenderTargets()
{
	ZED3D_DESTROY(Textures.DownSampled2xA);
	ZED3D_DESTROY(Textures.DownSampled2xB);
	ZED3D_DESTROY(Textures.DownSampled4xA);
	ZED3D_DESTROY(Textures.DownSampled4xB);
	ZED3D_DESTROY(Textures.DownSampled8xA);
	ZED3D_DESTROY(Textures.DownSampled8xB);
	ZED3D_DESTROY(Textures.DownSampled16xA);
	ZED3D_DESTROY(Textures.DownSampled16xB);

	ZED3D_DESTROY(Textures.Luminance5);
	ZED3D_DESTROY(Textures.Luminance4);
	ZED3D_DESTROY(Textures.Luminance3);
	ZED3D_DESTROY(Textures.Luminance2);
	ZED3D_DESTROY(Textures.Luminance1);
	ZED3D_DESTROY(Textures.Luminance);
	ZED3D_DESTROY(Textures.OldLuminance);
}

void ZED3D9HDRProcessor::MeasureLuminance(ZED3D9Texture2D* Input, ZED3D9Texture2D* OldLimunance, ZED3D9ViewPort* Limunance)
{
	static struct Vert  
	{
		float Position[3];
		float Texcoord[2];
	} 
	Vertices[] =
	{
		{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}},
		{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}},
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
		{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}
	};
	
	static const float Kernel[9][4] = 
	{
		{-1.0f, -1.0f, 0.0f, 0.0f},
		{-1.0f,  0.0f, 0.0f, 0.0f},
		{-1.0f,  1.0f, 0.0f, 0.0f},

		{ 0.0f, -1.0f, 0.0f, 0.0f},
		{ 0.0f,  0.0f, 0.0f, 0.0f},
		{ 0.0f,  1.0f, 0.0f, 0.0f},

		{ 1.0f, -1.0f, 0.0f, 0.0f},
		{ 1.0f,  0.0f, 0.0f, 0.0f},
		{ 1.0f,  1.0f, 0.0f, 0.0f}
	};
	GetDevice()->SetPixelShaderConstantF(10, (const float*) Kernel, 9);

	// Luminance Measure Start Pass
	GetDevice()->SetRenderTarget(0, ((ZED3D9ViewPort*)Textures.Luminance5->GetViewPort())->FrameBuffer);
	GetDevice()->SetTexture(0, Input->Texture);
	GetDevice()->SetPixelShader(Shaders.MeasureLuminanceStart->GetPixelShader());
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / 243.0f, 1.0f / 243.0f, 0.0f, 0.0f), 1);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / ScreenWidth, 1.0f / ScreenHeight, 0.0f, 0.0f), 1);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	// Luminance Measure Down Sample 3x (From : 243, To: 81)
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);

	GetDevice()->SetRenderTarget(0, ((ZED3D9ViewPort*)Textures.Luminance4->GetViewPort())->FrameBuffer);
	GetDevice()->SetTexture(0, Textures.Luminance5->Texture);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / 81.0f, 1.0f / 81.0f, 0.0f, 0.0f), 1);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / 243.0f, 1.0f / 243.0f, 0.0f, 0.0f), 1);
	GetDevice()->SetPixelShader(Shaders.MeasureLuminanceDownSample3x->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	// Luminance Measure Down Sample 3x (From : 81, To: 27)
	GetDevice()->SetRenderTarget(0, ((ZED3D9ViewPort*)Textures.Luminance3->GetViewPort())->FrameBuffer);
	GetDevice()->SetTexture(0, Textures.Luminance4->Texture);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / 27.0f, 1.0f / 27.0f, 0.0f, 0.0f), 1);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / 81.0f, 1.0f / 81.0f, 0.0f, 0.0f), 1);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	// Luminance Measure Down Sample 3x (From : 27, To: 9)
	GetDevice()->SetRenderTarget(0, ((ZED3D9ViewPort*)Textures.Luminance2->GetViewPort())->FrameBuffer);
	GetDevice()->SetTexture(0, Textures.Luminance3->Texture);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / 9.0f, 1.0f / 9.0f, 0.0f, 0.0f), 1);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / 27.0f, 1.0f / 27.0f, 0.0f, 0.0f), 1);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	// Luminance Measure Down Sample 3x (From : 9, To: 3)
	GetDevice()->SetRenderTarget(0, ((ZED3D9ViewPort*)Textures.Luminance1->GetViewPort())->FrameBuffer);
	GetDevice()->SetTexture(0, Textures.Luminance2->Texture);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / 3.0f, 1.0f / 3.0f, 0.0f, 0.0f), 1);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / 9.0f, 1.0f / 9.0f, 0.0f, 0.0f), 1);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	// Luminance Measure End Pass
	GetDevice()->SetRenderTarget(0, ((ZED3D9ViewPort*)Textures.Luminance->GetViewPort())->FrameBuffer);
	GetDevice()->SetTexture(1, Textures.Luminance1->Texture);
	GetDevice()->SetTexture(2, Textures.OldLuminance->Texture);
	GetDevice()->SetPixelShader(Shaders.MeasureLuminanceEnd->GetPixelShader());
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f, 1.0f, 0.0f, 0.0f), 1);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / 3.0f, 1.0f / 3.0f, 0.0f, 0.0f), 1);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
}

void ZED3D9HDRProcessor::BrightPass(ZED3D9Texture2D* Input, ZED3D9ViewPort* Output)
{
	static struct Vert  
	{
		float Position[3];
		float Texcoord[2];
	} 
	Vertices[] =
	{
		{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}},
		{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}},
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
		{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}
	};

	static const float Kernel[4][4] = 
	{
		{-0.5f, -0.5f, 0.0f, 0.0f},
		{-0.5f,  0.5f, 0.0f, 0.0f},

		{ 0.5f, -0.5f, 0.0f, 0.0f},
		{ 0.5f,  0.5f, 0.0f, 0.0f}
	};
	GetDevice()->SetPixelShaderConstantF(10, (const float*) Kernel, 4);

	// Bright Pass
	ZED3D9CommonTools::SetTexture(0, Input, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(1, Textures.Luminance, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetRenderTarget(0, Output);

	GetDevice()->SetPixelShader(Shaders.BrightPass->GetPixelShader());
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / Input->GetWidth(), 1.0f / Input->GetHeight(), 0.0f, 0.0f), 1);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / Output->GetWidth(), 1.0f / Output->GetWidth(), 0.0f, 0.0f), 1);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
}

void ZED3D9HDRProcessor::DownSample2x(ZED3D9Texture2D* Input, ZED3D9ViewPort* Output)
{
	static struct Vert  
	{
		float Position[3];
		float Texcoord[2];
	} 
	Vertices[] =
	{
		{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}},
		{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}},
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
		{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}
	};

	static const float Kernel[4][4] = 
	{
		{-0.5f, -0.5f, 0.0f, 0.0f},
		{-0.5f,  0.5f, 0.0f, 0.0f},

		{ 0.5f, -0.5f, 0.0f, 0.0f},
		{ 0.5f,  0.5f, 0.0f, 0.0f}
	};
	GetDevice()->SetPixelShaderConstantF(10, (const float*) Kernel, 4);

	// Down Sample 2x
	ZED3D9CommonTools::SetRenderTarget(0, Output);
	ZED3D9CommonTools::SetTexture(0, Input, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

	GetDevice()->SetPixelShader(Shaders.DownSample2x->GetPixelShader());
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / Input->GetWidth(), 1.0f / Input->GetHeight(), 0.0f, 0.0f), 1);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / Output->GetWidth(), 1.0f / Output->GetWidth(), 0.0f, 0.0f), 1);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
}

void ZED3D9HDRProcessor::BlurPass(ZED3D9Texture2D* Input, ZED3D9Texture2D* Temp, ZED3D9ViewPort* Output)
{
	static struct Vert  
	{
		float Position[3];
		float Texcoord[2];
	} 
	Vertices[] =
	{
		{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}},
		{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}},
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
		{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}
	};

	float Kernel[16][4];
	FillGaussianKernel1D((float*)Kernel, (ZESize)IntParameters.BloomSampleCount, Parameters.BloomStandardDeviation);
	GetDevice()->SetPixelShaderConstantF(10, (const float*) Kernel, IntParameters.BloomSampleCount);

	GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / Input->GetWidth(), 1.0f / Input->GetHeight(), 0.0f, 0.0f), 1);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / Output->GetWidth(), 1.0f / Output->GetWidth(), 0.0f, 0.0f), 1);

	// Vertical Bloom Pass
	ZED3D9CommonTools::SetTexture(0, Input, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetRenderTarget(0, Temp);

	GetDevice()->SetPixelShader(Shaders.VerticalBloom->GetPixelShader()); 
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	// Horizontal Bloom Pass
	ZED3D9CommonTools::SetTexture(0, Temp, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetRenderTarget(0, Output);
	GetDevice()->SetPixelShader(Shaders.HorizontalBloom->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
}

void ZED3D9HDRProcessor::ToneMap(ZED3D9Texture2D* Input, ZED3D9ViewPort* Output)
{
	static struct Vert  
	{
		float Position[3];
		float Texcoord[2];
	} 
	Vertices[] =
	{
		{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}},
		{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}},
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
		{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}
	};

	// Tone Mapping Pass
	ZED3D9CommonTools::SetTexture(0, Input, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(1, Textures.Luminance, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	if (IntParameters.BloomPassCount >0)
		ZED3D9CommonTools::SetTexture(3, Textures.DownSampled2xA, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	if (IntParameters.BloomPassCount >1)
		ZED3D9CommonTools::SetTexture(4, Textures.DownSampled4xA, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	if (IntParameters.BloomPassCount >2)
		ZED3D9CommonTools::SetTexture(5, Textures.DownSampled8xA, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	if (IntParameters.BloomPassCount >3)
		ZED3D9CommonTools::SetTexture(6, Textures.DownSampled16xA, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetRenderTarget(0, Output);

	GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / Input->GetWidth(), 1.0f / Input->GetHeight(), 0.0f, 0.0f), 1);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / Output->GetWidth(), 1.0f / Output->GetWidth(), 0.0f, 0.0f), 1);

	GetDevice()->SetPixelShader(Shaders.ToneMap->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
}


void ZED3D9HDRProcessor::Process(float ElapsedTime)
{
	CreateRenderTargets();

	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	GetDevice()->SetVertexShader(Shaders.VertexShader->GetVertexShader());

	Parameters.MaxLuminanceChange = ElapsedTime;
	GetDevice()->SetPixelShaderConstantF(1, (float*)&Parameters, 2);
	GetDevice()->SetPixelShaderConstantI(0, (ZEInt*)&IntParameters, 1);

	bool BloomPasses[4];
	BloomPasses[0] = IntParameters.BloomPassCount > 0;
	BloomPasses[1] = IntParameters.BloomPassCount > 1;
	BloomPasses[2] = IntParameters.BloomPassCount > 2;
	BloomPasses[3] = IntParameters.BloomPassCount > 3;

	GetDevice()->SetPixelShaderConstantB(0, (BOOL*)BloomPasses, 4);


	MeasureLuminance(Input, Textures.OldLuminance, (ZED3D9ViewPort*)Textures.Luminance->GetViewPort());

	BrightPass(Input, (ZED3D9ViewPort*)Textures.DownSampled2xA->GetViewPort());
	
	if (IntParameters.BloomPassCount > 1)
		DownSample2x(Textures.DownSampled2xA, (ZED3D9ViewPort*)Textures.DownSampled4xA->GetViewPort());
	if (IntParameters.BloomPassCount > 2)
		DownSample2x(Textures.DownSampled4xA, (ZED3D9ViewPort*)Textures.DownSampled8xA->GetViewPort());
	if (IntParameters.BloomPassCount > 3)
		DownSample2x(Textures.DownSampled8xA, (ZED3D9ViewPort*)Textures.DownSampled16xA->GetViewPort());
	
	if (IntParameters.BloomPassCount > 0)
		BlurPass(Textures.DownSampled2xA, Textures.DownSampled2xB, (ZED3D9ViewPort*)Textures.DownSampled2xA->GetViewPort());
	if (IntParameters.BloomPassCount > 1)
		BlurPass(Textures.DownSampled4xA, Textures.DownSampled4xB, (ZED3D9ViewPort*)Textures.DownSampled4xA->GetViewPort());
	if (IntParameters.BloomPassCount > 2)
		BlurPass(Textures.DownSampled8xA, Textures.DownSampled8xB, (ZED3D9ViewPort*)Textures.DownSampled8xA->GetViewPort());
	if (IntParameters.BloomPassCount > 3)
		BlurPass(Textures.DownSampled16xA, Textures.DownSampled16xB, (ZED3D9ViewPort*)Textures.DownSampled16xA->GetViewPort());

	ToneMap(Input, Output);

	ZED3D9Texture2D* TempTexture;
	TempTexture = Textures.OldLuminance;
	Textures.OldLuminance = Textures.Luminance;
	Textures.Luminance = TempTexture;
}

void ZED3D9HDRProcessor::SetKey(float Key)
{
	Parameters.Key = Key;
}
float ZED3D9HDRProcessor::GetKey() const
{
	return Parameters.Key;
}

void ZED3D9HDRProcessor::SetBrightPassTreshold(float Treshold)
{
	Parameters.BrightPassTreshold = Treshold;
}

float ZED3D9HDRProcessor::GetBrightPassTreshold() const
{
	return Parameters.BrightPassTreshold;
}


void ZED3D9HDRProcessor::SetMaxLuminanceChangePerSecond(float LuminanceChange)
{
	Parameters.MaxLuminanceChange = LuminanceChange;
}
float ZED3D9HDRProcessor::GetMaxLuminanceChangePerSecond() const
{
	return Parameters.MaxLuminanceChange;
}

void ZED3D9HDRProcessor::SetBloomFactor(float Factor)
{
	Parameters.BloomFactor = Factor;
}

float ZED3D9HDRProcessor::GetBloomFactor() const
{
	return Parameters.BloomFactor;
}

void ZED3D9HDRProcessor::SetBloomStandardDeviation(float Deviation)
{
	Parameters.BloomStandardDeviation = Deviation;
}

float ZED3D9HDRProcessor::GetBloomStandardDeviation() const
{
	return Parameters.BloomStandardDeviation;
}

void ZED3D9HDRProcessor::SetBloomSampleCount(ZEUInt Count)
{
	if (Count > 16)
		Count = 16;

	IntParameters.BloomSampleCount = Count;
}

ZEUInt ZED3D9HDRProcessor::GetBloomSampleCount() const
{
	return IntParameters.BloomSampleCount;
}

void ZED3D9HDRProcessor::SetBloomPassCount(ZEUInt Count)
{
	if (Count > 4)
		Count = 4;

	IntParameters.BloomPassCount = Count;
}

ZEUInt ZED3D9HDRProcessor::GetBloomPassCount() const
{
	return IntParameters.BloomPassCount;
}

void ZED3D9HDRProcessor::SetInput(ZED3D9Texture2D* Input)
{
	this->Input = Input;
}

ZED3D9Texture2D* ZED3D9HDRProcessor::GetInput()
{
	return Input;
}

void ZED3D9HDRProcessor::SetOutput(ZED3D9ViewPort* Output)
{
	this->Output = Output;
}

ZED3D9ViewPort* ZED3D9HDRProcessor::GetOutput()
{
	return Output;
}

void ZED3D9HDRProcessor::Initialize()
{
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	Shaders.VertexShader = ZED3D9VertexShader::CreateShader("HDRProcessor.hlsl", "ZED3D9HDRProcessor_VertexShader", 0, "vs_3_0");
	Shaders.MeasureLuminanceStart = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "ZED3D9HDRProcessor_MeasureLuminanceStart", 0, "ps_3_0");
	Shaders.MeasureLuminanceDownSample3x = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "ZED3D9HDRProcessor_MeasureLuminanceDownSample3x", 0, "ps_3_0");
	Shaders.MeasureLuminanceEnd = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "ZED3D9HDRProcessor_MeasureLuminanceEnd", 0, "ps_3_0");
	Shaders.BrightPass = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "ZED3D9HDRProcessor_BrightPass", 0, "ps_3_0");
	Shaders.DownSample2x = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "ZED3D9HDRProcessor_DownSample2x", 0, "ps_3_0");
	Shaders.HorizontalBloom = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "ZED3D9HDRProcessor_HorizontalBloom", 0, "ps_3_0");
	Shaders.VerticalBloom = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "ZED3D9HDRProcessor_VerticalBloom", 0, "ps_3_0");
	Shaders.ToneMap = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "ZED3D9HDRProcessor_ToneMap", 0, "ps_3_0");
}	

void ZED3D9HDRProcessor::Deinitialize()
{
	ZED3D_RELEASE(VertexDeclaration);
	ZED3D_RELEASE(Shaders.VertexShader);
	ZED3D_RELEASE(Shaders.DownSample2x);
	ZED3D_RELEASE(Shaders.MeasureLuminanceStart); 
	ZED3D_RELEASE(Shaders.MeasureLuminanceDownSample3x); 
	ZED3D_RELEASE(Shaders.MeasureLuminanceEnd);
	ZED3D_RELEASE(Shaders.BrightPass);
	ZED3D_RELEASE(Shaders.HorizontalBloom);
	ZED3D_RELEASE(Shaders.VerticalBloom);
	ZED3D_RELEASE(Shaders.ToneMap);

	ReleaseRenderTargets();
}

ZED3D9HDRProcessor::ZED3D9HDRProcessor()
{
	memset(&Textures, 0, sizeof(Textures));
	memset(&Shaders, 0, sizeof(Shaders));

	//Parameters.Key = 0.7f;
	Parameters.Key = 0.9f;
	Parameters.BrightPassTreshold = 0.01f;
	Parameters.BloomFactor = 0.9f;
	Parameters.BloomStandardDeviation = 2.0f;
	Parameters.MaxLuminanceChangePerSecond = 0.0001f;
	IntParameters.BloomSampleCount = 7;
	IntParameters.BloomPassCount = 4;
}

ZED3D9HDRProcessor::~ZED3D9HDRProcessor()
{
	Deinitialize();
}
