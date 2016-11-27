//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStage2D.cpp
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

#include "ZERNStage2D.h"

#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZERNRenderer.h"

#define ZERN_2DDF_OUTPUT	1

bool ZERNStage2D::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	OutputTexture = NULL;

	return ZERNStage::DeinitializeInternal();
}

void ZERNStage2D::CreateOutput(const ZEString& Name)
{
	ZEUInt Width = GetRenderer()->GetOutputTexture()->GetWidth();
	ZEUInt Height = GetRenderer()->GetOutputTexture()->GetHeight();
	
	if (Name == "Color")
	{
		if (DirtyFlags.GetFlags(ZERN_2DDF_OUTPUT))
		{
			OutputTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, ZEGR_TF_R8G8B8A8_UNORM_SRGB).GetPointer();
			DirtyFlags.UnraiseFlags(ZERN_2DDF_OUTPUT);
		}
	}
}

ZEInt ZERNStage2D::GetId() const
{
	return ZERN_STAGE_2D;
}

const ZEString& ZERNStage2D::GetName() const
{
	static const ZEString Name = "Stage 2D";
	return Name;
}

void ZERNStage2D::Resized(ZEUInt Width, ZEUInt Height)
{
	Viewport.SetWidth((float)Width);
	Viewport.SetHeight((float)Height);

	DirtyFlags.RaiseFlags(ZERN_2DDF_OUTPUT);
}

bool ZERNStage2D::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (GetCommands().GetCount() == 0)
		return false;

	const ZEGRRenderTarget* RenderTarget = OutputTexture->GetRenderTarget();

	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetViewports(1, &Viewport);

	return true;
}

void ZERNStage2D::CleanUp(ZEGRContext* Context)
{
	ZERNStage::CleanUp(Context);
}

ZERNStage2D::ZERNStage2D()
{
	DirtyFlags.RaiseAll();

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&OutputTexture), "ColorTexture", ZERN_SRUT_WRITE, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_CREATE_OWN | ZERN_SRCF_GET_OUTPUT);
}

ZERNStage2D::~ZERNStage2D()
{
	Deinitialize();
}

ZEGRRenderState ZERNStage2D::GetRenderState()
{
	static ZEGRRenderState RenderState;
	static bool Initialized = false;

	if (!Initialized)
	{
		Initialized = true;

		RenderState.SetDepthStencilFormat(ZEGR_TF_NONE);
		RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);

		ZEGRBlendState BlendingState;
		BlendingState.SetBlendEnable(true);
		ZEGRBlendRenderTarget RenderTargetBlend;
		RenderTargetBlend.SetBlendEnable(true);
		RenderTargetBlend.SetSource(ZEGR_BO_SRC_ALPHA);
		RenderTargetBlend.SetDestination(ZEGR_BO_INV_SRC_ALPHA);
		BlendingState.SetRenderTargetBlend(0, RenderTargetBlend);
 		RenderState.SetBlendState(BlendingState);

		ZEGRDepthStencilState DepthStencilState;
		DepthStencilState.SetDepthTestEnable(false);
		DepthStencilState.SetDepthWriteEnable(false);
		RenderState.SetDepthStencilState(DepthStencilState);
	}

	return RenderState;
}
