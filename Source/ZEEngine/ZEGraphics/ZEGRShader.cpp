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
#include "ZEGRCounter.h"
#include "ZEGRGraphicsModule.h"
#include "ZEFile\ZEFile.h"
#include "ZEGRShaderCompiler.h"
#include "ZEPointer\ZEPointer.h"

bool ZEGRShader::Initialize(ZEGRShaderType ShaderType, void* ShaderBinary, ZESize Size)
{
	this->ShaderType = ShaderType;

	SetSize(Size);
	ZEGR_COUNTER_RESOURCE_INCREASE(this, Shader, Pipeline);

	return true;
}

void ZEGRShader::Deinitialize()
{
	ZEGR_COUNTER_RESOURCE_DECREASE(this, Shader, Pipeline);
	SetSize(0);
}

ZEGRShader::ZEGRShader()
{
	ShaderType = ZEGR_ST_NONE;
}

ZEGRShader::~ZEGRShader()
{
	Deinitialize();
}

ZEGRResourceType ZEGRShader::GetResourceType()
{
	return ZEGR_RT_SHADER;
}

ZEGRShaderType ZEGRShader::GetShaderType()
{
	return ShaderType;
}

ZEGRShader* ZEGRShader::Create(ZEGRShaderType ShaderType, void* ShaderBinary, ZESize Size)
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

ZEGRShader* ZEGRShader::Compile(const ZEGRShaderCompileOptions& Options)
{
	ZEFile File;
	if (!File.Open(Options.FileName, ZE_FOM_READ, ZE_FCM_NONE))
	{
		zeError("Cannot open shader file. File Name: \"%s", Options.FileName.ToCString());
		return NULL;
	}

	File.Seek(0, ZE_SF_END);
	ZEUInt64 Size = File.Tell();
	File.Seek(0, ZE_SF_BEGINING);

	ZEBYTE* Buffer = new ZEBYTE[Size];
	if (File.Read(Buffer, Size, 1) != 1)
	{
		zeError("Cannot read shader file. File Name: \"%s", Options.FileName.ToCString());
		return NULL;
	}

	File.Close();

	ZEGRShaderCompileOptions UpdatedOptions = Options;
	ZEPointer<ZEGRShaderCompiler> Compiler = ZEGRShaderCompiler::CreateInstance();
	UpdatedOptions.SourceData = (char*)Buffer;
	return Compiler->Compile(Options, NULL, NULL);
}
