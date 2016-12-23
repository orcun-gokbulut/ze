//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDClassModel.h
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

#include "ZEDCore/ZEDComponent.h"

#include "ZEDS/ZEArray.h"
#include "ZEMeta/ZEEnumerator.h"
#include "ZERegEx/ZEWildcard.h"

#include <QAbstractItemModel>

ZE_ENUM(ZEDClassModelMode)
{
	ZED_CMM_NONE,
	ZED_CMM_TREE,
	ZED_CMM_LIST
};


ZE_ENUM(ZEDClassModelHierarchy)
{
	ZED_CMH_NONE,
	ZED_CMH_INHERITANCE,
	ZED_CMH_CATEGORY,
};

class ZEClass;

class ZEDClassModel : public QAbstractItemModel, public ZEDComponent
{
	private:
		ZEClass*							RootClass;
		ZEDClassModelMode					Mode;
		ZEDClassModelHierarchy				Hierarchy;
		bool								ExcludeAbstractClasses;
		ZEWildcard							SearchPattern;
		ZEArray<ZEClass*>					IncludeFilter;
		ZEArray<ZEClass*>					ExcludeFilter;

		bool								Filter(ZEClass* Class) const;
		bool								FilterForward(ZEClass* Class) const;
		bool								FilterBackward(ZEClass* Class) const;
		bool								FilterHierarchy(ZEClass* Class) const;

	public:
		void								SetMode(ZEDClassModelMode Mode);
		ZEDClassModelMode					GetMode() const;

		void								SetHierarchy(ZEDClassModelHierarchy Hierarchy);
		ZEDClassModelHierarchy				GetHierarchy() const;

		void								SetRootClass(ZEClass* Class);
		ZEClass*							GetRootClass() const;

		void								SetSearchPattern(const ZEString& SearchText);
		const ZEString&						GetSearchPattern() const;

		void								SetIncludeFilter(const ZEArray<ZEClass*>& Filter);
		const ZEArray<ZEClass*>&			GetIncludeFilter() const;

		void								SetExcludeFilter(const ZEArray<ZEClass*>& Filter);
		const ZEArray<ZEClass*>&			GetExcludeFilter() const;

		void								SetExcludeAbstractClasses(bool Enabled);
		bool								GetExcludeAbstractClasses() const;

		ZEClass*							ConvertToClass(const QModelIndex& Index) const;

	public: /* QAbstractItemModel */		
		virtual QModelIndex					index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
		virtual QModelIndex					parent(const QModelIndex &child) const override;
		virtual bool						hasChildren(const QModelIndex &parent = QModelIndex()) const;
		virtual int							rowCount(const QModelIndex &parent = QModelIndex()) const override;
		virtual int							columnCount(const QModelIndex &parent = QModelIndex()) const override;
		virtual QVariant					data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
		virtual QVariant					headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
		virtual Qt::ItemFlags				flags(const QModelIndex &index) const; 
		virtual QStringList					mimeTypes() const override;
		virtual QMimeData*					mimeData(const QModelIndexList &indexes) const override;
		virtual Qt::DropActions				supportedDragActions() const override;

											ZEDClassModel();
};
