//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D10ShaderCompiler.cpp
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


#include "ZEError.h"
#include "ZED3D10Shader.h"
#include "ZEFile/ZEFile.h"
#include "ZEDS/ZEPointer.h"
#include "ZEGraphics/ZEShader.h"
#include "ZEDS/ZEHashGenerator.h"
#include "ZED3D10ShaderCompiler.h"
#include "ZED3D10GraphicsDevice.h"
#include "ZED3D10GraphicsModule.h"
#include "ZEGraphics/ZEShaderCompiler.h"

#include <d3d10.h>
// #include <D3Dcompiler.h>


#define ZE_SHADER_COMPILER_DEFAULT_PARAMETERS	(D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_WARNINGS_ARE_ERRORS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR)

#ifdef ZE_DEBUG_D3D10_DEBUG_SHADERS
	#include <d3dx10async.h>
	#define ZE_SHADER_COMPILER_PARAMETERS		(ZE_SHADER_COMPILER_DEFAULT_PARAMETERS | D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION)
#else
	#define ZE_SHADER_COMPILER_PARAMETERS		(ZE_SHADER_COMPILER_DEFAULT_PARAMETERS | D3D10_SHADER_OPTIMIZATION_LEVEL3)
#endif


#define ZE_D3D10_DEVICE_REGISTER_SIZE		16	// Bytes

class ZED3D10Include : public ID3D10Include
{
	HRESULT __stdcall Open(D3D10_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID * ppData, UINT * pBytes)
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


inline static ZEShaderConstantType GetZEShaderConstantDataType(D3D10_SHADER_VARIABLE_TYPE Type)
{
	ZEShaderConstantType Var = ZE_SCDT_VOID;
	switch (Type)
	{
		case D3D10_SVT_VOID:
			Var = ZE_SCDT_VOID;
			break;
		case D3D10_SVT_BOOL:
			Var = ZE_SCDT_BOOL;
			break;
		case D3D10_SVT_INT:
			Var = ZE_SCDT_INT;
			break;
		case D3D10_SVT_UINT:
			Var = ZE_SCDT_UINT;
			break;
		case D3D10_SVT_FLOAT:
			Var = ZE_SCDT_FLOAT;
			break;
	}

	return Var;
}

inline static ZEShaderInputSystemValueType GetZEShaderSystemValueType(D3D10_NAME Name)
{
	ZEShaderInputSystemValueType Var = ZE_SSVT_NONE;

	switch (Name)
	{
		case D3D10_NAME_UNDEFINED:
			Var = ZE_SSVT_NONE;
			break;
		case D3D10_NAME_POSITION:
			Var = ZE_SSVT_POSITION;
			break;
		case D3D10_NAME_CLIP_DISTANCE:
			Var = ZE_SSVT_CLIP_DISTANCE;
			break;
		case D3D10_NAME_CULL_DISTANCE:
			Var = ZE_SSVT_CULL_DISTANCE;
			break;
		case D3D10_NAME_RENDER_TARGET_ARRAY_INDEX:
			Var = ZE_SSVT_RENDER_TARGET_ARRAY_INDEX;
			break;
		case D3D10_NAME_VIEWPORT_ARRAY_INDEX:
			Var = ZE_SSVT_VIEWPORT_ARRAY_INDEX;
			break;
		case D3D10_NAME_VERTEX_ID:
			Var = ZE_SSVT_VERTEX_ID;
			break;
		case D3D10_NAME_PRIMITIVE_ID:
			Var = ZE_SSVT_PRIMITIVE_ID;
			break;
		case D3D10_NAME_INSTANCE_ID:
			Var = ZE_SSVT_INSTANCE_ID;
			break;
		case D3D10_NAME_IS_FRONT_FACE:
			Var = ZE_SSVT_IS_FRONT_FACE;
			break;
		case D3D10_NAME_SAMPLE_INDEX:
			Var = ZE_SSVT_SAMPLE_INDEX;
			break;
		case D3D10_NAME_TARGET:
			Var = ZE_SSVT_TARGET;
			break;
		case D3D10_NAME_DEPTH:
			Var = ZE_SSVT_DEPTH;
			break;
		case D3D10_NAME_COVERAGE:
			Var = ZE_SSVT_COVERAGE;
			break;
	}

	return Var;
}

inline static ZEShaderInputRegisterComponentType GetZEShaderComponentType(D3D10_REGISTER_COMPONENT_TYPE Type)
{
	static const ZEShaderInputRegisterComponentType Values[] =
	{
		ZE_SRCT_NONE,				// D3D10_REGISTER_COMPONENT_UNKNOWN	= 0,
		ZE_SRCT_UNSIGNED_INT_32,	// D3D10_REGISTER_COMPONENT_UINT32	= 1,
		ZE_SRCT_SIGNED_INT_32,		// D3D10_REGISTER_COMPONENT_SINT32	= 2,
		ZE_SRCT_FLOAT_32			// D3D10_REGISTER_COMPONENT_FLOAT32	= 3
	};
	
	return Values[Type];
}

inline static ZETextureType GetZETextureType(D3D10_SRV_DIMENSION Dimension)
{
	ZETextureType Var = ZE_TT_NONE;
	switch(Dimension)
	{
		case D3D10_SRV_DIMENSION_TEXTURE1D:
		case D3D10_SRV_DIMENSION_TEXTURE1DARRAY:
			zeCriticalError("ZETexture1D not implemented yet");
			Var = ZE_TT_2D;
			break;
		case D3D10_SRV_DIMENSION_TEXTURE2D:
		case D3D10_SRV_DIMENSION_TEXTURE2DMS:
		case D3D10_SRV_DIMENSION_TEXTURE2DARRAY:
		case D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY:
			Var = ZE_TT_2D;
			break;
		case D3D10_SRV_DIMENSION_TEXTURE3D:
			Var = ZE_TT_3D;
			break;
		case D3D10_SRV_DIMENSION_TEXTURECUBE:
			Var = ZE_TT_CUBE;
			break;
	}

	return Var;
}

inline static ZEShaderSamplerReturnType GetZEShaderSamplerReturnType(D3D10_RESOURCE_RETURN_TYPE Type)
{
	static const ZEShaderSamplerReturnType Values[] =
	{
		(ZEShaderSamplerReturnType)-1,		
		ZE_SSRT_UNSIGNED_NORMALIZED,	// D3D10_RETURN_TYPE_UNORM	= 1,		
		ZE_SSRT_SIGNED_NORMALIZED,		// D3D10_RETURN_TYPE_SNORM	= 2,
		ZE_SSRT_SIGNED_INTEGER,			// D3D10_RETURN_TYPE_SINT	= 3,
		ZE_SSRT_UNSIGNED_INTEGER,		// D3D10_RETURN_TYPE_UINT	= 4,
		ZE_SSRT_FLOAT,					// D3D10_RETURN_TYPE_FLOAT	= 5,
		ZE_SSRT_MIXED					// D3D10_RETURN_TYPE_MIXED	= 6,
	};
	
	return Values[Type];
}


inline static ZEShaderConstantBufferType GetZEShaderConstantBufferType(D3D10_CBUFFER_TYPE Type)
{
	static const ZEShaderConstantBufferType Values[] =
	{
		ZE_SCBT_C_BUFFER,	//  D3D10_CT_CBUFFER = 0,
		ZE_SCBT_T_BUFFER	//  D3D10_CT_TBUFFER = 1,
	};
	
	return Values[Type];
}

inline static ZESize GetInputElementSize(ZEShaderInputRegisterComponent Components)
{
	ZESize Size = 0;
	if (Components.GetFlags(ZE_SRC_RED))
		Size += 4;
	if (Components.GetFlags(ZE_SRC_GREEN))
		Size += 4;
	if (Components.GetFlags(ZE_SRC_BLUE))
		Size += 4;
	if (Components.GetFlags(ZE_SRC_ALPHA))
		Size += 4;

	return Size;
}

inline static DXGI_FORMAT GetInputElementFormat(ZEShaderInputRegisterComponent Components, ZEShaderInputRegisterComponentType Type)
{
	DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;
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

	return Format;
}

inline static ZEVertexElementType D3D10ToZEVertexElementType(ZEShaderInputRegisterComponent Components, D3D10_REGISTER_COMPONENT_TYPE ComponentType)
{
	ZEVertexElementType Format = ZE_VET_NONE;
	if (Components.GetFlags(ZE_SRC_RED))
	{
		switch (ComponentType)
		{
			case D3D10_REGISTER_COMPONENT_FLOAT32:
				Format = ZE_VET_FLOAT;
				break;
			case D3D10_REGISTER_COMPONENT_SINT32:
				Format = ZE_VET_INT;
				break;
			case D3D10_REGISTER_COMPONENT_UINT32:
				Format = ZE_VET_UINT;
				break;
		}
	}
	if (Components.GetFlags(ZE_SRC_GREEN))
	{
		switch (ComponentType)
		{
			case D3D10_REGISTER_COMPONENT_FLOAT32:
				Format = ZE_VET_FLOAT2;
				break;
			case D3D10_REGISTER_COMPONENT_SINT32:
				Format = ZE_VET_INT2;
				break;
			case D3D10_REGISTER_COMPONENT_UINT32:
				Format = ZE_VET_UINT2;
				break;
		}
	}
	if (Components.GetFlags(ZE_SRC_BLUE))
	{
		switch (ComponentType)
		{
			case D3D10_REGISTER_COMPONENT_FLOAT32:
				Format = ZE_VET_FLOAT3;
				break;
			case D3D10_REGISTER_COMPONENT_SINT32:
				Format = ZE_VET_INT3;
				break;
			case D3D10_REGISTER_COMPONENT_UINT32:
				Format = ZE_VET_UINT3;
				break;
		}
	}
	if (Components.GetFlags(ZE_SRC_ALPHA))
	{
		switch (ComponentType)
		{
			case D3D10_REGISTER_COMPONENT_FLOAT32:
				Format = ZE_VET_FLOAT4;
				break;
			case D3D10_REGISTER_COMPONENT_SINT32:
				Format = ZE_VET_INT4;
				break;
			case D3D10_REGISTER_COMPONENT_UINT32:
				Format = ZE_VET_UINT4;
				break;
		}
	}

	return Format;
}

ZESSize ProcessPrimitive(ZEArray<ZEShaderConstantInfo>* Variables, const ZEString& VariableName, ZESize VariableOffset, ID3D10ShaderReflectionType* VariableInterface);
ZESSize ProcessStruct(ZEArray<ZEShaderConstantInfo>* Variables, const ZEString& VariableName, ZESize VariableOffset, ID3D10ShaderReflectionType* VariableInterface);
ZESSize ProcessVariable(ZEArray<ZEShaderConstantInfo>* Variables, const ZEString& ParentName, ZESize ParentOffset, ID3D10ShaderReflectionType* VariableInterface);

ZESSize ProcessPrimitive(ZEArray<ZEShaderConstantInfo>* Variables, const ZEString& VariableName, ZESize VariableOffset, ID3D10ShaderReflectionType* VariableInterface)
{
	// Get variable type description
	D3D10_SHADER_TYPE_DESC TypeDesc;
	if (FAILED(VariableInterface->GetDesc(&TypeDesc)))
	{
		zeCriticalError("Cannot get variable type description.");
		return -1;
	}

	ZESize NextOffset = VariableOffset;

	// Fill initial/common data
	ZEShaderConstantInfo Variable;
	Variable.RowCount = TypeDesc.Rows;
	Variable.ColumnCount = TypeDesc.Columns;
	// NOTE: If variable is an array, each element is placed at a new register regardless of their size
	Variable.Size = (TypeDesc.Rows - 1) * ZE_D3D10_DEVICE_REGISTER_SIZE + TypeDesc.Columns * 4 ;
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
		NextOffset += TypeDesc.Rows * ZE_D3D10_DEVICE_REGISTER_SIZE;

	} while (ElementN < ElementCount);

	// return the end of the variable
	return Variable.Offset + Variable.Size;
}

ZESSize ProcessStruct(ZEArray<ZEShaderConstantInfo>* Variables, const ZEString& VariableName, ZESize VariableOffset, ID3D10ShaderReflectionType* VariableInterface)
{
	// Get variable type description
	D3D10_SHADER_TYPE_DESC TypeDesc;
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
			ID3D10ShaderReflectionType* MemberInterface = VariableInterface->GetMemberTypeByIndex((UINT)MemberN);
		
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

inline static ZESSize ProcessVariable(ZEArray<ZEShaderConstantInfo>* Variables, const ZEString& ParentName, ZESize ParentOffset, ID3D10ShaderReflectionType* VariableInterface)
{
	// Get variable type description
	D3D10_SHADER_TYPE_DESC TypeDesc;
	if (FAILED(VariableInterface->GetDesc(&TypeDesc)))
	{
		zeError("Cannot get variable type description.");
		return -1;
	}

	ZESize VariableStart = ParentOffset + TypeDesc.Offset;

	ZESSize Result = 0;
	switch (TypeDesc.Class)
	{
		case D3D10_SVC_SCALAR:
		case D3D10_SVC_VECTOR:
		case D3D10_SVC_MATRIX_ROWS:
		case D3D10_SVC_MATRIX_COLUMNS:
		{
			Result = ProcessPrimitive(Variables, ParentName, VariableStart, VariableInterface);
			break;
		}
		case D3D10_SVC_STRUCT:
		{
			Result = ProcessStruct(Variables, ParentName, VariableStart, VariableInterface);
			break;
		}
		case D3D10_SVC_OBJECT:
		{
			zeError("Variable type of Object is not supported.");
			Result = -1;
			break;
		}
	};
	
	return Result;
}

bool ZED3D10ShaderCompiler::CreateMetaTable(ZED3D10Shader* Shader, ID3D10Blob* ByteCode)
{
	// Get meta containers
	ZEArray<ZEShaderBufferInfo>* Buffers = &Shader->MetaTable.Buffers;
	ZEArray<ZEShaderSamplerInfo>* Samplers = &Shader->MetaTable.Samplers;
	ZEArray<ZEShaderTextureInfo>* Textures = &Shader->MetaTable.Textures;
	ZEArray<ZEShaderInputInfo>* InputSignature = &Shader->MetaTable.Inputs;

	// Create reflection API for shader byte code inspection
	ID3D10ShaderReflection* Reflector = NULL;
	if (FAILED(D3D10ReflectShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), &Reflector)))
	{
		zeError("Cannot create reflection interface.");
		return false;
	}

	// Get shader info
	D3D10_SHADER_DESC ShaderDesc;
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
		D3D10_SIGNATURE_PARAMETER_DESC SignParamDesc;
		if (FAILED(Reflector->GetInputParameterDesc((UINT)I, &SignParamDesc)))
		{
			ZED3D_RELEASE(Reflector);
			zeError("Cannot get input parameter signature.");
			return false;
		}
		
		ZEShaderInputInfo* InputParameter = &InputSignature->GetItem(I);
		InputParameter->Components = SignParamDesc.Mask;
		InputParameter->Semantic = SignParamDesc.SemanticName;
		InputParameter->SemanticIndex = SignParamDesc.SemanticIndex;
		InputParameter->SystemValue = GetZEShaderSystemValueType(SignParamDesc.SystemValueType); 
		InputParameter->ComponentType = GetZEShaderComponentType(SignParamDesc.ComponentType);
		InputParameter->Hash = (ZEString(SignParamDesc.SemanticName) + ZEString(SignParamDesc.SemanticIndex)).Hash();
	}

	// Get textures buffer sampler data
	ZEUInt BindingCount = ShaderDesc.BoundResources;
	for (ZEUInt I = 0; I < BindingCount; ++I)
	{
		D3D10_SHADER_INPUT_BIND_DESC BindDesc;
		if (FAILED(Reflector->GetResourceBindingDesc(I, &BindDesc)))
		{
			ZED3D_RELEASE(Reflector);
			zeError("Cannot get binded resource info.");
			return false;
		}

		switch (BindDesc.Type)
		{
			case D3D10_SIT_CBUFFER:
			case D3D10_SIT_TBUFFER:
			{
				ZEShaderBufferInfo BufferInfo;
				BufferInfo.Name = BindDesc.Name;
				BufferInfo.Slot = BindDesc.BindPoint;
				BufferInfo.Hash = ZEString(BindDesc.Name).Hash();
				Buffers->Add(BufferInfo);
				break;
			}
			case D3D10_SIT_TEXTURE:
			{
				ZEShaderTextureInfo TextureInfo;
				TextureInfo.Name = BindDesc.Name;
				TextureInfo.Count = BindDesc.BindCount;
				TextureInfo.Slot = BindDesc.BindPoint;
				TextureInfo.Type = GetZETextureType(BindDesc.Dimension);
				Textures->Add(TextureInfo);
				break;
			}
			case D3D10_SIT_SAMPLER:
			{
				ZEShaderSamplerInfo SamplerInfo;
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
		ZEShaderBufferInfo* Buffer = &Buffers->GetItem(BuffN);
		ID3D10ShaderReflectionConstantBuffer* CBuffer = Reflector->GetConstantBufferByName(Buffer->Name.ToCString());

		D3D10_SHADER_BUFFER_DESC CBufferDesc;
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
			ID3D10ShaderReflectionVariable* CBufferVariable = CBuffer->GetVariableByIndex((UINT)VarN);

			// Get first variable description
			D3D10_SHADER_VARIABLE_DESC VariableDesc;
			if (FAILED(CBufferVariable->GetDesc(&VariableDesc)))
			{
				zeError("Cannot get variable description.");
				ZED3D_RELEASE(Reflector);
				return false;
			}

			ZEArray<ZEShaderConstantInfo>* Variables = &Buffer->Constants;
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

ZED3D10VertexShader* ZED3D10ShaderCompiler::CreateVertexShader(ID3D10Blob* ByteCode)
{
	// Create reflection API for shader byte code inspection
	ID3D10ShaderReflection* Reflector = NULL;
	if (FAILED(D3D10ReflectShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), &Reflector)))
	{
		zeError("Cannot create reflection interface.");
		return NULL;
	}

	// Get shader info
	D3D10_SHADER_DESC ShaderDesc;
	if (FAILED(Reflector->GetDesc(&ShaderDesc)))
	{
		ZED3D_RELEASE(Reflector);
		zeError("Cannot get shader description.");
		return NULL;
	}

	ZEVertexElement	Elements[ZE_MAX_VERTEX_LAYOUT_ELEMENT];

	// Get input signature
	ZEUInt InputCount = ShaderDesc.InputParameters;
	for (ZEUInt I = 0; I < InputCount; ++I)
	{
		D3D10_SIGNATURE_PARAMETER_DESC SignatureParameterDesc;
		if (FAILED(Reflector->GetInputParameterDesc(I, &SignatureParameterDesc)))
		{
			ZED3D_RELEASE(Reflector);
			zeError("Cannot get input parameter signature.");
			return NULL;
		}
		
		Elements[I].SemanticIndex = SignatureParameterDesc.SemanticIndex;
		sprintf(Elements[I].Semantic, "%s", SignatureParameterDesc.SemanticName);
		Elements[I].Type = D3D10ToZEVertexElementType(SignatureParameterDesc.Mask, SignatureParameterDesc.ComponentType);

		// Default values that cannot change for default vertex layout
		Elements[I].ByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
		Elements[I].Usage = ZE_VU_PER_VERTEX;
		Elements[I].InstanceCount = 0;
		Elements[I].StreamSlot = 0;
	}

	ZED3D_RELEASE(Reflector);

	ZED3D10VertexShader* VertexShader = new ZED3D10VertexShader();
	if (FAILED(D3D10Device->CreateVertexShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), &VertexShader->D3D10VertexShader)))
	{
		zeError("Cannot create vertex shader.");
		return NULL;
	}

	VertexShader->D3D10ByteCode = ByteCode;

	VertexShader->DefaultVertexLayout.SetLayout(Elements, InputCount);
	VertexShader->DefaultVertexLayout.UpdateHash();
	VertexShader->DefaultVertexLayout.Dirty = false;
	return VertexShader;
}

 ZED3D10GeometryShader* ZED3D10ShaderCompiler::CreateGeometryShader(ID3D10Blob* ByteCode)
{
	ZED3D10GeometryShader* GeometryShader = new ZED3D10GeometryShader();
	
	if (FAILED(D3D10Device->CreateGeometryShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), &GeometryShader->D3D10GeometryShader)))
	{
		zeError("Cannot create geometry shader.");
		return NULL;
	}

	GeometryShader->D3D10ByteCode = ByteCode;

	return GeometryShader;
}

ZED3D10PixelShader* ZED3D10ShaderCompiler::CreatePixelShader(ID3D10Blob* ByteCode)
{	
	ZED3D10PixelShader* PixelShader = new ZED3D10PixelShader();
	
	if (FAILED(D3D10Device->CreatePixelShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), &PixelShader->D3D10PixelShader)))
	{
		zeError("Cannot create pixel shader.");
		return NULL;
	}

	PixelShader->D3D10ByteCode = ByteCode;

	return PixelShader;
}

ZEShader* ZED3D10ShaderCompiler::CompileShader(ZEShaderCompileOptions* Options)
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
		default:
			zeError("Unsupported shader model");
			return NULL;
			break;
	}
	
	// Fill macros array
	ZESize ComponentCount = Options->Parameters.GetCount();
	ZEArray<D3D10_SHADER_MACRO> Macros;
	Macros.SetCount(ComponentCount + 1);
	for (ZESize I = 0; I < ComponentCount; I++)
	{
		Macros[I].Name = Options->Parameters[I].Name.ToCString();
		Macros[I].Definition = Options->Parameters[I].Definition.ToCString();

		// Also hash the parameter
		Options->Parameters[I].Hash = Options->Parameters[I].Name.Hash();
	}
	Macros[ComponentCount].Name = NULL;
	Macros[ComponentCount].Definition = NULL;
	
	// Assign temp variables
	ID3D10Blob* Output = NULL;
	ID3D10Blob* ByteCode = NULL;
	LPCSTR TempProfile = Profile.ToCString();
	LPCSTR TempSource = Options->SourceData.ToCString();
	SIZE_T TempSize = Options->SourceData.GetSize();
	LPCSTR TempEntry = Options->EntryPoint.ToCString();
	const D3D10_SHADER_MACRO* TempMacros = Macros.GetConstCArray();

	// Compile
	HRESULT Result;
	#if defined ZE_DEBUG_D3D10_DEBUG_SHADERS
		Result = D3DX10CompileFromMemory(TempSource, TempSize, NULL, TempMacros, &D3D10Include, TempEntry, TempProfile, ZE_SHADER_COMPILER_PARAMETERS, NULL, NULL, &ByteCode, &Output, NULL);
	#else
		// NOTE: D3D10CompileShader does not support debug flag !
		Result = D3D10CompileShader(TempSource, TempSize, NULL, TempMacros, &D3D10Include, TempEntry, TempProfile, ZE_SHADER_COMPILER_PARAMETERS, &ByteCode, &Output);
	#endif

	if (FAILED(Result))
	{
		zeError("Can not compile shader.\r\nFile: \"%s\".\r\nCompile output: \r\n%s\r\n", Options->FileName.ToCString(), Output->GetBufferPointer());
		return NULL;
	}

	ZED3D_RELEASE(Output);

	ZED3D10Shader* Shader = NULL;
	switch (Options->Type)
	{
		case ZE_ST_VERTEX:
			Shader = CreateVertexShader(ByteCode);
			break;
		case ZE_ST_PIXEL:
			Shader = CreatePixelShader(ByteCode);
			break;
		case ZE_ST_GEOMETRY:
			Shader = CreateGeometryShader(ByteCode);
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

	return Shader;
}

ZED3D10ShaderCompiler::ZED3D10ShaderCompiler()
{

}

ZED3D10ShaderCompiler::~ZED3D10ShaderCompiler()
{

}
