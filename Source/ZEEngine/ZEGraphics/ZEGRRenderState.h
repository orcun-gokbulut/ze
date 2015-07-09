//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRRenderState.h
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

#pragma once

#include "ZEGRVertexLayout.h"
#include "ZEGRDefinitions.h"
#include "ZEGRBlendState.h"
#include "ZEGRSamplerState.h"
#include "ZEGRRasterizerState.h"
#include "ZEGRDepthStencilState.h"
#include "ZEGRScissorRectangle.h"
#include "ZEGRViewPort.h"
#include "ZEGRHolder.h"

#include "ZETypes.h"
#include "ZECommon.h"

class ZEGRVertexLayout;
class ZEGRVertexBuffer;
class ZEGRIndexBuffer;
class ZEGRShader;
class ZEGRTexture;
class ZEGRRenderTarget;
class ZEGRConstantBuffer;
class ZEGRDepthStencilBuffer;
class ZEGRContext;

typedef ZEGRColorMask ColorBlendMask;

class ZEGRRenderStateData : public ZEGRResource
{
	public:
		virtual ZEGRResourceType				GetResourceType();
		virtual void							Setup(ZEGRContext* Device) = 0;
};

class ZEGRRenderState
{
	friend class ZEGRContext;

	private:
		ZEGRVertexLayout						VertexLayout;
		ZEGRHolder<ZEGRVertexBuffer>			VertexBuffers[ZEGR_MAX_VERTEX_BUFFER_SLOT];
		ZEGRHolder<ZEGRIndexBuffer>				IndexBuffer;

		ZEGRHolder<ZEGRShader>					Shaders[ZEGR_SHADER_TYPE_COUNT];
		ZEGRHolder<ZEGRConstantBuffer>			ShaderConstantBuffers[ZEGR_SHADER_TYPE_COUNT][ZEGR_MAX_CONSTANT_BUFFER_SLOT];
		ZEGRHolder<ZEGRTexture>					ShaderTextures[ZEGR_SHADER_TYPE_COUNT][ZEGR_MAX_TEXTURE_SLOT];
		ZEGRSamplerState						ShaderSamplers[ZEGR_SHADER_TYPE_COUNT][ZEGR_MAX_SAMPLER_SLOT];

		ZEGRRasterizerState						RasterizerState;
		ZEGRDepthStencilState					DepthStencilState;
		ZEGRBlendState							BlendState[ZEGR_MAX_RENDER_TARGET_SLOT];

		bool									RenderTargetsDirty;
		ZEGRHolder<ZEGRRenderTarget>			RenderTargets[ZEGR_MAX_RENDER_TARGET_SLOT];
		ZEGRHolder<ZEGRDepthStencilBuffer>		DepthStencilBuffer;

		ZEGRViewport							ViewPorts[ZEGR_MAX_VIEWPORT_SLOT];
		ZEGRScissorRectangle					ScissorRects[ZEGR_MAX_SCISSOR_SLOT];

	public:
		virtual ZEGRResourceType				GetResourceType();

		void									SetVertexLayout(const ZEGRVertexLayout& Layout);
		const ZEGRVertexLayout&					GetVertexLayout() const;

		void									SetVertexBuffer(ZEUInt Index, ZEGRVertexBuffer* Buffer);
		ZEGRVertexBuffer*						GetVertexBuffer(ZEUInt Index) const;

		void									SetIndexBuffer(ZEGRIndexBuffer* Buffer);
		ZEGRIndexBuffer*						GetIndexBuffer() const;

		void									SetShader(ZEGRShaderType Type, ZEGRShader* Shader);
		ZEGRShader*								GetShader(ZEGRShaderType Type) const;

		void									SetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index, ZEGRConstantBuffer* Buffer);
		ZEGRConstantBuffer*						GetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index) const;

		void									SetTexture(ZEGRShaderType Shader, ZEUInt Index, ZEGRTexture* Texture);
		ZEGRTexture*							GetTexture(ZEGRShaderType Shader, ZEUInt Index) const;

		void									SetRenderTarget(ZEUInt Index, ZEGRRenderTarget* Target);
		ZEGRRenderTarget*						GetRenderTarget(ZEUInt Index) const;

		void									SetDepthStencilBuffer(ZEGRDepthStencilBuffer* Buffer);
		ZEGRDepthStencilBuffer*					GetDepthStencilBuffer() const;

		void									SetRasterizerState(const ZEGRRasterizerState& State);
		const ZEGRRasterizerState&				GetRasterizerState() const;

		void									SetBlendState(ZEUInt Index, const ZEGRBlendState& State);
		const ZEGRBlendState&					GetBlendState(ZEUInt Index) const;

		void									SetDepthStencilState(const ZEGRDepthStencilState& State);
		const ZEGRDepthStencilState&			GetDepthStencilState() const;

		void									SetSampler(ZEGRShaderType Shader, ZEUInt Index, const ZEGRSamplerState& Sampler);
		const ZEGRSamplerState&					GetSampler(ZEGRShaderType Shader, ZEUInt Index) const;

		void									SetViewport(ZEUInt Index, const ZEGRViewport& ViewPort);
		const ZEGRViewport&						GetViewport(ZEUInt Index) const;


		void									SetToDefault();

		ZEGRRenderStateData*					Compile();

												ZEGRRenderState();
		virtual									~ZEGRRenderState();
};
