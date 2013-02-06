//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderStageLighting.cpp
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

#include "ZELight.h"
#include "ZECanvas.h"
#include "ZECamera.h"
#include "ZERenderer.h"
#include "ZEPointLight.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEAngle.h"
#include "ZEProjectiveLight.h"
#include "ZEDirectionalLight.h"
#include "ZEGraphics/ZEShader.h"
#include "ZERenderStageGeometry.h"
#include "ZERenderStageLighting.h"
#include "ZEOmniProjectiveLight.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGraphics/ZETextureCube.h"
#include "ZEGraphics/ZERenderTarget.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEShaderCompiler.h"
#include "ZEGraphics/ZEShaderMetaTable.h"
#include "ZEGraphics/ZEGraphicsEventTracer.h"

struct VertexShaderBuffer
{
	ZEMatrix4x4	WorldMatrix;
	ZEMatrix4x4	WorldViewMatrix;
	ZEMatrix4x4	WorldViewProjMatrix;
};

struct PixelShaderBuffer
{
	ZEVector2		PixelSize;
	float			Dummy0[2];
	ZEVector3		Position;
	float			Range;
	ZEVector3		Color;
	float			Intensity;
	ZEVector3		Attenuation;
	float			Dummy1;
	ZEVector3		Direction;
	float			FOV;
	ZEMatrix4x4		ProjectionMatrix;
};

void ZERenderStageLighting::DestroyShaders()
{
	ZE_DESTROY(Shaders.PointLightVS);
	ZE_DESTROY(Shaders.PointLightPS);
	ZE_DESTROY(Shaders.ProjectiveLightVS);
	ZE_DESTROY(Shaders.ProjectiveLightPS);
	ZE_DESTROY(Shaders.DirectionalLightVS);
	ZE_DESTROY(Shaders.DirectionalLightPS);
	ZE_DESTROY(Shaders.OmniProjectiveLightVS);
	ZE_DESTROY(Shaders.OmniProjectiveLightPS);
}

void ZERenderStageLighting::UpdateShaders()
{
	ZEShaderCompileOptions Compileoptions;

	if (Shaders.PointLightVS == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_VERTEX;
		Compileoptions.FileName = "Lights.hlsl";
		Compileoptions.EntryPoint = "ZEPointLight_VertexShader";
		Shaders.PointLightVS = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (Shaders.PointLightPS == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_PIXEL;
		Compileoptions.FileName = "Lights.hlsl";
		Compileoptions.EntryPoint = "ZEPointLight_PixelShader";
		Shaders.PointLightPS = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (Shaders.DirectionalLightVS == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_VERTEX;
		Compileoptions.FileName = "Lights.hlsl";
		Compileoptions.EntryPoint = "ZEDirectionalLight_VertexShader";
		Shaders.DirectionalLightVS = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (Shaders.DirectionalLightPS == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_PIXEL;
		Compileoptions.FileName = "Lights.hlsl";
		Compileoptions.EntryPoint = "ZEDirectionalLight_PixelShader";
		Shaders.DirectionalLightPS = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (Shaders.ProjectiveLightVS == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_VERTEX;
		Compileoptions.FileName = "Lights.hlsl";
		Compileoptions.EntryPoint = "ZEProjectiveLight_VertexShader";
		Shaders.ProjectiveLightVS = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (Shaders.ProjectiveLightPS == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_PIXEL;
		Compileoptions.FileName = "Lights.hlsl";
		Compileoptions.EntryPoint = "ZEProjectiveLight_PixelShader";
		Shaders.ProjectiveLightPS = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (Shaders.OmniProjectiveLightVS == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_VERTEX;
		Compileoptions.FileName = "Lights.hlsl";
		Compileoptions.EntryPoint = "ZEOmniProjectiveLight_VertexShader";
		Shaders.OmniProjectiveLightVS = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (Shaders.OmniProjectiveLightPS == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_PIXEL;
		Compileoptions.FileName = "Lights.hlsl";
		Compileoptions.EntryPoint = "ZEOmniProjectiveLight_PixelShader";
		Shaders.OmniProjectiveLightPS = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
}

void ZERenderStageLighting::DestroyBuffers()
{
	ZE_DESTROY(RenderTargets.LBuffer1);
	ZE_DESTROY(Textures.LBuffer1);
	ZE_DESTROY(RenderTargets.LBuffer2);
	ZE_DESTROY(Textures.LBuffer2);

	ZE_DESTROY(Buffers.PixelShader);
	ZE_DESTROY(Buffers.VertexShader);
	ZE_DESTROY(Buffers.LightMeshVertexBuffer);
}

void ZERenderStageLighting::UpdateBuffers()
{
	// Update parent buffers
	ZERenderStage::UpdateBuffers();

	// Create Static Vertex Buffer
	if (Buffers.LightMeshVertexBuffer == NULL)
	{
		/*
		
		Primitive	Start		Start(Byte)	Vertex Count	Prim. Count		Size (Bytes)
		Quad		0			0			6				2				288
		Sphere12	6			288			936				312				44928
		Sphere24	942			45216		3600			1200			172800
		Cone12		4542		218016		18				6				864
		End			4650
		
		*/

		ZECanvas Canvas;

		// Directional Light
		Canvas.AddQuad(	ZEVector3( 1.0f,  1.0f, 1.0f),
						ZEVector3(-1.0f,  1.0f, 1.0f),
						ZEVector3(-1.0f, -1.0f, 1.0f),
						ZEVector3( 1.0f, -1.0f, 1.0f)	);

		// Point Light
		Canvas.AddSphere(1.0f, 12, 12);

		// OmniProjective Light
		Canvas.AddSphere(1.0f, 24, 24);

		// Projective Light
		Canvas.SetRotation(ZEQuaternion(-ZE_PI_2, ZEVector3::UnitX));
		Canvas.SetTranslation(ZEVector3::UnitZ);
		Canvas.AddPyramid(1.0f, 1.0f, 1.0f);

		Buffers.LightMeshVertexBuffer = Canvas.CreateStaticVertexBuffer();
	}
	
	if (Buffers.VertexShader == NULL)
	{
		Buffers.VertexShader = ZEConstantBuffer::CreateInstance();
		Buffers.VertexShader->Create(sizeof(VertexShaderBuffer));
	}
	
	if (Buffers.PixelShader == NULL)
	{
		Buffers.PixelShader = ZEConstantBuffer::CreateInstance();
		Buffers.PixelShader->Create(sizeof(PixelShaderBuffer));	
	}

	ZEUInt Width = zeGraphics->GetScreenWidth();
	ZEUInt Height = zeGraphics->GetScreenHeight();

	if (Textures.LBuffer1 == NULL || Textures.LBuffer1->GetWidth() != Width || Textures.LBuffer1->GetHeight() != Height)
	{
		ZE_DESTROY(RenderTargets.LBuffer1);
		ZE_DESTROY(Textures.LBuffer1);

		Textures.LBuffer1 = ZETexture2D::CreateInstance();
		Textures.LBuffer1->CreateStatic(Width, Height, 1, ZE_TPF_F16_4, true, NULL);
		RenderTargets.LBuffer1 = Textures.LBuffer1->CreateRenderTarget(0);
	}

	if (Textures.LBuffer2 == NULL || Textures.LBuffer2->GetWidth() != Width || Textures.LBuffer2->GetHeight() != Height)
	{
		ZE_DESTROY(RenderTargets.LBuffer2);
		ZE_DESTROY(Textures.LBuffer2);

		Textures.LBuffer2 = ZETexture2D::CreateInstance();
		Textures.LBuffer2->CreateStatic(Width, Height, 1, ZE_TPF_I8_4, true, NULL);
		RenderTargets.LBuffer2 = Textures.LBuffer2->CreateRenderTarget(0);
	}
}

void ZERenderStageLighting::ResetStageDefaults()
{
	// Requested pipeline constants
	StageConstants.SetCount(2);
	StageConstants.Add(ZE_PC_CAMERA_POS);
	StageConstants.Add(ZE_PC_INV_VIEWPORT_WIDTH_HEIGHT);

	// Reset parent states
	ZERenderStage::ResetStageDefaults();

	// Vertex buffer
	DefaultStates.VertexBuffers[0] = Buffers.LightMeshVertexBuffer;

	// Render targets	
	DefaultStates.RenderTargets[0] = RenderTargets.LBuffer1;
	DefaultStates.RenderTargets[1] = RenderTargets.LBuffer2;
	DefaultStates.DepthStencilBuffer = zeGraphics->GetDepthBuffer();

	// Use default viewport and scissor rectangles
	ZESize ScreenCount = zeGraphics->GetScreenCount();
	for (ZESize I = 0; I < ScreenCount; ++I)
	{
		DefaultStates.ViewPorts[I] = zeGraphics->GetViewport(I);
		DefaultStates.ScissorRectangles[I] = zeGraphics->GetScissorRectangle(I);
	}

	// Rasterizer state
	DefaultStates.RasterizerState.SetCullDirection(ZE_CD_CLOCKWISE);
	DefaultStates.RasterizerState.SetFillMode(ZE_FM_SOLID);

	// Sampler
	DefaultStates.PixelShaderSamplers[3].SetAddressU(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[3].SetAddressV(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[3].SetMinFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[3].SetMagFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[3].SetMipFilter(ZE_TFM_POINT);

	// Blend state
	DefaultStates.BlendState.SetBlendEnable(0, true);
	DefaultStates.BlendState.SetBlendEnable(1, true);
	DefaultStates.BlendState.SetColorChannelMask(0, ZE_CCM_ALL);
	DefaultStates.BlendState.SetColorChannelMask(1, ZE_CCM_ALL);
	DefaultStates.BlendState.SetBlendEquation(ZE_BE_ADD);
	DefaultStates.BlendState.SetSourceBlendOption(ZE_BO_ONE);
	DefaultStates.BlendState.SetDestinationBlendOption(ZE_BO_ONE);
	DefaultStates.BlendState.SetSourceBlendAlphaOption(ZE_BO_ONE);
	DefaultStates.BlendState.SetDestinationBlendAlphaOption(ZE_BO_ONE);

	// Depth stencil state
	DefaultStates.DepthStencilState.SetZTestEnable(true);
	DefaultStates.DepthStencilState.SetZFunction(ZE_CF_GREATER);
	DefaultStates.DepthStencilState.SetZWriteEnable(false);
	DefaultStates.DepthStencilState.SetStencilTestEnable(false);

	// Point Sampler State
	DefaultStates.PixelShaderSamplers[0].SetAddressU(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[0].SetAddressV(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[0].SetMagFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[0].SetMinFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[0].SetMipFilter(ZE_TFM_POINT);

	if (GBufferInput == NULL)
	{
		zeWarning("Input stage not set");
		return;
	}

	// Textures
	DefaultStates.PixelShaderTextures[0] = GBufferInput->GetGBuffer1();
	DefaultStates.PixelShaderTextures[1] = GBufferInput->GetGBuffer2();
	DefaultStates.PixelShaderTextures[2] = GBufferInput->GetGBuffer3();
}

void ZERenderStageLighting::CommitStageDefaults()
{
	// Commit parent states
	ZERenderStage::CommitStageDefaults();

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetRenderTargetArray(DefaultStates.RenderTargets);
	Device->SetDepthStencilBuffer(DefaultStates.DepthStencilBuffer);

	Device->SetBlendState(DefaultStates.BlendState);
	Device->SetDepthStencilState(DefaultStates.DepthStencilState);
	
	Device->SetRasterizerState(DefaultStates.RasterizerState);

	ZESize ScreenCount = zeGraphics->GetScreenCount();
	for (ZESize I = 0; I < ScreenCount; ++I)
	{
		Device->SetViewport(I, DefaultStates.ViewPorts[I]);
		Device->SetScissorRectangle(I, DefaultStates.ScissorRectangles[I]);
	}

	Device->SetPixelShaderSampler(0, DefaultStates.PixelShaderSamplers[0]);

	Device->SetPixelShaderTexture(0, DefaultStates.PixelShaderTextures[0]);
	Device->SetPixelShaderTexture(1, DefaultStates.PixelShaderTextures[1]);
	Device->SetPixelShaderTexture(2, DefaultStates.PixelShaderTextures[2]);

	Device->SetVertexBuffer(0, DefaultStates.VertexBuffers[0]);
}

void ZERenderStageLighting::RenderPointLight(const ZEPointLight* Light)
{
	float Range = Light->GetRange();
	ZEUInt ScreenWidth = zeGraphics->GetScreenWidth();
	ZEUInt ScreenHeight = zeGraphics->GetScreenHeight();
	ZECamera* Camera = zeScene->GetRenderer()->GetCamera();
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetVertexShader(Shaders.PointLightVS);
	Device->SetPixelShader(Shaders.PointLightPS);

	// VS Parameters
	VertexShaderBuffer* ParametersVS = NULL;
	Buffers.VertexShader->Lock((void**)&ParametersVS);
	
		ZEMatrix4x4::CreateOrientation(ParametersVS->WorldMatrix, Light->GetWorldPosition(), ZEQuaternion::Identity, ZEVector3(Range, Range, Range));
		ZEMatrix4x4::Multiply(ParametersVS->WorldViewMatrix, Camera->GetViewTransform(), ParametersVS->WorldMatrix);
		ZEMatrix4x4::Multiply(ParametersVS->WorldViewProjMatrix, Camera->GetViewProjectionTransform(), ParametersVS->WorldMatrix);
	
	Buffers.VertexShader->Unlock();
	Device->SetVertexShaderBuffer(0, Buffers.VertexShader);

	// PS parameters
	PixelShaderBuffer* ParametersPS = NULL;
	Buffers.PixelShader->Lock((void**)&ParametersPS);
	
		ParametersPS->Color = Light->GetColor();
		ParametersPS->Range = Light->GetRange();
		ParametersPS->Intensity = Light->GetIntensity();
		ParametersPS->Attenuation = Light->GetAttenuation();
		ZEMatrix4x4::Transform(ParametersPS->Position, Camera->GetViewTransform(), Light->GetWorldPosition());
		ParametersPS->PixelSize = ZEVector2(1.0f / (float)ScreenWidth, 1.0f / (float)ScreenHeight);
	
	Buffers.PixelShader->Unlock();
	Device->SetPixelShaderBuffer(0, Buffers.PixelShader);

	Device->Draw(ZE_ROPT_TRIANGLE_LIST, 936, 6); // Sphere
}

void ZERenderStageLighting::RenderProjectiveLight(const ZEProjectiveLight* Light)
{
	float FOV = Light->GetFOV();
	float Range = Light->GetRange();
	ZEUInt ScreenWidth = zeGraphics->GetScreenWidth();
	ZEUInt ScreenHeight = zeGraphics->GetScreenHeight();
	ZECamera* Camera = zeScene->GetRenderer()->GetCamera();
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetVertexShader(Shaders.ProjectiveLightVS);
	Device->SetPixelShader(Shaders.ProjectiveLightPS);

	// VS parameters
	VertexShaderBuffer* ParametersVS = NULL;
	Buffers.VertexShader->Lock((void**)&ParametersVS);
	
		float TanFovRange = ZEAngle::Tan(FOV * 0.5f) * Range;
		ZEMatrix4x4::CreateOrientation(ParametersVS->WorldMatrix, Light->GetWorldPosition(), Light->GetWorldRotation(), ZEVector3(TanFovRange * Light->GetAspectRatio() * 2.0f, TanFovRange * 2.0f, Range));
		ZEMatrix4x4::Multiply(ParametersVS->WorldViewMatrix, Camera->GetViewTransform(), ParametersVS->WorldMatrix);
		ZEMatrix4x4::Multiply(ParametersVS->WorldViewProjMatrix, Camera->GetViewProjectionTransform(), ParametersVS->WorldMatrix);
	
	Buffers.VertexShader->Unlock();
	Device->SetVertexShaderBuffer(0, Buffers.VertexShader);

	// PS parameters
	PixelShaderBuffer* ParametersPS = NULL;
	Buffers.PixelShader->Lock((void**)&ParametersPS);
	
		ZEMatrix4x4::Transform(ParametersPS->Position, Camera->GetViewTransform(), Light->GetWorldPosition());
		ParametersPS->Color = Light->GetColor();
		ParametersPS->Attenuation = Light->GetAttenuation();
		ParametersPS->Range = Range;
		ParametersPS->Intensity = Light->GetIntensity();
		ParametersPS->FOV = FOV;
		ZEMatrix4x4::Transform3x3(ParametersPS->Direction, Camera->GetViewTransform(), Light->GetWorldFront());
		ParametersPS->PixelSize = ZEVector2(1.0f / (float)ScreenWidth, 1.0f / (float)ScreenHeight);
	
		// Projection Transformation
		ZEMatrix4x4 LightViewMatrix;
		ZEMatrix4x4::CreateViewTransform(LightViewMatrix, Light->GetWorldPosition(), Light->GetWorldRotation());
		ZEMatrix4x4 LightProjectionMatrix;
		ZEMatrix4x4::CreatePerspectiveProjection(LightProjectionMatrix, FOV, Light->GetAspectRatio(), zeGraphics->GetNearZ(), Range);
		ZEMatrix4x4 LightViewProjectionMatrix;
		ZEMatrix4x4::Multiply(LightViewProjectionMatrix, LightProjectionMatrix, LightViewMatrix);
		ZEMatrix4x4 TextureMatrix;
		ZEMatrix4x4::Create(TextureMatrix,	0.5f,  0.0f, 0.0f, 0.5f,
											0.0f, -0.5f, 0.0f, 0.5f,
											0.0f,  0.0f, 1.0f, 0.0f,
											0.0f,  0.0f, 0.0f, 1.0f);

		ZEMatrix4x4 InvCameraViewMatrix;
		ZEMatrix4x4::Inverse(InvCameraViewMatrix, Camera->GetViewTransform());
		ZEMatrix4x4 LightSpaceMatrix;
		ZEMatrix4x4::Multiply(LightSpaceMatrix, LightViewProjectionMatrix, InvCameraViewMatrix);
		ZEMatrix4x4::Multiply(ParametersPS->ProjectionMatrix, TextureMatrix, LightSpaceMatrix);
	
	Buffers.PixelShader->Unlock();
	Device->SetPixelShaderBuffer(0, Buffers.PixelShader);

	ZEDeviceStateRasterizer RasterizerState;
	RasterizerState.SetFillMode(ZE_FM_SOLID);
	RasterizerState.SetCullDirection(ZE_CD_COUNTER_CLOCKWISE);
	Device->SetRasterizerState(RasterizerState);

	ZEDeviceStateBlend BlendState;
	BlendState.SetBlendEnable(0, false);
	BlendState.SetBlendEnable(1, false);
	BlendState.SetBlendEquation(ZE_BE_ADD);
	BlendState.SetSourceBlendOption(ZE_BO_ONE);
	BlendState.SetDestinationBlendOption(ZE_BO_ONE);
	BlendState.SetSourceBlendAlphaOption(ZE_BO_ONE);
	BlendState.SetDestinationBlendAlphaOption(ZE_BO_ONE);
	BlendState.SetColorChannelMask(0, ZE_CCM_NONE);
	BlendState.SetColorChannelMask(1, ZE_CCM_NONE);
	Device->SetBlendState(BlendState);

	ZEDeviceStateDepthStencil DepthStencilState;
	DepthStencilState.SetZWriteEnable(false);
	DepthStencilState.SetZTestEnable(true);
	DepthStencilState.SetZFunction(ZE_CF_LESS);
	DepthStencilState.SetStencilTestEnable(true);
	DepthStencilState.SetFrontStencilFunction(ZE_CF_ALWAYS);
	DepthStencilState.SetFrontZFail(ZE_SO_REPLACE);
	DepthStencilState.SetFrontStencilFail(ZE_SO_KEEP);
	DepthStencilState.SetFrontStencilPass(ZE_SO_KEEP);
	DepthStencilState.SetStencilReferance(StencilMask++);
	Device->SetDepthStencilState(DepthStencilState);

	ZEDeviceStateSampler LinearBorderSampler;
	LinearBorderSampler.SetMagFilter(ZE_TFM_LINEAR);
	LinearBorderSampler.SetMinFilter(ZE_TFM_LINEAR);
	LinearBorderSampler.SetMipFilter(ZE_TFM_LINEAR);
	LinearBorderSampler.SetAddressU(ZE_TAM_BORDER);
	LinearBorderSampler.SetAddressV(ZE_TAM_BORDER);
	LinearBorderSampler.SetAddressW(ZE_TAM_BORDER);
	LinearBorderSampler.SetBorderColor(ZEVector4(0.0f, 0.0f, 0.0f, 1.0f));

	Device->SetPixelShaderSampler(5, LinearBorderSampler);
	Device->SetPixelShaderTexture(5, Light->GetProjectionTexture());

	Device->Draw(ZE_ROPT_TRIANGLE_LIST, 18, 4542);  // Cone 2
	
	RasterizerState.SetCullDirection(ZE_CD_CLOCKWISE);
	Device->SetRasterizerState(RasterizerState);

	BlendState.SetBlendEnable(0, true);
	BlendState.SetBlendEnable(1, true);
	BlendState.SetColorChannelMask(0 , ZE_CCM_ALL);
	BlendState.SetColorChannelMask(1 , ZE_CCM_ALL);
	Device->SetBlendState(BlendState);

	DepthStencilState.SetZFunction(ZE_CF_GREATER);
	DepthStencilState.SetFrontStencilFunction(ZE_CF_NOT_EQUAL);
	DepthStencilState.SetFrontZFail(ZE_SO_KEEP);
	Device->SetDepthStencilState(DepthStencilState);

	Device->Draw(ZE_ROPT_TRIANGLE_LIST, 18, 4542);  // Cone 2
}

void ZERenderStageLighting::RenderDirectionalLight(const ZEDirectionalLight* Light)
{
	float Range = Light->GetRange();
	ZEUInt ScreenWidth = zeGraphics->GetScreenWidth();
	ZEUInt ScreenHeight = zeGraphics->GetScreenHeight();
	ZECamera* Camera = zeScene->GetRenderer()->GetCamera();
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetVertexShader(Shaders.DirectionalLightVS);
	Device->SetPixelShader(Shaders.DirectionalLightPS);

	// VS parameters
	VertexShaderBuffer* ParametersVS = NULL;
	Buffers.VertexShader->Lock((void**)&ParametersVS);
		
		ZEMatrix4x4 WorldViewTransform;
		ZEMatrix4x4::Multiply(ParametersVS->WorldViewMatrix, Camera->GetViewTransform(), Light->GetWorldTransform());
	
	Buffers.VertexShader->Unlock();
	Device->SetVertexShaderBuffer(0, Buffers.VertexShader);

	// PS Parameters
	PixelShaderBuffer* ParametersPS = NULL;
	Buffers.PixelShader->Lock((void**)&ParametersPS);
	
		ZEMatrix4x4::Transform3x3(ParametersPS->Direction, Camera->GetViewTransform(), Light->GetWorldFront());
		ParametersPS->Direction = -ParametersPS->Direction;
		ParametersPS->Color = Light->GetColor();
		ParametersPS->Attenuation = Light->GetAttenuation();
		ParametersPS->Intensity = Light->GetIntensity();
		ParametersPS->PixelSize = ZEVector2(1.0f / (float)ScreenWidth, 1.0f / (float)ScreenHeight);
	
	Buffers.PixelShader->Unlock();
	Device->SetPixelShaderBuffer(0, Buffers.PixelShader);

	Device->Draw(ZE_ROPT_TRIANGLE_LIST, 6, 0);	// Quad
}

void ZERenderStageLighting::RenderOmniProjectiveLight(const ZEOmniProjectiveLight* Light)
{
	float Range = Light->GetRange();
	ZEUInt ScreenWidth = zeGraphics->GetScreenWidth();
	ZEUInt ScreenHeight = zeGraphics->GetScreenHeight();
	ZECamera* Camera = zeScene->GetRenderer()->GetCamera();
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetVertexShader(Shaders.OmniProjectiveLightVS);
	Device->SetPixelShader(Shaders.OmniProjectiveLightPS);

	// VS parameters
	VertexShaderBuffer* ParametersVS = NULL;
	Buffers.VertexShader->Lock((void**)&ParametersVS);
	
		ZEMatrix4x4::CreateOrientation(ParametersVS->WorldMatrix, Light->GetWorldPosition(), ZEQuaternion::Identity, ZEVector3(Range, Range, Range));
		ZEMatrix4x4::Multiply(ParametersVS->WorldViewMatrix, Camera->GetViewTransform(), ParametersVS->WorldMatrix);
		ZEMatrix4x4::Multiply(ParametersVS->WorldViewProjMatrix, Camera->GetViewProjectionTransform(), ParametersVS->WorldMatrix);
	
	Buffers.VertexShader->Unlock();
	Device->SetVertexShaderBuffer(0, Buffers.VertexShader);

	// PS parameters
	PixelShaderBuffer* ParametersPS = NULL;
	Buffers.PixelShader->Lock((void**)&ParametersPS);
	
		ZEMatrix4x4::Transform(ParametersPS->Position, Camera->GetViewTransform(), Light->GetWorldPosition());
		ParametersPS->Color = Light->GetColor();
		ParametersPS->Attenuation = Light->GetAttenuation();
		ParametersPS->Range = Light->GetRange();
		ParametersPS->Intensity = Light->GetIntensity();
		ParametersPS->PixelSize = ZEVector2(1.0f / (float)ScreenWidth, 1.0f / (float)ScreenHeight);

		// Projection Transform
		ZEQuaternion ProjectionRotation;
		ZEQuaternion::Product(ProjectionRotation, Light->GetWorldRotation().Conjugate(), Camera->GetWorldRotation());
		ZEQuaternion::Normalize(ProjectionRotation, ProjectionRotation);
		ZEMatrix4x4::CreateRotation(ParametersPS->ProjectionMatrix, ProjectionRotation);
	
	Buffers.PixelShader->Unlock();
	Device->SetPixelShaderBuffer(0, Buffers.PixelShader);

	ZEDeviceStateSampler LinearBorderSampler;
	LinearBorderSampler.SetMagFilter(ZE_TFM_LINEAR);
	LinearBorderSampler.SetMinFilter(ZE_TFM_LINEAR);
	LinearBorderSampler.SetMipFilter(ZE_TFM_LINEAR);
	LinearBorderSampler.SetAddressU(ZE_TAM_BORDER);
	LinearBorderSampler.SetAddressV(ZE_TAM_BORDER);
	LinearBorderSampler.SetAddressW(ZE_TAM_BORDER);
	LinearBorderSampler.SetBorderColor(ZEVector4(0.0f, 0.0f, 0.0f, 1.0f));
	
	Device->SetPixelShaderSampler(6, LinearBorderSampler);
	Device->SetPixelShaderTexture(6, Light->GetProjectionTexture());

	Device->Draw(ZE_ROPT_TRIANGLE_LIST, 936, 6);	 // Sphere 1
}

const ZETexture2D* ZERenderStageLighting::GetLBuffer1() const
{
	return Textures.LBuffer1;
}

const ZETexture2D* ZERenderStageLighting::GetLBuffer2() const
{
	return Textures.LBuffer2;
}
		
void ZERenderStageLighting::SetGBufferInput(const ZERenderStageGeometry* Stage)
{
	zeDebugCheck(Stage == NULL, "Null pointer.");
	
	GBufferInput = Stage;
}

const ZERenderStageGeometry* ZERenderStageLighting::GetGBufferInput() const
{
	return GBufferInput;
}

void ZERenderStageLighting::SetLightList(ZESmartArray<ZELight*>* Lights)
{
	zeDebugCheck(Lights == NULL, "Null pointer.");
	
	LightList = Lights;
}

ZESmartArray<ZELight*>* ZERenderStageLighting::GetLightList() const
{
	return LightList;
}

void ZERenderStageLighting::Setup()
{
	UpdateShaders();
	UpdateBuffers();
	ResetStageDefaults();
	CommitStageDefaults();

	StencilMask = 1;

	LastLight = ZE_LT_NONE;

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->ClearRenderTarget(RenderTargets.LBuffer1, ZEVector4::Zero);
	Device->ClearRenderTarget(RenderTargets.LBuffer2, ZEVector4::Zero);
}

void ZERenderStageLighting::Process(ZERenderCommand* RenderCommand)
{
	ZESize LightCount = LightList->GetCount();
	ZEGraphicsEventTracer* Tracer = ZEGraphicsEventTracer::GetInstance();

	for (ZESize I = 0; I < LightCount; I++)
	{
		ZELight* Light = LightList->GetItem(I);
		ZELightType Type = Light->GetLightType();

// 		if (LastLight != Type)
// 		{
// 			CommitStageDefaults();
// 			LastLight = Type;
// 		}

		switch(Type)
		{
			case ZE_LT_POINT:
				Tracer->StartEvent("Point Light Pass");
				RenderPointLight((ZEPointLight*)Light);
				Tracer->EndEvent();
				break;
			
			case ZE_LT_DIRECTIONAL:
				Tracer->StartEvent("Directional Light Pass");
				RenderDirectionalLight((ZEDirectionalLight*)Light);
				Tracer->EndEvent();
				break;

			case ZE_LT_PROJECTIVE:
				Tracer->StartEvent("Projective Light Pass");
				RenderProjectiveLight((ZEProjectiveLight*)Light);
				Tracer->EndEvent();
				break;

			case ZE_LT_OMNIPROJECTIVE:
				Tracer->StartEvent("Omni Projective Light Pass");
				RenderOmniProjectiveLight((ZEOmniProjectiveLight*)Light);
				Tracer->EndEvent();
				break;
		}
	}
}

ZEUInt32 ZERenderStageLighting::GetStageFlags() const
{
	return ZE_RENDER_STAGE_LIGHTING;
}

ZERenderStageLighting::ZERenderStageLighting()
{
	LastLight = ZE_LT_NONE;

	LightList = NULL;
	GBufferInput = NULL;
	
	Textures.LBuffer1 = NULL;
	Textures.LBuffer2 = NULL;
	RenderTargets.LBuffer1 = NULL;
	RenderTargets.LBuffer2 = NULL;

	Shaders.PointLightVS = NULL;
	Shaders.PointLightPS = NULL;
	Shaders.ProjectiveLightVS = NULL;
	Shaders.ProjectiveLightPS = NULL;
	Shaders.DirectionalLightVS = NULL;
	Shaders.DirectionalLightPS = NULL;
	Shaders.OmniProjectiveLightVS = NULL;
	Shaders.OmniProjectiveLightPS = NULL;

	Buffers.PixelShader = NULL;
	Buffers.VertexShader = NULL;
	Buffers.LightMeshVertexBuffer = NULL;

	StencilMask = 1;
}

ZERenderStageLighting::~ZERenderStageLighting()
{
	DestroyBuffers();
	DestroyShaders();
}
