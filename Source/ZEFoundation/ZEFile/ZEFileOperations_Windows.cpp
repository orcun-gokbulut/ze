//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileOperations_Windows.cpp
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

#include "ZEFileOperations.h"
#include "ZEFileInfo.h"
#include "ZEDirectoryInfo.h"
#include <ZEDS/ZEString.h>
#include <ZEDS/ZEArray.h>
#include <ZEFoundation/ZEError.h>

 
bool ZEFileOperations::CreateFolder(const ZEString& DestinationParentPath, const ZEString& Name)
{
	ZEString Path = ZEPathManager::GetRealPath(DestinationParentPath) + "\\" + Name;

	return CreateDirectoryW(Path.ToWCString(), NULL);
}

bool ZEFileOperations::Rename(const ZEString& Name, const ZEFileInfo& File, bool OverwriteIfExists)
{
	ZEString ExistingPath = File.GetPath();
	ZEString NewPath = ZEFileInfo::GetParentDirectory(ExistingPath) + "\\" + Name;

	if (OverwriteIfExists)
	{
		if (ZEFileUtils::IsFile(NewPath))
		{
			ZEFileInfo ExistingFile(NewPath);
			Delete(ExistingFile);
		}
	}

	if (!MoveFileW(ExistingPath.ToWCString(), NewPath.ToWCString()))
		return false;

	return true;
}

bool ZEFileOperations::Rename(const ZEString& Name, const ZEDirectoryInfo& Folder)
{
	ZEString ExistingPath = Folder.GetPath();
	ZEString NewPath = ZEDirectoryInfo::GetParentDirectory(ExistingPath) + "\\" + Name;

	if (ZEFileUtils::IsDirectory(NewPath))
		return false;

	if (!MoveFileW(ExistingPath.ToWCString(), NewPath.ToWCString()))
		return false;
	
	return true;
}

bool ZEFileOperations::Copy(const ZEString& DestinationParentPath, const ZEFileInfo& File, bool OverwriteIfExists)
{
	ZEString ExistingPath = File.GetPath();
	ZEString NewPath = ZEPathManager::GetRealPath(DestinationParentPath) + "\\" + File.GetName();

	if (OverwriteIfExists)
	{
		if (ZEFileUtils::IsFile(NewPath))
		{
			ZEFileInfo ExistingFile(NewPath);
			Delete(ExistingFile);
		}
	}

	return CopyFileExW(ExistingPath.ToWCString(), NewPath.ToWCString(), NULL, NULL, NULL, COPY_FILE_FAIL_IF_EXISTS);

}

bool ZEFileOperations::Copy(const ZEString& DestinationParentPath, const ZEDirectoryInfo& Folder)
{
	ZEArray<ZEFileInfo> Files = Folder.GetFileList();
	ZEInt FileCount = Files.GetCount();

	ZEArray<ZEDirectoryInfo> SubFolders = Folder.GetDirectoryList();
	ZEInt SubFolderCount = SubFolders.GetCount();
	
	ZEString NewPath = ZEPathManager::GetRealPath(DestinationParentPath) + "\\" + Folder.GetName();

	if (!CreateFolder(DestinationParentPath, Folder.GetName()))
		return false;

	for (ZEInt I = 0; I < FileCount; I++)
	{
		if (!(ZEFileOperations::Copy(NewPath, Files[I])))
		{
			ZEDirectoryInfo BackTrackFolder(NewPath);
			Delete(BackTrackFolder);

			return false;
		}
	}

	for (ZEInt I = 0; I < SubFolderCount; I++)
	{
		ZEString Name = SubFolders[I].GetName();

		if((Name == ".") || (Name == ".."))
			continue;

		if (!ZEFileOperations::Copy(NewPath, SubFolders[I]))
		{
			Delete(ZEDirectoryInfo(NewPath));
			return false;
		}
	}

	return true;
}

bool ZEFileOperations::Move(const ZEString& DestinationParentPath, const ZEFileInfo& File, bool OverwriteIfExists)
{
	ZEString ExistingPath = File.GetPath();
	ZEString NewPath = ZEPathManager::GetRealPath(DestinationParentPath) + "\\" + File.GetName();

	if (OverwriteIfExists)
	{
		if (ZEFileUtils::IsFile(NewPath))
			Delete(ZEFileInfo(NewPath));
	}

	if (!MoveFileExW(ExistingPath.ToWCString(), NewPath.ToWCString(), MOVEFILE_WRITE_THROUGH))
		return false;

	return true;
}

bool ZEFileOperations::Move(const ZEString& DestinationParentPath, const ZEDirectoryInfo& Folder)
{
	ZEString ExistingPath = Folder.GetPath();
	ZEString NewPath = ZEPathManager::GetRealPath(DestinationParentPath) + "\\" + Folder.GetName();

	if(ZEFileUtils::IsDirectory(NewPath))
		return false;

	if (!MoveFileExW(ExistingPath.ToWCString(), NewPath.ToWCString(), MOVEFILE_WRITE_THROUGH))
		return false;

	return true;
}

bool ZEFileOperations::Delete(const ZEFileInfo& File)
{
	return DeleteFileW(File.GetPath().ToWCString());
}

bool ZEFileOperations::Delete(const ZEDirectoryInfo& Folder)
{
	ZEArray<ZEFileInfo> Files = Folder.GetFileList();
	ZEInt FileCount = Files.GetCount();

	ZEArray<ZEDirectoryInfo> SubFolders = Folder.GetDirectoryList();
	ZEInt SubFolderCount = SubFolders.GetCount();

	for (ZEInt I = 0; I < FileCount; I++)
	{
		if (!ZEFileOperations::Delete(Files[I]))
			return false;
	}

	for (ZEInt I = 0; I < SubFolderCount; I++)
	{
		ZEString Name = SubFolders[I].GetName();

		if((Name == ".") || (Name == ".."))
			continue;

		if (!ZEFileOperations::Delete(SubFolders[I]))
			return false;
	}

	return RemoveDirectoryW(Folder.GetPath().ToWCString());
}
