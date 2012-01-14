//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9ColorTransformProcessor.cpp
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
#include "ZED3D9ColorTransformProcessor.h"
#include "ZEGraphics/ZETexture2DResource.h"

#include <d3d9.h>
#include <stdlib.h>

const ZEMatrix4x4 ZEColorMatrix::Sepia = ZEMatrix4x4(	0.1495f, 0.1495f, 0.1495f, 0.0f,
														0.2935f, 0.2935f, 0.2935f, 0.0f,
														0.057f,  0.057f,  0.057f,  0.0f,
														0.5f,    0.25f,   0.0f,    1.0f );

const ZEMatrix4x4 ZEColorMatrix::BlackWhite = ZEMatrix4x4(	0.299f, 0.587f, 0.184f, 0.0f,     
														  	0.299f, 0.587f, 0.184f, 0.0f,
														  	0.299f, 0.587f, 0.184f, 0.0f, 
														  	0.0f,   0.0f,	0.0f,	0.1f );

const ZEMatrix4x4 ZEColorMatrix::Red = ZEMatrix4x4(	1.0f, 0.0f, 0.0f, 0.0f,
													0.0f, 0.0f, 0.0f, 0.0f,
													0.0f, 0.0f, 0.0f, 0.0f,
													0.0f, 0.0f, 0.0f, 1.0f	);

const ZEMatrix4x4 ZEColorMatrix::Green = ZEMatrix4x4(	0.0f, 0.0f, 0.0f, 0.0f,
														0.0f, 1.0f, 0.0f, 0.0f,
														0.0f, 0.0f, 0.0f, 0.0f,
														0.0f, 0.0f, 0.0f, 1.0f	);

const ZEMatrix4x4 ZEColorMatrix::Blue = ZEMatrix4x4(	0.0f, 0.0f, 0.0f, 0.0f,
														0.0f, 0.0f, 0.0f, 0.0f,
														0.0f, 0.0f, 1.0f, 0.0f,
														0.0f, 0.0f, 0.0f, 1.0f	);

const ZEMatrix4x4 ZEColorMatrix::Black = ZEMatrix4x4(	0.0f, 0.0f, 0.0f, 0.0f,
														0.0f, 0.0f, 0.0f, 0.0f,
														0.0f, 0.0f, 0.0f, 0.0f,
														0.0f, 0.0f, 0.0f, 1.0f	);

const ZEMatrix4x4 ZEColorMatrix::White = ZEMatrix4x4(	5.0f, 5.0f, 5.0f, 0.0f,
														5.0f, 5.0f, 5.0f, 0.0f,
														5.0f, 5.0f, 5.0f, 0.0f,
														5.0f, 5.0f, 5.0f, 1.0f	);



void ZED3D9ColorTransformProcessor::SetRenderer(ZEFrameRenderer* Renderer)
{
	this->Renderer = (ZED3D9FrameRenderer*)Renderer;
}

ZEFrameRenderer* ZED3D9ColorTransformProcessor::GetRenderer()
{
	return Renderer;
}

void ZED3D9ColorTransformProcessor::SetInput(ZETexture2D* Texture)
{
	this->InputBuffer = (ZED3D9Texture2D*)Texture;
}

ZETexture2D* ZED3D9ColorTransformProcessor::GetInput()
{
	return InputBuffer;
}

void ZED3D9ColorTransformProcessor::SetOutput(ZED3D9ViewPort* Texture)
{
	this->OutputBuffer = Texture;
}

ZED3D9ViewPort* ZED3D9ColorTransformProcessor::GetOutput()
{
	return OutputBuffer;
}

void ZED3D9ColorTransformProcessor::SetColorMatrix(const ZEMatrix4x4* Matrix, float BlendFactor)
{
	this->Matrix = (ZEMatrix4x4*)Matrix;
	this->BlendFactor = BlendFactor;
}

float ZED3D9ColorTransformProcessor::GetBlendFactor()
{
	return BlendFactor;
}

ZEMatrix4x4* ZED3D9ColorTransformProcessor::GetColorMatrix()
{
	return Matrix;
}

void ZED3D9ColorTransformProcessor::Initialize()
{
	// Vertex declaration for screen aligned quad
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	// Compile Shaders
	this->VertexShader = ZED3D9VertexShader::CreateShader("ColorTransformProcessor.hlsl", "vs_main", 0, "vs_3_0");
	this->PixelShader = ZED3D9PixelShader::CreateShader("ColorTransformProcessor.hlsl", "ps_main", 0, "ps_3_0");

	this->SetColorMatrix(&ZEColorMatrix::Identity, 1.0f);
}

void ZED3D9ColorTransformProcessor::Deinitialize()
{
	Renderer		= NULL;
	InputBuffer		= NULL;
	OutputBuffer	= NULL;

	BlendFactor		= 0;
	this->SetColorMatrix(&ZEColorMatrix::Identity, 1.0f);

	ZED3D_RELEASE(PixelShader);
	ZED3D_RELEASE(VertexShader);
	ZED3D_RELEASE(VertexDeclaration);
}

void ZED3D9ColorTransformProcessor::OnDeviceLost()
{
	// Empty
}

void ZED3D9ColorTransformProcessor::OnDeviceRestored()
{
	// Empty
}

void ZED3D9ColorTransformProcessor::Process()
{
	zeProfilerStart("Color Transform Pass");

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
	
	// Set shaders and vertex declaration
	GetDevice()->SetVertexShader(VertexShader->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShader->GetPixelShader());
	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	
	// Set render states and output
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	
	// Send the color matrix and pixel size
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(BlendFactor, 0.0f, 0.0f, 0.0f), 1);
	GetDevice()->SetPixelShaderConstantF(1, (const float*)Matrix, 4);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / InputBuffer->GetWidth(), 1.0f / InputBuffer->GetHeight(), 0.0f, 0.0f), 1);

	// Set input and output
	ZED3D9CommonTools::SetRenderTarget(0, OutputBuffer);
	ZED3D9CommonTools::SetTexture(6, (ZETexture2D*)InputBuffer, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

	// Draw
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	zeProfilerEnd();
}

ZED3D9ColorTransformProcessor::ZED3D9ColorTransformProcessor()
{
	Renderer			= NULL;
	VertexDeclaration	= NULL;
	VertexShader		= NULL;
	PixelShader			= NULL;

	InputBuffer			= NULL;
	OutputBuffer		= NULL;

	BlendFactor			= 0;
	
}

ZED3D9ColorTransformProcessor::~ZED3D9ColorTransformProcessor()
{
	Deinitialize();
}