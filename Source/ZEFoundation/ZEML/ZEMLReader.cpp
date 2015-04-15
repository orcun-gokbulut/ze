//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLReader.cpp
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

ZEMLReaderProperty::ZEMLReaderProperty()
{
	Type = ZEML_ET_UNDEFINED;
	DataSize = 0;
	DataOffset = 0;
}

ZEMLReaderSubNode::ZEMLReaderSubNode()
{
	Offset = 0;
	Size = 0;
}

const ZEMLReaderProperty* ZEMLReaderNode::FindProperty(const char* Name)
{
	for (ZESize I = 0; I < Properties.GetCount(); I++)
		if (Properties[I].Name == Name)
			return &Properties[I];

	return NULL;
}

static bool LoadElementHeader(ZEFile* File, ZEString& ElementName, ZEMLElementType& ElementType)
{
	char Identifier;	
	if (File->Read(&Identifier, sizeof(char), 1) != 1)
		return false;

	if (Identifier != ZEML_ITEM_FILE_IDENTIFIER)
		return false;
	
	ZEUInt8 ElementTypeData = 0;
	if (File->Read(&ElementTypeData, sizeof(ZEUInt8), 1) != 1)
		return false;
	ElementType = (ZEMLElementType)ElementTypeData;

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

static bool LoadProperty(ZEFile* File, ZEMLElementType Type, ZEValue& Value)
{
	switch (Type)
	{
		case ZEML_ET_INT8:
		{
			ZEInt8 Temp;
			if (File->Read(&Temp, sizeof(ZEInt8), 1) != 1)
				return false;
			Value.SetInt8(Temp);
		}
		break;

		case ZEML_ET_INT16:
		{
			ZEInt16 Temp;
			if (File->Read(&Temp, sizeof(ZEInt16), 1) != 1)
				return false;
			Value.SetInt16(ZEEndian::Little(Temp));
		}
		break;

		case ZEML_ET_INT32:
		{
			ZEInt32 Temp;
			if (File->Read(&Temp, sizeof(ZEInt32), 1) != 1)
				return false;
			Value.SetInt32(ZEEndian::Little(Temp));
		}
		break;

		case ZEML_ET_INT64:
		{
			ZEInt64 Temp;
			if (File->Read(&Temp, sizeof(ZEInt64), 1) != 1)
				return false;
			Value.SetInt64(ZEEndian::Little(Temp));
		}
		break;

		case ZEML_ET_UINT8:
		{
			ZEUInt8 Temp;
			if (File->Read(&Temp, sizeof(ZEUInt8), 1) != 1)
				return false;
			Value.SetUInt8(Temp);
		}
		break;

		case ZEML_ET_UINT16:
		{
			ZEUInt16 Temp;
			if (File->Read(&Temp, sizeof(ZEUInt16), 1) != 1)
				return false;
			Value.SetUInt16(ZEEndian::Little(Temp));
		}
		break;

		case ZEML_ET_UINT32:
		{
			ZEUInt32 Temp;
			if (File->Read(&Temp, sizeof(ZEUInt32), 1) != 1)
				return false;
			Value.SetUInt32(ZEEndian::Little(Temp));
		}
		break;

		case ZEML_ET_UINT64:
		{
			ZEUInt64 Temp;
			if (File->Read(&Temp, sizeof(ZEUInt64), 1) != 1)
				return false;
			Value.SetUInt64(ZEEndian::Little(Temp));
		}
		break;

		case ZEML_ET_BOOLEAN:
		{
			bool Temp;
			if (File->Read(&Temp, sizeof(bool), 1) != 1)
				return false;
			Value.SetBoolean(Temp);
		}
		break;

		case ZEML_ET_FLOAT:
		{
			float Temp;
			if (File->Read(&Temp, sizeof(float), 1) != 1)
				return false;
			Value.SetFloat(Temp);
		}
		break;

		case ZEML_ET_DOUBLE:
		{
			double Temp;
			if (File->Read(&Temp, sizeof(double), 1) != 1)
				return false;
			Value.SetDouble(Temp);
		}
		break;

		case ZEML_ET_STRING:
		{
			ZEUInt32 StringSize;
			if (File->Read(&StringSize, sizeof(ZEUInt32), 1) != 1)
				return false;
			StringSize = ZEEndian::Little(StringSize);

			if (StringSize == 0)
			{
				Value.SetString("");
			}
			else
			{
				ZEPointer<char> Temp = new char[StringSize + 1];
				if (File->Read(Temp, StringSize, 1) != 1)
					return false;
				Temp[StringSize] = '\0';
				Value.SetString(Temp);
			}
		}
		break;

		case ZEML_ET_QUATERNION:
		{
			ZEQuaternion Temp;
			if (File->Read(&Temp, sizeof(ZEQuaternion), 1) != 1)
				return false;
			Value.SetQuaternion(Temp);
		}
		break;

		case ZEML_ET_VECTOR2:
		{
			ZEVector2 Temp;
			if (File->Read(&Temp, sizeof(ZEVector2), 1) != 1)
				return false;
			Value.SetVector2(Temp);
		}
		break;

		case ZEML_ET_VECTOR3:
		{
			ZEVector3 Temp;
			if (File->Read(&Temp, sizeof(ZEVector3), 1) != 1)
				return false;
			Value.SetVector3(Temp);
		}
		break;

		case ZEML_ET_VECTOR4:
		{
			ZEVector4 Temp;
			if (File->Read(&Temp, sizeof(ZEVector4), 1) != 1)
				return false;
			Value.SetVector4(Temp);
		}
		break;

		case ZEML_ET_MATRIX3X3:
		{
			ZEMatrix3x3 Temp;
			if (File->Read(&Temp, sizeof(ZEMatrix3x3), 1) != 1)
				return false;
			Value.SetMatrix3x3(Temp);
		}
		break;

		case ZEML_ET_MATRIX4X4:
		{
			ZEMatrix4x4 Temp;
			if (File->Read(&Temp, sizeof(ZEMatrix4x4), 1) != 1)
				return false;
			Value.SetMatrix4x4(Temp);
		}
		break;

		default:
			return false;
	}

	return true;
}

static bool LoadDeprecatedProperty(ZEFile* File, ZEMLElementType Type, ZEValue& Value)
{
	ZEUInt64 ValueSize;
	if (File->Read(&ValueSize, sizeof(ZEUInt64), 1) != 1)
		return false;
	ValueSize = ZEEndian::Little(ValueSize);

	switch (Type)
	{
		case ZEML_ET_INT8:
			{
				ZEInt8 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetInt8(Temp);
			}
			break;

		case ZEML_ET_INT16:
			{
				ZEInt16 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetInt16(ZEEndian::Little(Temp));
			}
			break;

		case ZEML_ET_INT32:
			{
				ZEInt32 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetInt32(ZEEndian::Little(Temp));
			}
			break;

		case ZEML_ET_INT64:
			{
				ZEInt64 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetInt64(ZEEndian::Little(Temp));
			}
			break;

		case ZEML_ET_UINT8:
			{
				ZEUInt8 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetUInt8(Temp);
			}
			break;

		case ZEML_ET_UINT16:
			{
				ZEUInt16 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetUInt16(ZEEndian::Little(Temp));
			}
			break;

		case ZEML_ET_UINT32:
			{
				ZEUInt32 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetUInt32(ZEEndian::Little(Temp));
			}
			break;

		case ZEML_ET_UINT64:
			{
				ZEUInt64 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetUInt64(ZEEndian::Little(Temp));
			}
			break;

		case ZEML_ET_BOOLEAN:
			{
				bool Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetBoolean(Temp);
			}
			break;

		case ZEML_ET_FLOAT:
			{
				float Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetFloat(Temp);
			}
			break;

		case ZEML_ET_DOUBLE:
			{
				double Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetDouble(Temp);
			}
			break;

		case ZEML_ET_STRING:
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

		case ZEML_ET_QUATERNION:
			{
				ZEQuaternion Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetQuaternion(Temp);
			}
			break;

		case ZEML_ET_VECTOR2:
			{
				ZEVector2 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetVector2(Temp);
			}
			break;

		case ZEML_ET_VECTOR3:
			{
				ZEVector3 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetVector3(Temp);
			}
			break;

		case ZEML_ET_VECTOR4:
			{
				ZEVector4 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetVector4(Temp);
			}
			break;

		case ZEML_ET_MATRIX3X3:
			{
				ZEMatrix3x3 Temp;
				if (File->Read(&Temp, ValueSize, 1) != 1)
					return false;
				Value.SetMatrix3x3(Temp);
			}
			break;

		case ZEML_ET_MATRIX4X4:
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

bool ZEMLReaderNode::LoadDeprecated()
{
	if (VersionMajor == 0)
	{
		File->Seek(this->Offset, ZE_SF_BEGINING);

		ZEMLElementType ElementType;
		if (!LoadElementHeader(File, Name, ElementType) && ElementType != ZEML_ET_NODE)
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
		ZEMLElementType SubElementType;
		for (ZESize I = 0; I < ElementCount; I++)
		{
			ZEUInt64 SubElementOffset = File->Tell();
			if (!LoadElementHeader(File, SubElementName, SubElementType))
			{
				zeError("Corrupted ZEML file. Cannot read element. File Name: \"%s\".", File->GetPath().ToCString());
				return false;
			}

			if (SubElementType == ZEML_ET_NODE)
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
			else if (SubElementType == ZEML_ET_INLINE_DATA)
			{
				ZEMLReaderProperty* Property = Properties.Add();
				Property->Name = SubElementName;
				Property->Type = ZEML_ET_INLINE_DATA;
				if (File->Read(&Property->DataSize, sizeof(ZEUInt64), 1) != 1)
				{
					zeError("Corrupted ZEML file. Cannot read element. File Name: \"%s\".", File->GetPath().ToCString());
					return false;
				}
				Property->DataSize = ZEEndian::Little(Property->DataSize);
				Property->DataOffset = File->Tell();
				File->Seek(Property->DataSize, ZE_SF_CURRENT);
			}
			else if (SubElementType == ZEML_ET_OFFSET_DATA)
			{
				ZEMLReaderProperty* Property = Properties.Add();
				Property->Name = SubElementName;
				Property->Type = ZEML_ET_OFFSET_DATA;
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
				Property->Type = SubElementType;
				if (!LoadDeprecatedProperty(File, Property->Type, Property->Value))
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

bool ZEMLReaderNode::Load()
{
	File->Seek(this->Offset, ZE_SF_BEGINING);

	ZEMLElementType ElementType;
	if (!LoadElementHeader(File, Name, ElementType) && ElementType != ZEML_ET_NODE)
	{
		zeError("Corrupted ZEML file. Cannot read element. File Name: \"%s\".", File->GetPath().ToCString());
		return false;
	}

	ZEUInt64 ElementCount;
	File->Read(&Size, sizeof(ZEUInt64), 1);
	Size = ZEEndian::Little(Size);

	File->Read(&ElementCount, sizeof(ZEUInt64), 1);
	ElementCount = ZEEndian::Little(ElementCount);

	ZEString SubElementName;
	ZEMLElementType SubElementType;
	for (ZESize I = 0; I < ElementCount; I++)
	{
		ZEUInt64 SubElementOffset = File->Tell();
		if (!LoadElementHeader(File, SubElementName, SubElementType))
		{
			zeError("Corrupted ZEML file. Cannot read element. File Name: \"%s\".", File->GetPath().ToCString());
			return false;
		}

		if (SubElementType == ZEML_ET_NODE)
		{
			ZEMLReaderSubNode* Node = SubNodes.Add();
			Node->Name = SubElementName;
			Node->Offset = SubElementOffset;
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
			File->Seek(Node->Size + sizeof(ZEUInt64), ZE_SF_CURRENT);
		}
		else if (SubElementType == ZEML_ET_INLINE_DATA)
		{
			ZEMLReaderProperty* Property = Properties.Add();
			Property->Name = SubElementName;
			Property->Type = ZEML_ET_INLINE_DATA;
			if (File->Read(&Property->DataSize, sizeof(ZEUInt64), 1) != 1)
			{
				zeError("Corrupted ZEML file. Cannot read element. File Name: \"%s\".", File->GetPath().ToCString());
				return false;
			}
			Property->DataSize = ZEEndian::Little(Property->DataSize);
			Property->DataOffset = File->Tell();
			File->Seek(Property->DataSize, ZE_SF_CURRENT);
		}
		else if (SubElementType == ZEML_ET_OFFSET_DATA)
		{
			ZEMLReaderProperty* Property = Properties.Add();
			Property->Name = SubElementName;
			Property->Type = ZEML_ET_OFFSET_DATA;
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
			Property->Type = SubElementType;
			if (!LoadProperty(File, Property->Type, Property->Value))
			{
				zeError("Corrupted ZEML file. Cannot read element. File Name: \"%s\".", File->GetPath().ToCString());
				return false;
			}
		}
	}

	zeDebugCheck(!_CrtCheckMemory(), "Heap problem");

	return true;
};

const ZEString& ZEMLReaderNode::GetName()
{
	return Name;
}

const ZESmartArray<ZEMLReaderSubNode>& ZEMLReaderNode::GetSubNodes()
{
	return SubNodes;
}

const ZESmartArray<ZEMLReaderProperty>& ZEMLReaderNode::GetProperties()
{
	return Properties;
}

ZESize ZEMLReaderNode::GetSubNodeCount()
{
	return SubNodes.GetCount();
}

ZESize ZEMLReaderNode::GetSubNodeCount(const char* Name)
{
	ZESize Count = 0;
	for (ZESize I = 0; I < SubNodes.GetCount(); I++)
	{
		if (SubNodes[I].Name == Name)
			Count++;
	}

	return Count;
}

ZEMLReaderNode ZEMLReaderNode::GetSubNode(const char* Name, ZESize Index)
{
	for (ZESize I = 0; I < SubNodes.GetCount(); I++)
	{
		if (SubNodes[I].Name == Name)
		{
			 if (Index == 0)
			 {
				 ZEMLReaderNode Node;
				 Node.File = this->File;
				 Node.Offset = SubNodes[I].Offset;
				 Node.VersionMajor = VersionMajor;
				 Node.VersionMinor = VersionMinor;
				 (VersionMajor == 0)?(Node.LoadDeprecated()):(Node.Load());
				 return Node;
			 }

			 Index--;
		}
	}

	return ZEMLReaderNode();
}

bool ZEMLReaderNode::IsValid()
{
	return Offset != -1;
}

bool ZEMLReaderNode::IsPropertyExists(const char* Name)
{
	return FindProperty(Name) != NULL;
}

bool ZEMLReaderNode::IsSubNodeExists(const char* Name)
{
	return GetSubNodeCount(Name) > 0;
}

ZEValue ZEMLReaderNode::ReadValue(const char* Name)
{
	const ZEMLReaderProperty* Property = FindProperty(Name);

	if (Property == NULL)
		return ZEValue();

	if (Property->Type == ZEML_ET_UNDEFINED || 
		Property->Type == ZEML_ET_NODE ||
		Property->Type == ZEML_ET_INLINE_DATA || 
		Property->Type == ZEML_ET_OFFSET_DATA)
	{
		return ZEValue();
	}

	return Property->Value;
}

ZEInt8 ZEMLReaderNode::ReadInt8(const char* Name, ZEInt8 Default)
{
	ZEValue Value = ReadValue(Name);
	
	if (Value.IsNull() || Value.GetType() != ZE_VRT_INTEGER_8)
		return Default;

	return Value.GetInt8();
}

ZEInt16 ZEMLReaderNode::ReadInt16(const char* Name, ZEInt16 Default)
{
	ZEValue Value = ReadValue(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_INTEGER_16)
		return Default;

	return Value.GetInt16();
}

ZEInt32 ZEMLReaderNode::ReadInt32(const char* Name, ZEInt32 Default)
{
	ZEValue Value = ReadValue(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_INTEGER_32)
		return Default;

	return Value.GetInt32();
}

ZEInt64 ZEMLReaderNode::ReadInt64(const char* Name, ZEInt64 Default)
{
	ZEValue Value = ReadValue(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_INTEGER_64)
		return Default;

	return Value.GetInt64();
}

ZEUInt8 ZEMLReaderNode::ReadUInt8(const char* Name, ZEUInt8 Default)
{
	ZEValue Value = ReadValue(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_UNSIGNED_INTEGER_8)
		return Default;

	return Value.GetUInt8();
}

ZEUInt16 ZEMLReaderNode::ReadUInt16(const char* Name, ZEUInt16 Default)
{
	ZEValue Value = ReadValue(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_UNSIGNED_INTEGER_16)
		return Default;

	return Value.GetUInt16();
}

ZEUInt32 ZEMLReaderNode::ReadUInt32(const char* Name, ZEUInt32 Default)
{
	ZEValue Value = ReadValue(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_UNSIGNED_INTEGER_32)
		return Default;

	return Value.GetUInt32();
}

ZEUInt64 ZEMLReaderNode::ReadUInt64(const char* Name, ZEUInt64 Default)
{
	ZEValue Value = ReadValue(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_UNSIGNED_INTEGER_64)
		return Default;

	return Value.GetUInt64();
}

float ZEMLReaderNode::ReadFloat(const char* Name, float Default)
{
	ZEValue Value = ReadValue(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_FLOAT)
		return Default;

	return Value.GetFloat();
}

double ZEMLReaderNode::ReadDouble(const char* Name, double Default)
{
	ZEValue Value = ReadValue(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_DOUBLE)
		return Default;

	return Value.GetDouble();
}

bool ZEMLReaderNode::ReadBoolean(const char* Name, bool Default)
{
	ZEValue Value = ReadValue(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_BOOLEAN)
		return Default;

	return Value.GetBoolean();	
}

ZEVector2 ZEMLReaderNode::ReadVector2(const char* Name, const ZEVector2& Default)
{
	ZEValue Value = ReadValue(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_VECTOR2)
		return Default;

	return Value.GetVector2();
}

ZEVector3 ZEMLReaderNode::ReadVector3(const char* Name, const ZEVector3& Default)
{
	ZEValue Value = ReadValue(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_VECTOR3)
		return Default;

	return Value.GetVector3();
}

ZEVector4 ZEMLReaderNode::ReadVector4(const char* Name, const ZEVector4& Default)
{
	ZEValue Value = ReadValue(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_VECTOR4)
		return Default;

	return Value.GetVector4();
}

ZEQuaternion ZEMLReaderNode::ReadQuaternion(const char* Name, const ZEQuaternion& Default)
{
	ZEValue Value = ReadValue(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_QUATERNION)
		return Default;

	return Value.GetQuaternion();
}

ZEMatrix3x3 ZEMLReaderNode::ReadMatrix3x3(const char* Name, const ZEMatrix3x3& Default)
{
	ZEValue Value = ReadValue(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_MATRIX3X3)
		return Default;

	return Value.GetMatrix3x3();
}

ZEMatrix4x4 ZEMLReaderNode::ReadMatrix4x4(const char* Name, const ZEMatrix4x4& Default)
{
	ZEValue Value = ReadValue(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_MATRIX4X4)
		return Default;

	return Value.GetMatrix4x4();
}

ZEString ZEMLReaderNode::ReadString(const char* Name, const ZEString& Default)
{
	ZEValue Value = ReadValue(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_STRING)
		return Default;

	return Value.GetString();
}

ZESize ZEMLReaderNode::ReadDataSize(const char* Name)
{
	const ZEMLReaderProperty* Property = FindProperty(Name);
	if (Property == NULL)
		return 0;

	if (Property->Type != ZEML_ET_INLINE_DATA && Property->Type != ZEML_ET_OFFSET_DATA)
		return 0;

	return Property->DataSize;
}

bool ZEMLReaderNode::ReadData(const char* Name, void* Buffer, ZESize BufferSize, ZESize Offset)
{
	const ZEMLReaderProperty* Property = FindProperty(Name);
	if (Property == NULL)
		return true;

	if (Property->Type != ZEML_ET_INLINE_DATA && Property->Type != ZEML_ET_OFFSET_DATA)
		return true;


	ZESize EffectiveSize = Property->DataSize - Offset;
	if (EffectiveSize > BufferSize)
		EffectiveSize = BufferSize;

	File->Seek(Property->DataOffset, ZE_SF_BEGINING);

	ZESize Result = File->Read(Buffer, EffectiveSize, 1);
	if (Result != 1)
		return false;

	return true;
}

ZEMLReaderNode::ZEMLReaderNode()
{
	File = NULL;
	Offset = -1;
	Size = 0;
	VersionMajor = 0;
}

bool ZEMLReader::Load()
{
	if (RootNode.Offset != -1)
		return true;

	if (File == NULL)
	{
		zeError("Cannot load ZEML file. File is NULL.");
		return false;
	}

	if (!File->IsOpen())
	{
		zeError("Cannot load ZEML file. File is not open.");
		return false;
	}

	File->Seek(0, ZE_SF_BEGINING);

	char Identifier[4];
	if (File->Read(Identifier, 4, 1) != 1)
	{
		zeError("Cannot load ZEML file. Corrupted ZEML file. File Name: \"%s\".", File->GetPath().ToCString());
		return false;
	}

	if (Identifier[0] == 'Z' &&
		Identifier[1] == 'E' &&
		Identifier[2] == 'M' &&
		Identifier[3] == 'L')
	{
		// Version 1+
		ZEUInt8 Version[2];
		if (File->Read(&Version, 2 * sizeof(ZEUInt8), 1) != 1)
		{
			zeError("Cannot load ZEML file. Corrupted ZEML file. File Name: \"%s\".", File->GetPath().ToCString());
			return false;
		}

		if (Version[0] > 1)
		{
			zeWarning("Higher unknown ZEML file major version detected. Unknown major versions can cause problems. Current Version: 1.0. Detected Version: %d.%d. File Name: \"%s\".", 
				Version[0],
				Version[1],
				File->GetPath());
		}

		ZEUInt64 StartOffset;
		if (File->Read(&StartOffset, sizeof(ZEUInt64), 1) != 1)
		{
			zeError("Cannot load ZEML file. Corrupted ZEML file. File Name: \"%s\".", File->GetPath().ToCString());
			return false;
		}
		StartOffset = ZEEndian::Little(StartOffset);

		RootNode.VersionMajor = Version[0];
		RootNode.VersionMinor = Version[1];
		RootNode.Offset = StartOffset;
		RootNode.File = File;

		return RootNode.Load();
	}
	else if (Identifier[0] == 'Z' && Identifier[1] == ZEML_ET_NODE)
	{
		// Version 0
		zeWarning("Old depricated ZEML file version detected. Please convert this file to new version for future compability. Current Version: 1.0. Detected Version: 0.0. File Name: \"%s\".", File->GetPath().ToCString());
		RootNode.VersionMajor = 0;
		RootNode.VersionMinor = 0;
		RootNode.Offset = 0;
		RootNode.File = File;

		return RootNode.LoadDeprecated();
	}
	else
	{
		zeError("Cannot read ZEML file. Corrupted ZEML file. File Name: \"%s\".", File->GetPath().ToCString());
		return false;
	}
}

ZEMLReaderNode ZEMLReader::GetRootNode()
{
	return RootNode;
}

ZEUInt ZEMLReader::GetVersionMajor()
{
	return VersionMajor;
}

ZEUInt ZEMLReader::GetVersionMinor()
{
	return VersionMinor;
}

bool ZEMLReader::Open(const char* FileName)
{
	Close();
	
	if (!OwnedFile.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE))
	{
		zeError("Cannot read ZEML file. Cannot open file. File Name: \"%s\"", FileName);
		return false;
	}

	this->File = &OwnedFile;
	return Load();
}

bool ZEMLReader::Open(ZEFile* File)
{
	Close();

	this->File = File;
	return Load();
}

void ZEMLReader::Close()
{
	if (OwnedFile.IsOpen())
	{
		OwnedFile.Close();
		File = NULL;
	}
	
	RootNode = ZEMLReaderNode();
	VersionMajor = 0;
	VersionMinor = 0;
}

ZEMLReader::ZEMLReader()
{
	File = NULL;
	VersionMajor = 0;
	VersionMinor = 0;
}

ZEMLReader::ZEMLReader(ZEFile* File)
{
	this->File = File;
}

ZEMLReader::~ZEMLReader()
{
	Close();
}
