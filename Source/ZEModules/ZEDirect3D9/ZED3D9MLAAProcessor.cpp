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

void ZED3D9MLAAProcessor::SetVisualizeEdges(bool Enabled)
{
	VisualizeEdges = Enabled;
}

bool ZED3D9MLAAProcessor::GetVisualizeEdges() const
{
	return VisualizeEdges;
}

void ZED3D9MLAAProcessor::SetVisualizeWeights(bool Enabled)
{
	VisualizeWeights = Enabled;
}

bool ZED3D9MLAAProcessor::GetVisualizeWeights() const
{
	return VisualizeWeights;
}

void ZED3D9MLAAProcessor::SetTreshold(float Value)
{
	Treshold = Value;
}

float ZED3D9MLAAProcessor::GetTreshold() const
{
	return Treshold;
}

void ZED3D9MLAAProcessor::SetSearchStep(float Value)
{
	SearchStep = Value;
}

float ZED3D9MLAAProcessor::GetSearchStep() const
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

void ZED3D9MLAAProcessor::UpdateBuffers()
{
	ZEUInt TargetWidth = Renderer->GetViewPort()->GetWidth();
	ZEUInt TargetHeight = Renderer->GetViewPort()->GetHeight();

	if (EdgeBuffer == NULL || TargetWidth != EdgeBuffer->GetWidth() || TargetHeight != EdgeBuffer->GetHeight())
	{
		ZED3D_DESTROY(EdgeBuffer);
	
		EdgeBuffer = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		EdgeBuffer->Create(TargetWidth, TargetHeight, 1, ZE_TPF_I8_4, true);
	}
	
	if (BlendWeightBuffer == NULL || TargetWidth != BlendWeightBuffer->GetWidth() || TargetHeight != BlendWeightBuffer->GetHeight())
	{
		ZED3D_DESTROY(BlendWeightBuffer);
	
		BlendWeightBuffer = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		BlendWeightBuffer->Create(TargetWidth, TargetHeight, 1, ZE_TPF_I8_4, true);
	}
	
	if (AreaBuffer == NULL)
	{	
		AreaBuffer = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		AreaBuffer->Create(ZE_MLAA_AREA_IMAGE_WIDTH, ZE_MLAA_AREA_IMAGE_WIDTH, 1, ZE_TPF_I8_4, false);

		ZEUInt8* AreaImage = ZED3D9MLAAProcessorAreaImage::GetD3D9LMAAProcessorAreaImage();

		// Copy AreaImage into buffer
		ZESize Pitch = 0;
		void* TargetBuffer = NULL;
		AreaBuffer->Lock(&TargetBuffer, &Pitch, 0);

		for(ZESize I = 0; I < ZE_MLAA_AREA_IMAGE_HEIGHT; I++)
			memcpy((unsigned char*)TargetBuffer + Pitch * I, AreaImage + Pitch * I, Pitch);

		AreaBuffer->Unlock(0);
	}
}

void ZED3D9MLAAProcessor::DestroyBuffers()
{
	ZED3D_DESTROY(EdgeBuffer);
	ZED3D_DESTROY(BlendWeightBuffer);
	ZED3D_DESTROY(AreaBuffer);	
}

void ZED3D9MLAAProcessor::EdgeDetectionPass(ZED3D9Texture2D* Depth, ZED3D9Texture2D* Normal, ZED3D9Texture2D* Output)
{
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	GetDevice()->SetVertexShader(VertexShaderCommon->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderEdgeDetection->GetPixelShader());

	ZEVector4 PixelSize(1.0f / Output->GetWidth(), 1.0f / Output->GetHeight(), 0.0f, 0.0f);

	GetDevice()->SetVertexShaderConstantF(0, PixelSize.M, 1);
	GetDevice()->SetPixelShaderConstantF(0, PixelSize.M, 1);
	GetDevice()->SetPixelShaderConstantF(1, ZEVector4(Treshold, 0.0f, 0.0f, 0.0f).M, 1);

	if (VisualizeEdges)
	{
		ZED3D9CommonTools::SetRenderTarget(0, OutputBuffer);
		GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0x00);
	}
	else
	{
		ZED3D9CommonTools::SetRenderTarget(0, Output);
		GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0x00);
	}

	ZED3D9CommonTools::SetTexture(0, Depth, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(1, Normal, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(3, InputColorBuffer, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(ZEMLAAScreenAlignedQuad));
}

void ZED3D9MLAAProcessor::WeightBlendingPass(ZED3D9Texture2D* Edges, ZED3D9Texture2D* AreaTexture, ZED3D9Texture2D* Output)
{
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	GetDevice()->SetVertexShader(VertexShaderCommon->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderWeightBlending->GetPixelShader());

	if (VisualizeWeights)
	{
		ZED3D9CommonTools::SetRenderTarget(0, OutputBuffer);
		GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0x00);
	}
	else
	{
		ZED3D9CommonTools::SetRenderTarget(0, Output);
		GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0x00);
	}


	ZEVector4 PixelSize(1.0f / Output->GetWidth(), 1.0f / Output->GetHeight(), 0.0f, 0.0f);
	GetDevice()->SetVertexShaderConstantF(0, PixelSize.M, 1);
	GetDevice()->SetPixelShaderConstantF(0, PixelSize.M, 1);

	ZED3D9CommonTools::SetTexture(4, Edges, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(6, AreaTexture, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(ZEMLAAScreenAlignedQuad));
}

void ZED3D9MLAAProcessor::ColorBlendingPass(ZED3D9Texture2D* Color, ZED3D9Texture2D* Weights, ZED3D9ViewPort* Output)
{
	if (VisualizeEdges || VisualizeWeights)
		return;

	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	GetDevice()->SetVertexShader(VertexShaderCommon->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderColorBlending->GetPixelShader());

	ZED3D9CommonTools::SetRenderTarget(0, Output);
	GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0x00);

	ZEVector4 PixelSize(1.0f / Output->GetWidth(), 1.0f / Output->GetHeight(), 0.0f, 0.0f);
	GetDevice()->SetVertexShaderConstantF(0, PixelSize.M, 1);
	GetDevice()->SetPixelShaderConstantF(0, PixelSize.M, 1);

	ZED3D9CommonTools::SetTexture(3, Color, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(5, Weights, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(ZEMLAAScreenAlignedQuad));
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
	this->PixelShaderEdgeDetection	= ZED3D9PixelShader::CreateShader("MLAAProcessor.hlsl", "ps_main_edge_detection_color", 0, "ps_3_0");
	this->PixelShaderColorBlending	= ZED3D9PixelShader::CreateShader("MLAAProcessor.hlsl", "ps_main_color_blending", 0, "ps_3_0");
	this->PixelShaderWeightBlending = ZED3D9PixelShader::CreateShader("MLAAProcessor.hlsl", "ps_main_weight_blending", 0, "ps_3_0");
	
	UpdateBuffers();
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

	DestroyBuffers();
}

void ZED3D9MLAAProcessor::OnDeviceLost()
{

}

void ZED3D9MLAAProcessor::OnDeviceRestored()
{

}

void ZED3D9MLAAProcessor::Process()
{
	zeProfilerStart("MLAA Pass");
	D3DPERF_BeginEvent(0, L"MLAA Pass");

	if (Renderer->GetCamera() == NULL)
		return;

	UpdateBuffers();

	EdgeDetectionPass(InputDepthBuffer, InputNormalBuffer, EdgeBuffer);
	WeightBlendingPass(EdgeBuffer, AreaBuffer, BlendWeightBuffer);
	ColorBlendingPass(InputColorBuffer, BlendWeightBuffer, OutputBuffer);
	
	zeProfilerEnd();
	D3DPERF_EndEvent();
}

ZEMLAAScreenAlignedQuad ZED3D9MLAAProcessor::Vertices[] =
{
	{-1.0f,  1.0f, 0.0f},
	{ 1.0f,  1.0f, 0.0f},
	{-1.0f, -1.0f, 0.0f},
	{ 1.0f, -1.0f, 0.0f}
};

ZED3D9MLAAProcessor::ZED3D9MLAAProcessor()
{
	VisualizeEdges				= false;
	VisualizeWeights			= false;

	Treshold					= 0.05f;
	SearchStep					= 4.0f;

	OutputBuffer				= NULL;
	InputColorBuffer			= NULL;
	InputDepthBuffer			= NULL;
	InputNormalBuffer			= NULL;
	Renderer					= NULL;

	EdgeBuffer					= NULL;
	AreaBuffer					= NULL;
	BlendWeightBuffer			= NULL;

	VertexDeclaration			= NULL;
	VertexShaderCommon			= NULL;
	PixelShaderEdgeDetection	= NULL;
	PixelShaderWeightBlending	= NULL;
	PixelShaderColorBlending	= NULL;
}

ZED3D9MLAAProcessor::~ZED3D9MLAAProcessor()
{
	this->Deinitialize();
}
