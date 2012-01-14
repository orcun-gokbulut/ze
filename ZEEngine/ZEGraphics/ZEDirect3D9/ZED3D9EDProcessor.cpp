//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9EDProcessor.cpp
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

#include "ZED3D9EDProcessor.h"
#include "ZED3D9CommonTools.h"
#include "ZEGraphics/ZETexture2DResource.h"
#include "ZED3D9FrameRenderer.h"
#include "ZED3D9ViewPort.h"
#include "ZED3D9Texture2D.h"
#include "ZEGraphics/ZECamera.h"
#include "ZED3D9Profiler.h"
#include "ZED3D9Shader.h"

#include <d3d9.h>
#include <stdlib.h>

void ZED3D9EDProcessor::SetRenderer(ZEFrameRenderer* Renderer)
{
	this->Renderer = (ZED3D9FrameRenderer*)Renderer;
}

ZEFrameRenderer* ZED3D9EDProcessor::GetRenderer()
{
	return Renderer;
}

void ZED3D9EDProcessor::SetInputColor(ZETexture2D* Texture)
{
	this->InputColorBuffer = (ZED3D9Texture2D*)Texture;
}

ZETexture2D* ZED3D9EDProcessor::GetInputColor()
{
	return InputColorBuffer;
}

void ZED3D9EDProcessor::SetInputDepth(ZETexture2D* Texture)
{
	this->InputDepthBuffer = (ZED3D9Texture2D*)Texture; 
}

ZETexture2D* ZED3D9EDProcessor::GetInputDepth()
{
	return InputDepthBuffer;
}

void ZED3D9EDProcessor::SetInputNormal(ZETexture2D* Texture)
{
	this->InputNormalBuffer = (ZED3D9Texture2D*)Texture;
}

ZETexture2D* ZED3D9EDProcessor::GetInputNormal()
{
	return InputNormalBuffer;
}

void ZED3D9EDProcessor::SetOutput(ZED3D9ViewPort* Texture)
{
	this->Output = Texture;
}

ZED3D9ViewPort* ZED3D9EDProcessor::GetOutput()
{
	return Output;
}

void ZED3D9EDProcessor::Initialize()
{
	// Vertex declaration for screen aligned quad
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	// Compile Shaders
	this->VertexShader = ZED3D9VertexShader::CreateShader("EDProcessor.hlsl", "vs_main", 0, "vs_3_0");
	this->PixelShader = ZED3D9PixelShader::CreateShader("EDProcessor.hlsl", "ps_main", 0, "ps_3_0");
}

void ZED3D9EDProcessor::Deinitialize()
{
	Output			  = NULL;
	Renderer		  = NULL;
	InputDepthBuffer  = NULL;
	InputNormalBuffer = NULL;
	InputColorBuffer  = NULL;

	ZED3D_RELEASE(VertexShader);
	ZED3D_RELEASE(PixelShader);
	ZED3D_RELEASE(VertexDeclaration);
}

void ZED3D9EDProcessor::OnDeviceLost()
{
	// Empty
}

void ZED3D9EDProcessor::OnDeviceRestored()
{
	// Empty
}

void ZED3D9EDProcessor::Process()
{
	zeProfilerStart("ED Pass");

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
	
	GetDevice()->SetVertexShader(VertexShader->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShader->GetPixelShader());
	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	ZED3D9CommonTools::SetRenderTarget(0, Output);
	
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / InputDepthBuffer->GetWidth(), 1.0f / InputDepthBuffer->GetHeight(), 0.0f, 0.0f), 1);
	
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)InputDepthBuffer, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(1, (ZETexture2D*)InputNormalBuffer, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(4, (ZETexture2D*)InputColorBuffer, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	zeProfilerEnd();
}

ZED3D9EDProcessor::ZED3D9EDProcessor()
{
	Renderer			= NULL;
	VertexDeclaration	= NULL;
	VertexShader		= NULL;
	PixelShader			= NULL;

	InputDepthBuffer	= NULL;
	InputNormalBuffer	= NULL;
	Output				= NULL;
	
}

ZED3D9EDProcessor::~ZED3D9EDProcessor()
{
	Deinitialize();
}