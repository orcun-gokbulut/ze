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
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"

void ZERNStageForward::DeinitializeSelf()
{
	ColorBuffer.Release();
	ColorRenderTarget.Release();
	DepthStencilBuffer.Release();
}

bool ZERNStageForward::UpdateRenderTargets()
{
	BindOutput(ZERN_SO_COLOR, ZEGR_TF_R11G11B10_FLOAT, true, ColorBuffer, ColorRenderTarget);
	BindDepthOutput(ZERN_SO_DEPTH, ZEGR_TF_D24_UNORM_S8_UINT, true, DepthBuffer, DepthStencilBuffer);

	Viewport.SetWidth((float)ColorRenderTarget->GetWidth());
	Viewport.SetHeight((float)ColorRenderTarget->GetHeight());

	return true;
}

bool ZERNStageForward::Update()
{
	if (!UpdateRenderTargets())
		return false;

	return true;
}

ZEInt ZERNStageForward::GetId() const
{
	return ZERN_STAGE_FORWARD;
}

const ZEString& ZERNStageForward::GetName() const
{
	static ZEString Name = "Forward";

	return Name;
}

const ZEGRRenderTarget*	ZERNStageForward::GetProvidedInput(ZERNStageBuffer Input) const
{
	if (GetEnabled() && Input == ZERN_SO_COLOR)
		return ColorRenderTarget;
	
	return ZERNStage::GetProvidedInput(Input);
}

const ZEGRTexture2D* ZERNStageForward::GetOutput(ZERNStageBuffer Output) const
{
	if (GetEnabled())
	{
		switch (Output)
		{
			case ZERN_SO_COLOR:
			case ZERN_SO_ACCUMULATION:
				return ColorBuffer;

			case ZERN_SO_DEPTH:
				return DepthBuffer;
		}
	}

	return ZERNStage::GetOutput(Output);
}


bool ZERNStageForward::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!Update())
		return false;

	Context->SetRenderTargets(1, ColorRenderTarget.GetPointerToPointer(), DepthStencilBuffer);
	Context->SetViewports(1, &Viewport);

	return true;
}

void ZERNStageForward::CleanUp(ZEGRContext* Context)
{
	Context->SetRenderTargets(0, NULL, NULL);

	ZERNStage::CleanUp(Context);
}

ZERNStageForward::ZERNStageForward()
{

}

ZEGRRenderState ZERNStageForward::GetRenderState()
{
	static ZEGRRenderState RenderState;
	static bool Initialized = false;
	if (!Initialized)
	{
		Initialized = true;

		RenderState.SetRenderTargetFormat(0, ZEGR_TF_R8G8B8A8_UNORM);
		RenderState.SetDepthStencilFormat(ZEGR_TF_D24_UNORM_S8_UINT);
	}

	return RenderState;
}

ZEInt ZERNStageForwardTransparent::GetId() const
{
	return ZERN_STAGE_FORWARD_TRANSPARENT;
}

const ZEString& ZERNStageForwardTransparent::GetName() const
{
	static ZEString Name = "ForwardTransparent";
	return Name;
}

ZERNStageForwardTransparent::ZERNStageForwardTransparent()
{
}

ZEGRRenderState ZERNStageForwardTransparent::GetRenderState()
{
	return ZERNStageForward::GetRenderState();
}


ZEInt ZERNStageForwardPostHDR::GetId() const
{
	return ZERN_STAGE_FORWARD_POST;
}

const ZEString& ZERNStageForwardPostHDR::GetName() const
{
	static ZEString Name = "ForwardPostHDR";
	return Name;
}

ZERNStageForwardPostHDR::ZERNStageForwardPostHDR()
{
}

ZEGRRenderState ZERNStageForwardPostHDR::GetRenderState()
{
	return ZERNStageForward::GetRenderState();
}
