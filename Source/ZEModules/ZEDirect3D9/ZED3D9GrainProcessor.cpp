//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9GrainProcessor.cpp
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
#include "ZED3D9Rendertarget.h"
#include "ZED3D9Profiler.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9GrainProcessor.h"
#include "ZED3D9FrameRenderer.h"
#include "ZED3D9FrameRenderer.h"
#include "ZEGraphics/ZECamera.h"
#include "ZETexture/ZETexture2DResource.h"


ZED3D9GrainProcessor::ZED3D9GrainProcessor()
{
	Strength = 0.3f;		// Noise strength
	NoiseSize = 4.0f;		// Pixels per noise
	Frequency = 20.0f;		// Buffer update rate
		
	Input = NULL;
	Output = NULL;
	GrainBuffer = NULL;
	VertexDeclaration = NULL;
}

ZED3D9GrainProcessor::~ZED3D9GrainProcessor()
{
	this->Deinitialize();
}

void ZED3D9GrainProcessor::CreateRenderTargets()
{
	this->DestroyRenderTargets();

	ZEInt Width, Height;
	zeGraphics->GetScreenSize(Width, Height);
	
	ZEInt NoiseDimension = (ZEUInt)(ZEMath::Sqrt((float)NoiseSize) + 0.5f);

	GrainBuffer = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	GrainBuffer->Create(Width / NoiseDimension , Height / NoiseDimension, 1, ZE_TPF_I8_4, true);

}

void ZED3D9GrainProcessor::DestroyRenderTargets()
{
	ZED3D_DESTROY(GrainBuffer);
}

void ZED3D9GrainProcessor::Initialize()
{
	bool Result;
	char Text[5 * 1024];
	static const ZEString ShaderFilePath = "shaders\\GrainProcessor.hlsl";

	// Vertex declaration for screen aligned quad
	D3DVERTEXELEMENT9 Declaration[] =
	{
		{0,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0,  16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);
	
	Result = ZEFile::ReadTextFile(ShaderFilePath, Text, 5*1024);
	if (!Result)
	{
		zeError("Cannot read shader file \"%s\".", ShaderFilePath.ToCString());
		return;
	}
	
	Result = VertexShader.CompileShader(NULL, 0, "vs_3_0", Text, "vs_main_common");
	if (!Result)
	{
		zeError("Cannot compile shader file \"%s\".", ShaderFilePath.ToCString());
		return;
	}

	Result = PixelShaderGrain.CompileShader(NULL, 0, "ps_3_0", Text, "ps_main_grain");
	if (!Result)
	{
		zeError("Cannot compile shader file \"%s\".", ShaderFilePath.ToCString());
		return;
	}

	Result = PixelShaderBlend.CompileShader(NULL, 0, "ps_3_0", Text, "ps_main_blend");
	if (!Result)
	{
		zeError("Cannot compile shader file \"%s\".", ShaderFilePath.ToCString());
		return;
	}

	this->CreateRenderTargets();
}

void ZED3D9GrainProcessor::Deinitialize()
{
	Input = NULL;
	Output = NULL;

	this->DestroyRenderTargets();
}

void ZED3D9GrainProcessor::SetRenderer(ZEFrameRenderer* Renderer)
{
	this->Renderer = (ZED3D9FrameRenderer*)Renderer;
}

ZEFrameRenderer* ZED3D9GrainProcessor::GetRenderer()
{
	return (ZEFrameRenderer*)Renderer;
}

float ZED3D9GrainProcessor::GetFrequency() const
{
	return Frequency;
}

void ZED3D9GrainProcessor::SetFrequency(float Value)
{
	Frequency = Value;
}

float ZED3D9GrainProcessor::GetStrength() const
{
	return Strength;
}

void ZED3D9GrainProcessor::SetStrength(float Value)
{
	Strength = Value;
}

float ZED3D9GrainProcessor::GetNoiseSize() const
{
	return NoiseSize;
}

void ZED3D9GrainProcessor::SetNoiseSize(float Value)
{
	NoiseSize = Value;
}

void ZED3D9GrainProcessor::SetInput(ZED3D9Texture2D* Texture)
{
	Input = Texture;
}

ZED3D9Texture2D* ZED3D9GrainProcessor::GetInput()
{
	return Input;
}

void ZED3D9GrainProcessor::SetOutput(ZED3D9RenderTarget* Texture)
{
	Output = Texture;
}

ZED3D9RenderTarget* ZED3D9GrainProcessor::GetOutput()
{
	return Output;
}

void ZED3D9GrainProcessor::Process(float ElapsedTime)
{
	bool Update;
	float UpdateInterval;
	static float Time = 0;
	
	static struct ScreenAlignedQuad  
	{
		float Position[4];
		float TexCoord[2];

	} SAQVertices[] = {

		{ {-1.0f,  1.0f, 1.0f, 1.0f}, {0.0f, 0.0f} },
		{ { 1.0f,  1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} },
		{ {-1.0f, -1.0f, 1.0f, 1.0f}, {0.0f, 1.0f} },
		{ { 1.0f, -1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} }
	};
	
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GetDevice()->SetVertexDeclaration(VertexDeclaration);

	ZEVector2 PixelSize(1.0f / Output->GetWidth(), 1.0f / Output->GetHeight());

	UpdateInterval = 1.0f / Frequency;
	if ( (Time + ElapsedTime) > UpdateInterval )
	{
		Update = true;
		Time = (Time + ElapsedTime) - UpdateInterval;
	}
	else
	{
		Time += ElapsedTime;
		Update = false;
	}

	if (Update)
	{
		GetDevice()->SetVertexShader(VertexShader.GetVertexShader());
		GetDevice()->SetPixelShader(PixelShaderGrain.GetPixelShader());

		VertexShader.SetConstant("PixelSize", PixelSize);
		
		PixelShaderGrain.SetConstant("Time", Time);
		PixelShaderGrain.SetConstant("GrainStrength", Strength);

		ZED3D9CommonTools::SetRenderTarget(0, GrainBuffer);

		GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, SAQVertices, sizeof(ScreenAlignedQuad));
	}
	
	// Blend grain buffer
	GetDevice()->SetVertexShader(VertexShader.GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderBlend.GetPixelShader());

	VertexShader.SetConstant("PixelSize", PixelSize);

	ZED3D9CommonTools::SetRenderTarget(0, Output);

	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)Input, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(1, (ZETexture2D*)GrainBuffer, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, SAQVertices, sizeof(ScreenAlignedQuad));
}
