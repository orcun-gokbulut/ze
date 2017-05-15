//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELNCrashReportsWidget.cpp
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

#include "ZELNCrashReportsWidget.h"
#include "ui_ZELNCrashReportsWidget.h"

#include "ZELNLauncher.h"
#include "ZELNCrashReportsModule.h"

#include "ZEDS/ZEFormat.h"
#include "ZEML/ZEMLReader.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEFile/ZEDirectoryInfo.h"
#include "ZETools/ZECrashReport/ZECRReportViewer.h"

#include <QtWidgets/QMessageBox>
#include "QTreeWidget"

void ZELNCrashReportsWidget::UpdateWidget()
{
	bool HasSelection = (Form->tblReports->selectedItems().count() != 0);
	Form->btnView->setEnabled(HasSelection);
	Form->btnUpload->setEnabled(HasSelection);
	Form->btnDelete->setEnabled(HasSelection);
}

void ZELNCrashReportsWidget::LoadReports()
{
	ZEDirectoryInfo DirectoryInfo(ReportDirectory);
	const ZEArray<ZEString> Files = DirectoryInfo.GetFiles();

	Form->tblReports->clear();
	for (ZESize I = 0; I < Files.GetCount(); I++)
	{
		ZEFileInfo FileInfo(ZEFormat::Format("{0}/{1}", ReportDirectory, Files[I]));
		
		if (FileInfo.GetExtension() != ".ZECRReport")
			continue;

		ZEMLReader Reader;
		if (!Reader.Open(FileInfo.GetPath()))
			continue;

		ZEMLReaderNode RootNode = Reader.GetRootNode();

		if (RootNode.GetName() != "ZECRReport")
			continue;

		QTreeWidgetItem* Item = new QTreeWidgetItem();
		Item->setText(0, FileInfo.GetName().ToCString());
		Item->setText(1, FileInfo.GetModificationTime().ToString().ToCString());
		Item->setText(2, RootNode.ReadBoolean("Uploaded") ? "Yes" : "No");
		Item->setData(0, Qt::UserRole, FileInfo.GetPath().ToCString());
		Form->tblReports->addTopLevelItem(Item);
	}
}

void ZELNCrashReportsWidget::btnView_clicked()
{
	if (Form->tblReports->selectedItems().count() != 1)
		return;

	ZEString ReportPath = Form->tblReports->selectedItems()[0]->data(0, Qt::UserRole).toString().toUtf8().begin();

	ZECRReportViewer Viewer;
	Viewer.LoadReport(ReportPath);
	Viewer.exec();
}

void ZELNCrashReportsWidget::btnUpload_clicked()
{
	if (Form->tblReports->selectedItems().count() != 1)
		return;

	ZEString ReportPath = Form->tblReports->selectedItems()[0]->data(0, Qt::UserRole).toString().toUtf8().begin();
}

void ZELNCrashReportsWidget::btnDelete_clicked()
{
	if (Form->tblReports->selectedItems().count() != 1)
		return;

	int Result = QMessageBox::question(this, "Zinek Launcher", "Are you sure that you want to delete this report ?", QMessageBox::Yes, QMessageBox::No);
	if (Result == QMessageBox::No)
		return;

	ZEFileInfo FileInfo;
	FileInfo.SetPath(Form->tblReports->selectedItems()[0]->data(0, Qt::UserRole).toString().toUtf8().begin());
	FileInfo.Delete();
	
	LoadReports();
	UpdateWidget();
}


void ZELNCrashReportsWidget::SetReportDirectory(const ZEString& ReportDirectory)
{
	this->ReportDirectory = ReportDirectory;
}

const ZEString& ZELNCrashReportsWidget::GetReportDirectory()
{
	return ReportDirectory;
}

ZELNCrashReportsWidget::ZELNCrashReportsWidget(QWidget* Parent) : QWidget(Parent)
{
	Form = new Ui_ZELNCrashReportsWidget();
	Form->setupUi(this);
	connect(Form->btnView, SIGNAL(clicked()), this, SLOT(btnView_clicked()));
	connect(Form->btnUpload, SIGNAL(clicked()), this, SLOT(btnUpload_clicked()));
	connect(Form->btnDelete, SIGNAL(clicked()), this, SLOT(btnDelete_clicked()));

	ReportDirectory = "#S:/CrashReports";

	UpdateWidget();
}

ZELNCrashReportsWidget::~ZELNCrashReportsWidget()
{
	delete Form;
}

