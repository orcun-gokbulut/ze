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

#define WIN32_LEAN_AND_MEAN

#include "ZEError.h"
#include "ZEPartialFile.h"
#include "ZEFile.h"


ZEPartialFile::ZEPartialFile()
{
	EoF = false;
	EndPosition = 0;
	StartPosition = 0;
}

ZEPartialFile::~ZEPartialFile()
{
	Close();
}

ZEInt ZEPartialFile::Close()
{
	ZEInt Result;
	
	Result = ZEFile::Close();
	if (Result != 0)
		return Result;
	
	EoF = false;
	EndPosition = 0;
	StartPosition = 0;

	return Result;
}

bool ZEPartialFile::Open(const ZEFile* ParentFile, const ZEInt64 Offset, const ZEInt64 Size)
{
	zeDebugCheck(Size < 0, "Negative size");
	zeDebugCheck(Offset < 0, "Negative offset");
	zeDebugCheck(!ParentFile->IsOpen() < 0, "Parent file is not open.");

	if (!ZEFile::Open(ParentFile->GetPath(), ParentFile->GetOpenMode(),  ZE_FCM_NONE))	
		return false;

	if (ZEFile::Seek(Offset, ZE_SF_BEGINING) != 0)
	{
		Close();
		return false;
	}
	
	StartPosition = Offset;
	EndPosition = Offset + Size;

	return true;
}


bool ZEPartialFile::Open(const ZEString& FilePath, const ZEFileOpenMode OpenType, const ZEFileCreationMode CreationType)
{
	if(!ZEFile::Open(FilePath, OpenType, CreationType))
		return false;

	EndPosition = 0;
	StartPosition = 0;
	
	return true;
}

ZEInt64 ZEPartialFile::Tell() const
{	
	return ZEFile::Tell() - StartPosition;
}

ZEInt ZEPartialFile::Seek(const ZEInt64 Offset, const ZESeekFrom Origin)
{	
	ZEInt64 SeekOffset = Offset;
	ZESeekFrom SeekOrigin = Origin;

	// If partial file have a limit
	if (EndPosition != 0)
	{
		ZEInt64 NewPosition;
		switch (Origin)
		{
			case ZE_SF_BEGINING:
				NewPosition = StartPosition + Offset;
				break;
			case ZE_SF_CURRENT:
				NewPosition = ZEFile::Tell() + Offset;
				break;
			case ZE_SF_END:
				NewPosition = EndPosition + Offset;	
				break;
			default:
				zeError("Wrong seek origin...");
				break;
		}

		if (NewPosition < StartPosition || EndPosition < NewPosition)
			return -1;

		SeekOrigin = ZE_SF_BEGINING;
		SeekOffset = NewPosition;
	}

	EoF = false;
	return ZEFile::Seek(SeekOffset, SeekOrigin);
}

ZESize ZEPartialFile::Read(void* Buffer, const ZESize Size, const ZESize Count)
{
	ZESize ReadCount = Count;

	// If partial file have a limit
	if (EndPosition != 0)
	{
		ZEInt64 Current = ZEFile::Tell();
		ZEInt64 ReadEndPos = Current + Size * Count;

		// Truncate the count so that we will not over read
		if (ReadEndPos > EndPosition)
		{
			ReadCount = (EndPosition - Current) / Size;
			EoF = true;
		}
	}

	return ZEFile::Read(Buffer, Size, ReadCount);
}

ZESize ZEPartialFile::Write(const void* Buffer, const ZESize Size, const ZESize Count)
{
	ZESize WriteCount = Count;
	
	// If partial file have a limit
	if (EndPosition != 0)
	{
		ZEInt64 Current = ZEFile::Tell();
		ZEInt64 WriteEndPos = Current + Size * Count;
		
		// Truncate the count so that we will not over write
		if (WriteEndPos > EndPosition)
		{
			WriteCount = (EndPosition - Current) / Size;
			EoF = true;
		}
	}

	return ZEFile::Write(Buffer, Size, WriteCount);
}

void ZEPartialFile::SetEndPosition(const ZEInt64 Position)
{
	zeDebugCheck(Position < 0, "Position is negative");
	zeDebugCheck(Position < StartPosition, "End is lower than start");

	EoF = false;
	EndPosition = Position;
}

ZEInt64 ZEPartialFile::GetEndPosition() const
{
	return EndPosition;
}

void ZEPartialFile::SetStartPosition(const ZEInt64 Position)
{
	zeDebugCheck(Position < 0, "Position is negative");
	zeDebugCheck(Position > EndPosition, "Start is bigger than end");
	
	StartPosition = Position;
}

ZEInt64 ZEPartialFile::GetStartPosition() const
{
	return StartPosition;
}

ZEInt64 ZEPartialFile::GetSize()
{
	if (EndPosition == 0)
		return ZEFile::GetSize();

	return EndPosition - StartPosition;
}
