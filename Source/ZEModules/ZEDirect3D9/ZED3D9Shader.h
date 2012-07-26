//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9Shader.h
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
#ifndef	__ZE_D3D9_SHADER_H__
#define __ZE_D3D9_SHADER_H__

#include "ZETypes.h"
#include "ZED3D9ComponentBase.h"
#include "ZEGraphics/ZEShader.h"
#include "ZEDS/ZEString.h"
#include <d3d9.h>
#include <d3dx9.h>

enum ZED3D9ShaderType
{
	ZE_D3D9_ST_VERTEX,
	ZE_D3D9_ST_PIXEL
};

class ZED3D9Shader : public ZEShader
{
	friend class ZED3D9ShaderManager;
	protected:
		ZESize							ReferanceCount;
		ZEUInt32						Hash;
		ZEUInt32						Components;		//??
		char							FileName[ZE_MAX_SHADER_LENGTH];
		char							FunctionName[ZE_MAX_SHADER_LENGTH];

		void							PopulateConstantTable(LPD3DXCONSTANTTABLE Table);

	public:
		virtual ZED3D9ShaderType		GetShaderType() = 0;

		const char*						GetFileName();
		const char*						GetFunctionName();
		ZEUInt32						GetComponents();

		void							Release();	
		
		static ZED3D9Shader*			CreateShader(const char* FileName, const char* FunctionName, ZEUInt32 Components, ZED3D9ShaderType Type, const char* Profile);

										ZED3D9Shader();
		virtual							~ZED3D9Shader();
};

class ZED3D9PixelShader : public ZED3D9Shader, private ZED3D9ComponentBase
{
	friend class ZED3D9ShaderManager;
	private:
		LPDIRECT3DPIXELSHADER9			PixelShader;
	protected:
		virtual	bool					CompileShader(const ZEString CompilerParameters[][2],
														int CompilerParameterCount,
														ZEString ShaderProfile, 
														ZEString Source,
														ZEString MainFunction);
	public:
		virtual ZED3D9ShaderType		GetShaderType();
		LPDIRECT3DPIXELSHADER9			GetPixelShader() const;

		// Set Constant
		virtual void					SetConstant(const ZEString& Name, const ZEVector2& Value) const;
		virtual void					SetConstant(const ZEString& Name, const ZEVector3& Value) const;
		virtual void					SetConstant(const ZEString& Name, const ZEVector4& Value) const;		
		virtual void					SetConstant(const ZEString& Name, const ZEMatrix3x3& Value) const;
		virtual void					SetConstant(const ZEString& Name, const ZEMatrix4x4& Value) const;
		virtual void					SetConstant(const ZEString& Name, bool Value) const;
		virtual void					SetConstant(const ZEString& Name, float Value) const;

		virtual void					SetConstantArray(const ZEString& Name, const ZEVector2* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(const ZEString& Name, const ZEVector3* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(const ZEString& Name, const ZEVector4* ValueArray, ZESize Count) const;		
		virtual void					SetConstantArray(const ZEString& Name, const ZEMatrix3x3* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(const ZEString& Name, const ZEMatrix4x4* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(const ZEString& Name, const bool* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(const ZEString& Name, const float* ValueArray, ZESize Count) const;

		virtual void					SetConstant(int Register, const ZEVector2& Value) const;
		virtual void					SetConstant(int Register, const ZEVector3& Value) const;
		virtual void					SetConstant(int Register, const ZEVector4& Value) const;		
		virtual void					SetConstant(int Register, const ZEMatrix3x3& Value) const;
		virtual void					SetConstant(int Register, const ZEMatrix4x4& Value) const;
		virtual void					SetConstant(int Register, bool Value) const;
		virtual void					SetConstant(int Register, float Value) const;

		virtual void					SetConstantArray(int Register, const ZEVector2* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(int Register, const ZEVector3* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(int Register, const ZEVector4* ValueArray, ZESize Count) const;		
		virtual void					SetConstantArray(int Register, const ZEMatrix3x3* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(int Register, const ZEMatrix4x4* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(int Register, const bool* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(int Register, const float* ValueArray, ZESize Count) const;

		// Get Constant Value
		virtual ZEVector2				GetConstantVector2(const char* Name) const;
		virtual ZEVector3				GetConstantVector3(const char* Name) const;
		virtual ZEVector4				GetConstantVector4(const char* Name) const;
		virtual ZEMatrix3x3				GetConstantMatrix3x3(const char* Name) const;
		virtual ZEMatrix4x4				GetConstantMatrix4x4(const char* Name) const;
		virtual bool					GetConstantBool(const char* Name) const;
		virtual float					GetConstantFloat(const char* Name) const;

		static ZED3D9PixelShader*		CreateShader(const char* FileName, const char* FunctionName, ZEUInt32 Components, const char* Profile); // Old One Does Not Have Constant Table
		static ZED3D9PixelShader*		CreateShaderFromSource(const char* Source, const char* FunctionName, const ZEString CompilerParameters[][2], int CompilerParameterCount, const char* Profile);
				
										ZED3D9PixelShader();
										~ZED3D9PixelShader();
};


class ZED3D9VertexShader : public ZED3D9Shader, private ZED3D9ComponentBase
{
	friend class ZED3D9ShaderManager;
	private:
		LPDIRECT3DVERTEXSHADER9			VertexShader;

	protected:
		virtual	bool					CompileShader(const ZEString CompilerParameters[][2],
														int CompilerParameterCount,
														ZEString ShaderProfile, 
														ZEString Source,
														ZEString MainFunction);

	public:
		virtual ZED3D9ShaderType		GetShaderType();
		LPDIRECT3DVERTEXSHADER9			GetVertexShader() const;
		
		// Set Constant
		virtual void					SetConstant(const ZEString& Name, const ZEVector2& Value) const;
		virtual void					SetConstant(const ZEString& Name, const ZEVector3& Value) const;
		virtual void					SetConstant(const ZEString& Name, const ZEVector4& Value) const;		
		virtual void					SetConstant(const ZEString& Name, const ZEMatrix3x3& Value) const;
		virtual void					SetConstant(const ZEString& Name, const ZEMatrix4x4& Value) const;
		virtual void					SetConstant(const ZEString& Name, bool Value) const;
		virtual void					SetConstant(const ZEString& Name, float Value) const;

		virtual void					SetConstantArray(const ZEString& Name, const ZEVector2* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(const ZEString& Name, const ZEVector3* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(const ZEString& Name, const ZEVector4* ValueArray, ZESize Count) const;		
		virtual void					SetConstantArray(const ZEString& Name, const ZEMatrix3x3* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(const ZEString& Name, const ZEMatrix4x4* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(const ZEString& Name, const bool* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(const ZEString& Name, const float* ValueArray, ZESize Count) const;

		virtual void					SetConstant(int Register, const ZEVector2& Value) const;
		virtual void					SetConstant(int Register, const ZEVector3& Value) const;
		virtual void					SetConstant(int Register, const ZEVector4& Value) const;		
		virtual void					SetConstant(int Register, const ZEMatrix3x3& Value) const;
		virtual void					SetConstant(int Register, const ZEMatrix4x4& Value) const;
		virtual void					SetConstant(int Register, bool Value) const;
		virtual void					SetConstant(int Register, float Value) const;

		virtual void					SetConstantArray(int Register, const ZEVector2* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(int Register, const ZEVector3* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(int Register, const ZEVector4* ValueArray, ZESize Count) const;		
		virtual void					SetConstantArray(int Register, const ZEMatrix3x3* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(int Register, const ZEMatrix4x4* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(int Register, const bool* ValueArray, ZESize Count) const;
		virtual void					SetConstantArray(int Register, const float* ValueArray, ZESize Count) const;

		// Get Constant Value
		virtual ZEVector2				GetConstantVector2(const char* Name) const;
		virtual ZEVector3				GetConstantVector3(const char* Name) const;
		virtual ZEVector4				GetConstantVector4(const char* Name) const;
		virtual ZEMatrix3x3				GetConstantMatrix3x3(const char* Name) const;
		virtual ZEMatrix4x4				GetConstantMatrix4x4(const char* Name) const;
		virtual bool					GetConstantBool(const char* Name) const;
		virtual float					GetConstantFloat(const char* Name) const;

		static ZED3D9VertexShader*		CreateShader(const char* FileName, const char* FunctionName, ZEUInt32 Components, const char* Profile);	// Old One Does Not Have Constant Table
		static ZED3D9VertexShader*		CreateShaderFromSource(const char* Source, const char* FunctionName, const ZEString CompilerParameters[][2], int CompilerParameterCount, const char* Profile);

										ZED3D9VertexShader();
										~ZED3D9VertexShader();
};
#endif




