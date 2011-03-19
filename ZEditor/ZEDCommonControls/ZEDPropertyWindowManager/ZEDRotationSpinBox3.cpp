//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDRotationSpinBox3.cpp
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

#include "ZEDRotationSpinBox3.h"
#include "ZEMath\ZEQuaternion.h"
#include "..\ZEDUndoRedo\ZEDUndoRedo.h"
#include "..\ZEDUndoRedo\ZEDPropertyUndoRedo.h"

ZEDRotationSpinBox3::ZEDRotationSpinBox3(QTreeWidget* ParentTree, QTreeWidgetItem *parent, ZEClass* Class, ZEPropertyDescription ClassAttribute) : QTreeWidgetItem(parent)
{
	this->ParentTree = ParentTree;
	this->Class = Class;
	this->ClassAttribute = ClassAttribute;
	ZEVariant Value;
	Class->GetProperty(ClassAttribute.Name, Value);
	//setForeground(0,QBrush(QColor(0,0,0)));
	setText(0, ClassAttribute.Name);
	ZEQuaternion Temp = Value.GetQuaternion();
	this->setToolTip (0, QString(ClassAttribute.Description));

	if (Value.GetType() != ZE_VRT_QUATERNION)
	{
		setText(1, QString("Error Quaternion"));
		return;
	}

	float x,y,z;
	ZEQuaternion::ConvertToEulerAngles(x,y,z,Temp);

	x = x*(180/ZE_PI);
	y = y*(180/ZE_PI);
	z = z*(180/ZE_PI);

	this->XValue = new ZEDFloatIntLineEdit();
	this->XValue->SetFloat(x);

	this->YValue = new ZEDFloatIntLineEdit();
	this->YValue->SetFloat(y);

	this->ZValue = new ZEDFloatIntLineEdit();
	this->ZValue->SetFloat(z);

	this->AllValue = new ZEDFloatIntLineEdit();
	ValueCollectionLine = new ZEDFloatIntLineEdit(StringMode);

	XValueItem = new QTreeWidgetItem(this);
	YValueItem = new QTreeWidgetItem(this);
	ZValueItem = new QTreeWidgetItem(this);
	AllValueItem = new QTreeWidgetItem(this);

	addChild(XValueItem);
	addChild(YValueItem);
	addChild(ZValueItem);
	addChild(AllValueItem);

	ParentTree->setItemWidget(XValueItem, 1, XValue);
	ParentTree->setItemWidget(YValueItem, 1, YValue);
	ParentTree->setItemWidget(ZValueItem, 1, ZValue);
	ParentTree->setItemWidget(AllValueItem, 1, AllValue);
	ParentTree->setItemWidget(this, 1, ValueCollectionLine);

	//XValueItem->setForeground(0,QBrush(QColor(0,0,0)));
	//YValueItem->setForeground(0,QBrush(QColor(0,0,0)));
	//ZValueItem->setForeground(0,QBrush(QColor(0,0,0)));
	//AllValueItem->setForeground(0,QBrush(QColor(0,0,0)));

	XValueItem->setText(0, "X");
	YValueItem->setText(0, "Y");
	ZValueItem->setText(0, "Z");
	AllValueItem->setText(0, "All");

	ValueCollectionLine->setText("[" + XValue->text() + " ;  " + YValue->text() + " ;  " + ZValue->text() + "]");

	if(this->ClassAttribute.Access & ZE_PA_WRITE != ZE_PA_WRITE)
	{
		this->XValue->setEnabled(false);
		this->YValue->setEnabled(false);
		this->ZValue->setEnabled(false);
		this->AllValue->setEnabled(false);
		this->ValueCollectionLine->setEnabled(false);
	}

	connect(this->XValue, SIGNAL(returnPressed()), this, SLOT(Changed()));
	connect(this->YValue, SIGNAL(returnPressed()), this, SLOT(Changed()));
	connect(this->ZValue, SIGNAL(returnPressed()), this, SLOT(Changed()));	
	connect(this->AllValue, SIGNAL(returnPressed()), this, SLOT(ChangedAll()));
	connect(this->ValueCollectionLine, SIGNAL(returnPressed()), this, SLOT(ChangedByCollection()));
}

ZEDRotationSpinBox3::~ZEDRotationSpinBox3()
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

void ZEDRotationSpinBox3::UpdateValues()
{
	float x,y,z;
	ZEQuaternion TempQuarternion;
	ZEVariant	 TempVariant;

	if(Class->GetProperty(ClassAttribute.Name, TempVariant) == false)
		return;

	this->Class->GetProperty(ClassAttribute.Name, TempVariant);
	TempQuarternion = TempVariant.GetQuaternion();
	ZEQuaternion::ConvertToEulerAngles(x, y, z, TempQuarternion);

	x = x*(180/ZE_PI);
	y = y*(180/ZE_PI);
	z = z*(180/ZE_PI);

	XValue->SetFloat(x);
	YValue->SetFloat(y);
	ZValue->SetFloat(z);

	ValueCollectionLine->setText("[" + XValue->text() + " ;  " + YValue->text() + " ;  " + ZValue->text() + "]");

	if(XValue->GetFloat() == YValue->GetFloat() && YValue->GetFloat() == ZValue->GetFloat())
		AllValue->SetFloat(XValue->GetFloat());
	else
		AllValue->setText(QString());
}

void ZEDRotationSpinBox3::Changed()
{
	ZEVariant Value;

	ZEDPropertyUndoRedoOperation* TempOperation = new ZEDPropertyUndoRedoOperation(this->Class, this->ClassAttribute);
	Class->GetProperty(ClassAttribute.Name, Value);
	TempOperation->SetOldValue(Value);

	ZEQuaternion Temp;

	if(XValue->GetFloat() > 360 || XValue->GetFloat() < 0)
		XValue->SetFloat((int)XValue->GetFloat() % 360);

	if(YValue->GetFloat() > 360 || YValue->GetFloat() < 0)
		YValue->SetFloat((int)YValue->GetFloat() % 360);

	if(ZValue->GetFloat() > 360 || ZValue->GetFloat() < 0)
		ZValue->SetFloat((int)ZValue->GetFloat() % 360);

	if(AllValue->text().length() != 0)
		if(AllValue->GetFloat() > 360 || AllValue->GetFloat() < 0)
			AllValue->SetFloat((int)AllValue->GetFloat() % 360);

	ValueCollectionLine->setText("[" + XValue->text() + " ;  " + YValue->text() + " ;  " + ZValue->text() + "]");

	ZEQuaternion::CreateFromEuler(Temp,XValue->GetFloat()*(ZE_PI/180), YValue->GetFloat()*(ZE_PI/180), ZValue->GetFloat()*(ZE_PI/180));
	Value.SetQuaternion(Temp);
	this->Class->SetProperty(ClassAttribute.Name, Value);

	if(XValue->GetFloat() == YValue->GetFloat() && YValue->GetFloat() == ZValue->GetFloat())
		AllValue->SetFloat(XValue->GetFloat());
	else
		AllValue->setText(QString());

	LastValidCollection.x = XValue->GetFloat();
	LastValidCollection.y = YValue->GetFloat();
	LastValidCollection.z = ZValue->GetFloat();

	Class->GetProperty(ClassAttribute.Name, Value);
	TempOperation->SetNewValue(Value);

	ZEDUndoRedoManager::RegisterOperation(TempOperation);
}

void ZEDRotationSpinBox3::ChangedAll()
{
	XValue->SetFloat(AllValue->GetFloat());
	YValue->SetFloat(AllValue->GetFloat());
	ZValue->SetFloat(AllValue->GetFloat());
	Changed();
}

void ZEDRotationSpinBox3::ChangedByCollection()
{
	QString Text = ValueCollectionLine->text();
	Text = Text.remove("]");
	Text = Text.remove("[");
	
	QString X;
	QString Y;
	QString Z;

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
