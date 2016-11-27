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

#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZERNRenderer.h"
#include "ZERNStageID.h"

#define ZERN_SPDF_OUTPUT	1

bool ZERNStagePostProcess::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;

	return true;
}

bool ZERNStagePostProcess::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	ColorTexture = NULL;
	DepthTexture = NULL;

	return ZERNStage::DeinitializeInternal();
}

void ZERNStagePostProcess::CreateOutput(const ZEString& Name)
{
	ZEUInt Width = GetRenderer()->GetOutputTexture()->GetWidth();
	ZEUInt Height = GetRenderer()->GetOutputTexture()->GetHeight();

	if (Name == "ColorTexture")
	{
		if (DirtyFlags.GetFlags(ZERN_SPDF_OUTPUT))
		{
			ColorTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, ZEGR_TF_R11G11B10_FLOAT, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 1, ZEGRGraphicsModule::SAMPLE_COUNT).GetPointer();
			DirtyFlags.UnraiseFlags(ZERN_SPDF_OUTPUT);
		}
	}
}

ZEInt ZERNStagePostProcess::GetId() const
{
	return ZERN_STAGE_POST_EFFECT;
}

const ZEString& ZERNStagePostProcess::GetName() const
{
	static const ZEString Name = "Stage post process";
	return Name;
}

void ZERNStagePostProcess::Resized(ZEUInt Width, ZEUInt Height)
{
	DirtyFlags.RaiseFlags(ZERN_SPDF_OUTPUT);
}

bool ZERNStagePostProcess::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (GetCommands().GetCount() == 0)
		return false;

	Context->SetTexture(ZEGR_ST_PIXEL, 4, DepthTexture);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, (float)ColorTexture->GetWidth(), (float)ColorTexture->GetHeight()));

	return true;
}

void ZERNStagePostProcess::CleanUp(ZEGRContext* Context)
{
	ZERNStage::CleanUp(Context);
}

ZERNStagePostProcess::ZERNStagePostProcess()
{
	DirtyFlags.RaiseAll();

	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&DepthTexture), "DepthTexture", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&ColorTexture), "ColorTexture", ZERN_SRUT_WRITE, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_CREATE_OWN | ZERN_SRCF_GET_OUTPUT);
}

ZERNStagePostProcess::~ZERNStagePostProcess()
{
	Deinitialize();
}

ZEGRRenderState ZERNStagePostProcess::GetRenderState()
{
	static ZEGRRenderState RenderState;
	static bool Initialized = false;

	if (!Initialized)
	{
		Initialized = true;

		ZEGRDepthStencilState DepthStencilStateNoTestWrite;
		DepthStencilStateNoTestWrite.SetDepthTestEnable(false);
		DepthStencilStateNoTestWrite.SetDepthWriteEnable(false);

		RenderState.SetDepthStencilState(DepthStencilStateNoTestWrite);

		RenderState.SetDepthStencilFormat(ZEGR_TF_D24_UNORM_S8_UINT);
		RenderState.SetRenderTargetFormat(0, ZEGR_TF_R11G11B10_FLOAT);
	}

	return RenderState;
}
