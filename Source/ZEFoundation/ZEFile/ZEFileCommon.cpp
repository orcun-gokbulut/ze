//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileCommon.cpp
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
#include "ZEFileCommon.h"

#include <windows.h>

typedef OSFileTime FILETIME ;
typedef ZEFileSearchData WIN32_FIND_DATA ;


void ZEFileCommon::GetErrorString(ZEString& ErrorString, ZEUInt32 ErrorId)
{
	DWORD Return;
	LPTSTR s;

	Return = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, ErrorId, 0, (LPTSTR)&s, 0, NULL);
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



ZESize ZEFileCommon::FileSizetoZESize(ZEUInt32 SizeHigh, ZEUInt32 SizeLow)
{
	ULARGE_INTEGER Temp;

	Temp.LowPart = SizeLow;
	Temp.HighPart = SizeHigh;

	return (ZESize)Temp.QuadPart;
}

bool ZEFileCommon::FILETIMEtoZEFileTime(ZEFileTime *Time, OSFileTime *FileTime)
{
	BOOL Result;
	SYSTEMTIME SystemTime;
	SYSTEMTIME LocalSystemTime;

	Result = FileTimeToSystemTime(&FileTime, &SystemTime);
	if (!Result)
		return false;

	Result = SystemTimeToTzSpecificLocalTime(NULL, &SystemTime, &LocalSystemTime);
	if (!Result)
		return false;

	Time.Year = LocalSystemTime.wYear;
	Time.Month = LocalSystemTime.wMonth;
	Time.DayOfWeek = LocalSystemTime.wDayOfWeek;
	Time.Day = LocalSystemTime.wDay;
	Time.Hour = LocalSystemTime.wHour;
	Time.Minute = LocalSystemTime.wMinute;
	Time.Second = LocalSystemTime.wSecond;
	Time.Milliseconds = LocalSystemTime.wMilliseconds;

	return true;
}

bool ZEFileCommon::CloseSearchHandle(void* SearchHandle)
{
	return FindClose((HANDLE)SearchHandle) != 0;
}

bool ZEFileCommon::GetNextFileFolderInfo(void* OldSearchHandle, ZEFileSearchData* FindData)
{
	return FindNextFile((HANDLE)OldSearchHandle, FindData) != 0;
}

bool ZEFileCommon::GetFileFolderInfo(const ZEString& Path, ZEFileSearchData* FindData, void** SearchHandle)
{
	HANDLE FirstFileHandle;

	FirstFileHandle = FindFirstFile(Path.ToCString(), FindData);
	if (FirstFileHandle == INVALID_HANDLE_VALUE) 
		return false;

	if (*SearchHandle != NULL)
		*SearchHandle = FirstFileHandle;
	else
		FindClose(FirstFileHandle);

	return true;
}
