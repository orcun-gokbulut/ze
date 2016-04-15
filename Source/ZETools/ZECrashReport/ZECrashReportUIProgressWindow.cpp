//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECrashReportUIProgressWindow.cpp
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

#include "ZECrashReportUIProgressWindow.h"
#include "ui_ProgressWindow.h"
#include "ZECrashReportUIMainWindow.h"


#include "ZECrashReportUploadThread.h"

#include "ZEDS/ZEDelegate.h"
#include "ZECompression/ZECompressorZLIB.h"
#include "ZECrashReport/ZECrashReportPackager.h"
#include "ZECrashReport/ZECrashReportSender.h"
#include "ZEGUID.h"
#include "ZEDS/ZEFormat.h"
#include "ZEThread/ZELock.h"

ZECrashReportUIProgressWindow::ZECrashReportUIProgressWindow(QWidget* Parent, Qt::WindowFlags Flags) : QMainWindow(Parent, Flags)
{
	
}

ZECrashReportUIProgressWindow::ZECrashReportUIProgressWindow(QMainWindow* ParentWidget, ZECrashReport* CrashReport, const ZEString& UploadURL)
{
	ProgressWindow = new Ui::ProgressWindowUI();
	MainWindow = (ZECrashReportUIMainWindow*)ParentWidget;
	this->CrashReport = CrashReport;
	this->UploadURL = UploadURL;

	ProgressWindow->setupUi(this);

	this->setFixedSize(480, 400);
	this->setStatusBar(NULL);

	ProgressWindow->progBar->setValue(0);
	ProgressWindow->btnCLose->setVisible(false);
	ProgressWindow->lblEstimatedTime->setText(" Calculating...");
	ProgressWindow->lblTransferSpeed->setText(" Connecting to server...");

	CreateMainWindowRawText();
	InitializeLinkButtons();

	QObject::connect(ProgressWindow->btnCancel, SIGNAL(clicked()), this, SLOT(btnCancel_Clicked()));
	QObject::connect(ProgressWindow->btnSendInBack, SIGNAL(clicked()), this, SLOT(btnSendInBack_Clicked()));
	QObject::connect(ProgressWindow->btnCLose, SIGNAL(clicked()), this, SLOT(btnCloseClicked()));
	QObject::connect(this, SIGNAL(ProgressCanceled()), MainWindow, SLOT(ShowMainWindow()));
	installEventFilter(this);
}

ZECrashReportUIProgressWindow::~ZECrashReportUIProgressWindow()
{
	
}

void ZECrashReportUIProgressWindow::InitializeLinkButtons()
{	
	plblViewReport = new ZECrashReportUIClickableLabel("View Report");
	plblViewReport->setObjectName(QString::fromUtf8("lblViewReport"));
	QObject::connect(this->plblViewReport, SIGNAL(clicked()), MainWindow, SLOT(ShowReport()));
	ProgressWindow->verticalLayout_4->addWidget(plblViewReport);

	plblViewPrivacyPolicy = new ZECrashReportUIClickableLabel("View Privacy Policy");
	plblViewPrivacyPolicy->setObjectName(QString::fromUtf8("lblViewPrivacyPolicy"));
	QObject::connect(this->plblViewPrivacyPolicy, SIGNAL(clicked()), MainWindow, SLOT(ShowPrivacyPolicy()));
	ProgressWindow->verticalLayout_4->addWidget(plblViewPrivacyPolicy);
}

void ZECrashReportUIProgressWindow::UploadError()
{
	if (this->isHidden())
		this->showNormal();

	QMessageBox* messageBox = new QMessageBox(QMessageBox::Critical, "Zinek Crash Manager", 
		"There is an error occurred while transferring crash report file.\n", 
		QMessageBox::Ok, this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

	int status = messageBox->exec();

	switch (status)
		case QMessageBox::Ok:
		{		
			emit ProgressCanceled();
		}
}

void ZECrashReportUIProgressWindow::btnCancel_Clicked()
{
	QMessageBox* messageBox = new QMessageBox(QMessageBox::Question, "Zinek Crash Manager", 
		"Current progress will be lost. Do you want to continue ?", 
		QMessageBox::No | QMessageBox::Yes, this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

	int status = messageBox->exec();

	switch (status)
		case QMessageBox::Yes:
		{
			TerminateProcess();
			emit ProgressCanceled();
		}
}

void ZECrashReportUIProgressWindow::btnSendInBack_Clicked()
{
	this->hide();	
}

void ZECrashReportUIProgressWindow::CreateMainWindowRawText()
{
	QList<QLabel*> LabelList;

	QString LabelText = "Thank you for your patience and interest.";
	LabelList.append(new QLabel(LabelText));
	QFont TempFont;
	TempFont.setBold(true);
	LabelList[LabelList.count() - 1]->setFont(TempFont);

	LabelText = "Sending the crash report to our crash report servers. This report is going to be used for engineering purposes only. "
		+ QString("It will help our development team to analyze and fix the problem in order to create better product.");

	LabelList.append(new QLabel(LabelText));
	LabelList[LabelList.count() - 1]->setWordWrap(true);

	LabelText = "You can click the \"Send in background\" button to close this dialog box. It will continue transferring the report in background and will inform you when it is done.";
	LabelList.append(new QLabel(LabelText));
	LabelList[LabelList.count() - 1]->setWordWrap(true);

	for (int I = 0; I < LabelList.count(); I++)
		ProgressWindow->InformationTextLayout->addWidget(LabelList[I]);
}

void ZECrashReportUIProgressWindow::UpdateUploadInformation()
{
	if(SenderThread->GetStatus() == ZE_TS_NOT_RUNNING)
	{
		UploadCompleted();
		return;
	}

	ZESize LastUploadSize = ProgressInformation.UploadedSize;

	Sender.GetProgressInformation(ProgressInformation);

	if(ProgressInformation.UploadedSize != 0)
	{
		ZEString TransferSpeedText;
		float TransferSpeed;
		ZESize ApproximateBytePerSec = (ProgressInformation.UploadedSize - LastUploadSize) * 10;		

		if(ApproximateBytePerSec > 1048576)
		{
			TransferSpeed = ((float)ApproximateBytePerSec / 1048576.0f) * 100.0f;
			TransferSpeedText = ZEFormat::Format("{0}.{1} MB/sec", (int)TransferSpeed / 100, (int)TransferSpeed % 100);
		}
		else if(ApproximateBytePerSec > 1024)
		{
			TransferSpeed = ((float)ApproximateBytePerSec / 1024.0f) * 100.0f;
			TransferSpeedText = ZEFormat::Format("{0}.{1} KB/sec", (int)TransferSpeed / 100, (int)TransferSpeed % 100);
		}
		else
		{
			TransferSpeed = ApproximateBytePerSec;
			TransferSpeedText = ZEFormat::Format("{0} B/sec", (int)TransferSpeed);
		}

		ZESize EstimatedSeconds = ProgressInformation.TotalUploadSize - ProgressInformation.UploadedSize;

		if(EstimatedSeconds != 0 && ApproximateBytePerSec != 0)
			EstimatedSeconds /= ApproximateBytePerSec;

		ZEString EstimatedTimeText;

		if(EstimatedSeconds / 86400 > 0)
			EstimatedTimeText = ZEFormat::Format("{0} Days ", EstimatedSeconds / 86400);

		if(EstimatedSeconds % 86400 > 3600)
			EstimatedTimeText += ZEFormat::Format("{0} Hours ", (EstimatedSeconds % 86400) / 3600);

		if(EstimatedSeconds % 3600 > 60)
			EstimatedTimeText += ZEFormat::Format("{0} Minutes ", (EstimatedSeconds % 3600) / 60);

		EstimatedTimeText += ZEFormat::Format("{0} Seconds", EstimatedSeconds % 60 );

		ProgressWindow->lblTransferSpeed->setText(TransferSpeedText.ToCString());
		ProgressWindow->lblEstimatedTime->setText(EstimatedTimeText.ToCString());
		ProgressWindow->progBar->setValue((int)ProgressInformation.ProcessPercentage);
	}

	UpdateInformationTimer.start();
}

void ZECrashReportUIProgressWindow::UploadCompleted()
{
	UpdateInformationTimer.stop();
	ProgressWindow->progBar->setValue(100);
	ProgressWindow->lblTransferSpeed->setText("0 B/sec");
	ProgressWindow->lblEstimatedTime->setText("0 Seconds");	
	ProgressWindow->btnCLose->setVisible(true);
}

void ZECrashReportUIProgressWindow::btnCloseClicked()
{	
	remove(FileName);	
	qApp->quit();
}

bool ZECrashReportUIProgressWindow::eventFilter(QObject* Obj, QEvent* Event)
{
	if (Event->type() == QEvent::Close)
	{
		Event->ignore();
		btnCancel_Clicked();
		return true;
	}
	return QMainWindow::eventFilter(Obj, Event);
}

void ZECrashReportUIProgressWindow::InitializeSenderThread()
{
	SenderThread = new ZEThread();
	SenderThread->SetName("SenderThread");
	SenderThread->SetParameter(NULL);
	SenderThread->SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<ZECrashReportUIProgressWindow, &ZECrashReportUIProgressWindow::SendReport>(this));
}

void ZECrashReportUIProgressWindow::InitializeSender()
{
	Sender.SetUploadURL(UploadURL);
	Sender.SetFileName(FileName);
}

bool ZECrashReportUIProgressWindow::PackageItems()
{
	ZEString TempPath = QDir::temp().tempPath().toStdString();
	TempPath += "/" + ZEGUID::Generate().ToString() + ".zePacked";

	ZECrashReportPackager* Packager = new ZECrashReportPackager();
	Packager->SetCrashReport(CrashReport);
	Packager->SetOutputFileName(TempPath);
	FileName = Packager->GetOutputFileName();
	return Packager->Pack();
}

bool ZECrashReportUIProgressWindow::CompressPackage()
{
	ZEString TempPath = QDir::temp().tempPath().toStdString();
	TempPath += "/" + ZEGUID::Generate().ToString() + ".zeCompressed";
	
	FILE* PackedFile = fopen(FileName,"rb");
	FILE* CompressedFile = fopen(TempPath.ToCString(), "wb");

	if(!PackedFile)
		return false;

	fseek(PackedFile, 0, SEEK_END);
	ZESize PackedFileSize = ftell(PackedFile);
	rewind(PackedFile);

	void* InputBuffer = new char[2048];
	void* OutputBuffer = new char[2048];

	ZECompressorZLIB* Compressor = new ZECompressorZLIB();
	Compressor->Reset();
	Compressor->SetInput(InputBuffer);
	Compressor->SetInputSize(2048);
	Compressor->SetOutput(OutputBuffer);
	Compressor->SetOutputSize(2048);

	bool ProcessError = false;
	bool Reading = true;
	while(Reading)
	{
		if(ProcessError)
			Reading = false;

		ZESize Readed = fread(InputBuffer, sizeof(char), 2048, PackedFile);

		if(Readed != 2048)
		{
			if(feof(PackedFile))
			{
				Compressor->SetEos(true);
				Compressor->SetInputSize(Readed);
				Compressor->Compress();
				fwrite(OutputBuffer, sizeof(char), Compressor->GetOutputSize(), CompressedFile);
				
				while(Compressor->GetState() == ZE_CS_OUTPUT_FULL)
				{
					Compressor->Compress();

					fwrite(OutputBuffer, sizeof(char), Compressor->GetOutputSize(), CompressedFile);
				}
				
				Reading = false;
				continue;
			}
		}

		Compressor->Compress();

		if(Compressor->GetState() == ZE_CS_OUTPUT_FULL)
			fwrite(OutputBuffer, sizeof(char), Compressor->GetOutputSize(), CompressedFile);

		if(Compressor->GetState() == ZE_CS_ERROR)
			ProcessError = true;
	}

	fclose(CompressedFile);
	fclose(PackedFile);
	remove(FileName);

	FileName = TempPath;

	if(!ProcessError)
		return true;
	else			
		return false;
}

void ZECrashReportUIProgressWindow::SendReport(ZEThread* Thread, void* Output)
{
	ProgressWindow->lblProgress->setText("Packing items..");
	if(!PackageItems())
	{
		remove(FileName);
		emit UploadError();
		return;
	}

	ProgressWindow->lblProgress->setText("Compressing package..");
	if(!CompressPackage())
	{
		remove(FileName);
		emit UploadError();
		return;
	}

	InitializeSender();	

	ProgressWindow->lblProgress->setText("Sending report..");
	if(!Sender.OpenConnection())
	{
		UpdateInformationTimer.stop();
		remove(FileName);
		emit UploadError();
		return;
	}

	ProgressWindow->lblProgress->setText("Zinek Crash Manager successfully uploaded report.");
	Sender.CloseConnection();	
}

void ZECrashReportUIProgressWindow::Process()
{
	InitializeSenderThread();
	SenderThread->Run();

	UpdateInformationTimer.setInterval(100);
	QObject::connect(&UpdateInformationTimer, SIGNAL(timeout()), this, SLOT(UpdateUploadInformation()));
	UpdateInformationTimer.start();
}

void ZECrashReportUIProgressWindow::TerminateProcess()
{
	SenderThread->Terminate();	
}

