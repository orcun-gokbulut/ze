//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageGBuffer.cpp
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

#include "ZERNStageGBuffer.h"
#include "ZERNRenderer.h"
#include "ZERNStageID.h"
#include "ZEGraphics\ZEGRContext.h"
#include "ZEGraphics\ZEGRTexture2D.h"
#include "ZEGraphics\ZEGROutput.h"
#include "ZEGraphics\ZEGRRenderTarget.h"

void ZERNStageGBuffer::UpdateRenderTargets(ZERNRenderer* Renderer)
{
	ZEGROutput* Output = Renderer->GetOutput();
	ZEGRRenderTarget* OutputRenderTarget = Output->GetRenderTarget();
	if (Output == NULL || OutputRenderTarget == NULL)
	{
		GBuffer0.Release();
		GBuffer1.Release();
		GBuffer2.Release();
		GBuffer3.Release();
		DepthStencilBuffer.Release();
	}

	if (!GBuffer0.IsNull() && GBuffer0->GetWidth() == OutputRenderTarget->GetWidth() && GBuffer0->GetHeight() == OutputRenderTarget->GetHeight())
		return;

	DepthStencilBuffer = ZEGRDepthStencilBuffer::Create(OutputRenderTarget->GetWidth(), OutputRenderTarget->GetHeight(), ZEGR_DSF_DEPTH24_STENCIL8);
	GBuffer0 = ZEGRTexture2D::CreateInstance(OutputRenderTarget->GetWidth(), OutputRenderTarget->GetHeight(), 1, ZEGR_TF_R10G10B10A2_UINT, true);
	GBuffer1 = ZEGRTexture2D::CreateInstance(OutputRenderTarget->GetWidth(), OutputRenderTarget->GetHeight(), 1, ZEGR_TF_R8G8B8A8_UNORM, true);
	GBuffer2 = ZEGRTexture2D::CreateInstance(OutputRenderTarget->GetWidth(), OutputRenderTarget->GetHeight(), 1, ZEGR_TF_R8G8B8A8_UNORM, true);
	GBuffer3 = ZEGRTexture2D::CreateInstance(OutputRenderTarget->GetWidth(), OutputRenderTarget->GetHeight(), 1, ZEGR_TF_R8G8B8A8_UNORM, true);

	Viewport.SetX(0.0f);
	Viewport.SetY(0.0f);
	Viewport.SetWidth((float)OutputRenderTarget->GetWidth());
	Viewport.SetHeight((float)OutputRenderTarget->GetHeight());
	Viewport.SetMinDepth(0.0f);
	Viewport.SetMaxDepth(1.0f);
}

ZEInt ZERNStageGBuffer::GetId()
{
	return ZERN_STAGE_GBUFFER;
}

const ZEString& ZERNStageGBuffer::GetName()
{
	static ZEString Name = "GBuffer";
	return Name;
}

const ZEGRRenderState& ZERNStageGBuffer::GetRenderState()
{
	static ZEGRRenderState RenderState;
	bool Initialized = false;
	if (!Initialized)
	{
		RenderState = ZERNStage::GetRenderState();

		ZEGRDepthStencilState DepthStencilState;
		DepthStencilState.SetDepthTestEnable(true);
		DepthStencilState.SetDepthWriteEnable(true);
		RenderState.SetDepthStencilState(DepthStencilState);

		ZEGRBlendState BlendState;

		RenderState.SetRenderTargetFormat(0, ZEGR_TF_R10G10B10A2_UINT);
		RenderState.SetRenderTargetFormat(1, ZEGR_TF_R8G8B8A8_UNORM);
		RenderState.SetRenderTargetFormat(2, ZEGR_TF_R8G8B8A8_UNORM);
		RenderState.SetRenderTargetFormat(3, ZEGR_TF_R8G8B8A8_UNORM);
	}

	return RenderState;
}

bool ZERNStageGBuffer::Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands)
{
	ZEGROutput* Output = Renderer->GetOutput();
	ZEGRRenderTarget* OutputRenderTarget = Output->GetRenderTarget();

	UpdateRenderTargets(Renderer);

	Context->SetViewports(1, &Viewport);
	Context->ClearRenderTarget(OutputRenderTarget, ZEVector4::One);
	Context->ClearRenderTarget(GBuffer1->GetRenderTarget(0), ZEVector4::Zero);
	Context->ClearRenderTarget(GBuffer2->GetRenderTarget(0), ZEVector4::Zero);
	Context->ClearRenderTarget(GBuffer3->GetRenderTarget(0), ZEVector4::Zero);
	Context->ClearDepthStencilBuffer(DepthStencilBuffer, true, true, 1.0f, 0x00);

	Context->SetRenderTargetCount(4);
	Context->SetRenderTarget(0, OutputRenderTarget);
	Context->SetRenderTarget(1, GBuffer1->GetRenderTarget(0));
	Context->SetRenderTarget(2, GBuffer2->GetRenderTarget(0));
	Context->SetRenderTarget(3, GBuffer3->GetRenderTarget(0));
	Context->SetDepthStencilBuffer(DepthStencilBuffer);

	return true;
}
