//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStagePostProcess.cpp
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

#include "ZERNStagePostProcess.h"

#include "ZERNRenderer.h"
#include "ZERNStageID.h"
#include "ZERNStageGBuffer.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRRenderTarget.h"

void ZERNStagePostProcess::DeinitializeSelf()
{
	OutputTexture.Release();

	ZERNStage::Deinitialize();
}

ZEInt ZERNStagePostProcess::GetId() const
{
	return ZERN_STAGE_POST_EFFECT;
}

const ZEString& ZERNStagePostProcess::GetName() const
{
	static const ZEString Name = "Post Process Stage";
	return Name;
}

bool ZERNStagePostProcess::Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands)
{
	if (!ZERNStage::Setup(Renderer, Context, Commands))
		return false;

	ZERNStageGBuffer* StageGBuffer = static_cast<ZERNStageGBuffer*>(Renderer->GetStage(ZERN_STAGE_GBUFFER));
	if(StageGBuffer == NULL)
		return false;

	Context->SetTexture(ZEGR_ST_PIXEL, 0, StageGBuffer->GetDepthMap());
	Context->SetTexture(ZEGR_ST_PIXEL, 1, StageGBuffer->GetAccumulationMap());
	Context->SetTexture(ZEGR_ST_PIXEL, 2, StageGBuffer->GetNormalMap());

	const ZEGRRenderTarget* RenderTarget = Renderer->GetOutputRenderTarget();
	if(RenderTarget == NULL)
		return false;

	ZEUInt Width = RenderTarget->GetWidth();
	ZEUInt Height = RenderTarget->GetHeight();

	if(OutputTexture == NULL || OutputTexture->GetWidth() != Width || OutputTexture->GetHeight() != Height)
	{
		OutputTexture.Release();
		OutputTexture = ZEGRTexture2D::CreateInstance(Width, Height, 1, 1, 1, ZEGR_TF_R11G11B10_FLOAT, true);
	}

	RenderTarget = OutputTexture->GetRenderTarget();

	Context->SetRenderTargets(1, &RenderTarget, NULL);

	return true;
}

void ZERNStagePostProcess::CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 1, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, NULL);
	Context->SetRenderTargets(0, NULL, NULL);

	ZERNStage::CleanUp(Renderer, Context);
}

ZEGRTexture2D* ZERNStagePostProcess::GetOutputTexture() const
{
	return OutputTexture;
}

ZERNStagePostProcess::ZERNStagePostProcess()
{

}

ZEGRRenderState ZERNStagePostProcess::GetRenderState()
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

		RenderState.SetDepthStencilFormat(ZEGR_TF_D24_UNORM_S8_UINT);
		RenderState.SetRenderTargetFormat(0, ZEGR_TF_R11G11B10_FLOAT);
	}

	return RenderState;
}
