//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLData.cpp
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

#include "ZEMLData.h"

#include "ZEFile/ZEFile.h"
#include "ZEError.h"

void ZEMLData::LoadDeferred()
{
	if (!Deferred)
		return;

	DeferredFile->Seek(DeferredOffset, ZE_SF_BEGINING);

	Allocate(DataSize);
	if (DeferredFile->Read(Data, DataSize, 1) != 1)
		zeError("Can not read ZEMLDataProperty data from file. Corrupted ZEML file.");

	Deferred = false;
	DeferredFile = NULL;
	DeferredOffset = 0;
}

ZEMLElementType1 ZEMLData::GetType()
{
	return ZEML_ET_DATA;
}

ZESize ZEMLData::GetSize()
{
	ZESize Size = 1 +				// Identifier
		1 + GetName().GetLength() +	// Name
		8 +							// Size;
		DataSize;					// Data

	return Size;
}

ZEMLElement* ZEMLData::Clone()
{
	ZEMLData* CloneData = new ZEMLData(GetName());
	CloneData->SetUserData(GetUserData());
	CloneData->Deferred = this->Deferred;
	CloneData->DeferredFile = this->DeferredFile;
	CloneData->DeferredOffset = this->DeferredOffset;

	if (CloneData->Deferred)
	{
		CloneData->Data = NULL;
		CloneData->DataSize = this->DataSize;
		CloneData->Referred = Referred;
	}
	else
	{
		SetData(this->Data, this->DataSize, this->Referred);
	}

	return CloneData;
}

void ZEMLData::Allocate(ZESize NewDataSize)
{
	Deallocate();
	
	if (NewDataSize != 0)
		Data = new ZEBYTE[NewDataSize];

	DataSize = NewDataSize;
	Referred = false;
}

void ZEMLData::Deallocate()
{
	if (!Referred && Data != NULL)
	{
		delete Data;
	}

	Data = NULL;
	DataSize = 0;
	Referred = false;
	Deferred = false;
	DeferredFile = NULL;
	DeferredOffset = 0;
}

void ZEMLData::SetData(void* NewData, ZESize NewDataSize, bool NewReferred)
{
	Deallocate();

	if (NewData == NULL || NewDataSize == 0)
		return;

	if (NewReferred)
	{
		Data = (ZEBYTE*)NewData;
		DataSize = NewDataSize;
		Referred = true;
	}
	else
	{
		Allocate(NewDataSize);
		memcpy(Data, NewData, NewDataSize);
		Referred = false;
	}
}

const void* ZEMLData::GetData()
{
	LoadDeferred();
	return Data;
}

ZESize ZEMLData::GetDataSize()
{
	return DataSize;
}

ZEMLData::ZEMLData()
{
	Data = NULL;
	DataSize = NULL;
	Referred = false;
	Deferred = false;
	DeferredFile = NULL;
	DeferredOffset = 0;
}

ZEMLData::ZEMLData(const char* NewName)
{
	Data = NULL;
	DataSize = NULL;
	Referred = false;
	Deferred = false;
	DeferredFile = NULL;
	DeferredOffset = 0;
	SetName(NewName);
}

ZEMLData::ZEMLData(const char* Name, void* NewData, ZEUInt64 NewDataSize, bool NewStatic)
{
	Data = NULL;
	DataSize = NULL;
	Referred = NewStatic;
	Deferred = false;
	DeferredFile = NULL;
	DeferredOffset = 0;
	SetName(Name);
	SetData(NewData, NewDataSize, NewStatic);
}

ZEMLData::~ZEMLData()
{
	if (Referred && Data != NULL)
		delete Data;
}
