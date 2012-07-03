//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileUtils_Unix.cpp
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

#include "errno.h"
#include "fcntl.h"
#include <string.h>
#include <dirent.h>

// Platform dependent declerations
struct OSFileTime
{
    time_t      Time;
};

struct OSFileSearchData : public stat
{
    ZEString    Name;
};

static ZEString LastSearchPath = ".";

// Converts operationg system's OSFileTime data into human readable ZEFileTime
static bool OSFileTimetoZEFileTime(ZEFileTime *Time, OSFileTime *FileTime)
{
    zeDebugCheck(Time == NULL, "NUll pointer");
    zeDebugCheck(FileTime == NULL, "NUll pointer");

    struct tm* TimeInfo = NULL;
    TimeInfo = localtime(&FileTime->Time);

    Time->Day = (ZEInt16)TimeInfo->tm_mday;
    Time->Hour = (ZEInt16)TimeInfo->tm_hour;
    Time->Year = (ZEInt16)TimeInfo->tm_year;
    Time->Month = (ZEInt16)TimeInfo->tm_mon;
    Time->Second = (ZEInt16)TimeInfo->tm_sec;
    Time->Minute = (ZEInt16)TimeInfo->tm_min;
    Time->DayOfWeek = (ZEInt16)TimeInfo->tm_wday;
    Time->Milliseconds = (ZEInt16)0;

    return true;
}

// Returns the size of a file from OSFileSearchData
void ZEFileUtils::GetSize(ZESize* Output, OSFileSearchData* FindData)
{
    // On unix implementation just return size
    *Output = (ZESize)FindData->st_size;
}

// Returns the creation time of a file from OSFileSearchData
void ZEFileUtils::GetCreationTime(ZEFileTime* Output, OSFileSearchData* FindData)
{
    OSFileTime FileTime = {FindData->st_ctim};
    OSFileTimetoZEFileTime(Output, &FileTime);
}

// Returns the modification time of a file from OSFileSearchData
void ZEFileUtils::GetModificationTime(ZEFileTime* Output, OSFileSearchData* FindData)
{
    OSFileTime FileTime = {FindData->st_mtim};
    OSFileTimetoZEFileTime(Output, &FileTime);
}

// Converts error id to error string
void ZEFileUtils::GetErrorString(ZEString& ErrorString, ZEUInt32 ErrorId)
{
    int Return;
    char CErrorString[256 + 1];

    Return = sterror_r(ErrorId, CErrorString, 256);
    if(Return == 0) // Success
    {
        // Use returned string to form error message
        ErrorString = ZEString::FromCString(CErrorString);

	}
    else // Fail
	{
        // Use ErrorId as string
        ErrorString = "ErrorId: ";
        ErrorString += ZEString::FromUInt32(ErrorId);
        ErrorString += ".";
    }
}

// Closes the search stream
bool ZEFileUtils::CloseSearchHandle(void* SearchHandle)
{
    ZEInt Return;

    zeDebugCheck(SearchHandle == NULL, "NUll pointer");

    Return = closedir((DIR*)SearchHandle);
    if (Return != 0)
    {
        ZEString ErrorString;
        GetErrorString(&ErrorString, errno);
        zeError("Can not close search handle.\nError: %s", ErrorString.ToCString());
    }

    LastSearchPath = ".";

    return Return == 0 ? true : false;
}

// Gets the next file info from search stream
bool ZEFileUtils::GetNextFileFolderInfo(void* OldSearchHandle, OSFileSearchData* FindData)
{
    ZEInt Result;
    ZEString ErrorString;
    dirent* Entry = NULL;

    zeDebugCheck(FindData == NULL, "NUll pointer");
    zeDebugCheck(OldSearchHandle == NULL, "NUll pointer");

    // Get first entry in the folder
    errno = 0;
    Entry = readdir (Directory);
    if (Entry == NULL)
    {
        // Nothing found
        return false;
    }

    // Get info and return
    Result = stat(ZEString(LastSearchPath + Entry->d_name).ToCString(), (stat*)FindData);
    if (Result != 0)
    {
        GetErrorString(&ErrorString, errno);
        zeError("Can not get info.\nError: %s", ErrorString.ToCString());
        return false;
    }

    // Set name of search data
    FindData->Name = Entry->d_name;

    return true;
}

// Opens a file search stream and returns the first found file info
bool ZEFileUtils::GetFileFolderInfo(const ZEString& Path, OSFileSearchData* FindData, void** SearchHandle)
{
    ZEInt Result;
    ZEString ErrorString;
    dirent* Entry = NULL;
    DIR* Directory = NULL;


    zeDebugCheck(Path.IsEmpty(), "Empty string");
    zeDebugCheck(FindData == NULL, "NUll pointer");
    zeDebugCheck(SearchHandle == NULL, "NUll pointer");

    // Open directory
    errno = 0;
    Directory = opendir(Path.ToCString());
    if (Directory == NULL)
    {
        GetErrorString(&ErrorString, errno);
        zeError("Can not open path.\nError: %s", ErrorString.ToCString());
        return false;
    }

    // Save path for future use
    LastSearchPath = Path;

    // Set the search handle
    *SearchHandle = (void*)Directory;

    // Get first entry in the folder
    errno = 0;
    Entry = readdir (Directory);
    if (Entry == NULL)
    {
        GetErrorString(&ErrorString, errno);
        zeError("Can not read dir.\nError: %s", ErrorString.ToCString());
        return false;
    }

    // Get info and return
    Result = stat(ZEString(LastSearchPath + Entry->d_name).ToCString(), (stat*)FindData);
    if (Result != 0)
    {
        GetErrorString(&ErrorString, errno);
        zeError("Can not get info.\nError: %s", ErrorString.ToCString());
        return false;
    }

    // Set name of search data
    FindData->Name = Entry->d_name;

    return true;
}
