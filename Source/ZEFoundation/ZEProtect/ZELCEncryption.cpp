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

#include <osrng.h>
#include <modes.h>
#include <rsa.h>
#include <aes.h>
#include <pssr.h>

static CryptoPP::AutoSeededRandomPool RandomSeedPool;

void ZELCEncryption::AESPasskey(void* OutputKey, ZESize KeySize, const ZEString& Password)
{
	ZESize PasskeySize = Password.GetLength();
	if (PasskeySize > KeySize)
		PasskeySize = KeySize;

	memset(OutputKey, 0, KeySize);
	memcpy(OutputKey, Password.GetValue(), PasskeySize);
}

void ZELCEncryption::AESEncrypt(ZEArray<ZEBYTE>& Output, const void* Input, ZESize InputSize, const void* Key, ZESize KeySize)
{
	byte iv[CryptoPP::AES::BLOCKSIZE];
	RandomSeedPool.GenerateBlock(iv, CryptoPP::AES::BLOCKSIZE);

	CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption Encryption((const byte*)Key, KeySize, iv);
	Output.SetCount(InputSize);
	Encryption.ProcessData(Output.GetCArray(), (const byte*)Input, InputSize);
	
	Output.MassInsert(0, iv, CryptoPP::AES::BLOCKSIZE);
}

void ZELCEncryption::AESDecrypt(ZEArray<ZEBYTE>& Output, const void* Input, ZESize InputSize, const void* Key, ZESize KeySize)
{
	byte iv[CryptoPP::AES::BLOCKSIZE];
	memcpy(iv, Input, CryptoPP::AES::BLOCKSIZE);

	CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption Decryption((const byte*)Key, KeySize, iv);
	Output.SetCount(InputSize - CryptoPP::AES::BLOCKSIZE);
	Decryption.ProcessData(Output.GetCArray(), (const byte*)Input + CryptoPP::AES::BLOCKSIZE, Output.GetCount());
}

void ZELCEncryption::RSAEncrypt(ZEArray<ZEBYTE>& Output, const void* Input, ZESize InputSize, const void* PublicKey, ZESize KeySize)
{
	Output.Clear();

	try
	{
		CryptoPP::RSA::PublicKey RSAPrivateKey;
		RSAPrivateKey.BERDecodePublicKey(CryptoPP::ArraySource((const byte*)PublicKey, KeySize, true), false, KeySize);

		CryptoPP::RSAES_OAEP_SHA_Encryptor Encryptor(RSAPrivateKey);
		if (Encryptor.FixedMaxPlaintextLength() < InputSize)
			return;

		ZESize OutputSize = Encryptor.CiphertextLength(InputSize);
		if (OutputSize == 0)
			return;

		Output.SetCount(OutputSize);
		Encryptor.Encrypt(RandomSeedPool, (const byte*)Input, InputSize, (byte*)Output.GetCArray());
	}
	catch (...)
	{
	}
}

void ZELCEncryption::RSADecrypt(ZEArray<ZEBYTE>& Output, const void* Input, ZESize InputSize, const void* PrivateKey, ZESize KeySize)
{
	Output.Clear();

	try 
	{
		CryptoPP::RSA::PrivateKey RSAPrivateKey;
		RSAPrivateKey.BERDecodePrivateKey(CryptoPP::ArraySource((const byte*)PrivateKey, KeySize, true), false, KeySize);
		CryptoPP::RSAES_OAEP_SHA_Decryptor Decryptor(RSAPrivateKey);
		if (Decryptor.FixedCiphertextLength() == 0)
			return;

		ZESize OutputSize = Decryptor.MaxPlaintextLength(InputSize);
		if (OutputSize == 0)
			return;

		Output.SetCount(OutputSize);
		CryptoPP::DecodingResult Result = Decryptor.Decrypt(RandomSeedPool, (const byte*)Input, InputSize, Output.GetCArray());
		if (!Result.isValidCoding || Result.messageLength > OutputSize)
		{
			Output.Clear();
			return;
		}

		Output.Resize(Result.messageLength);
	}
	catch(...)
	{
	}
}

void ZELCEncryption::RSASign(ZEArray<ZEBYTE>& Output, const void* Input, ZESize InputSize, const void* PrivateKey, ZESize KeySize)
{
	CryptoPP::RSA::PrivateKey RSAPrivateKey;
	RSAPrivateKey.BERDecodePrivateKey(CryptoPP::ArraySource((const byte*)PrivateKey, KeySize, true), false, KeySize);

	CryptoPP::RSASS<CryptoPP::PSS, CryptoPP::SHA1>::Signer Signer(RSAPrivateKey);

	size_t SignatureLenght = Signer.MaxSignatureLength();
	Output.SetCount(SignatureLenght);
	Output.Fill(0);

	SignatureLenght = Signer.SignMessage(RandomSeedPool, (const byte*)Input, InputSize, Output.GetCArray());
	Output.Resize(SignatureLenght);
}

bool ZELCEncryption::RSAVerify(const void* Input, ZESize InputSize, const void* Signature, ZESize SignatureSize, const void* PublicKey, ZESize KeySize)
{
	CryptoPP::RSA::PublicKey RSAPublicKey;
	RSAPublicKey.BERDecodePublicKey(CryptoPP::ArraySource((const byte*)PublicKey, KeySize, true), false, KeySize);

	CryptoPP::RSASS<CryptoPP::PSS, CryptoPP::SHA1>::Verifier Verifier(RSAPublicKey);
	return Verifier.VerifyMessage((const byte*)Input, InputSize, (const byte*)Signature, SignatureSize);
}

void ZELCEncryption::RSAGenerateKeys(ZEUInt KeySize, ZEArray<ZEBYTE>& PrivateKey, ZEArray<ZEBYTE>& PublicKey)
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
