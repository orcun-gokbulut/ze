//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterialLightOmniProjective.cpp
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
#include "ZEDS/ZEHashGenerator.h"
#include "ZEGraphics/ZETextureCube.h"
#include "ZEGraphics/ZEShaderCompiler.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEMaterialLightOmniProjective.h"
#include "ZEGraphics/ZEGraphicsEventTracer.h"
#include "ZEGraphics/ZEShaderCompileOptions.h"

void ZEMaterialLightOmniProjective::UpdateShaders()
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

void ZEMaterialLightOmniProjective::UpdateBuffers()
{
	if (LightVSProperties0 == NULL)
	{
		LightVSProperties0 = ZEConstantBuffer::CreateInstance();
		LightVSProperties0->Create(sizeof(VSProperties0));
		LightVSProperties0->SetZero();
	}
	if (LightPSProperties0 == NULL)
	{
		LightPSProperties0 = ZEConstantBuffer::CreateInstance();
		LightPSProperties0->Create(sizeof(PSProperties0));	
		LightPSProperties0->SetZero();
	}
	if (LightPSProperties1 == NULL)
	{
		LightPSProperties1 = ZEConstantBuffer::CreateInstance();
		LightPSProperties1->Create(sizeof(PSProperties1));
		LightPSProperties1->SetZero();
	}
}

void ZEMaterialLightOmniProjective::DestroyShaders()
{
	ZE_DESTROY(VertexShader);
	ZE_DESTROY(PixelShader);
}

void ZEMaterialLightOmniProjective::DestroyBuffers()
{
	ZE_DESTROY(LightVSProperties0);
	ZE_DESTROY(LightPSProperties0);
	ZE_DESTROY(LightPSProperties1);
}

bool ZEMaterialLightOmniProjective::SetupLightingPass(const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{	
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();
	
	Device->SetVertexShader(VertexShader);
	Device->SetPixelShader(PixelShader);

	Device->SetVertexShaderBuffer(0, LightVSProperties0);
	Device->SetPixelShaderBuffer(0, LightPSProperties0);
	Device->SetPixelShaderBuffer(1, LightPSProperties1);
	
	Device->SetPixelShaderSampler(6, SamplerState);
	Device->SetPixelShaderTexture(6, ProjectionTexture);

	// No more passes
	return false;
}

ZESize ZEMaterialLightOmniProjective::GetHash() const
{
	return ZEHashGenerator::Hash(ZEString("ZEMaterialLightOmniProjective"));
}

bool ZEMaterialLightOmniProjective::SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	zeDebugCheck(Stage == NULL, "Null pointer.");
	zeDebugCheck(RenderCommand == NULL, "Null pointer.");

	if(!ZEMaterial::SetupPass(PassId, Stage, RenderCommand))
		return false;

	UpdateShaders();
	UpdateBuffers();

	return SetupLightingPass(Stage, RenderCommand);
}

ZEMaterialLightOmniProjective* ZEMaterialLightOmniProjective::CreateInstance()
{
	return new ZEMaterialLightOmniProjective();
}

ZEMaterialLightOmniProjective::ZEMaterialLightOmniProjective()
{
	VertexShader = NULL;
	PixelShader = NULL;
	
	LightVSProperties0 = NULL;
	LightPSProperties0 = NULL;
	LightPSProperties1 = NULL;
	
	ProjectionTexture = NULL;
	
	SamplerState.SetMagFilter(ZE_TFM_LINEAR);
	SamplerState.SetMinFilter(ZE_TFM_LINEAR);
	SamplerState.SetMipFilter(ZE_TFM_LINEAR);
	SamplerState.SetAddressU(ZE_TAM_BORDER);
	SamplerState.SetAddressV(ZE_TAM_BORDER);
	SamplerState.SetAddressW(ZE_TAM_BORDER);
	SamplerState.SetBorderColor(ZEVector4(0.0f, 0.0f, 0.0f, 1.0f));

	SupportedStages = ZE_RST_LIGHTING;
	EnableStage(SupportedStages);
	
	UpdateBuffers();
}

ZEMaterialLightOmniProjective::~ZEMaterialLightOmniProjective()
{
	DestroyBuffers();
	DestroyShaders();
}
