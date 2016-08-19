//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Context.h
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

#include "ZEGraphics/ZEGRContext.h"
#include "ZED11ComponentBase.h"

#include "ZEDS/ZEArray.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRScissorRect.h"

class ZED11RenderStateData;
class ZED11ComputeRenderStateData;
class ZEGRTexture;
struct ID3D11Buffer;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;

class ZED11Context : public ZEGRContext, public ZED11ComponentBase
{
	friend class ZED11Module;
	protected:
		ID3D11DeviceContext*							Context;

		ZEFlags											DirtyFlags;
		ZEVector4										BlendFactors;
		ZEUInt											BlendMask;
		ZEUInt											StencilRef;

		ZEHolder<ZED11RenderStateData>					GraphicsState;
		ZEHolder<ZED11ComputeRenderStateData>			ComputeState;

		void											Initialize(ID3D11DeviceContext* Device);
		void											Deinitialize();

		void											UpdateGraphicsState();

		ID3D11DeviceContext*							GetContext() const;

														ZED11Context();
		virtual											~ZED11Context();

	public:
		virtual void									SetRenderState(const ZEGRRenderStateData* State);
		virtual void									SetComputeRenderState(const ZEGRComputeRenderStateData* State);

		virtual void									SetVertexBuffers(ZEUInt Index, ZEUInt Count, const ZEGRVertexBuffer*const* Buffers);
		virtual void									SetIndexBuffer(const ZEGRIndexBuffer* Buffer);
		virtual void									SetConstantBuffers(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRConstantBuffer*const* Buffers);

		virtual void									SetStructuredBuffers(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRStructuredBuffer*const* Buffer);
		virtual void									SetTextures(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRTexture*const* Texture);

		virtual void									SetRWStructuredBuffers(ZEUInt Index, ZEUInt Count, const ZEGRStructuredBuffer*const* Buffers);
		virtual void									SetRWTextures(ZEUInt Index, ZEUInt Count, const ZEGRTexture*const* Textures);

		virtual void									SetSamplers(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRSampler*const* Samplers);

		virtual void									SetRenderTargets(ZEUInt Count, const ZEGRRenderTarget*const* RenderTargets, const ZEGRDepthStencilBuffer* DepthStencilBuffer);

		virtual void									SetScissorRects(ZEUInt Count, const ZEGRScissorRect* Rects);
		virtual void									SetViewports(ZEUInt Count, const ZEGRViewport* ViewPorts);

		virtual void									SetStencilRef(ZEUInt Reference);
		virtual void									SetBlendFactors(const ZEVector4& Factors);
		virtual void									SetBlendMask(ZEUInt Mask);

		virtual void									CopyResource(const ZEGRResource* DestResource, const ZEGRResource* SrcResource);
		virtual void									ResolveSubresource(const ZEGRResource* DestResource, ZEUInt DestSubresource, const ZEGRResource* SrcResource, ZEUInt SrcSubresource, ZEGRFormat Format);

		virtual void									Draw(ZEUInt VertexCount, ZEUInt VertexOffset);
		virtual void									DrawIndexed(ZEUInt IndexCount, ZEUInt IndexOffset, ZEUInt VertexOffset);
		virtual void									DrawInstanced(ZEUInt VertexCount, ZEUInt VertexOffset, ZEUInt InstanceCount, ZEUInt InstanceOffset);

		virtual void									Dispatch(ZEUInt GroupCountX, ZEUInt GroupCountY, ZEUInt GroupCountZ);

		virtual void									GenerateMipMaps(const ZEGRTexture* Texture);

		virtual void									ClearRenderTarget(const ZEGRRenderTarget* RenderTarget, const ZEVector4& ClearColor);
		virtual void									ClearDepthStencilBuffer(const ZEGRDepthStencilBuffer* DepthStencil, bool Depth, bool Stencil, float DepthValue, ZEUInt8 StencilValue);
		virtual void									ClearUnorderedAccessView(const ZEGRTexture* Texture, const ZEVector4& ClearColor);
};
