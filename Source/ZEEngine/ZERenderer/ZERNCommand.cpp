//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNCommand.cpp
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

#include "ZERNCommand.h"

#include "ZEGame\ZEEntity.h"

ZELink<ZERNCommand>* ZERNCommand::GetFreeLink()
{
	for (ZESize I = 0; I < ZERN_MAX_COMMAND_LINK; I++)
		if (!Links[I].GetInUse())
			return &Links[I];

	zeBreak(true);
	return NULL;
}

bool ZERNCommand::AddSubCommand(ZERNCommand* Command)
{
	return false;
}

void ZERNCommand::Reset()
{
	zeCheckError(Link.GetInUse(), ZE_VOID, "Instancelink in use");

	SubCommands.AddBegin(&Link);
}

void ZERNCommand::Clear()
{
	//SubCommands.Clear();

	//ZERNCommand* Command = NULL;//SubCommands.GetFirstItem();
	//while ((Command = SubCommands.PopItem()) != NULL)
	//{
	//	if (Command == this)
	//		continue;
	//
	//	Command->Clear();
	//}
	//
	//SubCommands.AddBegin(&Link);

	ze_for_each(Command, SubCommands)
	{
		if (Command.GetPointer() == this)
			continue;
	
		Command->Clear();
	}
	
	SubCommands.Clear();
}

void ZERNCommand::Execute(const ZERNRenderParameters* Parameters)
{
	if (!Callback.IsNull())
		Callback(Parameters, this);
	else if (Entity != NULL)
		Entity->Render(Parameters, this);
}

ZERNCommand::ZERNCommand() : Link(this)
{
	Entity = NULL;
	Priority = 0;
	Order = 0.0f;
	SceneIndex = 0;
	StageMask = 0;
	ExtraParameters = NULL;

	for (ZESize I = 0; I < ZERN_MAX_COMMAND_LINK; I++)
		new (&Links[I]) ZELink<ZERNCommand>(this);
	
	//SubCommands.AddBegin(&Link);
}

void ZERNCommandList::AddCommand(ZERNCommand* Command)
{
	bool GroupFound = false;
	ze_for_each(DestCmd, CommandList)
	{
		zeCheckError(DestCmd.GetPointer() == Command, ZE_VOID, "Duplicated commands have been detected");

		if (DestCmd->AddSubCommand(Command))
		{
			GroupFound = true;
			break;
		}
	}

	if (!GroupFound)
		this->CommandList.AddBegin(Command->GetFreeLink());
}

void ZERNCommandList::AddCommandMultiple(const ZEList2<ZERNCommand>& CommandList)
{
	ze_for_each(SrcCmd, CommandList)
	{
		bool GroupFound = false;
		ze_for_each(DestCmd, this->CommandList)
		{
			zeCheckError(DestCmd.GetPointer() == SrcCmd.GetPointer(), ZE_VOID, "Duplicated commands have been detected");

			if (DestCmd->AddSubCommand(SrcCmd.GetPointer()))
			{
				GroupFound = true;
				break;
			}
		}

		if (!GroupFound)
			this->CommandList.AddBegin(SrcCmd->GetFreeLink());
	}
}

void ZERNCommandList::Clear()
{
	ze_for_each(Cmd, CommandList)
		Cmd->Clear();

	CommandList.Clear();
}
