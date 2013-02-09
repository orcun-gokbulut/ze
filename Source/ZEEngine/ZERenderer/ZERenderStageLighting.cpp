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

void ZERenderStageLighting::DestroyShaders()
{
	ZE_DESTROY(PointLight.VertexShader);
	ZE_DESTROY(PointLight.PixelShader);
	
	ZE_DESTROY(DirectionalLight.VertexShader);
	ZE_DESTROY(DirectionalLight.PixelShader);

	ZE_DESTROY(ProjectiveLight.VertexShader);
	ZE_DESTROY(ProjectiveLight.PixelShader);

	ZE_DESTROY(OmniProjectiveLight.VertexShader);
	ZE_DESTROY(OmniProjectiveLight.PixelShader);
}

void ZERenderStageLighting::UpdateShaders()
{
	ZEShaderCompileOptions Compileoptions;

	if (PointLight.VertexShader == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_VERTEX;
		Compileoptions.FileName = "PointLight.hlsl";
		Compileoptions.EntryPoint = "ZEPointLight_VertexShader";
		PointLight.VertexShader = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (PointLight.PixelShader == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_PIXEL;
		Compileoptions.FileName = "PointLight.hlsl";
		Compileoptions.EntryPoint = "ZEPointLight_PixelShader";
		PointLight.PixelShader = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (DirectionalLight.VertexShader == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_VERTEX;
		Compileoptions.FileName = "DirectionalLight.hlsl";
		Compileoptions.EntryPoint = "ZEDirectionalLight_VertexShader";
		DirectionalLight.VertexShader = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (DirectionalLight.PixelShader == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_PIXEL;
		Compileoptions.FileName = "DirectionalLight.hlsl";
		Compileoptions.EntryPoint = "ZEDirectionalLight_PixelShader";
		DirectionalLight.PixelShader = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (ProjectiveLight.VertexShader == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_VERTEX;
		Compileoptions.FileName = "ProjectiveLight.hlsl";
		Compileoptions.EntryPoint = "ZEProjectiveLight_VertexShader";
		ProjectiveLight.VertexShader = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (ProjectiveLight.PixelShader == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_PIXEL;
		Compileoptions.FileName = "ProjectiveLight.hlsl";
		Compileoptions.EntryPoint = "ZEProjectiveLight_PixelShader";
		ProjectiveLight.PixelShader = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (OmniProjectiveLight.VertexShader == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_VERTEX;
		Compileoptions.FileName = "OmniProjectiveLight.hlsl";
		Compileoptions.EntryPoint = "ZEOmniProjectiveLight_VertexShader";
		OmniProjectiveLight.VertexShader = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (OmniProjectiveLight.PixelShader == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_PIXEL;
		Compileoptions.FileName = "OmniProjectiveLight.hlsl";
		Compileoptions.EntryPoint = "ZEOmniProjectiveLight_PixelShader";
		OmniProjectiveLight.PixelShader = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
}

void ZERenderStageLighting::DestroyBuffers()
{
	ZE_DESTROY(RenderTargets.LBuffer1);
	ZE_DESTROY(RenderTargets.LBuffer2);
	ZE_DESTROY(Textures.LBuffer1);
	ZE_DESTROY(Textures.LBuffer2);
	ZE_DESTROY(VertexBuffer);

	ZE_DESTROY(PointLight.Transformations);
	ZE_DESTROY(PointLight.LightParameters);
	ZE_DESTROY(PointLight.ShadowParameters);
	
	ZE_DESTROY(DirectionalLight.Transformations);
	ZE_DESTROY(DirectionalLight.LightParameters);
	ZE_DESTROY(DirectionalLight.ShadowParameters);

	ZE_DESTROY(ProjectiveLight.Transformations);
	ZE_DESTROY(ProjectiveLight.LightParameters);
	ZE_DESTROY(ProjectiveLight.ShadowParameters);

	ZE_DESTROY(OmniProjectiveLight.Transformations);
	ZE_DESTROY(OmniProjectiveLight.LightParameters);
	ZE_DESTROY(OmniProjectiveLight.ShadowParameters);
}

void ZERenderStageLighting::UpdateBuffers()
{
	// Update parent buffers
	ZERenderStage::UpdateBuffers();

	// Create Static Vertex Buffer
	if (VertexBuffer == NULL)
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

		VertexBuffer = Canvas.CreateStaticVertexBuffer();
	}
	
	if (PointLight.Transformations == NULL)
	{
		PointLight.Transformations = ZEConstantBuffer::CreateInstance();
		PointLight.Transformations->Create(PointLight.VertexShader->GetMetaTable()->GetBufferInfo("TransformationsVS"));
	}
	if (PointLight.LightParameters == NULL)
	{
		PointLight.LightParameters = ZEConstantBuffer::CreateInstance();
		PointLight.LightParameters->Create(PointLight.PixelShader->GetMetaTable()->GetBufferInfo("LightParametersPS"));	
	}
	/*
	if (PointLight.ShadowParameters == NULL)
	{
		PointLight.ShadowParameters = ZEConstantBuffer::CreateInstance();
		PointLight.ShadowParameters->Create(PointLight.PixelShader->GetMetaTable()->GetBufferInfo("ShadowParametersPS"));
	}
	*/
	if (DirectionalLight.Transformations == NULL)
	{
		DirectionalLight.Transformations = ZEConstantBuffer::CreateInstance();
		DirectionalLight.Transformations->Create(DirectionalLight.VertexShader->GetMetaTable()->GetBufferInfo("TransformationsVS"));
	}
	if (DirectionalLight.LightParameters == NULL)
	{
		DirectionalLight.LightParameters = ZEConstantBuffer::CreateInstance();
		DirectionalLight.LightParameters->Create(DirectionalLight.PixelShader->GetMetaTable()->GetBufferInfo("LightParametersPS"));	
	}
	/*
	if (DirectionalLight.ShadowParameters == NULL)
	{
		DirectionalLight.ShadowParameters = ZEConstantBuffer::CreateInstance();
		DirectionalLight.ShadowParameters->Create(DirectionalLight.PixelShader->GetMetaTable()->GetBufferInfo("ShadowParametersPS"));
	}
	*/
	if (ProjectiveLight.Transformations == NULL)
	{
		ProjectiveLight.Transformations = ZEConstantBuffer::CreateInstance();
		ProjectiveLight.Transformations->Create(ProjectiveLight.VertexShader->GetMetaTable()->GetBufferInfo("TransformationsVS"));
	}
	if (ProjectiveLight.LightParameters == NULL)
	{
		ProjectiveLight.LightParameters = ZEConstantBuffer::CreateInstance();
		ProjectiveLight.LightParameters->Create(ProjectiveLight.PixelShader->GetMetaTable()->GetBufferInfo("LightParametersPS"));	
	}
	/*
	if (ProjectiveLight.ShadowParameters == NULL)
	{
		ProjectiveLight.ShadowParameters = ZEConstantBuffer::CreateInstance();
		ProjectiveLight.ShadowParameters->Create(ProjectiveLight.PixelShader->GetMetaTable()->GetBufferInfo("ShadowParametersPS"));
	}
	*/
	if (OmniProjectiveLight.Transformations == NULL)
	{
		OmniProjectiveLight.Transformations = ZEConstantBuffer::CreateInstance();
		OmniProjectiveLight.Transformations->Create(OmniProjectiveLight.VertexShader->GetMetaTable()->GetBufferInfo("TransformationsVS"));
	}
	if (OmniProjectiveLight.LightParameters == NULL)
	{
		OmniProjectiveLight.LightParameters = ZEConstantBuffer::CreateInstance();
		OmniProjectiveLight.LightParameters->Create(OmniProjectiveLight.PixelShader->GetMetaTable()->GetBufferInfo("LightParametersPS"));	
	}
	/*
	if (OmniProjectiveLight.ShadowParameters == NULL)
	{
		OmniProjectiveLight.ShadowParameters = ZEConstantBuffer::CreateInstance();
		OmniProjectiveLight.ShadowParameters->Create(OmniProjectiveLight.PixelShader->GetMetaTable()->GetBufferInfo("ShadowParametersPS"));
	}
	*/
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
	DefaultStates.VertexBuffers[0] = VertexBuffer;

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

	Device->SetVertexShader(PointLight.VertexShader);
	Device->SetPixelShader(PointLight.PixelShader);

	struct LightTransformations
	{
		ZEMatrix4x4	WorldViewMatrix;
		ZEMatrix4x4	WorldViewProjMatrix;
	};

	struct LightParameters
	{
		ZEVector3		Position;
		float			Range;
		ZEVector3		Color;
		float			Intensity;
		ZEVector3		Attenuation;
		ZEVector2		PixelSize;
	};

	// VS Parameters
	LightTransformations* Transformations = NULL;
	PointLight.Transformations->Lock((void**)&Transformations);
	
		ZEMatrix4x4 WorldTransform;
		ZEMatrix4x4::CreateOrientation(WorldTransform, Light->GetWorldPosition(), ZEQuaternion::Identity, ZEVector3(Range, Range, Range));
		
		ZEMatrix4x4::Multiply(Transformations->WorldViewMatrix, Camera->GetViewTransform(), WorldTransform);
		ZEMatrix4x4::Multiply(Transformations->WorldViewProjMatrix, Camera->GetViewProjectionTransform(), WorldTransform);
	
	PointLight.Transformations->Unlock();
	Device->SetVertexShaderBuffer(0, PointLight.Transformations);

	// PS parameters
	LightParameters* Parameters = NULL;
	PointLight.LightParameters->Lock((void**)&Parameters);
	
		ZEMatrix4x4::Transform(Parameters->Position, Camera->GetViewTransform(), Light->GetWorldPosition());
		Parameters->Range = Light->GetRange();
		Parameters->Color = Light->GetColor();
		Parameters->Intensity = Light->GetIntensity();
		Parameters->Attenuation = Light->GetAttenuation();
		Parameters->PixelSize = ZEVector2(1.0f / (float)ScreenWidth, 1.0f / (float)ScreenHeight);
	
	PointLight.LightParameters->Unlock();
	Device->SetPixelShaderBuffer(0, PointLight.LightParameters);

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

	Device->SetVertexShader(ProjectiveLight.VertexShader);
	Device->SetPixelShader(ProjectiveLight.PixelShader);

	struct LightTransformations
	{
		ZEMatrix4x4	WorldViewMatrix;
		ZEMatrix4x4	WorldViewProjMatrix;
	};

	struct LightParameters
	{
		ZEVector3		Position;
		float			Range;
		ZEVector3		Color;
		float			Intensity;
		ZEVector3		Attenuation;
		float			Dummy0;
		ZEVector2		PixelSize;
		float			Dummy1[2];
		ZEMatrix4x4		ProjectionMatrix;
	};

	// VS parameters
	LightTransformations* Transformations = NULL;
	ProjectiveLight.Transformations->Lock((void**)&Transformations);
	
		ZEMatrix4x4 WorldTransform;
		float TanFovRange = ZEAngle::Tan(FOV * 0.5f) * Range;
		ZEVector3 Scale(TanFovRange * Light->GetAspectRatio() * 2.0f, TanFovRange * 2.0f, Range);
		ZEMatrix4x4::CreateOrientation(WorldTransform, Light->GetWorldPosition(), Light->GetWorldRotation(), Scale);

		ZEMatrix4x4::Multiply(Transformations->WorldViewMatrix, Camera->GetViewTransform(), WorldTransform);
		ZEMatrix4x4::Multiply(Transformations->WorldViewProjMatrix, Camera->GetViewProjectionTransform(), WorldTransform);
	
	ProjectiveLight.Transformations->Unlock();
	Device->SetVertexShaderBuffer(0, ProjectiveLight.Transformations);

	// PS parameters
	LightParameters* Parameters = NULL;
	ProjectiveLight.LightParameters->Lock((void**)&Parameters);
	
		Parameters->Range = Range;
		Parameters->Color = Light->GetColor();
		Parameters->Attenuation = Light->GetAttenuation();	
		Parameters->Intensity = Light->GetIntensity();
		Parameters->PixelSize = ZEVector2(1.0f / (float)ScreenWidth, 1.0f / (float)ScreenHeight);
		ZEMatrix4x4::Transform(Parameters->Position, Camera->GetViewTransform(), Light->GetWorldPosition());

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
		ZEMatrix4x4::Multiply(Parameters->ProjectionMatrix, TextureMatrix, LightSpaceMatrix);
	
	ProjectiveLight.LightParameters->Unlock();
	Device->SetPixelShaderBuffer(0, ProjectiveLight.LightParameters);

	ZERasterizerState RasterizerState;
	RasterizerState.SetFillMode(ZE_FM_SOLID);
	RasterizerState.SetCullDirection(ZE_CD_COUNTER_CLOCKWISE);
	Device->SetRasterizerState(RasterizerState);

	ZEBlendState BlendState;
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

	ZEDepthStencilState DepthStencilState;
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

	ZESamplerState LinearBorderSampler;
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
	struct LightTransformations
	{
		ZEMatrix4x4 InvProjMatrix;
	};

	struct LightParameters
	{
		ZEVector3		Color;
		float			Intensity;
		ZEVector3		Attenuation;
		float			Dummy0;
		ZEVector3		Direction;
		float			Dummy1;
		ZEVector2		PixelSize;
	};

	float Range = Light->GetRange();
	ZEUInt ScreenWidth = zeGraphics->GetScreenWidth();
	ZEUInt ScreenHeight = zeGraphics->GetScreenHeight();
	ZECamera* Camera = zeScene->GetRenderer()->GetCamera();
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetVertexShader(DirectionalLight.VertexShader);
	Device->SetPixelShader(DirectionalLight.PixelShader);

	// VS parameters
	LightTransformations* Transformations = NULL;
	DirectionalLight.Transformations->Lock((void**)&Transformations);
		
		ZEMatrix4x4::Inverse(Transformations->InvProjMatrix, Camera->GetProjectionTransform());
	
	DirectionalLight.Transformations->Unlock();
	Device->SetVertexShaderBuffer(0, DirectionalLight.Transformations);

	// PS Parameters
	LightParameters* Parameters = NULL;
	DirectionalLight.LightParameters->Lock((void**)&Parameters);
	
		ZEMatrix4x4::Transform3x3(Parameters->Direction, Camera->GetViewTransform(), Light->GetWorldFront());
		Parameters->Direction = -Parameters->Direction;
		Parameters->Color = Light->GetColor();
		Parameters->Attenuation = Light->GetAttenuation();
		Parameters->Intensity = Light->GetIntensity();
		Parameters->PixelSize = ZEVector2(1.0f / (float)ScreenWidth, 1.0f / (float)ScreenHeight);
	
	DirectionalLight.LightParameters->Unlock();
	Device->SetPixelShaderBuffer(0, DirectionalLight.LightParameters);

	Device->Draw(ZE_ROPT_TRIANGLE_LIST, 6, 0);	// Quad
}

void ZERenderStageLighting::RenderOmniProjectiveLight(const ZEOmniProjectiveLight* Light)
{
	float Range = Light->GetRange();
	ZEUInt ScreenWidth = zeGraphics->GetScreenWidth();
	ZEUInt ScreenHeight = zeGraphics->GetScreenHeight();
	ZECamera* Camera = zeScene->GetRenderer()->GetCamera();
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetVertexShader(OmniProjectiveLight.VertexShader);
	Device->SetPixelShader(OmniProjectiveLight.PixelShader);

	struct LightTransformations
	{
		ZEMatrix4x4	WorldViewMatrix;
		ZEMatrix4x4	WorldViewProjMatrix;
	};

	struct LightParameters
	{
		ZEVector3		Position;
		float			Range;
		ZEVector3		Color;
		float			Intensity;
		ZEVector3		Attenuation;
		float			Dummy0;
		ZEVector2		PixelSize;
		float			Dummy1[2];
		ZEMatrix4x4		ProjectionMatrix;
	};

	// VS parameters
	LightTransformations* Transformations = NULL;
	OmniProjectiveLight.Transformations->Lock((void**)&Transformations);
	
		ZEMatrix4x4 WorldTransform;
		ZEMatrix4x4::CreateOrientation(WorldTransform, Light->GetWorldPosition(), ZEQuaternion::Identity, ZEVector3(Range, Range, Range));
		ZEMatrix4x4::Multiply(Transformations->WorldViewMatrix, Camera->GetViewTransform(), WorldTransform);
		ZEMatrix4x4::Multiply(Transformations->WorldViewProjMatrix, Camera->GetViewProjectionTransform(), WorldTransform);
	
	OmniProjectiveLight.Transformations->Unlock();
	Device->SetVertexShaderBuffer(0, OmniProjectiveLight.Transformations);

	// PS parameters
	LightParameters* Parameters = NULL;
	OmniProjectiveLight.LightParameters->Lock((void**)&Parameters);
	
		ZEMatrix4x4::Transform(Parameters->Position, Camera->GetViewTransform(), Light->GetWorldPosition());
		Parameters->Color = Light->GetColor();
		Parameters->Attenuation = Light->GetAttenuation();
		Parameters->Range = Light->GetRange();
		Parameters->Intensity = Light->GetIntensity();
		Parameters->PixelSize = ZEVector2(1.0f / (float)ScreenWidth, 1.0f / (float)ScreenHeight);

		// Projection Transform
		ZEQuaternion ProjectionRotation;
		ZEQuaternion::Product(ProjectionRotation, Light->GetWorldRotation().Conjugate(), Camera->GetWorldRotation());
		ZEQuaternion::Normalize(ProjectionRotation, ProjectionRotation);
		ZEMatrix4x4::CreateRotation(Parameters->ProjectionMatrix, ProjectionRotation);
	
	OmniProjectiveLight.LightParameters->Unlock();
	Device->SetPixelShaderBuffer(0, OmniProjectiveLight.LightParameters);

	ZESamplerState LinearBorderSampler;
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

	PointLight.VertexShader = NULL;
	PointLight.PixelShader = NULL;
	PointLight.Transformations = NULL;
	PointLight.LightParameters = NULL;
	PointLight.ShadowParameters = NULL;

	ProjectiveLight.VertexShader = NULL;
	ProjectiveLight.PixelShader = NULL;
	ProjectiveLight.Transformations = NULL;
	ProjectiveLight.LightParameters = NULL;
	ProjectiveLight.ShadowParameters = NULL;

	DirectionalLight.VertexShader = NULL;
	DirectionalLight.PixelShader = NULL;
	DirectionalLight.Transformations = NULL;
	DirectionalLight.LightParameters = NULL;
	DirectionalLight.ShadowParameters = NULL;

	OmniProjectiveLight.VertexShader = NULL;
	OmniProjectiveLight.PixelShader = NULL;
	OmniProjectiveLight.Transformations = NULL;
	OmniProjectiveLight.LightParameters = NULL;
	OmniProjectiveLight.ShadowParameters = NULL;

	VertexBuffer = NULL;
	StencilMask = 1;
}

ZERenderStageLighting::~ZERenderStageLighting()
{
	DestroyBuffers();
	DestroyShaders();
}
