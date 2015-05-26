//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSelectionManager.cpp
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

#include "ZEDSelectionManager.h"
#include "ZEDCore.h"
#include "ZEDEntityTag.h"
#include "ZEFoundation/ZEMath/ZEViewVolume.h"
#include "ZEGame/ZEEntity.h"


ZEDSelection* ZEDSelectionManager::CreateSelection(ZEViewVolume* ViewVolume)
{
	if (ViewVolume == NULL)
		return NULL;

	if (ViewVolume->GetViewVolumeType() == ZE_VVT_NONE)
		return NULL;

	switch (Mode)
	{
	case ZED_SLM_ENTITY:
		break;
	case ZED_SLM_VERTEX:
		break;
	}

	ZEArray<ZEDTag*> Tags /*= ZEDCore::GetCurrentScene()->GetEntityTags()*/;
	ZEDSelection * NewSelection = new ZEDSelection();
	
	for (ZESize I = 0; I < Tags.GetCount(); I++)
		if (!ViewVolume->CullTest(Tags[I]->GetWorldBoundingBox()))
			NewSelection->AddElement(Tags[I]);

	//With the pivot of new selection calculated (by who?) Transformation manager is called to calculate offsettopivot for elements.
}

ZEArray<ZEDSelection*>& ZEDSelectionManager::GetSelections()
{
	return Selections;
}

ZEDSelection* ZEDSelectionManager::GetSelection(ZESize Index)
{
	return Selections[Index];
}

void ZEDSelectionManager::AddSelection(ZEDSelection* Selection)
{
	if (Selections.Exists(Selection)) //Since old selections while changing current selection will be deleted. This adding should be a copy of the original selection.
		return;

	Selections.Add(Selection);
}

void ZEDSelectionManager::RemoveSelection(ZEDSelection* Selection)
{
	if (!Selections.Exists(Selection))
		return;

	Selections.RemoveValue(Selection);
}

void ZEDSelectionManager::LockSelection(ZESize Index, bool Value)
{
	if (Index >= Selections.GetCount())
		return;

	Selections[Index]->Lock = Value;
}

void ZEDSelectionManager::SetMode(ZEDSelectionMode Mode)
{
	this->Mode = Mode;
}

ZEDSelectionMode ZEDSelectionManager::GetMode()
{
	return Mode;
}

void ZEDSelectionManager::Destroy()
{
	delete this;
}

ZEDSelectionManager* ZEDSelectionManager::GetInstance()
{
	return ZEDCore::GetInstance()->GetSelectionManager();
}

ZEDSelectionManager::ZEDSelectionManager()
{
	CurrentSelection = NULL;
	Mode = ZED_SLM_ENTITY;
}

void ZEDSelectionManager::SetCurrentSelection(ZEDSelection* Selection)
{
	if (CurrentSelection != NULL)
		delete CurrentSelection;

	CurrentSelection = Selection;
}

ZEDSelection* ZEDSelectionManager::GetCurrentSelection()
{
	return CurrentSelection;
}
