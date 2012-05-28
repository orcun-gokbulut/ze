//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPartialFile.cpp
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
#include "ZEPartialFile.h"

#include <Windows.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fstream>

#pragma warning(push)
#pragma warning(disable:4996 4267)


bool ZEPartialFile::Open(const ZEString& FilePath, ZEFileMode Mode, bool Binary)
{
	//zeAssert(IsOpen(), "Close the previous file first");

	//ZEString String;
	//ZEString PurePath;

	//ZEUInt64 EndCursor = 0;
	//ZEUInt64 StartCursor = 0;
	//
	//ZEUInt EndIndex = 0;
	//ZEUInt StartIndex = 0;
	//
	//ZEUInt PathSize = FilePath.GetSize() - 1;
	//for(ZESize I = 0; I < PathSize; I++)
	//{
	//	if(FilePath[I] == ':')
	//	{
	//		StartIndex = I + 1;
	//		for(ZESize J = I; J < PathSize; J++)
	//		{
	//			if(FilePath[J] == '-')
	//			{
	//				EndIndex = J - 1;
	//				String = FilePath.SubString(StartIndex, EndIndex);
	//				StartCursor = _strtoui64(String.GetValue(), NULL, 10);

	//				StartIndex = J + 1;
	//				for(ZESize K = J; K < PathSize; K++)
	//				{
	//					if(FilePath[K] == '\0')
	//					{
	//						EndIndex = K - 1;
	//						String = FilePath.SubString(StartIndex, EndIndex);
	//						EndCursor = _strtoui64(String.GetValue(), NULL, 10);

	//						PurePath = FilePath.Left(I);
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	//if(!EndCursor || !StartCursor || EndCursor == StartCursor)
	//{
	//	zeError("Cannot resolve path: \"%s\".", FilePath.GetValue());
	//	return false;
	//}

	//const char* StrMode = NULL;
	//const char*	AltStrMode = NULL;

	//switch(Mode)
	//{
	//	case ZE_FM_READ_ONLY:
	//	{
	//		if (Binary)		
	//			StrMode = "rb";
	//		else
	//			StrMode = "r";
	//		break;
	//	}

	//	case ZE_FM_WRITE_ONLY:
	//	{
	//		if (Binary)
	//		{
	//			StrMode = "r+b";
	//			AltStrMode = "w+b";
	//		}
	//		else
	//		{
	//			StrMode = "r+";
	//			AltStrMode = "w+";
	//		}
	//		break;
	//	}

	//	case ZE_FM_APPEND_ONLY:
	//	{
	//		if (Binary)
	//			StrMode = "ab";
	//		else
	//			StrMode = "a";
	//		break;
	//	}

	//	case ZE_FM_READ_APPEND:
	//	{
	//		if(Binary)
	//			StrMode = "a+b";
	//		else
	//			StrMode = "a+";
	//		break;
	//	}

	//	case ZE_FM_READ_WRITE:
	//	{
	//		if(Binary)
	//		{
	//			StrMode = "r+b";
	//			AltStrMode = "w+b";
	//		}
	//		else
	//		{
	//			StrMode = "r+";
	//			AltStrMode = "w+";
	//		}
	//		break;
	//	}

	//	default:
	//	{
	//		return false;
	//		break;	
	//	}
	//}

	//// Try to open file
	//File = fopen(PurePath.GetValue(), StrMode);

	//if(File == NULL) // If cant open
	//{
	//	if(AltStrMode) // If there is an alternative opening mode
	//	{
	//		// Try to open with alternative mode
	//		File = fopen(PurePath.GetValue(), AltStrMode);

	//		if(File != NULL) // If opened
	//		{
	//			this->FilePath		= PurePath;
	//			this->FileCursor	= StartPosition;
	//			this->StartPosition = StartCursor;
	//			this->EndPosition	= EndCursor;
	//			this->IsEof			= false;
	//			_fseeki64((FILE*)this->File, StartPosition, SEEK_SET);
	//			return true;
	//		}
	//		else
	//		{
	//			zeError("Could not open file \"%s\".", PurePath.GetValue());
	//			return false;
	//		}
	//	}
	//	else
	//	{
	//		zeError("Could not open file \"%s\".", PurePath.GetValue());
	//		return false;
	//	}
	//}
	//else
	//{
	//	this->FilePath		= PurePath;
	//	this->FileCursor	= StartPosition;
	//	this->StartPosition = StartCursor;
	//	this->EndPosition	= EndCursor;
	//	this->IsEof			= false;
	//	_fseeki64((FILE*)this->File, StartPosition, SEEK_SET);
	//	return true;
	//}

	return false;
}

bool ZEPartialFile::Open(ZEFile* ParentFile, ZEUInt64 Offset, ZEUInt64 Size)
{
	zeAssert(IsOpen(), "File is already open. File Name: \"%s\".", FilePath.GetValue());
	zeAssert(!ParentFile->IsOpen(), "Parent file is not open. File Name: \"%s\".", FilePath.GetValue());
	zeAssert(Size == 0, "Cannot open a PartialFile with 0 size. File Name: \"%s\".", ParentFile->GetFilePath().GetValue());

	this->StartPosition = ParentFile->GetStartPosition() + Offset;
	this->EndPosition	= StartPosition + Size;
		
	this->FileType			= ZE_FT_PARTIAL;
	this->File				= ParentFile->GetFileHandle();
	this->FilePath			= ParentFile->GetFilePath();
	this->ParentFile		= ParentFile;
	this->IsEof				= false;
	this->FileCursor		= StartPosition;
	this->ReferenceCount	= 0;

	ParentFile->IncreaseReferenceCount();
	
	_fseeki64((FILE*)this->File, StartPosition, SEEK_SET);
	return true;
}

void ZEPartialFile::Close()
{
	// Parent File will close itself if and only if its reference count is zero
	if (ParentFile != NULL)
		ParentFile->DecreaseReferenceCount();
	
	// Close the partial file
	ReferenceCount	= 0;
	StartPosition	= 0;
	EndPosition		= 0;
	ParentFile		= NULL;
	File			= NULL;
	IsEof			= false;

	FilePath.Clear();
}

ZEUInt64 ZEPartialFile::Read(void* Buffer, ZEUInt64 Size, ZEUInt64 Count)
{
	if(!IsOpen())
		return 0;

	// Goto this File's cursor position
	_fseeki64((FILE*)File, FileCursor, SEEK_SET);

	if(FileCursor < StartPosition || EndPosition < FileCursor)
		return 0;

	// Calculate the maximum possible read count
	ZEUInt64 ReadEndPos = _ftelli64((FILE*)File) + Size * Count;
	if (ReadEndPos > EndPosition)
	{
		Count -= (ReadEndPos - EndPosition) / Size + (((ReadEndPos - EndPosition) % Size) == 0 ? 0 : 1);
		IsEof = true;
	}

	// Read
	ZEUInt64 ReadCount = fread(Buffer, (ZESize)Size, (ZESize)Count, (FILE*)File);
	FileCursor += ReadCount * Size;

	return ReadCount;
}

ZEUInt64 ZEPartialFile::ReadFormated(const char* Format, ...)
{
	if(!IsOpen())
		return 0;

	// Goto this File's cursor position
	_fseeki64((FILE*)File, FileCursor, SEEK_SET);

	// Check if the cursor is in the file boundary
	if(FileCursor < StartPosition || EndPosition < FileCursor)
		return 0;

	zeAssert(true, "Not Implemented Yet.");
	// 	va_list ArgList;
	// 
	// 	va_start(ArgList, Format);
	// 	ZESize CharCount = _tinput_l(File, Format, 0, ArgList);
	// 	va_end(ArgList);

	return 0;
}

ZEUInt64 ZEPartialFile::Write(void* Buffer, ZEUInt64 Size, ZEUInt64 Count)
{
	if(!IsOpen())
		return 0;

	// Goto this File's cursor position
	_fseeki64((FILE*)File, FileCursor, SEEK_SET);

	// Check if the cursor is in the file boundary
	if(FileCursor < StartPosition || EndPosition < FileCursor)
		return 0;

	// Calculate the maximum possible write count
	ZEUInt64 WriteEndPos = _ftelli64((FILE*)File) + Size * Count;
	if (WriteEndPos > EndPosition)
	{
		Count -= (WriteEndPos - EndPosition) / Size + (((WriteEndPos - EndPosition) % Size) == 0 ? 0 : 1);
		IsEof = true;
	}

	ZEUInt64 WriteCount = fwrite(Buffer, (ZESize)Size, (ZESize)Count, (FILE*)File);
	FileCursor += WriteCount * Size;

	return WriteCount;
}

ZEUInt64 ZEPartialFile::WriteFormated(const char* Format, ...)
{
	if(!IsOpen())
		return 0;

	_fseeki64((FILE*)File, FileCursor, SEEK_SET);
	if(FileCursor < StartPosition || EndPosition < FileCursor)
		return 0;

	va_list ArgList;

	va_start(ArgList, Format);
	ZEUInt64 WriteSize = vfprintf((FILE*)File, Format, ArgList);
	FileCursor += WriteSize;
	va_end(ArgList);

	return WriteSize;
}

/* If the seek operation tends to go beyond end position or below start position
the file cursor remains as it is and the return will be false */
bool ZEPartialFile::Seek(ZEInt64 Offset, ZESeekFrom Origin)
{
	if(!IsOpen())
		return 0;

	switch(Origin)
	{
			case ZE_SF_BEGINING:
			{
				if(Offset < 0 || EndPosition < StartPosition + Offset)
					return false;

				if(_fseeki64((FILE*)File, StartPosition + Offset, SEEK_SET) != 0)
					return false;

				break;
			}

			case ZE_SF_CURRENT:
			{
				_fseeki64((FILE*)File, FileCursor, SEEK_SET);

				ZEUInt64 CurrentPosition = 0;
				CurrentPosition = _ftelli64((FILE*)File);

				if(CurrentPosition + Offset < StartPosition || EndPosition < CurrentPosition + Offset)
					return false;

				if(_fseeki64((FILE*)File, Offset, SEEK_CUR) != 0)
					return false;

				break;
			}

			case ZE_SF_END:
			{
				if(EndPosition + Offset < StartPosition || Offset > 0)
					return false;

				if(_fseeki64((FILE*)File, EndPosition + Offset, SEEK_SET) != 0)
					return false;

				break;
			}

			default:
				return false;
				break;
	}

	FileCursor = _ftelli64((FILE*)File);

	if(IsEof == true && FileCursor >= StartPosition && FileCursor < EndPosition)
		IsEof = false;

	return true;
}

ZEUInt64 ZEPartialFile::Tell()
{
	return FileCursor - StartPosition;
}

ZEUInt64 ZEPartialFile::GetStartPosition()
{
	return StartPosition;
}

ZEUInt64 ZEPartialFile::GetEndPosition()
{
	return EndPosition;
}

ZEUInt64 ZEPartialFile::GetFileSize() const
{
	return EndPosition - StartPosition;	
}

bool ZEPartialFile::Eof()
{
	return IsEof;
}

ZEUInt ZEPartialFile::IncreaseReferenceCount()
{
	ParentFile->IncreaseReferenceCount();
	return ++ReferenceCount;
}

ZEUInt ZEPartialFile::DecreaseReferenceCount()
{
	ParentFile->DecreaseReferenceCount();
	return --ReferenceCount;
}

ZEPartialFile::ZEPartialFile()
{
	StartPosition	= 0;
	EndPosition		= 0;
	File			= NULL;
	IsEof			= false;
}

ZEPartialFile::~ZEPartialFile()
{
	Close();
}

#pragma warning(pop)
