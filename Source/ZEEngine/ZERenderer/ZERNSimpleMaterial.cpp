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

#include "ZECanvas.h"
#include "ZERNCommand.h"
#include "ZERNRenderer.h"
#include "ZERNShaderSlots.h"
#include "ZERNStageForward.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRShaderCompileOptions.h"

#define ZERN_SMDF_RENDER_STATE		1
#define ZERN_SMDF_CONSTANT_BUFFER	2
#define ZERN_SMDF_SHADERS			4

bool ZERNSimpleMaterial::UpdateShaders() const
{
	if (!DirtyFlags.GetFlags(ZERN_SMDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNSimpleMaterial.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNSimpleMaterial_VSMain_ForwardStage";
	VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(VertexShader == NULL, false, "Cannot compile vertex shader");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSimpleMaterial_PSMain_ForwardStage";
	PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(PixelShader == NULL, false, "Cannot compile pixel shader");

	DirtyFlags.UnraiseFlags(ZERN_SMDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_SMDF_RENDER_STATE);

	return true;
}

bool ZERNSimpleMaterial::UpdateRenderState() const
{
	if (!DirtyFlags.GetFlags(ZERN_SMDF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState;
	
	if (GetTransparent())
		RenderState = ZERNStageForwardTransparent::GetRenderState();
	else
		RenderState = ZERNStageForward::GetRenderState();

	RenderState.SetPrimitiveType(PrimitiveType);
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
	zeCheckError(RenderStateData == NULL, false, "Cannot set render state");

	DirtyFlags.UnraiseFlags(ZERN_SMDF_RENDER_STATE);

	return true;
}

bool ZERNSimpleMaterial::UpdateConstantBuffer() const
{
	if (!DirtyFlags.GetFlags(ZERN_SMDF_CONSTANT_BUFFER))
		return true;
		
	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_SMDF_CONSTANT_BUFFER);

	return true;
}

bool ZERNSimpleMaterial::InitializeInternal()
{
	if (!ZERNMaterial::InitializeInternal())
		return false;

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));

	if (!UpdateShaders())
		return false;

	if (!UpdateRenderState())
		return false;

	if (!UpdateConstantBuffer())
		return false;

	return true;
}

bool ZERNSimpleMaterial::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();
	StageMask = 0;

	VertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
	ConstantBuffer.Release();

	return ZERNMaterial::DeinitializeInternal();
}

ZERNSimpleMaterial::ZERNSimpleMaterial()
{
	DirtyFlags.RaiseAll();
	StageMask = 0;
	
	TwoSided = false;
	Wireframe = false;
	DepthTestDisabled = false;
	Transparent = false;
	PrimitiveType = ZEGR_PT_LINE_LIST;
	StageMask = ZERN_STAGE_FORWARD;

	Constants.MaterialColor = ZEVector4::One;
	Constants.TextureEnabled = false;
	Constants.VertexColorEnabled = true;
}

void ZERNSimpleMaterial::SetStageMask(ZERNStageMask Mask)
{
	StageMask = Mask;
}

ZERNStageMask ZERNSimpleMaterial::GetStageMask() const
{
	return StageMask;
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

void ZERNSimpleMaterial::SetPrimitiveType(ZEGRPrimitiveType Type)
{
	if (PrimitiveType == Type)
		return;

	PrimitiveType = Type;
	DirtyFlags.RaiseFlags(ZERN_SMDF_RENDER_STATE);
}

ZEGRPrimitiveType ZERNSimpleMaterial::GetPrimitiveType() const
{
	return PrimitiveType;
}

void ZERNSimpleMaterial::SetDepthTestDisabled(bool Disabled)
{
	if (DepthTestDisabled == Disabled)
		return;

	DepthTestDisabled = Disabled;
	DirtyFlags.RaiseFlags(ZERN_SMDF_RENDER_STATE);
}

bool ZERNSimpleMaterial::GetDepthTestDisabled() const
{
	return DepthTestDisabled;
}

void ZERNSimpleMaterial::SetTransparent(bool Transparent)
{
	if (this->Transparent == Transparent)
		return;

	this->Transparent = Transparent;
	DirtyFlags.RaiseFlags(ZERN_SMDF_RENDER_STATE);
}

bool ZERNSimpleMaterial::GetTransparent() const
{
	return Transparent;
}

void ZERNSimpleMaterial::SetOpacity(float Opacity)
{
	if (Constants.Opacity == Opacity)
		return;

	Constants.Opacity = Opacity;
	DirtyFlags.RaiseFlags(ZERN_SMDF_RENDER_STATE);
}

float ZERNSimpleMaterial::GetOpacity() const
{
	return Constants.Opacity;
}

void ZERNSimpleMaterial::SetVertexColorEnabled(bool Enabled)
{
	if ((bool)Constants.VertexColorEnabled == Enabled)
		return;

	Constants.VertexColorEnabled = Enabled;
	DirtyFlags.RaiseFlags(ZERN_SMDF_CONSTANT_BUFFER);
}

bool ZERNSimpleMaterial::GetVertexColorEnabled() const
{
	return (bool)Constants.VertexColorEnabled;
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

void ZERNSimpleMaterial::SetTexture(const ZERNMap& Map)
{
	TextureMap = Map;
	
	bool TextureEnabled = (Map.GetTexture() != NULL);
	if ((bool)Constants.TextureEnabled == TextureEnabled)
		return;
	
	Constants.TextureEnabled = TextureEnabled;
	DirtyFlags.RaiseFlags(ZERN_SMDF_CONSTANT_BUFFER);
}

const ZERNMap& ZERNSimpleMaterial::GetTexture() const
{
	return TextureMap;
}

bool ZERNSimpleMaterial::SetupMaterial(ZEGRContext* Context, const ZERNStage* Stage) const
{
	if (!ZERNMaterial::SetupMaterial(Context, Stage))
		return false;

	if (!Update())
		return false;

	Context->SetConstantBuffers(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_MATERIAL, 1, ConstantBuffer.GetPointerToPointer());
	Context->SetConstantBuffers(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_MATERIAL, 1, ConstantBuffer.GetPointerToPointer());

	Context->SetRenderState(RenderStateData);

	if (Constants.TextureEnabled)
	{
		const ZEGRSampler* Sampler = TextureMap.GetSampler();
		Context->SetSamplers(ZEGR_ST_PIXEL, 0, 1, &Sampler);

		const ZEGRTexture* Texture = TextureMap.GetTexture();
		Context->SetTextures(ZEGR_ST_PIXEL, 0, 1, &Texture);
	}

	return true;
}

void ZERNSimpleMaterial::CleanupMaterial(ZEGRContext* Context, const ZERNStage* Stage) const
{
	ZERNMaterial::CleanupMaterial(Context, Stage);
}

bool ZERNSimpleMaterial::Update() const
{
	if (!ZERNMaterial::Update())
		return false;

	if (!UpdateShaders())
		return false;

	if (!UpdateRenderState())
		return false;

	if (!UpdateConstantBuffer())
		return false;

	return true;
}

bool ZERNSimpleMaterial::PreRender(ZERNCommand& Command)
{
	Command.StageMask = ZERN_STAGE_DEBUG;
	if (GetTransparent())
		Command.StageMask |= ZERN_STAGE_FORWARD_TRANSPARENT;
	else
		Command.StageMask |= StageMask;

	return StageMask;
}

ZEHolder<ZERNSimpleMaterial> ZERNSimpleMaterial::CreateInstance()
{
	return new ZERNSimpleMaterial();
}
