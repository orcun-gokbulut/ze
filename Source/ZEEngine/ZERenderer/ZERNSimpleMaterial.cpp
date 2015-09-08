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

bool ZERNSimpleMaterial::InitializeSelf()
{
	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));
	ConstantBuffer->SetData(&Constants);

	ZEGRRenderState RenderState = ZERNStageForward::GetRenderState();
	RenderState.SetPrimitiveType(ZEGR_PT_LINE_LIST);
	RenderState.SetVertexLayout(*ZECanvasVertex::GetVertexLayout());

	ZEGRDepthStencilState DepthStencilState = RenderState.GetDepthStencilState();
	DepthStencilState.SetDepthTestEnable(false);
	RenderState.SetDepthStencilState(DepthStencilState);

	ZEGRRasterizerState RasterizerState = RenderState.GetRasterizerState();
	RasterizerState.SetCullDirection(ZEGR_CD_NONE);
	RenderState.SetRasterizerState(RasterizerState);

	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNSimpleMaterial.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNSimpleMaterial_VSMain_ForwardStage";
	ZEGRShader* VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(VertexShader == NULL, false, "Cannot compile vertex shader.");
	RenderState.SetShader(Options.Type, VertexShader);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSimpleMaterial_PSMain_ForwardStage";
	ZEGRShader* PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(PixelShader == NULL, false, "Cannot compile pixel shader.");
	RenderState.SetShader(Options.Type, PixelShader);

	this->RenderState = RenderState.Compile();
	zeCheckError(this->RenderState == NULL, false, "Cannot compile render state.");

	return true;
}

void ZERNSimpleMaterial::DeinitializeSelf()
{
	RenderStateData.Release();
	ConstantBuffer.Release();
}

ZERNSimpleMaterial::ZERNSimpleMaterial()
{
	Wireframe = false;
	TwoSided = false;
	VertexColorEnabled = true;
	Constants.TransparancyCullLimit = 0x80;
	Constants.MaterialColor = ZEVector4::One;
}

ZERNStageMask ZERNSimpleMaterial::GetStageMask()
{
	return ZERN_STAGE_FORWARD;
}

void ZERNSimpleMaterial::SetTwoSided(bool Enable)
{
	TwoSided = Enable;
}

bool ZERNSimpleMaterial::GetTwoSided() const
{
	return TwoSided;
}

void ZERNSimpleMaterial::SetWireframe(bool Enable)
{
	Wireframe = Enable;
}

bool ZERNSimpleMaterial::GetWireframe() const
{
	return Wireframe;
}

void ZERNSimpleMaterial::SetVertexColor(bool Enable)
{
	VertexColorEnabled = Enable;
}

bool ZERNSimpleMaterial::GetVertexColor()
{
	return VertexColorEnabled;
}

void ZERNSimpleMaterial::SetMaterialColor(const ZEVector4& Color)
{
	if (Constants.MaterialColor == Color)
		return;

	Constants.MaterialColor = Color;
}

const ZEVector4& ZERNSimpleMaterial::GetMaterialColor() const
{
	return Constants.MaterialColor;
}

void ZERNSimpleMaterial::SetTexture(const ZERNSampler& Sampler)
{
	TextureSampler = Sampler;
}

const ZERNSampler& ZERNSimpleMaterial::GetTexture() const
{
	return TextureSampler;
}

bool ZERNSimpleMaterial::SetupMaterial(ZEGRContext* Context, ZERNStage* Stage)
{
	if (!ZERNMaterial::SetupMaterial(Context, Stage))
		return false;

	Context->SetRenderState(RenderState);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, TextureSampler.GetTexture());
	Context->SetSampler(ZEGR_ST_PIXEL, 0, TextureSampler.GetSamplerState());
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_MATERIAL, ConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_MATERIAL, ConstantBuffer);

	return true;
}

ZERNSimpleMaterial* ZERNSimpleMaterial::CreateInstance()
{
	return new ZERNSimpleMaterial();
}
