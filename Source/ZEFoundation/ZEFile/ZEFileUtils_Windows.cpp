//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileUtils_Windows.cpp
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

#include "ZEError.h"
#include "ZEFileUtils.h"

#include <sys/stat.h>
#include <cerrno>
#include <time.h>


static bool GetStats(const ZEString& Path, struct stat* Stat)
{
	int Result;

	SetLastError(ERROR_SUCCESS);
	Result = stat(Path.ToCString(), Stat);
	if (Result != 0)
	{
		ZEString ErrorString;
		ZEFileUtils::GetErrorString(ErrorString, GetLastError());
		zeError("Can not get info.\nError: %s", ErrorString.ToCString());
		return false;
	}

	return true;
}

void ZEFileUtils::GetErrorString(ZEString& ErrorString, const ZEInt ErrorId)
{
	DWORD Return;
	LPTSTR s;

	Return = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, (DWORD)ErrorId, 0, (LPTSTR)&s, 0, NULL);
	if(Return == 0) // Fail
	{
		// Use ErrorId as string
		ErrorString = "ErrorId: ";
		ErrorString += ZEString::FromInt32(ErrorId);
		ErrorString += ".";
	}
	else // Success
	{
		// Use returned string to form error message
		ErrorString = ZEString::FromCString(s);
		LocalFree(s);
	}
}

// Converts file size of two 32 bits integer into ZESize
static ZESize OSFileSizetoZESize(ZEUInt32 SizeHigh, ZEUInt32 SizeLow)
{
	ULARGE_INTEGER Temp;

	Temp.LowPart = SizeLow;
	Temp.HighPart = SizeHigh;

	return (ZESize)Temp.QuadPart;
}

static bool OSFileTimetoZEFileTime(ZEFileTime *Time, ZEFileTimeOS *FileTime)
{
	BOOL Result;
	SYSTEMTIME SystemTime;
	SYSTEMTIME LocalSystemTime;

	zeDebugCheck(Time == NULL, "NUll pointer");
	zeDebugCheck(FileTime == NULL, "NUll pointer");

	Result = FileTimeToSystemTime(FileTime, &SystemTime);
	if (!Result)
		return false;

	Result = SystemTimeToTzSpecificLocalTime(NULL, &SystemTime, &LocalSystemTime);
	if (!Result)
		return false;

	Time->Year = LocalSystemTime.wYear;
	Time->Month = LocalSystemTime.wMonth;
	Time->DayOfWeek = LocalSystemTime.wDayOfWeek;
	Time->Day = LocalSystemTime.wDay;
	Time->Hour = LocalSystemTime.wHour;
	Time->Minute = LocalSystemTime.wMinute;
	Time->Second = LocalSystemTime.wSecond;
	Time->Milliseconds = LocalSystemTime.wMilliseconds;

	return true;
}

bool ZEFileUtils::IsFile(const ZEString& Path)
{
	bool Result;
	struct stat Stats;

	Result = GetStats(Path, &Stats);
	if (Result)
	{
		// Found it
		if(Stats.st_mode & S_IFREG)
			return true;
	}

	return false;
}

bool ZEFileUtils::IsDirectory(const ZEString& Path)
{
	bool Result;
	struct stat Stats;

	Result = GetStats(Path, &Stats);
	if (Result)
	{
		// Found it
		if(Stats.st_mode & S_IFDIR)
			return true;
	}

	return false;
}

bool ZEFileUtils::IsFile(const ZEFileSearchStream* FindData)
{
	return !((FindData->Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}

bool ZEFileUtils::IsDirectory(const ZEFileSearchStream* FindData)
{
	return ((FindData->Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}

ZEString ZEFileUtils::GetFileName(const ZEFileSearchStream* FindData)
{
	return ZEString(FindData->Data.cFileName);
}

ZESize ZEFileUtils::GetFileSize(const ZEString& Path)
{
	struct stat Stat;

	return GetStats(Path, &Stat) ? (ZESize)Stat.st_size : 0;
}

ZESize ZEFileUtils::GetFileSize(const ZEFileSearchStream* FindData)
{
	return OSFileSizetoZESize(FindData->Data.nFileSizeHigh, FindData->Data.nFileSizeLow);
}

bool ZEFileUtils::GetCreationTime(ZEFileTime* Output, const ZEString& Path)
{
	struct tm TimeInfo;
	struct stat Stat;

	if (!GetStats(Path, &Stat))
		return false;

	if (localtime_s(&TimeInfo, &Stat.st_ctime) != 0)
		return false;

	Output->Day = (ZEInt16)TimeInfo.tm_mday;
	Output->Hour = (ZEInt16)TimeInfo.tm_hour;
	Output->Year = (ZEInt16)TimeInfo.tm_year + 1900;
	Output->Month = (ZEInt16)TimeInfo.tm_mon;
	Output->Second = (ZEInt16)TimeInfo.tm_sec;
	Output->Minute = (ZEInt16)TimeInfo.tm_min;
	Output->DayOfWeek = (ZEInt16)TimeInfo.tm_wday;
	Output->Milliseconds = (ZEInt16)0;

	return true;
}

void ZEFileUtils::GetCreationTime(ZEFileTime* Output, const ZEFileSearchStream* FindData)
{
	ZEFileTimeOS FileTime;
	FileTime.dwLowDateTime =  FindData->Data.ftCreationTime.dwLowDateTime;
	FileTime.dwHighDateTime = FindData->Data.ftCreationTime.dwHighDateTime;
	
	OSFileTimetoZEFileTime(Output, &FileTime);
}

bool ZEFileUtils::GetModificationTime(ZEFileTime* Output, const ZEString& Path)
{
	struct stat Stat;

	if (!GetStats(Path, &Stat))
		return false;

	struct tm TimeInfo;
	if (localtime_s(&TimeInfo, &Stat.st_mtime) != 0)
		return false;

	Output->Day = (ZEInt16)TimeInfo.tm_mday;
	Output->Hour = (ZEInt16)TimeInfo.tm_hour;
	Output->Year = (ZEInt16)TimeInfo.tm_year + 1900;
	Output->Month = (ZEInt16)TimeInfo.tm_mon;
	Output->Second = (ZEInt16)TimeInfo.tm_sec;
	Output->Minute = (ZEInt16)TimeInfo.tm_min;
	Output->DayOfWeek = (ZEInt16)TimeInfo.tm_wday;
	Output->Milliseconds = (ZEInt16)0;

	return true;
}

void ZEFileUtils::GetModificationTime(ZEFileTime* Output, const ZEFileSearchStream* FindData)
{
	ZEFileTimeOS FileTime;
	FileTime.dwLowDateTime =  FindData->Data.ftLastWriteTime.dwLowDateTime;
	FileTime.dwHighDateTime = FindData->Data.ftLastWriteTime.dwHighDateTime;

	OSFileTimetoZEFileTime(Output, &FileTime);
}

bool ZEFileUtils::CloseSearchStream(ZEFileSearchStream* FindData)
{
	int Result;

	zeDebugCheck(FindData == NULL, "NUll pointer");

	SetLastError(ERROR_SUCCESS);
	Result = FindClose(FindData->Handle);
	if (Result == 0)
	{
		ZEString ErrorString;
		GetErrorString(ErrorString, GetLastError());
		zeError("Can not close search handle.\nError: %s", ErrorString.ToCString());
		return false;
	}

	return true;
}

bool ZEFileUtils::FindNextInStream(ZEFileSearchStream *FindData)
{
	zeDebugCheck(FindData == NULL, "NUll pointer");

	return FindNextFile(FindData->Handle, &FindData->Data) != 0;
}

bool ZEFileUtils::OpenSearchStream(ZEFileSearchStream* FindData, const ZEString& Path)
{
	zeDebugCheck(FindData == NULL, "Null Pointer");
	zeDebugCheck(Path.IsEmpty(), "Empty string..");
	
	ZEString SearchPath = Path + "\\*";
	FindData->Handle = FindFirstFile(SearchPath.ToCString(), &FindData->Data);
	if (FindData->Handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	return true;
}
