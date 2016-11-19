//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDCommand.cpp
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

#include "ZEDCommand.h"

#include "ZEDCommandManager.h"

ZEDCommandShortcut::ZEDCommandShortcut()
{
	Key = ZED_VKK_NONE;
	Modifier = ZED_VKM_NONE;
}

ZEDCommandShortcut::ZEDCommandShortcut(ZEDViewportKeyModifiers Modifier, ZEDViewportKeyboardKey Key)
{
	this->Modifier = Modifier;
	this->Key = Key;
}

void ZEDCommand::SetName(const ZEString& Name)
{
	if (this->Name == Name)
		return;

	this->Name = Name;
	
	OnUpdated(this);
}

const ZEString& ZEDCommand::GetName() const
{
	return Name;
}

void ZEDCommand::SetType(ZEDCommandType Type)
{
	if (this->Type == Type)
		return;

	this->Type = Type;


	switch (Type)
	{
		default:
		case ZED_CT_COMMAND:
			break;

		case ZED_CT_TOGGLE:
			if (Value.GetType() != ZE_VRT_BOOLEAN)
				Value.SetValue(false);
			break;

		case ZED_CT_LIST:
			if (Value.GetType() != ZE_VRT_STRING)
				Value.SetString("");
			break;

		case ZED_CT_INPUT_TEXT:
			if (Value.GetType() != ZE_VRT_STRING)
				Value.SetString("");
			break;

		case ZED_CT_INPUT_NUMBER:
			if (!Value.IsInteger())
				Value.SetInt32(0);
			break;

		case ZED_CT_INPUT_FLOAT:
			if (!Value.IsFloatingPoint())
				Value.SetFloat(0.0f);
			break;
	}

	OnUpdated(this);
}

ZEDCommandType ZEDCommand::GetType() const
{
	return Type;
}

void ZEDCommand::SetText(const ZEString& Text)
{
	if (this->Text == Text)
		return;

	this->Text = Text;

	OnUpdated(this);
}

const ZEString& ZEDCommand::GetText() const
{
	return Text;
}

void ZEDCommand::SetIcon(const ZEString& Icon)
{
	if (this->Icon == Icon)
		return;

	this->Icon = Icon;

	OnUpdated(this);
}

const ZEString& ZEDCommand::GetIcon() const
{
	return Icon;
}

void ZEDCommand::SetCategory(const ZEString& Category)
{
	this->Category = Category;
}

const ZEString& ZEDCommand::GetCategory() const
{
	return Category;
}

void ZEDCommand::SetEnabled(bool Enabled)
{
	if (this->Enabled == Enabled)
		return;

	this->Enabled = Enabled;

	OnUpdated(this);
}

bool ZEDCommand::GetEnabled() const
{
	return Enabled;
}

void ZEDCommand::SetVisible(bool Visible)
{
	if (this->Visible == Visible)
		return;

	this->Visible = Visible;

	OnUpdated(this);
}

bool ZEDCommand::GetVisible() const
{
	return Visible;
}

void ZEDCommand::SetValue(const ZEValue& Value)
{
	this->Value = Value;
	OnUpdated(this);
}

const ZEValue& ZEDCommand::GetValue() const
{
	return Value;
}

void ZEDCommand::SetListItems(const ZEArray<ZEString>& Items)
{
	ComboBoxItems = Items;
}

const ZEArray<ZEString>& ZEDCommand::GetListItems() const
{
	return ComboBoxItems;
}

void ZEDCommand::SetTooltip(const ZEString& Tooltip)
{
	if (this->Tooltip == Tooltip)
		return;

	this->Tooltip = Tooltip;

	OnUpdated(this);
}

const ZEString& ZEDCommand::GetTooltip() const
{
	return Tooltip;
}

void ZEDCommand::SetShortcut(const ZEDCommandShortcut& Shortcut)
{
	if (this->Shortcut.Key == Shortcut.Key && this->Shortcut.Modifier == Shortcut.Modifier)
		return;

	this->Shortcut = Shortcut;

	OnUpdated(this);
}

const ZEDCommandShortcut& ZEDCommand::GetShortcut() const
{
	return Shortcut;
}

ZEDCommand::ZEDCommand()
{
	Manager = NULL;
	Type = ZED_CT_COMMAND;
	Enabled = true;
	Visible = true;
}

ZEDCommand::~ZEDCommand()
{
	if (Manager != NULL)
		Manager->UnregisterCommand(this);
}
