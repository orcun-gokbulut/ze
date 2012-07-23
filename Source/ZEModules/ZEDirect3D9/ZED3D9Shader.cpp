//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9Shader.cpp
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

#include <sstream>

#include "ZED3D9Shader.h"
#include "ZED3D9ShaderManager.h"
#include "ZED3D9CommonTools.h"

// Utility 
#ifdef ZE_DEBUG_D3D9_DEBUG_SHADERS
#define ZE_SHADER_COMPILER_PARAMETERS	(D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION)
#else
#define ZE_SHADER_COMPILER_PARAMETERS	D3DXSHADER_OPTIMIZATION_LEVEL3
#endif

// D3DX Include Interface
class ZED3DXInclude2 : public ID3DXInclude
{
public:
	HRESULT __stdcall Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID * ppData, UINT * pBytes)
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

	HRESULT __stdcall Close(LPCVOID pData)
	{
		if (pData != NULL)
			delete[] pData;
		return S_OK;
	}
} D3DIncludeInterface2;

// DX Type Conversion
ZEShaderConstantDataType ConvertD3DXType(int Column, int Row, D3DXPARAMETER_TYPE Type)
{
	if(Type == D3DXPT_FLOAT)
	{
		if(Row == 1 && Column == 1)
			return ZE_SCDT_FLOAT;
		else if(Row == 3 && Column == 3)
			return ZE_SCDT_MATRIX3x3;
		else if(Row == 4 && Column == 3)
			return ZE_SCDT_MATRIX4x4;
		else if(Row == 1 && Column == 2)
			return ZE_SCDT_VECTOR2;
		else if(Row == 1 && Column == 3)
			return ZE_SCDT_VECTOR3;
		else if(Row == 1 && Column == 4)
			return ZE_SCDT_VECTOR4;
		else
		{
			zeWarning("Undefined ShaderConstant Type in 'ZED3D9Shader'");
			return ZE_SCDT_UNKNOWN;
		}
	}
	else if(Type == D3DXPT_BOOL)
	{
		if(Row == 1 && Column == 1)
			return ZE_SCDT_BOOLEAN;
		else
		{
			zeWarning("Undefined ShaderConstant Type in 'ZED3D9Shader'");
			return ZE_SCDT_UNKNOWN;
		}
	}
	else if(Type == D3DXPT_SAMPLER)
		return ZE_SCDT_SAMPLER;
	else if(Type == D3DXPT_SAMPLER1D)
		return ZE_SCDT_SAMPLER1D;
	else if(Type == D3DXPT_SAMPLER2D)
		return ZE_SCDT_SAMPLER2D;
	else if(Type == D3DXPT_SAMPLER3D)
		return ZE_SCDT_SAMPLER3D;
	else if(Type == D3DXPT_SAMPLERCUBE)
		return ZE_SCDT_SAMPLERCUBE;
	else
	{
		zeWarning("Undefined ShaderConstant Type in 'ZED3D9Shader'");
		return ZE_SCDT_UNKNOWN;
	}
}

// ZED3D9Shader
////////////////////////////////////////////////////////////////////////////////////////
ZED3D9Shader::ZED3D9Shader()
{
	ReferanceCount = 1;
	Compiled = false;
}

ZED3D9Shader::~ZED3D9Shader()
{

}

void ZED3D9Shader::PopulateConstantTable(LPD3DXCONSTANTTABLE Table)
{
	if(Table != NULL)
	{
		D3DXCONSTANTTABLE_DESC TableDescriptor;
		UINT NoOfConstants;
	
		Table->GetDesc(&TableDescriptor);
	
		// For Each Number of Constants
		NoOfConstants = TableDescriptor.Constants;
		for (UINT i = 0; i < NoOfConstants; i++)
		{
			ZEShaderConstant CurrentConstantCPU;
			D3DXHANDLE CurrentConstant;
			D3DXCONSTANT_DESC CurrentConstantDescriptor;
			UINT ConstantSize = 1;
			
			CurrentConstant = Table->GetConstant(NULL, i);
			Table->GetConstantDesc(CurrentConstant, &CurrentConstantDescriptor, &ConstantSize);
		
			if(CurrentConstantDescriptor.StructMembers > 0)
			{
				// This Constant is Struct
				
				// CheckIf Struct Constant Is Array
				if(CurrentConstantDescriptor.Elements > 1)
				{
					// It is an Array
					for(ZEUInt32 j = 0; j < CurrentConstantDescriptor.Elements; j++)
					{
						D3DXHANDLE ArrayHandle = Table->GetConstantElement(CurrentConstant, j);
						D3DXCONSTANT_DESC ArrayHandleDescriptor;
						UINT ArrayHandleSize = 1;
						Table->GetConstantDesc(ArrayHandle, &ArrayHandleDescriptor, &ArrayHandleSize);

						ZEShaderConstant SubConstantCPU;
						D3DXHANDLE SubConstant;
						D3DXCONSTANT_DESC SubConstantDescriptor;
						UINT SubConstantSize = 1;

						// Get Each Data Members
						for (UINT k = 0; k < ArrayHandleDescriptor.StructMembers; k++)
						{
							SubConstant = Table->GetConstant(ArrayHandle, k);
							Table->GetConstantDesc(SubConstant, &SubConstantDescriptor, &SubConstantSize);

							// Create Subscript Syntax
							std::ostringstream oss;
							oss << "[" << j << "].";

							SubConstantCPU.ConstantName = ArrayHandleDescriptor.Name;
							SubConstantCPU.ConstantName.Append(oss.str().c_str());
							SubConstantCPU.ConstantName.Append(SubConstantDescriptor.Name);
							SubConstantCPU.Count = SubConstantDescriptor.Elements;
							SubConstantCPU.Type = ConvertD3DXType(SubConstantDescriptor.Columns,
																	SubConstantDescriptor.Rows,
																	SubConstantDescriptor.Type);
							SubConstantCPU.RegisterNo = SubConstantDescriptor.RegisterIndex;
							SubConstantCPU.Semantic = "";		//PLACEHOLDER

							// Add This to Array
							ShaderConstants.Add(SubConstantCPU);

							// Now Add Each Element with subscript operator
							if(SubConstantDescriptor.Elements > 1)
							{
								// It is Array
								for(ZEUInt32 t = 0; t < SubConstantDescriptor.Elements; t++)
								{			
									ZEShaderConstant BaseArrayConstantCPU;
									D3DXCONSTANT_DESC BaseArrayDescriptor;
									ZEUInt32 BaseArrayCount = 1;

									Table->GetConstantDesc(Table->GetConstantElement(SubConstant, t), &BaseArrayDescriptor, &BaseArrayCount);

									// Create Subscript Syntax
									std::ostringstream outStringStream;
									outStringStream << ".[" << t << "]";

									BaseArrayConstantCPU.ConstantName = ArrayHandleDescriptor.Name;
									BaseArrayConstantCPU.ConstantName.Append(oss.str().c_str());
									BaseArrayConstantCPU.ConstantName.Append(BaseArrayDescriptor.Name);
									BaseArrayConstantCPU.ConstantName.Append(outStringStream.str().c_str());
									BaseArrayConstantCPU.Count = BaseArrayDescriptor.Elements;
									BaseArrayConstantCPU.Type = ConvertD3DXType(BaseArrayDescriptor.Columns,
																				BaseArrayDescriptor.Rows,
																				BaseArrayDescriptor.Type);
									BaseArrayConstantCPU.RegisterNo = BaseArrayDescriptor.RegisterIndex;
									BaseArrayConstantCPU.Semantic = "";		// PLACEHOLDER

									// Add This to Array
									ShaderConstants.Add(BaseArrayConstantCPU);
								}
							}
						}
					}
				}
				else
				{
					// It is not An Array
					ZEShaderConstant SubConstantCPU;
					D3DXHANDLE SubConstant;
					D3DXCONSTANT_DESC SubConstantDescriptor;
					UINT SubConstantSize = 1;

					// Get Each Data Members
					for (UINT k = 0; k < CurrentConstantDescriptor.StructMembers; k++)
					{
						SubConstant = Table->GetConstant(CurrentConstant, k);
						Table->GetConstantDesc(SubConstant, &SubConstantDescriptor, &SubConstantSize);

						SubConstantCPU.ConstantName = CurrentConstantDescriptor.Name;
						SubConstantCPU.ConstantName.Append(".");
						SubConstantCPU.ConstantName.Append(SubConstantDescriptor.Name);
						SubConstantCPU.Count = SubConstantDescriptor.Elements;
						SubConstantCPU.Type = ConvertD3DXType(SubConstantDescriptor.Columns,
							SubConstantDescriptor.Rows,
							SubConstantDescriptor.Type);
						SubConstantCPU.RegisterNo = SubConstantDescriptor.RegisterIndex;
						SubConstantCPU.Semantic = "";		// PLACEHOLDER

						// Add This to Array
						ShaderConstants.Add(SubConstantCPU);

						// Now Add Each Element with subscript operator
						if(SubConstantDescriptor.Elements > 1)
						{
							// It is Array
							for(ZEUInt32 t = 0; t < SubConstantDescriptor.Elements; t++)
							{			
								ZEShaderConstant BaseArrayConstantCPU;
								D3DXCONSTANT_DESC BaseArrayDescriptor;
								ZEUInt32 BaseArrayCount = 1;

								Table->GetConstantDesc(Table->GetConstantElement(SubConstant, t), &BaseArrayDescriptor, &BaseArrayCount);

								// Create Subscript Syntax
								std::ostringstream outStringStream;
								outStringStream << ".[" << t << "]";

								BaseArrayConstantCPU.ConstantName = CurrentConstantDescriptor.Name;
								BaseArrayConstantCPU.ConstantName.Append(".");
								BaseArrayConstantCPU.ConstantName.Append(BaseArrayDescriptor.Name);
								BaseArrayConstantCPU.ConstantName.Append(outStringStream.str().c_str());
								BaseArrayConstantCPU.Count = BaseArrayDescriptor.Elements;
								BaseArrayConstantCPU.Type = ConvertD3DXType(BaseArrayDescriptor.Columns,
									BaseArrayDescriptor.Rows,
									BaseArrayDescriptor.Type);
								BaseArrayConstantCPU.RegisterNo = BaseArrayDescriptor.RegisterIndex;
								BaseArrayConstantCPU.Semantic = "";		// PLACEHOLDER

								// Add This to Array
								ShaderConstants.Add(BaseArrayConstantCPU);
							}
						}
					}
				}
			}
			else
			{
				// This Constant is Not Struct
				CurrentConstantCPU.ConstantName = CurrentConstantDescriptor.Name;
				CurrentConstantCPU.Count = CurrentConstantDescriptor.Elements;
				CurrentConstantCPU.Type = ConvertD3DXType(CurrentConstantDescriptor.Columns,
															CurrentConstantDescriptor.Rows,
															CurrentConstantDescriptor.Type);
				CurrentConstantCPU.RegisterNo = CurrentConstantDescriptor.RegisterIndex;
				CurrentConstantCPU.Semantic = "";			// PLACEHOLDER

				// Add This to Array
				ShaderConstants.Add(CurrentConstantCPU);

				// Now Add Each Element with subscript operator
				if(CurrentConstantDescriptor.Elements > 1)
				{
					// It is Array
					for(ZEUInt32 t = 0; t < CurrentConstantDescriptor.Elements; t++)
					{			
						ZEShaderConstant BaseArrayConstantCPU;
						D3DXCONSTANT_DESC BaseArrayDescriptor;
						ZEUInt32 BaseArrayCount = 1;

						Table->GetConstantDesc(Table->GetConstantElement(CurrentConstant, t), &BaseArrayDescriptor, &BaseArrayCount);

						// Create Subscript Syntax
						std::ostringstream outStringStream;
						outStringStream << ".[" << t << "]";

						BaseArrayConstantCPU.ConstantName.Append(BaseArrayDescriptor.Name);
						BaseArrayConstantCPU.ConstantName.Append(outStringStream.str().c_str());
						BaseArrayConstantCPU.Count = BaseArrayDescriptor.Elements;
						BaseArrayConstantCPU.Type = ConvertD3DXType(BaseArrayDescriptor.Columns,
																	BaseArrayDescriptor.Rows,
																	BaseArrayDescriptor.Type);
						BaseArrayConstantCPU.RegisterNo = BaseArrayDescriptor.RegisterIndex;
						BaseArrayConstantCPU.Semantic = "";		// PLACEHOLDER

						// Add This to Array
						ShaderConstants.Add(BaseArrayConstantCPU);
					}
				}
			}
		}
	}
}

const char* ZED3D9Shader::GetFileName()
{
	return FileName;
}

const char* ZED3D9Shader::GetFunctionName()
{
	return FunctionName;
}

ZEUInt32 ZED3D9Shader::GetComponents()
{
	return Components;
}

void ZED3D9Shader::Release()
{
	/*
	ZED3D9ShaderManager::GetInstance()->ReleaseShader(this);
	*/
}

ZED3D9Shader* ZED3D9Shader::CreateShader(const char* FileName, const char* FunctionName, ZEUInt32 Components, ZED3D9ShaderType Type, const char* Profile)
{
	return ZED3D9ShaderManager::GetInstance()->GetShader(FileName, FunctionName, Components, Type, Profile);
}


// ZED3D9PixelShader
////////////////////////////////////////////////////////////////////////////////////////
ZED3D9PixelShader::ZED3D9PixelShader()
{
	PixelShader = NULL;
}

ZED3D9PixelShader::~ZED3D9PixelShader()
{
	ZED3D_RELEASE(PixelShader);
}

ZED3D9ShaderType ZED3D9PixelShader::GetShaderType()
{
	return ZE_D3D9_ST_PIXEL;
}

LPDIRECT3DPIXELSHADER9 ZED3D9PixelShader::GetPixelShader()
{
	return PixelShader;
}

void ZED3D9PixelShader::SetConstant(const ZEString& Name, const ZEVector2& Value)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		GetDevice()->SetPixelShaderConstantF(ShaderConstants[Index].RegisterNo, ZEVector4(Value.x, Value.y, 0.0f, 0.0f).M, 1);
}

void ZED3D9PixelShader::SetConstant(const ZEString& Name, const ZEVector3& Value)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		GetDevice()->SetPixelShaderConstantF(ShaderConstants[Index].RegisterNo, ZEVector4(Value.x, Value.y, Value.z, 0.0f).M, 1);
}

void ZED3D9PixelShader::SetConstant(const ZEString& Name, const ZEVector4& Value)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		GetDevice()->SetPixelShaderConstantF(ShaderConstants[Index].RegisterNo, Value.M, 1);
}

void ZED3D9PixelShader::SetConstant(const ZEString& Name, const ZEMatrix3x3& Value)
{
	ZEUInt32 Index;
	ZEMatrix4x4 ConstMatrix(Value.M11, Value.M12, Value.M13, 0.0f,
							Value.M21, Value.M22, Value.M23, 0.0f,
							Value.M31, Value.M32, Value.M33, 0.0f,
							0.0f, 0.0f, 0.0f, 0.0f);

	if(FindConstantLocation(Index, Name))
		GetDevice()->SetPixelShaderConstantF(ShaderConstants[Index].RegisterNo, ConstMatrix.MA, 3);

}

void ZED3D9PixelShader::SetConstant(const ZEString& Name, const ZEMatrix4x4& Value)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		GetDevice()->SetPixelShaderConstantF(ShaderConstants[Index].RegisterNo, Value.MA, 3);
}

void ZED3D9PixelShader::SetConstant(const ZEString& Name, bool Value)
{
	ZEUInt32 Index;

	static struct
	{
		BOOL A, B, C, D;
	}
	BoolRegister = {Value, 0, 0, 0};

	if(FindConstantLocation(Index, Name))
		GetDevice()->SetPixelShaderConstantB(ShaderConstants[Index].RegisterNo, (BOOL*)&BoolRegister, 1);
}

void ZED3D9PixelShader::SetConstant(const ZEString& Name, float Value)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		GetDevice()->SetPixelShaderConstantF(ShaderConstants[Index].RegisterNo, ZEVector4(Value, 0.0f, 0.0f, 0.0f).M, 1);
}

void ZED3D9PixelShader::SetConstantArray(const ZEString& Name, const ZEVector2* ValueArray, ZESize Count)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		for(ZEUInt32 i = 0; i < Count; i++)
		{	
			// Array Registers Guaranteed to be adjacent?
			ZEVector4 RegValue(ValueArray[i].x, ValueArray[i].y, 0.0f, 0.0f);
			GetDevice()->SetPixelShaderConstantF(ShaderConstants[Index + i].RegisterNo, RegValue.M, 1);
		}
	
}

void ZED3D9PixelShader::SetConstantArray(const ZEString& Name, const ZEVector3* ValueArray, ZESize Count)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		for(ZEUInt32 i = 0; i < Count; i++)
		{	
			// Array Registers Guaranteed to be adjacent?
			ZEVector4 RegValue(ValueArray[i].x, ValueArray[i].y, ValueArray[i].z, 0.0f);
			GetDevice()->SetPixelShaderConstantF(ShaderConstants[Index + i].RegisterNo, RegValue.M, 1);
		}
}

void ZED3D9PixelShader::SetConstantArray(const ZEString& Name, const ZEVector4* ValueArray, ZESize Count)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		GetDevice()->SetPixelShaderConstantF(ShaderConstants[Index].RegisterNo, (float*)ValueArray, Count);
}

void ZED3D9PixelShader::SetConstantArray(const ZEString& Name, const ZEMatrix3x3* ValueArray, ZESize Count)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		for(ZEUInt32 i = 0; i < Count; i++)
		{	
			ZEMatrix4x4 ConstMatrix(ValueArray[i].M11, ValueArray[i].M12, ValueArray[i].M13, 0.0f,
									ValueArray[i].M21, ValueArray[i].M22, ValueArray[i].M23, 0.0f,
									ValueArray[i].M31, ValueArray[i].M32, ValueArray[i].M33, 0.0f,
									0.0f, 0.0f, 0.0f, 0.0f);

			// Array Registers Guaranteed to be adjacent?
			GetDevice()->SetPixelShaderConstantF(ShaderConstants[Index + i * 3].RegisterNo, ConstMatrix.MA, 3);
		}
}

void ZED3D9PixelShader::SetConstantArray(const ZEString& Name, const ZEMatrix4x4* ValueArray, ZESize Count)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		GetDevice()->SetPixelShaderConstantF(ShaderConstants[Index].RegisterNo, (float*)ValueArray, Count * 4);
}

void ZED3D9PixelShader::SetConstantArray(const ZEString& Name, const bool* ValueArray, ZESize Count)
{
	ZEUInt32 Index;
	static struct
	{
		BOOL A, B, C, D;
	}R = {0, 0, 0, 0};

	if(FindConstantLocation(Index, Name))
		for(ZEUInt32 i = 0; i < Count; i++)
		{
			R.A = ValueArray[i];	// Rest Should Stay Zero
			GetDevice()->SetPixelShaderConstantB(ShaderConstants[Index + i].RegisterNo, (BOOL*)&R, 1);
		}
}

void ZED3D9PixelShader::SetConstantArray(const ZEString& Name, const float* ValueArray, ZESize Count)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		for(ZEUInt32 i = 0; i < Count; i++)
		{
			GetDevice()->SetPixelShaderConstantF(ShaderConstants[Index + i].RegisterNo, ZEVector4(ValueArray[i], 0.0f, 0.0f, 0.0f).M, 1);
		}
}

void ZED3D9PixelShader::SetConstant(int Register, const ZEVector2& Value)
{
	GetDevice()->SetPixelShaderConstantF(Register, ZEVector4(Value.x, Value.y, 0.0f, 0.0f).M, 1);
}

void ZED3D9PixelShader::SetConstant(int Register, const ZEVector3& Value)
{
	GetDevice()->SetPixelShaderConstantF(Register, ZEVector4(Value.x, Value.y, Value.z, 0.0f).M, 1);
}

void ZED3D9PixelShader::SetConstant(int Register, const ZEVector4& Value)
{
	GetDevice()->SetPixelShaderConstantF(Register, Value.M, 1);
}

void ZED3D9PixelShader::SetConstant(int Register, const ZEMatrix3x3& Value)
{
	ZEMatrix4x4 ConstMatrix(Value.M11, Value.M12, Value.M13, 0.0f,
							Value.M21, Value.M22, Value.M23, 0.0f,
							Value.M31, Value.M32, Value.M33, 0.0f,
							0.0f, 0.0f, 0.0f, 0.0f);

	GetDevice()->SetPixelShaderConstantF(Register, ConstMatrix.MA, 3);
}

void ZED3D9PixelShader::SetConstant(int Register, const ZEMatrix4x4& Value)
{
	GetDevice()->SetPixelShaderConstantF(Register, Value.MA, 4);
}

void ZED3D9PixelShader::SetConstant(int Register, bool Value)
{
	static struct
	{
		BOOL A, B, C, D;
	}BoolReg = {Value, 0, 0, 0};
	GetDevice()->SetPixelShaderConstantB(Register, (BOOL*)&BoolReg, 1);
}

void ZED3D9PixelShader::SetConstant(int Register, float Value)
{
	GetDevice()->SetPixelShaderConstantF(Register, ZEVector4(Value, 0.0f, 0.0f, 0.0f).M, 1);
}

void ZED3D9PixelShader::SetConstantArray(int Register, const ZEVector2* ValueArray, ZESize Count)
{
	for(ZEUInt32 i = 0; i < Count; i++)
		GetDevice()->SetPixelShaderConstantF(Register + i, ZEVector4(ValueArray[i].x, ValueArray[i].y, 0.0f, 0.0f).M, 1);

}

void ZED3D9PixelShader::SetConstantArray(int Register, const ZEVector3* ValueArray, ZESize Count)
{
	for(ZEUInt32 i = 0; i < Count; i++)
		GetDevice()->SetPixelShaderConstantF(Register + i, ZEVector4(ValueArray[i].x, ValueArray[i].y, ValueArray[i].z, 0.0f).M, 1);
}

void ZED3D9PixelShader::SetConstantArray(int Register, const ZEVector4* ValueArray, ZESize Count)
{
	GetDevice()->SetPixelShaderConstantF(Register, (float*)ValueArray, Count);
}

void ZED3D9PixelShader::SetConstantArray(int Register, const ZEMatrix3x3* ValueArray, ZESize Count)
{
	for(ZEUInt32 i = 0; i < Count; i++)
	{
		ZEMatrix4x4 ConstMatrix(ValueArray[i].M11, ValueArray[i].M12, ValueArray[i].M13, 0.0f,
			ValueArray[i].M21, ValueArray[i].M22, ValueArray[i].M23, 0.0f,
			ValueArray[i].M31, ValueArray[i].M32, ValueArray[i].M33, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f);

		GetDevice()->SetPixelShaderConstantF(Register + 3 * i, ConstMatrix.MA, 3);
	}
}

void ZED3D9PixelShader::SetConstantArray(int Register, const ZEMatrix4x4* ValueArray, ZESize Count)
{
	GetDevice()->SetPixelShaderConstantF(Register, (float*)ValueArray, Count * 4);
}

void ZED3D9PixelShader::SetConstantArray(int Register, const bool* ValueArray, ZESize Count)
{
	static struct
	{
		BOOL A, B, C, D;
	}BoolReg = {0, 0, 0, 0};

	for(ZEUInt32 i = 0; i < Count; i++)
	{
		BoolReg.A = (int)ValueArray[i];
		GetDevice()->SetPixelShaderConstantB(Register + i, (BOOL*)&BoolReg, 1);
	}
}

void ZED3D9PixelShader::SetConstantArray(int Register, const float* ValueArray, ZESize Count)
{
	for(ZEUInt32 i = 0; i < Count; i++)
		GetDevice()->SetPixelShaderConstantF(Register + i, ZEVector4(ValueArray[i], 0.0f, 0.0f, 0.0f).M, 1);
}

ZEVector2 ZED3D9PixelShader::GetConstantVector2(const char* Name) const
{
	ZEUInt32 Index;
	ZEVector4 Vector;
	if(FindConstantLocation(Index, Name))
		GetDevice()->GetPixelShaderConstantF(ShaderConstants[Index].RegisterNo, Vector.M, 1);
	return ZEVector2(Vector.x, Vector.y);
}

ZEVector3 ZED3D9PixelShader::GetConstantVector3(const char* Name) const
{
	ZEUInt32 Index;
	ZEVector4 Vector;
	if(FindConstantLocation(Index, Name))
		GetDevice()->GetPixelShaderConstantF(ShaderConstants[Index].RegisterNo, Vector.M, 1);
	return ZEVector3(Vector.x, Vector.y, Vector.z);
}

ZEVector4 ZED3D9PixelShader::GetConstantVector4(const char* Name) const
{
	ZEUInt32 Index;
	ZEVector4 Vector;
	if(FindConstantLocation(Index, Name))
		GetDevice()->GetPixelShaderConstantF(ShaderConstants[Index].RegisterNo, Vector.M, 1);
	return Vector;
}

ZEMatrix3x3 ZED3D9PixelShader::GetConstantMatrix3x3(const char* Name) const
{
	ZEUInt32 Index;
	ZEMatrix4x4 ReturnMatrix;
	if(FindConstantLocation(Index, Name))
		GetDevice()->GetPixelShaderConstantF(ShaderConstants[Index].RegisterNo, ReturnMatrix.MA, 3);
	return ZEMatrix3x3 (ReturnMatrix.M11, ReturnMatrix.M12, ReturnMatrix.M13,
						ReturnMatrix.M21, ReturnMatrix.M22, ReturnMatrix.M23,
						ReturnMatrix.M31, ReturnMatrix.M32, ReturnMatrix.M33);		
}

ZEMatrix4x4 ZED3D9PixelShader::GetConstantMatrix4x4(const char* Name) const
{
	ZEUInt32 Index;
	ZEMatrix4x4 ReturnMatrix;
	if(FindConstantLocation(Index, Name))
		GetDevice()->GetPixelShaderConstantF(ShaderConstants[Index].RegisterNo, ReturnMatrix.MA, 4);
	return ReturnMatrix;
}

bool ZED3D9PixelShader::GetConstantBool(const char* Name) const
{
	ZEUInt32 Index;
	BOOL ReturnBool[4];
	if(FindConstantLocation(Index, Name))
		GetDevice()->GetPixelShaderConstantB(ShaderConstants[Index].RegisterNo, ReturnBool, 1);
	return (bool)ReturnBool[0];
}

float ZED3D9PixelShader::GetConstantFloat(const char* Name) const
{
	ZEUInt32 Index;
	ZEVector4 ReturnFloat;
	if(FindConstantLocation(Index, Name))
		GetDevice()->GetPixelShaderConstantF(ShaderConstants[Index].RegisterNo, ReturnFloat.M, 1);
	return ReturnFloat.x;
}

bool ZED3D9PixelShader::CompileShader(const ZEString CompilerParameter[][2],
										int CompilerParameterCount,
										ZEString ShaderProfile, 
										ZEString Source,
										ZEString MainFunction)
{
	// Temp Buffer For Constant Table and Compiler Output
	LPD3DXBUFFER ShaderBuffer = NULL;
	LPD3DXBUFFER CompilerOutput = NULL;
	LPD3DXCONSTANTTABLE ConstantTable = NULL;
	LPD3DXMACRO MacroDefinitions = new D3DXMACRO[CompilerParameterCount + 1];

	int i;
	ShaderCompilerParameters.SetCount(CompilerParameterCount);
	for(i = 0; i < CompilerParameterCount; i++)
	{
		MacroDefinitions[i].Name = CompilerParameter[i][0];
		MacroDefinitions[i].Definition = CompilerParameter[i][1];

		ZEShaderCompilerParameter Parameter;
		Parameter.Name = CompilerParameter[i][0];
		Parameter.Definition = CompilerParameter[i][1];
		ShaderCompilerParameters.Insert(i, Parameter);
	}
	MacroDefinitions[i].Name = NULL;
	MacroDefinitions[i].Definition = NULL;

	if(D3DXCompileShader(Source.ToCString(), Source.GetSize(), MacroDefinitions, &D3DIncludeInterface2, MainFunction.ToCString(), ShaderProfile, D3DXSHADER_PACKMATRIX_COLUMNMAJOR | ZE_SHADER_COMPILER_PARAMETERS, &ShaderBuffer, &CompilerOutput, &ConstantTable) != D3D_OK)
	{
		if (CompilerOutput == NULL)
			zeError("Can not compile pixel shader. Shader file name : \"%s\"", FileName);
		else
			zeError("Can not compile pixel shader.\r\nShader file name : \"%s\".\r\nCompile output :\r\n%s\r\n", FileName, CompilerOutput->GetBufferPointer());

		Compiled = false;
		return false;
	}

	if (CompilerOutput != NULL)
		CompilerOutput->Release();

	// Populate Constants Array and Register Array
	PopulateConstantTable(ConstantTable);

	// Compile Done! Set FunctionName
//	FunctionName = MainFunction;

	// Create Shader Handle
	if (GetDevice()->CreatePixelShader((DWORD*)ShaderBuffer->GetBufferPointer(), &PixelShader) != NULL)
	{
		zeError("Can not create vertex shader.\r\n");
		PixelShader = NULL;
		return false;
	}

	if (ShaderBuffer != NULL)
		ShaderBuffer->Release();

	if(MacroDefinitions != NULL)
	{	
		delete [] MacroDefinitions;
		MacroDefinitions = NULL;
	}

	Compiled = true;
	return true;
}

ZED3D9PixelShader* ZED3D9PixelShader::CreateShader(const char* FileName, const char* FunctionName, ZEUInt32 Components, const char* Profile)
{
	return ZED3D9ShaderManager::GetInstance()->GetPixelShader(FileName, FunctionName, Components, Profile);
}

// ZED3D9VertexShader
////////////////////////////////////////////////////////////////////////////////////////
ZED3D9VertexShader::ZED3D9VertexShader()
{
	VertexShader = NULL;
}

ZED3D9VertexShader::~ZED3D9VertexShader()
{
	ZED3D_RELEASE(VertexShader);
}

ZED3D9ShaderType ZED3D9VertexShader::GetShaderType()
{
	return ZE_D3D9_ST_VERTEX;
}

LPDIRECT3DVERTEXSHADER9 ZED3D9VertexShader::GetVertexShader()
{
	return VertexShader;
}

void ZED3D9VertexShader::SetConstant(const ZEString& Name, const ZEVector2& Value)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		GetDevice()->SetVertexShaderConstantF(ShaderConstants[Index].RegisterNo, ZEVector4(Value.x, Value.y, 0.0f, 0.0f).M, 1);
}

void ZED3D9VertexShader::SetConstant(const ZEString& Name, const ZEVector3& Value)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		GetDevice()->SetVertexShaderConstantF(ShaderConstants[Index].RegisterNo, ZEVector4(Value.x, Value.y, Value.z, 0.0f).M, 1);
}

void ZED3D9VertexShader::SetConstant(const ZEString& Name, const ZEVector4& Value)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		GetDevice()->SetVertexShaderConstantF(ShaderConstants[Index].RegisterNo, Value.M, 1);
}

void ZED3D9VertexShader::SetConstant(const ZEString& Name, const ZEMatrix3x3& Value)
{
	ZEUInt32 Index;
	ZEMatrix4x4 ConstMatrix(Value.M11, Value.M12, Value.M13, 0.0f,
		Value.M21, Value.M22, Value.M23, 0.0f,
		Value.M31, Value.M32, Value.M33, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f);

	if(FindConstantLocation(Index, Name))
		GetDevice()->SetVertexShaderConstantF(ShaderConstants[Index].RegisterNo, ConstMatrix.MA, 3);

}

void ZED3D9VertexShader::SetConstant(const ZEString& Name, const ZEMatrix4x4& Value)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		GetDevice()->SetVertexShaderConstantF(ShaderConstants[Index].RegisterNo, Value.MA, 3);
}

void ZED3D9VertexShader::SetConstant(const ZEString& Name, bool Value)
{
	ZEUInt32 Index;

	static struct
	{
		BOOL A, B, C, D;
	}
	BoolRegister = {Value, 0, 0, 0};

	if(FindConstantLocation(Index, Name))
		GetDevice()->SetVertexShaderConstantB(ShaderConstants[Index].RegisterNo, (BOOL*)&BoolRegister, 1);
}

void ZED3D9VertexShader::SetConstant(const ZEString& Name, float Value)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		GetDevice()->SetVertexShaderConstantF(ShaderConstants[Index].RegisterNo, ZEVector4(Value, 0.0f, 0.0f, 0.0f).M, 1);
}

void ZED3D9VertexShader::SetConstantArray(const ZEString& Name, const ZEVector2* ValueArray, ZESize Count)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		for(ZEUInt32 i = 0; i < Count; i++)
		{	
			// Array Registers Guaranteed to be adjacent?
			ZEVector4 RegValue(ValueArray[i].x, ValueArray[i].y, 0.0f, 0.0f);
			GetDevice()->SetVertexShaderConstantF(ShaderConstants[Index + i].RegisterNo, RegValue.M, 1);
		}

}

void ZED3D9VertexShader::SetConstantArray(const ZEString& Name, const ZEVector3* ValueArray, ZESize Count)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		for(ZEUInt32 i = 0; i < Count; i++)
		{	
			// Array Registers Guaranteed to be adjacent?
			ZEVector4 RegValue(ValueArray[i].x, ValueArray[i].y, ValueArray[i].z, 0.0f);
			GetDevice()->SetVertexShaderConstantF(ShaderConstants[Index + i].RegisterNo, RegValue.M, 1);
		}
}

void ZED3D9VertexShader::SetConstantArray(const ZEString& Name, const ZEVector4* ValueArray, ZESize Count)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		GetDevice()->SetVertexShaderConstantF(ShaderConstants[Index].RegisterNo, (float*)ValueArray, Count);
}

void ZED3D9VertexShader::SetConstantArray(const ZEString& Name, const ZEMatrix3x3* ValueArray, ZESize Count)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		for(ZEUInt32 i = 0; i < Count; i++)
		{	
			ZEMatrix4x4 ConstMatrix(ValueArray[i].M11, ValueArray[i].M12, ValueArray[i].M13, 0.0f,
				ValueArray[i].M21, ValueArray[i].M22, ValueArray[i].M23, 0.0f,
				ValueArray[i].M31, ValueArray[i].M32, ValueArray[i].M33, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f);

			// Array Registers Guaranteed to be adjacent?
			GetDevice()->SetVertexShaderConstantF(ShaderConstants[Index + i * 3].RegisterNo, ConstMatrix.MA, 3);
		}
}

void ZED3D9VertexShader::SetConstantArray(const ZEString& Name, const ZEMatrix4x4* ValueArray, ZESize Count)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		GetDevice()->SetVertexShaderConstantF(ShaderConstants[Index].RegisterNo, (float*)ValueArray, Count * 4);
}

void ZED3D9VertexShader::SetConstantArray(const ZEString& Name, const bool* ValueArray, ZESize Count)
{
	ZEUInt32 Index;
	static struct
	{
		BOOL A, B, C, D;
	}R = {0, 0, 0, 0};

	if(FindConstantLocation(Index, Name))
		for(ZEUInt32 i = 0; i < Count; i++)
		{
			R.A = ValueArray[i];	// Rest Should Stay Zero
			GetDevice()->SetVertexShaderConstantB(ShaderConstants[Index + i].RegisterNo, (BOOL*)&R, 1);
		}
}

void ZED3D9VertexShader::SetConstantArray(const ZEString& Name, const float* ValueArray, ZESize Count)
{
	ZEUInt32 Index;
	if(FindConstantLocation(Index, Name))
		for(ZEUInt32 i = 0; i < Count; i++)
		{
			GetDevice()->SetVertexShaderConstantF(ShaderConstants[Index + i].RegisterNo, ZEVector4(ValueArray[i], 0.0f, 0.0f, 0.0f).M, 1);
		}
}

void ZED3D9VertexShader::SetConstant(int Register, const ZEVector2& Value)
{
	GetDevice()->SetVertexShaderConstantF(Register, ZEVector4(Value.x, Value.y, 0.0f, 0.0f).M, 1);
}

void ZED3D9VertexShader::SetConstant(int Register, const ZEVector3& Value)
{
	GetDevice()->SetVertexShaderConstantF(Register, ZEVector4(Value.x, Value.y, Value.z, 0.0f).M, 1);
}

void ZED3D9VertexShader::SetConstant(int Register, const ZEVector4& Value)
{
	GetDevice()->SetVertexShaderConstantF(Register, Value.M, 1);
}

void ZED3D9VertexShader::SetConstant(int Register, const ZEMatrix3x3& Value)
{
	ZEMatrix4x4 ConstMatrix(Value.M11, Value.M12, Value.M13, 0.0f,
		Value.M21, Value.M22, Value.M23, 0.0f,
		Value.M31, Value.M32, Value.M33, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f);

	GetDevice()->SetVertexShaderConstantF(Register, ConstMatrix.MA, 3);
}

void ZED3D9VertexShader::SetConstant(int Register, const ZEMatrix4x4& Value)
{
	GetDevice()->SetVertexShaderConstantF(Register, Value.MA, 4);
}

void ZED3D9VertexShader::SetConstant(int Register, bool Value)
{
	static struct
	{
		BOOL A, B, C, D;
	}BoolReg = {Value, 0, 0, 0};
	GetDevice()->SetVertexShaderConstantB(Register, (BOOL*)&BoolReg, 1);
}

void ZED3D9VertexShader::SetConstant(int Register, float Value)
{
	GetDevice()->SetVertexShaderConstantF(Register, ZEVector4(Value, 0.0f, 0.0f, 0.0f).M, 1);
}

void ZED3D9VertexShader::SetConstantArray(int Register, const ZEVector2* ValueArray, ZESize Count)
{
	for(ZEUInt32 i = 0; i < Count; i++)
		GetDevice()->SetVertexShaderConstantF(Register + i, ZEVector4(ValueArray[i].x, ValueArray[i].y, 0.0f, 0.0f).M, 1);

}

void ZED3D9VertexShader::SetConstantArray(int Register, const ZEVector3* ValueArray, ZESize Count)
{
	for(ZEUInt32 i = 0; i < Count; i++)
		GetDevice()->SetVertexShaderConstantF(Register + i, ZEVector4(ValueArray[i].x, ValueArray[i].y, ValueArray[i].z, 0.0f).M, 1);
}

void ZED3D9VertexShader::SetConstantArray(int Register, const ZEVector4* ValueArray, ZESize Count)
{
	GetDevice()->SetVertexShaderConstantF(Register, (float*)ValueArray, Count);
}

void ZED3D9VertexShader::SetConstantArray(int Register, const ZEMatrix3x3* ValueArray, ZESize Count)
{
	for(ZEUInt32 i = 0; i < Count; i++)
	{
		ZEMatrix4x4 ConstMatrix(ValueArray[i].M11, ValueArray[i].M12, ValueArray[i].M13, 0.0f,
			ValueArray[i].M21, ValueArray[i].M22, ValueArray[i].M23, 0.0f,
			ValueArray[i].M31, ValueArray[i].M32, ValueArray[i].M33, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f);

		GetDevice()->SetVertexShaderConstantF(Register + 3 * i, ConstMatrix.MA, 3);
	}
}

void ZED3D9VertexShader::SetConstantArray(int Register, const ZEMatrix4x4* ValueArray, ZESize Count)
{
	GetDevice()->SetVertexShaderConstantF(Register, (float*)ValueArray, Count * 4);
}

void ZED3D9VertexShader::SetConstantArray(int Register, const bool* ValueArray, ZESize Count)
{
	static struct
	{
		BOOL A, B, C, D;
	}BoolReg = {0, 0, 0, 0};

	for(ZEUInt32 i = 0; i < Count; i++)
	{
		BoolReg.A = (int)ValueArray[i];
		GetDevice()->SetVertexShaderConstantB(Register + i, (BOOL*)&BoolReg, 1);
	}
}

void ZED3D9VertexShader::SetConstantArray(int Register, const float* ValueArray, ZESize Count)
{
	for(ZEUInt32 i = 0; i < Count; i++)
		GetDevice()->SetVertexShaderConstantF(Register + i, ZEVector4(ValueArray[i], 0.0f, 0.0f, 0.0f).M, 1);
}

ZEVector2 ZED3D9VertexShader::GetConstantVector2(const char* Name) const
{
	ZEUInt32 Index;
	ZEVector4 Vector;
	if(FindConstantLocation(Index, Name))
		GetDevice()->GetVertexShaderConstantF(ShaderConstants[Index].RegisterNo, Vector.M, 1);
	return ZEVector2(Vector.x, Vector.y);
}

ZEVector3 ZED3D9VertexShader::GetConstantVector3(const char* Name) const
{
	ZEUInt32 Index;
	ZEVector4 Vector;
	if(FindConstantLocation(Index, Name))
		GetDevice()->GetVertexShaderConstantF(ShaderConstants[Index].RegisterNo, Vector.M, 1);
	return ZEVector3(Vector.x, Vector.y, Vector.z);
}

ZEVector4 ZED3D9VertexShader::GetConstantVector4(const char* Name) const
{
	ZEUInt32 Index;
	ZEVector4 Vector;
	if(FindConstantLocation(Index, Name))
		GetDevice()->GetVertexShaderConstantF(ShaderConstants[Index].RegisterNo, Vector.M, 1);
	return Vector;
}

ZEMatrix3x3 ZED3D9VertexShader::GetConstantMatrix3x3(const char* Name) const
{
	ZEUInt32 Index;
	ZEMatrix4x4 ReturnMatrix;
	if(FindConstantLocation(Index, Name))
		GetDevice()->GetVertexShaderConstantF(ShaderConstants[Index].RegisterNo, ReturnMatrix.MA, 3);
	return ZEMatrix3x3 (ReturnMatrix.M11, ReturnMatrix.M12, ReturnMatrix.M13,
		ReturnMatrix.M21, ReturnMatrix.M22, ReturnMatrix.M23,
		ReturnMatrix.M31, ReturnMatrix.M32, ReturnMatrix.M33);		
}

ZEMatrix4x4 ZED3D9VertexShader::GetConstantMatrix4x4(const char* Name) const
{
	ZEUInt32 Index;
	ZEMatrix4x4 ReturnMatrix;
	if(FindConstantLocation(Index, Name))
		GetDevice()->GetVertexShaderConstantF(ShaderConstants[Index].RegisterNo, ReturnMatrix.MA, 4);
	return ReturnMatrix;
}

bool ZED3D9VertexShader::GetConstantBool(const char* Name) const
{
	ZEUInt32 Index;
	BOOL ReturnBool[4];
	if(FindConstantLocation(Index, Name))
		GetDevice()->GetVertexShaderConstantB(ShaderConstants[Index].RegisterNo, ReturnBool, 1);
	return (bool)ReturnBool[0];
}

float ZED3D9VertexShader::GetConstantFloat(const char* Name) const
{
	ZEUInt32 Index;
	ZEVector4 ReturnFloat;
	if(FindConstantLocation(Index, Name))
		GetDevice()->GetVertexShaderConstantF(ShaderConstants[Index].RegisterNo, ReturnFloat.M, 1);
	return ReturnFloat.x;
}

bool ZED3D9VertexShader::CompileShader(const ZEString CompilerParameter[][2],
										int CompilerParameterCount,
										ZEString ShaderProfile, 
										ZEString Source,
										ZEString MainFunction)
{
	// Temp Buffer For Constant Table and Compiler Output
	LPD3DXBUFFER ShaderBuffer = NULL;
	LPD3DXBUFFER CompilerOutput = NULL;
	LPD3DXCONSTANTTABLE ConstantTable = NULL;
	LPD3DXMACRO MacroDefinitions = new D3DXMACRO[CompilerParameterCount + 1];

	int i;
	for(i = 0; i < CompilerParameterCount; i++)
	{
		MacroDefinitions[i].Name = CompilerParameter[i][0];
		MacroDefinitions[i].Definition = CompilerParameter[i][1];

		ZEShaderCompilerParameter Parameter;
		Parameter.Name = CompilerParameter[i][0];
		Parameter.Definition = CompilerParameter[i][1];
		ShaderCompilerParameters.Add(Parameter);
	}
	MacroDefinitions[i].Name = NULL;
	MacroDefinitions[i].Definition = NULL;

	if(D3DXCompileShader(Source.ToCString(), Source.GetSize(), MacroDefinitions, &D3DIncludeInterface2, MainFunction.ToCString(), ShaderProfile, D3DXSHADER_PACKMATRIX_COLUMNMAJOR | ZE_SHADER_COMPILER_PARAMETERS, &ShaderBuffer, &CompilerOutput, &ConstantTable) != D3D_OK)
	{
		if (CompilerOutput == NULL)
			zeError("Can not compile vertex shader.\r\n");
		else
			zeError("Can not compile vertex shader.\r\nCompile output :\r\n%s\r\n", CompilerOutput->GetBufferPointer());

		Compiled = false;
		return false;
	}

	if (CompilerOutput != NULL)
		CompilerOutput->Release();

	// Populate Constants Array and Register Array
	PopulateConstantTable(ConstantTable);

	// Compile Done! Set FunctionName
//	FunctionName = MainFunction;

	// Create Shader Handle
	if (GetDevice()->CreateVertexShader((DWORD*)ShaderBuffer->GetBufferPointer(), &VertexShader) != NULL)
	{
		zeError("Can not create vertex shader.\r\n");
		VertexShader = NULL;
		return false;
	}

	if(ShaderBuffer != NULL)
		ShaderBuffer->Release();

	if(MacroDefinitions != NULL)
	{	
		delete [] MacroDefinitions;
		MacroDefinitions = NULL;
	}

	Compiled = true;
	return true;
}

ZED3D9VertexShader* ZED3D9VertexShader::CreateShader(const char* FileName, const char* FunctionName, ZEUInt32 Components, const char* Profile)
{
	return ZED3D9ShaderManager::GetInstance()->GetVertexShader(FileName, FunctionName, Components, Profile);
}

