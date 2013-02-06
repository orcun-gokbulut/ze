//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9ShaderCompiler.cpp
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

#include "ZED3D9ShaderCompiler.h"

#include <d3dx9.h>
#include "ZEError.h"
#include "ZED3D9Shader.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9GraphicsDevice.h"
#include "ZED3D9GraphicsModule.h"
#include "ZEDS/ZEPointer.h"


#ifdef ZE_DEBUG_D3D9_DEBUG_SHADERS
	#define ZE_SHADER_COMPILER_PARAMETERS	(D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION)
#else
	#define ZE_SHADER_COMPILER_PARAMETERS	D3DXSHADER_OPTIMIZATION_LEVEL3
#endif

class ZED3DXInclude : public ID3DXInclude
{
	public:
		virtual HRESULT __stdcall Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID * ppData, UINT * pBytes)
		{
			char RelativeFileName[258];
			sprintf(RelativeFileName, "resources\\shaders\\%s", pFileName);
			FILE* File = fopen(RelativeFileName, "rb");
			if (File == NULL)
				return S_FALSE;

			_fseeki64(File, 0, SEEK_END);
			*pBytes = (UINT)_ftelli64(File);
			_fseeki64(File, 0, SEEK_SET);
			*ppData = (void*)new char[(ZESize)*pBytes];
			fread((void*)*ppData, 1, (ZESize)*pBytes, File);
			fclose(File);

			return S_OK;
		}

		virtual HRESULT __stdcall Close(LPCVOID pData)
		{
			if (pData != NULL)
				delete[] pData;
			return S_OK;
		}
} D3DXInclude;


static ZEShaderConstantDataType ConvertD3DXType(ZEInt Column, ZEInt Row, D3DXPARAMETER_TYPE Type)
{
	switch (Type)
	{
		case D3DXPT_FLOAT:
			switch (Row)
			{
				case 1:
					switch (Column)
					{
						case 1:
							return ZE_SCDT_FLOAT;
							break;
						case 2:
							return ZE_SCDT_VECTOR2;
							break;
						case 3: 
							return ZE_SCDT_VECTOR3;
							break;
						case 4:
							return ZE_SCDT_VECTOR4;
							break;
					}
					break;

				case 3:
					if (Column == 3)
						return ZE_SCDT_MATRIX3x3;
					break;

				case 4:
					if (Column == 4)
						return ZE_SCDT_MATRIX4x4;
					break;
			}
			break;
		case D3DXPT_BOOL:
			if(Row == 1 && Column == 1)
				return ZE_SCDT_BOOLEAN;
			break;
		case D3DXPT_SAMPLER:
			return ZE_SCDT_SAMPLER;
			break;
		case D3DXPT_SAMPLER1D:
			return ZE_SCDT_SAMPLER1D;
			break;
		case D3DXPT_SAMPLER2D:
			return ZE_SCDT_SAMPLER2D;
			break;
		case D3DXPT_SAMPLER3D:
			return ZE_SCDT_SAMPLER3D;
			break;
		case D3DXPT_SAMPLERCUBE:
			return ZE_SCDT_SAMPLERCUBE;
			break;
	}
	
	zeError("Undefined ShaderConstant Type in 'ZED3D9Shader'");
	return ZE_SCDT_NONE;
}

void ZED3D9ShaderCompiler::CreateConstantTable(ZEShader* Shader, LPD3DXCONSTANTTABLE Table)
{
	if (Shader == NULL || Table == NULL)
		return;
	
	// Get Table description
	D3DXCONSTANTTABLE_DESC TableDescriptor;
	if (FAILED(Table->GetDesc(&TableDescriptor)))
	{
		zeError("Cannot get constant table descriptor.");
		return;
	}
	
	// For Each Number of Constants
	ZESize ConstantCount = TableDescriptor.Constants;
	for (ZESize I = 0; I < ConstantCount; ++I)
	{
		UINT ConstantSize = 1;
		D3DXHANDLE DXConstantHandle;
		ZEShaderConstant CPUConstant;
		D3DXCONSTANT_DESC DXConstantDescriptor;
		
		// Get constant then its description
		DXConstantHandle = Table->GetConstant(NULL, I);
		if (FAILED(Table->GetConstantDesc(DXConstantHandle, &DXConstantDescriptor, &ConstantSize)))
		{
			zeError("Cannot get constant description");
			return;
		}
		
		// If it is a struct
		if(DXConstantDescriptor.StructMembers > 0)
		{
			// If it is array
			if(DXConstantDescriptor.Elements > 1)
			{
				ZESize ElementCount = DXConstantDescriptor.Elements;
				for(ZESize J = 0; J < ElementCount; ++J)
				{
					UINT ArrayHandleSize = 1;
					D3DXCONSTANT_DESC ArrayHandleDescriptor;
					D3DXHANDLE ArrayHandle;
					
					// Get constant element then its description
					ArrayHandle = Table->GetConstantElement(DXConstantHandle, J);
					if (FAILED(Table->GetConstantDesc(ArrayHandle, &ArrayHandleDescriptor, &ArrayHandleSize)))
					{
						zeError("Cannot get constant description");
						return;
					}

					D3DXHANDLE SubConstant;
					ZEShaderConstant CPUSubConstant;
					D3DXCONSTANT_DESC SubConstantDescriptor;
					UINT SubConstantSize = 1;

					// Get Each Data Members
					ZESize StructMemberCount = ArrayHandleDescriptor.StructMembers;
					for (ZESize K = 0; K < StructMemberCount; ++K)
					{
						SubConstant = Table->GetConstant(ArrayHandle, K);
						if (FAILED(Table->GetConstantDesc(SubConstant, &SubConstantDescriptor, &SubConstantSize)))
						{
							zeError("Cannot get constant description");
							return;
						}

						// Create Subscript Syntax
						ZEString Syntax = "[" + ZEString::FromUInt64(J) + "].";
						CPUSubConstant.ConstantName = ArrayHandleDescriptor.Name + Syntax + SubConstantDescriptor.Name;
						CPUSubConstant.Count = SubConstantDescriptor.Elements;
						CPUSubConstant.RegisterNo = SubConstantDescriptor.RegisterIndex;
						CPUSubConstant.Type = ConvertD3DXType(SubConstantDescriptor.Columns, SubConstantDescriptor.Rows, SubConstantDescriptor.Type);
						CPUSubConstant.Semantic = "";		//PLACEHOLDER

						// Add This to Array
						((ZED3D9Shader*)Shader)->ShaderConstants.Add(CPUSubConstant);

						// Now Add Each Element with subscript operator
						if(SubConstantDescriptor.Elements > 1)
						{
							// It is Array
							ZESize SubConstantCount = SubConstantDescriptor.Elements;
							for(ZESize T = 0; T < SubConstantCount; ++T)
							{	
								UINT BaseArrayCount = 1;
								ZEShaderConstant CPUBaseArrayConstant;
								D3DXCONSTANT_DESC BaseArrayDescriptor;
								
								if (FAILED(Table->GetConstantDesc(Table->GetConstantElement(SubConstant, T), &BaseArrayDescriptor, &BaseArrayCount)))
								{
									zeError("Cannot get constant description");
									return;
								}

								// Create Subscript Syntax
								ZEString Syntax2 = ".[" + ZEString::FromUInt64(T) + "]";
								CPUBaseArrayConstant.ConstantName = ArrayHandleDescriptor.Name + Syntax + BaseArrayDescriptor.Name + Syntax2;
								CPUBaseArrayConstant.Count = BaseArrayDescriptor.Elements;
								CPUBaseArrayConstant.RegisterNo = BaseArrayDescriptor.RegisterIndex;
								CPUBaseArrayConstant.Type = ConvertD3DXType(BaseArrayDescriptor.Columns, BaseArrayDescriptor.Rows, BaseArrayDescriptor.Type);
								CPUBaseArrayConstant.Semantic = "";		// PLACEHOLDER

								// Add This to Array
								((ZED3D9Shader*)Shader)->ShaderConstants.Add(CPUBaseArrayConstant);
							}
						}
					}
				}
			}
			else
			{
				// It is not An Array
				UINT SubConstantSize = 1;
				D3DXHANDLE SubConstant;
				ZEShaderConstant SubConstantCPU;
				D3DXCONSTANT_DESC SubConstantDescriptor;
				
				// Get Each Data Members
				ZESize StructMemberCount = DXConstantDescriptor.StructMembers;
				for (ZESize K = 0; K < StructMemberCount; ++K)
				{
					SubConstant = Table->GetConstant(DXConstantHandle, K);
					if (FAILED(Table->GetConstantDesc(SubConstant, &SubConstantDescriptor, &SubConstantSize)))
					{
						zeError("Cannot get constant description");
						return;
					}

					SubConstantCPU.Count = SubConstantDescriptor.Elements;
					SubConstantCPU.RegisterNo = SubConstantDescriptor.RegisterIndex;
					SubConstantCPU.ConstantName = DXConstantDescriptor.Name + ZEString(".") + SubConstantDescriptor.Name;
					SubConstantCPU.Type = ConvertD3DXType(SubConstantDescriptor.Columns, SubConstantDescriptor.Rows, SubConstantDescriptor.Type);
					SubConstantCPU.Semantic = "";		// PLACEHOLDER

					// Add This to Array
					((ZED3D9Shader*)Shader)->ShaderConstants.Add(SubConstantCPU);

					// Now Add Each Element with subscript operator
					if(SubConstantDescriptor.Elements > 1)
					{
						// It is Array
						for(ZESize T = 0; T < SubConstantDescriptor.Elements; ++T)
						{			
							ZEShaderConstant BaseArrayConstantCPU;
							D3DXCONSTANT_DESC BaseArrayDescriptor;
							ZEUInt32 BaseArrayCount = 1;

							Table->GetConstantDesc(Table->GetConstantElement(SubConstant, T), &BaseArrayDescriptor, &BaseArrayCount);

							// Create Subscript Syntax
							ZEString Syntax = ".[" + ZEString::FromUInt64(T) + "]";
							BaseArrayConstantCPU.ConstantName = DXConstantDescriptor.Name + ZEString(".") + BaseArrayDescriptor.Name + Syntax;
							BaseArrayConstantCPU.Count = BaseArrayDescriptor.Elements;
							BaseArrayConstantCPU.RegisterNo = BaseArrayDescriptor.RegisterIndex;
							BaseArrayConstantCPU.Type = ConvertD3DXType(BaseArrayDescriptor.Columns, BaseArrayDescriptor.Rows, BaseArrayDescriptor.Type);
							BaseArrayConstantCPU.Semantic = "";		// PLACEHOLDER

							// Add This to Array
							((ZED3D9Shader*)Shader)->ShaderConstants.Add(BaseArrayConstantCPU);
						}
					}
				}
			}
		}
		else
		{
			// This Constant is not struct
			CPUConstant.Count = DXConstantDescriptor.Elements;
			CPUConstant.ConstantName = DXConstantDescriptor.Name;
			CPUConstant.RegisterNo = DXConstantDescriptor.RegisterIndex;
			CPUConstant.Type = ConvertD3DXType(DXConstantDescriptor.Columns, DXConstantDescriptor.Rows, DXConstantDescriptor.Type);
			CPUConstant.Semantic = "";			// PLACEHOLDER

			// Add This to Array
			((ZED3D9Shader*)Shader)->ShaderConstants.Add(CPUConstant);

			// Now Add Each Element with subscript operator
			if(DXConstantDescriptor.Elements > 1)
			{
				// It is Array
				ZESize ConstantCount = DXConstantDescriptor.Elements;
				for(ZESize T = 0; T < ConstantCount; ++T)
				{	
					ZEUInt32 BaseArrayCount = 1;		
					ZEShaderConstant BaseArrayConstantCPU;
					D3DXCONSTANT_DESC BaseArrayDescriptor;
					
					if (FAILED(Table->GetConstantDesc(Table->GetConstantElement(DXConstantHandle, T), &BaseArrayDescriptor, &BaseArrayCount)))
					{
						zeError("Cannot get constant description");
						return;
					}

					// Create Subscript Syntax
					ZEString Syntax = ".[" + ZEString::FromUInt64(T) + "]";
					BaseArrayConstantCPU.ConstantName = BaseArrayDescriptor.Name + Syntax;
					BaseArrayConstantCPU.Count = BaseArrayDescriptor.Elements;
					BaseArrayConstantCPU.RegisterNo = BaseArrayDescriptor.RegisterIndex;
					BaseArrayConstantCPU.Type = ConvertD3DXType(BaseArrayDescriptor.Columns, BaseArrayDescriptor.Rows, BaseArrayDescriptor.Type);
					BaseArrayConstantCPU.Semantic = "";		// PLACEHOLDER

					// Add This to Array
					((ZED3D9Shader*)Shader)->ShaderConstants.Add(BaseArrayConstantCPU);
				}
			}
		}
	}
}


ZEShader* ZED3D9ShaderCompiler::CreateVertexShader(LPD3DXBUFFER ShaderBuffer)
{
	if (ShaderBuffer == NULL)
		return NULL;

	ZED3D9VertexShader* VertexShader = (ZED3D9VertexShader*)zeGraphics->CreateVertexShader();
	LPDIRECT3DDEVICE9 Device = ((ZED3D9GraphicsModule*)zeGraphics)->GetDirect3DDevice();

	
	if (FAILED(Device->CreateVertexShader((const DWORD*)ShaderBuffer->GetBufferPointer(), &VertexShader->VertexShader)))
	{
		zeError("Cannot create vertex shader.");
		VertexShader->Destroy();
		return NULL;
	}

	return VertexShader;
}

ZEShader* ZED3D9ShaderCompiler::CreatePixelShader(LPD3DXBUFFER ShaderBuffer)
{
	if (ShaderBuffer == NULL)
		return NULL;

	ZED3D9PixelShader* PixelShader = (ZED3D9PixelShader*)zeGraphics->CreatePixelShader();
	LPDIRECT3DDEVICE9 Device = ((ZED3D9GraphicsModule*)zeGraphics)->GetDirect3DDevice();

	if (FAILED(Device->CreatePixelShader((const DWORD*)ShaderBuffer->GetBufferPointer(), &PixelShader->PixelShader)))
	{
		zeError("Cannot create pixel shader.");
		PixelShader->Destroy();
		return NULL;
	}

	return PixelShader;
}


ZEShader* ZED3D9ShaderCompiler::CompileShader(const ZEShaderCompileOptions& Options)
{
	zeDebugCheck(Options.SourceData.IsEmpty(), "No shader source available.");
	zeDebugCheck(Options.EntryPoint.IsEmpty(), "Shader entry point is not available");
	zeDebugCheck(Options.Model != ZE_SM_3_0, "Shader model is not supported by this module.");
	zeDebugCheck(Options.Type != ZE_ST_PIXEL || Options.Type != ZE_ST_VERTEX, "Shader type is not supported by this module.");

	HRESULT Result;

	char ShaderProfile[10];
	switch (Options.Type)
	{
		case ZE_ST_VERTEX:
			sprintf(ShaderProfile, "vs_");
			break;
		case ZE_ST_PIXEL:
			sprintf(ShaderProfile, "ps_");
			break;

		default:
			zeError("Unsupported shader type");
			return NULL;
			break;
	}
	switch (Options.Model)
	{
		case ZE_SM_3_0:
			sprintf(ShaderProfile, "3_0");
			break;
		default:
			zeError("Unsupported shader model");
			return NULL;
			break;
	}

	ZESize ComponentCount = Options.Parameters.GetCount();
	ZEArray<D3DXMACRO> Macros;
	Macros.SetCount(ComponentCount + 1);
	for (ZESize I = 0; I < ComponentCount; I++)
	{
		Macros[ComponentCount].Name = Options.Parameters[I].Name.ToCString();
		Macros[ComponentCount].Definition = Options.Parameters[I].Definition.ToCString();
	}
	Macros[ComponentCount].Name = NULL;
	Macros[ComponentCount].Definition = NULL;

	
	LPD3DXBUFFER ShaderBuffer = NULL;
	LPD3DXBUFFER CompilerOutput = NULL;
	LPD3DXCONSTANTTABLE ConstantTable = NULL;
	DWORD Flags = D3DXSHADER_PACKMATRIX_COLUMNMAJOR | ZE_SHADER_COMPILER_PARAMETERS;

	if (FAILED(D3DXCompileShader(Options.SourceData.ToCString(), Options.SourceData.GetLength(), Macros.GetCArray(), &D3DXInclude, Options.EntryPoint.ToCString(), ShaderProfile, Flags, &ShaderBuffer, &CompilerOutput, &ConstantTable)))
	{
		zeError("Can not compile shader.\r\nShader name : \"%s\".\r\nCompile output :\r\n%s\r\n", Options.ShaderName.ToCString(), CompilerOutput->GetBufferPointer());
		ZED3D_RELEASE(CompilerOutput);
		return NULL;
	}

	ZEShader* Shader = NULL;
	if (Options.Type == ZE_ST_VERTEX)
	{
		Shader = CreateVertexShader(ShaderBuffer);
	}
	else if (Options.Type == ZE_ST_PIXEL)
	{
		Shader = CreatePixelShader(ShaderBuffer);
	}

	CreateConstantTable(Shader, ConstantTable);

	ZED3D_RELEASE(CompilerOutput);
	ZED3D_RELEASE(ConstantTable);
	ZED3D_RELEASE(ShaderBuffer);
	return Shader;
}

ZED3D9ShaderCompiler::ZED3D9ShaderCompiler()
{

}

ZED3D9ShaderCompiler::~ZED3D9ShaderCompiler()
{

}
