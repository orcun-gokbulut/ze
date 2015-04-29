//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLVisualizerWidget.cpp
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

#include "ZEMLVisualizerWidget.h"
#include "QtGui/QWidget"
#include "ui_ZEMLVisualizerWidget.h"
#include "ZETypes.h"
#include "ZEML/ZEMLElement.h"
#include "ZEML/ZEMLNode.h"
#include "ZEML/ZEMLRoot.h"
#include "ZEML/ZEMLProperty.h"
#include "ZEML/ZEMLData.h"
#include "ZEDS/ZEString.h"
#include "ZEMLVisualizerTreeWidget.h"
#include "ZEFile/ZEFile.h"
#include "ZEDS/ZEFormat.h"

ZEMLVisualizerQt::ZEMLVisualizerQt(QWidget* Parent)
{
	QApplicationCreated = false;
	Application = NULL;

	if(QApplication::instance() == NULL)
	{
		ZEInt32	Argc = 0;
		void*	Argv = NULL;
		Application = new QApplication(Argc, NULL);
		QApplicationCreated = true;
	}

	Widget = new QWidget(Parent);
	Form = new Ui::ZEMLVisualizerWidgetUI();
	Form->setupUi(Widget);

	Tree = new ZEMLVisualizerTreeWidget(Widget);
	Widget->layout()->addWidget(Tree);
}

ZEMLVisualizerQt::~ZEMLVisualizerQt()
{
	Widget->hide();
	delete Widget;
	Widget = NULL;

	delete Form;
	Form = NULL;

	if(QApplicationCreated)
	{
		Application->quit();
		delete Application;
		Application = NULL;
	}
}

ZEMLVisualizerWidget::ZEMLVisualizerWidget(QWidget* Parent)
{
	Node = NULL;
	QtComponents = new ZEMLVisualizerQt(Parent);
}

ZEMLVisualizerWidget::~ZEMLVisualizerWidget()
{
	if (QtComponents != NULL)
	{
		delete QtComponents;
		QtComponents = NULL;
	}

	if (Node != NULL)
	{
		delete Node;
		Node = NULL;
	}

	if (Root != NULL)
	{
		delete Root;
		Root = NULL;
	}
}

void ZEMLVisualizerWidget::Show()
{
	QtComponents->Widget->show();
}

void ZEMLVisualizerWidget::Hide()
{
	QtComponents->Widget->hide();
}

void ZEMLVisualizerWidget::SetZEMLNode(ZEMLNode* Node)
{
	if(this->Node != NULL)
	{
		delete this->Node;
		this->Node = NULL;
	}

	this->Node = Node;
	Refresh();
}

void ZEMLVisualizerWidget::SetZEMLFile(const ZEString& FileName)
{
	ZEFile File;

	if(!File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE))
		zeError("Can not open given file. File name : %s", FileName.ToCString());

	Root = new ZEMLRoot();
	Node = new ZEMLNode("Root");
	Root->SetRootNode(Node);
	Root->Read(&File);
	File.Close();

	SetZEMLNode(Node);
}

void ZEMLVisualizerWidget::Refresh()
{
	QtComponents->Tree->clear();
	AddItem(Node);
}

void ZEMLVisualizerWidget::AddItem(ZEMLElement* Item, QTreeWidgetItem* ParentItem)
{	
	QTreeWidgetItem* NewItem = new QTreeWidgetItem();
	NewItem->setTextAlignment(1, Qt::AlignmentFlag::AlignCenter);

	if(Item->GetType() == ZEML_ET_NODE)
	{
		//NewItem->setText(0, (ZEString(Item->GetFilePosition()) + " - " +Item->GetName()).ToCString());
		NewItem->setText(1, "node");
		NewItem->setBackgroundColor(0, QColor(230,230,230));
		NewItem->setBackgroundColor(1, QColor(230,230,230));
		NewItem->setBackgroundColor(2, QColor(230,230,230));
		ZEMLNode* CurrentItem = (ZEMLNode*)Item;
		
		ZEArray<ZEMLElement*> Nodes = CurrentItem->GetElements(ZEML_ET_NODE);
		ZEArray<ZEMLElement*> Properties = CurrentItem->GetElements(ZEML_ET_PROPERTY);
		ZEArray<ZEMLElement*> Data = CurrentItem->GetElements(ZEML_ET_DATA);

		ZESize SubNodeCount = Nodes.GetCount();
		ZESize PropertyCount = Properties.GetCount();
		ZESize DataCount = Data.GetCount();

		for (ZESize I = 0; I < SubNodeCount; I++)
			AddItem(Nodes[I], NewItem);

		for (ZESize I = 0; I < PropertyCount; I++)
			AddItem(Nodes[I], NewItem);
		
		for (ZESize I = 0; I < PropertyCount; I++)
			AddItem(Nodes[I], NewItem);
	}
	else if (Item->GetType() == ZEML_ET_DATA)
	{
		//NewItem->setText(0, (ZEString(Item->GetFilePosition()) + " - " +Item->GetName()).ToCString());
		NewItem->setText(1, "DataProperty");
		ZEMLData* CurrentItem = (ZEMLData*)Item;
		ZEString DataSize = CurrentItem->GetSize();
		NewItem->setText(2, ("Data Size : " + DataSize + " byte(s)").ToCString());
	}
	else if (Item->GetType() == ZEML_ET_PROPERTY)
	{
		//NewItem->setText(0, (ZEString(Item->GetFilePosition()) + " - " +Item->GetName()).ToCString());
		ZEMLProperty* CurrentItem = (ZEMLProperty*)Item;
		ZEValue Value = CurrentItem->GetValue();
		switch (Value.GetType())
		{
			case ZE_VRT_FLOAT:
				NewItem->setText(1, "Float");
				NewItem->setText(2, ZEString(Value.GetFloat()).ToCString());
				break;

			case ZE_VRT_DOUBLE:
				NewItem->setText(1, "Double");
				NewItem->setText(2, ZEString(Value.GetDouble()).ToCString());
				break;

			case ZE_VRT_INTEGER_8:
				NewItem->setText(1, "Int8");
				NewItem->setText(2, ZEString(Value.GetInt8()).ToCString());
				break;

			case ZE_VRT_INTEGER_16:
				NewItem->setText(1, "Int16");
				NewItem->setText(2, ZEString(Value.GetInt16()).ToCString());
				break;
			case ZE_VRT_INTEGER_32:
				NewItem->setText(1, "Int32");
				NewItem->setText(2, ZEString(Value.GetInt32()).ToCString());
				break;

			case ZE_VRT_INTEGER_64:
				NewItem->setText(1, "Int64");
				NewItem->setText(2, ZEString(Value.GetInt64()).ToCString());
				break;
			case ZE_VRT_UNSIGNED_INTEGER_8:
				NewItem->setText(1, "UInt8");
				NewItem->setText(2, ZEString(Value.GetUInt8()).ToCString());
				break;

			case ZE_VRT_UNSIGNED_INTEGER_16:
				NewItem->setText(1, "UInt16");
				NewItem->setText(2, ZEString(Value.GetUInt16()).ToCString());
				break;

			case ZE_VRT_UNSIGNED_INTEGER_32:
				NewItem->setText(1, "UInt32");
				NewItem->setText(2, ZEString(Value.GetUInt32()).ToCString());
				break;

			case ZE_VRT_UNSIGNED_INTEGER_64:
				NewItem->setText(1, "UInt64");
				NewItem->setText(2, ZEString(Value.GetUInt64()).ToCString());
				break;
			case ZE_VRT_BOOLEAN:
				NewItem->setText(1, "Boolean");
				NewItem->setText(2, ZEString(Value.GetBoolean()).ToCString());
				break;

			case ZE_VRT_STRING:
				NewItem->setText(1, "String");
				NewItem->setText(2, Value.GetString().ToCString());
				break;

			case ZE_VRT_QUATERNION:
				NewItem->setText(1, "Quaternion");
				NewItem->setText(2, ZEFormat::Format("<w:{0}, x:{1}, y:{2}, z:{3}>", Value.GetQuaternion().w, Value.GetQuaternion().x, Value.GetQuaternion().y, Value.GetQuaternion().z).ToCString());
				break;

			case ZE_VRT_VECTOR2:
				NewItem->setText(1, "vector2");
				NewItem->setText(2, ZEFormat::Format("<x:{1}, y:{2}}>", Value.GetVector2().x, Value.GetVector2().y).ToCString());
				break;

			case ZE_VRT_VECTOR3:
				NewItem->setText(1, "vector3");
				NewItem->setText(2, ZEFormat::Format("<x:{1}, y:{2}, z:{3}>", Value.GetVector3().x, Value.GetVector3().y, Value.GetVector3().z).ToCString());
				break;

			case ZE_VRT_VECTOR4:
				NewItem->setText(1, "vector3");
				NewItem->setText(2, ZEFormat::Format("<x:{1}, y:{2}, z:{3}, w:{3}>", Value.GetVector4().x, Value.GetVector4().y, Value.GetVector4().z, Value.GetVector4().w).ToCString());
				break;

			case ZE_VRT_MATRIX3X3:
				NewItem->setText(1, "Matrix3x3");
				break;

			case ZE_VRT_MATRIX4X4:
				NewItem->setText(1, "Matrix4x4");
				break;

			default:
				NewItem->setText(1, "Unknown Type");
				break;
		}
	}
	else
	{
		NewItem->setText(1, "Unknown ZEML Element Type");
		NewItem->setText(2, ZEFormat::Format("Code: 0x{0:x:2} ", Item->GetType()).ToCString());
	}

	if(ParentItem == NULL)
		QtComponents->Tree->addTopLevelItem(NewItem);
	else
		ParentItem->addChild(NewItem);

	//QtComponents->Tree->expandAll();
}
