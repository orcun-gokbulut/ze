//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLReaderV0.cpp
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

#include "ZEMLReader.h"
#include "ZEEndian.h"
#include "ZEPointer\ZEPointer.h"

#define ZEML_ITEM_FILE_IDENTIFIER	'Z'
#define ZEML_MAX_NAME_SIZE			256

enum ZEMLElementTypeV0
{
	ZEML_ETV0_UNDEFINED		= 0,
	ZEML_ETV0_FLOAT			= 1,
	ZEML_ETV0_DOUBLE		= 2,
	ZEML_ETV0_INT8			= 3,
	ZEML_ETV0_INT16			= 4,
	ZEML_ETV0_INT32			= 5,
	ZEML_ETV0_INT64			= 6,
	ZEML_ETV0_UINT8			= 7,
	ZEML_ETV0_UINT16		= 8,
	ZEML_ETV0_UINT32		= 9,
	ZEML_ETV0_UINT64		= 10,
	ZEML_ETV0_BOOLEAN		= 11,
	ZEML_ETV0_STRING		= 12,
	ZEML_ETV0_QUATERNION	= 13,
	ZEML_ETV0_VECTOR2		= 14,
	ZEML_ETV0_VECTOR3		= 15,
	ZEML_ETV0_VECTOR4		= 16,
	ZEML_ETV0_MATRIX3X3		= 17,
	ZEML_ETV0_MATRIX4X4		= 18,
	ZEML_ETV0_INLINE_DATA	= 19,
	ZEML_ETV0_OFFSET_DATA	= 20,
	ZEML_ETV0_NODE			= 21
};

static bool LoadV0Property(ZEFile* File, ZEMLElementTypeV0 Type, ZEValue& Value)
{
	ZEUInt64 ValueSize;
	if (File->Read(&ValueSize, sizeof(ZEUInt64), 1) != 1)
		return false;
	ValueSize = ZEEndian::Little(ValueSize);

	switch (Type)
	{
		case ZEML_ETV0_INT8:
			{
				ZEInt8 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetInt8(Temp);
			}
			break;

		case ZEML_ETV0_INT16:
			{
				ZEInt16 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetInt16(ZEEndian::Little(Temp));
			}
			break;

		case ZEML_ETV0_INT32:
			{
				ZEInt32 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetInt32(ZEEndian::Little(Temp));
			}
			break;

		case ZEML_ETV0_INT64:
			{
				ZEInt64 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetInt64(ZEEndian::Little(Temp));
			}
			break;

		case ZEML_ETV0_UINT8:
			{
				ZEUInt8 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetUInt8(Temp);
			}
			break;

		case ZEML_ETV0_UINT16:
			{
				ZEUInt16 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetUInt16(ZEEndian::Little(Temp));
			}
			break;

		case ZEML_ETV0_UINT32:
			{
				ZEUInt32 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetUInt32(ZEEndian::Little(Temp));
			}
			break;

		case ZEML_ETV0_UINT64:
			{
				ZEUInt64 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetUInt64(ZEEndian::Little(Temp));
			}
			break;

		case ZEML_ETV0_BOOLEAN:
			{
				bool Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetBoolean(Temp);
			}
			break;

		case ZEML_ETV0_FLOAT:
			{
				float Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetFloat(Temp);
			}
			break;

		case ZEML_ETV0_DOUBLE:
			{
				double Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetDouble(Temp);
			}
			break;

		case ZEML_ETV0_STRING:
			{
				if (ValueSize == 0)
				{
					Value.SetString("");
				}
				else
				{
					ZEPointer<char> Temp = new char[ValueSize];
					if (File->Read(Temp, ValueSize, 1) != 1)
						return false;
					Value.SetString(Temp);
				}
			}
			break;

		case ZEML_ETV0_QUATERNION:
			{
				ZEQuaternion Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetQuaternion(Temp);
			}
			break;

		case ZEML_ETV0_VECTOR2:
			{
				ZEVector2 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetVector2(Temp);
			}
			break;

		case ZEML_ETV0_VECTOR3:
			{
				ZEVector3 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetVector3(Temp);
			}
			break;

		case ZEML_ETV0_VECTOR4:
			{
				ZEVector4 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetVector4(Temp);
			}
			break;

		case ZEML_ETV0_MATRIX3X3:
			{
				ZEMatrix3x3 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetMatrix3x3(Temp);
			}
			break;

		case ZEML_ETV0_MATRIX4X4:
			{
				ZEMatrix4x4 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetMatrix4x4(Temp);
			}
			break;

		default:
			return false;
	}

	return true;
}

static bool LoadElementHeaderV0(ZEFile* File, ZEString& ElementName, ZEMLElementTypeV0& ElementType)
{
	char Identifier;	
	if (File->Read(&Identifier, sizeof(char), 1) != 1)
		return false;

	if (Identifier != ZEML_ITEM_FILE_IDENTIFIER)
		return false;

	ZEUInt8 ElementTypeData = 0;
	if (File->Read(&ElementTypeData, sizeof(ZEUInt8), 1) != 1)
		return false;
	ElementType = (ZEMLElementTypeV0)ElementTypeData;

	ZEUInt8 NameSize = 0;
	if (File->Read(&NameSize, sizeof(ZEUInt8), 1) != 1)
		return false;

	char NameBuffer[ZEML_MAX_NAME_SIZE];
	if (File->Read(NameBuffer, NameSize, 1) != 1)
		return false;
	NameBuffer[NameSize] = '\0';
	ElementName = NameBuffer;

	return true;
}

bool ZEMLReaderNode::LoadV0()
{
	if (VersionMajor == 0)
	{
		File->Seek(this->Offset, ZE_SF_BEGINING);

		ZEMLElementTypeV0 ElementType;
		if (!LoadElementHeaderV0(File, Name, ElementType) && ElementType != ZEML_ETV0_NODE)
		{
			zeError("Corrupted ZEML file. Cannot read element. File Name: \"%s\".", File->GetPath().ToCString());
			return false;
		}

		ZEUInt64 ElementCount;

		if (File->Read(&ElementCount, sizeof(ZEUInt64), 1) != 1)
		{
			zeError("Corrupted ZEML file. Cannot read element. File Name: \"%s\".", File->GetPath().ToCString());
			return false;
		}

		ElementCount = ZEEndian::Little(ElementCount);

		File->Read(&Size, sizeof(ZEUInt64), 1);
		Size = ZEEndian::Little(Size);

		ZEString SubElementName;
		ZEMLElementTypeV0 SubElementType;
		for (ZESize I = 0; I < ElementCount; I++)
		{
			ZEUInt64 SubElementOffset = File->Tell();
			if (!LoadElementHeaderV0(File, SubElementName, SubElementType))
			{
				zeError("Corrupted ZEML file. Cannot read element. File Name: \"%s\".", File->GetPath().ToCString());
				return false;
			}

			if (SubElementType == ZEML_ETV0_NODE)
			{
				ZEMLReaderSubNode* Node = SubNodes.Add();
				Node->Name = SubElementName;
				Node->Offset = SubElementOffset;
				File->Seek(sizeof(ZEUInt64), ZE_SF_CURRENT);

				if (File->Read(&Node->Size, sizeof(ZEUInt64), 1) != 1)
				{
					zeError("Corrupted ZEML file. Cannot read element. File Name: \"%s\".", File->GetPath().ToCString());
					return false;
				}
				Node->Size = ZEEndian::Little(Node->Size);

				Node->Index = 0;
				for (ZESSize I = SubNodes.GetCount() - 1; I >= 0; I--)
				{
					if (&SubNodes[I] != Node && SubNodes[I].Name == Node->Name)
						Node->Index = SubNodes[I].Index + 1;
				}
				File->Seek(Node->Size, ZE_SF_CURRENT);
			}
			else if (SubElementType == ZEML_ETV0_INLINE_DATA)
			{
				ZEMLReaderProperty* Property = Properties.Add();
				Property->Name = SubElementName;
				Property->ElementType = ZEML_ET_DATA;
				if (File->Read(&Property->DataSize, sizeof(ZEUInt64), 1) != 1)
				{
					zeError("Corrupted ZEML file. Cannot read element. File Name: \"%s\".", File->GetPath().ToCString());
					return false;
				}
				Property->DataSize = ZEEndian::Little(Property->DataSize);
				Property->DataOffset = File->Tell();
				File->Seek(Property->DataSize, ZE_SF_CURRENT);
			}
			else if (SubElementType == ZEML_ETV0_OFFSET_DATA)
			{
				ZEMLReaderProperty* Property = Properties.Add();
				Property->Name = SubElementName;
				Property->ElementType = ZEML_ET_DATA;
				if (File->Read(&Property->DataSize, sizeof(ZEUInt64), 1) != 1)
				{
					zeError("Corrupted ZEML file. Cannot read element. File Name: \"%s\".", File->GetPath().ToCString());
					return false;
				}
				Property->DataSize = ZEEndian::Little(Property->DataSize);
				if (File->Read(&Property->DataOffset, sizeof(ZEUInt64), 1) != 1)
				{
					zeError("Corrupted ZEML file. Cannot read element. File Name: \"%s\".", File->GetPath().ToCString());
					return false;
				}
				Property->DataOffset = ZEEndian::Little(Property->DataOffset);
			}
			else
			{
				ZEMLReaderProperty* Property = Properties.Add();
				Property->Name = SubElementName;
				Property->ElementType = ZEML_ET_PROPERTY;
				Property->ValueType = (ZEMLValueType)SubElementType;
				if (!LoadV0Property(File, SubElementType, Property->Value))
				{
					zeError("Corrupted ZEML file. Cannot read element. File Name: \"%s\".", File->GetPath().ToCString());
					return false;
				}
			}
		}
	}

	zeDebugCheck(!_CrtCheckMemory(), "Heap problem");	

	return true;
}
