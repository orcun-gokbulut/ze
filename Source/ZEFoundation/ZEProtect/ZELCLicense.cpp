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

#include "ZELCEncryption.h"
#include "ZELCKeys.h"
#include "ZELCUtils.h"
#include "ZELCActivationData.h"
#include "ZEML\ZEMLReader.h"
#include "ZEML\ZEMLWriter.h"

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
	ZELCActivationData ActivationData;
	if (!ZELCActivationData::Generate(ActivationData, *this))
		return false;

	ZEArray<ZEBYTE> Buffer;
	ZELCEncryption::RSAEncrypt(Buffer, &ActivationData, sizeof(ZELCActivationData), ZELCKeys::GetPreActivationPublicKey(), ZELCKeys::GetPreActivationPublicKeySize());

	return ZELCUtils::ConvertActivationCode((ZEUInt32*)Buffer.GetConstCArray());
}

bool ZELCLicense::CheckSerialKeyValid() const
{
	ZEUInt32 SerialKeyEncoded[ZELC_SERIAL_KEY_SIZE / sizeof(ZEUInt32)];
	if (!ZELCUtils::ConvertSerialKey(SerialKeyEncoded, SerialKey))
		return false;

	if (!ZELCUtils::CheckSerialCode(SerialKeyEncoded))
		return false;

	ZEUInt32 SerialKeyDecoded[4];
	SerialKeyDecoded[0] = SerialKeyEncoded[0] ^ 0x00C6487A;
	SerialKeyDecoded[1] = SerialKeyEncoded[0] ^ 0x5B52C4E9 ^ SerialKeyEncoded[1];			
	SerialKeyDecoded[2] = SerialKeyEncoded[0] ^ 0x07EA99BA ^ SerialKeyEncoded[2];
	SerialKeyDecoded[3] = SerialKeyEncoded[0] ^ 0xB5EFD017 ^ SerialKeyEncoded[3];

	ZEUInt8* VersionEditionFlags = (ZEUInt8*)&SerialKeyDecoded[3];

	if (SerialKeyDecoded[1] != GetApplicationName().Hash() ||
		SerialKeyDecoded[2] != GetLicenseeName().Hash() ||
		VersionEditionFlags[0] != GetApplicationVersionMajor() /* ||
		VersionEditionFlags[1] != GetApplicationVersionMinor() ||
		VersionEditionFlags[2] != GetApplicationEdition()*/)
	{
		return false;
	}

	return true;
}

bool ZELCLicense::CheckActivationCodeValid() const
{
	ZEBYTE ActivationCodeBinary[ZELC_ACTIVATION_CODE_SIZE];
	if (!ZELCUtils::ConvertActivationCode(ActivationCodeBinary, GetActivationCode()))
		return false;

	ZELCActivationData ActivationData;
	if (!ZELCActivationData::Generate(ActivationData, *this))
		return false;

	return ZELCEncryption::RSAVerify(&ActivationData, sizeof(ZELCActivationData), ActivationCodeBinary, sizeof(ActivationCodeBinary), ZELCKeys::GetActivationPublicKey(), ZELCKeys::GetPreActivationPublicKeySize());
}

bool ZELCLicense::CheckValid() const
{
	return CheckSerialKeyValid() && CheckActivationCodeValid();
}

void ZELCLicense::Load(ZEMLReaderNode* Reader)
{
	ZEString GUIDString;
	GUIDString = Reader->ReadString("GUID");
	ApplicationName = Reader->ReadString("ApplicationName");
	ApplicationVersionMajor = Reader->ReadUInt8("ApplicationVersionMajor");
	ApplicationVersionMinor = Reader->ReadUInt8("ApplicationVersionMinor");
	ApplicationEdition = Reader->ReadUInt8("ApplicationEdition");
	LicenseeName = Reader->ReadString("LicenseeName");
	SerialKey = Reader->ReadString("SerialKey");
	ActivationCode = Reader->ReadString("ActivationCode");
	Priority = Reader->ReadInt32("Priority");
	LicenseVersion = Reader->ReadUInt8("LicenseVersion");
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
	SystemWide = true;
}
