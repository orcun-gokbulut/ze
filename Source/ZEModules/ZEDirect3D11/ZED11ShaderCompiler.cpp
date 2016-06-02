//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11ShaderCompiler.cpp
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

#include "ZED11ShaderCompiler.h"
#include "ZED11ShaderCompilerIncludeInterface.h"
#include "ZED11ShaderMetaCompiler.h"

#define ZE_SHADER_COMPILER_DEFAULT_PARAMETERS	(D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR)
//#define ZE_DEBUG_D3D11_DEBUG_SHADERS

#ifdef ZE_DEBUG_D3D11_DEBUG_SHADERS
	#define ZE_SHADER_COMPILER_PARAMETERS		(ZE_SHADER_COMPILER_DEFAULT_PARAMETERS | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG)
#else
	#define ZE_SHADER_COMPILER_PARAMETERS		(ZE_SHADER_COMPILER_DEFAULT_PARAMETERS | D3DCOMPILE_OPTIMIZATION_LEVEL3)
#endif

#define ZE_D3D11_DEVICE_REGISTER_SIZE			16	// Bytes

typedef void (*ZEGRShaderEditorFunction)( ZEGRShaderCompileOptions& Options);

bool ZED11ShaderCompiler::Compile(ZEArray<ZEBYTE>& OutputByteCode, const ZEGRShaderCompileOptions& Options, ZEGRShaderMeta* Meta, ZEString* Output, bool ShaderEditorOpen)
{
	ZEArray<D3D_SHADER_MACRO> Macros;
	ZED11ShaderCompilerIncludeInterface IncludeInterface;
	ZEString Profile;

	if(!PrepareOptions(Options, Macros, IncludeInterface, Profile))
		return false;

	ID3DBlob* CompileOutput = NULL;
	ID3DBlob* ByteCode = NULL;
	ZEGRShaderCompileOptions ResultOptions = Options;

	// Compile
	HRESULT Result = D3DCompile(Options.SourceData, Options.SourceData.GetSize(), Options.FileName, Macros.GetConstCArray(), &IncludeInterface, Options.EntryPoint, Profile, ZE_SHADER_COMPILER_PARAMETERS, 0, &ByteCode, &CompileOutput);
	if (FAILED(Result))
	{	
		if (!ShaderEditorOpen)
		{
			if(OpenShaderEditor(ResultOptions))
			{
				if(!PrepareOptions(ResultOptions, Macros, IncludeInterface, Profile))
					return false;

				Result = D3DCompile(ResultOptions.SourceData, ResultOptions.SourceData.GetSize(), ResultOptions.FileName, Macros.GetConstCArray(), &IncludeInterface, ResultOptions.EntryPoint, Profile, ZE_SHADER_COMPILER_PARAMETERS, NULL, &ByteCode, &CompileOutput);
			}
		}
		// Compile
		if (FAILED(Result))
		{	
			zeError("Can not compile shader.\r\nFile: \"%s\".\r\nCompile output: \r\n%s\r\n", ResultOptions.FileName.ToCString(), CompileOutput->GetBufferPointer());
			if (Output != NULL)
				*Output = (char*)CompileOutput->GetBufferPointer();
			return false;
		}
	}

	if (Output != NULL && CompileOutput != NULL)
		*Output = (char*)CompileOutput->GetBufferPointer();
	ZEGR_RELEASE(CompileOutput);

	if (Meta != NULL)
	{
		ZED11ShaderMetaCompiler::Reflect(Meta, ByteCode);
		Meta->CompileOptions = ResultOptions;
	}

	OutputByteCode.SetCount(ByteCode->GetBufferSize());
	memcpy(OutputByteCode.GetCArray(), ByteCode->GetBufferPointer(), OutputByteCode.GetCount());
	ZEGR_RELEASE(ByteCode);

	return true;
}

bool ZED11ShaderCompiler::OpenShaderEditor(ZEGRShaderCompileOptions& Options)
{
	 HMODULE Module = GetModuleHandle("ZEDSHShaderEditorDll.dll");
	 if (Module == NULL)
	 {
		 Module = LoadLibrary("ZEDSHShaderEditorDll.dll");
		 if (Module == NULL)
			 return false;
	 }

	 ZEGRShaderEditorFunction Function = (ZEGRShaderEditorFunction)GetProcAddress(Module, "ZEDSHEditor_RunEditor");
	 if(Function == NULL)
		 return false;

	 Function(Options);
	 
	 return true;
}

bool ZED11ShaderCompiler::PrepareOptions(const ZEGRShaderCompileOptions& Options, ZEArray<D3D_SHADER_MACRO>& OutMacros, ZED11ShaderCompilerIncludeInterface& OutIncludeInterface, ZEString& OutProfile)
{
	zeDebugCheck(Options.SourceData.IsEmpty(), "No shader source available.");
	zeDebugCheck(Options.EntryPoint.IsEmpty(), "Shader entry point is not available");
	zeDebugCheck(Options.Model < ZEGR_SM_4_0, "Shader model is not supported by this module.");
	
	// Decide shader type
	ZEString Profile = "";
	switch (Options.Type)
	{
		case ZEGR_ST_VERTEX:
			Profile += "vs";
			break;

		case ZEGR_ST_PIXEL:
			Profile += "ps";
			break;

		case ZEGR_ST_GEOMETRY:
			Profile += "gs";
			break;

		case ZEGR_ST_COMPUTE:
			Profile += "cs";
			break;

		case ZEGR_ST_DOMAIN:
			Profile += "ds";
			break;

		case ZEGR_ST_HULL:
			Profile += "hs";
			break;

		default:
			zeError("Unsupported shader type");
			return false;
	}

	// Decide shader model
	switch (Options.Model)
	{
		case ZEGR_SM_4_0:
			Profile += "_4_0";
			break;

		case ZEGR_SM_4_1:
			Profile += "_4_1";
			break;
		
		case ZEGR_SM_5_0:
			Profile += "_5_0";
			break;

		default:
			zeError("Unsupported shader model");
			return false;
	}
	
	// Fill macros array
	ZESize ComponentCount = Options.Definitions.GetCount();
	ZEArray<D3D_SHADER_MACRO> Macros;
	Macros.SetCount(ComponentCount + 1);
	for (ZESize I = 0; I < ComponentCount; I++)
	{
		Macros[I].Name = Options.Definitions[I].Name.ToCString();
		Macros[I].Definition = Options.Definitions[I].Value.ToCString();
	}
	Macros[ComponentCount].Name = NULL;
	Macros[ComponentCount].Definition = NULL;

	OutProfile = Profile;
	OutMacros = Macros;
	OutIncludeInterface.SetCompileOptions(&Options);

	return true;
}

ZED11ShaderCompiler::ZED11ShaderCompiler()
{
}

ZED11ShaderCompiler::~ZED11ShaderCompiler()
{
}
