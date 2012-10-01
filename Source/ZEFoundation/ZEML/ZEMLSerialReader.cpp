//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLSerialReader.cpp
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

#include "ZEMLSerialReader.h"
#include "ZEFile\ZEFile.h"
#include "ZEEndian.h"

ZEMLSerialReader::ZEMLSerialReader(ZEFile* File)
{
	this->File = File;
	NextItemPosition = 0;
}

bool ZEMLSerialReader::ReadNextItem()
{
	ZEUInt64 TempUInt64;

	CurrentItemType			= ZEML_IT_UNDEFINED;
	CurrentItemValue		= ZEVariant();
	CurrentItemName			= "";
	CurrentItemDataSize		= 0;
	CurrentItemSubItemCount = 0;

	File->Seek(NextItemPosition, ZE_SF_BEGINING);

	char Identifier;	
	if(File->Read(&Identifier, sizeof(char), 1) != 1)
	{
		if(!File->Eof())
			zeError("Can not read ZEML file. Corrupted ZEML file.");
		else
			return false;
	}

	if(Identifier != ZEML_ITEM_FILE_IDENTIFIER)
		zeError("Corrupted ZEML file. Corrupted ZEML file.");

	if(File->Read(&CurrentItemType, sizeof(ZEUInt8), 1) != 1)
		zeError("Can not read ZEMLItem type from file. Corrupted ZEML file.");

	ZEUInt8 NameSize = 0;
	if(File->Read(&NameSize, sizeof(ZEUInt8), 1) != 1)
		zeError("Can not read ZEMLItem name size from file. Corrupted ZEML file.");

	char TempNameBuffer[ZEML_MAX_NAME_SIZE];
	if(File->Read(TempNameBuffer, NameSize, 1) != 1)
		zeError("Can not read ZEMLItem name from file. Corrupted ZEML file.");

	CurrentItemName = TempNameBuffer;

	if(CurrentItemType == ZEML_IT_NODE)
	{
		if(File->Read(&CurrentItemSubItemCount, sizeof(ZEUInt64), 1) != 1)
			zeError("Can not read ZEMLNode subitem count from file. Corrupted ZEML file.");

		CurrentItemSubItemCount = ZEEndian::Little(CurrentItemSubItemCount);

		if(File->Read(&CurrentItemDataSize, sizeof(ZEUInt64), 1) != 1)
			zeError("Can not read ZEMLNode size from file. Corrupted ZEML file.");

		NextItemPosition = File->Tell();
	}
	else if(CurrentItemType == ZEML_IT_INLINE_DATA)
	{
		if(File->Read(&CurrentItemDataSize, sizeof(ZEUInt64), 1) != 1)
			zeError("Can not read ZEMLDataProperty data size from file. Corrupted ZEML file.");

		CurrentItemDataSize = ZEEndian::Little(CurrentItemDataSize);

		NextItemPosition = File->Tell() + CurrentItemDataSize;
	}
	else
	{
		ZEUInt64 IsDataRead = 0;
		ZEUInt64 ValueSize = 0;

		if(File->Read(&ValueSize, sizeof(ZEUInt64), 1) != 1)
			zeError("Can not read ZEMLProperty value size from file. Corrupted ZEML file.");

		ValueSize = ZEEndian::Little(ValueSize);

		ZEInt64 TempInt64;
		ZEInt8	TempInt32;
		ZEInt8	TempInt16;
		ZEInt8	TempInt8;
		ZEUInt64 TempUInt64;
		ZEUInt8 TempUInt32;
		ZEUInt8 TempUInt16;
		ZEUInt8 TempUInt8;

		switch(CurrentItemType)
		{
			case ZEML_IT_FLOAT:
				CurrentItemValue.SetType(ZE_VRT_FLOAT);
				IsDataRead = File->Read(&CurrentItemValue.ImplicitAcesss().Float, ValueSize, 1);
				break;
			case ZEML_IT_DOUBLE:
				CurrentItemValue.SetType(ZE_VRT_DOUBLE);
				IsDataRead = File->Read(&CurrentItemValue.ImplicitAcesss().Double, ValueSize, 1);
				break;
			case ZEML_IT_INT8:
				CurrentItemValue.SetType(ZE_VRT_INTEGER_8);
				IsDataRead = File->Read(&CurrentItemValue.ImplicitAcesss().Int32, ValueSize, 1);
				break;
			case ZEML_IT_INT16:
				CurrentItemValue.SetType(ZE_VRT_INTEGER_16);
				IsDataRead = File->Read(&CurrentItemValue.ImplicitAcesss().Int32, ValueSize, 1);
				CurrentItemValue.SetInt16(ZEEndian::Little(CurrentItemValue.GetInt16()));
				break;
			case ZEML_IT_INT32:
				CurrentItemValue.SetType(ZE_VRT_INTEGER_32);
				IsDataRead = File->Read(&CurrentItemValue.ImplicitAcesss().Int32, ValueSize, 1);
				CurrentItemValue.SetInt32(ZEEndian::Little(CurrentItemValue.GetInt32()));
				break;
			case ZEML_IT_INT64:
				CurrentItemValue.SetType(ZE_VRT_INTEGER_64);
				IsDataRead = File->Read(&CurrentItemValue.ImplicitAcesss().Int64, ValueSize, 1);
				CurrentItemValue.SetInt64(ZEEndian::Little(CurrentItemValue.GetInt64()));
				break;
			case ZEML_IT_UINT8:
				CurrentItemValue.SetType(ZE_VRT_UNSIGNED_INTEGER_8);
				IsDataRead = File->Read(&CurrentItemValue.ImplicitAcesss().Int32, ValueSize, 1);
				break;
			case ZEML_IT_UINT16:
				CurrentItemValue.SetType(ZE_VRT_UNSIGNED_INTEGER_16);
				IsDataRead = File->Read(&CurrentItemValue.ImplicitAcesss().Int32, ValueSize, 1);
				CurrentItemValue.SetUInt16(ZEEndian::Little(CurrentItemValue.GetUInt16()));
				break;
			case ZEML_IT_UINT32:
				CurrentItemValue.SetType(ZE_VRT_UNSIGNED_INTEGER_32);
				IsDataRead = File->Read(&CurrentItemValue.ImplicitAcesss().Int32, ValueSize, 1);
				CurrentItemValue.SetUInt32(ZEEndian::Little(CurrentItemValue.GetUInt32()));
				break;
			case ZEML_IT_UINT64:
				CurrentItemValue.SetType(ZE_VRT_UNSIGNED_INTEGER_64);
				IsDataRead = File->Read(&CurrentItemValue.ImplicitAcesss().Int64, ValueSize, 1);
				CurrentItemValue.SetUInt64(ZEEndian::Little(CurrentItemValue.GetUInt64()));
				break;
			case ZEML_IT_BOOLEAN:
				CurrentItemValue.SetType(ZE_VRT_BOOLEAN);
				IsDataRead = File->Read(&CurrentItemValue.ImplicitAcesss().Boolean, ValueSize, 1);
				break;
			case ZEML_IT_STRING:
				{
					CurrentItemValue.SetType(ZE_VRT_STRING);

					if (ValueSize == 0)
					{
						IsDataRead = 1;
						CurrentItemValue.SetString(ZEString());
					}
					else
					{
						char* TempBuffer = new char[ValueSize];
						IsDataRead = File->Read(TempBuffer, ValueSize, 1);
						CurrentItemValue.SetString(TempBuffer);
						delete TempBuffer;
					}
				}
				break;
			case ZEML_IT_QUATERNION:
				CurrentItemValue.SetType(ZE_VRT_QUATERNION);
				IsDataRead = File->Read(&CurrentItemValue.ImplicitAcesss().Vectors, ValueSize, 1);
				break;
			case ZEML_IT_VECTOR2:
				CurrentItemValue.SetType(ZE_VRT_VECTOR2);
				IsDataRead = File->Read(&CurrentItemValue.ImplicitAcesss().Vectors, ValueSize, 1);
				break;
			case ZEML_IT_VECTOR3:
				CurrentItemValue.SetType(ZE_VRT_VECTOR3);
				IsDataRead = File->Read(&CurrentItemValue.ImplicitAcesss().Vectors, ValueSize, 1);
				break;
			case ZEML_IT_VECTOR4:
				CurrentItemValue.SetType(ZE_VRT_VECTOR4);
				IsDataRead = File->Read(&CurrentItemValue.ImplicitAcesss().Vectors, ValueSize, 1);
				break;
			case ZEML_IT_MATRIX3X3:
				CurrentItemValue.SetType(ZE_VRT_MATRIX3X3);
				IsDataRead = File->Read(CurrentItemValue.ImplicitAcesss().Matrix3x3, ValueSize, 1);
				break;
			case ZEML_IT_MATRIX4X4:
				CurrentItemValue.SetType(ZE_VRT_MATRIX4X4);
				IsDataRead = File->Read(CurrentItemValue.ImplicitAcesss().Matrix4x4, ValueSize, 1);
				break;
			default:
				zeError("Unsupported ZEMLProperty type.");
				break;
		}

		if(IsDataRead != 1)
			zeError("Can not read ZEMLProperty value from file. Corrupted ZEML file.");

		NextItemPosition = File->Tell();
	}
}

bool ZEMLSerialReader::ReadSiblingItem()
{
	if(CurrentItemType == ZEML_IT_NODE)
		File->Seek(CurrentItemDataSize, ZE_SF_CURRENT);

	return ReadNextItem();
}

ZEMLItemType ZEMLSerialReader::GetItemType()
{
	return CurrentItemType;
}

ZEString ZEMLSerialReader::GetItemName()
{
	return CurrentItemName;
}

ZEUInt64 ZEMLSerialReader::GetSubItemCount()
{
	return CurrentItemSubItemCount;
}

ZEVariant ZEMLSerialReader::GetItemValue()
{
	return CurrentItemValue;
}

ZEUInt64 ZEMLSerialReader::GetDataSize()
{
	return CurrentItemDataSize;
}

bool ZEMLSerialReader::GetData(void* Buffer, ZEUInt64 BufferSize, ZEUInt64 Offset)
{
	if(CurrentItemType != ZEML_IT_INLINE_DATA)
		return false;

	if(File->Seek(Offset, ZE_SF_CURRENT))
	{
		zeError("Can not seek ZEML file.");
		return false;
	}
	
	if(File->Read(Buffer, BufferSize, 1) != 1)
	{
		zeError("Can not read ZEMLDataProperty data with offset : %d", Offset);
		return false;
	}

	return true;
}
