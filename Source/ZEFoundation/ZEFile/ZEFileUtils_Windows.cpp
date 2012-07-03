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
#include "ZEFileTypes_Windows.h"


void ZEFileUtils::GetErrorString(ZEString& ErrorString, ZEUInt32 ErrorId)
{
	DWORD Return;
	LPTSTR s;

	Return = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, (DWORD)ErrorId, 0, (LPTSTR)&s, 0, NULL);
	if(Return == 0) // Fail
	{
		// Use ErrorId as string
		ErrorString = "ErrorId: ";
		ErrorString += ZEString::FromUInt32(ErrorId);
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

static bool OSFileTimetoZEFileTime(ZEFileTime *Time, OSFileTime *FileTime)
{
	BOOL Result;
	SYSTEMTIME SystemTime;
	SYSTEMTIME LocalSystemTime;

	zeDebugCheck(Time == NULL, "NUll pointer");
	zeDebugCheck(FileTime == NULL, "NUll pointer");

	Result = FileTimeToSystemTime((const FILETIME*)FileTime, &SystemTime);
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

void ZEFileUtils::DeleteOSFileSearchData(OSFileSearchData* SearchData)
{
	delete SearchData;
	SearchData = NULL;
}

OSFileSearchData* ZEFileUtils::CreateOSFileSearchData()
{
	return new OSFileSearchData;
}

bool ZEFileUtils::IsFile(const ZEString& Path)
{
	bool Result;
	void* Handle = NULL;
	OSFileSearchData SearchData;

	Result = ZEFileUtils::GetFileFolderInfo(Path.ToCString(), &SearchData, &Handle);
	if ( !Result )
		return false;

	return !((SearchData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}

bool ZEFileUtils::IsDirectory(const ZEString& Path)
{
	bool Result;
	void* Handle = NULL;
	OSFileSearchData SearchData;

	Result = ZEFileUtils::GetFileFolderInfo(Path.ToCString(), &SearchData, &Handle);
	if ( !Result )
		return false;

	return ((SearchData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}

bool ZEFileUtils::IsFile(OSFileSearchData* FindData)
{
	return !((FindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}

bool ZEFileUtils::IsDirectory(OSFileSearchData* FindData)
{
	return ((FindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}

ZEString ZEFileUtils::GetFileName(OSFileSearchData* FindData)
{
	return ZEString(FindData->cFileName);
}

ZESize ZEFileUtils::GetFileSize(OSFileSearchData* FindData)
{
	return OSFileSizetoZESize(FindData->nFileSizeHigh, FindData->nFileSizeLow);
}

void ZEFileUtils::GetCreationTime(ZEFileTime* Output, OSFileSearchData* FindData)
{
	OSFileTime FileTime;
	FileTime.dwLowDateTime =  FindData->ftCreationTime.dwLowDateTime;
	FileTime.dwHighDateTime = FindData->ftCreationTime.dwHighDateTime;
	
	OSFileTimetoZEFileTime(Output, &FileTime);
}

void ZEFileUtils::GetModificationTime(ZEFileTime* Output, OSFileSearchData* FindData)
{
	OSFileTime FileTime;
	FileTime.dwLowDateTime =  FindData->ftLastWriteTime.dwLowDateTime;
	FileTime.dwHighDateTime = FindData->ftLastWriteTime.dwHighDateTime;

	OSFileTimetoZEFileTime(Output, &FileTime);
}

bool ZEFileUtils::CloseSearchHandle(void* SearchHandle)
{
	zeDebugCheck(SearchHandle == NULL, "NUll pointer");

	return FindClose((HANDLE)SearchHandle) != 0;
}

bool ZEFileUtils::GetNextFileFolderInfo(void* OldSearchHandle, OSFileSearchData* FindData)
{
	zeDebugCheck(FindData == NULL, "NUll pointer");
	zeDebugCheck(OldSearchHandle == NULL, "NUll pointer");

	return FindNextFile((HANDLE)OldSearchHandle, (LPWIN32_FIND_DATA)FindData) != 0;
}

bool ZEFileUtils::GetFileFolderInfo(const ZEString& Path, OSFileSearchData* FindData, void** SearchHandle)
{
	HANDLE FirstFileHandle;

	zeDebugCheck(FindData == NULL, "NUll pointer");
	zeDebugCheck(*SearchHandle == NULL, "NUll pointer");

	FirstFileHandle = FindFirstFile(Path.ToCString(), (LPWIN32_FIND_DATA)FindData);
	if (FirstFileHandle == INVALID_HANDLE_VALUE) 
		return false;

	if (*SearchHandle != NULL)
		*SearchHandle = FirstFileHandle;
	else
		FindClose(FirstFileHandle);

	return true;
}
