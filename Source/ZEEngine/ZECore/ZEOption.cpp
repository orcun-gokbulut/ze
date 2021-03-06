//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEOption.cpp
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

#include "ZEOption.h"

#include "ZEOptionSection.h"
#include "ZEError.h"


const ZEString& ZEOption::GetName()
{
	return Name;
}

void ZEOption::SetName(const ZEString& Name)
{
	this->Name = Name;
}

bool ZEOption::IsChanged()
{
	return Changed;
}

ZEOptionAttribute ZEOption::GetAttribute()
{
	return Attribute;
}

void ZEOption::SetAttribute(ZEOptionAttribute NewAttribute)
{
	Attribute = NewAttribute;
}

void ZEOption::SetValue(ZEValue NewValue)
{	
	if (NewValue.GetType() != DefaultValue.GetType())
		return;

	bool Cancel = false;
	if (!OnChanging.IsNull())
		OnChanging(this, Cancel);

	bool SectionCancel = false;
	if (!Section->GetOnChanging().IsNull())
		Section->GetOnChanging()(this, Cancel);


	if (!Cancel)
	{
		Value = NewValue;
		Changed = true;
		Section->Changed = true;

		if (!OnChanged.IsNull())
			OnChanged(this);
		if (!Section->GetOnChanged().IsNull())
			Section->GetOnChanged()(this);
	}

}

void ZEOption::SetValueType(ZEValueType NewType)
{
	DefaultValue.SetType(NewType);
	Value.SetType(NewType);
}

ZEValueType ZEOption::GetValueType()
{
	return Value.GetType();
}

ZEValue ZEOption::GetValue()
{
	return Value;
}

void ZEOption::SetDefaultValue(ZEValue NewDefaultValue)
{
	DefaultValue = NewDefaultValue;
}

ZEValue ZEOption::GetDefaultValue() 
{
	return DefaultValue;
}

ZEOptionType ZEOption::GetType() 
{
	return ZE_OT_NORMAL;
}

const ZEOptionsChangingEvent& ZEOption::GetOnChanging()
{
	return OnChanging;
}

void ZEOption::SetOnChanging(ZEOptionsChangingEvent EventHandler)
{
	OnChanging = EventHandler;
}

const ZEOptionsChangedEvent& ZEOption::GetOnChanged()
{
	return OnChanged;
}
void ZEOption::SetOnChanged(ZEOptionsChangedEvent EventHandler)
{
	OnChanged = EventHandler;
}

void ZEOption::ChangeCommitted()
{
	if (!Changed)
	{
		zeWarning("Wrong change commit made on option. Options was not changed. (Option Name : \"%s\")", (const char*)this->GetName());
	}
	else
		Changed = false;
}

ZEOption::ZEOption(const char *InitialName, ZEValue InitialDefaultValue, ZEOptionAttribute InitialAttribute)
{
	Name = InitialName;
	SetValueType(InitialDefaultValue.GetType());
	DefaultValue = InitialDefaultValue;
	Value=InitialDefaultValue;
	Attribute = InitialAttribute;
	Changed = false;
}

ZEOption::ZEOption()
{
	Name = "";
	SetValueType(ZE_VRT_UNDEFINED);
	Attribute = ZE_OA_NORMAL;
	Changed = false;
}
