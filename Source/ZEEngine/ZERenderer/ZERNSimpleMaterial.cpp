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

#include "ZEFile/ZEPathInfo.h"
#include "ZERNCommand.h"
#include "ZERNRenderer.h"
#include "ZERNShaderSlots.h"
#include "ZERNStageForward.h"
#include "ZEGraphics/ZEGRCanvas.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRShaderCompileOptions.h"

#define ZERN_SMDF_RENDER_STATE		1
#define ZERN_SMDF_CONSTANT_BUFFER	2
#define ZERN_SMDF_SHADERS			4

bool ZERNSimpleMaterial::UpdateShaders()
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

bool ZERNSimpleMaterial::UpdateRenderState()
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

bool ZERNSimpleMaterial::UpdateConstantBuffer()
{
	if (!DirtyFlags.GetFlags(ZERN_SMDF_CONSTANT_BUFFER))
		return true;
		
	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_SMDF_CONSTANT_BUFFER);

	return true;
}

ZETaskResult ZERNSimpleMaterial::LoadInternal()
{
	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);
	if (ConstantBuffer == NULL)
		return ZE_TR_FAILED;

	if (!TextureFileName.IsEmpty())
	{
		ZEGRTextureOptions Options;
		Options.Type = ZEGR_TT_2D;
		Texture = ZEGRTexture::LoadResourceShared(TextureFileName, Options);
	}

	if (!UpdateShaders())
		return ZE_TR_FAILED;

	if (!UpdateRenderState())
		return ZE_TR_FAILED;

	if (!UpdateConstantBuffer())
		return ZE_TR_FAILED;

	return ZE_TR_DONE;
}

ZETaskResult ZERNSimpleMaterial::UnloadInternal()
{
	DirtyFlags.RaiseAll();
	StageMask = 0;

	VertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
	ConstantBuffer.Release();
	Texture.Release();

	return ZE_TR_DONE;
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
	Sampler = ZEGRSampler::GetDefaultSampler();
	Constants.MaterialColor = ZEVector4::One;
	Constants.TextureEnabled = false;
	Constants.VertexColorEnabled = true;

	Register();
}

ZERNSimpleMaterial::~ZERNSimpleMaterial()
{
	Unregister();
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

void ZERNSimpleMaterial::SetTextureFileName(const ZEString& FileName)
{
	if (ZEPathInfo::Compare(TextureFileName, FileName))
		return;

	TextureFileName = FileName;

	if (TextureFileName.IsEmpty())
		Texture.Release();

	if (IsLoadedOrLoading() && !TextureFileName.IsEmpty())
	{
		ZEGRTextureOptions Options;
		Options.Type = ZEGR_TT_2D;

		UnregisterExternalResource(Texture);
		Texture = ZEGRTexture::LoadResourceShared(FileName, Options);
		RegisterExternalResource(Texture);
	}
}

const ZEString& ZERNSimpleMaterial::GetTextureFileName() const
{
	return TextureFileName;
}

void ZERNSimpleMaterial::SetTexture(const ZEGRTexture* Texture)
{
	UnregisterExternalResource(Texture);
	this->Texture = Texture;
	RegisterExternalResource(Texture);

	TextureFileName = Texture->GetFileName();
}

const ZEGRTexture* ZERNSimpleMaterial::GetTexture() const
{
	return Texture;
}

void ZERNSimpleMaterial::SetSampler(const ZEGRSampler* Sampler)
{
	this->Sampler = Sampler;
}

const ZEGRSampler* ZERNSimpleMaterial::GetSampler() const
{
	return Sampler;
}

bool ZERNSimpleMaterial::SetupMaterial(ZEGRContext* Context, const ZERNStage* Stage, bool Instanced) const
{
	if (!ZERNMaterial::SetupMaterial(Context, Stage))
		return false;

	if (!const_cast<ZERNSimpleMaterial*>(this)->Update())
		return false;

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_MATERIAL, ConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_MATERIAL, ConstantBuffer);

	Context->SetRenderState(RenderStateData);

	if (Constants.TextureEnabled)
	{
		Context->SetSampler(ZEGR_ST_PIXEL, 0, Sampler.GetPointer());
		Context->SetTexture(ZEGR_ST_PIXEL, 0, Texture.GetPointer());
	}

	return true;
}

void ZERNSimpleMaterial::CleanupMaterial(ZEGRContext* Context, const ZERNStage* Stage, bool Instanced) const
{
	ZERNMaterial::CleanupMaterial(Context, Stage);
}

bool ZERNSimpleMaterial::Update()
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

bool ZERNSimpleMaterial::PreRender(ZERNCommand& Command) const
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
