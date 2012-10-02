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

#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <time.h>


static bool GetStats(ZEString& Path, struct _stati64 *Buffer)
{
	int Result;

	SetLastError(ERROR_SUCCESS);
	Result = _wstati64(Path.ToWCString(), Buffer);
	if (Result != 0)
	{
// 		ZEString ErrorString;
// 		ZEFileUtils::GetErrorString(ErrorString, GetLastError());
// 		zeError("Can not get info.\nError: %s", ErrorString.ToCString());
		return false;
	}

	return true;
}

void ZEFileUtils::GetErrorString(ZEString& ErrorString, const ZEInt ErrorId)
{
	DWORD Return = 0;
	LPWSTR Buffer = NULL;

	Return = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)ErrorId, 0, Buffer, 0, NULL);
	if(Return == 0) // Fail
	{
		// Use ErrorId as string
		ErrorString  = "Cannot convert error to string. ";
		ErrorString += "ErrorId = ";
		ErrorString += ZEString::FromInt32(ErrorId);
		ErrorString += ".";
	}
	else // Success
	{
		// Use returned string to form error message
		ErrorString = Buffer;
		LocalFree(Buffer);
	}
}

// Converts file size of two 32 bits integer into ZESize
static ZEInt64 OSFileSizetoZESize(ZEUInt32 SizeHigh, ZEUInt32 SizeLow)
{
	ULARGE_INTEGER Temp;

	Temp.LowPart = SizeLow;
	Temp.HighPart = SizeHigh;

	return (ZEInt64)Temp.QuadPart;
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
	ZEString Temp = Path;
	struct _stati64 Buffer;

	Result = GetStats(Temp, &Buffer);
	if (Result)
	{
		// Found it
		if(Buffer.st_mode & S_IFREG)
			return true;
	}

	return false;
}

bool ZEFileUtils::IsDirectory(const ZEString& Path)
{
	bool Result;
	ZEString Temp = Path;
	struct _stati64 Buffer;

	Result = GetStats(Temp, &Buffer);
	if (Result)
	{
		// Found it
		if(Buffer.st_mode & S_IFDIR)
			return true;
	}

	return false;
}

bool ZEFileUtils::IsFile(const ZEFileSearchStream* FindData)
{
	zeDebugCheck(FindData == NULL, "NUll pointer");
	
	return !((FindData->Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}

bool ZEFileUtils::IsDirectory(const ZEFileSearchStream* FindData)
{
	zeDebugCheck(FindData == NULL, "NUll pointer");

	return ((FindData->Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}

ZEString ZEFileUtils::GetFileName(const ZEFileSearchStream* FindData)
{
	zeDebugCheck(FindData == NULL, "NUll pointer");

	return ZEString(FindData->Data.cFileName);
}

ZEInt64 ZEFileUtils::GetFileSize(const ZEString& Path)
{
	ZEString Temp = Path;
	struct _stati64 Buffer;

	return GetStats(Temp, &Buffer) ? (ZEInt64)Buffer.st_size : (ZEInt64)-1;
}

ZEInt64 ZEFileUtils::GetFileSize(const ZEFileSearchStream* FindData)
{
	zeDebugCheck(FindData == NULL, "NUll pointer");

	return OSFileSizetoZESize(FindData->Data.nFileSizeHigh, FindData->Data.nFileSizeLow);
}

bool ZEFileUtils::GetCreationTime(ZEFileTime* Output, const ZEString& Path)
{
	zeDebugCheck(Output == NULL, "NUll pointer");

	ZEString Temp = Path;
	struct tm TimeInfo;
	struct _stati64 Buffer;

	if (!GetStats(Temp, &Buffer))
		return false;

	if (localtime_s(&TimeInfo, &Buffer.st_ctime) != 0)
		return false;

	Output->Day = TimeInfo.tm_mday;
	Output->Hour = TimeInfo.tm_hour;
	Output->Year = TimeInfo.tm_year + 1900;
	Output->Month = TimeInfo.tm_mon + 1;
	Output->Second = TimeInfo.tm_sec;
	Output->Minute = TimeInfo.tm_min;
	Output->DayOfWeek = TimeInfo.tm_wday;
	Output->Milliseconds = 0;

	return true;
}

void ZEFileUtils::GetCreationTime(ZEFileTime* Output, const ZEFileSearchStream* FindData)
{
	zeDebugCheck(Output == NULL, "NUll pointer");
	zeDebugCheck(FindData == NULL, "NUll pointer");
	
	ZEFileTimeOS FileTime;
	FileTime.dwLowDateTime =  FindData->Data.ftCreationTime.dwLowDateTime;
	FileTime.dwHighDateTime = FindData->Data.ftCreationTime.dwHighDateTime;
	
	OSFileTimetoZEFileTime(Output, &FileTime);
}

bool ZEFileUtils::GetModificationTime(ZEFileTime* Output, const ZEString& Path)
{
	ZEString Temp = Path;
	struct _stati64 Buffer;

	zeDebugCheck(Output == NULL, "NUll pointer");

	if (!GetStats(Temp, &Buffer))
		return false;

	struct tm* TimeInfo = _localtime64(&Buffer.st_mtime);
	if (TimeInfo == NULL)
		return false;

	Output->Day = TimeInfo->tm_mday;
	Output->Hour = TimeInfo->tm_hour;
	Output->Year = TimeInfo->tm_year + 1900;
	Output->Month = TimeInfo->tm_mon + 1;
	Output->Second = TimeInfo->tm_sec;
	Output->Minute = TimeInfo->tm_min;
	Output->DayOfWeek = TimeInfo->tm_wday;
	Output->Milliseconds = 0;

	return true;
}

void ZEFileUtils::GetModificationTime(ZEFileTime* Output, const ZEFileSearchStream* FindData)
{
	zeDebugCheck(Output == NULL, "NUll pointer");
	zeDebugCheck(FindData == NULL, "NUll pointer");

	ZEFileTimeOS FileTime;
	FileTime.dwLowDateTime =  FindData->Data.ftLastWriteTime.dwLowDateTime;
	FileTime.dwHighDateTime = FindData->Data.ftLastWriteTime.dwHighDateTime;

	OSFileTimetoZEFileTime(Output, &FileTime);
}

bool ZEFileUtils::CloseSearchStream(ZEFileSearchStream* FindData)
{
	int Result;

	zeDebugCheck(FindData == NULL, "NUll pointer");

	if (FindData->Handle == INVALID_HANDLE_VALUE || FindData->Handle == NULL)
		return true;

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

	if (FindData->Handle == INVALID_HANDLE_VALUE || FindData->Handle == NULL)
		return false;

	return FindNextFileW(FindData->Handle, &FindData->Data) != 0;
}

bool ZEFileUtils::OpenSearchStream(ZEFileSearchStream* FindData, const ZEString& Path)
{
	zeDebugCheck(FindData == NULL, "Null Pointer");
	zeDebugCheck(Path.IsEmpty(), "Empty string..");

	if (FindData->Handle == INVALID_HANDLE_VALUE || FindData->Handle == NULL)
		return false;
	
	ZEString SearchPath = Path + "\\*";
	FindData->Handle = FindFirstFileW(SearchPath.ToWCString(), &FindData->Data);
	if (FindData->Handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	return true;
}
