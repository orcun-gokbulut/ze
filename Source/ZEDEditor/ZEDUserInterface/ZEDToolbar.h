//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDToolbar.h
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

class ZEDToolbarItem;
class ZEDToolbarManager;
class ZEMLReaderNode;
class ZEMLWriterNode;
class QToolBar;

ZE_ENUM(ZEDToolbarDockLocation)
{
	ZED_TDL_LEFT,
	ZED_TDL_RIGHT,
	ZED_TDL_TOP,
	ZED_TDL_BOTTOM
};

class ZEDToolbar : public ZEObject
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEDToolbar)
	friend class ZEDToolbarManager;
	private:
		ZEDToolbarManager*				Manager;
		QToolBar*						Toolbar;
		ZEArray<ZEDToolbarItem*>		Items;
		ZEString						Name;
		ZEString						Text;
		ZEString						Icon;
		bool							Visible;
		ZEDToolbarDockLocation			DockLocation;
		ZEUInt							DockColumn;
		ZEUInt							DockRow;

										ZEDToolbar();
										~ZEDToolbar();

	public:
		ZEDToolbarManager*				GetManager();

		void							SetName(const ZEString&	Name);
		const ZEString&					GetName() const;

		void							SetText(const ZEString&	Name);
		const ZEString&					GetText() const;

		void							SetIcon(const ZEString& Icon);
		const ZEString&					GetIcon() const;

		void							SetVisible(bool Visible);
		bool							GetVisible() const;

		void							SetDockLocation(ZEDToolbarDockLocation Position);
		ZEDToolbarDockLocation			GetDockLocation() const;

		void							SetDockColumn(ZEUInt Column);
		ZEUInt							GetDockColumn() const;

		void							SetDockRow(ZEUInt Row);
		ZEUInt							GetDockRow() const;


		QToolBar*						GetNativeToolbar();

		const ZEArray<ZEDToolbarItem*>&	GetItems() const;
		void							AddItem(ZEDToolbarItem* Item);
		void							InsertItem(ZESize Index, ZEDToolbarItem* Item);
		void							RemoveItem(ZEDToolbarItem* Item);
		void							ClearItems();

		bool							Load(ZEMLReaderNode* Reader);
		bool							Save(ZEMLWriterNode* WriterNode);

		ZE_EVENT(						OnUpdated,(ZEDToolbar* Menu));

		void							Update();

		virtual void					Destroy();

		static ZEDToolbar*				CreateInstance();
};
