//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRWindowInformation.cpp
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

#include "ZECRWindowInformation.h"

#include "ui_ZECRWindowInformation.h"
#include "ZECRWindow.h"
#include "ZECRReportViewer.h"
#include "ZECRWindowViewPrivacyPolicy.h"
#include "ZEFile/ZEPathManager.h"

#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>


void ZECRWindowInformation::btnViewReport_Clicked()
{
	ZECRReportViewer Viewer;
	if (!Viewer.LoadReport(GetWindow()->GetCrashReport()->GetReportFileName()))
		QMessageBox::critical(this, "ZECRCrashReport", "Cannot load report. File may be corrupted.", QMessageBox::Ok);

	Viewer.exec();
}

void ZECRWindowInformation::btnOpenReportLocation_Clicked()
{
	ZEString RealPath = ZEPathManager::GetInstance()->TranslateToRealPath(GetWindow()->GetCrashReport()->GetReportFileDirectory()).Path;
	QDesktopServices::openUrl(QUrl(RealPath.ToCString()));
}

void ZECRWindowInformation::btnViewPrivacyPolicy_Clicked()
{
	ZECRWindowViewPrivacyPolicy Viewer;
	Viewer.exec();
}

void ZECRWindowInformation::btnGotoSupportPortal_Clicked()
{
	QDesktopServices::openUrl(QUrl("https://support.zinek.xyz"));
}

void ZECRWindowInformation::btnSend_Clicked()
{
	GetWindow()->SetPage(ZECR_WP_USER_FEEDBACK);
}

void ZECRWindowInformation::btnDontSend_Clicked()
{
	qApp->exit(EXIT_FAILURE);
}

ZECRWindowInformation::ZECRWindowInformation(QWidget* Parent) : ZECRWindowPage(Parent)
{
	Form = new Ui_ZECRWindowInformation();
	Form->setupUi(this);

	connect(Form->btnViewReport, SIGNAL(clicked()), this, SLOT(btnViewReport_Clicked()));
	connect(Form->btnOpenReportLocation, SIGNAL(clicked()), this, SLOT(btnOpenReportLocation_Clicked()));
	connect(Form->btnViewPrivacyPolicy, SIGNAL(clicked()), this, SLOT(btnViewPrivacyPolicy_Clicked()));
	connect(Form->btnGotoSupportPortal, SIGNAL(clicked()), this, SLOT(btnGotoSupportPortal_Clicked()));
	connect(Form->btnSend, SIGNAL(clicked()), this, SLOT(btnSend_Clicked()));
	connect(Form->btnDontSend, SIGNAL(clicked()), this, SLOT(btnDontSend_Clicked()));
}

ZECRWindowInformation::~ZECRWindowInformation()
{
	delete Form;
}
