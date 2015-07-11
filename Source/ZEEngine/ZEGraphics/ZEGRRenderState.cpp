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

void ZEGRRenderState::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEGRRenderState::GetEnabled()
{
	return Enabled;
}

void ZEGRRenderState::SetVertexLayout(const ZEGRVertexLayout& Layout)
{
	VertexLayout = Layout;
}

const ZEGRVertexLayout& ZEGRRenderState::GetVertexLayout() const
{
	return VertexLayout;
}

void ZEGRRenderState::SetVertexBuffer(ZEUInt Index, ZEGRVertexBuffer* Buffer)
{
	zeCheckError(Index >= ZEGR_MAX_VERTEX_BUFFER_SLOT, ZE_VOID, "Vertex buffer index is too much.");
	VertexBuffers[Index] = Buffer;
}

ZEGRVertexBuffer* ZEGRRenderState::GetVertexBuffer(ZEUInt Index) const
{
	zeCheckError(Index >= ZEGR_MAX_VERTEX_BUFFER_SLOT, NULL, "Vertex buffer index is too much.");
	return VertexBuffers[Index];
}

void ZEGRRenderState::SetIndexBuffer(ZEGRIndexBuffer* Buffer)
{
	IndexBuffer = Buffer;
}

ZEGRIndexBuffer* ZEGRRenderState::GetIndexBuffer() const
{
	return IndexBuffer;
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

void ZEGRRenderState::SetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index, ZEGRConstantBuffer* Buffer)
{
	zeCheckError(Shader >= ZEGR_SHADER_TYPE_COUNT, ZE_VOID, "Unknown shader type.");
	zeCheckError(Index >= ZEGR_MAX_CONSTANT_BUFFER_SLOT, ZE_VOID, "Constant buffer index is too much.");

	if (Shader >= ZEGR_SHADER_TYPE_COUNT || Index >= ZEGR_MAX_CONSTANT_BUFFER_SLOT)
		return;

	ShaderConstantBuffers[Shader][Index] = Buffer;
}

ZEGRConstantBuffer* ZEGRRenderState::GetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index) const
{
	zeCheckError(Index >= ZEGR_MAX_CONSTANT_BUFFER_SLOT, NULL, "Constant buffer index is too much.");
	zeCheckError(Shader >= ZEGR_SHADER_TYPE_COUNT, NULL, "Unknown shader type.");

	return ShaderConstantBuffers[Shader][Index];
}

void ZEGRRenderState::SetTexture(ZEGRShaderType Shader, ZEUInt Index, ZEGRTexture* Texture)
{
	zeCheckError(Index >= ZEGR_MAX_TEXTURE_SLOT, ZE_VOID, "Texture index is too much.");
	zeCheckError(Shader >= ZEGR_SHADER_TYPE_COUNT, ZE_VOID, "Unknown shader type.");

	ShaderTextures[Shader][Index] = Texture;
}

ZEGRTexture* ZEGRRenderState::GetTexture(ZEGRShaderType Shader, ZEUInt Index) const
{
	zeCheckError(Index >= ZEGR_MAX_TEXTURE_SLOT, NULL, "Texture index is too much.");
	zeCheckError(Shader >= ZEGR_SHADER_TYPE_COUNT, NULL, "Unknown shader type.");

	return ShaderTextures[Shader][Index];
}

void ZEGRRenderState::SetSampler(ZEGRShaderType Shader, ZEUInt Index, const ZEGRSamplerState& Sampler)
{
	zeCheckError(Index >= ZEGR_MAX_SAMPLER_SLOT, ZE_VOID, "Sampler index is too much.");
	zeCheckError(Shader >= ZEGR_SHADER_TYPE_COUNT, ZE_VOID, "Unknown shader type.");

	if (Index >= ZEGR_MAX_VIEWPORT_SLOT || Shader >= ZEGR_SHADER_TYPE_COUNT)
		return;

	ShaderSamplers[Shader][Index] = Sampler;
}

const ZEGRSamplerState& ZEGRRenderState::GetSampler(ZEGRShaderType Shader, ZEUInt Index) const
{
	zeCheckError(Index >= ZEGR_MAX_SAMPLER_SLOT, ZEGRSamplerState(), "Sampler index is too much.");
	zeCheckError(Shader >= ZEGR_SHADER_TYPE_COUNT, ZEGRSamplerState(), "Unknown shader type.");

	return ShaderSamplers[Shader][Index];
}

void ZEGRRenderState::SetRenderTarget(ZEUInt Index, ZEGRRenderTarget* Target)
{
	zeCheckError(Index >= ZEGR_MAX_RENDER_TARGET_SLOT, ZE_VOID, "RenderTarget index is too much.");
	RenderTargets[Index] = Target;
}

ZEGRRenderTarget* ZEGRRenderState::GetRenderTarget(ZEUInt Index) const
{
	zeCheckError(Index >= ZEGR_MAX_RENDER_TARGET_SLOT, NULL, "RenderTarget index is too much.");
	return RenderTargets[Index];
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
	BlendState[Index] = State;
}

const ZEGRBlendState& ZEGRRenderState::GetBlendState(ZEUInt Index) const
{
	zeCheckError(Index >= ZEGR_MAX_RENDER_TARGET_SLOT, ZEGRBlendState(), "Blend state index is too much.");
	return BlendState[Index];
}

void ZEGRRenderState::SetDepthStencilBuffer(ZEGRDepthStencilBuffer* Buffer)
{
	DepthStencilBuffer = Buffer;
}

ZEGRDepthStencilBuffer* ZEGRRenderState::GetDepthStencilBuffer() const
{
	return DepthStencilBuffer;
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

void ZEGRRenderState::SetStencilRef(ZEUInt32 Reference)
{
	StencilRef = Reference;
}

ZEUInt32 ZEGRRenderState::GetStencilRef() const
{
	return StencilRef;
}

void ZEGRRenderState::SetBlendFactors(ZEVector4& Factors)
{
	BlendFactor = Factors;
}

const ZEVector4& ZEGRRenderState::GetBlendFactors() const
{
	return BlendFactor;
}

void ZEGRRenderState::SetBlendMask(ZEUInt32 Mask)
{
	BlendMask = Mask;
}

ZEUInt32 ZEGRRenderState::GetBlendMask() const
{
	return BlendMask;
}

void ZEGRRenderState::SetScissorRect(ZEUInt Index, const ZEGRScissorRect& Rect)
{
	zeCheckError(Index >= ZEGR_MAX_SCISSOR_SLOT, ZE_VOID, "Scissor rectangle index is too much.");
	ScissorRects[Index] = Rect;
}

const ZEGRScissorRect& ZEGRRenderState::GetScissorRect(ZEUInt Index) const
{
	zeCheckError(Index >= ZEGR_MAX_SCISSOR_SLOT, ZEGRScissorRect(), "Scissor rectangle index is too much.");
	return ScissorRects[Index];
}

void ZEGRRenderState::SetScissorRectCount(ZEUInt Count)
{
	zeCheckError(Count >= ZEGR_MAX_SCISSOR_SLOT, ZE_VOID, "Scissor rectangle index is too much.");
	ScissorRectCount = Count;
}

ZEUInt ZEGRRenderState::GetScissorRectCount() const
{
	return ScissorRectCount;
}

void ZEGRRenderState::SetViewport(ZEUInt Index, const ZEGRViewport& ViewPort)
{
	zeCheckError(Index >= ZEGR_MAX_VIEWPORT_SLOT, ZE_VOID, "Viewport index is too much.");
	if (Index >= ZEGR_MAX_VIEWPORT_SLOT)
		return;

	Viewports[Index] = ViewPort;
}

const ZEGRViewport& ZEGRRenderState::GetViewport(ZEUInt Index) const
{
	zeCheckError(Index >= ZEGR_MAX_VIEWPORT_SLOT, ZEGRViewport(), "Viewport index is too much.");
	return Viewports[Index];
}

void ZEGRRenderState::SetViewportCount(ZEUInt Count)
{
	zeCheckError(Count >= ZEGR_MAX_VIEWPORT_SLOT, ZE_VOID, "Viewport index is too much.");
	ViewportCount = Count;
}

ZEUInt ZEGRRenderState::GetViewportCount() const
{
	return ViewportCount;
}

void ZEGRRenderState::SetToDefault()
{
	Enabled = false;
	VertexLayout.SetToDefault();
	memset(VertexBuffers, NULL, sizeof(ZEGRVertexBuffer*) * ZEGR_MAX_VERTEX_BUFFER_SLOT);
	IndexBuffer = NULL;
	
	memset(Shaders, NULL, sizeof(ZEGRShader*) * ZEGR_SHADER_TYPE_COUNT);
	memset(ShaderConstantBuffers, NULL, sizeof(ZEGRConstantBuffer*) * ZEGR_SHADER_TYPE_COUNT * ZEGR_MAX_CONSTANT_BUFFER_SLOT);
	memset(ShaderTextures, NULL, sizeof(const ZEGRTexture*) * ZEGR_SHADER_TYPE_COUNT * ZEGR_MAX_TEXTURE_SLOT);
	memset(ShaderSamplers, 0, sizeof(ZESize) * ZEGR_SHADER_TYPE_COUNT * ZEGR_MAX_SAMPLER_SLOT);
	
	DepthStencilBuffer = NULL;
	memset(RenderTargets, NULL, sizeof(const ZEGRRenderTarget*) * ZEGR_MAX_RENDER_TARGET_SLOT);

	for (ZESize I = 0; I < ZEGR_MAX_RENDER_TARGET_SLOT; I++)
		BlendState[I].SetToDefault();

	for (ZESize I = 0; I < ZEGR_MAX_VIEWPORT_SLOT; I++)
		Viewports[I].SetZero();

	for (ZESize I = 0; I < ZEGR_MAX_SCISSOR_SLOT; I++)
		ScissorRects[I].SetZero();

	RasterizerState.SetToDefault();
	DepthStencilState.SetToDefault();

	PrimitiveType = ZEGR_PT_TRIANGLE_LIST;

	ScissorRectCount = 0;
	memset(ScissorRects, 0, sizeof(ScissorRects));

	ViewportCount = 0;
	memset(Viewports, 0, sizeof(Viewports));
}

ZEGRRenderState::ZEGRRenderState()
{
	SetToDefault();
}

ZEGRRenderState::~ZEGRRenderState()
{

}
