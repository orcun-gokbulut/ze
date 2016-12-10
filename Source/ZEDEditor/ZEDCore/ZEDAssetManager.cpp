//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAssetManager.cpp
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

#include "ZEDAssetManager.h"

#include "ZEDAsset.h"
#include "ZEDAssetType.h"

#include "ZEDS/ZEFormat.h"
#include "ZEFile/ZEDirectoryInfo.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZERegEx/ZERegEx.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#include "ZERandom.h"
class ZEDTextureAssetType : public ZEDAssetType
{
	virtual const char* GetName() const 
	{
		return "Texture"; 
	}

	virtual const char*const* GetExtensions() const 
	{
		static const char* A[] = {".bmp", ".png", ".dds", ".tif", ".tiff", ".tga", ".dds"}; 
		return A;
	}
	
	virtual ZESize GetExtensionCount() const
	{
		return 7;
	}
	
	virtual bool Wrap(ZEDAsset* Asset, const ZEString& Path)
	{
		Asset->SetType(this);
		Asset->SetPath(Path);
		
		switch(ZERandom::GetInt() % 10)
		{
			case 0:
				Asset->SetCategory(" Station Ankara  . Models");
				break;

			case 1:
				Asset->SetCategory("Station Ankara . Texture ");
				break;

			case 2:
				Asset->SetCategory("Eskisehir .");
				break;

			case 3:
				Asset->SetCategory("     Eskisehir. A");
				break;

			case 4:
				Asset->SetCategory("  Eskisehir  .	B S");
				break;

			case 5:
				Asset->SetCategory("Eskisehir  .	B S . AA");
				break;

			case 6:
				Asset->SetCategory("SSS ");
				break;
		}

		return true;
	}
};

void ZEDAssetManager::UpdateCachedAsset(const ZEString& Path)
{
	ZEPathInfo PathInfo(Path);
	ZEString PathNormalized = PathInfo.Normalize();
	ZEUInt PathHash = PathNormalized.Hash();
	
	Assets.LockWriteNested();
	{
		ZETimeStamp ModificiationTime = PathInfo.GetModificationTime();
		bool PathExists = PathInfo.IsExists();

		for (ZESize I = 0; I < Assets.GetCount(); I++)
		{
			ZEDAsset* Asset = Assets[I];
			if (Asset->GetHash() != PathHash || Asset->GetPath() != PathNormalized)
				continue;

			if (!PathExists)
			{
				RemoveAssetFromCache(Asset);
				delete Asset;
			}
			else
			{
				if (Asset->GetModificationTime() != ModificiationTime)
				{
					ZEDAsset NewAsset;
					WrapAsset(&NewAsset, Path);
					if (NewAsset.GetType() == NULL)
					{
						RemoveAssetFromCache(Asset);
						delete Asset;
					}
					else
					{
						UpdateAssetInCache(I, NewAsset);
					}
				}
			}

			Assets.UnlockWrite();
			return;
		}

		if (PathExists)
		{
			ZEDAsset* NewAsset = new ZEDAsset();
			WrapAsset(NewAsset, Path);
			if (NewAsset->GetType() != NULL)
				AddAssetToCache(NewAsset);
			else
				delete NewAsset;
		}
	}
	Assets.UnlockWrite();
}

void ZEDAssetManager::CrawlerFunction(ZEThread* Thread, void* ExtraParameters)
{
	while(Thread->ControlPoint())
	{
		if (CrawlLocations.GetCount() == 0)
		{
			Sleep(200);
			continue;
		}

		ZEString Location = CrawlLocations.Pop();
		if (ZEDirectoryInfo(Location).IsDirectory())
		{
			ZEArray<ZEDAsset> Assets = GetAssetsInDirectory(Location, true, false);
			for (ZESize I = 0; I < Assets.GetCount(); I++)
				UpdateCachedAsset(Assets[I].GetPath());
		}
		else
		{
			UpdateCachedAsset(Location);
		}
	}
}

void ZEDAssetManager::MonitorFunction(ZEThread* Thread, void* ExtraParameters)
{
	ZEString ResourcePathOwn = ResourcePath;
	HANDLE NotificationHandle = NULL;
	HANDLE ChangeListHandle = NULL;

	while (Thread->ControlPoint())
	{
		if (NotificationHandle == NULL || ResourcePathOwn != ResourcePath)
		{
			if (NotificationHandle != NULL)
			{
				FindCloseChangeNotification(NotificationHandle);
				NotificationHandle = NULL;
			}

			if (ChangeListHandle != NULL)
			{
				CloseHandle(ChangeListHandle);
				ChangeListHandle = NULL;
			}

			ZEString ResourcePathReal = ZEDirectoryInfo(ResourcePath).GetRealPath().Path;

			ChangeListHandle = CreateFile(ResourcePathReal, FILE_LIST_DIRECTORY, 
				FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
				NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

			if (ChangeListHandle == INVALID_HANDLE_VALUE)
			{
				ChangeListHandle = NULL;
				Sleep(500);
				continue;
			}

			ResourcePathOwn = ResourcePath;
		}


		FILE_NOTIFY_INFORMATION FileNotifyInfos[1024];
		DWORD FileNotifyInfoCount = 0;
		BOOL Result = ReadDirectoryChangesW(ChangeListHandle, FileNotifyInfos, sizeof(FileNotifyInfos), TRUE, 
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | 
			FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SIZE |
			FILE_NOTIFY_CHANGE_LAST_WRITE, &FileNotifyInfoCount, NULL, NULL);

		if (!Result)
			continue;
		
		CrawlLocations.LockWrite();
		FILE_NOTIFY_INFORMATION* Current = FileNotifyInfos;
		while (true)
		{
			ZEString FileName = std::wstring(Current->FileName, Current->FileNameLength / sizeof(wchar_t));
	
			if (!CrawlLocations.Exists(FileName))
				CrawlLocations.Add(ZEFormat::Format("{0}/{1}", ResourcePath, FileName));
			
			if (Current->NextEntryOffset == 0)
				break;

			Current = (FILE_NOTIFY_INFORMATION*)((ZEBYTE*)Current + Current->NextEntryOffset);
		}
		CrawlLocations.UnlockWrite();

		FindNextChangeNotification(NotificationHandle);
	}

	if (NotificationHandle != NULL)
	{
		FindCloseChangeNotification(NotificationHandle);
		NotificationHandle = NULL;
	}

	if (ChangeListHandle != NULL)
	{
		CloseHandle(ChangeListHandle);
		ChangeListHandle = NULL;
	}
}

void ZEDAssetManager::AddAssetToCache(ZEDAsset* Asset)
{
	Assets.LockWriteNested();
	{
		Assets.Add(Asset);

		ZEDAssetEvent Event;
		Event.Type = ZED_AET_ASSET_ADDED;
		Event.Path = Asset->GetPath();
		RaiseEvent(&Event);

		AddAssetToCategory(Asset);
		AddAssetToTags(Asset);
	}
	Assets.UnlockWrite();
}

void ZEDAssetManager::RemoveAssetFromCache(ZEDAsset* Asset)
{
	Assets.LockWriteNested();
	{
		ZESSize Index = Assets.FindIndex(Asset);
		if (Index == -1)
		{
			Assets.UnlockWrite();
			return;
		}

		RemoveAssetFromCategory(Asset);
		RemoveAssetFromTags(Asset);
	
		const ZEString& Category = Asset->GetCategory();
		Assets.Remove(Index);

		ZEDAssetEvent Event;
		Event.Type = ZED_AET_ASSET_REMOVED;
		Event.Path = Asset->GetPath();
		RaiseEvent(&Event);
	}
	Assets.UnlockWrite();
}



void ZEDAssetManager::UpdateAssetInCache(ZESize Index, const ZEDAsset& Asset)
{
	Assets.LockWriteNested();
	{
		*Assets[Index] = Asset;

		ZEDAssetEvent Event;
		Event.Type = ZED_AET_ASSET_CHANGED;
		Event.Path = Asset.GetPath();
		RaiseEvent(&Event);
	}
	Assets.UnlockWrite();

}

ZEDAssetCategory* ZEDAssetManager::GetCategory(const ZEString& CategoryPath)
{
	ZERegExMatch Match;
	ZEString ParentName;
	ZEString CategoryName;
	if (MatchCategoryFirst.Match(CategoryPath, Match, ZE_REF_NO_MATCH_STRING))
	{
		ParentName = Match.SubMatches[0].String;
		CategoryName = Match.SubMatches[1].String;
	}
	else
	{
		if (MatchCategorySingle.Match(CategoryPath, Match, ZE_REF_NO_MATCH_STRING))
			CategoryName = Match.SubMatches[0].String;
		else
			return NULL;
	}

	Categories.LockRead();
	{
		if (!ParentName.IsEmpty())
		{
			ZEDAssetCategory* Parent = GetCategory(ParentName);
			if (Parent == NULL)
				return NULL;

			ZEArray<ZEDAssetCategory*>& SubCatagories = Parent->SubCatagories;
			for (ZESize I = 0; I < SubCatagories.GetCount(); I++)
			{
				if (SubCatagories[I]->Name.EqualsIncase(CategoryName))
				{
					Categories.UnlockRead();
					return const_cast<ZEDAssetCategory*>(SubCatagories[I]);
				}
			}
		}
		else
		{
			for (ZESize I = 0; I < Categories.GetCount(); I++)
			{
				if (Categories[I]->Name.EqualsIncase(CategoryName))
				{
					Categories.UnlockRead();
					return Categories[I];
				}
			}
		}
	}
	Categories.UnlockRead();

	return NULL;
}

ZEDAssetCategory* ZEDAssetManager::AddCategory(const ZEString& CategoryPath)
{
	ZERegExMatch Match;
	ZEString ParentPath;
	ZEString CategoryName;
	if (MatchCategoryLast.Match(CategoryPath, Match, ZE_REF_NO_MATCH_STRING))
	{
		ParentPath = Match.SubMatches[0].String;
		CategoryName = Match.SubMatches[1].String;
	}
	else
	{
		if (MatchCategorySingle.Match(CategoryPath, Match, ZE_REF_NO_MATCH_STRING))
			CategoryName = Match.SubMatches[0].String;
		else
			return NULL;
	}

	if (CategoryName.IsEmpty())
		return NULL;

	ZEDAssetCategory* NewCategory = NULL;
	Categories.LockWriteNested();
	{
		if (!ParentPath.IsEmpty())
		{
			ZEDAssetCategory* ParentCategory = GetCategory(ParentPath);
			if (ParentCategory == NULL)
				ParentCategory = AddCategory(ParentPath);

			if (ParentCategory == NULL)
				return NULL;

			for (ZESize I = 0; I < ParentCategory->SubCatagories.GetCount(); I++)
			{
				ZEDAssetCategory* SubCatagory = ParentCategory->SubCatagories[I];
				if (SubCatagory->Name.EqualsIncase(CategoryName))
				{
					Categories.UnlockWrite();
					return SubCatagory;
				}
			}

			NewCategory = new ZEDAssetCategory();
			NewCategory->Name = CategoryName;
			NewCategory->Parent = ParentCategory;
			ParentCategory->SubCatagories.Add(NewCategory);

			ZEDAssetEvent Event;
			Event.Type = ZED_AET_ASSET_CATEGORY_ADDED;
			Event.Path = NewCategory->GetPath();
			RaiseEvent(&Event);
		}
		else
		{
			for (ZESize I = 0; I < Categories.GetCount(); I++)
			{
				if (Categories[I]->Name.EqualsIncase(CategoryName))
				{
					Categories.UnlockWrite();
					return Categories[I];
				}
			}

			NewCategory = new ZEDAssetCategory();
			NewCategory->Name = CategoryName;
			NewCategory->Parent = NULL;
			Categories.Add(NewCategory);

			ZEDAssetEvent Event;
			Event.Type = ZED_AET_ASSET_CATEGORY_ADDED;
			Event.Path = NewCategory->GetPath();
			RaiseEvent(&Event);
		}
	}
	Categories.UnlockWrite();

	return NewCategory;
}

void ZEDAssetManager::RemoveCategory(ZEDAssetCategory* Category)
{
	Categories.LockWriteNested();
	{
		ZEString CategoryPath = Category->GetPath();
		if (Category->Parent != NULL)
		{
			Category->Parent->SubCatagories.RemoveValue(Category);
			if (Category->SubCatagories.GetCount() == 0)
				RemoveCategory(Category->Parent);
		}
		else
		{
			Categories.RemoveValue(Category);
		}

		ZEDAssetEvent Event;
		Event.Type = ZED_AET_ASSET_CATEGORY_REMOVED;
		Event.Path = CategoryPath;
		RaiseEvent(&Event);
	}
	Categories.UnlockWrite();
}

void ZEDAssetManager::AddAssetToCategory(ZEDAsset* Asset)
{
	Categories.LockWriteNested();
	{
		if (Asset->GetCategory().IsEmpty())
		{
			Categories.UnlockWrite();
			return;
		}

		ZEDAssetCategory* Category = GetCategory(Asset->GetCategory());
		if (Category == NULL)
		{
			Category = AddCategory(Asset->GetCategory());
			if (Category == NULL)
			{
				Categories.UnlockWrite();
				return;
			}
		}

		if (!Category->Assets.Exists(Asset))
		{
			Category->Assets.Add(Asset);
			ZEDAssetEvent Event;
			Event.Type = ZED_AET_ASSET_CATEGORY_CHANGED;
			Event.Path = Category->GetPath();
			RaiseEvent(&Event);
		}
	}
	Categories.UnlockWrite();
}

void ZEDAssetManager::RemoveAssetFromCategory(ZEDAsset* Asset)
{
	Categories.LockWriteNested();
	{
		ZEDAssetCategory* Category = GetCategory(Asset->GetCategory());
		if (Category == NULL)
		{
			Categories.UnlockWrite();
			return;
		}

		ZESSize Index = Category->Assets.FindIndex(Asset);
		if (Index != -1)
		{
			Category->Assets.RemoveValue(Asset);

			ZEDAssetEvent Event;
			Event.Type = ZED_AET_ASSET_CATEGORY_CHANGED;
			Event.Path = Category->GetPath();
			RaiseEvent(&Event);

			if (Category->Assets.GetCount() == 0)
				RemoveCategory(Category);
		}
	}
	Categories.UnlockWrite();
}

const ZEDAssetTag* ZEDAssetManager::GetTag(const ZEString& Name) const
{
	return NULL;
}

void ZEDAssetManager::AddTag(const ZEString& Name)
{

}

void ZEDAssetManager::RemoveTag(const ZEString& Name)
{

}

void ZEDAssetManager::AddAssetToTags(ZEDAsset* Asset)
{

}

void ZEDAssetManager::RemoveAssetFromTags(ZEDAsset* Asset)
{

}

bool ZEDAssetManager::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	CrawlLocations.Add(ResourcePath);
	CrawlerThread.Run();
	MonitorThread.Run();

	return true;
}

bool ZEDAssetManager::DeinitializeInternal()
{
	CrawlerThread.Exit();
	MonitorThread.Exit();

	Clear();

	return ZEInitializable::DeinitializeInternal();
}

void ZEDAssetManager::Clear()
{
	CrawlLocations.LockWrite();
	Assets.LockWrite();
	Categories.LockWrite();
	{
		CrawlLocations.Clear();
		CrawlLocations.Push(ResourcePath);
		Assets.Clear();
		Categories.Clear();
	}
	Categories.UnlockWrite();
	Assets.UnlockWrite();
	CrawlLocations.UnlockWrite();
}

ZEDAssetManager::ZEDAssetManager()
{
	MatchCategoryLast.Compile("^(.*)\\.\\s*(\\w*[\\w\\s]*?\\w*)\\s*$");
	MatchCategoryFirst.Compile("^\\s*(\\w*[\\w\\s]*?\\w*)\\s*\\.(.*)$");
	MatchCategorySingle.Compile("^\\s*(\\w*[\\w\\s]*?\\w*)\\s*$");
	ResourcePath = "#R:/";
	CrawlerThread.SetFunction(ZEThreadFunction::Create<ZEDAssetManager, &ZEDAssetManager::CrawlerFunction>(this));
	MonitorThread.SetFunction(ZEThreadFunction::Create<ZEDAssetManager, &ZEDAssetManager::MonitorFunction>(this));
	RegisterAssetType(new ZEDTextureAssetType());
}

ZEDAssetManager::~ZEDAssetManager()
{
	Deinitialize();
}

void ZEDAssetManager::SetResourcePath(const ZEString& ResourcePath)
{
	if (this->ResourcePath == ResourcePath)
		return;

	this->ResourcePath = ResourcePath;

	Clear();
}

const ZEString& ZEDAssetManager::GetResourcePath()
{
	return ResourcePath;
}

void ZEDAssetManager::GetCategoriesInternal(ZESmartArray<ZEString>& Output, ZEDAssetCategory* CurrentPath, const ZEString& CurrentPathString, bool Recursive)
{
	ZEString* Buffer = Output.AddMultiple(CurrentPath->SubCatagories.GetCount());
	for (ZESize I = 0; I < CurrentPath->SubCatagories.GetCount(); I++)
	{
		Buffer[I] = CurrentPathString + "." + CurrentPath->SubCatagories[I]->Name;
		if (Recursive)
			GetCategoriesInternal(Output, CurrentPath->SubCatagories[I], Buffer[I], true);
	}
}

ZESmartArray<ZEString> ZEDAssetManager::GetCategories(const ZEString& CategoryPath, bool Recursive)
{
	ZESmartArray<ZEString> Output;

	ZEDAssetCategory* RootPath = NULL;
	ZEDAssetCategory* CurrentPath = NULL;

	Categories.LockRead();
	{
		if (!CategoryPath.IsEmpty())
		{
			CurrentPath = GetCategory(CategoryPath);
			if (CurrentPath == NULL)
			{
				Categories.UnlockRead();
				return Output;
			}

			GetCategoriesInternal(Output, CurrentPath, "", Recursive);
		}
		else
		{
			for (ZESize I = 0; I < Categories.GetCount(); I++)
				GetCategoriesInternal(Output, Categories[I], Categories[I]->Name, Recursive);
		}
	}
	Categories.UnlockRead();

	return Output;
}

const ZEArray<const ZEDAssetType*>& ZEDAssetManager::GetAssetTypes()
{
	return *reinterpret_cast<ZEArray<const ZEDAssetType*>*>(&AssetTypes);
}

void ZEDAssetManager::RegisterAssetType(ZEDAssetType* AssetType)
{
	zeCheckError(AssetType == NULL, ZE_VOID, "Cannot register AssetType. AssetType is NULL.");
	zeCheckError(AssetTypes.Exists(AssetType), ZE_VOID, "Cannot register AssetType. AssetType is already registered. AssetType: \"%s\".", AssetType->GetName());

	AssetTypes.Add(AssetType);
}

void ZEDAssetManager::UnregisterAssetType(ZEDAssetType* AssetType)
{
	zeCheckError(AssetType == NULL, ZE_VOID, "Cannot unregister AssetType. AssetType is NULL.");
	zeCheckError(!AssetTypes.Exists(AssetType), ZE_VOID, "Cannot unregister AssetType. AssetType is not registered. AssetType: \"%s\".", AssetType->GetName());

	AssetTypes.RemoveValue(AssetType);
}

void ZEDAssetManager::UpdateAssetCache(const ZEString& Path)
{
	CrawlLocations.LockWrite();
	CrawlLocations.Add(Path);
	CrawlLocations.UnlockWrite();
}

bool ZEDAssetManager::WrapAsset(ZEDAsset* Asset, const ZEString& Path)
{
	ZEFileInfo Info(Path);
	ZEString Extension = Info.GetExtension();

	AssetTypes.LockRead();
	{
		for (ZESize I = 0; I < AssetTypes.GetCount(); I++)
		{
			const char* const* Extensions = AssetTypes[I]->GetExtensions();
			ZESize ExtensionCount = AssetTypes[I]->GetExtensionCount();
			for (ZESize N = 0; N < ExtensionCount; N++)
			{
				if (!Extension.EqualsIncase(Extensions[N]))
					continue;

				bool Result = AssetTypes[I]->Wrap(Asset, Path);

				AssetTypes.UnlockRead();
				return Result;
			}
		}
	}
	AssetTypes.UnlockRead();

	return false;
}

const ZESmartArray<const ZEDAsset*>& ZEDAssetManager::GetAssetsInCache()
{
	return *reinterpret_cast<ZESmartArray<const ZEDAsset*>*>(&Assets);
}

ZESmartArray<ZEDAsset> ZEDAssetManager::GetAssetsInDirectory(const ZEString& Directory, bool Recursive, bool IncludeNonAsset)
{
	ZESmartArray<ZEDAsset> Output;

	ZEDirectoryInfo DirectoryInfo(Directory);
	ZEArray<ZEString> Files = DirectoryInfo.GetFiles();
	for (ZESize I = 0; I < Files.GetCount(); I++)
	{
		ZEDAsset Asset;
		WrapAsset(&Asset, ZEFormat::Format("{0}/{1}", Directory, Files[I]));
		if (Asset.GetType() == NULL && !IncludeNonAsset)
			continue;

		Output.Add(Asset);
	}

	if (Recursive)
	{
		ZEArray<ZEString> SubDirectories = DirectoryInfo.GetSubDirectories();
		for (ZESize I = 0; I < SubDirectories.GetCount(); I++)
			Output += GetAssetsInDirectory(ZEFormat::Format("{0}/{1}", Directory, SubDirectories[I]), true, IncludeNonAsset);
	}

	return Output;
}

ZESmartArray<ZEDAsset> ZEDAssetManager::GetAssetsInCategory(const ZEString& Path, bool Recursive)
{
	ZESmartArray<ZEDAsset> Output;

	Assets.LockRead();
	{
		ZEDAssetCategory* Category = GetCategory(Path);
		if (Category == NULL)
		{
			Assets.UnlockRead();
			return Output;
		}

		ZEDAsset* Buffer = Output.AddMultiple(Category->Assets.GetCount());
		for (ZESize I = 0; I < Category->Assets.GetCount(); I++)
			Buffer[I] = *Category->Assets[I];

		if (Recursive)
		{
			for (ZESize I = 0; I < Category->SubCatagories.GetCount(); I++)
				Output += GetAssetsInCategory(Category->SubCatagories[I]->GetPath(), true);
		}
	}
	Assets.UnlockRead();

	return Output;
}

ZEDAssetManager* ZEDAssetManager::CreateInstance()
{
	return new ZEDAssetManager();
}

ZEString ZEDAssetCategory::GetPath()
{
	if (Parent == NULL)
		return Name;
	else
		return Parent->GetPath() + "." + Name;
}
