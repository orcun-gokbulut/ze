//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDPropertyEditorItemInteger.cpp
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

#include "ZEDPropertyEditorItemInteger.h"

#include "ZEDS/ZEVariant.h"
#include "ZEMeta/ZEProperty.h"
#include "ZEDPropertyEditor.h"
#include "ZEDObjectWrapper.h"

#include <QSpinBox>

bool ZEDPropertyEditorItemInteger::InitializeSelf()
{
	if (!ZEDPropertyEditorItem::InitializeSelf())
	{
		setText(1, "Error");
		return false;
	}

	const ZEProperty* Property = GetProperty();

	if (Property->Type.Type != ZE_TT_INTEGER_8&&
		Property->Type.Type != ZE_TT_INTEGER_16	&&
		Property->Type.Type != ZE_TT_INTEGER_32	&&
		Property->Type.Type != ZE_TT_INTEGER_64	&&
		Property->Type.Type != ZE_TT_UNSIGNED_INTEGER_8	&&
		Property->Type.Type != ZE_TT_UNSIGNED_INTEGER_16 &&
		Property->Type.Type != ZE_TT_UNSIGNED_INTEGER_32 &&
		Property->Type.Type != ZE_TT_UNSIGNED_INTEGER_64)
	{
		setText(1, "Type Error");
		return false;
	}

	if (Property->Access == ZEMT_PA_WRITE)
	{
		setText(1, "Write Only");
		return false;
	}

	SpinBox = new QSpinBox();
	treeWidget()->setItemWidget(this, 1, SpinBox);
	
	switch(Property->Type.Type)
	{
		default:
		case ZE_TT_INTEGER_8:
			SpinBox->setMinimum(-128);
			SpinBox->setMaximum(127);
			break;

		case ZE_TT_INTEGER_16:
			SpinBox->setMinimum(-32768);
			SpinBox->setMaximum(32767);
			break;

		case ZE_TT_INTEGER_32:
			SpinBox->setMinimum(-214748364);
			SpinBox->setMaximum(2147483647);
			break;

		case ZE_TT_INTEGER_64:
			SpinBox->setMinimum(-214748364);
			SpinBox->setMaximum(2147483647);
			break;

		case ZE_TT_UNSIGNED_INTEGER_8:
			SpinBox->setMinimum(0);
			SpinBox->setMaximum(255);
			break;

		case ZE_TT_UNSIGNED_INTEGER_16:
			SpinBox->setMinimum(0);
			SpinBox->setMaximum(65535);
			break;

		case ZE_TT_UNSIGNED_INTEGER_32:
			SpinBox->setMinimum(0);
			SpinBox->setMaximum(2147483647);
			break;

		case ZE_TT_UNSIGNED_INTEGER_64:
			SpinBox->setMinimum(0);
			SpinBox->setMaximum(2147483647);
			break;
	}

	Update();

	SpinBox->setReadOnly(Property->Access == ZEMT_PA_READ);
	this->connect(SpinBox, SIGNAL(valueChanged(int)), this, SLOT(SpinBox_valueChanged(int)));

	return true;
}

void ZEDPropertyEditorItemInteger::SpinBox_valueChanged(int)
{
	ZEVariant Value;
	switch(GetProperty()->Type.Type)
	{
		default:
		case ZE_TT_INTEGER_8:
			Value.SetInt8(SpinBox->value());
			break;

		case ZE_TT_INTEGER_16:
			Value.SetInt16(SpinBox->value());
			break;

		case ZE_TT_INTEGER_32:
			Value.SetInt32(SpinBox->value());
			break;

		case ZE_TT_INTEGER_64:
			Value.SetInt64(SpinBox->value());
			break;

		case ZE_TT_UNSIGNED_INTEGER_8:
			Value.SetUInt8(SpinBox->value());
			break;

		case ZE_TT_UNSIGNED_INTEGER_16:
			Value.SetUInt16(SpinBox->value());
			break;

		case ZE_TT_UNSIGNED_INTEGER_32:
			Value.SetInt32(SpinBox->value());
			break;

		case ZE_TT_UNSIGNED_INTEGER_64:
			Value.SetInt64(SpinBox->value());
			break;
	}

	PropertyChanged(Value);
	Update();
}

void ZEDPropertyEditorItemInteger::Update()
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
			SpinBox->setValue(0);
			SpinBox->setSpecialValueText("Error");
			return;
		}

		if (Value.GetType().Type == ZE_TT_UNDEFINED)
		{
			Value = CurrentValue;
		}
		else if (CurrentValue.GetInt64() != Value.GetInt64())
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
			case ZE_TT_INTEGER_8:
				SpinBox->setValue(Value.GetInt8());
				break;

			case ZE_TT_INTEGER_16:
				SpinBox->setValue(Value.GetInt16());
				break;

			case ZE_TT_INTEGER_32:
				SpinBox->setValue(Value.GetInt32());
				break;

			case ZE_TT_INTEGER_64:
				SpinBox->setValue(Value.GetInt64());
				break;

			case ZE_TT_UNSIGNED_INTEGER_8:
				SpinBox->setValue(Value.GetUInt8());
				break;

			case ZE_TT_UNSIGNED_INTEGER_16:
				SpinBox->setValue(Value.GetUInt16());
				break;

			case ZE_TT_UNSIGNED_INTEGER_32:
				SpinBox->setValue(Value.GetUInt32());
				break;

			case ZE_TT_UNSIGNED_INTEGER_64:
				SpinBox->setValue(Value.GetUInt32());

			default:
				SpinBox->setValue(0);
				SpinBox->setSpecialValueText("Error");
				break;
		}
	}
}

ZEDPropertyEditorItemInteger::ZEDPropertyEditorItemInteger()
{

}
