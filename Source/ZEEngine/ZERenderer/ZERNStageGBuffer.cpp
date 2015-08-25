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
#include "ZEGraphics\ZEGRContext.h"
#include "ZEGraphics\ZEGRTexture2D.h"
#include "ZERNRenderer.h"
#include "ZEGraphics\ZEGROutput.h"
#include "ZEGraphics\ZEGRRenderTarget.h"
#include "ZERNStageID.h"

void ZERNStageGBuffer::UpdateRenderTargets(ZERNRenderer* Renderer)
{
	ZEGROutput* Output = Renderer->GetOutput();
	ZEGRRenderTarget* OutputRenderTarget = Output->GetRenderTarget();
	if (Output == NULL || OutputRenderTarget == NULL)
	{
		GBuffer0.Release();
		GBuffer1.Release();
		GBuffer2.Release();
		GBuffer3.Release();
	}

	if (!GBuffer0.IsNull() && GBuffer0->GetWidth() == OutputRenderTarget->GetWidth() && GBuffer0->GetHeight() == OutputRenderTarget->GetHeight())
		return;

	GBuffer0 = ZEGRTexture2D::CreateInstance(OutputRenderTarget->GetWidth(), OutputRenderTarget->GetHeight(), 1, ZEGR_TF_R8G8B8A8_UNORM, true);
	GBuffer1 = ZEGRTexture2D::CreateInstance(OutputRenderTarget->GetWidth(), OutputRenderTarget->GetHeight(), 1, ZEGR_TF_R8G8B8A8_UNORM, true);
	GBuffer2 = ZEGRTexture2D::CreateInstance(OutputRenderTarget->GetWidth(), OutputRenderTarget->GetHeight(), 1, ZEGR_TF_R8G8B8A8_UNORM, true);
	GBuffer3 = ZEGRTexture2D::CreateInstance(OutputRenderTarget->GetWidth(), OutputRenderTarget->GetHeight(), 1, ZEGR_TF_R10G10B10A2_UINT, true);

	RenderTargets[0] = GBuffer0->GetRenderTarget(0);
	RenderTargets[1] = GBuffer1->GetRenderTarget(0);
	RenderTargets[2] = GBuffer2->GetRenderTarget(0);
	RenderTargets[3] = GBuffer3->GetRenderTarget(0);
}

ZEInt ZERNStageGBuffer::GetId()
{
	return ZERN_STAGE_GBUFFER;
}

const ZEString& ZERNStageGBuffer::GetName()
{
	static ZEString Name = "GBuffer";
	return Name;
}

ZEGRTexture2D* ZERNStageGBuffer::GetPositionBuffer()
{
	return GBuffer0;
}

ZEGRTexture2D* ZERNStageGBuffer::GetNormalBuffer()
{
	return GBuffer1;
}

ZEGRTexture2D* ZERNStageGBuffer::GetDiffuseBuffer()
{
	return GBuffer2;
}

ZEGRTexture2D* ZERNStageGBuffer::GetSpecularBuffer()
{
	return GBuffer3;
}

ZEGRTexture2D* ZERNStageGBuffer::GetAccumulationBuffer()
{
	return AccumulationBuffer;
}

bool ZERNStageGBuffer::Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands)
{
	UpdateRenderTargets(Renderer);
	Context->SetRenderTarget(4, RenderTargets);
	
	return true;
}

const ZEGRRenderState& ZERNStageGBuffer::GetRenderState()
{
	static ZEGRRenderState RenderState;
	bool Initialized = false;
	if (!Initialized)
	{
		RenderState = ZERNStage::GetRenderState();

		ZEGRDepthStencilState DepthStencilState;
		DepthStencilState.SetZTestEnable(true);
		DepthStencilState.SetZWriteEnable(true);
		RenderState.SetDepthStencilState(DepthStencilState);

		ZEGRBlendState BlendState;
		RenderState.SetRenderTargetFormat(0, ZEGR_TF_R8G8B8A8_UNORM);
		RenderState.SetRenderTargetFormat(1, ZEGR_TF_R8G8B8A8_UNORM);
		RenderState.SetRenderTargetFormat(2, ZEGR_TF_R8G8B8A8_UNORM);
		RenderState.SetRenderTargetFormat(3, ZEGR_TF_R10G10B10A2_UINT);
	}

	return RenderState;
}
