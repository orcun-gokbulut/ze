//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAssetModel.h
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

#include <QAbstractItemModel>

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZERegEx/ZEWildcard.h"
#include "ZEDCore/ZEDComponent.h"
#include "ZEDAssetModelColumn.h"

class ZEDAsset;
class ZEDAssetType;
class ZEDAssetDirectory;
class ZEDAssetCategory;
class ZEDAssetManager;

ZE_ENUM(ZEDAssetModelMode)
{
	ZED_AMM_NONE,
	ZED_AMM_TREE,
	ZED_AMM_LIST
};

ZE_ENUM(ZEDAssetModelHierarcy)
{
	ZED_AMH_NONE,
	ZED_AMH_DIRECTORY,
	ZED_AMH_CATAGORY
};

class ZEDAssetModel : public QAbstractItemModel, public ZEDComponent
{
	private:
		ZEString								RootPath;
		ZEDAssetModelMode						Mode;
		ZEDAssetModelHierarcy					Hierarcy;
		ZEArray<ZEDAssetModelColumn>			Columns;
		ZEWildcard								SearchPattern;
		ZEArray<ZEDAssetType*>					IncludeFilter;
		ZEArray<ZEDAssetType*>					ExcludeFilter;
		bool									CloseEvent;

		bool									Filter(ZEObject* AssetObject) const;
		bool									FilterForward(ZEObject* AssetObject) const;
		bool									FilterBackward(ZEObject* AssetObject) const;
		bool									FilterHierarchy(ZEObject* AssetObject) const;

		void									AssetManager_OnAssetAdding(ZEDAssetManager* Manager, ZEDAsset* Asset);
		void									AssetManager_OnAssetAdded(ZEDAssetManager* Manager, ZEDAsset* Asset);
		void									AssetManager_OnAssetRemoving(ZEDAssetManager* Manager, ZEDAsset* Asset);
		void									AssetManager_OnAssetRemoved(ZEDAssetManager* Manager, ZEDAsset* Asset);
		void									AssetManager_OnDirectoryAdding(ZEDAssetManager* Manager, ZEDAssetDirectory* Directory);
		void									AssetManager_OnDirectoryAdded(ZEDAssetManager* Manager, ZEDAssetDirectory* Directory);
		void									AssetManager_OnDirectoryRemoving(ZEDAssetManager* Manager, ZEDAssetDirectory* Directory);
		void									AssetManager_OnDirectoryRemoved(ZEDAssetManager* Manager, ZEDAssetDirectory* Directory);
		void									AssetManager_OnCategoryAdding(ZEDAssetManager* Manager, ZEDAssetCategory* Category);
		void									AssetManager_OnCategoryAdded(ZEDAssetManager* Manager, ZEDAssetCategory* Category);
		void									AssetManager_OnCategoryRemoving(ZEDAssetManager* Manager, ZEDAssetCategory* Category);
		void									AssetManager_OnCategoryRemoved(ZEDAssetManager* Manager, ZEDAssetCategory* Category);

		virtual bool							InitializeInternal() override;
		virtual bool							DeinitializeInternal() override;

	public:
		void									SetMode(ZEDAssetModelMode Mode);
		ZEDAssetModelMode						GetMode() const;

		void									SetHierarcy(ZEDAssetModelHierarcy Hierarycy);
		ZEDAssetModelHierarcy					GetHierarcy() const;

		void									SetRootPath(const ZEString& RootPath);
		const ZEString&							GetRootPath() const;

		void									SetColumns(ZEArray<ZEDAssetModelColumn>& Columns);
		const ZEArray<ZEDAssetModelColumn>&		GetColumns() const;

		void									SetSearchPattern(const ZEString& SearchText);
		const ZEString&							GetSearchPattern() const;

		void									SetIncludeFilter(const ZEArray<ZEDAssetType*>& Filter);
		const ZEArray<ZEDAssetType*>&			GetIncludeFilter() const;

		void									SetExcludeFilter(const ZEArray<ZEDAssetType*>& Filter);
		const ZEArray<ZEDAssetType*>&			GetExcludeFilter() const;

		ZEDAssetManager*						GetAssetManager() const;

		ZEDAssetDirectory*						GetRootDirectory() const;
		ZEDAssetCategory*						GetRootCatagory() const;

		ZEDAsset*								ConvertToAsset(const QModelIndex& Index) const;
		ZEDAssetDirectory*						ConvertToDirectory(const QModelIndex& Index) const;
		ZEDAssetCategory*						ConvertToCategory(const QModelIndex& Index) const;

	private:
		QString									display(const QModelIndex &index) const;
		ZEObject*								indexList(ZEObject* AssetObject, int Row, int& Index) const;
		int										rowCountList(ZEObject* AssetObject) const;


	public: /* QAbstractItemModel */			
		virtual QModelIndex						index(int Row, int Column, const QModelIndex& Parent = QModelIndex()) const override;
		virtual QModelIndex						parent(const QModelIndex& Child) const override;
		virtual bool							hasChildren(const QModelIndex& Parent = QModelIndex()) const;
		virtual int								rowCount(const QModelIndex& Parent = QModelIndex()) const override;
		virtual int								columnCount(const QModelIndex& Parent = QModelIndex()) const override;
		virtual QVariant						data(const QModelIndex& Index, int Role = Qt::DisplayRole) const override;
		virtual QVariant						headerData(int Section, Qt::Orientation Orientation, int Role = Qt::DisplayRole) const override;
		virtual Qt::ItemFlags					flags(const QModelIndex& Index) const; 
		virtual QStringList						mimeTypes() const override;
		virtual QMimeData*						mimeData(const QModelIndexList& Indexes) const override;
		virtual Qt::DropActions					supportedDragActions() const override;

												ZEDAssetModel();
												~ZEDAssetModel();
};
