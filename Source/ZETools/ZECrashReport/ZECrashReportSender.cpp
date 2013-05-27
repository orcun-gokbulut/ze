//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECrashReportSender.cpp
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

#define CURL_STATICLIB
#include "curl/curl.h"
#include "ZECrashReportSender.h"

static size_t ReadFunction(void* Ouput, size_t Size, size_t Count, void* File)
{
	return fread(Ouput, Size, Count, (FILE*)File);
}

void ZECrashReportSender::SetFileName(const char* FileName)
{
	this->FileName = FileName;
}

const char* ZECrashReportSender::GetFileName()
{
	return FileName;
}

void ZECrashReportSender::SetUploadURL(const char* URL)
{
	this->UploadURL = URL;
}

const char* ZECrashReportSender::GetUploadURL()
{
	return UploadURL;
}

ZESize ZECrashReportSender::GetFileSize()
{
	return FileSize;
}

ZESize ZECrashReportSender::GetTransferedDataSize()
{
	return TransferedDataSize;
}

bool ZECrashReportSender::OpenConnection()
{
	File = fopen(FileName, "rb");
	if (File == NULL)
		return false;

	fseek((FILE*)File, 0, SEEK_END);
	FileSize = ftell((FILE*)File);
	fseek((FILE*)File, 0, SEEK_SET);

	curl_global_init(CURL_GLOBAL_ALL);

	Curl = curl_easy_init();
	if(!Curl)
		return false;

	curl_easy_setopt((CURL*)Curl, CURLOPT_UPLOAD, 1L);
	curl_easy_setopt((CURL*)Curl, CURLOPT_URL, UploadURL.ToCString());
	curl_easy_setopt((CURL*)Curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)FileSize);
	//curl_easy_setopt((CURL*)Curl, CURLOPT_CONNECT_ONLY, 1L);
	curl_easy_setopt((CURL*)Curl, CURLOPT_READFUNCTION, ReadFunction);
	curl_easy_setopt((CURL*)Curl, CURLOPT_READDATA, File);

	CURLcode Result = curl_easy_perform(Curl);
	if(Result != CURLE_OK)
		return false;

	TransferedDataSize = 0;
	
	return true;
}

bool ZECrashReportSender::TransferChunk()
{
	return false;

	ZEUInt8 Buffer[8192];
	ZESize BlockSize = FileSize - TransferedDataSize > 8192 ? 8129 : FileSize - TransferedDataSize;

	if (BlockSize == 0)
		return false;

	if (fread(Buffer, BlockSize, 1, (FILE*)File) != 1)
		return false;

	size_t BytesTransferred = 0;
	CURLcode Result = curl_easy_send((CURL*)Curl, Buffer, BlockSize, &BytesTransferred);
	if(Result != CURLE_OK)
		return false;
		
	TransferedDataSize += BlockSize;
	return true;

}

void ZECrashReportSender::CloseConnection()
{
	curl_easy_cleanup((CURL*)Curl);
	curl_global_cleanup();
}
