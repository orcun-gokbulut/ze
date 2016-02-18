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

#include "ZELCUtils.h"
#include "ZELCLicense.h"
#include "ZERandom.h"
#include "ZEError.h"
#include "ZEPlatform.h"

#ifdef ZE_PLATFORM_WINDOWS

	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <Shlobj.h>
	#include <WbemCli.h>
	#include <comutil.h>

	#pragma comment(lib, "wbemuuid.lib")
	#pragma comment(lib, "comsuppw.lib")

	static bool WMIQuery(ZEString& Output, wchar_t* Query, wchar_t* Column)
	{
		// WARNING !!! WARNING !!! WARNING !!! WARNING !!!
		// DO NOT CHANGE SINGLE LINE OF THIS CODE !!!
		// HEAVY PROBLEMS DUE COM INITIALIZATION AND QT

		HRESULT hres;
		hres =  CoInitializeEx(0, COINIT_MULTITHREADED); 
		hres =  CoInitializeSecurity(
			NULL, 
			-1,                          // COM authentication
			NULL,                        // Authentication services
			NULL,                        // Reserved
			RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
			RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
			NULL,                        // Authentication info
			EOAC_NONE,                   // Additional capabilities 
			NULL                         // Reserved
			);

		// Step 3: ---------------------------------------------------
		// Obtain the initial locator to WMI -------------------------
		IWbemLocator *pLoc = NULL;
		hres = CoCreateInstance(
			CLSID_WbemLocator,             
			0, 
			CLSCTX_INPROC_SERVER, 
			IID_IWbemLocator, (LPVOID *) &pLoc);
		if (FAILED(hres))
		{
			//CoUninitialize();
			return false;
		}

		// Step 4: -----------------------------------------------------
		// Connect to WMI through the IWbemLocator::ConnectServer method
		IWbemServices *pSvc = NULL;
		hres = pLoc->ConnectServer(
			_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
			NULL,                    // User name. NULL = current user
			NULL,                    // User password. NULL = current
			0,                       // Locale. NULL indicates current
			NULL,                    // Security flags.
			0,                       // Authority (for example, Kerberos)
			0,                       // Context object 
			&pSvc                    // pointer to IWbemServices proxy
			);
		if (FAILED(hres))
		{
			pLoc->Release();     
			//CoUninitialize();
			return false;                // Program has failed.
		}

		// Step 5: --------------------------------------------------
		// Set security levels on the proxy -------------------------
		hres = CoSetProxyBlanket(
			pSvc,                        // Indicates the proxy to set
			RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
			RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
			NULL,                        // Server principal name 
			RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
			RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
			NULL,                        // client identity
			EOAC_NONE                    // proxy capabilities 
			);

		if (FAILED(hres))
		{
			pSvc->Release();
			pLoc->Release();     
			//CoUninitialize();
			return false;               // Program has failed.
		}


		// Step 6: --------------------------------------------------
		// Use the IWbemServices pointer to make requests of WMI ----

		// For example, get the name of the operating system
		IEnumWbemClassObject* pEnumerator = NULL;
		hres = pSvc->ExecQuery(
			bstr_t("WQL"), 
			bstr_t(Query),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
			NULL,
			&pEnumerator);

		if (FAILED(hres))
		{
			pSvc->Release();
			pLoc->Release();
			//CoUninitialize();
			return false;               // Program has failed.
		}


		// Step 7: -------------------------------------------------
		// Get the data from the query in step 6 -------------------

		IWbemClassObject *pclsObj = NULL;
		ULONG uReturn = 0;
		while (pEnumerator)
		{
			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
			if(0 == uReturn)
			{
				return false;
			}

			VARIANT vtProp;
			hr = pclsObj->Get(Column, 0, &vtProp, 0, 0);
			Output = vtProp.bstrVal;
			VariantClear(&vtProp);
			pclsObj->Release();
			break;
		}

		pSvc->Release();
		pLoc->Release();
		pEnumerator->Release();
		//CoUninitialize();
		return true;
	}

	static bool GetHardDiskSerialNumber(ZEUInt32& Output)
	{
		ZEString OutputText;
		if (!WMIQuery(OutputText, L"SELECT SerialNumber FROM Win32_DiskDrive WHERE InterfaceType != \"USB\"", L"SerialNumber"))
			return false;

		Output = OutputText.Hash();
		return true;
	}

	static bool GetBiosSerialNumber(ZEUInt32& Output)
	{
		ZEString OutputText;
		if (!WMIQuery(OutputText, L"SELECT SerialNumber FROM Win32_Bios", L"SerialNumber"))
			return false;

		Output = OutputText.Hash();
		return true;
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

void GenerateHardwareIds(ZELCActivationData& ActivationData)
{
	ActivationData.MachineSecurityGUID = GetMachineSecurityGUID();
	if (!GetHardDiskSerialNumber(ActivationData.HardDiskSerialNumber))
		zeError("Cannot generate activation data.");

	if (!GetBiosSerialNumber(ActivationData.BiosSerialNumber))
		zeError("Cannot generate activation data.");
}

bool ZELCActivationData::Generate(ZELCActivationData& ActivationData, const ZELCLicense& License)
{
	memset(&ActivationData, 0, sizeof(ZELCActivationData));

	ActivationData.Random = 0; //ZERandom::GetUInt64();
	ZELCUtils::ConvertSerialKey(ActivationData.SerialKey, License.GetSerialKey());
	GenerateHardwareIds(ActivationData);

	return true;
}
