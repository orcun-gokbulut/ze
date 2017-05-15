//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRPageSendInformation.cpp
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

#include "ZECRPageSendInformation.h"

#include "ZECRWindow.h"
#include "ZECRReportViewer.h"
#include "ZECRPrivacyPolicyViewer.h"
#include "ZEFile/ZEPathManager.h"
#include "Ui_ZECRPageSendInformation.h"

#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>


void ZECRPageSendInformation::btnViewReport_Clicked()
{
	ZECRReportViewer Viewer;
	if (!Viewer.LoadReport(GetWindow()->GetReport()->GetReportFileName()))
		QMessageBox::critical(this, "ZECRCrashReport", "Cannot load report. File may be corrupted.", QMessageBox::Ok);

	Viewer.exec();
}

void ZECRPageSendInformation::btnOpenReportLocation_Clicked()
{
	ZEString RealPath = ZEPathManager::GetInstance()->TranslateToRealPath(GetWindow()->GetReport()->GetReportFileDirectory()).Path;
	QDesktopServices::openUrl(QUrl::fromLocalFile(RealPath.ToCString()));
}

void ZECRPageSendInformation::btnViewPrivacyPolicy_Clicked()
{
	ZECRPrivacyPolicyViewer Viewer;
	Viewer.exec();
}

void ZECRPageSendInformation::btnGotoSupportPortal_Clicked()
{
	QDesktopServices::openUrl(QUrl("https://support.zinek.xyz"));
}

void ZECRPageSendInformation::btnSend_Clicked()
{
	GetWindow()->SetPage(ZECR_WP_TRANSFERING);
}

void ZECRPageSendInformation::btnDontSend_Clicked()
{
	qApp->exit(EXIT_FAILURE);
}

ZECRPageSendInformation::ZECRPageSendInformation(QWidget* Parent) : ZECRPage(Parent)
{
	Form = new Ui_ZECRPageSendInformation();
	Form->setupUi(this);

	connect(Form->btnViewReport, SIGNAL(clicked()), this, SLOT(btnViewReport_Clicked()));
	connect(Form->btnOpenReportLocation, SIGNAL(clicked()), this, SLOT(btnOpenReportLocation_Clicked()));
	connect(Form->btnViewPrivacyPolicy, SIGNAL(clicked()), this, SLOT(btnViewPrivacyPolicy_Clicked()));
	connect(Form->btnGotoSupportPortal, SIGNAL(clicked()), this, SLOT(btnGotoSupportPortal_Clicked()));
	connect(Form->btnSend, SIGNAL(clicked()), this, SLOT(btnSend_Clicked()));
	connect(Form->btnDontSend, SIGNAL(clicked()), this, SLOT(btnDontSend_Clicked()));
}

ZECRPageSendInformation::~ZECRPageSendInformation()
{
	delete Form;
}
