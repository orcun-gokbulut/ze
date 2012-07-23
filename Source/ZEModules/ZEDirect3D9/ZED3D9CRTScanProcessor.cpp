//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9CRTScanProcessor.cpp
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

#include "ZEMath/ZEAngle.h"
#include "ZED3D9Shader.h"
#include "ZED3D9Rendertarget.h"
#include "ZED3D9Profiler.h"
#include "ZED3D9Texture2D.h"
#include "ZEMath/ZEMatrix.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9FrameRenderer.h"
#include "ZEGraphics/ZECamera.h"
#include "ZED3D9CRTScanProcessor.h"
#include "ZETexture/ZETexture2DResource.h"

#include <d3d9.h>
#include <stdlib.h>


void ZED3D9CRTScanProcessor::SetRenderer(ZEFrameRenderer* Renderer)
{
	this->Renderer = (ZED3D9FrameRenderer*)Renderer;
}

ZEFrameRenderer* ZED3D9CRTScanProcessor::GetRenderer()
{
	return Renderer;
}

void ZED3D9CRTScanProcessor::SetInput(ZETexture2D* Texture)
{
	this->InputBuffer = (ZED3D9Texture2D*)Texture;
}

ZETexture2D* ZED3D9CRTScanProcessor::GetInput()
{
	return InputBuffer;
}

void ZED3D9CRTScanProcessor::SetOutput(ZED3D9RenderTarget* Texture)
{
	this->OutputBuffer = Texture;
}

ZED3D9RenderTarget* ZED3D9CRTScanProcessor::GetOutput()
{
	return OutputBuffer;
}


void ZED3D9CRTScanProcessor::SetScanThickness(float Value)
{
	ScanThickness = Value;
}

float ZED3D9CRTScanProcessor::GetScanThickness()
{
	return ScanThickness;
}
		
void ZED3D9CRTScanProcessor::SetScanColor(ZEVector3 Color)
{
	ScanColor = Color;
}

ZEVector3 ZED3D9CRTScanProcessor::GetScanColor()
{
	return ScanColor;
}

void ZED3D9CRTScanProcessor::SetDirection(CRTScanDirection Value)
{
	Direction = Value;
}

CRTScanDirection ZED3D9CRTScanProcessor::GetDirection()
{
	return Direction;
}

void ZED3D9CRTScanProcessor::SetSpeed(float Value)
{
	Speed = Value;
}

float ZED3D9CRTScanProcessor::GetSpeed()
{
	return Speed;
}

void ZED3D9CRTScanProcessor::SetSharpness(float Value)
{
	Sharpness = Value;
}

float ZED3D9CRTScanProcessor::GetSharpness()
{
	return Sharpness;
}

void ZED3D9CRTScanProcessor::Initialize()
{
	bool Result;
	char Text[5 * 1024];
	static const ZEString ShaderFilePath = "shaders\\CRTScanProcessor.hlsl";

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
	
	Result = VertexShader.CompileShader(NULL, 0, "vs_3_0", Text, "vs_main");
	if (!Result)
	{
		zeError("Cannot compile shader file \"%s\".", ShaderFilePath.ToCString());
		return;
	}

	Result = PixelShader.CompileShader(NULL, 0, "ps_3_0", Text, "ps_main");
	if (!Result)
	{
		zeError("Cannot compile shader file \"%s\".", ShaderFilePath.ToCString());
		return;
	}
}

void ZED3D9CRTScanProcessor::Deinitialize()
{
	Renderer		= NULL;
	InputBuffer		= NULL;
	OutputBuffer	= NULL;
}

void ZED3D9CRTScanProcessor::OnDeviceLost()
{
	// Empty
}

void ZED3D9CRTScanProcessor::OnDeviceRestored()
{
	// Empty
}

void ZED3D9CRTScanProcessor::Process(float ElapsedTime)
{
	zeProfilerStart("CRT Scan Pass");

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
	
	// Set shaders and vertex declaration
	GetDevice()->SetVertexShader(VertexShader.GetVertexShader());
	GetDevice()->SetPixelShader(PixelShader.GetPixelShader());
	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	
	// Set render states and output
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	static float ScanCoord = 0.0f;
	ScanCoord += ElapsedTime * Speed;
	ScanCoord = ZEMath::Circular(ScanCoord, 0.0f, 1.0f);

	float DirectedScanCooord = ScanCoord;
	if (Direction == SCAN_DIRECTION_UP)
		DirectedScanCooord = 1.0f - ScanCoord;

	ZEVector2 PixelSize(1.0f / (float)OutputBuffer->GetWidth(), 1.0f / (float)OutputBuffer->GetHeight());
		
	VertexShader.SetConstant("PixelSize", PixelSize);

	PixelShader.SetConstant("Color", ScanColor);
	PixelShader.SetConstant("Thickness", ScanThickness);
	PixelShader.SetConstant("Sharpness", Sharpness);
	PixelShader.SetConstant("ScanCoord", DirectedScanCooord);

	ZED3D9CommonTools::SetRenderTarget(0, OutputBuffer);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)InputBuffer, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);

	// Draw
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, SAQVertices, sizeof(ScreenAlignedQuad));

	zeProfilerEnd();
}

ZED3D9CRTScanProcessor::ZED3D9CRTScanProcessor()
{
	Renderer			= NULL;
	VertexDeclaration	= NULL;
	InputBuffer			= NULL;
	OutputBuffer		= NULL;
	
	Speed				= 0.6f;
	ScanThickness		= 0.06f;
	Sharpness			= 0.6f;
	ScanColor			= ZEVector3::Zero;
	Direction			= SCAN_DIRECTION_DOWN;
}

ZED3D9CRTScanProcessor::~ZED3D9CRTScanProcessor()
{
	Deinitialize();
}
