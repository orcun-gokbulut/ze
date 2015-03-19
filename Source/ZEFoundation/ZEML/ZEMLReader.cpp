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

ZEMLReaderProperty::ZEMLReaderProperty()
{
	Type = ZEML_IT_UNDEFINED;
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
	Load();

	for (ZESize I = 0; I < Properties.GetCount(); I++)
	{
		if (Properties[I].Name == Name)
			return &Properties[I];
	}

	return NULL;
}

static bool LoadItemV0(ZEFile* File, ZEString& Name, ZEMLItemType& ItemType, ZEValue& Value, ZEUInt64& Size, ZEUInt64& Offset, ZEUInt64& ItemCount)
{
	Offset = File->Tell();

	char Identifier;	
	if (File->Read(&Identifier, sizeof(char), 1) != 1)
		return false;

	if (Identifier != ZEML_ITEM_FILE_IDENTIFIER)
		return false;
	
	ZEUInt8 ItemTypeData = 0;
	if (File->Read(&ItemTypeData, sizeof(ZEUInt8), 1) != 1)
		return false;
	ItemType = (ZEMLItemType)ItemTypeData;

	ZEUInt8 NameSize = 0;
	if (File->Read(&NameSize, sizeof(ZEUInt8), 1) != 1)
		return false;

	char NameBuffer[ZEML_MAX_NAME_SIZE];
	if (File->Read(NameBuffer, NameSize, 1) != 1)
		return false;
	
	if (ItemType == ZEML_IT_NODE)
	{
		if (File->Read(&ItemCount, sizeof(ZEUInt64), 1) != 1)
			return false;
		ItemCount = ZEEndian::Little(ItemCount);

		if (File->Read(&ItemCount, sizeof(ZEUInt64), 1) != 1)
			return false;
		ItemCount = ZEEndian::Little(ItemCount);

		return true;
	}

	if (File->Read(&ItemCount, sizeof(ZEUInt64), 1) != 1)
		return false;
	ItemCount = ZEEndian::Little(ItemCount);

	if (ItemType == ZEML_IT_INLINE_DATA)
	{
		Offset = File->Tell();
		File->Seek(Size, ZE_SF_CURRENT);
	}
	else if (ItemType == ZEML_IT_OFFSET_DATA)
	{
		ZEUInt64 Temp;
		if (File->Read(&Temp, sizeof(ZEUInt64), 1) != 1)
			return false;
		Offset = ZEEndian::Little(Temp);
	}
	else if (ItemType == ZEML_IT_INT8)
	{
		ZEInt8 Temp;
		if (!File->Read(&Temp, sizeof(ZEInt8), 1))
			return false;
		Value.SetInt8(Temp);
	}
	else if (ItemType == ZEML_IT_INT16)
	{
		ZEInt16 Temp;
		if (!File->Read(&Temp, sizeof(ZEInt16), 1))
			return false;
		Value.SetInt16(ZEEndian::Little(Temp));
	}
	else if (ItemType == ZEML_IT_INT32)
	{
		ZEInt32 Temp;
		if (!File->Read(&Temp, sizeof(ZEInt32), 1))
			return false;
		Value.SetInt32(ZEEndian::Little(Temp));
	}
	else if (ItemType == ZEML_IT_INT64)
	{
		ZEInt64 Temp;
		if (!File->Read(&Temp, sizeof(ZEInt64), 1))
			return false;
		Value.SetInt64(ZEEndian::Little(Temp));
	}
	else if (ItemType == ZEML_IT_UINT8)
	{
		ZEUInt8 Temp;
		if (!File->Read(&Temp, sizeof(ZEUInt8), 1))
			return false;
		Value.SetUInt8(Temp);
	}
	else if (ItemType == ZEML_IT_UINT16)
	{
		ZEUInt16 Temp;
		if (!File->Read(&Temp, sizeof(ZEUInt16), 1))
			return false;
		Value.SetUInt16(ZEEndian::Little(Temp));
	}
	else if (ItemType == ZEML_IT_UINT32)
	{
		ZEUInt32 Temp;
		if (!File->Read(&Temp, sizeof(ZEUInt32), 1))
			return false;
		Value.SetUInt32(ZEEndian::Little(Temp));
	}
	else if (ItemType == ZEML_IT_UINT64)
	{
		ZEUInt64 Temp;
		if (!File->Read(&Temp, sizeof(ZEUInt64), 1))
			return false;
		Value.SetUInt64(ZEEndian::Little(Temp));
	}
	else if (ItemType == ZEML_IT_BOOLEAN)
	{
		bool Temp;
		if (!File->Read(&Temp, sizeof(bool), 1))
			return false;
		Value.SetBoolean(Temp);
	}
	else if (ItemType == ZEML_IT_FLOAT)
	{
		float Temp;
		if (!File->Read(&Temp, sizeof(float), 1))
			return false;
		Value.SetFloat(Temp);
	}
	else if (ItemType == ZEML_IT_DOUBLE)
	{
		double Temp;
		if (!File->Read(&Temp, sizeof(double), 1))
			return false;
		Value.SetDouble(Temp);
	}
	else if (ItemType == ZEML_IT_STRING)
	{
		if (Size == 0)
			Value.SetString("");
		else
		{
			char* Temp = new char[Size];
			if (!File->Read(Temp, Size, 1))
				return false;
			Value.SetString(Temp);
			delete Temp;
		}
	}
	else if (ItemType == ZEML_IT_QUATERNION)
	{
		ZEQuaternion Temp;
		if (!File->Read(&Temp, sizeof(ZEQuaternion), 1))
			return false;
		Value.SetQuaternion(Temp);
	}
	else if (ItemType ==  ZEML_IT_VECTOR2)
	{
		ZEVector2 Temp;
		if (!File->Read(&Temp, sizeof(ZEVector2), 1))
			return false;
		Value.SetVector2(Temp);
	}
	else if (ItemType == ZEML_IT_VECTOR3)
	{
		ZEVector3 Temp;
		if (!File->Read(&Temp, sizeof(ZEVector3), 1))
			return false;
		Value.SetVector3(Temp);
	}
	else if (ItemType == ZEML_IT_VECTOR4)
	{
		ZEVector4 Temp;
		if (!File->Read(&Temp, sizeof(ZEVector4), 1))
			return false;
		Value.SetVector4(Temp);
	}
	else if (ItemType == ZEML_IT_MATRIX3X3)
	{
		ZEMatrix3x3 Temp;
		if (!File->Read(&Temp, sizeof(ZEMatrix3x3), 1))
			return false;
		Value.SetMatrix3x3(Temp);
	}
	else if (ItemType == ZEML_IT_MATRIX4X4)
	{
		ZEMatrix4x4 Temp;
		if (!File->Read(&Temp, sizeof(ZEMatrix4x4), 1))
			return false;
		Value.SetMatrix4x4(Temp);
	}
	else
	{
		// Unknown Type Skipping It
		File->Seek(Size, ZE_SF_CURRENT);
	}

	return true;
}

void ZEMLReaderNode::Load()
{
	File->Seek(this->Offset, ZE_SF_BEGINING);

	ZEString ItemName;
	ZEMLItemType ItemType;
	ZEUInt64 ItemSize, ItemOffset, ItemCount;
	ZEValue ItemValue;

	if (!LoadItemV0(File, ItemName, ItemType, ItemValue, ItemSize, ItemOffset, ItemCount) && ItemType != ZEML_IT_NODE)
	{
		zeError("Corrupted ZEML file. Cannot read item. File Name: \"%s\".", File->GetPath().ToCString());
		return;
	}

	this->Name = ItemName;
	this->Size = ItemSize;
	this->NextNodeOffset = ItemOffset + ItemSize;

	NextNodeOffset = ItemOffset + ItemSize;

	for (ZESize I = 0; I < ItemCount; I++)
	{
		if (!LoadItemV0(File, ItemName, ItemType, ItemValue, ItemSize, ItemOffset, ItemCount))
		{
			zeError("Corrupted ZEML file. Cannot read item. File Name: \"%s\".", File->GetPath().ToCString());
			return;
		}

		if (ItemType == ZEML_IT_NODE)
		{
			ZEMLReaderSubNode* Node = SubNodes.Add();
			Node->Name = ItemName;
			Node->Offset = ItemOffset;
			Node->Size = ItemSize;
		}
		else if (ItemType == ZEML_IT_INLINE_DATA || ItemType == ZEML_IT_OFFSET_DATA)
		{
			ZEMLReaderProperty* Property = Properties.Add();
			Property->Name = ItemName;
			Property->Type = ItemType;
			Property->DataOffset = ItemOffset;
			Property->DataSize = ItemSize;
		}
		else
		{
			ZEMLReaderProperty* Property = Properties.Add();
			Property->Name = ItemName;
			Property->Type = ItemType;
			Property->Value = ItemValue;
		}
	}
};

const char* ZEMLReaderNode::GetName()
{
	Load();
	return Name;
}

const ZESmartArray<ZEMLReaderSubNode>& ZEMLReaderNode::GetSubNodes()
{
	Load();
	return SubNodes;
}

const ZESmartArray<ZEMLReaderProperty>& ZEMLReaderNode::GetProperties()
{
	Load();
	return Properties;
}

ZESize ZEMLReaderNode::GetSubNodeCount()
{
	Load();
	return SubNodes.GetCount();
}

ZESize ZEMLReaderNode::GetSubNodeCount(const char* Name)
{
	Load();
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
	Load();
	for (ZESize I = 0; I < SubNodes.GetCount(); I++)
	{
		if (SubNodes[I].Name == Name)
		{
			 if (Index == 0)
			 {
				 ZEMLReaderNode Node;
				 Node.File = this->File;
				 Node.Offset = SubNodes[I].Offset;
				 Node.Version = Version;
				 Node.PrevNodeOffset = (I == 0 ? -1 : SubNodes[I].Offset);
				 Node.NextNodeOffset = (I + 1 == SubNodes.GetCount() ? -1 : SubNodes[I + 1].Offset);
				 return Node;
			 }

			 Index--;
		}
	}

	return ZEMLReaderNode();
}

ZEMLReaderNode ZEMLReaderNode::GetPrevNode()
{
	Load();
	if (PrevNodeOffset == -1)
		return ZEMLReaderNode();

	ZEMLReaderNode Node;
	Node.File = this->File;
	Node.Offset = PrevNodeOffset;
	Node.NextNodeOffset = this->Offset;
	Node.Version = Version;
	return Node;
}


ZEMLReaderNode ZEMLReaderNode::GetNextNode()
{
	Load();
	if (NextNodeOffset == -1)
		return ZEMLReaderNode();

	ZEMLReaderNode Node;
	Node.File = this->File;
	Node.Offset = NextNodeOffset;
	Node.PrevNodeOffset = this->Offset;
	Node.Version = Version;
	return Node;
}

bool ZEMLReaderNode::IsValid()
{
	return Offset != -1;
}

bool ZEMLReaderNode::IsPropertyExists(const char* Name)
{
	ZEValue Value = Read(Name);
	return !Value.IsNull();
}

bool ZEMLReaderNode::IsSubNodeExists(const char* Name)
{
	return GetSubNodeCount(Name) > 0;
}

ZEValue ZEMLReaderNode::Read(const char* Name)
{
	const ZEMLReaderProperty* Property = FindProperty(Name);

	if (Property == NULL)
		return ZEValue();

	if (Property->Type == ZEML_IT_UNDEFINED || 
		Property->Type == ZEML_IT_NODE ||
		Property->Type == ZEML_IT_INLINE_DATA || 
		Property->Type == ZEML_IT_OFFSET_DATA)
	{
		return ZEValue();
	}

	return Property->Value;
}

ZEInt8 ZEMLReaderNode::ReadInt8(const char* Name, ZEInt8 Default)
{
	ZEValue Value = Read(Name);
	
	if (Value.IsNull() || Value.GetType() != ZE_VRT_INTEGER_8)
		return Default;

	return Value.GetInt8();
}

ZEInt16 ZEMLReaderNode::ReadInt16(const char* Name, ZEInt16 Default)
{
	ZEValue Value = Read(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_INTEGER_16)
		return Default;

	return Value.GetInt16();
}

ZEInt32 ZEMLReaderNode::ReadInt32(const char* Name, ZEInt32 Default)
{
	ZEValue Value = Read(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_INTEGER_32)
		return Default;

	return Value.GetInt32();
}

ZEInt64 ZEMLReaderNode::ReadInt64(const char* Name, ZEInt64 Default)
{
	ZEValue Value = Read(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_INTEGER_64)
		return Default;

	return Value.GetInt64();
}

ZEUInt8 ZEMLReaderNode::ReadUInt8(const char* Name, ZEUInt8 Default)
{
	ZEValue Value = Read(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_UNSIGNED_INTEGER_8)
		return Default;

	return Value.GetUInt8();
}

ZEUInt16 ZEMLReaderNode::ReadUInt16(const char* Name, ZEUInt16 Default)
{
	ZEValue Value = Read(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_UNSIGNED_INTEGER_16)
		return Default;

	return Value.GetUInt16();
}

ZEUInt32 ZEMLReaderNode::ReadUInt32(const char* Name, ZEUInt32 Default)
{
	ZEValue Value = Read(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_UNSIGNED_INTEGER_32)
		return Default;

	return Value.GetUInt32();
}

ZEUInt64 ZEMLReaderNode::ReadUInt64(const char* Name, ZEUInt64 Default)
{
	ZEValue Value = Read(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_UNSIGNED_INTEGER_64)
		return Default;

	return Value.GetUInt64();
}

float ZEMLReaderNode::ReadFloat(const char* Name, float Default)
{
	ZEValue Value = Read(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_FLOAT)
		return Default;

	return Value.GetFloat();
}

double ZEMLReaderNode::ReadDouble(const char* Name, double Default)
{
	ZEValue Value = Read(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_DOUBLE)
		return Default;

	return Value.GetDouble();
}

ZEVector2 ZEMLReaderNode::ReadVector3(const char* Name, const ZEVector2& Default)
{
	ZEValue Value = Read(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_VECTOR2)
		return Default;

	return Value.GetVector2();
}

ZEVector3 ZEMLReaderNode::ReadVector4(const char* Name, const ZEVector3& Default)
{
	ZEValue Value = Read(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_VECTOR3)
		return Default;

	return Value.GetVector3();
}

ZEVector4 ZEMLReaderNode::ReadVector4(const char* Name, const ZEVector4& Default)
{
	ZEValue Value = Read(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_VECTOR4)
		return Default;

	return Value.GetVector4();
}

ZEQuaternion ZEMLReaderNode::ReadQuaternion(const char* Name, const ZEQuaternion& Default)
{
	ZEValue Value = Read(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_QUATERNION)
		return Default;

	return Value.GetQuaternion();
}

ZEMatrix3x3 ZEMLReaderNode::ReadMatrix3x3(const char* Name, const ZEMatrix3x3& Default)
{
	ZEValue Value = Read(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_MATRIX3X3)
		return Default;

	return Value.GetMatrix3x3();
}

ZEMatrix4x4 ZEMLReaderNode::ReadMatrix4x4(const char* Name, const ZEMatrix4x4& Default)
{
	ZEValue Value = Read(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_MATRIX4X4)
		return Default;

	return Value.GetMatrix4x4();
}

ZEString ZEMLReaderNode::ReadString(const char* Name, const char* Default)
{
	ZEValue Value = Read(Name);

	if (Value.IsNull() || Value.GetType() != ZE_VRT_STRING)
		return Default;

	return Value.GetString();
}

ZESize ZEMLReaderNode::ReadDataSize(const char* Name)
{
	const ZEMLReaderProperty* Property = FindProperty(Name);
	if (Property == NULL)
		return 0;

	if (Property->Type != ZEML_IT_INLINE_DATA && Property->Type != ZEML_IT_OFFSET_DATA)
		return 0;

	return Property->DataSize;
}

bool ZEMLReaderNode::ReadData(const char* Name, void* Buffer, ZESize BufferSize, ZESize Offset)
{
	const ZEMLReaderProperty* Property = FindProperty(Name);
	if (Property == NULL)
		return true;

	if (Property->Type != ZEML_IT_INLINE_DATA && Property->Type != ZEML_IT_OFFSET_DATA)
		return true;


	ZESize EffectiveSize = Property->DataSize - Offset;
	if (EffectiveSize > BufferSize)
		EffectiveSize = BufferSize;

	ZESize Result = File->Read(Buffer, EffectiveSize, 1);
	if (Result != 1)
		return false;

	return true;
}

ZEMLReaderNode::ZEMLReaderNode()
{
	File = NULL;
	PrevNodeOffset = -1;
	Offset = -1;
	NextNodeOffset = -1;
	Size = 0;
	Version = 0;
	Dirty = true;
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
		ZEUInt16 Version;
		if (File->Read(&Version, sizeof(ZEUInt32), 1) != 1)
		{
			zeError("Cannot load ZEML file. Corrupted ZEML file. File Name: \"%s\".", File->GetPath().ToCString());
			return false;
		}
		Version == ZEEndian::Little(Version);

		if ((Version & 0xFF00) > 0x0100)
		{
			zeWarning("Higher unknown ZEML file major version detected. Unknown major versions can cause problems. Current Version: 1.0. Detected Version: %d.%d. File Name: \"%s\".", 
				(Version & 0xFF00) >> 8,
				(Version & 0x00FF),
				File->GetPath());
		}

		ZEUInt64 StartOffset;
		if (File->Read(&StartOffset, sizeof(ZEUInt64), 1) != 1)
		{
			zeError("Cannot load ZEML file. Corrupted ZEML file. File Name: \"%s\".", File->GetPath().ToCString());
			return false;
		}
		StartOffset == ZEEndian::Little(StartOffset);

		RootNode.Version = Version;
		RootNode.Offset = 0;
		RootNode.File = File;
	}
	else if (Identifier[0] == 'Z' && Identifier[1] == ZEML_IT_NODE)
	{
		// Version 0
		zeWarning("Old depricated ZEML file version detected. Please convert this file to new version for future compability. Current Version: 1.0. Detected Version: 0.0. File Name: \"%s\".", File->GetPath().ToCString());
		RootNode.Version = Version;
		RootNode.Offset = 0;
		RootNode.File = File;
	}
	else
	{
		zeError("Cannot read ZEML file. Corrupted ZEML file. File Name: \"%s\".", File->GetPath().ToCString());
		return false;
	}

	return true;
}

ZEMLReaderNode ZEMLReader::GetRootNode()
{
	return RootNode;
}

ZEUInt ZEMLReader::GetVersion()
{
	return Version;
}

bool ZEMLReader::Open(ZEFile* File)
{
	Close();

	this->File = File;
	RootNode = ZEMLReaderNode();
	return Load();
}

void ZEMLReader::Close()
{
}

ZEMLReader::ZEMLReader()
{
	File = NULL;
	Version = 0;
}

ZEMLReader::ZEMLReader(ZEFile* File)
{
	this->File = File;
}

ZEMLReader::~ZEMLReader()
{
	Close();
}
