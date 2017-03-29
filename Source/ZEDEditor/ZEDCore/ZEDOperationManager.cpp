//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDOperationManager.cpp
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

#include "ZEDOperationManager.h"

#include "ZEError.h"
#include "ZEDOperation.h"
#include "ZEDEditor.h"
#include "ZEDEditorCore.h"
#include "ZEDUserInterface\ZEDCommandManager.h"

void ZEDOperationManager::Editor_OnClosed(ZEDEditor* Editor)
{
	Clear();
}

bool ZEDOperationManager::InitializeInternal()
{
	if (!ZEDComponent::InitializeInternal())
		return false;

	GetEditor()->OnClosed.AddDelegate<ZEDOperationManager, &ZEDOperationManager::Editor_OnClosed>(this);
	
	RegisterCommands();

	return true;
}

bool ZEDOperationManager::DeinitializeInternal()
{
	GetEditor()->OnClosed.DisconnectObject(this);

	return ZEDComponent::Deinitialize();
}

ZEDOperationManager::ZEDOperationManager()
{
	StackIndex = -1;
	OperationIndex = 0;
}

ZEDOperationManager::~ZEDOperationManager()
{
	Clear();
}

void ZEDOperationManager::RegisterCommands()
{
	UndoCommand.SetName("ZEDOperationManager::UndoCommand");
	UndoCommand.SetCategory("Operations");
	UndoCommand.SetText("Undo");
	UndoCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Undo.png");
	UndoCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_Z));
	UndoCommand.OnAction.AddDelegate<ZEDOperationManager, &ZEDOperationManager::UndoCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&UndoCommand);

	RedoCommand.SetName("ZEDTransformationManager::RedoCommand");
	RedoCommand.SetCategory("Operations");
	RedoCommand.SetText("Redo");
	RedoCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Redo.png");
	RedoCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_Y));
	RedoCommand.OnAction.AddDelegate<ZEDOperationManager, &ZEDOperationManager::RedoCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&RedoCommand);

	UpdateCommands();
}

void ZEDOperationManager::UpdateCommands()
{
	UndoCommand.SetEnabled(CanUndo());
	RedoCommand.SetEnabled(CanRedo());
}

void ZEDOperationManager::UndoCommand_OnAction(const ZEDCommand* Command)
{
	Undo();
}

void ZEDOperationManager::RedoCommand_OnAction(const ZEDCommand* Command)
{
	Redo();
}

const ZEArray<ZEDOperation*>& ZEDOperationManager::GetStack()
{
	return Stack;
}

ZESSize ZEDOperationManager::GetStackIndex()
{
	return StackIndex;
}

bool ZEDOperationManager::CanUndo()
{
	return StackIndex != -1;
}

bool ZEDOperationManager::CanRedo()
{
	return StackIndex + 1 != Stack.GetCount();
}

bool ZEDOperationManager::Undo()
{
	if (!CanUndo())
		return false;

	OnUndoing(this, Stack[StackIndex]);
	if (!Stack[StackIndex]->Undo())
	{
		zeError("Cannot undo operation. Operation Text: \"%s\"", Stack[StackIndex]->GetText().ToCString());
		return false;
	}

	StackIndex--;
	OperationIndex--;
	if (OperationIndex == 0)
		GetEditor()->UnmarkDocumentModified();

	UpdateCommands();

	OnUndo(this, Stack[StackIndex + 1]);

	return true;
}

bool ZEDOperationManager::Redo()
{
	if (!CanRedo())
		return false;

	StackIndex++;	

	OnRedoing(this, Stack[StackIndex]);
	if (!Stack[StackIndex]->Do())
	{
		zeError("Cannot redo operation. Operation Text: \"%s\"", Stack[StackIndex]->GetText().ToCString());
		return false;
	}

	OperationIndex++;
	GetEditor()->UnmarkDocumentModified();

	UpdateCommands();

	OnRedo(this, Stack[StackIndex]);

	return true;
}

bool ZEDOperationManager::Do(ZEDOperation* Operation)
{
	zeCheckError(Operation == NULL, false, "Operation cannot be NULL.");
	zeCheckError(Operation->GetManager() != NULL, false, "Operation alread associated with another manager.");

	Operation->Manager = this;

	OnDoing(this, Operation);

	if (!Operation->Do())
	{
		Operation->Manager = NULL;
		zeError("Cannot do operation. Operation Text: \"%s\"", Stack[StackIndex]->GetText().ToCString());
		return false;
	}

	StackIndex++;
	OperationIndex++;
	GetEditor()->MarkDocumentModified();

	for (ZESize I = StackIndex; I < Stack.GetCount(); I++)
	{
		Stack[I]->Destroy();
		Stack.Remove(I);
		I--;
	}	
	
	Stack.Add(Operation);

	UpdateCommands();

	OnDo(this, Operation);

	return true;
}

void ZEDOperationManager::Clear()
{
	for (ZESize I = 0; I < Stack.GetSize(); I++)
		Stack[I]->Destroy();
	
	Stack.Clear();

	StackIndex = -1;
	OperationIndex = 0;

	UpdateCommands();
}

ZEDOperationManager* ZEDOperationManager::CreateInstance()
{
	return new ZEDOperationManager();
}
