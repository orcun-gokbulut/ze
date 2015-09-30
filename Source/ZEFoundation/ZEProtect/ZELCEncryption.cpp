//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELCEncryption.cpp
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

#include "ZELCEncryption.h"

#include "rsa.h"
#include "osrng.h"

static CryptoPP::AutoSeededRandomPool RandomSeedPool;

void ZELCRSAEncription::SetPrivateKey(const ZEArray<ZEBYTE>& KeyData)
{
	PrivateKey = KeyData;
}

const ZEArray<ZEBYTE>& ZELCRSAEncription::GetPrivateKey()
{
	return PrivateKey;
}

void ZELCRSAEncription::SetPublicKey(const ZEArray<ZEBYTE>& KeyData)
{
	PublicKey = KeyData;
}

const ZEArray<ZEBYTE>& ZELCRSAEncription::GetPubkicKey()
{
	return PublicKey;
}

void ZELCRSAEncription::Encrypt(ZEArray<ZEBYTE>& Output, void* Input, ZESize InputSize)
{
	Output.Clear();

	CryptoPP::RSA::PublicKey RSAPublicKey;
	RSAPublicKey.BERDecodePublicKey(CryptoPP::ArraySource((const byte*)PublicKey.GetCArray(), PublicKey.GetSize(), true), false, PublicKey.GetSize());

	CryptoPP::RSAES_OAEP_SHA_Encryptor Encryptor(RSAPublicKey);
	if (Encryptor.FixedMaxPlaintextLength() < InputSize)
		return;

	ZESize OutputSize = Encryptor.CiphertextLength(InputSize);
	if (OutputSize == 0)
		return;

	Output.SetCount(OutputSize);
	Encryptor.Encrypt(RandomSeedPool, (const byte*)Input, InputSize, (byte*)Output.GetCArray());
}

void ZELCRSAEncription::Decrypt(ZEArray<ZEBYTE>& Output, void* Input, ZESize InputSize)
{
	Output.Clear();

	CryptoPP::RSA::PrivateKey RSAPrivateKey;
	RSAPrivateKey.BERDecodePrivateKey(CryptoPP::ArraySource((const byte*)PrivateKey.GetCArray(), PrivateKey.GetCount(), true), false, PrivateKey.GetCount());
	CryptoPP::RSAES_OAEP_SHA_Decryptor Decryptor(RSAPrivateKey);

	if (Decryptor.FixedCiphertextLength() == 0)
		return;

	ZESize OutputSize = Decryptor.MaxPlaintextLength(InputSize);
	if (OutputSize == 0)
		return;

	CryptoPP::SecByteBlock OutputBlock(OutputSize);
	CryptoPP::DecodingResult Result = Decryptor.Decrypt(RandomSeedPool, (byte*)Input, InputSize, OutputBlock);
	if (!Result.isValidCoding || Result.messageLength > OutputSize)
		return;

	Output.SetCount(Result.messageLength);
	memcpy(Output.GetCArray(), OutputBlock.data(), Output.GetCount());
}

void ZELCRSAEncription::GenerateKeys(ZEUInt KeySize)
{
	CryptoPP::RSA::PrivateKey RSAPrivateKey;
	RSAPrivateKey.GenerateRandomWithKeySize(RandomSeedPool, KeySize);

	CryptoPP::RSA::PublicKey RSAPublicKey(RSAPrivateKey);

	CryptoPP::ByteQueue PrivateKeyBytes;
	RSAPrivateKey.DEREncodePrivateKey(PrivateKeyBytes);
	PrivateKey.SetCount(PrivateKeyBytes.CurrentSize());
	PrivateKeyBytes.Get((byte*)PrivateKey.GetCArray(), PrivateKey.GetCount());

	CryptoPP::ByteQueue PublicKeyBytes;
	RSAPrivateKey.DEREncodePublicKey(PublicKeyBytes);
	PublicKey.SetCount(PublicKeyBytes.CurrentSize());
	PublicKeyBytes.Get((byte*)PublicKey.GetCArray(), PublicKey.GetCount());
}
