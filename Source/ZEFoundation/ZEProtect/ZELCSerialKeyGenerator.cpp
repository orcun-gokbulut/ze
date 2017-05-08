//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELCSerialKeyGenerator.cpp
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

#include "ZELCSerialKeyGenerator.h"

#include "ZELCUtils.h"
#include "ZEDS\ZEFormat.h"

void ZELCSerialKeyGenerator::SetProductName(const ZEString& Name)
{
	ProductName = Name;
}

const ZEString& ZELCSerialKeyGenerator::GetProductName() const
{
	return ProductName;
}

void ZELCSerialKeyGenerator::SetProductVersionMajor(ZEUInt8 Version)
{
	ProductVersionMajor = Version;
}

ZEUInt8 ZELCSerialKeyGenerator::GetProductVersionMajor() const
{
	return ProductVersionMajor;
}

void ZELCSerialKeyGenerator::SetProductVersionMinor(ZEUInt8 Version)
{
	ProductVersionMinor = Version;
}

ZEUInt8 ZELCSerialKeyGenerator::GetProductVersionMinor() const
{
	return ProductVersionMinor;
}

void ZELCSerialKeyGenerator::SetProductEdition(ZEUInt8 Type)
{
	this->ProductEdition = Type;
}

ZEUInt8 ZELCSerialKeyGenerator::GetProductEdition() const
{
	return ProductEdition;
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
	ZEUInt32 Values[4];
	Values[0] = GetSerialNumber();
	Values[1] = GetProductName().Hash();
	Values[2] = GetLicenseeName().Hash();
	
	ZEUInt8* VersionEditionFlags = (ZEUInt8*)&Values[3];
	VersionEditionFlags[0] = GetProductVersionMajor();
	VersionEditionFlags[1] = GetProductVersionMinor();
	VersionEditionFlags[2] = GetProductEdition();
	VersionEditionFlags[3] = 0;

	ZEUInt32 SerialKey[ZELC_SERIAL_KEY_SIZE / sizeof(ZEUInt32)];
	SerialKey[0] = Values[0] ^ 0x00C6487A;
	SerialKey[1] = Values[1] ^ 0x5B52C4E9 ^ SerialKey[0]; 
	SerialKey[2] = Values[2] ^ 0x07EA99BA ^ SerialKey[0]; 
	SerialKey[3] = Values[3] ^ 0xB5EFD017 ^ SerialKey[0];

	ZEUInt32 Check;
	Check  = SerialKey[0] + 0x46AD788E;
	Check ^= SerialKey[1] - 0x6B71E511;
	Check ^= SerialKey[2] - 0x915067FA;
	Check ^= SerialKey[3] + 0xEE7C85CA;
	SerialKey[4] = Check;

	return ZELCUtils::ConvertSerialKey(SerialKey);
}

ZELCSerialKeyGenerator::ZELCSerialKeyGenerator()
{
	ProductVersionMajor = 1;
	ProductVersionMinor = 0;
	ProductEdition = 0;
	SerialNumber = 0;
}
