//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECrashReportUIProgressWindow.cpp
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

#include "ZECrashReportUIProgressWindow.h"
#include "ui_ProgressWindow.h"
#include "ZECrashReportUIMainWindow.h"
#include "ZECrashReport/ZECrashReportPackager.h"
#include "ZECrashReport/ZECrashReportSender.h"
#include "ZECrashReportUploadThread.h"

ZECrashReportUIProgressWindow::ZECrashReportUIProgressWindow(QWidget* Parent, Qt::WFlags Flags) : QMainWindow(Parent, Flags)
{
	
}

ZECrashReportUIProgressWindow::ZECrashReportUIProgressWindow(QMainWindow* ParentWidget, ZECrashReport* CrashReport)
{
	ProgressWindow = new Ui::ProgressWindowUI();
	MainWindow = (ZECrashReportUIMainWindow*)ParentWidget;
	ProgressWindow->setupUi(this);

	this->setFixedSize(480, 400);
	this->setStatusBar(NULL);

	ProgressWindow->progBar->setValue(0);
	ProgressWindow->lblEstimatedTime->setText(" Calculating...");
	ProgressWindow->lblTransferSpeed->setText(" Connecting to server...");

	CreateMainWindowRawText();	
	InitializeCompletedWindow();

	QObject::connect(ProgressWindow->btnCancel, SIGNAL(clicked()), this, SLOT(btnCancel_Clicked()));
	QObject::connect(ProgressWindow->btnSendInBack, SIGNAL(clicked()), this, SLOT(btnSendInBack_Clicked()));
	QObject::connect(this, SIGNAL(ProgressCanceled()), MainWindow, SLOT(ShowMainWindow()));
	installEventFilter(this);
}

ZECrashReportUIProgressWindow::~ZECrashReportUIProgressWindow()
{
	
}

void ZECrashReportUIProgressWindow::InitializeLinkButtons()
{	
	plblViewReport = new ZECrashReportUIClickableLabel("View Report");
	plblViewReport->setObjectName(QString::fromUtf8("lblViewReport"));
	QObject::connect(this->plblViewReport, SIGNAL(clicked()), MainWindow, SLOT(ShowReport()));
	ProgressWindow->verticalLayout_4->addWidget(plblViewReport);

	plblViewPrivacyPolicy = new ZECrashReportUIClickableLabel("View Privacy Policy");
	plblViewPrivacyPolicy->setObjectName(QString::fromUtf8("lblViewPrivacyPolicy"));
	QObject::connect(this->plblViewPrivacyPolicy, SIGNAL(clicked()), MainWindow, SLOT(ShowPrivacyPolicy()));
	ProgressWindow->verticalLayout_4->addWidget(plblViewPrivacyPolicy);
}

void ZECrashReportUIProgressWindow::UploadError()
{
	if (this->isHidden())
		this->showNormal();

	QMessageBox* messageBox = new QMessageBox(QMessageBox::Critical, "Zinek Crash Manager", 
		"There is an error occurred while transferring crash report file.\n", 
		QMessageBox::Ok, this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

	int status = messageBox->exec();

	switch (status)
		case QMessageBox::Ok:
		{		
			emit ProgressCanceled();
		}
}

void ZECrashReportUIProgressWindow::btnCancel_Clicked()
{
	QMessageBox* messageBox = new QMessageBox(QMessageBox::Question, "Zinek Crash Manager", 
		"Current progress will be lost. Do you want to continue ?", 
		QMessageBox::No | QMessageBox::Yes, this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

	int status = messageBox->exec();

	switch (status)
		case QMessageBox::Yes:
		{
			emit ProgressCanceled();
		}
}

void ZECrashReportUIProgressWindow::btnSendInBack_Clicked()
{
	this->hide();
}

void ZECrashReportUIProgressWindow::CreateMainWindowRawText()
{
	QList<QLabel*> LabelList;

	QString LabelText = "Thank you for your patience and interest.";
	LabelList.append(new QLabel(LabelText));
	QFont TempFont;
	TempFont.setBold(true);
	LabelList[LabelList.count() - 1]->setFont(TempFont);

	LabelText = "Sending the crash report to our crash report servers. This report is going to be used for engineering purposes only. "
		+ QString("It will help our development team to analyze and fix the problem in order to create better product.");

	LabelList.append(new QLabel(LabelText));
	LabelList[LabelList.count() - 1]->setWordWrap(true);

	LabelText = "You can click the \"Send in background\" button to close this dialog box. It will continue transferring the report in background and will inform you when it is done.";
	LabelList.append(new QLabel(LabelText));
	LabelList[LabelList.count() - 1]->setWordWrap(true);

	for (int I = 0; I < LabelList.count(); I++)
		ProgressWindow->InformationTextLayout->addWidget(LabelList[I]);
}

void ZECrashReportUIProgressWindow::UpdateUploadInformation()
{

}

void ZECrashReportUIProgressWindow::UploadCompleted()
{
	this->hide();
	CompletedWindow->show();
}

bool ZECrashReportUIProgressWindow::eventFilter(QObject* Obj, QEvent* Event)
{
	if (Event->type() == QEvent::Close)
	{
		Event->ignore();
		btnCancel_Clicked();
		return true;
	}
	return QMainWindow::eventFilter(Obj, Event);
}

void ZECrashReportUIProgressWindow::InitializeCompletedWindow()
{
	CompletedWindow = new ZECrashReportUICompletedWindow(this);
}

