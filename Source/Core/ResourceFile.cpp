//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ResourceFile.cpp
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

#include "Definitions.h"
#include "ResourceFile.h"
#include "Core/Core.h"

#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>

#pragma warning(push)
#pragma warning(disable:4996 4267)

const char* ZEResourceFile::GetFilename()
{
	return Filename;
}

void* ZEResourceFile::GetFileHandle()
{
	return File;
}

void ZEResourceFile::GetPartialResourceFile(ZEPartialResourceFile& PartialResourceFile, size_t StartPosition, size_t EndPosition)
{
	sprintf(PartialResourceFile.Filename, "%s:(%d, &d)", Filename, StartPosition, EndPosition);
	PartialResourceFile.File = this->File;
	PartialResourceFile.StartPosition = StartPosition;
	PartialResourceFile.EndPosition = EndPosition;
	Seek(StartPosition, SEEKFROM_BEGINING);
}

bool ZEResourceFile::Open(const char* Filename)
{
	char RelativeFilename[ZE_MAX_NAME_SIZE + 11];
	strncpy(this->Filename, Filename, ZE_MAX_FILE_NAME_SIZE);

	sprintf_s(RelativeFilename, ZE_MAX_NAME_SIZE + 11, "resources\\%s", Filename);

	File = fopen(RelativeFilename, "rb");
	if(File != NULL)
		return true;
	else
	{
		zeError("Resource File", "Could not open resource file \"%s\".", Filename);
		return false;
	}
}

bool ZEResourceFile::Seek(size_t Offset, ZESeekFrom Origin)
{
	if (fseek((FILE*)File, Offset, Origin) == 0)
		return true;
	else
		return false;
}

size_t ZEResourceFile::Read(void* Buffer, size_t Size, size_t Count)
{
	return fread(Buffer, Size, Count, (FILE*)File);
}

size_t ZEResourceFile::FormatedRead(void* Buffer, size_t BufferSize, void* Format, ...)
{
	ZEASSERT(true, "NOT IMPLAMENTED !!!");
	return 0;
}

size_t ZEResourceFile::Tell()
{
	return ftell((FILE*)File);
}

bool ZEResourceFile::Eof()
{
	return (bool)feof((FILE*)File);
}

void ZEResourceFile::Close()
{
	if (File != NULL)
		fclose((FILE*)File);
}

bool ZEResourceFile::ReadFile(const char* Filename, void* Buffer, size_t BufferSize)
{
	char RelativeFilename[ZE_MAX_NAME_SIZE + 11];
	sprintf_s(RelativeFilename, ZE_MAX_NAME_SIZE + 11, "resources\\%s", Filename);

	FILE* File = fopen(RelativeFilename, "rb");
	if(File == NULL)
	{
		zeError("Resource File", "Could not open resource file \"%s\".", Filename);
		return false;
	}

	fseek(File, 0, SEEK_END);
	size_t FileSize = ftell(File);
	fseek(File, 0, SEEK_SET);

	ZEASSERT(BufferSize < FileSize, "File size exceed buffer size.");

	fread(Buffer, sizeof(char), (BufferSize > FileSize ? FileSize : BufferSize), File);

	fclose(File);
	
	return true;
}

bool ZEResourceFile::ReadTextFile(const char* Filename, char* Buffer, size_t BufferSize)
{
	char RelativeFilename[ZE_MAX_NAME_SIZE + 11];

	sprintf_s(RelativeFilename, ZE_MAX_NAME_SIZE + 11, "resources\\%s", Filename);

	FILE* File = fopen(RelativeFilename, "rb");
	if(File == NULL)
	{
		zeError("Resource File", "Could not open resource file \"%s\".", Filename);
		return false;
	}

	fseek(File, 0, SEEK_END);
	size_t FileSize = ftell(File);
	fseek(File, 0, SEEK_SET);

	ZEASSERT(BufferSize < FileSize + 1, "File size exceed buffer size.");

	if (BufferSize < FileSize + 1)
		return false;

	fread(Buffer, sizeof(char), FileSize, File);
	Buffer[FileSize + 1] = '\0';

	fclose(File);
	return true;
}

ZEResourceFile::ZEResourceFile()
{
	File = NULL;
}

ZEResourceFile::~ZEResourceFile()
{
	Close();
}

bool ZEPartialResourceFile::Seek(size_t Offset, ZESeekFrom Origin)
{
	switch(Origin)
	{
		case SEEKFROM_BEGINING:
			if (fseek((FILE*)File, StartPosition + Offset, SEEK_SET) != 0)
				return false;
			break;
		case SEEKFROM_CURRENT:
			if (fseek((FILE*)File, Offset, SEEK_CUR) != 0)
				return false;
			break;
		case SEEKFROM_END:
			if (fseek((FILE*)File, EndPosition + Offset, SEEK_SET) != 0)
				return false;
			break;
	}

	if (ftell((FILE*)File) < (long)StartPosition || ftell((FILE*)File) > (long)EndPosition)
		return false;
	else
	{
		IsEof = false;	
		return true;
	}
}

size_t ZEPartialResourceFile::Read(void* Buffer, size_t Size, size_t Count)
{
	size_t ReadLastPos = ftell((FILE*)File) + Size * Count;
	if (ReadLastPos > EndPosition)
	{
		Count -= (ReadLastPos - EndPosition) / Size;
		IsEof = true;
	}

	size_t ReadBytes = fread(Buffer, Size, Count, (FILE*)File);
	zeWarning("Resource File", "Read bytes is less than requested read size."
		"(Read : %d, Requested : %d (ElementSize : %d, Count : %d), FileName : \"%s\", Cursor : %d)", 
		ReadBytes, Size * Count, Size, Count, GetFilename(), Tell());
	return ReadBytes;
}

size_t ZEPartialResourceFile::Tell()
{
	return ftell((FILE*)File) - StartPosition;
}

ZEPartialResourceFile::ZEPartialResourceFile()
{
	IsEof = false;
}

bool ZEPartialResourceFile::Eof()
{
	return IsEof;
}

#pragma warning(pop)
