//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageForward.cpp
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

#include "ZERNStageForward.h"
#include "ZERNRenderer.h"
#include "ZERNStageID.h"
#include "ZEGraphics/ZEGROutput.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRContext.h"

ZEInt ZERNStageForward::GetId()
{
	return ZERN_STAGE_FORWARD;
}

const ZEString& ZERNStageForward::GetName()
{
	static ZEString Name = "Forward";

	return Name;
}

const ZEGRRenderState& ZERNStageForward::GetRenderState()
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
		DepthStencilState.SetDepthFunction(ZEGR_CF_LESS);

		RenderState.SetDepthStencilState(DepthStencilState);

		ZEGRRasterizerState Rasterizer;
		Rasterizer.SetFillMode(ZEGR_FM_SOLID);
		Rasterizer.SetFrontIsCounterClockwise(false);
		Rasterizer.SetCullDirection(ZEGR_CD_COUNTER_CLOCKWISE);

		RenderState.SetRasterizerState(Rasterizer);

		ZEGRBlendState BlendState;
		BlendState.SetBlendEnable(false);

		RenderState.SetBlendState(BlendState);

		RenderState.SetRenderTargetFormat(0, ZEGR_TF_R8G8B8A8_UNORM);
	}

	return RenderState;
}

bool ZERNStageForward::InitializeSelf()
{
	return true;
}

bool ZERNStageForward::Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands)
{
	ZEGROutput* Output = Renderer->GetOutput();
	ZEGRRenderTarget* RenderTarget = Output->GetRenderTarget();
	ZEGRDepthStencilBuffer* DepthStencilBuffer = Output->GetDepthStencilBuffer();
	ZEGRViewport Viewport = Output->GetViewport();

	Context->ClearRenderTarget(RenderTarget, ZEVector4(0.5f, 0.5f, 0.5f, 1.0f));
	Context->ClearDepthStencilBuffer(DepthStencilBuffer, true, true, 1.0f, 0x00);

	Context->SetRenderTargets(1, &RenderTarget, DepthStencilBuffer);
	Context->SetViewports(1, &Viewport);

	return true;
}

void ZERNStageForward::CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	Context->SetRenderTargets(0, NULL, NULL);
}


ZEInt ZERNStageForwardTransparent::GetId()
{
	return ZERN_STAGE_FORWARD_TRANSPARANT;
}

const ZEString& ZERNStageForwardTransparent::GetName()
{
	static ZEString Name = "ForwardTransparent";
	return Name;
}

const ZEGRRenderState& ZERNStageForwardTransparent::GetRenderState()
{
	return ZERNStageForward::GetRenderState();
}
