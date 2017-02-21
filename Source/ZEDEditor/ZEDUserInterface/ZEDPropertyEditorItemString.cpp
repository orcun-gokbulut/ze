//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDPropertyEditorItemString.cpp
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

#include "ZEDPropertyEditorItemString.h"

#include "ZEMeta/ZEMTProperty.h"
#include "ZEDPropertyEditor.h"
#include "ZEDCore/ZEDObjectWrapper.h"

#include <QLineEdit>
#include <QEvent.h>
#include <QInputDialog>
#include <QToolButton>
#include <QBoxLayout>
#include "ZEDAssetBrowser.h"

bool ZEDPropertyEditorItemString::InitializeInternal()
{
	if (!ZEDPropertyEditorItem::InitializeInternal())
	{
		setText(1, "Error");
		return false;
	}

	const ZEMTProperty* Property = GetProperty();
	if (Property->Type.Type != ZEMT_BT_STRING)
	{
		setText(1, "Type Error");
		return false;
	}

	QWidget* Container = new QWidget;
	
	TextEdit = new QLineEdit(Container);
	TextEdit->setFrame(false);
	TextEdit->setStyleSheet("* { background-color: rgba(0, 0, 0, 0); }");
	TextEdit->setReadOnly(Property->Access == ZEMT_PA_READ);
	TextEdit->installEventFilter(this);
	connect(TextEdit, SIGNAL(editingFinished()), this, SLOT(TextEdit_editingFinished()));
	connect(TextEdit, SIGNAL(textChanged(const QString&)), this, SLOT(TextEdit_textChanged(const QString&)));

	DetailButton = new QToolButton(Container);
	DetailButton->setText("...");
	DetailButton->setMinimumWidth(2);
	connect(DetailButton, SIGNAL(clicked()), this, SLOT(DetailButton_clicked()));
	
	QHBoxLayout* Layout = new QHBoxLayout();
	Layout->setContentsMargins(0, 0, 0, 0);
	Layout->addWidget(TextEdit);
	Layout->addWidget(DetailButton);
	Container->setLayout(Layout);
	treeWidget()->setItemWidget(this, 1, Container);
	
	return true;
}

bool ZEDPropertyEditorItemString::eventFilter(QObject* Object, QEvent* Event)
{
	if(static_cast<QLineEdit*>(Object) == TextEdit && Event->type() == QEvent::KeyPress && static_cast<QKeyEvent*>(Event)->key() == Qt::Key_Escape)
		Update();

	return false;
}

void ZEDPropertyEditorItemString::TextEdit_textChanged(const QString&)
{
	ValueChanged = true;
}

void ZEDPropertyEditorItemString::TextEdit_editingFinished()
{
	if (!ValueChanged)
		return;

	ZEVariant Value;
	Value.SetString(TextEdit->text().toUtf8().begin());

	Changed(Value);
	Update();
}

void ZEDPropertyEditorItemString::DetailButton_clicked()
{
	if (IsAsset)
	{
		
	}
	else
	{
		bool Result = false;
		QString String = QInputDialog::getMultiLineText(treeWidget(), "Edit Property Value", GetProperty()->Name, TextEdit->text(), &Result);

		if (!Result)
			return;

		ZEVariant Value;
		Value.SetString(String.toUtf8().begin());

		Changed(Value);
		Update();
	}
}

void ZEDPropertyEditorItemString::Update()
{
	if (!IsInitialized())
		return;

	ValueChanged = false;

	QSignalBlocker Blocker(TextEdit);

	const ZEArray<ZEDObjectWrapper*> Wrappers = (GetPropertyEditor()->GetWrappers());
	bool MultipleValue = false;
	ZEVariant Value;
	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		ZEObject* Object = Wrappers[I]->GetObject();
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
		else if (CurrentValue.GetString() != Value.GetString())
		{
			Value.SetUndefined();
			break;
		}
	}

	TextEdit->setEnabled(true);

	if (Value.IsUndefined())
		TextEdit->setText("");
	else
		TextEdit->setText(Value.GetString().ToCString());
}

ZEDPropertyEditorItemString::ZEDPropertyEditorItemString()
{
	ValueChanged = false;
	TextEdit = NULL;
	DetailButton = NULL;
}
