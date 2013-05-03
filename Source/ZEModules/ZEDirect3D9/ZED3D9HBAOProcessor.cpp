//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9HBAOProcessor.cpp
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

#include "ZERandom.h"
#include "ZED3D9Module.h"
#include "ZED3D9Shader.h"
#include "ZECore/ZECore.h"
#include "ZEMath/ZEAngle.h"
#include "ZED3D9Texture2D.h"
#include "ZECore/ZEConsole.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9FrameRenderer.h"
#include "ZED3D9HBAOProcessor.h"
#include "ZEGraphics/ZECamera.h"

#define COLOR_RED	D3DCOLORWRITEENABLE_RED
#define COLOR_ALL	D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA

void ZED3D9HBAOProcessor::UpdateBuffers()
{
	ZED3D9Texture2D* SSAOBuffer = Renderer->SSAOBuffer;

	if (HalfResDepth == NULL || HalfResDepth->GetWidth() != (InputDepth->GetWidth() / 2) || HalfResDepth->GetHeight() != (InputDepth->GetHeight() / 2))
	{
		ZED3D_RELEASE(HalfResDepth);

		HalfResDepth = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		HalfResDepth->Create(InputDepth->GetWidth() / 2, InputDepth->GetHeight() / 2, 1, ZE_TPF_F32, true);
	}

	if (HalfResNormal == NULL || HalfResNormal->GetWidth() != (InputNormal->GetWidth() / 2) || HalfResNormal->GetHeight() != (InputNormal->GetHeight() / 2))
	{
		ZED3D_RELEASE(HalfResNormal);

		HalfResNormal = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		HalfResNormal->Create(InputNormal->GetWidth() / 2, InputNormal->GetHeight() / 2, 1, ZE_TPF_I8_4, true);
	}

	if (TempBuffer == NULL || TempBuffer->GetWidth() != SSAOBuffer->GetWidth() || TempBuffer->GetHeight() != SSAOBuffer->GetHeight())
	{
		ZED3D_RELEASE(TempBuffer);

		TempBuffer = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		TempBuffer->Create(SSAOBuffer->GetWidth(), SSAOBuffer->GetHeight(), 1, ZE_TPF_F16, true);
	}
	
	if (RandomAngles == NULL)
	{
		RandomAngles = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		RandomAngles->Create(64, 64, 1, ZE_TPF_F32_4, false);
		
		ZESize Pitch = 0;
		float* Data = NULL;
		RandomAngles->Lock((void**)&Data, &Pitch, 0);
		
		for (ZESize I = 0; I < 64 * 64 * 4; I += 4)
		{
			float Angle = 2.0f * ZE_PI * ZERandom::GetFloatPositive() / (float)DirectionCount;
			Data[I+0] = ZEAngle::Cos(Angle);
			Data[I+1] = ZEAngle::Sin(Angle);
			Data[I+2] = ZERandom::GetFloatPositive();
			Data[I+3] = 0.0f;
		}
		
		RandomAngles->Unlock(0);
	}
}

void ZED3D9HBAOProcessor::DestroyBuffers()
{
	ZED3D_RELEASE(TempBuffer);
	ZED3D_RELEASE(RandomAngles);
}

void ZED3D9HBAOProcessor::UpdateShaders()
{
	if (Shaders.Vertex == NULL)
	{
		Shaders.Vertex = ZED3D9VertexShader::CreateShader("HBAOProcessor.hlsl", "VSMainCommon", 0, "vs_3_0");
	}
	if(Shaders.DownSampleDepth == NULL)
	{
		Shaders.DownSampleDepth = ZED3D9PixelShader::CreateShader("HBAOProcessor.hlsl", "PSMainDownSampleDepth", 0, "ps_3_0");
	}
	if(Shaders.DownSampleNormal == NULL)
	{
		Shaders.DownSampleNormal = ZED3D9PixelShader::CreateShader("HBAOProcessor.hlsl", "PSMainDownSampleNormal", 0, "ps_3_0");
	}
	if(Shaders.AmbientOcclusion == NULL)
	{
		Shaders.AmbientOcclusion = ZED3D9PixelShader::CreateShader("HBAOProcessor.hlsl", "PSMainAmbientOcclusion", 0, "ps_3_0");
	}
	if(Shaders.BiliteralHorizontal == NULL)
	{
		Shaders.BiliteralHorizontal = ZED3D9PixelShader::CreateShader("HBAOProcessor.hlsl", "PSMainBlurHorizontal", 0, "ps_3_0");
	}
	if(Shaders.BiliteralVertical == NULL)
	{
		Shaders.BiliteralVertical = ZED3D9PixelShader::CreateShader("HBAOProcessor.hlsl", "PSMainBlurVertical", 0, "ps_3_0");
	}
}

void ZED3D9HBAOProcessor::DestroyShaders()
{
	ZED3D_RELEASE(Shaders.Vertex);
	ZED3D_RELEASE(Shaders.DownSampleDepth);
	ZED3D_RELEASE(Shaders.DownSampleNormal);
	ZED3D_RELEASE(Shaders.AmbientOcclusion);
	ZED3D_RELEASE(Shaders.BiliteralHorizontal);
	ZED3D_RELEASE(Shaders.BiliteralVertical);
}

void ZED3D9HBAOProcessor::UpdateStates()
{
	float Fov = Renderer->GetCamera()->GetFOV();
	float Width = Output->GetWidth();
	float Height = Output->GetHeight();

	float FocalWidth = 1.0f / ZEAngle::Tan(Fov * 0.5f) *  Height / Width;
	float FocalHeight = 1.0f / ZEAngle::Tan(Fov * 0.5f);

	ZEVector4 ParameterRadius = ZEVector4(OcclusionRadius * RadiusMultiplier, 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterRadiusInv = ZEVector4(1.0f / ParameterRadius.x, 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterRadiusSquare = ZEVector4(ParameterRadius.x * ParameterRadius.x, 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterAngleBias = ZEVector4(AngleBias, 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterAngleBiasTan = ZEVector4(ZEAngle::Tan(AngleBias), 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterContrast = ZEVector4(Contrast / (1.0f - ZEAngle::Sin(AngleBias)), 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterFocalLenght = ZEVector4(FocalWidth, FocalHeight, 0.0f, 0.0f);
	ZEVector4 ParameterFocalLenghtInv = ZEVector4(1.0f / FocalWidth, 1.0f / FocalHeight, 0.0f, 0.0f);
	ZEVector4 ParameterResolution = ZEVector4(Width, Height, 0.0f, 0.0f);
	ZEVector4 ParameterPixelSize = ZEVector4(1.0f / Width, 1.0f / Height, 0.0f, 0.0f);
	ZEVector4 ParameterAttenuation = ZEVector4(Attenuation, 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterRandomResolution = ZEVector4(64.0f, 64.0f, 0.0f, 0.0f);
	
	ZEVector4 ParameterBlurRadius = ZEVector4(BlurRadius, 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterBlurFalloff = ZEVector4(1.0f / (((BlurRadius+1.0f) * (BlurRadius+1.0f)) / 2.0f), 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterBlurSharpness = ZEVector4(BlurSharpness, 0.0f, 0.0f, 0.0f);
	ZEVector4 ParameterBlurEdgeThreshold = ZEVector4(BlurEdgeThreshold, 0.0f, 0.0f, 0.0f);


	GetDevice()->SetVertexShaderConstantF(0, ParameterPixelSize.M, 1);
	GetDevice()->SetVertexShaderConstantF(9, ParameterResolution.M, 1);
	GetDevice()->SetVertexShaderConstantF(11, ParameterRandomResolution.M, 1);
	
	GetDevice()->SetPixelShaderConstantF(0, ParameterPixelSize.M, 1);
	GetDevice()->SetPixelShaderConstantF(1, ParameterRadius.M, 1);
	GetDevice()->SetPixelShaderConstantF(2, ParameterRadiusInv.M, 1);
	GetDevice()->SetPixelShaderConstantF(3, ParameterRadiusSquare.M, 1);
	GetDevice()->SetPixelShaderConstantF(4, ParameterAngleBias.M, 1);
	GetDevice()->SetPixelShaderConstantF(5, ParameterAngleBiasTan.M, 1);
	GetDevice()->SetPixelShaderConstantF(6, ParameterContrast.M, 1);
	GetDevice()->SetPixelShaderConstantF(7, ParameterFocalLenght.M, 1);
	GetDevice()->SetPixelShaderConstantF(8, ParameterFocalLenghtInv.M, 1);
	GetDevice()->SetPixelShaderConstantF(9, ParameterResolution.M, 1);
	GetDevice()->SetPixelShaderConstantF(10, ParameterAttenuation.M, 1);

	GetDevice()->SetPixelShaderConstantF(12, ParameterBlurRadius.M, 1);
	GetDevice()->SetPixelShaderConstantF(13, ParameterBlurFalloff.M, 1);
	GetDevice()->SetPixelShaderConstantF(14, ParameterBlurSharpness.M, 1);
	GetDevice()->SetPixelShaderConstantF(15, ParameterBlurEdgeThreshold.M, 1);

	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Depth Buffer
	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

	// Normal Buffer
	GetDevice()->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(1, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

	// Random Texture
	GetDevice()->SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(3, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	GetDevice()->SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	GetDevice()->SetSamplerState(3, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);

	// SSAO Texture
	GetDevice()->SetSamplerState(4, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(4, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(4, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(4, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(4, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(4, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	GetDevice()->SetVertexShader(Shaders.Vertex->GetVertexShader());
}

void ZED3D9HBAOProcessor::DownSampleDepth(const ZED3D9Texture2D* Input, ZED3D9ViewPort* Output)
{
	D3DPERF_BeginEvent(0, L"Depth Down Sample Pass");

	GetDevice()->SetTexture(0, InputDepth->Texture);
	GetDevice()->SetRenderTarget(0, Output->FrameBuffer);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_RED);

	GetDevice()->SetPixelShader(Shaders.DownSampleDepth->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEHBAOScreenAlignedQuad));

	D3DPERF_EndEvent();
}

void ZED3D9HBAOProcessor::DownSampleNormal(const ZED3D9Texture2D* Input, ZED3D9ViewPort* Output)
{
	D3DPERF_BeginEvent(0, L"Normal Down Sample Pass");

	GetDevice()->SetTexture(1, InputNormal->Texture);
	GetDevice()->SetRenderTarget(0, Output->FrameBuffer);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_ALL);

	GetDevice()->SetPixelShader(Shaders.DownSampleNormal->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEHBAOScreenAlignedQuad));

	D3DPERF_EndEvent();
}

void ZED3D9HBAOProcessor::AmbientOcclusion(const ZED3D9Texture2D* InputDepth, const ZED3D9Texture2D* InputNormal, ZED3D9ViewPort* OutputBuffer)
{
	D3DPERF_BeginEvent(0, L"Ambient Occlusion Pass");

	GetDevice()->SetTexture(0, InputDepth->Texture);
	GetDevice()->SetTexture(1, InputNormal->Texture);
	GetDevice()->SetTexture(3, RandomAngles->Texture);
	GetDevice()->SetRenderTarget(0, OutputBuffer->FrameBuffer);
	GetDevice()->SetRenderTarget(1, NULL);
	GetDevice()->SetRenderTarget(2, NULL);
	GetDevice()->SetRenderTarget(3, NULL);

	GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET, 0xFFFFFFFF, 1.0f, 0x00);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_RED);

	GetDevice()->SetPixelShader(Shaders.AmbientOcclusion->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEHBAOScreenAlignedQuad));

	D3DPERF_EndEvent();
}

void ZED3D9HBAOProcessor::BiliteralFilterVertical(const ZED3D9Texture2D* Input, ZED3D9ViewPort* Output)
{
	D3DPERF_BeginEvent(0, L"Ambient Occlusion Blur Pass");

	GetDevice()->SetTexture(0, InputDepth->Texture);
	GetDevice()->SetTexture(4, Input->Texture);
	GetDevice()->SetRenderTarget(0, Output->FrameBuffer);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_RED);

	GetDevice()->SetPixelShader(Shaders.BiliteralVertical->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEHBAOScreenAlignedQuad));

	D3DPERF_EndEvent();
}

void ZED3D9HBAOProcessor::BiliteralFilterHorizontal(const ZED3D9Texture2D* Input, ZED3D9ViewPort* Output)
{
	D3DPERF_BeginEvent(0, L"Ambient Occlusion Blur Pass");

	GetDevice()->SetTexture(0, InputDepth->Texture);
	GetDevice()->SetTexture(4, Input->Texture);
	GetDevice()->SetRenderTarget(0, Output->FrameBuffer);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_RED);

	GetDevice()->SetPixelShader(Shaders.BiliteralHorizontal->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEHBAOScreenAlignedQuad));

	D3DPERF_EndEvent();
}

void ZED3D9HBAOProcessor::SetOcclusionRadius(float Value)
{
	OcclusionRadius = Value;
}

float ZED3D9HBAOProcessor::GetOcclusionRadius() const
{
	return OcclusionRadius;
}

void ZED3D9HBAOProcessor::SetRadiusMultiplier(float Value)
{
	RadiusMultiplier = Value;
}

float ZED3D9HBAOProcessor::GetRadiusMultiplier() const
{
	return RadiusMultiplier;
}

void ZED3D9HBAOProcessor::SetStepCount(ZEUInt Value)
{
	StepCount = Value;
}

ZEUInt ZED3D9HBAOProcessor::GetStepCount() const
{
	return StepCount;
}

void ZED3D9HBAOProcessor::SetDirectionCount(ZEUInt Value)
{
	DirectionCount = Value;
}

ZEUInt ZED3D9HBAOProcessor::GetDirectionCount() const
{
	return DirectionCount;
}

void ZED3D9HBAOProcessor::SetAngleBias(float Value)
{
	AngleBias = Value;
}

float ZED3D9HBAOProcessor::GetAngleBias() const
{
	return AngleBias;
}

void ZED3D9HBAOProcessor::SetAttenuation(float Value)
{
	Attenuation = Value;
}

float ZED3D9HBAOProcessor::GetAttenuation() const
{
	return Attenuation;
}

void ZED3D9HBAOProcessor::SetContrast(float Value)
{
	Contrast = Value;
}

float ZED3D9HBAOProcessor::GetContrast() const
{
	return Contrast;
}

void ZED3D9HBAOProcessor::SetBlurRadius(float Value)
{
	BlurRadius = Value;
}

float ZED3D9HBAOProcessor::GetBlurRadius() const
{
	return BlurRadius;
}

void ZED3D9HBAOProcessor::SetBlurSharpness(float Value)
{
	BlurSharpness = Value;
}

float ZED3D9HBAOProcessor::GetBlurSharpness() const
{
	return BlurSharpness;
}

void ZED3D9HBAOProcessor::SetBlurEdgeThreshold(float Value)
{
	BlurEdgeThreshold = Value;
}

float ZED3D9HBAOProcessor::GetBlurEdgeThreshold() const
{
	return BlurEdgeThreshold;
}

void ZED3D9HBAOProcessor::SetInputDepth(ZED3D9Texture2D* InputBuffer)
{
	zeDebugCheck(InputBuffer == NULL, "Null pointer");

	InputDepth = InputBuffer;
}

ZED3D9Texture2D* ZED3D9HBAOProcessor::GetInputDepth()
{
	return InputDepth;
}

void ZED3D9HBAOProcessor::SetInputNormal(ZED3D9Texture2D* InputBuffer)
{
	zeDebugCheck(InputBuffer == NULL, "Null pointer");

	InputNormal = InputBuffer;
}

ZED3D9Texture2D* ZED3D9HBAOProcessor::GetInputNormal()
{
	return InputNormal;
}

void ZED3D9HBAOProcessor::SetOutput(ZED3D9Texture2D* OutputBuffer)
{
	zeDebugCheck(OutputBuffer == NULL, "Null pointer");

	Output = OutputBuffer;
}

ZED3D9Texture2D* ZED3D9HBAOProcessor::GetOutput()
{
	return Output;
}

void ZED3D9HBAOProcessor::SetRenderer(ZED3D9FrameRenderer* FrameRenderer)
{
	Renderer = FrameRenderer;
}

ZED3D9FrameRenderer* ZED3D9HBAOProcessor::SetRenderer() const
{
	return Renderer;
}

void ZED3D9HBAOProcessor::Process()
{
	if (Renderer == NULL || Output == NULL)
		return;

	if (InputNormal == NULL || InputDepth == NULL)
		return;

	UpdateShaders();
	UpdateBuffers();
	UpdateStates();
	
	//DownSampleDepth(InputDepth, &HalfResDepth->ViewPort);
	//DownSampleNormal(InputNormal, &HalfResNormal->ViewPort);
	
	AmbientOcclusion(InputDepth, InputNormal, &Output->ViewPort);
	BiliteralFilterHorizontal(Output, &TempBuffer->ViewPort);
	BiliteralFilterVertical(TempBuffer, &Output->ViewPort);
}

void ZED3D9HBAOProcessor::Initialize()
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

void ZED3D9HBAOProcessor::Deinitialize()
{
	DestroyBuffers();
	DestroyShaders();
}

ZED3D9HBAOProcessor::~ZED3D9HBAOProcessor()
{
	Deinitialize();
}

ZEHBAOScreenAlignedQuad ZED3D9HBAOProcessor::Vertices[4] =
{
	{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}},
	{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}
};

ZED3D9HBAOProcessor::ZED3D9HBAOProcessor()
{
	Renderer = NULL;
	Output = NULL;
	InputDepth = NULL;
	InputNormal = NULL;
	
	TempBuffer = NULL;
	RandomAngles = NULL;
	HalfResDepth = NULL;
	HalfResNormal = NULL;

	Shaders.Vertex = NULL;
	Shaders.DownSampleDepth = NULL;
	Shaders.DownSampleNormal = NULL;
	Shaders.AmbientOcclusion = NULL;
	Shaders.BiliteralVertical = NULL;
	Shaders.BiliteralHorizontal = NULL;
	
	VertexDeclaration = NULL;

	OcclusionRadius	 = 1.0f;
	RadiusMultiplier = 2.0f;
	StepCount = 6;
	DirectionCount = 6;
	AngleBias = 0.785f;
	Attenuation = 1.0f;
	Contrast = 1.5f;

	BlurRadius = 7;
	BlurSharpness = 16;
	BlurEdgeThreshold = 0.1f;
}
