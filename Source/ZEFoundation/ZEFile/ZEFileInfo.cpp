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

const ZEString&	ZEFileInfo::GetPath()
{
	return Path;
}

ZEString ZEFileInfo::GetFullName()
{
	ZESSize Length = Path.GetLength();
	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if (Path[I] == '\\' || Path[I] == '/')
			return Path.Right(Length - 1 - I);
	}

	return Path;
}

ZEString ZEFileInfo::GetName()
{
	ZESSize Length = Path.GetLength();
	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if (Path[I] == '\\' || Path[I] == '/')
			return Path.Right(Length - 1 - I);
	}

	return Path;
}

ZEString ZEFileInfo::GetExtension()
{
	ZESSize Length = Path.GetLength();

	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if(Path[I] == '.')
			return Path.Right(Length - I);
	}

	return "";
}

ZEString ZEFileInfo::GetParentDirectory()
{
	ZESSize Length = Path.GetLength();

	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if (Path[I] == '\\' || Path[I] == '/')
			return Path.Left(I);
	}

	return "";
}

ZEString ZEFileInfo::GetRealPath()
{
	ZERealPath RealPath = ZEPathManager::GetInstance()->GetRealPath(Path);
	if (RealPath.Access == ZE_PA_NO_ACCESS)
		return "";

	if (RealPath.Root == ZE_PR_NONE)
		return "";

	return RealPath.Path;
}

ZEPathRoot ZEFileInfo::GetRoot()
{
	return ZEPathManager::GetInstance()->GetRoot(Path);
}

ZEPathAccess ZEFileInfo::GetAccess()
{
	return ZEPathManager::GetInstance()->GetRoot(GetRoot()).Access;
}

#ifdef ZE_PLATFORM_WINDOWS

#include <Windows.h>
#include <shlwapi.h>

ZEInt64 ZEFileInfo::GetSize()
{
	HANDLE Handle = CreateFile(Path, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (Handle == INVALID_HANDLE_VALUE)
		return false;

	LARGE_INTEGER FileSize;
	if (!GetFileSizeEx(Handle, &FileSize))
		return -1;

	return FileSize.QuadPart;
}

bool ZEFileInfo::IsExists()
{
	return PathFileExists(GetRealPath());
}

bool ZEFileInfo::IsInsidePackage()
{
	return false;
}

ZEFileTime ZEFileInfo::GetCreationDate()
{
	HANDLE Handle = CreateFile(Path, GENERIC_WRITE, FILE_SHARE_WRITE,	NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (Handle == INVALID_HANDLE_VALUE)
		return ZEFileTime();

	FILETIME CreationTime, Temp0, Temp1;
	if (!GetFileTime(Handle, &CreationTime, &Temp0, &Temp1))
	{
		CloseHandle(Handle);
		return ZEFileTime();
	}

	CloseHandle(Handle);

	SYSTEMTIME Time;
	FileTimeToSystemTime(&CreationTime, &Time);

	ZEFileTime Output;
	Output.Year = Time.wYear;
	Output.Month = Time.wMonth;
	Output.DayOfWeek = Time.wDayOfWeek;
	Output.Day = Time.wDay;
	Output.Hour = Time.wHour;
	Output.Minute = Time.wMinute;
	Output.Second = Time.wSecond;
	Output.Milliseconds = Time.wMilliseconds;

	return Output;
}

ZEFileTime ZEFileInfo::GetModificationTime()
{
	HANDLE Handle = CreateFile(Path, GENERIC_WRITE, FILE_SHARE_WRITE,	NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (Handle == INVALID_HANDLE_VALUE)
		return ZEFileTime();

	FILETIME ModificationTime, Temp0, Temp1;
	if (!GetFileTime(Handle, &Temp0, &Temp1, &ModificationTime))
	{
		CloseHandle(Handle);
		return ZEFileTime();
	}

	CloseHandle(Handle);

	SYSTEMTIME Time;
	FileTimeToSystemTime(&ModificationTime, &Time);

	ZEFileTime Output;
	Output.Year = Time.wYear;
	Output.Month = Time.wMonth;
	Output.DayOfWeek = Time.wDayOfWeek;
	Output.Day = Time.wDay;
	Output.Hour = Time.wHour;
	Output.Minute = Time.wMinute;
	Output.Second = Time.wSecond;
	Output.Milliseconds = Time.wMilliseconds;

	return Output;
}

bool ZEFileInfo::Rename(const char* Name)
{
	const char* Temp = Name;
	while (*Temp == '\0')
	{
		if (*Temp == '\\' || *Temp == ':' || *Temp == '/')
			return false;
		Temp++;
	}

	ZEString Destination = ZEFormat::Format("{0}\\{1}", GetParentDirectory(), Name);

	ZERealPath DestinationRealPath = ZEPathManager::GetInstance()->GetRealPath(Destination);
	return MoveFileEx(GetRealPath(), DestinationRealPath.Path, MOVEFILE_REPLACE_EXISTING);
}

bool ZEFileInfo::Move(const char* Destination)
{
	ZERealPath DestinationRealPath = ZEPathManager::GetInstance()->GetRealPath(Destination);
	return MoveFileEx(GetRealPath(), DestinationRealPath.Path, MOVEFILE_REPLACE_EXISTING);
}

bool ZEFileInfo::Copy(const char* Destination)
{
	ZERealPath DestinationRealPath = ZEPathManager::GetInstance()->GetRealPath(Destination);
	return CopyFile(GetRealPath(), DestinationRealPath.Path, TRUE);
}

bool ZEFileInfo::Delete()
{
	return DeleteFile(GetRealPath());
}

bool ZEFileInfo::Touch()
{
	SYSTEMTIME Time;
	GetSystemTime(&Time);

	FILETIME FileTime;
	SystemTimeToFileTime(&Time, &FileTime);

	HANDLE Handle = CreateFile(GetRealPath(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
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

ZEFileInfo ZEFileInfo::Populate(const char* Path)
{
	ZEFileInfo Info;
	Info.Path = Path;

	return Info;
}

#endif
