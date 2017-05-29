//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRContext.h
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

#include "ZEMeta/ZEObject.h"

#include "ZEGRRenderState.h"
#include "ZEGRViewport.h"
#include "ZEGRScissorRect.h"

class ZEGRResource;
class ZEGRBuffer;
class ZEGRTexture;
class ZEGRSampler;
class ZEGRRenderTarget;
class ZEGRDepthStencilBuffer;
class ZEGRTextureView;

class ZEGRContext : public ZEObject
{
	ZE_OBJECT
	protected:
		const ZEGRResource*				VertexBuffers[ZEGR_MAX_VERTEX_BUFFER_SLOT];
		const ZEGRResource*				IndexBuffer;
		const ZEGRResource*				ConstantBuffers[ZEGR_SHADER_TYPE_COUNT][ZEGR_MAX_CONSTANT_BUFFER_SLOT];
		const ZEGRSampler*				Samplers[ZEGR_SHADER_TYPE_COUNT][ZEGR_MAX_SAMPLER_SLOT];
		const ZEGRResource*				ShaderResources[ZEGR_SHADER_TYPE_COUNT][ZEGR_MAX_TEXTURE_SLOT];
		const ZEGRResource*				UnorderedAccesses[ZEGR_MAX_RWTEXTURE_SLOT];

		const ZEGRRenderTarget*			RenderTargets[ZEGR_MAX_RENDER_TARGET_SLOT];
		const ZEGRDepthStencilBuffer*	DepthStencilBuffer;

		ZEGRViewport					Viewports[ZEGR_MAX_VIEWPORT_SLOT];
		ZEGRScissorRect					ScissorRects[ZEGR_MAX_SCISSOR_SLOT];

		bool							CheckVertexBuffers(ZEUInt Index, ZEUInt Count, const ZEGRBuffer*const* Buffers);
		bool							CheckIndexBuffer(const ZEGRBuffer* Buffer);
		bool							CheckConstantBuffers(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRBuffer*const* Buffers, const ZEUInt* FirstConstant, const ZEUInt* NumConstants);
		bool							CheckSamplers(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRSampler*const* Samplers);
		bool							CheckShaderResources(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRResource*const* Resources);
		bool							CheckUnorderedAccesses(ZEUInt Index, ZEUInt Count, const ZEGRResource*const* Resources);
		bool							CheckRenderTargetsAndDepthStencilBuffer(ZEUInt Count, const ZEGRRenderTarget*const* RenderTargets, const ZEGRDepthStencilBuffer* DepthStencilBuffer);
		bool							CheckViewports(ZEUInt Count, const ZEGRViewport* Viewports);
		bool							CheckScissorRects(ZEUInt Count, const ZEGRScissorRect* ScissorRects);

										ZEGRContext();
		virtual							~ZEGRContext();

	public:
		virtual void					SetRenderState(const ZEGRRenderStateData* State) = 0;
		virtual void					SetComputeRenderState(const ZEGRComputeRenderStateData* State) = 0;

		virtual void					SetIndexBuffer(const ZEGRBuffer* Buffer, ZEUInt Offset = 0) = 0;

		void							SetVertexBuffer(ZEUInt Index, const ZEGRBuffer* Buffer, const ZEUInt Offset = 0);
		virtual void					SetVertexBuffers(ZEUInt Index, ZEUInt Count, const ZEGRBuffer*const* Buffers, const ZEUInt* Offsets = NULL) = 0;
		
		void							SetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index, const ZEGRBuffer* Buffer, const ZEUInt* FirstConstant = NULL, const ZEUInt* NumConstants = NULL);
		virtual void					SetConstantBuffers(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRBuffer*const* Buffers, const ZEUInt* FirstConstant = NULL, const ZEUInt* NumConstants = NULL) = 0;
		virtual void					GetConstantBuffers(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, ZEGRBuffer** Buffers);

		void							SetBuffer(ZEGRShaderType Shader, ZEUInt Index, const ZEGRBuffer* Buffer);
		virtual void					SetBuffers(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRBuffer*const* Buffers) = 0;

		virtual void					SetTextureView(ZEGRShaderType Shader, ZEUInt Index, const ZEGRTextureView* TextureView) = 0;
		virtual void					SetRWTextureView(ZEUInt Index, const ZEGRTextureView* RWTextureView) = 0;

		void							SetTexture(ZEGRShaderType Shader, ZEUInt Index, const ZEGRTexture* Texture);
		virtual void					SetTextures(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRTexture*const* Textures) = 0;
		virtual void					GetTextures(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, ZEGRTexture** Textures);

		void							SetSampler(ZEGRShaderType Shader, ZEUInt Index, const ZEGRSampler* Sampler);
		virtual void					SetSamplers(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRSampler*const* Samplers) = 0;
		
		virtual void					SetRenderTargets(ZEUInt Count, const ZEGRRenderTarget*const* RenderTargets, const ZEGRDepthStencilBuffer* DepthStencilBuffer) = 0;
		virtual void					GetRenderTargets(ZEUInt Count, ZEGRRenderTarget** RenderTargets);
		virtual void					GetDepthStencilBuffer(ZEGRDepthStencilBuffer** DepthStencilBuffer);

		virtual void					SetScissorRects(ZEUInt Count, const ZEGRScissorRect* Rects) = 0;
		virtual void					SetViewports(ZEUInt Count, const ZEGRViewport* ViewPorts) = 0;

		virtual void					SetStencilRef(ZEUInt Reference) = 0;
		virtual void					SetBlendFactors(const ZEVector4& Factors) = 0;
		virtual void					SetBlendMask(ZEUInt Mask) = 0;

		void							SetRWBuffer(ZEUInt Index, const ZEGRBuffer*Buffer);
		virtual void					SetRWBuffers(ZEUInt Index, ZEUInt Count, const ZEGRBuffer*const* Buffers) = 0;

		void							SetRWTexture(ZEUInt Index, const ZEGRTexture* Texture);
		virtual void					SetRWTextures(ZEUInt Index, ZEUInt Count, const ZEGRTexture*const* Textures) = 0;

		virtual	void					CopyResource(const ZEGRResource* DestResource, const ZEGRResource* SrcResource) = 0;
		virtual void					ResolveSubresource(const ZEGRTexture* DestResource, ZEUInt DestSubresource, const ZEGRTexture* SrcResource, ZEUInt SrcSubresource, ZEGRFormat Format) = 0;

		virtual void					Draw(ZEUInt VertexCount, ZEUInt VertexOffset) = 0;
		virtual void					DrawIndexed(ZEUInt IndexCount, ZEUInt IndexOffset, ZEUInt VertexOffset) = 0;
		virtual void					DrawInstanced(ZEUInt VertexCount, ZEUInt VertexOffset, ZEUInt InstanceCount, ZEUInt InstanceOffset) = 0;
		virtual void					DrawIndexedInstanced(ZEUInt IndexCount, ZEUInt IndexOffset, ZEUInt VertexOffset, ZEUInt InstanceCount, ZEUInt InstanceOffset) = 0;
		
		virtual void					Dispatch(ZEUInt GroupCountX, ZEUInt GroupCountY, ZEUInt GroupCountZ) = 0;

		virtual void					ClearRenderTarget(const ZEGRRenderTarget* RenderTarget, const ZEVector4& ClearColor) = 0;
		virtual void					ClearDepthStencilBuffer(const ZEGRDepthStencilBuffer* DepthStencil, bool Depth, bool Stencil, float DepthValue, ZEUInt8 StencilValue) = 0;
		virtual void					ClearRWView(const ZEGRTextureView* RWTextureView, const ZEVector4& ClearColor) = 0;

		virtual void					ClearShaderResources(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count);
		virtual void					ClearUnorderedAccesses(ZEUInt Index, ZEUInt Count);
		virtual void					ClearState();

		virtual void					BeginEvent(const ZEString& Name) = 0;
		virtual void					EndEvent() = 0;
		virtual void					SetMarker(const ZEString& Name) = 0;
};
