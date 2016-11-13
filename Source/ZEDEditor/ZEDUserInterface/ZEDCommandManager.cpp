//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDCommandManager.cpp
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

#include "ZEDCommandManager.h"

#include "ZEDCommand.h"


ZEDCommandManager::ZEDCommandManager()
{
	CategoriesDirtyFlag = false;
}

ZEDCommandManager::~ZEDCommandManager()
{
	for (ZESize I = 0; I < Commands.GetCount(); I++)
		Commands[I]->Manager = NULL;
	
	Commands.Clear();
}

const ZEArray<ZEDCommand*>& ZEDCommandManager::GetCommands()
{
	return Commands;
}

ZEArray<ZEDCommand*> ZEDCommandManager::GetCommands(const ZEString& Category)
{
	ZEArray<ZEDCommand*> CategoryCommands;

	for (ZESize I = 0; I < Commands.GetCount(); I++)
	{
		if (Commands[I]->Category != Category)
			continue;

		CategoryCommands.Add(Commands[I]);
	}

	return CategoryCommands;
}

ZEDCommand* ZEDCommandManager::GetCommand(const ZEString& Name)
{
	for (ZESize I = 0; I < Commands.GetCount(); I++)
	{
		if (Commands[I]->Name != Name)
			continue;

		return Commands[I];
	}

	return NULL;
}

const ZEArray<ZEString>& ZEDCommandManager::GetCatagories()
{
	if (CategoriesDirtyFlag)
	{
		Categories.Clear();
		for (ZESize I = 0; I < Commands.GetCount(); I++)
		{
			if (!Categories.Exists(Commands[I]->GetCategory()))
				Categories.Add(Commands[I]->GetCategory());
		}
		CategoriesDirtyFlag = false;
	}

	return Categories;
}

bool ZEDCommandManager::RegisterCommand(ZEDCommand* Command)
{
	zeCheckError(Command == NULL, false, "Cannot register command. Command is NULL.");
	zeCheckError(Command->Manager != NULL, false, "Cannot register command. Command is already registered. Command Name: \"%s\". Command Category: \"%s\".", Command->GetName(), Command->GetCategory());

	for (ZESize I = 0; I < Commands.GetCount(); I++)
	{
		if (Commands[I]->GetName() == Command->GetName())
		{
			zeError("Cannot register command. Another command with the same name is already registered. Command Name: \"%s\".", Command->GetName().ToCString());
			return false;
		}
	}

	Command->Manager = this;
	Commands.Add(Command);

	CategoriesDirtyFlag = true;

	return true;
}

bool ZEDCommandManager::UnregisterCommand(ZEDCommand* Command)
{
	zeCheckError(Command == NULL, false, "Cannot unregister command. Command is NULL.");
	zeCheckError(Command->Manager != this, false, "Cannot unregister command. Command is not registered. Command Name: \"%s\". Command Category: \"%s\".", Command->GetName(), Command->GetCategory());

	Command->Manager = NULL;
	Commands.RemoveValue(Command);

	CategoriesDirtyFlag = true;

	return true;
}

ZEDCommandManager* ZEDCommandManager::GetInstance()
{
	static ZEDCommandManager Manager;
	return &Manager;
}
