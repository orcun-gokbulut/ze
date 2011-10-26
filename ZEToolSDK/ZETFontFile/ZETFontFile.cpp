//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETFontFile.cpp
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

#include "ZETFontFile.h"
#include <stdio.h>
#include <memory.h>

#define ZEFONTFILE_HEADER									((ZEDWORD)((ZEDWORD)'ZEFF' + (ZEDWORD)'FONT'))

struct ZEFontFileHeader
{
	ZEDWORD				Header;
	ZEDWORD				TextureCount;
	ZEFontCharacter		Characters[256];
};

bool ZEFont::ReadFromFile(const char* Filename)
{
	ZEFontFileHeader	FileHeader;
	FILE* File = fopen(Filename, "rb");
	if (File == NULL)
	{
		// Error Error
		return false;
	}

	fread(&FileHeader, sizeof(ZEFontFileHeader), 1, File);
	if (FileHeader.Header != ZEFONTFILE_HEADER)
	{
		// Error Error
		fclose(File);	
		return false;
	}

	memcpy(Characters, FileHeader.Characters, sizeof(ZEFontCharacter) * ZEFONTFILE_CHARACTERCOUNT);

	Textures.SetCount(FileHeader.TextureCount);
	for (size_t I = 0; I < FileHeader.TextureCount; I++)
	{
		Textures[I].ReadFromResourceFile(File);
	}

	return true;
}

bool ZEFont::WriteToFile(const char* Filename)
{
	ZEFontFileHeader	FileHeader;
	
	FILE* File = fopen(Filename, "wb");
	if (File == NULL)
	{
		// Error Error
		return false;
	}

	FileHeader.Header = ZEFONTFILE_HEADER;
	FileHeader.TextureCount = Textures.GetCount();
	memcpy(FileHeader.Characters, Characters, sizeof(ZEFontCharacter) * ZEFONTFILE_CHARACTERCOUNT);

	fwrite(&FileHeader, sizeof(ZEFontFileHeader), 1, File);


	for (size_t I = 0; I < FileHeader.TextureCount; I++)
		Textures[I].WriteToResourceFile(File);

	return true;
}

ZEFont::~ZEFont()
{

}
