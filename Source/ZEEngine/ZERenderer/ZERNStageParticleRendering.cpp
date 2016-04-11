//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageParticleRendering.cpp
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

#include "ZERNStageParticleRendering.h"

#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNCommand.h"

ZEInt ZERNStageParticleRendering::GetId() const
{
	return ZERN_STAGE_PARTICLE_RENDERING;
}

const ZEString& ZERNStageParticleRendering::GetName() const
{
	static const ZEString Name = "Stage particle rendering";
	return Name;
}

bool ZERNStageParticleRendering::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	const ZEGRTexture2D* ColorMap = GetPrevOutput(ZERN_SO_COLOR);
	if (ColorMap == NULL)
		return false;

	const ZEGRTexture2D* DepthMap = GetPrevOutput(ZERN_SO_DEPTH);
	if (DepthMap == NULL)
		return false;

	const ZEGRRenderTarget* RenderTarget = ColorMap->GetRenderTarget();
	
	Viewport.SetWidth((float)RenderTarget->GetWidth());
	Viewport.SetHeight((float)RenderTarget->GetHeight());

	Context->SetRenderTargets(1, &RenderTarget, DepthMap->GetDepthStencilBuffer(true));
	Context->SetVertexBuffers(0, 0, NULL);
	Context->SetViewports(1, &Viewport);

	return true;
}

void ZERNStageParticleRendering::CleanUp(ZEGRContext* Context)
{
	Context->SetRenderTargets(0, NULL, NULL);

	ZERNStage::CleanUp(Context);
}

ZERNStageParticleRendering::ZERNStageParticleRendering()
{

}

ZEGRRenderState ZERNStageParticleRendering::GetRenderState()
{
	static ZEGRRenderState RenderState;
	static bool Initialized = false;

	if(!Initialized)
	{
		Initialized = true;

		ZEGRBlendState BlendStateAlphaBlended;
		BlendStateAlphaBlended.SetBlendEnable(true);
		ZEGRBlendRenderTarget BlendRenderTargetAlphaBlended = BlendStateAlphaBlended.GetRenderTarget(0);
		BlendRenderTargetAlphaBlended.SetSource(ZEGR_BO_SRC_ALPHA);
		BlendRenderTargetAlphaBlended.SetDestination(ZEGR_BO_INV_SRC_ALPHA);
		BlendRenderTargetAlphaBlended.SetOperation(ZEGR_BE_ADD);
		BlendRenderTargetAlphaBlended.SetBlendEnable(true);
		BlendStateAlphaBlended.SetRenderTargetBlend(0, BlendRenderTargetAlphaBlended);
		RenderState.SetBlendState(BlendStateAlphaBlended);
		
		ZEGRRasterizerState RasterizerStateWireframe;
		RasterizerStateWireframe.SetFillMode(ZEGR_FM_WIREFRAME);
		//RenderState.SetRasterizerState(RasterizerStateWireframe);

		RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_STRIPT);
	}

	return RenderState;
}
