//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDClassBrowser.cpp
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

#include "ZEDClassBrowser.h"

#include "ui_ZEDClassBrowser.h"
#include "ZEDCore/ZEDEditor.h"
#include "ZEDCore/ZEDObjectWrapper.h"
#include "ZEDCore/ZEDSelectionManager.h"
#include "ZEDCore/ZEDObjectManager.h"

#include <QMimeData>
#include <QDrag>
#include <qevent.h>


void ZEDClassBrowser::UpdateUI()
{
	ZEClass* SelectedClass = GetClassTree()->GetSelectedClass();
	if (SelectedClass == NULL)
	{
		Form->btnAdd->setEnabled(false);
		return;
	}

	if (SelectedClass->IsAbstract())
	{
		Form->btnAdd->setEnabled(false);
		return;
	}

	DestinationWrapper = NULL;
	ZEDSelectionManager* SelectionManager = GetEditor()->GetSelectionManager();
	if (SelectionManager->GetSelection().GetCount() != 1)
	{
		if (SelectionManager->GetFocusedObject() != NULL)
			DestinationWrapper = SelectionManager->GetFocusedObject();
		else
			DestinationWrapper = GetEditor()->GetObjectManager()->GetRootWrapper();
	}
	else
	{
		DestinationWrapper = SelectionManager->GetSelection()[0];
	}

	Form->btnAdd->setEnabled(DestinationWrapper != NULL && DestinationWrapper->CheckChildrenClass(SelectedClass));
}

bool ZEDClassBrowser::InitializeSelf()
{
	if (!ZEDComponent::InitializeSelf())
		return false;

	GetEditor()->AddComponent(Form->trwClasses);
	
	UpdateUI();

	return true;
}

void ZEDClassBrowser::SelectionEvent(const ZEDSelectionEvent* Event)
{
	UpdateUI();
}

bool ZEDClassBrowser::eventFilter(QObject* Object, QEvent* Event)
{
	if (Object != Form->trwClasses->viewport())
		return false;

	if (Event->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent* MouseEvent = static_cast<QMouseEvent*>(Event);
		if ((MouseEvent->buttons() & Qt::LeftButton) == 0)
			return false;

		QTreeWidgetItem* Item = Form->trwClasses->itemAt(MouseEvent->pos());
		if (Item == NULL)
			return false;

		DragClass = Form->trwClasses->GetSelectedClass();
		if (DragClass == NULL)
			return false;

		DragStartPos = MouseEvent->pos();

		return false;
	}
	else if (Event->type() == QEvent::MouseMove)
	{
		if (DragClass == NULL)
			return false;

		QMouseEvent* MouseEvent = static_cast<QMouseEvent*>(Event);

		if ((MouseEvent->pos() - DragStartPos).manhattanLength() < QApplication::startDragDistance())
			return false;

		QByteArray ClassPointerData((const char*)&DragClass, sizeof(ZEClass*));
		QMimeData* MimeData = new QMimeData;
		MimeData->setData("application/vnd.zinek.zeclass", ClassPointerData);

		QDrag* Drag = new QDrag(this);
		Drag->setMimeData(MimeData);

		Qt::DropAction Action = Drag->exec();

		DragClass = NULL;

		return false;
	}

	return false;
}

void ZEDClassBrowser::txtSearch_textChanged(const QString& Text)
{
	Form->trwClasses->SetSearchPattern(Text);
}

void ZEDClassBrowser::trwClasses_itemSelectionChanged()
{
	UpdateUI();
}

void ZEDClassBrowser::btnAdd_clicked()
{
	GetEditor()->GetObjectManager()->CreateObject(DestinationWrapper,  GetClassTree()->GetSelectedClass());
}

ZEDClassTree* ZEDClassBrowser::GetClassTree()
{
	return Form->trwClasses;
}

ZEDClassBrowser::ZEDClassBrowser(QWidget* Parent) : QWidget(Parent)
{
	Form = new Ui_ZEDClassBrowser();
	Form->setupUi(this);

	Form->trwClasses->viewport()->installEventFilter(this);
	Form->trwClasses->setMouseTracking(true);

	connect(Form->txtSearch, SIGNAL(textChanged(const QString&)), this, SLOT(txtSearch_textChanged(const QString&)));
	connect(Form->trwClasses, SIGNAL(itemSelectionChanged()), this, SLOT(trwClasses_itemSelectionChanged()));
	connect(Form->btnAdd, SIGNAL(clicked()), this, SLOT(btnAdd_clicked()));

	DragClass = NULL;
}

ZEDClassBrowser::~ZEDClassBrowser()
{
	delete Form;
}
