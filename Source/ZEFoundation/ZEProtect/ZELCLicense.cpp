//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELCLicense.cpp
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

#include "ZELCLicense.h"

#ifdef ZE_PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Shlobj.h>
#include <WbemCli.h>
#include "ZEML\ZEMLReader.h"
#include "ZEML\ZEMLWriter.h"
#include "ZEDS\ZEFormat.h"
#include "ZERegEx\ZERegEx.h"
#pragma comment(lib, "wbemuuid.lib")

#include <rsa.h>
#include "ZELCEncryption.h"
#include "ZERandom.h"

static ZEString WMIQuery(wchar_t* Query, wchar_t* Column)
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

static ZEUInt32 GetHarddiskSerialNumber()
{
	return WMIQuery(L"SELECT SerialKey FROM Win32_PhysicalMedia", L"SerialNumber").ToUInt32();
}

static ZEUInt32 GetBiosSerialNumber()
{
	return WMIQuery(L"SELECT SerialNumber FROM Win32_Bios", L"SerialNumber").ToUInt32();
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

struct ZELCActivationData
{
	ZEUInt64 Random0;
	ZEUInt32 SerialKey[5];
	ZEGUID MachineSecurityGUID;
	ZEUInt32 BiosSerialNumber;
	ZEUInt32 HarddiskSerialNumber;
	ZEUInt64 Random1;
};

static ZELCActivationData GenerateActivationData(const ZELCLicense* License)
{
	ZELCActivationData ActivationData;
	memset(&ActivationData, 0, sizeof(ZELCActivationData));

	ZERegEx RegEx("([a-fA-F0-9]{8})-([a-fA-F0-9]{8})-([a-FA-F0-9]{8})-([a-FA-F0-9]{8})-([a-FA-F0-9]{8})");
	ZERegExMatch Match;
	if (!RegEx.Match(License->GetSerialKey(), Match))
		return ActivationData;

	ActivationData.SerialKey[0] = Match.SubMatches[0].String.ToUInt32(16);
	ActivationData.SerialKey[1] = Match.SubMatches[1].String.ToUInt32(16);
	ActivationData.SerialKey[2] = Match.SubMatches[2].String.ToUInt32(16);
	ActivationData.SerialKey[3] = Match.SubMatches[3].String.ToUInt32(16);
	ActivationData.SerialKey[4] = Match.SubMatches[4].String.ToUInt32(16);

	ActivationData.MachineSecurityGUID = GetMachineSecurityGUID();
	ActivationData.HarddiskSerialNumber = GetHarddiskSerialNumber();
	ActivationData.BiosSerialNumber = GetBiosSerialNumber();

	return ActivationData;
}

void ZELCLicense::SetGUID(const ZEGUID& GUID)
{
	this->GUID = GUID;
}

const ZEGUID& ZELCLicense::GetGUID() const
{
	return GUID;
}

void ZELCLicense::SetApplicationName(const ZEString& Name)
{
	ApplicationName = Name;
}

const ZEString& ZELCLicense::GetApplicationName() const
{
	return ApplicationName;
}

void ZELCLicense::SetApplicationVersionMajor(ZEUInt8 Version)
{
	ApplicationVersionMajor = Version;
}

ZEUInt8 ZELCLicense::GetApplicationVersionMajor() const
{
	return ApplicationVersionMajor;
}

void ZELCLicense::SetApplicationVersionMinor(ZEUInt8 Version)
{
	ApplicationVersionMinor = Version;
}

ZEUInt8 ZELCLicense::GetApplicationVersionMinor() const
{
	return ApplicationVersionMinor;
}

void ZELCLicense::SetApplicationEdition(ZEUInt8 Type)
{
	this->ApplicationEdition = Type;
}

ZEUInt8 ZELCLicense::GetApplicationEdition() const
{
	return ApplicationEdition;
}

void ZELCLicense::SetLicenseeName(const ZEString& Name)
{
	LicenseeName = Name;
}

const ZEString& ZELCLicense::GetLicenseeName() const
{
	return LicenseeName;
}

void ZELCLicense::SetSerialKey(const ZEString& Serial)
{
	this->SerialKey = Serial;
}

const ZEString& ZELCLicense::GetSerialKey() const
{
	return SerialKey;
}

void ZELCLicense::SetActivationCode(const ZEString& Code)
{
	ActivationCode = Code;
}

const ZEString& ZELCLicense::GetActivationCode() const
{
	return ActivationCode;
}

void ZELCLicense::SetPriority(ZEInt Priority)
{
	this->Priority = Priority;
}

ZEInt ZELCLicense::GetPriority() const
{
	return Priority;
}

void ZELCLicense::SetLicenseVersion(ZEUInt Version)
{
	LicenseVersion = Version;
}

ZEUInt ZELCLicense::GetLicenseVersion() const
{
	return LicenseVersion;
}

void ZELCLicense::SetSystemWide(bool Enabled)
{
	SystemWide = Enabled;
}

bool ZELCLicense::GetSystemWide() const
{
	return SystemWide;
}

ZEString ZELCLicense::GeneratePreActivationCode() const
{
	ZELCActivationData Data = GenerateActivationData(this);
	Data.Random0 = ZERandom::GetUInt64();
	Data.Random1 = ZERandom::GetUInt64();

	/*ZELCRSAEncription Encription;
	Encription.SetPrivateKey();*/

	ZEArray<ZEBYTE> Buffer;
	//Encription.Encrypt(Buffer, &Data, sizeof(ZELCActivationData));

	ZEString Output;
	for (ZESize I = 0; I < Buffer.GetSize() / 8; I++)
		Output += ZEFormat::Format(I == 0 ? "{0:x}" : "-{0:x}", *(ZEUInt32*)&Buffer[I * 4]);

	return Output;
}

bool ZELCLicense::CheckSerialKeyValid() const
{
	ZERegEx RegEx("ZE-([a-fA-F0-9]{8})-([a-fA-F0-9]{8})-([a-FA-F0-9]{8})-([a-FA-F0-9]{8})-([a-FA-F0-9]{8})");
	ZERegExMatch Match;
	ZEUInt32 SerialKeyEncoded[5];

	SerialKeyEncoded[0] = Match.SubMatches[0].String.ToUInt32(16);
	SerialKeyEncoded[1] = Match.SubMatches[1].String.ToUInt32(16);
	SerialKeyEncoded[2] = Match.SubMatches[2].String.ToUInt32(16);
	SerialKeyEncoded[3] = Match.SubMatches[3].String.ToUInt32(16);
	SerialKeyEncoded[4] = Match.SubMatches[4].String.ToUInt32(16);


	ZEUInt32 Check;
	Check  = SerialKeyEncoded[0] + 0x46AD788E;
	Check ^= SerialKeyEncoded[1] - 0x6B71E511;
	Check ^= SerialKeyEncoded[2] - 0x915067FA;
	Check ^= SerialKeyEncoded[3] + 0xEE7C85CA;

	if (SerialKeyEncoded[4] != Check)
		return false;

	ZEUInt32 SerialKeyDecoded[5];
	ZEUInt8* VersionEditionFlags = (ZEUInt8*)&SerialKeyDecoded[3];

	if (SerialKeyDecoded[1] != GetApplicationName().Hash() ||
		SerialKeyDecoded[2] != GetLicenseeName().Hash() ||
		VersionEditionFlags[0] != GetApplicationVersionMajor()  ||
		VersionEditionFlags[1] != GetApplicationVersionMinor() ||
		VersionEditionFlags[2] != GetApplicationEdition())
	{
		return false;
	}

	return true;
}

bool ZELCLicense::CheckActivationCodeValid() const
{
	ZEArray<ZEBYTE> Buffer;
	ZELCRSAEncription Encryption;
	/*Encryption.SetPublicKey();
	Encryption.Decrypt(Buffer, )*/
		
	if (Buffer.GetCount() != sizeof(ZELCActivationData))
		return false;

	ZELCActivationData Data = GenerateActivationData(this);
	ZELCActivationData* DecryptedData = (ZELCActivationData*)Buffer.GetCArray();
	if (DecryptedData->MachineSecurityGUID != Data.MachineSecurityGUID ||
		DecryptedData->BiosSerialNumber != Data.BiosSerialNumber ||
		DecryptedData->HarddiskSerialNumber != Data.HarddiskSerialNumber ||
		DecryptedData->SerialKey[0] != Data.SerialKey[0] ||
		DecryptedData->SerialKey[1] != Data.SerialKey[1] ||
		DecryptedData->SerialKey[2] != Data.SerialKey[2] ||
		DecryptedData->SerialKey[3] != Data.SerialKey[3] ||
		DecryptedData->SerialKey[4] != Data.SerialKey[4])
	{
		return false;
	}

	return true;
}

bool ZELCLicense::CheckValid() const
{
	return CheckSerialKeyValid() && CheckActivationCodeValid();
}

void ZELCLicense::Load(ZEMLReaderNode* Reader)
{
	ZEString GUIDString;
	Reader->ReadString("GUID", GUIDString);
	Reader->ReadString("ApplicationName", ApplicationName);
	Reader->ReadUInt8("ApplicationVersionMajor", ApplicationVersionMajor);
	Reader->ReadUInt8("ApplicationVersionMinor", ApplicationVersionMinor);
	Reader->ReadUInt8("ApplicationEdition", ApplicationEdition);
	Reader->ReadString("LicenseeName", LicenseeName);
	Reader->ReadString("SerialKey", SerialKey);
	Reader->ReadString("ActivationCode", ActivationCode);
	Reader->ReadInt32("Priority", Priority);
	Reader->ReadUInt8("LicenseVersion", LicenseVersion);
}

void ZELCLicense::Save(ZEMLWriterNode* Writer) const
{
	Writer->WriteString("GUID", GUID.ToString());
	Writer->WriteString("ApplicationName", ApplicationName);
	Writer->WriteUInt8("ApplicationVersionMajor", ApplicationVersionMajor);
	Writer->WriteUInt8("ApplicationVersionMinor", ApplicationVersionMinor);
	Writer->WriteUInt8("ApplicationEdition", ApplicationEdition);
	Writer->WriteString("LicenseeName", LicenseeName);
	Writer->WriteString("SerialKey", SerialKey);
	Writer->WriteString("ActivationCode", ActivationCode);
	Writer->WriteUInt32("Priority", Priority);
	Writer->WriteUInt8("LicenseVersion", LicenseVersion);
}

ZELCLicense::ZELCLicense()
{
	GUID = ZEGUID::Generate();
	ApplicationVersionMajor = 0;
	ApplicationVersionMinor = 0;
	ApplicationEdition = 0;
	Priority = 0;
	LicenseVersion = 1;
	SystemWide = false;
}
