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

void ZEMLWriterNode::Write(const char* Name, const ZEValue& Value)
{
	ZEMLProperty Property;
	Property.SetName(Name);
	Property.SetValue(Value);
	Property.WriteSelf(File);
	Size += Property.GetTotalSize();
	SubItemCount++;
}

void ZEMLWriterNode::WriteFloat(const char* Name, float Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteDouble(const char* Name, double Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteInt8(const char* Name, ZEInt8 Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteInt16(const char* Name, ZEInt16 Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteInt32(const char* Name, ZEInt32 Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteInt64(const char* Name, ZEInt64 Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteUInt8(const char* Name, ZEUInt8 Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteUInt16(const char* Name, ZEUInt16 Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteUInt32(const char* Name, ZEUInt32 Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteUInt64(const char* Name, ZEUInt64 Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteBool(const char* Name, bool Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteString(const char* Name, const char* Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteQuaternion(const char* Name, const ZEQuaternion& Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteVector2(const char* Name, const ZEVector2& Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteVector3(const char* Name, const ZEVector3& Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteVector4(const char* Name, const ZEVector4& Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteMatrix3x3(const char* Name, const ZEMatrix3x3& Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteMatrix4x4(const char* Name, const ZEMatrix4x4& Value)
{
	Write(Name, ZEValue(Value));
}

void ZEMLWriterNode::WriteData(const char* Name, void* Data, ZEUInt64 DataSize)
{
	ZEMLDataProperty DataProperty;
	DataProperty.SetName(Name);
	DataProperty.SetData(Data, DataSize, false);
	DataProperty.WriteSelf(File);
	Size += DataProperty.GetTotalSize();
	SubItemCount++;
}

ZEMLWriterNode ZEMLWriterNode::WriteNode(const char* Name)
{
	ZEMLWriterNode NewNode;
	NewNode.Name = Name;
	NewNode.File = File;
	NewNode.ParentNode = this;
	NewNode.ParentNode->SubItemCount++;

	const char Identifier = 'Z';
	if (File->Write(&Identifier, sizeof(char), 1) != 1)
	{
		zeError("Cannot write to ZEML file. File Name: \"%s\".", File->GetPath());
		return ZEMLWriterNode();
	}

	ZEMLItemType Type = ZEML_IT_NODE;
	if (File->Write(&Type, sizeof(ZEUInt8), 1) != 1)
	{
		zeError("Cannot write to ZEML file. File Name: \"%s\".", File->GetPath());
		return ZEMLWriterNode();
	}

	ZEUInt8 NameLength = NewNode.Name.GetSize();
	if (File->Write(&NameLength, sizeof(ZEUInt8), 1) != 1)
	{
		zeError("Cannot write to ZEML file. File Name: \"%s\".", File->GetPath());
		return ZEMLWriterNode();
	}

	if (File->Write(NewNode.Name.GetValue(), sizeof(char) * NameLength, 1) != 1)
	{
		zeError("Cannot write to ZEML file. File Name: \"%s\".", File->GetPath());
		return ZEMLWriterNode();
	}

	NewNode.FileUpdatePosition = File->Tell();

	ZEUInt64 Nodesize = ZEEndian::Little(NewNode.Size);
	if (File->Write(&Nodesize, sizeof(ZEUInt64), 1) != 1)
	{
		zeError("Cannot write to ZEML file. File Name: \"%s\".", File->GetPath());
		return ZEMLWriterNode();
	}

	ZEUInt64 NodeSubItemCount = ZEEndian::Little(NewNode.SubItemCount);
	if (File->Write(&NodeSubItemCount, sizeof(ZEUInt64), 1) != 1)
	{
		zeError("Cannot write to ZEML file. File Name: \"%s\".", File->GetPath());
		return ZEMLWriterNode();
	}

	return NewNode;
}

void ZEMLWriterNode::CloseNode()
{
	ParentNode->Size += Size;
	ParentNode->Size += 4 *	sizeof(char) + sizeof(ZEUInt8) + sizeof(ZEUInt8) + Name.GetSize() + sizeof(ZEUInt64) + sizeof(ZEUInt64);
	
	ZEUInt64 CurrentFilePos = File->Tell();
	if (File->Seek(FileUpdatePosition, ZE_SF_BEGINING))
	{
		zeError("Cannot write to ZEML file. File Name: \"%s\".", File->GetPath());
		return;
	}

	ZEUInt64 NodeSubItemCount = ZEEndian::Little(SubItemCount);
	if (File->Write(&NodeSubItemCount, sizeof(ZEUInt64), 1) != 1)
	{
		zeError("Cannot write to ZEML file. File Name: \"%s\".", File->GetPath());
		return;
	}

	ZEUInt64 NodeSize = ZEEndian::Little(Size);
	if (File->Write(&NodeSize, sizeof(ZEUInt64), 1) != 1)
	{
		zeError("Cannot write to ZEML file. File Name: \"%s\".", File->GetPath());
		return;
	}

	if (File->Seek(CurrentFilePos, ZE_SF_BEGINING))
	{
		zeError("Cannot write to ZEML file. File Name: \"%s\".", File->GetPath());
		return;
	}
}

/************************************************************************/
/*							RootNode	                                */
/************************************************************************/

ZEMLWriterNode ZEMLWriter::WriteRootNode(const char* Name)
{
	ZEMLWriterNode Node;
	Node.File = File;
	ZEMLWriterNode RootNode = Node.WriteNode(Name);
	RootNode.ParentNode = NULL;
	return RootNode;
}


bool ZEMLWriter::Open(ZEFile* File)
{
	Close();

	this->File = File;
	if (!File->Seek(0, ZE_SF_BEGINING))
		return false;
	
	const char Identifer[5] =
	{
		'Z', 'E', 'M', 'L',
		1 // Version
	};

	if (!File->Write(Identifer, 5, 1))
		return false;

	if (!File->Write(Off))
	return true;
}

void ZEMLWriter::Close()
{
}

ZEMLWriter::ZEMLWriter()
{
	File = NULL;
}
