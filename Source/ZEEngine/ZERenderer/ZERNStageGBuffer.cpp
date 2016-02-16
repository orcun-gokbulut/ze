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

#include "ZEDS/ZEString.h"
#include "ZERNRenderer.h"
#include "ZERNStageID.h"
#include "ZEGraphics/ZEGROutput.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"

void ZERNStageGBuffer::DeinitializeSelf()
{
	CleanRenderTargets();

	ZERNStage::DeinitializeSelf();
}

bool ZERNStageGBuffer::UpdateRenderTargets(ZEUInt Width, ZEUInt Height)
{
	CleanRenderTargets();

	DepthStencilBuffer	= ZEGRTexture2D::CreateInstance(Width, Height, 1, 1, 1, ZEGR_TF_D24_UNORM_S8_UINT, false, true);
	GBuffer0 = ZEGRTexture2D::CreateInstance(Width, Height, 1, 1, 1, ZEGR_TF_R11G11B10_FLOAT, true);
	GBuffer1 = ZEGRTexture2D::CreateInstance(Width, Height, 1, 1, 1, ZEGR_TF_R8G8B8A8_UNORM, true);
	GBuffer2 = ZEGRTexture2D::CreateInstance(Width, Height, 1, 1, 1, ZEGR_TF_R8G8B8A8_UNORM, true);
	GBuffer3 = ZEGRTexture2D::CreateInstance(Width, Height, 1, 1, 1, ZEGR_TF_R8G8B8A8_UNORM, true);

	RenderTargets[0] = GBuffer0->GetRenderTarget();
	RenderTargets[1] = GBuffer1->GetRenderTarget();
	RenderTargets[2] = GBuffer2->GetRenderTarget();
	RenderTargets[3] = GBuffer3->GetRenderTarget();

	return true;
}


void ZERNStageGBuffer::CleanRenderTargets()
{
	DepthStencilBuffer.Release();
	GBuffer0.Release();
	GBuffer1.Release();
	GBuffer2.Release();
	GBuffer3.Release();
}

ZEInt ZERNStageGBuffer::GetId() const
{
	return ZERN_STAGE_GBUFFER;
}

const ZEString& ZERNStageGBuffer::GetName() const
{
	static ZEString Name = "GBuffer";
	return Name;
}

ZEGRTexture2D* ZERNStageGBuffer::GetDiffuseColorMap() const
{
	return GBuffer2;
}

ZEGRTexture2D* ZERNStageGBuffer::GetSpecularColorMap() const
{
	return GBuffer3;
}

ZEGRTexture2D* ZERNStageGBuffer::GetNormalMap() const
{
	return GBuffer1;
}

ZEGRTexture2D* ZERNStageGBuffer::GetAccumulationMap() const
{
	return GBuffer0;
}

ZEGRTexture2D* ZERNStageGBuffer::GetDepthMap() const
{
	return DepthStencilBuffer;
}

bool ZERNStageGBuffer::Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands)
{
	if (!ZERNStage::Setup(Renderer, Context, Commands))
		return false;

	ZEGRRenderTarget* RenderTarget = Renderer->GetOutputRenderTarget();
	if(RenderTarget == NULL)
		return false;

	ZEUInt Width = RenderTarget->GetWidth();
	ZEUInt Height = RenderTarget->GetHeight();

	if (GBuffer0 == NULL || 
		GBuffer0->GetWidth() != Width || GBuffer0->GetHeight() != Height)
	{
		if(!UpdateRenderTargets(Width, Height))
			return false;
	}

	Viewport.SetWidth(static_cast<float>(Width));
	Viewport.SetHeight(static_cast<float>(Height));

	Context->ClearRenderTarget(GBuffer0->GetRenderTarget(), ZEVector4::Zero);
	Context->ClearRenderTarget(GBuffer1->GetRenderTarget(), ZEVector4::Zero);
	Context->ClearRenderTarget(GBuffer2->GetRenderTarget(), ZEVector4::Zero);
	Context->ClearRenderTarget(GBuffer3->GetRenderTarget(), ZEVector4::Zero);
	Context->ClearDepthStencilBuffer(DepthStencilBuffer->GetDepthStencilBuffer(), true, false, 1.0f, 0x00);

	Context->SetViewports(1, &Viewport);
	Context->SetRenderTargets(4, RenderTargets, DepthStencilBuffer->GetDepthStencilBuffer());

	return true;
}

void ZERNStageGBuffer::CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	Context->SetRenderTargets(0, NULL, NULL);

	ZERNStage::CleanUp(Renderer, Context);
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
