//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSpinBox3.cpp
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

#include "ZEDSpinBox3.h"
#include "ZEDUndoRedo\ZEDUndoRedo.h"
#include "ZEDPropertyUndoRedo.h"

ZEDSpinBox3::ZEDSpinBox3(QTreeWidget* ParentTree, QTreeWidgetItem *parent, ZEObject* Class, ZEPropertyDescription ClassAttribute) : QTreeWidgetItem(parent)
{
	this->ParentTree = ParentTree;
	this->Class = Class;
	this->ClassAttribute = ClassAttribute;
	ZEVariant Value;
	Class->GetProperty(ClassAttribute.Name, Value);
	//setForeground(0,QBrush(QColor(0,0,0)));
	setText(0, ClassAttribute.Name);
	this->setToolTip (0, QString(ClassAttribute.Description));

	if (Value.GetType() != ZE_VRT_VECTOR3)
	{
		setText(1, QString("Error Vector3"));
		return;
	}

	this->XValue = new ZEDFloatIntLineEdit();
	this->YValue = new ZEDFloatIntLineEdit();
	this->ZValue = new ZEDFloatIntLineEdit();	
	this->AllValue = new ZEDFloatIntLineEdit();

	XValueItem = new QTreeWidgetItem(this);
	YValueItem = new QTreeWidgetItem(this);
	ZValueItem = new QTreeWidgetItem(this);
	AllValueItem = new QTreeWidgetItem(this);

	ValueCollectionLine = new ZEDFloatIntLineEdit(StringMode);

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

	XValueItem->setText(0, "X");
	YValueItem->setText(0, "Y");
	ZValueItem->setText(0, "Z");
	AllValueItem->setText(0, "All");

	ParentTree->setItemWidget(this, 1, ValueCollectionLine);

	this->XValue->SetFloat(Value.GetVector3().x);
	this->YValue->SetFloat(Value.GetVector3().y);
	this->ZValue->SetFloat(Value.GetVector3().z);

	ValueCollectionLine->setText("[" + XValue->text() + " ;  " + YValue->text() + " ;  " + ZValue->text() + "]");

	if((this->ClassAttribute.Access & ZE_PA_WRITE) != ZE_PA_WRITE)
	{
		this->ValueCollectionLine->setEnabled(false);
		this->XValue->setEnabled(false);
		this->YValue->setEnabled(false);
		this->ZValue->setEnabled(false);
		this->AllValue->setEnabled(false);
	}

	connect(this->XValue, SIGNAL(returnPressed()), this, SLOT(Changed()));
	connect(this->YValue, SIGNAL(returnPressed()), this, SLOT(Changed()));
	connect(this->ZValue, SIGNAL(returnPressed()), this, SLOT(Changed()));	
	connect(this->AllValue, SIGNAL(returnPressed()), this, SLOT(ChangedAll()));
	connect(this->ValueCollectionLine, SIGNAL(returnPressed()), this, SLOT(ChangedByCollection()));
}

ZEDSpinBox3::~ZEDSpinBox3()
{
	//delete ValueCollectionLine;

	//delete XValue;
	//delete YValue;
	//delete ZValue;
	//delete AllValue;

	//delete XValueItem;
	//delete YValueItem;
	//delete ZValueItem;
	//delete AllValueItem;
}

void ZEDSpinBox3::UpdateValues()
{
	ZEVariant	TempVariant;
	if(Class->GetProperty(ClassAttribute.Name, TempVariant) == false)
		return;

	XValue->SetFloat(TempVariant.GetVector3().x);
	YValue->SetFloat(TempVariant.GetVector3().y);
	ZValue->SetFloat(TempVariant.GetVector3().z);

	ValueCollectionLine->setText("[" + XValue->text() + " ;  " + YValue->text() + " ;  " + ZValue->text() + "]");

	if(XValue->GetFloat() == YValue->GetFloat() && YValue->GetFloat() == ZValue->GetFloat())
		AllValue->SetFloat(XValue->GetFloat());
	else
		AllValue->setText(QString());
}


void ZEDSpinBox3::Changed()
{
	if(XValue->GetFloat() == YValue->GetFloat() && YValue->GetFloat() == ZValue->GetFloat())
		AllValue->SetFloat(XValue->GetFloat());
	else
		AllValue->setText(QString());

	ZEVariant Value;

	ZEDPropertyUndoRedoOperation* TempOperation = new ZEDPropertyUndoRedoOperation(this->Class, this->ClassAttribute);
	Class->GetProperty(ClassAttribute.Name, Value);
	TempOperation->SetOldValue(Value);

	Value.SetVector3(ZEVector3(XValue->GetFloat(), YValue->GetFloat(), ZValue->GetFloat()));
	this->Class->SetProperty(ClassAttribute.Name, Value);
	ValueCollectionLine->setText("[" + XValue->text() + " ;  " + YValue->text() + " ;  " + ZValue->text() + "]");
	LastValidCollection.x = XValue->GetFloat();
	LastValidCollection.y = YValue->GetFloat();
	LastValidCollection.z = ZValue->GetFloat();

	Class->GetProperty(ClassAttribute.Name, Value);
	TempOperation->SetNewValue(Value);

	ZEDUndoRedoManagerOld::RegisterOperation(TempOperation);
}

void ZEDSpinBox3::ChangedAll()
{
	XValue->SetFloat(AllValue->GetFloat());
	YValue->SetFloat(AllValue->GetFloat());
	ZValue->SetFloat(AllValue->GetFloat());
	Changed();
}

void ZEDSpinBox3::ChangedByCollection()
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
