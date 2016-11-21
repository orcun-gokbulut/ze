//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDToolbarItem.h
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

#include <QObject>
#include <QAction>

class ZEDCommand;
class ZEDToolbarItem;
class ZEDToolbar;
class ZEDMenu;
class QMenu;

enum ZEDToolbarItemType
{
	ZED_TIT_NONE,
	ZED_TIT_COMMAND,
	ZED_TIT_SEPERATOR
};

class ZEDToolbarAction : public QAction
{
	Q_OBJECT
	friend class ZEDToolbar;
	friend class ZEDToolbarItem;
	private:
		ZEDToolbarItem*				Item;

		void							SubAction_triggered(bool Triggered);
		void							Action_triggered(bool Triggered);

										ZEDToolbarAction(ZEDToolbarItem* Item);
};

class ZEDToolbarItem : public ZEObject
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEDToolbarItem)
	friend class ZEDToolbarAction;
	friend class ZEDToolbar;
	private:
		ZEDToolbarAction*				Action;
		ZEDToolbar*						Toolbar;
		ZEDToolbarItemType				Type;
		QMenu*							SubMenu;

		ZEString						TargetName;
		ZEDCommand*						TargetCommand;
		ZEDMenu*						TargetMenu;

		void							Action_Triggered();
		void							SubAction_Triggered(QAction* Action);

		void							TargetCommand_OnUpdate(const ZEDCommand* Command);
		void							TargetMenu_OnUpdate(const ZEDMenu* Menu);

										ZEDToolbarItem();
										~ZEDToolbarItem();

	public:
		ZEDToolbar*						GetToolbar();

		void							SetType(ZEDToolbarItemType Type);
		ZEDToolbarItemType				GetType() const;

		void							SetTargetName(const ZEString& Name);
		const ZEString&					GetTargetName() const;

		void							Update();

		bool							Load(ZEMLReaderNode* ItemNode);
		bool							Save(ZEMLWriterNode* ItemsNode);

		virtual void					Destroy();

		static ZEDToolbarItem*			CreateInstance();

};
