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

#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZERNRenderer.h"
#include "ZERNStageID.h"

#define ZERN_SADF_OUTPUT	1

bool ZERNStageAtmosphere::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;

	return true;
}

bool ZERNStageAtmosphere::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	AccumulationTexture = NULL;
	DepthTexture = NULL;

	return ZERNStage::DeinitializeInternal();
}

void ZERNStageAtmosphere::CreateOutput(const ZEString& Name)
{
	ZEUInt Width = GetRenderer()->GetOutputTexture()->GetWidth();
	ZEUInt Height = GetRenderer()->GetOutputTexture()->GetHeight();

	if (Name == "ColorTexture")
	{
		if (DirtyFlags.GetFlags(ZERN_SADF_OUTPUT))
		{
			AccumulationTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, ZEGR_TF_R11G11B10_FLOAT, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 1, ZEGRGraphicsModule::SAMPLE_COUNT).GetPointer();
			DirtyFlags.UnraiseFlags(ZERN_SADF_OUTPUT);
		}
	}
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

void ZERNStageAtmosphere::Resized(ZEUInt Width, ZEUInt Height)
{
	DirtyFlags.RaiseFlags(ZERN_SADF_OUTPUT);
}

bool ZERNStageAtmosphere::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
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
	DirtyFlags.RaiseAll();

	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&DepthTexture), "DepthTexture", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&AccumulationTexture), "ColorTexture", ZERN_SRUT_WRITE, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_CREATE_OWN | ZERN_SRCF_GET_OUTPUT);
}

ZERNStageAtmosphere::~ZERNStageAtmosphere()
{
	Deinitialize();
}

ZEGRRenderState ZERNStageAtmosphere::GetRenderState()
{
	static ZEGRRenderState RenderState;
	static bool Initialized = false;

	if (!Initialized)
	{
		Initialized = true;

		ZEGRDepthStencilState DepthStencilStateTestNoWrite;
		DepthStencilStateTestNoWrite.SetDepthTestEnable(true);
		DepthStencilStateTestNoWrite.SetDepthWriteEnable(false);

		RenderState.SetDepthStencilState(DepthStencilStateTestNoWrite);

		RenderState.SetDepthStencilFormat(ZEGR_TF_D24_UNORM_S8_UINT);
		RenderState.SetRenderTargetFormat(0, ZEGR_TF_R11G11B10_FLOAT);
	}

	return RenderState;
}
