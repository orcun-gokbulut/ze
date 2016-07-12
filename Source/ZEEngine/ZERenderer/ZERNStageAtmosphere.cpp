//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageAtmosphere.cpp
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

#include "ZERNStageAtmosphere.h"

#include "ZERNStageID.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"

bool ZERNStageAtmosphere::InitializeSelf()
{
	if (!ZERNStage::InitializeSelf())
		return false;

	return UpdateInputOutputs();
}

bool ZERNStageAtmosphere::DeinitializeSelf()
{
	AccumulationTexture = NULL;
	DepthTexture = NULL;

	return ZERNStage::DeinitializeSelf();
}

bool ZERNStageAtmosphere::UpdateInputOutputs()
{
	AccumulationTexture = GetPrevOutput(ZERN_SO_ACCUMULATION);
	if (AccumulationTexture == NULL)
		return false;

	DepthTexture = GetPrevOutput(ZERN_SO_DEPTH);
	if (DepthTexture == NULL)
		return false;

	return true;
}

ZEInt ZERNStageAtmosphere::GetId() const
{
	return ZERN_STAGE_ATMOSPHERE;
}

const ZEString& ZERNStageAtmosphere::GetName() const
{
	static const ZEString Name = "Stage atmosphere";
	return Name;
}

const ZEGRTexture2D* ZERNStageAtmosphere::GetOutput(ZERNStageBuffer Output) const
{
	if (GetEnabled() && (Output == ZERN_SO_COLOR || Output == ZERN_SO_ACCUMULATION))
		return AccumulationTexture;

	return ZERNStage::GetOutput(Output);
}

bool ZERNStageAtmosphere::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!UpdateInputOutputs())
		return false;

	if (GetCommands().GetCount() == 0)
		return false;

	const ZEGRRenderTarget* RenderTarget = AccumulationTexture->GetRenderTarget();
	if (RenderTarget == NULL)
		return false;

	const ZEGRDepthStencilBuffer* DepthBufferReadonly = DepthTexture->GetDepthStencilBuffer(true);
	if (DepthBufferReadonly == NULL)
		return false;

	Context->SetRenderTargets(1, &RenderTarget, DepthBufferReadonly);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, (float)DepthTexture->GetWidth(), (float)DepthTexture->GetHeight()));

	return true;
}

void ZERNStageAtmosphere::CleanUp(ZEGRContext* Context)
{
	ZERNStage::CleanUp(Context);
}

ZERNStageAtmosphere::ZERNStageAtmosphere()
{
	AccumulationTexture = NULL;
	DepthTexture = NULL;
}

ZEGRRenderState ZERNStageAtmosphere::GetRenderState()
{
	static ZEGRRenderState RenderState;
	static bool Initialized = false;

	if(!Initialized)
	{
		Initialized = true;

		ZEGRDepthStencilState DepthStencilStateTestNoWrite;
		DepthStencilStateTestNoWrite.SetDepthTestEnable(true);
		DepthStencilStateTestNoWrite.SetDepthWriteEnable(false);

		RenderState.SetDepthStencilState(DepthStencilStateTestNoWrite);

		RenderState.SetDepthStencilFormat(ZEGR_TF_D24_UNORM_S8_UINT);
		RenderState.SetRenderTargetFormat(0, ZEGR_TF_NONE);
	}

	return RenderState;
}
