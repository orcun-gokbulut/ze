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
#include "ZEFile/ZEFile.h"

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
	{
		delete Properties[I];
		Properties[I] = NULL;
	}

// 	for (ZESize I = 0; I < SubNodes.GetCount(); I++)
// 		delete SubNodes[I];

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

bool ZEMLNode::AddProperty(ZEMLProperty* Property)
{
	for (ZESize I = 0; I < Properties.GetCount(); I++)
		if(Properties[I]->GetName() == Property->GetName())
		{
			zeError("ZEML node can not contain properties with duplicate name : %s.", Name);
			return false;
		}

	Property->Parent = this;
	Properties.Add(Property);
	return true;
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const ZEVariant& Value)
{
	for (ZESize I = 0; I < Properties.GetCount(); I++)
		if(Properties[I]->GetName() == Name)
		{
			zeError("ZEML node can not contain properties with duplicate name : %s.", Name);
			return NULL;
		}

	ZEMLProperty* Property = new ZEMLProperty(Name, Value);
	Property->Parent = this;
	Properties.Add(Property);
	return Property;
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name)
{
	for (ZESize I = 0; I < Properties.GetCount(); I++)
		if(Properties[I]->GetName() == Name)
		{
			zeError("ZEML node can not contain properties with duplicate name : %s.", Name);
			return NULL;
		}

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

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, float Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, double Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, ZEInt8 Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, ZEInt16 Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, ZEInt32 Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, ZEInt64 Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, ZEUInt8 Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, ZEUInt16 Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, ZEUInt32 Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, ZEUInt64 Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, bool Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const ZEString& Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const char* Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const ZEQuaternion& Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const ZEVector2& Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const ZEVector3& Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const ZEVector4& Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const ZEMatrix3x3& Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

ZEMLProperty* ZEMLNode::AddProperty(const ZEString& Name, const ZEMatrix4x4& Value)
{
	return AddProperty(Name, ZEVariant(Value));
}

bool ZEMLNode::AddDataProperty(ZEMLDataProperty* Property)
{	
	for (ZESize I = 0; I < Properties.GetCount(); I++)
		if(Properties[I]->GetName() == Property->GetName())
		{
			zeError("ZEML node can not contain data properties with duplicate name : %s.", Name);
			return false;
		}

	Property->Parent = this;
	Properties.Add(Property);
	return true;
}

ZEMLDataProperty* ZEMLNode::AddDataProperty(const ZEString& Name ,void* Data, ZEUInt64 DataSize, bool Cache)
{
	for (ZESize I = 0; I < Properties.GetCount(); I++)
		if(Properties[I]->GetName() == Name)
		{
			zeError("ZEML node can not contain data properties with duplicate name : %s.", Name);
			return NULL;
		}

	ZEMLDataProperty* DataProperty = new ZEMLDataProperty(Name, Data, DataSize, Cache);
	DataProperty->Parent = this;
	Properties.Add(DataProperty);
	return DataProperty;
}

ZEMLDataProperty* ZEMLNode::AddDataProperty(const ZEString& Name)
{
	for (ZESize I = 0; I < Properties.GetCount(); I++)
		if(Properties[I]->GetName() == Name)
		{
			zeError("ZEML node can not contain data properties with duplicate name : %s.", Name);
			return NULL;
		}

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

bool ZEMLNode::InsertSubNode(ZEMLNode* Node, ZESize Index)
{
	if(Node == NULL)
	{
		zeError("Node can not be NULL.");
		return false;
	}

	if(Index > SubNodes.GetCount())
	{
		zeError("Can not insert sub node index is out of range. Node name : %s, index : %d", Node->GetName().ToCString(), Index);
		return false;
	}

	Node->Parent = this;
	SubNodes.Insert(Index, Node);
	return true;
}

ZEMLNode* ZEMLNode::InsertSubNode(const ZEString& Name, ZESize Index)
{
	ZEMLNode* Node = new ZEMLNode(Name);
	Node->Parent = this;
	SubNodes.Insert(Index, Node);
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

const ZEMLItem*	ZEMLNode::GetProperty(const ZEString& PropertyName)
{
	for (ZESize I = 0; I < Properties.GetCount(); I++)
		if(Properties[I]->GetName() == PropertyName)
			return Properties[I];

	return NULL;
}

const ZEMLNode*	ZEMLNode::GetParent()
{
	if(Parent == NULL)
		return NULL;
	else
		return (ZEMLNode*)Parent;
}

bool ZEMLNode::WriteSelf(ZEFile* File)
{
	ZEUInt64 TempUInt64;

	char Identifier = 'Z';
	if(File->Write(&Identifier, sizeof(char), 1) != 1)
	{
		zeError("Can not write ZEMLNode identifier to file.");
		return false;
	}

	if(File->Write(&Type, sizeof(ZEUInt8), 1) != 1)
	{
		zeError("Can not write ZEMLNode type to file.");
		return false;
	}

	ZEUInt8 NameLength = Name.GetSize();
	if(File->Write(&NameLength, sizeof(ZEUInt8), 1) != 1)
	{
		zeError("Can not write ZEMLNode name lenght to file.");
		return false;
	}

	if(File->Write(Name.GetValue(), sizeof(char) * NameLength, 1) != 1)
	{
		zeError("Can not write ZEMLNode name to file.");
		return false;
	}

	ZEUInt64 SubItemCount = (ZEUInt64)Properties.GetCount() + SubNodes.GetCount();
	TempUInt64 = ZEEndian::Little(SubItemCount);
	if(File->Write(&TempUInt64, sizeof(ZEUInt64), 1) != 1)
	{
		zeError("Can not write ZEMLNode subitem count to file.");
		return false;
	}

	TempUInt64 = ZEEndian::Little(NodeSize);
	if(File->Write(&TempUInt64, sizeof(ZEUInt64), 1) != 1)
	{
		zeError("Can not write ZEMLNode node size to file.");
		return false;
	}

	for (ZESize I = 0; I < Properties.GetCount(); I++)
	{
		ZEMLItemType CurrentItemType = Properties[I]->GetType();

		if(CurrentItemType == ZEML_IT_OFFSET_DATA)
		{
			if(!((ZEMLDataProperty*)Properties[I])->WriteSelf(File))
			{
				zeError("Can not write data property, name : %s", Properties[I]->GetName());
				return false;
			}
		}

		else
		{
			if(!((ZEMLProperty*)Properties[I])->WriteSelf(File))
			{
				zeError("Can not write property, name : %s", Properties[I]->GetName());
				return false;
			}
		}
	}

	for (ZESize I = 0; I < SubNodes.GetCount(); I++)
	{
		if(!SubNodes[I]->WriteSelf(File))
		{
			zeError("Can not write sub node, sub node name : %s", SubNodes[I]->GetName().ToCString());
			return false;
		}
	}

	return true;
}

bool ZEMLNode::Write(ZEFile* File)
{
	GetTotalSize();
	return WriteSelf(File);
}

bool ZEMLNode::ReadSelf(ZEFile* File, bool DeferredDataReading)
{
	char		Identifier;	
	ZEUInt8		NameSize;
	char		TempNameBuffer[ZEML_MAX_NAME_SIZE];	
	ZEUInt64	SubItemCount;
	FilePosition = File->Tell();

	if(File->Read(&Identifier, sizeof(char), 1) != 1)
	{
		zeError("Can not read ZEMLNode identifier from file. Corrupted ZEML file.");
		return false;
	}

	if(Identifier != ZEML_ITEM_FILE_IDENTIFIER)
	{
		zeError("ZEMLNode identifier mismatch. Corrupted ZEML file.");
		return false;
	}

	if(File->Read(&Type, sizeof(ZEUInt8), 1) != 1)
	{
		zeError("Can not read ZEMLNode type from file. Corrupted ZEML file.");
		return false;
	}

	if(File->Read(&NameSize, sizeof(ZEUInt8), 1) != 1)
	{
		zeError("Can not read ZEMLNode name length from file. Corrupted ZEML file.");
		return false;
	}

	if(File->Read(TempNameBuffer, NameSize, 1) != 1)
	{
		zeError("Can not read ZEMLDataProperty name from file. Corrupted ZEML file.");
		return false;
	}

	if(File->Read(&SubItemCount, sizeof(ZEUInt64), 1) != 1)
	{
		zeError("Can not read ZEMLNode sub item count from file. Corrupted ZEML file.");
		return false;
	}

	SubItemCount = ZEEndian::Little(SubItemCount);

	if(File->Read(&NodeSize, sizeof(ZEUInt64), 1) != 1)
	{
		zeError("Can not read ZEMLNode node size from file. Corrupted ZEML file.");
		return false;
	}

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
			if(!NewNode->ReadSelf(File, DeferredDataReading))
			{
				zeError("Can not read sub node.");
				return false;
			}
			NewNode->Parent = this;
			SubNodes.Append(NewNode);
		}
		else if((ZEMLItemType)SubItemType == ZEML_IT_INLINE_DATA)
		{
			ZEMLDataProperty* NewDataProperty = new ZEMLDataProperty();
			if(!NewDataProperty->ReadSelf(File, DeferredDataReading))
			{
				zeError("Can not read data property.");
				return false;
			}
			NewDataProperty->Parent = this;
			Properties.Add(NewDataProperty);
		}
		else
		{
			ZEMLProperty* NewProperty = new ZEMLProperty();
			if(!NewProperty->ReadSelf(File, DeferredDataReading))
			{
				zeError("Can not read property.");
				return false;
			}
			NewProperty->Parent = this;
			Properties.Add(NewProperty);
		}
	}

	SetName(TempNameBuffer);
	return true;
}

bool ZEMLNode::Read(ZEFile* File, bool DeferredDataReading)
{
	return ReadSelf(File, DeferredDataReading);
}

bool ZEMLNode::AddItem(ZEMLItem* Item)
{
	if(Item == NULL)
	{
		zeError("Given item for addition is NULL.");
		return false;
	}

	if(Item->GetType() == ZEML_IT_NODE)
	{
		AddSubNode((ZEMLNode*)Item);
		return true;
	}
	else if (Item->GetType() == ZEML_IT_INLINE_DATA)
	{
		return AddDataProperty((ZEMLDataProperty*)Item);
	}	
	else
	{
		return AddProperty((ZEMLProperty*)Item);
	}

	zeError("Can not add item, type is not supported.");
	return false;
}

bool ZEMLNode::RemoveItem(ZEMLItem* Item)
{
	if(Item == NULL)
	{
		zeError("Given item for removal is NULL.");
		return false;
	}

	if(Item->GetType() == ZEML_IT_NODE)
		return RemoveSubNode((ZEMLNode*)Item);
	else if (Item->GetType() == ZEML_IT_INLINE_DATA)
		return RemoveDataProperty((ZEMLDataProperty*)Item);
	else
		return RemoveProperty((ZEMLProperty*)Item);

	zeError("Can not remove item, type is not supported.");
	return false;
}
