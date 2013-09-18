//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterialSimple.cpp
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
#include "ZERenderer.h"
#include "ZERenderStage.h"
#include "ZEGame/ZEScene.h"
#include "ZERenderCommand.h"
#include "ZEMaterialSimple.h"
#include "ZEGraphics/ZEShader.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEShaderCompiler.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEBlendState.h"
#include "ZEGraphics/ZESamplerState.h"
#include "ZEGraphics/ZEDepthStencilState.h"
#include "ZEGraphics/ZERasterizerState.h"
#include "ZEDS/ZEHashGenerator.h"


struct SimpleMaterialVSData
{
	ZEMatrix4x4 TransformationMatrix;
};

struct SimpleMaterialPSData
{
	ZEVector4	MaterialColor;
	ZEInt32		EnableTexture;
	ZEInt32		EnableVertexColor;
};

ZEMaterialSimple::ZEMaterialSimple()
{
	Texture = NULL;
	VertexShader = NULL;
	PixelShader = NULL;
	Wireframe = false;
	TwoSided = false;
	VertexColorEnabled = true;
	TransparancyMode = ZE_TM_NONE;
	TransparancyCullLimit = 128;
	MaterialColor = ZEVector4::One;
	TextureAddressModeU = ZE_TAM_MIRROR;
	TextureAddressModeV = ZE_TAM_MIRROR;

	CreateShaders();
}

ZEMaterialSimple::~ZEMaterialSimple()
{
	ReleaseShaders();
}

void ZEMaterialSimple::CreateShaders()
{
	ZEShaderCompileOptions Options;
	
	Options.Model = ZE_SM_4_0;
	Options.Type = ZE_ST_VERTEX;
	Options.FileName = "SimpleMaterial.hlsl";
	Options.EntryPoint = "VSMain";
	VertexShader = ZEShaderCompiler::CompileShaderFromFile(&Options);
	
	VertexShaderData = ZEConstantBuffer::CreateInstance();
	VertexShaderData->Create(VertexShader->GetMetaTable()->GetBufferInfo("SimpleMaterialVSData"));

	Options.Model = ZE_SM_4_0;
	Options.Type = ZE_ST_PIXEL;
	Options.FileName = "SimpleMaterial.hlsl";
	Options.EntryPoint = "PSMain";
	PixelShader = ZEShaderCompiler::CompileShaderFromFile(&Options);
	
	PixelShaderData = ZEConstantBuffer::CreateInstance();
	PixelShaderData->Create(VertexShader->GetMetaTable()->GetBufferInfo("SimpleMaterialPSData"));
}

void ZEMaterialSimple::ReleaseShaders()
{
	ZE_DESTROY(VertexShader);
	ZE_DESTROY(PixelShader);
	ZE_DESTROY(VertexShaderData);
	ZE_DESTROY(PixelShaderData);
}

ZESize ZEMaterialSimple::GetHash() const
{
	return ZEHashGenerator::Hash(ZEString("ZEMaterialSimple"));
}

void ZEMaterialSimple::SetTwoSided(bool Enable)
{
	TwoSided = Enable;
}

bool ZEMaterialSimple::GetTwoSided() const
{
	return TwoSided;
}

void ZEMaterialSimple::SetWireframe(bool Enable)
{
	Wireframe = Enable;
}

bool ZEMaterialSimple::GetWireframe() const
{
	return Wireframe;
}

void ZEMaterialSimple::SetVertexColor(bool Enable)
{
	VertexColorEnabled = Enable;
}

bool ZEMaterialSimple::GetVertexColor()
{
	return VertexColorEnabled;
}

void ZEMaterialSimple::SetMaterialColor(const ZEVector4& Color)
{
	MaterialColor = Color;
}

const ZEVector4& ZEMaterialSimple::GetMaterialColor() const
{
	return MaterialColor;
}

void ZEMaterialSimple::SetTransparancyMode(ZETransparancyMode Mode)
{
	TransparancyMode = Mode;
}

ZETransparancyMode ZEMaterialSimple::GetTransparancyMode() const
{
	return TransparancyMode;
}

void ZEMaterialSimple::SetTransparancyCullLimit(ZEUInt Limit)
{
	Limit = TransparancyCullLimit;
}

ZEUInt ZEMaterialSimple::GetTransparancyCullLimit() const
{
	return TransparancyCullLimit;
}

void ZEMaterialSimple::SetTexture(ZETexture2D* Texture)
{
	this->Texture = Texture;
}

ZETexture2D* ZEMaterialSimple::GetTexture() const
{
	return Texture;
}

void ZEMaterialSimple::SetTextureAddressModeU(ZETextureAddressMode Mode)
{
	TextureAddressModeU = Mode;
}

ZETextureAddressMode ZEMaterialSimple::GetTextureAddressModeU() const
{
	return TextureAddressModeU;
}

void ZEMaterialSimple::SetTextureAddressModeV(ZETextureAddressMode Mode)
{
	TextureAddressModeV = Mode;
}

ZETextureAddressMode ZEMaterialSimple::GetTextureAddressModeV() const
{
	return TextureAddressModeV;
}

bool ZEMaterialSimple::SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	/*
	if (Stage->GetStageIndentifier() != ZE_RENDER_STAGE_FORWARD)
		return true;

	ZECamera* Camera = zeScene->GetActiveCamera();
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetVertexShader(VertexShader);
	Device->SetPixelShader(PixelShader);

	// Setup Transformations
	ZEMatrix4x4 ViewProjMatrix;
	if (RenderCommand->Flags & ZE_RCF_VIEW_PROJECTION_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetViewProjectionTransform();
	}
	else if (RenderCommand->Flags & ZE_RCF_VIEW_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetViewTransform();
	}
	else if (RenderCommand->Flags & ZE_RCF_PROJECTION_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetProjectionTransform();
	}
	else
	{
		ViewProjMatrix = ZEMatrix4x4::Identity;
	}

	ZEMatrix4x4 WorldViewMatrix;
	ZEMatrix4x4 WorldViewProjMatrix;
	if (RenderCommand->Flags & ZE_RCF_WORLD_TRANSFORM)
	{
		ZEMatrix4x4::Multiply(WorldViewProjMatrix, ViewProjMatrix, RenderCommand->WorldMatrix);
	}
	else
	{
		WorldViewProjMatrix = ViewProjMatrix;
	}

	SimpleMaterialVSData* VSConstantData = NULL;
	VertexShaderData->Lock((void**)&VSConstantData);
	
		VSConstantData->TransformationMatrix = WorldViewProjMatrix;
	
	VertexShaderData->Unlock();
	Device->SetVertexShaderBuffer(0, VertexShaderData);
	

	SimpleMaterialPSData* PSConstantData = NULL;
	PixelShaderData->Lock((void**)&PSConstantData);
	
		PSConstantData->MaterialColor = MaterialColor;
		PSConstantData->EnableTexture = (Texture != NULL);
		PSConstantData->EnableVertexColor = VertexColorEnabled;
	
	PixelShaderData->Unlock();
	Device->SetPixelShaderBuffer(0, PixelShaderData);

	ZEDepthStencilState DepthStencilState;
	DepthStencilState.SetZFunction(ZE_CF_LESS_EQUAL);
	DepthStencilState.SetZTestEnable((RenderCommand->Flags & ZE_RCF_Z_CULL) == 0 ? false : true);
	DepthStencilState.SetZWriteEnable((RenderCommand->Flags & ZE_RCF_NO_Z_WRITE) == 0 ? true : false);
	Device->SetDepthStencilState(DepthStencilState);
	
	ZERasterizerState RasterizerState;
	RasterizerState.SetCullDirection(TwoSided ? ZE_CD_NONE : ZE_CD_COUNTER_CLOCKWISE);
	RasterizerState.SetFillMode(Wireframe ? ZE_FM_WIREFRAME : ZE_FM_SOLID);
	Device->SetRasterizerState(RasterizerState);
	
	// Setup Transparancy
	ZEBlendState BlendState;
	if (TransparancyMode != ZE_TM_NONE)
	{	
		BlendState.SetBlendEnable(0, true);

		switch(TransparancyMode)
		{
			case ZE_TM_ADDAPTIVE:
				BlendState.SetBlendEquation(ZE_BE_ADD);
				BlendState.SetSourceBlendOption(ZE_BO_SRC_ALPHA);
				BlendState.SetDestinationBlendOption(ZE_BO_ONE);
				break;
			case ZE_TM_SUBTRACTIVE:
				BlendState.SetBlendEquation(ZE_BE_SUBTRACT);
				BlendState.SetSourceBlendOption(ZE_BO_SRC_ALPHA);
				BlendState.SetDestinationBlendOption(ZE_BO_ONE);
				break;
			case ZE_TM_REGULAR:
				BlendState.SetBlendEquation(ZE_BE_ADD);
				BlendState.SetSourceBlendOption(ZE_BO_SRC_ALPHA);
				BlendState.SetDestinationBlendOption(ZE_BO_INV_SRC_ALPHA);
				break;
		}
	}
	else
	{
		BlendState.SetBlendEnable(0, false);
	}
	Device->SetBlendState(BlendState);

	ZESamplerState SamplerState;
	SamplerState.SetAddressU(TextureAddressModeU);
	SamplerState.SetAddressV(TextureAddressModeV);
	SamplerState.SetMinFilter(ZE_TFM_LINEAR);
	SamplerState.SetMagFilter(ZE_TFM_LINEAR);
	SamplerState.SetMipFilter(ZE_TFM_LINEAR);
	
	Device->SetPixelShaderSampler(5, SamplerState);
	Device->SetPixelShaderTexture(5, Texture);

	*/
	return true;
}

	/*
bool ZEMaterialSimple::SetupForwardPass(ZERenderer* Renderer, ZERenderCommand* RenderCommand)
{

	static ZEDeviceStageShader VertexShaderStage;
	static ZEDeviceStageShader PixelShaderStage;
	
	ZECamera* Camera = Renderer->GetCamera();
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();
	
	// Setup Transformations
	ZEMatrix4x4 ViewProjMatrix;
	if ((RenderCommand->Flags & ZE_RCF_VIEW_PROJECTION_TRANSFORM) == ZE_RCF_VIEW_PROJECTION_TRANSFORM)
		ViewProjMatrix = Camera->GetViewProjectionTransform();
	else if (RenderCommand->Flags & ZE_RCF_VIEW_TRANSFORM)
		ViewProjMatrix = Camera->GetViewTransform();
	else if (RenderCommand->Flags & ZE_RCF_PROJECTION_TRANSFORM)
		ViewProjMatrix = Camera->GetProjectionTransform();
	else
		ViewProjMatrix = ZEMatrix4x4::Identity;

	ZEMatrix4x4 WorldViewMatrix;
	ZEMatrix4x4 WorldViewProjMatrix;
	if (RenderCommand->Flags & ZE_RCF_WORLD_TRANSFORM)
		ZEMatrix4x4::Multiply(WorldViewProjMatrix, ViewProjMatrix, RenderCommand->WorldMatrix);
	else
		WorldViewProjMatrix = ViewProjMatrix;

	SimpleMaterialVSData* VSConstantData = NULL;
	VertexShaderData->Lock((void**)&VSConstantData);
	VSConstantData->TransformationMatrix = WorldViewProjMatrix;
	VertexShaderData->Unlock();
	VertexShaderStage.SetBuffer(0, VertexShaderData);
	VertexShaderStage.SetShader(VertexShader);

	SimpleMaterialPSData* PSConstantData = NULL;
	PixelShaderData->Lock((void**)&PSConstantData);
	PSConstantData->MaterialColor = MaterialColor;
	PSConstantData->EnableTexture = (Texture != NULL);
	PSConstantData->EnableVertexColor = VertexColorEnabled;
	PixelShaderData->Unlock();
	PixelShaderStage.SetBuffer(0, PixelShaderData);

	static ZEDepthStencilState DepthStencilState;
	DepthStencilState.SetZFunction(ZE_CF_LESS_EQUAL);
	DepthStencilState.SetZTestEnable((RenderCommand->Flags & ZE_RCF_Z_CULL) == 0 ? false : true);
	DepthStencilState.SetZWriteEnable((RenderCommand->Flags & ZE_RCF_NO_Z_WRITE) == 0 ? true : false);
	Device->SetDepthStencilState(&DepthStencilState);
	
	ZERasterizerState RasterizerState;
	RasterizerState.SetCullDirection(TwoSided ? ZE_CD_NONE : ZE_CD_COUNTER_CLOCKWISE);
	RasterizerState.SetFillMode(Wireframe ? ZE_FM_WIREFRAME : ZE_FM_SOLID);
	Device->SetRasterizerState(&RasterizerState);
	
	// Setup Transparancy
	static ZEBlendState BlendState;
	if (TransparancyMode != ZE_MTM_NONE)
	{	
		BlendState.SetBlendEnable(0, true);

		switch(TransparancyMode)
		{
			case ZE_MTM_ADDAPTIVE:
				BlendState.SetBlendEquation(ZE_BE_ADD);
				BlendState.SetSourceBlendOption(ZE_BO_SRC_ALPHA);
				BlendState.SetDestinationBlendOption(ZE_BO_ONE);
				break;
			case ZE_MTM_SUBTRACTIVE:
				BlendState.SetBlendEquation(ZE_BE_SUBTRACT);
				BlendState.SetSourceBlendOption(ZE_BO_SRC_ALPHA);
				BlendState.SetDestinationBlendOption(ZE_BO_ONE);
				break;
			case ZE_MTM_REGULAR:
				BlendState.SetBlendEquation(ZE_BE_ADD);
				BlendState.SetSourceBlendOption(ZE_BO_SRC_ALPHA);
				BlendState.SetDestinationBlendOption(ZE_BO_INV_SRC_ALPHA);
				break;
		}
	}
	else
	{
		BlendState.SetBlendEnable(0, false);
	}

	static ZESamplerState SamplerState;
	SamplerState.SetAddressU(TextureAddressModeU);
	SamplerState.SetAddressV(TextureAddressModeV);
	SamplerState.SetMinFilter(ZE_TFM_LINEAR);
	SamplerState.SetMagFilter(ZE_TFM_LINEAR);
	SamplerState.SetMipFilter(ZE_TFM_LINEAR);
	PixelShaderStage.SetSamplerState(5, &SamplerState);
	PixelShaderStage.SetTexture(5, Texture);
	PixelShaderStage.SetShader(PixelShader);
	
	Device->SetVertexShaderStage(&VertexShaderStage);
	Device->SetPixelShaderStage(&PixelShaderStage);

	return true;
}
*/

ZEMaterialSimple* ZEMaterialSimple::CreateInstance()
{
	return NULL; //new ZEMaterialSimple();
}
