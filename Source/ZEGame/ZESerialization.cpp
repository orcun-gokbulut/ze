//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESerialization.cpp
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
#include "ZECore\ZEError.h"
#include "ZESerialization.h"
#include <stdio.h>

size_t ZEFileSerializer::Write(const void* Buffer, size_t Size, size_t Count)
{
	return fwrite(Buffer, Size, Count, (FILE*)File);
}

bool ZEFileSerializer::OpenFile(const char* FileName)
{
	File = fopen(FileName, "wb");
	if (File == NULL)
	{
		zeError("File Serializer", "Can not open \"%s\" file.", FileName);
		return false;
	}
	return true;
}

void ZEFileSerializer::CloseFile()
{
	if (File != NULL)
	{
		fclose((FILE*)File);
		File = NULL;
	}
}

ZEFileSerializer::ZEFileSerializer()
{
	File = NULL;
}

ZEFileSerializer::~ZEFileSerializer()
{
	if (File != NULL)
		CloseFile();
}

size_t ZEFileUnserializer::Read(void* Buffer, size_t Size, size_t Count)
{
	return fread(Buffer, Size, Count, (FILE*)File);
}

bool ZEFileUnserializer::OpenFile(const char* FileName)
{
	File = fopen(FileName, "rb");
	if (File == NULL)
	{
		zeError("File Unserializer", "Can not open \"%s\" file.", FileName);
		return false;
	}
	return true;
}

void ZEFileUnserializer::CloseFile()
{
	if (File != NULL)
	{
		fclose((FILE*)File);
		File = NULL;
	}
}

ZEFileUnserializer::ZEFileUnserializer()
{
	File = NULL;
}

ZEFileUnserializer::~ZEFileUnserializer()
{
	if (File != NULL)
		CloseFile();
}




