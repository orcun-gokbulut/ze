//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageOutputTexture.cpp
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

#include "ZERNStageOutputTexture.h"

#include "ZERNRenderer.h"
#include "ZERNStageID.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRRenderTarget.h"

bool ZERNStageOutputTexture::Update()
{
	if (!Dirty)
		return true;

	Texture = ZEGRTexture2D::CreateInstance(Width, Height, 1, Format, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE);
	if (Texture == NULL)
		return false;

	RenderTarget = Texture->GetRenderTarget();
	if (RenderTarget == NULL)
		return false;
	
	Dirty = false;

	return true;
}

ZERNStageOutputTexture::ZERNStageOutputTexture()
{
	TargetBuffer = ZERN_SO_COLOR;
	Width = 0;
	Height = 0;
	Format = ZEGR_TF_NONE;
}

ZEInt ZERNStageOutputTexture::GetId() const
{
	return ZERN_STAGE_TEXTURE_OUTPUT;
}

const ZEString& ZERNStageOutputTexture::GetName() const
{
	static const ZEString Name = "TextureOutput";
	return Name;
}

const ZEGRRenderTarget* ZERNStageOutputTexture::GetProvidedInput(ZERNStageBuffer Buffer) const
{
	if (Buffer == GetTargetBuffer())
		return RenderTarget;
	else
		return ZERNStage::GetProvidedInput(Buffer);
}

void ZERNStageOutputTexture::SetWidth(ZEUInt Width)
{
	if (this->Width == Width)
		return;

	this->Width = Width;

	Dirty = true;
}

ZEUInt ZERNStageOutputTexture::GetWidth() const
{
	return Width;
}

void ZERNStageOutputTexture::SetHeight(ZEUInt Height)
{
	if (this->Height == Height)
		return;

	this->Height = Height;

	Dirty = true;
}

ZEUInt ZERNStageOutputTexture::GetHeight() const
{
	return Height;
}

void ZERNStageOutputTexture::SetTargetBuffer(ZERNStageBuffer Buffer)
{
	TargetBuffer = Buffer;
}

ZERNStageBuffer ZERNStageOutputTexture::GetTargetBuffer() const
{
	return TargetBuffer;
}

void ZERNStageOutputTexture::SetFormat(ZEGRFormat Format)
{
	if (this->Format == Format)
		return;

	this->Format = Format;

	Dirty = true;
}

ZEGRFormat ZERNStageOutputTexture::GetFormat() const
{
	return Format;
}

bool ZERNStageOutputTexture::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!Update())
		return false;

	return true;
}
