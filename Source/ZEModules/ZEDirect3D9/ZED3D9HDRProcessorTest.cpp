//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9HDRProcessorTest.cpp
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
#include "ZED3D9HDRProcessorTest.h"

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
	for (ZESize I = 0; I < KernelSize; I++)
	{
		Kernel[I] = GausianFunction((float)I - (float)HalfKernelSize, StandartDeviation);
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

void ZED3D9HDRProcessorTest::UpdateRenderTargets()
{
	float TargetWidth = (float)zeGraphics->GetFrameBufferViewPort()->GetWidth();
	float TargetHeight = (float)zeGraphics->GetFrameBufferViewPort()->GetHeight();

}

void ZED3D9HDRProcessorTest::ReleaseRenderTargets()
{
	
}

void ZED3D9HDRProcessorTest::ApplyBloomToneMap()
{

}

void ZED3D9HDRProcessorTest::GenerateBloomTexture()
{

}

void ZED3D9HDRProcessorTest::CreateMipMapsMeasureLuminance()
{

}

void ZED3D9HDRProcessorTest::Process(float ElapsedTime)
{
	UpdateRenderTargets();

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

	
}

void ZED3D9HDRProcessorTest::SetKey(float Key)
{
	Parameters.Key = Key;
}
float ZED3D9HDRProcessorTest::GetKey() const
{
	return Parameters.Key;
}

void ZED3D9HDRProcessorTest::SetBrightPassTreshold(float Treshold)
{
	Parameters.BrightPassTreshold = Treshold;
}

float ZED3D9HDRProcessorTest::GetBrightPassTreshold() const
{
	return Parameters.BrightPassTreshold;
}


void ZED3D9HDRProcessorTest::SetMaxLuminanceChangePerSecond(float LuminanceChange)
{
	Parameters.MaxLuminanceChange = LuminanceChange;
}
float ZED3D9HDRProcessorTest::GetMaxLuminanceChangePerSecond() const
{
	return Parameters.MaxLuminanceChange;
}

void ZED3D9HDRProcessorTest::SetBloomFactor(float Factor)
{
	Parameters.BloomFactor = Factor;
}

float ZED3D9HDRProcessorTest::GetBloomFactor() const
{
	return Parameters.BloomFactor;
}

void ZED3D9HDRProcessorTest::SetBloomStandardDeviation(float Deviation)
{
	Parameters.BloomStandardDeviation = Deviation;
}

float ZED3D9HDRProcessorTest::GetBloomStandardDeviation() const
{
	return Parameters.BloomStandardDeviation;
}

void ZED3D9HDRProcessorTest::SetBloomSampleCount(ZEUInt Count)
{
	if (Count > 16)
		Count = 16;

	IntParameters.BloomSampleCount = Count;
}

ZEUInt ZED3D9HDRProcessorTest::GetBloomSampleCount() const
{
	return IntParameters.BloomSampleCount;
}

void ZED3D9HDRProcessorTest::SetBloomPassCount(ZEUInt Count)
{
	if (Count > 4)
		Count = 4;

	IntParameters.BloomPassCount = Count;
}

ZEUInt ZED3D9HDRProcessorTest::GetBloomPassCount() const
{
	return IntParameters.BloomPassCount;
}

void ZED3D9HDRProcessorTest::SetInput(ZED3D9Texture2D* Input)
{
	this->Input = Input;
}

ZED3D9Texture2D* ZED3D9HDRProcessorTest::GetInput()
{
	return Input;
}

void ZED3D9HDRProcessorTest::SetOutput(ZED3D9ViewPort* Output)
{
	this->Output = Output;
}

ZED3D9ViewPort* ZED3D9HDRProcessorTest::GetOutput()
{
	return Output;
}

void ZED3D9HDRProcessorTest::Initialize()
{
	// Vertex declaration for screen aligned quad
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, D3DDECL_END()
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

void ZED3D9HDRProcessorTest::Deinitialize()
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

ZEHDRScreenAlignedQuad ZED3D9HDRProcessorTest::Vertices[] =
{
	{-1.0f,  1.0f, 0.0f},
	{ 1.0f,  1.0f, 0.0f},
	{-1.0f, -1.0f, 0.0f},
	{ 1.0f, -1.0f, 0.0f}
};

ZED3D9HDRProcessorTest::ZED3D9HDRProcessorTest()
{
	//Parameters.Key = 0.7f;
	Parameters.Key = 0.9f;
	Parameters.BrightPassTreshold = 0.95f;
	Parameters.BloomFactor = 1.0f;
	Parameters.BloomStandardDeviation = 2.0f;
	Parameters.MaxLuminanceChangePerSecond = 0.0001f;
	IntParameters.BloomSampleCount = 7;
	IntParameters.BloomPassCount = 4;
}

ZED3D9HDRProcessorTest::~ZED3D9HDRProcessorTest()
{
	Deinitialize();
}
