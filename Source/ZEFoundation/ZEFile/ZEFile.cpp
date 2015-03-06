//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFile.cpp
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

#define WIN32_LEAN_AND_MEAN

#include "ZEError.h"
#include "ZEFile.h"
#include "ZEPathManager.h"

#pragma warning(push)
#pragma warning(disable:4996 4267)

#include "ZEFileInfo.h"

#include <stdio.h>

#if defined ZE_PLATFORM_WINDOWS
	#include <cerrno>

    static ZEString GetErrorString(ZEInt ErrorId)
    {
        int Result;
        ZEString Error;

        char ErrorString[256];

        Result = strerror_s(ErrorString, 256, ErrorId);
        if (Result == 0)
        {
            // Use returned string to form error message
            Error += ErrorString;

        }
        else // Fail
        {
            // Use ErrorId as string
            Error += "ErrorId: ";
            Error += ZEString::FromInt32(ErrorId);
        }

        return Error;
    }

	static FILE* FileOpen(const ZEString& Path, const ZEString& Mode)
	{
		return _wfopen(Path.ToWCString(), Mode.ToWCString());
	}

#elif defined ZE_PLATFORM_UNIX
	#include <errno.h>
	#include <fcntl.h>
	#include <string.h>

	static ZEString GetErrorString(ZEInt32 ErrorId)
	{
		ZEString Error;

		char ErrorString[256] = "UNKNOWN";

        void* Result = (void*)strerror_r((int)ErrorId, ErrorString, 256);
		if (Result == NULL)
		{
			// Use returned string to form error message
			Error += ErrorString;

		}
		else // Fail
		{
			// Use ErrorId as string
			Error += "ErrorId: ";
			Error += ZEString::FromInt32(ErrorId);
		}

		return Error;
	}

	static FILE* FileOpen(const ZEString& Path, const ZEString& Mode)
	{
		return fopen(Path.ToCString(), Mode.ToCString());
	}
#endif


#if defined(ZE_PLATFORM_LINUX)
    #define _fseeki64 fseeko64
    #define _ftelli64 ftello64
#elif defined(ZE_PLATFORM_MACOSX)
	#define _fseeki64 fseeko
	#define _ftelli64 ftello
#endif


ZEFile::ZEFile()
{
	File = NULL;
	OpenMode = ZE_FOM_READ;
	CreationMode = ZE_FCM_NONE;
}

ZEFile::~ZEFile()
{
	Close();
}

ZEInt ZEFile::Close()
{
	if (File == NULL)
		return 0;

	int Result;

	errno = 0;
	Result = fclose((FILE*)File);
	if (Result != 0)
	{
		zeError("Error: \"%s\" occurred in file: \"%s\".", GetErrorString(errno).ToCString(), Path.ToCString());
		return Result;
	}

	Path.Clear();

	File = NULL;
	OpenMode = ZE_FOM_READ;
	CreationMode = ZE_FCM_NONE;

	return Result;
}

static bool FileCreate(const ZEString& Path, bool DeleteExisting)
{
	int Result;
	FILE* File;
	ZEString Mode;

	if (DeleteExisting)
		Mode = "wb+";
	else
		Mode = "ab";
	
	errno = 0;
	File = 	FileOpen(Path, Mode);
	if (File == NULL)
	{
		zeError("Error: \"%s\" occurred in file: \"%s\".", GetErrorString(errno).ToCString(), Path.ToCString());
		return false;
	}

	errno = 0;
	Result = fclose((FILE*)File);
	if (Result != 0)
	{
		zeError("Error: \"%s\" occurred in file: \"%s\".", GetErrorString(errno).ToCString(), Path.ToCString());
		return false;
	}

	return true;
}

bool ZEFile::Open(const ZEString& FilePath, const ZEFileOpenMode FileOpenMode, const ZEFileCreationMode FileCreationMode)
{
	zeDebugCheck(FilePath.IsEmpty(), "Empty path.");
	zeDebugCheck(File != NULL, "File is already open.");

	ZEString ModeStr;
	ZERealPath RealPath = ZEFileInfo(FilePath).GetRealPath();

	if (RealPath.Access == ZE_PA_NO_ACCESS)
	{
		zeError("File access denied. File: \"%s\".", RealPath.Path.ToCString());
		return false;
	}
	else if (FileOpenMode == ZE_FOM_READ && (RealPath.Access & ZE_PA_READ) == 0)
	{
		zeError("File read access denied. File: \"%s\".", RealPath.Path.ToCString());
		return false;
	}
	else if (FileOpenMode == ZE_FOM_WRITE && (RealPath.Access & ZE_PA_WRITE) == 0)
	{
		zeError("File read access denied. File: \"%s\".", RealPath.Path.ToCString());
		return false;
	}

	// Handle file creation
	switch (FileCreationMode)
	{
		case ZE_FCM_NONE:
		{
			errno = 0;
			// If file is not there give error
			FILE* Valid = FileOpen(RealPath.Path, ZEString("rb"));
			if (Valid == NULL)
			{
				zeError("Error: \"%s\" occurred in file: \"%s\".", GetErrorString(errno).ToCString(), RealPath.Path.ToCString());
				return false;
			}
			else
			{
				fclose(Valid);
			}
			break;
		}

		case ZE_FCM_CREATE:
			// If cannot create give error
			if (!FileCreate(RealPath.Path, false))
			{
				zeError("Cannot create file. File: \"%s\".", RealPath.Path.ToCString());
				return false;
			}
			break;

		case ZE_FCM_OVERWRITE:
			// If cannot create or overwrite give error
			if (!FileCreate(RealPath.Path, true))
			{
				zeError("Cannot create file. File: \"%s\".", RealPath.Path.ToCString());
				return false;
			}
			break;

		default:
			zeError("Unknown creation type");
			return false;
	}

	// Handle file mode
	switch (FileOpenMode)
	{
		case ZE_FOM_READ:
			ModeStr = "rb";
			break;

		case ZE_FOM_WRITE:
			ModeStr = "wb";
			break;

		case ZE_FOM_READ_WRITE:
			ModeStr = "r+b";
			break;

		default:
			zeError("Unknown open mode");
			return false;
			break;
	}

	// FILE OPEN MODES
	//*********************************************************
	// r  - open for reading
	// w  - open for writing (file need not exist)
	// a  - open for appending (file need not exist)
	// r+ - open for reading and writing, start at beginning
	// w+ - open for reading and writing (overwrite file)
	// a+ - open for reading and writing (append if file exists)

	// Open file
	errno = 0;
	File = (void*)FileOpen(RealPath.Path, ModeStr);
	if (File == NULL)
	{
		zeError("Error: \"%s\" occurred in file: \"%s\".", GetErrorString(errno).ToCString(), RealPath.Path.ToCString());
		return false;
	}

	Path = RealPath.Path;
	OpenMode = FileOpenMode;
	CreationMode = FileCreationMode;

	return true;
}

ZESize ZEFile::Read(void* Buffer, const ZESize Size, const ZESize Count)
{
	zeDebugCheck(File == NULL, "File is closed.");

	return fread(Buffer, Size, Count, (FILE*)File);
}

ZESize ZEFile::Write(const void* Buffer, const ZESize Size, const ZESize Count)
{
	zeDebugCheck(File == NULL, "File is closed.");

	return fwrite(Buffer, Size, Count, (FILE*)File);
}

ZEInt ZEFile::Seek(const ZEInt64 Offset, const ZESeekFrom Origin)
{
	zeDebugCheck(File == NULL, "File is closed.");

	int Mode;

	switch (Origin)
	{
		case ZE_SF_BEGINING:
			Mode = SEEK_SET;
			break;
		case ZE_SF_CURRENT:
			Mode = SEEK_CUR;
			break;
		case ZE_SF_END:
			Mode = SEEK_END;
			break;
		default:
			zeError("Unknown origin type");
			return -1;
			break;
	}

	return _fseeki64((FILE*)File, Offset, Mode);
}


ZEInt64 ZEFile::Tell() const
{
	zeDebugCheck(File == NULL, "File is closed.");

	return _ftelli64((FILE*)File);
}

ZEInt ZEFile::Eof() const
{
	zeDebugCheck(File == NULL, "File is closed.");

	return feof((FILE*)File) ;
}

ZEInt ZEFile::Flush() const
{
	zeDebugCheck(File == NULL, "File is closed.");

	return fflush((FILE*)File);
}

bool ZEFile::IsOpen() const
{
	return (File == NULL) ? false : true;
}

ZEInt64 ZEFile::GetSize()
{
	zeDebugCheck(File == NULL, "File is closed.");

	ZEInt64 Cursor = Tell();
	Seek(0, ZE_SF_END);
	ZEInt64 Size = Tell();
	Seek(Cursor, ZE_SF_BEGINING);

	return Size;
}

ZEFileOpenMode ZEFile::GetOpenMode() const
{
	return OpenMode;
}

ZEFileCreationMode ZEFile::GetCreationMode() const
{
	return CreationMode;
}

bool ZEFile::ReadFile(const ZEString& FilePath, void* Buffer, const ZESize BufferSize)
{
	ZERealPath RealPath = ZEFileInfo(FilePath).GetRealPath();
	if ((RealPath.Access & ZE_PA_READ) == 0)
		return false;

	ZEFile File;
	if (!File.Open(RealPath.Path, ZE_FOM_READ, ZE_FCM_NONE))
		return false;

	ZEInt64 FileSize = File.GetSize();
	zeDebugCheck(BufferSize < (ZESize)FileSize, "File size exceeds buffer size.");

	ZESize ReadSize = BufferSize > (ZESize)FileSize ? (ZESize)FileSize : BufferSize;

	if (File.Read(Buffer, sizeof(char), ReadSize) != ReadSize)
	{
		File.Close();
		return false;
	}

	File.Close();
	return true;
}

bool ZEFile::ReadTextFile(const ZEString& FilePath, char* Buffer, const ZESize BufferSize)
{
	ZERealPath RealPath = ZEFileInfo(FilePath).GetRealPath();
	if ((RealPath.Access & ZE_PA_READ) == 0)
		return false;

	ZEFile File;
	if (!File.Open(RealPath.Path, ZE_FOM_READ, ZE_FCM_NONE))
		return false;

	ZEInt64 FileSize = File.GetSize();
	zeDebugCheck(BufferSize < (ZESize)FileSize + 1, "File size exceeds buffer size.");

	ZESize ReadSize = BufferSize > (ZESize)FileSize ? (ZESize)FileSize : BufferSize;

	if (File.Read((void*)Buffer, sizeof(char), ReadSize) != ReadSize)
	{
		File.Close();
		return false;
	}

	Buffer[ReadSize] = '\0';
	File.Close();
	return true;
}

void* ZEFile::GetHandle() const
{
	return File;
}

const ZEString& ZEFile::GetPath() const
{
	return Path;
}

ZEFile&	ZEFile::operator = (ZEFile& OtherFile)
{
	File = OtherFile.File;
	Path = OtherFile.Path;
	OpenMode = OtherFile.OpenMode;
	CreationMode = OtherFile.CreationMode;

	return *this;
}

#pragma warning(pop)
