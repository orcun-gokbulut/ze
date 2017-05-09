//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRCIM.cpp
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

#include "ZECRCIM.h"

#include "ZEDS/ZEFormat.h"

#define WIN32_LEAN_AND_MEAN
#define _WIN32_DCOM
#include <comdef.h>
#include <Windows.h>
#include <WbemIdl.h>
#pragma comment(lib, "wbemuuid.lib")

bool ZECRCIM::Initialize()
{
	DeInitialize();

	HRESULT hResult = CoInitializeEx(0, COINIT_MULTITHREADED);
	
	if (FAILED(hResult))
		return false;

	hResult = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, 
		RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

	if (hResult != S_OK && hResult != RPC_E_TOO_LATE)
		return false;
	
	return true;
}

void ZECRCIM::DeInitialize()
{
	CoUninitialize();
}

bool ZECRCIM::ExecuteQuery(ZEString& Output, const ZEString& NameSpace, const ZEString& Language, const ZEString& Query)
{
	HRESULT			hResult;
	IWbemLocator*	pLocator = 0;

	hResult = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*) &pLocator);

	if (FAILED(hResult))
		return false;

	IWbemServices*	pServices = 0;

	hResult = pLocator->ConnectServer(_bstr_t(NameSpace.ToCString()), NULL, NULL, 
		0, NULL, 0, 0, &pServices);

	if (FAILED(hResult))
	{
		pLocator->Release();		
		return false;
	}

	IEnumWbemClassObject* pEnumerator = NULL;

	hResult = pServices->ExecQuery(bstr_t(Language.ToCString()), bstr_t(Query.ToCString()), 
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

	if (FAILED(hResult))
	{
		pServices->Release();
		pLocator->Release();		
		return false;
	}

	IWbemClassObject* pClassObject;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		hResult = pEnumerator->Next(WBEM_INFINITE, 1, &pClassObject, &uReturn);
		if (0 == uReturn)
			break;

		if (pClassObject->BeginEnumeration(WBEM_FLAG_NONSYSTEM_ONLY) == WBEM_S_NO_ERROR)
		{
			BSTR bstrName;
			VARIANT vtProp;		

			while (pClassObject->Next(0, &bstrName, &vtProp, 0, 0) == WBEM_S_NO_ERROR)
			{
				VariantChangeType(&vtProp, &vtProp, 0, VT_BSTR);
				if (vtProp.vt == VT_BSTR && !ZEString(vtProp.bstrVal).IsEmpty() && ZEString(bstrName) != "SystemName" && ZEString(bstrName) != "CSName")
				{						
					Output += ZEFormat::Format("<{0}>{1}</{0}>\n", ZEString(bstrName), ZEString(vtProp.bstrVal));
				}
			}
		}
	}
	pServices->Release();
	pLocator->Release();
	return true;
}
