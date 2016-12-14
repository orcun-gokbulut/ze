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

#include "ZEDObjectModel.h"
#include "ZEDObjectSelectionModel.h"
#include "ZEDCore\ZEDEditor.h"
#include "ZEDCore\ZEDObjectWrapper.h"
#include "ZEDCore\ZEDObjectManager.h"
#include "ui_ZEDObjectBrowser.h"

bool ZEDObjectBrowser::InitializeInternal()
{
	if (!ZEDComponent::InitializeInternal())
		return false;

	GetEditor()->AddComponent(Model);
	GetEditor()->AddComponent(SelectionModel);

	return true;
}

bool ZEDObjectBrowser::DeinitializeInternal()
{
	GetEditor()->RemoveComponent(SelectionModel);
	GetEditor()->RemoveComponent(Model);
	
	return ZEDWindow::DeinitializeInternal();
}

void ZEDObjectBrowser::UpdateUI()
{
	if (Form->txtSearch->text().isEmpty())
		Model->SetMode(ZED_OTM_TREE);
	else
		Model->SetMode(ZED_OTM_LIST);

	bool RootSelected = false;
	QModelIndexList Items = Form->trwObjects->selectionModel()->selectedRows();
	for (int I = 0; I < Items.count(); I++)
	{
		if (Model->ConvertToWrapper(Items[I]) == Model->GetRootWrapper())
		{
			RootSelected = true;
			break;
		}
	}

	Form->btnDelete->setEnabled(!RootSelected);
}

void ZEDObjectBrowser::trwObjects_itemSelectionChanged(const QItemSelection&, const QItemSelection&)
{
	UpdateUI();
}

void ZEDObjectBrowser::txtSearch_textChanged(const QString& Text)
{
	Model->SetFilterPattern(Text.toUtf8().data());
	UpdateUI();
}

void ZEDObjectBrowser::btnDelete_clicked()
{
	ZEArray<ZEDObjectWrapper*> Wrappers;

	QModelIndexList Items = Form->trwObjects->selectionModel()->selectedRows();
	for (int I = 0; I < Items.count(); I++)
		Wrappers.Add(Model->ConvertToWrapper(Items[I]));

	GetEditor()->GetObjectManager()->DeleteObjects(Wrappers);
}

void ZEDObjectBrowser::SetRootWrapper(ZEDObjectWrapper* RootWrapper)
{
	Model->SetRootWrapper(RootWrapper);
}

const ZEDObjectWrapper* ZEDObjectBrowser::GetRootWrapper() const
{
	return Model->GetRootWrapper();
}

ZEDObjectBrowser::ZEDObjectBrowser()
{
	SetName("Object Browser");

	Widget = new QWidget();
	SetWidget(Widget);

	Form = new Ui_ZEDObjectBrowser();
	Form->setupUi(Widget);

	Model = new ZEDObjectModel();
	Model->SetMode(ZED_OTM_TREE);
	SelectionModel = new ZEDObjectSelectionModel(Model);
	SelectionModel->setSelectionBehavior(QAbstractItemView::SelectRows);
	Form->trwObjects->setModel(Model);
	Form->trwObjects->setSelectionModel(SelectionModel);
	Form->trwObjects->setDragEnabled(true);
	Form->trwObjects->header()->setStretchLastSection(false);
	Form->trwObjects->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	Form->trwObjects->setSelectionBehavior(QAbstractItemView::SelectRows);
	Form->trwObjects->setSelectionMode(QAbstractItemView::ExtendedSelection);
	Form->trwObjects->setAlternatingRowColors(true);
	Form->trwObjects->setDragEnabled(true),
	Form->trwObjects->setAcceptDrops(true);

	connect(Form->txtSearch, SIGNAL(textChanged(const QString&)), this, SLOT(txtSearch_textChanged(const QString&)));
	connect(Form->btnDelete, SIGNAL(clicked()), this, SLOT(btnDelete_clicked()));
	connect(Form->trwObjects->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(trwObjects_itemSelectionChanged(const QItemSelection&, const QItemSelection&)));
}

ZEDObjectBrowser::~ZEDObjectBrowser()
{
	Deinitialize();

	delete Model;
	delete Form;
	delete Widget;
}
