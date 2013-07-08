//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECrashReportUICompletedWindow.cpp
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

#include "ZECrashReportUICompletedWindow.h"
#include "ui_CompletedWindow.h"
#include <QtGui/QDesktopServices>
#include <QtCore/QURL>

ZECrashReportUICompletedWindow::ZECrashReportUICompletedWindow(QMainWindow* ParentWidget)
{
	MainWindow = ParentWidget;
	CompletedWindow = new Ui::CompletedWindowUI();
	CompletedWindow->setupUi(this);
	this->setStatusBar(NULL);
	InitializeLinkButtons();
	CreateWindowRawText();
	this->setFixedSize(480, 400);	
	QObject::connect(CompletedWindow->btnClose, SIGNAL(clicked()), this, SLOT(btnClose_Clicked()));	
}

ZECrashReportUICompletedWindow::~ZECrashReportUICompletedWindow()
{
	
}

void ZECrashReportUICompletedWindow::InitializeLinkButtons()
{
	pbtnWebSite = new ZECrashReportUIClickableLabel("http://www.zinekengine.com/en/contact-now");
	pbtnWebSite->setObjectName("btnWebSite");
	pbtnWebSite->setFixedWidth(250);
	QObject::connect(this->pbtnWebSite, SIGNAL(clicked()), this, SLOT(btnWebSite_Clicked()));
	

	pbtnEmail = new ZECrashReportUIClickableLabel("<contact@zinekengine.com>");
	pbtnEmail->setObjectName("btnEmail");
	pbtnEmail->setFixedWidth(150);
	QObject::connect(this->pbtnEmail, SIGNAL(clicked()), this, SLOT(btnEmail_Clicked()));
}

void ZECrashReportUICompletedWindow::btnWebSite_Clicked()
{
	
	QUrl URL(QString("http://www.zinekengine.com/en/contact-now"));
	QDesktopServices::openUrl(URL);
}

void ZECrashReportUICompletedWindow::btnEmail_Clicked()
{
	QUrl URL(QString("mailto:contact@zinekengine.com"));	
	QDesktopServices::openUrl(URL);
}

void ZECrashReportUICompletedWindow::CreateWindowRawText()
{	
	QList<QLabel*> LabelList;

	QString LabelText = "Thank you for your patience and interest.";
	LabelList.append(new QLabel(LabelText));
	QFont TempFont;
	TempFont.setBold(true);
	LabelList[LabelList.count() - 1]->setFont(TempFont);

	LabelText = "Zinek Crash Reporter have successfully sent the crash report to our crash report servers. "
		+ QString("We will inform you about the issue as soon as possible if you proclaimed your contact information.");

	LabelList.append(new QLabel(LabelText));
	LabelList[LabelList.count() - 1]->setWordWrap(true);

	LabelText = "You may contact us from our web site";
	LabelList.append(new QLabel(LabelText));
	LabelList[LabelList.count() - 1]->setWordWrap(true);

	
	LabelList.append(pbtnWebSite);
	LabelList[LabelList.count() - 1]->setWordWrap(true);

	LabelText = " or our contact mail";
	LabelList.append(new QLabel(LabelText));
	LabelList[LabelList.count() - 1]->setWordWrap(true);

	
	LabelList.append(pbtnEmail);
	LabelList[LabelList.count() - 1]->setWordWrap(true);
	
	for (int I = 0; I < LabelList.count(); I++)
		CompletedWindow->layoutRawText->addWidget(LabelList[I]);

}

bool ZECrashReportUICompletedWindow::eventFilter(QObject* Obj, QEvent* Event)
{
	if (Event->type() == QEvent::Close)
	{
		Event->ignore();
		btnClose_Clicked();
		return true;
	}
	return QMainWindow::eventFilter(Obj, Event);
}

void ZECrashReportUICompletedWindow::btnClose_Clicked()
{
	qApp->exit();
}

