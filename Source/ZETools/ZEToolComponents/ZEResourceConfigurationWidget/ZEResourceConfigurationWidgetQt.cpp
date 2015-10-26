//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEResourceConfigurationWidgetQt.cpp
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

#include "ZEResourceConfigurationWidgetQt.h"
#include "ui_ZEResourceConfigurationWidget.h"
#include "QtGui/QTreeWidgetItem"
#include "ZEResourceConfigurationWidget.h"
#include "ZEResourceOptionTreeItem.h"
#include "ui_ZEResourceConfigurationWidget.h"

ZEResourceConfigurationWidgetQt::ZEResourceConfigurationWidgetQt(ZEResourceConfigurationWidget* ConfigurationWidget, QWidget* Parent)
{
	CurrentSelectedResource = NULL;
	QApplicationCreated = false;

	if(QApplication::instance() == NULL)
	{
		ZEInt32	Argc = 0;
		void*	Argv = NULL;
		Application = new QApplication(Argc, NULL);
		QApplicationCreated = true;
	}

	Form = new Ui::ZEResourceConfiguratorUI();
	Dialog = new QDialog(Parent);
	Form->setupUi(Dialog);
	Dialog->setWindowTitle("Resource Configuration");
	setParent(Dialog);

	OptionWidget = new ZEResourceOptionWidget(Form->OptionsParentWidget);
	Form->OptionsParentWidgetLayout->addWidget(OptionWidget);
	OptionWidget->setVisible(false);

	this->ConfigurationWidget = ConfigurationWidget;
	connect(Form->ResourcesTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(TreeItemClicked(QTreeWidgetItem*, int)));
	connect(OptionWidget, SIGNAL(OptionChanged(ZEResourceOptionWidget*)), this, SLOT(OptionChanged(ZEResourceOptionWidget*)));
}

ZEResourceConfigurationWidgetQt::~ZEResourceConfigurationWidgetQt()
{
	if(QApplicationCreated)
	{
		Application->quit();
		delete Application;
		Application = NULL;
	}
}

void ZEResourceConfigurationWidgetQt::TreeItemClicked(QTreeWidgetItem* Item, int Column)
{
	ZEResourceOptionTreeItem* ZEItem = (ZEResourceOptionTreeItem*)Item;
	OptionWidget->setVisible(true);
	OptionWidget->SetOptions(ZEItem->GetResourceOption());
	CurrentSelectedResource = (ZEResourceOptionTreeItem*)Item;
}

void ZEResourceConfigurationWidgetQt::OptionChanged(ZEResourceOptionWidget* Widget)
{
	CurrentSelectedResource->GetResourceOption()->Action = Widget->GetActionOnAvailable();
	CurrentSelectedResource->GetResourceOption()->IsAutoGenerated = Widget->GetAutoGenerated();
	CurrentSelectedResource->GetResourceOption()->ExportPath = Widget->GetExportPath();
	CurrentSelectedResource->GetResourceOption()->PhysicalPath = Widget->GetPhysicalPath();
	((ZEResourceOptionTreeItem*)CurrentSelectedResource)->Update();
}
