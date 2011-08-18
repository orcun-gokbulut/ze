//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERealTimeTextureCompressor.cpp
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

#include "ZECore\ZEFile.h"
#include "ZECore\ZECore.h"
#include "ZECore\ZEError.h"
#include "ZERealTimeTextureCompressor.h"
#include "ZEDirect3D9\ZED3D9CommonTools.h"
#include "ZEDirect3D9\ZED3D9Module.h"

#include	<d3d9.h>
#include	<d3dx9.h>
#include	<DxErr.h>
#include	<string>

LPD3DXCONSTANTTABLE		VSConstTable;
LPD3DXCONSTANTTABLE		DXT1PSConstTable;
LPD3DXCONSTANTTABLE		DXT5PSConstTable;

LPDIRECT3DVERTEXSHADER9	VertexShader;
LPDIRECT3DPIXELSHADER9	DXT1PixelShader;
LPDIRECT3DPIXELSHADER9	DXT5PixelShader;

LPDIRECT3DTEXTURE9		InputTexture;
//IDirect3DTexture9*		OutputTexture;
LPDIRECT3DTEXTURE9		RenderTexture;
LPDIRECT3DSURFACE9		RenderTarget;

ZERealTimeTextureCompressor::ZERealTimeTextureCompressor()
{
	
}

ZERealTimeTextureCompressor::~ZERealTimeTextureCompressor()
{

}

bool ZERealTimeTextureCompressor::Compress(void* DestBuffer, unsigned int DestPitch, D3DFORMAT DestFormat, void* SrcBuffer, unsigned int SrcWidth, unsigned int SrcHeight, unsigned int SrcPitch, unsigned int SrcBitsPP, D3DFORMAT SrcFormat)
{
	HRESULT Result;

	// Create Input texture
	Result = GetDevice()->CreateTexture(SrcWidth, SrcHeight, 1, NULL, SrcFormat, D3DPOOL_MANAGED, &InputTexture, NULL);
	if (Result != D3D_OK)
	{
		zeError("RealTime Compressor", "Cannot create texture...");
		return false;
	}

	// Write source data to InputTexture
	D3DLOCKED_RECT Rect;
	InputTexture->LockRect(0, &Rect, NULL, NULL);

	for(unsigned int I = 0; I < SrcHeight; I++)
		memcpy((unsigned char*)Rect.pBits + I * Rect.Pitch, (unsigned char*)SrcBuffer + I * SrcPitch, SrcWidth * SrcBitsPP / 8);

	InputTexture->UnlockRect(0);

	// Create lockable Render Target
	Result = GetDevice()->CreateTexture(SrcWidth, SrcHeight, 1, D3DUSAGE_RENDERTARGET, DestFormat, D3DPOOL_MANAGED, &RenderTexture, NULL);
	//Result = Device->CreateRenderTarget(SrcWidth, SrcHeight, DestFormat, D3DMULTISAMPLE_NONE, 0, true, &RenderTarget, NULL);
	if (Result != D3D_OK)
	{
		zeError("RealTime Compressor", "Cannot create Render Target...");
		return false;
	}

	const D3DXVECTOR4	ImageSize((float)SrcWidth, (float)SrcHeight, 0.0f, 0.0f);

	// Bind the vertex shader
	GetDevice()->SetVertexShader(VertexShader);
	// Bind the pixel shader
	// Set block size
	unsigned int BlockSize = 0;
	// Set shader parameters
	D3DXHANDLE Handle;
	switch(DestFormat)
	{
		case D3DFMT_DXT1:
			BlockSize = 8;
			GetDevice()->SetPixelShader(DXT1PixelShader);
			if(Handle = DXT1PSConstTable->GetConstantByName(NULL, "imageSize"))
				DXT1PSConstTable->SetVector(GetDevice(), Handle, &ImageSize);
			break;

		case D3DFMT_DXT5:
			BlockSize = 16;
			GetDevice()->SetPixelShader(DXT5PixelShader);
			if(Handle = DXT5PSConstTable->GetConstantByName(NULL, "imageSize"))
				DXT5PSConstTable->SetVector(GetDevice(), Handle, &ImageSize);
			break;

		default:
			zeError("RealTime Compressor", "Unknown destination format...");
			return false;
			break;
	}
	
	RenderTexture->GetSurfaceLevel(0, &RenderTarget);

	// Set render target
	GetDevice()->SetRenderTarget(0, RenderTarget);

	// Set texture
	GetDevice()->SetTexture(0, InputTexture);
	//Set sampler states
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP);
	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER,	D3DTEXF_NONE);
	
	GetDevice()->SetRenderState(D3DRS_CULLMODE,			D3DCULL_CCW); // Set culling mode counter clock wise
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE,	FALSE); // Set alpha blending
	GetDevice()->SetRenderState(D3DRS_ZENABLE,			FALSE ); // Set z buffer of
	
	// Create the quad
	struct
	{
		float Position[3];
		float UV[2];
	} 
	ScreenAlignedQuad[] = {
		{{-1.0f + SrcWidth, 1.0f +  SrcHeight, 0.0f},  {0.0f, 0.0f}},
		{{ 1.0f + SrcWidth, 1.0f +  SrcHeight, 0.0f},  {1.0f, 0.0f}},
		{{ 1.0f + SrcWidth, -1.0f + SrcHeight, 0.0f},  {1.0f, 1.0f}},
		{{-1.0f + SrcWidth, -1.0f + SrcHeight, 0.0f},  {0.0f, 1.0f}}};

	// Draw primitive
	GetDevice()->BeginScene();
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, ScreenAlignedQuad, 5 * 4);
	GetDevice()->EndScene();

	// Copy compressed texture from render target to destination buffer
	RenderTexture->LockRect(0, &Rect, NULL, D3DLOCK_READONLY);
	
	unsigned int RowCopySize = SrcWidth / 4 * BlockSize;
	for(unsigned int I = 0; I < SrcHeight / 4; I++)
		memcpy((unsigned char*)DestBuffer + I * DestPitch, (unsigned char*)Rect.pBits + I * Rect.Pitch,  RowCopySize);

	RenderTexture->UnlockRect(0);


	// Release the surfaces
	if (InputTexture)
	{
		InputTexture->Release();
		InputTexture = NULL;
	}
	/*if (OutputTexture)
	{
		OutputTexture->Release();
		OutputTexture = NULL;
	}*/
	if(RenderTarget)
	{
		RenderTarget->Release();
		RenderTarget = NULL;
	}
}

void ZERealTimeTextureCompressor::Downsample()
{
	
}

bool ZERealTimeTextureCompressor::Initialize(const char* CompressionShaderPath, const char* DownSampleShaderPath)
{
	// Load the shader into the shader buffer
	/*unsigned int ShaderFileSize = ZEFile::GetFileSize("resources\\shaders\\compress_YCoCgDXT.cg") + 1;
	char* ShaderBuffer = (char*)malloc(ShaderFileSize);
	ZEFile::ReadTextFile("shaders\\compress_YCoCgDXT.cg",ShaderBuffer, ShaderFileSize);

	DWORD Flags = 0; 
	Flags |= D3DXSHADER_DEBUG;
	char			ErrorString[2048];
	LPD3DXBUFFER	Error = NULL;
	const char*		VSProfile = "vs_4_0";
	const char*		PSProfile = "ps_4_0";
	LPD3DXBUFFER	CompiledShader = NULL;
	const char*		VertexShaderEntry = "compress_vp";
	const char*		DXT1PixelShaderEntry = "compress_DXT1_fp";
	const char*		DXT5PixelShaderEntry = "compress_YCoCgDXT5_fp";
	

	// Compile the vertex shader
	HRESULT Result = D3DXCompileShader(ShaderBuffer, ShaderFileSize, NULL, NULL, VertexShaderEntry, VSProfile, Flags, &CompiledShader, &Error, &VSConstTable);
	if(Result != D3D_OK)
	{
		OutputDebugString( Error ? (char*)Error->GetBufferPointer() : "" );
		zeError("RealTime Compressor", "Cannot Compile vertex shader..");
		return false;
	}
	// Create vertex shader
	Result = GetDevice()->CreateVertexShader((DWORD*)CompiledShader->GetBufferPointer(), &VertexShader);
	if(Result != D3D_OK)
	{
		zeError("RealTime Compressor", "Cannot create vertex shader..");
		return false;
	}
	Error->Release();
	CompiledShader->Release();
	CompiledShader = NULL;
	

	// Compile the dxt1 pixel shader
	Result = D3DXCompileShader(ShaderBuffer, ShaderFileSize, NULL, NULL, DXT1PixelShaderEntry, PSProfile, D3DXSHADER_OPTIMIZATION_LEVEL3, &CompiledShader, &Error, &DXT1PSConstTable);
	if(Result != D3D_OK)
	{
		OutputDebugString( Error ? (char*)Error->GetBufferPointer() : "" );
		zeError("RealTime Compressor", "Cannot Compile DXT1 Pixel Shader..");
		return false;
	}
	// Create dxt1 pixel shader
	Result = GetDevice()->CreatePixelShader((DWORD*)CompiledShader->GetBufferPointer(), &DXT1PixelShader);
	if(Result != D3D_OK)
	{
		zeError("RealTime Compressor", "Cannot create DXT1 Pixel shader..");
		return false;
	}
	Error->Release();
	CompiledShader->Release();
	CompiledShader = NULL;

	// Compile the dxt5 pixel shader
	Result = D3DXCompileShader(ShaderBuffer, ShaderFileSize, NULL, NULL, DXT5PixelShaderEntry, PSProfile, D3DXSHADER_OPTIMIZATION_LEVEL3, &CompiledShader, &Error, &DXT5PSConstTable);
	if(Result != D3D_OK)
	{
		OutputDebugString( Error ? (char*)Error->GetBufferPointer() : "" );
		zeError("RealTime Compressor", "Cannot Compile DXT5 Pixel Shader..");
		return false;
	}
	// Create dxt5 pixel shader
	Result = GetDevice()->CreatePixelShader((DWORD*)CompiledShader->GetBufferPointer(), &DXT5PixelShader);
	if(Result != D3D_OK)
	{
		zeError("RealTime Compressor", "Cannot create DXT5 Pixel shader..");
		return false;
	}
	Error->Release();
	CompiledShader->Release();
	CompiledShader = NULL;*/
		
	return true;
}
		
bool ZERealTimeTextureCompressor::Deinitialize()
{
	if (VertexShader)
	{
		VertexShader->Release();
		VertexShader = NULL;
	}
	if (DXT1PixelShader)
	{
		DXT1PixelShader->Release();
		DXT1PixelShader = NULL;
	}
	if(DXT5PixelShader)
	{
		DXT5PixelShader->Release();
		DXT5PixelShader = NULL;
	}
	if (VSConstTable)
	{
		VSConstTable->Release();
		VSConstTable = NULL;
	}
	if (DXT1PSConstTable)
	{
		DXT1PSConstTable->Release();
		DXT1PSConstTable = NULL;
	}
	if (DXT5PSConstTable)
	{
		DXT5PSConstTable->Release();
		DXT5PSConstTable =NULL;
	}
	
	return true;
}
