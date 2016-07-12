//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageManager.cpp
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

#include "ZERNStageManager.h"
#include "ZERNStage.h"

bool ZERNStageManager::InitializeSelf()
{
	if (!ZEInitializable::InitializeSelf())
		return false;

	for (ZESize I = 0; I < Stages.GetCount(); I++)
		if (!Stages[I]->Initialize())
			return false;

	return true;
}

bool ZERNStageManager::DeinitializeSelf()
{
	for (ZESize I = 0; I < Stages.GetCount(); I++)
		Stages[I]->Deinitialize();

	return ZEInitializable::DeinitializeSelf();
}

ZERNStageManager::ZERNStageManager()
{

}

ZERNStageManager::~ZERNStageManager()
{

}

const ZEArray<ZERNStage*>& ZERNStageManager::GetStages()
{
	return Stages;
}

ZERNStage* ZERNStageManager::GetStage(const char* Name)
{
	for (ZESize I = 0; I < Stages.GetCount(); I++)
		if (Stages[I]->GetName() == Name)
			return Stages[I];

	return NULL;
}

void ZERNStageManager::AddStage(ZERNStage* Stage)
{
	if (Stages.Exists(Stage))
		zeError("Stage is already added.");

	if (IsInitialized())
		Stage->Initialize();

	Stages.Add(Stage);
}

void ZERNStageManager::RemoveStage(ZERNStage* Stage)
{
	if (IsInitialized())
		Stage->Deinitialize();

	Stages.RemoveValue(Stage);
}

ZERNStageManager* ZERNStageManager::GetInstance()
{
	static ZERNStageManager Manager;
	return &Manager;
}
