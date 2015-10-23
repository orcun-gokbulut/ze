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

#define ZERN_GSDF_RENDER_TARGET		1

bool ZERNStageGBuffer::InitializeSelf()
{
	DirtyFlags.RaiseAll();

	return true;
}

void ZERNStageGBuffer::DeinitializeSelf()
{
	DepthStencilBuffer.Release();
	GBuffer0.Release();
	GBuffer1.Release();
	GBuffer2.Release();
	GBuffer3.Release();
}

bool ZERNStageGBuffer::UpdateRenderTargets(ZEUInt Width, ZEUInt Height)
{
	if(!DirtyFlags.GetFlags(ZERN_GSDF_RENDER_TARGET))
		return true;

	DeinitializeSelf();

	DepthStencilBuffer	= ZEGRDepthStencilBuffer::Create(Width, Height, ZEGR_DSF_DEPTH24_STENCIL8, true);
	GBuffer0 = ZEGRTexture2D::CreateInstance(Width, Height, 1, ZEGR_TF_R11FG11FB10F_FLOAT, true);
	GBuffer1 = ZEGRTexture2D::CreateInstance(Width, Height, 1, ZEGR_TF_R8G8B8A8_UNORM, true);
	GBuffer2 = ZEGRTexture2D::CreateInstance(Width, Height, 1, ZEGR_TF_R8G8B8A8_UNORM, true);
	GBuffer3 = ZEGRTexture2D::CreateInstance(Width, Height, 1, ZEGR_TF_R8G8B8A8_UNORM, true);

	RenderTargets[0] = GBuffer0->GetRenderTarget();
	RenderTargets[1] = GBuffer1->GetRenderTarget();
	RenderTargets[2] = GBuffer2->GetRenderTarget();
	RenderTargets[3] = GBuffer3->GetRenderTarget();

	DirtyFlags.UnraiseFlags(ZERN_GSDF_RENDER_TARGET);

	return true;
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
	static bool Initialized = false;
	if (!Initialized)
	{
		Initialized = true;

		RenderState = ZERNStage::GetRenderState();

		ZEGRDepthStencilState DepthStencilState;
		DepthStencilState.SetDepthTestEnable(true);
		DepthStencilState.SetDepthWriteEnable(true);
		RenderState.SetDepthStencilState(DepthStencilState);

		ZEGRBlendState BlendState;
		BlendState.SetBlendEnable(false);

		RenderState.SetBlendState(BlendState);

		ZEGRRasterizerState Rasterizer;
		Rasterizer.SetFillMode(ZEGR_FM_SOLID);
		Rasterizer.SetFrontIsCounterClockwise(false);
		Rasterizer.SetCullDirection(ZEGR_CD_COUNTER_CLOCKWISE);

		RenderState.SetRasterizerState(Rasterizer);

		RenderState.SetRenderTargetFormat(0, ZEGR_TF_R11FG11FB10F_FLOAT);
		RenderState.SetRenderTargetFormat(1, ZEGR_TF_R8G8B8A8_UNORM);
		RenderState.SetRenderTargetFormat(2, ZEGR_TF_R8G8B8A8_UNORM);
		RenderState.SetRenderTargetFormat(3, ZEGR_TF_R8G8B8A8_UNORM);
	}

	return RenderState;
}

bool ZERNStageGBuffer::Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands)
{
	ZEGROutput* Output = Renderer->GetOutput();
	if(Output == NULL)
		return false;

	ZEGRRenderTarget* OutputRenderTarget = Output->GetRenderTarget();
	ZEGRViewport Viewport = Output->GetViewport();

	ZEUInt Width = OutputRenderTarget->GetWidth();
	ZEUInt Height = OutputRenderTarget->GetHeight();

	if (!GBuffer0.IsNull() && GBuffer0->GetWidth() != Width && GBuffer0->GetHeight() != Height)
		DirtyFlags.RaiseFlags(ZERN_GSDF_RENDER_TARGET);

	UpdateRenderTargets(Width, Height);

	Context->SetViewports(1, &Viewport);
	Context->ClearRenderTarget(GBuffer0->GetRenderTarget(), ZEVector4::Zero);
	Context->ClearRenderTarget(GBuffer1->GetRenderTarget(), ZEVector4::Zero);
	Context->ClearRenderTarget(GBuffer2->GetRenderTarget(), ZEVector4::Zero);
	Context->ClearRenderTarget(GBuffer3->GetRenderTarget(), ZEVector4::Zero);
	Context->ClearDepthStencilBuffer(DepthStencilBuffer, true, true, 1.0f, 0x00);

	Context->SetRenderTargets(4, RenderTargets, DepthStencilBuffer);

	return true;
}

void ZERNStageGBuffer::CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	Context->SetRenderTargets(0, NULL, NULL);
}

ZEGRTexture2D* ZERNStageGBuffer::GetDiffuseColorMap() const
{
	return GBuffer2;
}

ZEGRTexture2D* ZERNStageGBuffer::GetSpecularColorMap() const
{
	return GBuffer3;
}

ZEGRTexture2D* ZERNStageGBuffer::GetAccumulationMap() const
{
	return GBuffer0;
}

ZEGRDepthStencilBuffer* ZERNStageGBuffer::GetDepthStencilBuffer() const
{
	return DepthStencilBuffer;
}
