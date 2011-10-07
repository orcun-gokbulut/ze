//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEOptionSection.cpp
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

#include "ZEOptionSection.h"
#include "ZEOption.h"
#include "ZEError.h"
#include <ctype.h>
#include <string.h>

bool ZEOptionSection::AddOption(ZEOption* Option)
{
	if (GetOption(Option->GetName()) != NULL)
	{
		zeError("Options Section", 
			"Can not add option to option section. An option with same name is already exist in the option section. (Option Section Name : \"%s\", Option Name : \"%s\")", 
			this->GetName(),
			Option->GetName());
		return false;
	}
	Option->Section = this;
	Options.Add(Option);
	return true;
}

void ZEOptionSection::DeleteOption(size_t Index)
{
	Options.DeleteAt(Index);
}

size_t ZEOptionSection::GetNumberOfOptions()
{
	return Options.GetCount();
}

ZEOption* ZEOptionSection::GetOption(const char* OptionName)
{
	for(size_t I=0; I < Options.GetCount(); I++)
		if (_stricmp(Options[I]->GetName(), OptionName) == 0)
			return Options[I];
	return NULL;
}

ZEOption* ZEOptionSection::GetOption(size_t Index)
{
	if (Index >= 0 && Index < Options.GetCount())
			return Options[Index];
	else
		return NULL;
}

void ZEOptionSection::SetEventHandler(ZEOptionsChangedEventCallback NewEventHandler)
{
	EventHandler = NewEventHandler;
}

bool ZEOptionSection::HasChanges()
{
	return Changed;
}

void ZEOptionSection::CommitChanges()
{
	if (!Changed)
	{
		zeWarning("Option Section",  "Wrong change commit made on option section. Options in option section was not changed. (Option Section Name : \"%s\")", this->GetName());
	}
	else
		if (EventHandler != NULL)
			EventHandler();
}

void ZEOptionSection::ResetChanges()
{
	ZEOption* CurrOption;
	for (size_t I = 0; I < Options.GetCount(); I++)
	{
		CurrOption = Options[I];
		if (CurrOption->IsChanged())
			CurrOption->ChangeCommitted();
	}
}


ZEOptionSection::ZEOptionSection()
{
	SetName("");
	Changed = false;
}

ZEOptionSection::ZEOptionSection(const char* Name)
{
	SetName(Name);
	Changed = false;
}

ZEOptionSection::~ZEOptionSection()
{
	for (size_t I = 0; I < Options.GetCount(); I++)
		delete Options[I];
}
