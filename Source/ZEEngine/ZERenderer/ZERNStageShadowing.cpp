//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageShadowing.cpp
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

#include "ZERNStageShadowing.h"

#include "ZERNStageID.h"
#include "ZEDS/ZEString.h"
#include "ZEGraphics/ZEGRTexture2D.h"

bool ZERNStageShadowing::InitializeSelf()
{
	if (!ZERNStage::InitializeSelf())
		return false;

	ProjectiveShadowMaps = ZEGRTexture2D::CreateInstance(512, 512, 1, ZEGR_TF_D32_FLOAT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_DEPTH_STENCIL, 4);

	return true;
}

void ZERNStageShadowing::DeinitializeSelf()
{
	ProjectiveShadowMaps.Release();

	ZERNStage::DeinitializeSelf();
}

ZEInt ZERNStageShadowing::GetId() const
{
	return ZERN_STAGE_SHADOWING;
}

const ZEString& ZERNStageShadowing::GetName() const
{
	static const ZEString Name = "Shadowing";
	return Name;
}

const ZEGRTexture2D* ZERNStageShadowing::GetOutput(ZERNStageBuffer Output) const
{
	if (GetEnabled() && (Output == ZERN_SO_PROJECTIVE_SHADOWMAPS))
		return ProjectiveShadowMaps;

	return ZERNStage::GetOutput(Output);
}

bool ZERNStageShadowing::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (GetCommands().GetCount() == 0)
		return false;

	ShadowMapCount = 0;

	return true;
}

ZERNStageShadowing::ZERNStageShadowing()
{
	ShadowMapCount = 0;
}
