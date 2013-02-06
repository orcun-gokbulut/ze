//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIMaterial.cpp
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


#include "ZEUIMaterial.h"
#include "ZERenderCommand.h"
#include "ZEGraphics/ZEShader.h"
#include "ZEGraphics/ZETexture.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGraphics/ZEShaderCompiler.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEConstantBuffer.h"
#include "ZEGraphics/ZEShaderCompileOptions.h"
#include "ZERenderStage.h"


ZEShader* ZEUIMaterial::VertexShader = NULL;
ZEShader* ZEUIMaterial::PixelShader = NULL;
ZEShader* ZEUIMaterial::PixelShaderTextured = NULL;

struct ZEUIConstantBuffer
{
	ZEMatrix4x4 WorldMatrix;
};

ZEUIMaterial::ZEUIMaterial()
{
	Texture = NULL;
	WireFrame = false;

	CreateShaders();
}

ZEUIMaterial::~ZEUIMaterial()
{
	DestroyShaders();
}

void ZEUIMaterial::CreateShaders()
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

void ZEUIMaterial::DestroyShaders()
{
	ZE_DESTROY(VertexShaderBuffer);
}

ZEUInt32 ZEUIMaterial::GetHash() const
{
	return 1 << 30;
}

ZEMaterialFlags ZEUIMaterial::GetMaterialFlags() const
{
	return ZE_MTF_NONE;
}

void ZEUIMaterial::SetWireFrame(bool Enabled)
{
	WireFrame = Enabled;
}

bool ZEUIMaterial::GetWireFrame() const
{
	return WireFrame;
}

void ZEUIMaterial::SetTexture(const ZETexture2D* Texture)
{
	this->Texture = Texture;
}

const ZETexture2D* ZEUIMaterial::GetTexture() const
{
	return Texture;
}

bool ZEUIMaterial::SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	if (Stage->GetStageFlags() != ZE_RENDER_STAGE_UI)
		return true;

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	ZEUIConstantBuffer* Data = NULL;
	VertexShaderBuffer->Lock((void**)&Data);
	
		Data->WorldMatrix = RenderCommand->WorldMatrix;
	
	VertexShaderBuffer->Unlock();
	Device->SetVertexShaderBuffer(0, VertexShaderBuffer);
		
	ZEDeviceStateRasterizer RasterizerState;
	RasterizerState.SetCullDirection(ZE_CD_COUNTER_CLOCKWISE);
	RasterizerState.SetFillMode(WireFrame ? ZE_FM_WIREFRAME : ZE_FM_SOLID);
	Device->SetRasterizerState(RasterizerState);

	ZEDeviceStateDepthStencil DepthStencilState;
	DepthStencilState.SetZTestEnable(false);
	Device->SetDepthStencilState(DepthStencilState);

	ZEDeviceStateBlend BlendState;
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
	
		ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_LINEAR);
		SamplerState.SetMagFilter(ZE_TFM_LINEAR);
		SamplerState.SetMipFilter(ZE_TFM_POINT);
		SamplerState.SetAddressU(ZE_TAM_WRAP);
		SamplerState.SetAddressV(ZE_TAM_WRAP);

		Device->SetPixelShaderSampler(0, SamplerState);
		Device->SetPixelShaderTexture(0, Texture);
	}

	return true;
}

/*
bool ZEUIMaterial::SetupForwardPass(ZERenderer* Renderer, ZERenderCommand* RenderCommand) 
{

	static ZEDeviceStageShader VertexShaderStage;
	static ZEDeviceStageShader PixelShaderStage;
	static ZEDeviceStageShader TexturedPixelShaderStage;

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	static ZEDeviceStateRasterizer RasterizerState;
	RasterizerState.SetCullDirection(ZE_CD_NONE);
	RasterizerState.SetFillMode(WireFrame ? ZE_FM_WIREFRAME : ZE_FM_SOLID);
	
	static ZEDeviceStateDepthStencil DepthStencilState;
	DepthStencilState.SetZTestEnable(false);
	
	static ZEDeviceStateBlend BlendState;
	BlendState.SetBlendEnable(0, true);
	BlendState.SetBlendEquation(ZE_BE_ADD);
	BlendState.SetSourceBlendOption(ZE_BO_SRC_ALPHA);
	BlendState.SetDestinationBlendOption(ZE_BO_INV_SRC_ALPHA);
	
	static ZEDeviceStateSampler SamplerState;
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
void ZEUIMaterial::UpdateMaterial()
{	

}

ZEUIMaterial* ZEUIMaterial::CreateInstance()
{
	return new ZEUIMaterial();
}
