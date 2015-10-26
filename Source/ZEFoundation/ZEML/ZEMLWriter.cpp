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
#include "ZEMLData.h"
#include "ZEEndian.h"

bool ZEMLWriterNode::WriteValue(const char* Name, const ZEValue& Value)
{
	ZEMLValueType ValueType = ZEMLUtils::ConvertValueType(Value.GetType());

	ZEMLFormatElement Element;
	Element.Name = Name;
	Element.ElementType = ZEML_ET_PROPERTY;
	Element.ValueType = ZEMLUtils::ConvertValueType(Value.GetType());
	Element.Value = Value;
	
	if (!Format->WriteElement(File, Element))
	{
		zeError("Cannot write ZEML data. File Name: \"%s\", Element Name: \"%s\".", File->GetPath().ToCString(), Name);
		return false;
	}
	
	Node.Count++;

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
	ZEMLFormatElement Element;
	Element.Name = Name;
	Element.ElementType = ZEML_ET_DATA;
	Element.Size = Size;
	Element.Value = (ZEUInt64)Data;

	Node.Count++;

	if (!Format->WriteElement(File, Element))
	{
		zeError("Cannot write ZEML data. File Name: \"%s\", Element Name: \"%s\".", File->GetPath().ToCString(), Name);
		return false;
	}

	return true;
}

bool ZEMLWriterNode::OpenNode(const char* Name, ZEMLWriterNode& NewNode)
{
	NewNode = ZEMLWriterNode();
	NewNode.Parent = this;
	NewNode.File = File;
	NewNode.Format = Format;
	NewNode.Node.Name = Name;
	NewNode.Node.ElementType = ZEML_ET_NODE;

	if (!Format->WriteElement(File, NewNode.Node))
	{
		zeError("Cannot open ZEML node. File Name: \"%s\", Element Name: \"%s\".", File->GetPath().ToCString(), Name);
		return false;
	}

	Node.Count++;
	SubNodeIsOpen = true;

	return true;
}

bool ZEMLWriterNode::CloseNode()
{
	if (SubNodeIsOpen)
	{
		zeError("You cannot close the node while a child node is still open.");
		return false;
	}

	if (!Format->WriteElementClose(File, Node))
	{
		zeError("Cannot close ZEML node. File Name: \"%s\", Element Name: \"%s\".", File->GetPath().ToCString(), Node.Name.ToCString());
		return false;
	}

	if (Parent != NULL)
		Parent->SubNodeIsOpen = false;

	return true;
}

ZEMLWriterNode::ZEMLWriterNode()
{
	File = NULL;
	Format = NULL;
	Parent = NULL;
	Node.ElementType = ZEML_ET_NODE;
	SubNodeIsOpen = false;
}

ZEMLWriterNode::~ZEMLWriterNode()
{

}


/************************************************************************/
/*							RootNode	                                */
/************************************************************************/

void ZEMLWriter::SetFormat(ZEMLFormat* Format)
{
	this->Format = Format;
}

ZEMLFormat* ZEMLWriter::GetFormat()
{
	return Format;
}

bool ZEMLWriter::OpenRootNode(const char* Name, ZEMLWriterNode& RootNode)
{
	RootNode.File = File;
	RootNode.Format = Format;
	RootNode.Node.Name = Name;
	RootNode.Node.ElementType = ZEML_ET_NODE;
	RootNode.Parent = NULL;

	if (!Format->WriteElement(File, RootNode.Node))
	{
		zeError("Cannot open ZEML file root node. File Name: \"%s\"", File->GetPath().ToCString());
		return false;
	}

	return true;
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

	if (Format == NULL)
		Format = ZEMLFormat::GetFormats()[0]->CreateInstance();

	if ((Format->GetDescription()->GetSupport() & ZEML_FS_WRITE) == 0)
	{
		zeError("Setted format does not support writing. Format Name: \"%s\" File Name: \"%s\"", Format->GetDescription()->GetName(), File->GetPath().ToCString());
		return false;
	}

	this->File = File;
	Format->WriteHeader(File);

	return true;
}

bool ZEMLWriter::Open(ZEFile* File)
{
	Close();

	if (!File->IsOpen())
	{
		zeError("Cannot write ZEML file. File is not open.");
		return false;
	}

	if ((File->GetOpenMode() & ZE_FOM_WRITE) != ZE_FOM_WRITE)
	{
		zeError("Cannot write ZEML file. File open mode does not allow writing. File Name: \"%s\"", File->GetPath().ToCString());
		return false;
	}

	if (Format == NULL)
		Format = ZEMLFormat::GetFormats()[0]->CreateInstance();

	if ((Format->GetDescription()->GetSupport() & ZEML_FS_WRITE) == 0)
	{
		zeError("Selected ZEML format does not support writing. Format Name: \"%s\" File Name: \"%s\"", Format->GetDescription()->GetName(), File->GetPath().ToCString());
		return false;
	}

	this->File = File;
	Format->WriteHeader(File);

	return true;
}

void ZEMLWriter::Close()
{
	if (File != NULL)
		Format->WriteHeaderClose(File);
	OwnedFile.Close();
	File = NULL;
}

ZEMLWriter::ZEMLWriter()
{
	File = NULL;
}

ZEMLWriter::~ZEMLWriter()
{
	Close();
}
