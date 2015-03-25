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

bool ZEMLWriterNode::WriteElementHeader(const char* Name, ZEMLElementType Type)
{
	if (Name == NULL)
		return false;

	if (Type == ZEML_ET_UNDEFINED)
		return false;

	char Header[2];
	Header[0] = 'Z';
	Header[1] = Type;
	File->Write(Header, 2 * sizeof(char), 1);

	ZESize Size = strlen(Name) + 1;
	if (Size > 254)
	{
		zeError("Cannot write ZEML property. Item name is too long (Max Size: 254). Item Name: \"%s\". File Name: \"%s\"", Name, File->GetPath());
		return false;
	}

	ZEUInt8 NameSize = (ZEUInt8)Size;
	File->Write(&NameSize, sizeof(ZEUInt8), 1);
	File->Write(Name, Size, 1);

	return true;
}

bool ZEMLWriterNode::WriteValue(const char* Name, const ZEValue& Value)
{
	ZEMLElementType ItemType = ZEMLUtils::ConvertType(Value.GetType());
	if (ItemType == ZEML_ET_UNDEFINED)
	{
		zeError("Cannot write ZEML property. Unsupported ZEValue type. File Name: \"%s\"", File->GetPath());
		return false;
	}

	if (!WriteElementHeader(Name, ItemType))
		return false;

	switch (ItemType)
	{
		default:
		case ZEML_ET_UNDEFINED:
			return false;

		case ZEML_ET_FLOAT:
		{
			float ValueTemp = Value.GetFloat();
			File->Write(&ValueTemp, sizeof(float), 1);
			break;
		}

		case ZEML_ET_DOUBLE:
		{
			double ValueTemp = Value.GetDouble();
			File->Write(&ValueTemp, sizeof(double), 1);
			break;
		}

		case ZEML_ET_INT8:
		{
			ZEInt8 ValueTemp = Value.GetInt8();
			File->Write(&ValueTemp, sizeof(ZEInt8), 1);
			break;
		}

		case ZEML_ET_INT16:
		{
			ZEInt16 ValueTemp = ZEEndian::Little(Value.GetInt16());
			File->Write(&ValueTemp, sizeof(ZEInt16), 1);
			break;
		}

		case ZEML_ET_INT32:
		{
			ZEInt32 ValueTemp = ZEEndian::Little(Value.GetInt32());
			File->Write(&ValueTemp, sizeof(ZEInt32), 1);
			break;
		}

		case ZEML_ET_INT64:
		{
			ZEInt64 ValueTemp = ZEEndian::Little(Value.GetInt64());
			File->Write(&ValueTemp, sizeof(ZEInt64), 1);
			break;
		}

		case ZEML_ET_UINT8:
		{
			ZEUInt8 ValueTemp = Value.GetUInt8();
			File->Write(&ValueTemp, sizeof(ZEUInt8), 1);
			break;
		}

		case ZEML_ET_UINT16:
		{
			ZEUInt16 ValueTemp = ZEEndian::Little(Value.GetUInt16());
			File->Write(&ValueTemp, sizeof(ZEUInt16), 1);
			break;
		}

		case ZEML_ET_UINT32:
		{
			ZEUInt32 ValueTemp = ZEEndian::Little(Value.GetUInt32());
			File->Write(&ValueTemp, sizeof(ZEUInt32), 1);
			break;
		}

		case ZEML_ET_UINT64:
		{
			ZEUInt64 ValueTemp = ZEEndian::Little(Value.GetUInt64());
			File->Write(&ValueTemp, sizeof(ZEUInt64), 1);
			break;
		}

		case ZEML_ET_BOOLEAN:
		{
			bool ValueTemp = Value.GetBoolean();
			File->Write(&ValueTemp, sizeof(bool), 1);
			break;
		}

		case ZEML_ET_STRING:
		{
			ZEUInt32 StringSize = ZEEndian::Little((ZEUInt32)Value.GetString().GetSize());
			File->Write(&StringSize, sizeof(ZEUInt32), 1);
			File->Write(Value.GetString().ToCString(), Value.GetString().GetSize() * sizeof(char), 1);
			break;
		}

		case ZEML_ET_QUATERNION:
		{
			ZEQuaternion ValueTemp = Value.GetQuaternion();
			File->Write(&ValueTemp, sizeof(ZEQuaternion), 1);
			break;
		}

		case ZEML_ET_VECTOR2:
		{
			ZEVector2 ValueTemp = Value.GetVector2();
			File->Write(&ValueTemp, sizeof(ZEVector2), 1);
			break;
		}

		case ZEML_ET_VECTOR3:
		{
			ZEVector3 ValueTemp = Value.GetVector3();
			File->Write(&ValueTemp, sizeof(ZEVector3), 1);
			break;
		}

		case ZEML_ET_VECTOR4:
		{
			ZEVector4 ValueTemp = Value.GetVector4();
			File->Write(&ValueTemp, sizeof(ZEVector4), 1);
			break;
		}

		case ZEML_ET_MATRIX3X3:
		{
			ZEMatrix3x3 ValueTemp = Value.GetMatrix3x3();
			File->Write(&ValueTemp, sizeof(ZEMatrix3x3), 1);
			break;
		}

		case ZEML_ET_MATRIX4X4:
		{
			ZEMatrix4x4 ValueTemp = Value.GetMatrix4x4();
			File->Write(&ValueTemp, sizeof(ZEMatrix4x4), 1);
			break;
		}
	}

	SubElementCount++;

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

bool ZEMLWriterNode::WriteData(const char* Name, const void* Data, ZEUInt64 Size)
{
	if (!WriteElementHeader(Name, ZEML_ET_INLINE_DATA))
		return false;

	ZEUInt64 TempSize = ZEEndian::Little(Size);
	File->Write(&TempSize, sizeof(ZEUInt64), 1);

	if (!File->Write(Data, Size, 1) != 0)
	{
		zeError("Cannot write ZEML file. Cannot write data property. Property Name: \"%s\". File Name: \"%s\".", Name, File->GetPath().ToCString());
		return false;
	}

	SubElementCount++;

	return true;
}

ZEMLWriterNode ZEMLWriterNode::OpenSubNode(const char* Name)
{
	if (!WriteElementHeader(Name, ZEML_ET_NODE))
		return ZEMLWriterNode();

	ZEMLWriterNode NewNode;
	NewNode.Name = Name;
	NewNode.File = File;

	ZEUInt64 NodeSize = 0;
	File->Write(&NodeSize, sizeof(ZEUInt64), 1);

	ZEUInt64 NodeSubItemCount = 0;
	File->Write(&NodeSubItemCount, sizeof(ZEUInt64), 1);

	NewNode.NodeDataOffset = File->Tell();

	SubElementCount++;

	return NewNode;
}

void ZEMLWriterNode::CloseNode()
{
	ZEUInt64 CurrentFilePos = File->Tell();
	File->Seek(NodeDataOffset - 2 * sizeof(ZEUInt64), ZE_SF_BEGINING);

	ZEUInt64 NodeSize = ZEEndian::Little(CurrentFilePos - NodeDataOffset);
	File->Write(&NodeSize, sizeof(ZEUInt64), 1);

	ZEUInt64 NodeSubItemCount = ZEEndian::Little(SubElementCount);
	File->Write(&NodeSubItemCount, sizeof(ZEUInt64), 1);

	File->Seek(NodeDataOffset + NodeSize, ZE_SF_BEGINING);
}

ZEMLWriterNode::ZEMLWriterNode()
{
	File = NULL;
	Name = "";
	SubElementCount = 0;
}

ZEMLWriterNode::~ZEMLWriterNode()
{

}


/************************************************************************/
/*							RootNode	                                */
/************************************************************************/

ZEMLWriterNode ZEMLWriter::WriteRootNode(const char* Name)
{
	ZEMLWriterNode Node;
	Node.File = File;
	Node.NodeDataOffset = File->Tell();

	ZEMLWriterNode RootNode	= Node.OpenSubNode(Name);
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

	ZEUInt64 StartOffset = ZEEndian::Little(OwnedFile.Tell() + sizeof(ZEUInt64));
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
	OwnedFile.Close();
}

ZEMLWriter::ZEMLWriter()
{
	File = NULL;
}


ZEMLWriter::~ZEMLWriter()
{
	Close();
}
