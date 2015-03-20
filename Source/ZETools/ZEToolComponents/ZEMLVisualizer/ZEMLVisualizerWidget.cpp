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
#include "ZEML/ZEMLItem.h"
#include "ZEML/ZEMLNode.h"
#include "ZEML/ZEMLProperty.h"
#include "ZEML/ZEMLDataProperty.h"
#include "ZEDS/ZEString.h"
#include "ZEMLVisualizerTreeWidget.h"
#include "ZEFile/ZEFile.h"

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
	delete QtComponents;
	QtComponents = NULL;
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

	Node = new ZEMLNode("Root");
	Node->Read(&File);
	File.Close();

	SetZEMLNode(Node);
}

void ZEMLVisualizerWidget::Refresh()
{
	QtComponents->Tree->clear();
	AddItem(Node);
}

void ZEMLVisualizerWidget::AddItem(ZEMLItem* Item, QTreeWidgetItem* ParentItem)
{	
	QTreeWidgetItem* NewItem = new QTreeWidgetItem();
	NewItem->setTextAlignment(1, Qt::AlignmentFlag::AlignCenter);

	if(Item->GetType() == ZEML_ET_NODE)
	{
		NewItem->setText(0, (ZEString(Item->GetFilePosition()) + " - " +Item->GetName()).ToCString());
		NewItem->setText(1, "node");
		NewItem->setBackgroundColor(0,QColor(230,230,230));
		NewItem->setBackgroundColor(1,QColor(230,230,230));
		NewItem->setBackgroundColor(2,QColor(230,230,230));
		ZEMLNode* CurrentItem = (ZEMLNode*)Item;

		ZESize SubNodeCount = CurrentItem->GetSubNodes().GetCount();
		ZESize PropertyCount = CurrentItem->GetProperties().GetCount();

		for (ZESize I = 0; I < SubNodeCount; I++)
		{
			AddItem((ZEMLItem*)CurrentItem->GetSubNodes()[I], NewItem);
		}
		for (ZESize I = 0; I < PropertyCount; I++)
		{
			AddItem((ZEMLItem*)CurrentItem->GetProperties()[I], NewItem);
		}
	}
	else if(Item->GetType() == ZEML_ET_INLINE_DATA)
	{
		NewItem->setText(0, (ZEString(Item->GetFilePosition()) + " - " +Item->GetName()).ToCString());
		NewItem->setText(1, "DataProperty");
		ZEMLDataProperty* CurrentItem = (ZEMLDataProperty*)Item;
		ZEString DataSize = CurrentItem->GetDataSize();
		NewItem->setText(2, ("Data Size : " + DataSize + " byte(s)").ToCString());
	}
	else
	{
		ZEQuaternion	TempQuat;
		ZEVector2		TempVec2;
		ZEVector3		TempVec3;
		ZEVector4		TempVec4;
		ZEMatrix3x3		TempM3;
		ZEMatrix4x4		TempM4;
		ZEString		TempString = "";

		NewItem->setText(0, (ZEString(Item->GetFilePosition()) + " - " +Item->GetName()).ToCString());
		ZEMLProperty* CurrentItem = (ZEMLProperty*)Item;
		ZEValue Value = CurrentItem->GetValue();
		switch (Value.GetType())
		{
		case ZE_VRT_FLOAT:
			NewItem->setText(1, "float");
			NewItem->setText(2, ZEString(Value.GetFloat()).ToCString());
			break;
		case ZE_VRT_DOUBLE:
			NewItem->setText(1, "double");
			NewItem->setText(2, ZEString(Value.GetDouble()).ToCString());
			break;
		case ZE_VRT_INTEGER_8:
			NewItem->setText(1, "int8");
			NewItem->setText(2, ZEString(Value.GetInt8()).ToCString());
			break;
		case ZE_VRT_INTEGER_16:
			NewItem->setText(1, "int16");
			NewItem->setText(2, ZEString(Value.GetInt16()).ToCString());
			break;
		case ZE_VRT_INTEGER_32:
			NewItem->setText(1, "int32");
			NewItem->setText(2, ZEString(Value.GetInt32()).ToCString());
			break;
		case ZE_VRT_INTEGER_64:
			NewItem->setText(1, "int64");
			NewItem->setText(2, ZEString(Value.GetInt64()).ToCString());
			break;
		case ZE_VRT_UNSIGNED_INTEGER_8:
			NewItem->setText(1, "uint8");
			NewItem->setText(2, ZEString(Value.GetUInt8()).ToCString());
			break;
		case ZE_VRT_UNSIGNED_INTEGER_16:
			NewItem->setText(1, "uint16");
			NewItem->setText(2, ZEString(Value.GetUInt16()).ToCString());
			break;
		case ZE_VRT_UNSIGNED_INTEGER_32:
			NewItem->setText(1, "uint32");
			NewItem->setText(2, ZEString(Value.GetUInt32()).ToCString());
			break;
		case ZE_VRT_UNSIGNED_INTEGER_64:
			NewItem->setText(1, "uint64");
			NewItem->setText(2, ZEString(Value.GetUInt64()).ToCString());
			break;
		case ZE_VRT_BOOLEAN:
			NewItem->setText(1, "bool");
			NewItem->setText(2, ZEString(Value.GetBoolean()).ToCString());
			break;
		case ZE_VRT_STRING:
			NewItem->setText(1, "string");
			NewItem->setText(2, Value.GetString().ToCString());
			break;
		case ZE_VRT_QUATERNION:
			TempQuat = Value.GetQuaternion();
			TempString = "x : " + ZEString(TempQuat.x) + " y : " + ZEString(TempQuat.y) + " z : " +ZEString(TempQuat.z) + " w : " + ZEString(TempQuat.w);
			NewItem->setText(1, "quaternion");
			NewItem->setText(2, TempString.ToCString());
			break;
		case ZE_VRT_VECTOR2:
			TempVec2 = Value.GetVector2();
			TempString = "x : " + ZEString(TempVec2.x) + " y : " + ZEString(TempVec2.y);
			NewItem->setText(1, "vector2");
			NewItem->setText(2, TempString.ToCString());
			break;
		case ZE_VRT_VECTOR3:
			TempVec3 = Value.GetVector3();
			TempString = "x : " + ZEString(TempVec3.x) + " y : " + ZEString(TempVec3.y) + " z : " +ZEString(TempVec3.z);
			NewItem->setText(1, "vector3");
			NewItem->setText(2, TempString.ToCString());
			break;
		case ZE_VRT_VECTOR4:
			TempVec4 = Value.GetVector4();
			TempString = "x : " + ZEString(TempVec4.x) + " y : " + ZEString(TempVec4.y) + " z : " +ZEString(TempVec4.z) + " w : " + ZEString(TempVec4.w);
			NewItem->setText(1, "vector4");
			NewItem->setText(2, TempString.ToCString());
			break;
		case ZE_VRT_MATRIX3X3:
			NewItem->setText(1, "matrix3x3");
			break;
		case ZE_VRT_MATRIX4X4:
			NewItem->setText(1, "matrix4x4");
			break;
		default:
			zeError("Unsupported ZEMLProperty type.");
			break;
		}
	}

	if(ParentItem == NULL)
		QtComponents->Tree->addTopLevelItem(NewItem);
	else
		ParentItem->addChild(NewItem);

	//QtComponents->Tree->expandAll();
}
