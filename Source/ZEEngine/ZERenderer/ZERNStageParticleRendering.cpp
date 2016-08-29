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
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNCommand.h"

#define ZERN_SPDF_OUTPUT	1

bool ZERNStageParticleRendering::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;

	return true;
}

bool ZERNStageParticleRendering::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	AccumulationTexture = NULL;
	DepthTexture = NULL;

	return ZERNStage::DeinitializeInternal();
}

void ZERNStageParticleRendering::CreateOutput(const ZEString& Name)
{
	ZEUInt Width = GetRenderer()->GetOutputTexture()->GetWidth();
	ZEUInt Height = GetRenderer()->GetOutputTexture()->GetHeight();

	if (Name == "ColorTexture")
	{
		if (DirtyFlags.GetFlags(ZERN_SPDF_OUTPUT))
		{
			AccumulationTexture = ZEGRTexture2D::CreateResource(Width, Height, 1, ZEGR_TF_R11G11B10_FLOAT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 1, ZEGRGraphicsModule::SAMPLE_COUNT).GetPointer();
			DirtyFlags.UnraiseFlags(ZERN_SPDF_OUTPUT);
		}
	}
}

ZEInt ZERNStageParticleRendering::GetId() const
{
	return ZERN_STAGE_PARTICLE_RENDERING;
}

const ZEString& ZERNStageParticleRendering::GetName() const
{
	static const ZEString Name = "Stage particle rendering";
	return Name;
}

void ZERNStageParticleRendering::Resized(ZEUInt Width, ZEUInt Height)
{
	Viewport.SetWidth((float)Width);
	Viewport.SetHeight((float)Height);

	DirtyFlags.RaiseFlags(ZERN_SPDF_OUTPUT);
}

bool ZERNStageParticleRendering::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (GetCommands().GetCount() == 0)
		return false;

	const ZEGRRenderTarget* RenderTarget = AccumulationTexture->GetRenderTarget();

	Context->SetRenderTargets(1, &RenderTarget, DepthTexture->GetDepthStencilBuffer(true));
	Context->SetViewports(1, &Viewport);

	return true;
}

void ZERNStageParticleRendering::CleanUp(ZEGRContext* Context)
{
	ZERNStage::CleanUp(Context);
}

ZERNStageParticleRendering::ZERNStageParticleRendering()
{
	DirtyFlags.RaiseAll();

	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&DepthTexture), "DepthTexture", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&AccumulationTexture), "ColorTexture", ZERN_SRUT_WRITE, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_CREATE_OWN | ZERN_SRCF_GET_OUTPUT);
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

		RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_STRIPT);
	}

	return RenderState;
}
