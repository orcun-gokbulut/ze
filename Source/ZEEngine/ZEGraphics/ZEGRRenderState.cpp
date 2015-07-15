//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRRenderState.cpp
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

#include "ZEGRRenderState.h"
#include "ZEGRGraphicsModule.h"

ZEGRRenderState ZEGRRenderState::Default;

ZEGRResourceType ZEGRRenderStateData::GetResourceType()
{
	return ZEGR_RT_RENDER_STATE;
}

ZEGRRenderStateData* ZEGRRenderStateData::Create(const ZEGRRenderState& RenderState)
{
	ZEGRRenderStateData* RenderStateData = ZEGRGraphicsModule::GetInstance()->CreateRenderStateData();
	RenderStateData->Initialize(RenderState);
	return RenderStateData;
}

void ZEGRRenderState::SetVertexLayout(const ZEGRVertexLayout& Layout)
{
	VertexLayout = Layout;
}

const ZEGRVertexLayout& ZEGRRenderState::GetVertexLayout() const
{
	return VertexLayout;
}

void ZEGRRenderState::SetShader(ZEGRShaderType Type, ZEGRShader* Shader)
{
	zeCheckError(Type >= ZEGR_SHADER_TYPE_COUNT, ZE_VOID, "Unknown shader type.");
	Shaders[Type] = Shader;
}

ZEGRShader* ZEGRRenderState::GetShader(ZEGRShaderType Type) const
{
	zeCheckError(Type >= ZEGR_SHADER_TYPE_COUNT, NULL, "Unknown shader type.");
	return Shaders[Type];
}

void ZEGRRenderState::SetRenderTargetFormat(ZEUInt Index, ZEGRFormat Format)
{
	zeCheckError(Index >= ZEGR_MAX_RENDER_TARGET_SLOT, ZE_VOID, "RenderTarget index is too much.");
	RenderTargetFormats[Index] = Format;
}

ZEGRFormat ZEGRRenderState::GetRenderTargetFormat(ZEUInt Index) const
{
	zeCheckError(Index >= ZEGR_MAX_RENDER_TARGET_SLOT, ZEGR_TF_NONE, "RenderTarget index is too much.");
	return RenderTargetFormats[Index];
}

void ZEGRRenderState::SetRasterizerState(const ZEGRRasterizerState& State)
{
	RasterizerState = State;
}

const ZEGRRasterizerState& ZEGRRenderState::GetRasterizerState() const
{
	return RasterizerState;
}

void ZEGRRenderState::SetBlendState(ZEUInt Index, const ZEGRBlendState& State)
{
	zeCheckError(Index >= ZEGR_MAX_RENDER_TARGET_SLOT, ZE_VOID, "Blend state index is too much.");
	BlendStates[Index] = State;
}

const ZEGRBlendState& ZEGRRenderState::GetBlendState(ZEUInt Index) const
{
	zeCheckError(Index >= ZEGR_MAX_RENDER_TARGET_SLOT, ZEGRBlendState(), "Blend state index is too much.");
	return BlendStates[Index];
}

void ZEGRRenderState::SetDepthStencilFormat(ZEGRDepthStencilFormat Format)
{
	DepthStencilFormat = Format;
}

ZEGRDepthStencilFormat ZEGRRenderState::GetDepthStencilFormat() const
{
	return DepthStencilFormat;
}

void ZEGRRenderState::SetDepthStencilState(const ZEGRDepthStencilState& State)
{
	DepthStencilState = State;
}

const ZEGRDepthStencilState& ZEGRRenderState::GetDepthStencilState() const
{
	return DepthStencilState;
}

void ZEGRRenderState::SetPrimitiveType(ZEGRPrimitiveType Type)
{
	PrimitiveType = Type;
}

ZEGRPrimitiveType ZEGRRenderState::GetPrimitiveType() const
{
	return PrimitiveType;
}

void ZEGRRenderState::SetToDefault()
{
	VertexLayout.SetToDefault();
	memset(Shaders, NULL, sizeof(ZEGRShader*) * ZEGR_SHADER_TYPE_COUNT);
	DepthStencilFormat = ZEGR_DSF_NONE;
	memset(RenderTargetFormats, ZEGR_TF_NONE, sizeof(ZEGRFormat) * ZEGR_MAX_RENDER_TARGET_SLOT);

	for (ZESize I = 0; I < ZEGR_MAX_RENDER_TARGET_SLOT; I++)
		BlendStates[I].SetToDefault();
	RasterizerState.SetToDefault();
	DepthStencilState.SetToDefault();

	PrimitiveType = ZEGR_PT_TRIANGLE_LIST;

}


ZEGRRenderState& ZEGRRenderState::operator=(const ZEGRRenderState& RenderState)
{
	PrimitiveType = RenderState.PrimitiveType;
	VertexLayout = RenderState.VertexLayout;
	for (ZESize I = 0; I < ZEGR_SHADER_TYPE_COUNT; I++)
		Shaders[I] = RenderState.Shaders[I];

	RasterizerState = RenderState.RasterizerState;
	DepthStencilState = RenderState.DepthStencilState;
	DepthStencilFormat = RenderState.DepthStencilFormat;

	for (ZESize I = 0; I < ZEGR_MAX_RENDER_TARGET_SLOT; I++)
		RenderTargetFormats[I] = RenderState.RenderTargetFormats[I];

	for (ZESize I = 0; I < ZEGR_MAX_RENDER_TARGET_SLOT; I++)
		BlendStates[I] = RenderState.BlendStates[I];

	return *this;
}

ZEGRRenderState::ZEGRRenderState()
{
	SetToDefault();
}

ZEGRRenderState::ZEGRRenderState(const ZEGRRenderState& RenderState)
{
	operator=(RenderState);
}

ZEGRRenderState::~ZEGRRenderState()
{

}
