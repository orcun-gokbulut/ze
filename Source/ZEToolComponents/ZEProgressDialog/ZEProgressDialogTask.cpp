//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEProgressDialogTask.cpp
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

#include "ZEProgressDialogTask.h"
#include "QtGui\QTreeWidget"
#include "QtGui\qtreewidget.h"
#include "QtGui\qcolor.h"

void ZEProgressDialogTask::SetUpTreeItem()
{
	TreeItem->setExpanded(true);
	SetState(ZE_PDTS_PENDING);
	LogItem->setHidden(true);
	IsLogVisible = false;
}

ZEProgressDialogTask::ZEProgressDialogTask(QTreeWidget* Parent, const ZEString& Name)
{
	this->Name = Name;
	TreeItem = new QTreeWidgetItem(Parent);
	LogItem = new QTreeWidgetItem(TreeItem);
	TreeItem->setText(0, this->Name.ToCString());
	SetValue(0);
	ClearLog();
	State = ZE_PDTS_PENDING;
	ParentTask = NULL;
	SubTasks.Clear();
	SetUpTreeItem();
}

ZEProgressDialogTask::ZEProgressDialogTask(QTreeWidgetItem* Parent, const ZEString& Name)
{
	this->Name = Name;
	TreeItem = new QTreeWidgetItem(Parent);
	LogItem = new QTreeWidgetItem(TreeItem);
	TreeItem->setText(0, this->Name.ToCString());
	SetValue(0);
	ClearLog();
	State = ZE_PDTS_PENDING;
	ParentTask = NULL;
	SubTasks.Clear();
	SetUpTreeItem();
}

ZEProgressDialogTask::ZEProgressDialogTask(QTreeWidget* Parent)
{
	TreeItem = new QTreeWidgetItem(Parent);
	LogItem = new QTreeWidgetItem(TreeItem);
	SetValue(0);
	ClearLog();
	State = ZE_PDTS_PENDING;
	ParentTask = NULL;
	SubTasks.Clear();
	SetUpTreeItem();
}

ZEProgressDialogTask::~ZEProgressDialogTask()
{
	TreeItem->setHidden(true);
	delete TreeItem;
	TreeItem = NULL;

	LogItem->setHidden(true);
	delete LogItem;
	LogItem = NULL;

	ClearLog();
	Name.Clear();
}

void  ZEProgressDialogTask::SetState(ZEProgressDialogTaskState State)
{
	if(State == ZE_PDTS_PENDING)
		TreeItem->setBackgroundColor(1, QColor(184, 204, 228));
	else if(State == ZE_PDTS_WARNING)
		TreeItem->setBackgroundColor(1, QColor(255, 235, 156));
	else if(State == ZE_PDTS_ERROR)
		TreeItem->setBackgroundColor(1, QColor(255, 199, 206));
	else if(State == ZE_PDTS_OK)
		TreeItem->setBackgroundColor(1, QColor(198, 239, 206));	

	this->State = State;
}

ZEProgressDialogTaskState ZEProgressDialogTask::GetState() const
{
	return State;
}

void ZEProgressDialogTask::SetIsTitle(bool IsTitle)
{
	this->IsTitle = IsTitle;

	if(IsTitle)
		TreeItem->setBackgroundColor(1, Qt::white);
	else
		SetState(ZE_PDTS_PENDING);

}

bool ZEProgressDialogTask::GetIsTitle() const
{
	return IsTitle;
}

void ZEProgressDialogTask::SetParentTask(ZEProgressDialogTask* ParentTask)
{
	this->ParentTask = ParentTask;
}

void ZEProgressDialogTask::SetName(const ZEString& Name)
{
	this->Name = Name;
	TreeItem->setText(0, Name.ToCString());
}

const ZEString&	ZEProgressDialogTask::GetName() const
{
	return Name;
}

void ZEProgressDialogTask::AppendLog(const ZEString& Text)
{
	TaskLog.Append(Text + "\n");
	LogItem->setText(0, QString(TaskLog.ToCString()));
}

void ZEProgressDialogTask::ClearLog()
{
	TaskLog.Clear();
	LogItem->setText(0, "");
	SetLogVisible(false);
}

const ZEString& ZEProgressDialogTask::GetLog() const
{
	return TaskLog;
}

void ZEProgressDialogTask::SetValue(ZEInt8 Value)
{
	TaskPersentage = Value;
	
	if(Value > 0)
		TreeItem->setText(1, (ZEString(Value) + "%").ToCString());
}

ZEInt8 ZEProgressDialogTask::GetValue() const
{
	return TaskPersentage;
}

ZEProgressDialogTask* ZEProgressDialogTask::GetParentTask() const
{
	return ParentTask;
}

QTreeWidgetItem* ZEProgressDialogTask::GetQTreeWidgetItem() const
{
	return TreeItem;
}

void  ZEProgressDialogTask::AddSubTask(ZEProgressDialogTask* SubTask)
{
	SubTasks.Add(SubTask);
	SubTask->SetParentTask(this);
}

ZEArray<ZEProgressDialogTask*>& ZEProgressDialogTask::GetSubTasks()
{
	return SubTasks;
}

void ZEProgressDialogTask::SetLogVisible(bool IsVisible)
{
	IsLogVisible = IsVisible;

	if(TaskLog.GetLength() != 0)
		LogItem->setHidden(!IsLogVisible);
}

bool ZEProgressDialogTask::GetLogVisible() const
{
	return IsLogVisible;
}
