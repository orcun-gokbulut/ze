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

#include "ZEError.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"

#include "ZERNStageID.h"
#include "ZERNRenderer.h"
#include "ZEGraphics/ZEGROutput.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRShaderCompiler.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"

#define ZERN_HSDF_CONSTANT_BUFFER	1
#define ZERN_HSDF_SHADERS			2
#define ZERN_HSDF_RENDER_STATE		4
#define ZERN_HSDF_RESIZE			8

static ZEUInt ConvertBlurTextureSize(ZERNHDRBlurTextureSize BlurTextureSize)
{
	switch (BlurTextureSize)
	{
		default:
		case ZERN_HBTS_EXACT:
			return 0;

		case ZERN_HBTS_HALF:
			return 1;

		case ZERN_HBTS_QUARTER:
			return 2;
	}
}

bool ZERNStageHDR::UpdateRenderStates()
{
	if(!DirtyFlags.GetFlags(ZERN_HSDF_RENDER_STATE))
		return true;
	
	ZEGRRenderState RenderState;

	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetRenderTargetFormat(0, ZEGR_TF_R8G8B8A8_UNORM_SRGB);
	RenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShaderPositionTexcoord);

	ZEGRDepthStencilState DepthStencilStateNoTestWrite;
	DepthStencilStateNoTestWrite.SetDepthTestEnable(false);
	DepthStencilStateNoTestWrite.SetDepthWriteEnable(false);

	RenderState.SetDepthStencilState(DepthStencilStateNoTestWrite);

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
	ScreenCoverVertexShaderPositionTexcoord = ZEGRShader::Compile(Options);

	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString(ZEGRGraphicsModule::SAMPLE_COUNT)));
	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNHDR_Luminance_Calculate_PixelShader";
	CalculateLuminance_PixelShader = ZEGRShader::Compile(Options);

	Options.Definitions.Clear();
	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNHDR_Luminance_CalculateAdapted_PixelShader";
	CalculateAdaptedLuminance_PixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNHDR_Luminance_DownScale_PixelShader";
	DownSampling_PixelShader = ZEGRShader::Compile(Options);

	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString(ZEGRGraphicsModule::SAMPLE_COUNT)));
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

bool ZERNStageHDR::UpdateInputOutput()
{
	InputTexture = GetPrevOutput(ZERN_SO_COLOR);
	if (InputTexture == NULL)
		return false;

	ZEUInt Width = InputTexture->GetWidth();
	ZEUInt Height = InputTexture->GetHeight();

	if (Viewport.GetWidth() != Width || Viewport.GetHeight() != Height)
		DirtyFlags.RaiseFlags(ZERN_HSDF_RESIZE);

	OutputRenderTarget = GetNextProvidedInput(ZERN_SO_COLOR);
	if (OutputRenderTarget == NULL)
	{
		// No Provided Output - Create Own Buffer
		if (OutputTexture == NULL || 
			OutputTexture->GetWidth() != Width || OutputTexture->GetHeight() != Height)
			OutputTexture = ZEGRTexture2D::CreateInstance(Width, Height, 1, ZEGR_TF_R8G8B8A8_UNORM_SRGB);

		OutputRenderTarget = OutputTexture->GetRenderTarget();
	}
	else
	{
		// Output is provided - Release own output texture and use provided one.
		OutputTexture.Release();
	}

	return true;
}

bool ZERNStageHDR::UpdateTextures()
{
	if (!DirtyFlags.GetFlags(ZERN_HSDF_RESIZE))
		return true;

	ZEUInt CurrentWidth = OutputRenderTarget->GetWidth();
	ZEUInt CurrentHeight = OutputRenderTarget->GetHeight();

	ZEInt ExponentWidth = (ZEInt)(ZEMath::Log10((float)CurrentWidth) / ZEMath::Log10(2.0f));
	ZEInt ExponentHeight = (ZEInt)(ZEMath::Log10((float)CurrentHeight) / ZEMath::Log10(2.0f));

	ZEInt Count = ZEMath::Min(ExponentWidth, ExponentHeight);
	ZEInt ComputedCount = (Count + 3) / 2;

	LuminanceMips.Resize(ComputedCount);

	ZEUInt Index = 0;
	for(ZEInt I = Count; I > 0; I -= 2)
	{
		ZEUInt Size = (ZEUInt)ZEMath::Power(2.0f, (float)I);
		LuminanceMips[Index++] = ZEGRTexture2D::CreateInstance(Size, Size, 1, ZEGR_TF_R16_FLOAT);
	}

	LuminanceMips[Index] = ZEGRTexture2D::CreateInstance(1, 1, 1, ZEGR_TF_R16_FLOAT);
	CurrentAdaptedLuminance = ZEGRTexture2D::CreateInstance(1, 1, 1, ZEGR_TF_R16_FLOAT);
	PreviousAdaptedLuminance = ZEGRTexture2D::CreateInstance(1, 1, 1, ZEGR_TF_R16_FLOAT);

	ZEUInt16 One = 1;
	PreviousAdaptedLuminance->UpdateSubResource(0, 0, NULL, &One, 1);

	CurrentWidth >>= ConvertBlurTextureSize(BlurTextureSize);
	CurrentHeight >>= ConvertBlurTextureSize(BlurTextureSize);

	BrightTexture = ZEGRTexture2D::CreateInstance(CurrentWidth, CurrentHeight, 1, ZEGR_TF_R11G11B10_FLOAT);
	BlurTextureTemp1 = ZEGRTexture2D::CreateInstance(CurrentWidth, CurrentHeight, 1, ZEGR_TF_R11G11B10_FLOAT);
	BlurTextureTemp2 = ZEGRTexture2D::CreateInstance(CurrentWidth, CurrentHeight, 1, ZEGR_TF_R11G11B10_FLOAT);
	BlurTextureFinal = ZEGRTexture2D::CreateInstance(CurrentWidth, CurrentHeight, 1, ZEGR_TF_R11G11B10_FLOAT);

	DirtyFlags.UnraiseFlags(ZERN_HSDF_RESIZE);

	return true;
}

bool ZERNStageHDR::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateConstantBuffer())
		return false;

	if (!UpdateInputOutput())
		return false;

	if (!UpdateTextures())
		return false;

	return true;
}

void ZERNStageHDR::CalculateLuminance(ZEGRContext* Context, const ZEGRTexture2D* Input, const ZEGRRenderTarget* Output)
{
	Viewport.SetWidth((float)Output->GetWidth());
	Viewport.SetHeight((float)Output->GetHeight());

	Context->SetRenderState(CalculateLuminance_RenderState);
	Context->SetRenderTargets(1, &Output, NULL);
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 1, reinterpret_cast<const ZEGRTexture**>(&Input));
	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);
}

void ZERNStageHDR::DownSample(ZEGRContext* Context, const ZEGRTexture2D* Input, const ZEGRRenderTarget* Output)
{
	Viewport.SetWidth((float)Output->GetWidth());
	Viewport.SetHeight((float)Output->GetHeight());

	Context->SetRenderState(DownSampling_RenderState);
	Context->SetRenderTargets(1, &Output, NULL);
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 1, reinterpret_cast<const ZEGRTexture**>(&Input));
	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);
}

void ZERNStageHDR::GenerateMipMaps(ZEGRContext* Context)
{
	ZESize Count = LuminanceMips.GetCount() - 1;
	for(ZESize I = 0; I < Count; ++I)
	{
		DownSample(Context, LuminanceMips[I], LuminanceMips[I + 1]->GetRenderTarget());
	}
}

void ZERNStageHDR::CalculateAdaptedLuminance(ZEGRContext* Context)
{
	Viewport.SetWidth(1.0f);
	Viewport.SetHeight(1.0f);

	const ZEGRRenderTarget* RenderTarget = CurrentAdaptedLuminance->GetRenderTarget();

	Context->SetRenderState(CalculateAdaptedLuminance_RenderState);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	const ZEGRTexture* Textures[] = {LuminanceMips.GetLastItem(), PreviousAdaptedLuminance};
	Context->SetTextures(ZEGR_ST_PIXEL, 7, 2, Textures);
	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);
}

void ZERNStageHDR::CalculateBrightness(ZEGRContext* Context, const ZEGRTexture2D* Input, const ZEGRRenderTarget* Output)
{
	Viewport.SetWidth((float)Output->GetWidth());
	Viewport.SetHeight((float)Output->GetHeight());

	Context->SetConstantBuffers(ZEGR_ST_PIXEL, 8, 1, ConstantBuffer.GetPointerToPointer());
	Context->SetRenderState(CalculateBrightness_RenderState);
	Context->SetRenderTargets(1, &Output, NULL);
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 1, reinterpret_cast<const ZEGRTexture**>(&Input));
	Context->SetTextures(ZEGR_ST_PIXEL, 7, 1, reinterpret_cast<ZEGRTexture**>(&CurrentAdaptedLuminance));
	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);
}

void ZERNStageHDR::ApplyBlur(ZEGRContext* Context, const ZEGRTexture2D* Input, const ZEGRRenderTarget* Output)
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

void ZERNStageHDR::ToneMapping(ZEGRContext* Context, const ZEGRTexture2D* Input, const ZEGRRenderTarget* Output)
{
	Viewport.SetWidth((float)Output->GetWidth());
	Viewport.SetHeight((float)Output->GetHeight());

	Context->SetConstantBuffers(ZEGR_ST_PIXEL, 8, 1, ConstantBuffer.GetPointerToPointer());
	Context->SetRenderState(ToneMapping_RenderState);
	Context->SetRenderTargets(1, &Output, NULL);
	const ZEGRTexture* Textures[] = {Input, BlurTextureFinal, CurrentAdaptedLuminance};
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 3, Textures);
	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);
}

bool ZERNStageHDR::InitializeSelf()
{
	if (!ZERNStage::InitializeSelf())
		return false;
	
	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));
	Filter.Initialize();
	ZERNFilter::GenerateGaussianKernel(HorizontalValues, 11, 2.0f);
	ZERNFilter::GenerateGaussianKernel(VerticalValues, 11, 2.0f, false);

	return Update();
}

void ZERNStageHDR::DeinitializeSelf()
{
	OutputTexture.Release();

	CurrentAdaptedLuminance.Release();
	PreviousAdaptedLuminance.Release();

	ScreenCoverVertexShaderPositionTexcoord.Release();
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

	Filter.Deinitialize();

	HorizontalValues.Clear();
	VerticalValues.Clear();

	BlurTextureTemp1.Release();
	BlurTextureTemp2.Release();
	BlurTextureFinal.Release();
	BrightTexture.Release();
	LuminanceMips.Clear();
	CurrentAdaptedLuminance.Release();
	PreviousAdaptedLuminance.Release();

	ConstantBuffer.Release();

	InputTexture = NULL;
	OutputRenderTarget = NULL;

	ZERNStage::DeinitializeSelf();
}

ZEInt ZERNStageHDR::GetId() const
{
	return ZERN_STAGE_HDR;
}

const ZEString& ZERNStageHDR::GetName() const
{
	static const ZEString Name = "HDR Stage";
	return Name;
}

void ZERNStageHDR::SetKey(float Key)
{
	if (Constants.Key == Key)
		return;

	Constants.Key = Key;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetKey() const
{
	return Constants.Key;
}

void ZERNStageHDR::SetAutoKey(bool AutoKeyEnabled)
{
	if ((bool)Constants.AutoKeyEnabled == AutoKeyEnabled)
		return;

	Constants.AutoKeyEnabled = static_cast<ZEBool32>(AutoKeyEnabled);

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

bool ZERNStageHDR::GetAutoKey() const
{
	return Constants.AutoKeyEnabled != ZEGR_FALSE;
}

void ZERNStageHDR::SetBloomEnabled(bool BloomEnabled)
{
	if ((bool)Constants.BloomEnabled == BloomEnabled)
		return;

	Constants.BloomEnabled = static_cast<ZEBool32>(BloomEnabled);

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

bool ZERNStageHDR::GetBloomEnabled() const
{
	return Constants.BloomEnabled != ZEGR_FALSE;
}

void ZERNStageHDR::SetBloomFactor(float BloomFactor)
{
	if (Constants.BloomFactor == BloomFactor)
		return;

	Constants.BloomFactor = BloomFactor;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetBloomFactor() const
{
	return Constants.BloomFactor;
}

void ZERNStageHDR::SetBloomThreshold(float BloomThreshold)
{
	if (Constants.BloomThreshold == BloomThreshold)
		return;

	Constants.BloomThreshold = ZEMath::Clamp(BloomThreshold, 1.0f, 10.0f);

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetBloomThreshold() const
{
	return Constants.BloomThreshold;
}

void ZERNStageHDR::SetLuminanceMin(float LuminanceMin)
{
	if (Constants.LuminanceMin == LuminanceMin)
		return;

	Constants.LuminanceMin = LuminanceMin;
	
	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetLuminanceMin() const
{
	return Constants.LuminanceMin;
}

void ZERNStageHDR::SetLuminanceMax(float LuminanceMax)
{
	if (Constants.LuminanceMax == LuminanceMax)
		return;

	Constants.LuminanceMax = LuminanceMax;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetLuminanceMax() const
{
	return Constants.LuminanceMax;
}

void ZERNStageHDR::SetSaturation(float Saturation)
{
	if (Constants.Saturation == Saturation)
		return;

	Constants.Saturation = Saturation;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetSaturation() const
{
	return Constants.Saturation;
}

void ZERNStageHDR::SetWhiteLevel(float WhiteLevel)
{
	if (Constants.WhiteLevel == WhiteLevel)
		return;

	Constants.WhiteLevel = WhiteLevel;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetWhiteLevel() const
{
	return Constants.WhiteLevel;
}

void ZERNStageHDR::SetToneMapOperator(ZERNHDRToneMapOperator ToneMapOperator)
{
	if (Constants.ToneMapOperator == ToneMapOperator)
		return;

	Constants.ToneMapOperator = ToneMapOperator;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

ZERNHDRToneMapOperator ZERNStageHDR::GetToneMapOperator() const
{
	return (ZERNHDRToneMapOperator)Constants.ToneMapOperator;
}

void ZERNStageHDR::SetBlurTextureSize(ZERNHDRBlurTextureSize BlurTextureSize)
{
	this->BlurTextureSize = BlurTextureSize;
}

ZERNHDRBlurTextureSize ZERNStageHDR::GetBlurTextureSize() const
{
	return BlurTextureSize;
}

const ZEGRRenderTarget* ZERNStageHDR::GetProvidedInput(ZERNStageBuffer Input) const
{
	if (GetEnabled() && (Input == ZERN_SO_COLOR))
		return NULL;

	return ZERNStage::GetProvidedInput(Input);
}

const ZEGRTexture2D* ZERNStageHDR::GetOutput(ZERNStageBuffer Output) const
{
	if (GetEnabled() && (Output == ZERN_SO_COLOR || Output == ZERN_SO_HDR))
		return OutputTexture;

	return ZERNStage::GetOutput(Output);
}

bool ZERNStageHDR::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!Update())
		return false;

	CalculateLuminance(Context, InputTexture, LuminanceMips[0]->GetRenderTarget());
	GenerateMipMaps(Context);
	CalculateAdaptedLuminance(Context);

	if (Constants.BloomEnabled)
	{
		CalculateBrightness(Context, InputTexture, BrightTexture->GetRenderTarget());
		ApplyBlur(Context, BrightTexture, BlurTextureFinal->GetRenderTarget());
	}

	ToneMapping(Context, InputTexture, OutputRenderTarget);

	ZEHolder<ZEGRTexture2D> Temporary = PreviousAdaptedLuminance;
	PreviousAdaptedLuminance = CurrentAdaptedLuminance;
	CurrentAdaptedLuminance = Temporary;

	return true;
}

void ZERNStageHDR::CleanUp(ZEGRContext* Context)
{
	ZERNStage::CleanUp(Context);
}

ZERNStageHDR::ZERNStageHDR()
{
	DirtyFlags.RaiseAll();

	Constants.AutoKeyEnabled = ZEGR_FALSE;
	Constants.BloomEnabled = ZEGR_FALSE;
	Constants.Key = 0.18f;
	Constants.WhiteLevel = 1.0f;
	Constants.ToneMapOperator = ZERN_HTMO_UNCHARTED;
	Constants.BloomFactor = 1.0f;
	Constants.BloomThreshold = 2.0f;
	Constants.LuminanceMin = 0.1f;
	Constants.LuminanceMax = 10.0f;
	Constants.Saturation = 0.7f;

	BlurTextureSize = ZERN_HBTS_HALF;

	InputTexture = NULL;
	OutputRenderTarget = NULL;
}