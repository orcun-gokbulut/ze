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

}

ZEDirectoryInfo::ZEDirectoryInfo(const ZEString& DirectoryPath)
{
	Path = ZEPathManager::GetRealPath(DirectoryPath, &Root);
	Name = GetDirectoryName(Path);
}

ZEDirectoryInfo::~ZEDirectoryInfo()
{

}

void ZEDirectoryInfo::SetPath(const ZEString& DirectoryPath)
{
	Path = ZEPathManager:: GetRealPath(DirectoryPath, &Root);
	Name = GetDirectoryName(Path);
}

const ZEString& ZEDirectoryInfo::GetName() const
{
	return Name;
}

const ZEString& ZEDirectoryInfo::GetPath() const
{
	return Path;
}

ZEFileTime ZEDirectoryInfo::GetCreationTime() const
{
	ZEFileTime Time;
    ZEFileUtils::GetCreationTime(&Time, Path);
    return Time;
}

ZEFileTime ZEDirectoryInfo::GetModificationTime() const
{
	ZEFileTime Time;
	ZEFileUtils::GetModificationTime(&Time, Path);
	return Time;
}

ZEArray<ZEFileInfo> ZEDirectoryInfo::GetFileList() const
{
	ZEArray<ZEFileInfo> FileList;

	const ZEString Seperator = ZEPathUtils::GetSeperator();
	ZEString NewPath = ZEPathManager::GetRealPath(Path, NULL);

	ZEFileSearchStream FindData;
    bool Continue = ZEFileUtils::OpenSearchStream(&FindData, Path);
    while (Continue)
	{
		// If file
		if (ZEFileUtils::IsFile(&FindData))
		{
			ZEFileInfo FileInfo;
			FileInfo.Root = Root;
			FileInfo.Name = ZEFileUtils::GetFileName(&FindData);
			FileInfo.Path = NewPath + Seperator + FileInfo.Name;
			FileInfo.Extension = ZEFileInfo::GetFileExtension(FileInfo.Name);
			FileList.Add(FileInfo);
		}

		// Get next
        Continue = ZEFileUtils::FindNextInStream(&FindData);
	}

    ZEFileUtils::CloseSearchStream(&FindData);

	return FileList;
}

ZEArray<ZEDirectoryInfo> ZEDirectoryInfo::GetDirectoryList() const
{
	ZEArray<ZEDirectoryInfo> DirectoryList;
    
	const ZEString Seperator = ZEPathUtils::GetSeperator();
	ZEString NewPath = ZEPathManager::GetRealPath(Path, &Root);

	ZEFileSearchStream FindData;
	bool Continue = ZEFileUtils::OpenSearchStream(&FindData, Path);
    while (Continue)
    {
		ZEDirectoryInfo DirectoryInfo;
		if (ZEFileUtils::IsDirectory(&FindData))
		{
			DirectoryInfo.Root = Root;
			DirectoryInfo.Name = ZEFileUtils::GetFileName(&FindData);
			DirectoryInfo.Path = NewPath + Seperator + DirectoryInfo.Name;
			DirectoryList.Add(DirectoryInfo);
		}

        Continue = ZEFileUtils::FindNextInStream(&FindData);
	}

    ZEFileUtils::CloseSearchStream(&FindData);

    return DirectoryList;
}

bool ZEDirectoryInfo::IsDirectory(const ZEString& DirectoryPath)
{
	if(DirectoryPath.GetLength() == 0)
		return false;

	return ZEFileUtils::IsDirectory(DirectoryPath);
}

ZEString ZEDirectoryInfo::GetDirectoryName(const ZEString& DirectoryPath)
{
	ZESSize Length = DirectoryPath.GetLength();

	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if (DirectoryPath[I] == '\\' || DirectoryPath[I] == '/')
			return DirectoryPath.Right(Length - 1 - I);
	}

	return DirectoryPath;
}

ZEString ZEDirectoryInfo::GetParentDirectory(const ZEString& DirectoryPath)
{
	ZESSize Length = DirectoryPath.GetLength();

	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if (DirectoryPath[I] == '\\' || DirectoryPath[I] == '/')
			return DirectoryPath.Left(I);
	}

	return "";
}
