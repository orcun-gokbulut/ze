//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNMaterial.cpp
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

#include "ZERNMaterial.h"

#include "ZECore/ZECore.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZERNCommand.h"

ZERNMaterialStage::ZERNMaterialStage()
{
	Stage = NULL;
}

void ZERNMaterial::AddStage(ZERNStage* Stage, ZEGRRenderStateData* State)
{
	bool Found = false;
	for (ZESize I = 0; I < StageCount; I++)
	{
		if (Stages[I].Stage == Stage)
		{
			Stages[I].RenderState = State;
			Found = true;
		}
	}

	if (!Found)
	{
		Stages[StageCount].Stage = Stage;
		Stages[StageCount].RenderState = State;
		StageCount++;
	}
}

ZEGRRenderStateData* ZERNMaterial::GetRenderState(ZERNStage* Stage)
{
	return NULL;
}

ZERNMaterial::ZERNMaterial()
{
	LightningEnabled = true;
	ShadowCaster = true;
	ShadowReceiver = true;
}

ZERNMaterial::~ZERNMaterial()
{

}

ZEGRResourceType ZERNMaterial::GetResourceType()
{
	return ZEGR_RT_MATERIAL;
}

void ZERNMaterial::SetShadowCaster(bool Value)
{
	ShadowCaster = Value;
}

bool ZERNMaterial::GetShadowCaster() const
{
	return ShadowCaster;
}

void ZERNMaterial::SetShadowReceiver(bool Value)
{
	ShadowReceiver = Value;
}

bool ZERNMaterial::GetShadowReceiver() const
{
	return ShadowReceiver;
}

void ZERNMaterial::SetLightningEnabled(bool Enabled)
{
	LightningEnabled = Enabled;
}

bool ZERNMaterial::GetLightningEnabled() const
{
	return LightningEnabled;
}

bool ZERNMaterial::SetupMaterial(ZEGRContext* Context, ZERNStage* Stage)
{
	ZEGRRenderStateData* State =  GetRenderState(Stage);
	if (Stage == NULL)
		return false;

	Context->SetRenderState(State);
	return true;
}

void ZERNMaterial::CleanupMaterial(ZEGRContext* Context, ZERNStage* Stage)
{

}
