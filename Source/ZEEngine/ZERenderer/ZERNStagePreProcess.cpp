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

ZEInt ZERNStagePreProcess::GetId() const
{
	return ZERN_STAGE_PRE_EFFECT;
}

const ZEString& ZERNStagePreProcess::GetName() const
{
	static const ZEString Name = "Pre Process Stage";
	return Name;
}

bool ZERNStagePreProcess::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (GetCommands().GetCount() == 0)
		return false;

	const ZEGRTexture2D* AccumulationMap = GetPrevOutput(ZERN_SO_COLOR);
	if(AccumulationMap == NULL)
		return false;

	Context->SetTexture(ZEGR_ST_PIXEL, 0, GetPrevOutput(ZERN_SO_DEPTH));
	Context->SetTexture(ZEGR_ST_PIXEL, 2, GetPrevOutput(ZERN_SO_NORMAL));
	Context->SetTexture(ZEGR_ST_PIXEL, 3, GetPrevOutput(ZERN_SO_GBUFFER_DIFFUSE));
	Context->SetTexture(ZEGR_ST_PIXEL, 4, GetPrevOutput(ZERN_SO_GBUFFER_SPECULAR));

	const ZEGRRenderTarget* RenderTarget = AccumulationMap->GetRenderTarget();
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, (float)RenderTarget->GetWidth(), (float)RenderTarget->GetHeight()));

	return true;
}

void ZERNStagePreProcess::CleanUp(ZEGRContext* Context)
{
	Context->SetRenderTargets(0, NULL, NULL);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 3, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 4, NULL);

	ZERNStage::CleanUp(Context);
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
