//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDUndoRedo.cpp
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

#include "ZEDUndoRedo.h"

///////////////////////////////////////////////////////////OPERATION///////////////////////////////////////////////////

QString ZEDUndoRedoOperation::GetInformation() const
{
	return Information;
}

void ZEDUndoRedoOperation::SetInformation(QString Information)
{
	this->Information = Information;
}

ZEDUndoRedoOperation::ZEDUndoRedoOperation()
{
	this->Information = QString();
}

///////////////////////////////////////////////////////////MANAGER////////////////////////////////////////////////////

int ZEDUndoRedoManagerOld::CurrentIndex = -1;
unsigned int ZEDUndoRedoManagerOld::MaxOperationCount = 100;
ZEArray<ZEDUndoRedoOperation*> ZEDUndoRedoManagerOld::UndoRedoStack;
bool ZEDUndoRedoManagerOld::PreviousActionIsUndo = false;
bool ZEDUndoRedoManagerOld::PreviousActionIsRedo = false;

void ZEDUndoRedoManagerOld::RegisterOperation(ZEDUndoRedoOperation* Operation)
{
	/*if(CurrentIndex != UndoRedoStack.GetCount() - 1)
	{
		for(int I = CurrentIndex; I <= UndoRedoStack.GetCount(); I++)
		{
			UndoRedoStack.DeleteAt(I);
		}
		CurrentIndex = UndoRedoStack.GetCount();
	}*/

	unsigned int TotalCount = UndoRedoStack.GetCount() + 1;

	if(TotalCount < MaxOperationCount)
	{
		UndoRedoStack.Add(Operation);
		CurrentIndex++;
	}

	else
	{
		UndoRedoStack.DeleteAt(0);
		UndoRedoStack.Add(Operation);
		//CurrentIndex = CurrentIndex;
	}
}

bool ZEDUndoRedoManagerOld::PerformUndo()
{
	if(PreviousActionIsUndo)
		CurrentIndex--;

	if(CurrentIndex >= (int)UndoRedoStack.GetCount())
		CurrentIndex = UndoRedoStack.GetCount() - 1;

	if(CurrentIndex < 0)
	{
		CurrentIndex = 0;
		return false;
	}

	if(UndoRedoStack[CurrentIndex]->Undo())
	{
		PreviousActionIsUndo = true;
		PreviousActionIsRedo = false;
		return true;
	}

	else
	{
		PreviousActionIsUndo = true;
		PreviousActionIsRedo = false;
		CurrentIndex--;
		return false;
	}
}

bool ZEDUndoRedoManagerOld::PerformRedo()
{
	if(PreviousActionIsRedo)
		CurrentIndex++;

	if(CurrentIndex < 0 || CurrentIndex >= (int)UndoRedoStack.GetCount())
	{
		return false;
	}
	if(UndoRedoStack[CurrentIndex]->Redo())
	{	
		PreviousActionIsUndo = false;
		PreviousActionIsRedo = true;
		return true;
	}
	else
	{
		PreviousActionIsUndo = false;
		PreviousActionIsRedo = true;
		CurrentIndex++;
		return false;
	}
}

void ZEDUndoRedoManagerOld::SetMaxOperationCount(unsigned int Count)
{
	this->MaxOperationCount = Count;
}

unsigned int ZEDUndoRedoManagerOld::GetMaxOperationCount() const
{
	return MaxOperationCount;
}

ZEDUndoRedoManagerOld::ZEDUndoRedoManagerOld()
{

}
