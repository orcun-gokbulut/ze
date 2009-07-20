//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9ComponentBase.cpp
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

#include "D3D9ComponentBase.h"
#include "Direct3D9Module.h"
#include "Core/Error.h"
#include <d3dx9.h>

ZEDirect3D9Module* ZED3D9ComponentBase::Module;
LPDIRECT3DDEVICE9 ZED3D9ComponentBase::Device;

D3DFORMAT ZED3D9ComponentBase::ConvertPixelFormat(ZETexturePixelFormat Format)
{
	switch(Format)
	{
		case ZE_TPF_ARGB32:
			return D3DFMT_A8R8G8B8;
		case ZE_TPF_SHADOWMAP:
			return D3DFMT_D32;
		case ZE_TPF_DEPTH:
			return D3DFMT_R32F;
		case ZE_TPF_HDR:
			return D3DFMT_A16B16G16R16F;
		default:
			return (D3DFORMAT)0;
	}
}

bool ZED3D9ComponentBase::CompileVertexShader(const char* Source, LPDIRECT3DVERTEXSHADER9* VertexShader, ZEArray<D3DXMACRO>* Macros)
{
	LPD3DXBUFFER ShaderBuffer;
	LPD3DXBUFFER CompilerOutput;
	
	if (*VertexShader != NULL)
		(*VertexShader)->Release();

	if (D3DXCompileShader(Source, (UINT)strlen(Source), (Macros != NULL ? Macros->GetConstCArray() : NULL), NULL, "vs_main", "vs_2_0", D3DXSHADER_OPTIMIZATION_LEVEL3, &ShaderBuffer, &CompilerOutput, NULL) != D3D_OK)
	{
		zeError("D3D9 Vertex Shader Compiler", "Can not compile vertex shader.\r\n Compile output :\r\n%s\r\n", CompilerOutput->GetBufferPointer());
		*VertexShader = NULL;
		return false;
	}

	if (Device->CreateVertexShader((DWORD*)ShaderBuffer->GetBufferPointer(), VertexShader) != NULL)
	{
		zeError("D3D9 Vertex Shader Compiler", "Can not create vertex shader.\r\n");
		*VertexShader = NULL;
		return false;
	}

	if (ShaderBuffer != NULL)
		ShaderBuffer->Release();

	if (CompilerOutput != NULL)
		CompilerOutput->Release();

	return true;
}


bool ZED3D9ComponentBase::CompilePixelShader(const char* Source, LPDIRECT3DPIXELSHADER9* PixelShader, ZEArray<D3DXMACRO>* Macros)
{
	LPD3DXBUFFER ShaderBuffer;
	LPD3DXBUFFER CompilerOutput;
	
	if (*PixelShader != NULL)
		(*PixelShader)->Release();

	if (D3DXCompileShader(Source, (UINT)strlen(Source), (Macros != NULL ? Macros->GetConstCArray() : NULL), NULL, "ps_main", "ps_2_0", D3DXSHADER_OPTIMIZATION_LEVEL3 | D3DXSHADER_PARTIALPRECISION, &ShaderBuffer, &CompilerOutput, NULL) != D3D_OK)
	{
		zeError("D3D9 Pixel Shader Compiler", "Can not compile pixel shader.\r\n Compile output :\r\n%s\r\n", CompilerOutput->GetBufferPointer());
		*PixelShader = NULL;
		return false;
	}

	if (Device->CreatePixelShader((DWORD*)ShaderBuffer->GetBufferPointer(), PixelShader) != NULL)
	{
		zeError("D3D9 Pixel Shader Compiler", "Can not create pixel shader.\r\n");
		*PixelShader = NULL;
		return false;
	}

	if (ShaderBuffer != NULL)
		ShaderBuffer->Release();
	if (CompilerOutput != NULL)
		CompilerOutput->Release();

	return true;
}

bool ZED3D9ComponentBase::CreateRenderTarget(LPDIRECT3DTEXTURE9* Target, int Width, int Height, ZETexturePixelFormat Format)
{
	D3DSURFACE_DESC SurDesc;
	D3DFORMAT D3DFormat = ConvertPixelFormat(Format);
	if (*Target != NULL)
	{
		(*Target)->GetLevelDesc(0, &SurDesc);
		if (SurDesc.Width != Width ||SurDesc.Height != Height || SurDesc.Format != D3DFormat)
			(*Target)->Release();
		else
			return true;
	}

	HRESULT Hr = Device->CreateTexture(Width, Height, 1, D3DUSAGE_RENDERTARGET, D3DFormat, D3DPOOL_DEFAULT, Target, NULL);
	if (Hr != D3D_OK)
	{
		zeError("Direct3D 9", "Can not create render target.");
		*Target = NULL;
		return false;
	}

	return true;
}

bool ZED3D9ComponentBase::CreateDepthRenderTarget(LPDIRECT3DSURFACE9* Target, int Width, int Height)
{
	D3DSURFACE_DESC SurDesc;
	if (*Target != NULL)
	{
		(*Target)->GetDesc(&SurDesc);
		if (SurDesc.Width != Width ||SurDesc.Height != Height || SurDesc.Format != D3DFMT_D24S8)
			(*Target)->Release();
		else
			return true;
	}
		
	HRESULT Hr = Device->CreateDepthStencilSurface(Width, Height, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, FALSE, Target, NULL);
	if (Hr != D3D_OK)
	{
		zeError("Direct3D 9", "Can not create depth render target.");
		*Target = NULL;
		return false;
	}

	return true;
}

bool ZED3D9ComponentBase::CreateRenderTarget(LPDIRECT3DSURFACE9* Target, int Width, int Height, ZETexturePixelFormat Format)
{
	D3DSURFACE_DESC SurDesc;
	D3DFORMAT D3DFormat = ConvertPixelFormat(Format);
	if (*Target != NULL)
	{
		(*Target)->GetDesc(&SurDesc);
		if (SurDesc.Width != Width ||SurDesc.Height != Height || SurDesc.Format != D3DFormat)
			(*Target)->Release();
		else
			return true;
	}

	HRESULT Hr = Device->CreateRenderTarget(Width, Height, D3DFormat, D3DMULTISAMPLE_NONE, 0, FALSE, Target, NULL);
	if (Hr != D3D_OK)
	{
		zeError("Direct3D 9", "Can not create render target.");
		*Target = NULL;
		return false;
	}

	return true;
}

bool ZED3D9ComponentBase::BaseInitialize(ZEDirect3D9Module* D3DModule)
{
	Module = D3DModule;
	Device = D3DModule->Device;

	return true;
}
