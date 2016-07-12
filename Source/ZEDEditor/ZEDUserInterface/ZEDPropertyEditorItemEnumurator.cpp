//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDPropertyEditorItemEnumurator.cpp
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

#include "ZEDPropertyEditorItemEnumurator.h"

#include "ZEMeta/ZEProperty.h"
#include "ZEDCore/ZEDObjectWrapper.h"
#include "ZEDPropertyEditor.h"

#include <QComboBox>
#include <QLineEdit>
#include <QPalette>
#include <QGuiApplication>

bool ZEDPropertyEditorItemEnumurator::InitializeInternal()
{
	if (!ZEDPropertyEditorItem::InitializeInternal())
	{
		setText(1, "Error");
		return false;
	}

	const ZEProperty* Property = GetProperty();
	if (Property->Type.Type != ZE_TT_ENUMERATOR)
	{
		setText(1, "Type Error");
		return false;
	}

	ComboBox = new QComboBox();
	ComboBox->setEditable(true);
	ComboBox->setFrame(false);
	ComboBox->setStyleSheet("QComboBox { background-color: rgba(0, 0, 0, 0); }");
	ComboBox->setEnabled(Property->Access == ZEMT_PA_READ);
	treeWidget()->setItemWidget(this, 1, ComboBox);

	if (Property->Access == ZEMT_PA_READ_WRITE)
	{
		ComboBox->lineEdit()->setReadOnly(true);
		ZESize EnumuratorCount = Property->Type.Enumerator->GetItemCount();
		const ZEEnumeratorItem* Enumurators = Property->Type.Enumerator->GetItems();
		for (ZESize I = 0; I < EnumuratorCount; I++)
			ComboBox->addItem(Enumurators[I].Name, Enumurators[I].Value);
	}
	
	connect(ComboBox, SIGNAL(currentTextChanged(const QString&)), this, SLOT(ComboBox_currentTextChanged(const QString&)));

	return true;
}

void ZEDPropertyEditorItemEnumurator::ComboBox_currentTextChanged(const QString& Text)
{
	ZEVariant Value;

	bool Result = false;
	Value.SetEnum(Text.toInt(&Result));

	if (Result)
	{
		QPalette Palette = ComboBox->palette();
		Palette.setColor(QPalette::Window, Qt::red);    
		Palette.setColor(QPalette::WindowText, Qt::white);
		ComboBox->setPalette(Palette);
		return;
	}
	else
	{
		ComboBox->setPalette(QGuiApplication::palette());
	}

	Changed(Value);
	Update();
}

void ZEDPropertyEditorItemEnumurator::Update()
{
	if (!IsInitialized())
		return;

	QSignalBlocker Blocker(ComboBox);

	const ZEArray<ZEDObjectWrapper*> ObjectWrappers = (GetPropertyEditor()->GetWrappers());

	ZEVariant Value;
	for (ZESize I = 0; I < ObjectWrappers.GetCount(); I++)
	{
		ZEObject* Object = ObjectWrappers[I]->GetObject();
		ZEClass* Class = Object->GetClass();

		ZEVariant CurrentValue;
		if (!Class->GetProperty(Object, GetProperty()->ID, CurrentValue))
		{
			ComboBox->setCurrentText("Value Error");
			ComboBox->setEnabled(false);
			return;
		}

		if (Value.IsUndefined())
		{
			Value = CurrentValue;
		}
		else if (CurrentValue.GetEnum() != Value.GetEnum())
		{
			Value.SetUndefined();
			break;
		}
	}

	ComboBox->setEnabled(true);

	if (Value.IsUndefined())
	{
		ComboBox->setCurrentText("");
	}
	else
	{
		bool Found = false;
		ZESize EnumuratorCount = GetProperty()->Type.Enumerator->GetItemCount();
		const ZEEnumeratorItem* Enumurators = GetProperty()->Type.Enumerator->GetItems();
		for (ZESize I = 0; I < EnumuratorCount; I++)
		{
			if (Value.GetEnum() == Enumurators[I].Value)
			{
				ComboBox->setCurrentText(Enumurators[I].Name);
				Found = true;
			}
		}
		
		if (!Found)
			ComboBox->setCurrentText(QString::number(Value.GetEnum()));
	}
}

ZEDPropertyEditorItemEnumurator::ZEDPropertyEditorItemEnumurator()
{

}
