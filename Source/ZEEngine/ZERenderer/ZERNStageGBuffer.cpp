//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageGBuffer.cpp
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

#include "ZERNStageGBuffer.h"

#include "ZERNRenderer.h"
#include "ZERNStageID.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"

#define ZERN_SGDF_OUTPUT0		1
#define ZERN_SGDF_OUTPUT1		2
#define ZERN_SGDF_OUTPUT2		4
#define ZERN_SGDF_OUTPUT3		8
#define ZERN_SGDF_OUTPUT4		16
#define ZERN_SGDF_OUTPUT_ALL	(ZERN_SGDF_OUTPUT0 | ZERN_SGDF_OUTPUT1 | ZERN_SGDF_OUTPUT2 | ZERN_SGDF_OUTPUT3 | ZERN_SGDF_OUTPUT4)

bool ZERNStageGBuffer::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;

	return true;
}

bool ZERNStageGBuffer::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	GBuffer0.Release();
	GBuffer1.Release();
	GBuffer2.Release();
	GBuffer3.Release();
	DepthStencilBuffer.Release();

	return ZERNStage::DeinitializeInternal();
}

void ZERNStageGBuffer::CreateOutput(const ZEString& Name)
{
	ZEUInt Width = GetRenderer()->GetOutputTexture()->GetWidth();
	ZEUInt Height = GetRenderer()->GetOutputTexture()->GetHeight();

	if (Name == "ColorTexture")
	{
		if (DirtyFlags.GetFlags(ZERN_SGDF_OUTPUT0))
		{
			GBuffer0 = ZEGRTexture2D::CreateResource(Width, Height, 1, ZEGR_TF_R11G11B10_FLOAT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 1, ZEGRGraphicsModule::SAMPLE_COUNT);
			DirtyFlags.UnraiseFlags(ZERN_SGDF_OUTPUT0);
		}
	}
	else if (Name == "GBufferEmissive")
	{
		if (DirtyFlags.GetFlags(ZERN_SGDF_OUTPUT1))
		{
			GBuffer1 = ZEGRTexture2D::CreateResource(Width, Height, 1, ZEGR_TF_R8G8B8A8_UNORM, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 1, ZEGRGraphicsModule::SAMPLE_COUNT);
			DirtyFlags.UnraiseFlags(ZERN_SGDF_OUTPUT1);
		}
	}
	else if (Name == "GBufferDiffuse")
	{
		if (DirtyFlags.GetFlags(ZERN_SGDF_OUTPUT2))
		{
			GBuffer2 = ZEGRTexture2D::CreateResource(Width, Height, 1, ZEGR_TF_R8G8B8A8_UNORM, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 1, ZEGRGraphicsModule::SAMPLE_COUNT);
			DirtyFlags.UnraiseFlags(ZERN_SGDF_OUTPUT2);
		}
	}
	else if (Name == "GBufferNormal")
	{
		if (DirtyFlags.GetFlags(ZERN_SGDF_OUTPUT3))
		{
			GBuffer3 = ZEGRTexture2D::CreateResource(Width, Height, 1, ZEGR_TF_R8G8B8A8_UNORM, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 1, ZEGRGraphicsModule::SAMPLE_COUNT);
			DirtyFlags.UnraiseFlags(ZERN_SGDF_OUTPUT3);
		}
	}
	else if (Name == "DepthTexture")
	{
		if (DirtyFlags.GetFlags(ZERN_SGDF_OUTPUT4))
		{
			DepthStencilBuffer = ZEGRTexture2D::CreateResource(Width, Height, 1, ZEGR_TF_D24_UNORM_S8_UINT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_DEPTH_STENCIL, 1, ZEGRGraphicsModule::SAMPLE_COUNT);
			DirtyFlags.UnraiseFlags(ZERN_SGDF_OUTPUT4);
		}
	}
}

ZEInt ZERNStageGBuffer::GetId() const
{
	return ZERN_STAGE_GBUFFER;
}

const ZEString& ZERNStageGBuffer::GetName() const
{
	static const ZEString Name = "Stage GBuffer";
	return Name;
}

void ZERNStageGBuffer::Resized(ZEUInt Width, ZEUInt Height)
{
	Viewport.SetWidth((float)Width);
	Viewport.SetHeight((float)Height);

	DirtyFlags.RaiseFlags(ZERN_SGDF_OUTPUT_ALL);
}

bool ZERNStageGBuffer::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;
	
	const ZEGRRenderTarget* RenderTargets[] = {GBuffer0->GetRenderTarget(), GBuffer1->GetRenderTarget(), GBuffer2->GetRenderTarget(), GBuffer3->GetRenderTarget()};

	Context->ClearRenderTarget(RenderTargets[0], ZEVector4::Zero);
	Context->ClearRenderTarget(RenderTargets[1], ZEVector4::Zero);
	Context->ClearRenderTarget(RenderTargets[2], ZEVector4::Zero);
	Context->ClearRenderTarget(RenderTargets[3], ZEVector4::Zero);
	const ZEGRDepthStencilBuffer* DepthStencil = DepthStencilBuffer->GetDepthStencilBuffer();
	Context->ClearDepthStencilBuffer(DepthStencil, true, true, 0.0f, 0x00);

	Context->SetViewports(1, &Viewport);
	Context->SetRenderTargets(4, RenderTargets, DepthStencil);

	return true;
}

void ZERNStageGBuffer::CleanUp(ZEGRContext* Context)
{
	ZERNStage::CleanUp(Context);
}

ZERNStageGBuffer::ZERNStageGBuffer()
{
	DirtyFlags.RaiseAll();

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&GBuffer0), "ColorTexture", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN);
	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&GBuffer1), "GBufferEmissive", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN);
	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&GBuffer2), "GBufferDiffuse", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN);
	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&GBuffer3), "GBufferNormal", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN);
	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&DepthStencilBuffer), "DepthTexture", ZERN_SRUT_WRITE, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_CREATE_OWN);
}

ZEGRRenderState ZERNStageGBuffer::GetRenderState()
{
	static ZEGRRenderState RenderState;
	static bool Initialized = false;
	if (!Initialized)
	{
		Initialized = true;

		RenderState.SetRenderTargetFormat(0, ZEGR_TF_R11G11B10_FLOAT);
		RenderState.SetRenderTargetFormat(1, ZEGR_TF_R8G8B8A8_UNORM);
		RenderState.SetRenderTargetFormat(2, ZEGR_TF_R8G8B8A8_UNORM);
		RenderState.SetRenderTargetFormat(3, ZEGR_TF_R8G8B8A8_UNORM);

		RenderState.SetDepthStencilFormat(ZEGR_TF_D24_UNORM_S8_UINT);
	}

	return RenderState;
}
