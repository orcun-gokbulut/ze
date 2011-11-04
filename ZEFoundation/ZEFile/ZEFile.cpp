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

#include "ZEFile.h"
#include "ZEError.h"
#include "ZEDefinitions.h"

#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <fstream>

#pragma warning(push)
#pragma warning(disable:4996 4267)

ZEFile::ZEFile()
{
	File = NULL;
	FileCursor = 0;
}

ZEFile::~ZEFile()
{
	Close();
}

const ZEString& ZEFile::GetFileName() const
{
	return FileName;
}

void* ZEFile::GetFileHandle() const
{
	return File;
}

bool ZEFile::Eof()
{
	if(!File)
		return false;

	return (feof((FILE*)File) == 0) ? false : true;
}

bool ZEFile::Seek(int Offset, ZESeekFrom Origin)
{
	if(!File)
		return false;

	int OriginNorm;
	switch(Origin)
	{
		case ZE_SF_BEGINING:
			OriginNorm = SEEK_SET;
			break;
		case ZE_SF_CURRENT:
			fseek((FILE*)File, FileCursor, SEEK_SET);
			OriginNorm = SEEK_CUR;
			break;
		case ZE_SF_END:
			OriginNorm = SEEK_END;
			break;
	}
	
	if (fseek((FILE*)File, Offset, OriginNorm) == 0)
	{	
		FileCursor = ftell((FILE*)File);
		return true;
	}
	else
		return false;
}

size_t ZEFile::Tell()
{
	if(!File)
		return 0;

	return FileCursor;
}

void ZEFile::Close()
{
	if (File != NULL)
	{	
		fclose((FILE*)File);
		File = NULL;
		FileCursor = 0;
	}
}

void ZEFile::Flush()
{
	if(!File)
		return;

	fflush((FILE*)File);
}

bool ZEFile::IsOpen()
{
	if(File && FileName[0])
		return true;

	return false;
}

bool ZEFile::Open(const ZEString FileName, ZEFileMode Mode, bool Binary)
{
	zeAssert(File, "Close the previous file first");

	this->FileName = FileName;

	const char* StrMode = NULL;
	const char*	AltStrMode = NULL;

	switch(Mode)
	{
		case ZE_FM_READ_ONLY:
		{
			if (Binary)		
				StrMode = "rb";
			else
				StrMode = "r";
			break;
		}
			
		case ZE_FM_WRITE_ONLY:
		{
			if (Binary)
			{
				StrMode = "r+b";
				AltStrMode = "w+b";
			}
			else
			{
				StrMode = "r+";
				AltStrMode = "w+";
			}
			break;
		}
			
		case ZE_FM_APPEND_ONLY:
		{
			if (Binary)
				StrMode = "ab";
			else
				StrMode = "a";
			break;
		}

		case ZE_FM_READ_APPEND:
		{
			if(Binary)
				StrMode = "a+b";
			else
				StrMode = "a+";
			break;
		}
			
		case ZE_FM_READ_WRITE:
		{
			if(Binary)
			{
				StrMode = "r+b";
				AltStrMode = "w+b";
			}
			else
			{
				StrMode = "r+";
				AltStrMode = "w+";
			}
			break;
		}
			
		default:
			return false;
			break;	
	}

	// Try to open file
	File = fopen(this->FileName, StrMode);

	if(File == NULL) // If cant open
	{
		if(AltStrMode) // If there is an alternative opening mode
		{
			// Try to open with alternative mode
			File = fopen(this->FileName, AltStrMode);
			
			if(File != NULL) // If opened
			{
				return true;
			}
			else
			{
				zeError("Could not open file \"%s\".", (const char*)FileName);
				return false;
			}
		}
		else
		{
			zeError("Could not open file \"%s\".", (const char*)FileName);
			return false;
		}
	}
	else
	{
		return true;
	}

	return false;
}

size_t ZEFile::Read(void* Buffer, size_t Size, size_t Count)
{
	if(!File)
		return 0;

	fseek((FILE*)File, FileCursor, SEEK_SET);
	size_t ReadCount = fread(Buffer, Size, Count, (FILE*)File);
	FileCursor += ReadCount * Size;

	return ReadCount;
}

size_t ZEFile::ReadFormated(const char* Format, ...)
{
	if(!File)
		return 0;

	zeAssert(true, "Not Implemented Yet.");

//	fseek((FILE*)File, FileCursor, SEEK_SET);

// 	va_list ArgList;
// 
// 	va_start(ArgList, Format);
// 	size_t ReadCount = _tinput_l((FILE*)File, Format, 0, ArgList);
// 	va_end(ArgList);
//	FileCursor += ReadCount;
//	return ReadCount;
	return 0;
}

size_t ZEFile::Write( const void* Buffer, size_t Size, size_t Count)
{
	if(!File)
		return 0;

	fseek((FILE*)File, FileCursor, SEEK_SET);
	size_t WriteCount = fwrite(Buffer, Size, Count, (FILE*)File);
	FileCursor += WriteCount * Size;

	return WriteCount;
}

size_t ZEFile::WriteFormated(const char* Format, ...)
{
	if(!File)
		return 0;

	fseek((FILE*)File, FileCursor, SEEK_SET);

	va_list ArgList;
	va_start(ArgList, Format);
	size_t WriteSize = vfprintf((FILE*)File, Format, ArgList);
	va_end(ArgList);

	FileCursor += WriteSize;

	return WriteSize;
}

size_t ZEFile::GetFileSize(const char* FileName)
{
	struct stat FileStatus;

	if(stat( FileName, &FileStatus ) != 0)
		return 0;

	return FileStatus.st_size;
}
size_t ZEFile::GetFileSize()
{
	if (!File)
		return 0;

	fseek((FILE*)File, 0, SEEK_END);
	size_t EndCursor = ftell((FILE*)File);
	fseek((FILE*)File, FileCursor, SEEK_SET);

	return EndCursor;
}

bool ZEFile::ReadFile(const ZEString FileName, void* Buffer, size_t BufferSize)
{
	char RelativeFileName[ZE_MAX_NAME_SIZE + 11];
	sprintf_s(RelativeFileName, ZE_MAX_NAME_SIZE + 11, "resources\\%s", (const char*)FileName);

	FILE* File = fopen(RelativeFileName, "rb");
	if(File == NULL)
	{
		zeError("Could not open file in binary read mode \"%s\".", (const char*)FileName);
		return false;
	}

	fseek((FILE*)File, 0, SEEK_END);
	size_t FileSize = ftell(File);
	fseek((FILE*)File, 0, SEEK_SET);

	zeAssert(BufferSize < FileSize, "File size exceed buffer size.");

	fread(Buffer, sizeof(char), (BufferSize > FileSize ? FileSize : BufferSize), File);

	fclose(File);

	return true;
}

bool ZEFile::ReadTextFile(const ZEString FileName, char* Buffer, size_t BufferSize)
{
	char RelativeFileName[ZE_MAX_NAME_SIZE + 11];

	sprintf_s(RelativeFileName, ZE_MAX_NAME_SIZE + 11, "resources\\%s", (const char*)FileName);

	FILE* File = fopen(RelativeFileName, "rb");
	if(File == NULL)
	{
		zeError("Could not open file in binary read mode \"%s\".", (const char*)FileName);
		return false;
	}

	fseek(File, 0, SEEK_END);
	size_t FileSize = ftell(File);
	fseek(File, 0, SEEK_SET);

	zeAssert(BufferSize < FileSize + 1, "File size exceed buffer size.");

	if (BufferSize < FileSize + 1)
		return false;

	fread(Buffer, sizeof(char), FileSize, File);
	Buffer[FileSize] = '\0';

	fclose(File);
	return true;
}

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>

ZEString ZEFile::GetAbsolutePath(const ZEString Path)
{
	return "";
}

bool ZEFile::IsDirectoryExists(const ZEString Path)
{
	WIN32_FIND_DATA Info;
	HANDLE Handle = FindFirstFile(Path, &Info);

	BOOL Result = Handle != INVALID_HANDLE_VALUE;
	while(Result)
	{
		if ((Info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			return true;
		
		Result = FindNextFile(Handle, &Info);
	}

	FindClose(Handle);

	return false;
}

ZEString ZEFile::GetParentDirectory(const ZEString Path)
{
	size_t Length = Path.GetLength();
	
	if (Length == 0)
		return "";

	for (int I = Length - 1; I >= 0; I--)
	{
		if (Path[I] == '\\')
			return Path.Left(I);
	}
	
	return "";
}

bool ZEFile::IsFileExists(const ZEString Path)
{
	WIN32_FIND_DATA Info;
	HANDLE Handle = FindFirstFile(Path, &Info);

	BOOL Result = Handle != INVALID_HANDLE_VALUE;
	while(Result)
	{
		if (Info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY == 0)
			return true;

		Result = FindNextFile(Handle, &Info);
	}

	FindClose(Handle);

	return false;
}

ZEString ZEFile::GetFileName(const ZEString Path)
{
	size_t Length = Path.GetLength();

	if (Length == 0)
		return "";

	for (int I = Length - 1; I >= 0; I--)
	{
		if (Path[I] == '\\')
			return Path.Right(Length - 1 - I);
	}

	return Path;
}

ZEString ZEFile::GetFileExtension(const ZEString Path)
{
	return "";
}

/////////////////////////////////////////////////////////////////////////
//////////////////		ZEPartialFile		/////////////////////////////
/////////////////////////////////////////////////////////////////////////

ZEPartialFile::ZEPartialFile() : StartPosition(0), EndPosition(0), IsEof(false)
{
	// Empty
}

ZEPartialFile::~ZEPartialFile()
{
	File = NULL;
}

// Should not be used
bool ZEPartialFile::Open(const ZEString FileName, ZEFileMode Mode, bool Binary)
{
	return false;
}

bool ZEPartialFile::Open(ZEFile* ParentFile, size_t Offset, size_t Size)
{
	zeAssert(Size == 0, "Cannot open a PartialFile with 0 size");

	IsEof			= false;
	StartPosition	= Offset;
	FileCursor		= StartPosition;
	EndPosition		= Offset + Size;
	this->File		= ParentFile->GetFileHandle();

	this->FileName = ParentFile->GetFileName();
	
	fseek((FILE*)this->File, StartPosition, SEEK_SET);
	return true;
}

void ZEPartialFile::Close()
{
	StartPosition	= 0;
	EndPosition		= 0;
	File			= NULL;
	IsEof			= false;
}

size_t ZEPartialFile::GetFileSize()
{
	return EndPosition - StartPosition;	
}

size_t ZEPartialFile::Read(void* Buffer, size_t Size, size_t Count)
{
	if(!File)
		return 0;

	// Goto this File's cursor position
	fseek((FILE*)File, FileCursor, SEEK_SET);

	if(FileCursor < StartPosition || EndPosition < FileCursor)
		return 0;

	// Calculate the maximum possible read count
	size_t ReadEndPos = ftell((FILE*)File) + Size * Count;
	if (ReadEndPos > EndPosition)
	{
		Count -= (ReadEndPos - EndPosition) / Size + (((ReadEndPos - EndPosition) % Size) == 0 ? 0 : 1);
		IsEof = true;
	}

	// Read
	size_t ReadCount = fread(Buffer, Size, Count, (FILE*)File);
	FileCursor += ReadCount * Size;

	return ReadCount;
}


size_t ZEPartialFile::ReadFormated(const char* Format, ...)
{
	if(!File)
		return 0;

	// Goto this File's cursor position
	fseek((FILE*)File, FileCursor, SEEK_SET);

	// Check if the cursor is in the file boundary
	if(FileCursor < StartPosition || EndPosition < FileCursor)
		return 0;

	zeAssert(true, "Not Implemented Yet.");
// 	va_list ArgList;
// 
// 	va_start(ArgList, Format);
// 	size_t CharCount = _tinput_l(File, Format, 0, ArgList);
// 	va_end(ArgList);

	return 0;
}

size_t ZEPartialFile::Write(void* Buffer, size_t Size, size_t Count)
{
	if(!File)
		return 0;

	// Goto this File's cursor position
	fseek((FILE*)File, FileCursor, SEEK_SET);

	// Check if the cursor is in the file boundary
	if(FileCursor < StartPosition || EndPosition < FileCursor)
		return 0;

	// Calculate the maximum possible write count
	size_t WriteEndPos = ftell((FILE*)File) + Size * Count;
	if (WriteEndPos > EndPosition)
	{
		Count -= (WriteEndPos - EndPosition) / Size + (((WriteEndPos - EndPosition) % Size) == 0 ? 0 : 1);
		IsEof = true;
	}

	size_t WriteCount = fwrite(Buffer, Size, Count, (FILE*)File);
	FileCursor += WriteCount * Size;

	return WriteCount;
}

size_t ZEPartialFile::WriteFormated(const char* Format, ...)
{
	if(!File)
		return 0;

	fseek((FILE*)File, FileCursor, SEEK_SET);
	if(FileCursor < StartPosition || EndPosition < FileCursor)
		return 0;

	va_list ArgList;

	va_start(ArgList, Format);
	size_t WriteSize = vfprintf((FILE*)File, Format, ArgList);
	FileCursor += WriteSize;
	va_end(ArgList);

	return WriteSize;
}



bool ZEPartialFile::Seek(int Offset, ZESeekFrom Origin)
{
	if(!File)
		return 0;

	switch(Origin)
	{
		case ZE_SF_BEGINING:
		{
			if(Offset < 0 || EndPosition < StartPosition + Offset)
				return false;

			if(fseek((FILE*)File, StartPosition + Offset, SEEK_SET) != 0)
				return false;

			break;
		}

		case ZE_SF_CURRENT:
		{
			fseek((FILE*)File, FileCursor, SEEK_SET);

			size_t CurrentPosition = 0;
			CurrentPosition = ftell((FILE*)File);

			if(CurrentPosition + Offset < StartPosition || EndPosition < CurrentPosition + Offset)
				return false;

			if(fseek((FILE*)File, Offset, SEEK_CUR) != 0)
				return false;

			break;
		}

		case ZE_SF_END:
		{
			if(EndPosition + Offset < StartPosition || Offset > 0)
				return false;

			if(fseek((FILE*)File, EndPosition + Offset, SEEK_SET) != 0)
				return false;

			break;
		}

		default:
			return false;
			break;
	}

	FileCursor = ftell((FILE*)File);

	if(IsEof == true && FileCursor >= StartPosition && FileCursor < EndPosition)
		IsEof = false;

	return true;
}

size_t ZEPartialFile::Tell()
{
	return FileCursor - StartPosition;
}

bool ZEPartialFile::Eof()
{
	return IsEof;
}

#pragma warning(pop)





