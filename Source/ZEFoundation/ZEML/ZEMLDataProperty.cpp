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

ZEMLElementType1 ZEMLData::GetType()
{
	return ZEML_ET_DATA;
}

ZESize ZEMLData::GetSize()
{
	return DataSize;
}

void ZEMLData::SetData(void* Data, ZESize DataSize)
{
	this->Data = (ZEBYTE*)Data;
	
	if (Data == NULL)
		this->DataSize = 0;
	else
		this->DataSize = DataSize;

	Offset = 0;
	File = NULL;
}

const void* ZEMLData::GetData()
{
	if (Data == NULL && File != NULL && File->IsOpen())
	{
		File->Seek(Offset, ZE_SF_BEGINING);
		Data = new ZEBYTE[DataSize];
		if(File->Read(Data, DataSize, 1) != 1)
		{
			zeError("Can not read ZEMLDataProperty data from file. Corrupted ZEML file.");
			Data.Release();
		}
	}

	return Data;
}

ZEMLData::ZEMLData()
{
	File = NULL;
	DataSize = 0;
	Offset = 0;
}

ZEMLData::ZEMLData(const char* Name)
{
	DataSize = NULL;
	Offset = 0;
	File = NULL;
	SetName(Name);
}

ZEMLData::ZEMLData(const char* Name, void* Data, ZEUInt64 DataSize)
{
	DataSize = NULL;
	Offset = 0;
	File = NULL;
	SetName(Name);
	SetData(Data, DataSize);
}
