//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRDeviceState.h
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

#include "ZEGRDefinitions.h"
#include "ZEGRBlendState.h"
#include "ZEGRSamplerState.h"
#include "ZEGRRasterizerState.h"
#include "ZEGRDepthStencilState.h"
#include "ZEGRScissorRectangle.h"
#include "ZEGRViewPort.h"
#include "ZEGRHolder.h"

#include "ZETypes.h"

class ZEGRVertexLayout;
class ZEGRVertexBuffer;
class ZEGRIndexBuffer;
class ZEGRShader;
class ZEGRTexture;
class ZEGRRenderTarget;
class ZEGRConstantBuffer;
class ZEGRDepthStencilBuffer;

typedef ZEGRColorMask ColorBlendMask;

class ZEGRDeviceState
{
	friend class ZEGRDevice;

	private:
		ZEGRHolder<ZEGRVertexLayout>			VertexLayout;
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


												ZEGRDeviceState();
		virtual									~ZEGRDeviceState();

	public:
		void									SetVertexLayout(ZEGRVertexLayout* Layout);
		ZEGRVertexLayout*						GetVertexLayout();

		void									SetVertexBuffer(ZEUInt Index, ZEGRVertexBuffer* Buffer);
		const ZEGRVertexBuffer*					GetVertexBuffer(ZEUInt Index) const;

		void									SetIndexBuffer(ZEGRIndexBuffer* Buffer);
		const ZEGRIndexBuffer*					GetIndexBuffer() const;

		void									SetShader(ZEGRShaderType Type, ZEGRShader* Shader);
		const ZEGRShader*						GetVertexShader(ZEGRShaderType Type) const;

		void									SetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index, ZEGRConstantBuffer* Buffer);
		const ZEGRConstantBuffer*				GetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index) const;

		void									SetTexture(ZEGRShaderType Shader, ZEUInt Index, const ZEGRTexture* Texture);
		const ZEGRTexture*						GetTexture(ZEGRShaderType Shader, ZEUInt Index) const;

		void									SetSampler(ZEGRShaderType Shader, ZEUInt Index, ZEGRSamplerState& Sampler);
		ZEGRSamplerState&						GetSampler(ZEGRShaderType Shader, ZEUInt Index);

		void									SetViewport(ZEUInt Index, ZEGRViewport& ViewPort);
		ZEGRViewport&							GetViewport(ZEUInt Index);

		void									SetRenderTarget(ZEUInt Index, const ZEGRRenderTarget* Target);
		const ZEGRRenderTarget*					GetRenderTarget(ZEUInt Index) const;

		void									SetRasterizerState(ZEGRRasterizerState& State);
		ZEGRRasterizerState&					GetRasterizerState();

		void									SetBlendState(ZEUInt Index, ZEGRBlendState& State);
		ZEGRBlendState&							GetBlendState();

		void									SetDepthStencilBuffer(const ZEGRDepthStencilBuffer* Buffer);
		const ZEGRDepthStencilBuffer*			GetDepthStencilBuffer() const;

		void									SetDepthStencilState(ZEGRDepthStencilState& State);
		ZEGRDepthStencilState&					GetDepthStencilState();

		void									SetToDefault();

		virtual void							Update();

		static ZEGRDeviceState*					CreateInstance();
};
