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


void ZED3D9TextureResizer::SetRenderer(ZEFrameRenderer* Renderer)
{
	this->Renderer = (ZED3D9FrameRenderer*)Renderer;
}

ZEFrameRenderer* ZED3D9TextureResizer::GetRenderer()
{
	return Renderer;
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

void ZED3D9TextureResizer::Initialize(unsigned char* DestData, unsigned int DestPitch, unsigned int DestWidth, unsigned int DestHegiht,
									  unsigned char* SrcData, unsigned int SrcPitch, unsigned int SrcWidth, unsigned int SrcHeight)
{
	// Vertex declaration for screen aligned quad
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	// Create and compile shaders
	this->VertexShader = ZED3D9VertexShader::CreateShader("TextureResize.hlsl", "vs_main", 0, "vs_3_0");
	this->PixelShaderHorizontal = ZED3D9PixelShader::CreateShader("TextureResize.hlsl", "ps_main_horizontal", 0, "ps_3_0");
	this->PixelShaderVertical = ZED3D9PixelShader::CreateShader("TextureResize.hlsl", "ps_main_vertical", 0, "ps_3_0");

	// Set the parameters
	this->SrcInfo.Buffer = SrcData;
	this->SrcInfo.Pitch = SrcPitch;
	this->SrcInfo.Width = SrcWidth;
	this->SrcInfo.Height = SrcHeight;

	this->DestInfo.Buffer = DestData;
	this->DestInfo.Pitch = DestPitch;
	this->DestInfo.Width = DestWidth;
	this->DestInfo.Height = DestHegiht;

	this->AutoFitMode = ZE_D3D9_FPO2_AUTO;
	this->ResizeFilter = ZE_D3D9_RF_KAISER;
}

void ZED3D9TextureResizer::Deinitialize()
{
	// Clear the source and destination info
	memset(&SrcInfo, 0, sizeof(TextureInfo));
	memset(&DestInfo, 0, sizeof(TextureInfo));

	ZED3D_RELEASE(VertexShader);
	ZED3D_RELEASE(PixelShaderVertical);
	ZED3D_RELEASE(PixelShaderHorizontal);
	ZED3D_RELEASE(VertexDeclaration);

	Renderer = NULL;
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

	Filter*	Filt =  NULL;
	ZED3D9Texture2D* Input	= NULL;
	ZED3D9Texture2D* Output = NULL;
	IDirect3DSurface9* ReadBack = NULL;

	void* Dest = NULL;
	const unsigned int Bpp = 4;
	unsigned int DestPitch = 0; 
	
	static struct Vert  
	{
		float Position[3];
	} Vertices[] =
	{
		{-1.0f,  1.0f, 0.0f},
		{ 1.0f,  1.0f, 0.0f},
		{-1.0f, -1.0f, 0.0f},
		{ 1.0f, -1.0f, 0.0f}
	};


	switch(ResizeFilter)
	{
		default:
		case ZE_D3D9_RF_BOX:
		{
			Filt = (Filter*)new BoxFilter;
			break;
		}
		
		case ZE_D3D9_RF_TRIANGLE:
		{
			Filt = (Filter*)new TriangleFilter;
			break;
		}
		
		case ZE_D3D9_RF_CUBIC:
		{
			Filt = (Filter*)new CubicFilter;
			break;
		}
		
		case ZE_D3D9_RF_GAUSSIAN:
		{
			Filt = (Filter*)new GaussianFilter;
			break;
		}
		
		case ZE_D3D9_RF_QUADRATIC:
		{
			Filt = (Filter*)new QuadraticFilter;
			break;
		}
		
		case ZE_D3D9_RF_KAISER:
		{
			Filt = (Filter*)new KaiserFilter(5.0f);
			break;
		}
		
		case ZE_D3D9_RF_MITCHELL:
		{
			Filt = (Filter*)new MitchellFilter(1.0f / 3.0f, 1.0f / 3.0f);
			break;
		}
	}

	// Set common render states, vertex shader etc once
	Result = GetDevice()->SetVertexShader(VertexShader->GetVertexShader());
	Result = GetDevice()->SetVertexDeclaration(VertexDeclaration);
	Result = GetDevice()->SetDepthStencilSurface(NULL);

	Result = GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	Result = GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	Result = GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	Result = GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	Result = GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	Result = GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


	// If it is not power or two resize its width to NewWidth(power of two)
	if(!IsPowerOfTwo(SrcInfo.Width))
	{
		unsigned int NewWidth = GetPowerOfTwo(SrcInfo.Width);
		PolyPhaseKernel	KernHorizResize(Filt, SrcInfo.Width, NewWidth, 32);
		
		Input = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Input->Create(SrcInfo.Width, SrcInfo.Height, ZE_TPF_RGBA_INT32, false);
		Output = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Output->Create(NewWidth, SrcInfo.Height, ZE_TPF_RGBA_INT32, true);

		// Fill the input texture
		Input->Lock(&Dest, &DestPitch, 0);
		// Copy source data to input texture
		for(size_t I = 0; I < SrcInfo.Height; I++)
			memcpy((unsigned char*)Dest + I * DestPitch, SrcInfo.Buffer + I * SrcInfo.Pitch, SrcInfo.Width * Bpp);
		Input->Unlock(0);
		Dest = NULL;
		DestPitch = 0;

		
		// Set Output Texture
		ZED3D9CommonTools::SetRenderTarget(0, Output);
		// Set Input Texture
		ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)Input, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

		// Set pixel size = (1 / new width, 1 / source height)
		Result = GetDevice()->SetVertexShaderConstantF(8, (const float*)&ZEVector4(1.0f / SrcInfo.Width, 1.0f / SrcInfo.Height, 0.0f, 0.0f), 1);
		Result = GetDevice()->SetPixelShaderConstantF(8, (const float*)&ZEVector4(1.0f / SrcInfo.Width, 1.0f / SrcInfo.Height, 0.0f, 0.0f), 1);

		// Pass the kernel and other variables to shaders
		unsigned int Vector4Count = (unsigned int)ceilf(KernHorizResize.GetKernelWindowSize() / 4.0f); 
		Result = GetDevice()->SetPixelShaderConstantF(9, (const float*)&ZEVector4((float)KernHorizResize.GetKernelWindowSize(), KernHorizResize.GetKernelWidth(), KernHorizResize.GetKernelCenter(), 0.0f),1);
		Result = GetDevice()->SetPixelShaderConstantF(100, (const float*)KernHorizResize.GetKernel(),Vector4Count);

		// Resize
		GetDevice()->BeginScene();
		Result = GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
		GetDevice()->EndScene();
		
		// Set new width
		SrcInfo.Width = NewWidth;

		ZED3D_RELEASE(Input);
	}

	// if it is not power of two, resize its Height to NewHeight(power of two)
	if(!IsPowerOfTwo(SrcInfo.Height))
	{
		// Create a new Texture which have Height of NewHeight
		unsigned int NewHeight = GetPowerOfTwo(SrcInfo.Height);
		PolyPhaseKernel	KernVertResize(Filt, SrcInfo.Height, NewHeight, 32);
		
		// Create input and output textures
		if(Output && Input == NULL) // if it is already resized horizontally
		{
			Input = Output;
			Output = NULL;
		}
		else
		{
			Input = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
			Input->Create(SrcInfo.Width, SrcInfo.Height, ZE_TPF_RGBA_INT32, false);
			
			// Fill the input texture
			Input->Lock(&Dest, &DestPitch, 0);
			for(size_t I = 0; I < SrcInfo.Height; I++)
				memcpy((unsigned char*)Dest + I * DestPitch, SrcInfo.Buffer + I * SrcInfo.Pitch, SrcInfo.Width * Bpp);
			Input->Unlock(0);
			Dest = NULL;
			DestPitch = 0;

		}
		Output = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Output->Create(SrcInfo.Width, NewHeight, ZE_TPF_RGBA_INT32, true);

		// Set Pixel shader
		Result = GetDevice()->SetPixelShader(PixelShaderVertical->GetPixelShader());

		// Set Output Texture
		ZED3D9CommonTools::SetRenderTarget(0, Output);
		// Set Input Texture
		ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)Input, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

		// Set pixel size = (1 / new width, 1 / source height)
		Result = GetDevice()->SetVertexShaderConstantF(8, (const float*)&ZEVector4(1.0f / SrcInfo.Width, 1.0f / SrcInfo.Height, 0.0f, 0.0f), 1);
		Result = GetDevice()->SetPixelShaderConstantF(8, (const float*)&ZEVector4(1.0f / SrcInfo.Width, 1.0f / SrcInfo.Height, 0.0f, 0.0f), 1);

		// Pass the kernel and other variables to shaders
		unsigned int Vector4Count = (unsigned int)ceilf(KernVertResize.GetKernelWindowSize() / 4.0f); 
		Result = GetDevice()->SetPixelShaderConstantF(9, (const float*)&ZEVector4((float)KernVertResize.GetKernelWindowSize(), KernVertResize.GetKernelWidth(), KernVertResize.GetKernelCenter(), 0.0f),1);
		Result = GetDevice()->SetPixelShaderConstantF(100, (const float*)KernVertResize.GetKernel(),Vector4Count);

		// Resize
		GetDevice()->BeginScene();
		Result = GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
		GetDevice()->EndScene();

		// Set new height
		SrcInfo.Height = NewHeight;

		// Release the unnecessary texture
		ZED3D_RELEASE(Input);
	}

	
	// Create kernels for final resize
	PolyPhaseKernel*	HorizontalPassKernel = NULL;
	PolyPhaseKernel*	VerticalPassKernel = NULL;

	if(SrcInfo.Width != DestInfo.Width)
		HorizontalPassKernel = new PolyPhaseKernel(Filt, SrcInfo.Width, DestInfo.Width, 32);
	

	if(SrcInfo.Height != DestInfo.Height)
		VerticalPassKernel = new PolyPhaseKernel(Filt, SrcInfo.Height, DestInfo.Height, 32);


	if(HorizontalPassKernel)
	{
		// If temp is set(resized previously)
		if(Output && Input == NULL)
		{
			Input = Output;
			Output = NULL;
		}
		// if none happened create the input texture
		else
		{
			// Create the input texture and copy the image data into it
			Input = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
			Input->Create(SrcInfo.Width, SrcInfo.Height, ZE_TPF_RGBA_INT32, false);

			// Fill the input texture
			Input->Lock(&Dest, &DestPitch, 0);
			for(size_t I = 0; I < SrcInfo.Height; I++)
				memcpy((unsigned char*)Dest + I * DestPitch, SrcInfo.Buffer + I * SrcInfo.Pitch, SrcInfo.Width * Bpp);
			Input->Unlock(0);
			Dest = NULL;
			DestPitch = 0;
		}
	
		// Create output texture
		Output = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Output->Create(DestInfo.Width, SrcInfo.Height, ZE_TPF_RGBA_INT32, true);


		// Set render states, vs, ps
		Result = GetDevice()->SetPixelShader(PixelShaderHorizontal->GetPixelShader());

		// Set Output Texture
		ZED3D9CommonTools::SetRenderTarget(0, Output);
		// Set Input Texture
		ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)Input, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

		// Set pixel size
		Result = GetDevice()->SetVertexShaderConstantF(8, (const float*)&ZEVector4(1.0f / SrcInfo.Width, 1.0f / SrcInfo.Height, 0.0f, 0.0f), 1);
		Result = GetDevice()->SetPixelShaderConstantF(8, (const float*)&ZEVector4(1.0f / SrcInfo.Width, 1.0f / SrcInfo.Height, 0.0f, 0.0f), 1);

		// Pass the kernel and other variables to shaders
		unsigned int Vector4Count = (unsigned int)ceilf(HorizontalPassKernel->GetKernelWindowSize() / 4.0f); 
		Result = GetDevice()->SetPixelShaderConstantF(9, (const float*)&ZEVector4((float)HorizontalPassKernel->GetKernelWindowSize(), HorizontalPassKernel->GetKernelWidth(), HorizontalPassKernel->GetKernelCenter(), 0.0f),1);
		Result = GetDevice()->SetPixelShaderConstantF(100, (const float*)HorizontalPassKernel->GetKernel(),Vector4Count);

		// Resize Horizontally
		GetDevice()->BeginScene();
		Result = GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
		GetDevice()->EndScene();
		

		///////////////////////////  TEST  ////////////////////////////////

		if(Output)
		{
			// Create a temp texture to read the render target data from graphics device
			Result = GetDevice()->CreateOffscreenPlainSurface(DestInfo.Width, SrcInfo.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &ReadBack, NULL);
			Result = GetDevice()->GetRenderTargetData(((ZED3D9ViewPort*)Output->GetViewPort())->FrameBuffer, ReadBack);

			// Copy resized Texture back to destination buffer
			D3DLOCKED_RECT Rect;
			Result = ReadBack->LockRect(&Rect, NULL, NULL);

			for(size_t I = 0; I < SrcInfo.Height; I++)
				memcpy(DestInfo.Buffer + I * DestInfo.Pitch, (unsigned char*)Rect.pBits + I * Rect.Pitch, DestInfo.Width * Bpp);

			Result = ReadBack->UnlockRect();
		}

		return true;

		/////////////////////////////////////////////////////////////////////

	
		// Clean up the unnecessary textures
		ZED3D_RELEASE(Input);
	}
	
	
	
	// Vertical resize pass
	if(VerticalPassKernel)
	{
		if(Output && Input == NULL)
		{
			Input = Output;
			Output = NULL;
		}
		else
		{
			// Create the input texture and copy the image data into it
			Input = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
			Input->Create(SrcInfo.Width, SrcInfo.Height, ZE_TPF_RGBA_INT32, false);

			// Fill the input texture
			Input->Lock(&Dest, &DestPitch, 0);
			for(size_t I = 0; I < SrcInfo.Height; I++)
				memcpy((unsigned char*)Dest + I * DestPitch, SrcInfo.Buffer + I * SrcInfo.Pitch, SrcInfo.Width * Bpp);
			Input->Unlock(0);
			Dest = NULL;
			DestPitch = 0;
		}

		
		// Create output texture
		Output = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Output->Create(DestInfo.Width, DestInfo.Height, ZE_TPF_RGBA_INT32, true);

		// Set pixel shader
		Result = GetDevice()->SetPixelShader(PixelShaderVertical->GetPixelShader());

		// Set Output Texture
		ZED3D9CommonTools::SetRenderTarget(0, Output);
		// Set Input Texture
		ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)Input, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

		// Set pixel size = (1 / new width, 1 / source height)
		Result = GetDevice()->SetVertexShaderConstantF(8, (const float*)&ZEVector4(1.0f / DestInfo.Width, 1.0f / SrcInfo.Height, 0.0f, 0.0f), 1);
		Result = GetDevice()->SetPixelShaderConstantF(8, (const float*)&ZEVector4(1.0f / DestInfo.Width, 1.0f / SrcInfo.Height, 0.0f, 0.0f), 1);

		// Pass the kernel and other variables to shaders
		unsigned int Vector4Count = (unsigned int)ceilf(VerticalPassKernel->GetKernelWindowSize() / 4.0f); 
		Result = GetDevice()->SetPixelShaderConstantF(9, (const float*)&ZEVector4((float)VerticalPassKernel->GetKernelWindowSize(), VerticalPassKernel->GetKernelWidth(), VerticalPassKernel->GetKernelCenter(), 0.0f),1);
		Result = GetDevice()->SetPixelShaderConstantF(100, (const float*)VerticalPassKernel->GetKernel(),Vector4Count);

		// Resize Vertically
		GetDevice()->BeginScene();
		Result = GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
		GetDevice()->EndScene();

		
		// Clean up the unnecessary textures
		ZED3D_RELEASE(Input);
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
			memcpy(DestInfo.Buffer + I * DestInfo.Pitch, (unsigned char*)Rect.pBits + I * Rect.Pitch, DestInfo.Width * Bpp);

		Result = ReadBack->UnlockRect();
	}

	// Final Cleanup
	ZED3D_RELEASE(Input);
	ZED3D_RELEASE(Output);
	ZED3D_RELEASE(ReadBack);
	
	if(Filt)
	{
		delete Filt;
		Filt = NULL;
	}
	if(HorizontalPassKernel)
	{
		delete HorizontalPassKernel;
		HorizontalPassKernel = NULL;
	}
	if(VerticalPassKernel)
	{
		delete VerticalPassKernel;
		VerticalPassKernel = NULL;
	}
	
	return true;
}

ZED3D9TextureResizer::ZED3D9TextureResizer()
{
	Renderer				= NULL;
	VertexDeclaration		= NULL;
	VertexShader			= NULL;
	PixelShaderHorizontal	= NULL;
	PixelShaderVertical		= NULL;

	memset(&SrcInfo, 0, sizeof(TextureInfo));
	memset(&DestInfo, 0, sizeof(TextureInfo));
}
ZED3D9TextureResizer::~ZED3D9TextureResizer()
{
	Deinitialize();
}
