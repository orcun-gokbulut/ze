//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9AerialPerspectiveProcessor.cpp
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


#include "ZED3D9Module.h"
#include "ZED3D9Shader.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9FrameRenderer.h"
#include "ZEGraphics/ZECamera.h"
#include "ZED3D9AerialPerspectiveProcessor.h"

#define COLOR_ALL	D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA

ZE_OBJECT_IMPL(ZED3D9AerialPerspectiveProcessor)

void ZED3D9AerialPerspectiveProcessor::UpdateShaders()
{
	if (VertexShader == NULL)
	{
		VertexShader = ZED3D9VertexShader::CreateShader("AerialPerspectiveProcessor.hlsl", "VSMain", 0, "vs_3_0");
	}
	if (PixelShader == NULL)
	{
		PixelShader = ZED3D9PixelShader::CreateShader("AerialPerspectiveProcessor.hlsl", "PSMain", 0, "ps_3_0");
	}
}

void ZED3D9AerialPerspectiveProcessor::DestroyShaders()
{
	ZED3D_RELEASE(PixelShader);
	ZED3D_RELEASE(VertexShader);
}

void ZED3D9AerialPerspectiveProcessor::SetScatterSymmetry(float Value)
{
	ScatterSymmetry = Value;
}

float ZED3D9AerialPerspectiveProcessor::GetScatterSymmetry() const
{
	return ScatterSymmetry;
}

void ZED3D9AerialPerspectiveProcessor::SetMieFactor(float Value)
{
	MieFactor = Value;
}

float ZED3D9AerialPerspectiveProcessor::GetMieFactor() const
{
	return MieFactor;
}

void ZED3D9AerialPerspectiveProcessor::SetRayleighFactor(float Value)
{
	RayleighFactor = Value;
}

float ZED3D9AerialPerspectiveProcessor::GetRayleighFactor() const
{
	return RayleighFactor;
}

void ZED3D9AerialPerspectiveProcessor::SetExtinctionFactor(float Value)
{
	ExtinctionFactor = Value;
}

float ZED3D9AerialPerspectiveProcessor::GetExtinctionFactor() const
{
	return ExtinctionFactor;
}

void ZED3D9AerialPerspectiveProcessor::SetSunIntensity(float Value)
{
	SunIntensity = Value;
}

float ZED3D9AerialPerspectiveProcessor::GetSunIntensity() const
{
	return SunIntensity;
}

void ZED3D9AerialPerspectiveProcessor::SetSunDirection(const ZEVector3& Value)
{
	SunDirection = Value;
}

ZEVector3 ZED3D9AerialPerspectiveProcessor::GetSunDirection() const
{
	return SunDirection;
}

void ZED3D9AerialPerspectiveProcessor::SetSunColor(const ZEVector3& Value)
{
	SunColor = Value;
}

ZEVector3 ZED3D9AerialPerspectiveProcessor::GetSunColor() const
{
	return SunColor;
}

void ZED3D9AerialPerspectiveProcessor::Initialize()
{
	// Vertex declaration for screen aligned quad
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0}, 
		D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	UpdateShaders();
}

void ZED3D9AerialPerspectiveProcessor::Deinitialize()
{
	DestroyShaders();

	ZED3D_RELEASE(VertexDeclaration);
}

void ZED3D9AerialPerspectiveProcessor::SetRenderer(ZED3D9FrameRenderer* FrameRenderer)
{
	Renderer = FrameRenderer;
}

ZED3D9FrameRenderer* ZED3D9AerialPerspectiveProcessor::GetRenderer() const	
{
	return Renderer;
}

void ZED3D9AerialPerspectiveProcessor::SetInputColor(ZED3D9Texture2D* Texture)
{
	InputColor = Texture;
}

ZED3D9Texture2D* ZED3D9AerialPerspectiveProcessor::GetInputColor()
{
	return InputColor;
}

void ZED3D9AerialPerspectiveProcessor::SetInputDepth(ZED3D9Texture2D* Texture)
{
	InputDepth = Texture;
}

ZED3D9Texture2D* ZED3D9AerialPerspectiveProcessor::GetInputDepth()
{
	return InputDepth;
}

void ZED3D9AerialPerspectiveProcessor::SetOutput(ZED3D9ViewPort* Texture)
{
	Output = Texture;
}

ZED3D9ViewPort* ZED3D9AerialPerspectiveProcessor::GetOutput()
{
	return Output;
}

#define BetaMie			ZEVector3(0.00000040f, 0.0000006f, 0.0000024f)
#define BetaRayleigh	ZEVector3(0.00000695f, 0.0000118f, 0.0000244f)

void ZED3D9AerialPerspectiveProcessor::Process()
{
	if (Renderer == NULL)
		return;

	if (Output == NULL)
		return;

	if (InputColor == NULL || InputDepth == NULL)
		return;

	D3DPERF_BeginEvent(0, L"Aerial Perspective Pass");

	ZECamera* Camera = Renderer->GetCamera();
	ZEVector4 ParameterPixelSize = ZEVector4(1.0f / Output->GetWidth(), 1.0f / Output->GetHeight(), 0.0f, 0.0f);
	ZEVector4 ParameterBetaMie = ZEVector4(BetaMie * MieFactor, 0.0f);
	ZEVector4 ParameterBetaRayleigh = ZEVector4(BetaRayleigh * RayleighFactor, 0.0f);
	ZEVector4 ParameterExtFactor = ZEVector4(ExtinctionFactor, 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterScatterSymmetry = ZEVector4(ZEMath::Clamp(ScatterSymmetry, -0.999f, 0.999f), 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterSunIntensity = ZEVector4(SunIntensity, 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterSunDirection = ZEVector4(SunDirection, 0.0f);
	ZEVector4 ParameterCameraPosition = ZEVector4(Camera->GetWorldPosition(), 0.0f);
	ZEVector4 ParameterSunColor = ZEVector4(SunColor, 0.0f);
	
	ZEMatrix4x4 InvViewMatrix;
	ZEMatrix4x4::Inverse(InvViewMatrix, Camera->GetViewTransform());

	ZEMatrix4x4 InvProjectionMatrix;
	ZEMatrix4x4::Inverse(InvProjectionMatrix, Camera->GetProjectionTransform());

	GetDevice()->SetVertexShaderConstantF(0, ParameterPixelSize.M, 1);
	GetDevice()->SetVertexShaderConstantF(1, InvProjectionMatrix.MA, 4);
	
	GetDevice()->SetPixelShaderConstantF(1, ParameterScatterSymmetry.M, 1);
	GetDevice()->SetPixelShaderConstantF(2, ParameterBetaMie.M, 1);
	GetDevice()->SetPixelShaderConstantF(3, ParameterBetaRayleigh.M, 1);
	GetDevice()->SetPixelShaderConstantF(4, ParameterSunIntensity.M, 1);
	GetDevice()->SetPixelShaderConstantF(6, ParameterSunDirection.M, 1);
	GetDevice()->SetPixelShaderConstantF(7, ParameterCameraPosition.M, 1);
	GetDevice()->SetPixelShaderConstantF(8, ParameterSunColor.M, 1);
	GetDevice()->SetPixelShaderConstantF(9, ParameterExtFactor.M, 1);
	GetDevice()->SetPixelShaderConstantF(10, InvViewMatrix.MA, 4);
	

	GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATEREQUAL);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_ALL);

	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	GetDevice()->SetPixelShader(PixelShader->GetPixelShader());
	GetDevice()->SetVertexShader(VertexShader->GetVertexShader());
	
	GetDevice()->SetRenderTarget(0, Output->FrameBuffer);

	GetDevice()->SetTexture(0, InputDepth->Texture);
	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

	GetDevice()->SetTexture(3, InputColor->Texture);
	GetDevice()->SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(3, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(3, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
	
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEAerialPerspectiveSAQuad));

	D3DPERF_EndEvent();
}

ZEAerialPerspectiveSAQuad ZED3D9AerialPerspectiveProcessor::Vertices[4] =
{
	{{-1.0f,  1.0f, 1.0f}, {0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 1.0f}, {1.0f, 0.0f}},
	{{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 1.0f}, {1.0f, 1.0f}}
};

ZED3D9AerialPerspectiveProcessor::ZED3D9AerialPerspectiveProcessor()
{
	Output = NULL;
	Renderer = NULL;
	InputColor = NULL;
	InputDepth = NULL;
	
	PixelShader = NULL;
	VertexShader = NULL;
	
	VertexDeclaration = NULL;
	
	MieFactor = 1.0f;
	RayleighFactor = 1.0f;
	ExtinctionFactor = 5.0f;
	ScatterSymmetry = -0.0f;
	
	SunIntensity = 3.0f;
	SunColor = ZEVector3::One;
	
	SunDirection = ZEVector3(-0.5f, -0.5f, -0.5f);
	SunDirection.NormalizeSelf();
}

ZED3D9AerialPerspectiveProcessor::~ZED3D9AerialPerspectiveProcessor()
{
	Deinitialize();
}
