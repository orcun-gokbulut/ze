//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9CommonTools.cpp
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
#include "ZED3D9RenderTarget.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9TextureCube.h"
#include "ZED3D9CommonTools.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZETexture/ZETextureCubeResource.h"

#include <d3dx9.h>

#include <stdio.h>
#include "ZETexture/ZETexture3DResource.h"
#include "ZED3D9Texture3D.h"

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
} D3DIncludeInterface;

/*
void ZED3D9CommonTools::SetRenderTarget(DWORD RenderTarget, ZERenderTarget* ViewPort)
{
	zeDebugCheck(ViewPort == NULL, "ViewPort is null.");

	Device->SetRenderTarget(RenderTarget, ((ZED3D9RenderTarget*)ViewPort)->Surface);
}

void ZED3D9CommonTools::SetRenderTarget(DWORD RenderTarget, ZETexture2D* Texture)
{
	zeDebugCheck(!Texture->IsRenderTarget(), "Texture is not render target.");
	zeDebugCheck(Texture == NULL, "Texture is null.");

	Device->SetRenderTarget(RenderTarget, ((ZED3D9RenderTarget*)Texture->GetViewPort())->Surface);
	
	return;
}

void ZED3D9CommonTools::SetTexture(DWORD Stage, const ZETexture2D* Texture, DWORD Filter, DWORD MipMappingFilter, DWORD Addressing)
{
	Device->SetSamplerState(Stage, D3DSAMP_ADDRESSU, Addressing);
	Device->SetSamplerState(Stage, D3DSAMP_ADDRESSV, Addressing);
	Device->SetSamplerState(Stage, D3DSAMP_MAGFILTER, Filter);
	Device->SetSamplerState(Stage, D3DSAMP_MINFILTER, Filter);
	Device->SetSamplerState(Stage, D3DSAMP_MIPFILTER, MipMappingFilter);
	Device->SetTexture(Stage, ((ZED3D9Texture2D*)Texture)->Texture);
}

void ZED3D9CommonTools::SetTexture(DWORD Stage, ZETexture2DResource* TextureResource, DWORD Filter, DWORD MipMappingFilter, DWORD Addressing)
{
	ZETexture2D* Texture = (ZETexture2D*)TextureResource->GetTexture();
	if (Texture == NULL)
		Device->SetTexture(Stage, NULL);

	ZED3D9CommonTools::SetTexture(Stage, Texture, Filter, MipMappingFilter, Addressing);
}

void ZED3D9CommonTools::SetTexture(DWORD Stage, ZETextureCube* Texture, DWORD Filter, DWORD MipMappingFilter, DWORD Addressing)
{
	Device->SetSamplerState(Stage, D3DSAMP_ADDRESSU, Addressing);
	Device->SetSamplerState(Stage, D3DSAMP_ADDRESSV, Addressing);
	Device->SetSamplerState(Stage, D3DSAMP_ADDRESSW, Addressing);
	Device->SetSamplerState(Stage, D3DSAMP_MAGFILTER, Filter);
	Device->SetSamplerState(Stage, D3DSAMP_MINFILTER, Filter);
	Device->SetSamplerState(Stage, D3DSAMP_MIPFILTER, MipMappingFilter);
	Device->SetTexture(Stage, ((ZED3D9TextureCube*)Texture)->CubeTexture);
}

void ZED3D9CommonTools::SetTexture(DWORD Stage, ZETextureCubeResource* TextureResource, DWORD Filter, DWORD MipMappingFilter, DWORD Addressing)
{
	ZETextureCube* Texture = (ZETextureCube*)TextureResource->GetTexture();
	if (Texture == NULL)
		Device->SetTexture(Stage, NULL);

	ZED3D9CommonTools::SetTexture(Stage, Texture, Filter, MipMappingFilter, Addressing);
}


void ZED3D9CommonTools::SetTexture(DWORD Stage, ZETexture3D* Texture, DWORD Filter, DWORD MipMappingFilter, DWORD Addressing)
{
	Device->SetSamplerState(Stage, D3DSAMP_ADDRESSU, Addressing);
	Device->SetSamplerState(Stage, D3DSAMP_ADDRESSV, Addressing);
	Device->SetSamplerState(Stage, D3DSAMP_ADDRESSW, Addressing);
	Device->SetSamplerState(Stage, D3DSAMP_MAGFILTER, Filter);
	Device->SetSamplerState(Stage, D3DSAMP_MINFILTER, Filter);
	Device->SetSamplerState(Stage, D3DSAMP_MIPFILTER, MipMappingFilter);
	Device->SetTexture(Stage, ((ZED3D9Texture3D*)Texture)->VolumeTexture);
}

void ZED3D9CommonTools::SetTexture(DWORD Stage, ZETexture3DResource* TextureResource, DWORD Filter, DWORD MipMappingFilter, DWORD Addressing)
{
	ZETexture3D* Texture = (ZETexture3D*)TextureResource->GetTexture();
	if (Texture == NULL)
		Device->SetTexture(Stage, NULL);

	ZED3D9CommonTools::SetTexture(Stage, Texture, Filter, MipMappingFilter, Addressing);
}
*/

D3DFORMAT  ZED3D9CommonTools::ConvertPixelFormat(ZETexturePixelFormat Format)
{
	switch(Format)
	{
		case ZE_TPF_I8:
			return D3DFMT_L8;
		case ZE_TPF_I8_4:
			return D3DFMT_A8R8G8B8;
		case ZE_TPF_I16:
			return D3DFMT_L16;
		case ZE_TPF_I16_2:
			return D3DFMT_G16R16;

		case ZE_TPF_F16:
			return D3DFMT_R16F;
		case ZE_TPF_F16_2:
			return D3DFMT_G16R16F;
		case ZE_TPF_F16_4:
			return D3DFMT_A16B16G16R16F;

		case ZE_TPF_F32:
			return D3DFMT_R32F;
		case ZE_TPF_F32_2:
			return D3DFMT_G32R32F;
		case ZE_TPF_F32_4:
			return D3DFMT_A32B32G32R32F;

		case ZE_TPF_DXT1:
			return D3DFMT_DXT1;
		case ZE_TPF_DXT3:
			return D3DFMT_DXT3;
		case ZE_TPF_DXT5:
			return D3DFMT_DXT5;

		default:
			return (D3DFORMAT)0;
	}
}

bool ZED3D9CommonTools::CompileVertexShader(LPDIRECT3DVERTEXSHADER9* VertexShader, const char* FileName, const char* MainFunction, const char* ShaderProfile, ZEUInt32 Components)
{
	LPD3DXBUFFER ShaderBuffer = NULL;
	LPD3DXBUFFER CompilerOutput = NULL;

	ZED3D_RELEASE(*VertexShader);
	
	D3DXMACRO Macros[33];
	char ComponentTexts[32][26];

	ZESize ComponentCount = 0;
	for (ZEUInt I = 0; I < 32; I++)
	{
		if ((ZEUInt)Components & (1 << I))
		{
			sprintf(ComponentTexts[ComponentCount], "ZE_SHADER_COMPONENT_%d", I);
			Macros[ComponentCount].Name = ComponentTexts[ComponentCount];
			Macros[ComponentCount].Definition = "";
			ComponentCount++;
		}
	}
	
	Macros[ComponentCount].Name = NULL;
	Macros[ComponentCount].Definition = NULL;

	if (D3DXCompileShaderFromFile(FileName, Macros, &D3DIncludeInterface, MainFunction, ShaderProfile, D3DXSHADER_PACKMATRIX_COLUMNMAJOR | ZE_SHADER_COMPILER_PARAMETERS, &ShaderBuffer, &CompilerOutput, NULL) != D3D_OK)
	{
		if (CompilerOutput == NULL)
		{
			zeError("Can not vertex compile shader. Shader file name : \"%s\"", FileName);
		}
		else
		{
			zeError("Can not compile vertex shader.\r\nShader file name : \"%s\".\r\nCompile output :\r\n%s\r\n", FileName, CompilerOutput->GetBufferPointer());
		}

		*VertexShader = NULL;
		return false;
	}

	if (Device->CreateVertexShader((DWORD*)ShaderBuffer->GetBufferPointer(), VertexShader) != NULL)
	{
		zeError("Can not create vertex shader.\r\n");
		*VertexShader = NULL;
		return false;
	}

	if (ShaderBuffer != NULL)
		ShaderBuffer->Release();

	if (CompilerOutput != NULL)
		CompilerOutput->Release();

	return true;
}


bool ZED3D9CommonTools::CompilePixelShader(LPDIRECT3DPIXELSHADER9* PixelShader, const char* FileName, const char* MainFunction, const char* ShaderProfile, ZEUInt32 Components)
{
	LPD3DXBUFFER ShaderBuffer = NULL;
	LPD3DXBUFFER CompilerOutput = NULL;

	ZED3D_RELEASE(*PixelShader);

	D3DXMACRO Macros[33];
	char ComponentTexts[32][26];

	ZEInt ComponentCount = 0;
	for (ZEInt I = 0; I < 32; I++)
		if (Components & (1 << I))
		{
			sprintf(ComponentTexts[ComponentCount], "ZE_SHADER_COMPONENT_%d", I);
			Macros[ComponentCount].Name = ComponentTexts[ComponentCount];
			Macros[ComponentCount].Definition = "";
			ComponentCount++;
		}
		Macros[ComponentCount].Name = NULL;
		Macros[ComponentCount].Definition = NULL;
	if (D3DXCompileShaderFromFile(FileName, Macros, &D3DIncludeInterface, MainFunction, ShaderProfile, D3DXSHADER_PACKMATRIX_COLUMNMAJOR | ZE_SHADER_COMPILER_PARAMETERS, &ShaderBuffer, &CompilerOutput, NULL) != D3D_OK)
	{
		if (CompilerOutput == NULL)
		{
			zeError("Can not vertex compile shader. Shader file : \"%s\"", FileName);
		}
		else
		{
			zeError("Can not compile pixel shader.\r\nShader file : \"%s\".\r\n Compile output :\r\n%s\r\n", FileName, CompilerOutput->GetBufferPointer());
		}

		*PixelShader = NULL;
		return false;
	}

	if (Device->CreatePixelShader((DWORD*)ShaderBuffer->GetBufferPointer(), PixelShader) != NULL)
	{
		zeError("Can not create pixel shader.");
		*PixelShader = NULL;
		return false;
	}

	if (ShaderBuffer != NULL)
		ShaderBuffer->Release();
	if (CompilerOutput != NULL)
		CompilerOutput->Release();

	return true;
}

bool ZED3D9CommonTools::CompileVertexShaderFromMemory(LPDIRECT3DVERTEXSHADER9* VertexShader, const char* Source, const char* ShaderName, const char* ShaderProfile, ZEUInt32 Components)
{
	LPD3DXBUFFER ShaderBuffer;
	LPD3DXBUFFER CompilerOutput;
	
	ZED3D_RELEASE(*VertexShader);

	D3DXMACRO Macros[33];
	char ComponentTexts[32][26];

	ZEInt ComponentCount = 0;
	for (ZEInt I = 0; I < 32; I++)
		if (Components & (1 << I))
		{
			sprintf(ComponentTexts[ComponentCount], "ZE_SHADER_COMPONENT_%d", I);
			Macros[ComponentCount].Name = ComponentTexts[ComponentCount];
			Macros[ComponentCount].Definition = "";
			ComponentCount++;
		}
		Macros[ComponentCount].Name = NULL;
		Macros[ComponentCount].Definition = NULL;

	if (D3DXCompileShader(Source, (UINT)strlen(Source), Macros, &D3DIncludeInterface, "vs_main", ShaderProfile, D3DXSHADER_PACKMATRIX_COLUMNMAJOR | ZE_SHADER_COMPILER_PARAMETERS, &ShaderBuffer, &CompilerOutput, NULL) != D3D_OK)
	{
		zeError("Can not compile vertex shader.\r\nShader name : \"%s\".\r\nCompile output :\r\n%s\r\n", ShaderName, CompilerOutput->GetBufferPointer());
		*VertexShader = NULL;
		return false;
	}

	if (Device->CreateVertexShader((DWORD*)ShaderBuffer->GetBufferPointer(), VertexShader) != NULL)
	{
		zeError("Can not create vertex shader.\r\n");
		*VertexShader = NULL;
		return false;
	}

	if (ShaderBuffer != NULL)
		ShaderBuffer->Release();

	if (CompilerOutput != NULL)
		CompilerOutput->Release();

	return true;
}


bool ZED3D9CommonTools::CompilePixelShaderFromMemory(LPDIRECT3DPIXELSHADER9* PixelShader, const char* Source, const char* ShaderName, const char* ShaderProfile, ZEUInt32 Components)
{
	LPD3DXBUFFER ShaderBuffer;
	LPD3DXBUFFER CompilerOutput;
	
	ZED3D_RELEASE(*PixelShader);

	D3DXMACRO Macros[33];
	char ComponentTexts[32][26];

	ZEInt ComponentCount = 0;
	for (ZEInt I = 0; I < 32; I++)
		if (Components & (1 << I))
		{
			sprintf(ComponentTexts[ComponentCount], "ZE_SHADER_COMPONENT_%d", I);
			Macros[ComponentCount].Name = ComponentTexts[ComponentCount];
			Macros[ComponentCount].Definition = "";
			ComponentCount++;
		}
		Macros[ComponentCount].Name = NULL;
		Macros[ComponentCount].Definition = NULL;

	if (D3DXCompileShader(Source, (UINT)strlen(Source), Macros, &D3DIncludeInterface, "ps_main", ShaderProfile, D3DXSHADER_PACKMATRIX_COLUMNMAJOR | ZE_SHADER_COMPILER_PARAMETERS, &ShaderBuffer, &CompilerOutput, NULL) != D3D_OK)
	{
		zeError("Can not compile pixel shader.\r\nShader Name : \"%s\".\r\n Compile output :\r\n%s\r\n", ShaderName, CompilerOutput->GetBufferPointer());
		*PixelShader = NULL;
		return false;
	}

	if (Device->CreatePixelShader((DWORD*)ShaderBuffer->GetBufferPointer(), PixelShader) != NULL)
	{
		zeError("Can not create pixel shader.");
		*PixelShader = NULL;
		return false;
	}

	if (ShaderBuffer != NULL)
		ShaderBuffer->Release();
	if (CompilerOutput != NULL)
		CompilerOutput->Release();

	return true;
}
