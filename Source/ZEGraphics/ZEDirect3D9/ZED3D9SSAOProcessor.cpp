//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9SSAOProcessor.cpp
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

#include "ZED3D9SSAOProcessor.h"
#include "ZEMath\ZEDefinitions.h"
#include "ZED3D9Common.h"
#include "ZED3D9CommonTools.h"
#include "ZEGraphics/ZETexture2DResource.h"
#include "ZED3D9Renderer.h"
#include "ZED3D9ViewPort.h"
#include "ZED3D9Texture2D.h"
#include "ZEGraphics/ZECamera.h"

#include <d3d9.h>
#include <stdlib.h>
#include <freeimage.h>
#include <math.h>

void ZED3D9SSAOProcessor::SetIntensity(float Intensity)
{
	Parameters.Intensity = Intensity;
}


float ZED3D9SSAOProcessor::GetIntensity() const
{
	return Parameters.Intensity;
}


void ZED3D9SSAOProcessor::SetSampleRadius(float Radius)
{
	Parameters.SampleRadius = Radius;
}


float ZED3D9SSAOProcessor::GetSampleRadius() const
{
	return Parameters.SampleRadius;
}


void ZED3D9SSAOProcessor::SetSampleScale(float Scale)
{
	Parameters.SampleScale = Scale;
}


float ZED3D9SSAOProcessor::GetSampleScale() const
{
	return Parameters.SampleScale;
}


void ZED3D9SSAOProcessor::SetSampleBias(float Bias)
{
	Parameters.SampleBias = Bias;
}


float ZED3D9SSAOProcessor::GetSampleBias() const
{
	return Parameters.SampleBias;
}


void ZED3D9SSAOProcessor::Initialize()
{
	// Vertex Declaration
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	// RandomTexture
	RandomTextureResource = ZETexture2DResource::LoadSharedResource("RandomNormal.tga");

	// Compile Shaders
	ZED3D9CommonTools::CompileVertexShader(&VertexShader, "SSOAProcessor.hlsl", "VSMain", "vs_3_0", NULL);
	ZED3D9CommonTools::CompilePixelShader(&PixelShader, "SSOAProcessor.hlsl", "PSMain", "ps_3_0", NULL);
}


void ZED3D9SSAOProcessor::Deinitialize()
{
	if (RandomTextureResource != NULL)
	{
		RandomTextureResource->Release();
		RandomTextureResource = NULL;
	}

	ZED3D_RELEASE(VertexShader);
	ZED3D_RELEASE(PixelShader);
	ZED3D_RELEASE(VertexDeclaration);
}


void ZED3D9SSAOProcessor::OnDeviceLost()
{

}


void ZED3D9SSAOProcessor::OnDeviceRestored()
{

}

void ZED3D9SSAOProcessor::Process()
{
	static struct Vert  
	{
		float Position[3];
		float Texcoord[2];
	} 
	Vertices[] =
	{
		{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}},
		{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}},
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
		{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}
	};

	static const float KernelDisc[4][4] =
	{
		{-1.0f,  1.0f,	0.0f, 0.0f},
		{ 1.0f,  1.0f,	0.0f, 0.0f},
		{-1.0f, -1.0f,	0.0f, 0.0f},
		{ 1.0f, -1.0f,	0.0f, 0.0f},
	};

	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	
	GetDevice()->SetRenderTarget(0, Output);

	GetDevice()->SetVertexShader(VertexShader);
	GetDevice()->SetPixelShader(PixelShader);
	GetDevice()->SetVertexDeclaration(VertexDeclaration);

	D3DSURFACE_DESC SurfaceDesc;
	InputDepth->GetLevelDesc(0, &SurfaceDesc);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&Parameters, 1);
	GetDevice()->SetVertexShaderConstantF(1, (const float*)&ZEVector4(1.0f / SurfaceDesc.Width, 1.0f / SurfaceDesc.Height, 0.0f, 0.0f), 1);
	
	ZEVector4 ViewVector;
	ViewVector.y = tanf(Renderer->GetCamera()->GetFOV() * 0.5f);// * Renderer->GetCamera()->GetFarZ();
	ViewVector.x = ViewVector.y * Renderer->GetViewPort()->GetAspectRatio();
	ViewVector.z = 1.0f;//Renderer->GetCamera()->GetFarZ();
	ViewVector.w = 0.0f;
	GetDevice()->SetVertexShaderConstantF(2, (const float*)&ViewVector, 1);

	GetDevice()->SetPixelShaderConstantF(5, (float*)&KernelDisc, 4);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetTexture(0, InputDepth);

	GetDevice()->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetTexture(1, InputNormal);

	GetDevice()->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
	GetDevice()->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
	GetDevice()->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetTexture(2, ((ZED3D9Texture2D*)RandomTextureResource->GetTexture())->Texture);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
}


ZED3D9SSAOProcessor::ZED3D9SSAOProcessor()
{
	Renderer = NULL;

	VertexShader = NULL;
	PixelShader = NULL;
	VertexDeclaration = NULL;

	InputDepth = NULL;
	InputNormal = NULL;
	Output = NULL;

	Parameters.Intensity = 5.0f;
	Parameters.SampleRadius = 1.0f;
	Parameters.SampleScale = 0.2f;
	Parameters.SampleBias = 0.05f;
	
	IterationCount = 4;
	HalfResolution = false;

	RandomTextureResource = NULL;

}

ZED3D9SSAOProcessor::~ZED3D9SSAOProcessor()
{
	Deinitialize();
}
