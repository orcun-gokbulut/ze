//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLWriter.cpp
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

#include "ZEMLWriter.h"

#include "ZEFile/ZEFile.h"
#include "ZEMLProperty.h"
#include "ZEMLDataProperty.h"
#include "ZEEndian.h"

ZEMLWriterNode::ZEMLWriterNode()
{
	File = NULL;
	Name = "";
	ParentNode = NULL;
	SubItemCount = 0;
	Size = 0;
}

static ZEMLItemType ConvertType(ZEValueType Type)
{
	switch (Type)
	{
		default:
		case ZE_VRT_UNDEFINED:
		case ZE_VRT_NULL:
		case ZE_VRT_CLASS:
			return ZEML_IT_UNDEFINED;

		case ZE_VRT_FLOAT:
			return ZEML_IT_FLOAT;

		case ZE_VRT_DOUBLE:
			return ZEML_IT_DOUBLE;

		case ZE_VRT_INTEGER_8:
			return ZEML_IT_INT8;

		case ZE_VRT_INTEGER_16:
			return ZEML_IT_INT16;

		case ZE_VRT_INTEGER_32:
			return ZEML_IT_INT32;

		case ZE_VRT_INTEGER_64:
			return ZEML_IT_UINT64;

		case ZE_VRT_UNSIGNED_INTEGER_8:
			return ZEML_IT_UINT8;

		case ZE_VRT_UNSIGNED_INTEGER_16:
			return ZEML_IT_UINT16;

		case ZE_VRT_UNSIGNED_INTEGER_32:
			return ZEML_IT_UINT32;

		case ZE_VRT_UNSIGNED_INTEGER_64:
			return ZEML_IT_UINT64;

		case ZE_VRT_BOOLEAN:
			return ZEML_IT_BOOLEAN;

		case ZE_VRT_STRING:
			return ZEML_IT_STRING;

		case ZE_VRT_QUATERNION:
			return ZEML_IT_QUATERNION;

		case ZE_VRT_VECTOR2:
			return ZEML_IT_VECTOR2;

		case ZE_VRT_VECTOR3:
			return ZEML_IT_VECTOR3;

		case ZE_VRT_VECTOR4:
			return ZEML_IT_VECTOR4;

		case ZE_VRT_MATRIX3X3:
			return ZEML_IT_MATRIX3X3;

		case ZE_VRT_MATRIX4X4:
			return ZEML_IT_MATRIX4X4;
	}
}

bool ZEMLWriterNode::WriteItemHeader(const char* Name, ZEMLItemType Type)
{
	if (Name == NULL)
		return false;

	if (Type == ZEML_IT_UNDEFINED)
		return false;

	char Header[2];
	Header[0] = 'Z';
	Header[1] = Type;
	File->Write(Header, 2 * sizeof(char), 1);

	ZESize Size = strlen(Name);
	if (Size > 254)
	{
		zeError("Cannot write ZEML property. Item name is too long (Max Size: 254). Item Name: \"%s\". File Name: \"%s\"", Name, File->GetPath());
		return false;
	}

	ZEUInt8 NameSize = (ZEUInt8)Size;
	File->Write(&NameSize, sizeof(ZEUInt8), 1);
	File->Write(Name, Size + 1, 1);

	return true;
}

bool ZEMLWriterNode::WriteValue(const char* Name, const ZEValue& Value)
{
	ZEMLItemType ItemType = ConvertType(Value.GetType());
	if (ItemType == ZEML_IT_UNDEFINED)
	{
		zeError("Cannot write ZEML property. Unsupported ZEValue type. File Name: \"%s\"", File->GetPath());
		return false;
	}

	if (!WriteItemHeader(Name, ItemType))
		return false;

	switch (ItemType)
	{
		default:
		case ZEML_IT_UNDEFINED:
			return false;

		case ZEML_IT_FLOAT:
		{
			float ValueTemp = Value.GetFloat();
			File->Write(&ValueTemp, sizeof(float), 1);
			break;
		}

		case ZEML_IT_DOUBLE:
		{
			double ValueTemp = Value.GetDouble();
			File->Write(&ValueTemp, sizeof(double), 1);
			break;
		}

		case ZEML_IT_INT8:
		{
			ZEInt8 ValueTemp = Value.GetInt8();
			File->Write(&ValueTemp, sizeof(ZEInt8), 1);
			break;
		}

		case ZEML_IT_INT16:
		{
			ZEInt16 ValueTemp = ZEEndian::Little(Value.GetInt16());
			File->Write(&ValueTemp, sizeof(ZEInt16), 1);
			break;
		}

		case ZEML_IT_INT32:
		{
			ZEInt32 ValueTemp = ZEEndian::Little(Value.GetInt32());
			File->Write(&ValueTemp, sizeof(ZEInt32), 1);
			break;
		}

		case ZEML_IT_INT64:
		{
			ZEInt64 ValueTemp = ZEEndian::Little(Value.GetInt64());
			File->Write(&ValueTemp, sizeof(ZEInt64), 1);
			break;
		}

		case ZEML_IT_UINT8:
		{
			ZEUInt8 ValueTemp = Value.GetUInt8();
			File->Write(&ValueTemp, sizeof(ZEUInt8), 1);
			break;
		}

		case ZEML_IT_UINT16:
		{
			ZEUInt16 ValueTemp = ZEEndian::Little(Value.GetUInt16());
			File->Write(&ValueTemp, sizeof(ZEUInt16), 1);
			break;
		}

		case ZEML_IT_UINT32:
		{
			ZEUInt32 ValueTemp = ZEEndian::Little(Value.GetUInt32());
			File->Write(&ValueTemp, sizeof(ZEUInt32), 1);
			break;
		}

		case ZEML_IT_UINT64:
		{
			ZEUInt64 ValueTemp = ZEEndian::Little(Value.GetUInt64());
			File->Write(&ValueTemp, sizeof(ZEUInt64), 1);
			break;
		}

		case ZEML_IT_BOOLEAN:
		{
			bool ValueTemp = Value.GetBoolean();
			File->Write(&ValueTemp, sizeof(bool), 1);
			break;
		}

		case ZEML_IT_STRING:
		{
			ZEUInt64 StringSize = ZEEndian::Little((ZEUInt64)Value.GetString().GetSize());
			File->Write(&StringSize, sizeof(ZEUInt64), 1);
			File->Write(Value.GetString().ToCString(), Value.GetString().GetSize() * sizeof(char), 1);
			break;
		}

		case ZEML_IT_QUATERNION:
		{
			ZEQuaternion ValueTemp = Value.GetQuaternion();
			File->Write(&ValueTemp, sizeof(ZEQuaternion), 1);
			break;
		}

		case ZEML_IT_VECTOR2:
		{
			ZEVector2 ValueTemp = Value.GetVector2();
			File->Write(&ValueTemp, sizeof(ZEVector2), 1);
			break;
		}

		case ZEML_IT_VECTOR3:
		{
			ZEVector3 ValueTemp = Value.GetVector3();
			File->Write(&ValueTemp, sizeof(ZEVector3), 1);
			break;
		}

		case ZEML_IT_VECTOR4:
		{
			ZEVector4 ValueTemp = Value.GetVector4();
			File->Write(&ValueTemp, sizeof(ZEVector4), 1);
			break;
		}

		case ZEML_IT_MATRIX3X3:
		{
			ZEMatrix3x3 ValueTemp = Value.GetMatrix3x3();
			File->Write(&ValueTemp, sizeof(ZEMatrix3x3), 1);
			break;
		}

		case ZEML_IT_MATRIX4X4:
		{
			ZEMatrix4x4 ValueTemp = Value.GetMatrix4x4();
			File->Write(&ValueTemp, sizeof(ZEMatrix4x4), 1);
			break;
		}
	}

	SubItemCount++;

	return true;
}

bool ZEMLWriterNode::WriteFloat(const char* Name, float Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteDouble(const char* Name, double Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteInt8(const char* Name, ZEInt8 Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteInt16(const char* Name, ZEInt16 Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteInt32(const char* Name, ZEInt32 Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteInt64(const char* Name, ZEInt64 Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteUInt8(const char* Name, ZEUInt8 Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteUInt16(const char* Name, ZEUInt16 Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteUInt32(const char* Name, ZEUInt32 Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteUInt64(const char* Name, ZEUInt64 Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteBool(const char* Name, bool Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteString(const char* Name, const char* Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteQuaternion(const char* Name, const ZEQuaternion& Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteVector2(const char* Name, const ZEVector2& Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteVector3(const char* Name, const ZEVector3& Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteVector4(const char* Name, const ZEVector4& Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteMatrix3x3(const char* Name, const ZEMatrix3x3& Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteMatrix4x4(const char* Name, const ZEMatrix4x4& Value)
{
	return WriteValue(Name, ZEValue(Value));
}

bool ZEMLWriterNode::WriteData(const char* Name, void* Data, ZEUInt64 DataSize)
{
	if (!WriteItemHeader(Name, ZEML_IT_INLINE_DATA))
		return false;

	ZESize Size = ZEEndian::Little(DataSize);
	File->Write(&Size, sizeof(ZEUInt64), 1);

	if (!File->Write(Data, DataSize, 1) != 0)
	{
		zeError("Cannot write ZEML file. Cannot write data property. Property Name: \"%s\". File Name: \"%s\".", Name, File->GetPath().ToCString());
		return false;
	}

	SubItemCount++;

	return true;
}

ZEMLWriterNode ZEMLWriterNode::OpenSubNode(const char* Name)
{
	if (WriteItemHeader(Name, ZEML_IT_NODE))
		return ZEMLWriterNode();

	ZEMLWriterNode NewNode;
	NewNode.Name = Name;
	NewNode.File = File;
	NewNode.ParentNode = this;
	NewNode.ParentNode->SubItemCount++;

	NewNode.FileUpdatePosition = File->Tell();

	ZEUInt64 NodeSize = 0;
	File->Write(&NodeSize, sizeof(ZEUInt64), 1);

	ZEUInt64 NodeSubItemCount = 0;
	File->Write(&NodeSubItemCount, sizeof(ZEUInt64), 1);

	return NewNode;
}

void ZEMLWriterNode::CloseNode()
{
	if (ParentNode != NULL)
	{
		ParentNode->Size += Size;
		ParentNode->Size +=	sizeof(char) + sizeof(ZEUInt8) + sizeof(ZEUInt8) + Name.GetSize() + sizeof(ZEUInt64) + sizeof(ZEUInt64);
	}
	
	ZEUInt64 CurrentFilePos = File->Tell();
	File->Seek(FileUpdatePosition, ZE_SF_BEGINING);

	ZEUInt64 NodeSize = ZEEndian::Little(Size);
	File->Write(&NodeSize, sizeof(ZEUInt64), 1);

	ZEUInt64 NodeSubItemCount = ZEEndian::Little(SubItemCount);
	File->Write(&NodeSubItemCount, sizeof(ZEUInt64), 1);

	File->Seek(CurrentFilePos, ZE_SF_BEGINING);
}

/************************************************************************/
/*							RootNode	                                */
/************************************************************************/

ZEMLWriterNode ZEMLWriter::WriteRootNode(const char* Name)
{
	ZEMLWriterNode Node;
	Node.File = File;

	ZEMLWriterNode RootNode	= Node.OpenSubNode(Name);
	RootNode.ParentNode = NULL;
	return RootNode;
}

void ZEMLWriter::WriteHeader()
{
	const char Identifer[6] =
	{
		'Z', 'E', 'M', 'L',
		1, // Major Version
		0 // Minor Version
	};
	File->Write(Identifer, 6, 1);

	ZEUInt64 StartOffset = ZEEndian::Little(OwnedFile.Tell());
	File->Write(&StartOffset, sizeof(ZEUInt64), 1);
}

bool ZEMLWriter::Open(const char* FileName)
{
	Close();

	if (!OwnedFile.Open(FileName, ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
	{
		zeError("Cannot write ZEML file. Cannot open file for writing. File Name: \"%s\"", FileName);
		return false;
	}

	File = &OwnedFile;

	WriteHeader();

	return true;
}

bool ZEMLWriter::Open(ZEFile* File)
{
	Close();

	if (File->IsOpen())
	{
		zeError("Cannot write ZEML file. File is not open.");
		return false;
	}

	if ((File->GetOpenMode() & ZE_FOM_WRITE) != ZE_FOM_WRITE)
	{
		zeError("Cannot write ZEML file. File open mode does not allow writing. File Name: \"%s\"", File->GetPath().ToCString());
		return false;
	}


	this->File = File;
	File->Seek(0, ZE_SF_BEGINING);

	WriteHeader();

	return true;
}

void ZEMLWriter::Close()
{
	OwnedFile.Flush();
	OwnedFile.Close();
}

ZEMLWriter::ZEMLWriter()
{
	File = NULL;
}
