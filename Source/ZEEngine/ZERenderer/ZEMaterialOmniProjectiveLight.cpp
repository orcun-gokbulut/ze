//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterialOmniProjectiveLight.cpp
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
#include "ZEGraphics/ZEShader.h"
#include "ZEGraphics/ZETextureCube.h"
#include "ZEGraphics/ZEShaderCompiler.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEMaterialOmniProjectiveLight.h"
#include "ZEGraphics/ZEShaderCompileOptions.h"
#include "ZEDS/ZEHashGenerator.h"

void ZEMaterialOmniProjectiveLight::UpdateShaders()
{
	ZEShaderCompileOptions Compileoptions;

	if (VertexShader == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_VERTEX;
		Compileoptions.FileName = "OmniProjectiveLight.hlsl";
		Compileoptions.EntryPoint = "ZEOmniProjectiveLight_VertexShader";
		VertexShader = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (PixelShader == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_PIXEL;
		Compileoptions.FileName = "OmniProjectiveLight.hlsl";
		Compileoptions.EntryPoint = "ZEOmniProjectiveLight_PixelShader";
		PixelShader = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
}

void ZEMaterialOmniProjectiveLight::UpdateBuffers()
{
	if (VertexBuffer == NULL)
	{
		ZECanvas Canvas;
		Canvas.AddSphere(1.0f, 24, 24);

		VertexBuffer = Canvas.CreateStaticVertexBuffer();
	}
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

void ZEMaterialOmniProjectiveLight::DestroyShaders()
{
	ZE_DESTROY(VertexShader);
	ZE_DESTROY(PixelShader);
}

void ZEMaterialOmniProjectiveLight::DestroyBuffers()
{
	ZE_DESTROY(Transformations);
	ZE_DESTROY(LightParameters);
	ZE_DESTROY(ShadowParameters);
}

bool ZEMaterialOmniProjectiveLight::SetupLightingPass(const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	UpdateMaterial();

	ZECamera* Camera = zeScene->GetActiveCamera();
	ZEUInt ScreenWidth = zeGraphics->GetScreenWidth();
	ZEUInt ScreenHeight = zeGraphics->GetScreenHeight();
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	struct OmniProjectiveLightTransformations
	{
		ZEMatrix4x4	WorldViewMatrix;
		ZEMatrix4x4	WorldViewProjMatrix;
	};

	struct OmniProjectiveLightParameters
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
	OmniProjectiveLightTransformations* Transforms = NULL;
	Transformations->Lock((void**)&Transforms);
	
		ZEMatrix4x4 WorldTransform;
		ZEMatrix4x4::CreateOrientation(WorldTransform, WorldPosition, ZEQuaternion::Identity, ZEVector3(Range, Range, Range));
		ZEMatrix4x4::Multiply(Transforms->WorldViewMatrix, Camera->GetViewTransform(), WorldTransform);
		ZEMatrix4x4::Multiply(Transforms->WorldViewProjMatrix, Camera->GetViewProjectionTransform(), WorldTransform);
	
	Transformations->Unlock();

	// PS parameters
	OmniProjectiveLightParameters* Parameters = NULL;
	LightParameters->Lock((void**)&Parameters);
	
		Parameters->Range = Range;
		Parameters->Color = Color;
		Parameters->Intensity = Intensity;
		Parameters->Attenuation = Attenuation;
		ZEMatrix4x4::Transform(Parameters->Position, Camera->GetViewTransform(), WorldPosition);
		Parameters->PixelSize = ZEVector2(1.0f / (float)ScreenWidth, 1.0f / (float)ScreenHeight);
		
		// Projection Transform
		ZEQuaternion ProjectionRotation;
		ZEQuaternion::Product(ProjectionRotation, WorldRotation.Conjugate(), Camera->GetWorldRotation());
		ZEQuaternion::Normalize(ProjectionRotation, ProjectionRotation);
		ZEMatrix4x4::CreateRotation(Parameters->ProjectionMatrix, ProjectionRotation);
	
	LightParameters->Unlock();
	
	Device->SetVertexShader(VertexShader);
	Device->SetPixelShader(PixelShader);

	Device->SetVertexShaderBuffer(0, Transformations);
	Device->SetPixelShaderBuffer(0, LightParameters);
	
	Device->SetPixelShaderSampler(6, SamplerState);
	Device->SetPixelShaderTexture(6, ProjectionTexture);

	Device->Draw(ZE_PT_TRIANGLE_LIST, 936, 0);	 // Sphere 1

	return true;
}

ZEUInt32 ZEMaterialOmniProjectiveLight::GetHash() const
{
	return ZEHashGenerator::Hash(ZEString("ZEMaterialOmniProjectiveLight"));
}

ZEMaterialFlags ZEMaterialOmniProjectiveLight::GetMaterialFlags() const
{
	return ZE_MTF_L_BUFFER_PASS;
}

void ZEMaterialOmniProjectiveLight::UpdateMaterial()
{
	UpdateShaders();
	UpdateBuffers();
}

bool ZEMaterialOmniProjectiveLight::SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	if ((Stage->GetStageFlags() & ZE_RENDER_STAGE_LIGHTING) == 0)
		return true;

	return SetupLightingPass(Stage, RenderCommand);
}

ZEMaterialOmniProjectiveLight* ZEMaterialOmniProjectiveLight::CreateInstance()
{
	return new ZEMaterialOmniProjectiveLight();
}

ZEMaterialOmniProjectiveLight::ZEMaterialOmniProjectiveLight()
{
	VertexShader = NULL;
	PixelShader = NULL;
	Transformations = NULL;
	LightParameters = NULL;
	ShadowParameters = NULL;
	VertexBuffer = NULL;
	
	ProjectionTexture = NULL;
	SamplerState.SetMagFilter(ZE_TFM_LINEAR);
	SamplerState.SetMinFilter(ZE_TFM_LINEAR);
	SamplerState.SetMipFilter(ZE_TFM_LINEAR);
	SamplerState.SetAddressU(ZE_TAM_BORDER);
	SamplerState.SetAddressV(ZE_TAM_BORDER);
	SamplerState.SetAddressW(ZE_TAM_BORDER);
	SamplerState.SetBorderColor(ZEVector4(0.0f, 0.0f, 0.0f, 1.0f));
}

ZEMaterialOmniProjectiveLight::~ZEMaterialOmniProjectiveLight()
{
	DestroyBuffers();
	DestroyShaders();
}
