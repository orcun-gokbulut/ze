//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDObjectBrowser.cpp
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

#include "ZEDObjectBrowser.h"

#include "ui_ZEDObjectBrowser.h"
#include "ZEDCore\ZEDEditor.h"
#include "ZEDCore\ZEDObjectWrapper.h"
#include "ZEDCore\ZEDObjectManager.h"

#include <QEvent.h>
#include <QDrag>
#include <QMimeData>
#include <QTreeWidget>

bool ZEDObjectBrowser::InitializeSelf()
{
	if (!ZEDComponent::InitializeSelf())
		return false;

	GetEditor()->AddComponent(Form->trwObjects);

	return true;
}

bool ZEDObjectBrowser::eventFilter(QObject* Object, QEvent* Event)
{
	if (Object != Form->trwObjects->viewport())
		return false;

	if (Event->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent* MouseEvent = static_cast<QMouseEvent*>(Event);
		if ((MouseEvent->buttons() & Qt::LeftButton) == 0)
			return false;

		QTreeWidgetItem* Item = Form->trwObjects->itemAt(MouseEvent->pos());
		if (Item == NULL)
			return false;

		DragWrapper = Form->trwObjects->GetWrapper(Item);
		if (DragWrapper == NULL)
			return false;

		DragStartPos = MouseEvent->pos();

		return false;
	}
	else if (Event->type() == QEvent::MouseMove)
	{
		if (DragWrapper == NULL)
			return false;

		QMouseEvent* MouseEvent = static_cast<QMouseEvent*>(Event);

		if ((MouseEvent->pos() - DragStartPos).manhattanLength() < QApplication::startDragDistance())
			return false;

		QMimeData* MimeData = new QMimeData;
		MimeData->setData("application/vnd.zinek.zeobjectwrapper", QByteArray((const char*)&DragWrapper, sizeof(ZEDObjectWrapper)));

		QDrag* Drag = new QDrag(this);
		Drag->setMimeData(MimeData);

		Qt::DropAction Action = Drag->exec();

		return false;
	}
	if (Event->type() == QEvent::DragEnter)
	{
		QDragEnterEvent* DragEvent = static_cast<QDragEnterEvent*>(Event);
		
		if (DragEvent->mimeData()->hasFormat("application/vnd.zinek.zeobjectwrapper") || 
			DragEvent->mimeData()->hasFormat("application/vnd.zinek.zeclass") ||
			DragEvent->mimeData()->hasFormat("application/vnd.zinek.zeasset"))
		{
			DragEvent->acceptProposedAction();
			return true;
		}

		return false;
	}
	else if (Event->type() == QEvent::DragMove)
	{
		QDragMoveEvent* DragEvent = static_cast<QDragMoveEvent*>(Event);

		QTreeWidgetItem* Item = Form->trwObjects->itemAt(DragEvent->pos());
		if (Item == NULL)
			return false;

		ZEDObjectWrapper* TargetWrapper = Form->trwObjects->GetWrapper(Item);
		if (TargetWrapper == NULL)
			return false;

		if (DragEvent->mimeData()->hasFormat("application/vnd.zinek.zeobjectwrapper"))
		{
			ZEDObjectWrapper* ObjectWrapper = *(ZEDObjectWrapper**)DragEvent->mimeData()->data("application/vnd.zinek.zeobjectwrapper").data();
			
			if (ObjectWrapper == TargetWrapper)
				return false;

			if (ObjectWrapper->GetParent() == TargetWrapper)
				return false;

			if (!TargetWrapper->CheckChildrenClass(ObjectWrapper->GetObjectClass()))
				return false;

			DragEvent->acceptProposedAction();
			return true;
		}
		else if (DragEvent->mimeData()->hasFormat("application/vnd.zinek.zeclass"))
		{
			ZEClass* Class = *(ZEClass**)DragEvent->mimeData()->data("application/vnd.zinek.zeclass").data();
			
			if (Class->IsAbstract())
				return false;
				
			if (!TargetWrapper->CheckChildrenClass(Class))
				return false;

			DragEvent->acceptProposedAction();
			return true;
		}
		else if (DragEvent->mimeData()->hasFormat("application/vnd.zinek.zeassert"))
		{
			// Later
			return false;
		}
	}
	else if (Event->type() == QEvent::Drop)
	{
		QDropEvent* DropEvent = static_cast<QDropEvent*>(Event);

		QTreeWidgetItem* TargetItem = Form->trwObjects->itemAt(DropEvent->pos());
		if (TargetItem == NULL)
			return false;

		ZEDObjectWrapper* TargetWrapper = Form->trwObjects->GetWrapper(TargetItem);
		if (TargetWrapper == NULL)
			return false;

		if (DropEvent->mimeData()->hasFormat("application/vnd.zinek.zeobjectwrapper"))
		{
			ZEDObjectWrapper* ObjectWrapper = *(ZEDObjectWrapper**)DropEvent->mimeData()->data("application/vnd.zinek.zeobjectwrapper").data();
			
			if (ObjectWrapper == TargetWrapper)
				return false;

			if (ObjectWrapper->GetParent() == TargetWrapper)
				return false;

			if (!TargetWrapper->CheckChildrenClass(ObjectWrapper->GetObjectClass()))
				return false;
			
			DropEvent->acceptProposedAction();

			GetEditor()->GetObjectManager()->RelocateObject(TargetWrapper, ObjectWrapper);

			return true;
		}
		else if (DropEvent->mimeData()->hasFormat("application/vnd.zinek.zeclass"))
		{
			ZEClass* Class = *(ZEClass**)DropEvent->mimeData()->data("application/vnd.zinek.zeclass").data();

			if (Class->IsAbstract())
				return false;

			if (!TargetWrapper->CheckChildrenClass(Class))
				return false;

			DropEvent->acceptProposedAction();
			
			GetEditor()->GetObjectManager()->CreateObject(TargetWrapper, Class);

			return true;
		}
		else if (DropEvent->mimeData()->hasFormat("application/vnd.zinek.zeasset"))
		{
			// Later
			return false;
		}

		return false;
	}

	return false;
}

void ZEDObjectBrowser::trwObjects_itemSelectionChanged()
{
	bool RootSelected = false;
	QList<QTreeWidgetItem*> Items = GetObjectTree()->selectedItems();
	for (int I = 0; I < Items.count(); I++)
	{
		if (GetObjectTree()->GetWrapper(Items[I]) == GetObjectTree()->GetRootWrapper())
			RootSelected = true;
	}

	Form->btnDelete->setEnabled(!RootSelected);
}

void ZEDObjectBrowser::txtSearch_textChanged(const QString& Text)
{
	if (Text.isEmpty())
		Form->trwObjects->SetMode(ZED_OTM_TREE);
	else
		Form->trwObjects->SetMode(ZED_OTM_TREE);

	Form->trwObjects->SetFilterPattern(Text);
	Form->trwObjects->Update();
}

void ZEDObjectBrowser::btnDelete_clicked()
{
	ZEArray<ZEDObjectWrapper*> Wrappers;
	QList<QTreeWidgetItem*> Items = GetObjectTree()->selectedItems();
	for (int I = 0; I < Items.count(); I++)
		Wrappers.Add(GetObjectTree()->GetWrapper(Items[I]));

	GetEditor()->GetObjectManager()->DeleteObjects(Wrappers);
}

ZEDObjectTree* ZEDObjectBrowser::GetObjectTree()
{
	return Form->trwObjects;
}

ZEDObjectBrowser::ZEDObjectBrowser(QWidget* Parent) : QWidget(Parent)
{
	Form = new Ui_ZEDObjectBrowser();
	Form->setupUi(this);
	
	Form->trwObjects->setAcceptDrops(true);
	Form->trwObjects->viewport()->installEventFilter(this);

	connect(Form->txtSearch, SIGNAL(textChanged(const QString&)), this, SLOT(txtSearch_textChanged(const QString&)));
	connect(Form->btnDelete, SIGNAL(clicked()), this, SLOT(btnDelete_clicked()));
	connect(Form->trwObjects, SIGNAL(itemSelectionChanged()), this, SLOT(trwObjects_itemSelectionChanged()));
}

ZEDObjectBrowser::~ZEDObjectBrowser()
{
	delete Form;
}
