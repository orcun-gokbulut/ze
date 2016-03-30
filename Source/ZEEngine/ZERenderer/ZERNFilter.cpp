//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNFilter.cpp
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

#include "ZERNFilter.h"
#include "ZEGraphics\ZEGRTexture2D.h"
#include "ZEGraphics\ZEGRContext.h"
#include "ZEGraphics\ZEGRShaderCompileOptions.h"
#include "ZEGraphics\ZEGRShader.h"
#include "ZEGraphics\ZEGRSampler.h"
#include "ZEGraphics\ZEGRRenderState.h"
#include "ZEGraphics\ZEGRConstantBuffer.h"
#include "ZEMath\ZEMath.h"
#include "ZEMath\ZEAngle.h"

#define ZERN_FDF_SHADERS			1
#define ZERN_FDF_CONSTANT_BUFFER	2
#define ZERN_FDF_RENDER_STATE		4

static float GausianFunction(float x, float StandartDeviation)
{
	return (1.0f / (ZEMath::Sqrt(2.0f * ZE_PI) * StandartDeviation)) * ZEMath::Power(ZE_E, -((x * x) / (2.0f * StandartDeviation * StandartDeviation)));
}

static float GausianFunction(float x, float y, float StandartDeviation)
{
	return (1.0f / (2.0f * ZE_PI * StandartDeviation * StandartDeviation)) * ZEMath::Power(ZE_E, -((x * x + y * y) / (2.0f * StandartDeviation * StandartDeviation))); 
}

bool ZERNFilter::InitializeSelf()
{
	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(FilterConstants));

	DirtyFlags.RaiseAll();

	ZEGRSamplerDescription SamplerDescriptionPointClamp;
	SamplerDescriptionPointClamp.MinFilter = ZEGR_TFM_POINT;
	SamplerDescriptionPointClamp.MagFilter = ZEGR_TFM_POINT;
	SamplerDescriptionPointClamp.MipFilter = ZEGR_TFM_POINT;

	SamplerPointClamp = ZEGRSampler::GetSampler(SamplerDescriptionPointClamp);

	return Update();
}

void ZERNFilter::DeinitializeSelf()
{
	VertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
	ConstantBuffer.Release();
}

void ZERNFilter::SetInput(const ZEGRTexture2D* Input)
{
	this->Input = Input;
}

const ZEGRTexture2D* ZERNFilter::GetInput() const
{
	return Input;
}

void ZERNFilter::SetOutput(const ZEGRRenderTarget* Output)
{
	this->Output = Output;
}

const ZEGRRenderTarget* ZERNFilter::GetOutput() const
{
	return Output;
}

ZEUInt ZERNFilter::GetKernelSize() const
{
	return Constants.KernelSize;
}

void ZERNFilter::SetKernelValues(const ZEVector4* Values, ZESize KernelSize)
{
	zeDebugCheck(Values == NULL, "Kernel Values cannot be NULL");
	zeDebugCheck(KernelSize == 0, "Kernel size cannot be zero");

	if((KernelSize & 0x01) != 0 && ((KernelSize >> 1) & 0x01) == 0)
	{
		ComputeLinearFilter(Values, KernelSize);
	}
	else
	{
		memcpy(&Constants.KernelValues[0], Values, KernelSize * sizeof(ZEVector4));
		Constants.KernelSize = KernelSize;
	}

	DirtyFlags.RaiseFlags(ZERN_FDF_CONSTANT_BUFFER);
}

const ZEVector4* ZERNFilter::GetKernelValues()
{
	return &Constants.KernelValues[0];
}

void ZERNFilter::ComputeLinearFilter(const ZEVector4* Values, ZESize KernelSize)
{
	ZEUInt HalfWidth = KernelSize / 2;
	ZEUInt IterationCount = HalfWidth / 2;

	for(ZEUInt I = 0; I < IterationCount; I++)
	{
		ZEUInt Index = I * 2;

		ZEVector4 Value1 = Values[Index];
		ZEVector4 Value2 = Values[Index + 1];

		float Weight = Value1.w + Value2.w;
		float CoordX = (ZEMath::Abs(Value1.x) * Value1.w + ZEMath::Abs(Value2.x) * Value2.w) / Weight;
		float CoordY = (ZEMath::Abs(Value1.y) * Value1.w + ZEMath::Abs(Value2.y) * Value2.w) / Weight;

		Constants.KernelValues[I].x = -CoordX;
		Constants.KernelValues[I].y = -CoordY;
		Constants.KernelValues[I].z = 0.0f;
		Constants.KernelValues[I].w = Weight;

		Constants.KernelValues[HalfWidth - I].x = CoordX;
		Constants.KernelValues[HalfWidth - I].y = CoordY;
		Constants.KernelValues[HalfWidth - I].z = 0.0f;
		Constants.KernelValues[HalfWidth - I].w = Weight;
	}

	Constants.KernelValues[IterationCount] = Values[HalfWidth];
	Constants.KernelSize = HalfWidth + 1;
}

bool ZERNFilter::UpdateShaders()
{
	if(!DirtyFlags.GetFlags(ZERN_FDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNFiltering.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_Position";
	VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(VertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFiltering_PixelShader";
	PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(PixelShader == NULL, false, "Cannot set pixel shader.");

	DirtyFlags.UnraiseFlags(ZERN_FDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_FDF_RENDER_STATE);

	return true;
}

bool ZERNFilter::UpdateConstantBuffer()
{
	if(!DirtyFlags.GetFlags(ZERN_FDF_CONSTANT_BUFFER))
		return true;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_FDF_CONSTANT_BUFFER);

	return true;
}

bool ZERNFilter::UpdateRenderState()
{
	if(!DirtyFlags.GetFlags(ZERN_FDF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState;

	ZEGRDepthStencilState DepthStencilState;
	DepthStencilState.SetDepthTestEnable(false);
	DepthStencilState.SetDepthWriteEnable(false);

	RenderState.SetDepthStencilState(DepthStencilState);

	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PixelShader);
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderStateData = RenderState.Compile();
	zeCheckError(RenderStateData == NULL, false, "Cannot set render state.");

	DirtyFlags.UnraiseFlags(ZERN_FDF_RENDER_STATE);

	return true;
}

bool ZERNFilter::Update()
{
	if(!UpdateShaders())
		return false;

	if(!UpdateRenderState())
		return false;

	if(!UpdateConstantBuffer())
		return false;

	return true;
}

void ZERNFilter::Process(ZEGRContext* Context)
{
	zeDebugCheck(Input == NULL, "Input texture cannot be null");
	zeDebugCheck(Output == NULL, "Output render target cannot be null");

	if(!Update())
		return;

	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerPointClamp.GetPointer());
	Context->SetTexture(ZEGR_ST_PIXEL, 5, Input);
	Context->SetRenderTargets(1, &Output, NULL);
	Context->SetVertexBuffers(0, 0, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, ConstantBuffer);
	Context->SetRenderState(RenderStateData);

	Context->Draw(3, 0);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 0, NULL);
}

ZERNFilter::ZERNFilter()
{
	Input = NULL;
	Output = NULL;

	memset(&Constants, 0, sizeof(Constants));
}

void ZERNFilter::GenerateGaussianKernel(ZEArray<ZEVector4>& Values, ZEInt Size, float StandartDeviation, bool Horizontal)
{
	if(Values.GetCount() != Size)
		Values.Resize(Size);

	ZEInt HalfSize = Size / 2;

	for(ZEInt I = 0; I < Size; ++I)
	{
		Values[I].x = Horizontal ? I - HalfSize : 0.0f;
		Values[I].y = !Horizontal ? I - HalfSize : 0.0f;
		Values[I].z = 0.0f;
		Values[I].w = GausianFunction((Values[I].x + Values[I].y), StandartDeviation);
	}
}
