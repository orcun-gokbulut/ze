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

#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"

#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"

#define	ZERN_FDF_SHADERS			1
#define ZERN_FDF_RENDER_STATE		2
#define ZERN_FDF_CONSTANT_BUFFER	4

static float GausianFunction(float X, float StandartDeviation)
{
	return (1.0f / ZEMath::Sqrt(2.0f * ZE_PI * StandartDeviation * StandartDeviation)) * ZEMath::Exp(-(X * X) / (2.0f * StandartDeviation * StandartDeviation));
}

static float GausianFunction(float X, float Y, float StandartDeviation)
{
	return (1.0f / (2.0f * ZE_PI * StandartDeviation * StandartDeviation)) * ZEMath::Exp(-(X * X + Y * Y) / (2.0f * StandartDeviation * StandartDeviation)); 
}

bool ZERNFilter::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(FilterConstants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

	return Update();
}

bool ZERNFilter::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	ScreenCoverVertexShader.Release();
	BlurHorizontalPixelShader.Release();
	BlurVerticalPixelShader.Release();
	BlurHorizontalComputeShader.Release();
	BlurVerticalComputeShader.Release();

	BlurHorizontalGraphicsRenderStateData.Release();
	BlurVerticalGraphicsRenderStateData.Release();

	BlurHorizontalComputeRenderStateData.Release();
	BlurVerticalComputeRenderStateData.Release();

	ConstantBuffer.Release();
	
	TempTexture.Release();

	return ZEInitializable::DeinitializeInternal();
}

bool ZERNFilter::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_FDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.Model = ZEGR_SM_5_0;
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString(ZEGRGraphicsModule::SAMPLE_COUNT)));

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNFiltering.hlsl";

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_PositionTexcoords";
	ScreenCoverVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(ScreenCoverVertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFiltering_BlurHorizontal_PixelShader";
	BlurHorizontalPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(BlurHorizontalPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFiltering_BlurVertical_PixelShader";
	BlurVerticalPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(BlurVerticalPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFiltering_Scale_PixelShader";
	ScalePixelShader = ZEGRShader::Compile(Options);
	zeCheckError(ScalePixelShader == NULL, false, "Cannot set pixel shader.");

	Options.EntryPoint = "ZERNFiltering_EdgeDetection_PixelShader";
	EdgeDetectionPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(EdgeDetectionPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNBlurCompute.hlsl";

	Options.Type = ZEGR_ST_COMPUTE;
	Options.EntryPoint = "ZERNBlurCompute_BlurHorizontal_ComputeShader_Main";
	BlurHorizontalComputeShader = ZEGRShader::Compile(Options);
	zeCheckError(BlurHorizontalComputeShader == NULL, false, "Cannot set compute shader.");

	Options.Type = ZEGR_ST_COMPUTE;
	Options.EntryPoint = "ZERNBlurCompute_BlurVertical_ComputeShader_Main";
	BlurVerticalComputeShader = ZEGRShader::Compile(Options);
	zeCheckError(BlurVerticalComputeShader == NULL, false, "Cannot set compute shader.");

	DirtyFlags.UnraiseFlags(ZERN_FDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_FDF_RENDER_STATE);

	return true;
}

bool ZERNFilter::UpdateConstantBuffer()
{
	if (!DirtyFlags.GetFlags(ZERN_FDF_CONSTANT_BUFFER))
		return true;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_FDF_CONSTANT_BUFFER);

	return true;
}

bool ZERNFilter::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZERN_FDF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState;
	RenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);

	RenderState.SetShader(ZEGR_ST_PIXEL, BlurHorizontalPixelShader);
	BlurHorizontalGraphicsRenderStateData = RenderState.Compile();
	zeCheckError(BlurHorizontalGraphicsRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, BlurVerticalPixelShader);
	BlurVerticalGraphicsRenderStateData = RenderState.Compile();
	zeCheckError(BlurVerticalGraphicsRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, ScalePixelShader);
	ScaleGraphicsRenderStateData = RenderState.Compile();
	zeCheckError(ScaleGraphicsRenderStateData == NULL, false, "Cannot set render state.");

	ZEGRDepthStencilState DepthStencilStateWriteStencil;
	DepthStencilStateWriteStencil.SetDepthTestEnable(true);
	DepthStencilStateWriteStencil.SetDepthWriteEnable(false);
	DepthStencilStateWriteStencil.SetDepthFunction(ZEGR_CF_LESS);
	DepthStencilStateWriteStencil.SetStencilTestEnable(true);
	DepthStencilStateWriteStencil.SetFrontStencilPass(ZEGR_SO_REPLACE);
	DepthStencilStateWriteStencil.SetBackStencilPass(ZEGR_SO_REPLACE);

	RenderState.SetDepthStencilState(DepthStencilStateWriteStencil);
	RenderState.SetShader(ZEGR_ST_PIXEL, EdgeDetectionPixelShader);

	EdgeDetectionGraphicsRenderStateData = RenderState.Compile();
	zeCheckError(EdgeDetectionGraphicsRenderStateData == NULL, false, "Cannot set render state.");

	ZEGRComputeRenderState ComputeRenderState;

	ComputeRenderState.SetComputeShader(BlurHorizontalComputeShader);
	BlurHorizontalComputeRenderStateData = ComputeRenderState.Compile();
	zeCheckError(BlurHorizontalComputeRenderStateData == NULL, false, "Cannot set compute render state.");

	ComputeRenderState.SetComputeShader(BlurVerticalComputeShader);
	BlurVerticalComputeRenderStateData = ComputeRenderState.Compile();
	zeCheckError(BlurVerticalComputeRenderStateData == NULL, false, "Cannot set compute render state.");

	DirtyFlags.UnraiseFlags(ZERN_FDF_RENDER_STATE);

	return true;
}

bool ZERNFilter::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateConstantBuffer())
		return false;

	return true;
}

void ZERNFilter::ApplyGaussianBlur(ZEGRContext* Context, const ZEGRTexture* InputTexture, ZEUInt KernelRadius, float StandartDeviation, bool PixelShader)
{
	zeDebugCheck(Context == NULL, "Context cannot be null");
	zeDebugCheck(InputTexture == NULL, "Input texture cannot be null");
	zeDebugCheck(KernelRadius == 0, "Kernel radius cannot be zero");
	zeDebugCheck(StandartDeviation == 0.0f, "Standart deviation cannot be zero");

	if (Constants.KernelRadius != KernelRadius)
	{
		Constants.KernelRadius = KernelRadius;
		DirtyFlags.RaiseFlags(ZERN_FDF_CONSTANT_BUFFER);
	}

	if (!Update())
		return;

	ZEUInt Width = InputTexture->GetWidth();
	ZEUInt Height = InputTexture->GetHeight();

	if (TempTexture == NULL || 
		TempTexture->GetWidth() != Width || TempTexture->GetHeight() != Height)
		TempTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, InputTexture->GetFormat(), InputTexture->GetResourceUsage(), InputTexture->GetResourceBindFlags());

	if (PixelShader)
	{
		Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, ConstantBuffer);
		Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, InputTexture->GetWidth(), InputTexture->GetHeight()));

		const ZEGRRenderTarget* OutputRenderTarget = InputTexture->GetRenderTarget();
		const ZEGRRenderTarget* TempRenderTarget = TempTexture->GetRenderTarget();

		Context->SetRenderState(BlurHorizontalGraphicsRenderStateData);
		Context->SetRenderTargets(1, &TempRenderTarget, NULL);
		Context->SetTexture(ZEGR_ST_PIXEL, 5, InputTexture);
		Context->Draw(3, 0);

		Context->SetRenderState(BlurVerticalGraphicsRenderStateData);
		Context->SetRenderTargets(1, &OutputRenderTarget, NULL);
		Context->SetTexture(ZEGR_ST_PIXEL, 5, TempTexture);
		Context->Draw(3, 0);
	}
	else
	{
		const ZEUInt GroupDimX = 320;

		Context->SetConstantBuffer(ZEGR_ST_COMPUTE, 9, ConstantBuffer);

		Context->SetComputeRenderState(BlurHorizontalComputeRenderStateData);
		Context->SetTexture(ZEGR_ST_COMPUTE, 5, InputTexture);
		Context->SetRWTexture(0, TempTexture);

		ZEUInt GroupCountX = (Width + GroupDimX - 1) / GroupDimX;
		Context->Dispatch(GroupCountX, Height, 1);

		Context->SetComputeRenderState(BlurVerticalComputeRenderStateData);
		Context->SetTexture(ZEGR_ST_COMPUTE, 5, TempTexture);
		Context->SetRWTexture(0, InputTexture);

		GroupCountX = (Height + GroupDimX - 1) / GroupDimX;
		Context->Dispatch(GroupCountX, Width, 1);
	}
}

void ZERNFilter::ApplyScale(ZEGRContext* Context, const ZEGRTexture* InputTexture, const ZEGRRenderTarget* OutputRenderTarget)
{
	zeDebugCheck(Context == NULL, "Context cannot be null");
	zeDebugCheck(InputTexture == NULL, "Input texture cannot be null");
	zeDebugCheck(OutputRenderTarget == NULL, "Output texture cannot be null");

	if (!Update())
		return;

	Viewport.SetWidth((float)OutputRenderTarget->GetWidth());
	Viewport.SetHeight((float)OutputRenderTarget->GetHeight());

	Context->SetRenderState(ScaleGraphicsRenderStateData);
	Context->SetRenderTargets(1, &OutputRenderTarget, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, InputTexture);
	Context->SetViewports(1, &Viewport);
	Context->Draw(3, 0);
}

void ZERNFilter::ApplyEdgeDetection(ZEGRContext* Context, float StencilRef)
{
	ZEGRTexture* DepthTexture;
	Context->GetTextures(ZEGR_ST_PIXEL, 4, 1, &DepthTexture);
	const ZEGRDepthStencilBuffer* DepthStencilBuffer = static_cast<ZEGRTexture*>(DepthTexture)->GetDepthStencilBuffer(true);

	Viewport.SetWidth(DepthStencilBuffer->GetWidth());
	Viewport.SetHeight(DepthStencilBuffer->GetHeight());

	Context->SetRenderState(EdgeDetectionGraphicsRenderStateData);
	Context->SetRenderTargets(0, NULL, DepthStencilBuffer);
	Context->SetStencilRef(StencilRef);
	Context->Draw(3, 0);
}

ZERNFilter::ZERNFilter()
{
	DirtyFlags.RaiseAll();

	memset(&Constants, 0, sizeof(Constants));
}

ZERNFilter::~ZERNFilter()
{
	Deinitialize();
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
