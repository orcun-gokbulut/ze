//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECrashReportUIMainWindow.cpp
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

#include "ZECrashReportUIMainWindow.h"
#include "ui_CrashMainWindow.h"
#include "ZECrashReport/ZECrashReportApplicationInformationProvider.h"
#include "ZECrashReport/ZECrashReportSystemInformationProvider.h"
#include "ZECrashReport/ZECrashReportFileProvider.h"
#include "ZECrashReportUploadThread.h"

ZECrashReportUIMainWindow::ZECrashReportUIMainWindow(QWidget *Parent, Qt::WFlags Flags) : QMainWindow(Parent, Flags)
{
	
}

ZECrashReportUIMainWindow::ZECrashReportUIMainWindow(ZECrashReport& CrashReport, const ZEString& UploadURL)
{
	CrashMainWindow = new Ui::CrashMainWindowUI();	
	CrashMainWindow->setupUi(this);

	this->CrashReport = CrashReport;
	GenerateUserCommentProvider();
	InitializeMainWindow();
	CreateMainWindowRawText();
	CreateTrayIcon();	
	InitializeProgressWindow();
	InitializePrivacyDialog();
	InitializeReportDialog();	
	InitializeUploadThread(UploadURL);
	installEventFilter(this);
	this->show();
}

void ZECrashReportUIMainWindow::GenerateUserCommentProvider()
{
	UserCommand = new ZECrashReportUserCommentProvider();
	UserCommand->SetNameSurname("");
	UserCommand->SetEMail("");
	UserCommand->SetComment("");
	UserCommand->SetContactBack(false);
	CrashReport.RegisterProvider(UserCommand);
}

void ZECrashReportUIMainWindow::UpdateUserCommand()
{
	UserCommand->SetNameSurname(CrashMainWindow->txtNameSurname->text().toLocal8Bit());
	UserCommand->SetEMail(CrashMainWindow->txtEmail->text().toLocal8Bit());
	UserCommand->SetComment(CrashMainWindow->txtFeedback->toPlainText().toLocal8Bit());
	if(CrashMainWindow->chkCanContact->isChecked())
		UserCommand->SetContactBack(true);
	else
		UserCommand->SetContactBack(false);

	UserCommand->Generate();
}

void ZECrashReportUIMainWindow::InitializeProgressWindow()
{
	ProgressWindow = new ZECrashReportUIProgressWindow(this, &CrashReport);
}

void ZECrashReportUIMainWindow::InitializePrivacyDialog()
{
	PrivacyDialog = new ZECrashReportUIPrivacyDialog(this);
	QObject::connect(PrivacyDialog, SIGNAL(DialogClosed()), this, SLOT(HidePrivacyPolicy()));	
}

void ZECrashReportUIMainWindow::InitializeReportDialog()
{
	ReportDialog = new ZECrashReportUIReportDialog(this);
	QObject::connect(ReportDialog, SIGNAL(DialogClosed()), this, SLOT(HideReport()));	
}

void ZECrashReportUIMainWindow::InitializeUploadThread(const ZEString& UploadURL)
{
	CrashReportUploadThread = new ZECrashReportUploadThread(CrashReport);
	ZECrashReportUploadThread* UploadThread = (ZECrashReportUploadThread*)CrashReportUploadThread;
	UploadThread->SetUploadURL(UploadURL); //http://localhost:8080/puttest/test.dat
	QObject::connect(CrashReportUploadThread, SIGNAL(UploadError()), ProgressWindow, SLOT(UploadError()));
	QObject::connect(CrashReportUploadThread, SIGNAL(UpdateUploadInformation()), ProgressWindow, SLOT(UpdateUploadInformation()));
	QObject::connect(CrashReportUploadThread, SIGNAL(UploadCompleted()), ProgressWindow, SLOT(UploadCompleted()));
}

void ZECrashReportUIMainWindow::StartUploadThread()
{	
	CrashReportUploadThread->start(QThread::InheritPriority);
}

void ZECrashReportUIMainWindow::InitializeMainWindow()
{
	setFixedSize(480, 640);
	setStatusBar(NULL);
	CrashMainWindow->centralwidget->layout()->setSpacing(10);
	/* Set window Icon */
	//this->setWindowIcon(QIcon("C:\Users\onur.babaoglu\Desktop\Zinek Sticker Calismasi\Logolar Eski\Logolar\"));
	InitializeLinkButtons();

	QObject::connect(CrashMainWindow->btnSend, SIGNAL(clicked()), this, SLOT(ShowProgress()));
	QObject::connect(CrashMainWindow->btnClose, SIGNAL(clicked()), this, SLOT(btnCloseClicked()));
}

void ZECrashReportUIMainWindow::InitializeLinkButtons()
{	
	plblViewReport = new ZECrashReportUIClickableLabel("View Report");
	plblViewReport->setObjectName(QString::fromUtf8("lblViewReport"));
	CrashMainWindow->verticalLayout_2->addWidget(plblViewReport);
	QObject::connect(this->plblViewReport, SIGNAL(clicked()), this, SLOT(ShowReport()));

	plblViewPrivacyPolicy = new ZECrashReportUIClickableLabel("View Privacy Policy");
	plblViewPrivacyPolicy->setObjectName(QString::fromUtf8("lblViewPrivacyPolicy"));	
	CrashMainWindow->verticalLayout_2->addWidget(plblViewPrivacyPolicy);
	QObject::connect(this->plblViewPrivacyPolicy, SIGNAL(clicked()), this, SLOT(ShowPrivacyPolicy()));
	
}

void ZECrashReportUIMainWindow::CreateTrayIcon()
{	
	TrayIconMenu = new QMenu(this);

	QAction* restoreAction = new QAction(tr("&Restore"), this);
	connect(restoreAction, SIGNAL(triggered()), this, SLOT(TrayRestore()));
		
	TrayIconMenu->addAction(restoreAction);

	QIcon IconTrayIcon;
	/* Set System Tray Icon */
	//IconTrayIcon.addPixmap(QPixmap("C:\\Users\\onur.babaoglu\\Desktop\\Icon.png"), QIcon::Normal, QIcon::Off);
	//IconTrayIcon.addPixmap(QPixmap("C:/Users/onur.babaoglu/Desktop/Zinek Sticker Calismasi/Logolar Eski/Logolar/ZELogo 10000x10000/Logo_10000x10000.png"), QIcon::Normal, QIcon::Off);

	TrayIcon = new QSystemTrayIcon(this);
	TrayIcon->setIcon(IconTrayIcon);
	TrayIcon->setToolTip("Zinek Crash Reporter");
	TrayIcon->setContextMenu(TrayIconMenu);
	TrayIcon->setVisible(true);
}

ZECrashReportUIMainWindow::~ZECrashReportUIMainWindow()
{
	
}

void ZECrashReportUIMainWindow::btnCloseClicked()
{
	QMessageBox* messageBox = new QMessageBox(QMessageBox::Question, "Zinek Crash Manager", 
		"Every information about crash will be lost. Do you want to continue?", 
		QMessageBox::No | QMessageBox::Yes, this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	int status = messageBox->exec();

	switch (status)
		case QMessageBox::Yes:
		{	
			qApp->quit();
		}
}

void ZECrashReportUIMainWindow::CreateMainWindowRawText()
{
	QList<QLabel*> TopLabelList;
	QList<QLabel*> MiddleLabelList;
	QList<QLabel*> BottomLabelList;

	QString LabelText = "Zinek Product has crashed. Your progress or data in Zinek Product may be lost. " 
		+ QString("The problem may be due bug in Zinek Product or a problem with your machine. ") 
		+ "Please restart the application and do not forget to save often. We apologize for the inconvenience.";

	TopLabelList.append(new QLabel(LabelText));
	TopLabelList[TopLabelList.count() - 1]->setWordWrap(true);

	LabelText = "You can help us to find the root cause of this problem !";

	TopLabelList.append(new QLabel(LabelText));
	TopLabelList[TopLabelList.count() - 1]->setWordWrap(true);
	QFont TempFont;
	TempFont.setBold(true);
	TopLabelList[TopLabelList.count() - 1]->setFont(TempFont);

	LabelText = "We have created a report about the crash that you can send it to us. This report is going to help our engineers for finding, "
		+ QString("analyzing and fixing the problem. Therefore, it is going to help us to improve our software");

	TopLabelList.append(new QLabel(LabelText));
	TopLabelList[TopLabelList.count() - 1]->setWordWrap(true);

	LabelText = "This reports contains:";
	MiddleLabelList.append(new QLabel(LabelText));	
	MiddleLabelList[MiddleLabelList.count() - 1]->setFont(TempFont);

	LabelText = " - The last state of the Zinek Product when it crashed.";
	MiddleLabelList.append(new QLabel(LabelText));

	LabelText = " - Zinek Product log file.";
	MiddleLabelList.append(new QLabel(LabelText));

	LabelText = " - Brief information of your computer's hardware, software and drivers";
	MiddleLabelList.append(new QLabel(LabelText));

	LabelText = " - If you provide, your feedback about this issue.";
	MiddleLabelList.append(new QLabel(LabelText));

	LabelText = "This report does not contain any personal information (unless you have provided by filling the form bellow) "
		+ QString("such as your name, your computers name, your ip address, your real address, your account name and etc. You can also view and ")
		+ "inspect this report by clicking \"View Report\" link bellow ";
	BottomLabelList.append(new QLabel(LabelText));
	BottomLabelList[BottomLabelList.count() - 1]->setWordWrap(true);

	for (int I = 0; I < TopLabelList.count(); I++)
		CrashMainWindow->InformationTextLayout->addWidget(TopLabelList[I]);

	QLayout* bulletedLayout = new QVBoxLayout();
	bulletedLayout->setSpacing(0);
	CrashMainWindow->InformationTextLayout->addLayout(bulletedLayout);

	for (int I = 0; I < MiddleLabelList.count(); I++)
		bulletedLayout->addWidget(MiddleLabelList[I]);

	for (int I = 0; I < BottomLabelList.count(); I++)
		CrashMainWindow->InformationTextLayout->addWidget(BottomLabelList[I]);

	CrashMainWindow->InformationTextLayout->setSpacing(10);
}

void ZECrashReportUIMainWindow::ShowMainWindow()
{
	ProgressWindow->hide();
	this->show();
	UiWindowState = ZE_CRUI_WINDOWSTATE_CRASHMAINWINDOW;
		
	if(CrashReportUploadThread->isRunning())
		CrashReportUploadThread->terminate();
}

void ZECrashReportUIMainWindow::ShowPrivacyPolicy()
{	
	PrivacyDialog->exec();	
}

void ZECrashReportUIMainWindow::HidePrivacyPolicy()
{
	PrivacyDialog->hide();
}

void ZECrashReportUIMainWindow::ShowReport()
{
	UpdateUserCommand();
	ReportDialog->exec();
}

void ZECrashReportUIMainWindow::HideReport()
{
	ReportDialog->hide();
}

void ZECrashReportUIMainWindow::ShowProgress()
{
	UpdateUserCommand();
	this->hide();
	ProgressWindow->show();
	UiWindowState = ZE_CRUI_WINDOWSTATE_PROGRESSWINDOW;
	StartUploadThread();
}

void ZECrashReportUIMainWindow::TrayRestore()
{
	if (UiWindowState == ZE_CRUI_WINDOWSTATE_CRASHMAINWINDOW)
		this->showNormal();
	else if (UiWindowState == ZE_CRUI_WINDOWSTATE_PROGRESSWINDOW)
		ProgressWindow->showNormal();
	else
		return;
}

bool ZECrashReportUIMainWindow::eventFilter(QObject* Obj, QEvent* Event)
{
	if (Event->type() == QEvent::Close)
	{
		Event->ignore();
		emit btnCloseClicked();
		return true;
	}
	return QMainWindow::eventFilter(Obj, Event);
}

