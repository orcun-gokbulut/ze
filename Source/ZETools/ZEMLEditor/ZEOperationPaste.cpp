//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEOperationPaste.cpp
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

#include "ZEOperationPaste.h"

#include "ZEML/ZEMLNode.h"

#include <QTreeWidgetItem>

bool ZEOperationPaste::PasteToTree(ZEMLElement* Element, ZEMLNode* ParentNode)
{
	QTreeWidgetItem* ElementItem = new QTreeWidgetItem();
	Element->SetUserData(ElementItem);
	ElementItem->setData(0, Qt::UserRole, (ZEUInt64)Element);
	ElementItem->setText(0, Element->GetName().ToCString());

	switch (Element->GetType())
	{
	case ZEML_ET_PROPERTY:
		ElementItem->setText(1, "Property");
		break;

	case ZEML_ET_NODE:
		{
			ElementItem->setText(1, "Node");
			ZEMLNode* Node = (ZEMLNode*)Element;
			ZEList<ZEMLElement>::Iterator Iterator = Node->GetElements().GetConstIterator();

			while (!Iterator.IsEnd())
			{
				ZEMLElement* ChildElement = Iterator.GetItem();
				PasteToTree(ChildElement, Node);
				Iterator++;
			}

		}
		break;

	case ZEML_ET_DATA:
		ElementItem->setText(1, "Data");
		break;

	default:
		ElementItem->setText(1, "Unknown");
		break;
	}

	QTreeWidgetItem* ParentNodeItem = (QTreeWidgetItem*)(ParentNode->GetUserData());
	ParentNodeItem->addChild(ElementItem);

	return true;
}

bool ZEOperationPaste::Apply()
{
	if (RootNode == NULL)
		return false;

	for (ZESize I = 0; I < PastedElements.GetCount(); I++)
	{
		RootNode->AddElement(PastedElements[I].Element);

		PasteToTree(PastedElements[I].Element, PastedElements[I].Parent);
		PastedElements[I].Index = RootNode->GetElements().FindIndex(PastedElements[I].Element);
	}

	return true;
}

bool ZEOperationPaste::Revert()
{
	if (RootNode == NULL)
		return false;

	for (ZESSize I = PastedElements.GetCount() - 1; I >= 0; I--)
	{
		ZEOperationPastedElement& CurrentElement = PastedElements[I];
		((QTreeWidgetItem*)CurrentElement.Parent->GetUserData())->removeChild((QTreeWidgetItem*)CurrentElement.Element->GetUserData());
		CurrentElement.Parent->RemoveElement(CurrentElement.Element);
	}

	return true;
}

ZEOperationPaste::ZEOperationPaste(ZEMLNode* ClipBoard, ZEMLNode* TargetNode)
{
	SetText("Paste");

	if (ClipBoard == NULL || TargetNode == NULL)
		return;

	if (ClipBoard->GetElements().GetCount() == 0)
		return;

	this->ClipBoard = ClipBoard;
	this->RootNode = TargetNode;

	ZEList<ZEMLElement>::Iterator Iterator = ClipBoard->GetElements().GetConstIterator();
	ZESize ItemCount = ClipBoard->GetElements().GetCount();
	PastedElements.SetCount(ItemCount);
	ZESize Counter = 0;

	while (!Iterator.IsEnd())
	{
		ZEMLElement* PasteElement = Iterator.GetItem()->Clone();
		PastedElements[Counter].Element = PasteElement;
		PastedElements[Counter].Parent = RootNode;

		Counter++;
		Iterator++;
	}
}
