//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9SunRaysProcessor.cpp
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

#include "ZED3D9SunRaysProcessor.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9Module.h"
#include "ZED3D9Shader.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9FrameRenderer.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEGame/ZEWeather.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"

#define COLOR_ALL	D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA

void ZED3D9SunRaysProcessor::SetEnabled(bool Value)
{
	Enabled = Value;
}

bool ZED3D9SunRaysProcessor::GetEnabled() const
{
	return Enabled;
}

void ZED3D9SunRaysProcessor::SetInput(ZED3D9Texture2D* Input)
{
	this->Input = Input;
}

ZED3D9Texture2D* ZED3D9SunRaysProcessor::GetInput()
{
	return Input;
}

void ZED3D9SunRaysProcessor::SetOutput(ZED3D9ViewPort* Output)
{
	this->Output = Output;
}

ZED3D9ViewPort* ZED3D9SunRaysProcessor::GetOutput()
{
	return Output;
}

void ZED3D9SunRaysProcessor::SetSunDirection(const ZEVector3& SunDirection)
{
	this->SunDirection = SunDirection;
}

const ZEVector3& ZED3D9SunRaysProcessor::GetSunDirection()
{
	return SunDirection;
}

void ZED3D9SunRaysProcessor::SetSunDirectionFromScene()
{
	ZEArray<ZEEntity*> Entities = Renderer->GetCamera()->GetOwnerScene()->GetEntities(ZEWeather::Class());
	if (Entities.GetSize() == 0)
		return;

	ZEWeather* Weather = static_cast<ZEWeather*>(Entities[0]);
	SunDirection = Weather->GetSunDirection();
}

void ZED3D9SunRaysProcessor::SetDebugOutput(bool Value)
{
	DebugOutput = Value;
}

bool ZED3D9SunRaysProcessor::GetDebugOutput() const
{
	return DebugOutput;
}

void ZED3D9SunRaysProcessor::SetIntensity(float Intensity)
{
	this->Intensity = Intensity;
}

float ZED3D9SunRaysProcessor::GetIntensity() const
{
	return Intensity;
}

void ZED3D9SunRaysProcessor::SetDensity(float Density)
{
	this->Density = Density;
}

float ZED3D9SunRaysProcessor::GetDensity() const
{
	return Density;
}

void ZED3D9SunRaysProcessor::SetRenderer(ZEFrameRenderer* Renderer)
{
	this->Renderer = (ZED3D9FrameRenderer*)Renderer;
}

ZEFrameRenderer* ZED3D9SunRaysProcessor::GetRenderer()
{
	return (ZEFrameRenderer*)Renderer;
}

void ZED3D9SunRaysProcessor::Initialize()
{
	if (VertexDeclaration == NULL)
	{
		// Vertex declaration for screen aligned quad
		D3DVERTEXELEMENT9 Declaration[] = 
		{
			{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
			{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0}, 
			D3DDECL_END()
		};

		GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);
	}
	
	if (VertexShader == NULL)
	{
		VertexShader = ZED3D9VertexShader::CreateShader("SunRays.hlsl", "VSMain", 0, "vs_3_0");
	}

	if (PixelShader == NULL)
	{
		PixelShader = ZED3D9PixelShader::CreateShader("SunRays.hlsl", "PSMain", 0, "ps_3_0");
	}
}

void ZED3D9SunRaysProcessor::Deinitialize()
{
	ZED3D_RELEASE(PixelShader);
	ZED3D_RELEASE(VertexShader);
	ZED3D_RELEASE(VertexDeclaration);
}

void ZED3D9SunRaysProcessor::Process()
{
	if (!Enabled || 
		Input == NULL || 
		Output == NULL || 
		SunDirection == ZEVector3::Zero)
	{
		return;
	}

	ZEVector3 SunDirectionNormalized = SunDirection.Normalize();

	// Early Rejection
	ZECamera* Camera = Renderer->GetCamera();
	if (ZEVector3::DotProduct(SunDirectionNormalized, Camera->GetWorldFront()) >= 0)
		return;

	// Full Rejection
	SunDirectionNormalized = Camera->GetWorldPosition() - SunDirectionNormalized * 100.0f;

	ZEVector4 SunDirectionScreen;
	ZEMatrix4x4::Transform(SunDirectionScreen, Camera->GetViewProjectionTransform(), ZEVector4(SunDirectionNormalized, 1.0f));
	SunDirectionScreen.x /= SunDirectionScreen.w;
	SunDirectionScreen.y /= SunDirectionScreen.w;

	if (SunDirectionScreen.x < -1.2f || SunDirectionScreen.x > 1.2f ||
		SunDirectionScreen.y < -1.2f || SunDirectionScreen.y > 1.2f)
	{
		return;
	}

	D3DPERF_BeginEvent(0, L"Sun Rays Pass");

	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_ALL);
	
	if (!DebugOutput)
	{
		GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}
	else
	{
		GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}

	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	GetDevice()->SetPixelShader(PixelShader->GetPixelShader());
	GetDevice()->SetVertexShader(VertexShader->GetVertexShader());
	GetDevice()->SetRenderTarget(0, Output->FrameBuffer);

	GetDevice()->SetTexture(0, Input->Texture);
	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0);

	ZEVector4 PixelSizeHalf(0.5f / Input->GetWidth(),  0.5f / Input->GetHeight(), 0.0f, 0.0f);
	GetDevice()->SetVertexShaderConstantF(0, ZEVector4(PixelSizeHalf.x, PixelSizeHalf.y, SunDirectionScreen.x, SunDirectionScreen.y).M, 1);
	GetDevice()->SetVertexShaderConstantF(1, ZEVector4(Density, 0.0f, 0.0f, 0.0f).M, 1);

	GetDevice()->SetPixelShaderConstantF(0, ZEVector4(Intensity, 0.0f, 0.0f, 0.0f).M, 1);

	float Vertices[4][3] =
	{
		{-1.0f,  1.0f, 1.0f},
		{ 1.0f,  1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f},
		{ 1.0f, -1.0f, 1.0f}
	};

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEVector3));
	
	D3DPERF_EndEvent();
}


ZED3D9SunRaysProcessor::ZED3D9SunRaysProcessor()
{
	Input = NULL;
	Output = NULL;
	SunDirection = ZEVector3::Zero;
	Intensity = 2.0f;
	Density = 1.0f;

	PixelShader = NULL;
	VertexShader = NULL;

	Enabled = true;
	DebugOutput = false;

	VertexDeclaration = NULL;
}

ZED3D9SunRaysProcessor::~ZED3D9SunRaysProcessor()
{
	Deinitialize();
}
