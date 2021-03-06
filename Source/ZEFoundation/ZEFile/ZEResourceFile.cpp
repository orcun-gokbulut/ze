//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEResourceFile.cpp
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

#include "ZEDefinitions.h"
#include "ZEResourceFile.h"
#include "ZEError.h"

#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#pragma warning(push)
#pragma warning(disable:4996 4267)


ZEResourceFile::ZEResourceFile() : File(NULL), FileCursor(0)
{
	// Empty
}

ZEResourceFile::~ZEResourceFile()
{
	Close();
}

bool ZEResourceFile::IsOpen()
{
	if(File && FileName[0])
		return true;

	return false;
}

const char* ZEResourceFile::GetFileName()
{
	return FileName;
}

void* ZEResourceFile::GetFileHandle()
{
	return File;
}

void ZEResourceFile::GetPartialResourceFile(ZEPartialResourceFile& PartialResourceFile, size_t StartPosition, size_t EndPosition)
{
	sprintf(PartialResourceFile.FileName, "%s:(%d, &d)", FileName, StartPosition, EndPosition);
	PartialResourceFile.File = this->File;
	PartialResourceFile.StartPosition = StartPosition;
	PartialResourceFile.EndPosition = EndPosition;
	Seek(StartPosition, ZE_SF_BEGINING);
}

bool ZEResourceFile::Open(const char* FileName)
{
	zeAssert(File, "Close the previous ZEResourceFile first.");

	char RelativeFileName[ZE_MAX_NAME_SIZE + 11];
	strncpy(this->FileName, FileName, ZE_MAX_FILE_NAME_SIZE);

	sprintf_s(RelativeFileName, ZE_MAX_NAME_SIZE + 11, "resources\\%s", FileName);

	File = fopen(RelativeFileName, "rb");
	if(File != NULL)
		return true;
	else
	{
		zeError("Could not open resource file \"%s\".", FileName);
		return false;
	}
}

bool ZEResourceFile::Seek(size_t Offset, ZESeekFrom Origin)
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

size_t ZEResourceFile::Read(void* Buffer, size_t Size, size_t Count)
{
	if(!File)
		return 0;

	fseek((FILE*)File, FileCursor, SEEK_SET);
	size_t ReadCount = fread(Buffer, Size, Count, (FILE*)File);
	FileCursor += ReadCount * Size;

	return ReadCount;
}

size_t ZEResourceFile::FormatedRead(void* Buffer, size_t BufferSize, void* Format, ...)
{
	if(!File)
		return 0;

	zeAssert(true, "NOT IMPLAMENTED !!!");
	return 0;
}

size_t ZEResourceFile::Tell()
{
	return FileCursor;
}

bool ZEResourceFile::Eof()
{
	if(!File)
		return false;

	return (feof((FILE*)File) == 0) ? false : true;
}

void ZEResourceFile::Close()
{
	if (File)
	{
		fclose((FILE*)File);
		File = NULL;
		FileCursor = 0;
	}
}

bool ZEResourceFile::ReadFile(const char* FileName, void* Buffer, size_t BufferSize)
{
	char RelativeFileName[ZE_MAX_NAME_SIZE + 11];
	sprintf_s(RelativeFileName, ZE_MAX_NAME_SIZE + 11, "resources\\%s", FileName);

	FILE* File = fopen(RelativeFileName, "rb");
	if(File == NULL)
	{
		zeError("Could not open resource file \"%s\".", FileName);
		return false;
	}

	fseek(File, 0, SEEK_END);
	size_t FileSize = ftell(File);
	fseek(File, 0, SEEK_SET);

	zeAssert(BufferSize < FileSize, "File size exceed buffer size.");

	fread(Buffer, sizeof(char), (BufferSize > FileSize ? FileSize : BufferSize), File);

	fclose(File);
	
	return true;
}

bool ZEResourceFile::ReadTextFile(const char* FileName, char* Buffer, size_t BufferSize)
{
	char RelativeFileName[ZE_MAX_NAME_SIZE + 11];

	sprintf_s(RelativeFileName, ZE_MAX_NAME_SIZE + 11, "resources\\%s", FileName);

	FILE* File = fopen(RelativeFileName, "rb");
	if(File == NULL)
	{
		zeError("Could not open resource file \"%s\".", FileName);
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

bool ZEPartialResourceFile::Open(const char* FileName, size_t Offset, size_t Size)
{
	if(File)
	{
		zeError("A file is already open with the file name: \"%s\".", this->FileName);
		return false;
	}

	char RelativeFileName[ZE_MAX_NAME_SIZE + 11];
	strncpy(this->FileName, FileName, ZE_MAX_FILE_NAME_SIZE);

	sprintf_s(RelativeFileName, ZE_MAX_NAME_SIZE + 11, "resources\\%s", FileName);

	File = fopen(RelativeFileName, "rb");
	if(File != NULL)
	{
		IsEof			= false;
		StartPosition	= Offset;
		FileCursor		= StartPosition;
		EndPosition		= Offset + Size;
		return true;
	}
	else
	{
		zeError("Could not open resource file \"%s\".", FileName);
		return false;
	}
}

bool ZEPartialResourceFile::Open(ZEFile* File, size_t Offset, size_t Size)
{
	if(!File->IsOpen())
	{
		zeError("The ZEFile passed to this function must be opened: \"%s\".", this->FileName);
		return false;
	}

	strncpy(this->FileName, File->GetFileName(), ZE_MAX_FILE_NAME_SIZE);

	this->File = File->GetFileHandle();
	if(File)
	{
		IsEof			= false;
		StartPosition	= Offset;
		FileCursor		= StartPosition;
		EndPosition		= Offset + Size;
		return true;
	}	
	else
	{
		zeError("Could not open resource file using given ZEFile\"%s\".", File->GetFileName());
		return false;
	}
}

void ZEPartialResourceFile::Close()
{
	if(File)
	{		
		StartPosition	= 0;
		EndPosition		= 0;
		File			= NULL;
		IsEof			= false;
	}
}

bool ZEPartialResourceFile::Seek(size_t Offset, ZESeekFrom Origin)
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

size_t ZEPartialResourceFile::Read(void* Buffer, size_t Size, size_t Count)
{
	if(!File)
		return 0;

	// Goto this File's cursor position
	fseek((FILE*)File, FileCursor, SEEK_SET);

	if(FileCursor < StartPosition || EndPosition < FileCursor)
		return 0;

	size_t ReadLastPos = ftell((FILE*)File) + Size * Count;
	if (ReadLastPos > EndPosition)
	{
		Count -= (ReadLastPos - EndPosition) / Size + (((ReadLastPos - EndPosition) % Size) == 0 ? 0 : 1);
		IsEof = true;
	}

	// Read
	size_t ReadCount = fread(Buffer, Size, Count, (FILE*)File);
	FileCursor += ReadCount * Size;

	return ReadCount;
}

size_t ZEPartialResourceFile::Tell()
{
	return FileCursor - StartPosition;
}

ZEPartialResourceFile::ZEPartialResourceFile() : IsEof(false), StartPosition(0), EndPosition(0)
{
	// Empty
}

bool ZEPartialResourceFile::Eof()
{
	return IsEof;
}

#pragma warning(pop)
