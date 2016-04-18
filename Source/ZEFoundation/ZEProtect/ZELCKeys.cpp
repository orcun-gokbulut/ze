//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELCKeys.cpp
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

#include "ZELCKeys.h"

#include "ZEEncryption.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEML/ZEMLFormatXMLV1.h"

#define ZELC_AES_BLOCK_SIZE 16

ZEArray<ZEBYTE> ZELCKeys::GetPreActivationPrivateKey(const ZEString& Password)
{
	ZEBYTE Passkey[32];
	ZELCEncryption::AESPasskey(Passkey, 32, Password);

	ZEArray<ZEBYTE> Output;
	ZELCEncryption::AESDecrypt(Output, GetEncryptedPreActivationPrivateKey(), GetEncryptedPreActivationPrivateKeySize(), Passkey, 32);
	
	return Output;
}

ZESize ZELCKeys::GetPreActivationPrivateKeySize()
{
	return GetEncryptedPreActivationPrivateKeySize() - ZELC_AES_BLOCK_SIZE;
}

ZEArray<ZEBYTE> ZELCKeys::GetActivationPrivateKey(const ZEString& Password)
{
	ZEBYTE Passkey[32];
	ZELCEncryption::AESPasskey(Passkey, 32, Password);

	ZEArray<ZEBYTE> Output;
	ZELCEncryption::AESDecrypt(Output, GetEncryptedActivationPrivateKey(), GetEncryptedActivationPrivateKeySize(), Passkey, 32);

	return Output;
}

ZESize ZELCKeys::GetActivationPrivateKeySize()
{
	return GetEncryptedActivationPrivateKeySize() - ZELC_AES_BLOCK_SIZE;
}

static void GenerateKeyAccessFunctions(FILE* Output, const ZEString& KeyName, const ZEArray<ZEBYTE>& Key)
{
	fprintf(Output, 
		"const ZEBYTE* ZELCKeys::Get%sKey()\n"
		"{\n",
		KeyName.ToCString());

	fprintf(Output, 
		"\tstatic const ZEBYTE Key[%d] = \n"
		"\t{", 
		Key.GetCount());

	for (ZESize I = 0; I < Key.GetCount(); I++)
	{
		if ((I % 16) == 0)
			fprintf(Output, "\n\t\t");
		fprintf(Output, "0x%02X, ", Key[I]);

	}
	fprintf(Output, 
		"\n"
		"\t};\n\n"
		"\treturn Key;\n"
		"}\n\n");

	fprintf(Output, 
		"ZESize ZELCKeys::Get%sKeySize()\n"
		"{\n",
		KeyName.ToCString());

	fprintf(Output, 
		"\t return %d;\n"
		"}\n\n", Key.GetCount());
}

bool ZELCKeys::GenerateKeys(const ZEString& Password, const ZEString& KeyFile, const ZEString& KeyCodeFile)
{
	// SETUP PASSKEY
	///////////////////////////////////////////////////////////////////////////////

	ZEBYTE Passkey[32];
	ZELCEncryption::AESPasskey(Passkey, 32, Password);


	// GENERATE KEYS
	///////////////////////////////////////////////////////////////////////////////

	ZEArray<ZEBYTE> PreActivationPrivateKey;
	ZEArray<ZEBYTE> PreActivationPublicKey;
	ZELCEncryption::RSAGenerateKeys(1024, PreActivationPrivateKey, PreActivationPublicKey);

	ZEArray<ZEBYTE> ActivationPrivateKey;
	ZEArray<ZEBYTE> ActivationPublicKey;
	ZELCEncryption::RSAGenerateKeys(1024, ActivationPrivateKey, ActivationPublicKey);

	ZEArray<ZEBYTE> EncryptedPreActivationPrivateKey;
	ZEArray<ZEBYTE> EncryptedActivationPrivateKey;
	ZELCEncryption::AESEncrypt(EncryptedPreActivationPrivateKey, PreActivationPrivateKey.GetConstCArray(), PreActivationPrivateKey.GetCount(), Passkey, 32);
	ZELCEncryption::AESEncrypt(EncryptedActivationPrivateKey, ActivationPrivateKey.GetConstCArray(), ActivationPrivateKey.GetCount(), Passkey, 32);


	// GENERATE KEY FILE
	///////////////////////////////////////////////////////////////////////////////

	if (!KeyFile.IsEmpty())
	{
		ZEMLWriter Writer;
		Writer.SetFormat(new ZEMLFormatXMLV1());
		if (!Writer.Open(KeyFile))
			return false;

		ZEMLWriterNode RootNode;
		Writer.OpenRootNode("Keys", RootNode);
		RootNode.WriteUInt8("VersionMajor", 1);
		RootNode.WriteUInt8("VersionMinor", 0);
		RootNode.WriteData("EncryptedPreActivationPrivateKey", EncryptedPreActivationPrivateKey.GetConstCArray(), EncryptedPreActivationPrivateKey.GetCount());
		RootNode.WriteData("PreActivationPublicKey", PreActivationPublicKey.GetConstCArray(), PreActivationPublicKey.GetCount());
		RootNode.WriteData("EncryptedActivationPrivateKey", EncryptedActivationPrivateKey.GetConstCArray(), EncryptedActivationPrivateKey.GetCount());
		RootNode.WriteData("ActivationPublicKey", ActivationPublicKey.GetConstCArray(), ActivationPublicKey.GetCount());
		RootNode.CloseNode();
		Writer.Close();
	}
	

	// GENERATE PUBLIC KEY SOURCE CODE
	///////////////////////////////////////////////////////////////////////////////

	if (!KeyCodeFile.IsEmpty())
	{
		FILE* File = fopen(KeyCodeFile, "w");
		if (File == NULL)
			return false;

		fprintf(File, "#include \"ZELCKeys.h\"\n\n");

		GenerateKeyAccessFunctions(File, "EncryptedPreActivationPrivate", EncryptedPreActivationPrivateKey);
		GenerateKeyAccessFunctions(File, "PreActivationPublic", PreActivationPublicKey);
		GenerateKeyAccessFunctions(File, "EncryptedActivationPrivate", EncryptedActivationPrivateKey);
		GenerateKeyAccessFunctions(File, "ActivationPublic", ActivationPublicKey);

		fclose(File);
	}

	return true;
}
