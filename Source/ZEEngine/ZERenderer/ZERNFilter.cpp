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
#include "ZERNStage.h"

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

	return true;
}

bool ZERNFilter::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	ScreenCoverVertexShader.Release();

	InputTexture = NULL;
	InputSubresource = 0;

	OutputTexture = NULL;
	OutputSubresource = 0;

	return ZEInitializable::DeinitializeInternal();
}

bool ZERNFilter::UpdateShaders()
{
	ZEGRShaderCompileOptions Options;
	Options.Model = ZEGR_SM_5_0;
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString::FromInt32(ZEGRGraphicsModule::SAMPLE_COUNT)));

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNFiltering.hlsl";

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_PositionTexcoords";
	ScreenCoverVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(ScreenCoverVertexShader == NULL, false, "Cannot set vertex shader.");

	return true;
}

bool ZERNFilter::UpdateRenderStates()
{
	return true;
}

bool ZERNFilter::UpdateConstantBuffers()
{
	return true;
}

bool ZERNFilter::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateConstantBuffers())
		return false;

	return true;
}

void ZERNFilter::SetInputTexture(const ZEGRTexture* InputTexture)
{
	this->InputTexture = InputTexture;
}

const ZEGRTexture* ZERNFilter::GetInputTexture() const
{
	return InputTexture;
}

void ZERNFilter::SetOutputTexture(const ZEGRTexture* OutputTexture)
{
	this->OutputTexture = OutputTexture;
}

const ZEGRTexture* ZERNFilter::GetOutputTexture() const
{
	return OutputTexture;
}

void ZERNFilter::Apply(ZEGRContext* Context)
{
	zeDebugCheck(Context == NULL, "Context cannot be null");
	zeDebugCheck(InputTexture == NULL, "Input texture cannot be null");
	zeDebugCheck(InputTexture->GetLevelCount() * InputTexture->GetDepthOrArrayCount() <= InputSubresource, "Input subresource index is out-of-range");
	zeDebugCheck(InputTexture == OutputTexture, "Input texture cannot be the same as output texture");
	//zeDebugCheck(OutputTexture == NULL, "Output texture cannot be null");
}

ZERNFilter::ZERNFilter() : Link(this)
{
	DirtyFlags.RaiseAll();

	InputTexture = NULL;
	InputSubresource = 0;

	OutputTexture = NULL;
	OutputSubresource = 0;
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

bool ZERNFilterGaussianBlur::InitializeInternal()
{
	if (!ZERNFilter::InitializeInternal())
		return false;

	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

	return true;
}

bool ZERNFilterGaussianBlur::DeinitializeInternal()
{
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

	return ZERNFilter::DeinitializeInternal();
}

bool ZERNFilterGaussianBlur::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_FDF_SHADERS))
		return true;

	if (!ZERNFilter::UpdateShaders())
		return false;

	ZEGRShaderCompileOptions Options;
	Options.Model = ZEGR_SM_5_0;
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString::FromInt32(ZEGRGraphicsModule::SAMPLE_COUNT)));

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNFiltering.hlsl";

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFiltering_BlurHorizontal_PixelShader";
	BlurHorizontalPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(BlurHorizontalPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFiltering_BlurVertical_PixelShader";
	BlurVerticalPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(BlurVerticalPixelShader == NULL, false, "Cannot set pixel shader.");

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

bool ZERNFilterGaussianBlur::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZERN_FDF_RENDER_STATE))
		return true;

	if (!ZERNFilter::UpdateRenderStates())
		return false;

	ZEGRRenderState RenderState;
	RenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);

	RenderState.SetShader(ZEGR_ST_PIXEL, BlurHorizontalPixelShader);
	BlurHorizontalGraphicsRenderStateData = RenderState.Compile();
	zeCheckError(BlurHorizontalGraphicsRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, BlurVerticalPixelShader);
	BlurVerticalGraphicsRenderStateData = RenderState.Compile();
	zeCheckError(BlurVerticalGraphicsRenderStateData == NULL, false, "Cannot set render state.");

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

bool ZERNFilterGaussianBlur::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZERN_FDF_CONSTANT_BUFFER))
		return true;

	if (!ZERNFilter::UpdateConstantBuffers())
		return false;

	zeDebugCheck(Constants.KernelRadius == 0, "Kernel radius cannot be zero");
	zeDebugCheck(Constants.StandartDeviation == 0.0f, "Standart deviation cannot be zero");

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_FDF_CONSTANT_BUFFER);
}

void ZERNFilterGaussianBlur::SetUseComputeShader(bool UseComputeShader)
{
	this->UseComputeShader = UseComputeShader;
}

bool ZERNFilterGaussianBlur::GetUseComputeShader() const
{
	return UseComputeShader;
}

void ZERNFilterGaussianBlur::SetKernelRadius(ZEUInt KernelRadius)
{
	if (Constants.KernelRadius == KernelRadius)
		return;

	Constants.KernelRadius = KernelRadius;

	DirtyFlags.RaiseFlags(ZERN_FDF_CONSTANT_BUFFER);
}

ZEUInt ZERNFilterGaussianBlur::GetKernelRadius() const
{
	return Constants.KernelRadius;
}

void ZERNFilterGaussianBlur::SetStandartDeviation(float StandartDeviation)
{
	if (Constants.StandartDeviation == StandartDeviation)
		return;

	Constants.StandartDeviation = StandartDeviation;

	DirtyFlags.RaiseFlags(ZERN_FDF_CONSTANT_BUFFER);
}

float ZERNFilterGaussianBlur::GetStandartDeviation() const
{
	return Constants.StandartDeviation;
}

void ZERNFilterGaussianBlur::Apply(ZEGRContext* Context)
{
	ZERNFilter::Apply(Context);

	if (!Update())
		return;

	ZEUInt Width = InputTexture->GetWidth();
	ZEUInt Height = InputTexture->GetHeight();

	if (TempTexture == NULL || 
		TempTexture->GetWidth() != Width || TempTexture->GetHeight() != Height)
		TempTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, InputTexture->GetFormat(), InputTexture->GetResourceUsage(), InputTexture->GetResourceBindFlags());

	if (!UseComputeShader)
	{
		Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, ConstantBuffer);
		Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, InputTexture->GetWidth(), InputTexture->GetHeight()));

		const ZEGRRenderTarget* OutputRenderTarget = (OutputTexture != NULL) ? OutputTexture->GetRenderTarget() : InputTexture->GetRenderTarget();
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

ZERNFilterGaussianBlur::ZERNFilterGaussianBlur()
{
	UseComputeShader = false;

	Constants.KernelRadius = 5;
	Constants.StandartDeviation = 2.0f;
}

bool ZERNFilterImageTransform::InitializeInternal()
{
	if (!ZERNFilter::InitializeInternal())
		return false;

	return Update();
}

bool ZERNFilterImageTransform::DeinitializeInternal()
{
	ScalePixelShader.Release();
	ScaleGraphicsRenderStateData.Release();

	return ZERNFilter::DeinitializeInternal();
}

bool ZERNFilterImageTransform::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_FDF_SHADERS))
		return true;

	if (!ZERNFilter::UpdateShaders())
		return false;

	ZEGRShaderCompileOptions Options;
	Options.Model = ZEGR_SM_5_0;
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString::FromInt32(ZEGRGraphicsModule::SAMPLE_COUNT)));

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNFiltering.hlsl";

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFiltering_Scale_PixelShader";
	ScalePixelShader = ZEGRShader::Compile(Options);
	zeCheckError(ScalePixelShader == NULL, false, "Cannot set pixel shader.");

	DirtyFlags.UnraiseFlags(ZERN_FDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_FDF_RENDER_STATE);

	return true;
}

bool ZERNFilterImageTransform::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZERN_FDF_RENDER_STATE))
		return true;

	if (!ZERNFilter::UpdateRenderStates())
		return false;

	ZEGRRenderState RenderState;
	RenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, ScalePixelShader);
	ScaleGraphicsRenderStateData = RenderState.Compile();
	zeCheckError(ScaleGraphicsRenderStateData == NULL, false, "Cannot set render state.");

	DirtyFlags.UnraiseFlags(ZERN_FDF_RENDER_STATE);

	return true;
}

void ZERNFilterImageTransform::Apply(ZEGRContext* Context)
{
	ZERNFilter::Apply(Context);

	if (!Update())
		return;

	Viewport.SetWidth((float)OutputTexture->GetWidth());
	Viewport.SetHeight((float)OutputTexture->GetHeight());

	const ZEGRRenderTarget* RenderTarget = OutputTexture->GetRenderTarget();

	Context->SetRenderState(ScaleGraphicsRenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, InputTexture);
	Context->SetViewports(1, &Viewport);
	Context->Draw(3, 0);
}

ZERNFilterImageTransform::ZERNFilterImageTransform()
{

}

bool ZERNFilterEdgeDetection::InitializeInternal()
{
	if (!ZERNFilter::InitializeInternal())
		return false;

	return Update();
}

bool ZERNFilterEdgeDetection::DeinitializeInternal()
{
	EdgeDetectionPixelShader.Release();
	EdgeDetectionGraphicsRenderStateData.Release();

	return ZERNFilter::DeinitializeInternal();
}

bool ZERNFilterEdgeDetection::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_FDF_SHADERS))
		return true;

	if (!ZERNFilter::UpdateShaders())
		return false;

	ZEGRShaderCompileOptions Options;
	Options.Model = ZEGR_SM_5_0;
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString::FromInt32(ZEGRGraphicsModule::SAMPLE_COUNT)));

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNFiltering.hlsl";

	Options.EntryPoint = "ZERNFiltering_EdgeDetection_PixelShader";
	EdgeDetectionPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(EdgeDetectionPixelShader == NULL, false, "Cannot set pixel shader.");

	DirtyFlags.UnraiseFlags(ZERN_FDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_FDF_RENDER_STATE);

	return true;
}

bool ZERNFilterEdgeDetection::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZERN_FDF_RENDER_STATE))
		return true;

	if (!ZERNFilter::UpdateRenderStates())
		return false;

	ZEGRRenderState RenderState;
	RenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);

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

	DirtyFlags.UnraiseFlags(ZERN_FDF_RENDER_STATE);

	return true;
}

void ZERNFilterEdgeDetection::Apply(ZEGRContext* Context)
{
	ZERNFilter::Apply(Context);

	ZEGRTexture* DepthTexture;
	Context->GetTextures(ZEGR_ST_PIXEL, 4, 1, &DepthTexture);
	const ZEGRDepthStencilBuffer* DepthStencilBuffer = static_cast<ZEGRTexture*>(DepthTexture)->GetDepthStencilBuffer(true);

	Viewport.SetWidth(DepthStencilBuffer->GetWidth());
	Viewport.SetHeight(DepthStencilBuffer->GetHeight());

	Context->SetRenderState(EdgeDetectionGraphicsRenderStateData);
	Context->SetRenderTargets(0, NULL, DepthStencilBuffer);
	//Context->SetStencilRef(StencilRef);
	Context->Draw(3, 0);
}

ZERNFilterEdgeDetection::ZERNFilterEdgeDetection()
{

}

bool ZERNFilterColorTransform::InitializeInternal()
{
	if (!ZERNFilter::InitializeInternal())
		return false;

	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

	return Update();
}

bool ZERNFilterColorTransform::DeinitializeInternal()
{
	ColorTransformPixelShader.Release();
	ColorTransformGraphicsRenderStateData.Release();
	ConstantBuffer.Release();

	return ZERNFilter::DeinitializeInternal();
}

bool ZERNFilterColorTransform::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_FDF_SHADERS))
		return true;

	if (!ZERNFilter::UpdateShaders())
		return false;

	ZEGRShaderCompileOptions Options;
	Options.Model = ZEGR_SM_5_0;
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString::FromInt32(ZEGRGraphicsModule::SAMPLE_COUNT)));

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNFiltering.hlsl";

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFiltering_ColorTransform_PixelShader";
	ColorTransformPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(ColorTransformPixelShader == NULL, false, "Cannot set pixel shader.");

	DirtyFlags.UnraiseFlags(ZERN_FDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_FDF_RENDER_STATE);

	return true;
}

bool ZERNFilterColorTransform::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZERN_FDF_RENDER_STATE))
		return true;

	if (!ZERNFilter::UpdateRenderStates())
		return false;

	ZEGRRenderState RenderState;
	RenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, ColorTransformPixelShader);
	ColorTransformGraphicsRenderStateData = RenderState.Compile();
	zeCheckError(ColorTransformGraphicsRenderStateData == NULL, false, "Cannot set render state.");

	DirtyFlags.UnraiseFlags(ZERN_FDF_RENDER_STATE);

	return true;
}

bool ZERNFilterColorTransform::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZERN_FDF_CONSTANT_BUFFER))
		return true;

	if (!ZERNFilter::UpdateConstantBuffers())
		return false;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_FDF_CONSTANT_BUFFER);

	return true;
}

void ZERNFilterColorTransform::SetColorMatrix(const ZEMatrix4x4& ColorMatrix)
{
	if (Constants.ColorMatrix == ColorMatrix)
		return;

	Constants.ColorMatrix = ColorMatrix;

	DirtyFlags.RaiseFlags(ZERN_FDF_CONSTANT_BUFFER);
}

const ZEMatrix4x4& ZERNFilterColorTransform::GetColorMatrix() const
{
	return Constants.ColorMatrix;
}

void ZERNFilterColorTransform::Apply(ZEGRContext* Context)
{
	ZERNFilter::Apply(Context);

	if (!Update())
		return;

	Viewport.SetWidth((float)OutputTexture->GetWidth());
	Viewport.SetHeight((float)OutputTexture->GetHeight());

	const ZEGRRenderTarget* RenderTarget = OutputTexture->GetRenderTarget();

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, ConstantBuffer);
	Context->SetRenderState(ColorTransformGraphicsRenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, InputTexture);
	Context->SetViewports(1, &Viewport);
	Context->Draw(3, 0);
}

ZERNFilterColorTransform::ZERNFilterColorTransform()
{
	Constants.ColorMatrix = ZEMatrix4x4::Identity;
}

bool ZERNFilterNoise::InitializeInternal()
{
	if (!ZERNFilter::InitializeInternal())
		return false;

	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);
	NoiseTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, 1024, 1024, 1, ZEGR_TF_R8G8B8A8_UNORM, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_UNORDERED_ACCESS);

	return Update();
}

bool ZERNFilterNoise::DeinitializeInternal()
{
	GenerateNoiseComputeShader.Release();
	NoisePixelShader.Release();

	GenerateNoiseComputeRenderStateData.Release();
	NoiseGraphicsRenderStateData.Release();

	ConstantBuffer.Release();
	NoiseTexture.Release();

	return ZERNFilter::DeinitializeInternal();
}

bool ZERNFilterNoise::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_FDF_SHADERS))
		return true;

	if (!ZERNFilter::UpdateShaders())
		return false;

	ZEGRShaderCompileOptions Options;
	Options.Model = ZEGR_SM_5_0;
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString::FromInt32(ZEGRGraphicsModule::SAMPLE_COUNT)));

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNFiltering.hlsl";

	Options.Type = ZEGR_ST_COMPUTE;
	Options.EntryPoint = "ZERNFiltering_GenerateNoise_ComputeShader";
	GenerateNoiseComputeShader = ZEGRShader::Compile(Options);
	zeCheckError(GenerateNoiseComputeShader == NULL, false, "Cannot set compute shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFiltering_Noise_PixelShader";
	NoisePixelShader = ZEGRShader::Compile(Options);
	zeCheckError(NoisePixelShader == NULL, false, "Cannot set pixel shader.");

	DirtyFlags.UnraiseFlags(ZERN_FDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_FDF_RENDER_STATE);

	return true;
}

bool ZERNFilterNoise::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZERN_FDF_RENDER_STATE))
		return true;

	if (!ZERNFilter::UpdateRenderStates())
		return false;

	ZEGRComputeRenderState ComputeRenderState;
	ComputeRenderState.SetComputeShader(GenerateNoiseComputeShader);
	GenerateNoiseComputeRenderStateData = ComputeRenderState.Compile();
	zeCheckError(GenerateNoiseComputeRenderStateData == NULL, false, "Cannot set compute render state.");

	ZEGRRenderState GraphicsRenderState;
	GraphicsRenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);

	GraphicsRenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);
	GraphicsRenderState.SetShader(ZEGR_ST_PIXEL, NoisePixelShader);

	NoiseGraphicsRenderStateData = GraphicsRenderState.Compile();
	zeCheckError(NoiseGraphicsRenderStateData == NULL, false, "Cannot set graphics render state.");

	DirtyFlags.UnraiseFlags(ZERN_FDF_RENDER_STATE);

	return true;
}

bool ZERNFilterNoise::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZERN_FDF_CONSTANT_BUFFER))
		return true;

	if (!ZERNFilter::UpdateConstantBuffers())
		return false;

	if (InputTexture != NULL)
	{
		Constants.Size.x = InputTexture->GetWidth() / Constants.Granularity;
		Constants.Size.y = InputTexture->GetHeight() / Constants.Granularity;
	}

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_FDF_CONSTANT_BUFFER);

	return true;
}

void ZERNFilterNoise::SetInputTexture(const ZEGRTexture* InputTexture)
{
	if (this->InputTexture == NULL || (InputTexture != NULL && 
		this->InputTexture->GetWidth() != InputTexture->GetWidth() || 
		this->InputTexture->GetHeight() != InputTexture->GetHeight()))
		DirtyFlags.RaiseFlags(ZERN_FDF_CONSTANT_BUFFER);

	ZERNFilter::SetInputTexture(InputTexture);
}

void ZERNFilterNoise::SetWeight(float Weight)
{
	if (Constants.Weight == Weight)
		return;

	Constants.Weight = Weight;

	DirtyFlags.RaiseFlags(ZERN_FDF_CONSTANT_BUFFER);
}

float ZERNFilterNoise::GetWeight() const
{
	return Constants.Weight;
}

void ZERNFilterNoise::SetGranularity(ZEUInt Granularity)
{
	if (Constants.Granularity == Granularity)
		return;

	Constants.Granularity = Granularity;

	DirtyFlags.RaiseFlags(ZERN_FDF_CONSTANT_BUFFER);
}

ZEUInt ZERNFilterNoise::GetGranularity() const
{
	return Constants.Granularity;
}

void ZERNFilterNoise::Apply(ZEGRContext* Context)
{
	ZERNFilter::Apply(Context);

	if (!Update())
		return;

	//Context->SetRWTexture(0, NoiseTexture);
	//Context->SetComputeRenderState(GenerateNoiseComputeRenderStateData);
	//Context->Dispatch(32, 32, 1);

	Viewport.SetWidth((float)OutputTexture->GetWidth());
	Viewport.SetHeight((float)OutputTexture->GetHeight());

	const ZEGRRenderTarget* RenderTarget = OutputTexture->GetRenderTarget();

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, ConstantBuffer);
	Context->SetRenderState(NoiseGraphicsRenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, InputTexture);
	//Context->SetTexture(ZEGR_ST_PIXEL, 6, NoiseTexture);
	Context->SetViewports(1, &Viewport);
	Context->Draw(3, 0);
}

ZERNFilterNoise::ZERNFilterNoise()
{
	Constants.Weight = 0.5f;
	Constants.Granularity = 1;
	Constants.Size = ZEVector2(1024.0f, 1024.0f);
}
