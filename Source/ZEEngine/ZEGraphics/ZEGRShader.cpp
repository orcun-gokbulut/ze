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

#include "ZEEncryption.h"
#include "ZEFile/ZEFile.h"
#include "ZEFile/ZEPathManager.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEPointer/ZEPointer.h"

#include "ZEGRGraphicsModule.h"
#include "ZEGRShaderCompiler.h"

static ZEArray<ZEList2<ZEGRShader>> ShaderCache(1024);

bool ZEGRShader::CheckIncludeFile(const ZEGRShaderCompileOptions& Options, const ZEString& Filename)
{
	ZEFileInfo FileInfo(Options.PrecompiledFilename);
	ZEFileInfo IncludeFileInfo(FileInfo.GetParentDirectory() + "/" + Filename);
	bool FileExist = false;

	if (IncludeFileInfo.IsExists())
	{
		FileExist = true;
	}
	else
	{
		ze_for_each(Directory, Options.IncludeDirectories)
		{
			IncludeFileInfo.SetPath(Directory.GetItem() + "/" + Filename);

			if (IncludeFileInfo.IsExists())
			{
				FileExist = true;
				break;
			}
		}
	}
	
	return FileExist && IncludeFileInfo.GetModificationTime() > FileInfo.GetModificationTime();
}

bool ZEGRShader::RequiredCompilation(const ZEGRShaderCompileOptions& Options, ZEArray<ZEBYTE>& ShaderByteCode)
{
	ZEFileInfo FileInfo(Options.FileName);
	ZEFileInfo CompiledFileInfo(Options.PrecompiledFilename);
	if (!CompiledFileInfo.IsExists() || (CompiledFileInfo.GetModificationTime() < FileInfo.GetModificationTime()))
		return true;
	
	ZEFile File;
	if (!File.Open(CompiledFileInfo.GetPath(), ZE_FOM_READ, ZE_FCM_NONE))
		return false;

	ZEUInt64 Size = File.GetSize();
	ZEArray<ZEBYTE> ShaderData;
	ShaderData.SetCount(Size);

	if (File.Read(ShaderData.GetCArray(), Size, 1) != 1)
	return true;

	File.Close();

	ZEUInt ReadIndex = 0;
	ZEBYTE IncludeFileCount = ShaderData[ReadIndex++];


	for (ZEUInt I = 0; I < IncludeFileCount; I++)
	{
		ZEString IncludeFilename;
		IncludeFilename.SetValue(&ShaderData[ReadIndex + 1], ShaderData[ReadIndex]);

		if (CheckIncludeFile(Options, IncludeFilename))
			return true;

		ReadIndex += ShaderData[ReadIndex] + 1;
	}

	ShaderByteCode.AddMultiple(&ShaderData[ReadIndex], ShaderData.GetCount() - ReadIndex);

	return false;
}

bool ZEGRShader::LoadFromFile(ZEArray<ZEBYTE>& Output, const ZEString& FileName)
{
	ZEFile File;
	ZEFileInfo FileInfo(FileName);
	ZEFileInfo EncriptedFileInfo(FileName + ".ZEEnc");
	if (EncriptedFileInfo.IsExists() && (EncriptedFileInfo.GetModificationTime() > FileInfo.GetModificationTime()))
	{
		if (!File.Open(EncriptedFileInfo.GetPath(), ZE_FOM_READ, ZE_FCM_NONE))
			return false;
	}
	else
	{
		if (!File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE))
			return false;
	}
	
	ZEUInt64 Size = File.GetSize();
	ZEArray<ZEBYTE> Buffer;
	Buffer.SetCount(Size);

	if (File.Read(Buffer.GetCArray(), Size, 1) != 1)
	{
		zeError("Cannot read shader file. File Name: \"%s", FileName.ToCString());
		return false;
	}
	File.Close();
	
	const ZEBYTE Key[32] =
	{
		0x49, 0x5d, 0xea, 0xff, 0x97, 0x48, 0x08, 0xd4, 
		0xc8, 0xa9,	0xcd, 0xf6, 0x49, 0xb0, 0xa3, 0x72, 
		0xa6, 0xa1, 0xf5, 0xf9, 0xf3, 0xeb, 0xa4, 0xfe, 
		0x56, 0xc4, 0x1c, 0x09, 0x42, 0xa8, 0x4c, 0xd8
	};
	
	if (*(ZEUInt32*)Buffer.GetCArray() == 'ZEEN')
	{
		ZELCEncryption::AESDecrypt(Output, Buffer.GetCArray() + 4, Size - 4, Key, 32);
	}
	else
	{
		ZELCEncryption::AESEncrypt(Output, Buffer.GetCArray(), Size, Key, 32);
	
		bool AccessControl = ZEPathManager::GetInstance()->GetAccessControl();
		ZEPathManager::GetInstance()->SetAccessControl(false);
		ZEFile File;
		if (File.Open(FileName + ".ZEEnc", ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
		{
			ZEUInt32 Header = 'ZEEN';
			File.Write(&Header, sizeof(ZEUInt32), 1);
				
			File.Write(Output.GetCArray(), Output.GetCount(), 1);
			File.Close();
		}
		ZEPathManager::GetInstance()->SetAccessControl(AccessControl);
		
		Output = Buffer;
	}
	
	Output.Add('\0');

	return true;
}

bool ZEGRShader::Initialize(ZEGRShaderType ShaderType, const void* ShaderBinary, ZESize Size)
{
	this->ShaderType = ShaderType;
	SetSize(Size);

	return true;
}

void ZEGRShader::Deinitialize()
{
	SetSize(0);
}

void ZEGRShader::PreDestroy()
{
	ShaderCache.LockWrite();
	{
		ZEUInt32 Index = CompileOptions.GetHash() % ShaderCache.GetCount();
		ShaderCache[Index].Remove(&ShaderCacheLink);
	}
	ShaderCache.UnlockWrite();
}

ZEGRShader::ZEGRShader() : ShaderCacheLink(this)
{
	ShaderType = ZEGR_ST_NONE;

	Register();
}

ZEGRShader::~ZEGRShader()
{
	Unregister();
}

ZEGRResourceType ZEGRShader::GetResourceType() const
{
	return ZEGR_RT_SHADER;
}

ZEGRShaderType ZEGRShader::GetShaderType() const
{
	return ShaderType;
}

const ZEGRShaderCompileOptions& ZEGRShader::GetCompileOptions() const
{
	return CompileOptions;
}

ZEHolder<ZEGRShader> ZEGRShader::CreateInstance(ZEGRShaderType ShaderType, const void* ShaderBinary, ZESize Size)
{
	ZEHolder<ZEGRShader> Shader = ZEGRGraphicsModule::GetInstance()->CreateShader();
	if (Shader == NULL)
		return NULL;

	if (!Shader->Initialize(ShaderType, ShaderBinary, Size))
		return NULL;

	return Shader;
}

ZEHolder<ZEGRShader> ZEGRShader::Compile(const ZEGRShaderCompileOptions& Options)
{
	ZEUInt32 Hash = Options.GetHash();
	ZEUInt32 Index = Hash % ShaderCache.GetCount();

	ShaderCache[Index].LockRead();
	ze_for_each(Entry, ShaderCache[Index])
	{
		ZEGRShaderCompileOptions DestOptions = Entry->GetCompileOptions();
		if (DestOptions.Equal(Options))
		{
			ZEHolder<ZEGRShader> Temp = Entry.GetPointer();
			ShaderCache[Index].UnlockRead();

			return Temp;
		}
	}
	ShaderCache[Index].UnlockRead();

	ZEGRShaderCompileOptions UpdatedOptions = Options;
	if (UpdatedOptions.PrecompiledFilename.IsEmpty())
		UpdatedOptions.PrecompiledFilename = ZEFileInfo(Options.FileName).GetParentDirectory() + "/" + Options.EntryPoint + ZEString::FromUInt32(Hash) + ".ZECS";
	
	ZEArray<ZEBYTE> ShaderByteCode;
	if (RequiredCompilation(UpdatedOptions, ShaderByteCode))
	{
		zeLog("Compiling shader. Type: \"%s\", Entry: \"%s\", Filename: \"%s\"", ZEGRShaderType_Enumerator()->ToText(Options.Type).ToCString(), Options.EntryPoint.ToCString(), Options.FileName.ToCString());

		if (!LoadFromFile(UpdatedOptions.SourceData, UpdatedOptions.FileName))
			return NULL;
		
		ZEPointer<ZEGRShaderCompiler> Compiler = ZEGRShaderCompiler::CreateInstance();
		if (!Compiler->Compile(ShaderByteCode, UpdatedOptions, NULL, NULL))
			return NULL;

		ZEArray<ZEBYTE> ShaderData;

		ShaderData.Add(UpdatedOptions.IncludeFiles.GetCount());
		ze_for_each(IncludeFile, UpdatedOptions.IncludeFiles)
		{
			ShaderData.Add(IncludeFile.GetItem().GetLength() + 1);
			ShaderData.AddMultiple(reinterpret_cast<ZEBYTE*>(const_cast<char*>(IncludeFile.GetItem().ToCString())), IncludeFile.GetItem().GetLength() + 1);
		}

		ShaderData.AddMultiple(ShaderByteCode);

		bool AccessControl = ZEPathManager::GetInstance()->GetAccessControl();
		ZEPathManager::GetInstance()->SetAccessControl(false);
		ZEFile File;
		if (File.Open(UpdatedOptions.PrecompiledFilename, ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
		{
			File.Write(ShaderData.GetCArray(), ShaderData.GetCount(), 1);
			File.Close();
		}
		ZEPathManager::GetInstance()->SetAccessControl(AccessControl);
	}
	
	ZEHolder<ZEGRShader> Shader = ZEGRShader::CreateInstance(Options.Type, ShaderByteCode.GetConstCArray(), ShaderByteCode.GetCount());
	if (Shader != NULL)
	{
		Shader->CompileOptions = Options;
		ShaderCache[Index].AddEnd(&Shader->ShaderCacheLink);
	}
	return Shader;
}
