//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRShader.cpp
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

#include "ZEGRShader.h"

#include "ZEFile/ZEFile.h"
#include "ZEPointer/ZEPointer.h"
#include "ZEGRGraphicsModule.h"
#include "ZEGRShaderCompiler.h"
#include "ZEProtect/ZELCEncryption.h"
#include "ZEFile/ZEPathManager.h"
#include "ZEFile/ZEFileInfo.h"

bool ZEGRShader::Initialize(ZEGRShaderType ShaderType, const void* ShaderBinary, ZESize Size)
{
	this->ShaderType = ShaderType;

	return true;
}

void ZEGRShader::Deinitialize()
{

}

ZEGRShader::ZEGRShader()
{
	ShaderType = ZEGR_ST_NONE;
}

ZEGRShaderType ZEGRShader::GetShaderType() const
{
	return ShaderType;
}

ZEHolder<ZEGRShader> ZEGRShader::CreateInstance(ZEGRShaderType ShaderType, const void* ShaderBinary, ZESize Size)
{
	ZEGRShader* Shader = ZEGRGraphicsModule::GetInstance()->CreateShader();
	if (Shader == NULL)
		return NULL;

	if (!Shader->Initialize(ShaderType, ShaderBinary, Size))
	{
		Shader->Destroy();
		return NULL;
	}

	return Shader;
}

ZEHolder<ZEGRShader> ZEGRShader::Compile(const ZEGRShaderCompileOptions& Options)
{
	ZEFile File;

	ZEFileInfo FileInfo(Options.FileName);
	ZEFileInfo EncriptedFileInfo(Options.FileName + ".ZEEnc");
	if (EncriptedFileInfo.IsExists() && (EncriptedFileInfo.GetModificationTime() > FileInfo.GetModificationTime()))
	{
		if (!File.Open(EncriptedFileInfo.GetPath(), ZE_FOM_READ, ZE_FCM_NONE))
			return false;
	}
	else
	{
		if (!File.Open(Options.FileName, ZE_FOM_READ, ZE_FCM_NONE))
			return false;
	}

	ZEUInt64 Size = File.GetSize();
	ZEArray<ZEBYTE> Buffer;
	Buffer.SetCount(Size + 1);

	if (File.Read(Buffer.GetCArray(), Size, 1) != 1)
	{
		zeError("Cannot read shader file. File Name: \"%s", Options.FileName.ToCString());
		return NULL;
	}
	File.Close();

	const ZEBYTE Key[32] =
	{
		0x49, 0x5d, 0xea, 0xff, 0x97, 0x48, 0x08, 0xd4, 
		0xc8, 0xa9,	0xcd, 0xf6, 0x49, 0xb0, 0xa3, 0x72, 
		0xa6, 0xa1, 0xf5, 0xf9, 0xf3, 0xeb, 0xa4, 0xfe, 
		0x56, 0xc4, 0x1c, 0x09, 0x42, 0xa8, 0x4c, 0xd8
	};

	ZEGRShaderCompileOptions UpdatedOptions = Options;
	if (*(ZEUInt32*)Buffer.GetCArray() == 'ZEEN')
	{
		ZEArray<ZEBYTE> Output;
		ZELCEncryption::AESDecrypt(Output, Buffer.GetCArray() + 4, Size - 4, Key, 32);
		Output.Add('\0');
		UpdatedOptions.SourceData = reinterpret_cast<char*>(Output.GetCArray());
	}
	else
	{
		Buffer[Size] = '\0';
		UpdatedOptions.SourceData = reinterpret_cast<char*>(Buffer.GetCArray());

		ZEArray<ZEBYTE> Output;
		ZELCEncryption::AESEncrypt(Output, Buffer.GetCArray(), Size, Key, 32);

		bool AccessControl = ZEPathManager::GetInstance()->GetAccessControl();
		ZEPathManager::GetInstance()->SetAccessControl(false);
		ZEFile File;
		if (File.Open(Options.FileName + ".ZEEnc", ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
		{
			ZEUInt32 Header = 'ZEEN';
			File.Write(&Header, sizeof(ZEUInt32), 1);
				
			File.Write(Output.GetCArray(), Output.GetCount(), 1);
			File.Close();
		}
		ZEPathManager::GetInstance()->SetAccessControl(AccessControl);
	}

	#ifdef ZEGR_DEBUG_SHADERS
		UpdatedOptions.Debug = true;
		UpdatedOptions.OptimizationLevel = 0;
	#endif
		
	ZEArray<ZEBYTE> OutputShaderBinary;
	ZEPointer<ZEGRShaderCompiler> Compiler = ZEGRShaderCompiler::CreateInstance();
	if (!Compiler->Compile(OutputShaderBinary, UpdatedOptions, NULL, NULL))
		return NULL;

	return ZEGRShader::CreateInstance(Options.Type, OutputShaderBinary.GetConstCArray(), OutputShaderBinary.GetCount());
}
