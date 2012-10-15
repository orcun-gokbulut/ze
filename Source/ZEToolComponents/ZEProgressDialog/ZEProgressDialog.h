//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEProgressDialog.h
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
#ifndef	__ZE_PROGRESS_DIALOG_H__
#define __ZE_PROGRESS_DIALOG_H__

#include "QtGui\QDialog"
#include "ZEDS\ZEString.h"
#include "ZEError.h"
#include "ZEDS\ZEArray.h"

namespace Ui
{
	class ZEProgressDialogUI;
}

class ZEProgressDialogTask;
class QTreeWidgetItem;
class ZEProgressDialogTreeWidget;
class ZEFile;

class ZEProgressDialog
{
	friend class ZEProgressDialogSignalHandler;

	private:

		static ZEProgressDialog*		Instance;
		bool							QApplicationCreated;
		bool							IsWaitForClose;
		bool							Canceled;

		QApplication*					Application;
		ZEProgressDialogSignalHandler*	SignalHandler;
		ZEArray<ZEProgressDialogTask*>	Tasks;

		QDialog*						Dialog;
		ZEProgressDialogTreeWidget*		TasksTreeWidget;
		Ui::ZEProgressDialogUI*			Form;

		ZEProgressDialogTask*			RootTask;
		ZEProgressDialogTask*			CurrentTask;

		ZEErrorCallback					OldErrorCallback;
		ZELogCallback					OldLogCallBack;

		bool							IsFileLoggingEnabled;
		ZEFile*							LogFile;
		ZEString						LogFilePath;

										ZEProgressDialog();

	public:

		void							SetTitle(const ZEString& Title);

		void							SetProgressBarVisibility(bool IsVisible);
		bool							GetProgressBarVisibility();

		void							SetHeaderVisibility(bool IsVisible);
		bool							GetHeaderVisibility();

		void							SetLogFilePath(const ZEString& FilePath);
		const ZEString&					GetLogFilePath();

		void							SetFileLoggingEnabled(bool Enabled);
		bool							GetFileLoggingEnabled() const;

 		void							Start(); 
		ZEProgressDialogTask*			OpenTask(const ZEString& Name, bool IsTitle = false);
		void							CloseTask();
		void							End();

		void							TaskSucceded();
		void							TaskFailed();
		void							SetTaskProgress(ZEInt8 Progress);

 		void							Message(ZELogType Type, const char* Text);
 		void							WaitForClose(bool Enable);

		bool							IsCanceled();

		static ZEProgressDialog*		GetInstance();
		static ZEProgressDialog*		CreateInstance();
											
										~ZEProgressDialog();
};

#endif
