//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterialProjectiveLight.cpp
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

#include "ZECamera.h"
#include "ZECanvas.h"
#include "ZERenderStage.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEAngle.h"
#include "ZEGraphics/ZEShader.h"
#include "ZEGraphics/ZETextureCube.h"
#include "ZEMaterialProjectiveLight.h"
#include "ZEGraphics/ZEShaderCompiler.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEShaderCompileOptions.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEDS/ZEHashGenerator.h"

void ZEMaterialProjectiveLight::UpdateShaders()
{
	ZEShaderCompileOptions Compileoptions;

	if (VertexShader == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_VERTEX;
		Compileoptions.FileName = "ProjectiveLight.hlsl";
		Compileoptions.EntryPoint = "ZEProjectiveLight_VertexShader";
		VertexShader = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (PixelShader == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_PIXEL;
		Compileoptions.FileName = "ProjectiveLight.hlsl";
		Compileoptions.EntryPoint = "ZEProjectiveLight_PixelShader";
		PixelShader = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
}

void ZEMaterialProjectiveLight::UpdateBuffers()
{
	if (Transformations == NULL)
	{
		Transformations = ZEConstantBuffer::CreateInstance();
		Transformations->Create(VertexShader->GetMetaTable()->GetBufferInfo("TransformationsVS"));
	}
	if (LightCaster && LightParameters == NULL)
	{
		LightParameters = ZEConstantBuffer::CreateInstance();
		LightParameters->Create(PixelShader->GetMetaTable()->GetBufferInfo("LightParametersPS"));	
	}
	if (ShadowCaster && ShadowParameters == NULL)
	{
		ShadowParameters = ZEConstantBuffer::CreateInstance();
		ShadowParameters->Create(PixelShader->GetMetaTable()->GetBufferInfo("ShadowParametersPS"));
	}
}

void ZEMaterialProjectiveLight::DestroyShaders()
{
	ZE_DESTROY(VertexShader);
	ZE_DESTROY(PixelShader);
}

void ZEMaterialProjectiveLight::DestroyBuffers()
{
	ZE_DESTROY(Transformations);
	ZE_DESTROY(LightParameters);
	ZE_DESTROY(ShadowParameters);
}

bool ZEMaterialProjectiveLight::SetupLightingPass(const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	UpdateMaterial();

	ZECamera* Camera = zeScene->GetActiveCamera();
	ZEUInt ScreenWidth = zeGraphics->GetScreenWidth();
	ZEUInt ScreenHeight = zeGraphics->GetScreenHeight();
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	struct ProjectiveLightTransformations
	{
		ZEMatrix4x4	WorldViewMatrix;
		ZEMatrix4x4	WorldViewProjMatrix;
	};

	struct ProjectiveLightParameters
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
	ProjectiveLightTransformations* Transforms = NULL;
	Transformations->Lock((void**)&Transforms);
	
		ZEMatrix4x4 WorldTransform;
		float TanFovRange = ZEAngle::Tan(FOV * 0.5f) * Range;
		ZEVector3 Scale(TanFovRange * AspectRatio * 2.0f, TanFovRange * 2.0f, Range);
		ZEMatrix4x4::CreateOrientation(WorldTransform, WorldPosition, WorldRotation, Scale);

		ZEMatrix4x4::Multiply(Transforms->WorldViewMatrix, Camera->GetViewTransform(), WorldTransform);
		ZEMatrix4x4::Multiply(Transforms->WorldViewProjMatrix, Camera->GetViewProjectionTransform(), WorldTransform);
	
	Transformations->Unlock();
	
	// PS parameters
	ProjectiveLightParameters* Parameters = NULL;
	LightParameters->Lock((void**)&Parameters);
	
		Parameters->Range = Range;
		Parameters->Color = Color;
		Parameters->Attenuation = Attenuation;	
		Parameters->Intensity = Intensity;
		ZEMatrix4x4::Transform(Parameters->Position, Camera->GetViewTransform(), WorldPosition);
		Parameters->PixelSize = ZEVector2(1.0f / (float)ScreenWidth, 1.0f / (float)ScreenHeight);
		
		// Projection Transformation
		ZEMatrix4x4 LightViewMatrix;
		ZEMatrix4x4::CreateViewTransform(LightViewMatrix, WorldPosition, WorldRotation);
		ZEMatrix4x4 LightProjectionMatrix;
		ZEMatrix4x4::CreatePerspectiveProjection(LightProjectionMatrix, FOV, AspectRatio, Camera->GetNearZ(), Range);
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
	
	LightParameters->Unlock();
	
	ZERasterizerState RasterizerState;
	RasterizerState.SetFillMode(ZE_FM_SOLID);
	RasterizerState.SetCullDirection(ZE_CD_COUNTER_CLOCKWISE);
	
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

	Device->SetVertexShader(VertexShader);
	Device->SetPixelShader(PixelShader);

	Device->SetVertexShaderBuffer(0, Transformations);
	Device->SetPixelShaderBuffer(0, LightParameters);

	Device->SetBlendState(BlendState);
	Device->SetRasterizerState(RasterizerState);
	Device->SetDepthStencilState(DepthStencilState);
	
	Device->SetPixelShaderSampler(5, SamplerState);
	Device->SetPixelShaderTexture(5, ProjectionTexture);

	Device->Draw(ZE_PT_TRIANGLE_LIST, 18, 0);  // Cone 2
	
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

	Device->Draw(ZE_PT_TRIANGLE_LIST, 18, 0);  // Cone 2

	return true;
}

ZEUInt32 ZEMaterialProjectiveLight::GetHash() const
{
	return ZEHashGenerator::Hash(ZEString("ZEMaterialProjectiveLight"));
}

ZEMaterialFlags ZEMaterialProjectiveLight::GetMaterialFlags() const
{
	return ZE_MTF_L_BUFFER_PASS;
}

void ZEMaterialProjectiveLight::UpdateMaterial()
{
	UpdateShaders();
	UpdateBuffers();
}

bool ZEMaterialProjectiveLight::SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	if ((Stage->GetStageFlags() & ZE_RENDER_STAGE_LIGHTING) == 0)
		return true;

	return SetupLightingPass(Stage, RenderCommand);
}

ZEMaterialProjectiveLight* ZEMaterialProjectiveLight::CreateInstance()
{
	return new ZEMaterialProjectiveLight();
}

ZEMaterialProjectiveLight::ZEMaterialProjectiveLight()
{
	VertexShader = NULL;
	PixelShader = NULL;
	Transformations = NULL;
	LightParameters = NULL;
	ShadowParameters = NULL;
	
	ProjectionTexture = NULL;
	SamplerState.SetMagFilter(ZE_TFM_LINEAR);
	SamplerState.SetMinFilter(ZE_TFM_LINEAR);
	SamplerState.SetMipFilter(ZE_TFM_LINEAR);
	SamplerState.SetAddressU(ZE_TAM_BORDER);
	SamplerState.SetAddressV(ZE_TAM_BORDER);
	SamplerState.SetAddressW(ZE_TAM_BORDER);
	SamplerState.SetBorderColor(ZEVector4(0.0f, 0.0f, 0.0f, 1.0f));
}

ZEMaterialProjectiveLight::~ZEMaterialProjectiveLight()
{
	DestroyBuffers();
	DestroyShaders();
}
