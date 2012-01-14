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

#include "ZED3D9ComponentBase.h"
#include "ZEDS/ZEArray.h"
#include <d3d9.h>

enum ZED3D9ShaderType
{
	ZE_D3D9_ST_VERTEX,
	ZE_D3D9_ST_PIXEL
};

class ZED3D9Shader
{
	friend class ZED3D9ShaderManager;
	private:
		ZEUInt32							Hash;
		char							FileName[100];
		char							FunctionName[100];
		ZEUInt32							Components;

		int								CacheIndex;
		int								ReferanceCount;
	
	protected:
										ZED3D9Shader();
		virtual							~ZED3D9Shader();

	public:
		virtual ZED3D9ShaderType		GetShaderType() = 0;

		const char*						GetFileName();
		const char*						GetFunctionName();
		ZEUInt32							GetComponents();

		void							Release();

		static ZED3D9Shader*			CreateShader(const char* FileName, const char* FunctionName, ZEUInt32 Components, ZED3D9ShaderType Type, const char* Profile);
};


class ZED3D9PixelShader : public ZED3D9Shader
{
	friend class ZED3D9ShaderManager;
	private:
		LPDIRECT3DPIXELSHADER9			PixelShader;
		
										ZED3D9PixelShader();
		virtual							~ZED3D9PixelShader();

	public:
		virtual ZED3D9ShaderType		GetShaderType();
		LPDIRECT3DPIXELSHADER9			GetPixelShader();

		static ZED3D9PixelShader*		CreateShader(const char* FileName, const char* FunctionName, ZEUInt32 Components, const char* Profile);
};


class ZED3D9VertexShader : public ZED3D9Shader
{
	friend class ZED3D9ShaderManager;
	private:
		LPDIRECT3DVERTEXSHADER9			VertexShader;

										ZED3D9VertexShader();
										~ZED3D9VertexShader();

	public:
		virtual ZED3D9ShaderType		GetShaderType();
		LPDIRECT3DVERTEXSHADER9			GetVertexShader();

		static ZED3D9VertexShader*		CreateShader(const char* FileName, const char* FunctionName, ZEUInt32 Components, const char* Profile);
};

#endif




