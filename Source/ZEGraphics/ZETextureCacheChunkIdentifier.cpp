//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureCacheChunkIdentifier.cpp
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

#include "ZEGraphics/ZETextureCacheChunkIdentifier.h"



ZETextureCacheChunkIdentifier::ZETextureCacheChunkIdentifier( )
{
	/* Empty */
}

ZETextureCacheChunkIdentifier::ZETextureCacheChunkIdentifier( const char* ItemName, const ZETextureOptions &TextureOptions, unsigned int Offset )
{
	sprintf(this->ItemName, "%s", ItemName);

	this->TextureOptions = TextureOptions;
	this->Offset = Offset;
}

ZETextureCacheChunkIdentifier::~ZETextureCacheChunkIdentifier()
{
	// Empty
}

size_t ZETextureCacheChunkIdentifier::GetDataSize()const
{
	return 284;
}

ZEDWORD ZETextureCacheChunkIdentifier::GetHash() const
{
	unsigned int Hash = 0;
	unsigned int I = 0;	
	int Char = 0;

	while(I < ZE_MAX_FILE_NAME_SIZE)
	{
		Char = ItemName[I];
		Hash = Char + (Hash << 6) + (Hash << 16) - Hash;
		I++;
	}
	
	return (ZEDWORD)Hash;
}

// Returns total bytes written
size_t ZETextureCacheChunkIdentifier::Write(ZEFile* File) const
{
	size_t FileNameSize = sizeof(char) * ZE_MAX_FILE_NAME_SIZE;
	File->Write(ItemName, sizeof(char), ZE_MAX_FILE_NAME_SIZE);
	
	size_t TextureOptionsSize = sizeof(ZETextureOptions);
	File->Write(&TextureOptions, sizeof(ZETextureOptions), 1);

	size_t OffsetSize = sizeof(unsigned int);
	File->Write(&Offset, sizeof(unsigned int), 1);

	return FileNameSize + TextureOptionsSize + OffsetSize ;


	//////////////////// OLD CODE ////////////////////

	/*size_t FileNameSize = sizeof(char) * ZE_MAX_FILE_NAME_SIZE;
	fwrite(FileName, sizeof(char), ZE_MAX_FILE_NAME_SIZE, (FILE*)File);

	size_t TextureOptionsSize = sizeof(ZETextureOptions);
	fwrite(&TextureOptions, sizeof(ZETextureOptions), 1, (FILE*)File);

	size_t OffsetSize = sizeof(unsigned int);
	fwrite(&Offset, sizeof(unsigned int), 1, (FILE*)File);

	return FileNameSize + TextureOptionsSize + OffsetSize ;*/
}

bool ZETextureCacheChunkIdentifier::Equal(ZEFile* File) const
{
	
	char ItemNameBuffer[ZE_MAX_FILE_NAME_SIZE];
	File->Read(ItemNameBuffer, sizeof(char), ZE_MAX_FILE_NAME_SIZE);

	unsigned int I = 0;
	while(I < ZE_MAX_FILE_NAME_SIZE)
	{
		if(ItemNameBuffer[I] != ItemName[I])
			return false;
		I++;
	}

	ZETextureOptions TextureOptionsRead;
	File->Read(&TextureOptionsRead, sizeof(ZETextureOptions), 1);

	if(TextureOptionsRead.CompressionQuality != TextureOptions.CompressionQuality ||
		TextureOptionsRead.CompressionType != TextureOptions.CompressionType ||
		TextureOptionsRead.DownSample != TextureOptions.DownSample ||
		TextureOptionsRead.FileCaching != TextureOptions.FileCaching ||
		TextureOptionsRead.MaximumMipmapLevel != TextureOptions.MaximumMipmapLevel ||
		TextureOptionsRead.MipMapping != TextureOptions.MipMapping)
		return false;

	unsigned int OffsetRead;
	File->Read(&OffsetRead, sizeof(unsigned int), 1);
	if(OffsetRead != Offset)
		return false;

	return true;



	//////////////////// OLD CODE ////////////////////

	/*char FileNameBuffer[ZE_MAX_FILE_NAME_SIZE];
	fread(FileNameBuffer, sizeof(char), ZE_MAX_FILE_NAME_SIZE, (FILE*)File);

	unsigned int I = 0;
	while(I < ZE_MAX_FILE_NAME_SIZE)
	{
		if(FileNameBuffer[I] != FileName[I])
			return false;
		I++;
	}

	ZETextureOptions	TextureOptionsRead;
	fread((void*)&TextureOptionsRead, sizeof(ZETextureOptions), 1, (FILE*)File);

	if(TextureOptionsRead.CompressionQuality != TextureOptions.CompressionQuality ||
		TextureOptionsRead.CompressionType != TextureOptions.CompressionType ||
		TextureOptionsRead.DownSample != TextureOptions.DownSample ||
		TextureOptionsRead.FileCaching != TextureOptions.FileCaching ||
		TextureOptionsRead.MaximumMipmapLevel != TextureOptions.MaximumMipmapLevel ||
		TextureOptionsRead.MipMapping != TextureOptions.MipMapping)
		return false;

	unsigned int OffsetRead;
	fread(&OffsetRead, sizeof(unsigned int), 1, (FILE*)File);
	if(OffsetRead != Offset)
		return false;

	return true;*/
}
