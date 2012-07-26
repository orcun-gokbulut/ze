//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEShader.h
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

#ifndef __ZE_SHADER_H__
#define __ZE_SHADER_H__

#include "ZETypes.h"
#include "ZEDS/ZEString.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEDS/ZEArray.h"

#define ZE_MAX_SHADER_LENGTH 256		
#define ZE_MAX_SHADER_FUNCTION_NAME_LENGTH 100

#define ZE_MAX_MACRO_AMOUNT 32

enum ZEShaderConstantDataType
{
	ZE_SCDT_VECTOR2,
	ZE_SCDT_VECTOR3,
	ZE_SCDT_VECTOR4,
	ZE_SCDT_MATRIX3x3,
	ZE_SCDT_MATRIX4x4,
	ZE_SCDT_FLOAT,
	ZE_SCDT_BOOLEAN,
	ZE_SCDT_SAMPLER,
	ZE_SCDT_SAMPLER1D,
	ZE_SCDT_SAMPLER2D,
	ZE_SCDT_SAMPLER3D,
	ZE_SCDT_SAMPLERCUBE,
	ZE_SCDT_UNKNOWN

};

struct ZEShaderCompilerParameter
{
	ZEString Name;
	ZEString Definition;
};

struct ZEShaderConstant
{
	ZEString							ConstantName;
	ZEShaderConstantDataType			Type;
	ZESize								Count;				
	ZEString							Semantic;
	ZEUInt32							RegisterNo;			// Samplers Are Also Considered Constants Therefore This Stores Sampler No if it is Sampler	
};

class ZEShader
{
	protected:
		ZEArray<ZEShaderConstant>				ShaderConstants;
		ZEArray<ZEShaderCompilerParameter>		ShaderCompilerParameters;				
		bool									Compiled;

		bool									FindConstantLocation(ZEUInt32& OutLocation, const ZEString Name) const;
		bool									FindParameterLocation(ZEUInt32& OutLocation, const ZEString Name) const;

	public:
		// Compilation Query
		bool									IsCompiled() const;		// Means compiled & Ready to Use

		// Macro Definition Query
		bool									IsDefined(const ZEString& ParameterName) const;
		bool									GetCompilerParameterString(ZEString& Out, const ZEString& ParameterName) const;

		// Shader Constant Query
		bool									GetConstantCount(ZEUInt32& Out, const ZEString& Name) const;
		bool									GetConstantSemantic(ZEString& Out, const ZEString& Name) const;
		bool									GetConstantDataType(ZEShaderConstantDataType& Out, const ZEString& Name) const;
		bool									GetShaderConstantRegister(ZEUInt32& Out, const ZEString& Name) const;

		// This directly calls "GetShaderConstantRegister()" but its has a better logical meaning for sampler constants
		// This function does not check specified "Name" is a sampler or not it directly returns the Sampler No. / Register No.
		// It only returns false if "Name" does not match any of the Names stored in the constant table.
		bool									GetSamplerNumber(ZEUInt32& Out, const ZEString& Name) const;

		// File and Function Name
		virtual const char*						GetFileName() = 0;
		virtual const char*						GetFunctionName() = 0;
		virtual ZEUInt32						GetComponents() = 0;

		// Set Constant
		virtual void							SetConstant(const ZEString& Name, const ZEVector2& Value) const = 0;
		virtual void							SetConstant(const ZEString& Name, const ZEVector3& Value) const = 0;
		virtual void							SetConstant(const ZEString& Name, const ZEVector4& Value) const = 0;		
		virtual void							SetConstant(const ZEString& Name, const ZEMatrix3x3& Value) const = 0;
		virtual void							SetConstant(const ZEString& Name, const ZEMatrix4x4& Value) const = 0;
		virtual void							SetConstant(const ZEString& Name, bool Value) const = 0;
		virtual void							SetConstant(const ZEString& Name, float Value) const = 0;

		virtual void							SetConstantArray(const ZEString& Name, const ZEVector2* ValueArray, ZESize Count) const = 0;
		virtual void							SetConstantArray(const ZEString& Name, const ZEVector3* ValueArray, ZESize Count) const = 0;
		virtual void							SetConstantArray(const ZEString& Name, const ZEVector4* ValueArray, ZESize Count) const = 0;		
		virtual void							SetConstantArray(const ZEString& Name, const ZEMatrix3x3* ValueArray, ZESize Count) const = 0;
		virtual void							SetConstantArray(const ZEString& Name, const ZEMatrix4x4* ValueArray, ZESize Count) const = 0;
		virtual void							SetConstantArray(const ZEString& Name, const bool* ValueArray, ZESize Count) const = 0;
		virtual void							SetConstantArray(const ZEString& Name, const float* ValueArray, ZESize Count) const = 0;

		virtual void							SetConstant(int Register, const ZEVector2& Value) const = 0;
		virtual void							SetConstant(int Register, const ZEVector3& Value) const = 0;
		virtual void							SetConstant(int Register, const ZEVector4& Value) const = 0;		
		virtual void							SetConstant(int Register, const ZEMatrix3x3& Value) const = 0;
		virtual void							SetConstant(int Register, const ZEMatrix4x4& Value) const = 0;
		virtual void							SetConstant(int Register, bool Value) const = 0;
		virtual void							SetConstant(int Register, float Value) const = 0;

		virtual void							SetConstantArray(int Register, const ZEVector2* ValueArray, ZESize Count) const = 0;
		virtual void							SetConstantArray(int Register, const ZEVector3* ValueArray, ZESize Count) const = 0;
		virtual void							SetConstantArray(int Register, const ZEVector4* ValueArray, ZESize Count) const = 0;		
		virtual void							SetConstantArray(int Register, const ZEMatrix3x3* ValueArray, ZESize Count) const = 0;
		virtual void							SetConstantArray(int Register, const ZEMatrix4x4* ValueArray, ZESize Count) const = 0;
		virtual void							SetConstantArray(int Register, const bool* ValueArray, ZESize Count) const = 0;
		virtual void							SetConstantArray(int Register, const float* ValueArray, ZESize Count) const = 0;

		// Get Constant Value
		virtual ZEVector2						GetConstantVector2(const char* Name) const = 0;
		virtual ZEVector3						GetConstantVector3(const char* Name) const = 0;
		virtual ZEVector4						GetConstantVector4(const char* Name) const = 0;
		virtual ZEMatrix3x3						GetConstantMatrix3x3(const char* Name) const = 0;
		virtual ZEMatrix4x4						GetConstantMatrix4x4(const char* Name) const = 0;
		virtual bool							GetConstantBool(const char* Name) const = 0;
		virtual float							GetConstantFloat(const char* Name) const = 0;
		
		// Compilation
		virtual	bool							CompileShader(const ZEString CompilerParameters[][2],
																int CompilerParameterCount,
																ZEString ShaderProfile, 
																ZEString Source,
																ZEString MainFunction) = 0;

		virtual void							Release() = 0;

		virtual									~ZEShader();
};
#endif
