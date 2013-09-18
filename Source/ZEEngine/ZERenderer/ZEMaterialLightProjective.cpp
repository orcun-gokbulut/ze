//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterialLightProjective.cpp
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
#include "ZEMath/ZEMath.h"
#include "ZERenderStage.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEAngle.h"
#include "ZEGraphics/ZEShader.h"
#include "ZEDS/ZEHashGenerator.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGraphics/ZETextureCube.h"
#include "ZEMaterialLightProjective.h"
#include "ZEGraphics/ZEShaderCompiler.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEGraphicsEventTracer.h"
#include "ZEGraphics/ZEShaderCompileOptions.h"

void ZEMaterialLightProjective::UpdateShaders()
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

void ZEMaterialLightProjective::UpdateBuffers()
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

void ZEMaterialLightProjective::DestroyShaders()
{
	ZE_DESTROY(VertexShader);
	ZE_DESTROY(PixelShader);
}

void ZEMaterialLightProjective::DestroyBuffers()
{
	ZE_DESTROY(LightVSProperties0);
	ZE_DESTROY(LightPSProperties0);
	ZE_DESTROY(LightPSProperties1);
}

bool ZEMaterialLightProjective::SetupLightingPass(const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();
	
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
	BlendState.SetComponentWriteMask(0, ZE_CM_NONE);
	BlendState.SetComponentWriteMask(1, ZE_CM_NONE);
	
	ZEDepthStencilState DepthStencilState;
	DepthStencilState.SetZWriteEnable(false);
	DepthStencilState.SetZTestEnable(true);
	DepthStencilState.SetZFunction(ZE_CF_LESS);
	DepthStencilState.SetStencilTestEnable(true);
	DepthStencilState.SetFrontStencilFunction(ZE_CF_ALWAYS);
	DepthStencilState.SetFrontZFail(ZE_SO_REPLACE);
	DepthStencilState.SetFrontStencilFail(ZE_SO_KEEP);
	DepthStencilState.SetFrontStencilPass(ZE_SO_KEEP);

	Device->SetVertexShader(VertexShader);
	Device->SetPixelShader(PixelShader);

	Device->SetVertexShaderBuffer(0, LightVSProperties0);
	Device->SetPixelShaderBuffer(0, LightPSProperties0);
	Device->SetPixelShaderBuffer(1, LightPSProperties1);

	Device->SetBlendState(BlendState);
	Device->SetRasterizerState(RasterizerState);
	Device->SetDepthStencilState(DepthStencilState, StencilMask++);
	
	StencilMask %= 255;

	Device->SetPixelShaderSampler(5, SamplerState);
	Device->SetPixelShaderTexture(5, ProjectionTexture);

	Device->Draw(ZE_PT_TRIANGLE_LIST, 18, 0);
	
	RasterizerState.SetCullDirection(ZE_CD_CLOCKWISE);
	
	BlendState.SetBlendEnable(0, true);
	BlendState.SetBlendEnable(1, true);
	BlendState.SetComponentWriteMask(0 , ZE_CM_ALL);
	BlendState.SetComponentWriteMask(1 , ZE_CM_ALL);
	
	DepthStencilState.SetZFunction(ZE_CF_GREATER);
	DepthStencilState.SetFrontStencilFunction(ZE_CF_NOT_EQUAL);
	DepthStencilState.SetFrontZFail(ZE_SO_KEEP);

	Device->SetBlendState(BlendState);
	Device->SetRasterizerState(RasterizerState);
	Device->SetDepthStencilState(DepthStencilState);

	// No more passes
	return false;
}

ZESize ZEMaterialLightProjective::GetHash() const
{
	return ZEHashGenerator::Hash(ZEString("ZEMaterialLightProjective"));
}

bool ZEMaterialLightProjective::SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	zeDebugCheck(Stage == NULL, "Null pointer.");
	zeDebugCheck(RenderCommand == NULL, "Null pointer.");

	if(!ZEMaterial::SetupPass(PassId, Stage, RenderCommand))
		return false;

	UpdateShaders();
	UpdateBuffers();

	return SetupLightingPass(Stage, RenderCommand);
}

ZEMaterialLightProjective* ZEMaterialLightProjective::CreateInstance()
{
	return new ZEMaterialLightProjective();
}

ZEMaterialLightProjective::ZEMaterialLightProjective()
{
	StencilMask = 1;

	VertexShader = NULL;
	PixelShader = NULL;
	
	LightVSProperties0	= NULL;
	LightPSProperties0	= NULL;
	LightPSProperties1	= NULL;

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

ZEMaterialLightProjective::~ZEMaterialLightProjective()
{
	DestroyBuffers();
	DestroyShaders();
}
