//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNSimpleMaterial.cpp
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

#include "ZERNSimpleMaterial.h"

#include "ZERNRenderer.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRShaderCompileOptions.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZERNStageManager.h"
#include "ZERNStage.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZERNShaderConfig.h"
#include "ZERNStageForward.h"
#include "ZECanvas.h"

#define ZERN_SMDF_RENDER_STATE		1
#define ZERN_SMDF_CONSTANT_BUFFER	2
#define ZERN_SMDF_SHADERS			4

void ZERNSimpleMaterial::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_SMDF_RENDER_STATE))
		return;

	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNSimpleMaterial.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNSimpleMaterial_VSMain_ForwardStage";
	VertexShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSimpleMaterial_PSMain_ForwardStage";
	PixelShader = ZEGRShader::Compile(Options);

	DirtyFlags.UnraiseFlags(ZERN_SMDF_RENDER_STATE);
	DirtyFlags.RaiseFlags(ZERN_SMDF_RENDER_STATE);
}

void ZERNSimpleMaterial::UpdateRenderState()
{
	if (!DirtyFlags.GetFlags(ZERN_SMDF_RENDER_STATE))
		return;

	ZEGRRenderState RenderState = ZERNStageForward::GetRenderState();
	RenderState.SetPrimitiveType(ZEGR_PT_LINE_LIST);
	RenderState.SetVertexLayout(*ZECanvasVertex::GetVertexLayout());

	if (DepthTestDisabled)
	{
		ZEGRDepthStencilState DepthStencilState = RenderState.GetDepthStencilState();
		DepthStencilState.SetDepthTestEnable(false);
		RenderState.SetDepthStencilState(DepthStencilState);
	}

	ZEGRRasterizerState RasterizerState = RenderState.GetRasterizerState();
	RasterizerState.SetCullMode(TwoSided ? ZEGR_CMD_NONE : RasterizerState.GetCullMode());
	RasterizerState.SetFillMode(Wireframe ? ZEGR_FM_WIREFRAME : ZEGR_FM_SOLID);
	RenderState.SetRasterizerState(RasterizerState);

	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PixelShader);

	RenderStateData = RenderState.Compile();

	DirtyFlags.UnraiseFlags(ZERN_SMDF_RENDER_STATE);
}

void ZERNSimpleMaterial::UpdateConstantBuffer()
{
	if (!DirtyFlags.GetFlags(ZERN_SMDF_CONSTANT_BUFFER))
		return
		
	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_SMDF_CONSTANT_BUFFER);
}

bool ZERNSimpleMaterial::InitializeSelf()
{
	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));

	return Update();
}

void ZERNSimpleMaterial::DeinitializeSelf()
{
	VertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
	ConstantBuffer.Release();

	DirtyFlags.RaiseAll();
}

ZERNSimpleMaterial::ZERNSimpleMaterial()
{
	Wireframe = false;
	TwoSided = false;
	DepthTestDisabled = false;
	Constants.VertexColorEnabled = true;
	Constants.TextureEnabled = false;
	Constants.MaterialColor = ZEVector4::One;
	DirtyFlags.RaiseAll();
}

ZERNStageMask ZERNSimpleMaterial::GetStageMask()
{
	return ZERN_STAGE_FORWARD;
}

void ZERNSimpleMaterial::SetTwoSided(bool Enable)
{
	if (TwoSided == Enable)
		return;

	TwoSided = Enable;
	DirtyFlags.RaiseFlags(ZERN_SMDF_RENDER_STATE);
}

bool ZERNSimpleMaterial::GetTwoSided() const
{
	return TwoSided;
}

void ZERNSimpleMaterial::SetWireframe(bool Enable)
{
	if (Wireframe == Enable)
		return;

	Wireframe = Enable;
	DirtyFlags.RaiseFlags(ZERN_SMDF_RENDER_STATE);
}

bool ZERNSimpleMaterial::GetWireframe() const
{
	return Wireframe;
}

void ZERNSimpleMaterial::SetDepthTestDisabled(bool Disabled)
{
	if (DepthTestDisabled == Disabled)
		return;

	DepthTestDisabled = Disabled;
	DirtyFlags.RaiseFlags(ZERN_SMDF_RENDER_STATE);
}

bool ZERNSimpleMaterial::GetDepthTestDisabled()
{
	return DepthTestDisabled;
}

void ZERNSimpleMaterial::SetVertexColorEnabled(bool Enable)
{
	if (Constants.VertexColorEnabled == Enable)
		return;

	Constants.VertexColorEnabled = Enable;
	DirtyFlags.RaiseFlags(ZERN_SMDF_CONSTANT_BUFFER);
}

bool ZERNSimpleMaterial::GetVertexColorEnabled()
{
	return Constants.VertexColorEnabled;
}

void ZERNSimpleMaterial::SetMaterialColor(const ZEVector4& Color)
{
	if (Constants.MaterialColor == Color)
		return;

	Constants.MaterialColor = Color;
	DirtyFlags.RaiseFlags(ZERN_SMDF_CONSTANT_BUFFER);
}

const ZEVector4& ZERNSimpleMaterial::GetMaterialColor() const
{
	return Constants.MaterialColor;
}

void ZERNSimpleMaterial::SetTexture(const ZERNMap& Sampler)
{
	TextureMap = Sampler;
	
	bool TextureEnabled = (Sampler.GetTexture() != NULL);
	if (Constants.TextureEnabled == TextureEnabled)
		return;
	
	Constants.TextureEnabled = TextureEnabled;
	DirtyFlags.RaiseFlags(ZERN_SMDF_CONSTANT_BUFFER);
}

const ZERNMap& ZERNSimpleMaterial::GetTexture() const
{
	return TextureMap;
}

bool ZERNSimpleMaterial::SetupMaterial(ZEGRContext* Context, ZERNStage* Stage)
{
	if (!ZERNMaterial::SetupMaterial(Context, Stage))
		return false;

	if(!Update())
		return false;

	Context->SetRenderState(RenderStateData);

	if (Constants.TextureEnabled)
	{
		Context->SetTexture(ZEGR_ST_PIXEL, 0, TextureMap.GetTexture());
		Context->SetSampler(ZEGR_ST_PIXEL, 0, TextureMap.GetSamplerState());
	}

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_MATERIAL, ConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_MATERIAL, ConstantBuffer);

	return true;
}

void ZERNSimpleMaterial::CleanupMaterial(ZEGRContext* Context, ZERNStage* Stage)
{
	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_MATERIAL, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_MATERIAL, NULL);
}

bool ZERNSimpleMaterial::Update()
{
	UpdateShaders();
	UpdateRenderState();
	UpdateConstantBuffer();

	return true;
}

ZERNSimpleMaterial* ZERNSimpleMaterial::CreateInstance()
{
	return new ZERNSimpleMaterial();
}
