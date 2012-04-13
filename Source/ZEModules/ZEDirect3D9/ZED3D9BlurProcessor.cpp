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
#include "ZEMath/ZEMath.h"

ZED3D9BlurKernel::ZED3D9BlurKernel(const ZEFilter* Filt, ZEUInt SrcLength, ZEUInt DestLength, ZEInt Samples, float PixelSize)
{
	float Scale = (float)DestLength / (float)SrcLength;
	float InverseScale = 1.0f / Scale;

	// Upsampling Case
	if (Scale > 1) 
	{
		Samples = 1;
		Scale = 1;
	}

	KernelWidth = Filt->GetFilterWidth() * InverseScale;
	KernelWindowSize = (ZESize)ceilf(KernelWidth * 2.0f);
	// allocation is fixed since we pass it to graphics device
	KernelWeights = new ZEVector4[(ZESize)KernelWindowSize];
	memset(KernelWeights, 0, sizeof(ZEVector4) * (ZESize)KernelWindowSize);

	Center = float(KernelWindowSize) / 2.0f;

	float PixelCoord = -1.0f * (Center - 0.5f);
	float Total = 0.0f;
	for(ZESize I = 0; I < KernelWindowSize; I++)
	{
		const float Sample = Filt->SampleBox((float)I - Center, Scale, Samples);
		KernelWeights[I] = ZEVector4(Sample, PixelCoord * PixelSize, 0.0f, 0.0f);
		Total += Sample;
		PixelCoord += 1.0f;
	}

	float InverseTotal = 1.0f / Total;
	// Normalize the weight of the WindowSize
	for(ZESize I = 0; I < KernelWindowSize; I++)
	{
		KernelWeights[I].x *= InverseTotal;
	}
}

void ZED3D9BlurProcessor::CreateKernels()
{
	ZEUInt TargetWidth;
	ZEUInt TargetHeight;
	ZEUInt SourceWidth;
	ZEUInt SourceHeight;

	if (HorizontalKernel != NULL)
		delete HorizontalKernel;
	if (VerticalKernel != NULL)
		delete VerticalKernel;
	
	TargetWidth = SourceWidth = Renderer->GetViewPort()->GetWidth() / 8;
	TargetHeight = SourceHeight = Renderer->GetViewPort()->GetHeight() / 8;

	GaussianFilter GaussFilter(3.5f, 1.0f);
	ZEVector2 SourcePixelSize(1.0f / (float)TargetWidth, 1.0f / (float)TargetHeight);

	HorizontalKernel = new ZED3D9BlurKernel((const ZEFilter*)&GaussFilter, SourceWidth, TargetWidth, 32, SourcePixelSize.x);
	VerticalKernel = new ZED3D9BlurKernel((const ZEFilter*)&GaussFilter, SourceHeight, TargetHeight, 32, SourcePixelSize.y);

}

void ZED3D9BlurProcessor::CreateTextures()
{
	// The function only creates the textures and kernels when
	// the Input/Output(viewport) texture size changes.
	// Such as the resize of application window.
	
	ZEUInt TargetWidth = Renderer->GetViewPort()->GetWidth() / 8;
	ZEUInt TargetHeight = Renderer->GetViewPort()->GetHeight() / 8;
	
	// If created before
	if (TempTexture1 != NULL)
	{
		// If dimensions needs to change
		if (TargetWidth != TempTexture1->GetWidth() || TargetHeight != TempTexture1->GetHeight())
		{
			TempTexture1->Release();
			TempTexture1->Create(TargetWidth, TargetHeight, 1, ZE_TPF_I8_4, true);
			this->CreateKernels();
		}
	}
	else // If not created before
	{
		TempTexture1 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		TempTexture1->Create(TargetWidth, TargetHeight, 1, ZE_TPF_I8_4, true);
		this->CreateKernels();
	}

	// If created before
	if (TempTexture2 != NULL)
	{
		// If dimensions needs to change
		if (TargetWidth != TempTexture2->GetWidth() || TargetHeight != TempTexture2->GetHeight())
		{
			TempTexture2->Release();
			TempTexture2->Create(TargetWidth, TargetHeight, 1, ZE_TPF_I8_4, true);
			this->CreateKernels();
		}
	}
	else // If not created before
	{
		TempTexture2 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		TempTexture2->Create(TargetWidth, TargetHeight, 1, ZE_TPF_I8_4, true);
		this->CreateKernels();
	}
		
}

void ZED3D9BlurProcessor::SetBlurFactor(float BlurFactor)
{
	if (BlurFactor > 1.0f) BlurFactor = 1.0f;
	if (BlurFactor < 0.0f) BlurFactor = 0.0f;

	this->BlurFactor = BlurFactor;
}

float ZED3D9BlurProcessor::GetBlurFactor()
{
	return BlurFactor;
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
	this->InputBuffer	= (ZED3D9Texture2D*)Texture;

	this->CreateTextures();
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
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	// Create and compile shaders
	this->VertexShaderDownSample	= ZED3D9VertexShader::CreateShader("DownSample2x.hlsl", "vs_main", 0, "vs_3_0");
	this->PixelShaderDownSample		= ZED3D9PixelShader::CreateShader("DownSample2x.hlsl", "ps_main", 0, "ps_3_0");
	
	this->PixelShaderBlend			= ZED3D9PixelShader::CreateShader("BlurProcessor.hlsl", "ps_main_lerp", 0, "ps_3_0");
		
	this->VertexShaderBlur		= ZED3D9VertexShader::CreateShader("BlurProcessor.hlsl", "vs_main", 0, "vs_3_0");
	this->PixelShaderHorizontal = ZED3D9PixelShader::CreateShader("BlurProcessor.hlsl", "ps_main_horizontal", 0, "ps_3_0");
	this->PixelShaderVertical	= ZED3D9PixelShader::CreateShader("BlurProcessor.hlsl", "ps_main_vertical", 0, "ps_3_0");
	
	CreateTextures();
}

void ZED3D9BlurProcessor::Deinitialize()
{
	Renderer			= NULL;
	InputBuffer			= NULL;
	OutputBuffer		= NULL;

	if (TempTexture1)
	{
		TempTexture1->Release();
		TempTexture1->Destroy();
		TempTexture1 = NULL;
	}
	if (TempTexture2)
	{
		TempTexture2->Release();
		TempTexture2->Destroy();
		TempTexture2 = NULL;
	}
	if (HorizontalKernel)
	{
		delete HorizontalKernel;
		HorizontalKernel = NULL;
	}
	if (VerticalKernel)
	{
		delete VerticalKernel;
		VerticalKernel = NULL;
	}

	ZED3D_RELEASE(PixelShaderBlend);
	ZED3D_RELEASE(VertexShaderBlur);
	ZED3D_RELEASE(VertexDeclaration);
	ZED3D_RELEASE(PixelShaderVertical);
	ZED3D_RELEASE(PixelShaderHorizontal);
	ZED3D_RELEASE(PixelShaderDownSample);
	ZED3D_RELEASE(VertexShaderDownSample);

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

	ZEVector4* KernelData;
	ZESize Vector4Count;
	
	static struct Vertex
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

	// Set shader and vertex declaration
	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	
	// Set render states and output
	GetDevice()->SetRenderState(D3DRS_CULLMODE,			D3DCULL_NONE);
	GetDevice()->SetRenderState(D3DRS_ZENABLE,			FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE,		FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE,	FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE,	FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	
	/************************************************************************/
	/*                           DownSample 2x Pass                         */
	/************************************************************************/
	
	// Set shaders
	GetDevice()->SetPixelShader(PixelShaderDownSample->GetPixelShader());
	GetDevice()->SetVertexShader(VertexShaderDownSample->GetVertexShader());
	
	// Send shader data to pipeline
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / (float)TempTexture1->GetWidth(), 1.0f / (float)TempTexture1->GetHeight(), 0.0f, 0.0f), 1);

	// Set input and output textures
	ZED3D9CommonTools::SetRenderTarget(0, (ZETexture2D*)TempTexture1);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)InputBuffer, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP);

	// Draw
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(Vertex));

	/************************************************************************/
	/*                         Horizontal Blur Pass                         */
	/************************************************************************/

	// Set shader
	GetDevice()->SetVertexShader(VertexShaderBlur->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderHorizontal->GetPixelShader());

	// Send shader data to pipeline
	KernelData = HorizontalKernel->GetKernel();
	Vector4Count = HorizontalKernel->GetKernelWindowSize();
	GetDevice()->SetPixelShaderConstantF(0, (const float*)KernelData, (UINT)Vector4Count);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / (float)TempTexture2->GetWidth(), 1.0f / (float)TempTexture2->GetHeight(), 0.0f, 0.0f), 1);
	
	// Set input and output textures
	ZED3D9CommonTools::SetRenderTarget(0, (ZETexture2D*)TempTexture2);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)TempTexture1, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);

	// Draw
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(Vertex));

	/************************************************************************/
	/*                     Vertical Blur Pass		                        */
	/************************************************************************/
	
	// Set shaders
	GetDevice()->SetVertexShader(VertexShaderBlur->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderVertical->GetPixelShader());

	// Send shader data to pipeline
	KernelData = VerticalKernel->GetKernel();
	Vector4Count = VerticalKernel->GetKernelWindowSize();
	GetDevice()->SetPixelShaderConstantF(0, (const float*)KernelData, (UINT)Vector4Count);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / (float)TempTexture1->GetWidth(), 1.0f / (float)TempTexture1->GetHeight(), 0.0f, 0.0f), 1);
	
	// Set input and output textures
	ZED3D9CommonTools::SetRenderTarget(0, (ZETexture2D*)TempTexture1);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)TempTexture2, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);
	
	// Draw
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(Vertex));

	/************************************************************************/
	/*                         Horizontal Blur Pass                         */
	/************************************************************************/

	// Set shader
	GetDevice()->SetVertexShader(VertexShaderBlur->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderHorizontal->GetPixelShader());

	// Send shader data to pipeline
	KernelData = HorizontalKernel->GetKernel();
	Vector4Count = HorizontalKernel->GetKernelWindowSize();
	GetDevice()->SetPixelShaderConstantF(0, (const float*)KernelData, (UINT)Vector4Count);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / (float)TempTexture2->GetWidth(), 1.0f / (float)TempTexture2->GetHeight(), 0.0f, 0.0f), 1);

	// Set input and output textures
	ZED3D9CommonTools::SetRenderTarget(0, (ZETexture2D*)TempTexture2);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)TempTexture1, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);

	// Draw
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(Vertex));

	/************************************************************************/
	/*							Vertical Blur Pass			                */
	/************************************************************************/

	// Set pixel shader
	GetDevice()->SetVertexShader(VertexShaderBlur->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderVertical->GetPixelShader());

	// Send shader data to pipeline
	KernelData = VerticalKernel->GetKernel();
	Vector4Count = VerticalKernel->GetKernelWindowSize();
	GetDevice()->SetPixelShaderConstantF(0, (const float*)KernelData, (UINT)Vector4Count);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / (float)TempTexture1->GetWidth(), 1.0f / (float)TempTexture1->GetHeight(), 0.0f, 0.0f), 1);

	// Set input and output textures
	ZED3D9CommonTools::SetRenderTarget(0, (ZETexture2D*)TempTexture1);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)TempTexture2, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);

	// Draw
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(Vertex));

	/************************************************************************/
	/*                     Color Blend(Lerp) Pass	                        */
	/************************************************************************/

	// Set pixel shader
	GetDevice()->SetVertexShader(VertexShaderBlur->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderBlend->GetPixelShader());

	// Send shader data to pipeline
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(BlurFactor, 0.0f, 0.0f, 0.0f), 1);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / (float)OutputBuffer->GetWidth(), 1.0f / (float)OutputBuffer->GetHeight(), 0.0f, 0.0f), 1);

	// Set input and output textures
	ZED3D9CommonTools::SetRenderTarget(0, (ZEViewPort*)OutputBuffer);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)InputBuffer, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(1, (ZETexture2D*)TempTexture1, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP);

	// Draw
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(Vertex));

	zeProfilerEnd();
}

ZED3D9BlurProcessor::ZED3D9BlurProcessor()
{
	BlurFactor				= 0.0f;
	Renderer				= NULL;
	InputBuffer				= NULL;
	OutputBuffer			= NULL;
	TempTexture1			= NULL;
	TempTexture2			= NULL;
	VertexDeclaration		= NULL;
	PixelShaderBlend		= NULL;
	VertexShaderDownSample	= NULL;
	PixelShaderDownSample	= NULL;
	PixelShaderHorizontal	= NULL;
	PixelShaderVertical		= NULL;
	VertexShaderBlur		= NULL;
	HorizontalKernel		= NULL;
	VerticalKernel			= NULL;
}

ZED3D9BlurProcessor::~ZED3D9BlurProcessor()
{
	Deinitialize();
}
