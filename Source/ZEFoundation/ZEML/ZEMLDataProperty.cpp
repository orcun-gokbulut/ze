//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLDataProperty.cpp
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

#include "ZEMLDataProperty.h"
#include "ZEFile/ZEFile.h"
#include "ZEError.h"

ZEMLDataProperty::~ZEMLDataProperty()
{
	if(IsCached && Data != NULL)
		delete []Data;

	Data = NULL;
}

ZEMLDataProperty::ZEMLDataProperty()
{
	Data = NULL;
	IsCached = false;
	SetType(ZEML_IT_INLINE_DATA);
	FileDataPosition = 0;
	File = NULL;
}

ZEMLDataProperty::ZEMLDataProperty(const ZEString& Name)
{
	Data = NULL;
	IsCached = false;
	SetName(Name);
	SetType(ZEML_IT_INLINE_DATA);
	FileDataPosition = 0;
	File = NULL;
}

ZEMLDataProperty::ZEMLDataProperty(const ZEString& Name ,void* Data, ZEUInt64 DataSize, bool Cache)
{
	this->Data = NULL;
	IsCached = false;
	SetName(Name);
	SetData(Data, DataSize, Cache);
	SetType(ZEML_IT_INLINE_DATA);
	FileDataPosition = 0;
	File = NULL;
}

ZEUInt64 ZEMLDataProperty::GetTotalSize()
{
	return sizeof(char) + sizeof(ZEUInt8) + sizeof(ZEUInt8) + GetName().GetSize() + sizeof(ZEUInt64) + DataSize;
}

void ZEMLDataProperty::SetData(void* Data, ZEUInt64 DataSize, bool Cache)
{
	if(IsCached && this->Data != NULL)
	{
		delete[] this->Data;
		this->Data = NULL;
	}

	IsCached = Cache;

	if(IsCached)
	{
		this->Data = new char[DataSize];
		memcpy(this->Data, Data, DataSize);
	}
	else
		this->Data = Data;

	this->DataSize = DataSize;
}

const void* ZEMLDataProperty::GetData()
{
	if(File != NULL && Data == NULL && IsCached == false && FileDataPosition != 0)
	{
		IsCached = true;
		File->Seek(FileDataPosition, ZE_SF_BEGINING);
		Data = new char[DataSize];
		
		if(File->Read(Data, DataSize, 1) != 1)
			zeError("Can not read ZEMLDataProperty data from file. Corrupted ZEML file.");
	}

	return Data;
}

void ZEMLDataProperty::WriteToFile(ZEFile* File)
{
	char Identifier = 'Z';
	if(File->Write(&Identifier, sizeof(char), 1) != 1)
		zeError("Can not write ZEMLDataProperty identifier to file.");

	if(File->Write(&Type, sizeof(ZEUInt8), 1) != 1)
		zeError("Can not write ZEMLDataProperty type to file.");

	ZEUInt8 NameLength = Name.GetSize();
	if(File->Write(&NameLength, sizeof(ZEUInt8), 1) != 1)
		zeError("Can not write ZEMLDataProperty name lenght to file.");

	if(File->Write(Name.GetValue(), sizeof(char) * NameLength, 1) != 1)
		zeError("Can not write ZEMLDataProperty name to file.");

	if(File->Write(&DataSize, sizeof(ZEUInt64), 1) != 1)
		zeError("Can not write ZEMLDataProperty data size to file.");

	if(File->Write(GetData(), sizeof(char) * DataSize, 1) != 1)
		zeError("Can not write ZEMLDataProperty data to file.");
}

void ZEMLDataProperty::ReadFromFile(ZEFile* File, bool DeferredDataReading)
{
	IsCached = !DeferredDataReading;

	char		Identifier;	
	ZEUInt8		NameSize;
	char		TempNameBuffer[ZEML_MAX_NAME_SIZE];	

	if(File->Read(&Identifier, sizeof(char), 1) != 1)
		zeError("Can not read ZEMLDataProperty identifier from file. Corrupted ZEML file.");

	if(Identifier != ZEML_ITEM_FILE_IDENTIFIER)
		zeError("ZEMLProperty identifier mismatch. Corrupted ZEML file.");

	if(File->Read(&Type, sizeof(ZEUInt8), 1) != 1)
		zeError("Can not read ZEMLDataProperty type from file. Corrupted ZEML file.");

	if(File->Read(&NameSize, sizeof(ZEUInt8), 1) != 1)
		zeError("Can not read ZEMLDataProperty name length from file. Corrupted ZEML file.");

	if(File->Read(TempNameBuffer, NameSize, 1) != 1)
		zeError("Can not read ZEMLDataProperty name from file. Corrupted ZEML file.");

	if(File->Read(&DataSize, sizeof(ZEUInt64), 1) != 1)
		zeError("Can not read ZEMLDataProperty data size from file. Corrupted ZEML file.");

	SetName(TempNameBuffer);

	if(IsCached)
	{
		Data = new char[DataSize];

		if(File->Read(Data, DataSize, 1) != 1)
			zeError("Can not read ZEMLDataProperty data from file. Corrupted ZEML file.");
	}
	else
	{
		FileDataPosition = File->Tell();
		File->Seek(DataSize, ZE_SF_CURRENT);
	}

}
