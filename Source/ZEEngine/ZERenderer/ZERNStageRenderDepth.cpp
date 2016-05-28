//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageRenderDepth.cpp
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

#include "ZERNStageRenderDepth.h"

#include "ZERNStageID.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"

bool ZERNStageRenderDepth::InitializeSelf()
{
	if (!ZERNStage::InitializeSelf())
		return false;

	return UpdateInputOutputs();
}

void ZERNStageRenderDepth::DeinitializeSelf()
{
	DepthBuffer = NULL;

	ZERNStage::DeinitializeSelf();
}

bool ZERNStageRenderDepth::UpdateInputOutputs()
{
	DepthBuffer = GetPrevOutput(ZERN_SO_DEPTH);
	if (DepthBuffer == NULL)
		return false;

	return true;
}

ZEInt ZERNStageRenderDepth::GetId() const
{
	return ZERN_STAGE_RENDER_DEPTH;
}

const ZEString& ZERNStageRenderDepth::GetName() const
{
	static const ZEString Name = "Stage render depth";
	return Name;
}

bool ZERNStageRenderDepth::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!UpdateInputOutputs())
		return false;

	if (GetCommands().GetCount() == 0)
		return false;

	Context->SetRenderTargets(0, NULL, DepthBuffer->GetDepthStencilBuffer());
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, (float)DepthBuffer->GetWidth(), (float)DepthBuffer->GetHeight()));

	return true;
}

void ZERNStageRenderDepth::CleanUp(ZEGRContext* Context)
{
	ZERNStage::CleanUp(Context);
}

ZERNStageRenderDepth::ZERNStageRenderDepth()
{
	DepthBuffer = NULL;
}

ZEGRRenderState ZERNStageRenderDepth::GetRenderState()
{
	static ZEGRRenderState RenderState;
	static bool Initialized = false;

	if(!Initialized)
	{
		Initialized = true;

		RenderState.SetDepthStencilFormat(ZEGR_TF_D24_UNORM_S8_UINT);
		RenderState.SetRenderTargetFormat(0, ZEGR_TF_NONE);
	}

	return RenderState;
}
