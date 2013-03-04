//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECommandSection.cpp
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
#include "ZECommandSection.h"
#include "ZEError.h"
#include <string.h>

const ZEString& ZECommandSection::GetName()
{
	return Name;
}

void ZECommandSection::SetName(const ZEString& Name)
{
	this->Name = Name;
}

ZESize ZECommandSection::GetNumberOfCommands()
{
	return Commands.GetCount();
}

bool ZECommandSection::AddCommand(ZECommand* Command)
{
	if (GetCommand(Command->GetName()) != NULL)
	{
		zeError("Can not add command to command section. An command with same name is already exist in the command section. (Command Section Name : \"%s\", Command Name : \"%s\")", 
			this->GetName().ToCString(),
			Command->GetName().ToCString());
		return false;
	}
	Commands.Add(Command);
	return true;
}

ZECommand* ZECommandSection::GetCommand(const ZEString& Name)
{
	for(ZESize I=0; I < Commands.GetCount(); I++)
		if (Commands[I]->GetName() == Name)
			return Commands[I];
	return NULL;
}

ZECommand* ZECommandSection::GetCommand(ZESize Index)
{
	return Commands.GetItem(Index);
}

void ZECommandSection::DeleteCommand(ZESize Index)
{
	Commands.DeleteAt(Index);
}

bool ZECommandSection::ExecuteCommand(const ZEString& Name, ZEArray<ZEValue>* Paramlist)
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

}

ZECommandSection::ZECommandSection(const ZEString& Name)
{
	this->Name = Name;
}

ZECommandSection::~ZECommandSection()
{
	for(ZESize I = 0; I < Commands.GetCount(); I++)
		delete Commands[I];
	Commands.Clear();
}
