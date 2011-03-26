//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDGizmoUndoRedo.cpp
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

#include "ZEDGizmoUndoRedo.h"
#include "ZEDGizmo.h"

void ZEDGizmoUndoRedoOperation::SetOldValues(ZEArray<ZEVariant> OldValues)
{
	this->OldValues = OldValues;
}

void ZEDGizmoUndoRedoOperation::SetNewValues(ZEArray<ZEVariant> NewValues)
{
	this->NewValues = NewValues;
}

void ZEDGizmoUndoRedoOperation::SetClasses(ZEArray<ZEClass*> SelectedClasses)
{	
	this->SelectedClasses = SelectedClasses;
}

void ZEDGizmoUndoRedoOperation::SetGizmoMode(ZEDGizmoMode GizmoMode)
{
	this->GizmoMode = GizmoMode;
}

ZEDGizmoMode ZEDGizmoUndoRedoOperation::GetGizmoMode()
{
	return GizmoMode;
}

bool ZEDGizmoUndoRedoOperation::Undo()
{	
	if (GizmoMode == ZED_GM_MOVE)
	{	
		for(int I = 0; I < SelectedClasses.GetCount(); I++)
		{
			if(SelectedClasses[I] != NULL)
			{
				SelectedClasses[I]->SetProperty("Position", OldValues[I]);
			}
		}
	}
	if (GizmoMode == ZED_GM_SCALE)
	{	
		for(int I = 0; I < SelectedClasses.GetCount(); I++)
		{
			if(SelectedClasses[I] != NULL)
			{
				SelectedClasses[I]->SetProperty("Scale", OldValues[I]);
			}
		}
	}
	if (GizmoMode == ZED_GM_ROTATE)
	{	
		for(int I = 0; I < SelectedClasses.GetCount(); I++)
		{
			if(SelectedClasses[I] != NULL)
			{
				SelectedClasses[I]->SetProperty("Rotation", OldValues[I]);
			}
		}
	}

	return true;
}

bool ZEDGizmoUndoRedoOperation::Redo()
{
	if (GizmoMode == ZED_GM_MOVE)
	{	
		for(int I = 0; I < SelectedClasses.GetCount(); I++)
		{
			if(SelectedClasses[I] != NULL)
			{
				SelectedClasses[I]->SetProperty("Position", NewValues[I]);
			}
		}
	}
	if (GizmoMode == ZED_GM_SCALE)
	{	
		for(int I = 0; I < SelectedClasses.GetCount(); I++)
		{
			if(SelectedClasses[I] != NULL)
			{
				SelectedClasses[I]->SetProperty("Scale", NewValues[I]);
			}
		}
	}
	if (GizmoMode == ZED_GM_ROTATE)
	{	
		for(int I = 0; I < SelectedClasses.GetCount(); I++)
		{
			if(SelectedClasses[I] != NULL)
			{
				SelectedClasses[I]->SetProperty("Rotation", NewValues[I]);
			}
		}
	}
	
	return true;
}

ZEDGizmoUndoRedoOperation::ZEDGizmoUndoRedoOperation(ZEArray<ZEClass*> SelectedClasses, ZEDGizmoMode GizmoMode)
{
	this->SelectedClasses = SelectedClasses;
	this->GizmoMode = GizmoMode;
}
