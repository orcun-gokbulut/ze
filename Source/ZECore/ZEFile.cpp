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
#include "ZEDefinitions.h"
#include "ZECore\ZEError.h"

#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#pragma warning(push)
#pragma warning(disable:4996 4267)


/////////////////////////////////////////////////////////////////////////
////////////////////////		ZEFile		/////////////////////////////
/////////////////////////////////////////////////////////////////////////


ZEFile::ZEFile()
{
	File = NULL;
}

ZEFile::~ZEFile()
{
	Close();
}

const char* ZEFile::GetFileName()
{
	return FileName;
}

void* ZEFile::GetFileHandle()
{
	return File;
}

bool ZEFile::Eof()
{
	if(!File)
		return;

	return (bool)feof((FILE*)File);
}

bool ZEFile::Seek(size_t Offset, ZESeekFrom Origin)
{
	if(!File)
		return;

	int OriginNorm;
	switch(Origin)
	{
	case ZE_SF_BEGINING:
		OriginNorm = SEEK_SET;
		break;
	case ZE_SF_CURRENT:
		OriginNorm = SEEK_CUR;
		break;
	case ZE_SF_END:
		OriginNorm = SEEK_END;
		break;
	}

	if (fseek((FILE*)File, Offset, OriginNorm) == 0)
		return true;
	else
		return false;
}

size_t ZEFile::Tell()
{
	if(!File)
		return;

	return ftell((FILE*)File);
}

void ZEFile::Close()
{
	if (File != NULL)
	{
		fclose((FILE*)File);
		File = NULL;
	}
}

bool ZEFile::Open(const char* FileName, ZEFileMode Mode, bool Binary)
{
	char RelativeFileName[ZE_MAX_NAME_SIZE + 11];
	strncpy(this->FileName, FileName, ZE_MAX_FILE_NAME_SIZE);

	sprintf_s(RelativeFileName, ZE_MAX_NAME_SIZE + 11, "resources\\%s", FileName);

	const char* StrMode;
	const char*	AltStrMode;

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

	File = fopen(RelativeFileName, StrMode);
	if(File != NULL)
		return true;
	else
	{
		File = fopen(RelativeFileName, AltStrMode);
		if(File != NULL)
			return true;
		else
		{
			zeError("ZEFile", "Could not open file \"%s\".", FileName);
			return false;
		}
	}

	return false;
}

size_t ZEFile::Read(void* Buffer, size_t Size, size_t Count)
{
	if(!File)
		return;

	return fread(Buffer, Size, Count, (FILE*)File);
}

extern "C"
int __cdecl _tinput_l(FILE*, const TCHAR*, _locale_t, va_list);

size_t ZEFile::ReadFormated(const char* Format, ...)
{
	if(!File)
		return;

	va_list ArgList;

	va_start(ArgList, Format);
	size_t CharCount = _tinput_l(File, Format, 0, ArgList);
	va_end(ArgList);

	return CharCount;
}

size_t ZEFile::Write(void* Buffer, size_t Size, size_t Count)
{
	if(!File)
		return;

	return fwrite(Buffer, Size, (FILE*)File);
}

size_t ZEFile::WriteFormated(const char* Format, ...)
{
	if(!File)
		return;

	va_list ArgList;

	va_start(ArgList, Format);
	size_t CharCount = vfprintf(File, Format, ArgList);
	va_end(ArgList);
	
	return CharCount;
}

size_t ZEFile::GetFileSize()
{
	if (!File)
		return 0;

	fseek((FILE*)File, 0, SEEK_SET);
	size_t FileStart = ftell((FILE*)File);
	fseek((FILE*)File, 0, SEEK_END);
	size_t FileEnd = ftell((FILE*)File);

	return FileEnd - FileStart;
}

bool ZEFile::ReadFile(const char* FileName, void* Buffer, size_t BufferSize)
{
	char RelativeFileName[ZE_MAX_NAME_SIZE + 11];
	sprintf_s(RelativeFileName, ZE_MAX_NAME_SIZE + 11, "resources\\%s", FileName);

	FILE* File = fopen(RelativeFileName, "rb");
	if(File == NULL)
	{
		zeError("Resource File", "Could not open resource file \"%s\".", FileName);
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

bool ZEFile::ReadTextFile(const char* FileName, char* Buffer, size_t BufferSize)
{
	char RelativeFileName[ZE_MAX_NAME_SIZE + 11];

	sprintf_s(RelativeFileName, ZE_MAX_NAME_SIZE + 11, "resources\\%s", FileName);

	FILE* File = fopen(RelativeFileName, "rb");
	if(File == NULL)
	{
		zeError("Resource File", "Could not open resource file \"%s\".", FileName);
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


/////////////////////////////////////////////////////////////////////////
//////////////////		ZEPartialFile		/////////////////////////////
/////////////////////////////////////////////////////////////////////////

ZEPartialFile::ZEPartialFile(ZEFile* File, size_t Offset, size_t Size)
{
	ParentFile = File;
	StartPosition = Offset;
	EndPosition = Offset + Size;
	IsEof = false;
}

ZEPartialFile::~ZEPartialFile()
{
	if (ParentFile)
	{
		delete ParentFile;
		ParentFile = NULL;
	}
}

size_t ZEPartialFile::Read(void* Buffer, size_t Size, size_t Count)
{
	if(!File)
		return;

	size_t ReadEndPos = ftell((FILE*)File) + Size * Count;
	if (ReadEndPos > EndPosition)
	{
		Count -= (ReadEndPos - EndPosition) / Size + (((ReadEndPos - EndPosition) % Size) == 0 ? 0 : 1);
		IsEof = true;
	}

	return fread(Buffer, Size, Count, (FILE*)File);
}

size_t ZEPartialFile::ReadFormated(const char* Format, ...)
{
	if(!File)
		return;

	va_list ArgList;

	va_start(ArgList, Format);
	size_t CharCount = _tinput_l(File, Format, 0, ArgList);
	va_end(ArgList);

	return CharCount;
}

size_t ZEPartialFile::Write(void* Buffer, size_t Size, size_t Count)
{
	if(!File)
		return;

	size_t WriteEndPos = ftell((FILE*)File) + Size * Count;

	if (WriteEndPos > EndPosition)
	{
		Count -= (WriteEndPos - EndPosition) / Size + (((WriteEndPos - EndPosition) % Size) == 0 ? 0 : 1);
		IsEof = true;
	}

	return fwrite(Buffer, Size, Count, (FILE*)File);
}

size_t ZEPartialFile::WriteFormated(const char* Format, ...)
{
	if(!File)
		return;

	va_list ArgList;

	va_start(ArgList, Format);
	size_t CharCount = vfprintf(File, Format, ArgList);
	va_end(ArgList);

	return CharCount;
}

bool ZEPartialFile::Seek(int Offset, ZESeekFrom Origin)
{
	if(!File)
		return;

	switch(Origin)
	{
		case ZE_SF_BEGINING:
		{
			if(StartPosition + Offset > EndPosition)
			{
				fseek((FILE*)File, EndPosition, SEEK_SET);
				IsEof = true;
				return false;
			}
			else if (Offset < 0)
			{
				fseek((FILE*)File, StartPosition, SEEK_SET);
				IsEof = false;
				return false;
			}
			else
			{
				fseek((FILE*)File, StartPosition + Offset, SEEK_SET);
				return true;
			}
			break;
		}

		case ZE_SF_CURRENT:
		{
			size_t CurrentPosition = 0;
			CurrentPosition = ftell((FILE*)File);
			
			if (CurrentPosition + Offset > EndPosition)
			{
				fseek((FILE*)File, EndPosition, SEEK_SET);
				IsEof = true;
				return false;
			}
			else if (CurrentPosition + Offset < StartPosition)
			{
				fseek((FILE*)File, StartPosition, SEEK_SET);
				IsEof = false;
				return false;
			}
			else
			{
				fseek((FILE*)File, CurrentPosition + Offset, SEEK_SET);
				return true;
			}
			break;
		}

		case ZE_SF_END:
		{
			if (Offset > 0)
			{
				fseek((FILE*)File, EndPosition, SEEK_SET);
				IsEof = true;
				return false;
			}
			else if (EndPosition + Offset < StartPosition)
			{
				fseek((FILE*)File, StartPosition, SEEK_SET);
				IsEof = false;
				return false;
			}
			else
			{
				fseek((FILE*)File, EndPosition + Offset, SEEK_SET);
				return true;
			}
			break;
		}

		default:
			return false;
			break;
	}
}

size_t ZEPartialFile::Tell()
{
	return ftell((FILE*)File) - StartPosition;
}

size_t ZEPartialFile::GetFileSize()
{
	return EndPosition - StartPosition;
}

bool ZEPartialFile::Eof()
{
	return IsEof;
}


#pragma warning(pop)





