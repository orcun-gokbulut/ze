//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETResourceFilePart.cpp
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

#include "ZETOutput.h"
#include "ZETResourceFilePart.h"
#include <memory.h>

ZESize ZEResourceInternalFile::GetFileSize() const
{
	return Size;
}

void* ZEResourceInternalFile::GetFileBuffer() const
{
	return Data;
}

void ZEResourceInternalFile::AllocateBuffer(ZESize Size)
{
	if (Data != NULL)
		delete[] Data;

	Data = new unsigned char[Size];
	this->Size = Size;
}
bool ZEResourceInternalFile::WriteToResourceFile(FILE* File) const
{
	fwrite(&Size, sizeof(ZEUInt32), 1, File);
	fwrite(Data, Size, 1, File);
	return true;
}

bool ZEResourceInternalFile::WriteToExternalFile(const char* Filename) const
{
	FILE* File = fopen(Filename, "wb");
	if (File == NULL)
	{
		// ERROR ERROR
		return false;
	}
	fwrite(Data, Size, 1, File);
	fclose(File);
	return true;
}

bool ZEResourceInternalFile::ReadFromResourceFile(FILE* File)
{
	fread(&Size, sizeof(ZEUInt32), 1, File);
	AllocateBuffer(Size);
	fread(Data, Size, 1, File);
	return true;
}

bool ZEResourceInternalFile::ReadFromExternalFile(const char* Filename)
{
	FILE* File = fopen(Filename, "rb");
	if (File == NULL)
	{
		zesdkError("Resource Internal File", "Can not open file. (Filename: \"%s\")", Filename);
		return false;
	}

	fseek(File, 0, SEEK_END);
	Size = ftell(File);
	AllocateBuffer(Size);
	fseek(File, 0, SEEK_SET);
	fread(Data, Size, 1, File);
	fclose(File);
	return true;
}

void ZEResourceInternalFile::CopyTo(ZEResourceInternalFile& Destination) const
{
	Destination.AllocateBuffer(Size);
	memcpy(Destination.Data, Data, Size);
}

ZEResourceInternalFile& ZEResourceInternalFile::operator=(const ZEResourceInternalFile& Source)
{
	Source.CopyTo(*this);
	return *this;
}

ZEResourceInternalFile::ZEResourceInternalFile(const ZEResourceInternalFile& Source)
{
	Source.CopyTo(*this);
}

ZEResourceInternalFile::ZEResourceInternalFile()
{
	Data = NULL;
	Size = 0;
}

ZEResourceInternalFile::~ZEResourceInternalFile()
{
	if (Data != NULL)
		delete[] Data;
}
