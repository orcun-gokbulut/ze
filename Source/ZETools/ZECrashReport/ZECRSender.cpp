//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRSender.cpp
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

#include "ZECRSender.h"

#include "ZEDS/ZEDelegate.h"
#include "ZEDS/ZEFormat.h"

#define CURL_STATICLIB
#include <curl/curl.h>

void ZECRSender::ResetProgress()
{
	Progress.Status = ZECR_SS_NONE;
	Progress.UpdloadSize = 0;
	Progress.UploadedBytes = 0;
	Progress.UploadedBytesPerSeconds = 0.0f;
	Progress.DownloadSize = 0.0f;
	Progress.DownloadedBytes = 0;
	Progress.DownloadedBytesPerSeconds = 0.0f;
}

static size_t ReadFunction(void* Ouput, size_t Size, size_t Count, void* File)
{
	return static_cast<ZEFile*>(File)->Read(Ouput, Size, Count);	
}

int ZECRSender::ProgressFunction(void* Output, double TotalDownloadSize, double Downloaded, double TotalUploadSize, double Uploaded)
{
	ZECRSender* Sender = static_cast<ZECRSender*>(Output);
	if (Uploaded == 0)
		return 0;

	Sender->ProgressLock.Lock();
	{
		double ElapsedTime = Sender->Counter.GetTimeSeconds() - Sender->LastMeasuredTime;
		Sender->LastMeasuredTime = Sender->Counter.GetTimeSeconds();

		Sender->Progress.DownloadedBytesPerSeconds = (double)(Downloaded - Sender->Progress.DownloadedBytes) / ElapsedTime;
		Sender->Progress.DownloadSize = (ZESize)TotalDownloadSize;
		Sender->Progress.DownloadedBytes = (ZESize)Downloaded;

		Sender->Progress.DownloadedBytesPerSeconds = (double)(Uploaded - Sender->Progress.UploadedBytes) / ElapsedTime;
		Sender->Progress.UploadedBytes = (ZESize)TotalUploadSize;
		Sender->Progress.UploadedBytesPerSeconds = (ZESize)Uploaded;
	}
	Sender->ProgressLock.Unlock();

	return 0;
}

void ZECRSender::SetFileName(const char* FileName)
{
	this->FileName = FileName;
}

const char* ZECRSender::GetFileName()
{
	return FileName;
}

void ZECRSender::SetUploadURL(const char* URL)
{
	this->UploadURL = URL;
}

const char* ZECRSender::GetUploadURL()
{
	return UploadURL;
}

ZESize ZECRSender::GetFileSize()
{
	return FileSize;
}


bool ZECRSender::OpenConnection()
{
	ZEFile File;
	if (!File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE))
		return false;

	FileSize = File.GetSize();

	curl_global_init(CURL_GLOBAL_ALL);

	Curl = curl_easy_init();
	if (!Curl)
		return false;

	curl_easy_setopt((CURL*)Curl, CURLOPT_UPLOAD, 1L);
	curl_easy_setopt((CURL*)Curl, CURLOPT_URL, UploadURL.ToCString());
	curl_easy_setopt((CURL*)Curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)FileSize);	
	//curl_easy_setopt((CURL*)Curl, CURLOPT_CONNECT_ONLY, 1L);
	curl_easy_setopt((CURL*)Curl, CURLOPT_READFUNCTION, ReadFunction);
	curl_easy_setopt((CURL*)Curl, CURLOPT_READDATA, &File);
	curl_easy_setopt((CURL*)Curl, CURLOPT_PUT, 1L);
	curl_easy_setopt((CURL*)Curl, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt((CURL*)Curl, CURLOPT_PROGRESSFUNCTION, this->ProgressFunction);
	curl_easy_setopt((CURL*)Curl, CURLOPT_PROGRESSDATA, Progress);

	Counter.Start();
	CURLcode Result = curl_easy_perform(Curl);
	if (Result != CURLE_OK)
	{
		File.Close();
		return false;
	}

	File.Close();
	return true;
}

bool ZECRSender::TransferChunk()
{
	return false;

	ZEUInt8 Buffer[8192];
	ZESize BlockSize = FileSize - Progress.UploadedBytes > 8192 ? 8129 : FileSize - Progress.UploadedBytes;

	if (BlockSize == 0)
		return false;

	if (File.Read(Buffer, BlockSize, 1) != 1)
		return false;

	size_t BytesTransferred = 0;
	CURLcode Result = curl_easy_send((CURL*)Curl, Buffer, BlockSize, &BytesTransferred);
	if (Result != CURLE_OK)
		return false;


	Progress.UploadedBytes += BlockSize;
	return true;
}

void ZECRSender::CloseConnection()
{
	curl_easy_cleanup((CURL*)Curl);
	curl_global_cleanup();
}

ZEString ZECRSender::FormatTransferSpeed(ZESize BytesPerSecond)
{
	if (BytesPerSecond > ZE_SIZE_MB)
	{
		float TransferSpeed = (float)BytesPerSecond / (float)ZE_SIZE_MB;
		return ZEFormat::Format("{0}.{1} MB/sec", (int)TransferSpeed, (int)(TransferSpeed * 100.0f) % 100);
	}
	else if (BytesPerSecond > ZE_SIZE_KB)
	{
		float TransferSpeed = (float)BytesPerSecond / ZE_SIZE_KB;
		return ZEFormat::Format("{0}.{1} KB/sec", (int)TransferSpeed, (int)(TransferSpeed * 100) % 100);
	}
	else
	{
		return ZEFormat::Format("{0} B/sec", (int)BytesPerSecond);
	}
}

ZEString ZECRSender::FormatEstimatedTime(ZESize TotalSize, ZESize BytesPerSecond)
{
	if (BytesPerSecond == 0)
		return "Not Available";

	ZEUInt64 EstimatedSeconds = TotalSize - BytesPerSecond;

	if (EstimatedSeconds / 86400 > 0)
		return ZEFormat::Format("{0} Days", EstimatedSeconds / 86400);
	else if (EstimatedSeconds % 86400 > 3600)
		return ZEFormat::Format("{0} Hours", (EstimatedSeconds % 86400) / 3600);
	else if (EstimatedSeconds % 3600 > 60)
		return  ZEFormat::Format("{0} Minutes", (EstimatedSeconds % 3600) / 60);
	else
		return ZEFormat::Format("{0} Seconds", EstimatedSeconds % 60 );
}

ZECRSenderProgress ZECRSender::GetProgress()
{
	ProgressLock.Lock();
	ZECRSenderProgress Output = Progress;
	ProgressLock.Unlock();

	return Output;
}


ZECRSender::ZECRSender()
{	

}

ZECRSender::~ZECRSender()
{

}
