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
#include "ZEPathUtils.h"

#include "errno.h"
#include "fcntl.h"
#include <string.h>
#include <dirent.h>

// Full path is needed to get stats of a file
// Keep search directory to get stats of file after each search
static ZEString LastSearchPath = "";


static bool GetStats(const ZEString& Path, struct stat* Stat)
{
    int Result;

    errno = 0;
    Result = stat(Path.ToCString(), Stat);
    if (Result != 0)
    {
        ZEString ErrorString;
        ZEFileUtils::GetErrorString(ErrorString, errno);
        zeError("Can not get info.\nError: %s", ErrorString.ToCString());
        return false;
    }

    return true;
}

// Converts error id to error string
void ZEFileUtils::GetErrorString(ZEString& ErrorString, const ZEInt ErrorId)
{
    char* ErrorStr;

    ErrorStr = strerror((int)ErrorId);
    if(ErrorStr != NULL) // Success
    {
        // Use returned string to form error message
        ErrorString = ZEString::FromCString(ErrorStr);

	}
    else // Fail
	{
        // Use ErrorId as string
        ErrorString = "ErrorId: ";
        ErrorString += ZEString::FromInt32(ErrorId);
        ErrorString += ".";
    }
}

// Converts operating system's OSFileTime data into human readable ZEFileTime
static bool OSFileTimetoZEFileTime(ZEFileTime* Time, ZEFileTimeOS* FileTime)
{
    zeDebugCheck(Time == NULL, "NUll pointer");
    zeDebugCheck(FileTime == NULL, "NUll pointer");

	struct tm TimeInfo;

	if (localtime_r(FileTime, &TimeInfo) == NULL)
        return false;


    Time->Day = (ZEInt16)TimeInfo.tm_mday;
    Time->Hour = (ZEInt16)TimeInfo.tm_hour;
    Time->Year = (ZEInt16)TimeInfo.tm_year + 1900;
    Time->Month = (ZEInt16)TimeInfo.tm_mon;
    Time->Second = (ZEInt16)TimeInfo.tm_sec;
    Time->Minute = (ZEInt16)TimeInfo.tm_min;
    Time->DayOfWeek = (ZEInt16)TimeInfo.tm_wday;
    Time->Milliseconds = (ZEInt16)0;

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

bool ZEFileUtils::IsFile(const ZEFileSearchStream* FindData)
{
    zeDebugCheck(FindData == NULL, "NULL Pointer");

    if(S_ISREG(FindData->Data.st_mode))
        return true;

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
        if(S_ISDIR(Stats.st_mode))
            return true;
    }

    return false;
}

bool ZEFileUtils::IsDirectory(const ZEFileSearchStream* FindData)
{
    zeDebugCheck(FindData == NULL, "NULL Pointer");

    if(S_ISDIR(FindData->Data.st_mode))
        return true;

    return false;
}

ZEString ZEFileUtils::GetFileName(const ZEFileSearchStream* FindData)
{
    zeDebugCheck(FindData == NULL, "NULL Pointer");

    return ZEString(FindData->Name);
}

ZESize ZEFileUtils::GetFileSize(const ZEString& Path)
{
    struct stat Stat;

    return GetStats(Path, &Stat) ? (ZESize)Stat.st_size : 0;
}

// Returns the size of a file from ZEFileSearchStream
ZESize ZEFileUtils::GetFileSize(const ZEFileSearchStream* FindData)
{
    // On Unix implementation just return size
    return (ZESize)FindData->Data.st_size;
}

bool ZEFileUtils::GetCreationTime(ZEFileTime* Output, const ZEString& Path)
{
    struct stat Stat;

    if (!GetStats(Path, &Stat))
        return false;

    ZEFileTimeOS FileTime = {Stat.st_ctim.tv_sec};
    OSFileTimetoZEFileTime(Output, &FileTime);

    return true;
}

// Returns the creation time of a file from ZEFileSearchStream
void ZEFileUtils::GetCreationTime(ZEFileTime* Output, const ZEFileSearchStream* FindData)
{
    ZEFileTimeOS FileTime = {FindData->Data.st_ctim.tv_sec};
    OSFileTimetoZEFileTime(Output, &FileTime);
}

bool ZEFileUtils::GetModificationTime(ZEFileTime* Output, const ZEString& Path)
{
    struct stat Stat;

    if (!GetStats(Path, &Stat))
        return false;

    ZEFileTimeOS FileTime = {Stat.st_mtim.tv_sec};
    OSFileTimetoZEFileTime(Output, &FileTime);

    return true;
}

// Returns the modification time of a file from ZEFileSearchStream
void ZEFileUtils::GetModificationTime(ZEFileTime* Output, const ZEFileSearchStream* FindData)
{
    ZEFileTimeOS FileTime = {FindData->Data.st_mtim.tv_sec};
    OSFileTimetoZEFileTime(Output, &FileTime);
}


// Closes the search stream
bool ZEFileUtils::CloseSearchStream(ZEFileSearchStream* FindData)
{
    ZEInt Return;

    zeDebugCheck(FindData == NULL, "NUll pointer");

    if (FindData->Directory == NULL)
        return true;

	errno = 0;
    Return = closedir(FindData->Directory);
    if (Return != 0)
    {
        ZEString ErrorString;
        GetErrorString(ErrorString, errno);
        zeError("Can not close search handle.\nError: %s", ErrorString.ToCString());
		return false;
    }

    FindData->Directory = NULL;
    LastSearchPath = ".";

    return true;
}

// Gets the next file info from search stream
bool ZEFileUtils::FindNextInStream(ZEFileSearchStream* FindData)
{
    ZEInt Result;
    ZEString ErrorString;
    dirent* Entry = NULL;

    zeDebugCheck(FindData == NULL, "NUll pointer");

    if (FindData->Directory == NULL)
        return false;

    // Get first entry in the folder
    errno = 0;
    Entry = readdir(FindData->Directory);
    if (Entry == NULL)
    {
        // Nothing found
        return false;
    }

    // Get info and return
    ZEString StatPath = LastSearchPath + ZEPathUtils::GetSeperator() + Entry->d_name;

    if (!GetStats(StatPath.ToCString(),&FindData->Data))
        return false;

    // Set name of search data
    FindData->Name = Entry->d_name;

    return true;
}

// Opens a file search stream and returns the first found file info
bool ZEFileUtils::OpenSearchStream(ZEFileSearchStream* FindData, const ZEString& Path)
{
    ZEInt Result;
    ZEString ErrorString;
    dirent* Entry = NULL;

    zeDebugCheck(Path.IsEmpty(), "Empty string");

    // Open directory
    errno = 0;
    FindData->Directory = opendir(Path.ToCString());
    if (FindData->Directory == NULL)
    {
        GetErrorString(ErrorString, errno);
        zeError("Can not open path.\nError: %s", ErrorString.ToCString());
        return false;
    }

    // Save path for future use
    LastSearchPath = Path;

    // Get first entry in the folder
    errno = 0;
    Entry = readdir(FindData->Directory);
    if (Entry == NULL)
    {
        CloseSearchStream(FindData);
        GetErrorString(ErrorString, errno);
        zeError("Can not read dir.\nError: %s", ErrorString.ToCString());
        return false;
    }

    // Get info and return
    ZEString StatPath = LastSearchPath + "/" + Entry->d_name;

    Result = GetStats(StatPath.ToCString(), &FindData->Data);
    if (!Result)
    {
         CloseSearchStream(FindData);
         return false;
    }

    // Set name of search data
    FindData->Name = Entry->d_name;

    return true;
}
