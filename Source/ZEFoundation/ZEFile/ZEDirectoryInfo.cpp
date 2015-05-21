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

#include "ZEDirectoryInfo.h"
#include "ZEPathManager.h"
#include "ZEDS\ZEFormat.h"

#ifdef ZE_PLATFORM_WINDOWS
#include <Windows.h>
#include <shlwapi.h>

ZEArray<ZEString> ZEDirectoryInfo::GetSubDirectories()
{
	if ((GetAccess() & ZE_PA_READ) == 0)
		return ZEArray<ZEString>();
	
	WIN32_FIND_DATA fdFile; 
	HANDLE hFind = NULL; 
	char sPath[2048]; 

	sprintf(sPath, "%s\\*.*", GetRealPath().Path.ToCString()); 
	if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE) 
		return ZEArray<ZEString>();

	ZEArray<ZEString> Output;
	do
	{ 
		if (strcmp(fdFile.cFileName, ".") != 0 && strcmp(fdFile.cFileName, "..") != 0) 	
			if ((fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) 
				Output.Add(fdFile.cFileName); 

	} while(FindNextFile(hFind, &fdFile));

	FindClose(hFind);

	return Output; 
}

ZEArray<ZEString> ZEDirectoryInfo::GetFiles()
{
	if ((GetAccess() & ZE_PA_READ) == 0)
		return ZEArray<ZEString>();
	
	WIN32_FIND_DATA fdFile; 
	HANDLE hFind = NULL; 
	char sPath[2048]; 

	sprintf(sPath, "%s\\*.*", GetRealPath().Path.ToCString()); 
	if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE) 
		return ZEArray<ZEString>();

	ZEArray<ZEString> Output;
	do
	{ 
		if (strcmp(fdFile.cFileName, ".") != 0 && strcmp(fdFile.cFileName, "..") != 0) 	
			if ((fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) 
				Output.Add(fdFile.cFileName); 

	} while(FindNextFile(hFind, &fdFile));

	FindClose(hFind);

	return Output; 
}

bool ZEDirectoryInfo::Rename(const char* Name)
{
	if ((GetAccess() & ZE_PA_READ_WRITE) == 0)
		return false;

	if (!IsExists())
		return false;

	ZEString Destination = ZEFormat::Format("{0}\\{1}", GetParentDirectory(), GetFileName());

	ZERealPath DestinationRealPath = ZEDirectoryInfo(Destination).GetRealPath();
	return MoveFileEx(GetRealPath().Path, DestinationRealPath.Path, MOVEFILE_REPLACE_EXISTING) != 0;
}

bool ZEDirectoryInfo::Move(const char* Destination)
{
	if ((GetAccess() & ZE_PA_READ_WRITE) == 0)
		return false;

	if (!IsExists())
		return false;
	
	ZEDirectoryInfo DestinationInfo(Destination);
	if ((DestinationInfo.GetAccess() & ZE_PA_WRITE) == 0)
		return false;

	ZERealPath DestinationRealPath = DestinationInfo.GetRealPath();
	return MoveFileEx(GetRealPath().Path, DestinationRealPath.Path, MOVEFILE_REPLACE_EXISTING) != 0;
}

bool ZEDirectoryInfo::Copy(const char* Destination)
{
	if ((GetAccess() & ZE_PA_READ) == 0)
		return false;

	if (!IsExists())
		return false;

	ZEDirectoryInfo DestinationInfo(Destination);
	if ((DestinationInfo.GetAccess() & ZE_PA_WRITE) == 0)
		return false;

	ZERealPath DestinationRealPath = DestinationInfo.GetRealPath();
	return CopyFile(GetRealPath().Path, DestinationRealPath.Path, TRUE) != 0;
}

bool ZEDirectoryInfo::Delete()
{
	if ((GetAccess() & ZE_PA_WRITE) == 0)
		return false;

	if (!IsExists())
		return false;

	return DeleteFile(GetRealPath().Path) != 0;
}

bool ZEDirectoryInfo::Create()
{
	if ((GetAccess() & ZE_PA_WRITE) == 0)
		return false;

	if (IsExists())
		return true;

	return CreateDirectoryA(GetPath(), NULL) != 0;
}

#endif

ZEDirectoryInfo::ZEDirectoryInfo()
{

}

ZEDirectoryInfo::ZEDirectoryInfo(const char* Path)
{
	SetPath(Path);
}

ZEDirectoryInfo::ZEDirectoryInfo(const char* ParentPath, const char* RelativePath)
{
	SetRelativePath(ParentPath, RelativePath);
}
