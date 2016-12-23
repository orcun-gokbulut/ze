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

#include "ZEDS/ZEFormat.h"
#include "ZEDCore/ZEDEditor.h"
#include "ZEDCore/ZEDObjectWrapper.h"
#include "ZEDCore/ZEDSelectionManager.h"
#include "ZEDCore/ZEDObjectManager.h"
#include "ZEDClassModel.h"
#include "ZEGame/ZEEntity.h"
#include "ui_ZEDClassBrowser.h"

void ZEDClassBrowser::Update()
{
	if (!Form->txtSearch->text().isEmpty())
		Model->SetSearchPattern(ZEFormat::Format("*{0}*", Form->txtSearch->text().toUtf8().begin()));
	else
		Model->SetSearchPattern("");

	Model->SetMode(Form->btnMode->isChecked() ? ZED_CMM_TREE : ZED_CMM_LIST);
	ZEClass* SelectedClass = NULL;
	QModelIndexList SelectedIndexes = Form->trwClasses->selectionModel()->selectedRows();
	if (SelectedIndexes.count() == 1)
		SelectedClass = Model->ConvertToClass(SelectedIndexes[0]);

	if (SelectedClass != NULL && !SelectedClass->IsAbstract())
	{
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
	else
	{
		Form->btnAdd->setEnabled(false);
	}
}

bool ZEDClassBrowser::InitializeInternal()
{
	if (!ZEDWindow::InitializeInternal())
		return false;

	Update();

	return true;
}

void ZEDClassBrowser::SelectionEvent(const ZEDSelectionEvent* Event)
{
	Update();
}

void ZEDClassBrowser::txtSearch_textChanged(const QString& Text)
{
	Update();
}

void ZEDClassBrowser::trwClasses_itemSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	Update();
}

void ZEDClassBrowser::btnMode_toggled(bool Checked)
{
	Update();
}

void ZEDClassBrowser::btnAdd_clicked()
{
	QModelIndexList SelectedIndexes = Form->trwClasses->selectionModel()->selectedRows();
	GetEditor()->GetObjectManager()->CreateObject(DestinationWrapper,  Model->ConvertToClass(SelectedIndexes[0]));
}

ZEDClassBrowser::ZEDClassBrowser()
{
	SetName("Class Browser");

	Widget = new QWidget();
	SetWidget(Widget);

	Form = new Ui_ZEDClassBrowser();
	Form->setupUi(Widget);

	Model = new ZEDClassModel();
	Model->SetRootClass(ZEEntity::Class());
	Model->SetMode(ZED_CMM_TREE);
	Model->SetHierarchy(ZED_CMH_INHERITANCE);
	Form->btnMode->setChecked(Model->GetMode() == ZED_CMM_TREE);
	Form->trwClasses->setModel(Model);
	Form->trwClasses->setExpanded(Model->index(0, 0, QModelIndex()), true);
	Form->trwClasses->setDragEnabled(true);
	Form->trwClasses->header()->setStretchLastSection(false);
	Form->trwClasses->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	Form->trwClasses->hideColumn(2);
	Form->trwClasses->hideColumn(1);
	Form->trwClasses->setSelectionBehavior(QAbstractItemView::SelectRows);
	Form->trwClasses->setSelectionMode(QAbstractItemView::SingleSelection);
	Form->trwClasses->setAlternatingRowColors(true);

	Update();

	connect(Form->txtSearch, SIGNAL(textChanged(const QString&)), this, SLOT(txtSearch_textChanged(const QString&)));
	connect(Form->btnMode, SIGNAL(toggled(bool)), this, SLOT(btnMode_toggled(bool)));
	connect(Form->trwClasses->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(trwClasses_itemSelectionChanged(const QItemSelection&, const QItemSelection&)));
	connect(Form->btnAdd, SIGNAL(clicked()), this, SLOT(btnAdd_clicked()));
}

ZEDClassBrowser::~ZEDClassBrowser()
{
	Deinitialize();

	delete Model;
	delete Form;
	delete Widget;
}
