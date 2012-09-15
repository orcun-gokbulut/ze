//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLNode.cpp
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

#include "ZEMLNode.h"
#include "ZEMLProperty.h"
#include "ZEMLDataProperty.h"
#include "ZEFile\ZEFile.h"

ZEMLNode::ZEMLNode()
{
	Properties.Clear();
	SetType(ZEML_IT_NODE);
	NodeSize = 0;
}

ZEMLNode::ZEMLNode(const ZEString& Name)
{
	Properties.Clear();
	SetType(ZEML_IT_NODE);
	SetName(Name);
	NodeSize = 0;
}

ZEMLNode::~ZEMLNode()
{
	for (ZESize I = 0; I < Properties.GetCount(); I++)
		delete Properties[I];

	for (ZESize I = 0; I < SubNodes.GetCount(); I++)
		delete SubNodes[I];

	Properties.Clear();
}

ZEUInt64 ZEMLNode::GetTotalSize()
{
	ZEUInt64 TotalSize = 0;

	for (ZESize I = 0; I < Properties.GetCount(); I++)
	{
		ZEMLItemType CurrentItemType = Properties[I]->GetType();

		if(CurrentItemType == ZEML_IT_OFFSET_DATA)
			TotalSize += ((ZEMLDataProperty*)Properties[I])->GetTotalSize();
		else
			TotalSize += ((ZEMLProperty*)Properties[I])->GetTotalSize();
	}

	for (ZESize I = 0; I < SubNodes.GetCount(); I++)
	{
		TotalSize += sizeof(char) + sizeof(ZEUInt8) + sizeof(ZEUInt8) + SubNodes[I]->GetName().GetSize() + sizeof(ZEUInt64) + sizeof(ZEUInt64);
		TotalSize += ((ZEMLNode*)SubNodes[I])->GetTotalSize();
	}

	NodeSize = TotalSize;
	return TotalSize;
}

void ZEMLNode::AddProperty(ZEMLProperty* Property)
{
	Property->Parent = this;
	Properties.Add(Property);
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const ZEVariant& Value)
{
	ZEMLProperty* Property = new ZEMLProperty(Name, Value);
	Property->Parent = this;
	Properties.Add(Property);
	return Property;
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name)
{
	ZEMLProperty* Property = new ZEMLProperty(Name);
	Property->Parent = this;
	Properties.Add(Property);
	return Property;
}

ZEMLProperty* ZEMLNode::AddProperty()
{
	ZEMLProperty* Property = new ZEMLProperty();
	Property->Parent = this;
	Properties.Add(Property);
	return Property;
}

bool ZEMLNode::RemoveProperty(ZEMLProperty* Property)
{
	ZEMLItem* ToRemove = Property;
	if(Properties.Exists(ToRemove))
	{
		Properties.DeleteValue(ToRemove);
		return true;
	}

	return false;
}

void ZEMLNode::AddProperty(const ZEString& Name, float Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, double Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, ZEInt8 Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, ZEInt16 Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, ZEInt32 Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, ZEInt64 Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, ZEUInt8 Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, ZEUInt16 Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, ZEUInt32 Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, ZEUInt64 Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, bool Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, const ZEString& Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, const char* Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, const ZEQuaternion& Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, const ZEVector2& Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, const ZEVector3& Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, const ZEVector4& Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, const ZEMatrix3x3& Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddProperty(const ZEString& Name, const ZEMatrix4x4& Value)
{
	AddProperty(Name, ZEVariant(Value));
}

void ZEMLNode::AddDataProperty(ZEMLDataProperty* Property)
{
	Property->Parent = this;
	Properties.Add(Property);
}

ZEMLDataProperty* ZEMLNode::AddDataProperty(const ZEString& Name ,void* Data, ZEUInt64 DataSize, bool Cache)
{
	ZEMLDataProperty* DataProperty = new ZEMLDataProperty(Name, Data, DataSize, Cache);
	DataProperty->Parent = this;
	Properties.Add(DataProperty);
	return DataProperty;
}

ZEMLDataProperty* ZEMLNode::AddDataProperty(const ZEString& Name)
{
	ZEMLDataProperty* DataProperty = new ZEMLDataProperty(Name);
	DataProperty->Parent = this;
	Properties.Add(DataProperty);
	return DataProperty;
}

ZEMLDataProperty* ZEMLNode::AddDataProperty()
{
	ZEMLDataProperty* DataProperty = new ZEMLDataProperty();
	DataProperty->Parent = this;
	Properties.Add(DataProperty);
	return DataProperty;
}

bool ZEMLNode::RemoveDataProperty(ZEMLDataProperty* Property)
{
	ZEMLItem* ToRemove = Property;
	if(Properties.Exists(ToRemove))
	{
		Properties.DeleteValue(ToRemove);
		return true;
	}

	return false;
}

void ZEMLNode::AddSubNode(ZEMLNode* Node)
{
	Node->Parent = this;
	SubNodes.Append(Node);
}

ZEMLNode* ZEMLNode::AddSubNode(const ZEString& Name)
{
	ZEMLNode* Node = new ZEMLNode(Name);
	Node->Parent = this;
	SubNodes.Append(Node);
	return Node;
}

ZEMLNode* ZEMLNode::AddSubNode()
{
	ZEMLNode* Node = new ZEMLNode();
	Node->Parent = this;
	SubNodes.Append(Node);
	return Node;
}

bool ZEMLNode::RemoveSubNode(ZEMLNode* SubNode)
{
	if(SubNodes.Exists(SubNode))
	{
		SubNodes.Remove(SubNode);
		return true;
	}

	return false;
}

const ZEList<ZEMLNode>& ZEMLNode::GetSubNodes() const
{
	return SubNodes;
}

const ZEArray<ZEMLNode*> ZEMLNode::GetSubNodes(const ZEString& NodeName)
{
	ZEArray<ZEMLNode*> Result;
	Result.Clear();

	for (ZESize I = 0; I < SubNodes.GetCount(); I++)
		if(SubNodes[I]->GetName() == NodeName)
			Result.Add(SubNodes[I]);

	return Result;
}

const ZEArray<ZEMLItem*>& ZEMLNode::GetProperties() const
{
	return Properties;
}

const ZEArray<ZEMLItem*> ZEMLNode::GetProperties(const ZEString& PropertyName)
{
	ZEArray<ZEMLItem*> Result;
	Result.Clear();

	for (ZESize I = 0; I < Properties.GetCount(); I++)
		if(Properties[I]->GetName() == PropertyName)
			Result.Add(Properties[I]);

	return Result;
}

const ZEMLNode*	ZEMLNode::GetParent()
{
	if(Parent == NULL)
		return NULL;
	else
		return (ZEMLNode*)Parent;
}

void ZEMLNode::WriteToFile(ZEFile* File)
{
	ZEUInt64 TempUInt64;

	char Identifier = 'Z';
	if(File->Write(&Identifier, sizeof(char), 1) != 1)
		zeError("Can not write ZEMLNode identifier to file.");

	if(File->Write(&Type, sizeof(ZEUInt8), 1) != 1)
		zeError("Can not write ZEMLNode type to file.");

	ZEUInt8 NameLength = Name.GetSize();
	if(File->Write(&NameLength, sizeof(ZEUInt8), 1) != 1)
		zeError("Can not write ZEMLNode name lenght to file.");

	if(File->Write(Name.GetValue(), sizeof(char) * NameLength, 1) != 1)
		zeError("Can not write ZEMLNode name to file.");

	ZEUInt64 SubItemCount = (ZEUInt64)Properties.GetCount() + SubNodes.GetCount();
	TempUInt64 = ZEEndian::Little(SubItemCount);
	if(File->Write(&TempUInt64, sizeof(ZEUInt64), 1) != 1)
		zeError("Can not write ZEMLNode subitem count to file.");

	TempUInt64 = ZEEndian::Little(NodeSize);
	if(File->Write(&TempUInt64, sizeof(ZEUInt64), 1) != 1)
		zeError("Can not write ZEMLNode node size to file.");

	for (ZESize I = 0; I < Properties.GetCount(); I++)
	{
		ZEMLItemType CurrentItemType = Properties[I]->GetType();

		if(CurrentItemType == ZEML_IT_OFFSET_DATA)
			((ZEMLDataProperty*)Properties[I])->WriteToFile(File);

		else
			((ZEMLProperty*)Properties[I])->WriteToFile(File);
	}

	for (ZESize I = 0; I < SubNodes.GetCount(); I++)
		SubNodes[I]->WriteToFile(File);
}

void ZEMLNode::Write(ZEFile* File)
{
	GetTotalSize();
	WriteToFile(File);
}

void ZEMLNode::ReadFromFile(ZEFile* File, bool DeferredDataReading)
{
	char		Identifier;	
	ZEUInt8		NameSize;
	char		TempNameBuffer[ZEML_MAX_NAME_SIZE];	
	ZEUInt64	SubItemCount;

	if(File->Read(&Identifier, sizeof(char), 1) != 1)
		zeError("Can not read ZEMLNode identifier from file. Corrupted ZEML file.");

	if(Identifier != ZEML_ITEM_FILE_IDENTIFIER)
		zeError("ZEMLNode identifier mismatch. Corrupted ZEML file.");

	if(File->Read(&Type, sizeof(ZEUInt8), 1) != 1)
		zeError("Can not read ZEMLNode type from file. Corrupted ZEML file.");

	if(File->Read(&NameSize, sizeof(ZEUInt8), 1) != 1)
		zeError("Can not read ZEMLNode name length from file. Corrupted ZEML file.");

	if(File->Read(TempNameBuffer, NameSize, 1) != 1)
		zeError("Can not read ZEMLDataProperty name from file. Corrupted ZEML file.");

	if(File->Read(&SubItemCount, sizeof(ZEUInt64), 1) != 1)
		zeError("Can not read ZEMLNode sub item count from file. Corrupted ZEML file.");

	SubItemCount = ZEEndian::Little(SubItemCount);

	if(File->Read(&NodeSize, sizeof(ZEUInt64), 1) != 1)
		zeError("Can not read ZEMLNode node size from file. Corrupted ZEML file.");

	NodeSize = ZEEndian::Little(NodeSize);

	for(ZESize I = 0; I < SubItemCount; I++)
	{
		ZEUInt8 SubItemType;
		File->Read(&SubItemType, sizeof(ZEUInt8), 1);
		File->Read(&SubItemType, sizeof(ZEUInt8), 1);
		File->Seek((ZEInt64)sizeof(ZEUInt8) * -2, ZE_SF_CURRENT);

		if((ZEMLItemType)SubItemType == ZEML_IT_NODE)
		{
			ZEMLNode* NewNode = new ZEMLNode();
			NewNode->ReadFromFile(File, DeferredDataReading);
			NewNode->Parent = this;
			SubNodes.Append(NewNode);
		}
		else if((ZEMLItemType)SubItemType == ZEML_IT_INLINE_DATA)
		{
			ZEMLDataProperty* NewDataProperty = new ZEMLDataProperty();
			NewDataProperty->ReadFromFile(File, DeferredDataReading);
			NewDataProperty->Parent = this;
			Properties.Add(NewDataProperty);
		}
		else
		{
			ZEMLProperty* NewProperty = new ZEMLProperty();
			NewProperty->ReadFromFile(File, DeferredDataReading);
			NewProperty->Parent = this;
			Properties.Add(NewProperty);
		}
	}

	SetName(TempNameBuffer);
}

void ZEMLNode::Read(ZEFile* File, bool DeferredDataReading)
{
	ReadFromFile(File, DeferredDataReading);
}
