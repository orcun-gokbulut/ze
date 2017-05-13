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
#include "ZECrashReport/ZECRSender.h"

#include "ZEGUID.h"
#include "ZETimeStamp.h"
#include "ZEDS/ZEDelegate.h"
#include "ZEDS/ZEFormat.h"
#include "ZEThread/ZELock.h"
#include "ZECompression/ZECompressorZLIB.h"

#include <QDir>
#include <QMessageBox>

void ZECRWindowTransfering::SendReport(ZEThread* Thread, void* Output)
{
	Form->lblOperation->setText("Transferring report...");
	if (!Sender.OpenConnection())
	{
		UpdateInformationTimer.stop();
		emit UploadError();
		return;
	}

	while (Sender.TransferChunk())
	{
		//Progress = Sender.GetProgress();
	}

	Form->lblOperation->setText("Report transfered");
	Sender.CloseConnection();	
}

void ZECRWindowTransfering::Activated()
{
	Sender.SetUploadURL(UploadURL);
	Sender.SetFileName(GetWindow()->GetCrashReport()->GetReportFileName());

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
	ZECRSenderProgress Progress = Sender.GetProgress();
	if (Progress.Status == ZECR_SS_DONE)
	{
		UploadCompleted();
	}
	else if (Progress.Status == ZECR_SS_ERROR)
	{
		UploadError();
	}
	else
	{
		Form->lblTransferSpeed->setText(ZECRSender::FormatTransferSpeed(Progress.UploadedBytesPerSeconds).ToCString());
		Form->lblEstimatedTime->setText(
			ZECRSender::FormatEstimatedTime(
				Progress.UpdloadSize - Progress.UploadedBytes, 
				Progress.UploadedBytesPerSeconds).ToCString());
		Form->pbProgress->setValue(Progress.UploadedBytes / Progress.UpdloadSize);
	}

	UpdateInformationTimer.start();
}

void ZECRWindowTransfering::UploadCompleted()
{
	UpdateInformationTimer.stop();
	Form->pbProgress->setValue(100);
	Form->lblTransferSpeed->setText("");
	Form->lblEstimatedTime->setText("");	
	Form->btnCancel->setVisible(true);
}

ZECRWindowTransfering::ZECRWindowTransfering(QWidget* Parent) : ZECRWindowPage(Parent)
{
	Form = new Ui_ZECRWindowTransfering();
	Form->setupUi(this);

	Form->pbProgress->setValue(0);
	Form->lblEstimatedTime->setText(" Calculating...");
	Form->lblTransferSpeed->setText(" Connecting to server...");

	QObject::connect(Form->btnCancel, SIGNAL(clicked()), this, SLOT(btnCancel_Clicked()));
}

ZECRWindowTransfering::~ZECRWindowTransfering()
{
	delete Form;
}
