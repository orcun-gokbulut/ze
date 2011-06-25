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

ZETextureCacheChunkIdentifier::ZETextureCacheChunkIdentifier( char* FileName, const ZETextureOptions &TextureOptions, const ZETextureLoaderInfo &TextureInfo, unsigned int Offset )
{
	sprintf(this->FileName, "\\resources\\%s", FileName);

	this->TextureOptions = TextureOptions;
	this->TextureInfo = TextureInfo;
	this->Offset = Offset;
}

ZETextureCacheChunkIdentifier::~ZETextureCacheChunkIdentifier( )
{
	// Empty
}

ZEDWORD ZETextureCacheChunkIdentifier::GetHash()
{
	unsigned int Hash = 0;
	unsigned int I = 0;	
	int Char = 0;

	while(I < ZE_MAX_FILE_NAME_SIZE)
	{
		Char = FileName[I];
		Hash = Char + (Hash << 6) + (Hash << 16) - Hash;
		I++;
	}
	
	return (ZEDWORD)Hash;
}

// Returns total bytes written
size_t ZETextureCacheChunkIdentifier::Write(void* File) const
{
	size_t FileNameSize = sizeof(char) * ZE_MAX_FILE_NAME_SIZE;
	fwrite(FileName, sizeof(char), ZE_MAX_FILE_NAME_SIZE, (FILE*)File);

	size_t TextureInfoSize = sizeof(ZETextureLoaderInfo) * 1;
	fwrite(&TextureInfo, sizeof(ZETextureLoaderInfo), 1, (FILE*)File);

	size_t TextureOptionsSize = sizeof(ZETextureOptions) * 1;
	fwrite(&TextureOptions, sizeof(ZETextureOptions), 1, (FILE*)File);

	size_t OffsetSize = sizeof(unsigned int) * 1;
	fwrite(&Offset, sizeof(unsigned int), 1, (FILE*)File);

	return FileNameSize + TextureInfoSize + TextureOptionsSize + OffsetSize ;
}

bool ZETextureCacheChunkIdentifier::Equal(void* File) const
{
	char FileNameBuffer[ZE_MAX_FILE_NAME_SIZE];
	fread(FileNameBuffer, sizeof(char), ZE_MAX_FILE_NAME_SIZE, (FILE*)File);

	unsigned int I = 0;
	while(I < ZE_MAX_FILE_NAME_SIZE)
	{
		if(FileNameBuffer[I] != FileName[I])
			return false;
		I++;
	}

	ZETextureLoaderInfo TextureInfoRead;
	fread(&TextureInfoRead, sizeof(ZETextureLoaderInfo), 1, (FILE*)File);
	if(TextureInfoRead != TextureInfo)
		return false;

	ZETextureOptions	TextureOptionsRead;
	fread(&TextureOptionsRead, sizeof(ZETextureOptions), 1, (FILE*)File);
	if(TextureOptionsRead != TextureOptions)
		return false;

	unsigned int OffsetRead;
	fread(&OffsetRead, sizeof(unsigned int), 1, (FILE*)File);
	if(OffsetRead != Offset)
		return false;

	return true;
}
