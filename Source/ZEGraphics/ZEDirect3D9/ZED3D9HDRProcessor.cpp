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

static float GausianFunction(float x, float StandartDeviation)
{
	return (1.0f / (sqrtf(2.0f * ZE_PI) * StandartDeviation)) * powf(ZE_E, -((x * x) / (2.0f * StandartDeviation * StandartDeviation)));
}


static float GausianFunction(float x, float y, float StandartDeviation)
{
	return (1.0f / (2.0f * ZE_PI * StandartDeviation * StandartDeviation)) * powf(ZE_E, -((x * x + y * y) / (2.0f * StandartDeviation * StandartDeviation))); 
}


static void FillGaussianKernel1D(float* Kernel, size_t KernelSize, float StandartDeviation)
{
	int HalfKernelSize = (KernelSize - 1) / 2;
	for (int x = 0; x < KernelSize; x++)
	{
		Kernel[4 * x] = x - HalfKernelSize;
		Kernel[4 * x + 1] = GausianFunction(x - HalfKernelSize, StandartDeviation);
		zeOutput("{%lff, %lff}, \r\n", Kernel[4 * x], Kernel[4 * x + 1]);
	}
}


static void FillGaussianKernel2D(float* Kernel, size_t KernelSize, float StandartDeviation)
{
	int HalfKernelSize = (KernelSize - 1) / 2;
	for (int y = 0; y < KernelSize; y++)
	{
		for (int x = 0; x < KernelSize; x++)
		{
			float* CurrentSample = &Kernel[4 * KernelSize * y + x];
			CurrentSample[0] = x - HalfKernelSize;
			CurrentSample[1] = y - HalfKernelSize;
			CurrentSample[2] = GausianFunction(x - HalfKernelSize, y - HalfKernelSize, StandartDeviation);
			zeOutput("%lf ", CurrentSample[2]);
		}
		zeOutput("\r\n");
	}
}

void ZED3D9HDRProcessor::CreateRenderTargets()
{
	if (Textures.Luminance5 == NULL)
		Textures.Luminance5 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.Luminance5->Create(243, 243, ZE_TPF_LUM_HDR);

	if (Textures.Luminance4 == NULL)
		Textures.Luminance4 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.Luminance4->Create(81, 81, ZE_TPF_LUM_HDR);

	if (Textures.Luminance3 == NULL)
		Textures.Luminance3 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.Luminance3->Create(27, 27, ZE_TPF_LUM_HDR);

	if (Textures.Luminance2 == NULL)
		Textures.Luminance2 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.Luminance2->Create(9, 9, ZE_TPF_LUM_HDR);

	if (Textures.Luminance1 == NULL)
		Textures.Luminance1 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.Luminance1->Create(3, 3, ZE_TPF_LUM_HDR);

	if (Textures.Luminance == NULL)
		Textures.Luminance = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.Luminance->Create(1, 1, ZE_TPF_LUM_HDR);

	if (Textures.OldLuminance == NULL)
		Textures.OldLuminance = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.OldLuminance->Create(1, 1, ZE_TPF_LUM_HDR);

	/*if (Textures.DownSampled2x == NULL)
		Textures.DownSampled2x = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.DownSampled2x->Create( ScreenWidth / 2, ScreenHeight / 2, ZE_TPF_RGBA_HDR);*/

	if (Textures.DownSampled4xA == NULL)
		Textures.DownSampled4xA = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.DownSampled4xA->Create( ScreenWidth / 4, ScreenHeight / 4, ZE_TPF_RGBA_HDR);

	if (Textures.DownSampled4xB == NULL)
		Textures.DownSampled4xB = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Textures.DownSampled4xB->Create( ScreenWidth / 4, ScreenHeight / 4, ZE_TPF_RGBA_HDR);
}


void ZED3D9HDRProcessor::ReleaseRenderTargets()
{
	//ZED3D_DESTROY(Textures.DownSampled2x);
	ZED3D_DESTROY(Textures.DownSampled4xA);
	ZED3D_DESTROY(Textures.DownSampled4xB);

	ZED3D_DESTROY(Textures.Luminance5);
	ZED3D_DESTROY(Textures.Luminance4);
	ZED3D_DESTROY(Textures.Luminance3);
	ZED3D_DESTROY(Textures.Luminance2);
	ZED3D_DESTROY(Textures.Luminance1);
	ZED3D_DESTROY(Textures.Luminance);
	ZED3D_DESTROY(Textures.OldLuminance);
}

void ZED3D9HDRProcessor::Initialize()
{
	size_t ScreenWidth, ScreenHeight;
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	Shaders.VertexShader = ZED3D9VertexShader::CreateShader("HDRProcessor.hlsl", "VSMain", 0, "vs_3_0");
	Shaders.LumMeasureStart = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSLumMeasureStart", 0, "ps_3_0");
	Shaders.LumDownSample3x = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSLumDownSample3x", 0, "ps_3_0");
	Shaders.LumMeasureEnd = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSLumMeasureEnd", 0, "ps_3_0");
	Shaders.BrightPass = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSBrightPass", 0, "ps_3_0");
	Shaders.ColorDownSample4x = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSColorDownSample4x", 0, "ps_3_0");
	Shaders.HorizontalBloom = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSHorizontalBloom", 0, "ps_3_0");
	Shaders.VerticalBloom = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSVerticalBloom", 0, "ps_3_0");
	Shaders.ToneMap = ZED3D9PixelShader::CreateShader("HDRProcessor.hlsl", "PSToneMap", 0, "ps_3_0");
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

void ZED3D9HDRProcessor::Deinitialize()
{
	ZED3D_RELEASE(VertexDeclaration);
	ZED3D_RELEASE(Shaders.VertexShader);
	ZED3D_RELEASE(Shaders.ColorDownSample4x);
	ZED3D_RELEASE(Shaders.LumMeasureStart); 
	ZED3D_RELEASE(Shaders.LumDownSample3x); 
	ZED3D_RELEASE(Shaders.LumMeasureEnd);
	ZED3D_RELEASE(Shaders.BrightPass);
	ZED3D_RELEASE(Shaders.HorizontalBloom);
	ZED3D_RELEASE(Shaders.VerticalBloom);
	ZED3D_RELEASE(Shaders.ToneMap);

	ReleaseRenderTargets();
}

void ZED3D9HDRProcessor::Process()
{
	ScreenWidth = GetModule()->GetScreenWidth();
	ScreenHeight = GetModule()->GetScreenHeight();

	CreateRenderTargets();
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

	static const float Kernel2x2[4][4] = 
	{
		{-0.5f, -0.5f, 0.0f, 0.0f},
		{-0.5f,  0.5f, 0.0f, 0.0f},

		{ 0.5f, -0.5f, 0.0f, 0.0f},
		{ 0.5f,  0.5f, 0.0f, 0.0f}
	};

	static const float Kernel3x3[9][4] = 
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

	static const float Kernel4x4[16][4] = 
	{
		{-1.5f, -1.5f, 0.0f, 0.0f},
		{-1.5f, -0.5f, 0.0f, 0.0f},
		{-1.5f,  0.5f, 0.0f, 0.0f},
		{-1.5f,  1.5f, 0.0f, 0.0f},

		{-0.5f, -1.5f, 0.0f, 0.0f},
		{-0.5f, -0.5f, 0.0f, 0.0f},
		{-0.5f,  0.5f, 0.0f, 0.0f},
		{-0.5f,  1.5f, 0.0f, 0.0f},

		{ 0.5f, -1.5f, 0.0f, 0.0f},
		{ 0.5f, -0.5f, 0.0f, 0.0f},
		{ 0.5f,  0.5f, 0.0f, 0.0f},
		{ 0.5f,  1.5f, 0.0f, 0.0f},

		{ 1.5f, -1.5f, 0.0f, 0.0f},
		{ 1.5f, -0.5f, 0.0f, 0.0f},
		{ 1.5f,  0.5f, 0.0f, 0.0f},
		{ 1.5f,  1.5f, 0.0f, 0.0f}
	};

	static const float BloomKernel[15][4] = 
	{
		{-7.000000f, 0.008741f, 0.0f, 0.0f}, 
		{-6.000000f, 0.017997f, 0.0f, 0.0f}, 
		{-5.000000f, 0.033159f, 0.0f, 0.0f}, 
		{-4.000000f, 0.054670f, 0.0f, 0.0f}, 
		{-3.000000f, 0.080657f, 0.0f, 0.0f}, 
		{-2.000000f, 0.106483f, 0.0f, 0.0f}, 
		{-1.000000f, 0.125794f, 0.0f, 0.0f}, 
		{0.000000f, 0.132981f, 0.0f, 0.0f}, 
		{1.000000f, 0.125794f, 0.0f, 0.0f}, 
		{2.000000f, 0.106483f, 0.0f, 0.0f}, 
		{3.000000f, 0.080657f, 0.0f, 0.0f}, 
		{4.000000f, 0.054670f, 0.0f, 0.0f}, 
		{5.000000f, 0.033159f, 0.0f, 0.0f}, 
		{6.000000f, 0.017997f, 0.0f, 0.0f}, 
		{7.000000f, 0.008741f, 0.0f, 0.0f}, 
	};

	struct 
	{
		float Exposure;
		float BrightPassTreshold;
		float BrightPassOffset;
		float BloomFactor;
		float MaxLimunance;
		float MinLimunance;
		float ElapsedTime;
		float Reserved0; 
	} Parameters;

	Parameters.Exposure = 0.78f;
	Parameters.BrightPassTreshold = 0.5f;
	Parameters.BrightPassOffset = 0.2f;
	Parameters.BloomFactor = 1.5f;
	Parameters.MaxLimunance = 2.0f;

	Parameters.MinLimunance = 0.2f;
	Parameters.ElapsedTime = zeCore->GetFrameTime();

	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	GetDevice()->SetVertexShader(Shaders.VertexShader->GetVertexShader());

	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);



	GetDevice()->SetPixelShaderConstantF(1, (const float*)&Parameters, 2);
	GetDevice()->SetPixelShaderConstantF(10, (const float*) Kernel2x2, 4);
	GetDevice()->SetPixelShaderConstantF(15, (const float*) Kernel3x3, 9);

	// Luminance Measure Start Pass
	GetDevice()->SetRenderTarget(0, ((ZED3D9ViewPort*)Textures.Luminance5->GetViewPort())->FrameBuffer);
	GetDevice()->SetTexture(0, Input->Texture);
	GetDevice()->SetPixelShader(Shaders.LumMeasureStart->GetPixelShader());
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
	GetDevice()->SetPixelShader(Shaders.LumDownSample3x->GetPixelShader());
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

	ZED3D9Texture2D* TempTexture;
	TempTexture = Textures.OldLuminance;
	Textures.OldLuminance = Textures.Luminance;
	Textures.Luminance = TempTexture;

	// Luminance Measure End Pass
	GetDevice()->SetRenderTarget(0, ((ZED3D9ViewPort*)Textures.Luminance->GetViewPort())->FrameBuffer);
	GetDevice()->SetTexture(0, Textures.Luminance1->Texture);
	GetDevice()->SetTexture(3, Textures.OldLuminance->Texture);
	GetDevice()->SetPixelShader(Shaders.LumMeasureEnd->GetPixelShader());
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f, 1.0f, 0.0f, 0.0f), 1);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / 3.0f, 1.0f / 3.0f, 0.0f, 0.0f), 1);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	// Bright Pass
	GetDevice()->SetRenderTarget(0, ((ZED3D9ViewPort*)Textures.DownSampled4xA->GetViewPort())->FrameBuffer);
	GetDevice()->SetTexture(0, Input->Texture);
	GetDevice()->SetTexture(1, Textures.Luminance->Texture);
	GetDevice()->SetPixelShader(Shaders.BrightPass->GetPixelShader());

	GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / ScreenWidth, 1.0f / ScreenHeight, 0.0f, 0.0f), 1);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / (ScreenHeight / 4.0f), 1.0f / (ScreenHeight / 4.0f), 0.0f, 0.0f), 1);

	GetDevice()->SetPixelShaderConstantF(15, (const float*) Kernel4x4, 16);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	// Down Sample 4x
	/*SetRenderTarget(Textures.DownSampled8xA);
	GetDevice()->SetPixelShaderConstantF(15, (const float*) Kernel4x4, 16);
	GetDevice()->SetTexture(0, Textures.DownSampled4xB);
	GetDevice()->SetPixelShader(Shaders.ColorDownSample4x);

	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / (ScreenWidth / 4.0f), 1.0f / (ScreenWidth / 4.0f), 0.0f, 0.0f), 1);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4 (1.0f / (ScreenWidth / 4.0f), 1.0f / (ScreenHeight / 4.0f), 0.0f, 0.0f), 1);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));*/

	// Vertical Bloom Pass
	GetDevice()->SetTexture(0, Textures.DownSampled4xA->Texture);
	GetDevice()->SetRenderTarget(0, ((ZED3D9ViewPort*)Textures.DownSampled4xB->GetViewPort())->FrameBuffer);
	GetDevice()->SetPixelShaderConstantF(15, (const float*)BloomKernel, 15);
	GetDevice()->SetPixelShader(Shaders.VerticalBloom->GetPixelShader()); 
	//GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / (ScreenWidth / 8.0f), 1.0f / (ScreenWidth / 8.0f), 0.0f, 0.0f), 1);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	// Horizontal Bloom Pass
	GetDevice()->SetTexture(0, Textures.DownSampled4xB->Texture);
	GetDevice()->SetRenderTarget(0, ((ZED3D9ViewPort*)Textures.DownSampled4xA->GetViewPort())->FrameBuffer);
	GetDevice()->SetPixelShader(Shaders.HorizontalBloom->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	// Tone Mapping Pass
	GetDevice()->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	GetDevice()->SetRenderTarget(0, Output->FrameBuffer);
	GetDevice()->SetTexture(0, Input->Texture);
	GetDevice()->SetTexture(1, Textures.Luminance->Texture);
	GetDevice()->SetTexture(2, Textures.DownSampled4xB->Texture);
	GetDevice()->SetPixelShader(Shaders.ToneMap->GetPixelShader());
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4 (1.0f / ScreenWidth, 1.0f / ScreenHeight, 0.0f, 0.0f), 1);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
}

ZED3D9HDRProcessor::ZED3D9HDRProcessor()
{
	memset(&Textures, 0, sizeof(Textures));
	memset(&Shaders, 0, sizeof(Shaders));
}

ZED3D9HDRProcessor::~ZED3D9HDRProcessor()
{
	Deinitialize();
}
