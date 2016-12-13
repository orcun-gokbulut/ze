//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDObjectModel.h
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

#include "ZEDS/ZEArray.h"
#include "ZERegEx/ZEWildcard.h"

#include <QRegExp>
#include <QAbstractItemModel>

class ZEClass;
class ZEDObjectWrapper;

enum ZEDObjectTreeMode
{
	ZED_OTM_NONE,
	ZED_OTM_LIST,
	ZED_OTM_TREE
};

// Read Only
	// Tree
	// List

// Write
// Drag and Drop Support

class ZEDObjectModel : public QAbstractItemModel, public ZEDComponent
{
	private:
		ZEDObjectWrapper*				RootWrapper;
		ZEDObjectTreeMode				Mode;

		ZEWildcard						FilterSearch;
		ZEArray<ZEObject*>				FilterIncludedObjects;
		ZEArray<ZEObject*>				FilterExcludedObjects;
		ZEArray<ZEClass*>				FilterIncludedClasses;
		ZEArray<ZEClass*>				FilterExcludedClasses;

		bool							Filter(ZEDObjectWrapper* Wrapper) const;
		bool							FilterHierarchy(ZEDObjectWrapper* Wrapper) const;

	public:
		void							SetRootWrapper(ZEDObjectWrapper* Wrapper);
		ZEDObjectWrapper*				GetRootWrapper() const;

		void							SetMode(ZEDObjectTreeMode Mode);
		ZEDObjectTreeMode				GetMode() const;
		
		void							SetFilterPattern(const ZEString& Text);
		const ZEString&					GetFilterPattern() const;

		void							SetFilterIncludedObjects(const ZEArray<ZEObject*>& Objects);
		const ZEArray<ZEObject*>&		GetFilterIncludedObjects() const;

		void							SetFilterExcludedObjects(const ZEArray<ZEObject*>& Objects);
		const ZEArray<ZEObject*>&		GetFilterExcludedObjects() const;

		void							SetFilterIncludedClasses(const ZEArray<ZEClass*>& Clases);
		const ZEArray<ZEClass*>&		GetFilterIncludedClasses() const;

		void							SetFilterExclucedClasses(const ZEArray<ZEClass*>& Classes);
		const ZEArray<ZEClass*>&		GetFilterExcludedClasses() const;

		ZEDObjectWrapper*				ConvertToWrapper(const QModelIndex& Index) const;

	private: /* Events */
		virtual void					ObjectEvent(const ZEDObjectEvent* Event);

	public: /* QAbstractItemModel */
		virtual ZEDObjectWrapper*		indexList(ZEDObjectWrapper* Target, int Row, int& Index) const;
		virtual QModelIndex				index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
		virtual QModelIndex				parent(const QModelIndex &child) const override;
		virtual bool					hasChildren(const QModelIndex &parent = QModelIndex()) const;
		virtual int						rowCountList(ZEDObjectWrapper* Root) const;
		virtual int						rowCount(const QModelIndex &parent = QModelIndex()) const override;
		virtual int						columnCount(const QModelIndex &parent = QModelIndex()) const override;
		virtual QVariant				data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
		virtual QVariant				headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

										ZEDObjectModel();
};
