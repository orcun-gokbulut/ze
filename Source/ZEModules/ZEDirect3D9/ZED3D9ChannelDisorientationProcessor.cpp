//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9ChannelDisorientationProcessor.cpp
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
#include "ZED3D9FrameRenderer.h"
#include "ZED3D9FrameRenderer.h"
#include "ZERenderer/ZECamera.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZED3D9ChannelDisorientationProcessor.h"

ZED3D9ChannelDisorientationProcessor::ZED3D9ChannelDisorientationProcessor()
{
	Input = NULL;
	Output = NULL;
	PixelShader = NULL;
	VertexShader = NULL;
	VertexDeclaration = NULL;

	RedOffset = ZEVector2(0.01f, 0.0f);
	GreenOffset = ZEVector2(0.0f, 0.01f);
	BlueOffset = ZEVector2(-0.01f, 0.0f);
	AlphaOffset = ZEVector2(0.0f, -0.01f);
}

ZED3D9ChannelDisorientationProcessor::~ZED3D9ChannelDisorientationProcessor()
{
	this->Deinitialize();
}

void ZED3D9ChannelDisorientationProcessor::Initialize()
{
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	VertexShader = ZED3D9VertexShader::CreateShader("ChannelDisorientationProcessor.hlsl", "vs_main", 0, "vs_3_0");
	PixelShader = ZED3D9PixelShader::CreateShader("ChannelDisorientationProcessor.hlsl", "ps_main", 0, "ps_3_0");
	
}

void ZED3D9ChannelDisorientationProcessor::Deinitialize()
{
	Input = NULL;
	Output = NULL;

	ZED3D_RELEASE(VertexShader);
	ZED3D_RELEASE(PixelShader);
	ZED3D_RELEASE(VertexDeclaration);
}

void ZED3D9ChannelDisorientationProcessor::SetRenderer(ZEFrameRenderer* Renderer)
{
	this->Renderer = (ZED3D9FrameRenderer*)Renderer;
}

ZEFrameRenderer* ZED3D9ChannelDisorientationProcessor::GetRenderer()
{
	return (ZEFrameRenderer*)Renderer;
}

void ZED3D9ChannelDisorientationProcessor::SetRedOffset(const ZEVector2& Offset)
{
	RedOffset = Offset;
}

ZEVector2 ZED3D9ChannelDisorientationProcessor::GetRedOffset() const
{
	return RedOffset;
}

void ZED3D9ChannelDisorientationProcessor::SetGreenOffset(const ZEVector2& Offset)
{
	GreenOffset = Offset;
}

ZEVector2 ZED3D9ChannelDisorientationProcessor::GetGreenOffset() const
{
	return GreenOffset;
}

void ZED3D9ChannelDisorientationProcessor::SetBlueOffset(const ZEVector2& Offset)
{
	BlueOffset = Offset;
}

ZEVector2 ZED3D9ChannelDisorientationProcessor::GetBlueOffset() const
{
	return BlueOffset;
}

void ZED3D9ChannelDisorientationProcessor::SetAlphaOffset(const ZEVector2& Offset)
{
	AlphaOffset = Offset;
}

ZEVector2 ZED3D9ChannelDisorientationProcessor::GetAlphaOffset() const
{
	return AlphaOffset;
}

void ZED3D9ChannelDisorientationProcessor::SetInput(ZED3D9Texture2D* Texture)
{
	Input = Texture;
}

ZED3D9Texture2D* ZED3D9ChannelDisorientationProcessor::GetInput()
{
	return Input;
}

void ZED3D9ChannelDisorientationProcessor::SetOutput(ZED3D9ViewPort* Texture)
{
	Output = Texture;
}

ZED3D9ViewPort* ZED3D9ChannelDisorientationProcessor::GetOutput()
{
	return Output;
}

void ZED3D9ChannelDisorientationProcessor::Process()
{
	static struct Vert  
	{
		float Position[4];

	} Vertices[] = {
		{-1.0f,  1.0f, 1.0f, 1.0f},
		{ 1.0f,  1.0f, 1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f, 1.0f},
		{ 1.0f, -1.0f, 1.0f, 1.0f}
	};

	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	

	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	struct 
	{
		float	PixelSize[2];
		float	Reserved0;
		float	Reserved1;
	
	} VertexShaderParameters = {
	
		{1.0f / Output->GetWidth(), 1.0f / Output->GetHeight()}, 0.0f, 0.0f
	};
	

	
	struct
	{
		float RedOffset[2];
		float GreenOffset[2];
		float BlueOffset[2];
		float AlphaOffset[2];	

	} PixelShaderParameters = {

		{RedOffset.x, RedOffset.y},
		{GreenOffset.x, GreenOffset.y},
		{BlueOffset.x, BlueOffset.y},
		{AlphaOffset.x, AlphaOffset.y}

	};

	GetDevice()->SetPixelShader(PixelShader->GetPixelShader());
	GetDevice()->SetVertexShader(VertexShader->GetVertexShader());

	ZED3D9CommonTools::SetRenderTarget(0, Output);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)Input, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);


	GetDevice()->SetVertexShaderConstantF(0, (const float*)&VertexShaderParameters, sizeof(VertexShaderParameters) / 16);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&PixelShaderParameters, sizeof(PixelShaderParameters) / 16);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
	
	
}
