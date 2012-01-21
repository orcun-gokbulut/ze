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

#define WINDOWS_LEAN_AND_MEAN

#include "ZEError.h"
#include "ZEFile.h"
#include "ZEPack.h"
#include "ZEDefinitions.h"
#include "ZEPartialFile.h"
#include "ZECompressedFile.h"

#include <Windows.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fstream>

#include <wchar.h>

#pragma warning(push)
#pragma warning(disable:4996 4267)


bool ZEFile::Open(const ZEString& FilePath, ZEFileMode Mode, bool Binary)
{
	zeAssert(IsOpen(), "Close the previous file first");

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
		{
			return false;
			break;	
		}
	}

	// Try to open file
	File = fopen(FilePath, StrMode);

	if(File == NULL) // If cant open
	{
		if(AltStrMode) // If there is an alternative opening mode
		{
			// Try to open with alternative mode
			File = fopen(FilePath, AltStrMode);

			if(File != NULL) // If opened
			{
				this->FilePath	= FilePath;
				FileCursor		= 0;
				ReferenceCount	= 0;
				FileType		= ZE_FT_FILE;
				return true;
			}
			else
			{
				zeError("Could not open file \"%s\".", FilePath.ToCString());
				return false;
			}
		}
		else
		{
			zeError("Could not open file \"%s\".", FilePath.ToCString());
			return false;
		}
	}
	else
	{
		this->FilePath	= FilePath;
		FileCursor		= 0;
		ReferenceCount	= 0;
		FileType		= ZE_FT_FILE;
		return true;
	}

	return false;
}

bool ZEFile::Seek(ZEInt64 Offset, ZESeekFrom Origin)
{
	if (!IsOpen())
		return false;

	int OriginNorm;
	switch(Origin)
	{
		case ZE_SF_BEGINING:
			OriginNorm = SEEK_SET;
			break;
		case ZE_SF_CURRENT:
			_fseeki64((FILE*)File, FileCursor, SEEK_SET);
			OriginNorm = SEEK_CUR;
			break;
		case ZE_SF_END:
			OriginNorm = SEEK_END;
			break;
	}

	if (_fseeki64((FILE*)File, Offset, OriginNorm) == 0)
	{	
		FileCursor = _ftelli64((FILE*)File);
		return true;
	}
	else
		return false;
}

ZEUInt64 ZEFile::Tell()
{
	if (!IsOpen())
		return 0;

	return FileCursor;
}

void ZEFile::Close()
{
	if(IsOpen() && ReferenceCount == 0)
	{
		fclose((FILE*)File);
		FilePath.Clear();

		File			= NULL;
		FileCursor		= 0;
		ReferenceCount	= 0;
	}
}

bool ZEFile::Eof()
{
	if (!IsOpen())
		return false;

	return (feof((FILE*)File) == 0) ? false : true;
}

void ZEFile::Flush()
{
	if(!IsOpen())
		return;

	fflush((FILE*)File);
}

bool ZEFile::IsOpen()
{
	if(File != NULL && !FilePath.IsEmpty())
		return true;

	return false;
}

ZEUInt64 ZEFile::Read(void* Buffer, ZEUInt64 Size, ZEUInt64 Count)
{
	if(!IsOpen())
		return 0;

	_fseeki64((FILE*)File, FileCursor, SEEK_SET);
	ZEUInt64 ReadCount = fread(Buffer, (size_t)Size, (size_t)Count, (FILE*)File);
	FileCursor += ReadCount * Size;

	return ReadCount;
}

ZEUInt64 ZEFile::ReadFormated(const char* Format, ...)
{
	if(!IsOpen())
		return 0;

	zeAssert(true, "Not Implemented Yet.");

	_fseeki64((FILE*)File, FileCursor, SEEK_SET);

	va_list ArgList;

	va_start(ArgList, Format);
	ZEUInt64 ReadCount = 0;
	// 	ReadCount = _tinput_l((FILE*)File, Format, 0, ArgList);
	va_end(ArgList);
	FileCursor += ReadCount;
	return ReadCount;
}

ZEUInt64 ZEFile::Write( const void* Buffer, ZEUInt64 Size, ZEUInt64 Count)
{
	if(!IsOpen())
		return 0;

	_fseeki64((FILE*)File, FileCursor, SEEK_SET);
	ZEUInt64 WriteCount = fwrite(Buffer, (size_t)Size, (size_t)Count, (FILE*)File);
	FileCursor += WriteCount * Size;

	return WriteCount;
}

ZEUInt64 ZEFile::WriteFormated(const char* Format, ...)
{
	if(!IsOpen())
		return 0;

	_fseeki64((FILE*)File, FileCursor, SEEK_SET);

	va_list ArgList;
	va_start(ArgList, Format);
	ZEUInt64 WriteSize = vfprintf((FILE*)File, Format, ArgList);
	va_end(ArgList);

	FileCursor += WriteSize;

	return WriteSize;
}

ZEUInt64 ZEFile::GetFileSize(const ZEString& FilePath)
{
	struct stat FileStatus;

	if (stat(FilePath.GetValue(), &FileStatus) != 0)
		return 0;

	return FileStatus.st_size;
}

ZEUInt64 ZEFile::GetFileSize()
{
	if(!IsOpen())
		return 0;

	_fseeki64((FILE*)File, 0, SEEK_END);
	ZEUInt64 EndCursor = _ftelli64((FILE*)File);
	_fseeki64((FILE*)File, FileCursor, SEEK_SET);

	return EndCursor;
}

bool ZEFile::ReadFile(const ZEString& FilePath, void* Buffer, ZEUInt64 BufferSize)
{
	char RelativeFileName[ZE_MAX_NAME_SIZE + 11];
	sprintf_s(RelativeFileName, ZE_MAX_NAME_SIZE + 11, "resources\\%s", FilePath.GetValue());

	FILE* File = fopen(RelativeFileName, "rb");
	if(File == NULL)
	{
		zeError("Could not open file in binary read mode \"%s\".", FilePath.GetValue());
		return false;
	}

	_fseeki64((FILE*)File, 0, SEEK_END);
	ZEUInt64 FileSize = _ftelli64(File);
	_fseeki64((FILE*)File, 0, SEEK_SET);

	zeAssert(BufferSize < FileSize, "File size exceed buffer size.");

	fread(Buffer, sizeof(char), (BufferSize > FileSize ? (size_t)FileSize : (size_t)BufferSize), File);

	fclose(File);
	return true;
}

bool ZEFile::ReadTextFile(const ZEString& FilePath, char* Buffer, ZEUInt64 BufferSize)
{
	char RelativeFileName[ZE_MAX_NAME_SIZE + 11];

	sprintf_s(RelativeFileName, ZE_MAX_NAME_SIZE + 11, "resources\\%s", FilePath.ToCString());

	FILE* File = fopen(RelativeFileName, "rb");
	if(File == NULL)
	{
		zeError("Could not open file in binary read mode \"%s\".", FilePath.ToCString());
		return false;
	}

	_fseeki64((FILE*)File, 0, SEEK_END);
	ZEUInt64 FileSize = _ftelli64(File);
	_fseeki64((FILE*)File, 0, SEEK_SET);

	zeAssert(BufferSize < FileSize + 1, "File size exceed buffer size.");

	fread(Buffer, sizeof(char), (size_t)FileSize, File);
	Buffer[FileSize] = '\0';

	fclose(File);
	return true;
}

ZEFileType ZEFile::GetFileType() const
{
	return FileType;
}

void* ZEFile::GetFileHandle() const
{
	return File;
}

ZEUInt64 ZEFile::GetStartPosition()
{
	return 0;
}

ZEUInt64 ZEFile::GetEndPosition()
{
	return this->GetFileSize();
}

const ZEString ZEFile::GetFilePath() const
{
	return FilePath;
}

unsigned int ZEFile::GetReferenceCount() const
{
	return ReferenceCount;
}

unsigned int ZEFile::IncreaseReferenceCount()
{
	return ++ReferenceCount;
}

unsigned int ZEFile::DecreaseReferenceCount()
{
	return --ReferenceCount;
}

ZEString ZEFile::GetFileName(const ZEString& FilePath)
{
	size_t Length = FilePath.GetLength();

	if (Length == 0)
		return "";

	for (ptrdiff_t I = Length - 1; I >= 0; I--)
	{
		if (FilePath[I] == '\\' || FilePath[I] == '/')
			return FilePath.Right(Length - 1 - I);
	}

	return FilePath;
}

ZEString ZEFile::GetAbsolutePath(const ZEString& FilePath)
{
	return "";
}

ZEString ZEFile::GetFileExtension(const ZEString& FilePath)
{
	size_t Length = FilePath.GetLength();

	if (Length == 0)
		return "";

	for (ptrdiff_t I = Length - 1; I >= 0; I--)
	{
		if(FilePath[I] == '.')
			return FilePath.Right(Length - I);
	}

	return "";
}

ZEString ZEFile::GetParentDirectory(const ZEString& FilePath)
{
	size_t Length = FilePath.GetLength();

	if (Length == 0)
		return "";

	for (ptrdiff_t I = Length - 1; I >= 0; I--)
	{
		if (FilePath[I] == '\\' || FilePath[I] == '/')
			return FilePath.Left(I);
	}

	return "";
}

bool ZEFile::IsDirectoryExists(const ZEString& FilePath)
{
	struct stat FileStat; 
	
	if(stat(FilePath.GetValue(), &FileStat) == 0 ) 
	{     
		if(FileStat.st_mode & S_IFDIR)     
			return true;
	}

	return false;

	// OLD CODE
	/*WIN32_FIND_DATA Info;
	HANDLE Handle = FindFirstFile(FilePath.GetValue(), &Info);

	BOOL Result = Handle != INVALID_HANDLE_VALUE;
	while(Result)
	{
		if ((Info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			return true;

		Result = FindNextFile(Handle, &Info);
	}

	FindClose(Handle);

	return false;*/
}

bool ZEFile::IsFileExists(const ZEString& FilePath)
{
	struct stat FileStat; 

	if(stat(FilePath.GetValue(), &FileStat) == 0 ) 
	{         
		if(FileStat.st_mode & S_IFREG)
			return true;
	}

	return false;

	// OLD CODE
	/*WIN32_FIND_DATA Info;
	HANDLE Handle = FindFirstFile(FilePath.GetValue(), &Info);

	BOOL Result = Handle != INVALID_HANDLE_VALUE;
	while(Result)
	{
		if ((Info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			return true;

		Result = FindNextFile(Handle, &Info);
	}

	FindClose(Handle);

	return false;*/
}

ZEFile& ZEFile::operator = (ZEFile& OtherFile)
{	
	File = OtherFile.File;
	FilePath = OtherFile.FilePath;
	FileCursor = OtherFile.FileCursor;

	return *this;
}

// Opens and returns the first file
ZEFile* ZEFile::Open(const ZEString& FilePath)
{
	size_t		TokenStart = 0;
	size_t		TokenEnd = 0;
	size_t		Lenght = 0;
	ZEString	Token;
	ZEString	NewPath;

	Lenght = FilePath.GetSize() - 1;

	if (Lenght == 0)
		return NULL;
	
	// Clear the '\\' character from beginning and end
	if ((FilePath[0] == '\\' || FilePath[0] == '/') && (FilePath[Lenght - 1] == '\\' || FilePath[Lenght - 1] == '/'))
	{
		if(Lenght - 2 <= 0)
			return NULL;

		NewPath = FilePath.SubString(1, Lenght - 2);
		Lenght -= 2;
		
	}
	else if (FilePath[0] == '\\' || FilePath[0] == '/')
	{
		if(Lenght - 1 <= 0)
			return NULL;

		NewPath = FilePath.Right(Lenght - 1);
		Lenght -= 1;
	}
	else if (FilePath[Lenght - 1] == '\\' || FilePath[Lenght - 1] == '/')
	{
		if(Lenght - 1 <= 0)
			return NULL;

		NewPath = FilePath.Left(Lenght - 1);
		Lenght -= 1;
	}
	else
	{
		NewPath = FilePath;
	}


	// Until all the string is tokenized/visited
	while(TokenEnd <= Lenght)
	{
		// For the rest of the string starting from token end
		for(size_t I = TokenEnd; I < Lenght; I++)
		{
			if(NewPath[I] == '\\' || NewPath[I] == '/')
			{
				TokenEnd = I;
				break;
			}
		}

		// If no new token is found, between '\\' characters, since the last iteration
		// It means that the last token of the string still remains
		if(TokenEnd == Token.GetLength())
			  TokenEnd = Lenght;

		// Add the new found token to the old one and get the new path
		Token += NewPath.SubString(TokenStart, TokenEnd);

		// If path corresponds to a File
		if (IsFileExists(Token))
		{
			if (ZEPack::IsPack(Token))
			{
				ZEPack Pack;
				if(!Pack.Open(Token))
				{
					zeError("Cannot resolve the path \"%s\".", FilePath.ToCString());
					return NULL;
				}

				ZEPartialCompressedFile* PartialCompressedFile = new ZEPartialCompressedFile();
				if (!Pack.OpenChunk(*PartialCompressedFile, FilePath.Right(Token.GetLength())))
				{
					zeError("Cannot resolve the path \"%s\".", FilePath.ToCString());
					return NULL;
				}

				return (ZEFile*)PartialCompressedFile;
			}
			else
			{
				ZEFile* File = new ZEFile();
				if (!File->Open(Token, ZE_FM_READ_WRITE, true))
				{
					zeError("Cannot resolve the path \"%s\".", FilePath.ToCString());
					return NULL;
				}
				
				return File;
			}
		}
		else
		{
			TokenStart = TokenEnd += 1;
		}
	}

	return NULL;
}

ZEFile::ZEFile()
{
	File			= NULL;
	FileType		= ZE_FT_FILE;
	
	FilePath		= "";
	FileCursor		= 0;
	ReferenceCount	= 0;
}

ZEFile::~ZEFile()
{
	Close();
}

#pragma warning(pop)
