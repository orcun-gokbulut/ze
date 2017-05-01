//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRTransferThread.cpp
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

#include "ZECRTransferThread.h"

#include "ZECrashReport/ZECRPackager.h"
#include "ZECrashReport/ZECRSender.h"

#include "ZETimeStamp.h"
#include "ZEDS/ZEFormat.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZECompression/ZECompressorZLIB.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void ZECRTransferThread::run()
{
	SendReport();	
}

ZECRTransferThread::ZECRTransferThread(ZECRCrashReport& CrashReport)
{
	this->CrashReport = &CrashReport;		
}

ZECRTransferThread::~ZECRTransferThread()
{

}

void ZECRTransferThread::SendReport()
{
	if(!PackageItems())
	{
		emit UploadError();
		return;
	}

	if(!CompressPackage())
	{		
		emit UploadError();
		return;
	}

	ZECRSender* Sender = new ZECRSender();
	Sender->SetFileName(FileName);
	Sender->SetUploadURL(UploadURL);
	if(!Sender->OpenConnection())
	{
		CleanUp();
		emit UploadError();
		return;
	}
	Sender->CloseConnection();	
	remove(FileName);	
	emit UploadCompleted();
}

bool ZECRTransferThread::PackageItems()
{
	char ComputerName[256];
	DWORD Size = sizeof(ComputerName);
	GetComputerNameA(ComputerName, &Size);
	FileName = ZEFormat::Format("#S:/CrashReports/{0}-{1}.ZECrashReport", ComputerName, ZETimeStamp::Now().ToString("%Y%m%d-%H%M%S"));

	ZECRPackager* Packager = new ZECRPackager();
	Packager->SetCrashReport(CrashReport);
	Packager->SetOutputFileName(ZEFormat::Format("{0}.uncompressed", FileName));

	FileName = Packager->GetOutputFileName();
	return Packager->Pack();
}

void ZECRTransferThread::CleanUp()
{
	ZEFileInfo PackedFileInfo(ZEFormat::Format("{0}.uncompressed", FileName));
	PackedFileInfo.Delete();
}

void ZECRTransferThread::SetUploadURL(const char* UploadURL)
{
	this->UploadURL = UploadURL;
}

const char* ZECRTransferThread::GetUploadURL()
{
	return UploadURL;
}
