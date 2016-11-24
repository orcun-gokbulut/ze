//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDMenu.h
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

#include "ZEDS/ZEArray.h"
#include "ZEMeta/ZEEvent.h"

class ZEDMenuItem;
class ZEDMenuManager;
class ZEMLReaderNode;
class ZEMLWriterNode;
class QMenu;

class ZEDMenu : public ZEObject
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEDMenu)
	friend class ZEDMenuItem;
	friend class ZEDMenuManager;
	private:
		ZEDMenuManager*					Manager;
		QMenu*							Menu;
		ZEArray<ZEDMenuItem*>			Items;
		ZEString						Name;
		ZEString						Text;
		ZEString						Icon;
		bool							Protected;
		bool							DeferredSetup;

		void							Setup();
		void							CleanUp();

										ZEDMenu();
										~ZEDMenu();

	public:
		ZEDMenuManager*					GetManager();

		void							SetName(const ZEString&	Name);
		const ZEString&					GetName() const;

		void							SetText(const ZEString&	Name);
		const ZEString&					GetText() const;

		void							SetIcon(const ZEString& Icon);
		const ZEString&					GetIcon() const;

		void							SetProtected(bool Protected);
		bool							GetProtected() const;

		QMenu*							GetNativeMenu();

		const ZEArray<ZEDMenuItem*>&	GetItems() const;
		void							AddItem(ZEDMenuItem* Item);
		void							InsertItem(ZESize Index, ZEDMenuItem* Item);
		void							RemoveItem(ZEDMenuItem* Item);
		void							ClearItems();

		bool							Load(ZEMLReaderNode* MenuNode);
		bool							Save(ZEMLWriterNode* MenusNode);

		ZE_EVENT(						OnUpdated,(const ZEDMenu* Menu));

		virtual void					Destroy();

		static ZEDMenu*					CreateInstance();
};
