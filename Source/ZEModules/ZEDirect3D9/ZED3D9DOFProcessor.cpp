//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9DOFProcessor.cpp
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
#include "ZEGame/ZEScene.h"
#include "ZED3D9ViewPort.h"
#include "ZED3D9Profiler.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9DOFProcessor.h"
#include "ZED3D9BlurProcessor.h"
#include "ZED3D9FrameRenderer.h"
#include "ZERenderer/ZECamera.h"
#include "ZETexture/ZETexture2DResource.h"

#include <d3d9.h>
#include <stdlib.h>

void ZED3D9DOFProcessor::CreateRenderTargets()
{
	ZEUInt OutputWidth = Renderer->GetViewPort()->GetWidth();
	ZEUInt OutputHeight = Renderer->GetViewPort()->GetHeight();

	// Always allocate level 0
	if (BlurBuffers[0] == NULL || OutputWidth != BlurBuffers[0]->GetWidth() || OutputHeight != BlurBuffers[0]->GetHeight())
	{
		ZED3D_DESTROY(BlurBuffers[0]);
	
		BlurBuffers[0] = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		BlurBuffers[0]->Create(OutputWidth, OutputHeight, 1, ZE_TPF_I8_4, true);
	}

	// (Re)Allocate buffers
	for (ZESize I = 1; I < BlurPassCount + 1 && I < 9; ++I)
	{
		ZEUInt LevelWidth = OutputWidth >> I;
		ZEUInt LevelHeight = OutputHeight >> I;

		if (BlurBuffers[I] == NULL || LevelWidth != BlurBuffers[I]->GetWidth() || LevelHeight != BlurBuffers[I]->GetHeight())
		{
			ZED3D_DESTROY(BlurBuffers[I]);

			BlurBuffers[I] = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
			BlurBuffers[I]->Create(LevelWidth, LevelHeight, 1, ZE_TPF_I8_4, true);
		}
	}

	// deallocate unused buffers
	for (ZESize I = BlurPassCount + 1; I < 9; ++I)
	{
		ZED3D_DESTROY(BlurBuffers[I]);
	}
}

void ZED3D9DOFProcessor::UpDownSampleBlur(ZED3D9Texture2D* Input, ZED3D9Texture2D* Output)
{
	GetDevice()->SetPixelShader(PixelShaderUsDsBlur->GetPixelShader());
	GetDevice()->SetVertexShader(VertexShaderCommon->GetVertexShader());

	GetDevice()->SetRenderTarget(0, Output->ViewPort.FrameBuffer);

	GetDevice()->SetTexture(0, Input->Texture);
	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

	ZEVector4 PixelSize = ZEVector4(1.0f / Output->GetWidth(), 1.0f / Output->GetHeight(), 0.0f, 0.0f);

	GetDevice()->SetVertexShaderConstantF(0, PixelSize.M, 1);
	GetDevice()->SetPixelShaderConstantF(0, PixelSize.M, 1);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEDOFScreenAlignedQuad));
}

void ZED3D9DOFProcessor::SetBlurPassCount(ZEUInt Count)
{
	BlurPassCount = Count;
}

ZEUInt ZED3D9DOFProcessor::GetBlurPassCount() const
{
	return (ZEUInt)BlurPassCount;
}

void ZED3D9DOFProcessor::SetFocusDistance(float Value)
{
	FocusDistance = Value;
}

float ZED3D9DOFProcessor::GetFocusDistance() const
{
	return FocusDistance;
}

void ZED3D9DOFProcessor::SetFullFocusRange(float Value)
{
	FullFocusRange = Value;
}

float ZED3D9DOFProcessor::GetFullFocusRange() const
{
	return FullFocusRange;
}

void ZED3D9DOFProcessor::SetFarDistance(float Value)
{
	FarDistance = Value;
}

float ZED3D9DOFProcessor::GetFarDistance() const
{
	return FarDistance;
}

void ZED3D9DOFProcessor::SetNearDistance(float Value)
{
	NearDistance = Value;
}

float ZED3D9DOFProcessor::GetNearDistance() const
{
	return NearDistance;
}

void ZED3D9DOFProcessor::SetFarClamp(float Value)
{
	FarClamp = Value;
}

float ZED3D9DOFProcessor::GetFarClamp() const
{
	return FarClamp;
}

void ZED3D9DOFProcessor::SetNearClamp(float Value)
{
	NearClamp = Value;
}

float ZED3D9DOFProcessor::GetNearClamp() const
{
	return NearClamp;
}

void ZED3D9DOFProcessor::SetRenderer(ZEFrameRenderer* Renderer)
{
	this->Renderer = (ZED3D9FrameRenderer*)Renderer;
}

ZEFrameRenderer* ZED3D9DOFProcessor::GetRenderer()
{
	return Renderer;
}

void ZED3D9DOFProcessor::SetInputDepth(ZETexture2D* Texture)
{
	this->InputDepthBuffer = (ZED3D9Texture2D*)Texture;
}

ZETexture2D* ZED3D9DOFProcessor::GetInputDepth()
{
	return InputDepthBuffer;
}

void ZED3D9DOFProcessor::SetInputColor(ZETexture2D* Texture)
{
	this->InputColorBuffer = (ZED3D9Texture2D*)Texture;
}

ZETexture2D* ZED3D9DOFProcessor::GetInputColor()
{
	return InputColorBuffer;
}

void ZED3D9DOFProcessor::SetOutput(ZED3D9ViewPort* Texture)
{
	OutputBuffer = Texture;
}

ZED3D9ViewPort* ZED3D9DOFProcessor::GetOutput()
{
	return OutputBuffer;
}

void ZED3D9DOFProcessor::Initialize()
{
	// Vertex declaration for screen aligned quad
	D3DVERTEXELEMENT9 Declaration[] = 
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, D3DDECL_END()
	};

	GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

	// Compile Shaders
	this->VertexShaderCommon = ZED3D9VertexShader::CreateShader("DOFProcessor.hlsl", "vs_main_common", 0, "vs_3_0");

	this->PixelShaderDOF = ZED3D9PixelShader::CreateShader("DOFProcessor.hlsl", "ps_main_dof", 0, "ps_3_0");
	this->PixelShaderUsDsBlur = ZED3D9PixelShader::CreateShader("DOFProcessor.hlsl", "ps_main_down_up_sample_blur", 0, "ps_3_0");

	this->CreateRenderTargets();
}

void ZED3D9DOFProcessor::Deinitialize()
{
	Renderer			= NULL;

	InputColorBuffer	= NULL;
	InputDepthBuffer	= NULL;
	OutputBuffer		= NULL;

	for (ZESize I = 0; I < 8; ++I)
	{
		ZED3D_DESTROY(BlurBuffers[I]);
	}

	ZED3D_RELEASE(VertexShaderCommon);

	ZED3D_RELEASE(PixelShaderDOF);
	ZED3D_RELEASE(PixelShaderUsDsBlur);
}

void ZED3D9DOFProcessor::OnDeviceLost()
{

}

void ZED3D9DOFProcessor::OnDeviceRestored()
{

}

void ZED3D9DOFProcessor::Process()
{
	CreateRenderTargets();

	zeProfilerStart("DOF Pass");
	D3DPERF_BeginEvent(0, L"DOF Pass");

	GetDevice()->SetVertexDeclaration(VertexDeclaration);

	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GetDevice()->SetVertexShader(VertexShaderCommon->GetVertexShader());
	
	ZED3D9Texture2D* Input = InputColorBuffer;

	// DownSample and blur
	for (ZESize I = 0; I < BlurPassCount; ++I)
	{
		UpDownSampleBlur(Input, BlurBuffers[I+1]);

		Input = BlurBuffers[I + 1];
	}

	// Upsample and blur
	for (ZESize I = BlurPassCount; I > 0; --I)
	{
		UpDownSampleBlur(BlurBuffers[I], BlurBuffers[I - 1]);
	}

	// Switch to DOF shader
	GetDevice()->SetVertexShader(VertexShaderCommon->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderDOF->GetPixelShader());

	ZED3D9CommonTools::SetRenderTarget(0, OutputBuffer);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)InputColorBuffer, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(1, (ZETexture2D*)InputDepthBuffer, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(2, (ZETexture2D*)BlurBuffers[0], D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_MIRROR);

	struct DOFParameters
	{
		float		NearClamp;
		float		NearDistance;
		float		FarClamp;
		float		FarDistance;
		float		FocusDistance;
		float		FullFocusRange;
		float		Reserved1;
		float		Reserved2;

	} Parameters;

	Parameters.NearClamp = NearClamp;
	Parameters.NearDistance = NearDistance;
	Parameters.FarClamp = FarClamp;
	Parameters.FarDistance = FarDistance;
	Parameters.FocusDistance = FocusDistance;
	Parameters.FullFocusRange = FullFocusRange;

	GetDevice()->SetPixelShaderConstantF(0, (const float*)&Parameters, sizeof(DOFParameters) / 16);

	ZEVector4 PixelSize = ZEVector4(1.0f / OutputBuffer->GetWidth(), 1.0f / OutputBuffer->GetHeight(), 0.0f, 0.0f);
	GetDevice()->SetVertexShaderConstantF(0, PixelSize.M, 1);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEDOFScreenAlignedQuad));

	D3DPERF_EndEvent();
	zeProfilerEnd();
}

ZEDOFScreenAlignedQuad ZED3D9DOFProcessor::Vertices[] =
{
	{-1.0f,  1.0f, 0.0f},
	{ 1.0f,  1.0f, 0.0f},
	{-1.0f, -1.0f, 0.0f},
	{ 1.0f, -1.0f, 0.0f}
};

ZED3D9DOFProcessor::ZED3D9DOFProcessor()
{
	BlurPassCount		= 3;

	FocusDistance		= 40.0f;
	FullFocusRange		= 5.0f;
	FarDistance			= 80.0f;
	NearDistance		= 1.0f;
	FarClamp			= 1.0f;
	NearClamp			= 1.0f;
	
	Renderer			= NULL;
	VertexDeclaration	= NULL;
	VertexShaderCommon	= NULL;
	PixelShaderDOF		= NULL;
	PixelShaderDOF		= NULL;
	PixelShaderUsDsBlur	= NULL;

	InputColorBuffer	= NULL;
	InputDepthBuffer	= NULL;
	OutputBuffer		= NULL;

	for (ZESize I = 0; I < 9; ++I)
	{
		BlurBuffers[I] = NULL;
	}
}

ZED3D9DOFProcessor::~ZED3D9DOFProcessor()
{
	Deinitialize();
}
