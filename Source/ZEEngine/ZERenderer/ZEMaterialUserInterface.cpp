//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterialUserInterface.cpp
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



#include "ZERenderCommand.h"
#include "ZEGraphics/ZEShader.h"
#include "ZEGraphics/ZETexture.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEMaterialUserInterface.h"
#include "ZEGraphics/ZEShaderCompiler.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEConstantBuffer.h"
#include "ZEGraphics/ZEShaderCompileOptions.h"
#include "ZERenderStage.h"
#include "ZEDS/ZEHashGenerator.h"


ZEShader* ZEMaterialUserInterface::VertexShader = NULL;
ZEShader* ZEMaterialUserInterface::PixelShader = NULL;
ZEShader* ZEMaterialUserInterface::PixelShaderTextured = NULL;

struct ZEUIConstantBuffer
{
	ZEMatrix4x4 WorldMatrix;
};

ZEMaterialUserInterface::ZEMaterialUserInterface()
{
	Texture = NULL;
	WireFrame = false;

	CreateShaders();
}

ZEMaterialUserInterface::~ZEMaterialUserInterface()
{
	DestroyShaders();
}

void ZEMaterialUserInterface::CreateShaders()
{
	if (VertexShader == NULL)
	{
		ZEShaderCompileOptions Options;
		Options.Model = ZE_SM_4_0;
		Options.Type = ZE_ST_VERTEX;
		Options.FileName = "UI.hlsl";
		Options.EntryPoint = "VSMain";
		VertexShader = ZEShaderCompiler::CompileShaderFromFile(&Options);
	}
	if (VertexShaderBuffer == NULL)
	{
		VertexShaderBuffer = ZEConstantBuffer::CreateInstance();
		VertexShaderBuffer->Create(VertexShader->GetMetaTable()->GetBufferInfo("BufferVS"));
	}
	if (PixelShader == NULL)
	{
		ZEShaderCompileOptions Options;
		Options.Model = ZE_SM_4_0;
		Options.Type = ZE_ST_PIXEL;
		Options.FileName = "UI.hlsl";
		Options.EntryPoint = "PSMain";
		PixelShader = ZEShaderCompiler::CompileShaderFromFile(&Options);
	}
	if (PixelShaderTextured == NULL)
	{
		ZEShaderCompileOptions Options;
		Options.Model = ZE_SM_4_0;
		Options.Type = ZE_ST_PIXEL;
		Options.FileName = "UI.hlsl";
		Options.EntryPoint = "PSMainTextured";
		PixelShaderTextured = ZEShaderCompiler::CompileShaderFromFile(&Options);
	}
}

void ZEMaterialUserInterface::DestroyShaders()
{
	ZE_DESTROY(VertexShaderBuffer);
}

ZESize ZEMaterialUserInterface::GetHash() const
{
	return ZEHashGenerator::Hash(ZEString("ZEMaterialUserInterface"));
}

void ZEMaterialUserInterface::SetWireFrame(bool Enabled)
{
	WireFrame = Enabled;
}

bool ZEMaterialUserInterface::GetWireFrame() const
{
	return WireFrame;
}

void ZEMaterialUserInterface::SetTexture(const ZETexture2D* Texture)
{
	this->Texture = Texture;
}

const ZETexture2D* ZEMaterialUserInterface::GetTexture() const
{
	return Texture;
}

bool ZEMaterialUserInterface::SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	/*
	if (Stage->GetStageFlags() != ZE_RENDER_STAGE_UI)
		return true;

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	ZEUIConstantBuffer* Data = NULL;
	VertexShaderBuffer->Lock((void**)&Data);
	
		Data->WorldMatrix = RenderCommand->WorldMatrix;
	
	VertexShaderBuffer->Unlock();
	Device->SetVertexShaderBuffer(0, VertexShaderBuffer);
		
	ZERasterizerState RasterizerState;
	RasterizerState.SetCullDirection(ZE_CD_COUNTER_CLOCKWISE);
	RasterizerState.SetFillMode(WireFrame ? ZE_FM_WIREFRAME : ZE_FM_SOLID);
	Device->SetRasterizerState(RasterizerState);

	ZEDepthStencilState DepthStencilState;
	DepthStencilState.SetZTestEnable(false);
	Device->SetDepthStencilState(DepthStencilState);

	ZEBlendState BlendState;
	BlendState.SetBlendEnable(0, true);
	BlendState.SetBlendEquation(ZE_BE_ADD);
	BlendState.SetSourceBlendOption(ZE_BO_SRC_ALPHA);
	BlendState.SetDestinationBlendOption(ZE_BO_INV_SRC_ALPHA);
	Device->SetBlendState(BlendState);

	Device->SetVertexShader(VertexShader);
	
	if (Texture == NULL)
	{
		Device->SetPixelShader(PixelShader);
	}
	else
	{
		Device->SetPixelShader(PixelShaderTextured);
	
		ZESamplerState SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_LINEAR);
		SamplerState.SetMagFilter(ZE_TFM_LINEAR);
		SamplerState.SetMipFilter(ZE_TFM_POINT);
		SamplerState.SetAddressU(ZE_TAM_WRAP);
		SamplerState.SetAddressV(ZE_TAM_WRAP);

		Device->SetPixelShaderSampler(0, SamplerState);
		Device->SetPixelShaderTexture(0, Texture);
	}
	*/
	return true;
}

/*
bool ZEMaterialUserInterface::SetupForwardPass(ZERenderer* Renderer, ZERenderCommand* RenderCommand) 
{

	static ZEDeviceStageShader VertexShaderStage;
	static ZEDeviceStageShader PixelShaderStage;
	static ZEDeviceStageShader TexturedPixelShaderStage;

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	static ZERasterizerState RasterizerState;
	RasterizerState.SetCullDirection(ZE_CD_NONE);
	RasterizerState.SetFillMode(WireFrame ? ZE_FM_WIREFRAME : ZE_FM_SOLID);
	
	static ZEDepthStencilState DepthStencilState;
	DepthStencilState.SetZTestEnable(false);
	
	static ZEBlendState BlendState;
	BlendState.SetBlendEnable(0, true);
	BlendState.SetBlendEquation(ZE_BE_ADD);
	BlendState.SetSourceBlendOption(ZE_BO_SRC_ALPHA);
	BlendState.SetDestinationBlendOption(ZE_BO_INV_SRC_ALPHA);
	
	static ZESamplerState SamplerState;
	SamplerState.SetMinFilter(ZE_TFM_LINEAR);
	SamplerState.SetMagFilter(ZE_TFM_LINEAR);
	SamplerState.SetMipFilter(ZE_TFM_POINT);
	SamplerState.SetAddressU(ZE_TAM_WRAP);
	SamplerState.SetAddressV(ZE_TAM_WRAP);

	ZEUIConstantBuffer* Data = NULL;
	VertexShaderBuffer->Lock((void**)&Data);
	Data->WorldMatrix = RenderCommand->WorldMatrix;
	VertexShaderStage.SetBuffer(0, VertexShaderBuffer);
	VertexShaderStage.SetShader(VertexShader);

	PixelShaderStage.SetShader(PixelShader);

	TexturedPixelShaderStage.SetTexture(0, Texture);
	TexturedPixelShaderStage.SetSamplerState(0, &SamplerState);
	TexturedPixelShaderStage.SetShader(PixelShaderTextured);

	Device->SetBlendState(&BlendState);
	Device->SetRasterizerState(&RasterizerState);
	Device->SetDepthStencilState(&DepthStencilState);
	Device->SetVertexShaderStage(&VertexShaderStage);
	if (Texture != NULL)
		Device->SetPixelShaderStage(&PixelShaderStage);
	else
		Device->SetPixelShaderStage(&TexturedPixelShaderStage);

	return true;
}
	*/
ZEMaterialUserInterface* ZEMaterialUserInterface::CreateInstance()
{
	return NULL; //  new ZEMaterialUserInterface();
}
