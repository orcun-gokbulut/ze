//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9UnsharpenFilterProcessor.cpp
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
#include "ZED3D9RenderTarget.h"
#include "ZED3D9Profiler.h"
#include "ZED3D9Texture2D.h"
#include "ZEMath/ZEMatrix.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9FrameRenderer.h"
#include "ZEGraphics/ZECamera.h"
#include "ZED3D9UnsharpenFilterProcessor.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZEFile/ZEFileInfo.h"

#include <d3d9.h>
#include <stdlib.h>

void ZED3D9UnsharpenFilterProcessor::SetAmount(float Value)
{
	Amount = Value;
}

float ZED3D9UnsharpenFilterProcessor::GetAmount() const
{
	return Amount;
}

void ZED3D9UnsharpenFilterProcessor::SetRenderer(ZEFrameRenderer* Renderer)
{
	this->Renderer = (ZED3D9FrameRenderer*)Renderer;
}

ZEFrameRenderer* ZED3D9UnsharpenFilterProcessor::GetRenderer()
{
	return Renderer;
}

void ZED3D9UnsharpenFilterProcessor::SetInput(ZETexture2D* Texture)
{
	this->Input = (ZED3D9Texture2D*)Texture;
}

ZETexture2D* ZED3D9UnsharpenFilterProcessor::GetInput()
{
	return Input;
}

void ZED3D9UnsharpenFilterProcessor::SetOutput(ZED3D9RenderTarget* Texture)
{
	this->Output = Texture;
}

ZED3D9RenderTarget* ZED3D9UnsharpenFilterProcessor::GetOutput()
{
	return Output;
}

void ZED3D9UnsharpenFilterProcessor::Initialize()
{
	// Vertex declaration for screen aligned quad
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0,  16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	char *ShaderSource;
	ZEInt64 SourceSize;
	ZEFileInfo FInfo("resources\\shaders\\UnsharpenFilterProcessor.hlsl");
	SourceSize = FInfo.GetFileSize("shaders\\UnsharpenFilterProcessor.hlsl");
	SourceSize++;
	ShaderSource = new char[SourceSize];

	ZEFile::ReadTextFile("shaders\\UnsharpenFilterProcessor.hlsl", ShaderSource, SourceSize);
	
	// Compile Shaders
	VertexShader.CompileShader(NULL, 0, "vs_3_0", ShaderSource, "vs_main");
	PixelShader.CompileShader(NULL, 0, "ps_3_0", ShaderSource, "ps_main");

	delete [] ShaderSource;
}

void ZED3D9UnsharpenFilterProcessor::Deinitialize()
{
	Renderer	= NULL;
	Input		= NULL;
	Output		= NULL;
	ZED3D_RELEASE(VertexDeclaration);

	// Does Not need to Release anymore since it is a data member of this class
//	ZED3D_RELEASE(PixelShader);
//	ZED3D_RELEASE(VertexShader);
}

void ZED3D9UnsharpenFilterProcessor::OnDeviceLost()
{
	// Empty
}

void ZED3D9UnsharpenFilterProcessor::OnDeviceRestored()
{
	// Empty
}

void ZED3D9UnsharpenFilterProcessor::Process()
{
	static struct Vert  
	{
		float Position[4];
		float TexCoord[2];

	} Vertices[] = {
		{ {-1.0f,  1.0f, 1.0f, 1.0f}, {0.0f, 0.0f} },
		{ { 1.0f,  1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} },
		{ {-1.0f, -1.0f, 1.0f, 1.0f}, {0.0f, 1.0f} },
		{ { 1.0f, -1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} }
	};

	GetDevice()->SetVertexDeclaration(VertexDeclaration);

	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	float PixelWidth = 1.0f / (float)Output->GetWidth();
	float PixelHeigth = 1.0f / (float)Output->GetHeight();

	GetDevice()->SetPixelShader(PixelShader.GetPixelShader());
	GetDevice()->SetVertexShader(VertexShader.GetVertexShader());

	ZEUInt32 ColorBufferSampler;
	PixelShader.GetSamplerNumber(ColorBufferSampler, "ColorBuffer");

	ZED3D9CommonTools::SetRenderTarget(0, Output);
	ZED3D9CommonTools::SetTexture(ColorBufferSampler, (ZETexture2D*)Input, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);
	
	VertexShader.SetConstant("VSParameters", ZEVector4(PixelWidth, PixelHeigth, 0.0f, 0.0));
	PixelShader.SetConstant("PSParameters", ZEVector4(PixelWidth, PixelHeigth, Amount, 0.0));

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
}

ZED3D9UnsharpenFilterProcessor::ZED3D9UnsharpenFilterProcessor()
{
	Renderer			= NULL;
	VertexDeclaration	= NULL;
	Input				= NULL;
	Output				= NULL;
	Amount				= 0.5f;
}

ZED3D9UnsharpenFilterProcessor::~ZED3D9UnsharpenFilterProcessor()
{
	Deinitialize();
}
