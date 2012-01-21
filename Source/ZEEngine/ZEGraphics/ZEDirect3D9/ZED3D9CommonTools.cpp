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
#include "ZED3D9ViewPort.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9TextureCube.h"
#include "ZED3D9CommonTools.h"
#include "ZEGraphics/ZETexture2DResource.h"
#include "ZEGraphics/ZETextureCubeResource.h"

#include <d3dx9.h>
#include <stdio.h>

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

			fseek(File, 0, SEEK_END);
			*pBytes = ftell(File);
			fseek(File, 0, SEEK_SET);
			*ppData = (void*)new char[*pBytes];
			fread((void*)*ppData, 1, *pBytes, File);
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

void ZED3D9CommonTools::SetRenderTarget(DWORD RenderTarget, ZEViewPort* ViewPort)
{
	zeAssert(ViewPort == NULL, "ViewPort is null.");

	GetDevice()->SetRenderTarget(RenderTarget, ((ZED3D9ViewPort*)ViewPort)->FrameBuffer);
}

void ZED3D9CommonTools::SetRenderTarget(DWORD RenderTarget, ZETexture2D* Texture)
{
	zeAssert(!Texture->IsRenderTarget(), "Texture is not render target.");
	zeAssert(Texture == NULL, "Texture is null.");

	GetDevice()->SetRenderTarget(RenderTarget, ((ZED3D9ViewPort*)Texture->GetViewPort())->FrameBuffer);
	
	return;
}

void ZED3D9CommonTools::SetTexture(DWORD Stage, ZETexture2D* Texture, DWORD Filter, DWORD MipMappingFilter, DWORD Addressing)
{
	GetDevice()->SetSamplerState(Stage, D3DSAMP_ADDRESSU, Addressing);
	GetDevice()->SetSamplerState(Stage, D3DSAMP_ADDRESSV, Addressing);
	GetDevice()->SetSamplerState(Stage, D3DSAMP_MAGFILTER, Filter);
	GetDevice()->SetSamplerState(Stage, D3DSAMP_MINFILTER, Filter);
	GetDevice()->SetSamplerState(Stage, D3DSAMP_MIPFILTER, MipMappingFilter);
	GetDevice()->SetTexture(Stage, ((ZED3D9Texture2D*)Texture)->Texture);
}

void ZED3D9CommonTools::SetTexture(DWORD Stage, ZETexture2DResource* TextureResource, DWORD Filter, DWORD MipMappingFilter, DWORD Addressing)
{
	ZETexture2D* Texture = (ZETexture2D*)TextureResource->GetTexture();
	if (Texture == NULL)
		GetDevice()->SetTexture(Stage, NULL);

	ZED3D9CommonTools::SetTexture(Stage, Texture, Filter, MipMappingFilter, Addressing);
}

void ZED3D9CommonTools::SetTexture(DWORD Stage, ZETextureCube* Texture, DWORD Filter, DWORD MipMappingFilter, DWORD Addressing)
{
	GetDevice()->SetSamplerState(Stage, D3DSAMP_ADDRESSU, Addressing);
	GetDevice()->SetSamplerState(Stage, D3DSAMP_ADDRESSV, Addressing);
	GetDevice()->SetSamplerState(Stage, D3DSAMP_ADDRESSW, Addressing);
	GetDevice()->SetSamplerState(Stage, D3DSAMP_MAGFILTER, Filter);
	GetDevice()->SetSamplerState(Stage, D3DSAMP_MINFILTER, Filter);
	GetDevice()->SetSamplerState(Stage, D3DSAMP_MIPFILTER, MipMappingFilter);
	GetDevice()->SetTexture(Stage, ((ZED3D9Texture2D*)Texture)->Texture);
}

void ZED3D9CommonTools::SetTexture(DWORD Stage, ZETextureCubeResource* TextureResource, DWORD Filter, DWORD MipMappingFilter, DWORD Addressing)
{
	ZETextureCube* Texture = (ZETextureCube*)TextureResource->GetTexture();
	if (Texture == NULL)
		GetDevice()->SetTexture(Stage, NULL);

	ZED3D9CommonTools::SetTexture(Stage, Texture, Filter, MipMappingFilter, Addressing);
}

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
			zeError("Can not vertex compile shader. Shader file name : \"%s\"", FileName);
		}
		else
		{
			zeError("Can not compile vertex shader.\r\nShader file name : \"%s\".\r\nCompile output :\r\n%s\r\n", FileName, CompilerOutput->GetBufferPointer());
		}

		*VertexShader = NULL;
		return false;
	}

	if (GetDevice()->CreateVertexShader((DWORD*)ShaderBuffer->GetBufferPointer(), VertexShader) != NULL)
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

	if (GetDevice()->CreatePixelShader((DWORD*)ShaderBuffer->GetBufferPointer(), PixelShader) != NULL)
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

	if (GetDevice()->CreateVertexShader((DWORD*)ShaderBuffer->GetBufferPointer(), VertexShader) != NULL)
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

	if (GetDevice()->CreatePixelShader((DWORD*)ShaderBuffer->GetBufferPointer(), PixelShader) != NULL)
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

/*
bool ZED3D9CommonTools::CreateRenderTarget(LPDIRECT3DTEXTURE9* Target, ZEInt Width, ZEInt Height, ZETexturePixelFormat Format)
{
	D3DSURFACE_DESC SurDesc;
	D3DFORMAT D3DFormat = ConvertPixelFormat(Format);
	if (*Target != NULL)
	{
		(*Target)->GetLevelDesc(0, &SurDesc);
		
		if (SurDesc.Width != Width ||SurDesc.Height != Height || SurDesc.Format != D3DFormat || SurDesc.Usage != D3DUSAGE_RENDERTARGET)
			(*Target)->Release();
		else
			return true;
	}

	HRESULT Hr = GetDevice()->CreateTexture(Width, Height, 1, D3DUSAGE_RENDERTARGET, D3DFormat, D3DPOOL_DEFAULT, Target, NULL);
	if (Hr != D3D_OK)
	{
		zeError("Can not create render target.");
		*Target = NULL;
		return false;
	}

	return true;
}

bool ZED3D9CommonTools::CreateRenderTarget(LPDIRECT3DSURFACE9* Target, ZEInt Width, ZEInt Height, ZETexturePixelFormat Format)
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

	HRESULT Hr = GetDevice()->CreateRenderTarget(Width, Height, D3DFormat, D3DMULTISAMPLE_NONE, 0, FALSE, Target, NULL);
	if (Hr != D3D_OK)
	{
		zeError("Can not create render target.");
		*Target = NULL;
		return false;
	}

	return true;
}


bool ZED3D9CommonTools::CreateDepthRenderTarget(LPDIRECT3DSURFACE9* Target, ZEInt Width, ZEInt Height)
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

	HRESULT Hr = GetDevice()->CreateDepthStencilSurface(Width, Height, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, FALSE, Target, NULL);
	if (Hr != D3D_OK)
	{
		zeError("Can not create depth render target.");
		*Target = NULL;
		return false;
	}

	return true;
}

bool ZED3D9CommonTools::CreateDepthRenderTarget(LPDIRECT3DTEXTURE9* Target, ZEInt Width, ZEInt Height)
{
	D3DSURFACE_DESC SurDesc;
	if (*Target != NULL)
	{
		(*Target)->GetLevelDesc(0, &SurDesc);
		if (SurDesc.Width != Width ||SurDesc.Height != Height || SurDesc.Format != D3DFMT_D24S8)
			(*Target)->Release();
		else
			return true;
	}

	HRESULT Hr = GetDevice()->CreateTexture(Width, Height, 0, D3DUSAGE_DEPTHSTENCIL ,D3DFMT_D24S8, D3DPOOL_DEFAULT, Target, NULL);
	if (Hr != D3D_OK)
	{
		zeError("Can not create depth render target.");
		*Target = NULL;
		return false;
	}

	return true;
}
*/
