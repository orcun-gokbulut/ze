//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9TextureMaskProcessor.cpp
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
#include "ZED3D9TextureMaskProcessor.h"
#include "ZED3D9FrameRenderer.h"
#include "ZED3D9FrameRenderer.h"
#include "ZERenderer/ZECamera.h"
#include "ZETexture/ZETexture2DResource.h"


ZED3D9TextureMaskProcessor::ZED3D9TextureMaskProcessor()
{
	Input = NULL;
	Output = NULL;
	Renderer = NULL;
	MaskFactor = 0.8f;
	MaskTexture = NULL;
	PixelShader = NULL;
	VertexShader = NULL;
	VertexDeclaration = NULL;

	Adressing = D3DTADDRESS_WRAP;
}

ZED3D9TextureMaskProcessor::~ZED3D9TextureMaskProcessor()
{
	this->Deinitialize();
}

void ZED3D9TextureMaskProcessor::Initialize()
{
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0,  16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	VertexShader = ZED3D9VertexShader::CreateShader("TextureMaskProcessor.hlsl", "vs_main", 0, "vs_3_0");
	PixelShader = ZED3D9PixelShader::CreateShader("TextureMaskProcessor.hlsl", "ps_main", 0, "ps_3_0");
}

void ZED3D9TextureMaskProcessor::Deinitialize()
{
	Input = NULL;
	Output = NULL;

	ZED3D_RELEASE(PixelShader);
	ZED3D_RELEASE(VertexShader);
	ZED3D_RELEASE(VertexDeclaration);
}

void ZED3D9TextureMaskProcessor::SetRenderer(ZEFrameRenderer* Renderer)
{
	this->Renderer = (ZED3D9FrameRenderer*)Renderer;
}

ZEFrameRenderer* ZED3D9TextureMaskProcessor::GetRenderer() const
{
	return (ZEFrameRenderer*)Renderer;
}

void ZED3D9TextureMaskProcessor::SetAdressing(ZEUInt Adress)
{
	Adressing = Adress;
}

ZEUInt ZED3D9TextureMaskProcessor::GetAdressing() const
{
	return Adressing;
}

void ZED3D9TextureMaskProcessor::SetMaskFactor(float Factor)
{
	MaskFactor = Factor;
}

float ZED3D9TextureMaskProcessor::GetMaskFactor() const
{
	return MaskFactor;
}

bool ZED3D9TextureMaskProcessor::SetMaskTexture(const ZEString& Path)
{
	static ZETextureOptions MaskOptions = {ZE_TCT_AUTO, ZE_TCQ_HIGH, ZE_TDS_NONE, ZE_TFC_DISABLED, ZE_TMM_DISABLED, 1};
	MaskTexture = ZETexture2DResource::LoadResource(Path, &MaskOptions)->GetTexture();

	return MaskTexture != NULL;
}

const ZETexture2D* ZED3D9TextureMaskProcessor::GetMaskTexture() const
{
	return MaskTexture;
}

void ZED3D9TextureMaskProcessor::SetInput(ZED3D9Texture2D* Texture)
{
	Input = Texture;
}

ZED3D9Texture2D* ZED3D9TextureMaskProcessor::GetInput()
{
	return Input;
}

void ZED3D9TextureMaskProcessor::SetOutput(ZED3D9ViewPort* Texture)
{
	Output = Texture;
}

ZED3D9ViewPort* ZED3D9TextureMaskProcessor::GetOutput()
{
	return Output;
}

void ZED3D9TextureMaskProcessor::Process()
{
	
	static struct Vert  
	{
		float Position[4];
		float TexCoord[2];

	} Vertices[] = {

		{ {-1.0f,  1.0f, 1.0f, 1.0f}, {0.0f, 0.0f} },	// TopLeft
		{ { 1.0f,  1.0f, 1.0f, 1.0f}, {0.0f, 0.0f} },	// TopRight
		{ {-1.0f, -1.0f, 1.0f, 1.0f}, {0.0f, 0.0f} },	// BottomLeft
		{ { 1.0f, -1.0f, 1.0f, 1.0f}, {0.0f, 0.0f} }	// BottomRight
	};

	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	

	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	
	float OutputWidth = (float)Output->GetWidth();
	float OutputHeight = (float)Output->GetHeight();
	float MaskWidth = (float)MaskTexture->GetWidth();
	float MaskHeight = (float)MaskTexture->GetHeight();
	float OutputMaskWidthRatio = OutputWidth / MaskWidth;
	float OutputMaskHeightRatio = OutputHeight /MaskHeight;

	// TopLeft
	Vertices[0].TexCoord[0] = 0.0f;
	Vertices[0].TexCoord[1] = 0.0f;

	// TopRight
	Vertices[1].TexCoord[0] = OutputMaskWidthRatio;
	Vertices[1].TexCoord[1] = 0.0f;

	// BottomLeft
	Vertices[2].TexCoord[0] = 0.0f;
	Vertices[2].TexCoord[1] = OutputMaskHeightRatio;
	
	// BottomRight
	Vertices[3].TexCoord[0] = OutputMaskWidthRatio;
	Vertices[3].TexCoord[1] = OutputMaskHeightRatio;

	struct VertexShaderParameters
	{
		float	PixelSizeInput[2];
		float	PixelSizeMask[2];
	
	} VSParameters = {
	
		{1.0f / OutputWidth, 1.0f / OutputHeight}, {1.0f / MaskWidth, 1.0f / MaskHeight}
	};


	struct PixelSahderParameters
	{
		float	MaskFactor;
		float	Reserved0;
		float	Reserved1;
		float	Reserved2;

	} PSParameters = {

		MaskFactor, 0.0f, 0.0f, 0.0f
	};

	GetDevice()->SetVertexShader(VertexShader->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShader->GetPixelShader());

	ZED3D9CommonTools::SetRenderTarget(0, Output);
	ZED3D9CommonTools::SetTexture(0, Input, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(1, MaskTexture, D3DTEXF_POINT, D3DTEXF_POINT, Adressing);

	GetDevice()->SetVertexShaderConstantF(0, (const float*)&VSParameters, sizeof(VertexShaderParameters) / 16);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&PSParameters, sizeof(PixelSahderParameters) / 16);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
	
}
