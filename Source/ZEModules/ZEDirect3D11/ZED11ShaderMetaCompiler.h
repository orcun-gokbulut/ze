//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11ShaderMetaCompiler.h
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

#pragma once

#include "ZEGraphics/ZEGRShaderCompiler.h"

#include "ZED11ComponentBase.h"
#include "ZEGraphics/ZEGRShaderMeta.h"

#include <d3dcompiler.h>
#include <d3dcompiler.inl>
#include <d3dcommon.h>

class ZED11ShaderMetaCompiler : public ZEGRShaderCompiler
{
	friend class ZED11Module;

	private:
		static ZEGRShaderConstantType		ConvertConstantType(D3D_SHADER_VARIABLE_TYPE Type);
		static ZEGRShaderSystemSemantic		ConvertSystemSemantic(D3D_NAME Name);
		static ZEGRShaderRegisterType		ConvertRegisterType(D3D_REGISTER_COMPONENT_TYPE Type);
		static ZEGRTextureType				ConvertTextureType(D3D_SRV_DIMENSION Dimension);
		static ZEGRShaderSamplerType		ConvertSamplerType(D3D_RESOURCE_RETURN_TYPE Type);
		static ZEGRShaderConstantBufferType ConvertConstantBufferType(D3D_CBUFFER_TYPE Type);
		static DXGI_FORMAT					ConvertInputElementFormat(ZEShaderRegisterMask UsedRegisters, ZEGRShaderRegisterType RegisterType);
		static ZEGRVertexElementType		ConvertVertexElementType(ZEShaderRegisterMask UsedRegisters, D3D_REGISTER_COMPONENT_TYPE ComponentType);
		
		static ZESSize						ProcessPrimitive(ZEArray<ZEGRShaderConstant>* Variables, const ZEString& VariableName, ZESize VariableOffset, ID3D11ShaderReflectionType* VariableInterface);
		static ZESSize						ProcessStruct(ZEArray<ZEGRShaderConstant>* Variables, const ZEString& VariableName, ZESize VariableOffset, ID3D11ShaderReflectionType* VariableInterface);
		static ZESSize						ProcessVariable(ZEArray<ZEGRShaderConstant>* Variables, const ZEString& ParentName, ZESize ParentOffset, ID3D11ShaderReflectionType* VariableInterface);
	public:
		static bool							Reflect(ZEGRShaderMeta* Meta, ID3DBlob* ByteCode);
};
