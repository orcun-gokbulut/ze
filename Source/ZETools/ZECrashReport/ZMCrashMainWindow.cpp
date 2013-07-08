//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZMCrashMainWindow.cpp
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

ZECrashReportUIMainWindow::ZECrashReportUIMainWindow(QWidget *Parent, Qt::WFlags Flags) : QMainWindow(Parent, Flags)
{	
	CrashMainWindow = new Ui::CrashMainWindowUI();
	CrashMainWindow->setupUi(this);
	InitializeWindow();
	CreateMainWindowRawText();
	CreateTrayIcon();

	CrashMainWindow->centralwidget->layout()->setSpacing(10);
	/* Link buttons */
	plblViewReport = new ZECrashReportUIClickableLabel("View Report");
	plblViewReport->setObjectName(QString::fromUtf8("lblViewReport"));
	plblViewPrivacyPolicy = new ZECrashReportUIClickableLabel("View Privacy Policy");
	plblViewPrivacyPolicy->setObjectName(QString::fromUtf8("lblViewPrivacyPolicy"));

	CrashMainWindow->verticalLayout_2->addWidget(plblViewReport);
	CrashMainWindow->verticalLayout_2->addWidget(plblViewPrivacyPolicy);

	/* Signal Slots */
	

	PrivacyDialog = new ZECrashReportUIPrivacyDialog(this);
	ReportDialog = new ZECrashReportUIReportDialog(this);
	ProgressWindow = new ZECrashReportUIProgressWindow(this);

	this->show();
}

void ZECrashReportUIMainWindow::InitializeWindow()
{
	setFixedSize(480, 640);
	setStatusBar(NULL);
	setWindowIcon(QIcon(QPixmap("C:\\Users\\onur.babaoglu\\Desktop\\QtTry\\QtTry\\QtTry\\f02a62985cde")));

	bool result = QObject::connect(CrashMainWindow->btnSend, SIGNAL(clicked()), this, SLOT(ShowProgress()));
	result = QObject::connect(CrashMainWindow->btnClose, SIGNAL(clicked()), this, SLOT(btnCloseClicked()));
	result = QObject::connect(this->plblViewPrivacyPolicy, SIGNAL(clicked()), this, SLOT(ShowPrivacyPolicy()));
	result = QObject::connect(this->plblViewReport, SIGNAL(clicked()), this, SLOT(ShowReport()));
}

void ZECrashReportUIMainWindow::CreateTrayIcon()
{	
	TrayIconMenu = new QMenu(this);

	QAction* restoreAction = new QAction(tr("&Restore"), this);
	connect(restoreAction, SIGNAL(triggered()), this, SLOT(TrayRestore()));

	TrayIconMenu->addAction(restoreAction);

	QIcon IconTrayIcon;
	IconTrayIcon.addPixmap(QPixmap("C:\\Users\\onur.babaoglu\\Desktop\\Icon.png"), QIcon::Normal, QIcon::Off);

	TrayIcon = new QSystemTrayIcon(this);
	TrayIcon->setIcon(IconTrayIcon);
	TrayIcon->setToolTip("Zinek Engine Crash Report");	
	TrayIcon->setContextMenu(TrayIconMenu);
	TrayIcon->setVisible(true);
}


ZECrashReportUIMainWindow::~ZECrashReportUIMainWindow()
{

}

void ZECrashReportUIMainWindow::btnCloseClicked()
{
	QMessageBox* messageBox = new QMessageBox(QMessageBox::Question, "Zinek Engine Crash Manager", 
		"Every information about crash will be lost. Do you want to continue?", 
		QMessageBox::No | QMessageBox::Yes, this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	int status = messageBox->exec();

	switch(status)
	{
	case QMessageBox::Yes:
		this->close();
	}
}

void ZECrashReportUIMainWindow::CreateMainWindowRawText()
{
	QList<QLabel*> TopLabelList;
	QList<QLabel*> MiddleLabelList;
	QList<QLabel*> BottomLabelList;

	QString LabelText = "Zinek Engine has crashed. Your progress or data in Zinek Engine may be lost. " 
		+ QString("The problem may be due bug in Zinek Engine or a problem with your machine. ") 
		+ "Please restart the application and do not forget to save often. We apologize for the inconvenience.";

	TopLabelList.append(new QLabel(LabelText));
	TopLabelList[TopLabelList.count() - 1]->setWordWrap(true);

	LabelText = "You can help us to find the root cause of this problem !";

	TopLabelList.append(new QLabel(LabelText));
	TopLabelList[TopLabelList.count() - 1]->setWordWrap(true);
	QFont TempFont;
	TempFont.setBold(true);
	TopLabelList[TopLabelList.count() - 1]->setFont(TempFont);

	LabelText = "We have created a report about the crash that you can send it to us. This report is going to help "
		+ QString("our engineers for finding, analyzing and fixing the problem. Therefore, it is going to help us to ") 
		+ "improve our software";

	TopLabelList.append(new QLabel(LabelText));
	TopLabelList[TopLabelList.count() - 1]->setWordWrap(true);

	LabelText = "This reports contains:";
	MiddleLabelList.append(new QLabel(LabelText));

	LabelText = " - The last state of the Zinek Engine when it crashed.";
	MiddleLabelList.append(new QLabel(LabelText));

	LabelText = " - Zinek Engine log file.";
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

	for(int i = 0; i < TopLabelList.count(); i++)
		CrashMainWindow->InformationTextLayout->addWidget(TopLabelList[i]);

	QLayout* bulletedLayout = new QVBoxLayout();
	bulletedLayout->setSpacing(0);
	CrashMainWindow->InformationTextLayout->addLayout(bulletedLayout);

	for(int i = 0; i < MiddleLabelList.count(); i++)
		bulletedLayout->addWidget(MiddleLabelList[i]);

	for(int i = 0; i < BottomLabelList.count(); i++)
		CrashMainWindow->InformationTextLayout->addWidget(BottomLabelList[i]);

	CrashMainWindow->InformationTextLayout->setSpacing(10);
}

void ZECrashReportUIMainWindow::ShowMainWindow()
{
	ProgressWindow->hide();
	this->show();
	UiWindowState = ZE_CRUI_WINDOWSTATE_CRASHMAINWINDOW;
}

void ZECrashReportUIMainWindow::ShowPrivacyPolicy()
{
	PrivacyDialog->exec();
}

void ZECrashReportUIMainWindow::ShowReport()
{	
	ReportDialog->exec();	
}

void ZECrashReportUIMainWindow::ShowProgress()
{
	this->hide();
	ProgressWindow->show();
	UiWindowState = ZE_CRUI_WINDOWSTATE_PROGRESSWINDOW;
}

void ZECrashReportUIMainWindow::TrayRestore()
{
	if(UiWindowState == ZE_CRUI_WINDOWSTATE_CRASHMAINWINDOW)
		this->showNormal();
	else if(UiWindowState == ZE_CRUI_WINDOWSTATE_PROGRESSWINDOW)
		ProgressWindow->showNormal();
	else
		return;
}
