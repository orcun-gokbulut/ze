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
#include "ZEDAssetEvent.h"
#include "ZEDAssetDirectory.h"
#include "ZEDAssetCategory.h"
#include "ZERandom.h"
#include "ZEDS/ZEFormat.h"
#include "ZEFile/ZEDirectoryInfo.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEFile/ZEPathTokenizer.h"
#include "ZEMeta/ZEMTProvider.h"
#include "ZERegEx/ZERegEx.h"


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef CreateDirectory
#undef RemoveDirectory

void ZEDAssetManager::Crawl()
{
	if (CrawlLocations.GetCount() == 0)
		return;

	ZEString Location = CrawlLocations.Dequeue();
	if (ZEDirectoryInfo(Location).IsDirectory() || GetDirectory(Location) != NULL)
	{
		ScanDirectory(Location, true);
	}
	else
	{
		ScanFile(Location);
	}
}

void ZEDAssetManager::CrawlerFunction(ZEThread* Thread, void* ExtraParameters)
{
	while(Thread->ControlPoint())
	{
		if (CrawlLocations.GetCount() == 0)
		{
			Sleep(500);
			continue;
		}

		Crawl();
	}
}

void ZEDAssetManager::MonitorFunction(ZEThread* Thread, void* ExtraParameters)
{
	MonitorThreadHandle = GetCurrentThread();

	ZEString ResourcePathOwn = ResourcePath;
	HANDLE NotificationHandle = NULL;
	HANDLE ChangeListHandle = NULL;

	while (Thread->ControlPoint())
	{
		if (ChangeListHandle == NULL || ResourcePathOwn != ResourcePath)
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

			/*NotificationHandle = FindFirstChangeNotification(ResourcePathReal, TRUE, 
				FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | 
				FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SIZE |
				FILE_NOTIFY_CHANGE_LAST_WRITE);*/

			ResourcePathOwn = ResourcePath;
		}

		/*HRESULT WaitResult = WaitForSingleObject(NotificationHandle, 500);
		if (WaitResult == WAIT_TIMEOUT)
			continue;
		else if (WaitResult == WAIT_ABANDONED)
			continue;*/

		FILE_NOTIFY_INFORMATION FileNotifyInfos[1024];
		DWORD FileNotifyInfoCount = 0;
		BOOL ReadChangeResult = ReadDirectoryChangesW(ChangeListHandle, FileNotifyInfos, sizeof(FileNotifyInfos), TRUE, 
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | 
			FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SIZE |
			FILE_NOTIFY_CHANGE_LAST_WRITE, &FileNotifyInfoCount, NULL, NULL);

		if (!ReadChangeResult)
			continue;
		
		CrawlLocations.LockWrite();
		FILE_NOTIFY_INFORMATION* Current = FileNotifyInfos;
		while (true)
		{
			ZEString FileName = std::wstring(Current->FileName, Current->FileNameLength / sizeof(wchar_t));
	
			AddCrawlLocation(ZEFormat::Format("{0}/{1}", ResourcePath, FileName));	
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

void ZEDAssetManager::AddCrawlLocation(const ZEString& Location)
{
	CrawlLocations.LockWriteNested();
	if (!CrawlLocations.Exists(Location))
		CrawlLocations.Enqueue(Location);
	CrawlLocations.UnlockWrite();
}

ZEDAsset* ZEDAssetManager::CreateAsset(const ZEString& Path)
{
	ZEPathInfo PathInfo(Path);
	PathInfo.NormalizeSelf();

	ZEDAssetType* AssetType = GetAssetType(PathInfo.GetPath());
	if (AssetType == NULL)
		return NULL;

	ZEDAssetDirectory* Directory = CreateDirectory(PathInfo.GetParentDirectory());
	if (Directory == NULL)
		return NULL;

	ZEString FileName = PathInfo.GetFileName();
	ze_for_each(Asset, Directory->GetAssets())
	{
		if (Asset->GetName() == FileName)
			return Asset.GetPointer();
	}

	ZEDAsset* NewAsset = AssetType->Wrap(PathInfo.GetPath());
	if (NewAsset == NULL)
		return NULL;

	ZEDAssetCategory* Category = CreateCategory(NewAsset->GetCategoryPath());
	
	NewAsset->Manager = this;
	NewAsset->SetName(FileName);
	NewAsset->Type = AssetType;
	NewAsset->Directory = Directory;
	NewAsset->Category = Category;

	ZEDAssetEvent Event;
	Event.SetType(ZED_AET_ASSET_ADDING);
	Event.SetAsset(NewAsset);
	Event.SetPath(PathInfo.GetPath());
	RaiseEvent(&Event);

	AssetType->Assets.AddEnd(&NewAsset->TypeLink);
	Directory->Assets.AddEnd(&NewAsset->DirectoryLink);
	if (Category != NULL)
		Category->Assets.AddEnd(&NewAsset->CategoryLink);

	Event.SetType(ZED_AET_ASSET_ADDED);
	RaiseEvent(&Event);

	return NewAsset;
}

void ZEDAssetManager::RemoveAsset(ZEDAsset* Asset)
{
	ZEDAssetEvent Event;
	Event.SetType(ZED_AET_ASSET_REMOVING);
	Event.SetAsset(Asset);
	Event.SetPath(Asset->GetPath());
	RaiseEvent(&Event);

	Asset->Directory->Assets.Remove(&Asset->DirectoryLink);
	Asset->Type->Assets.Remove(&Asset->TypeLink);
	ZEDAssetCategory* Category = Asset->Category;
	if (Asset->Category != NULL)
		Asset->Category->Assets.Remove(&Asset->CategoryLink);

	Event.SetType(ZED_AET_ASSET_REMOVED);
	RaiseEvent(&Event);

	Asset->Manager = NULL;
	Asset->Type = NULL;
	Asset->Directory = NULL;
	Asset->Category = NULL;

	if (Asset->Category != NULL)
		RemoveCategory(Asset->Category);
}

ZEDAssetDirectory* ZEDAssetManager::CreateDirectory(const ZEString& DirectoryPath)
{
	if (GetDirectoryRoot() == NULL)
		return NULL;

	ZEString RelativePath = ZEPathInfo(DirectoryPath).GetRelativeTo(DirectoryRoot->GetPath());
	if (RelativePath.IsEmpty() && !ZEPathInfo::Compare(ResourcePath, DirectoryPath))
		return NULL;

	ZEPathTokenizer Tokenizer;
	Tokenizer.Tokenize(RelativePath);

	ZEDAssetDirectory* ParentDirectory = GetDirectoryRoot();
	for (ZESize I = 0; I < Tokenizer.GetTokenCount(); I++)
	{
		bool Found = false;
		const char* Token = Tokenizer.GetToken(I);
		ze_for_each(SubDirectory, ParentDirectory->GetSubDirectories())
		{
			if (SubDirectory->GetName() == Token)
			{
				ParentDirectory = SubDirectory.GetPointer();
				Found = true;
				break;
			}
		}

		if (!Found)
		{
			ZEDAssetDirectory* NewDirectory = new ZEDAssetDirectory();
			NewDirectory->Manager = this;
			NewDirectory->SetName(Token);
			NewDirectory->ParentDirectory = ParentDirectory;

			ZEDAssetEvent Event;
			Event.SetType(ZED_AET_DIRECTORY_ADDING);
			Event.SetDirectory(NewDirectory);
			Event.SetPath(NewDirectory->GetPath());
			RaiseEvent(&Event);

			ParentDirectory->SubDirectories.AddEnd(&NewDirectory->ParentDirectoryLink);
			Event.SetType(ZED_AET_DIRECTORY_ADDED);
			RaiseEvent(&Event);

			ParentDirectory = NewDirectory;
		}
	}

	return ParentDirectory;
}

void ZEDAssetManager::RemoveDirectory(ZEDAssetDirectory* Directory)
{
	while (Directory->GetSubDirectories().GetFirst() != NULL)
	{
		ZEDAssetDirectory* DirectoryLink = Directory->GetSubDirectories().GetFirst()->GetItem();
		RemoveDirectory(DirectoryLink);
		delete DirectoryLink;
	}


	while (Directory->GetAssets().GetFirst() != NULL)
	{
		ZEDAsset* Asset = Directory->GetAssets().GetFirst()->GetItem();
		RemoveAsset(Asset);
		delete Asset;
	}

	ZEDAssetEvent Event;
	Event.SetType(ZED_AET_DIRECTORY_REMOVING);
	Event.SetDirectory(Directory);
	Event.SetPath(Directory->GetPath());
	RaiseEvent(&Event);

	Directory->Manager = NULL;
	Directory->ParentDirectory->SubDirectories.Remove(&Directory->ParentDirectoryLink);
	Directory->ParentDirectory = NULL;
	
	Event.SetType(ZED_AET_DIRECTORY_REMOVED);
	RaiseEvent(&Event);
}

ZEDAssetCategory* ZEDAssetManager::CreateCategory(const ZEString& Path)
{
	return NULL;
}

void ZEDAssetManager::RemoveCategory(ZEDAssetCategory* Category)
{
	
}

void ZEDAssetManager::SetAssetCategory(ZEDAsset* Asset, ZEDAssetCategory* Category)
{
	ZEDAssetEvent Event;
	Event.SetType(ZED_AET_ASSET_CATEGORY_CHANGING);
	Event.SetPath(Asset->GetPath());
	Event.SetAsset(Asset);
	RaiseEvent(&Event);

	if (Asset->GetCategory() != NULL)
		Asset->Category->Assets.Remove(&Asset->CategoryLink);

	Asset->Category = Category;

	if (Category != NULL)
		Category->Assets.AddEnd(&Asset->CategoryLink);

	Event.SetType(ZED_AET_ASSET_CATEGORY_CHANGED);
	RaiseEvent(&Event);
}

void ZEDAssetManager::RegisterAssetTypes()
{
	ZEArray<ZEClass*> AssetTypeClasses = ZEMTProvider::GetInstance()->GetClasses(ZEDAssetType::Class(), true);
	for (ZESize I = 0; I < AssetTypeClasses.GetCount(); I++)
	{
		if (AssetTypeClasses[I]->IsAbstract())
			continue;

		RegisterAssetType(static_cast<ZEDAssetType*>(AssetTypeClasses[I]->CreateInstance()));
	}
}

bool ZEDAssetManager::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	RegisterAssetTypes();

	DirectoryRoot = new ZEDAssetDirectory();
	DirectoryRoot->SetName(ResourcePath);

	AddCrawlLocation(ResourcePath);

	//CrawlerThread.Run();
	MonitorThread.Run();

	Crawl();

	return true;
}

bool ZEDAssetManager::DeinitializeInternal()
{
	CrawlerThread.Exit();
	MonitorThread.Exit();
	CancelSynchronousIo(MonitorThreadHandle);

	Clear();

	return ZEInitializable::DeinitializeInternal();
}

void ZEDAssetManager::Clear()
{
	CrawlLocations.LockWrite();
	/*Assets.LockWrite();
	Categories.LockWrite();
	{
		CrawlLocations.Clear();
		CrawlLocations.Push(ResourcePath);
		Assets.Clear();
		Categories.Clear();
	}
	Categories.UnlockWrite();
	Assets.UnlockWrite();*/
	CrawlLocations.UnlockWrite();
}

void ZEDAssetManager::TickEvent(const ZEDTickEvent* Event)
{
	Crawl();
}

ZEDAssetManager::ZEDAssetManager()
{
	DirectoryRoot = NULL;
	CategoryRoot = NULL;
	ResourcePath = "#R:/";
	CrawlerThread.SetFunction(ZEThreadFunction::Create<ZEDAssetManager, &ZEDAssetManager::CrawlerFunction>(this));
	MonitorThread.SetFunction(ZEThreadFunction::Create<ZEDAssetManager, &ZEDAssetManager::MonitorFunction>(this));
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

const ZEArray<const ZEDAssetType*>& ZEDAssetManager::GetAssetTypes()
{
	return *reinterpret_cast<ZEArray<const ZEDAssetType*>*>(&AssetTypes);
}

void ZEDAssetManager::RegisterAssetType(ZEDAssetType* AssetType)
{
	zeCheckError(AssetType == NULL, ZE_VOID, "Cannot register AssetType. AssetType is NULL.");
	zeCheckError(AssetTypes.Exists(AssetType), ZE_VOID, "Cannot register AssetType. AssetType is already registered. AssetType: \"%s\".", AssetType->GetName());

	AssetTypes.Add(AssetType);

	AddCrawlLocation(ResourcePath);
}

void ZEDAssetManager::UnregisterAssetType(ZEDAssetType* AssetType)
{
	zeCheckError(AssetType == NULL, ZE_VOID, "Cannot unregister AssetType. AssetType is NULL.");
	zeCheckError(!AssetTypes.Exists(AssetType), ZE_VOID, "Cannot unregister AssetType. AssetType is not registered. AssetType: \"%s\".", AssetType->GetName());

	AssetTypes.RemoveValue(AssetType);
}

ZEDAssetDirectory* ZEDAssetManager::GetDirectoryRoot()
{
	return DirectoryRoot;
}

ZEDAssetCategory* ZEDAssetManager::GetCategoryRoot()
{
	return CategoryRoot;
}

ZEDAsset* ZEDAssetManager::GetAsset(const ZEString& AssetPath)
{
	ZEFileInfo AssetInfo(AssetPath);
	
	ZEDAssetDirectory* Directory = GetDirectory(AssetInfo.GetParentDirectory());
	if (Directory == NULL)
		return NULL;

	ZEString FileName = AssetInfo.GetFileName();
	ze_for_each(Asset, Directory->GetAssets())
	{
		if (Asset->GetName() == FileName)
			return Asset.GetPointer();
	}

	return NULL;
}

ZEDAssetDirectory* ZEDAssetManager::GetDirectory(const ZEString& DirectoryPath)
{
	if (GetDirectoryRoot() == NULL)
		return NULL;

	ZEString RelativePath = ZEPathInfo(DirectoryPath).GetRelativeTo(DirectoryRoot->GetPath());
	if (RelativePath.IsEmpty() && !ZEPathInfo::Compare(ResourcePath, DirectoryPath))
		return NULL;

	ZEPathTokenizer Tokenizer;
	Tokenizer.Tokenize(RelativePath);

	ZEDAssetDirectory* ParentDirectory = GetDirectoryRoot();
	for (ZESize I = 0; I < Tokenizer.GetTokenCount(); I++)
	{
		bool Found = false;
		const char* Token = Tokenizer.GetToken(I);
		ze_for_each(SubDirectory, ParentDirectory->GetSubDirectories())
		{
			if (SubDirectory->GetName() == Token)
			{
				ParentDirectory = SubDirectory.GetPointer();
				Found = true;
				break;
			}
		}

		if (!Found)
			return NULL;
	}

	return ParentDirectory;
}

ZEDAssetCategory* ZEDAssetManager::GetCategory(const ZEString& CategoryPath)
{
	return NULL;
	/*
	if (GetCategoryRoot() == NULL)
		return NULL;

	ZEString RelativePath = ZEPathInfo(CategoryPath).GetRelativeTo(CategoryRoot->GetPath());
	if (RelativePath.IsEmpty())
		return NULL;

	ZEPathTokenizer Tokenizer;
	Tokenizer.Tokenize(RelativePath);

	ZEDAssetCategory* ParentCategory = GetCategoryRoot();
	for (ZESize I = 0; I < Tokenizer.GetTokenCount(); I++)
	{
		bool Found = false;
		const char* Token = Tokenizer.GetToken(I);
		ze_for_each(SubCategory, ParentCategory->GetSubCatagories())
		{
			if (SubCategory->GetName() != Token)
				continue;

			ParentCategory = SubCategory.GetPointer();
			Found = true;
			break;
		}

		if (!Found)
			return NULL;
	}

	return ParentCategory;*/
}

ZEDAsset* ZEDAssetManager::ScanFile(const ZEString& FilePath)
{
	if (!ZEPathInfo(ResourcePath).IsChildOf(FilePath))
		return NULL;

	ZEFileInfo FileInfo(FilePath);
	ZEDAsset* Asset = GetAsset(FilePath);
	bool Exists = FileInfo.IsExists();
	if (Asset != NULL)
	{
		if (!Exists)
		{
			RemoveAsset(Asset);
			return NULL;
		}
		else if (Asset->GetModificationTime() != FileInfo.GetModificationTime())
		{
			return CreateAsset(FilePath);
		}
		else
		{
			return Asset;
		}
	}
	else
	{
		if (Exists)
			return CreateAsset(FilePath);
	}

	return NULL;
}

ZEDAssetDirectory* ZEDAssetManager::ScanDirectory(const ZEString& DirectoryPath, bool Recursive)
{
	if (!ZEPathInfo(ResourcePath).IsChildOf(DirectoryPath))
		return NULL;

	ZEDirectoryInfo DirectoryInfo(DirectoryPath);
	ZEDAssetDirectory* Directory = GetDirectory(DirectoryPath);
	bool Exists = DirectoryInfo.IsExists();
	if (Directory != NULL)
	{
		if (!Exists)
		{
			RemoveDirectory(Directory);
			return NULL;
		}
	}
	else
	{
		if (Exists)
			Directory = CreateDirectory(DirectoryPath);
	}

	ZEArray<ZEString> Files = DirectoryInfo.GetFiles();
	for (ZESize I = 0; I < Files.GetCount(); I++)
		ScanFile(ZEFormat::Format("{0}/{1}", DirectoryInfo.GetPath(), Files[I]));

	if (Recursive)
	{
		ZEArray<ZEString> SubDirectories = DirectoryInfo.GetSubDirectories();
		for (ZESize I = 0; I < SubDirectories.GetCount(); I++)
			ScanDirectory(ZEFormat::Format("{0}/{1}", DirectoryInfo.GetPath(), SubDirectories[I]), true);
	}

	return Directory;
}

void ZEDAssetManager::UpdatePath(const ZEString& Path)
{
	AddCrawlLocation(Path);
}

void ZEDAssetManager::Process()
{
	Crawl();
}

ZEDAssetType* ZEDAssetManager::GetAssetType(const ZEString& Path)
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
				
				AssetTypes.UnlockRead();
				return AssetTypes[I];
			}
		}
	}
	AssetTypes.UnlockRead();

	return NULL;
}

ZEDAssetManager* ZEDAssetManager::CreateInstance()
{
	return new ZEDAssetManager();
}
