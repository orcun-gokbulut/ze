//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELCActivationCodeGenerator.cpp
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

#include "ZELCActivationCodeGenerator.h"

#include "ZELCKeys.h"
#include "ZELCActivationData.h"
#include "ZELCEncryption.h"
#include "ZEDS\ZEFormat.h"
#include "ZERegEx\ZERegEx.h"
#include "ZELCUtils.h"

void ZELCActivationCodeGenerator::SetPreActivationKey(const void* Key, ZESize KeySize)
{
	PreActivationKey.SetCount(KeySize);
	memcpy(PreActivationKey.GetCArray(), Key, KeySize);
}

const ZEArray<ZEBYTE>& ZELCActivationCodeGenerator::GetPreActivationKey() const
{
	return PreActivationKey;
}

void ZELCActivationCodeGenerator::SetActivationKey(const void* Key, ZESize KeySize)
{
	ActivationKey.SetCount(KeySize);
	memcpy(ActivationKey.GetCArray(), Key, KeySize);
}

const ZEArray<ZEBYTE>& ZELCActivationCodeGenerator::GetActivationKey() const
{
	return ActivationKey;
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
	ZEBYTE PreActivationCode[ZELC_ACTIVATION_CODE_SIZE];
	if (!ZELCUtils::ConvertActivationCode(PreActivationCode, GetPreActivationCode()))
		return false;

	ZEArray<ZEBYTE> ActivationData;
	ZELCEncryption::RSADecrypt(ActivationData, PreActivationCode, ZELC_ACTIVATION_CODE_SIZE, GetPreActivationKey().GetConstCArray(), GetPreActivationKey().GetCount());
	if (ActivationData.GetSize() != sizeof(ZELCActivationData))
		return false;
	
	ZEBYTE SerialKeyBinary[ZELC_SERIAL_KEY_SIZE];
	if (!ZELCUtils::ConvertSerialKey(SerialKeyBinary, GetSerialKey()))
		return false;

	const ZELCActivationData* ActivationDataStruct = (ZELCActivationData*)ActivationData.GetConstCArray();
	if (memcmp(ActivationDataStruct->SerialKey, SerialKeyBinary, ZELC_SERIAL_KEY_SIZE) != 0)
		return false;

	ZEArray<ZEBYTE> Signature;
	ZELCEncryption::RSASign(Signature, ActivationData.GetConstCArray(), sizeof(ZELCActivationData), ActivationKey.GetConstCArray(), ActivationKey.GetCount());

	Output = ZELCUtils::ConvertActivationCode((ZEUInt32*)Signature.GetCArray());

	return true;
}

void ZELCActivationCodeGenerator::LoadKeys(const ZEString& Password)
{
	ZEArray<ZEBYTE> Output = ZELCKeys::GetPreActivationPrivateKey(Password);
	SetPreActivationKey(Output.GetConstCArray(), Output.GetCount());
	Output.Fill(0);

	Output = ZELCKeys::GetActivationPrivateKey(Password);
	SetActivationKey(Output.GetConstCArray(), Output.GetCount());
	Output.Fill(0);
}
