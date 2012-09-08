//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEProgressDialog.cpp
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

#include "ZETypes.h"
#include "ZEProgressDialog.h"
#include "ui_ZEProgressDialog.h"
#include "ZEProgressDialogTask.h"
#include "ZEProgresDialogSignalHandler.h"
#include "ZEProgressDialogTreeWidget.h"

void LogCallback(const char* Module, ZELogType Type, const char* LogText)
{
	ZEProgressDialog::GetInstance()->Message(Type, LogText);
}

ZEProgressDialog* ZEProgressDialog::Instance = NULL;

ZEProgressDialog::ZEProgressDialog()
{
	Instance = this;
	QApplicationCreated = false;

	if(QApplication::instance() == NULL)
	{
		ZEInt32	Argc = 0;
		void*	Argv = NULL;
		Application = new QApplication(Argc, NULL);
		QApplicationCreated = true;
	}

	Dialog = new QDialog();
	Form = new Ui::ZEProgressDialogUI();
	Form->setupUi(Dialog);
	
	Canceled = false;
	IsWaitForClose = true;
	TasksTreeWidget = new ZEProgressDialogTreeWidget(Form->tab);
	Form->tab->layout()->addWidget(TasksTreeWidget);

	RootTask = NULL;
	CurrentTask = NULL;
	SignalHandler = new ZEProgressDialogSignalHandler(this, this->Dialog);

	OldErrorCallback = ZEError::GetInstance()->GetCallBack();
	OldLogCallBack = ZELog::GetInstance()->GetCallback();

	ZELog::GetInstance()->SetCallback(LogCallback);
	ZEError::GetInstance()->SetCallback(NULL);
}

ZEProgressDialog::~ZEProgressDialog()
{
	Instance = NULL;
	ZELog::GetInstance()->SetCallback(OldLogCallBack);
	ZEError::GetInstance()->SetCallback(OldErrorCallback);
	TasksTreeWidget->hide();
	delete TasksTreeWidget;
	TasksTreeWidget = NULL;

	if(QApplicationCreated)
	{
		Application->quit();
		delete Application;
	}
}

void ZEProgressDialog::SetTitle(const ZEString& Title)
{
	Dialog->setWindowTitle(Title.ToCString());
}

ZEProgressDialogTask* ZEProgressDialog::OpenTask(const ZEString& Name, bool IsTitle)
{
	ZEProgressDialogTask* NewTask = NULL;

	if(RootTask == NULL)
	{
		NewTask = new ZEProgressDialogTask(TasksTreeWidget, Name);
		RootTask = NewTask;
		TasksTreeWidget->addTopLevelItem(RootTask->GetQTreeWidgetItem());
		CurrentTask = RootTask;
	}
	else
	{
		NewTask = new ZEProgressDialogTask(CurrentTask->GetQTreeWidgetItem(), Name);
		CurrentTask->AddSubTask(NewTask);
		CurrentTask->GetQTreeWidgetItem()->addChild(NewTask->GetQTreeWidgetItem());
		CurrentTask = NewTask;
	}

	NewTask->SetIsTitle(IsTitle);
	Form->lblCurrentTaskName->setText(CurrentTask->GetName().ToCString());
	Tasks.Add(NewTask);
	
	return NewTask;
}

void ZEProgressDialog::CloseTask()
{
	if(CurrentTask == RootTask && CurrentTask->GetParentTask() != NULL)
		return;

	if(CurrentTask->GetState() == ZE_PDTS_PENDING && !CurrentTask->GetIsTitle())
		CurrentTask->SetState(ZE_PDTS_OK);

	CurrentTask = CurrentTask->GetParentTask();
}

void ZEProgressDialog::Start()
{
	Dialog->setWindowModality(Qt::WindowModality::WindowModal);
	Dialog->show();
}

void ZEProgressDialog::End()
{
	Form->buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Close);
	Form->lblCurrentTaskName->setText("Finished");

	if(!IsWaitForClose)
		Dialog->close();
}

void ZEProgressDialog::TaskSucceded()
{
	CurrentTask->SetState(ZE_PDTS_OK);
}

void ZEProgressDialog::TaskFailed()
{
	CurrentTask->SetState(ZE_PDTS_ERROR);
}

void ZEProgressDialog::SetTaskProgress(ZEInt8 Progress)
{
	CurrentTask->SetValue(Progress);
}

void ZEProgressDialog::Message(ZELogType Type, const char* Text)
{
	if(Type == ZE_LOG_WARNING && CurrentTask->GetState() != ZE_PDTS_ERROR)
		CurrentTask->SetState(ZE_PDTS_WARNING);
	if(Type == ZE_LOG_ERROR)
		CurrentTask->SetState(ZE_PDTS_ERROR);
	if(Type == ZE_LOG_CRITICAL_ERROR)
		CurrentTask->SetState(ZE_PDTS_ERROR);

	CurrentTask->AppendLog(Text);
	Form->txtLog->append(Text);
}

ZEProgressDialog* ZEProgressDialog::GetInstance()
{
	return Instance;
}

void ZEProgressDialog::SetProgressBarVisibility(bool IsVisible)
{
	Form->progressBar->setVisible(IsVisible);
	Form->lblProgress->setVisible(IsVisible);
}

bool ZEProgressDialog::GetProgressBarVisibility()
{
	return Form->progressBar->isVisible();
}

void ZEProgressDialog::SetHeaderVisibility(bool IsVisible)
{
	Form->lblHeader->setVisible(IsVisible);
}

bool ZEProgressDialog::GetHeaderVisibility()
{
	return Form->lblHeader->isVisible();
}

void ZEProgressDialog::WaitForClose(bool Enable)
{
	IsWaitForClose = Enable;
}

bool ZEProgressDialog::IsCanceled()
{
	return Canceled;
}
