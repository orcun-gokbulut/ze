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

#include "ZERandom.h"
#include "ZED3D9Module.h"
#include "ZED3D9Shader.h"
#include "ZECore/ZECore.h"
#include "ZEMath/ZEAngle.h"
#include "ZED3D9Texture2D.h"
#include "ZECore/ZEConsole.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9FrameRenderer.h"
#include "ZED3D9SSAOProcessor.h"
#include "ZEGraphics/ZECamera.h"

#include "ZEFile/ZEFile.h"
#include "ZEMath/ZEMath.h"

static ZEVector4 RandomRotations[16] = 
{
	ZEVector4(-0.75f,+0.75f,+0.5f,1.0f), ZEVector4(-0.25f,+0.75f,+0.5f,1.0f), ZEVector4(+0.25f,+0.75f,+0.5f,1.0f), ZEVector4(+0.75f,+0.75f,+0.5f,1.0f),
	ZEVector4(-0.75f,+0.25f,+0.5f,1.0f), ZEVector4(-0.25f,+0.25f,+0.5f,1.0f), ZEVector4(+0.25f,+0.25f,+0.5f,1.0f), ZEVector4(+0.75f,+0.25f,+0.5f,1.0f),
	ZEVector4(-0.75f,-0.25f,+0.5f,1.0f), ZEVector4(-0.25f,-0.25f,+0.5f,1.0f), ZEVector4(+0.25f,-0.25f,+0.5f,1.0f), ZEVector4(+0.75f,-0.25f,+0.5f,1.0f),
	ZEVector4(-0.75f,-0.75f,+0.5f,1.0f), ZEVector4(-0.25f,-0.75f,+0.5f,1.0f), ZEVector4(+0.25f,-0.75f,+0.5f,1.0f), ZEVector4(+0.75f,-0.75f,+0.5f,1.0f),
};

#define COLOR_RED	D3DCOLORWRITEENABLE_RED
#define COLOR_ALL	D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA

class HaltonSequence
{
	private:
		double Value;
		double InverseBase;

	public:
		double GetNext()
		{
			double Radius = 1.0 - Value - 0.0000000001;
			if (InverseBase < Radius) 
			{
				Value += InverseBase;
			}
			else
			{
				double h = InverseBase, hh;
				do
				{
					hh = h;
					h *= InverseBase;

				} while ( h >= Radius );
				
				Value += hh + h - 1.0;
			}

			return Value;
		}

		HaltonSequence(double Base)
		{
			Value = 0;
			InverseBase = 1.0f / Base;
		}

		~HaltonSequence()
		{

		}
};

void ZED3D9SSAOProcessor::UpdateFilterKernel()
{	
	if (!UpdateKernel)
		return;

	HaltonSequence	Halton1(2.0);
	HaltonSequence	Halton2(3.0);
	HaltonSequence	Halton3(5.0);

	ZESize I = 0;
	while(I < ZE_SSAO_MAX_SAMPLE_COUNT)
	{

		double x = Halton1.GetNext() * 2.0 - 1.0;
		double y = Halton2.GetNext() * 2.0 - 1.0;
			
		double xx = x * x;
		double yy = y * y;

		if(sqrt(xx + yy) > 1.0)
			continue;

		double z, w;
		z = w = sqrt(1.0 - xx - yy);

		double s = Halton3.GetNext();

		SampleOffsets[I].x = (float)x;
		SampleOffsets[I].y = (float)y;
		SampleOffsets[I].z = (float)z;
		
		SampleOffsets[I] *= (float)(s * s);
		SampleOffsets[I].w = 1.0f;

		ZEVector3 Normalized = SampleOffsets[I].ToVector3().Normalize();
		float Cosine = ZEVector3::DotProduct(ZEVector3::UnitZ, Normalized);
		if (Cosine < AngleBias)
			continue;

		I++;
	}
	
	UpdateKernel = false;
}

void ZED3D9SSAOProcessor::UpdateBuffers()
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
		RandomAngles->Create(4, 4, 1, ZE_TPF_F32_4, false);
		
		ZESize Pitch = 0;
		float* Data = NULL;
		RandomAngles->Lock((void**)&Data, &Pitch, 0);
		
		for (ZESize I = 0; I < 4 * 4 * 4; I += 4)
		{
			RandomRotations[I / 4].NormalizeSelf();
			Data[I+0] =	RandomRotations[I / 4].x;
			Data[I+1] = RandomRotations[I / 4].y;
			Data[I+2] = RandomRotations[I / 4].z;
			Data[I+3] = RandomRotations[I / 4].w;
		}
		
		RandomAngles->Unlock(0);
	}
}

void ZED3D9SSAOProcessor::DestroyBuffers()
{
	ZED3D_RELEASE(TempBuffer);
	ZED3D_RELEASE(RandomAngles);
}

void ZED3D9SSAOProcessor::UpdateShaders()
{
	if (Shaders.Vertex == NULL)
	{
		Shaders.Vertex = ZED3D9VertexShader::CreateShader("SSAOProcessor.hlsl", "VSMainCommon", 0, "vs_3_0");
	}
	if(Shaders.AmbientOcclusion == NULL)
	{
		Shaders.AmbientOcclusion = ZED3D9PixelShader::CreateShader("SSAOProcessor.hlsl", "PSMainAmbientOcclusion", 0, "ps_3_0");
	}
	if(Shaders.BiliteralHorizontal == NULL)
	{
		Shaders.BiliteralHorizontal = ZED3D9PixelShader::CreateShader("SSAOProcessor.hlsl", "PSMainBlurHorizontal", 0, "ps_3_0");
	}
	if(Shaders.BiliteralVertical == NULL)
	{
		Shaders.BiliteralVertical = ZED3D9PixelShader::CreateShader("SSAOProcessor.hlsl", "PSMainBlurVertical", 0, "ps_3_0");
	}
}

void ZED3D9SSAOProcessor::DestroyShaders()
{
	ZED3D_RELEASE(Shaders.Vertex);
	ZED3D_RELEASE(Shaders.AmbientOcclusion);
	ZED3D_RELEASE(Shaders.BiliteralHorizontal);
	ZED3D_RELEASE(Shaders.BiliteralVertical);
}

void ZED3D9SSAOProcessor::UpdateStates()
{
	D3DPERF_BeginEvent(0, L"State Setup");

	float Fov = Renderer->GetCamera()->GetHorizontalFOV();
	float Width = (float)Output->GetWidth();
	float Height = (float)Output->GetHeight();
	
	ZEVector4 ParameterPixel = ZEVector4(1.0f / Width, 1.0f / Height, Width / 4.0f, Height / 4.0f);
	
	ZEVector4 ParameterSSAO = ZEVector4(SampleRadius, Contrast, 0.0f, 0.0f);

	float BlurFalloff = 1.0f / (((BlurRadius+1.0f) * (BlurRadius+1.0f)) / 2.0f);
	ZEVector4 ParametersBlur = ZEVector4(BlurRadius, BlurFalloff, BlurSharpness, BlurEdgeThreshold);

	ZEMatrix4x4 ProjectionMatrix;
	ZEMatrix4x4 InvProjectionMatrix;

	ProjectionMatrix = Renderer->GetCamera()->GetProjectionTransform();
	ZEMatrix4x4::Inverse(InvProjectionMatrix, ProjectionMatrix);

	GetDevice()->SetVertexShaderConstantF(0, ParameterPixel.M, 1);
	GetDevice()->SetVertexShaderConstantF(1, InvProjectionMatrix.MA, 4);

	GetDevice()->SetPixelShaderConstantF(0, ParameterPixel.M, 1);
	GetDevice()->SetPixelShaderConstantF(1, ParametersBlur.M, 1);
	GetDevice()->SetPixelShaderConstantF(2, ParameterSSAO.M, 1);
	GetDevice()->SetPixelShaderConstantF(3, ProjectionMatrix.MA, 4);
	GetDevice()->SetPixelShaderConstantF(7, SampleOffsets->M, SampleCount);

	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Depth Buffer
	// Prevent false occlusion near screen edge by setting bordercolor to max
	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
	//! Use FLOAT_MAX as border color in d3d10

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

	D3DPERF_EndEvent();
}

void ZED3D9SSAOProcessor::AmbientOcclusion(const ZED3D9Texture2D* InputDepth, const ZED3D9Texture2D* InputNormal, ZED3D9ViewPort* OutputBuffer)
{
	D3DPERF_BeginEvent(0, L"Ambient Occlusion Pass");

	GetDevice()->SetTexture(0, InputDepth->Texture);
	GetDevice()->SetTexture(1, InputNormal->Texture);
	GetDevice()->SetTexture(3, RandomAngles->Texture);
	GetDevice()->SetRenderTarget(0, OutputBuffer->FrameBuffer);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_RED);

	GetDevice()->SetPixelShader(Shaders.AmbientOcclusion->GetPixelShader());
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEHBAOScreenAlignedQuad));

	D3DPERF_EndEvent();
}

void ZED3D9SSAOProcessor::BiliteralFilterVertical(const ZED3D9Texture2D* Input, ZED3D9ViewPort* Output)
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

void ZED3D9SSAOProcessor::BiliteralFilterHorizontal(const ZED3D9Texture2D* Input, ZED3D9ViewPort* Output)
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

void ZED3D9SSAOProcessor::SetEnabled(bool Value)
{
	Enabled = Value;
}

bool ZED3D9SSAOProcessor::GetEnabled() const
{
	return Enabled;
}

void ZED3D9SSAOProcessor::SetContrast(float Value)
{
	Contrast = Value;
}

float ZED3D9SSAOProcessor::GetContrast() const
{
	return Contrast;
}

void ZED3D9SSAOProcessor::SetAngleBias(float Value)
{
	AngleBias = ZEMath::Saturate(Value);
	UpdateKernel = true;
}

float ZED3D9SSAOProcessor::GetAngleBias() const
{
	return AngleBias;
}

void ZED3D9SSAOProcessor::SetSampleCount(ZEUInt Value)
{	
	SampleCount = ZEMath::Clamp(Value, 0U, 32U);
}

ZEUInt ZED3D9SSAOProcessor::GetSampleCount() const
{
	return SampleCount;
}

void ZED3D9SSAOProcessor::SetSampleRadius(float Value)
{
	SampleRadius = Value;
}

float ZED3D9SSAOProcessor::GetSampleRadius() const
{
	return SampleRadius;
}

void ZED3D9SSAOProcessor::SetBlurRadius(float Value)
{
	BlurRadius = Value;
}

float ZED3D9SSAOProcessor::GetBlurRadius() const
{
	return BlurRadius;
}

void ZED3D9SSAOProcessor::SetBlurSharpness(float Value)
{
	BlurSharpness = Value;
}

float ZED3D9SSAOProcessor::GetBlurSharpness() const
{
	return BlurSharpness;
}

void ZED3D9SSAOProcessor::SetBlurEdgeThreshold(float Value)
{
	BlurEdgeThreshold = Value;
}

float ZED3D9SSAOProcessor::GetBlurEdgeThreshold() const
{
	return BlurEdgeThreshold;
}

void ZED3D9SSAOProcessor::SetInputDepth(ZED3D9Texture2D* InputBuffer)
{
	zeDebugCheck(InputBuffer == NULL, "Null pointer");

	InputDepth = InputBuffer;
}

ZED3D9Texture2D* ZED3D9SSAOProcessor::GetInputDepth()
{
	return InputDepth;
}

void ZED3D9SSAOProcessor::SetInputNormal(ZED3D9Texture2D* InputBuffer)
{
	zeDebugCheck(InputBuffer == NULL, "Null pointer");

	InputNormal = InputBuffer;
}

ZED3D9Texture2D* ZED3D9SSAOProcessor::GetInputNormal()
{
	return InputNormal;
}

void ZED3D9SSAOProcessor::SetOutput(ZED3D9Texture2D* OutputBuffer)
{
	zeDebugCheck(OutputBuffer == NULL, "Null pointer");

	Output = OutputBuffer;
}

ZED3D9Texture2D* ZED3D9SSAOProcessor::GetOutput()
{
	return Output;
}

void ZED3D9SSAOProcessor::SetRenderer(ZED3D9FrameRenderer* FrameRenderer)
{
	Renderer = FrameRenderer;
}

ZED3D9FrameRenderer* ZED3D9SSAOProcessor::SetRenderer() const
{
	return Renderer;
}

void ZED3D9SSAOProcessor::Process()
{
	if (!Enabled)
		return;

	if (Renderer == NULL || Output == NULL)
		return;

	if (InputNormal == NULL || InputDepth == NULL)
		return;

	UpdateFilterKernel();
	UpdateShaders();
	UpdateBuffers();
	UpdateStates();
	
	AmbientOcclusion(InputDepth, InputNormal, &Output->ViewPort);

	BiliteralFilterHorizontal(Output, &TempBuffer->ViewPort);
	BiliteralFilterVertical(TempBuffer, &Output->ViewPort);
}

void ZED3D9SSAOProcessor::Initialize()
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

void ZED3D9SSAOProcessor::Deinitialize()
{
	DestroyBuffers();
	DestroyShaders();
}

ZED3D9SSAOProcessor::~ZED3D9SSAOProcessor()
{
	Deinitialize();
}

ZESSAOScreenAlignedQuad ZED3D9SSAOProcessor::Vertices[4] =
{
	{{-1.0f,  1.0f, 1.0f}, {0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 1.0f}, {1.0f, 0.0f}},
	{{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 1.0f}, {1.0f, 1.0f}}
};

ZED3D9SSAOProcessor::ZED3D9SSAOProcessor()
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
	Shaders.AmbientOcclusion = NULL;
	Shaders.BiliteralVertical = NULL;
	Shaders.BiliteralHorizontal = NULL;
	
	VertexDeclaration = NULL;

	Enabled = true;
	UpdateKernel = true;
	SampleRadius = 1.0f;
	SampleCount = 16;
	AngleBias = 0.075f;
	Contrast = 5.0f;

	BlurRadius = 7;
	BlurSharpness = 32;
	BlurEdgeThreshold = 0.05f;
}
