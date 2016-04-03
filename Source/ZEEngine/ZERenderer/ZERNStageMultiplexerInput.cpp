//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageMultiplexerInput.cpp
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

#include "ZERNStageMultiplexerInput.h"

#include "ZERNStageMultiplexer.h"
#include "ZEGraphics\ZEGRConstantBuffer.h"
#include "ZEGraphics\ZEGRTexture2D.h"
#include "ZEGraphics\ZEGRContext.h"
#include "ZERNShaderSlots.h"
#include "ZEGraphics\ZEGRRenderTarget.h"

#define ZERN_SMIDF_SAMPLER	0x01
#define ZERN_SMIDF_CONSTANT_BUFFER	0x02

void ZERNStageMultiplexerInput::DeinitializeSelf()
{
	ConstantBuffer.Release();
	Sampler.Release();
	DirtyFlags.RaiseAll();
}

bool ZERNStageMultiplexerInput::UpdateInputOutput()
{

	if (Input != ZERN_SO_NONE)
	{
		const ZEGRTexture2D* NewTexture = Owner->GetPrevOutput(Input);
		if (NewTexture == NULL)
		{
			return false;
		}
		else if (InputTexture != NewTexture)
		{
			InputTexture = NewTexture;
			DirtyFlags.RaiseFlags(ZERN_SMIDF_CONSTANT_BUFFER);
		}
	}

	if (Owner != NULL)
	{
		if (OutputRenderTarget != Owner->OutputRenderTarget)
		{
			OutputRenderTarget = Owner->OutputRenderTarget;
			DirtyFlags.RaiseFlags(ZERN_SMIDF_CONSTANT_BUFFER);
		}
	}

	return true;
}

bool ZERNStageMultiplexerInput::UpdateConstantBuffer()
{
	if (!DirtyFlags.GetFlags(ZERN_SMIDF_CONSTANT_BUFFER))
		return true;

	if (ConstantBuffer.IsNull())
		ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));

	if (ConstantBuffer.IsNull())
		return false;

	if (InputTexture.IsNull())
		return false;

	ZEVector2 InputSizeMod;
	InputSizeMod.x = (InputSize.x <= 0 ? InputTexture->GetWidth() : InputSize.x);
	InputSizeMod.y = (InputSize.y <= 0 ? InputTexture->GetHeight() : InputSize.y);

	ZEVector2 OutputSizeMod;
	OutputSizeMod.x = (OutputSize.x <= 0 ? OutputRenderTarget->GetWidth() : OutputSize.x);
	OutputSizeMod.y = (OutputSize.y <= 0 ? OutputRenderTarget->GetHeight() : OutputSize.y);

	Viewport.SetX(OutputOffset.x);
	Viewport.SetY(OutputOffset.y);
	Viewport.SetWidth(OutputSizeMod.x);
	Viewport.SetHeight(OutputSizeMod.y);

	ZEVector2 PixelScale = ZEVector2(1.0f / InputTexture->GetWidth(), 1.0f / InputTexture->GetHeight());

	// Input Transform
	ZEMatrix3x3 InputFlipTransform = ZEMatrix3x3::Identity;
	if (FlipMode != ZERN_SMF_NONE)
	{
		ZEVector2 Center = (InputSizeMod / 2.0f - InputOffset) * PixelScale;

		ZEMatrix3x3 TranslateCenterTransform;
		ZEMatrix3x3::CreateTranslation2D(TranslateCenterTransform, -Center);

		ZEMatrix3x3 TranslateOriginTransform;
		ZEMatrix3x3::CreateTranslation2D(TranslateOriginTransform, Center);

		ZEVector2 FlipScale = ZEVector2::One;
		if (FlipMode == ZERN_SMF_HORIZONTAL)
			FlipScale = ZEVector2(-1.0f, 1.0f);
		else if (FlipMode == ZERN_SMF_VERTICAL)
			FlipScale = ZEVector2(1.0f, -1.0f);
		else if (FlipMode == ZERN_SMF_BOTH)
			FlipScale = ZEVector2(-1.0f, -1.0f);

		ZEMatrix3x3 FlipScaleTransform;
		ZEMatrix3x3::CreateScale2D(FlipScaleTransform, FlipScale);

		InputFlipTransform = TranslateOriginTransform * FlipScaleTransform * TranslateCenterTransform;
	}

	ZEMatrix3x3 InputZoomTransform;
	ZEMatrix3x3::CreateScale2D(InputZoomTransform, ZEVector2(1.0f, 1.0f) / Zoom);
	
	ZEMatrix3x3 InputOffsetTransform;
	ZEMatrix3x3::CreateTranslation2D(InputOffsetTransform, -InputOffset);

	ZEMatrix3x3 InputTransform = InputOffsetTransform * InputZoomTransform * InputFlipTransform;


	// Output Transform
	ZERNStageMultiplexerInputScaleMode ModScaleMode = ScaleMode;
	if (ScaleMode == ZERN_SMIM_FIT_AUTO)
	{
		if (InputSizeMod.x > InputSizeMod.y)
			ScaleMode = ZERN_SMIM_FIT_HORIZONTAL;
		else
			ScaleMode = ZERN_SMIM_FIT_VERTICAL;
	}

	ZEMatrix3x3 OutputTransform;
	if (ScaleMode == ZERN_SMIM_FILL)
	{
		OutputTransform = ZEMatrix3x3::Identity;
	}
	else if (ScaleMode == ZERN_SMIM_FIT_HORIZONTAL)
	{
		float ScaleX = OutputSizeMod.x / InputSizeMod.x;
		ZEMatrix3x3::CreateScale2D(OutputTransform, ScaleX, ScaleX);
	}
	else if (ScaleMode == ZERN_SMIM_FIT_VERTICAL)
	{
		float ScaleY = OutputSizeMod.y / InputSizeMod.y;
		ZEMatrix3x3::CreateScale2D(OutputTransform, ScaleY, ScaleY);
	}
	else
	{
		ZEMatrix3x3::CreateScale2D(OutputTransform, OutputSizeMod * PixelScale);
	}

	ZEMatrix3x3 Matrix = OutputTransform * InputTransform;

	Constants.Transform0 = ZEVector4(Matrix.M[0][0], Matrix.M[0][1], Matrix.M[0][2], 0.0f);
	Constants.Transform1 = ZEVector4(Matrix.M[1][0], Matrix.M[1][1], Matrix.M[1][2], 0.0f);
	Constants.Transform2 = ZEVector4(Matrix.M[2][0], Matrix.M[2][1], Matrix.M[2][2], 0.0f);
	Constants.InputSize = InputSizeMod * PixelScale;

	void* Buffer;
	ConstantBuffer->Lock(&Buffer);
	memcpy(Buffer, &Constants, sizeof(Constants));
	ConstantBuffer->Unlock();

	return true;
}

bool ZERNStageMultiplexerInput::UpdateSampler()
{
	if (!DirtyFlags.GetFlags(ZERN_SMIDF_SAMPLER))
		return true;

	ZEGRSamplerDescription SamplerDescription;
	SamplerDescription.MipFilter = Filter;
	SamplerDescription.MagFilter = Filter;
	SamplerDescription.MinFilter = Filter;
	SamplerDescription.BorderColor = ZEVector4::Zero;
	SamplerDescription.AddressU = ZEGR_TAM_BORDER;
	SamplerDescription.AddressV = ZEGR_TAM_BORDER;

	Sampler = ZEGRSampler::GetSampler(SamplerDescription);

	return true;
}

void ZERNStageMultiplexerInput::SetInput(ZERNStageBuffer Input)
{
	this->Input = Input;
}

ZERNStageBuffer ZERNStageMultiplexerInput::GetInput() const
{
	return Input;
}

void ZERNStageMultiplexerInput::SetInputGlobal(const ZEString& Name)
{

}

const ZEString& ZERNStageMultiplexerInput::GetInputGlobal()
{
	return ZEString::Empty;
}

void ZERNStageMultiplexerInput::SetTexture(const ZEGRTexture2D* Texture)
{
	this->InputTexture = Texture;
}

const ZEGRTexture2D* ZERNStageMultiplexerInput::GetTexture() const
{
	return InputTexture;
}

void ZERNStageMultiplexerInput::SetInputOffset(const ZEVector2& Offset)
{
	if (InputOffset == Offset)
		return;

	InputOffset = Offset;

	DirtyFlags.RaiseFlags(ZERN_SMIDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNStageMultiplexerInput::GetInputOffset() const
{
	return InputOffset;
}

void ZERNStageMultiplexerInput::SetInputSize(const ZEVector2& Size)
{
	if (InputSize == Size)
		return;

	InputSize = Size;

	DirtyFlags.RaiseFlags(ZERN_SMIDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNStageMultiplexerInput::GetInputSize() const
{
	return InputSize;
}

void ZERNStageMultiplexerInput::SetOutputOffset(const ZEVector2& Offset)
{
	if (OutputOffset == Offset)
		return;

	OutputOffset = Offset;

	DirtyFlags.RaiseFlags(ZERN_SMIDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNStageMultiplexerInput::GetOutputOffset() const
{
	return OutputOffset;
}

void ZERNStageMultiplexerInput::SetOutputSize(const ZEVector2& Size)
{
	if (OutputSize == Size)
		return;

	OutputSize = Size;

	DirtyFlags.RaiseFlags(ZERN_SMIDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNStageMultiplexerInput::GetOutputSize() const
{
	return OutputSize;
}

void ZERNStageMultiplexerInput::SetScaleMode(ZERNStageMultiplexerInputScaleMode Mode)
{
	if (ScaleMode == Mode)
		return;

	ScaleMode = Mode;

	DirtyFlags.RaiseFlags(ZERN_SMIDF_CONSTANT_BUFFER);
}

ZERNStageMultiplexerInputScaleMode ZERNStageMultiplexerInput::GetScaleMode() const
{
	return ScaleMode;
}

void ZERNStageMultiplexerInput::SetZoom(const ZEVector2& Zoom)
{
	if (this->Zoom == Zoom)
		return;

	this->Zoom = Zoom;

	DirtyFlags.RaiseFlags(ZERN_SMIDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNStageMultiplexerInput::GetZoom() const
{
	return Zoom;
}

void ZERNStageMultiplexerInput::SetRotate(float Rotation)
{
	if (this->Rotation == Rotation)
		return;

	this->Rotation = Rotation;

	DirtyFlags.RaiseFlags(ZERN_SMIDF_CONSTANT_BUFFER);
}

float ZERNStageMultiplexerInput::GetRotation()const
{
	return Rotation;
}

void ZERNStageMultiplexerInput::SetFlipMode(ZERNStageMultiplexerInputFlipMode Mode)
{
	if (FlipMode == Mode)
		return;

	FlipMode = Mode;

	DirtyFlags.RaiseFlags(ZERN_SMIDF_CONSTANT_BUFFER);
}

ZERNStageMultiplexerInputFlipMode ZERNStageMultiplexerInput::GetFlipMode() const
{
	return FlipMode;
}

void ZERNStageMultiplexerInput::SetFilter(ZEGRTextureFilter Filter)
{
	if (this->Filter == Filter)
		return;

	this->Filter = Filter;

	DirtyFlags.RaiseFlags(ZERN_SMIDF_SAMPLER);
}

ZEGRTextureFilter ZERNStageMultiplexerInput::GetFilter() const
{
	return Filter;
}

bool ZERNStageMultiplexerInput::Draw(ZEGRContext* Context)
{
	if (Owner == NULL)
		return false;

	if (!UpdateInputOutput())
		return false;

	if (!UpdateSampler())
		return false;

	if (!UpdateConstantBuffer())
		return false;

	Context->SetViewports(1, &Viewport);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_STAGE, ConstantBuffer);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, InputTexture);
	Context->Draw(3, 0);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_STAGE, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);

	return true;
}

ZERNStageMultiplexerInput::ZERNStageMultiplexerInput() : StageLink(this)
{
	DirtyFlags.RaiseAll();

	Owner = NULL;
	Input = ZERN_SO_NONE;
	InputOffset = ZEVector2::Zero;
	InputSize = ZEVector2::Zero;
	OutputOffset = ZEVector2::Zero;
	OutputSize = ZEVector2::Zero;

	ScaleMode = ZERN_SMIM_NONE;
	FlipMode = ZERN_SMF_NONE;
	Zoom = ZEVector2::One;
	Rotation = 0.0f;
	Filter = ZEGR_TFM_LINEAR;
}
