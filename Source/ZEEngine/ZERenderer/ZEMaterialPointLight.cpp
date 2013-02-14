//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterialPointLight.cpp
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
#include "ZEMaterialPointLight.h"
#include "ZEGraphics/ZEShaderCompiler.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEShaderCompileOptions.h"
#include "ZEDS/ZEHashGenerator.h"

void ZEMaterialPointLight::UpdateShaders()
{
	ZEShaderCompileOptions Compileoptions;

	if (VertexShader == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_VERTEX;
		Compileoptions.FileName = "PointLight.hlsl";
		Compileoptions.EntryPoint = "ZEPointLight_VertexShader";
		VertexShader = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (PixelShader == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_PIXEL;
		Compileoptions.FileName = "PointLight.hlsl";
		Compileoptions.EntryPoint = "ZEPointLight_PixelShader";
		PixelShader = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
}

void ZEMaterialPointLight::UpdateBuffers()
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

void ZEMaterialPointLight::DestroyShaders()
{
	ZE_DESTROY(VertexShader);
	ZE_DESTROY(PixelShader);
}

void ZEMaterialPointLight::DestroyBuffers()
{
	ZE_DESTROY(Transformations);
	ZE_DESTROY(LightParameters);
	ZE_DESTROY(ShadowParameters);
}

bool ZEMaterialPointLight::SetupLightingPass(const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	UpdateMaterial();

	ZECamera* Camera = zeScene->GetActiveCamera();
	ZEUInt ScreenWidth = zeGraphics->GetScreenWidth();
	ZEUInt ScreenHeight = zeGraphics->GetScreenHeight();
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	struct PointLightTransformations
	{
		ZEMatrix4x4	WorldViewMatrix;
		ZEMatrix4x4	WorldViewProjMatrix;
	};

	struct PointLightParameters
	{
		ZEVector3		ViewSpacePosition;
		float			Range;
		ZEVector3		Color;
		float			Intensity;
		ZEVector3		Attenuation;
		float			Dummy0;
		ZEVector2		PixelSize;
		float			Dummy1[2];
	};

	// VS Parameters
	PointLightTransformations* Transforms = NULL;
	Transformations->Lock((void**)&Transforms);
	
		ZEMatrix4x4 WorldTransform;
		ZEMatrix4x4::CreateOrientation(WorldTransform, WorldPosition, ZEQuaternion::Identity, ZEVector3(Range, Range, Range));
		
		ZEMatrix4x4::Multiply(Transforms->WorldViewMatrix, Camera->GetViewTransform(), WorldTransform);
		ZEMatrix4x4::Multiply(Transforms->WorldViewProjMatrix, Camera->GetViewProjectionTransform(), WorldTransform);
	
	Transformations->Unlock();

	// PS parameters
	PointLightParameters* Parameters = NULL;
	LightParameters->Lock((void**)&Parameters);
	
		Parameters->Range = Range;
		Parameters->Color = Color;
		Parameters->Intensity = Intensity;
		Parameters->Attenuation = Attenuation;
		ZEMatrix4x4::Transform(Parameters->ViewSpacePosition, Camera->GetViewTransform(), WorldPosition);
		Parameters->PixelSize = ZEVector2(1.0f / (float)ScreenWidth, 1.0f / (float)ScreenHeight);
	
	LightParameters->Unlock();
	
	Device->SetVertexShader(VertexShader);
	Device->SetPixelShader(PixelShader);
	
	Device->SetVertexShaderBuffer(0, Transformations);
	Device->SetPixelShaderBuffer(0, LightParameters);

	Device->Draw(ZE_PT_TRIANGLE_LIST, 936, 0);

	return true;
}

ZEUInt32 ZEMaterialPointLight::GetHash() const
{
	return ZEHashGenerator::Hash(ZEString("ZEMaterialPointLight"));
}

ZEMaterialFlags ZEMaterialPointLight::GetMaterialFlags() const
{
	return ZE_MTF_L_BUFFER_PASS;
}

void ZEMaterialPointLight::UpdateMaterial()
{
	UpdateShaders();
	UpdateBuffers();
}

bool ZEMaterialPointLight::SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	if ((Stage->GetStageFlags() & ZE_RENDER_STAGE_LIGHTING) == 0)
		return true;

	return SetupLightingPass(Stage, RenderCommand);
}

ZEMaterialPointLight* ZEMaterialPointLight::CreateInstance()
{
	return new ZEMaterialPointLight();
}

ZEMaterialPointLight::ZEMaterialPointLight()
{
	VertexShader = NULL;
	PixelShader = NULL;
	Transformations = NULL;
	LightParameters = NULL;
	ShadowParameters = NULL;
}

ZEMaterialPointLight::~ZEMaterialPointLight()
{
	DestroyBuffers();
	DestroyShaders();
}
