//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9BlurMaskProcessor.cpp
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
#include "ZED3D9BlurMaskProcessor.h"
#include "ZED3D9FrameRenderer.h"
#include "ZED3D9FrameRenderer.h"
#include "ZEGraphics/ZECamera.h"
#include "ZETexture/ZETexture2DResource.h"



ZED3D9BlurMaskProcessor::ZED3D9BlurMaskProcessor()
{
	Input = NULL;
	Output = NULL;
	Renderer = NULL;
	BlurFactor = 1.0f;
	PixelShaderBlur = NULL;
	PixelShaderDownUpSample = NULL;
	PixelShaderBlurMask = NULL;
	VertexShaderCommon = NULL;
	BlurMaskTexture = NULL;
	VertexDeclaration = NULL;
	BlurColor = ZEVector3(0.5f, 0.0f, 0.5f);
}

ZED3D9BlurMaskProcessor::~ZED3D9BlurMaskProcessor()
{
	this->Deinitialize();
}

void ZED3D9BlurMaskProcessor::CreateRenderTargets()
{
	bool Result;
	
	ZEUInt ViewportWidth = Renderer->GetViewPort()->GetWidth();
	ZEUInt ViewportHeight = Renderer->GetViewPort()->GetHeight();

	ZEUInt HalfViewportWidth = ViewportWidth / 2;
	ZEUInt HalfViewportHeight = ViewportHeight / 2;

	DS2x = ZETexture2D::CreateInstance();
	Result = DS2x->Create(HalfViewportWidth, HalfViewportHeight, 1, ZE_TPF_I8_4, true);
	zeDebugCheck(!Result, "Cannot Create ZED3D9Texture2D...");

	DS2xBlurred = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Result = DS2xBlurred->Create(HalfViewportWidth, HalfViewportHeight, 1, ZE_TPF_I8_4, true);
	zeDebugCheck(!Result, "Cannot Create ZED3D9Texture2D...");

	DS2xBlurredUS2x = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	Result = DS2xBlurredUS2x->Create(ViewportWidth, ViewportWidth, 1, ZE_TPF_I8_4, true);
	zeDebugCheck(!Result, "Cannot Create ZED3D9Texture2D...");
}

void ZED3D9BlurMaskProcessor::DestroyRenderTargets()
{
	ZED3D_DESTROY(DS2x);
	ZED3D_DESTROY(DS2xBlurred);
	ZED3D_DESTROY(DS2xBlurredUS2x);
}

void ZED3D9BlurMaskProcessor::Initialize()
{
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	PixelShaderBlur = ZED3D9PixelShader::CreateShader("BlurMaskProcessor.hlsl", "ps_main_blur", 0, "ps_3_0");
	PixelShaderBlurMask = ZED3D9PixelShader::CreateShader("BlurMaskProcessor.hlsl", "ps_main_blur_mask", 0, "ps_3_0");
	PixelShaderDownUpSample = ZED3D9PixelShader::CreateShader("BlurMaskProcessor.hlsl", "ps_main_sample", 0, "ps_3_0");

	VertexShaderCommon = ZED3D9VertexShader::CreateShader("BlurMaskProcessor.hlsl", "vs_main_common", 0, "vs_3_0");

	CreateRenderTargets();
}

void ZED3D9BlurMaskProcessor::Deinitialize()
{
	Input = NULL;
	Output = NULL;

	ZED3D_RELEASE(PixelShaderBlur);
	ZED3D_RELEASE(PixelShaderDownUpSample);
	ZED3D_RELEASE(PixelShaderBlurMask);
	ZED3D_RELEASE(VertexShaderCommon);
	ZED3D_RELEASE(VertexDeclaration);

	DestroyRenderTargets();
}

void ZED3D9BlurMaskProcessor::SetRenderer(ZEFrameRenderer* Renderer)
{
	this->Renderer = (ZED3D9FrameRenderer*)Renderer;
}

ZEFrameRenderer* ZED3D9BlurMaskProcessor::GetRenderer() const
{
	return (ZEFrameRenderer*)Renderer;
}

void ZED3D9BlurMaskProcessor::SetBlurFactor(float Factor)
{
	BlurFactor = Factor;
}

float ZED3D9BlurMaskProcessor::GetBlurFactor()
{
	return BlurFactor;
}

void ZED3D9BlurMaskProcessor::SetBlurColor(const ZEVector3& Color)
{
	BlurColor = Color;
}

ZEVector3 ZED3D9BlurMaskProcessor::GetBlurColor()
{
	return BlurColor;
}

bool ZED3D9BlurMaskProcessor::SetBlurMaskTexture(const ZEString& Path)
{
	static ZETextureOptions MaskOptions = {ZE_TCT_AUTO, ZE_TCQ_HIGH, ZE_TDS_NONE, ZE_TFC_DISABLED, ZE_TMM_DISABLED, 1};
	BlurMaskTexture = ZETexture2DResource::LoadResource(Path, &MaskOptions)->GetTexture();

	return BlurMaskTexture != NULL;
}

const ZETexture2D* ZED3D9BlurMaskProcessor::GetBlurMaskTexture() const
{
	return BlurMaskTexture;
}

void ZED3D9BlurMaskProcessor::SetInput(ZED3D9Texture2D* Texture)
{
	Input = Texture;
}

ZED3D9Texture2D* ZED3D9BlurMaskProcessor::GetInput()
{
	return Input;
}

void ZED3D9BlurMaskProcessor::SetOutput(ZED3D9ViewPort* Texture)
{
	Output = Texture;
}

ZED3D9ViewPort* ZED3D9BlurMaskProcessor::GetOutput()
{
	return Output;
}

void ZED3D9BlurMaskProcessor::Process()
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
	
	/************************************************************************/
	/*						Pass 1: Down Sample                             */
	/************************************************************************/
	struct VertexShaderParametersCommon
	{
		float	PixelSizeInput[2];

		float	Reserved0;
		float	Reserved1;

	} VSParametersP1 = {

		{1.0f / (float)DS2x->GetWidth(), 1.0f / (float)DS2x->GetHeight()}, 0.0f, 0.0f
	};

	GetDevice()->SetVertexShader(VertexShaderCommon->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderDownUpSample->GetPixelShader());

	ZED3D9CommonTools::SetRenderTarget(0, DS2x);
	ZED3D9CommonTools::SetTexture(0, Input, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP);

	GetDevice()->SetVertexShaderConstantF(0, (const float*)&VSParametersP1, sizeof(VertexShaderParametersCommon) / 16);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

	/************************************************************************/
	/*						Pass 2: Blur	                                */
	/************************************************************************/

	VertexShaderParametersCommon VSParametersP2 = {

		{1.0f / (float)DS2xBlurred->GetWidth(), 1.0f / (float)DS2xBlurred->GetHeight()}, 0.0f, 0.0f
	};

	GetDevice()->SetVertexShader(VertexShaderCommon->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderBlur->GetPixelShader());

	ZED3D9CommonTools::SetRenderTarget(0, DS2xBlurred);
	ZED3D9CommonTools::SetTexture(0, DS2x, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP);

	GetDevice()->SetVertexShaderConstantF(0, (const float*)&VSParametersP2, sizeof(VertexShaderParametersCommon) / 16);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&VSParametersP2, sizeof(VertexShaderParametersCommon) / 16);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));



	/************************************************************************/
	/*						Pass 3: Up Sample                               */
	/************************************************************************/

	VertexShaderParametersCommon VSParametersP3 = {

		{1.0f / (float)DS2xBlurredUS2x->GetWidth(), 1.0f / (float)DS2xBlurredUS2x->GetHeight()}, 0.0f, 0.0f
	};

	GetDevice()->SetVertexShader(VertexShaderCommon->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderDownUpSample->GetPixelShader());

	ZED3D9CommonTools::SetRenderTarget(0, DS2xBlurredUS2x);
	ZED3D9CommonTools::SetTexture(0, DS2xBlurred, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP);

	GetDevice()->SetVertexShaderConstantF(0, (const float*)&VSParametersP2, sizeof(VertexShaderParametersCommon) / 16);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));



	/************************************************************************/
	/*							Pass 4: Mask                                */
	/************************************************************************/

	VertexShaderParametersCommon VSParametersP4 = {

		{1.0f / (float)Output->GetWidth(), 1.0f / (float)Output->GetHeight()}, 0.0f, 0.0f
	};
	
	struct PixelSahderParametersP4
	{
		float	Factor;
		float	Color[3];

	} PSParametersP4 = {

		BlurFactor, {BlurColor.x, BlurColor.y, BlurColor.z}
	};

	GetDevice()->SetVertexShader(VertexShaderCommon->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderBlurMask->GetPixelShader());

	ZED3D9CommonTools::SetRenderTarget(0, Output);
	ZED3D9CommonTools::SetTexture(0, Input, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(1, BlurMaskTexture, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(2, DS2xBlurredUS2x, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP);

	GetDevice()->SetVertexShaderConstantF(0, (const float*)&VSParametersP4, sizeof(VertexShaderParametersCommon) / 16);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&PSParametersP4, sizeof(PixelSahderParametersP4) / 16);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
	
}
