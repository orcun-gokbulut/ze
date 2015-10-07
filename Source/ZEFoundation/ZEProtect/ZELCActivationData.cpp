//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELCActivationData.cpp
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

#include "ZELCActivationData.h"

#include "ZERandom.h"
#include "ZELCUtils.h"
#include "ZELCLicense.h"

#ifdef ZE_PLATFORM_WINDOWS

	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <Shlobj.h>
	#include <WbemCli.h>
	#include "ZERandom.h"

	#pragma comment(lib, "wbemuuid.lib")

	static ZEString WMIQuery(wchar_t* Query, wchar_t* Column)
	{
		ZEString Result;

		HRESULT hRes = 0;
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
			if(SUCCEEDED(clsObj->Get(Column, 0, &vRet, NULL, NULL)) && vRet.vt == VT_BSTR)
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

	static ZEUInt32 GetHardDiskSerialNumber()
	{
		return WMIQuery(L"SELECT SerialNumber FROM Win32_PhysicalMedia", L"SerialNumber").Hash();
	}

	static ZEUInt32 GetBiosSerialNumber()
	{
		return WMIQuery(L"SELECT SerialNumber FROM Win32_Bios", L"SerialNumber").Hash();
	}

	static ZEGUID GetMachineSecurityGUID()
	{
		ZEGUID Output;

		HKEY Key;
		if (RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Cryptography\\"), &Key) != ERROR_SUCCESS)
			return Output;

		char Value[1024];
		DWORD ValueSize = 1024;
		DWORD Type = REG_SZ;
		RegQueryValueEx(Key, "MachineGUID", NULL, &Type, (LPBYTE)&Value, &ValueSize);

		Output.FromString(Value);

		return Output;
	}

#else

	static ZEUInt32 GetHardDiskSerialNumber()
	{
		return "";
	}

	static ZEUInt32 GetBiosSerialNumber()
	{
		return "";
	}

	static ZEGUID GetMachineSecurityGUID()
	{
		return ""
	}

#endif

bool ZELCActivationData::Generate(ZELCActivationData& ActivationData, const ZELCLicense& License)
{
	memset(&ActivationData, 0, sizeof(ZELCActivationData));

	ActivationData.Random = 0; //ZERandom::GetUInt64();
	ZELCUtils::ConvertSerialKey(ActivationData.SerialKey, License.GetSerialKey());
	ActivationData.MachineSecurityGUID = GetMachineSecurityGUID();
	ActivationData.HardDiskSerialNumber = GetHardDiskSerialNumber();
	ActivationData.BiosSerialNumber = GetBiosSerialNumber();

	return true;
}
