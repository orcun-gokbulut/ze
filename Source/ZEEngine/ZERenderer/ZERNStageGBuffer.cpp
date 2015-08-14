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

bool ZERNStageGBuffer::InitializeSelf()
{

}

void ZERNStageGBuffer::DeinitializeSelf()
{

}

ZEInt ZERNStageGBuffer::GetId()
{
	return 1;
}

const ZEString& ZERNStageGBuffer::GetName()
{
	return "GBuffer";
}

const ZEGRRenderState& ZERNStageGBuffer::GetRenderState()
{
	return RenderState;
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

const ZEGRRenderState& ZERNStageGBuffer::GetRenderState()
{
	return RenderState;
}

void ZERNStageGBuffer::Setup(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	Context->SetRenderTarget(0, GBuffer0->GetRenderTarget());
	Context->SetRenderTarget(1, GBuffer0);
	Context->SetRenderTarget(2, GBuffer0);
	Context->SetRenderTarget(3, GBuffer0);
}

void ZERNStageGBuffer::CleanUp()
{

}
