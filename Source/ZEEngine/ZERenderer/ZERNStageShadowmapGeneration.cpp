//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageShadowmapGeneration.cpp
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

#include "ZERNStageShadowmapGeneration.h"

#include "ZERNStageID.h"
#include "ZERNCommand.h"
#include "ZERNRenderer.h"
#include "ZEGame/ZEEntity.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRTexture2D.h"

bool ZERNStageShadowmapGeneration::InitializeSelf()
{
	return true;
}

void ZERNStageShadowmapGeneration::DeinitializeSelf()
{
}

ZEInt ZERNStageShadowmapGeneration::GetId() const
{
	return ZERN_STAGE_SHADOW_MAP_GENERATION;
}

const ZEString& ZERNStageShadowmapGeneration::GetName() const
{
	static const ZEString Name = "Shadow map generation";
	return Name;
}

bool ZERNStageShadowmapGeneration::Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands)
{
	return true;
}

void ZERNStageShadowmapGeneration::CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	Context->SetRenderTargets(0, NULL, NULL);
}

ZERNStageShadowmapGeneration::ZERNStageShadowmapGeneration()
{
}

ZEGRRenderState ZERNStageShadowmapGeneration::GetRenderState()
{
	static ZEGRRenderState RenderState;
	static bool Initialized = false;
	
	if(!Initialized)
	{
		Initialized = true;

		ZEGRRasterizerState RasterizerState;
		RasterizerState.SetDepthBias(0.0f);
		RasterizerState.SetDepthBiasClamp(0.0f);
		RasterizerState.SetSlopeScaledDepthBias(1.0f);
		RasterizerState.SetDepthClipEnable(true);

		RenderState.SetRasterizerState(RasterizerState);
		RenderState.SetDepthStencilFormat(ZEGR_TF_D32_FLOAT);
		RenderState.SetRenderTargetFormat(0, ZEGR_TF_NONE);
	}

	return RenderState;
}
