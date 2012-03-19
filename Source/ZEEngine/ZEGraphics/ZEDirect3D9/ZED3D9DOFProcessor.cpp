//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9DOFProcessor.cpp
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
#include "ZED3D9CommonTools.h"
#include "ZED3D9DOFProcessor.h"
#include "ZED3D9BlurProcessor.h"
#include "ZED3D9FrameRenderer.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEGraphics/ZETexture2DResource.h"

#include <d3d9.h>
#include <stdlib.h>
#include "ZEGame/ZEScene.h"

void ZED3D9DOFProcessor::CreateBlurKernels()
{
	unsigned int TargetWidth;
	unsigned int TargetHeight;
	unsigned int SourceWidth;
	unsigned int SourceHeight;

	if (HorizontalKernel != NULL)
		delete HorizontalKernel;
	if (VerticalKernel != NULL)
		delete VerticalKernel;

	TargetWidth = SourceWidth = Renderer->GetViewPort()->GetWidth() / 2;
	TargetHeight = SourceHeight = Renderer->GetViewPort()->GetHeight() / 2;

	GaussianFilter GaussFilter(3.5f, 1.0f);
	ZEVector2 SourcePixelSize(1.0f / (float)TargetWidth, 1.0f / (float)TargetHeight);

	HorizontalKernel = new ZED3D9BlurKernel((const ZEFilter*)&GaussFilter, SourceWidth, TargetWidth, 32, SourcePixelSize.x);
	VerticalKernel = new ZED3D9BlurKernel((const ZEFilter*)&GaussFilter, SourceHeight, TargetHeight, 32, SourcePixelSize.y);
}

void ZED3D9DOFProcessor::CreateRenderTargets()
{
	unsigned int TargetWidth = Renderer->GetViewPort()->GetWidth() / 2;
	unsigned int TargetHeight = Renderer->GetViewPort()->GetHeight() / 2;

	// If ColorBufferDS2x created before
	if (ColorBufferDS2xBlur != NULL)
	{
		// If dimensions needs to change
		if (TargetWidth != ColorBufferDS2xBlur->GetWidth() || TargetHeight != ColorBufferDS2xBlur->GetHeight())
		{
			ColorBufferDS2xBlur->Release();
			ColorBufferDS2xBlur->Create(TargetWidth, TargetHeight, 1, ZE_TPF_I8_4, true);
		}
	}
	else // If ColorBufferDS2x not created before
	{
		ColorBufferDS2xBlur = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		ColorBufferDS2xBlur->Create(TargetWidth, TargetHeight, 1, ZE_TPF_I8_4, true);
	}

	// If DepthBufferDS2x created before
	if (ColorBufferDS2xTmp != NULL)
	{
		// If dimensions needs to change
		if (TargetWidth != ColorBufferDS2xTmp->GetWidth() || TargetHeight != ColorBufferDS2xTmp->GetHeight())
		{
			ColorBufferDS2xTmp->Release();
			ColorBufferDS2xTmp->Create(TargetWidth, TargetHeight, 1, ZE_TPF_I8_4, true);
		}
	}
	else // If DepthBufferDS2x not created before
	{
		ColorBufferDS2xTmp = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		ColorBufferDS2xTmp->Create(TargetWidth, TargetHeight, 1, ZE_TPF_I8_4, true);
	}
}

float ZED3D9DOFProcessor::GetFocusDistance()
{
	return FocusDistance;
}

void ZED3D9DOFProcessor::SetFocusDistance(float Value)
{
	FocusDistance = Value;
}

float ZED3D9DOFProcessor::GetFarDistance()
{
	return FarDistance;
}

void ZED3D9DOFProcessor::SetFarDistance(float Value)
{
	FarDistance = Value;
}

float ZED3D9DOFProcessor::GetNearDistance()
{
	return NearDistance;
}

void ZED3D9DOFProcessor::SetNearDistance(float Value)
{
	NearDistance = Value;
}

float ZED3D9DOFProcessor::GetFarClamp()
{
	return FarClamp;
}

void ZED3D9DOFProcessor::SetFarClamp(float Value)
{
	FarClamp = Value;
}

float ZED3D9DOFProcessor::GetNearClamp()
{
	return NearClamp;
}

void ZED3D9DOFProcessor::SetNearClamp(float Value)
{
	NearClamp = Value;
}

void ZED3D9DOFProcessor::SetRenderer(ZEFrameRenderer* Renderer)
{
	this->Renderer = (ZED3D9FrameRenderer*)Renderer;
}

ZEFrameRenderer* ZED3D9DOFProcessor::GetRenderer()
{
	return Renderer;
}

void ZED3D9DOFProcessor::SetInputDepth(ZETexture2D* Texture)
{
	this->InputDepthBuffer = (ZED3D9Texture2D*)Texture;
}

ZETexture2D* ZED3D9DOFProcessor::GetInputDepth()
{
	return InputDepthBuffer;
}

void ZED3D9DOFProcessor::SetInputColor(ZETexture2D* Texture)
{
	this->InputColorBuffer = (ZED3D9Texture2D*)Texture;
}

ZETexture2D* ZED3D9DOFProcessor::GetInputColor()
{
	return InputColorBuffer;
}

void ZED3D9DOFProcessor::SetOutput(ZED3D9ViewPort* Texture)
{
	OutputBuffer = Texture;
}

ZED3D9ViewPort* ZED3D9DOFProcessor::GetOutput()
{
	return OutputBuffer;
}

void ZED3D9DOFProcessor::Initialize()
{
	// Vertex declaration for screen aligned quad
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	// Compile Shaders
	this->VertexShaderCommon = ZED3D9VertexShader::CreateShader("DOFProcessor.hlsl", "vs_main_common", 0, "vs_3_0");

	this->PixelShaderDOF = ZED3D9PixelShader::CreateShader("DOFProcessor.hlsl", "ps_main_dof", 0, "ps_3_0");
	this->PixelShaderDS2x = ZED3D9PixelShader::CreateShader("DOFProcessor.hlsl", "ps_main_ds2x", 0, "ps_3_0");
	this->PixelShaderBlurH = ZED3D9PixelShader::CreateShader("DOFProcessor.hlsl", "ps_main_gauss_blur_horizontal", 0, "ps_3_0");
	this->PixelShaderBlurV = ZED3D9PixelShader::CreateShader("DOFProcessor.hlsl", "ps_main_gauss_blur_vertical", 0, "ps_3_0");

	this->CreateBlurKernels();
	this->CreateRenderTargets();
}

void ZED3D9DOFProcessor::Deinitialize()
{	
	Renderer			= NULL;

	InputColorBuffer	= NULL;
	InputDepthBuffer	= NULL;
	OutputBuffer		= NULL;

	if (ColorBufferDS2xBlur != NULL)
	{
		ColorBufferDS2xBlur->Destroy();
		ColorBufferDS2xBlur = NULL;
	}

	if (ColorBufferDS2xTmp != NULL)
	{
		ColorBufferDS2xTmp->Destroy();
		ColorBufferDS2xTmp = NULL;
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

	ZED3D_RELEASE(VertexShaderCommon);

	ZED3D_RELEASE(PixelShaderDOF);
	ZED3D_RELEASE(PixelShaderDS2x);
	ZED3D_RELEASE(PixelShaderBlurH);
	ZED3D_RELEASE(PixelShaderBlurV);
}

void ZED3D9DOFProcessor::OnDeviceLost()
{
	// Empty
}

void ZED3D9DOFProcessor::OnDeviceRestored()
{
	CreateRenderTargets();
	CreateBlurKernels();
}

void ZED3D9DOFProcessor::Process()
{
	zeProfilerStart("DOF Pass");

	ZEVector4* KernelData;
	unsigned int Vector4Count;

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

	GetDevice()->SetVertexDeclaration(VertexDeclaration);

	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GetDevice()->SetVertexShader(VertexShaderCommon->GetVertexShader());

	// Downsample the color buffer to 1/4 size
	GetDevice()->SetPixelShader(PixelShaderDS2x->GetPixelShader());

	ZED3D9CommonTools::SetRenderTarget(0, ColorBufferDS2xBlur);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)InputColorBuffer, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / InputColorBuffer->GetWidth(), 1.0f / InputColorBuffer->GetHeight(), 0.0f, 0.0f), 1);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	// Horizontal Blur Pass
	GetDevice()->SetPixelShader(PixelShaderBlurH->GetPixelShader());

	ZED3D9CommonTools::SetRenderTarget(0, ColorBufferDS2xTmp);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)ColorBufferDS2xBlur, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

	KernelData = HorizontalKernel->GetKernel();
	Vector4Count = HorizontalKernel->GetKernelWindowSize();
	GetDevice()->SetPixelShaderConstantF(0, (const float*)KernelData, Vector4Count);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / ColorBufferDS2xBlur->GetWidth(), 1.0f / ColorBufferDS2xBlur->GetHeight(), 0.0f, 0.0f), 1);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	// Vertical Blur Pass
	GetDevice()->SetPixelShader(PixelShaderBlurV->GetPixelShader());

	ZED3D9CommonTools::SetRenderTarget(0, ColorBufferDS2xBlur);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)ColorBufferDS2xTmp, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

	KernelData = VerticalKernel->GetKernel();
	Vector4Count = VerticalKernel->GetKernelWindowSize();
	GetDevice()->SetPixelShaderConstantF(0, (const float*)KernelData, Vector4Count);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / ColorBufferDS2xTmp->GetWidth(), 1.0f / ColorBufferDS2xTmp->GetHeight(), 0.0f, 0.0f), 1);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	// Horizontal Blur Pass
	GetDevice()->SetPixelShader(PixelShaderBlurH->GetPixelShader());

	ZED3D9CommonTools::SetRenderTarget(0, ColorBufferDS2xTmp);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)ColorBufferDS2xBlur, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

	KernelData = HorizontalKernel->GetKernel();
	Vector4Count = HorizontalKernel->GetKernelWindowSize();
	GetDevice()->SetPixelShaderConstantF(0, (const float*)KernelData, Vector4Count);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / ColorBufferDS2xBlur->GetWidth(), 1.0f / ColorBufferDS2xBlur->GetHeight(), 0.0f, 0.0f), 1);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	// Vertical Blur Pass
	GetDevice()->SetPixelShader(PixelShaderBlurV->GetPixelShader());

	ZED3D9CommonTools::SetRenderTarget(0, ColorBufferDS2xBlur);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)ColorBufferDS2xTmp, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

	KernelData = VerticalKernel->GetKernel();
	Vector4Count = VerticalKernel->GetKernelWindowSize();
	GetDevice()->SetPixelShaderConstantF(0, (const float*)KernelData, Vector4Count);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / ColorBufferDS2xTmp->GetWidth(), 1.0f / ColorBufferDS2xTmp->GetHeight(), 0.0f, 0.0f), 1);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));


	// Horizontal Blur Pass
	GetDevice()->SetPixelShader(PixelShaderBlurH->GetPixelShader());

	ZED3D9CommonTools::SetRenderTarget(0, ColorBufferDS2xTmp);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)ColorBufferDS2xBlur, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

	KernelData = HorizontalKernel->GetKernel();
	Vector4Count = HorizontalKernel->GetKernelWindowSize();
	GetDevice()->SetPixelShaderConstantF(0, (const float*)KernelData, Vector4Count);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / ColorBufferDS2xBlur->GetWidth(), 1.0f / ColorBufferDS2xBlur->GetHeight(), 0.0f, 0.0f), 1);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	// Vertical Blur Pass
	GetDevice()->SetPixelShader(PixelShaderBlurV->GetPixelShader());

	ZED3D9CommonTools::SetRenderTarget(0, ColorBufferDS2xBlur);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)ColorBufferDS2xTmp, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

	KernelData = VerticalKernel->GetKernel();
	Vector4Count = VerticalKernel->GetKernelWindowSize();
	GetDevice()->SetPixelShaderConstantF(0, (const float*)KernelData, Vector4Count);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / ColorBufferDS2xTmp->GetWidth(), 1.0f / ColorBufferDS2xTmp->GetHeight(), 0.0f, 0.0f), 1);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));


	// Switch to DOF shader
	GetDevice()->SetPixelShader(PixelShaderDOF->GetPixelShader());

	ZED3D9CommonTools::SetRenderTarget(0, (ZEViewPort*)OutputBuffer);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)InputColorBuffer, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(1, (ZETexture2D*)InputDepthBuffer, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(2, (ZETexture2D*)ColorBufferDS2xBlur, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTADDRESS_CLAMP);


	GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(FocusDistance, 0, 0.0f, 0.0f), 1);
	GetDevice()->SetPixelShaderConstantF(1, (const float*)&ZEVector4(NearDistance, 0, 0.0f, 0.0f), 1);
	GetDevice()->SetPixelShaderConstantF(2, (const float*)&ZEVector4(FarDistance, 0, 0.0f, 0.0f), 1);
	GetDevice()->SetPixelShaderConstantF(3, (const float*)&ZEVector4(NearClamp, 0, 0.0f, 0.0f), 1);
	GetDevice()->SetPixelShaderConstantF(4, (const float*)&ZEVector4(FarClamp, 0, 0.0f, 0.0f), 1);


	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / InputColorBuffer->GetWidth(), 1.0f / InputColorBuffer->GetHeight(), 0.0f, 0.0f), 1);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	zeProfilerEnd();
}

ZED3D9DOFProcessor::ZED3D9DOFProcessor()
{
	FocusDistance		= 20.0f;
	FarDistance			= 60.0f;
	NearDistance		= 3.0f;
	FarClamp			= 0.6f;
	NearClamp			= 0.8f;


	Renderer			= NULL;
	VertexDeclaration	= NULL;
	VertexShaderCommon	= NULL;
	PixelShaderDOF		= NULL;
	PixelShaderDOF		= NULL;
	PixelShaderDS2x		= NULL;
	PixelShaderBlurH	= NULL;
	PixelShaderBlurV	= NULL;
	ColorBufferDS2xBlur	= NULL;
	ColorBufferDS2xTmp	= NULL;

	HorizontalKernel	= NULL;
	VerticalKernel		= NULL;

	InputColorBuffer	= NULL;
	InputDepthBuffer	= NULL;
	OutputBuffer		= NULL;
}

ZED3D9DOFProcessor::~ZED3D9DOFProcessor()
{
	Deinitialize();
}
