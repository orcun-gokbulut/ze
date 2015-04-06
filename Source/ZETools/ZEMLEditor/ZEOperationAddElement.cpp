//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEOperationAddElement.cpp
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

#include "ZEOperationAddElement.h"
#include "ZEML/ZEMLNode.h"
#include "ZEMLEditorWindow.h"

#include <QtGui/QTreeWidgetItem>

bool ZEOperationAddElement::Apply()
{
	ParentNode->AddElement(Element);
	
	QTreeWidgetItem* ElementItem = (QTreeWidgetItem*)Element->GetUserData();
	if (Element->GetUserData() == NULL)
	{
		ElementItem = new QTreeWidgetItem();
		Element->SetUserData(ElementItem);
		ElementItem->setData(0, Qt::UserRole, (ZEUInt64)Element);
		ElementItem->setText(0, Element->GetName().ToCString());
		switch (Element->GetType())
		{
			case ZEML_ET_PROPERTY:
				SetText("Add New Property");
				ElementItem->setText(1, "Property");
				break;

			case ZEML_ET1_NODE:
				SetText("Add New Node");
				ElementItem->setText(1, "Node");
				break;

			case ZEML_ET_DATA:
				SetText("Add New Data");
				ElementItem->setText(1, "Data");
				break;

			default:
				SetText("Add New Element");
				ElementItem->setText(1, "Unknown");
				break;
		}
	}
	
	QTreeWidgetItem* ParentNodeItem = (QTreeWidgetItem*)ParentNode->GetUserData();
	ParentNodeItem->addChild(ElementItem);
	return true;
}

bool ZEOperationAddElement::Revert()
{
	ParentNode->RemoveElement(Element);
	((QTreeWidgetItem*)ParentNode->GetUserData())->removeChild((QTreeWidgetItem*)Element->GetUserData());
	return true;
}

ZEOperationAddElement::ZEOperationAddElement(ZEMLNode* ParentNode, ZEMLElement* Element)
{
	this->ParentNode = ParentNode;
	this->Element = Element;
}

ZEOperationAddElement::~ZEOperationAddElement()
{
	if (GetStatus() == ZED_OS_NOT_DONE)
	{
		delete Element;
		delete (QTreeWidgetItem*)Element->GetUserData();
	}
}
