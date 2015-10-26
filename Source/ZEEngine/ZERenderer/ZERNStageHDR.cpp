//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageHDR.cpp
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

#include "ZERNStageHDR.h"

#include "ZERNStageID.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRShaderCompiler.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZERNRenderer.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEError.h"
#include "ZEGraphics/ZEGROutput.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZERNStageGBuffer.h"

#define ZERN_HSDF_CONSTANT_BUFFER	1
#define ZERN_HSDF_SHADERS			2
#define ZERN_HSDF_RENDER_STATE		4
#define ZERN_HSDF_RESIZE			8

bool ZERNStageHDR::UpdateRenderStates()
{
	if(!DirtyFlags.GetFlags(ZERN_HSDF_RENDER_STATE))
		return true;
	
	ZEGRRenderState RenderState;

	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetRenderTargetFormat(0, ZEGR_TF_R11FG11FB10F_FLOAT);
	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);

	ZEGRDepthStencilState DepthStencilState;
	DepthStencilState.SetDepthTestEnable(false);
	DepthStencilState.SetDepthWriteEnable(false);
	DepthStencilState.SetStencilTestEnable(false);

	RenderState.SetDepthStencilState(DepthStencilState);

	RenderState.SetShader(ZEGR_ST_PIXEL, CalculateLuminance_PixelShader);
	CalculateLuminance_RenderState = RenderState.Compile();
	zeCheckError(CalculateLuminance_RenderState == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, CalculateAdaptedLuminance_PixelShader);
	CalculateAdaptedLuminance_RenderState = RenderState.Compile();
	zeCheckError(CalculateAdaptedLuminance_RenderState == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, DownSampling_PixelShader);
	DownSampling_RenderState = RenderState.Compile();
	zeCheckError(DownSampling_RenderState == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, CalculateBrightness_PixelShader);
	CalculateBrightness_RenderState = RenderState.Compile();
	zeCheckError(CalculateBrightness_RenderState == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, ToneMapping_PixelShader);
	ToneMapping_RenderState = RenderState.Compile();
	zeCheckError(ToneMapping_RenderState == NULL, false, "Cannot set render state.");

	DirtyFlags.UnraiseFlags(ZERN_HSDF_RENDER_STATE);

	return true;
}

bool ZERNStageHDR::UpdateShaders()
{
	if(!DirtyFlags.GetFlags(ZERN_HSDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNHDR.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_PositionTexcoords";
	VertexShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNHDR_Luminance_Calculate_PixelShader";
	CalculateLuminance_PixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNHDR_Luminance_CalculateAdapted_PixelShader";
	CalculateAdaptedLuminance_PixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNHDR_Luminance_DownScale_PixelShader";
	DownSampling_PixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNHDR_Bright_Calculate_PixelShader";
	CalculateBrightness_PixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNHDR_ToneMapping_PixelShader";
	ToneMapping_PixelShader = ZEGRShader::Compile(Options);

	DirtyFlags.UnraiseFlags(ZERN_HSDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_HSDF_RENDER_STATE);

	return true;
}

bool ZERNStageHDR::UpdateConstantBuffer()
{
	if(!DirtyFlags.GetFlags(ZERN_HSDF_CONSTANT_BUFFER))
		return true;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_HSDF_CONSTANT_BUFFER);

	return true;
}

bool ZERNStageHDR::UpdateTextures()
{
	if(!DirtyFlags.GetFlags(ZERN_HSDF_RESIZE))
		return true;

	ZEUInt CurrentWidth = OutputRenderTarget->GetWidth();
	ZEUInt CurrentHeight = OutputRenderTarget->GetHeight();

	ZEInt ExponentWidth = ZEMath::Log10(CurrentWidth) / ZEMath::Log10(2);
	ZEInt ExponentHeight = ZEMath::Log10(CurrentHeight) / ZEMath::Log10(2);

	ZEInt Count = ZEMath::Min(ExponentWidth, ExponentHeight);
	ZEInt ComputedCount = (Count + 3) / 2;

	LuminanceMips.Resize(ComputedCount);

	ZEUInt Index = 0;
	for(ZEInt I = Count; I > 0; I -= 2)
	{
		ZEUInt Size = ZEMath::Power(2, I);
		LuminanceMips[Index].Release();
		LuminanceMips[Index++] = ZEGRTexture2D::CreateInstance(Size, Size, 1, ZEGR_TF_R16_FLOAT, true);
	}

	LuminanceMips[Index].Release();
	CurrentAdaptedLuminance.Release();
	PreviousAdaptedLuminance.Release();

	LuminanceMips[Index] = ZEGRTexture2D::CreateInstance(1, 1, 1, ZEGR_TF_R16_FLOAT, true);
	CurrentAdaptedLuminance = ZEGRTexture2D::CreateInstance(1, 1, 1, ZEGR_TF_R16_FLOAT, true);
	PreviousAdaptedLuminance = ZEGRTexture2D::CreateInstance(1, 1, 1, ZEGR_TF_R16_FLOAT, true);

	BrightTexture.Release();
	BlurTextureTemp1.Release();
	BlurTextureTemp2.Release();
	BlurTextureFinal.Release();

	CurrentWidth >>= (ZEUInt)BlurTextureSize;
	CurrentHeight >>= (ZEUInt)BlurTextureSize;

	BrightTexture = ZEGRTexture2D::CreateInstance(CurrentWidth, CurrentHeight, 1, ZEGR_TF_R11FG11FB10F_FLOAT, true);
	BlurTextureTemp1 = ZEGRTexture2D::CreateInstance(CurrentWidth, CurrentHeight, 1, ZEGR_TF_R11FG11FB10F_FLOAT, true);
	BlurTextureTemp2 = ZEGRTexture2D::CreateInstance(CurrentWidth, CurrentHeight, 1, ZEGR_TF_R11FG11FB10F_FLOAT, true);
	BlurTextureFinal = ZEGRTexture2D::CreateInstance(CurrentWidth, CurrentHeight, 1, ZEGR_TF_R11FG11FB10F_FLOAT, true);

	DirtyFlags.UnraiseFlags(ZERN_HSDF_RESIZE);
}

bool ZERNStageHDR::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if(!UpdateTextures())
		return false;

	if (!UpdateConstantBuffer())
		return false;

	return true;
}

bool ZERNStageHDR::InitializeSelf()
{
	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(HDRConstants));
	DirtyFlags.RaiseAll();

	Filter.Initialize();

	ZERNFilter::GenerateGaussianKernel(HorizontalValues, 11, 2.0f);
	ZERNFilter::GenerateGaussianKernel(VerticalValues, 11, 2.0f, false);

	return true;
}

void ZERNStageHDR::DeinitializeSelf()
{
	CurrentAdaptedLuminance.Release();
	PreviousAdaptedLuminance.Release();

	VertexShader.Release();
	CalculateLuminance_PixelShader.Release();
	CalculateAdaptedLuminance_PixelShader.Release();
	DownSampling_PixelShader.Release();
	CalculateBrightness_PixelShader.Release();
	ToneMapping_PixelShader.Release();
	
	CalculateLuminance_RenderState.Release();
	CalculateAdaptedLuminance_RenderState.Release();
	DownSampling_RenderState.Release();
	CalculateBrightness_RenderState.Release();
	ToneMapping_RenderState.Release();

	HorizontalValues.Clear();
	VerticalValues.Clear();
	LuminanceMips.Clear();

	BlurTextureTemp1.Release();
	BlurTextureTemp2.Release();
	BlurTextureFinal.Release();
	BrightTexture.Release();
	CurrentAdaptedLuminance.Release();
	PreviousAdaptedLuminance.Release();
	ConstantBuffer.Release();

	Filter.Deinitialize();
}

ZEInt ZERNStageHDR::GetId()
{
	return ZERN_STAGE_HDR;
}

const ZEString& ZERNStageHDR::GetName()
{
	return "HDR Stage";
}

void ZERNStageHDR::SetKey(float Value)
{
	if (Constants.Key == Value)
		return;

	Constants.Key = Value;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetKey() const
{
	return Constants.Key;
}

void ZERNStageHDR::SetAutoKey(bool Enabled)
{
	if (Constants.AutoKey == Enabled)
		return;

	Constants.AutoKey = Enabled;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

bool ZERNStageHDR::GetAutoKey() const
{
	return Constants.AutoKey;
}

void ZERNStageHDR::SetBloomEnabled(bool Enabled)
{
	if (Constants.BloomEnabled == Enabled)
		return;

	Constants.BloomEnabled = Enabled;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

bool ZERNStageHDR::GetBloomEnabled() const
{
	return Constants.BloomEnabled;
}

void ZERNStageHDR::SetBloomFactor(float Value)
{
	if (Constants.BloomFactor == Value)
		return;

	Constants.BloomFactor = Value;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetBloomFactor() const
{
	return Constants.BloomFactor;
}

void ZERNStageHDR::SetBloomThreshold(float Value)
{
	if (Constants.BloomThreshold == Value)
		return;

	Constants.BloomThreshold = ZEMath::Clamp(Value, 0.0f, 10.0f);

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetBloomThreshold() const
{
	return Constants.BloomThreshold;
}

void ZERNStageHDR::SetWhiteLevel(float Value)
{
	if (Constants.WhiteLevel == Value)
		return;

	Constants.WhiteLevel = Value;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetWhiteLevel() const
{
	return Constants.WhiteLevel;
}

void ZERNStageHDR::SetToneMapOperator(ZERNHDRToneMapOperator Operator)
{
	if (Constants.ToneMapOperator == Operator)
		return;

	Constants.ToneMapOperator = Operator;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

ZERNHDRToneMapOperator ZERNStageHDR::GetToneMapOperator() const
{
	return (ZERNHDRToneMapOperator)Constants.ToneMapOperator;
}

ZERNStageHDR::ZERNStageHDR()
{
	DirtyFlags.RaiseAll();

	Constants.AutoKey = true;
	Constants.BloomEnabled = true;
	Constants.Key = 0.35f;
	Constants.WhiteLevel = 5.0f;
	Constants.ToneMapOperator = ZERN_HTMO_REINHARD;
	Constants.BloomFactor = 0.9f;
	Constants.BloomThreshold = 3.5f;

	PrevWidth = 0;
	PrevHeight = 0;

	BlurTextureSize = ZERN_HBTS_EXACT;
}

bool ZERNStageHDR::SetupInputOutput(ZERNRenderer* Renderer)
{
	ZERNStageGBuffer* Gbuffer = (ZERNStageGBuffer*)Renderer->GetStage(ZERN_STAGE_GBUFFER);
	ZEGROutput* Output = Renderer->GetOutput();
	if(Gbuffer == NULL || Output == NULL)
		return false;

	InputTexture = Gbuffer->GetAccumulationMap();
	OutputRenderTarget = Output->GetRenderTarget();

	return true;
}

bool ZERNStageHDR::Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands)
{
	if(!SetupInputOutput(Renderer))
		return false;

	ZEUInt CurrentWidth = OutputRenderTarget->GetWidth();
	ZEUInt CurrentHeight = OutputRenderTarget->GetHeight();

	if(PrevWidth != CurrentWidth || PrevHeight != CurrentHeight)
	{
		DirtyFlags.RaiseFlags(ZERN_HSDF_RESIZE);
		PrevWidth = CurrentWidth;
		PrevHeight = CurrentHeight;
	}

	if (!Update())
		return false;

	CalculateLuminance(Context, InputTexture, LuminanceMips[0]->GetRenderTarget());
	GenerateMipMaps(Context);
	CalculateAdaptedLuminance(Context);
	CalculateBrightness(Context, InputTexture, BrightTexture->GetRenderTarget());
	ApplyBlur(Context, BrightTexture, BlurTextureFinal->GetRenderTarget());
	ToneMapping(Context, InputTexture, OutputRenderTarget);

	ZEGRHolder<ZEGRTexture2D> Temporary = PreviousAdaptedLuminance;
	PreviousAdaptedLuminance = CurrentAdaptedLuminance;
	CurrentAdaptedLuminance = Temporary;

	return true;
}

void ZERNStageHDR::CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	Context->SetRenderTargets(0, NULL, NULL);
}

void ZERNStageHDR::SetInput(ZEGRTexture2D* Input)
{
	InputTexture = Input;
}

ZEGRTexture2D* ZERNStageHDR::GetInput() const
{
	return InputTexture;
}

void ZERNStageHDR::SetOutput(ZEGRRenderTarget* Output)
{
	OutputRenderTarget = Output;
}

ZEGRRenderTarget* ZERNStageHDR::GetOutput() const
{
	return OutputRenderTarget;
}

void ZERNStageHDR::SetBlurTextureSize(ZERNHDRBlurTextureSize Value)
{
	BlurTextureSize = Value;
}

ZERNHDRBlurTextureSize ZERNStageHDR::GetBlurTextureSize() const
{
	return BlurTextureSize;
}

void ZERNStageHDR::CalculateLuminance(ZEGRContext* Context, ZEGRTexture2D* Input, ZEGRRenderTarget* Output)
{
	ZEGRViewport Viewport;
	Viewport.SetWidth(Output->GetWidth());
	Viewport.SetHeight(Output->GetHeight());
	Viewport.SetX(0.0f);
	Viewport.SetY(0.0f);
	Viewport.SetMinDepth(0.0f);
	Viewport.SetMaxDepth(1.0f);

	Context->SetViewports(1, &Viewport);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, Input);
	Context->SetRenderTargets(1, &Output, NULL);
	Context->SetRenderState(CalculateLuminance_RenderState);
	Context->SetVertexBuffer(0, NULL);

	Context->Draw(3, 0);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetRenderTargets(0, NULL, NULL);
}

void ZERNStageHDR::CalculateAdaptedLuminance(ZEGRContext* Context)
{
	ZEGRViewport Viewport;
	Viewport.SetWidth(1);
	Viewport.SetHeight(1);
	Viewport.SetX(0.0f);
	Viewport.SetY(0.0f);
	Viewport.SetMinDepth(0.0f);
	Viewport.SetMaxDepth(1.0f);
	
	ZEGRRenderTarget* RenderTarget = CurrentAdaptedLuminance->GetRenderTarget();

	Context->SetViewports(1, &Viewport);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, LuminanceMips.GetLastItem());
	Context->SetTexture(ZEGR_ST_PIXEL, 3, PreviousAdaptedLuminance);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetRenderState(CalculateAdaptedLuminance_RenderState);
	Context->SetVertexBuffer(0, NULL);

	Context->Draw(3, 0);

	Context->SetTexture(ZEGR_ST_PIXEL, 2, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 3, NULL);
	Context->SetRenderTargets(0, NULL, NULL);
}

void ZERNStageHDR::DownSample(ZEGRContext* Context, ZEGRTexture2D* Input, ZEGRRenderTarget* Output)
{
	ZEGRViewport Viewport;
	Viewport.SetWidth(Output->GetWidth());
	Viewport.SetHeight(Output->GetHeight());
	Viewport.SetX(0.0f);
	Viewport.SetY(0.0f);
	Viewport.SetMinDepth(0.0f);
	Viewport.SetMaxDepth(1.0f);

	Context->SetViewports(1, &Viewport);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, Input);
	Context->SetRenderTargets(1, &Output, NULL);
	Context->SetRenderState(DownSampling_RenderState);
	Context->SetVertexBuffer(0, NULL);

	Context->Draw(3, 0);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetRenderTargets(0, NULL, NULL);
}

void ZERNStageHDR::GenerateMipMaps(ZEGRContext* Context)
{
	ZESize Count = LuminanceMips.GetCount() - 1;
	for(ZESize I = 0; I < Count; ++I)
	{
		DownSample(Context, LuminanceMips[I], LuminanceMips[I + 1]->GetRenderTarget());
	}
}

void ZERNStageHDR::CalculateBrightness(ZEGRContext* Context, ZEGRTexture2D* Input, ZEGRRenderTarget* Output)
{
	ZEGRViewport Viewport;
	Viewport.SetWidth(Output->GetWidth());
	Viewport.SetHeight(Output->GetHeight());
	Viewport.SetX(0.0f);
	Viewport.SetY(0.0f);
	Viewport.SetMinDepth(0.0f);
	Viewport.SetMaxDepth(1.0f);

	Context->SetViewports(1, &Viewport);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, ConstantBuffer);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, Input);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, CurrentAdaptedLuminance);
	Context->SetRenderTargets(1, &Output, NULL);
	Context->SetRenderState(CalculateBrightness_RenderState);
	Context->SetVertexBuffer(0, NULL);

	Context->Draw(3, 0);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);
	Context->SetRenderTargets(0, NULL, NULL);
}

void ZERNStageHDR::ApplyBlur(ZEGRContext* Context, ZEGRTexture2D* Input, ZEGRRenderTarget* Output)
{
	Filter.SetInput(Input);
	Filter.SetOutput(BlurTextureTemp1->GetRenderTarget());
	Filter.SetKernelValues(&HorizontalValues[0], HorizontalValues.GetCount());
	Filter.Process(Context);

	Filter.SetInput(BlurTextureTemp1);
	Filter.SetOutput(BlurTextureTemp2->GetRenderTarget());
	Filter.SetKernelValues(&VerticalValues[0], VerticalValues.GetCount());
	Filter.Process(Context);

	Filter.SetInput(BlurTextureTemp2);
	Filter.SetOutput(BlurTextureTemp1->GetRenderTarget());
	Filter.SetKernelValues(&HorizontalValues[0], HorizontalValues.GetCount());
	Filter.Process(Context);

	Filter.SetInput(BlurTextureTemp1);
	Filter.SetOutput(BlurTextureTemp2->GetRenderTarget());
	Filter.SetKernelValues(&VerticalValues[0], VerticalValues.GetCount());
	Filter.Process(Context);

	Filter.SetInput(BlurTextureTemp2);
	Filter.SetOutput(BlurTextureTemp1->GetRenderTarget());
	Filter.SetKernelValues(&HorizontalValues[0], HorizontalValues.GetCount());
	Filter.Process(Context);

	Filter.SetInput(BlurTextureTemp1);
	Filter.SetOutput(Output);
	Filter.SetKernelValues(&VerticalValues[0], VerticalValues.GetCount());
	Filter.Process(Context);
}

void ZERNStageHDR::ToneMapping(ZEGRContext* Context, ZEGRTexture2D* Input, ZEGRRenderTarget* Output)
{
	ZEGRViewport Viewport;
	Viewport.SetWidth(Output->GetWidth());
	Viewport.SetHeight(Output->GetHeight());
	Viewport.SetX(0.0f);
	Viewport.SetY(0.0f);
	Viewport.SetMinDepth(0.0f);
	Viewport.SetMaxDepth(1.0f);

	Context->SetViewports(1, &Viewport);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, ConstantBuffer);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, Input);
	Context->SetTexture(ZEGR_ST_PIXEL, 1, BlurTextureFinal);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, CurrentAdaptedLuminance);
	Context->SetRenderTargets(1, &Output, NULL);
	Context->SetRenderState(ToneMapping_RenderState);
	Context->SetVertexBuffer(0, NULL);

	Context->Draw(3, 0);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 1, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);
	Context->SetRenderTargets(0, NULL, NULL);
}
