//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9MLAAProcessor.cpp
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
#include "ZEGame/ZEScene.h"
#include "ZED3D9ViewPort.h"
#include "ZED3D9Profiler.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9MLAAProcessor.h"
#include "ZED3D9FrameRenderer.h"
#include "ZEGraphics/ZECamera.h"
#include "ZED3D9MLAAProcessorAreaImage.h"
#include "ZETexture/ZETexture2DResource.h"

#include <d3d9.h>

void ZED3D9MLAAProcessor::SetTreshold(float Value)
{
	Treshold = Value;
}

float ZED3D9MLAAProcessor::GetTreshold()
{
	return Treshold;
}

void ZED3D9MLAAProcessor::SetSearchStep(float Value)
{
	SearchStep = Value;
}

float ZED3D9MLAAProcessor::GetSearchStep()
{
	return SearchStep;
}

void ZED3D9MLAAProcessor::SetRenderer(ZEFrameRenderer* Renderer)
{
	this->Renderer = (ZED3D9FrameRenderer*)Renderer;
}

ZEFrameRenderer* ZED3D9MLAAProcessor::GetRenderer()
{
	return (ZEFrameRenderer*)Renderer;
}

void ZED3D9MLAAProcessor::SetInputColor(ZETexture2D* Texture)
{
	InputColorBuffer = (ZED3D9Texture2D*)Texture;
}

ZETexture2D* ZED3D9MLAAProcessor::GetInputColor()
{
	return (ZETexture2D*)InputColorBuffer;
}

void ZED3D9MLAAProcessor::SetInputDepth(ZETexture2D* Texture)
{
	InputDepthBuffer = (ZED3D9Texture2D*)Texture;
}

ZETexture2D* ZED3D9MLAAProcessor::GetInputDepth()
{
	return (ZETexture2D*)InputDepthBuffer;
}

void ZED3D9MLAAProcessor::SetInputNormal(ZETexture2D* Texture)
{
	InputNormalBuffer = (ZED3D9Texture2D*)Texture;
}

ZETexture2D* ZED3D9MLAAProcessor::GetInputNormal()
{
	return (ZETexture2D*)InputNormalBuffer;
}

void ZED3D9MLAAProcessor::SetOutput(ZED3D9ViewPort* Texture)
{
	OutputBuffer = Texture;
}

ZED3D9ViewPort* ZED3D9MLAAProcessor::GetOutput()
{
	return OutputBuffer;	
}

void ZED3D9MLAAProcessor::CreateRenderTargets()
{
	bool Result = false;
	unsigned int TargetWidth = Renderer->GetViewPort()->GetWidth();
	unsigned int TargetHeight = Renderer->GetViewPort()->GetHeight();

	this->DestroyRenderTargets();
	
	EdgeBuffer = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Result = EdgeBuffer->Create(TargetWidth, TargetHeight, 1, ZE_TPF_I8_4, true);
	zeDebugCheck(!Result, "Cannot Create ZED3D9Texture2D: \"EdgeBuffer at ZED3D9MLAAProcessor\".");

	BlendWeightBuffer = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Result = BlendWeightBuffer->Create(TargetWidth, TargetHeight, 1, ZE_TPF_I8_4, true);
	zeDebugCheck(!Result, "Cannot Create ZED3D9Texture2D: \"BlendWeightBuffer at ZED3D9MLAAProcessor\".");
	
	AreaBuffer = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Result = AreaBuffer->Create(ZE_MLAA_AREA_IMAGE_WIDTH, ZE_MLAA_AREA_IMAGE_HEIGHT, 1, ZE_TPF_I8_4, false);
	zeDebugCheck(!Result, "Cannot Create ZED3D9Texture2D: \"AreaBuffer at ZED3D9MLAAProcessor\".");

	ZEUInt8* AreaImage = ZED3D9MLAAProcessorAreaImage::GetD3D9LMAAProcessorAreaImage();

	// Copy AreaImage into buffer
	ZESize Pitch = 0;
	void* TargetBuffer = NULL;
	AreaBuffer->Lock(&TargetBuffer, &Pitch, 0);

	for(ZESize I = 0; I < ZE_MLAA_AREA_IMAGE_HEIGHT; I++)
		memcpy((unsigned char*)TargetBuffer + Pitch * I, AreaImage + Pitch * I, Pitch);

	AreaBuffer->Unlock(0);
}

void ZED3D9MLAAProcessor::DestroyRenderTargets()
{
	if (EdgeBuffer != NULL)
	{
		EdgeBuffer->Destroy();
		EdgeBuffer = NULL;
	}

	if (BlendWeightBuffer != NULL)
	{
		BlendWeightBuffer->Destroy();
		BlendWeightBuffer = NULL;
	}

	if (AreaBuffer != NULL)
	{
		AreaBuffer->Destroy();
		AreaBuffer = NULL;
	}
	
}


void ZED3D9MLAAProcessor::Initialize()
{
	// Vertex declaration for screen aligned quad
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	// Compile Shaders
	this->VertexShaderCommon		= ZED3D9VertexShader::CreateShader("MLAAProcessor.hlsl", "vs_main_common", 0, "vs_3_0");
	this->PixelShaderEdgeDetection	= ZED3D9PixelShader::CreateShader("MLAAProcessor.hlsl", "ps_main_edge_detection_depth", 0, "ps_3_0");
	this->PixelShaderColorBlending	= ZED3D9PixelShader::CreateShader("MLAAProcessor.hlsl", "ps_main_color_blending", 0, "ps_3_0");
	this->PixelShaderWeightBlending = ZED3D9PixelShader::CreateShader("MLAAProcessor.hlsl", "ps_main_weight_blending", 0, "ps_3_0");
	
	this->CreateRenderTargets();
}

void ZED3D9MLAAProcessor::Deinitialize()
{
	Renderer			= NULL;
	OutputBuffer		= NULL;
	InputColorBuffer	= NULL;
	InputDepthBuffer	= NULL;
	InputNormalBuffer	= NULL;

	ZED3D_RELEASE(VertexDeclaration);
	ZED3D_RELEASE(VertexShaderCommon);
	ZED3D_RELEASE(PixelShaderEdgeDetection);
	ZED3D_RELEASE(PixelShaderWeightBlending);
	ZED3D_RELEASE(PixelShaderColorBlending);

	this->DestroyRenderTargets();
}

void ZED3D9MLAAProcessor::OnDeviceLost()
{
	this->Deinitialize();
}

void ZED3D9MLAAProcessor::OnDeviceRestored()
{
	this->Initialize();
}

void ZED3D9MLAAProcessor::Process()
{
	zeProfilerStart("MLAA Pass");

	static struct Vertex  
	{
		float Position[3];

	} Vertices[] = {
		{-1.0f,  1.0f, 1.0f},
		{ 1.0f,  1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f},
		{ 1.0f, -1.0f, 1.0f}
	};

	GetDevice()->SetVertexDeclaration(VertexDeclaration);

	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	float FarZ = ((ZED3D9FrameRenderer*)zeScene->GetRenderer())->GetCamera()->GetFarZ();
	ZEVector4 PixelSize(1.0f / InputDepthBuffer->GetWidth(), 1.0f / InputDepthBuffer->GetHeight(), 0.0f, 0.0f);

	// Clear Render Targets
	ZED3D9CommonTools::SetRenderTarget(0, (ZETexture2D*)EdgeBuffer);
	ZED3D9CommonTools::SetRenderTarget(1, (ZETexture2D*)BlendWeightBuffer);
	ZED3D9CommonTools::SetRenderTarget(2, (ZEViewPort*)OutputBuffer);
	GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET, 0x000000FF, 1.0f, 0x00);
	GetDevice()->SetRenderTarget(1, NULL);
	GetDevice()->SetRenderTarget(2, NULL);
	

	/************************************************************************/
	/*						Edge Detection Pass                             */
	/************************************************************************/
	GetDevice()->SetVertexShader(VertexShaderCommon->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderEdgeDetection->GetPixelShader());

	GetDevice()->SetVertexShaderConstantF(0, (const float*)&PixelSize, 1);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&PixelSize, 1);
	GetDevice()->SetPixelShaderConstantF(1, (const float*)&ZEVector4(Treshold, FarZ, 0.0f, 0.0f), 1);

	//ZED3D9CommonTools::SetRenderTarget(0, OutputBuffer);
	ZED3D9CommonTools::SetRenderTarget(0, (ZETexture2D*)EdgeBuffer);
	
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)InputDepthBuffer, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(1, (ZETexture2D*)InputNormalBuffer, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);
	
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vertex));

	/************************************************************************/
	/*                      Weight Blending Pass	                        */
	/************************************************************************/
	GetDevice()->SetVertexShader(VertexShaderCommon->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderWeightBlending->GetPixelShader());

	//ZED3D9CommonTools::SetRenderTarget(0, OutputBuffer);
	ZED3D9CommonTools::SetRenderTarget(0, (ZETexture2D*)BlendWeightBuffer);
	
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&PixelSize, 1);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&PixelSize, 1);

	ZED3D9CommonTools::SetTexture(4, (ZETexture2D*)EdgeBuffer, D3DTEXF_LINEAR, D3DTEXF_POINT, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(6, (ZETexture2D*)AreaBuffer, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vertex));
	
	/************************************************************************/
	/*                      Color Blending Pass	                            */
	/************************************************************************/
	GetDevice()->SetVertexShader(VertexShaderCommon->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderColorBlending->GetPixelShader());

	ZED3D9CommonTools::SetRenderTarget(0, (ZEViewPort*)OutputBuffer);

	GetDevice()->SetPixelShaderConstantF(0, (const float*)&PixelSize, 1);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&PixelSize, 1);

	ZED3D9CommonTools::SetTexture(3, (ZETexture2D*)InputColorBuffer, D3DTEXF_LINEAR, D3DTEXF_POINT, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(5, (ZETexture2D*)BlendWeightBuffer, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);
	
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vertex));
	
	zeProfilerEnd();

}

ZED3D9MLAAProcessor::ZED3D9MLAAProcessor()
{
	Treshold					= 0.05f;
	SearchStep					= 4.0f;
	OutputBuffer				= NULL;
	Renderer					= NULL;
	EdgeBuffer					= NULL;
	AreaBuffer					= NULL;
	VertexShaderCommon			= NULL;
	InputColorBuffer			= NULL;
	InputDepthBuffer			= NULL;
	InputNormalBuffer			= NULL;
	BlendWeightBuffer			= NULL;
	VertexDeclaration			= NULL;
	PixelShaderEdgeDetection	= NULL;
	PixelShaderWeightBlending	= NULL;
	PixelShaderColorBlending	= NULL;
	
}

ZED3D9MLAAProcessor::~ZED3D9MLAAProcessor()
{
	this->Deinitialize();
}
