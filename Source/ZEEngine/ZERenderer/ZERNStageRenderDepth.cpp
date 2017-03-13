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

#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZERNStageID.h"
#include "ZERNRenderer.h"
#include "ZERNRenderParameters.h"
#include "ZERNCommand.h"

#define ZERN_SRDF_OUTPUT	1

bool ZERNStageRenderDepth::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;

	return true;
}

bool ZERNStageRenderDepth::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	DepthTexture = NULL;

	return ZERNStage::DeinitializeInternal();
}

void ZERNStageRenderDepth::CreateOutput(const ZEString& Name)
{
	ZEUInt Width = GetRenderer()->GetOutputTexture()->GetWidth();
	ZEUInt Height = GetRenderer()->GetOutputTexture()->GetHeight();

	if (Name == "DepthTexture")
	{
		if (DirtyFlags.GetFlags(ZERN_SRDF_OUTPUT))
		{
			DepthTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, ZEGR_TF_D24_UNORM_S8_UINT, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_DEPTH_STENCIL, 1, ZEGRGraphicsModule::SAMPLE_COUNT);
			DirtyFlags.UnraiseFlags(ZERN_SRDF_OUTPUT);
		}
	}
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

void ZERNStageRenderDepth::Resized(ZEUInt Width, ZEUInt Height)
{
	DirtyFlags.RaiseFlags(ZERN_SRDF_OUTPUT);
}

bool ZERNStageRenderDepth::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	ze_for_each(Command, GetCommands())
	{
		if (Command->StageMask & ZERN_STAGE_FORWARD_TRANSPARENT)
			TransparentCommands.Add(Command.GetPointer());
		else
			OpaqueCommands.Add(Command.GetPointer());
	}

	Context->ClearDepthStencilBuffer(DepthTexture->GetDepthStencilBuffer(), true, true, 0.0f, 0x00);
	Context->SetRenderTargets(0, NULL, DepthTexture->GetDepthStencilBuffer());
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, (float)DepthTexture->GetWidth(), (float)DepthTexture->GetHeight()));
	
	ZERNRenderParameters Parameters;
	Parameters.Context = Context;
	Parameters.View = &GetRenderer()->GetView();
	Parameters.Renderer = GetRenderer();
	Parameters.Type = ZERN_RT_COLOR;
	Parameters.Stage = this;


	ze_for_each(Command, OpaqueCommands)
		Command.GetItem()->Execute(&Parameters);

	Context->SetStencilRef(1);
	ze_for_each(Command, TransparentCommands)
		Command.GetItem()->Execute(&Parameters);

	return false;
}

void ZERNStageRenderDepth::CleanUp(ZEGRContext* Context)
{
	OpaqueCommands.Clear();
	TransparentCommands.Clear();

	ZERNStage::CleanUp(Context);
}

ZERNStageRenderDepth::ZERNStageRenderDepth()
{
	DirtyFlags.RaiseAll();

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&DepthTexture), "DepthTexture", ZERN_SRUT_WRITE, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED | ZERN_SRCF_CREATE_OWN);
}

ZERNStageRenderDepth::~ZERNStageRenderDepth()
{
	Deinitialize();
}

ZEGRRenderState ZERNStageRenderDepth::GetRenderState()
{
	static ZEGRRenderState RenderState;
	static bool Initialized = false;

	if (!Initialized)
	{
		Initialized = true;

		RenderState.SetDepthStencilFormat(ZEGR_TF_D24_UNORM_S8_UINT);
		RenderState.SetRenderTargetFormat(0, ZEGR_TF_NONE);
	}

	return RenderState;
}
