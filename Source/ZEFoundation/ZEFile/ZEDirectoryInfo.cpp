//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDirectoryInfo.cpp
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
#include "ZEDirectoryInfo.h"

#include <memory.h>
#include "ZEPathUtils.h"

ZEDirectoryInfo::ZEDirectoryInfo()
{
	memset((void*)&Creation, 0, sizeof(ZEFileTime));
	memset((void*)&Modification, 0, sizeof(ZEFileTime));
}

ZEDirectoryInfo::ZEDirectoryInfo(const ZEString& DirectoryPath)
{
	Path = ZEPathManager::GetFinalPath(DirectoryPath, &Root);
	Name = GetDirectoryName(Path);

	memset((void*)&Creation, 0, sizeof(ZEFileTime));
	memset((void*)&Modification, 0, sizeof(ZEFileTime));
}

ZEDirectoryInfo::~ZEDirectoryInfo()
{

}

void ZEDirectoryInfo::SetPath(const ZEString& DirectoryPath)
{
	Path = ZEPathManager:: GetFinalPath(DirectoryPath, &Root);
	Name = GetDirectoryName(Path);
	
	memset((void*)&Creation, 0, sizeof(ZEFileTime));
	memset((void*)&Modification, 0, sizeof(ZEFileTime));
}

const ZEString& ZEDirectoryInfo::GetName() const
{
	return Name;
}

const ZEString& ZEDirectoryInfo::GetPath() const
{
	return Path;
}

bool ZEDirectoryInfo::GetCreationDate(ZEFileTime& Time)
{
    bool Result;

    Result = ZEFileUtils::GetCreationTime(&Creation, Path);
    memcpy((void*)&Time, (void*)&Creation, sizeof(ZEFileTime));

    return Result;
}

bool ZEDirectoryInfo::GetModificationDate(ZEFileTime& Time)
{
    bool Result;

    Result = ZEFileUtils::GetModificationTime(&Modification, Path);
    memcpy((void*)&Time, (void*)&Modification, sizeof(ZEFileTime));

    return Result;
}

ZEArray<ZEFileInfo*>* ZEDirectoryInfo::GetFileList()
{
	ZEFileInfo*	Temp;
	bool Continue = true;
	ZEArray<ZEFileInfo*>* FileList;
    ZEFileSearchStream FindData;
	const ZEString Seperator = ZEPathUtils::GetSeperator();

	// Check if path can be opened
	ZEString NewPath = ZEPathManager::GetFinalPath(Path, NULL);

	FileList = new ZEArray<ZEFileInfo*>;
	if (FileList == NULL)
    {
        zeError("Cannot allocate...");
        return NULL;
    }

	// Open directory
    Continue = ZEFileUtils::OpenSearchStream(&FindData, Path);
    while (Continue)
	{
		// If file
		if (ZEFileUtils::IsFile(&FindData))
		{
			Temp = new ZEFileInfo();
            if (Temp == NULL)
            {
                zeError("Cannot allcoate...");
                ZEFileUtils::CloseSearchStream(&FindData);
                return FileList;
            }

			Temp->Root = Root;
			Temp->Name = ZEFileUtils::GetFileName(&FindData);
			Temp->Path = NewPath + Seperator + Temp->Name;
			Temp->Extension = ZEFileInfo::GetFileExtension(Temp->Name);
			FileList->Add(Temp);
		}

		// Get next
        Continue = ZEFileUtils::FindNextInStream(&FindData);
	}

    ZEFileUtils::CloseSearchStream(&FindData);
	return FileList;
}

ZEArray<ZEDirectoryInfo*>* ZEDirectoryInfo::GetDirectoryList()
{
	ZEDirectoryInfo* Temp;
	bool Continue = true;
	ZEArray<ZEDirectoryInfo*>* FolderList;
    ZEFileSearchStream FindData;
	const ZEString Seperator = ZEPathUtils::GetSeperator();

	// Check if path can be opened
	ZEString NewPath = ZEPathManager::GetFinalPath(Path, &Root);

	FolderList = new ZEArray<ZEDirectoryInfo*>;
    if (FolderList == NULL)
    {
        zeError("Cannot allocate...");
        return NULL;
    }

    Continue = ZEFileUtils::OpenSearchStream(&FindData, Path);
    while (Continue)
    {
		// If Folder
		if (ZEFileUtils::IsDirectory(&FindData))
		{
			Temp = new ZEDirectoryInfo();
			if ( Temp == NULL )
            {
                zeError("Cannot allcoate...");
                ZEFileUtils::CloseSearchStream(&FindData);
                return FolderList;
            }

            Temp->Root = Root;
            Temp->Name = ZEFileUtils::GetFileName(&FindData);
			Temp->Path = NewPath + Seperator + Temp->Name;
			FolderList->Add(Temp);
		}

        Continue = ZEFileUtils::FindNextInStream(&FindData);
	}

    ZEFileUtils::CloseSearchStream(&FindData);
    return FolderList;
}

bool ZEDirectoryInfo::IsDirectory(const ZEString& DirectoryPath)
{
	return ZEFileUtils::IsDirectory(DirectoryPath);
}

ZEString ZEDirectoryInfo::GetDirectoryName(const ZEString& DirectoryPath)
{
	ZESSize Length = DirectoryPath.GetLength();

	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if (ISSEPERATOR(DirectoryPath[I]))
			return DirectoryPath.Right(Length - 1 - I);
	}

	return DirectoryPath;
}

ZEString ZEDirectoryInfo::GetParentDirectory(const ZEString& DirectoryPath)
{
	ZESSize Length = DirectoryPath.GetLength();

	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if (ISSEPERATOR(DirectoryPath[I]))
			return DirectoryPath.Left(I);
	}

	return "";
}
