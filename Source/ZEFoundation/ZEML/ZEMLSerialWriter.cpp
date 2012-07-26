//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLSerialWriter.cpp
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

#include "ZEMLSerialWriter.h"
#include "ZEFile\ZEFile.h"
#include "ZEMLProperty.h"
#include "ZEMLDataProperty.h"
#include "ZEEndian.h"

ZEMLSerialNode::ZEMLSerialNode()
{
	File = NULL;
	Name = "";
	ParentNode = NULL;
	SubItemCount = 0;
	Size = 0;
}

ZEMLSerialNode::ZEMLSerialNode(const ZEString& Name, ZEFile* File, ZEMLSerialNode* Parent)
{
	this->File = File;
	this->Name = Name;
	ParentNode = Parent;
	SubItemCount = 0;
	Size = 0;
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, const ZEVariant& Value)
{
	ZEMLProperty Property;
	Property.SetName(Name);
	Property.SetValue(Value);
	Property.WriteToFile(File);
	Size += Property.GetTotalSize();
	SubItemCount++;
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, float Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, double Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, ZEInt8 Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, ZEInt16 Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, ZEInt32 Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, ZEInt64 Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, ZEUInt8 Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, ZEUInt16 Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, ZEUInt32 Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, ZEUInt64 Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, bool Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, const ZEString& Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, const char* Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, const ZEQuaternion& Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, const ZEVector2& Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, const ZEVector3& Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, const ZEVector4& Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, const ZEMatrix3x3& Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteProperty(const ZEString& Name, const ZEMatrix4x4& Value)
{
	WriteProperty(Name, ZEVariant(Value));
}

void ZEMLSerialNode::WriteDataProperty(const ZEString& Name, void* Data, ZEUInt64 DataSize)
{
	ZEMLDataProperty DataProperty;
	DataProperty.SetName(Name);
	DataProperty.SetData(Data, DataSize, false);
	DataProperty.WriteToFile(File);
	ParentNode->Size += DataProperty.GetTotalSize();
	ParentNode->SubItemCount++;
}

ZEMLSerialNode  ZEMLSerialNode::OpenNode(const ZEString& Name)
{
	ZEUInt64 TempUInt64;

	ZEMLSerialNode NewNode;
	NewNode.Name = Name;
	NewNode.File = File;
	NewNode.ParentNode = this;
	NewNode.ParentNode->SubItemCount++;

	char Identifier = 'Z';
	if(File->Write(&Identifier, sizeof(char), 1) != 1)
		zeError("Can not write ZEMLSerialNode identifier to file.");

	ZEMLItemType Type = ZEML_IT_NODE;
	if(File->Write(&Type, sizeof(ZEUInt8), 1) != 1)
		zeError("Can not write ZEMLSerialNode type to file.");

	ZEUInt8 NameLength = NewNode.Name.GetSize();
	if(File->Write(&NameLength, sizeof(ZEUInt8), 1) != 1)
		zeError("Can not write ZEMLSerialNode name lenght to file.");

	if(File->Write(NewNode.Name.GetValue(), sizeof(char) * NameLength, 1) != 1)
		zeError("Can not write ZEMLSerialNode name to file.");

	NewNode.FileUpdatePosition = File->Tell();

	TempUInt64 = ZEEndian::Little(NewNode.SubItemCount);
	if(File->Write(&TempUInt64, sizeof(ZEUInt64), 1) != 1)
		zeError("Can not write ZEMLSerialNode subitem count to file.");

	TempUInt64 = ZEEndian::Little(NewNode.Size);
	if(File->Write(&TempUInt64, sizeof(ZEUInt64), 1) != 1)
		zeError("Can not write ZEMLSerialNode node size to file.");

	return NewNode;
}

void  ZEMLSerialNode::CloseNode()
{
	ZEUInt64 TempUInt64;

	ParentNode->Size += Size;
	ParentNode->Size += 	sizeof(char) + sizeof(ZEUInt8) + sizeof(ZEUInt8) + Name.GetSize() + sizeof(ZEUInt64) + sizeof(ZEUInt64);
	
	ZEUInt64 CurrentFilePos = File->Tell();
	if(!File->Seek(FileUpdatePosition, ZE_SF_BEGINING))
		zeError("Can not seek file.");

	TempUInt64 = ZEEndian::Little(SubItemCount);
	if(File->Write(&TempUInt64, sizeof(ZEUInt64), 1) != 1)
		zeError("Can not update ZEMLSerialNode subitem count to file.");

	TempUInt64 = ZEEndian::Little(Size);
	if(File->Write(&TempUInt64, sizeof(ZEUInt64), 1) != 1)
		zeError("Can not update ZEMLSerialNode node size to file.");

	if(!File->Seek(CurrentFilePos, ZE_SF_BEGINING))
		zeError("Can not seek file.");

}

/************************************************************************/
/*							RootNode	                                */
/************************************************************************/

ZEMLSerialRootNode::ZEMLSerialRootNode()
{

}

ZEMLSerialNode ZEMLSerialRootNode::OpenNode(const ZEString& Name)
{
	ZEUInt64 TempUInt64;

	ZEMLSerialNode NewNode;
	NewNode.Name = Name;
	NewNode.File = File;
	NewNode.ParentNode = this;
	NewNode.ParentNode->SubItemCount++;

	char Identifier = 'Z';
	if(File->Write(&Identifier, sizeof(char), 1) != 1)
		zeError("Can not write ZEMLSerialNode identifier to file.");

	ZEMLItemType Type = ZEML_IT_NODE;
	if(File->Write(&Type, sizeof(ZEUInt8), 1) != 1)
		zeError("Can not write ZEMLSerialNode type to file.");

	ZEUInt8 NameLength = NewNode.Name.GetSize();
	if(File->Write(&NameLength, sizeof(ZEUInt8), 1) != 1)
		zeError("Can not write ZEMLSerialNode name length to file.");

	if(File->Write(NewNode.Name.GetValue(), sizeof(char) * NameLength, 1) != 1)
		zeError("Can not write ZEMLSerialNode name to file.");

	NewNode.FileUpdatePosition = File->Tell();

	TempUInt64 = ZEEndian::Little(NewNode.SubItemCount);
	if(File->Write(&TempUInt64, sizeof(ZEUInt64), 1) != 1)
		zeError("Can not write ZEMLSerialNode subitem count to file.");

	TempUInt64 = ZEEndian::Little(NewNode.Size);
	if(File->Write(&TempUInt64, sizeof(ZEUInt64), 1) != 1)
		zeError("Can not write ZEMLSerialNode node size to file.");

	return NewNode;
}

void ZEMLSerialRootNode::CloseNode()
{
	ZEUInt64 TempUInt64;
	ZEUInt64 CurrentFilePos = File->Tell();

	if(!File->Seek(FileUpdatePosition, ZE_SF_BEGINING))
		zeError("Can not seek file.");

	TempUInt64 = ZEEndian::Little(SubItemCount);
	if(File->Write(&TempUInt64, sizeof(ZEUInt64), 1) != 1)
		zeError("Can not update ZEMLSerialNode subitem count to file.");

	TempUInt64 = ZEEndian::Little(Size);
	if(File->Write(&TempUInt64, sizeof(ZEUInt64), 1) != 1)
		zeError("Can not update ZEMLSerialNode node size to file.");

	if(!File->Seek(CurrentFilePos, ZE_SF_BEGINING))
		zeError("Can not seek file.");
}

ZEMLSerialRootNode::ZEMLSerialRootNode(const ZEString& Name, ZEFile* File)
{
	ZEUInt64 TempUInt64;

	this->Name = Name;
	this->File = File;
	this->ParentNode = NULL;
	SubItemCount = 0;
	Size = 0;

	char Identifier = 'Z';
	if(File->Write(&Identifier, sizeof(char), 1) != 1)
		zeError("Can not write ZEMLSerialRootNode identifier to file.");

	ZEMLItemType Type = ZEML_IT_NODE;
	if(File->Write(&Type, sizeof(ZEUInt8), 1) != 1)
		zeError("Can not write ZEMLSerialRootNode type to file.");

	ZEUInt8 NameLength = this->Name.GetSize();
	if(File->Write(&NameLength, sizeof(ZEUInt8), 1) != 1)
		zeError("Can not write ZEMLSerialRootNode name lenght to file.");

	if(File->Write(this->Name.GetValue(), sizeof(char) * NameLength, 1) != 1)
		zeError("Can not write ZEMLSerialRootNode name to file.");

	FileUpdatePosition = File->Tell();

	TempUInt64 = ZEEndian::Little(SubItemCount);
	if(File->Write(&TempUInt64, sizeof(ZEUInt64), 1) != 1)
		zeError("Can not write ZEMLSerialRootNode subitem count to file.");
	
	TempUInt64 = ZEEndian::Little(Size);
	if(File->Write(&TempUInt64, sizeof(ZEUInt64), 1) != 1)
		zeError("Can not write ZEMLSerialRootNode node size to file.");
}
