//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSpinBox4.cpp
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

#include "ZEDSpinBox4.h"
#include "ZEDUndoRedo\ZEDUndoRedo.h"
#include "ZEDPropertyUndoRedo.h"

ZEDSpinBox4::ZEDSpinBox4(QTreeWidget* ParentTree, QTreeWidgetItem *parent, ZEObject* Class, ZEPropertyDescription ClassAttribute) : QTreeWidgetItem(parent)
{
	this->ParentTree = ParentTree;
	this->Class = Class;
	this->ClassAttribute = ClassAttribute;
	ZEVariant Value;
	Class->GetProperty(ClassAttribute.Name, Value);
	//setForeground(0,QBrush(QColor(0,0,0)));
	setText(0, ClassAttribute.Name);
	this->setToolTip (0, QString(ClassAttribute.Description));

	if (Value.GetType() != ZE_VRT_VECTOR4)
	{
		setText(1, QString("Error Vector4"));
		return;
	}

	this->XValue = new ZEDFloatIntLineEdit();
	this->YValue = new ZEDFloatIntLineEdit();
	this->ZValue = new ZEDFloatIntLineEdit();	
	this->WValue = new ZEDFloatIntLineEdit();
	this->AllValue = new ZEDFloatIntLineEdit();

	XValueItem = new QTreeWidgetItem(this);
	YValueItem = new QTreeWidgetItem(this);
	ZValueItem = new QTreeWidgetItem(this);
	WValueItem = new QTreeWidgetItem(this);
	AllValueItem = new QTreeWidgetItem(this);

	ValueCollectionLine = new ZEDFloatIntLineEdit(StringMode);

	addChild(XValueItem);
	addChild(YValueItem);
	addChild(ZValueItem);
	addChild(WValueItem);
	addChild(AllValueItem);

	ParentTree->setItemWidget(XValueItem, 1, XValue);
	ParentTree->setItemWidget(YValueItem, 1, YValue);
	ParentTree->setItemWidget(ZValueItem, 1, ZValue);
	ParentTree->setItemWidget(WValueItem, 1, WValue);
	ParentTree->setItemWidget(AllValueItem, 1, AllValue);

	//XValueItem->setForeground(0,QBrush(QColor(0,0,0)));
	//YValueItem->setForeground(0,QBrush(QColor(0,0,0)));
	//ZValueItem->setForeground(0,QBrush(QColor(0,0,0)));
	//WValueItem->setForeground(0,QBrush(QColor(0,0,0)));
	//AllValueItem->setForeground(0,QBrush(QColor(0,0,0)));

	XValueItem->setText(0, "X");
	YValueItem->setText(0, "Y");
	ZValueItem->setText(0, "Z");
	WValueItem->setText(0, "W");
	AllValueItem->setText(0, "All");

	ParentTree->setItemWidget(this, 1, ValueCollectionLine);

	this->XValue->SetFloat(Value.GetVector4().x);
	this->YValue->SetFloat(Value.GetVector4().y);
	this->ZValue->SetFloat(Value.GetVector4().z);
	this->WValue->SetFloat(Value.GetVector4().w);

	ValueCollectionLine->setText("[" + XValue->text() + " ;  " + YValue->text() + " ;  " + ZValue->text()+ " ;  " + WValue->text() + "]");

	if((this->ClassAttribute.Access & ZE_PA_WRITE) != ZE_PA_WRITE)
	{
		this->ValueCollectionLine->setEnabled(false);
		this->XValue->setEnabled(false);
		this->YValue->setEnabled(false);
		this->ZValue->setEnabled(false);
		this->WValue->setEnabled(false);
		this->AllValue->setEnabled(false);
	}

	connect(this->XValue, SIGNAL(returnPressed()), this, SLOT(Changed()));
	connect(this->YValue, SIGNAL(returnPressed()), this, SLOT(Changed()));
	connect(this->ZValue, SIGNAL(returnPressed()), this, SLOT(Changed()));	
	connect(this->WValue, SIGNAL(returnPressed()), this, SLOT(Changed()));
	connect(this->AllValue, SIGNAL(returnPressed()), this, SLOT(ChangedAll()));
	connect(this->ValueCollectionLine, SIGNAL(returnPressed()), this, SLOT(ChangedByCollection()));
}

ZEDSpinBox4::~ZEDSpinBox4()
{
	//delete ValueCollectionLine;

	//delete XValue;
	//delete YValue;
	//delete ZValue;
	//delete WValue;
	//delete AllValue;

	//delete XValueItem;
	//delete YValueItem;
	//delete ZValueItem;
	//delete WValueItem;
	//delete AllValueItem;
}

void ZEDSpinBox4::UpdateValues()
{

}

void ZEDSpinBox4::Changed()
{
	if(XValue->GetFloat() == YValue->GetFloat() && YValue->GetFloat() == ZValue->GetFloat() && ZValue->GetFloat() == WValue->GetFloat())
		AllValue->SetFloat(XValue->GetFloat());
	else
		AllValue->setText(QString());

	ZEVariant Value;

	ZEDPropertyUndoRedoOperation* TempOperation = new ZEDPropertyUndoRedoOperation(this->Class, this->ClassAttribute);
	Class->GetProperty(ClassAttribute.Name, Value);
	TempOperation->SetOldValue(Value);

	Value.SetVector4(ZEVector4(XValue->GetFloat(), YValue->GetFloat(), ZValue->GetFloat(), WValue->GetFloat()));
	this->Class->SetProperty(ClassAttribute.Name, Value);
	ValueCollectionLine->setText("[" + XValue->text() + " ;  " + YValue->text() + " ;  " + ZValue->text()+ " ;  " + WValue->text() + "]");
	LastValidCollection.x = XValue->GetFloat();
	LastValidCollection.y = YValue->GetFloat();
	LastValidCollection.z = ZValue->GetFloat();
	LastValidCollection.w = WValue->GetFloat();

	Class->GetProperty(ClassAttribute.Name, Value);
	TempOperation->SetNewValue(Value);

	ZEDUndoRedoManagerOld::RegisterOperation(TempOperation);
}

void ZEDSpinBox4::ChangedAll()
{
	XValue->SetFloat(AllValue->GetFloat());
	YValue->SetFloat(AllValue->GetFloat());
	ZValue->SetFloat(AllValue->GetFloat());
	WValue->SetFloat(AllValue->GetFloat());
	Changed();
}

void ZEDSpinBox4::ChangedByCollection()
{
	QString Text = ValueCollectionLine->text();
	Text = Text.remove("]");
	Text = Text.remove("[");
	
	QString X;
	QString Y;
	QString Z;
	QString W;

	int ParsedParameterCount = 0;

	for(int I = 0; I < Text.length(); I++)
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

			if(ParsedParameterCount == 3)
			{
				W.append(Text[I]);
				Text = Text.remove(0,1);
				I--;
			}
		}
	}

	if(ParsedParameterCount != 3)
	{
		XValue->SetFloat(LastValidCollection.x);
		YValue->SetFloat(LastValidCollection.y);
		ZValue->SetFloat(LastValidCollection.z);
		WValue->SetFloat(LastValidCollection.w);
		Changed();
		return;
	}

	if(ParsedParameterCount == 3)
	{
		LastValidCollection.x = X.toFloat();
		LastValidCollection.y = Y.toFloat();
		LastValidCollection.z = Z.toFloat();
		LastValidCollection.w = W.toFloat();
		XValue->SetFloat(X.toFloat());
		YValue->SetFloat(Y.toFloat());
		ZValue->SetFloat(Z.toFloat());
		WValue->SetFloat(W.toFloat());
		Changed();
	}
}
