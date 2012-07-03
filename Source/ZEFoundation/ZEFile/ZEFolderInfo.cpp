//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFolderInfo.cpp
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

#include "ZEFileInfo.h"
#include "ZEFileUtils.h"
#include "ZEFolderInfo.h"
#include "ZEPathManager.h"

#include "ZEDS/ZEArray.h"


#include <windows.h>


ZEFolderInfo::ZEFolderInfo()
{
	memset((void*)&Creation, 0, sizeof(ZEFileTime));
	memset((void*)&Modification, 0, sizeof(ZEFileTime));
}

ZEFolderInfo::ZEFolderInfo(const ZEString& FolderPath)
{
	Path = ZEPathManager::GetFinalPath(FolderPath, Root);
	Name = GetFolderName(Path);

	memset((void*)&Creation, 0, sizeof(ZEFileTime));
	memset((void*)&Modification, 0, sizeof(ZEFileTime));
}

ZEFolderInfo::~ZEFolderInfo()
{

}

const ZEString& ZEFolderInfo::GetName() const
{
	return Name;
}

const ZEString& ZEFolderInfo::GetPath() const
{
	return Path;
}

bool ZEFolderInfo::GetCreationDate(ZEFileTime& Time)
{
	void* Handle;
	WIN32_FIND_DATA FindData;

	Handle = NULL;
	if ( !ZEFileUtils::GetFileFolderInfo(Path, (OSFileSearchData*)&FindData, &Handle) )
		return false;

	ZEFileUtils::FILETIMEtoZEFileTime(&Creation, (OSFileTime*)&FindData.ftCreationTime);
	memcpy((void*)&Time, (void*)&Creation, sizeof(ZEFileTime));

	return true;
}

bool ZEFolderInfo::GetModificationDate(ZEFileTime& Time)
{
	void* Handle;
	WIN32_FIND_DATA FindData;

	Handle = NULL;
	if ( !ZEFileUtils::GetFileFolderInfo(Path, (OSFileSearchData*)&FindData, &Handle) )
		return false;

	ZEFileUtils::FILETIMEtoZEFileTime(&Modification, (OSFileTime*)&FindData.ftLastWriteTime);
	memcpy((void*)&Time, (void*)&Modification, sizeof(ZEFileTime));

	return true;
}

ZEArray<ZEFileInfo*>* ZEFolderInfo::GetFileList()
{
	bool GetNext;
	void* SearchHandle;
	ZEString FileName;
	ZEFileInfo*	Temp;
	WIN32_FIND_DATA FindData;
	ZEArray<ZEFileInfo*>* FileList;

	// if path is out of boundary
	if ( !ZEPathManager::PathBoundaryCheck(ZEPathManager::GetKnownPath(Root), Path) )
	{
		zeError("Paths above the root are not reachable..");
		return NULL;
	}

	FileList = new ZEArray<ZEFileInfo*>;
	if ( FileList == NULL )
		zeCriticalError("Cannot allocate...");

	GetNext = ZEFileUtils::GetFileFolderInfo(Path + "\\*", (OSFileSearchData*)&FindData, &SearchHandle);
	while (GetNext)
	{
		// If file
		if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		{
			FileName = "\\";
			FileName += FindData.cFileName;

			Temp = new ZEFileInfo();
			if ( Temp == NULL )
				zeCriticalError("Cannot allcoate...");

			Temp->Root = Root;
			Temp->Path = Path + FileName;
			Temp->Name = ZEFileInfo::GetFileName(Temp->Path);
			Temp->Extension = ZEFileInfo::GetFileExtension(Temp->Path);
			FileList->Add(Temp);
		}

		GetNext = ZEFileUtils::GetNextFileFolderInfo(SearchHandle, (OSFileSearchData*)&FindData);
	}

	return FileList;
}

ZEArray<ZEFolderInfo*>* ZEFolderInfo::GetFolderList()
{
	bool GetNext;
	void* SearchHandle;
	ZEString FolderName;
	ZEFolderInfo* Temp;
	WIN32_FIND_DATA FindData;
	ZEArray<ZEFolderInfo*>* FolderList;

	// if path is out of boundary
	if ( !ZEPathManager::PathBoundaryCheck(ZEPathManager::GetKnownPath(Root), Path) )
	{
		zeError("Paths above the root are not reachable..");
		return NULL;
	}

	FolderList = new ZEArray<ZEFolderInfo*>;
	if ( FolderList == NULL )
		zeCriticalError("Cannot allocate...");

	GetNext = ZEFileUtils::GetFileFolderInfo(Path + "\\*", (OSFileSearchData*)&FindData, &SearchHandle);
	while (GetNext)
	{
		// If Folder
		if ( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			FolderName = "\\";
			FolderName += FindData.cFileName;

			Temp = new ZEFolderInfo();
			if ( Temp == NULL )
				zeCriticalError("Cannot allcoate...");

			Temp->Root = Root;
			Temp->Path = Path + FolderName;
			Temp->Name = ZEFolderInfo::GetFolderName(Temp->Path);
			FolderList->Add(Temp);
		}

		GetNext = ZEFileUtils::GetNextFileFolderInfo(SearchHandle, (OSFileSearchData*)&FindData);
	}

	return FolderList;
}



// STATIC
bool ZEFolderInfo::IsFolder(const ZEString& FolderPath)
{
	WIN32_FIND_DATA FindData;
	HANDLE FirstFileHandle = INVALID_HANDLE_VALUE;

	FirstFileHandle = FindFirstFile(FolderPath.ToCString(), &FindData);
	if (FirstFileHandle == INVALID_HANDLE_VALUE)
		return false;

	return (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
}

// STATIC
ZEString ZEFolderInfo::GetFolderName(const ZEString& FolderPath)
{
	ZESize Length = FolderPath.GetLength();

	if (Length == 0)
		return "";

	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if (FolderPath[I] == '\\' || FolderPath[I] == '/')
			return FolderPath.Right(Length - 1 - I);
	}

	return FolderPath;
}

// STATIC
ZEString ZEFolderInfo::GetParentFolder(const ZEString& Path)
{
	ZESize Length = Path.GetLength();

	if (Length == 0)
		return "";

	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if (Path[I] == '\\' || Path[I] == '/')
			return Path.Left(I);
	}

	return "";
}
