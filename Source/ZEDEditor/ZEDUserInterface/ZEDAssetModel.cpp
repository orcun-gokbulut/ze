//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAssetModel.cpp
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

#include "ZEDAssetModel.h"

#include "ZEDCore/ZEDEditor.h"
#include "ZEDCore/ZEDAssetManager.h"
#include "ZEDCore/ZEDAssetCategory.h"
#include "ZEDCore/ZEDAsset.h"
#include "ZEDCore/ZEDAssetDirectory.h"
#include "ZEDCore/ZEDAssetType.h"
#include "ZEDS/ZEFormat.h"
#include "ZEFile/ZEDirectoryInfo.h"

#include <QIcon>
#include <QMimeData>


bool ZEDAssetModel::Filter(ZEObject* AssetObject) const
{
	if (ZEClass::IsDerivedFrom(ZEDAsset::Class(), AssetObject->GetClass()))
	{
		ZEDAsset* Asset = static_cast<ZEDAsset*>(AssetObject);
		if (!SearchPattern.GetPattern().IsEmpty())
		{
			bool Found = SearchPattern.Match(Asset->GetName());

			if (!Found)
			{
				if (SearchPattern.Match(Asset->GetType()->GetName()))
					Found = true;
			}

			if (!Found)
			{
				const ZEArray<ZEString>& Tags = Asset->GetTags();
				for (ZESize I = 0; I < Tags.GetCount(); I++)
				{
					if (SearchPattern.Match(Tags[I]))
					{
						Found = true;
						break;
					}
				}
			}

			if (!Found)
			{
				if (Asset->GetCategory() != NULL && 
					SearchPattern.Match(Asset->GetCategory()->GetName()))
				{
					Found = true;
				}
			}

			if (!Found)
				return false;
		}


		if (IncludeFilter.GetCount() != 0)
		{
			for (ZESize I = 0; I < IncludeFilter.GetCount(); I++)
			{
				if (!IncludeFilter.Exists(Asset->GetType()))
					return false;
			}
		}

		if (ExcludeFilter.GetCount() != 0)
		{
			for (ZESize I = 0; I < ExcludeFilter.GetCount(); I++)
			{
				if (ExcludeFilter.Exists(Asset->GetType()))
					return false;
			}
		}
	}
	else if (ZEClass::IsDerivedFrom(ZEDAssetDirectory::Class(), AssetObject->GetClass()))
	{
		ZEDAssetDirectory* Directory = static_cast<ZEDAssetDirectory*>(AssetObject);
		if (!SearchPattern.GetPattern().IsEmpty() &&
			!SearchPattern.Match(Directory->GetName()))
		{
			return false;
		}
	}
	else if (ZEClass::IsDerivedFrom(ZEDAssetDirectory::Class(), AssetObject->GetClass()))
	{
		ZEDAssetCategory* Category = static_cast<ZEDAssetCategory*>(AssetObject);
		if (!SearchPattern.GetPattern().IsEmpty() &&
			!SearchPattern.Match(Category->GetName()))
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool ZEDAssetModel::FilterForward(ZEObject* AssetObject) const
{
	if (ZEClass::IsDerivedFrom(ZEDAssetDirectory::Class(), AssetObject->GetClass()))
	{
		ZEDAssetDirectory* Directory = static_cast<ZEDAssetDirectory*>(AssetObject);

		const ZEList2<ZEDAsset>& Assets = Directory->GetAssets();
		ze_for_each(Asset, Assets)
		{
			if (Filter(Asset.GetPointer()))
				return true;
		}

		const ZEList2<ZEDAssetDirectory>& SubDirectories = Directory->GetSubDirectories();
		ze_for_each(SubDirectory, SubDirectories)
		{
			if (Filter(SubDirectory.GetPointer()))
				return true;

			if (FilterForward(SubDirectory.GetPointer()))
				return true;
		}

		return false;
	}
	else if (ZEClass::IsDerivedFrom(ZEDAssetDirectory::Class(), AssetObject->GetClass()))
	{
		ZEDAssetCategory* Category = static_cast<ZEDAssetCategory*>(AssetObject);

		const ZEList2<ZEDAsset>& Assets = Category->GetAssets();
		ze_for_each(Asset, Assets)
		{
			if (Filter(Asset.GetPointer()))
				return true;
		}

		const ZEList2<ZEDAssetCategory>& SubCategories = Category->GetSubCatagories();
		ze_for_each(SubCategory, SubCategories)
		{
			if (Filter(SubCategory.GetPointer()))
				return true;

			if (FilterForward(SubCategory.GetPointer()))
				return true;
		}
	}

	return false;
}

bool ZEDAssetModel::FilterBackward(ZEObject* AssetObject) const
{
	if (AssetObject == NULL)
		return false;

	if (ZEClass::IsDerivedFrom(ZEDAssetDirectory::Class(), AssetObject->GetClass()))
	{
		ZEDAssetDirectory* Directory = static_cast<ZEDAssetDirectory*>(AssetObject);
		if (Filter(Directory))
			return true;
		else
			return FilterBackward(Directory->GetParentDirectory());
	}
	else if (ZEClass::IsDerivedFrom(ZEDAssetDirectory::Class(), AssetObject->GetClass()))
	{
		ZEDAssetCategory* Category = static_cast<ZEDAssetCategory*>(AssetObject);
		if (Filter(Category))
			return true;
		else
			return FilterBackward(Category->GetParentCategory());
	}

	return false;
}

bool ZEDAssetModel::FilterHierarchy(ZEObject* AssetObject) const
{
	if (ZEClass::IsDerivedFrom(ZEDAsset::Class(), AssetObject->GetClass()))
	{
		ZEDAsset* Asset = static_cast<ZEDAsset*>(AssetObject);
		if (Filter(Asset))
			return true;
		
		if (GetHierarcy() == ZED_AMH_DIRECTORY)
		{
			if (FilterBackward(Asset->GetDirectory()))
				return true;
		}
		else if (GetHierarcy() == ZED_AMH_CATAGORY)
		{
			if (FilterBackward(Asset->GetCategory()))
				return true;
		}
	}
	else if (ZEClass::IsDerivedFrom(ZEDAssetDirectory::Class(), AssetObject->GetClass()))
	{
		ZEDAssetDirectory* Directory = static_cast<ZEDAssetDirectory*>(AssetObject);

		if (Filter(Directory))
			return true;
		else if (FilterBackward(Directory->GetParentDirectory()))
			return true;
		else if (FilterForward(Directory))
			return true;
	}
	else if (ZEClass::IsDerivedFrom(ZEDAssetDirectory::Class(), AssetObject->GetClass()))
	{
		ZEDAssetCategory* Category = static_cast<ZEDAssetCategory*>(AssetObject);
		
		if (Filter(Category))
			return true;
		else if (FilterBackward(Category->GetParentCategory()))
			return true;
		else if (FilterForward(Category))
			return true;
	}

	return false;
}

void ZEDAssetModel::AssetManager_OnAssetAdding(ZEDAssetManager* Manager, ZEDAsset* Asset)
{
	if (GetRootDirectory() == NULL)
		return;

	if (!FilterHierarchy(Asset))
		return;

	if (Mode == ZED_AMM_TREE)
	{
		const ZEList2<ZEDAsset>* AssetList = NULL;
		if (GetHierarcy() == ZED_AMH_DIRECTORY)
			AssetList = &Asset->GetDirectory()->GetAssets();
		else if (GetHierarcy() == ZED_AMH_CATAGORY)
			AssetList = &Asset->GetCategory()->GetAssets();
		else
			return;

		int Index = 0;
		ze_for_each(Current, Asset->GetDirectory()->GetAssets())
		{
			if (Asset == Current.GetPointer())
			{
				CloseEvent = true;
				beginInsertRows(parent(createIndex(0, 0, Asset)), Index, Index);
				return;
			}

			if (FilterHierarchy(Current.GetPointer()))
				continue;

			Index++;
		}
	}
	else if (Mode == ZED_AMM_LIST)
	{
		beginResetModel();
		CloseEvent = true;
	}
}

void ZEDAssetModel::AssetManager_OnAssetAdded(ZEDAssetManager* Manager, ZEDAsset* Asset)
{
	if (GetRootDirectory() == NULL)
		return;

	if (!CloseEvent)
		return;

	if (Mode == ZED_AMM_TREE)
		endInsertRows();
	else if (Mode == ZED_AMM_LIST)
		endResetModel();

	CloseEvent = false;
}

void ZEDAssetModel::AssetManager_OnAssetRemoving(ZEDAssetManager* Manager, ZEDAsset* Asset)
{
	if (GetRootDirectory() == NULL)
		return;

	if (!FilterHierarchy(Asset))
		return;

	if (Mode == ZED_AMM_TREE)
	{
		const ZEList2<ZEDAsset>* AssetList = NULL;
		if (GetHierarcy() == ZED_AMH_DIRECTORY)
			AssetList = &Asset->GetDirectory()->GetAssets();
		else if (GetHierarcy() == ZED_AMH_CATAGORY)
			AssetList = &Asset->GetCategory()->GetAssets();
		else
			return;

		int Index = 0;
		ze_for_each(Current, Asset->GetDirectory()->GetAssets())
		{
			if (Asset == Current.GetPointer())
			{
				CloseEvent = true;
				beginRemoveRows(parent(createIndex(0, 0, Asset)), Index, Index);
				return;
			}

			if (FilterHierarchy(Current.GetPointer()))
				continue;

			Index++;
		}
	}
	else if (Mode == ZED_AMM_LIST)
	{
		beginResetModel();
		CloseEvent = true;
	}
}

void ZEDAssetModel::AssetManager_OnAssetRemoved(ZEDAssetManager* Manager, ZEDAsset* Asset)
{
	if (GetRootDirectory() == NULL)
		return;

	if (!CloseEvent)
		return;

	if (Mode == ZED_AMM_TREE)
		endRemoveRows();
	else if (Mode == ZED_AMM_LIST)
		endResetModel();

	CloseEvent = false;
}

void ZEDAssetModel::AssetManager_OnDirectoryAdding(ZEDAssetManager* Manager, ZEDAssetDirectory* Directory)
{
	if (GetRootDirectory() == NULL)
		return;

	if (Hierarcy != ZED_AMH_DIRECTORY || Mode != ZED_AMM_TREE)
		return;

	if (!FilterHierarchy(Directory))
		return;

	int Index = 0;
	ze_for_each(Current, Directory->GetParentDirectory()->GetSubDirectories())
	{
		if (Directory == Current.GetPointer())
		{
			CloseEvent = true;
			beginInsertRows(parent(createIndex(0, 0, Directory)), Index, Index);
			return;
		}

		if (FilterHierarchy(Current.GetPointer()))
			continue;

		Index++;
	}
}

void ZEDAssetModel::AssetManager_OnDirectoryAdded(ZEDAssetManager* Manager, ZEDAssetDirectory* Directory)
{
	if (GetRootDirectory() == NULL)
		return;

	if (Hierarcy != ZED_AMH_DIRECTORY || Mode != ZED_AMM_TREE)
		return;	

	if (!FilterHierarchy(Directory))
		return;


	if (!CloseEvent)
		return;

	endInsertRows();
	CloseEvent = false;
}

void ZEDAssetModel::AssetManager_OnDirectoryRemoving(ZEDAssetManager* Manager, ZEDAssetDirectory* Directory)
{
	if (GetRootDirectory() == NULL)
		return;

	if (Hierarcy != ZED_AMH_DIRECTORY || Mode != ZED_AMM_TREE)
		return;	
	
	if (!FilterHierarchy(Directory))
		return;

	int Index = 0;
	ze_for_each(Current, Directory->GetParentDirectory()->GetSubDirectories())
	{
		if (Directory == Current.GetPointer())
		{
			CloseEvent = true;
			beginRemoveRows(parent(createIndex(0, 0, Directory)), Index, Index);
			return;
		}

		if (FilterHierarchy(Current.GetPointer()))
			continue;

		Index++;
	}
}

void ZEDAssetModel::AssetManager_OnDirectoryRemoved(ZEDAssetManager* Manager, ZEDAssetDirectory* Directory)
{
	if (GetRootDirectory() == NULL)
		return;

	if (Hierarcy != ZED_AMH_DIRECTORY || Mode != ZED_AMM_TREE)
		return;

	if (!FilterHierarchy(Directory))
		return;

	if (!CloseEvent)
		return;

	endRemoveRows();
	CloseEvent = false;
}

void ZEDAssetModel::AssetManager_OnCategoryAdding(ZEDAssetManager* Manager, ZEDAssetCategory* Category)
{
	if (GetRootDirectory() == NULL)
		return;

	if (Hierarcy != ZED_AMH_CATAGORY || Mode != ZED_AMM_TREE)
		return;

	if (!FilterHierarchy(Category))
		return;

	int Index = 0;
	ze_for_each(Current, Category->GetParentCategory()->GetSubCatagories())
	{
		if (Category == Current.GetPointer())
		{
			CloseEvent = true;
			beginInsertRows(parent(createIndex(0, 0, Category)), Index, Index);
			return;
		}

		if (FilterHierarchy(Current.GetPointer()))
			continue;

		Index++;
	}
}

void ZEDAssetModel::AssetManager_OnCategoryAdded(ZEDAssetManager* Manager, ZEDAssetCategory* Category)
{
	if (GetRootDirectory() == NULL)
		return;

	if (Hierarcy != ZED_AMH_CATAGORY || Mode != ZED_AMM_TREE)
		return;

	if (!FilterHierarchy(Category))
		return;

	if (!CloseEvent)
		return;

	endInsertRows();
	CloseEvent = false;
}

void ZEDAssetModel::AssetManager_OnCategoryRemoving(ZEDAssetManager* Manager, ZEDAssetCategory* Category)
{
	if (GetRootDirectory() == NULL)
		return;

	if (Hierarcy != ZED_AMH_CATAGORY || Mode != ZED_AMM_TREE)
		return;

	if (!FilterHierarchy(Category))
		return;

	int Index = 0;
	ze_for_each(Current, Category->GetParentCategory()->GetSubCatagories())
	{
		if (Category == Current.GetPointer())
		{
			CloseEvent = true;
			beginRemoveRows(parent(createIndex(0, 0, Category)), Index, Index);
			return;
		}

		if (FilterHierarchy(Current.GetPointer()))
			continue;

		Index++;
	}
}

void ZEDAssetModel::AssetManager_OnCategoryRemoved(ZEDAssetManager* Manager, ZEDAssetCategory* Category)
{
	if (GetRootDirectory() == NULL)
		return;

	if (Hierarcy != ZED_AMH_CATAGORY || Mode != ZED_AMM_TREE)
		return;

	if (!FilterHierarchy(Category))
		return;

	if (!CloseEvent)
		return;

	endRemoveRows();
	CloseEvent = false;
}

bool ZEDAssetModel::InitializeInternal()
{
	if (!ZEDComponent::InitializeInternal())
		return false;

	GetEditor()->GetAssetManager()->OnAssetAdding.AddDelegate<ZEDAssetModel, &ZEDAssetModel::AssetManager_OnAssetAdding>(this);
	GetEditor()->GetAssetManager()->OnAssetAdded.AddDelegate<ZEDAssetModel, &ZEDAssetModel::AssetManager_OnAssetAdded>(this);
	GetEditor()->GetAssetManager()->OnAssetRemoving.AddDelegate<ZEDAssetModel, &ZEDAssetModel::AssetManager_OnAssetRemoving>(this);
	GetEditor()->GetAssetManager()->OnAssetRemoved.AddDelegate<ZEDAssetModel, &ZEDAssetModel::AssetManager_OnAssetRemoved>(this);
	GetEditor()->GetAssetManager()->OnDirectoryAdding.AddDelegate<ZEDAssetModel, &ZEDAssetModel::AssetManager_OnDirectoryAdding>(this);
	GetEditor()->GetAssetManager()->OnDirectoryAdded.AddDelegate<ZEDAssetModel, &ZEDAssetModel::AssetManager_OnDirectoryAdded>(this);
	GetEditor()->GetAssetManager()->OnDirectoryRemoving.AddDelegate<ZEDAssetModel, &ZEDAssetModel::AssetManager_OnDirectoryRemoving>(this);
	GetEditor()->GetAssetManager()->OnDirectoryRemoved.AddDelegate<ZEDAssetModel, &ZEDAssetModel::AssetManager_OnDirectoryRemoved>(this);
	GetEditor()->GetAssetManager()->OnCategoryAdding.AddDelegate<ZEDAssetModel, &ZEDAssetModel::AssetManager_OnCategoryAdding>(this);
	GetEditor()->GetAssetManager()->OnCategoryAdded.AddDelegate<ZEDAssetModel, &ZEDAssetModel::AssetManager_OnCategoryAdded>(this);
	GetEditor()->GetAssetManager()->OnCategoryRemoving.AddDelegate<ZEDAssetModel, &ZEDAssetModel::AssetManager_OnCategoryRemoving>(this);
	GetEditor()->GetAssetManager()->OnCategoryRemoved.AddDelegate<ZEDAssetModel, &ZEDAssetModel::AssetManager_OnCategoryRemoved>(this);

	return true;
}

bool ZEDAssetModel::DeinitializeInternal()
{
	GetEditor()->GetAssetManager()->OnAssetAdding.DisconnectObject(this);
	GetEditor()->GetAssetManager()->OnAssetAdded.DisconnectObject(this);
	GetEditor()->GetAssetManager()->OnAssetRemoving.DisconnectObject(this);
	GetEditor()->GetAssetManager()->OnAssetRemoved.DisconnectObject(this);
	GetEditor()->GetAssetManager()->OnDirectoryAdding.DisconnectObject(this);
	GetEditor()->GetAssetManager()->OnDirectoryAdded.DisconnectObject(this);
	GetEditor()->GetAssetManager()->OnDirectoryRemoving.DisconnectObject(this);
	GetEditor()->GetAssetManager()->OnDirectoryRemoved.DisconnectObject(this);
	GetEditor()->GetAssetManager()->OnCategoryAdding.DisconnectObject(this);
	GetEditor()->GetAssetManager()->OnCategoryAdded.DisconnectObject(this);
	GetEditor()->GetAssetManager()->OnCategoryRemoving.DisconnectObject(this);
	GetEditor()->GetAssetManager()->OnCategoryRemoved.DisconnectObject(this);

	return ZEDComponent::DeinitializeInternal();
}

void ZEDAssetModel::SetMode(ZEDAssetModelMode Mode)
{
	if (this->Mode == Mode)
		return;

	beginResetModel();
	this->Mode = Mode;
	endResetModel();
}

ZEDAssetModelMode ZEDAssetModel::GetMode() const
{
	return Mode;
}

void ZEDAssetModel::SetHierarcy(ZEDAssetModelHierarcy Hierarcy)
{
	if (this->Hierarcy == Hierarcy)
		return;

	beginResetModel();
	this->Hierarcy = Hierarcy;
	endResetModel();
}

ZEDAssetModelHierarcy ZEDAssetModel::GetHierarcy() const
{
	return Hierarcy;
}

void ZEDAssetModel::SetRootPath(const ZEString& Path)
{
	if (RootPath == Path)
		return;

	beginResetModel();
	RootPath = Path;
	endResetModel();
}

const ZEString& ZEDAssetModel::GetRootPath() const
{
	return RootPath;
}

void ZEDAssetModel::SetColumns(ZEArray<ZEDAssetModelColumn>& Columns)
{
	beginResetModel();
	this->Columns = Columns;
	endResetModel();
}

const ZEArray<ZEDAssetModelColumn>& ZEDAssetModel::GetColumns() const
{
	return Columns;
}

void ZEDAssetModel::SetSearchPattern(const ZEString& Pattern)
{
	if (SearchPattern.GetPattern() == Pattern)
		return;

	beginResetModel();
	SearchPattern.Compile(Pattern);
	endResetModel();
}

const ZEString& ZEDAssetModel::GetSearchPattern() const
{
	return SearchPattern.GetPattern();
}

void ZEDAssetModel::SetIncludeFilter(const ZEArray<ZEDAssetType*>& Filter)
{
	if (IncludeFilter.GetSize() == Filter.GetSize())
	{
		bool Found = false;
		for (ZESize I = 0; I < Filter.GetCount(); I++)
		{
			if (!IncludeFilter.Exists(Filter[I]))
				Found = true;
		}

		if (!Found)
			return;
	}

	beginResetModel();
	IncludeFilter = Filter;
	endResetModel();
}

const ZEArray<ZEDAssetType*>& ZEDAssetModel::GetIncludeFilter() const
{
	return IncludeFilter;
}

void ZEDAssetModel::SetExcludeFilter(const ZEArray<ZEDAssetType*>& Filter)
{
	if (ExcludeFilter.GetSize() == Filter.GetSize())
	{
		bool Found = false;
		for (ZESize I = 0; I < Filter.GetCount(); I++)
		{
			if (!ExcludeFilter.Exists(Filter[I]))
				Found = true;
		}

		if (!Found)
			return;
	}

	beginResetModel();
	ExcludeFilter = Filter;
	endResetModel();
}

const ZEArray<ZEDAssetType*>& ZEDAssetModel::GetExcludeFilter() const
{
	return ExcludeFilter;
}

ZEDAssetManager* ZEDAssetModel::GetAssetManager() const
{
	ZEDEditor* Editor = GetEditor();
	if (Editor == NULL)
		return NULL;

	return Editor->GetAssetManager();
}

ZEDAssetDirectory* ZEDAssetModel::GetRootDirectory() const
{
	if (GetHierarcy() != ZED_AMH_DIRECTORY)
		return NULL;

	if (GetAssetManager() == NULL)
		return NULL;

	return GetAssetManager()->GetDirectory(ZEFormat::Format("{0}/{1}", GetAssetManager()->GetResourcePath(), RootPath));
}

ZEDAssetCategory* ZEDAssetModel::GetRootCatagory() const
{
	if (GetHierarcy() != ZED_AMH_CATAGORY)
		return NULL;

	if (GetAssetManager() == NULL)
		return NULL;

	return GetAssetManager()->GetCategory(RootPath);
}

ZEDAsset* ZEDAssetModel::ConvertToAsset(const QModelIndex& Index) const
{
	ZEObject* Object = static_cast<ZEObject*>(Index.internalPointer());
	if (!ZEClass::IsDerivedFrom(ZEDAsset::Class(), Object->GetClass()))
		return NULL;

	return static_cast<ZEDAsset*>(Index.internalPointer());
}

ZEDAssetDirectory* ZEDAssetModel::ConvertToDirectory(const QModelIndex& Index) const
{
	ZEObject* Object = static_cast<ZEObject*>(Index.internalPointer());
	if (!ZEClass::IsDerivedFrom(ZEDAssetDirectory::Class(), Object->GetClass()))
		return NULL;

	return static_cast<ZEDAssetDirectory*>(Index.internalPointer());
}

ZEDAssetCategory* ZEDAssetModel::ConvertToCategory(const QModelIndex& Index) const
{
	ZEObject* Object = static_cast<ZEObject*>(Index.internalPointer());
	if (!ZEClass::IsDerivedFrom(ZEDAssetCategory::Class(), Object->GetClass()))
		return NULL;

	return static_cast<ZEDAssetCategory*>(Index.internalPointer());
}

QString ZEDAssetModel::display(const QModelIndex& Index) const
{
	const ZEDAssetModelColumn& Column = Columns[Index.column()];

	ZEDAsset* Asset = ConvertToAsset(Index);
	if (Asset != NULL)
	{
		switch (Column.GetType())
		{
			case ZED_AMHT_NAME:
				return Asset->GetName().ToCString();

			case ZED_AMHT_TYPE:
				return Asset->GetType()->GetName();

			case ZED_AMHT_PATH:
				return Asset->GetPath().ToCString();

			case ZED_AMHT_PARENT:
				if (Hierarcy == ZED_AMH_DIRECTORY)
				{
					if (Asset->GetDirectory() != NULL)
						return Asset->GetDirectory()->GetName().ToCString();
				}
				else if (Hierarcy == ZED_AMH_CATAGORY)
				{
					if (Asset->GetCategory() != NULL)
						return Asset->GetCategory()->GetName().ToCString();
				}
	
				return "";

			case ZED_AMHT_SIZE:
				return QString::number(Asset->GetFileSize());

			case ZED_AMHT_LAST_MODIFICATION_TIME:
				return Asset->GetModificationTime().ToString().ToCString();

			case ZED_AMHT_CATEGORY:
				if (Asset->GetCategory() != NULL)
					return Asset->GetCategory()->GetName().ToCString();
				else
					return "";

			case ZED_AMHT_TAGS:
			{
				const ZEArray<ZEString>& Tags = Asset->GetTags();
				ZEString TagsString;
				for (ZESize I = 0; I < Tags.GetCount(); I++)
				{
					if (I == 0)
						TagsString = Tags[0];
					else
						TagsString = ZEFormat::Format("{0}, {1}", TagsString, Tags[I]);
				}

				return TagsString.ToCString();
			}

			default:
				return "";
		}
	}

	ZEDAssetDirectory* Directory = ConvertToDirectory(Index);
	if (Directory != NULL)
	{
		switch (Column.GetType())
		{
			case ZED_AMHT_NAME:
				return Directory->GetName().ToCString();

			case ZED_AMHT_TYPE:
				return "Directory";

			case ZED_AMHT_PATH:
				return Directory->GetPath().ToCString();

			case ZED_AMHT_PARENT:
				if (Hierarcy == ZED_AMH_DIRECTORY)
				{
					if (Directory->GetParentDirectory() != NULL)
						return Directory->GetParentDirectory()->GetName().ToCString();
				}
				
				return "";

			case ZED_AMHT_LAST_MODIFICATION_TIME:
				return ZEDirectoryInfo(Directory->GetPath()).GetModificationTime().ToString().ToCString();

			default:
				return "";
		}
	}

	ZEDAssetCategory* Category = ConvertToCategory(Index);
	if (Category != NULL)
	{
		switch (Column.GetType())
		{
			case ZED_AMHT_NAME:
				return Category->GetName().ToCString();

			case ZED_AMHT_TYPE:
				return "Category";

			case ZED_AMHT_PATH:
				return Category->GetPath().ToCString();

			case ZED_AMHT_PARENT:
				if (Hierarcy == ZED_AMH_DIRECTORY)
				{
					if (Category->GetParentCategory() != NULL)
						return Category->GetParentCategory()->GetName().ToCString();
				}

				return "";

			default:
				return "";
		}
	}

	return "";
}

ZEObject* ZEDAssetModel::indexList(ZEObject* AssetObject, int Row, int& Index) const
{
	int Count = 0;
	if (ZEClass::IsDerivedFrom(ZEDAsset::Class(), AssetObject->GetClass()))
	{
		if (!Filter(AssetObject))
			return NULL;

		if (Row == Index)
			return AssetObject;

		Index++;
	}
	else if (ZEClass::IsDerivedFrom(ZEDAssetDirectory::Class(), AssetObject->GetClass()))
	{
		ZEDAssetDirectory* Directory = static_cast<ZEDAssetDirectory*>(AssetObject);

		const ZEList2<ZEDAssetDirectory>& SubDirectories = Directory->GetSubDirectories();
		ze_for_each(SubDirectory, SubDirectories)
		{
			ZEObject* Result = indexList(SubDirectory.GetPointer(), Row, Index);
			if (Result != NULL)
				return Result;
		}

		const ZEList2<ZEDAsset>& Assets = Directory->GetAssets();
		ze_for_each(Asset, Assets)
		{
			ZEObject* Result = indexList(Asset.GetPointer(), Row, Index);
			if (Result != NULL)
				return Result;
		}
	}

	return NULL;
}

int ZEDAssetModel::rowCountList(ZEObject* AssetObject) const
{
	int Count = 0;
	if (ZEClass::IsDerivedFrom(ZEDAsset::Class(), AssetObject->GetClass()))
	{
		if (Filter(AssetObject))
			Count++;
	}
	else if (ZEClass::IsDerivedFrom(ZEDAssetDirectory::Class(), AssetObject->GetClass()))
	{
		ZEDAssetDirectory* Directory = static_cast<ZEDAssetDirectory*>(AssetObject);

		const ZEList2<ZEDAssetDirectory>& SubDirectories = Directory->GetSubDirectories();
		ze_for_each(SubDirectory, SubDirectories)
			Count += rowCountList(SubDirectory.GetPointer());

		const ZEList2<ZEDAsset>& Assets = Directory->GetAssets();
		ze_for_each(Asset, Assets)
			Count += rowCountList(Asset.GetPointer());
	}
	
	return Count;
}

QModelIndex ZEDAssetModel::index(int Row, int Column, const QModelIndex& Parent) const
{
	if (GetAssetManager() == NULL)
		return QModelIndex();

	if (Mode == ZED_AMM_TREE && Hierarcy == ZED_AMH_DIRECTORY)
	{
		if (!Parent.isValid())
		{
			if (Row != 0)
				return QModelIndex();

			ZEDAssetDirectory* RootDirectort = GetRootDirectory();
			return (RootDirectort != NULL ? createIndex(0, Column, RootDirectort) : QModelIndex());
		}

		ZEDAssetDirectory* ParentDirectory = ConvertToDirectory(Parent);
		if (ParentDirectory == NULL)
			return QModelIndex();

		// SubDirectories
		ZESize Index = 0;
		ze_for_each(SubDirectory, ParentDirectory->GetSubDirectories())
		{
			if (!FilterHierarchy(SubDirectory.GetPointer()))
				continue;

			if (Index == Row)
				return createIndex(Row, Column, SubDirectory.GetPointer());

			Index++;
		}

		// Assets
		ze_for_each(Asset, ParentDirectory->GetAssets())
		{
			if (!FilterHierarchy(Asset.GetPointer()))
				continue;

			if (Index == Row)
				return createIndex(Row, Column, Asset.GetPointer());

			Index++;
		}
	}
	else if (Mode == ZED_AMM_LIST && Hierarcy == ZED_AMH_DIRECTORY)
	{
		int Index = 0;
		ZEObject* Object = indexList(GetRootDirectory(), Row, Index);
		if (Object == NULL)
			return QModelIndex();

		return  createIndex(Row, Column, Object);
	}
	
	return QModelIndex();
}

QModelIndex ZEDAssetModel::parent(const QModelIndex& Child) const
{
	if (GetAssetManager() == NULL)
		return QModelIndex();

	if (Mode == ZED_AMM_TREE && Hierarcy == ZED_AMH_DIRECTORY)
	{
		ZEDAssetDirectory* ParentDirectory = NULL;
		ZEDAssetDirectory* ChildDirectory = ConvertToDirectory(Child);
		if (ChildDirectory == NULL)
		{
			ZEDAsset* ChildAsset = ConvertToAsset(Child);
			if (ChildAsset == NULL)
				return QModelIndex();

			ParentDirectory = ChildAsset->GetDirectory();
		}
		else
		{
			ParentDirectory = ChildDirectory->GetParentDirectory();
		}

		if (ParentDirectory == NULL)
			return QModelIndex();

		ZEDAssetDirectory* GrandParentDirectory = ParentDirectory->GetParentDirectory();
		if (GrandParentDirectory == NULL)
			return createIndex(0, 0, ParentDirectory);

		
		ZESize Index = 0;
		ze_for_each(CurrentDirectory, GrandParentDirectory->GetSubDirectories())
		{
			if (!FilterHierarchy(CurrentDirectory.GetPointer()))
				continue;

			if (CurrentDirectory.GetPointer() == ParentDirectory)
				return createIndex((int)Index, 0, ParentDirectory);
			Index++;
		}
	}

	return QModelIndex();
}

bool ZEDAssetModel::hasChildren(const QModelIndex& Parent) const
{
	if (GetAssetManager() == NULL)
		return false;

	if (Mode == ZED_AMM_TREE && Hierarcy == ZED_AMH_DIRECTORY)
	{
		if (!Parent.isValid())
			return (GetRootDirectory() == NULL ? false : true);

		if (Parent.column() != 0)
			return false;

		ZEDAssetDirectory* ParentDirectory = ConvertToDirectory(Parent);
		if (ParentDirectory == NULL)
			return false;

		// SubDirectories
		ze_for_each(SubDirectory, ParentDirectory->GetSubDirectories())
		{
			if (!FilterHierarchy(SubDirectory.GetPointer()))
				continue;

			return true;
		}

		// Assets
		ze_for_each(Asset, ParentDirectory->GetAssets())
		{
			if (!FilterHierarchy(Asset.GetPointer()))
				continue;

			return true;
		}
	}
	else if (Mode == ZED_AMM_LIST && Hierarcy == ZED_AMH_DIRECTORY)
	{
		if (Parent.isValid())
			return false;

		int Index = 0;
		return (indexList(GetRootDirectory(), 0, Index) != 0);
	}

	return false;
}

int ZEDAssetModel::rowCount(const QModelIndex& Parent) const
{
 	if (GetAssetManager() == NULL)
		return 0;

	if (Mode == ZED_AMM_TREE && Hierarcy == ZED_AMH_DIRECTORY)
	{
		if (!Parent.isValid())
			return (GetRootDirectory() == NULL ? 0 : 1);
		
		if (Parent.column() != 0)
			return 0;
		
		ZEDAssetDirectory* ParentDirectory = ConvertToDirectory(Parent);
		if (ParentDirectory == NULL)
			return 0;

		// SubDirectories
		ZESize Count = 0;
		ze_for_each(SubDirectory, ParentDirectory->GetSubDirectories())
		{
			if (!FilterHierarchy(SubDirectory.GetPointer()))
				continue;

			Count++;
		}

		// Assets
		ze_for_each(Asset, ParentDirectory->GetAssets())
		{
			if (!FilterHierarchy(Asset.GetPointer()))
				continue;

			Count++;
		}

		return (int)Count;
	}
	else if (Mode == ZED_AMM_LIST && Hierarcy == ZED_AMH_DIRECTORY)
	{
		if (Parent.isValid())
			return 0;
		else
			return rowCountList(GetRootDirectory());
	}

	return 0;
}

int ZEDAssetModel::columnCount(const QModelIndex& Parent) const
{
	return (int)Columns.GetCount();
}

QVariant ZEDAssetModel::data(const QModelIndex& Index, int Role) const
{
	if (Index.column() >= Columns.GetCount())
		return QVariant();

	switch (Role)
	{
		case Qt::DisplayRole:
			return display(Index);

		case Qt::DecorationRole:
			if (Columns[Index.column()].GetType() == ZED_AMHT_NAME)
			{
				QString IconFile;
	
				ZEDAsset* Asset = ConvertToAsset(Index);
				if (Asset != NULL)
				{
					ZEString AssetExtension = ZEPathInfo(Asset->GetName()).GetExtension();
					IconFile = ZEPathInfo(Asset->GetType()->GetIconPath(AssetExtension)).GetRealPath().Path;
				}
				else if (ConvertToDirectory(Index))
				{
					IconFile = ZEPathInfo("#R:/ZEDEditor/Icons/ZEDAsset/ZEDAssetDirectory-Opened.png").GetRealPath().Path;
				}
				else if (!ConvertToCategory(Index))
				{
					IconFile = ZEPathInfo("#R:/ZEDEditor/Icons/ZEDAsset/ZEDAssetCategory.png").GetRealPath().Path;
				}

				if (IconFile.isEmpty())
					return QIcon();
				else
					return QIcon(IconFile);
			}
			break;

		case Qt::TextAlignmentRole:
			return (int)Columns[Index.column()].GetAlignmnent();
	}

	return QVariant();
}

QVariant ZEDAssetModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
	if (Orientation == Qt::Vertical)
		return QVariant();

	if (Section >= Columns.GetCount())
		return QVariant();

	switch (Role)
	{
		case Qt::DisplayRole:
			return Columns[Section].GetHeaderText().ToCString();

		case Qt::TextAlignmentRole:
			return (int)Columns[Section].GetAlignmnent();
	}
	
	return QVariant();
}

Qt::ItemFlags ZEDAssetModel::flags(const QModelIndex& Index) const
{
	
	if (Index.column() < Columns.GetCount())
	{
		Qt::ItemFlags Flags = QAbstractItemModel::flags(Index);

		ZEDAsset* Asset = ConvertToAsset(Index);
		if (Asset != NULL)
			Flags |= Qt::ItemIsDragEnabled;
		else
			Flags &= ~Qt::ItemIsDragEnabled;

		if (Columns[Index.column()].GetSelectable())
			Flags |= Qt::ItemIsSelectable;
		else
			Flags &= ~Qt::ItemIsSelectable;

		return Flags;
	}
	else
	{
		return  QAbstractItemModel::flags(Index);
	}
}

QStringList ZEDAssetModel::mimeTypes() const
{
	QStringList List;
	List << "application/x.zinek.zeasset";
	return List;
}

QMimeData* ZEDAssetModel::mimeData(const QModelIndexList& Indexes) const
{
	if (Indexes.count() != 1)
		return NULL;

	ZEDAsset* Asset = ConvertToAsset(Indexes[0]);
	QMimeData* Data = new QMimeData();
	//Data->setText(Asset->GetPath().ToCString());
	Data->setData("application/x.zinek.zedasset", QByteArray((const char*)&Asset, sizeof(ZEDAsset*)));
	return Data;
}

Qt::DropActions ZEDAssetModel::supportedDragActions() const
{
	return Qt::DropAction::CopyAction | Qt::DropAction::MoveAction | Qt::DropAction::LinkAction;
}

ZEDAssetModel::ZEDAssetModel()
{
	Mode = ZED_AMM_TREE;
	CloseEvent = false;
}

ZEDAssetModel::~ZEDAssetModel()
{
	Deinitialize();
}
