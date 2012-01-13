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

#include "ZED3D9Shader.h"
#include "ZED3D9ShaderManager.h"
#include "ZED3D9CommonTools.h"

// ZED3D9Shader
////////////////////////////////////////////////////////////////////////////////////////
ZED3D9Shader::ZED3D9Shader()
{
	ReferanceCount = 1;
}

ZED3D9Shader::~ZED3D9Shader()
{

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
	ZED3D9ShaderManager::GetInstance()->ReleaseShader(this);
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

ZED3D9VertexShader* ZED3D9VertexShader::CreateShader(const char* FileName, const char* FunctionName, ZEUInt32 Components, const char* Profile)
{
	return ZED3D9ShaderManager::GetInstance()->GetVertexShader(FileName, FunctionName, Components, Profile);
}

