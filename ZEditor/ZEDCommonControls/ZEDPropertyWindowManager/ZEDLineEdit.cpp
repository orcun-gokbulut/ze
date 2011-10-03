//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDLineEdit.cpp
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

#include "ZEDLineEdit.h"
#include "ZEDUndoRedo\ZEDUndoRedo.h"
#include "ZEDPropertyUndoRedo.h"
ZEDLineEdit::ZEDLineEdit(QTreeWidget* ParentTree, QTreeWidgetItem *parent, ZEClass* Class, ZEPropertyDescription ClassAttribute) : QTreeWidgetItem(parent)
{
	this->ParentTree = ParentTree;
	this->Class = Class;
	this->ClassAttribute = ClassAttribute;
	ZEVariant Value;
	Class->GetProperty(ClassAttribute.Name, Value);
	//setForeground(0,QBrush(QColor(0,0,0)));
	setText(0, ClassAttribute.Name);
	this->setToolTip (0, QString(ClassAttribute.Description));

	if (Value.GetType() != ZE_VRT_STRING)
	{
		setText(1, QString("Error String"));
		return;
	}

	XValue = new ZEDFloatIntLineEdit(StringMode);
	XValue->setText(QString(Value.GetString()));

	ParentTree->setItemWidget(this, 1, XValue);

	if((this->ClassAttribute.Access & ZE_PA_WRITE) != ZE_PA_WRITE)
		XValue->setEnabled(false);

	connect(this->XValue, SIGNAL(returnPressed()), this, SLOT(Changed()));
}

ZEDLineEdit::~ZEDLineEdit()
{
	//delete XValue;
}

void ZEDLineEdit::UpdateValues()
{

}

void ZEDLineEdit::Changed()
{
	ZEVariant Value;

	ZEDPropertyUndoRedoOperation* TempOperation = new ZEDPropertyUndoRedoOperation(this->Class, this->ClassAttribute);
	Class->GetProperty(ClassAttribute.Name, Value);
	TempOperation->SetOldValue(Value);

	Value.SetString((const char*)(XValue->text().toLatin1()));
	this->Class->SetProperty(ClassAttribute.Name, Value);

	Class->GetProperty(ClassAttribute.Name, Value);
	TempOperation->SetNewValue(Value);

	ZEDUndoRedoManager::RegisterOperation(TempOperation);
}
