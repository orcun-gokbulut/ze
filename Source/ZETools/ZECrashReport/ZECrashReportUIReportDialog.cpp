//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECrashReportUIReportDialog.cpp
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

#include "ZECrashReportUIReportDialog.h"
#include "ui_ReportDialog.h"
#include "ZECrashReportUIMainWindow.h"
#include "ZETypes.h"
#include "ZECrashReport/ZECrashReportProvider.h"
#include "ZECrashReport/ZECrashReportFileProvider.h"
#include "ZEFile/ZEFile.h"

class ZECrashReportUIMainWindow;

ZECrashReportUIReportDialog::ZECrashReportUIReportDialog(QWidget *Parent, Qt::WFlags Flags) : QDialog(Parent)
{
	ParentWidget = Parent;
	ReportDialog = new Ui::ReportDialogUI();
	ReportDialog->setupUi(this);
	this->setWindowFlags(Qt::Tool | Qt::WindowMaximizeButtonHint);

	InitiazeTree();

	ReportDialog->btnASCII->setChecked(true);

	QObject::connect(ReportDialog->btnSave, SIGNAL(clicked()), this, SLOT(btnSaveClicked()));
	QObject::connect(ReportDialog->btnHex, SIGNAL(clicked()), this, SLOT(TreeItemSelected()));
	QObject::connect(ReportDialog->btnASCII, SIGNAL(clicked()), this, SLOT(TreeItemSelected()));
	QObject::connect(ReportDialog->trwItems, SIGNAL(itemSelectionChanged()), this, SLOT(TreeItemSelected()));
	
	installEventFilter(this);
}

ZECrashReportUIReportDialog::~ZECrashReportUIReportDialog()
{

}

bool ZECrashReportUIReportDialog::SaveAs(QString FileName)
{
	if (FileName.isEmpty())
		return false;
	
	ZEFile FileToSave;

	if (!FileToSave.Open(FileName.toStdString(), ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
		return false;

	if (FileToSave.Write(SelectedData, 1, SelectedDataSize) != SelectedDataSize)
	{
		FileToSave.Close();
		remove(FileName.toLocal8Bit());
		return false;
	}
	FileToSave.Close();
	return true;
}

void ZECrashReportUIReportDialog::btnSaveClicked()
{
	QList<QTreeWidgetItem*> Itemlist = ReportDialog->trwItems->selectedItems();

	if (Itemlist.count() == 0)
		return;

	QFileDialog* fDialog = new QFileDialog();
	fDialog->setOption(QFileDialog::Option::HideNameFilterDetails);
	QString FileName;

	if (SelectedItem == "Application Information" || SelectedItem == "System Information")
	{
		FileName = fDialog->getSaveFileName(this, tr("Save as..."), SelectedItem.toString(),tr("Xml Files (*.xml)"));
		
		if (FileName.isEmpty())
			return;

		if (!FileName.endsWith(".xml"))
			FileName += ".xml";
	}

	if (SelectedItem == "Log File")
	{
		FileName = fDialog->getSaveFileName(this, tr("Save as..."), SelectedItem.toString(),tr("Text Files (*.txt)"));

		if(FileName.isEmpty())
			return;

		if(!FileName.endsWith(".txt"))
			FileName += ".txt";
	}

	if (SelectedItem == "Dump File")
	{
		FileName = fDialog->getSaveFileName(this, tr("Save as..."), SelectedItem.toString(),tr("Dump Files (*.dmp)"));

		if(FileName.isEmpty())
			return;

		if(!FileName.endsWith(".dmp"))
			FileName += ".dmp";
	}

	if (!SaveAs(FileName))
	{		
		QMessageBox* messageBox = new QMessageBox(QMessageBox::Warning, "Zinek Crash Manager", 
			"There was a problem occurred while saving. Please try again.", 
			QMessageBox::Ok, this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
		messageBox->exec();
		QFile::remove(FileName);
	}
}

void ZECrashReportUIReportDialog::InitiazeTree()
{
	ZECrashReportUIMainWindow* Report = (ZECrashReportUIMainWindow*)ParentWidget;
	ZEArray<ZECrashReportProvider*> ReportProviders = Report->CrashReport.GetProviders();

	ReportDialog->trwItems->setHeaderLabel("Items");
	
	QList<QTreeWidgetItem*> ItemList;

	for (ZESize I = 0; I < Report->CrashReport.GetProviders().GetCount(); I++)
	{		
		QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem();
		treeWidgetItem->setText(ReportDialog->trwItems->topLevelItemCount(), Report->CrashReport.GetProviders()[I]->GetName());
		QVariant Index((ZEInt)I);
		treeWidgetItem->setData(1, 0, Index);
		ItemList.append(treeWidgetItem);
	}

	ReportDialog->trwItems->addTopLevelItems(ItemList);
}

void ZECrashReportUIReportDialog::TreeItemSelected()
{
	QList<QTreeWidgetItem*> Itemlist = ReportDialog->trwItems->selectedItems();

	if (Itemlist.count() == 0)
		return;

	int ItemIndex;
	ItemIndex = Itemlist[0]->data(1, 0).toInt();
	SelectedItem = Itemlist[0]->text(0);
	
	ZECrashReportUIMainWindow* Report = (ZECrashReportUIMainWindow*)ParentWidget;
	ZEArray<ZECrashReportProvider*> ReportProviders = Report->CrashReport.GetProviders();

	SelectedDataSize = ReportProviders[ItemIndex]->GetSize();

	SelectedData = new char[ReportProviders[ItemIndex]->GetSize()];
	ReportProviders[ItemIndex]->GetData(SelectedData, 0, ReportProviders[ItemIndex]->GetSize());

	if (ReportProviders[ItemIndex]->GetProviderType() == ZE_CRPT_BINARY)
	{
		ReportDialog->txtInformation->setEnabled(false);
		ReportDialog->txtInformation->setText("Does not support to display binary files.");
		return;
	}

	if (!ReportDialog->txtInformation->isEnabled())
		ReportDialog->txtInformation->setEnabled(true);

	if (ReportDialog->btnHex->isChecked())
		ReportDialog->txtInformation->setText(QByteArray((char*)SelectedData).toHex().toUpper());
	else	
		ReportDialog->txtInformation->setText((char*)SelectedData);
}

bool ZECrashReportUIReportDialog::eventFilter(QObject* Obj, QEvent* Event)
{
	if (Event->type() == QEvent::Close)
	{
		Event->ignore();
		this->showNormal();
		emit DialogClosed();
		return true;
	}
	return QDialog::eventFilter(Obj, Event);
}
