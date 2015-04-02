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
#include "ZEMLData.h"
#include "ZEMLReader.h"
#include "ZEMLWriter.h"

bool ZEMLNode::Read(ZEMLReaderNode* Reader)
{
	Name = Reader->GetName();
	const ZESmartArray<ZEMLReaderProperty>& Properties = Reader->GetProperties();
	for (ZESize I = 0; I < Properties.GetCount(); I++)
	{
		if (Properties[I].Type == ZEML_ET_INLINE_DATA || Properties[I].Type == ZEML_ET_OFFSET_DATA)
		{
			ZEPointer<ZEMLData> NewData = new ZEMLData(Properties[I].Name);
			NewData->Allocate(Properties[I].DataSize);
			if (!Reader->ReadData(Properties[I].Name, const_cast<void*>(NewData->GetData()), NewData->GetSize()))
				continue;

			AddElement(NewData.Transfer());
		}
		else
		{
			ZEPointer<ZEMLProperty> NewProperty = new ZEMLProperty(Properties[I].Name);
			if (NewProperty != NULL)
			{
				NewProperty->SetValue(Properties[I].Value);
				AddElement(NewProperty.Transfer());
			}
		}
	}

	const ZESmartArray<ZEMLReaderSubNode>& Nodes = Reader->GetSubNodes();
	for (ZESize I = 0; I < Nodes.GetCount(); I++)
	{
		ZEPointer<ZEMLNode> NewNode = new ZEMLNode();
		ZEMLReaderNode NewReaderNode = Reader->GetSubNode(Nodes[I].Name, Nodes[I].Index);
		if (!NewNode->Read(&NewReaderNode))
			return false;

		AddElement(NewNode.Transfer());
	}
	return true;
}

bool ZEMLNode::Write(ZEMLWriterNode* WriterNode)
{
	for (ZESize I = 0; I < Elements.GetCount(); I++)
	{
		if (Elements[I]->GetType() == ZEML_ET1_NODE)
		{
			ZEMLNode* Node = static_cast<ZEMLNode*>(Elements[I]);
			ZEMLWriterNode NewWriterNode = WriterNode->OpenSubNode(Node->GetName());
			if (!Node->Write(&NewWriterNode))
				return false;
			NewWriterNode.CloseNode();
		}
		else if (Elements[I]->GetType() == ZEML_ET_PROPERTY)
		{
			ZEMLProperty* Property = static_cast<ZEMLProperty*>(Elements[I]);
			if (!WriterNode->WriteValue(Property->GetName(), Property->GetValue()))
				return false;
		}
		else if (Elements[I]->GetType() == ZEML_ET_DATA)
		{
			ZEMLData* Data = static_cast<ZEMLData*>(Elements[I]);
			if (!WriterNode->WriteData(Data->GetName(), Data->GetData(), Data->GetSize()))
				return false;
		}
		else
		{
			continue;
		}
	}

	return true;
}

ZEMLElementType1 ZEMLNode::GetType()
{
	return ZEML_ET1_NODE;
}

ZESize ZEMLNode::GetSize()
{
	ZESize Size = 1 +			// Identifier
		1 + Name.GetLength() +	// Name
		8 +						// Size
		8;						// Element Count

	ZEList<ZEMLElement>::Iterator Iterator = Elements.GetIterator();
	while(!Iterator.IsEnd())
	{
		Size += Iterator->GetSize();
		Iterator++;
	}

	return Size;
}

const ZEList<ZEMLElement>& ZEMLNode::GetElements()
{
	return Elements;
}

ZEMLElement* ZEMLNode::GetElement(const char* Name, ZEMLElementType1 Type, ZESize Index)
{
	ZEList<ZEMLElement>::Iterator Iterator = Elements.GetIterator();
	while(!Iterator.IsEnd())
	{
		if ((Type == ZEML_ET_ALL || Iterator->GetType() == Type) && Iterator->GetName() == Name)
		{
			if (Index == 0)
				return Iterator;
			else
				Index--;
		}

		Iterator++;
	}

	return NULL;
}

ZEArray<ZEMLElement*> ZEMLNode::GetElements(ZEMLElementType1 Type)
{
	ZEArray<ZEMLElement*> FoundElements;

	ZEList<ZEMLElement>::Iterator Iterator = Elements.GetIterator();
	while(!Iterator.IsEnd())
	{
		if (Type == ZEML_ET_ALL || Iterator->GetType() == Type)
			FoundElements.Add(Iterator.GetItem());

		Iterator++;
	}

	return FoundElements;
}

ZEArray<ZEMLElement*> ZEMLNode::GetElements(const char* Name, ZEMLElementType1 Type)
{
	ZEArray<ZEMLElement*> FoundElements;

	ZEList<ZEMLElement>::Iterator Iterator = Elements.GetIterator();
	while(!Iterator.IsEnd())
	{
		if ((Name == NULL || Iterator->GetName() == Name) && 
			(Type == ZEML_ET_ALL || Iterator->GetType() == Type))
			FoundElements.Add(Iterator.GetItem());

		Iterator++;
	}

	return FoundElements;
}

ZEMLNode* ZEMLNode::GetNode(const char* Name, ZESize Index)
{
	return (ZEMLNode*)GetElement(Name, ZEML_ET1_NODE, Index);
}

ZEArray<ZEMLNode*> ZEMLNode::GetNodes(const char* Name)
{
	ZEArray<ZEMLNode*> FoundElements;

	ZEList<ZEMLElement>::Iterator Iterator = Elements.GetIterator();
	while(!Iterator.IsEnd())
	{
		if ((Name == NULL || Iterator->GetName() == Name) && 
			(Iterator->GetType() == ZEML_ET1_NODE))
			FoundElements.Add((ZEMLNode*)Iterator.GetItem());

		Iterator++;
	}

	return FoundElements;
}

ZEMLProperty* ZEMLNode::GetProperty(const char* Name)
{
	return (ZEMLProperty*)GetElement(Name, ZEML_ET_PROPERTY);
}

ZEMLData* ZEMLNode::GetData(const char* Name)
{
	return (ZEMLData*)GetElement(Name, ZEML_ET_DATA);
}

bool ZEMLNode::AddElement(ZEMLElement* Element)
{
	if (Elements.Exists(Element))
		return true;

	Element->Parent = this;
	Elements.Append(Element);

	return true;
}

bool ZEMLNode::InsertElement(ZESize Index, ZEMLElement* Element)
{
	if (Elements.Exists(Element))
		return true;

	Element->Parent = this;
	Elements.Insert(Index, Element);

	return true;
}

bool ZEMLNode::RemoveElement(ZEMLElement* Element)
{
	Elements.Remove(Element);
	return true;
}

ZEMLNode* ZEMLNode::AddNode(const char* Name)
{
	ZEPointer<ZEMLNode> Element = new ZEMLNode(Name);
	if (!AddElement(Element))
	{
		Element.Release();
		return NULL;
	}
	else
	{
		return Element.Transfer();
	}
}

ZEMLProperty* ZEMLNode::AddProperty(const char* Name)
{
	ZEPointer<ZEMLProperty> Element = new ZEMLProperty(Name);
	if (!AddElement(Element))
	{
		Element.Release();
		return NULL;
	}
	else
	{
		return Element.Transfer();
	}
}

ZEMLData* ZEMLNode::AddData(const char* Name)
{
	ZEPointer<ZEMLData> Element = new ZEMLData(Name);
	if (!AddElement(Element))
	{
		Element.Release();
		return NULL;
	}
	else
	{
		return Element.Transfer();
	}
}

ZEMLNode::ZEMLNode()
{

}

ZEMLNode::ZEMLNode(const char* Name)
{
	SetName(Name);
}

ZEMLNode::~ZEMLNode()
{
	/*for (ZESize I = 0; I < Elements.GetCount(); I++)
		delete Elements[I];*/
}
