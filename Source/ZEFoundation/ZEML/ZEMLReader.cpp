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
#include "ZEDS\ZEHashGenerator.h"

#define ZEML_ITEM_FILE_IDENTIFIER	'Z'
#define ZEML_MAX_NAME_SIZE			256

ZEMLReaderProperty::ZEMLReaderProperty()
{
	ElementType = ZEML_ET_NONE;
	ValueType = ZEML_VT_UNDEFINED;
	DataSize = 0;
	DataOffset = 0;
}

ZEMLReaderSubNode::ZEMLReaderSubNode()
{
	Offset = 0;
	Size = 0;
}

const ZEMLFormatElement* ZEMLReaderNode::FindElement(const char* Name, ZEMLElementType Type, ZESize Index)
{
	ZEUInt32 Hash = ZEHashGenerator::Hash(Name);
	for (ZESize I = 0; I < Elements.GetCount(); I++)
		if (Elements[I].NameHash == Hash && Elements[I].Name.Equals(Name))
			return &Elements[I];

	return NULL;
}

bool ZEMLReaderNode::Load()
{
	if (!Format->ReadGoToNode(File, Node))
		return false;

	Elements.SetCount(Node.Count);
	for (ZESize I = 0; I < Node.Count; I++)
	{
		if (!Format->ReadElement(File, Elements[I]))
			return false;
	}

	return true;
};

const ZEString& ZEMLReaderNode::GetName()
{
	return Node.Name;
}

const ZEArray<ZEMLFormatElement>& ZEMLReaderNode::GetElements()
{
	return Elements;
}

ZESize ZEMLReaderNode::GetNodeCount()
{
	return NodeCount;
}

ZESize ZEMLReaderNode::GetNodeCount(const char* Name)
{
	ZESize Count = 0;
	ZEUInt32 NameHash = ZEHashGenerator::Hash(Name);

	for (ZESize I = 0; I < Elements.GetCount(); I++)
		if (Elements[I].NameHash == NameHash && Elements[I].Name == Name)
			Count++;

	return Count;
}

ZEMLReaderNode ZEMLReaderNode::GetNode(const char* Name, ZESize Index)
{
	ZEUInt32 NameHash = ZEHashGenerator::Hash(Name);
	for (ZESize I = 0; I < Elements.GetCount(); I++)
	{
		if (Elements[I].NameHash == NameHash && Elements[I].Name == Name)
		{
			 if (Index == 0)
				 return GetNode(I);
			 Index--;
		}
	}

	return ZEMLReaderNode();
}

ZEMLReaderNode ZEMLReaderNode::GetNode(ZESize Index)
{
	if (Index >= NodeCount)
		return ZEMLReaderNode();

	if (Elements[Index].ElementType != ZEML_ET_NODE)
		return ZEMLReaderNode();

	ZEMLReaderNode Node;
	Node.File = File;
	Node.Format = Format;
	Node.Node = Elements[Index];

	if (!Node.Load())
	{
		zeError("Cannot load ZEML node.");
		return ZEMLReaderNode();
	}

	return Node;
}

bool ZEMLReaderNode::IsValid()
{
	return File != NULL && Format != NULL;
}

bool ZEMLReaderNode::IsPropertyExists(const char* Name)
{
	return FindElement(Name, ZEML_ET_PROPERTY) != NULL;
}

bool ZEMLReaderNode::IsSubNodeExists(const char* Name)
{
	return FindElement(Name, ZEML_ET_NODE) != NULL;
}

ZEValue ZEMLReaderNode::ReadValue(const char* Name)
{
	const ZEMLFormatElement* Property = FindElement(Name, ZEML_ET_PROPERTY);

	if (Property == NULL)
		return ZEValue();

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
	const ZEMLFormatElement* Property = FindElement(Name, ZEML_ET_DATA);
	if (Property == NULL)
		return 0;

	if (Property->ElementType != ZEML_ET_DATA)
		return 0;

	return Property->Size;
}

bool ZEMLReaderNode::ReadData(const char* Name, void* Buffer, ZESize BufferSize, ZESize Offset)
{
	const ZEMLFormatElement* Property = FindElement(Name, ZEML_ET_DATA);
	if (Property == NULL)
		return true;

	if (Property->ElementType != ZEML_ET_DATA)
		return true;

	ZESize EffectiveSize = Property->Size - Offset;
	if (EffectiveSize > BufferSize)
		EffectiveSize = BufferSize;


	return Format->ReadData(File, *Property, Buffer, Offset, BufferSize);
}

bool ZEMLReaderNode::ReadDataItems(const char* Name, void* Buffer, ZESize ElementSize, ZESize ElementCount, ZESize Offset)
{
	const ZEMLFormatElement* Property = FindElement(Name, ZEML_ET_DATA);
	if (Property == NULL)
		return true;

	if (Property->ElementType != ZEML_ET_DATA)
		return true;
	
	if (Offset + ElementCount * ElementSize > Property->Size)
		return false;

	File->Seek(Property->Offset + Offset, ZE_SF_BEGINING);

	ZESize Result = File->Read(Buffer, ElementCount * ElementSize, 1);
	if (Result != 1)
		return false;

	return true;
}

ZEMLReaderNode::ZEMLReaderNode()
{
	File = NULL;
	Format = NULL;
}

bool ZEMLReader::Load()
{
	ZEMLFormatDescription*const* FormatDescriptions = ZEMLFormat::GetFormats();
	for (ZESize I = 0; I < ZEMLFormat::GetFormatCount(); I++)
	{
		if ((FormatDescriptions[I]->GetSupport() & ZEML_FS_READ) == 0)
			continue;

		if (!FormatDescriptions[I]->Determine(File))
			continue;

		Format = FormatDescriptions[I]->CreateInstance();
		break;
	}

	if (Format == NULL)
	{
		zeError("Unknown ZEML file format. File Name: \"%s\".", File->GetPath().ToCString());
		return false;
	}

	RootNode.File = File;
	RootNode.Format = Format;

	Format->ReadHeader(File);
	Format->ReadElement(File, RootNode.Node);
	if (RootNode.Node.ElementType != ZEML_ET_NODE)
		return false;

	return RootNode.Load();

	return false;
}

ZEMLReaderNode ZEMLReader::GetRootNode()
{
	return RootNode;
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

	if (!Load())
	{
		zeError("Cannot read ZEML file. Corrupted or unknown ZEML file. File Name: \"%s\"", FileName);
		return false;
	}

	return true;
}

bool ZEMLReader::Open(ZEFile* File)
{
	Close();

	this->File = File;

	if (!Load())
	{
		zeError("Cannot read ZEML file. Corrupted or unknown ZEML file. File Name: \"%s\"", File->GetPath());
		return false;
	}

	return true;
}

void ZEMLReader::Close()
{
	if (OwnedFile.IsOpen())
	{
		OwnedFile.Close();
		File = NULL;
	}
	
	RootNode = ZEMLReaderNode();
}

ZEMLReader::ZEMLReader()
{
	File = NULL;
	Format = NULL;
}

ZEMLReader::~ZEMLReader()
{
	Close();
}
