//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEProgressDialogTask.h
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
#ifndef	__ZE_PROGRESS_DIALOG_TASK_H__
#define __ZE_PROGRESS_DIALOG_TASK_H__

#include "ZETypes.h"
#include "ZEDS\ZEString.h"
#include "ZEDS\ZEArray.h"

class QTreeWidget;
class QTreeWidgetItem;

enum ZEProgressDialogTaskState
{
	ZE_PDTS_PENDING,
	ZE_PDTS_OK,
	ZE_PDTS_WARNING,
	ZE_PDTS_ERROR
};

class ZEProgressDialogTask
{
	friend class ZEProgressDialog;
	friend class ZEProgressDialogSignalHandler;

	private:

		ZEProgressDialogTask*			ParentTask;
		ZEArray<ZEProgressDialogTask*>	SubTasks;

		ZEString						Name;
		ZEString						TaskLog;
		ZEInt8							TaskPersentage;
		ZEProgressDialogTaskState		State;
		bool							IsTitle;
		bool							IsLogVisible;

		QTreeWidgetItem*				TreeItem;
		QTreeWidgetItem*				LogItem;

		void							SetState(ZEProgressDialogTaskState State);
		ZEProgressDialogTaskState		GetState() const;

		void							SetParentTask(ZEProgressDialogTask* ParentTask);
		ZEProgressDialogTask*			GetParentTask() const;

		void							AddSubTask(ZEProgressDialogTask* SubTask);
		ZEArray<ZEProgressDialogTask*>&	GetSubTasks();

		void							SetUpTreeItem();

		void							SetIsTitle(bool IsTitle);

										ZEProgressDialogTask(QTreeWidget* Parent);
										ZEProgressDialogTask(QTreeWidget* Parent, const ZEString& Name);
										ZEProgressDialogTask(QTreeWidgetItem* Parent, const ZEString& Name);

										~ZEProgressDialogTask();

	public:

		bool							GetIsTitle() const;

		void							SetName(const ZEString& Name);
		const ZEString&					GetName() const;

		void							AppendLog(const ZEString& Text);
		void							ClearLog();
		const ZEString&					GetLog() const;

		void							SetValue(ZEInt8 Value);
		ZEInt8							GetValue() const;

		void							SetLogVisible(bool IsVisible);
		bool							GetLogVisible() const;

		QTreeWidgetItem*				GetQTreeWidgetItem() const;
};

#endif
