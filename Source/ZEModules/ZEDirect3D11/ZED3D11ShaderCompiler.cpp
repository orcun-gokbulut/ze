//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D11ShaderCompiler.cpp
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

#include <d3dcompiler.h>
#include <d3dcompiler.inl>

#include "ZEError.h"
#include "ZED3D11Shader.h"
#include "ZEFile/ZEFile.h"
#include "ZEDS/ZEPointer.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEDS/ZEHashGenerator.h"
#include "ZED3D11ShaderCompiler.h"
#include "ZED3D11GraphicsDevice.h"
#include "ZED3D11GraphicsModule.h"
#include "ZEGraphics/ZEGRShaderCompiler.h"

#define ZE_SHADER_COMPILER_DEFAULT_PARAMETERS	(D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR)

#ifdef ZE_DEBUG_D3D11_DEBUG_SHADERS
	#define ZE_SHADER_COMPILER_PARAMETERS		(ZE_SHADER_COMPILER_DEFAULT_PARAMETERS | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG)
#else
	#define ZE_SHADER_COMPILER_PARAMETERS		(ZE_SHADER_COMPILER_DEFAULT_PARAMETERS | D3DCOMPILE_OPTIMIZATION_LEVEL3)
#endif

#define ZE_D3D11_DEVICE_REGISTER_SIZE			16	// Bytes

class ZED3D11Include : public ID3DInclude
{
	HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID * ppData, UINT * pBytes)
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

} D3D10Include;


inline static ZEGRShaderConstantType GetZEShaderConstantDataType(D3D_SHADER_VARIABLE_TYPE Type)
{
	ZEGRShaderConstantType Var = ZEGR_SCDT_VOID;
	switch (Type)
	{
		case D3D_SVT_VOID:
			Var = ZEGR_SCDT_VOID;
			break;
		case D3D_SVT_BOOL:
			Var = ZEGR_SCDT_BOOL;
			break;
		case D3D_SVT_INT:
			Var = ZEGR_SCDT_INT;
			break;
		case D3D_SVT_UINT:
			Var = ZEGR_SCDT_UINT;
			break;
		case D3D_SVT_FLOAT:
			Var = ZEGR_SCDT_FLOAT;
			break;
	}

	return Var;
}

inline static ZEGRShaderSystemSemantic GetZEShaderSystemValueType(D3D_NAME Name)
{
	ZEGRShaderSystemSemantic Type = ZEGR_SSS_NONE;

	switch (Name)
	{
		case D3D_NAME_UNDEFINED:
			Type = ZEGR_SSS_NONE;
			break;
		case D3D_NAME_POSITION:
			Type = ZEGR_SSS_POSITION;
			break;
		case D3D_NAME_CLIP_DISTANCE:
			Type = ZEGR_SSS_CLIP_DISTANCE;
			break;
		case D3D_NAME_CULL_DISTANCE:
			Type = ZEGR_SSS_CULL_DISTANCE;
			break;
		case D3D_NAME_RENDER_TARGET_ARRAY_INDEX:
			Type = ZEGR_SSS_RENDER_TARGET_ARRAY_INDEX;
			break;
		case D3D_NAME_VIEWPORT_ARRAY_INDEX:
			Type = ZEGR_SSS_VIEWPORT_ARRAY_INDEX;
			break;
		case D3D_NAME_VERTEX_ID:
			Type = ZEGR_SSS_VERTEX_ID;
			break;
		case D3D_NAME_PRIMITIVE_ID:
			Type = ZEGR_SSS_PRIMITIVE_ID;
			break;
		case D3D_NAME_INSTANCE_ID:
			Type = ZEGR_SSS_INSTANCE_ID;
			break;
		case D3D_NAME_IS_FRONT_FACE:
			Type = ZEGR_SSS_IS_FRONT_FACE;
			break;
		case D3D_NAME_SAMPLE_INDEX:
			Type = ZEGR_SSS_SAMPLE_INDEX;
			break;
		case D3D_NAME_TARGET:
			Type = ZEGR_SSS_TARGET;
			break;
		case D3D_NAME_DEPTH:
			Type = ZEGR_SSS_DEPTH;
			break;
		case D3D_NAME_COVERAGE:
			Type = ZEGR_SSS_COVERAGE;
			break;
	}

	return Type;
}

inline static ZEGRShaderRegisterType GetZEShaderComponentType(D3D_REGISTER_COMPONENT_TYPE Type)
{
	static const ZEGRShaderRegisterType Values[] =
	{
		ZEGR_SRT_NONE,				// D3D_REGISTER_COMPONENT_UNKNOWN	= 0,
		ZEGR_SRT_UNSIGNED_INT_32,		// D3D_REGISTER_COMPONENT_UINT32	= 1,
		ZEGR_SRT_SIGNED_INT_32,		// D3D_REGISTER_COMPONENT_SINT32	= 2,
		ZEGR_SRT_FLOAT_32				// D3D_REGISTER_COMPONENT_FLOAT32	= 3
	};
	
	return Values[Type];
}

inline static ZEGRTextureType GetZETextureType(D3D_SRV_DIMENSION Dimension)
{
	ZEGRTextureType Var = ZEGR_TT_NONE;
	switch(Dimension)
	{
		case D3D_SRV_DIMENSION_TEXTURE1D:
		case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
			zeCriticalError("ZETexture1D not implemented yet");
			break;
		case D3D_SRV_DIMENSION_TEXTURE2D:
		case D3D_SRV_DIMENSION_TEXTURE2DMS:
		case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
		case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:
			Var = ZEGR_TT_2D;
			break;
		case D3D_SRV_DIMENSION_TEXTURE3D:
			Var = ZEGR_TT_3D;
			break;
		case D3D_SRV_DIMENSION_TEXTURECUBE:
			Var = ZEGR_TT_CUBE;
			break;
	}

	return Var;
}

inline static ZEGRShaderSamplerType GetZEShaderSamplerReturnType(D3D_RESOURCE_RETURN_TYPE Type)
{
	static const ZEGRShaderSamplerType Values[] =
	{
		(ZEGRShaderSamplerType)-1,		
		ZEGR_SSRT_UNSIGNED_NORMALIZED,	// D3D_RETURN_TYPE_UNORM	= 1,		
		ZEGR_SSRT_SIGNED_NORMALIZED,		// D3D_RETURN_TYPE_SNORM	= 2,
		ZEGR_SSRT_SIGNED_INTEGER,			// D3D_RETURN_TYPE_SINT		= 3,
		ZEGR_SSRT_UNSIGNED_INTEGER,		// D3D_RETURN_TYPE_UINT		= 4,
		ZEGR_SSRT_FLOAT,					// D3D_RETURN_TYPE_FLOAT	= 5,
		ZEGR_SSRT_MIXED					// D3D_RETURN_TYPE_MIXED	= 6,
	};
	
	return Values[Type];
}


inline static ZEGRShaderConstantBufferType GetZEShaderConstantBufferType(D3D_CBUFFER_TYPE Type)
{
	static const ZEGRShaderConstantBufferType Values[] =
	{
		ZEGR_SCBT_C_BUFFER,	//  D3D_CT_CBUFFER = 0,
		ZEGR_SCBT_T_BUFFER	//  D3D_CT_TBUFFER = 1,
	};
	
	return Values[Type];
}

inline static DXGI_FORMAT GetInputElementFormat(ZEShaderRegisterMask UsedRegisters, ZEGRShaderRegisterType RegisterType)
{
	DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;

	// UInt ofssets of data types based on their component count. 
	// Order is Offset + 0 = float, Offset + 1 = uint, Offset + 2 = int
	static const ZEUInt8 Offsets[4] = 
	{
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT
	};

	// Not very healthy!!
	switch (UsedRegisters)
	{
		case ZE_CM_RED:
			Format = (DXGI_FORMAT)(Offsets[0] + RegisterType);
			break;
		case ZE_CM_RED | ZE_CM_GREEN:
			Format = (DXGI_FORMAT)(Offsets[1] + RegisterType);
			break;
		case ZE_CM_RED | ZE_CM_GREEN | ZE_CM_BLUE:
			Format = (DXGI_FORMAT)(Offsets[2] + RegisterType);
			break;
		case ZE_CM_RED | ZE_CM_GREEN | ZE_CM_BLUE | ZE_CM_ALPHA:
			Format = (DXGI_FORMAT)(Offsets[3] + RegisterType);
			break;
	}

	/*
	if (Components.GetFlags(ZE_SRC_RED))
	{
		switch (Type)
		{
			case ZE_SRCT_FLOAT_32:
				Format = DXGI_FORMAT_R32_FLOAT;
				break;
			case ZE_SRCT_SIGNED_INT_32:
				Format = DXGI_FORMAT_R32_SINT;
				break;
			case ZE_SRCT_UNSIGNED_INT_32:
				Format = DXGI_FORMAT_R32_UINT;
				break;
		}
	}
	if (Components.GetFlags(ZE_SRC_GREEN))
	{
		switch (Type)
		{
			case ZE_SRCT_FLOAT_32:
				Format = DXGI_FORMAT_R32G32_FLOAT;
				break;
			case ZE_SRCT_SIGNED_INT_32:
				Format = DXGI_FORMAT_R32G32_SINT;
				break;
			case ZE_SRCT_UNSIGNED_INT_32:
				Format = DXGI_FORMAT_R32G32_UINT;
				break;
		}
	}
	if (Components.GetFlags(ZE_SRC_BLUE))
	{
		switch (Type)
		{
			case ZE_SRCT_FLOAT_32:
				Format = DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case ZE_SRCT_SIGNED_INT_32:
				Format = DXGI_FORMAT_R32G32B32_SINT;
				break;
			case ZE_SRCT_UNSIGNED_INT_32:
				Format = DXGI_FORMAT_R32G32B32_UINT;
				break;
		}
	}
	if (Components.GetFlags(ZE_SRC_ALPHA))
	{
		switch (Type)
		{
			case ZE_SRCT_FLOAT_32:
				Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;
			case ZE_SRCT_SIGNED_INT_32:
				Format = DXGI_FORMAT_R32G32B32A32_SINT;
				break;
			case ZE_SRCT_UNSIGNED_INT_32:
				Format = DXGI_FORMAT_R32G32B32A32_UINT;
				break;
		}
	}
	*/

	return Format;
}

inline static ZEGRVertexElementType D3D10ToZEVertexElementType(ZEShaderRegisterMask UsedRegisters, D3D_REGISTER_COMPONENT_TYPE ComponentType)
{
	ZEGRVertexElementType Type = ZE_VET_NONE;

	switch (UsedRegisters)
	{
		case ZE_CM_RED:
			switch (ComponentType)
			{
				case D3D_REGISTER_COMPONENT_UINT32:
					Type = ZE_VET_UINT;
					break;
				case D3D_REGISTER_COMPONENT_SINT32:
					Type = ZE_VET_INT;
					break;	
				case D3D_REGISTER_COMPONENT_FLOAT32:
					Type = ZE_VET_FLOAT;
					break;
			};
			break;
		case ZE_CM_RED | ZE_CM_GREEN:
			switch (ComponentType)
			{
				case D3D_REGISTER_COMPONENT_UINT32:
					Type = ZE_VET_UINT2;
					break;
				case D3D_REGISTER_COMPONENT_SINT32:
					Type = ZE_VET_INT2;
					break;	
				case D3D_REGISTER_COMPONENT_FLOAT32:
					Type = ZE_VET_FLOAT2;
					break;
			};
			break;
		case ZE_CM_RED | ZE_CM_GREEN | ZE_CM_BLUE:
			switch (ComponentType)
			{
				case D3D_REGISTER_COMPONENT_UINT32:
					Type = ZE_VET_UINT3;
					break;
				case D3D_REGISTER_COMPONENT_SINT32:
					Type = ZE_VET_INT3;
					break;	
				case D3D_REGISTER_COMPONENT_FLOAT32:
					Type = ZE_VET_FLOAT3;
					break;
			};
			break;
		case ZE_CM_RED | ZE_CM_GREEN | ZE_CM_BLUE | ZE_CM_ALPHA:
			switch (ComponentType)
			{
				case D3D_REGISTER_COMPONENT_UINT32:
					Type = ZE_VET_UINT4;
					break;
				case D3D_REGISTER_COMPONENT_SINT32:
					Type = ZE_VET_INT4;
					break;	
				case D3D_REGISTER_COMPONENT_FLOAT32:
					Type = ZE_VET_FLOAT4;
					break;
			};
			break;
	};

	return Type;
}

ZESSize ProcessPrimitive(ZEArray<ZEGRShaderConstant>* Variables, const ZEString& VariableName, ZESize VariableOffset, ID3D11ShaderReflectionType* VariableInterface);
ZESSize ProcessStruct(ZEArray<ZEGRShaderConstant>* Variables, const ZEString& VariableName, ZESize VariableOffset, ID3D11ShaderReflectionType* VariableInterface);
ZESSize ProcessVariable(ZEArray<ZEGRShaderConstant>* Variables, const ZEString& ParentName, ZESize ParentOffset, ID3D11ShaderReflectionType* VariableInterface);

ZESSize ProcessPrimitive(ZEArray<ZEGRShaderConstant>* Variables, const ZEString& VariableName, ZESize VariableOffset, ID3D11ShaderReflectionType* VariableInterface)
{
	// Get variable type description
	D3D11_SHADER_TYPE_DESC TypeDesc;
	if (FAILED(VariableInterface->GetDesc(&TypeDesc)))
	{
		zeCriticalError("Cannot get variable type description.");
		return -1;
	}

	ZESize NextOffset = VariableOffset;

	// Fill initial/common data
	ZEGRShaderConstant Variable;
	Variable.RowCount = TypeDesc.Rows;
	Variable.ColumnCount = TypeDesc.Columns;
	// NOTE: If variable is an array, each element is placed at a new register regardless of their size
	Variable.Size = (TypeDesc.Rows - 1) * ZE_D3D11_DEVICE_REGISTER_SIZE + TypeDesc.Columns * 4 ;
	Variable.Type = GetZEShaderConstantDataType(TypeDesc.Type);

	// Process elements
	ZESize ElementN = 0;
	ZESize ElementCount = TypeDesc.Elements;
	do
	{
		// If array put Index operator [n] to its name
		ZEString IndexStr = (ElementCount > 0) ? "[" + ZEString::FromInt32((ZEUInt32)ElementN) + "]" : "";
		Variable.Name = VariableName + IndexStr;
		Variable.Offset = NextOffset;
		Variable.Hash = VariableName.Hash();
		Variables->Add(Variable);	
			
		ElementN++;

		// NOTE: Increment start offset for next array variable
		NextOffset += TypeDesc.Rows * ZE_D3D11_DEVICE_REGISTER_SIZE;

	} while (ElementN < ElementCount);

	// return the end of the variable
	return Variable.Offset + Variable.Size;
}

ZESSize ProcessStruct(ZEArray<ZEGRShaderConstant>* Variables, const ZEString& VariableName, ZESize VariableOffset, ID3D11ShaderReflectionType* VariableInterface)
{
	// Get variable type description
	D3D11_SHADER_TYPE_DESC TypeDesc;
	if (FAILED(VariableInterface->GetDesc(&TypeDesc)))
	{
		zeError("Cannot get variable type description.");
		return false;
	}

	ZESize VariableEnd = 0;

	ZEString MemberName = "";
	ZEString MemberIndex = "";

	bool IsArray = TypeDesc.Elements > 0;
	bool IsStruct = TypeDesc.Members > 0;

	// Process every element
	ZESize ElementN = 0;
	ZESize ElementCount = TypeDesc.Elements;
	do
	{
		// form index string
		MemberIndex = IsArray ? "[" + ZEString::FromInt32((ZEUInt32)ElementN) + "]" : "";

		// Process every struct member
		ZESize MemberN = 0;
		ZESize MemberCount = TypeDesc.Members;
		do
		{
			ID3D11ShaderReflectionType* MemberInterface = VariableInterface->GetMemberTypeByIndex((UINT)MemberN);
		
			MemberName = "." + ZEString(VariableInterface->GetMemberTypeName((UINT)MemberN));
			MemberName += (IsStruct ? "" : ".");
		
			VariableEnd = ProcessVariable(Variables, VariableName + MemberIndex + MemberName, VariableOffset, MemberInterface);
			
			MemberN++;

		} while (MemberN < MemberCount);

		// New element should start from next register
		VariableOffset = VariableEnd;
		ZESize UsedComponents = VariableEnd % 16;
		if (UsedComponents != 0)
			VariableOffset += (16 - UsedComponents);
		
		ElementN++;

	} while (ElementN < ElementCount);

	return VariableEnd;
}

inline static ZESSize ProcessVariable(ZEArray<ZEGRShaderConstant>* Variables, const ZEString& ParentName, ZESize ParentOffset, ID3D11ShaderReflectionType* VariableInterface)
{
	// Get variable type description
	D3D11_SHADER_TYPE_DESC TypeDesc;
	if (FAILED(VariableInterface->GetDesc(&TypeDesc)))
	{
		zeError("Cannot get variable type description.");
		return -1;
	}

	ZESize VariableStart = ParentOffset + TypeDesc.Offset;

	ZESSize Result = 0;
	switch (TypeDesc.Class)
	{
		case D3D_SVC_SCALAR:
		case D3D_SVC_VECTOR:
		case D3D_SVC_MATRIX_ROWS:
		case D3D_SVC_MATRIX_COLUMNS:
		{
			Result = ProcessPrimitive(Variables, ParentName, VariableStart, VariableInterface);
			break;
		}
		case D3D_SVC_STRUCT:
		{
			Result = ProcessStruct(Variables, ParentName, VariableStart, VariableInterface);
			break;
		}
		case D3D_SVC_OBJECT:
		{
			zeError("Variable type of Object is not supported.");
			Result = -1;
			break;
		}
	};
	
	return Result;
}

bool ZED3D11ShaderCompiler::CreateMetaTable(ZED3D11Shader* Shader, ID3DBlob* ByteCode)
{
	// Get meta containers
	ZEArray<ZEGRShaderConstantBuffer>* Buffers = &Shader->MetaTable.Buffers;
	ZEArray<ZEGRShaderSampler>* Samplers = &Shader->MetaTable.Samplers;
	ZEArray<ZEGRShaderTexture>* Textures = &Shader->MetaTable.Textures;
	ZEArray<ZEGRShaderInput>* InputSignature = &Shader->MetaTable.Inputs;
	
	LPVOID ByteData = ByteCode->GetBufferPointer();
	SIZE_T ByteSize = ByteCode->GetBufferSize();
	
	// Create reflection API for shader byte code inspection
	ID3D11ShaderReflection* Reflector = NULL; 
	HRESULT Result = D3D11Reflect(ByteData, ByteSize, &Reflector);

	if (FAILED(Result))
	{
		zeError("Cannot create reflection interface.");
		return false;
	}
	
	// Get shader info
	D3D11_SHADER_DESC ShaderDesc;
	if (FAILED(Reflector->GetDesc(&ShaderDesc)))
	{
		ZED3D_RELEASE(Reflector);
		zeError("Cannot get shader description.");
		return false;
	}

	// Get input signature
	InputSignature->Resize(ShaderDesc.InputParameters);
	ZESize InputCount = ShaderDesc.InputParameters;
	for (ZESize I = 0; I < InputCount; ++I)
	{
		D3D11_SIGNATURE_PARAMETER_DESC SignParamDesc;
		if (FAILED(Reflector->GetInputParameterDesc((UINT)I, &SignParamDesc)))
		{
			ZED3D_RELEASE(Reflector);
			zeError("Cannot get input parameter signature.");
			return false;
		}
		
		ZEGRShaderInput* InputParameter = &InputSignature->GetItem(I);
		
		if (strnlen(SignParamDesc.SemanticName, ZE_MAX_SHADER_VARIABLE_NAME) >= ZE_MAX_SHADER_VARIABLE_NAME)
		{
			ZED3D_RELEASE(Reflector);
			zeError("Shader input semantic name too long. \"%s\"", SignParamDesc.SemanticName);
			return false;
		}

		strcpy(InputParameter->Semantic, SignParamDesc.SemanticName);
		InputParameter->Mask = SignParamDesc.Mask;
		InputParameter->Index = SignParamDesc.SemanticIndex;
		InputParameter->RegisterType = GetZEShaderComponentType(SignParamDesc.ComponentType);
		InputParameter->SystemSemantic = GetZEShaderSystemValueType(SignParamDesc.SystemValueType); 
		InputParameter->ElementType = D3D10ToZEVertexElementType(SignParamDesc.Mask, SignParamDesc.ComponentType);
		InputParameter->Hash = ZEGRShaderInput::GetHash(SignParamDesc.SemanticName, (ZEUInt8)SignParamDesc.SemanticIndex);
	}

	// Get textures buffer sampler data
	ZEUInt BindingCount = ShaderDesc.BoundResources;
	for (ZEUInt I = 0; I < BindingCount; ++I)
	{
		D3D11_SHADER_INPUT_BIND_DESC BindDesc;
		if (FAILED(Reflector->GetResourceBindingDesc(I, &BindDesc)))
		{
			ZED3D_RELEASE(Reflector);
			zeError("Cannot get binded resource info.");
			return false;
		}

		switch (BindDesc.Type)
		{
			case D3D_SIT_CBUFFER:
			case D3D_SIT_TBUFFER:
			{
				ZEGRShaderConstantBuffer BufferInfo;
				BufferInfo.Name = BindDesc.Name;
				BufferInfo.Slot = BindDesc.BindPoint;
				BufferInfo.Hash = ZEString(BindDesc.Name).Hash();
				Buffers->Add(BufferInfo);
				break;
			}
			case D3D_SIT_TEXTURE:
			{
				ZEGRShaderTexture TextureInfo;
				TextureInfo.Name = BindDesc.Name;
				TextureInfo.Count = BindDesc.BindCount;
				TextureInfo.Slot = BindDesc.BindPoint;
				TextureInfo.Type = GetZETextureType(BindDesc.Dimension);
				Textures->Add(TextureInfo);
				break;
			}
			case D3D_SIT_SAMPLER:
			{
				ZEGRShaderSampler SamplerInfo;
				SamplerInfo.Name = BindDesc.Name;
				SamplerInfo.Slot = BindDesc.BindPoint;
				SamplerInfo.SampleCount = BindDesc.NumSamples;
				SamplerInfo.SamplerReturnType = GetZEShaderSamplerReturnType(BindDesc.ReturnType);
				Samplers->Add(SamplerInfo);
				break;
			}
		}
	}

	// Process buffers
	ZESize CBufferCount = ShaderDesc.ConstantBuffers;
	Buffers->SetCount(CBufferCount);
	for (ZESize	BuffN = 0; BuffN < CBufferCount; ++BuffN)
	{
		// Get const buffer
		ZEGRShaderConstantBuffer* Buffer = &Buffers->GetItem(BuffN);
		ID3D11ShaderReflectionConstantBuffer* CBuffer = Reflector->GetConstantBufferByName(Buffer->Name.ToCString());

		D3D11_SHADER_BUFFER_DESC CBufferDesc;
		if (FAILED(CBuffer->GetDesc(&CBufferDesc)))
		{
			ZED3D_RELEASE(Reflector);
			zeError("Cannot get binded resource info.");
			return false;
		}
		
		Buffer->Size = CBufferDesc.Size;
		Buffer->Type = GetZEShaderConstantBufferType(CBufferDesc.Type);

		// Process buffer variables
		ZESize VariableCount = CBufferDesc.Variables;
		//Buffer->Constants.SetCount(VariableCount);
		for (ZESize VarN = 0; VarN < VariableCount; ++VarN)
		{
			// Get interface to variable
			ID3D11ShaderReflectionVariable* CBufferVariable = CBuffer->GetVariableByIndex((UINT)VarN);

			// Get first variable description
			D3D11_SHADER_VARIABLE_DESC VariableDesc;
			if (FAILED(CBufferVariable->GetDesc(&VariableDesc)))
			{
				zeError("Cannot get variable description.");
				ZED3D_RELEASE(Reflector);
				return false;
			}

			ZEArray<ZEGRShaderConstant>* Variables = &Buffer->Constants;
			if (ProcessVariable(Variables, VariableDesc.Name, VariableDesc.StartOffset, CBufferVariable->GetType()) < 0)
			{
				zeError("Can not create buffer meta table");
				ZED3D_RELEASE(Reflector);
				return false;
			}
		}	
	}
	
	ZED3D_RELEASE(Reflector);

	return true;
}

ZED3D11VertexShader* ZED3D11ShaderCompiler::CreateVertexShader(ID3DBlob* ByteCode)
{
	ID3D11VertexShader* Shader = NULL;
	HRESULT Result = D3DDevices[0]->CreateVertexShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), NULL, &Shader);
	if (FAILED(Result))
	{
		zeError("D3D10 Vertex shader creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	ZED3D11VertexShader* VertexShader = new ZED3D11VertexShader(ByteCode, Shader);

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Vertex shader created. Size: %Iu.", (ZEUInt64)ByteCode->GetBufferSize());
#endif

	return VertexShader;
}

 ZED3D11GeometryShader* ZED3D11ShaderCompiler::CreateGeometryShader(ID3DBlob* ByteCode)
{	
	ID3D11GeometryShader* Shader = NULL;
	HRESULT Result = D3DDevices[0]->CreateGeometryShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), NULL, &Shader);
	if (FAILED(Result))
	{
		zeError("D3D10 Geometry shader creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	ZED3D11GeometryShader* GeometryShader = new ZED3D11GeometryShader(ByteCode, Shader);

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Geometry shader created. Size: %Iu.", (ZEUInt64)ByteCode->GetBufferSize());
#endif

	return GeometryShader;
}

ZED3D11DomainShader* ZED3D11ShaderCompiler::CreateDomainShader(ID3DBlob* ByteCode)
{
	ID3D11DomainShader* Shader = NULL;
	HRESULT Result = D3DDevices[0]->CreateDomainShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), NULL, &Shader);
	if (FAILED(Result))
	{
		zeError("D3D10 Domain shader creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	ZED3D11DomainShader* DomainShader = new ZED3D11DomainShader(ByteCode, Shader);

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Domain shader created. Size: %Iu.", (ZEUInt64)ByteCode->GetBufferSize());
#endif

	return DomainShader;
}

ZED3D11HullShader* ZED3D11ShaderCompiler::CreateHullShader(ID3DBlob* ByteCode)
{
	ID3D11HullShader* Shader = NULL;
	HRESULT Result = D3DDevices[0]->CreateHullShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), NULL, &Shader);
	if (FAILED(Result))
	{
		zeError("D3D10 Hull shader creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	ZED3D11HullShader* HullShader = new ZED3D11HullShader(ByteCode, Shader);

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Hull shader created. Size: %Iu.", (ZEUInt64)ByteCode->GetBufferSize());
#endif

	return HullShader;
}

ZED3D11PixelShader* ZED3D11ShaderCompiler::CreatePixelShader(ID3DBlob* ByteCode)
{	
	ID3D11PixelShader* Shader = NULL;
	HRESULT Result = D3DDevices[0]->CreatePixelShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), NULL, &Shader);
	if (FAILED(Result))
	{
		zeError("D3D10 Pixel shader creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	ZED3D11PixelShader* PixelShader = new ZED3D11PixelShader(ByteCode, Shader);

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Pixel shader created. Size: %Iu.", (ZEUInt64)ByteCode->GetBufferSize());
#endif

	return PixelShader;
}

ZED3D11ComputeShader* ZED3D11ShaderCompiler::CreateComputeShader(ID3DBlob* ByteCode)
{
	ID3D11ComputeShader* Shader = NULL;
	HRESULT Result = D3DDevices[0]->CreateComputeShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), NULL, &Shader);
	if (FAILED(Result))
	{
		zeError("D3D10 Hull shader creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	ZED3D11ComputeShader* ComputeShader = new ZED3D11ComputeShader(ByteCode, Shader);

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Compute shader created. Size: %Iu.", (ZEUInt64)ByteCode->GetBufferSize());
#endif

	return ComputeShader;
}

ZEGRShader* ZED3D11ShaderCompiler::CompileShader(ZEGRShaderCompileOptions* Options)
{
	zeDebugCheck(Options->SourceData.IsEmpty(), "No shader source available.");
	zeDebugCheck(Options->EntryPoint.IsEmpty(), "Shader entry point is not available");
	zeDebugCheck(Options->Type == ZE_ST_COMPUTE, "Shader type is not supported.");
	zeDebugCheck(Options->Type == ZE_ST_DOMAIN, "Shader type is not supported.");
	zeDebugCheck(Options->Type == ZE_ST_HULL, "Shader type is not supported.");
	zeDebugCheck(Options->Model != ZE_SM_4_0, "Shader model is not supported by this module.");
	
	// Decide shader type
	ZEString Profile = "";
	switch (Options->Type)
	{
		case ZE_ST_VERTEX:
			Profile += "vs";
			break;

		case ZE_ST_PIXEL:
			Profile += "ps";
			break;

		case ZE_ST_GEOMETRY:
			Profile += "gs";
			break;

		case ZE_ST_COMPUTE:
			Profile += "cs";
			break;

		case ZE_ST_DOMAIN:
			Profile += "ds";
			break;

		case ZE_ST_HULL:
			Profile += "hs";
			break;

		default:
			zeError("Unsupported shader type");
			return NULL;
			break;
	}

	// Decide shader model
	switch (Options->Model)
	{
		case ZE_SM_4_0:
			Profile += "_4_0";
			break;

		case ZE_SM_4_1:
			Profile += "_4_1";
			break;
		
		case ZE_SM_5_0:
			Profile += "_5_0";
			break;

		default:
			zeError("Unsupported shader model");
			return NULL;
			break;
	}
	
	// Fill macros array
	ZESize ComponentCount = Options->Definitions.GetCount();
	ZEArray<D3D_SHADER_MACRO> Macros;
	Macros.SetCount(ComponentCount + 1);
	for (ZESize I = 0; I < ComponentCount; I++)
	{
		Macros[I].Name = Options->Definitions[I].Name.ToCString();
		Macros[I].Definition = Options->Definitions[I].Value.ToCString();

		// Also hash the parameter
		Options->Definitions[I].Hash = Options->Definitions[I].Name.Hash();
	}
	Macros[ComponentCount].Name = NULL;
	Macros[ComponentCount].Definition = NULL;
	
	// Assign temp variables
	ID3DBlob* Output = NULL;
	ID3DBlob* ByteCode = NULL;
	LPCSTR TempProfile = Profile.ToCString();
	LPCSTR TempSource = Options->SourceData.ToCString();
	SIZE_T TempSize = Options->SourceData.GetSize();
	LPCSTR TempEntry = Options->EntryPoint.ToCString();
	const D3D_SHADER_MACRO* TempMacros = Macros.GetConstCArray();

	// Compile
	HRESULT Result = D3DCompile(TempSource, TempSize, Options->FileName, TempMacros, &D3D10Include, TempEntry, TempProfile, ZE_SHADER_COMPILER_PARAMETERS, NULL, &ByteCode, &Output);
	if (FAILED(Result))
	{
		zeError("Can not compile shader.\r\nFile: \"%s\".\r\nCompile output: \r\n%s\r\n", Options->FileName.ToCString(), Output->GetBufferPointer());
		return NULL;
	}

	ZED3D_RELEASE(Output);

	ZED3D11Shader* Shader = NULL;
	switch (Options->Type)
	{
		case ZE_ST_VERTEX:
			Shader = CreateVertexShader(ByteCode);
			break;

		case ZE_ST_DOMAIN:
			Shader = CreateDomainShader(ByteCode);
			break;

		case ZE_ST_HULL:
			Shader = CreateHullShader(ByteCode);
			break;

		case ZE_ST_GEOMETRY:
			Shader = CreateGeometryShader(ByteCode);
			break;

		case ZE_ST_PIXEL:
			Shader = CreatePixelShader(ByteCode);
			break;

		case ZE_ST_COMPUTE:
			Shader = CreateComputeShader(ByteCode);
			break;
	}
	
	if (Shader == NULL)
		return NULL;

	if (!CreateMetaTable(Shader, ByteCode))
	{
		ZE_DESTROY(Shader);
		return NULL;
	}

	Shader->MetaTable.CompileOptions = *Options;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	char Parameter[36] = {0};
	char Parameters[1024] = {0};
	for (ZESize I = 0; I < Options->Definitions.GetCount(); ++I)
	{
		const char* Name = Options->Definitions[I].Name.ToCString();
		const char* Defn = Options->Definitions[I].Value.ToCString();

		sprintf(Parameter, "%s %s, ", Name, Defn == NULL ? "" : Defn);
		strcat(Parameters, Parameter);
	}

	zeLog("Shader compiled with options: ShaderModel: %u, FileName: %s, Entry: %s, Parameters: %s.", 
			Options->Model + 2, Options->FileName.ToCString(), Options->EntryPoint.ToCString(), Parameters);
#endif

	return Shader;
}

ZED3D11ShaderCompiler::ZED3D11ShaderCompiler()
{

}

ZED3D11ShaderCompiler::~ZED3D11ShaderCompiler()
{

}
