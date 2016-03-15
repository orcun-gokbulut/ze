//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStagePreProcess.cpp
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

#include "ZERNStagePreProcess.h"

#include "ZERNRenderer.h"
#include "ZERNStageID.h"
#include "ZERNStageGBuffer.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"

ZEInt ZERNStagePreProcess::GetId() const
{
	return ZERN_STAGE_PRE_EFFECT;
}

const ZEString& ZERNStagePreProcess::GetName() const
{
	static const ZEString Name = "Pre Process Stage";
	return Name;
}

bool ZERNStagePreProcess::Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands)
{
	if (!ZERNStage::Setup(Renderer, Context, Commands))
		return false;

	ZERNStageGBuffer* StageGBuffer = static_cast<ZERNStageGBuffer*>(Renderer->GetStage(ZERN_STAGE_GBUFFER));
	if (StageGBuffer == NULL)
		return false;

	ZEGRTexture2D* AccumulationMap = StageGBuffer->GetAccumulationMap();
	if (AccumulationMap == NULL)
		return false;

	ZEGRTexture2D* DepthMap = StageGBuffer->GetDepthMap();
	if (DepthMap == NULL)
		return false;

	const ZEGRRenderTarget* RenderTarget = AccumulationMap->GetRenderTarget();

	Context->SetTexture(ZEGR_ST_PIXEL, 2, StageGBuffer->GetNormalMap());
	Context->SetTexture(ZEGR_ST_PIXEL, 3, StageGBuffer->GetDiffuseColorMap());
	Context->SetTexture(ZEGR_ST_PIXEL, 4, StageGBuffer->GetSpecularColorMap());

	Context->SetRenderTargets(1, &RenderTarget, DepthMap->GetDepthStencilBuffer());
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, RenderTarget->GetWidth(), RenderTarget->GetHeight()));

	return true;
}

void ZERNStagePreProcess::CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	Context->SetRenderTargets(0, NULL, NULL);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 3, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 4, NULL);

	ZERNStage::CleanUp(Renderer, Context);
}

ZERNStagePreProcess::ZERNStagePreProcess()
{

}

ZEGRRenderState ZERNStagePreProcess::GetRenderState()
{
	static ZEGRRenderState RenderState;
	static bool Initialized = false;

	if(!Initialized)
	{
		Initialized = true;

		ZEGRDepthStencilState DepthStencilState;
		DepthStencilState.SetDepthTestEnable(false);
		DepthStencilState.SetDepthWriteEnable(false);

		RenderState.SetDepthStencilState(DepthStencilState);

		RenderState.SetRenderTargetFormat(0, ZEGR_TF_R11G11B10_FLOAT);
	}

	return RenderState;
}
