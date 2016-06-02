//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11ShaderCompilerIncludeInterface.cpp
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

#include "ZED11ShaderCompilerIncludeInterface.h"

#include "ZEGraphics\ZEGRShaderCompileOptions.h"
#include "ZEFile\ZEFile.h"
#include "ZEDS\ZEFormat.h"
#include "ZEFile\ZEFileInfo.h"
#include "ZEProtect\ZELCEncryption.h"
#include "ZEFile\ZEPathManager.h"

void  ZED11ShaderCompilerIncludeInterface::SetCompileOptions(const ZEGRShaderCompileOptions* Options)
{
	CompileOptions = Options;
}

const ZEGRShaderCompileOptions* ZED11ShaderCompilerIncludeInterface::GetCompileOptions()
{
	return CompileOptions;
}

bool ZED11ShaderCompilerIncludeInterface::ReadInclude(const ZEString& pFileName, LPCVOID* ppData, UINT * pBytes)
{
	ZEFile IncludeFile;

	ZEFileInfo FileInfo(pFileName);
	ZEFileInfo EncriptedFileInfo(pFileName + ".ZEEnc");
	if (EncriptedFileInfo.IsExists() && (EncriptedFileInfo.GetModificationTime() > FileInfo.GetModificationTime()))
	{
		if (!IncludeFile.Open(pFileName + ".ZEEnc", ZE_FOM_READ, ZE_FCM_NONE))
			return false;
	}
	else
	{
		if (!IncludeFile.Open(pFileName, ZE_FOM_READ, ZE_FCM_NONE))
			return false;
	}

	ZEArray<ZEBYTE> Buffer;
	Buffer.SetCount(IncludeFile.GetSize());
	IncludeFile.Read(Buffer.GetCArray(), Buffer.GetSize(), 1);
	IncludeFile.Close();

	const ZEBYTE Key[32] =
	{
		0x49, 0x5d, 0xea, 0xff, 0x97, 0x48, 0x08, 0xd4, 
		0xc8, 0xa9,	0xcd, 0xf6, 0x49, 0xb0, 0xa3, 0x72, 
		0xa6, 0xa1, 0xf5, 0xf9, 0xf3, 0xeb, 0xa4, 0xfe, 
		0x56, 0xc4, 0x1c, 0x09, 0x42, 0xa8, 0x4c, 0xd8
	};

	if (*(ZEUInt32*)Buffer.GetCArray() == 'ZEEN')
	{
		ZEArray<ZEBYTE> Output;
		ZELCEncryption::AESDecrypt(Output, Buffer.GetCArray() + 4, Buffer.GetCount() - 4, Key, 32);
		*pBytes =  (UINT)Output.GetCount();
		*ppData = new ZEBYTE[*pBytes];
		memcpy((void*)*ppData, Output.GetConstCArray(), *pBytes);
	}
	else
	{
		*pBytes =  (UINT)Buffer.GetCount();
		*ppData = new ZEBYTE[*pBytes];
		memcpy((void*)*ppData, Buffer.GetConstCArray(), *pBytes);

		ZEArray<ZEBYTE> Output;
		ZELCEncryption::AESEncrypt(Output, Buffer.GetCArray(), Buffer.GetCount(), Key, 32);

		bool AccessControl = ZEPathManager::GetInstance()->GetAccessControl();
		ZEPathManager::GetInstance()->SetAccessControl(false);
		ZEFile File;
		if (File.Open(pFileName + ".ZEEnc", ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
		{
			ZEUInt32 Header = 'ZEEN';
			File.Write(&Header, sizeof(ZEUInt32), 1);

			File.Write(Output.GetCArray(), Output.GetCount(), 1);
			File.Close();
		}
		ZEPathManager::GetInstance()->SetAccessControl(AccessControl);
	}
	
	return true;

}

HRESULT __stdcall ZED11ShaderCompilerIncludeInterface::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT * pBytes)
{
	if (ReadInclude(ZEFormat::Format("{0}/{1}", ZEFileInfo(CompileOptions->FileName).GetParentDirectory(), pFileName), ppData, pBytes))
	{
		return S_OK;
	}
	else
	{
		for (ZESize I = 0; I < CompileOptions->IncludeDirectories.GetCount(); I++)
		{
			if (!ReadInclude(ZEFormat::Format("{0}/{1}", ZEFileInfo(CompileOptions->FileName).GetParentDirectory(), pFileName), ppData, pBytes))
				continue;

			return S_OK;
		}
	}

	*ppData = NULL;
	return S_OK;
}

HRESULT __stdcall ZED11ShaderCompilerIncludeInterface::Close(LPCVOID pData)
{
	if (pData != NULL)
		delete[] pData;
	return S_OK;
}

ZED11ShaderCompilerIncludeInterface::ZED11ShaderCompilerIncludeInterface()
{
	CompileOptions = NULL;
}
