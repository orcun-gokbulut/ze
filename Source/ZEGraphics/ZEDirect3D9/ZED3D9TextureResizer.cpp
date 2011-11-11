//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9TextureResizer.cpp
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

#include "ZED3D9TextureResizer.h"

#include "ZEGraphics/ZEDirect3D9/ZED3D9ComponentBase.h"
#include "ZEGraphics/ZEDirect3D9/ZED3D9Module.h"
#include "ZEGraphics/ZEGraphicsModule.h"

#include "ZED3D9Common.h"
#include "ZED3D9CommonTools.h"
#include "ZEGraphics/ZETexture2DResource.h"
#include "ZED3D9FrameRenderer.h"
#include "ZED3D9ViewPort.h"
#include "ZED3D9Texture2D.h"
#include "ZEGraphics/ZECamera.h"
#include "ZED3D9Profiler.h"
#include "ZED3D9Shader.h"
#include "ZED3D9TextureResizeFilters.h"

#include <d3d9.h>
#include <stdlib.h>


#include "ZECore/ZEConsole.h"


bool ZED3D9TextureResizer::IsPowerOfTwo(unsigned int Value)
{
	return ((Value & (Value - 1)) != 0)  ? false : true;
}

unsigned int ZED3D9TextureResizer::NextPowerOfTwo(unsigned int Value)
{
	if(Value <= 1)
		return 1;

	Value = (Value >> 1)  | Value;
	Value = (Value >> 2)  | Value;
	Value = (Value >> 4)  | Value;
	Value = (Value >> 8)  | Value;
	Value = (Value >> 16) | Value;

	return ((Value << 1) + 1) - Value;
}

unsigned int ZED3D9TextureResizer::PreviousPowerOfTwo(unsigned int Value)
{
	if(Value <= 1)
		return 1;
	
	Value = (Value >> 1)  | Value;
	Value = (Value >> 2)  | Value;
	Value = (Value >> 4)  | Value;
	Value = (Value >> 8)  | Value;
	Value = (Value >> 16) | Value;

	return Value - (Value >> 1);
}

unsigned int ZED3D9TextureResizer::GetPowerOfTwo(unsigned int Value)
{
	unsigned int Next;
	unsigned int Previous;

	switch(AutoFitMode)
	{
		default:
		// Calculate the closest power of two
		case ZE_D3D9_FPO2_AUTO:
			Next = NextPowerOfTwo(Value);
			Previous = PreviousPowerOfTwo(Value);
			if(Value > (Next + Previous) / 2)
				return Next;
			else
				return Previous;
			break;

		// Calculate next power of two
		case ZE_D3D9_FPO2_NEXTPO2:
			Next = NextPowerOfTwo(Value);
			return Next;
			break;

		// Calculate previous power of two
		case ZE_D3D9_FPO2_PREVIOUSPO2:
			Previous = PreviousPowerOfTwo(Value);
			return Previous;
			break;
	}
}

void ZED3D9TextureResizer::SetResizeFilter(ZED3D9TextureResizeFilter ResizeFilter)
{
	this->ResizeFilter = ResizeFilter;
}

ZED3D9TextureResizeFilter ZED3D9TextureResizer::GetResizeFilter()
{
	return ResizeFilter;
}

void ZED3D9TextureResizer::SetAutoFitMode(ZED3D9FittingPowerof2Mode AutoFitMode)
{
	this->AutoFitMode = AutoFitMode;
}

ZED3D9FittingPowerof2Mode ZED3D9TextureResizer::GetAutoFitMode()
{
	return AutoFitMode;
}

void ZED3D9TextureResizer::Initialize(void* DestData, unsigned int DestPitch, unsigned int DestWidth, unsigned int DestHegiht,
									  void* SrcData, unsigned int SrcPitch, unsigned int SrcWidth, unsigned int SrcHeight)
{
	// Set the parameters
	this->SrcInfo.Buffer = SrcData;
	this->SrcInfo.Pitch = SrcPitch;
	this->SrcInfo.Width = SrcWidth;
	this->SrcInfo.Height = SrcHeight;

	this->DestInfo.Buffer = DestData;
	this->DestInfo.Pitch = DestPitch;
	this->DestInfo.Width = DestWidth;
	this->DestInfo.Height = DestHegiht;

	
}

void ZED3D9TextureResizer::Deinitialize()
{
	// Clear the source and destination info
	memset(&SrcInfo, 0, sizeof(TextureInfo));
	memset(&DestInfo, 0, sizeof(TextureInfo));

}

void ZED3D9TextureResizer::OnDeviceLost()
{
	// Empty
}
void ZED3D9TextureResizer::OnDeviceRestored()
{
	// Empty
}



bool ZED3D9TextureResizer::Process()
{
	HRESULT Result;

	ZEFilter*	Filt =  NULL;
	ZED3D9Texture2D* Input	= NULL;
	ZED3D9Texture2D* Output = NULL;
	IDirect3DSurface9* ReadBack = NULL;
	IDirect3DSurface9* DepthStencil = NULL;	

	void* Dest = NULL;
	const unsigned int Bpp = 4;
	unsigned int DestPitch = 0; 
	
	static struct Vert  
	{
		float Position[3];
		float Texcoord[2];
	} 
	Vertices[] =
	{
		{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}},
		{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}},
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
		{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}
	};


	// There is only one filter for now
	/*switch(ResizeFilter)
	{
		default:
		case ZE_D3D9_RF_BOX:
		{
			Filt = (ZEFilter*)new BoxFilter;
			break;
		}
		
		case ZE_D3D9_RF_TRIANGLE:
		{
			Filt = (ZEFilter*)new TriangleFilter;
			break;
		}
		
		case ZE_D3D9_RF_CUBIC:
		{
			Filt = (ZEFilter*)new CubicFilter;
			break;
		}
		
		case ZE_D3D9_RF_GAUSSIAN:
		{
			Filt = (ZEFilter*)new GaussianFilter;
			break;
		}
		
		case ZE_D3D9_RF_QUADRATIC:
		{
			Filt = (ZEFilter*)new QuadraticFilter;
			break;
		}
		
		case ZE_D3D9_RF_KAISER:
		{
			Filt = (ZEFilter*)new KaiserFilter(5.0f);
			break;
		}
		
		case ZE_D3D9_RF_MITCHELL:
		{
			Filt = (ZEFilter*)new MitchellFilter(1.0f / 3.0f, 1.0f / 3.0f);
			break;
		}
	}*/

	// There is only one resize filter for now
	KaiserFilter Filter(5.0f);
	Filt = (ZEFilter*)&Filter;


	if(SrcInfo.Width != DestInfo.Width)
	{
		// Calculate Pixel size of the source texture
		const float SourcePixelWidth = 1.0f / (float)SrcInfo.Width;
		const float SourcePixelHeight = 1.0f / (float)SrcInfo.Height;

		const float DestPixelWidth = 1.0f / (float)DestInfo.Width;
		const float DestPixelHeight = 1.0f / (float)SrcInfo.Height;

		ZEKernel	HorizontalPassKernel(Filt, SrcInfo.Width, DestInfo.Width, 32, SourcePixelWidth);

		// Create the input texture and copy the image data into it
		Input = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Input->Create(SrcInfo.Width, SrcInfo.Height, ZE_TPF_A8R8G8B8, false, 1);

		// Fill the input texture
		Input->Lock(&Dest, &DestPitch, 0);
		for(size_t I = 0; I < SrcInfo.Height; I++)
			memcpy((unsigned char*)Dest + I * DestPitch, (unsigned char*)SrcInfo.Buffer + I * SrcInfo.Pitch, SrcInfo.Width * Bpp);
		Input->Unlock(0);

		Dest = NULL;
		DestPitch = 0;
		
		// Create output texture
		Output = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Output->Create(DestInfo.Width, SrcInfo.Height, ZE_TPF_A8R8G8B8, true, 1);

		// Create depth stencil buffer with the same size of the render target
		GetDevice()->CreateDepthStencilSurface(DestInfo.Width, SrcInfo.Height, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE, &DepthStencil, NULL);
		GetDevice()->SetDepthStencilSurface(DepthStencil);

		// Set vs, ps
		GetDevice()->SetVertexShader(VertexShader->GetVertexShader());
		GetDevice()->SetPixelShader(PixelShaderHorizontal->GetPixelShader());

		// Set Input Output textures
		ZED3D9CommonTools::SetRenderTarget(0, (ZETexture2D*)Output);
		ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)Input, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

		// Set render states
		GetDevice()->SetVertexDeclaration(VertexDeclaration);
		GetDevice()->SetRenderState(D3DRS_ZENABLE,			FALSE);
		GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE,		FALSE);
		GetDevice()->SetRenderState(D3DRS_STENCILENABLE,	FALSE);
		GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE,	FALSE);
		GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		GetDevice()->SetRenderState(D3DRS_CULLMODE,			D3DCULL_NONE);

		const int WindowSize = HorizontalPassKernel.GetKernelWindowSize();
		const ZEVector4* Kernel = HorizontalPassKernel.GetKernel();

		// pass the parameters to graphics device
		GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(DestPixelWidth, DestPixelHeight, 0.0f, 0.0f), 1);
		GetDevice()->SetPixelShaderConstantF(0, (const float*)Kernel, WindowSize);
		
		// Resize Horizontally
		GetDevice()->BeginScene();
		GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
		GetDevice()->EndScene();
		
		// Clean up the unnecessary textures
		ZED3D_RELEASE(Input);
		ZED3D_RELEASE(DepthStencil);
		GetDevice()->SetDepthStencilSurface(NULL);
	}

	// Vertical resize pass
	if(SrcInfo.Height != DestInfo.Height)
	{
		// calculate pixel size
		float SourcePixelWidth = 1.0f / (float)DestInfo.Width;
		float SourcePixelHeight = 1.0f / (float)SrcInfo.Height;

		float DestPixelWidth = 1.0f / (float)DestInfo.Width;
		float DestPixelHeight = 1.0f / (float)DestInfo.Height;

		ZEKernel	VerticalPassKernel(Filt, SrcInfo.Height, DestInfo.Height, 32, SourcePixelHeight);

		if(Output && Input == NULL)
		{
			Input = Output;
			Output = NULL;
		}
		else
		{
			// Create the input texture and copy the image data into it
			Input = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
			Input->Create(SrcInfo.Width, SrcInfo.Height, ZE_TPF_A8R8G8B8, false, 1);

			// Fill the input texture
			Input->Lock(&Dest, &DestPitch, 0);
			for(size_t I = 0; I < SrcInfo.Height; I++)
				memcpy((unsigned char*)Dest + I * DestPitch, (unsigned char*)SrcInfo.Buffer + I * SrcInfo.Pitch, SrcInfo.Width * Bpp);
			Input->Unlock(0);
			
			Dest = NULL;
			DestPitch = 0;
		}

		// Create output texture
		Output = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Output->Create(DestInfo.Width, DestInfo.Height, ZE_TPF_A8R8G8B8, true, 1);

		// Set pixel shader
		Result = GetDevice()->SetVertexShader(VertexShader->GetVertexShader());
		Result = GetDevice()->SetPixelShader(PixelShaderVertical->GetPixelShader());

		// Create depth stencil buffer with the same size of the render target
		Result = GetDevice()->CreateDepthStencilSurface(DestInfo.Width, DestInfo.Height, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE, &DepthStencil, NULL);
		Result = GetDevice()->SetDepthStencilSurface(DepthStencil);

		// Set Input Output textures
		ZED3D9CommonTools::SetRenderTarget(0, (ZETexture2D*)Output);
		ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)Input, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

		// Set render states
		Result = GetDevice()->SetVertexDeclaration(VertexDeclaration);
		Result = GetDevice()->SetRenderState(D3DRS_ZENABLE,				FALSE);
		Result = GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE,		FALSE);
		Result = GetDevice()->SetRenderState(D3DRS_STENCILENABLE,		FALSE);
		Result = GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE,		FALSE);
		Result = GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE,	FALSE);
		Result = GetDevice()->SetRenderState(D3DRS_CULLMODE,			D3DCULL_NONE);

		const int WindowSize = VerticalPassKernel.GetKernelWindowSize();
		const ZEVector4* Kernel = VerticalPassKernel.GetKernel();

		// pass the parameters to graphics device
		GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(DestPixelWidth, DestPixelHeight, 0.0f, 0.0f), 1);
		GetDevice()->SetPixelShaderConstantF(0, (const float*)Kernel, WindowSize);
		
		// Resize Vertically
		Result = GetDevice()->BeginScene();
		Result = GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
		Result = GetDevice()->EndScene();

		// Clean up the unnecessary textures
		ZED3D_RELEASE(Input);
		ZED3D_RELEASE(DepthStencil);
		GetDevice()->SetDepthStencilSurface(NULL);
	}

	
	if(Output)
	{
		// Create a temp texture to read the render target data from graphics device
		Result = GetDevice()->CreateOffscreenPlainSurface(DestInfo.Width, DestInfo.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &ReadBack, NULL);
		Result = GetDevice()->GetRenderTargetData(((ZED3D9ViewPort*)Output->GetViewPort())->FrameBuffer, ReadBack);

		// Copy resized Texture back to destination buffer
		D3DLOCKED_RECT Rect;
		Result = ReadBack->LockRect(&Rect, NULL, NULL);

		for(size_t I = 0; I < DestInfo.Height; I++)
			memcpy((unsigned char*)DestInfo.Buffer + I * DestInfo.Pitch, (unsigned char*)Rect.pBits + I * Rect.Pitch, DestInfo.Width * Bpp);

		Result = ReadBack->UnlockRect();

		ZED3D_RELEASE(Output);
		ZED3D_RELEASE(ReadBack);
	}

	// Restore the original depth stencil surface
	GetDevice()->SetDepthStencilSurface(((ZED3D9ViewPort*)(GetModule()->GetFrameBufferViewPort()))->ZBuffer);
	return true;
}

ZED3D9TextureResizer::ZED3D9TextureResizer()
{
	VertexDeclaration		= NULL;
	VertexShader			= NULL;
	PixelShaderHorizontal	= NULL;
	PixelShaderVertical		= NULL;

	memset(&SrcInfo, 0, sizeof(TextureInfo));
	memset(&DestInfo, 0, sizeof(TextureInfo));

	// Vertex declaration for screen aligned quad
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0,  12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	// Create and compile shaders
	this->VertexShader = ZED3D9VertexShader::CreateShader("TextureResizeProcessor.hlsl", "vs_main_generic", 0, "vs_3_0");
	this->PixelShaderHorizontal = ZED3D9PixelShader::CreateShader("TextureResizeProcessor.hlsl", "ps_main_horizontal", 0, "ps_3_0");
	this->PixelShaderVertical = ZED3D9PixelShader::CreateShader("TextureResizeProcessor.hlsl", "ps_main_vertical", 0, "ps_3_0");

	this->AutoFitMode = ZE_D3D9_FPO2_AUTO;
	this->ResizeFilter = ZE_D3D9_RF_KAISER;
}
ZED3D9TextureResizer::~ZED3D9TextureResizer()
{
	ZED3D_RELEASE(VertexShader);
	ZED3D_RELEASE(PixelShaderVertical);
	ZED3D_RELEASE(PixelShaderHorizontal);
	ZED3D_RELEASE(VertexDeclaration);
}
