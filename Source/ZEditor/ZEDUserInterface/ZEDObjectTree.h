//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDObjectTree.h
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

#include "ZEDCore/ZEDComponent.h"
#include <QTreeWidget>

#include "ZEDS/ZEArray.h"
#include <QRegExp>

class ZEClass;
class ZEDObjectWrapper;
class ZEDSelectionManager;

enum ZEDObjectTreeMode
{
	ZED_OTM_NONE,
	ZED_OTM_LIST,
	ZED_OTM_TREE
};

class ZEDObjectTree : public QTreeWidget, public ZEDComponent
{
	Q_OBJECT
	private:
		ZEDObjectWrapper*				RootWrapper;
		ZEDObjectTreeMode				Mode;

		QRegExp							FilterSearch;
		ZEArray<ZEClass*>				FilterIncludedClasses;
		ZEArray<ZEClass*>				FilterExcludedClasses;

		QTreeWidgetItem*				FindItem(QTreeWidgetItem* Parent, ZEDObjectWrapper* Wrapper)const;

		bool							FilterWrapper(ZEDObjectWrapper* Wrapper);

		bool							CheckWrapper(ZEDObjectWrapper* Wrapper);
		void							AddWrapper(ZEDObjectWrapper* Wrapper);
		void							RemoveWrapper(ZEDObjectWrapper* Wrapper);
		
		void							UpdateItem(QTreeWidgetItem* TreeItem, ZEDObjectWrapper* Wrapper);

		virtual bool					InitializeSelf();

		virtual void					ObjectEvent(const ZEDObjectEvent* Event);
		virtual void					SelectionEvent(const ZEDSelectionEvent* Event);

	private slots:
		void							OnSelectionChanged();

	public:
		void							SetRootWrapper(ZEDObjectWrapper* Wrapper);
		ZEDObjectWrapper*				GetRootWrapper() const;

		void							SetMode(ZEDObjectTreeMode Mode);
		ZEDObjectTreeMode				GetMode() const;


		ZEDObjectWrapper*				GetWrapper(QTreeWidgetItem* Item) const;
		QTreeWidgetItem*				GetTreeItem(ZEDObjectWrapper* Wrapper) const;

		void							SetFilterPattern(const QString& Text);
		QString							GetFilterPattern() const;

		const ZEArray<ZEClass*>			GetFilterIncludedClasses() const;
		void							AddFilterIncludedClass(ZEClass* Class);
		void							RemoveFilterIncludedClass(ZEClass* Class);

		const ZEArray<ZEClass*>			GetFilterExcludedClasses() const;
		void							AddFilterExcludedClass(ZEClass* Class);
		void							RemoveFilterExcludedClass(ZEClass* Class);

		void							Update();

										ZEDObjectTree(QWidget* Parent = 0);
};
