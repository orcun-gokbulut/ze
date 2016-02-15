//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileInfo.cpp
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

#include "ZEDS/ZEFormat.h"
#include "ZEPathManager.h"
#include "ZEDirectoryInfo.h"
#include "ZEPlatform.h"

#ifdef ZE_PLATFORM_WINDOWS
#include <Windows.h>
#include <shlwapi.h>

ZEInt64 ZEFileInfo::GetSize() const
{
	if ((GetAccess() & ZE_PA_READ) == 0)
		return -1;

	HANDLE Handle = CreateFile(GetRealPath().Path, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (Handle == INVALID_HANDLE_VALUE)
		return false;

	LARGE_INTEGER FileSize;
	if (!GetFileSizeEx(Handle, &FileSize))
		return -1;

	return FileSize.QuadPart;
}

bool ZEFileInfo::Rename(const char* Name) const
{
	if ((GetAccess() & ZE_PA_WRITE) == 0)
		return false;

	const char* Temp = Name;
	while (*Temp == '\0')
	{
		if (*Temp == '\\' || *Temp == ':' || *Temp == '/')
			return false;
		Temp++;
	}

	ZEString Destination = ZEFormat::Format("{0}\\{1}", GetParentDirectory(), Name);

	ZERealPath DestinationRealPath = ZEFileInfo(Destination).GetRealPath();
	return MoveFileEx(GetRealPath().Path, DestinationRealPath.Path, MOVEFILE_REPLACE_EXISTING) != 0;
}

bool ZEFileInfo::Move(const char* Destination) const
{
	if ((GetAccess() & ZE_PA_READ_WRITE) == 0)
		return false;

	ZEDirectoryInfo DestinationInfo(Destination);
	if ((DestinationInfo.GetAccess() & ZE_PA_WRITE) == 0)
		return false;

	ZERealPath DestinationRealPath = DestinationInfo.GetRealPath();
	return MoveFileEx(GetRealPath().Path, DestinationRealPath.Path, MOVEFILE_REPLACE_EXISTING) != 0;
}

bool ZEFileInfo::Copy(const char* Destination) const
{
	if ((GetAccess() & ZE_PA_READ) == 0)
		return false;

	ZEDirectoryInfo DestinationInfo(Destination);
	if ((DestinationInfo.GetAccess() & ZE_PA_WRITE) == 0)
		return false;

	ZERealPath DestinationRealPath = DestinationInfo.GetRealPath();
	return CopyFile(GetRealPath().Path, DestinationRealPath.Path, FALSE) != 0;
}

bool ZEFileInfo::Delete() const
{
	if ((GetAccess() & ZE_PA_WRITE) == 0)
		return false;

	return DeleteFile(GetRealPath().Path) != 0;
}

bool ZEFileInfo::Touch() const
{
	if ((GetAccess() & ZE_PA_WRITE) == 0)
		return false;

	SYSTEMTIME Time;
	GetSystemTime(&Time);

	FILETIME FileTime;
	SystemTimeToFileTime(&Time, &FileTime);

	HANDLE Handle = CreateFile(GetRealPath().Path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (Handle == INVALID_HANDLE_VALUE)
		return false;

	if (SetFileTime(Handle, &FileTime, &FileTime, &FileTime) == 0)
	{
		CloseHandle(Handle);
		return false;
	}

	CloseHandle(Handle);
	return true;
}

#endif

ZEFileInfo::ZEFileInfo()
{

}

ZEFileInfo::ZEFileInfo(const char* Path)
{
	SetPath(Path);
}
