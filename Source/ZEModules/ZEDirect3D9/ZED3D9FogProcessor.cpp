//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9FogProcessor.cpp
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
#include "ZED3D9FogProcessor.h"
#include "ZED3D9FrameRenderer.h"
#include "ZEGraphics/ZECamera.h"
#include "ZETexture/ZETexture2DResource.h"

#include <d3d9.h>
#include "ZEGame/ZEScene.h"

void ZED3D9FogProcessor::SetFogModel(ZED3D9FogModel Model)
{
	FogModel = Model;
}

ZED3D9FogModel ZED3D9FogProcessor::GetFogModel()
{
	return FogModel;
}

void ZED3D9FogProcessor::SetFogColor(ZEVector3 Color)
{
	FogColor = Color;
}

ZEVector3 ZED3D9FogProcessor::GetFogColor()
{
	return FogColor;
}

void ZED3D9FogProcessor::SetFogHeight(float Value)
{
	FogHeight = Value;
}

void ZED3D9FogProcessor::SetOutScatterFactor(ZEVector3 Color)
{
	OutScatterFactor = Color;
}

ZEVector3 ZED3D9FogProcessor::GetOutScatterFactor()
{
	return OutScatterFactor;
}

void ZED3D9FogProcessor::SetInScatterFactor(ZEVector3 Color)
{
	InScatterFactor = Color;
}

ZEVector3 ZED3D9FogProcessor::GetInScatterFactor()
{
	return InScatterFactor;
}

float ZED3D9FogProcessor::GetFogHeight()
{
	return FogHeight;
}

void ZED3D9FogProcessor::SetFogDistanceFar(float Value)
{
	FogDistanceFar = Value;
}

float ZED3D9FogProcessor::GetFogDistanceFar()
{
	return FogDistanceFar;
}

void ZED3D9FogProcessor::SetFogDistanceNear(float Value)
{
	FogDistanceNear = Value;
}

float ZED3D9FogProcessor::GetFogDistanceNear()
{
	return FogDistanceNear;
}

void ZED3D9FogProcessor::SetFogVisibility(float Value)
{
	FogVisibility = Value;
}

float ZED3D9FogProcessor::GetFogVisibility()
{
	return FogVisibility;
}

void ZED3D9FogProcessor::SetRenderer(ZEFrameRenderer* Renderer)
{
	this->Renderer = (ZED3D9FrameRenderer*)Renderer;
}

ZEFrameRenderer* ZED3D9FogProcessor::GetRenderer()
{
	return (ZEFrameRenderer*)Renderer;
}

void ZED3D9FogProcessor::SetInputDepth(ZETexture2D* Texture)
{
	InputDepthBuffer = (ZED3D9Texture2D*)Texture;
}

ZETexture2D* ZED3D9FogProcessor::GetInputDepth()
{
	return (ZETexture2D*)InputDepthBuffer;
}

void ZED3D9FogProcessor::SetInputColor(ZETexture2D* Texture)
{
	InputColorBuffer = (ZED3D9Texture2D*)Texture;
}

ZETexture2D* ZED3D9FogProcessor::GetInputColor()
{
	return (ZETexture2D*)InputColorBuffer;
}

void ZED3D9FogProcessor::SetOutput(ZED3D9ViewPort* Texture)
{
	OutputBuffer = Texture;
}

ZED3D9ViewPort* ZED3D9FogProcessor::GetOutput()
{
	return OutputBuffer;
}

void ZED3D9FogProcessor::OnDeviceLost()
{

}

void ZED3D9FogProcessor::OnDeviceRestored()
{

}

void ZED3D9FogProcessor::Process()
{
	zeProfilerStart("Fog Pass");

	static struct Vert  
	{
		float Position[3];

	} Vertices[] = {
		{-1.0f,  1.0f, 1.0f},
		{ 1.0f,  1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f},
		{ 1.0f, -1.0f, 1.0f}
	};

	GetDevice()->SetVertexDeclaration(VertexDeclaration);

	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


	// Set Pixel & Vertex shader
	switch(FogModel)
	{
		default:
		case ZE_D3D9_FM_LINEAR:
			GetDevice()->SetVertexShader(VertexShaderFog->GetVertexShader());
			GetDevice()->SetPixelShader(PixelShaderLinearFog->GetPixelShader());
			break;
		case ZE_D3D9_FM_EXPONENTIAL:
			GetDevice()->SetVertexShader(VertexShaderFog->GetVertexShader());
			GetDevice()->SetPixelShader(PixelShaderExpFog->GetPixelShader());
			break;
		case ZE_D3D9_FM_EXPONENTIAL_SQUARE:
			GetDevice()->SetVertexShader(VertexShaderFog->GetVertexShader());
			GetDevice()->SetPixelShader(PixelShaderExpSquareFog->GetPixelShader());
			break;
	}

	// Set Input output buffers
	ZED3D9CommonTools::SetRenderTarget(0, OutputBuffer);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)InputDepthBuffer, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(4, (ZETexture2D*)InputColorBuffer, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);
	
	ZECamera* Camera = zeScene->GetActiveCamera();

	struct 
	{
		float	PixelSize[2];
		float	Reserved0;
		float	Reserved1;

	}VertexShaderParameters = {
		{1.0f / OutputBuffer->GetWidth(), 1.0f / OutputBuffer->GetHeight()}, 0.0f, 0.0f,

	};

	struct
	{
		// FogParameters0
		float	FogColor[3];
		float	FarZ;

		// FogParameters1
		float	FogDistanceFar;
		float	FogDistanceNear;
		float	FogHeight;
		float	FogVisibility;
		
		// FogParameters2
		float	OutScatterFactor[3];
		float	Reserved1;

		// FogParameters3
		float	InScatterFactor[3];
		float	Reserved2;

	}PixelShaderParameters = {
		{FogColor.x, FogColor.y, FogColor.z}, Camera->GetFarZ(),
		FogDistanceFar, FogDistanceNear, FogHeight, FogVisibility,
		{OutScatterFactor.x, OutScatterFactor.y, OutScatterFactor.z}, 0.0f,
		{InScatterFactor.x, InScatterFactor.y, InScatterFactor.z}, 0.0f,

	};

	// Send parameters to pipeline
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&VertexShaderParameters, sizeof(VertexShaderParameters) / 16);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&PixelShaderParameters, sizeof(PixelShaderParameters) / 16);
	
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	zeProfilerEnd();
}

void ZED3D9FogProcessor::Initialize()
{
	// Vertex declaration for screen aligned quad
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	// Compile Shaders
	this->VertexShaderFog			= ZED3D9VertexShader::CreateShader("FogProcessor.hlsl", "vs_main_common", 0, "vs_3_0");
	this->PixelShaderLinearFog		= ZED3D9PixelShader::CreateShader("FogProcessor.hlsl", "ps_main_linear", 0, "ps_3_0");
	this->PixelShaderExpFog			= ZED3D9PixelShader::CreateShader("FogProcessor.hlsl", "ps_main_exponential", 0, "ps_3_0");
	this->PixelShaderExpSquareFog	= ZED3D9PixelShader::CreateShader("FogProcessor.hlsl", "ps_main_exponential_squared", 0, "ps_3_0");

}

void ZED3D9FogProcessor::Deinitialize()
{
	Renderer			= NULL;

	InputColorBuffer	= NULL;
	InputDepthBuffer	= NULL;
	OutputBuffer		= NULL;

	ZED3D_RELEASE(VertexShaderFog);
	ZED3D_RELEASE(PixelShaderExpFog);
	ZED3D_RELEASE(PixelShaderLinearFog);
	ZED3D_RELEASE(PixelShaderExpSquareFog);

}

ZED3D9FogProcessor::ZED3D9FogProcessor()
{
	Renderer				= NULL;
	InputColorBuffer		= NULL;
	InputDepthBuffer		= NULL;
	OutputBuffer			= NULL;
	VertexShaderFog			= NULL;
	PixelShaderExpFog		= NULL;
	PixelShaderLinearFog	= NULL;
	PixelShaderExpSquareFog	= NULL;

	FogHeight				= 15.0f;
	FogDistanceFar			= 400.0f;
	FogDistanceNear			= 20.0f;
	FogVisibility			= 0.1f;

	FogModel				= ZE_D3D9_FM_LINEAR;

	OutScatterFactor		= ZEVector3(0.003f, 0.003f, 0.003f);
	InScatterFactor			= ZEVector3(0.003f, 0.003f, 0.003f);
	FogColor				= ZEVector3(0.590f, 0.580f, 0.600f);


}

ZED3D9FogProcessor::~ZED3D9FogProcessor()
{	
		this->Deinitialize();
	
}
