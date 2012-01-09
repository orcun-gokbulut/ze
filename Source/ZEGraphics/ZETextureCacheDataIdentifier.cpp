//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureCacheDataIdentifier.cpp
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
#include "ZEGraphics/ZETextureCacheDataIdentifier.h"


ZETextureCacheDataIdentifier::ZETextureCacheDataIdentifier( )
{
	/* Empty */
}

ZETextureCacheDataIdentifier::ZETextureCacheDataIdentifier( const char* ItemName, const ZETextureOptions &TextureOptions, ZEQWORD Offset )
{
	sprintf_s(this->ItemName, sizeof(char) * ZE_MAX_FILE_NAME_SIZE, "%s", ItemName);

	this->TextureOptions = TextureOptions;
	this->Offset = Offset;
}

ZETextureCacheDataIdentifier::~ZETextureCacheDataIdentifier()
{
	// Empty
}

ZEQWORD ZETextureCacheDataIdentifier::GetDataSize()const
{
	return 288;
}

ZEQWORD ZETextureCacheDataIdentifier::GetHash() const
{
	unsigned int Hash = 0;
	unsigned int I = 0;	
	int Char = 0;

	while (I < ZE_MAX_FILE_NAME_SIZE)
	{
		Char = ItemName[I];
		Hash = Char + (Hash << 6) + (Hash << 16) - Hash;
		I++;
	}

	return (ZEQWORD)Hash;
}

// Returns total bytes written
ZEQWORD ZETextureCacheDataIdentifier::Write(ZEFile* File) const
{
	size_t ZeQWordSize = sizeof(ZEQWORD);
	size_t ZeTexOptSize = sizeof(ZETextureOptions);
	size_t ZeCharSize = sizeof(char);

	ZEQWORD BytesWritten = 0;
	ZEQWORD WriteCount = 0;

	WriteCount = File->Write(ItemName, (ZEQWORD)ZeCharSize, ZE_MAX_FILE_NAME_SIZE);
	if (WriteCount != ZE_MAX_FILE_NAME_SIZE)
	{
		return 0;
	}
	else
	{
		BytesWritten += WriteCount * (ZEQWORD)ZeCharSize;
		WriteCount = 0;
	}
	
	WriteCount = File->Write(&TextureOptions, (ZEQWORD)ZeTexOptSize, 1);
	if (WriteCount != 1)
	{
		return 0;
	}
	else
	{
		BytesWritten += WriteCount * (ZEQWORD)ZeTexOptSize;
		WriteCount = 0;
	}
	
	WriteCount = File->Write(&Offset, (ZEQWORD)ZeQWordSize, 1);
	if (WriteCount != 1)
	{
		return 0;
	}
	else
	{
		BytesWritten += WriteCount * (ZEQWORD)ZeQWordSize;
		WriteCount = 0;
	}

	return BytesWritten ;
}

bool ZETextureCacheDataIdentifier::Equal(ZEFile* File) const
{
	size_t ZETexOptSize = sizeof(ZETextureOptions);

	char ItemNameBuffer[ZE_MAX_FILE_NAME_SIZE];
	if (File->Read(ItemNameBuffer, sizeof(char), ZE_MAX_FILE_NAME_SIZE) != ZE_MAX_FILE_NAME_SIZE)
	{
		zeAssert(true, "Cannot read item name from cache: \"%s\".", File->GetFilePath().GetValue());
		return false;
	}

	// Burda sorun olabilir
	unsigned int I = 0;
	unsigned int StringLenght = strlen(ItemNameBuffer);
	while (I < StringLenght)
	{
		if(ItemNameBuffer[I] != ItemName[I])
			return false;
		I++;
	}

	ZETextureOptions TextureOptionsRead;
	if (File->Read(&TextureOptionsRead, ZETexOptSize, 1) != 1)
	{
		zeAssert(true, "Cannot read texture options from cache: \"%s\".", File->GetFilePath().GetValue());
		return false;
	}

	if (memcmp(&TextureOptionsRead, &TextureOptions, ZETexOptSize) != 0)
		return false;

	ZEQWORD OffsetRead;
	if (File->Read(&OffsetRead, sizeof(ZEQWORD), 1) != 1)
	{
		zeAssert(true, "Cannot read offset from cache: \"%s\".", File->GetFilePath().GetValue());
		return false;
	}

	if(OffsetRead != Offset)
		return false;

	return true;
}
