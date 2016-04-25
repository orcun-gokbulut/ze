//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStage.cpp
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

#include "ZERNStage.h"

#include "ZEDS/ZEList2.h"

#include "ZERNRenderer.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"

const ZEGRTexture2D* ZERNStage::GetPrevOutput(ZERNStageBuffer Input) const
{
	ZERNStage* PrevStage = GetPrevStage();
	if (PrevStage == NULL)
		return NULL;

	return PrevStage->GetOutput(Input);
}

const ZEGRRenderTarget* ZERNStage::GetNextProvidedInput(ZERNStageBuffer RenderTarget) const
{
	ZERNStage* NextStage = GetNextStage();
	if (NextStage == NULL)
		return NULL;

	return NextStage->GetProvidedInput(RenderTarget);
}

bool ZERNStage::BindOutput(ZERNStageBuffer Output, ZEGRFormat Format, bool BothWay, ZEHolder<const ZEGRTexture2D>& Buffer, ZEHolder<const ZEGRRenderTarget>& Target)
{
	if (BothWay)
	{
		ZEHolder<const ZEGRTexture2D> PrevOutput = GetPrevOutput(Output);
		if (PrevOutput != NULL)
		{
			Buffer = PrevOutput;
			Target = PrevOutput->GetRenderTarget(0);
			return false;
		}
	}

	ZEHolder<const ZEGRRenderTarget> ProvidedRenderTarget = GetProvidedInput(Output);
	if (ProvidedRenderTarget != NULL)
	{
		Buffer.Release();
		Target = ProvidedRenderTarget;
		return false;
	}

	const ZEGRRenderTarget* OriginalRenderTarget = GetRenderer()->GetOutputRenderTarget();
	if (Buffer->GetWidth() != OriginalRenderTarget->GetWidth() ||
		Buffer->GetHeight() != OriginalRenderTarget->GetHeight())
	{
		Buffer = ZEGRTexture2D::CreateInstance(OriginalRenderTarget->GetWidth(), OriginalRenderTarget->GetHeight(), 1, Format, ZEGR_RU_GPU_READ_CPU_WRITE, ZEGR_RBF_RENDER_TARGET).GetPointer();
		Target = ProvidedRenderTarget;
	}

	return true;
}

bool ZERNStage::BindDepthOutput(ZERNStageBuffer Output, ZEGRFormat Format, bool BothWay, ZEHolder<const ZEGRTexture2D>& Buffer, ZEHolder<const ZEGRDepthStencilBuffer>& Target)
{
	if (BothWay)
	{
		ZEHolder<const ZEGRTexture2D> PrevOutput = GetPrevOutput(Output);
		if (PrevOutput != NULL)
		{
			Buffer = PrevOutput;
			Target = PrevOutput->GetDepthStencilBuffer();
			return false;
		}
	}

	const ZEGRRenderTarget* OriginalRenderTarget = GetRenderer()->GetOutputRenderTarget();
	if (Buffer->GetWidth() != OriginalRenderTarget->GetWidth() ||
		Buffer->GetHeight() != OriginalRenderTarget->GetHeight())
	{
		Buffer = ZEGRTexture2D::CreateInstance(OriginalRenderTarget->GetWidth(), OriginalRenderTarget->GetHeight(), 1, Format, ZEGR_RU_GPU_READ_CPU_WRITE, ZEGR_RBF_DEPTH_STENCIL).GetPointer();
		Target = Buffer->GetDepthStencilBuffer(0);
	}

	return true;
}

ZERNRenderer* ZERNStage::GetRenderer() const
{
	return Renderer;
}

void ZERNStage::SetEnabled(bool Enable)
{
	this->Enabled = Enable;
}

bool ZERNStage::GetEnabled() const
{
	return Enabled;
}

const ZEList2<ZERNCommand>& ZERNStage::GetCommands() const
{
	return Commands;
}

ZERNStage* ZERNStage::GetPrevStage() const
{
	return (Link.GetPrev() != NULL ? Link.GetPrev()->GetItem() : NULL);
}

ZERNStage* ZERNStage::GetNextStage() const
{
	return (Link.GetNext() != NULL ? Link.GetNext()->GetItem() : NULL);
}

const ZEGRTexture2D* ZERNStage::GetOutput(ZERNStageBuffer Output) const
{
	if (GetPrevStage() == NULL)
		return NULL;

	return GetPrevStage()->GetOutput(Output);
}

const ZEGRRenderTarget* ZERNStage::GetProvidedInput(ZERNStageBuffer Input) const
{
	if (GetNextStage() == NULL)
		return NULL;
	
	return GetNextStage()->GetProvidedInput(Input);
}

bool ZERNStage::Setup(ZEGRContext* Context)
{
	if(Renderer == NULL || Context == NULL)
		return false;

	return true;
}

void ZERNStage::CleanUp(ZEGRContext* Context)
{

}

ZERNStage::ZERNStage() : Link(this)
{
	Renderer = NULL;
	Enabled = true;
}

ZEGRRenderState ZERNStage::GetRenderState()
{
	return ZEGRRenderState::Default;
}
