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

#include "ZEEncryption.h"
#include "ZEML\ZEMLReader.h"
#include "ZEML\ZEMLWriter.h"

#include "ZELCKeys.h"
#include "ZELCUtils.h"
#include "ZELCActivationData.h"

static __forceinline bool ConverSerial(ZEUInt32* SerialKeyBinary, const ZEString& SerialKey)
{
	ZEUInt32 SerialKeyEncoded[ZELC_SERIAL_KEY_SIZE / sizeof(ZEUInt32)];
	if (!ZELCUtils::ConvertSerialKey(SerialKeyEncoded, SerialKey))
		return false;

	if (!ZELCUtils::CheckSerialCode(SerialKeyEncoded))
		return false;

	SerialKeyBinary[0] = SerialKeyEncoded[0] ^ 0x00C6487A;
	SerialKeyBinary[1] = SerialKeyEncoded[0] ^ 0x5B52C4E9 ^ SerialKeyEncoded[1];			
	SerialKeyBinary[2] = SerialKeyEncoded[0] ^ 0x07EA99BA ^ SerialKeyEncoded[2];
	SerialKeyBinary[3] = SerialKeyEncoded[0] ^ 0xB5EFD017 ^ SerialKeyEncoded[3];

	return true;
}

void ZELCLicense::SetGUID(const ZEGUID& GUID)
{
	this->GUID = GUID;
}

const ZEGUID& ZELCLicense::GetGUID() const
{
	return GUID;
}

void ZELCLicense::SetProductName(const ZEString& Name)
{
	ProductName = Name;
}

const ZEString& ZELCLicense::GetProductName() const
{
	return ProductName;
}

ZEUInt8 ZELCLicense::GetProductVersionMajor() const
{
	ZEUInt32 SerialKeyBinary[4];
	if (!ConverSerial(SerialKeyBinary, GetSerialKey()))
		return 0;

	ZEUInt8* VersionEditionFlags = (ZEUInt8*)&SerialKeyBinary[3];
	return VersionEditionFlags[0];
}

ZEUInt8 ZELCLicense::GetProductVersionMinor() const
{
	ZEUInt32 SerialKeyBinary[4];
	if (!ConverSerial(SerialKeyBinary, GetSerialKey()))
		return 0;

	ZEUInt8* VersionEditionFlags = (ZEUInt8*)&SerialKeyBinary[3];
	return VersionEditionFlags[1];
}

ZEUInt8 ZELCLicense::GetProductEdition() const
{
	ZEUInt32 SerialKeyBinary[4];
	if (!ConverSerial(SerialKeyBinary, GetSerialKey()))
		return 0;

	ZEUInt8* VersionEditionFlags = (ZEUInt8*)&SerialKeyBinary[3];
	return VersionEditionFlags[2];
}

ZEUInt8 ZELCLicense::GetFlags() const
{
	ZEUInt32 SerialKeyBinary[4];
	if (!ConverSerial(SerialKeyBinary, GetSerialKey()))
		return 0;

	ZEUInt8* VersionEditionFlags = (ZEUInt8*)&SerialKeyBinary[3];
	return VersionEditionFlags[3];
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

void ZELCLicense::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZELCLicense::GetEnabled() const
{
	return Enabled;
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
		return ZEString::Empty;

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

	if (SerialKeyDecoded[1] != GetProductName().Hash() ||
		SerialKeyDecoded[2] != GetLicenseeName().Hash())
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
	return GetEnabled() && CheckSerialKeyValid() && CheckActivationCodeValid();
}

void ZELCLicense::Load(ZEMLReaderNode* Reader)
{
	GUID = ZEGUID::FromString(Reader->ReadString("GUID"));
	ProductName = Reader->ReadString("ProductName", Reader->ReadString("ApplicationName"));
	LicenseeName = Reader->ReadString("LicenseeName");
	SerialKey = Reader->ReadString("SerialKey");
	ActivationCode = Reader->ReadString("ActivationCode");
	Priority = Reader->ReadInt32("Priority");
	LicenseVersion = Reader->ReadUInt8("LicenseVersion");
	Enabled = Reader->ReadBoolean("Enabled", true);
}

void ZELCLicense::Save(ZEMLWriterNode* Writer) const
{
	Writer->WriteString("GUID", GUID.ToString());
	Writer->WriteString("ProductName", ProductName);
	Writer->WriteString("LicenseeName", LicenseeName);
	Writer->WriteString("SerialKey", SerialKey);
	Writer->WriteString("ActivationCode", ActivationCode);
	Writer->WriteUInt32("Priority", Priority);
	Writer->WriteBool("Enabled", Enabled);
	Writer->WriteUInt8("LicenseVersion", LicenseVersion);
}

ZELCLicense::ZELCLicense()
{
	GUID = ZEGUID::Generate();
	Priority = 0;
	LicenseVersion = 1;
	SystemWide = true;
	Enabled = true;
}
