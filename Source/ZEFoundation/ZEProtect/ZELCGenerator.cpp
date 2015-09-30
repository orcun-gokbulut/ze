//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELCGenerator.cpp
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
#include "ZELCGenerator.h"

#include "ZEDS\ZEFormat.h"
#include "ZEGUID.h"

void ZELCSerialKeyGenerator::SetApplicationName(const ZEString& Name)
{
	ApplicationName = Name;
}

const ZEString& ZELCSerialKeyGenerator::GetApplicationName() const
{
	return ApplicationName;
}

void ZELCSerialKeyGenerator::SetApplicationVersionMajor(ZEUInt8 Version)
{
	ApplicationVersionMajor = Version;
}

ZEUInt8 ZELCSerialKeyGenerator::GetApplicationVersionMajor() const
{
	return ApplicationVersionMajor;
}

void ZELCSerialKeyGenerator::SetApplicationVersionMinor(ZEUInt8 Version)
{
	ApplicationVersionMinor = Version;
}

ZEUInt8 ZELCSerialKeyGenerator::GetApplicationVersionMinor() const
{
	return ApplicationVersionMinor;
}

void ZELCSerialKeyGenerator::SetApplicationEdition(ZEUInt8 Type)
{
	this->ApplicationEdition = Type;
}

ZEUInt8 ZELCSerialKeyGenerator::GetApplicationEdition() const
{
	return ApplicationEdition;
}

void ZELCSerialKeyGenerator::SetLicenseeName(const ZEString& Name)
{
	LicenseeName = Name;
}

const ZEString& ZELCSerialKeyGenerator::GetLicenseeName() const
{
	return LicenseeName;
}

void ZELCSerialKeyGenerator::SetSerialNumber(ZEUInt32 Number)
{
	SerialNumber = Number;
}

ZEUInt32 ZELCSerialKeyGenerator::GetSerialNumber() const
{
	return SerialNumber;
}

ZEString ZELCSerialKeyGenerator::GenerateSerialKey() const
{
	ZEUInt32 SerialKey[5];
	SerialKey[0] = GetSerialNumber();
	SerialKey[1] = GetApplicationName().Hash();
	SerialKey[2] = GetLicenseeName().Hash();
	
	ZEUInt8* VersionEditionFlags = (ZEUInt8*)&SerialKey[3];
	VersionEditionFlags[0] = GetApplicationVersionMajor();
	VersionEditionFlags[1] = GetApplicationVersionMinor();
	VersionEditionFlags[2] = GetApplicationEdition();
	VersionEditionFlags[3] = 0;

	ZEUInt32 Check;
	Check  = SerialKey[0] + 0x46AD788E;
	Check ^= SerialKey[1] - 0x6B71E511;
	Check ^= SerialKey[2] - 0x915067FA;
	Check ^= SerialKey[3] + 0xEE7C85CA;

	SerialKey[4] = Check;

	return ZEFormat::Format("ZE-([a-fA-F0-9]{8})-([a-fA-F0-9]{8})-([a-FA-F0-9]{8})-([a-FA-F0-9]{8})-([a-FA-F0-9]{8})",
		SerialKey[0], SerialKey[1], SerialKey[2], SerialKey[3], SerialKey[4]);
}

ZELCSerialKeyGenerator::ZELCSerialKeyGenerator()
{
	ApplicationVersionMajor = 1;
	ApplicationVersionMinor = 0;
	ApplicationEdition = 0;
	SerialNumber = 0;
}

void ZELCActivationCodeGenerator::SetPreActivationPrivateKey(const ZEArray<ZEBYTE>& KeyData)
{
	PreActivationPrivateKey = KeyData;
}

const ZEArray<ZEBYTE>& ZELCActivationCodeGenerator::GetPreActivationPrivateKey() const
{
	return PreActivationPrivateKey;
}

void ZELCActivationCodeGenerator::SetActivationPublicKey(const ZEArray<ZEBYTE>& KeyData)
{
	ActivationPublicKey = KeyData;
}

const ZEArray<ZEBYTE>& ZELCActivationCodeGenerator::GetActivationPublicKey() const
{
	return ActivationPublicKey;
}

void ZELCActivationCodeGenerator::SetSerialKey(const ZEString& SerialKey)
{
	this->SerialKey = SerialKey;
}

const ZEString& ZELCActivationCodeGenerator::GetSerialKey() const
{
	return SerialKey;
}

void ZELCActivationCodeGenerator::SetPreActivationCode(const ZEString& PreActivationCode)
{
	this->PreActivationCode = PreActivationCode;
}

const ZEString& ZELCActivationCodeGenerator::GetPreActivationCode() const
{
	return PreActivationCode;
}

bool ZELCActivationCodeGenerator::GenerateActivationCode(ZEString& Output) const
{
	return false;
}
