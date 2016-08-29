//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageDisplay.cpp
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

#include "ZERNStageDisplay.h"

#include "ZEGraphics\ZEGRConstantBuffer.h"
#include "ZEGraphics\ZEGRTexture2D.h"
#include "ZEGraphics\ZEGRContext.h"
#include "ZEGraphics\ZEGRRenderTarget.h"
#include "ZERNShaderSlots.h"
#include "ZERNStageMultiplexer.h"

#define ZERN_SDDF_SAMPLER			1
#define ZERN_SDDF_CONSTANT_BUFFER	2

bool ZERNStageDisplay::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	ConstantBuffer = ZEGRConstantBuffer::CreateResource(sizeof(Constants));

	return true;
}

bool ZERNStageDisplay::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	ConstantBuffer.Release();
	Sampler.Release();

	InputTexture = NULL;

	return ZEInitializable::DeinitializeInternal();
}

bool ZERNStageDisplay::UpdateConstantBuffer()
{
	if (!DirtyFlags.GetFlags(ZERN_SDDF_CONSTANT_BUFFER))
		return true;

	ZEVector2 InputSizeMod;
	InputSizeMod.x = (InputSize.x <= 0 ? InputTexture->GetWidth() : InputSize.x) - InputOffset.x;
	InputSizeMod.y = (InputSize.y <= 0 ? InputTexture->GetHeight() : InputSize.y) - InputOffset.y;

	ZEVector2 OutputSizeMod;
	OutputSizeMod.x = (OutputSize.x <= 0 ? Viewport.GetWidth() : OutputSize.x);
	OutputSizeMod.y = (OutputSize.y <= 0 ? Viewport.GetHeight() : OutputSize.y);

	ZEVector2 PixelScale = ZEVector2(1.0f / InputTexture->GetWidth(), 1.0f / InputTexture->GetHeight());

	// Input Transform
	ZEMatrix3x3 InputFlipTransform;
	if (FlipMode != ZERN_SDMF_NONE)
	{
		ZEVector2 Center = ((InputSizeMod - InputOffset) / 2.0f) * PixelScale;

		ZEMatrix3x3 TranslateCenterTransform;
		ZEMatrix3x3::CreateTranslation2D(TranslateCenterTransform, -Center);

		ZEMatrix3x3 TranslateOriginTransform;
		ZEMatrix3x3::CreateTranslation2D(TranslateOriginTransform, Center);

		ZEVector2 FlipScale = ZEVector2::One;
		if (FlipMode == ZERN_SDMF_HORIZONTAL)
			FlipScale = ZEVector2(-1.0f, 1.0f);
		else if (FlipMode == ZERN_SDMF_VERTICAL)
			FlipScale = ZEVector2(1.0f, -1.0f);
		else if (FlipMode == ZERN_SDMF_BOTH)
			FlipScale = ZEVector2(-1.0f, -1.0f);

		ZEMatrix3x3 FlipScaleTransform;
		ZEMatrix3x3::CreateScale2D(FlipScaleTransform, FlipScale);

		InputFlipTransform = TranslateOriginTransform * FlipScaleTransform * TranslateCenterTransform;
	}
	else
	{
		InputFlipTransform = ZEMatrix3x3::Identity;
	}

	ZEMatrix3x3 InputZoomTransform;
	ZEMatrix3x3::CreateScale2D(InputZoomTransform, ZEVector2(1.0f, 1.0f) / Zoom);
	
	ZEMatrix3x3 InputOffsetTransform;
	ZEMatrix3x3::CreateTranslation2D(InputOffsetTransform, InputOffset);

	ZEMatrix3x3 InputTransform = InputOffsetTransform * InputZoomTransform * InputFlipTransform;

	// Output Transform
	ZERNStageDisplayScaleMode ModScaleMode = ScaleMode;
	if (ScaleMode == ZERN_SDSM_FIT_AUTO)
	{
		if (InputSizeMod.x > InputSizeMod.y)
			ScaleMode = ZERN_SDSM_FIT_HORIZONTAL;
		else
			ScaleMode = ZERN_SDSM_FIT_VERTICAL;
	}

	ZEMatrix3x3 OutputTransform;
	if (ScaleMode == ZERN_SDSM_FILL)
	{
		ZEMatrix3x3::CreateScale2D(OutputTransform, InputSizeMod / OutputSizeMod);
	}
	else if (ScaleMode == ZERN_SDSM_FIT_HORIZONTAL)
	{
		float ScaleX = 1.0f / (OutputSizeMod.x / InputSizeMod.x);
		ZEMatrix3x3::CreateScale2D(OutputTransform, ZEVector2(ScaleX, ScaleX));
	}
	else if (ScaleMode == ZERN_SDSM_FIT_VERTICAL)
	{
		float ScaleY = 1.0f / (OutputSizeMod.y / InputSizeMod.y);
		ZEMatrix3x3::CreateScale2D(OutputTransform, ZEVector2(ScaleY, ScaleY));
	}
	else
	{
		OutputTransform = ZEMatrix3x3::Identity;
	}

	ZEMatrix3x3 Matrix = OutputTransform * InputTransform;

	Constants.Transform0 = ZEVector4(Matrix.M[0][0], Matrix.M[0][1], Matrix.M[0][2], 0.0f);
	Constants.Transform1 = ZEVector4(Matrix.M[1][0], Matrix.M[1][1], Matrix.M[1][2], 0.0f);
	Constants.Transform2 = ZEVector4(Matrix.M[2][0], Matrix.M[2][1], Matrix.M[2][2], 0.0f);
	Constants.InputSize = InputSizeMod * PixelScale;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_SDDF_CONSTANT_BUFFER);

	return true;
}

bool ZERNStageDisplay::UpdateSampler()
{
	if (!DirtyFlags.GetFlags(ZERN_SDDF_SAMPLER))
		return true;

	ZEGRSamplerDescription SamplerDescription;
	SamplerDescription.MinFilter = Filter;
	SamplerDescription.MagFilter = Filter;
	SamplerDescription.MipFilter = Filter;
	SamplerDescription.BorderColor = ZEVector4::Zero;
	SamplerDescription.AddressU = ZEGR_TAM_BORDER;
	SamplerDescription.AddressV = ZEGR_TAM_BORDER;

	Sampler = ZEGRSampler::GetSampler(SamplerDescription);

	DirtyFlags.UnraiseFlags(ZERN_SDDF_SAMPLER);

	return true;
}

bool ZERNStageDisplay::Update()
{
	if (!UpdateConstantBuffer())
		return false;

	if (!UpdateSampler())
		return false;

	return true;
}

bool ZERNStageDisplay::Draw(ZEGRContext* Context)
{
	if (Owner == NULL)
		return false;

	if (InputTexture == NULL)
		return false;

	if (!Update())
		return false;

	Context->SetViewports(1, &Viewport);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_STAGE, ConstantBuffer);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, Sampler);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, InputTexture);

	Context->Draw(3, 0);

	return true;
}

ZERNStageDisplay::ZERNStageDisplay() : Link(this)
{
	DirtyFlags.RaiseAll();

	Owner = NULL;
	InputOffset = ZEVector2::Zero;
	InputSize = ZEVector2::Zero;
	OutputOffset = ZEVector2::Zero;
	OutputSize = ZEVector2::Zero;

	ScaleMode = ZERN_SDSM_NONE;
	FlipMode = ZERN_SDMF_NONE;
	Zoom = ZEVector2::One;
	Rotation = 0.0f;
	Filter = ZEGR_TFM_LINEAR;

	memset(&Constants, 0, sizeof(Constants));
}

ZERNStageDisplay::~ZERNStageDisplay()
{

}

void ZERNStageDisplay::SetInput(const ZEString& InputName)
{
	this->InputName = InputName;
}

const ZEString& ZERNStageDisplay::GetInput() const
{
	return InputName;
}

void ZERNStageDisplay::SetInputOffset(const ZEVector2& Offset)
{
	if (InputOffset == Offset)
		return;

	InputOffset = Offset;

	DirtyFlags.RaiseFlags(ZERN_SDDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNStageDisplay::GetInputOffset() const
{
	return InputOffset;
}

void ZERNStageDisplay::SetInputSize(const ZEVector2& Size)
{
	if (InputSize == Size)
		return;

	InputSize = Size;

	DirtyFlags.RaiseFlags(ZERN_SDDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNStageDisplay::GetInputSize() const
{
	return InputSize;
}

void ZERNStageDisplay::SetOutputOffset(const ZEVector2& Offset)
{
	if (OutputOffset == Offset)
		return;

	OutputOffset = Offset;

	DirtyFlags.RaiseFlags(ZERN_SDDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNStageDisplay::GetOutputOffset() const
{
	return OutputOffset;
}

void ZERNStageDisplay::SetOutputSize(const ZEVector2& Size)
{
	if (OutputSize == Size)
		return;

	OutputSize = Size;

	DirtyFlags.RaiseFlags(ZERN_SDDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNStageDisplay::GetOutputSize() const
{
	return OutputSize;
}

void ZERNStageDisplay::SetScaleMode(ZERNStageDisplayScaleMode Mode)
{
	if (ScaleMode == Mode)
		return;

	ScaleMode = Mode;

	DirtyFlags.RaiseFlags(ZERN_SDDF_CONSTANT_BUFFER);
}

ZERNStageDisplayScaleMode ZERNStageDisplay::GetScaleMode() const
{
	return ScaleMode;
}

void ZERNStageDisplay::SetZoom(const ZEVector2& Zoom)
{
	if (this->Zoom == Zoom)
		return;

	this->Zoom = Zoom;

	DirtyFlags.RaiseFlags(ZERN_SDDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNStageDisplay::GetZoom() const
{
	return Zoom;
}

void ZERNStageDisplay::SetRotation(float Rotation)
{
	if (this->Rotation == Rotation)
		return;

	this->Rotation = Rotation;

	DirtyFlags.RaiseFlags(ZERN_SDDF_CONSTANT_BUFFER);
}

float ZERNStageDisplay::GetRotation()const
{
	return Rotation;
}

void ZERNStageDisplay::SetFlipMode(ZERNStageDisplayFlipMode Mode)
{
	if (FlipMode == Mode)
		return;

	FlipMode = Mode;

	DirtyFlags.RaiseFlags(ZERN_SDDF_CONSTANT_BUFFER);
}

ZERNStageDisplayFlipMode ZERNStageDisplay::GetFlipMode() const
{
	return FlipMode;
}

void ZERNStageDisplay::SetFilter(ZEGRTextureFilter Filter)
{
	if (this->Filter == Filter)
		return;

	this->Filter = Filter;

	DirtyFlags.RaiseFlags(ZERN_SDDF_SAMPLER);
}

ZEGRTextureFilter ZERNStageDisplay::GetFilter() const
{
	return Filter;
}

void ZERNStageDisplay::SetViewport(const ZEGRViewport& Viewport)
{
	if (this->Viewport.GetX() == Viewport.GetX() &&
		this->Viewport.GetY() == Viewport.GetY() &&
		this->Viewport.GetWidth() == Viewport.GetWidth() &&
		this->Viewport.GetHeight() == Viewport.GetHeight())
		return;

	this->Viewport = Viewport;

	DirtyFlags.RaiseFlags(ZERN_SDDF_CONSTANT_BUFFER);
}

const ZEGRViewport& ZERNStageDisplay::GetViewport() const
{
	return Viewport;
}

ZERNStageDisplay* ZERNStageDisplay::CreateInstance()
{
	return new ZERNStageDisplay();
}
