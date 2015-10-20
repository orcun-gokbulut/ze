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
#include "ZEPointer/ZEPointer.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZERNRenderer.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZECanvas.h"
#include "ZEError.h"
#include "ZEGraphics\ZEGROutput.h"
#include "ZEGraphics\ZEGRRenderTarget.h"

#define GAUSSIAN_FILTER_WIDTH		21	//9 13 17 21 25 etc

#define COLOR_RED	D3DCOLORWRITEENABLE_RED
#define COLOR_ALL	D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA

#define ZERN_HSDF_CONSTANT_BUFFER	1
#define ZERN_HSDF_SHADERS			2
#define ZERN_HSDF_RENDER_STATE		4

void ZERNStageHDR::SwitchLuminanceBuffers()
{
	ZEGRTexture2D* TempPtr = PreviousLuminance;
	PreviousLuminance = CurrentLuminance;
	CurrentLuminance = TempPtr;
}

bool ZERNStageHDR::UpdateInputOutput()
{
	return false;
}

bool ZERNStageHDR::UpdateRenderTargets()
{
	return false;
}

bool ZERNStageHDR::UpdateRenderStates()
{
	return false;
}

bool ZERNStageHDR::UpdateShaders()
{
	if(!DirtyFlags.GetFlags(ZERN_HSDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.FileName = "R#:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNHDR.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_Position";
	VertexShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNHDR_Luminance_Calculate_PixelShader";
	CalculateLuminance_PixelShader = ZEGRShader::Compile(Options);

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

	void* Data = NULL;
	if (!ConstantBuffer->Lock(&Data))
		return false;

	memcpy(Data, &Constants, sizeof(Constants));
	ConstantBuffer->Unlock();

	DirtyFlags.UnraiseFlags(ZERN_HSDF_CONSTANT_BUFFER);

	return true;
}

bool ZERNStageHDR::Update()
{
	if (!UpdateInputOutput())
		return false;

	if (!UpdateRenderTargets())
		return false;

	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	//if (!UpdateConstantBuffer())
		//return false;

	return true;
}

bool ZERNStageHDR::InitializeSelf()
{
	if (!ZERNStage::InitializeSelf())
		return false;

	//ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));

	LuminanceMips[0] = ZEGRTexture2D::CreateInstance(512, 512, 1, ZEGR_TF_R11FG11FB10F_FLOAT, true);
	LuminanceMips[1] = ZEGRTexture2D::CreateInstance(256, 256, 1, ZEGR_TF_R11FG11FB10F_FLOAT, true);
	LuminanceMips[2] = ZEGRTexture2D::CreateInstance(64, 64, 1, ZEGR_TF_R11FG11FB10F_FLOAT, true);
	LuminanceMips[3] = ZEGRTexture2D::CreateInstance(16, 16, 1, ZEGR_TF_R11FG11FB10F_FLOAT, true);
	LuminanceMips[4] = ZEGRTexture2D::CreateInstance(4, 4, 1, ZEGR_TF_R11FG11FB10F_FLOAT, true);
	LuminanceMips[5] = ZEGRTexture2D::CreateInstance(1, 1, 1, ZEGR_TF_R11FG11FB10F_FLOAT, true);

	BrightTexture = ZEGRTexture2D::CreateInstance(256, 256, 1, ZEGR_TF_R11FG11FB10F_FLOAT, true);

	return Update();
}

void ZERNStageHDR::DeinitializeSelf()
{
	CurrentLuminance.Release();
	PreviousLuminance.Release();
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

void ZERNStageHDR::SetAutoExposure(bool Enabled)
{
	if (Constants.AutoExposure == Enabled)
		return;

	Constants.AutoExposure = Enabled;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

bool ZERNStageHDR::GetAutoExposure() const
{
	return Constants.AutoExposure;
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

void ZERNStageHDR::SetBloomWeightLarge(float Value)
{
	if (Constants.BloomWeightLarge == Value)
		return;

	Constants.BloomWeightLarge = Value;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetBloomWeightLarge() const
{
	return Constants.BloomWeightLarge;
}

void ZERNStageHDR::SetBloomWeightMedium(float Value)
{
	if (Constants.BloomWeightMedium == Value)
		return;

	Constants.BloomWeightMedium = Value;
	
	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetBloomWeightMedium() const
{
	return Constants.BloomWeightMedium;
}

void ZERNStageHDR::SetBloomWeightSmall(float Value)
{
	if (Constants.BloomWeightSmall == Value)
		return;

	Constants.BloomWeightSmall = Value;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetBloomWeightSmall() const
{
	return Constants.BloomWeightSmall;
}

void ZERNStageHDR::SetExposure(float Value)
{
	if (Constants.Exposure == Value)
		return;

	Constants.Exposure = Value;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetExposure() const
{
	return Constants.Exposure;
}

void ZERNStageHDR::SetAdaptationRate(float Value)
{
	if (Constants.AdaptationRate == Value)
		return;

	Constants.AdaptationRate = Value;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetAdaptationRate() const
{
	return Constants.AdaptationRate;
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

void ZERNStageHDR::SetSaturation(float Value)
{
	if (Constants.Saturation == Value)
		return;

	Constants.Saturation = Value;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetSaturation() const
{
	return Constants.Saturation;
}

void ZERNStageHDR::SetLuminanceUpper(float Value)
{
	if (Constants.LuminanceUpper == Value)
		return;

	Constants.LuminanceUpper = Value;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetLuminanceUpper() const
{
	return Constants.LuminanceUpper;
}

void ZERNStageHDR::SetLuminanceLower(float Value)
{
	if (Constants.LuminanceLower == Value)
		return;

	Constants.LuminanceLower = Value;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetLuminanceLower() const
{
	return Constants.LuminanceLower;
}

void ZERNStageHDR::SetBloomPassCount(ZEUInt Count)
{
	if (Constants.BloomPassCount == Count)
		return;

	Constants.BloomPassCount = Count;

	DirtyFlags.RaiseFlags(ZERN_HSDF_CONSTANT_BUFFER);
}

ZEUInt ZERNStageHDR::GetBloomPassCount() const
{
	return Constants.BloomPassCount;
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

	Constants.AutoKey = false;
	Constants.AutoExposure = true;
	Constants.BloomEnabled = true;
	Constants.Key = 0.35f;
	Constants.Exposure = 0.3f;
	Constants.Saturation = 1.0f;
	Constants.WhiteLevel = 5.0f;
	Constants.LuminanceUpper = 10.0f;
	Constants.LuminanceLower = 0.00f;
	Constants.AdaptationRate = 1.5f;
	Constants.ToneMapOperator = ZERN_HTMO_FILMIC;
	Constants.BloomFactor = 2.0f;
	Constants.BloomThreshold = 5.0f;
	//Constants.BloomDeviation = 4.0f;
	Constants.BloomWeightLarge = 0.9f;
	Constants.BloomWeightMedium = 0.05f;
	Constants.BloomWeightSmall = 0.05f;
	Constants.BloomPassCount = 5;
}


bool ZERNStageHDR::Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands)
{
	if (!Update())
		return false;

	//Context->SetConstantBuffer(ZEGR_ST_PIXEL, 0, ConstantBuffer);

	CalculateLuminance(Context, InputTexture, LuminanceMips[0]->GetRenderTarget());
	GenerateMipMaps(Context);
	CalculateBrightness(Context, InputTexture, BrightTexture->GetRenderTarget());
	ToneMapping(Context, InputTexture, OutputRenderTarget);

	// Switch new and previous luminance
	SwitchLuminanceBuffers();

	return true;
}

void ZERNStageHDR::CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	Context->SetRenderTargets(0, NULL, NULL);
}

const ZEGRRenderState& ZERNStageHDR::GetRenderState()
{
	static ZEGRRenderState RenderState;

	RenderState = ZERNStage::GetRenderState();

	ZEGRDepthStencilState DepthStencilState;
	DepthStencilState.SetDepthTestEnable(false);
	DepthStencilState.SetDepthWriteEnable(false);
	DepthStencilState.SetStencilTestEnable(false);

	RenderState.SetDepthStencilState(DepthStencilState);

	ZEGRRasterizerState Rasterizer;
	Rasterizer.SetFillMode(ZEGR_FM_SOLID);
	Rasterizer.SetFrontIsCounterClockwise(false);
	Rasterizer.SetCullDirection(ZEGR_CD_COUNTER_CLOCKWISE);

	RenderState.SetRasterizerState(Rasterizer);

	ZEGRBlendState BlendState;
	BlendState.SetBlendEnable(false);

	RenderState.SetBlendState(BlendState);

	RenderState.SetRenderTargetFormat(0, ZEGR_TF_R11FG11FB10F_FLOAT);

	return RenderState;
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

void ZERNStageHDR::CreateRenderStates()
{
	ZEGRRenderState RenderState;

	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetRenderTargetFormat(0, ZEGR_TF_R11FG11FB10F_FLOAT);
	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);

	RenderState.SetShader(ZEGR_ST_PIXEL, CalculateLuminance_PixelShader);

	CalculateLuminance_RenderState = RenderState.Compile();
	zeCheckError(CalculateLuminance_RenderState == NULL, , "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, DownSampling_PixelShader);

	DownSampling_RenderState = RenderState.Compile();
	zeCheckError(DownSampling_RenderState == NULL, , "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, CalculateBrightness_PixelShader);

	CalculateBrightness_RenderState = RenderState.Compile();
	zeCheckError(CalculateBrightness_RenderState == NULL, , "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, ToneMapping_PixelShader);

	ToneMapping_RenderState = RenderState.Compile();
	zeCheckError(ToneMapping_RenderState == NULL, , "Cannot set render state.");


}

void ZERNStageHDR::CalculateLuminance(ZEGRContext* Context, ZEGRTexture2D* Input, ZEGRRenderTarget* Output)
{
	ZEGRViewport Viewport;
	Viewport.SetWidth(512.0f);
	Viewport.SetHeight(512.0f);
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
}

void ZERNStageHDR::DownSample(ZEGRContext* Context, ZEGRTexture2D* Input, ZEGRRenderTarget* Output)
{
	ZEGRViewport Viewport;
	Viewport.SetWidth((float)Output->GetWidth());
	Viewport.SetHeight((float)Output->GetHeight());
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
}

void ZERNStageHDR::GenerateMipMaps(ZEGRContext* Context)
{
	DownSample(Context, LuminanceMips[0], LuminanceMips[1]->GetRenderTarget());
	DownSample(Context, LuminanceMips[1], LuminanceMips[2]->GetRenderTarget());
	DownSample(Context, LuminanceMips[2], LuminanceMips[3]->GetRenderTarget());
	DownSample(Context, LuminanceMips[3], LuminanceMips[4]->GetRenderTarget());
	DownSample(Context, LuminanceMips[4], LuminanceMips[5]->GetRenderTarget());
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
	Context->SetTexture(ZEGR_ST_PIXEL, 0, Input);
	Context->SetRenderTargets(1, &Output, NULL);
	Context->SetRenderState(CalculateBrightness_RenderState);
	Context->SetVertexBuffer(0, NULL);

	Context->Draw(3, 0);
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
	Context->SetTexture(ZEGR_ST_PIXEL, 0, Input);
	Context->SetTexture(ZEGR_ST_PIXEL, 1, LuminanceMips[5]);
	Context->SetRenderTargets(1, &Output, NULL);
	Context->SetRenderState(ToneMapping_RenderState);
	Context->SetVertexBuffer(0, NULL);

	Context->Draw(3, 0);
}
