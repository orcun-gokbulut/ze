//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDCommand.h
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

#include "ZEMeta/ZEObject.h"

#include "ZEDS/ZEString.h"
#include "ZEDS/ZEValue.h"
#include "ZEDS/ZEArray.h"
#include "ZEMeta/ZEEvent.h"
#include "ZEMeta/ZEEventDelegate.h"
#include "ZEDCore/ZEDViewportEvent.h"


ZE_ENUM(ZEDCommandType)
{
	ZED_CT_COMMAND,
	ZED_CT_TOGGLE,
	ZED_CT_LIST,
	ZED_CT_LIST_COMMAND,
	ZED_CT_INPUT_TEXT,
	ZED_CT_INPUT_NUMBER,
	ZED_CT_INPUT_FLOAT
};

class ZEDCommand;
class ZEDCommandManager;

typedef ZEEventDelegate<void (const ZEDCommand* Command)> ZEDCommandDelegate;

class ZEDCommandShortcut : public ZEObject
{
	ZE_OBJECT
	public:
		ZEDViewportKeyboardKey				Key;
		ZEDViewportKeyModifiers				Modifier;

											ZEDCommandShortcut(ZEDViewportKeyModifiers Modifier, ZEDViewportKeyboardKey Key);
											ZEDCommandShortcut();
};

class ZEDCommand : public ZEObject
{
	ZE_OBJECT
	friend class ZEDCommandManager;
	private:
		ZEDCommandManager*					Manager;
		ZEString							Name;
		ZEDCommandType						Type;
		ZEString							Text;
		ZEString							Icon;
		ZEValue								Value;
		bool								Enabled;
		bool								Visible;
		ZEDCommandShortcut					Shortcut;
		ZEString							Tooltip;
		ZEArray<ZEString>					ComboBoxItems;
		ZEString							Category;

	public:
		void								SetName(const ZEString& Name);
		const ZEString&						GetName() const;

		void								SetType(ZEDCommandType Type);
		ZEDCommandType						GetType() const;

		void								SetText(const ZEString& Text);
		const ZEString&						GetText() const;

		void								SetIcon(const ZEString& Name);
		const ZEString&						GetIcon() const;

		void								SetCategory(const ZEString& Category);
		const ZEString&						GetCategory() const;

		void								SetEnabled(bool Enabled);
		bool								GetEnabled() const;

		void								SetVisible(bool Visible);
		bool								GetVisible() const;

		void								SetValue(const ZEValue& Value);
		const ZEValue&						GetValue() const;

		void								SetListItems(const ZEArray<ZEString>& Items);
		const ZEArray<ZEString>&			GetListItems() const;

		void								SetTooltip(const ZEString& Tooltip);
		const ZEString&						GetTooltip() const;

		void								SetShortcut(const ZEDCommandShortcut& Shortcut);
		const ZEDCommandShortcut&			GetShortcut() const;

		mutable ZE_EVENT(					OnAction, (const ZEDCommand* Command));
		mutable ZE_EVENT(					OnUpdated, (const ZEDCommand* Command));

											ZEDCommand();
											~ZEDCommand();
};
