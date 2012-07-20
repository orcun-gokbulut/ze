//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9ZoomBlurProcessor.cpp
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
#include "ZED3D9CommonTools.h"
#include "ZED3D9ZoomBlurProcessor.h"
#include "ZED3D9FrameRenderer.h"
#include "ZED3D9FrameRenderer.h"
#include "ZEGraphics/ZECamera.h"
#include "ZETexture/ZETexture2DResource.h"


ZED3D9ZoomBlurProcessor::ZED3D9ZoomBlurProcessor()
{
	Start = 0.3f;		
	Strength = 0.1f;
	TransitionOffset = 0.15f;
	Center = ZEVector2(0.5f, 0.5f);

	Input = NULL;
	Output = NULL;
	Renderer = NULL;
	PixelShader = NULL;
	VertexShader = NULL;
	VertexDeclaration = NULL;
}

ZED3D9ZoomBlurProcessor::~ZED3D9ZoomBlurProcessor()
{
	this->Deinitialize();
}

void ZED3D9ZoomBlurProcessor::Initialize()
{
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	VertexShader = ZED3D9VertexShader::CreateShader("ZoomBlurProcessor.hlsl", "vs_main", 0, "vs_3_0");
	PixelShader = ZED3D9PixelShader::CreateShader("ZoomBlurProcessor.hlsl", "ps_main", 0, "ps_3_0");
}

void ZED3D9ZoomBlurProcessor::Deinitialize()
{
	Input = NULL;
	Output = NULL;
	Renderer = NULL;

	ZED3D_RELEASE(PixelShader);
	ZED3D_RELEASE(VertexShader);
	ZED3D_RELEASE(VertexDeclaration);
}

void ZED3D9ZoomBlurProcessor::SetRenderer(ZEFrameRenderer* Renderer)
{
	this->Renderer = (ZED3D9FrameRenderer*)Renderer;
}

ZEFrameRenderer* ZED3D9ZoomBlurProcessor::GetRenderer() const
{
	return (ZEFrameRenderer*)Renderer;
}

void ZED3D9ZoomBlurProcessor::SetStart(float Value)
{
	Start = Value;
}

float ZED3D9ZoomBlurProcessor::GetStart() const
{
	return Start;
}

void ZED3D9ZoomBlurProcessor::SetStrength(float Value)
{
	Strength = Value;
}

float ZED3D9ZoomBlurProcessor::GetStrength() const
{
	return Strength;
}

void ZED3D9ZoomBlurProcessor::SetTransitionOffset(float Value)
{
	TransitionOffset = Value;
}

float ZED3D9ZoomBlurProcessor::GetTransitionOffset() const
{
	return TransitionOffset;
}

void ZED3D9ZoomBlurProcessor::SetCenter(const ZEVector2& Value)
{
	Center = Value;
}

ZEVector2 ZED3D9ZoomBlurProcessor::GetCenter() const
{
	return Center;
}

void ZED3D9ZoomBlurProcessor::SetInput(ZED3D9Texture2D* Texture)
{
	Input = Texture;
}

ZED3D9Texture2D* ZED3D9ZoomBlurProcessor::GetInput()
{
	return Input;
}

void ZED3D9ZoomBlurProcessor::SetOutput(ZED3D9RenderTarget* Texture)
{
	Output = Texture;
}

ZED3D9RenderTarget* ZED3D9ZoomBlurProcessor::GetOutput()
{
	return Output;
}

void ZED3D9ZoomBlurProcessor::Process()
{
	
	static struct Vert  
	{
		float Position[4];

	} Vertices[] = {

		{-1.0f,  1.0f, 1.0f, 1.0f},	// TopLeft
		{ 1.0f,  1.0f, 1.0f, 1.0f},	// TopRight
		{-1.0f, -1.0f, 1.0f, 1.0f},	// BottomLeft
		{ 1.0f, -1.0f, 1.0f, 1.0f}	// BottomRight
	};

	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	

	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	struct VertexShaderParameters
	{
		float	PixelSize[2];

		float	Reserved0;
		float	Reserved1;
		
	} VSParameters = {
	
		{1.0f / (float)Output->GetWidth(), 1.0f / (float)Output->GetHeight()}, 0.0f, 0.0f
	};


	struct PixelSahderParameters
	{
		float	Start;
		float	Strength;
		float	TransitionOffset;
		float	Reserved0;

		float	Center[2];
		float	Reserved1;
		float	Reserved2;

	} PSParameters = {

		Start, Strength, TransitionOffset, 0.0f,
		{Center.x, Center.y}, 0.0f, 0.0f
	};

	GetDevice()->SetVertexShader(VertexShader->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShader->GetPixelShader());

	ZED3D9CommonTools::SetRenderTarget(0, Output);
	ZED3D9CommonTools::SetTexture(0, Input, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);

	GetDevice()->SetVertexShaderConstantF(0, (const float*)&VSParameters, sizeof(VertexShaderParameters) / 16);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&PSParameters, sizeof(PixelSahderParameters) / 16);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
}
