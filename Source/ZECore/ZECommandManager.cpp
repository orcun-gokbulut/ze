//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECommandManager.cpp
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

#pragma once
#include "ZECommandManager.h"
#include "ZECommand.h"
#include "ZECommandSection.h"
#include "ZECore.h"
#include "ZEError.h"
#include "ZEConsole.h"

bool ZECommandManager::Callback_ListSections(ZECommand* Command, const ZECommandParameterList* Params)
{
	size_t Index, Count;

	if (Params->GetCount() == 0)
	{
		Index = 0;
		Count = Sections.GetCount();
	}
	else if (Params->GetCount() == 2 && 
			 Params->GetItem(0).GetType() == ZE_VRT_INTEGER && 
			 Params->GetItem(1).GetType() == ZE_VRT_INTEGER)
	{
		Index = Params->GetItem(0).GetInteger();
		Count = Params->GetItem(1).GetInteger();
	}
	else
	{
		zeOutput("Incorrect parameters\r\n"
						 "Usage :\r\n"
						 "  Command.ListSections() - Lists all of the command sections\r\n"
						 "  Command.ListSection(Integer From, Integer Count) - Lists 'Count' number of command sections starting from index 'From'\r\n"
						 "Example :\r\n"
						 "  Command.ListSections(10, 50) - List command sections from 10 to 60\r\n");
		return false;
	}

	zeOutput(" Name                       Number Of Commands \r\n"
					 "-----------------------------------------------\r\n");

	for (size_t I = Index; I < Sections.GetCount() && I <= Count; I++)
		zeOutput(" %-30s      %d\r\n", Sections.GetItem(I)->GetName(), Sections.GetItem(I)->GetNumberOfCommands());
	return true;
}

bool ZECommandManager::Callback_ListCommands(ZECommand* Command, const ZECommandParameterList* Params)
{
	size_t Index, Count;
	ZECommandSection* Sec;

	if (Params->GetCount() == 1 &&
		Params->GetItem(0).GetType() == ZE_VRT_STRING)
	{
		Sec = GetCommandSection(Params->GetItem(0).GetString());
		Index = 0;
		Count = -1;
	}
	else if (Params->GetCount() == 1 &&
			 Params->GetItem(0).GetType() == ZE_VRT_STRING &&
			 Params->GetItem(1).GetType() == ZE_VRT_INTEGER &&
			 Params->GetItem(2).GetType() == ZE_VRT_INTEGER)
	{
		Sec = GetCommandSection(Params->GetItem(0).GetString());
		Index = Params->GetItem(1).GetInteger();
		Count = Params->GetItem(2).GetInteger();
	}
	else
	{
		zeOutput("Incorrect parameters. \r\n"
						 "Usage :\r\n"
						 " Command.ListCommands(String Section) - Lists all of the commans in the command section named 'Section'\r\n"
						 " Command.ListCommands(String Section, Integer Index, Integer Count) - Lists 'Count' number of commands in the command section named 'Section' starting from index 'Index'\r\n"
						 "Example :\r\n"
						 "  Command.ListCommands(\"Core\", 1, 5) - List commands in the Core command section from 1 to 5\r\n");
		return false;
	}

	if (Sec != NULL)
	{
		zeOutput(" Name                        Access Level\r\n"
						 "------------------------------------------\r\n");
		
		ZECommand* Cmd;
		for (size_t I = 0; I < Sec->GetNumberOfCommands() && I <= Count; I++)
		{
			Cmd = Sec->GetCommand(I);	
			zeOutput(" %-30s", Cmd->GetName());
			switch(Cmd->GetAccessLevel())
			{
				case ZE_UL_DEVELOPPER:
					zeOutput(" Developer\r\n");
					break;
				case ZE_UL_PLAYER:
					zeOutput(" Player\r\n");
					break;
				case ZE_UL_CHEATER:
					zeOutput("Cheater\r\n");
					break;
			}
		}
		return true;
	}
	else
	{
		zeOutput("There is no such an command section. \r\n");
		return false;
	}
}

size_t ZECommandManager::GetNumberOfSections()
{
	return Sections.GetCount();
}

ZECommandSection* ZECommandManager::GetCommandSection(const ZEString& Name)
{
	for (size_t I = 0 ; I < Sections.GetCount(); I++)
		if (Sections[I]->GetName() == Name)
			return Sections[I];
	return NULL;
}


ZECommand* ZECommandManager::GetCommand(const ZEString& Section, const ZEString& Name)
{
	ZECommandSection* Sec = GetCommandSection(Section);
	ZECommand* Cmd;
	if (Sec != NULL)
		return Cmd = Sec->GetCommand(Section);
	else
		return NULL;
}

bool ZECommandManager::ExecuteCommand(const ZEString& Section, const ZEString& Name, ZEArray<ZEVariant>* Paramslist)
{
	ZECommandSection* Temp = GetCommandSection(Section);
	if (Temp != NULL)
		if (Temp->ExecuteCommand(Name,Paramslist))
			return true;
		else
			return false;
	else
		return false;
}

bool ZECommandManager::RegisterSection(ZECommandSection* Section)
{
	if (GetCommandSection(Section->GetName()) != NULL)
	{
		zeError("Can not register command section. A command section with same name is already registered. (Command Section Name : \"%s\")", 
			Section->GetName());
		return false;
	}
	Sections.Add(Section);
	return true;
}

bool ZECommandManager::UnregisterSection(ZECommandSection* Section)
{
	if (GetCommandSection(Section->GetName()) == NULL)
	{
		zeError("Can not unregister command section. There is no such a registered command section. (Command Section Name : \"%s\")", 
			Section->GetName());
		return false;
	}
	Sections.DeleteValue(Section);
	return true;
}

ZECommandManager* ZECommandManager::GetInstance()
{
	return ZECore::GetInstance()->GetCommands();
}

ZECommandManager::ZECommandManager()
{
	Commands.SetName("Commands");
	Commands.AddCommand(new ZECommand("ListCommands", BindDelegate(this, &ZECommandManager::Callback_ListCommands)));
	Commands.AddCommand(new ZECommand("ListSections",BindDelegate(this, &ZECommandManager::Callback_ListSections)));
	RegisterSection(&Commands);
}

ZECommandManager::~ZECommandManager()
{
	UnregisterSection(&Commands);
}
