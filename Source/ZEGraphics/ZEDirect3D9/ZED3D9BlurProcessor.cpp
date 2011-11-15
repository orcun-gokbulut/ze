//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9BlurProcessor.cpp
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

#include "ZED3D9Common.h"
#include "ZED3D9Shader.h"
#include "ZED3D9ViewPort.h"
#include "ZED3D9Profiler.h"
#include "ZED3D9Texture2D.h"
#include "ZEMath/ZEMatrix.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9FrameRenderer.h"
#include "ZEGraphics/ZECamera.h"
#include "ZED3D9BlurProcessor.h"
#include "ZED3D9TextureResizeFilters.h"
#include "ZEGraphics/ZETexture2DResource.h"

#include <d3d9.h>
#include <stdlib.h>

void ZED3D9BlurProcessor::CreateTempTexture()
{
	// The function only changes the temp texture size when 
	// the Input texture size changes. Such as the resize of application window
	if (TempTexture != NULL)
	{
		if (InputBuffer->GetWidth() != TempTexture->GetWidth() || InputBuffer->GetHeight() != TempTexture->GetHeight())
		{
			TempTexture->Release();
			TempTexture->Create(InputBuffer->GetWidth(), InputBuffer->GetHeight(), ZE_TPF_A8R8G8B8, true, 1);
		}
	}
	else
	{
		TempTexture = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		TempTexture->Create(InputBuffer->GetWidth(), InputBuffer->GetHeight(), ZE_TPF_A8R8G8B8, true, 1);
	}
}

void ZED3D9BlurProcessor::CreateKernels()
{
	GaussianFilter GaussFilter(1.5f, BlurFactor);

	if (KernelVertical == NULL)
	{
		float SourcePixelHeight = 1.0f / InputBuffer->GetHeight();
		KernelVertical = new ZEKernel((const ZEFilter*)&GaussFilter, InputBuffer->GetHeight(), OutputBuffer->GetHeight(), 32, SourcePixelHeight);
	}

	if (KernelHorizontal == NULL)
	{
		float SourcePixelWidth = 1.0f / InputBuffer->GetWidth();
		KernelHorizontal = new ZEKernel((const ZEFilter*)&GaussFilter, InputBuffer->GetWidth(), OutputBuffer->GetWidth(), 32, SourcePixelWidth);
	}	
}

void ZED3D9BlurProcessor::SetBlurFactor(float BlurFactor)
{
	this->BlurFactor = BlurFactor;
	CreateKernels();
}

float ZED3D9BlurProcessor::GetBlurFactor()
{
	return BlurFactor;
}

ZEKernel* ZED3D9BlurProcessor::GetKernelVertical()
{
	return KernelVertical;
}

ZEKernel* ZED3D9BlurProcessor::GetKernelHorizontal()
{
	return KernelHorizontal;
}

void ZED3D9BlurProcessor::SetRenderer(ZEFrameRenderer* Renderer)
{
	this->Renderer = (ZED3D9FrameRenderer*)Renderer;
}

ZEFrameRenderer* ZED3D9BlurProcessor::GetRenderer()
{
	return Renderer;
}

void ZED3D9BlurProcessor::SetInput(ZETexture2D* Texture)
{
	this->InputBuffer = (ZED3D9Texture2D*)Texture;
}

ZETexture2D* ZED3D9BlurProcessor::GetInput()
{
	return InputBuffer;
}

void ZED3D9BlurProcessor::SetOutput(ZED3D9ViewPort* Texture)
{
	this->OutputBuffer = Texture;
}

ZED3D9ViewPort* ZED3D9BlurProcessor::GetOutput()
{
	return OutputBuffer;
}

void ZED3D9BlurProcessor::Initialize()
{
	// Vertex declaration for screen aligned quad
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	// Create and compile shaders
	this->VertexShader = ZED3D9VertexShader::CreateShader("BlurProcessor.hlsl", "vs_main", 0, "vs_3_0");
	this->PixelShaderHorizontal = ZED3D9PixelShader::CreateShader("BlurProcessor.hlsl", "ps_main_horizontal", 0, "ps_3_0");
	this->PixelShaderVertical = ZED3D9PixelShader::CreateShader("BlurProcessor.hlsl", "ps_main_vertical", 0, "ps_3_0");

}

void ZED3D9BlurProcessor::Deinitialize()
{
	if (KernelVertical)
		delete KernelVertical;
	if (KernelHorizontal)
		delete KernelHorizontal;
	if (TempTexture)
	{
		TempTexture->Release();
		TempTexture->Destroy();
	}

	BlurFactor			= 0.0f;
	Renderer			= NULL;
	InputBuffer			= NULL;
	OutputBuffer		= NULL;
	TempTexture			= NULL;
	KernelVertical		= NULL;
	KernelHorizontal	= NULL;

	ZED3D_RELEASE(VertexShader);
	ZED3D_RELEASE(VertexDeclaration);
	ZED3D_RELEASE(PixelShaderVertical);
	ZED3D_RELEASE(PixelShaderHorizontal);

}

void ZED3D9BlurProcessor::OnDeviceLost()
{
	// Empty
}

void ZED3D9BlurProcessor::OnDeviceRestored()
{
	// Empty
}

void ZED3D9BlurProcessor::Process()
{
	zeProfilerStart("Blur Pass");

	int WindowSize		= 0;
	ZEVector4* Kernel	= NULL;

	static struct Vert
	{
		float Position[3];
	} 
	Vertices[] =
	{
		{-1.0f,  1.0f, 0.0f},
		{ 1.0f,  1.0f, 0.0f},
		{-1.0f, -1.0f, 0.0f},
		{ 1.0f, -1.0f, 0.0f}
	};
	
	// Set shaders and vertex declaration
	GetDevice()->SetVertexShader(VertexShader->GetVertexShader());
	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	
	// Set render states and output
	GetDevice()->SetRenderState(D3DRS_CULLMODE,			D3DCULL_NONE);
	GetDevice()->SetRenderState(D3DRS_ZENABLE,			FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE,		FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE,	FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE,	FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	
	// Create temp texture and kernels
	this->CreateTempTexture();
	this->CreateKernels();

	// Horizontal Pass
	//Set pixel shader
	GetDevice()->SetPixelShader(PixelShaderHorizontal->GetPixelShader());

	// Send kernel, pixel size to pipeline
	Kernel		= KernelHorizontal->GetKernel();
	WindowSize	= KernelHorizontal->GetKernelWindowSize();

	GetDevice()->SetPixelShaderConstantF(0, (const float*)Kernel, WindowSize);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / InputBuffer->GetWidth(), 1.0f / InputBuffer->GetHeight(), 0.0f, 0.0f), 1);

	// Set input and output textures
	ZED3D9CommonTools::SetRenderTarget(0, (ZETexture2D*)TempTexture);
	ZED3D9CommonTools::SetTexture(6, (ZETexture2D*)InputBuffer, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

	// Draw
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	// Vertical Pass
	//Set pixel shader
	GetDevice()->SetPixelShader(PixelShaderVertical->GetPixelShader());

	// Send kernel, pixel size to pipeline
	Kernel		= KernelVertical->GetKernel();
	WindowSize	= KernelVertical->GetKernelWindowSize();

	GetDevice()->SetPixelShaderConstantF(0, (const float*)Kernel, WindowSize);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / InputBuffer->GetWidth(), 1.0f / InputBuffer->GetHeight(), 0.0f, 0.0f), 1);

	// Set input and output textures
	ZED3D9CommonTools::SetRenderTarget(0, OutputBuffer);
	ZED3D9CommonTools::SetTexture(6, (ZETexture2D*)TempTexture, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

	// Draw
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	zeProfilerEnd();
}

ZED3D9BlurProcessor::ZED3D9BlurProcessor()
{
	BlurFactor				= 0.0f;
	KernelVertical			= NULL;
	KernelHorizontal		= NULL;

	Renderer				= NULL;
	VertexDeclaration		= NULL;
	VertexShader			= NULL;
	PixelShaderHorizontal	= NULL;
	PixelShaderVertical		= NULL;

	TempTexture				= NULL;
	InputBuffer				= NULL;
	OutputBuffer			= NULL;

}

ZED3D9BlurProcessor::~ZED3D9BlurProcessor()
{
	Deinitialize();
}
