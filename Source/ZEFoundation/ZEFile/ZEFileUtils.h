//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileUtils.h
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


#ifndef __ZE_FILE_UTILS_H__
#define __ZE_FILE_UTILS_H__

#include "ZETypes.h"
#include "ZEDS/ZEString.h"

#if defined ZE_PLATFORM_WINDOWS

	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>

	typedef FILETIME ZEFileTimeOS;

	struct ZEFileSearchStream
	{
		WIN32_FIND_DATAW	Data;
		HANDLE				Handle;
	};

#elif defined ZE_PLATFORM_UNIX

    #include "fcntl.h"
    #include <time.h>
    #include <dirent.h>

	typedef time_t ZEFileTimeOS;

	struct ZEFileSearchStream
	{
		struct stat Data;
		DIR*        Directory;
		ZEString    Name;
	};

#endif

struct ZEFileTime
{
	ZEInt	Year;
	ZEInt	Month;
	ZEInt	DayOfWeek;
	ZEInt	Day;
	ZEInt	Hour;
	ZEInt	Minute;
	ZEInt	Second;
	ZEInt	Milliseconds;
};

class ZEFileUtils
{
	public:
		static bool					IsFile(const ZEString& Path);
		static bool					IsFile(const ZEFileSearchStream* FindData);

		static bool					IsDirectory(const ZEString& Path);
        static bool					IsDirectory(const ZEFileSearchStream* FindData);

        static ZEString				GetFileName(const ZEFileSearchStream* FindData);

        static ZEInt64              GetFileSize(const ZEString& Path);
        static ZEInt64				GetFileSize(const ZEFileSearchStream* FindData);

        static bool                 GetCreationTime(ZEFileTime* Output, const ZEString& Path);
        static void					GetCreationTime(ZEFileTime* Output, const ZEFileSearchStream* FindData);

        static bool                 GetModificationTime(ZEFileTime* Output, const ZEString& Path);
        static void					GetModificationTime(ZEFileTime* Output, const ZEFileSearchStream* FindData);

        static void					GetErrorString(ZEString& ErrorString, const ZEInt ErrorId);

        static bool					OpenSearchStream(ZEFileSearchStream* FindData, const ZEString& Path);
        static bool					FindNextInStream(ZEFileSearchStream* FindData);
        static bool					CloseSearchStream(ZEFileSearchStream* FindData);

};

#endif
