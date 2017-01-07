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
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRShaderCompiler.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"

#define ZERN_SHDF_CONSTANT_BUFFER	1
#define ZERN_SHDF_SHADERS			2
#define ZERN_SHDF_RENDER_STATE		4
#define ZERN_SHDF_TEXTURE			8
#define ZERN_SHDF_OUTPUT			16

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
	if (!DirtyFlags.GetFlags(ZERN_SHDF_RENDER_STATE))
		return true;
	
	ZEGRRenderState RenderState;
	RenderState.SetRenderTargetFormat(0, ZEGR_TF_R8G8B8A8_UNORM_SRGB);
	RenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverPositionTexcoordVertexShader);

	ZEGRDepthStencilState DepthStencilStateNoTestWrite;
	DepthStencilStateNoTestWrite.SetDepthTestEnable(false);
	DepthStencilStateNoTestWrite.SetDepthWriteEnable(false);

	RenderState.SetDepthStencilState(DepthStencilStateNoTestWrite);

	RenderState.SetShader(ZEGR_ST_PIXEL, CalculateLuminancePixelShader);
	CalculateLuminanceRenderStateData = RenderState.Compile();
	zeCheckError(CalculateLuminanceRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, CalculateAdaptedLuminancePixelShader);
	CalculateAdaptedLuminanceRenderStateData = RenderState.Compile();
	zeCheckError(CalculateAdaptedLuminanceRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, CalculateBrightnessPixelShader);
	CalculateBrightnessRenderStateData = RenderState.Compile();
	zeCheckError(CalculateBrightnessRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, ToneMappingPixelShader);
	ToneMappingRenderStateData = RenderState.Compile();
	zeCheckError(ToneMappingRenderStateData == NULL, false, "Cannot set render state.");

	DirtyFlags.UnraiseFlags(ZERN_SHDF_RENDER_STATE);

	return true;
}

bool ZERNStageHDR::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_SHDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNHDR.hlsl";
	Options.Model = ZEGR_SM_5_0;
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString::FromInt32(ZEGRGraphicsModule::SAMPLE_COUNT)));

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_PositionTexcoords";
	ScreenCoverPositionTexcoordVertexShader= ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNHDR_Luminance_Calculate_PixelShader";
	CalculateLuminancePixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNHDR_Luminance_CalculateAdapted_PixelShader";
	CalculateAdaptedLuminancePixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNHDR_Bright_Calculate_PixelShader";
	CalculateBrightnessPixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNHDR_ToneMapping_PixelShader";
	ToneMappingPixelShader = ZEGRShader::Compile(Options);

	DirtyFlags.UnraiseFlags(ZERN_SHDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_SHDF_RENDER_STATE);

	return true;
}

bool ZERNStageHDR::UpdateConstantBuffer()
{
	if (!DirtyFlags.GetFlags(ZERN_SHDF_CONSTANT_BUFFER))
		return true;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_SHDF_CONSTANT_BUFFER);

	return true;
}

bool ZERNStageHDR::UpdateTextures()
{
	if (!DirtyFlags.GetFlags(ZERN_SHDF_TEXTURE))
		return true;

	ZEUInt CurrentWidth = GetRenderer()->GetOutputTexture()->GetWidth();
	ZEUInt CurrentHeight = GetRenderer()->GetOutputTexture()->GetHeight();

	BrightTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, CurrentWidth / 2, CurrentHeight / 2, 1, ZEGR_TF_R11G11B10_FLOAT);
	DownScaledTexture4x = ZEGRTexture::CreateResource(ZEGR_TT_2D, CurrentWidth / 4, CurrentHeight / 4, 1, ZEGR_TF_R11G11B10_FLOAT);
	DownScaledTexture8x = ZEGRTexture::CreateResource(ZEGR_TT_2D, CurrentWidth / 8, CurrentHeight / 8, 1, ZEGR_TF_R11G11B10_FLOAT);

	ZEInt ExponentWidth = (ZEInt)(ZEMath::Log10((float)CurrentWidth) / ZEMath::Log10(2.0f));
	ZEInt ExponentHeight = (ZEInt)(ZEMath::Log10((float)CurrentHeight) / ZEMath::Log10(2.0f));

	ZEInt LevelCount = ZEMath::Min(ExponentWidth, ExponentHeight) + 1;
	LuminanceTextures.Resize(LevelCount);

	ZEUInt Index = 0;
	for (ZEInt I = (LevelCount - 1); I >= 0; I--)
	{
		ZEUInt Size = (ZEUInt)ZEMath::Power(2.0f, (float)I);
		LuminanceTextures[Index++] = ZEGRTexture::CreateResource(ZEGR_TT_2D, Size, Size, 1, ZEGR_TF_R16_FLOAT);
	}

	CurrentAdaptedLuminance = ZEGRTexture::CreateResource(ZEGR_TT_2D, 1, 1, 1, ZEGR_TF_R16_FLOAT);
	PreviousAdaptedLuminance = ZEGRTexture::CreateResource(ZEGR_TT_2D, 1, 1, 1, ZEGR_TF_R16_FLOAT);

	ZEUInt16 One = 1;
	PreviousAdaptedLuminance->Update(&One, 1);

	DirtyFlags.UnraiseFlags(ZERN_SHDF_TEXTURE);

	return true;
}

bool ZERNStageHDR::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateTextures())
		return false;

	if (!UpdateConstantBuffer())
		return false;

	return true;
}

void ZERNStageHDR::CalculateAverageLuminance(ZEGRContext* Context)
{
	const ZEGRRenderTarget* RenderTarget = LuminanceTextures[0]->GetRenderTarget();

	Viewport.SetWidth((float)RenderTarget->GetWidth());
	Viewport.SetHeight((float)RenderTarget->GetHeight());

	Context->SetRenderState(CalculateLuminanceRenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, InputColorTexture);
	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);

	for (ZEUInt I = 1; I < LuminanceTextures.GetCount(); I++)
		Filter.ApplyScale(Context, LuminanceTextures[I - 1], LuminanceTextures[I]->GetRenderTarget());
}

void ZERNStageHDR::CalculateAdaptedLuminance(ZEGRContext* Context)
{
	Viewport.SetWidth(1.0f);
	Viewport.SetHeight(1.0f);

	const ZEGRRenderTarget* RenderTarget = CurrentAdaptedLuminance->GetRenderTarget();

	Context->SetRenderState(CalculateAdaptedLuminanceRenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	const ZEGRTexture* Textures[] = {LuminanceTextures.GetLastItem(), PreviousAdaptedLuminance};
	Context->SetTextures(ZEGR_ST_PIXEL, 7, 2, Textures);
	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);
}

const ZEGRTexture* ZERNStageHDR::CalculateBloom(ZEGRContext* Context)
{
	const ZEGRRenderTarget* RenderTarget = BrightTexture->GetRenderTarget();

	Viewport.SetWidth((float)RenderTarget->GetWidth());
	Viewport.SetHeight((float)RenderTarget->GetHeight());

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, ConstantBuffer);
	Context->SetRenderState(CalculateBrightnessRenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, InputColorTexture);
	Context->SetTexture(ZEGR_ST_PIXEL, 7, CurrentAdaptedLuminance);
	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);

	Filter.ApplyScale(Context, BrightTexture, DownScaledTexture4x->GetRenderTarget());

	for (ZEUInt I = 0; I < 3; I++)
		Filter.ApplyGaussianBlur(Context, DownScaledTexture4x, 6, 2.0f);

	return DownScaledTexture4x;
}

void ZERNStageHDR::ToneMapping(ZEGRContext* Context, const ZEGRTexture* BloomTexture)
{
	Viewport.SetWidth((float)OutputColorTexture->GetWidth());
	Viewport.SetHeight((float)OutputColorTexture->GetHeight());

	const ZEGRRenderTarget* RenderTarget = OutputColorTexture->GetRenderTarget();

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, ConstantBuffer);
	Context->SetRenderState(ToneMappingRenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	const ZEGRTexture* Textures[] = {InputColorTexture, BloomTexture, CurrentAdaptedLuminance};
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 3, Textures);
	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);
}

void ZERNStageHDR::CreateOutput(const ZEString& Name)
{
	if (Name == "ColorTexture")
	{
		if (DirtyFlags.GetFlags(ZERN_SHDF_OUTPUT))
		{
			OutputColorTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, InputColorTexture->GetWidth(), InputColorTexture->GetHeight(), 1, ZEGR_TF_R8G8B8A8_UNORM_SRGB).GetPointer();
			DirtyFlags.UnraiseFlags(ZERN_SHDF_OUTPUT);
		}
	}
}

bool ZERNStageHDR::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;
	
	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

	Filter.Initialize();

	return Update();
}

bool ZERNStageHDR::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();
	Filter.Deinitialize();

	CurrentAdaptedLuminance.Release();
	PreviousAdaptedLuminance.Release();

	ScreenCoverPositionTexcoordVertexShader.Release();
	CalculateLuminancePixelShader.Release();
	CalculateAdaptedLuminancePixelShader.Release();
	CalculateBrightnessPixelShader.Release();
	ToneMappingPixelShader.Release();
	
	CalculateLuminanceRenderStateData.Release();
	CalculateAdaptedLuminanceRenderStateData.Release();
	CalculateBrightnessRenderStateData.Release();
	ToneMappingRenderStateData.Release();

	ConstantBuffer.Release();

	DownScaledTexture4x.Release();
	DownScaledTexture8x.Release();
	BrightTexture.Release();
	LuminanceTextures.Clear();
	CurrentAdaptedLuminance.Release();
	PreviousAdaptedLuminance.Release();

	InputColorTexture = NULL;
	OutputColorTexture = NULL;

	return ZERNStage::DeinitializeInternal();
}

ZEInt ZERNStageHDR::GetId() const
{
	return ZERN_STAGE_HDR;
}

const ZEString& ZERNStageHDR::GetName() const
{
	static const ZEString Name = "Stage HDR";
	return Name;
}

void ZERNStageHDR::SetKey(float Key)
{
	if (Constants.Key == Key)
		return;

	Constants.Key = Key;

	DirtyFlags.RaiseFlags(ZERN_SHDF_CONSTANT_BUFFER);
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

	DirtyFlags.RaiseFlags(ZERN_SHDF_CONSTANT_BUFFER);
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

	DirtyFlags.RaiseFlags(ZERN_SHDF_CONSTANT_BUFFER);
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

	DirtyFlags.RaiseFlags(ZERN_SHDF_CONSTANT_BUFFER);
}

float ZERNStageHDR::GetBloomFactor() const
{
	return Constants.BloomFactor;
}

void ZERNStageHDR::SetBloomThreshold(float BloomThreshold)
{
	if (Constants.BloomThreshold == BloomThreshold)
		return;

	Constants.BloomThreshold = ZEMath::Max(0.0f, BloomThreshold);

	DirtyFlags.RaiseFlags(ZERN_SHDF_CONSTANT_BUFFER);
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
	
	DirtyFlags.RaiseFlags(ZERN_SHDF_CONSTANT_BUFFER);
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

	DirtyFlags.RaiseFlags(ZERN_SHDF_CONSTANT_BUFFER);
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

	DirtyFlags.RaiseFlags(ZERN_SHDF_CONSTANT_BUFFER);
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

	DirtyFlags.RaiseFlags(ZERN_SHDF_CONSTANT_BUFFER);
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

	DirtyFlags.RaiseFlags(ZERN_SHDF_CONSTANT_BUFFER);
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

void ZERNStageHDR::Resized(ZEUInt Width, ZEUInt Height)
{
	DirtyFlags.RaiseFlags(ZERN_SHDF_TEXTURE | ZERN_SHDF_OUTPUT);
}

bool ZERNStageHDR::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!Update())
		return false;

	CalculateAverageLuminance(Context);
	CalculateAdaptedLuminance(Context);
	const ZEGRTexture* BloomTexture = (Constants.BloomEnabled) ? CalculateBloom(Context) : NULL;
	ToneMapping(Context, BloomTexture);

	ZEHolder<ZEGRTexture> Temp = PreviousAdaptedLuminance;
	PreviousAdaptedLuminance = CurrentAdaptedLuminance;
	CurrentAdaptedLuminance = Temp;

	return true;
}

void ZERNStageHDR::CleanUp(ZEGRContext* Context)
{
	ZERNStage::CleanUp(Context);
}

ZERNStageHDR::ZERNStageHDR()
{
	DirtyFlags.RaiseAll();

	BlurTextureSize = ZERN_HBTS_HALF;

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

	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&InputColorTexture), "ColorTexture", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&OutputColorTexture), "ColorTexture", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN | ZERN_SRCF_GET_OUTPUT);
}

ZERNStageHDR::~ZERNStageHDR()
{
	Deinitialize();
}
