//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECrashReportUploadThread.cpp
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

#include "ZECrashReportUploadThread.h"
#include "ZECrashReport/ZECrashReportPackager.h"
#include "ZECrashReport/ZECrashReportSender.h"
#include "ZECompression/ZECompressorZLIB.h"
#include <Windows.h>
#define WIN32_LEAN_AND_MEAN

void ZECrashReportUploadThread::run()
{
	SendReport();	
}

ZECrashReportUploadThread::ZECrashReportUploadThread(ZECrashReport& CrashReport)
{
	this->CrashReport = &CrashReport;		
}

ZECrashReportUploadThread::~ZECrashReportUploadThread()
{

}

void ZECrashReportUploadThread::SendReport()
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

	ZECrashReportSender* Sender = new ZECrashReportSender();
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

bool ZECrashReportUploadThread::PackageItems()
{
	ZECrashReportPackager* Packager = new ZECrashReportPackager();
	Packager->SetCrashReport(CrashReport);
	Packager->SetOutputFileName("C:/Users/onur.babaoglu/Desktop/PACKTRY/Packed.zeCrashReport");
	FileName = Packager->GetOutputFileName();
	return Packager->Pack();
}

bool ZECrashReportUploadThread::CompressPackage()
{
	FILE* PackedFile = fopen(FileName,"rb");
	FILE* CompressedFile = fopen("C:/Users/onur.babaoglu/Desktop/PACKTRY/Compressed.zeCrashReport", "wb");

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

		ZESize Readed = fread(InputBuffer, 1, 2048, PackedFile);

		if(Readed != 2048)
		{
			if(feof(PackedFile))
			{
				Compressor->SetEos(true);
				Compressor->SetInputSize(Readed);
				Compressor->Compress();
				fwrite(OutputBuffer, 1, Compressor->GetOutputSize(), CompressedFile);
				Reading = false;
				continue;
			}
		}
		
		Compressor->Compress();

		if(Compressor->GetState() == ZE_CS_OUTPUT_FULL)
			fwrite(OutputBuffer, 1, Compressor->GetOutputSize(), CompressedFile);

		if(Compressor->GetState() == ZE_CS_ERROR)
			ProcessError = true;
	}

	fclose(CompressedFile);
	fclose(PackedFile);
	remove(FileName);

	FileName = "C:/Users/onur.babaoglu/Desktop/PACKTRY/Compressed.zeCrashReport";

	if(!ProcessError)
		return true;
	else			
		return false;
}

void ZECrashReportUploadThread::CleanUp()
{
	remove(FileName);	
}

void ZECrashReportUploadThread::SetUploadURL(const char* UploadURL)
{
	this->UploadURL = UploadURL;
}

const char* ZECrashReportUploadThread::GetUploadURL()
{
	return UploadURL;
}
