//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEProtect.cpp
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

#include "ZEProtect.h"
#include "md5.h"

#include <stdio.h>
#include <string.h>
#include "ZEDS\ZEFormat.h"

#ifdef ZE_PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Shlobj.h>
#include <WbemCli.h>
#pragma comment(lib, "wbemuuid.lib")

ZEString WMIQuery(wchar_t* Query, wchar_t* Column)
{
	ZEString Result;

	HRESULT hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(FAILED(hRes))
		return "";

	if((FAILED(hRes = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_CONNECT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, 0))))
		return "";

	IWbemLocator* pLocator = NULL;
	if(FAILED(hRes = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pLocator))))
		return "";

	IWbemServices* pService = NULL;
	if(FAILED(hRes = pLocator->ConnectServer(L"root\\CIMV2", NULL, NULL, NULL, WBEM_FLAG_CONNECT_USE_MAX_WAIT, NULL, NULL, &pService)))
		return "";

	IEnumWbemClassObject* pEnumerator = NULL;
	if(FAILED(hRes = pService->ExecQuery(L"WQL", Query, WBEM_FLAG_FORWARD_ONLY, NULL, &pEnumerator)))
	{
		pLocator->Release();
		pService->Release();
		return "";
	}

	IWbemClassObject* clsObj = NULL;
	int numElems;
	while((hRes = pEnumerator->Next(WBEM_INFINITE, 1, &clsObj, (ULONG*)&numElems)) != WBEM_S_FALSE)
	{
		if(FAILED(hRes))
			break;

		VARIANT vRet;
		VariantInit(&vRet);
		if(SUCCEEDED(clsObj->Get(L"Description", 0, &vRet, NULL, NULL)) && vRet.vt == VT_BSTR)
		{
			Result = vRet.bstrVal;
			VariantClear(&vRet);
			break;
		}

		clsObj->Release();
	}

	pEnumerator->Release();
	pService->Release();
	pLocator->Release();

	return Result;
}

static ZEString GetHDDSerial()
{
	DWORD SerialNumber;
	GetVolumeInformation("c:\\", NULL, 0, &SerialNumber, 0, 0, NULL, 0);
	return ZEFormat::Format("{0}", (ZEUInt)SerialNumber);
}

static ZEString GetMACAddress()
{
	return "";
}

static ZEString GetMotherboardSerial()
{
	return WMIQuery(L"SELECT SerialNumber FROM Win32_Bios", L"SerialNumber");
}

static ZEString GetSystemWidePath()
{
	char Buffer[300];
	SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, Buffer);
	return Buffer;
}

#else

static ZEString GetHDDSerial()
{
	return "";
}

static ZEString GetMACAddress()
{
	return "";
}

static ZEString GetMotherboardSerial()
{
	return "";
}

static ZEString GetSystemWidePath()
{
	return "~";
}

#endif

void ZEProtect::SetApplicationName(const char* Name)
{
	ApplicationName = Name;
}

const ZEString& ZEProtect::GetApplicationName()
{
	return ApplicationName;
}

void ZEProtect::SetKey(const char* Key)
{
	this->Key = Key;
}

const ZEString& ZEProtect::GetKey()
{
	return Key;
}

void ZEProtect::SetActivationFileName(const char* FileName)
{
	ActivationFileName = FileName;
}

const ZEString& ZEProtect::GetActivationFileName()
{
	return ActivationFileName;
}

void ZEProtect::SetSystemWide(bool SystemWide)
{
	this->SystemWide = SystemWide;
}

bool ZEProtect::GetSystemWide()
{
	return SystemWide;
}

ZEString ZEProtect::GenerateActivationCode()
{
	return md5(ZEFormat::Format("{0}:MJaHmS6V1A:Zinek:{1}:{2}:{3}:{4}:EDIEDxUjSB", GetKey(), GetApplicationName(), GetHDDSerial(), GetMACAddress(), GetMotherboardSerial()).ToCString());
}

bool ZEProtect::Activate()
{
	if (Verify())
		return true;

	char Buffer[256];
	ZEString ActivationCode = GenerateActivationCode();
	ZESize ActivationCodeSize = ActivationCode.GetSize() < 256 ? ActivationCode.GetSize() : 256;

	ZEString FileName;
	if (SystemWide)
		FileName = GetSystemWidePath() + "/" + ActivationFileName;
	else
		FileName = ActivationFileName;

	FILE* File = fopen(FileName, "a");
	if (File == NULL)
		File = fopen(FileName, "w");
	if (File == NULL)
		return false;

	fprintf(File, "%s\n", ActivationCode.ToCString());
	fclose(File);

	return true;
}

bool ZEProtect::Verify()
{
	char Buffer[256];
	ZEString ActivationCode = GenerateActivationCode();
	ZESize ActivationCodeSize = ActivationCode.GetSize() < 256 ? ActivationCode.GetSize() : 256;
	
	ZEString FileName;
	if (SystemWide)
		FileName = GetSystemWidePath() + "/" + ActivationFileName;
	else
		FileName = ActivationFileName;

	FILE* File = fopen(FileName, "r");
	if (File != NULL)
	{
		while(!feof(File))
		{
			fgets(Buffer, ActivationCodeSize, File);
			if (strncmp(Buffer, ActivationCode, ActivationCodeSize) == 0)
			{
				fclose(File);
				return true;
			}
		}
		fclose(File);
	}
	
	return false;
}

ZEProtect::ZEProtect()
{
	SystemWide = true;
}
