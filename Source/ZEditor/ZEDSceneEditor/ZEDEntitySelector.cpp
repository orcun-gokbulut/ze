//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDEntitySelector.cpp
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

#include <QtCore/QString>
#include <QtCore/QStringList>
#include "ZEDEntitySelector.h"
#include "ZEGame\ZEGame.h"
#include "ZEGame\ZEScene.h"
#include "ZEGame\ZEEntity.h"
#include "ZECore\ZECore.h"
#include <ZEGame\ZEEntityProvider.h>

void ZEDEntitySelectorButton::mouseReleaseEvent(QMouseEvent* Event)
{
	if(this->Role)
	{
		this->ParentSelector->FillSelectedEntities();
		this->ParentSelector->close();
	}
	else
		this->ParentSelector->close();
}

ZEDEntitySelector::ZEDEntitySelector()
{
	this->setWindowFlags(Qt::Dialog);
	this->setWindowModality(Qt::ApplicationModal);

	this->Headers  << "Id" << "Name" << "Type" ;

	this->MenuBar = new QMenuBar(this);
	this->Menu = new QMenu(QString("Menu"), this->MenuBar);
	this->MenuBar->addMenu(this->Menu);
	this->ListTreeWidget = new QTreeWidget(this);
	this->SelectButton = new ZEDEntitySelectorButton();
	this->SelectButton->setText(QString("Select"));
	this->SelectButton->setParent(this);
	this->SelectButton->ParentSelector = this;
	this->SelectButton->Role = true;
	this->CancelButton = new ZEDEntitySelectorButton();
	this->CancelButton->setText(QString("Cancel"));
	this->CancelButton->setParent(this);
	this->CancelButton->ParentSelector = this;
	this->CancelButton->Role = false;
	this->SelectButton->setFixedWidth(70);
	this->CancelButton->setFixedWidth(70);

	this->ListTreeWidget->setAlternatingRowColors(true);
	this->ListTreeWidget->setSortingEnabled(true);

	this->MainLayout = new QVBoxLayout(this);
	this->ButtonLayout = new QHBoxLayout(this);
	this->SearchLabel = new QLabel(QString("Search"), this);
	this->FilterLabel = new QLabel(QString("Filter"), this);
	this->SearchLineEdit = new QLineEdit(this);
	this->FilterComboBox = new QComboBox(this);
	this->SearchFilterLayout = new QHBoxLayout(this);

	this->Spacer = new QSpacerItem(20,0, QSizePolicy::Expanding);
	this->ButtonLayout->addSpacerItem(this->Spacer);


	this->SearchFilterLayout->addWidget(this->SearchLabel);
	this->SearchFilterLayout->addWidget(this->SearchLineEdit);
	this->SearchFilterLayout->addWidget(this->FilterLabel);
	this->SearchFilterLayout->addWidget(this->FilterComboBox);

	this->setLayout(this->MainLayout);	
	this->MainLayout->addLayout(this->SearchFilterLayout);
	this->MainLayout->addWidget(this->ListTreeWidget);

	this->ButtonLayout->addWidget(this->SelectButton);
	this->ButtonLayout->addWidget(this->CancelButton);

	this->MainLayout->addLayout(this->ButtonLayout);
	this->MainLayout->setMenuBar(this->MenuBar);

	this->ListTreeWidget->setColumnCount(3);
	this->ListTreeWidget->setHeaderLabels(this->Headers);

	this->ListTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

	QObject::connect(this->FilterComboBox, SIGNAL(currentIndexChanged(ZEInt)), this, SLOT(UpdateListByFilter()));
	QObject::connect(this->SearchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(UpdateListBySearch()));
}


void ZEDEntitySelector::UpdateListByFilter()
{
	QString SearchFilter;
	SearchFilter = this->FilterComboBox->currentText();

	for(ZEInt I = 0; I < this->ListTreeWidget->topLevelItemCount(); I++)
	{
		this->ListTreeWidget->topLevelItem(I)->setHidden(true);
	}
	
	for(ZEInt I = 0; I < this->ListTreeWidget->topLevelItemCount(); I++)
	{
		if(this->ListTreeWidget->topLevelItem(I)->text(2).contains(SearchFilter) == true)

			this->ListTreeWidget->topLevelItem(I)->setHidden(false);

		if(SearchFilter == "All")
			
			this->ListTreeWidget->topLevelItem(I)->setHidden(false);
	}

}

void ZEDEntitySelector::UpdateListBySearch()
{
	QString SearchText;
	SearchText = this->SearchLineEdit->text().toLower();

	for(ZEInt I = 0; I < this->ListTreeWidget->topLevelItemCount(); I++)
	{
		this->ListTreeWidget->topLevelItem(I)->setHidden(true);
	}


	
	for(ZEInt I = 0; I < this->ListTreeWidget->topLevelItemCount(); I++)
	{
		if(this->ListTreeWidget->topLevelItem(I)->text(1).toLower().contains(SearchText) == true)
			
			this->ListTreeWidget->topLevelItem(I)->setHidden(false);
	}
}


void ZEDEntitySelector::GenerateList()
{
	this->ListTreeWidget->clear();
	this->FilterComboBox->clear();

	ZEScene*			Scene = zeCore->GetGame()->GetScene();
	ZEEntity*			CurrentEntity;
	ZEInt					Id;
	const char*			Name;
	const char*			Type;
	QTreeWidgetItem*	CurrentItem;
	QString				TempString;

	for(ZESize I = 0; I < Scene->GetEntities().GetCount(); I++)
	{
			CurrentEntity = Scene->GetEntities()[I];

			if (QString(CurrentEntity->GetName()) == QString(".:SELECTION__ITEM:."))
				continue;

			Id = CurrentEntity->GetEntityId();
			Name = CurrentEntity->GetName();
			Type = CurrentEntity->GetDescription()->GetName();
			CurrentItem = new ZEDEntitySelectorItem();
			((ZEDEntitySelectorItem*)CurrentItem)->Entity = CurrentEntity;

			CurrentItem->setText(0, TempString.setNum(Id));
			CurrentItem->setText(1, QString(Name));
			CurrentItem->setText(2, QString(Type));
			this->ListTreeWidget->addTopLevelItem(CurrentItem);
	}

	this->FilterComboBox->addItem(QString("All"));

	for(ZESize I = 0; I < ZEEntityProvider::GetInstance()->GetClasses().GetCount(); I++)
	{
		this->FilterComboBox->addItem(QString(ZEEntityProvider::GetInstance()->GetClasses().GetItem(I)->GetName()));
	}
}

void ZEDEntitySelector::GetSelectedEntities(ZEArray<ZEEntity*>* SelectedEntities)
{
	this->SelectedEntities = SelectedEntities;
	this->GenerateList();
	this->UpdateListByFilter();
	this->exec();	
}

void ZEDEntitySelector::FillSelectedEntities()
{
	if(this->ListTreeWidget->selectedItems().count() != 0)
	{
		this->SelectedEntities->SetCount(0);
		for(ZEInt I = 0; I < this->ListTreeWidget->selectedItems().count(); I++)
		{
			this->SelectedEntities->Add((((ZEDEntitySelectorItem*)(this->ListTreeWidget->selectedItems()[I]))->Entity));
		}
	}
}
