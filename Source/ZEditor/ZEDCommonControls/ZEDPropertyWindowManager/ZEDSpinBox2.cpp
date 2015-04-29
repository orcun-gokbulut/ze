//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSpinBox2.cpp
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

#include "ZEDSpinBox2.h"
#include "ZEDUndoRedo\ZEDUndoRedo.h"
#include "ZEDPropertyUndoRedo.h"

ZEDSpinBox2::ZEDSpinBox2(QTreeWidget* ParentTree, QTreeWidgetItem *parent, ZEObject* Object, ZEProperty* Property) : QTreeWidgetItem(parent)
{
	this->ParentTree = ParentTree;
	this->Object = Object;
	this->Property = Property;
	ZEVariant Value;
	Object->GetClass()->GetProperty(Object, Property->Name, Value);
//	setForeground(0,QBrush(QColor(0,0,0)));
	setText(0, Property->Name);
	//this->setToolTip (0, QString(ClassAttribute.Description));

	if (Value.GetType().Type != ZE_TT_VECTOR2)
	{
		setText(1, QString("Error Vector2"));
		return;
	}

	this->XValue = new ZEDFloatIntLineEdit();
	this->YValue = new ZEDFloatIntLineEdit();
	this->AllValue = new ZEDFloatIntLineEdit();

	XValueItem = new QTreeWidgetItem(this);
	YValueItem = new QTreeWidgetItem(this);
	AllValueItem = new QTreeWidgetItem(this);

	ValueCollectionLine = new ZEDFloatIntLineEdit(StringMode);

	addChild(XValueItem);
	addChild(YValueItem);
	addChild(AllValueItem);

	ParentTree->setItemWidget(XValueItem, 1, XValue);
	ParentTree->setItemWidget(YValueItem, 1, YValue);
	ParentTree->setItemWidget(AllValueItem, 1, AllValue);

	//XValueItem->setForeground(0,QBrush(QColor(0,0,0)));
	//YValueItem->setForeground(0,QBrush(QColor(0,0,0)));
	//AllValueItem->setForeground(0,QBrush(QColor(0,0,0)));

	XValueItem->setText(0, "X");
	YValueItem->setText(0, "Y");
	AllValueItem->setText(0, "All");

	ParentTree->setItemWidget(this, 1, ValueCollectionLine);

	this->XValue->SetFloat(Value.GetVector2().x);
	this->YValue->SetFloat(Value.GetVector2().y);

	ValueCollectionLine->setText("[" + XValue->text() + " ;  " + YValue->text() +  "]");

	if((this->Property->Access & ZEMT_PA_WRITE) != ZEMT_PA_WRITE)
	{
		this->ValueCollectionLine->setEnabled(false);
		this->XValue->setEnabled(false);
		this->YValue->setEnabled(false);
		this->AllValue->setEnabled(false);
	}

	connect(this->XValue, SIGNAL(returnPressed()), this, SLOT(Changed()));
	connect(this->YValue, SIGNAL(returnPressed()), this, SLOT(Changed()));	
	connect(this->AllValue, SIGNAL(returnPressed()), this, SLOT(ChangedAll()));
	connect(this->ValueCollectionLine, SIGNAL(returnPressed()), this, SLOT(ChangedByCollection()));
}

ZEDSpinBox2::~ZEDSpinBox2()
{
	//delete ValueCollectionLine;

	//delete XValue;
	//delete YValue;
	//delete AllValue;

	//delete XValueItem;
	//delete YValueItem;
	//delete AllValueItem;
}

void ZEDSpinBox2::UpdateValues()
{

}


void ZEDSpinBox2::Changed()
{
	if(XValue->GetFloat() == YValue->GetFloat())
		AllValue->SetFloat(XValue->GetFloat());
	else
		AllValue->setText(QString());


	ZEVariant Value;

	ZEDPropertyUndoRedoOperation* TempOperation = new ZEDPropertyUndoRedoOperation(this->Object, this->Property);
	Object->GetClass()->GetProperty(Object, Property->Name, Value);
	TempOperation->SetOldValue(Value);

	Value.SetVector2(ZEVector2(XValue->GetFloat(), YValue->GetFloat()));
	Object->GetClass()->SetProperty(Object, Property->Name, Value);
	ValueCollectionLine->setText("[" + XValue->text() + " ;  " + YValue->text() + "]");
	LastValidCollection.x = XValue->GetFloat();
	LastValidCollection.y = YValue->GetFloat();

	Object->GetClass()->GetProperty(Object, Property->Name, Value);
	TempOperation->SetNewValue(Value);

	ZEDUndoRedoManagerOld::RegisterOperation(TempOperation);
}

void ZEDSpinBox2::ChangedAll()
{
	XValue->SetFloat(AllValue->GetFloat());
	YValue->SetFloat(AllValue->GetFloat());
	Changed();
}

void ZEDSpinBox2::ChangedByCollection()
{
	QString Text = ValueCollectionLine->text();
	Text = Text.remove("]");
	Text = Text.remove("[");
	
	QString X;
	QString Y;

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
		}
	}

	if(ParsedParameterCount != 1)
	{
		XValue->SetFloat(LastValidCollection.x);
		YValue->SetFloat(LastValidCollection.y);
		Changed();
		return;
	}

	if(ParsedParameterCount == 1)
	{
		LastValidCollection.x = X.toFloat();
		LastValidCollection.y = Y.toFloat();
		XValue->SetFloat(X.toFloat());
		YValue->SetFloat(Y.toFloat());
		Changed();
	}
}
