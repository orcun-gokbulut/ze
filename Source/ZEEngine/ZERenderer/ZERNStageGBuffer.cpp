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
#include "ZEGraphics/ZEGROutput.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"

bool ZERNStageGBuffer::UpdateRenderTargets()
{
	const ZEGRRenderTarget* RenderTarget = GetRenderer()->GetOutputRenderTarget();
	if(RenderTarget == NULL)
		return false;

	ZEUInt Width = RenderTarget->GetWidth();
	ZEUInt Height = RenderTarget->GetHeight();

	if (GBuffer0 != NULL &&
		GBuffer0->GetWidth() == Width && GBuffer0->GetHeight() == Height)
	{
		return true;
	}

	Viewport.SetWidth((float)Width);
	Viewport.SetHeight((float)Height);

	GBuffer0 = ZEGRTexture2D::CreateInstance(Width, Height, 1, ZEGR_TF_R11G11B10_FLOAT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 1, ZEGRGraphicsModule::SAMPLE_COUNT);
	GBuffer1 = ZEGRTexture2D::CreateInstance(Width, Height, 1, ZEGR_TF_R8G8B8A8_UNORM, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 1, ZEGRGraphicsModule::SAMPLE_COUNT);
	GBuffer2 = ZEGRTexture2D::CreateInstance(Width, Height, 1, ZEGR_TF_R8G8B8A8_UNORM, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 1, ZEGRGraphicsModule::SAMPLE_COUNT);
	GBuffer3 = ZEGRTexture2D::CreateInstance(Width, Height, 1, ZEGR_TF_R8G8B8A8_UNORM, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 1, ZEGRGraphicsModule::SAMPLE_COUNT);
	DepthStencilBuffer	= ZEGRTexture2D::CreateInstance(Width, Height, 1, ZEGR_TF_D24_UNORM_S8_UINT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_DEPTH_STENCIL, 1, ZEGRGraphicsModule::SAMPLE_COUNT);

	RenderTargets[0] = GBuffer0->GetRenderTarget();
	RenderTargets[1] = GBuffer1->GetRenderTarget();
	RenderTargets[2] = GBuffer2->GetRenderTarget();
	RenderTargets[3] = GBuffer3->GetRenderTarget();

	return true;
}

bool ZERNStageGBuffer::Update()
{
	if (!UpdateRenderTargets())
		return false;

	return true;
}

bool ZERNStageGBuffer::InitializeSelf()
{
	if (!ZERNStage::InitializeSelf())
		return false;

	return Update();
}

void ZERNStageGBuffer::DeinitializeSelf()
{
	GBuffer0.Release();
	GBuffer1.Release();
	GBuffer2.Release();
	GBuffer3.Release();
	DepthStencilBuffer.Release();

	ZERNStage::DeinitializeSelf();
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

ZEGRTexture2D* ZERNStageGBuffer::GetAccumulationMap() const
{
	return GBuffer0;
}

ZEGRTexture2D* ZERNStageGBuffer::GetEmissiveGlossMap() const
{
	return GBuffer1;
}

ZEGRTexture2D* ZERNStageGBuffer::GetDiffuseSubSurfaceMap() const
{
	return GBuffer2;
}

ZEGRTexture2D* ZERNStageGBuffer::GetNormalSpecularMap() const
{
	return GBuffer3;
}

ZEGRTexture2D* ZERNStageGBuffer::GetDepthStencilMap() const
{
	return DepthStencilBuffer;
}

const ZEGRTexture2D* ZERNStageGBuffer::GetOutput(ZERNStageBuffer Output) const 
{
	if (GetEnabled())
	{
		switch (Output)
		{
			case ZERN_SO_COLOR:
			case ZERN_SO_ACCUMULATION:
				return GetAccumulationMap();

			case ZERN_SO_GBUFFER_EMISSIVE:
				return GetEmissiveGlossMap();

			case ZERN_SO_GBUFFER_DIFFUSE:
				return GetDiffuseSubSurfaceMap();

			case ZERN_SO_NORMAL:
				return GetNormalSpecularMap();	

			case ZERN_SO_DEPTH:
				return GetDepthStencilMap();
		}
	}

	return ZERNStage::GetOutput(Output);
}

bool ZERNStageGBuffer::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!Update())
		return false;
	
	Context->ClearRenderTarget(GBuffer0->GetRenderTarget(), ZEVector4::Zero);
	Context->ClearRenderTarget(GBuffer1->GetRenderTarget(), ZEVector4::Zero);
	Context->ClearRenderTarget(GBuffer2->GetRenderTarget(), ZEVector4::Zero);
	Context->ClearRenderTarget(GBuffer3->GetRenderTarget(), ZEVector4::Zero);
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
	memset(RenderTargets, NULL, sizeof(ZEGRRenderTarget*) * 4);
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