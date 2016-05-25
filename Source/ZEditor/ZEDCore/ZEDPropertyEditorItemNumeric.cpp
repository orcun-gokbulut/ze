//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDPropertyEditorItemNumeric.cpp
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

#include "ZEDPropertyEditorItemNumeric.h"

#include "ZEDS/ZEVariant.h"
#include "ZEMeta/ZEProperty.h"
#include "ZEDPropertyEditor.h"
#include "ZEDObjectWrapper.h"

#include "QEvent.h"
#include <QLineEdit>
#include <QGuiApplication>

bool ZEDPropertyEditorItemNumeric::InitializeSelf()
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
		Property->Type.Type != ZE_TT_UNSIGNED_INTEGER_64 &&
		Property->Type.Type != ZE_TT_FLOAT &&
		Property->Type.Type != ZE_TT_DOUBLE)
	{
		setText(1, "Type Error");
		return false;
	}

	TextEdit = new QLineEdit();
	TextEdit->setFrame(false);
	TextEdit->setStyleSheet("* { background-color: rgba(0, 0, 0, 0); }");
	TextEdit->setReadOnly(Property->Access == ZEMT_PA_READ);
	treeWidget()->setItemWidget(this, 1, TextEdit);

	TextEdit->installEventFilter(this);

	connect(TextEdit, SIGNAL(textChanged(const QString&)), this, SLOT(TextEdit_textChanged(const QString&)));
	connect(TextEdit, SIGNAL(editingFinished()), this, SLOT(TextEdit_editingFinished()));

	return true;
}

bool ZEDPropertyEditorItemNumeric::eventFilter(QObject* Object, QEvent* Event)
{
	if(static_cast<QLineEdit*>(Object) == TextEdit && Event->type() == QEvent::KeyPress && static_cast<QKeyEvent*>(Event)->key() == Qt::Key_Escape)
		Update();

	return false;
}

void ZEDPropertyEditorItemNumeric::TextEdit_textChanged(const QString& Text)
{
	Value.SetUndefined();
	switch(GetProperty()->Type.Type)
	{
		default:
		case ZE_TT_INTEGER_8:
		{
			bool Result = false;
			ZEInt Integer = Text.toInt(&Result);
			if (!Result || Integer < -128 || Integer > 127)
				break;

			Value.SetInt8(Integer);
			break;
		}

		case ZE_TT_INTEGER_16:
		{
			bool Result = false;
			ZEInt Integer = Text.toInt(&Result);
			if (!Result || Integer < -32768 || Integer > 32767)
				break;

			Value.SetInt16(Integer);
			break;
		}

		case ZE_TT_INTEGER_32:
		{
			bool Result = false;
			ZEInt Integer = Text.toInt(&Result);
			if (!Result)
				break;

			Value.SetInt32(Integer);
			break;
		}

		case ZE_TT_INTEGER_64:
		{
			bool Result = false;
			ZEInt Integer = Text.toInt(&Result);
			if (!Result)
				break;

			Value.SetInt64(Integer);
			break;
		}

		case ZE_TT_UNSIGNED_INTEGER_8:
		{
			bool Result = false;
			ZEInt Integer = Text.toUInt(&Result);
			if (!Result || Integer > 255)
				break;

			Value.SetUInt8(Integer);
			break;
		}

		case ZE_TT_UNSIGNED_INTEGER_16:
		{
			bool Result = false;
			ZEInt Integer = Text.toUInt(&Result);
			if (!Result || Integer > 65535)
				break;

			Value.SetUInt16(Integer);
			break;
		}

		case ZE_TT_UNSIGNED_INTEGER_32:
		{
			bool Result = false;
			ZEInt Integer = Text.toUInt(&Result);
			if (!Result)
				break;

			Value.SetUInt32(Integer);
			break;
		}

		case ZE_TT_UNSIGNED_INTEGER_64:
		{
			bool Result = false;
			ZEInt Integer = Text.toUInt(&Result);
			if (!Result)
				break;

			Value.SetUInt64(Integer);
			break;
		}

		case ZE_TT_FLOAT:
		{
			bool Result = false;
			float Float = Text.toFloat(&Result);
			if (!Result)
				break;

			Value.SetFloat(Float);
			break;
		}

		case ZE_TT_DOUBLE:
		{
			bool Result = false;
			double Double = Text.toDouble(&Result);
			if (!Result)
				break;

			Value.SetDouble(Double);
			break;
		}
	}

	if (Value.IsUndefined())
	{
		Error = true;
		TextEdit->setStyleSheet("* { background-color: rgba(255, 200, 210, 255); }");
		return;
	}
	else
	{
		Error = false;
		TextEdit->setStyleSheet("* { background-color: rgba(0, 0, 0, 0); }");
	}
}

void ZEDPropertyEditorItemNumeric::TextEdit_editingFinished()
{
	if (Error)
	{
		Update();
		return;
	}

	if (Value.IsUndefined())
		return;
	
	Changed(Value);
	Update();

	Error = false;
}

void ZEDPropertyEditorItemNumeric::Update()
{
	if (!IsInitialized())
		return;
	
	QSignalBlocker Blocker(TextEdit);

	Value.SetUndefined();
	const ZEArray<ZEDObjectWrapper*> ObjectWrappers = (GetPropertyEditor()->GetWrappers());
	for (ZESize I = 0; I < ObjectWrappers.GetCount(); I++)
	{
		ZEObject* Object = ObjectWrappers[I]->GetObject();
		ZEClass* Class = Object->GetClass();

		ZEVariant CurrentValue;
		if (!Class->GetProperty(Object, GetProperty()->ID, CurrentValue))
		{
			TextEdit->setText("Value Error");
			TextEdit->setEnabled(false);
			return;
		}

		if (Value.IsUndefined())
		{
			Value = CurrentValue;
		}
		else if (CurrentValue.GetInt64() != Value.GetInt64())
		{
			Value.SetUndefined();
			break;
		}
	}

	TextEdit->setEnabled(true);

	if (Value.IsUndefined())
	{
		TextEdit->setText("");
	}
	else
	{
		switch (GetProperty()->Type.Type)
		{
			case ZE_TT_INTEGER_8:
				TextEdit->setText(QString::number(Value.GetInt8()));
				break;

			case ZE_TT_INTEGER_16:
				TextEdit->setText(QString::number(Value.GetInt16()));
				break;

			case ZE_TT_INTEGER_32:
				TextEdit->setText(QString::number(Value.GetInt32()));
				break;

			case ZE_TT_INTEGER_64:
				TextEdit->setText(QString::number(Value.GetInt64()));
				break;

			case ZE_TT_UNSIGNED_INTEGER_8:
				TextEdit->setText(QString::number(Value.GetUInt8()));
				break;

			case ZE_TT_UNSIGNED_INTEGER_16:
				TextEdit->setText(QString::number(Value.GetUInt16()));
				break;

			case ZE_TT_UNSIGNED_INTEGER_32:
				TextEdit->setText(QString::number(Value.GetUInt32()));
				break;

			case ZE_TT_UNSIGNED_INTEGER_64:
				TextEdit->setText(QString::number(Value.GetUInt64()));
				break;

			case ZE_TT_FLOAT:
				TextEdit->setText(QString::number(Value.GetFloat()));
				break;

			case ZE_TT_DOUBLE:
				TextEdit->setText(QString::number(Value.GetDouble()));
				break;

			default:
				TextEdit->setText("Type Error");
				TextEdit->setEnabled(false);
				break;
		}
	}

	TextEdit->setStyleSheet("* { background-color: rgba(0, 0, 0, 0); }");
	Error = false;
}

ZEDPropertyEditorItemNumeric::ZEDPropertyEditorItemNumeric()
{
	TextEdit = NULL;
	Error = false;
}
