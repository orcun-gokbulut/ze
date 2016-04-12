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
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNCommand.h"

ZEInt ZERNStage2D::GetId() const
{
	return ZERN_STAGE_2D;
}

const ZEString& ZERNStage2D::GetName() const
{
	static const ZEString Name = "2D";
	return Name;
}

bool ZERNStage2D::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (GetCommands().GetCount() == 0)
		return false;

	const ZEGRRenderTarget* RenderTarget = GetNextProvidedInput(ZERN_SO_COLOR);
	if (RenderTarget == NULL)
	{
		const ZEGRTexture2D* ColorOutput = GetPrevOutput(ZERN_SO_COLOR);
		if (ColorOutput == NULL)
			return false;

		RenderTarget = ColorOutput->GetRenderTarget();
	}

	if (RenderTarget == NULL)
		return false;

	Viewport.SetWidth((float)RenderTarget->GetWidth());
	Viewport.SetHeight((float)RenderTarget->GetHeight());

	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetVertexBuffers(0, 0, NULL);
	Context->SetViewports(1, &Viewport);

	return true;
}

void ZERNStage2D::CleanUp(ZEGRContext* Context)
{
	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetRenderTargets(0, NULL, NULL);

	ZERNStage::CleanUp(Context);
}

ZERNStage2D::ZERNStage2D()
{

}

ZEGRRenderState ZERNStage2D::GetRenderState()
{
	static ZEGRRenderState RenderState;
	static bool Initialized = false;

	if(!Initialized)
	{
		Initialized = true;

		RenderState.SetDepthStencilFormat(ZEGR_TF_NONE);
		RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);

		// Alpha Blending
		ZEGRBlendState BlendingState;
		BlendingState.SetBlendEnable(true);
		ZEGRBlendRenderTarget RenderTargetBlend;
		RenderTargetBlend.SetBlendEnable(true);
		RenderTargetBlend.SetSource(ZEGR_BO_SRC_ALPHA);
		RenderTargetBlend.SetDestination(ZEGR_BO_INV_SRC_ALPHA);
		BlendingState.SetRenderTargetBlend(0, RenderTargetBlend);
 		RenderState.SetBlendState(BlendingState);

		// Depth Stencil
		ZEGRDepthStencilState DepthStencilState;
		DepthStencilState.SetDepthTestEnable(false);
		DepthStencilState.SetDepthWriteEnable(false);
		RenderState.SetDepthStencilState(DepthStencilState);

		RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	}

	return RenderState;
}
