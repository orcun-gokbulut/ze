//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECrashReportUIMainWindow.h
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

#pragma once
#ifndef __ZE_CRASHREPORT_CRASHMAINWINDOW_H__
#define __ZE_CRASHREPORT_CRASHMAINWINDOW_H__

#include <QtGui/QMainWindow>
#include <QtGui/QMessageBox>
#include <QtGui/QLayout>
#include <QtGui/QSystemTrayIcon>
#include <QtGui/QMenu>

#include "ZECrashReportUIClickableLabel.h"
#include "ZECrashReportUIProgressWindow.h"
#include "ZECrashReportUIReportDialog.h"
#include "ZECrashReportUIPrivacyDialog.h"
#include "ZECrashReport/ZECrashReport.h"
#include "ZECrashReport/ZECrashReportUserCommentProvider.h"
#include "ZEDS/ZEString.h"

namespace Ui
{
	class CrashMainWindowUI;
}

enum ZECrashReportUIWindowState
{
	ZE_CRUI_WINDOWSTATE_CRASHMAINWINDOW,
	ZE_CRUI_WINDOWSTATE_PROGRESSWINDOW
};

class ZECrashReportUIMainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		ZECrashReport						CrashReport;
		ZEString							UploadUrl;
		
											ZECrashReportUIMainWindow(QWidget *Parent = 0, Qt::WFlags Flags = 0);
											ZECrashReportUIMainWindow(ZECrashReport& CrashReport, const ZEString& UploadURL);
											~ZECrashReportUIMainWindow();	

	private:		
		Ui::CrashMainWindowUI*				CrashMainWindow;
		ZECrashReportUIClickableLabel*		plblViewReport;
		ZECrashReportUIClickableLabel*		plblViewPrivacyPolicy;

		ZECrashReportUIProgressWindow*		ProgressWindow;		
		ZECrashReportUIReportDialog*		ReportDialog;
		ZECrashReportUIPrivacyDialog*		PrivacyDialog;

		ZECrashReportUserCommentProvider*	UserCommand;

		QMenu*								TrayIconMenu;
		QSystemTrayIcon*					TrayIcon;

		ZECrashReportUIWindowState			UiWindowState;

		void								CreateMainWindowRawText();
		void								CreateTrayIcon();
		void								InitializeMainWindow();
		void								InitializeLinkButtons();		
		void								InitializeProgressWindow();
		void								InitializePrivacyDialog();
		void								InitializeReportDialog();
		void								GenerateUserCommentProvider();				
		bool								eventFilter(QObject* Obj, QEvent* Event);

	public slots:
		void								UpdateUserCommand();
		void								btnCloseClicked();
		void								ShowMainWindow();
		void								ShowPrivacyPolicy();
		void								HidePrivacyPolicy();
		void								ShowReport();
		void								HideReport();
		void								ShowProgress();
		void								TrayRestore();		
};

#endif
