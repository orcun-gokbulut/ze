//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDPropertyEditorItemFloat.cpp
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

#pragma once

#include "ZEDPropertyEditorItemFloat.h"

#include "ZEMeta/ZEProperty.h"
#include "ZEDPropertyEditor.h"
#include "ZEDObjectWrapper.h"

#include <QDoubleSpinBox>

bool ZEDPropertyEditorItemFloat::InitializeSelf()
{
	if (!ZEDPropertyEditorItem::InitializeSelf())
	{
		setText(1, "Error");
		return false;
	}

	const ZEProperty* Property = GetProperty();

	if (Property->Type.Type != ZE_TT_FLOAT&&
		Property->Type.Type != ZE_TT_DOUBLE)
	{
		setText(1, "Type Error");
		return false;
	}

	if (Property->Access == ZEMT_PA_WRITE)
	{
		setText(1, "Write Only");
		return false;
	}

	SpinBox = new QDoubleSpinBox();
	treeWidget()->setItemWidget(this, 1, SpinBox);
	
	Update();

	SpinBox->setReadOnly(Property->Access == ZEMT_PA_READ);
	this->connect(SpinBox, SIGNAL(valueChanged(double)), this, SLOT(SpinBox_valueChanged(double)));

	return true;
}

void ZEDPropertyEditorItemFloat::SpinBox_valueChanged(float)
{
	ZEVariant Value;
	switch(GetProperty()->Type.Type)
	{
		default:
		case ZE_TT_FLOAT:
			Value.SetFloat(SpinBox->value());
			break;

		case ZE_TT_DOUBLE:
			Value.SetDouble(SpinBox->value());
			break;
	}

	PropertyChanged(Value);
	Update();
}

void ZEDPropertyEditorItemFloat::Update()
{
	if (GetProperty() == NULL ||GetPropertyEditor() == NULL)
		return;

	QSignalBlocker Blocker(SpinBox);

	const ZEArray<ZEDObjectWrapper*> ObjectWrappers = (GetPropertyEditor()->GetWrappers());
	bool MultipleValue = false;
	ZEVariant Value;
	for (ZESize I = 0; I < ObjectWrappers.GetCount(); I++)
	{
		ZEObject* Object = ObjectWrappers[I]->GetObject();
		ZEClass* Class = Object->GetClass();

		ZEVariant CurrentValue;
		if (!Class->GetProperty(Object, GetProperty()->ID, CurrentValue))
		{
			SpinBox->setValue(0.0f);
			SpinBox->setSpecialValueText("Error");
			return;
		}

		if (Value.GetType().Type == ZE_TT_UNDEFINED)
		{
			Value = CurrentValue;
		}
		else if (CurrentValue.GetDouble() != Value.GetDouble())
		{
			MultipleValue = true;
			break;
		}
	}

	if (MultipleValue)
	{
		SpinBox->setSpecialValueText("");
	}
	else
	{
		switch (GetProperty()->Type.Type)
		{
			case ZE_TT_FLOAT:
				SpinBox->setValue(Value.GetFloat());
				break;

			case ZE_TT_DOUBLE:
				SpinBox->setValue(Value.GetDouble());
				break;

			default:
				SpinBox->setValue(0);
				SpinBox->setSpecialValueText("Error");
				break;
		}
	}
}

ZEDPropertyEditorItemFloat::ZEDPropertyEditorItemFloat()
{

}
