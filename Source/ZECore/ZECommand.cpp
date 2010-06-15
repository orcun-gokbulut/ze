//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECommand.cpp
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
#include "ZECommand.h"
#include "ZECore.h"
#include "ZEError.h"
#include "ZEConsole.h"
#include <stdarg.h>

#pragma warning(push)
#pragma warning(disable:4996)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CCommand  CCommand  CCommand  CCommand  CCommand  CCommand  CCommand  CCommand  CCommand  CCommand  CCommand  //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ZECommand::SetAccessLevel(int Access)
{
	this->AccessLevel = Access;
}

int ZECommand::GetAccessLevel()
{
	return this->AccessLevel;
}

bool ZECommand::Execute(ZEArray<ZEVariant>* ParamList)
{
	if (zeCore->GetUserLevel() >= this->AccessLevel)																			    
		return Callback(this, ParamList);
	else
	{
		zeOutput("Invalid Command.\r\n");
		return false;
	}
}

void ZECommand::SetCallback(ZECommandCallback NewCallback)
{
	this->Callback = NewCallback;
}

ZECommand::ZECommand(const char* Name, ZECommandCallback Callback, int AccessLevel)
{
	this->AccessLevel = AccessLevel;
	this->SetName(Name);
	this->Callback = Callback;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CCommandSection  CCommandSection  CCommandSection  CCommandSection  CCommandSection  CCommandSection			 //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t ZECommandSection::GetNumberOfCommands()
{
	return Commands.GetCount();
}

bool ZECommandSection::AddCommand(ZECommand* Command)
{
	if (GetCommand(Command->GetName()) != NULL)
	{
		zeError("Command Section",
			"Can not add command to command section. An command with same name is already exist in the command section. (Command Section Name : \"%s\", Command Name : \"%s\")", 
			this->GetName(),
			Command->GetName());
		return false;
	}
	Commands.Add(Command);
	return true;
}

ZECommand* ZECommandSection::GetCommand(const char* Name)
{
	for(size_t I=0; I < Commands.GetCount(); I++)
		if (_stricmp(Commands[I]->GetName(), Name) == 0)
			return Commands[I];
	return NULL;
}

ZECommand* ZECommandSection::GetCommand(size_t Index)
{
	return Commands.GetItem(Index);
}

void ZECommandSection::DeleteCommand(size_t Index)
{
	Commands.DeleteAt(Index);
}

bool ZECommandSection::ExecuteCommand(const char* Name, ZEArray<ZEVariant>* Paramlist)
{
	ZECommand* Temp = GetCommand(Name);
	if (Temp != NULL)
	{
		Temp->Execute(Paramlist);
		return true;
	}
	else 
		return false;
}
ZECommandSection::ZECommandSection()
{
	SetName("");
}

ZECommandSection::ZECommandSection(const char* Name)
{
	SetName(Name);
}

ZECommandSection::~ZECommandSection()
{
	for(size_t I = 0; I < Commands.GetCount(); I++)
		delete Commands[I];
	Commands.Clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CCommandManager  CCommandManager  CCommandManager  CCommandManager  CCommandManager  CCommandManager			 //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

ZECommandSection* ZECommandManager::GetCommandSection(const char* Name)
{
	for (size_t I = 0 ; I < Sections.GetCount(); I++)
		if (_stricmp(Sections[I]->GetName(), Name) == 0)
			return Sections[I];
	return NULL;
}


ZECommand* ZECommandManager::GetCommand(const char* Section, const char* Name)
{
	ZECommandSection* Sec = GetCommandSection(Section);
	ZECommand* Cmd;
	if (Sec != NULL)
		return Cmd = Sec->GetCommand(Section);
	else
		return NULL;
}

bool ZECommandManager::ExecuteCommand(const char* Section, const char* Name, ZEArray<ZEVariant>* Paramslist)
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
		zeError("Command Manager",
			"Can not register command section. A command section with same name is already registered. (Command Section Name : \"%s\")", 
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
		zeError("Command Manager",
			"Can not unregister command section. There is no such a registered command section. (Command Section Name : \"%s\")", 
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

#pragma warning(pop)



