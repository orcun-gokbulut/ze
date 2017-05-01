//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRWindowTransfering.cpp
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

#include "ZECRWindowTransfering.h"

#include "Ui_ZECRWindowTransfering.h"

#include "ZECRWindow.h"
#include "ZECrashReport/ZECRPackager.h"
#include "ZECrashReport/ZECRSender.h"

#include "ZEGUID.h"
#include "ZEDS/ZEDelegate.h"
#include "ZEDS/ZEFormat.h"
#include "ZEThread/ZELock.h"
#include "ZECompression/ZECompressorZLIB.h"

#include <QDir>
#include <QMessageBox>

void ZECRWindowTransfering::SendReport(ZEThread* Thread, void* Output)
{
	Form->lblOperation->setText("Packing items...");
	if (!PackageItems())
	{
		remove(FileName);
		emit UploadError();
		return;
	}

	Form->lblOperation->setText("Transferring report...");
	if (!Sender.OpenConnection())
	{
		UpdateInformationTimer.stop();
		remove(FileName);
		emit UploadError();
		return;
	}

	while (Sender.TransferChunk())
	{
		Sender.GetProgress(ProgressInformation);
	}

	Form->lblOperation->setText("Report transfered");
	Sender.CloseConnection();	
}

void ZECRWindowTransfering::Activated()
{
	Sender.SetUploadURL(UploadURL);
	Sender.SetFileName(FileName);

	SenderThread.SetName("SenderThread");
	SenderThread.SetParameter(NULL);
	SenderThread.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<ZECRWindowTransfering, &ZECRWindowTransfering::SendReport>(this));
	SenderThread.Run();

	UpdateInformationTimer.setInterval(100);
	QObject::connect(&UpdateInformationTimer, SIGNAL(timeout()), this, SLOT(UpdateUploadInformation()));
	UpdateInformationTimer.start();
}


void ZECRWindowTransfering::UploadError()
{
	QMessageBox::critical(this, "ZECRCrashReporter", "There is an error occurred while transferring crash report package.\n", QMessageBox::Ok);
	qApp->exit(EXIT_FAILURE);
}

void ZECRWindowTransfering::btnCancel_Clicked()
{
	int Result = QMessageBox::question(this, "ZECRCrashReporter", "Current progress will be lost. Do you want to continue ?", QMessageBox::Yes, QMessageBox::No);
	if (Result == QMessageBox::Yes)
		qApp->exit(EXIT_FAILURE);
}


void ZECRWindowTransfering::UpdateUploadInformation()
{

	ProgressInformation.stat
	if(SenderThread->GetStatus() == ZE_TS_NOT_RUNNING)
	{
		UploadCompleted();
		return;
	}

	ZESize LastUploadSize = ProgressInformation.UploadedSize;

	Sender.GetProgress(ProgressInformation);

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

		if (EstimatedSeconds != 0 && ApproximateBytePerSec != 0)
			EstimatedSeconds /= ApproximateBytePerSec;

		ZEString EstimatedTimeText;

		if (EstimatedSeconds / 86400 > 0)
			EstimatedTimeText = ZEFormat::Format("{0} Days ", EstimatedSeconds / 86400);

		if (EstimatedSeconds % 86400 > 3600)
			EstimatedTimeText += ZEFormat::Format("{0} Hours ", (EstimatedSeconds % 86400) / 3600);

		if (EstimatedSeconds % 3600 > 60)
			EstimatedTimeText += ZEFormat::Format("{0} Minutes ", (EstimatedSeconds % 3600) / 60);

		EstimatedTimeText += ZEFormat::Format("{0} Seconds", EstimatedSeconds % 60 );

		Form->lblTransferSpeed->setText(TransferSpeedText.ToCString());
		Form->lblEstimatedTime->setText(EstimatedTimeText.ToCString());
		Form->progProgress->setValue((int)ProgressInformation.ProcessPercentage);
	}

	UpdateInformationTimer.start();
}

void ZECRWindowTransfering::UploadCompleted()
{
	UpdateInformationTimer.stop();
	Form->progProgress->setValue(100);
	Form->lblTransferSpeed->setText("0 B/sec");
	Form->lblEstimatedTime->setText("0 Seconds");	
	Form->btnCancel->setVisible(true);
}

bool ZECRWindowTransfering::PackageItems()
{
	ZEString TempPath = QDir::temp().tempPath().toStdString();
	TempPath += "/" + ZEGUID::Generate().ToString() + ".zePacked";

	ZECRPackager* Packager = new ZECRPackager();
	Packager->SetCrashReport(GetWindow()->GetCrashReport());
	Packager->SetOutputFileName(TempPath);
	FileName = Packager->GetOutputFileName();
	return Packager->Pack();
}

bool ZECRWindowTransfering::CompressPackage()
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



ZECRWindowTransfering::ZECRWindowTransfering(QWidget* Parent) : ZECRWindowPage(Parent)
{
	Form = new Ui_ZECRWindowTransfering();
	Form->setupUi(this);

	Form->progProgress->setValue(0);
	Form->lblEstimatedTime->setText(" Calculating...");
	Form->lblTransferSpeed->setText(" Connecting to server...");

	QObject::connect(Form->btnCancel, SIGNAL(clicked()), this, SLOT(btnCancel_Clicked()));
}

ZECRWindowTransfering::~ZECRWindowTransfering()
{

}
