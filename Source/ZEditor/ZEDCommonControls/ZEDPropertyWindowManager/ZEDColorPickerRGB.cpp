//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDColorPickerRGB.cpp
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

#include <QtGui/QColorDialog>
#include "ZEDColorPickerRGB.h"
#include "ZEDUndoRedo\ZEDUndoRedo.h"
#include "ZEDPropertyUndoRedo.h"
#include "ZEDCommonControls/CSS.h"

ZEDColorPickerRGB::ZEDColorPickerRGB(QTreeWidget* ParentTree, QTreeWidgetItem *parent, ZEObject* Object, ZEProperty* Property) : QTreeWidgetItem(parent)
{
	this->ParentTree = ParentTree;
	this->Object = Object;
	this->Property = Property;
	ZEVariant Value;

	Object->GetClass()->GetProperty(Object, Property->Name, Value);
	setText(0, Property->Name);
	//this->setToolTip (0, QString(ClassAttribute.Description));

	if (Value.GetType().Type != ZE_TT_VECTOR3)
	{
		setText(1, QString("Error Color RGB"));
		return;
	}

	this->LayoutWidget = new QWidget();
	//this->LayoutWidget->setStyleSheet(GetCSSFromFile(":/CSS/PropertyWindowTransparentWidget.qss"));
	this->Layout = new QHBoxLayout(LayoutWidget);
	LayoutWidget->setLayout(Layout);
	Layout->setContentsMargins(0,0,0,0);

	this->XValue = new ZEDFloatIntLineEdit();
	this->YValue = new ZEDFloatIntLineEdit();
	this->ZValue = new ZEDFloatIntLineEdit();	
	this->AllValue = new ZEDFloatIntLineEdit();

	XValueItem = new QTreeWidgetItem(this);
	YValueItem = new QTreeWidgetItem(this);
	ZValueItem = new QTreeWidgetItem(this);
	AllValueItem = new QTreeWidgetItem(this);

	ValueCollectionLine = new ZEDFloatIntLineEdit(StringMode);
	Layout->addWidget(ValueCollectionLine);
	ColorButton = new ZEDColorLabel(LayoutWidget, this, ZED_CLT_RGB);
	ColorButton->setFixedWidth(25);
	Layout->addWidget(ColorButton);

	addChild(XValueItem);
	addChild(YValueItem);
	addChild(ZValueItem);
	addChild(AllValueItem);

	ParentTree->setItemWidget(XValueItem, 1, XValue);
	ParentTree->setItemWidget(YValueItem, 1, YValue);
	ParentTree->setItemWidget(ZValueItem, 1, ZValue);
	ParentTree->setItemWidget(AllValueItem, 1, AllValue);

	//XValueItem->setForeground(0,QBrush(QColor(0,0,0)));
	//YValueItem->setForeground(0,QBrush(QColor(0,0,0)));
	//ZValueItem->setForeground(0,QBrush(QColor(0,0,0)));
	//AllValueItem->setForeground(0,QBrush(QColor(0,0,0)));

	XValueItem->setText(0, "R");
	YValueItem->setText(0, "G");
	ZValueItem->setText(0, "B");
	AllValueItem->setText(0, "All");

	ParentTree->setItemWidget(this, 1, LayoutWidget);

	this->XValue->SetFloat(Value.GetVector3().x);
	this->YValue->SetFloat(Value.GetVector3().y);
	this->ZValue->SetFloat(Value.GetVector3().z);

	this->Color = QColor(Value.GetVector3().x, Value.GetVector3().y, Value.GetVector3().z);
	ColorButton->SetColor(QColor(Value.GetVector3().x * 255, Value.GetVector3().y * 255, Value.GetVector3().z * 255));

	ValueCollectionLine->setText("[" + XValue->text() + " ;  " + YValue->text() + " ;  " + ZValue->text() + "]");

	if((this->Property->Access & ZEMT_PA_WRITE) != ZEMT_PA_WRITE)
	{
		this->ValueCollectionLine->setEnabled(false);
		this->XValue->setEnabled(false);
		this->YValue->setEnabled(false);
		this->ZValue->setEnabled(false);
		this->AllValue->setEnabled(false);
		this->ColorButton->setEnabled(false);
	}

	connect(this->XValue, SIGNAL(returnPressed()), this, SLOT(Changed()));
	connect(this->YValue, SIGNAL(returnPressed()), this, SLOT(Changed()));
	connect(this->ZValue, SIGNAL(returnPressed()), this, SLOT(Changed()));	
	connect(this->AllValue, SIGNAL(returnPressed()), this, SLOT(ChangedAll()));
	connect(this->ValueCollectionLine, SIGNAL(returnPressed()), this, SLOT(ChangedByCollection()));	
}

ZEDColorPickerRGB::~ZEDColorPickerRGB()
{
	//delete XValue;
	//delete YValue;
	//delete ZValue;
	//delete AllValue;
	//delete ValueCollectionLine;
	//delete XValueItem;
	//delete YValueItem;
	//delete ZValueItem;
	//delete AllValueItem;
	//delete ColorButton;
	//delete Layout;
	//delete LayoutWidget;
}

void ZEDColorPickerRGB::UpdateValues()
{

}

void ZEDColorPickerRGB::ChangeByDialog()
{
	Color = ColorButton->GetColor();

	if(Color.isValid() == false)
		return;

	ZEVariant Value;

	ZEDPropertyUndoRedoOperation* TempOperation = new ZEDPropertyUndoRedoOperation(this->Object, this->Property);
	Object->GetClass()->GetProperty(Object, Property->Name, Value);
	TempOperation->SetOldValue(Value);

	XValue->SetFloat(Color.red() / 255.0f);
	YValue->SetFloat(Color.green() / 255.0f);
	ZValue->SetFloat(Color.blue() / 255.0f);

	Value.SetVector3(ZEVector3(XValue->GetFloat(), YValue->GetFloat(), ZValue->GetFloat()));
	Object->GetClass()->SetProperty(Object, Property->Name, Value);

	ValueCollectionLine->setText("[" + XValue->text() + " ;  " + YValue->text() + " ;  " + ZValue->text() + "]");

	if(XValue->GetFloat() == YValue->GetFloat() && YValue->GetFloat() == ZValue->GetFloat())
		AllValue->SetFloat(XValue->GetFloat());
	else
		AllValue->setText(QString());

	Object->GetClass()->GetProperty(Object, Property->Name, Value);
	TempOperation->SetNewValue(Value);

	ZEDUndoRedoManagerOld::RegisterOperation(TempOperation);
}

void ZEDColorPickerRGB::Changed()
{
	ZEVariant Value;

	ZEDPropertyUndoRedoOperation* TempOperation = new ZEDPropertyUndoRedoOperation(this->Object, this->Property);
	Object->GetClass()->GetProperty(Object, Property->Name, Value);
	TempOperation->SetOldValue(Value);

	Value.SetVector3(ZEVector3(XValue->GetFloat(), YValue->GetFloat(), ZValue->GetFloat()));
	ValueCollectionLine->setText("[" + XValue->text() + " ;  " + YValue->text() + " ;  " + ZValue->text() + "]");
	Object->GetClass()->SetProperty(Object, Property->Name, Value);
	ColorButton->SetColor(QColor(XValue->GetFloat() * 255, YValue->GetFloat() * 255, ZValue->GetFloat() * 255));
	
	if(XValue->GetFloat() == YValue->GetFloat() && YValue->GetFloat() == ZValue->GetFloat())
		AllValue->SetFloat(XValue->GetFloat());
	else
		AllValue->setText(QString());

	LastValidCollection.x = XValue->GetFloat();
	LastValidCollection.y = YValue->GetFloat();
	LastValidCollection.z = ZValue->GetFloat();

	Object->GetClass()->GetProperty(Object, Property->Name, Value);
	TempOperation->SetNewValue(Value);
	ZEDUndoRedoManagerOld::RegisterOperation(TempOperation);
}

void ZEDColorPickerRGB::ChangedAll()
{
	XValue->SetFloat(AllValue->GetFloat());
	YValue->SetFloat(AllValue->GetFloat());
	ZValue->SetFloat(AllValue->GetFloat());
	Changed();
}

void ZEDColorPickerRGB::ChangedByCollection()
{
	QString Text = ValueCollectionLine->text();
	Text = Text.remove("]");
	Text = Text.remove("[");
	
	QString X;
	QString Y;
	QString Z;

	ZEInt ParsedParameterCount = 0;

	for(ZEInt I = 0; I < Text.length(); I++)
	{
		if(QString(Text[I]) == QString(";"))
			ParsedParameterCount++;

		if(QString(Text[I]) != QString(";"))
		{
			if(ParsedParameterCount == 0)
			{
				X.append(Text[I]);
				Text = Text.remove(0,1);
				I--;
			}
			if(ParsedParameterCount == 1)
			{
				Y.append(Text[I]);
				Text = Text.remove(0,1);
				I--;
			}
			if(ParsedParameterCount == 2)
			{
				Z.append(Text[I]);
				Text = Text.remove(0,1);
				I--;
			}
		}
	}

	if(ParsedParameterCount != 2)
	{
		XValue->SetFloat(LastValidCollection.x);
		YValue->SetFloat(LastValidCollection.y);
		ZValue->SetFloat(LastValidCollection.z);
		Changed();
		return;
	}

	if(ParsedParameterCount == 2)
	{
		LastValidCollection.x = X.toFloat();
		LastValidCollection.y = Y.toFloat();
		LastValidCollection.z = Z.toFloat();
		XValue->SetFloat(X.toFloat());
		YValue->SetFloat(Y.toFloat());
		ZValue->SetFloat(Z.toFloat());
		Changed();
	}
}
