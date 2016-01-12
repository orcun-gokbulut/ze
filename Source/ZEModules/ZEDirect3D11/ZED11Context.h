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

class ZEGRRenderTarget;
class ZEGRDepthStencilBuffer;
class ZED11RenderStateData;

class ZED11Context : public ZEGRContext, public ZED11ComponentBase
{
	friend class ZED11Module;
	protected:
		ID3D11DeviceContext1*				Context;
		
		ZEFlags								DirtyFlags;
		ZEGRHolder<ZED11RenderStateData>	RenderState;
		ID3D11BlendState*					BlendState;
		ZEVector4							BlendFactors;
		ZEUInt								BlendMask;
		ID3D11DepthStencilState*			DepthStencilState;
		ZEUInt								StencilRef;

		void								UpdateContext();

		void								Initialize(ID3D11DeviceContext1* Device);
		void								Deinitialize();

											ZED11Context();
		virtual								~ZED11Context();

	public:
		ID3D11DeviceContext1*				GetContext();

		virtual void						SetRenderState(ZEGRRenderStateData* State);
		
		virtual void						SetVertexBuffers(ZEUInt Index, ZEUInt Count, ZEGRVertexBuffer** Buffers);
		virtual void						SetIndexBuffer(ZEGRIndexBuffer* Buffer);
		
		virtual void						SetStructuredBuffer(ZEGRShaderType Shader, ZEUInt Index, ZEGRStructuredBuffer* Buffer);
		virtual void						SetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index, ZEGRConstantBuffer* Buffer, ZEUInt StartOffset = 0, ZEUInt Size = 0);
		virtual void						GetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index, ZEGRConstantBuffer** Buffer);
		virtual void						SetSampler(ZEGRShaderType Shader, ZEUInt Index, const ZEGRSamplerState& Sampler);
		virtual void						SetTexture(ZEGRShaderType Shader, ZEUInt Index, ZEGRTexture* Texture);
		virtual void						GetTexture(ZEGRShaderType Shader, ZEUInt Index, ZEGRTexture** Texture);

		virtual void						SetRenderTargets(ZEUInt Count, ZEGRRenderTarget** RenderTargets, ZEGRDepthStencilBuffer* DepthStencilBuffer);
		virtual void						GetRenderTargets(ZEUInt Count, ZEGRRenderTarget** RenderTargets, ZEGRDepthStencilBuffer** DepthStencilBuffer);

		virtual void						SetViewports(ZEUInt Count, const ZEGRViewport* ViewPorts);
		virtual void						SetScissorRects(ZEUInt Count, const ZEGRScissorRect* Rects);

		virtual void						SetStencilRef(ZEUInt Reference);
		virtual void						SetBlendFactors(ZEVector4& Factors);
		virtual void						SetBlendMask(ZEUInt Mask);

		virtual void						SetUnorderedAccessView(ZEGRShaderType Shader, ZEUInt Index, ZEGRTexture* Texture);

		virtual void						Draw(ZEUInt VertexCount, ZEUInt VertexOffset);
		virtual void						DrawInstanced(ZEUInt VertexCount, ZEUInt VertexOffset, ZEUInt InstanceCount, ZEUInt InstanceOffset);

		virtual void						Dispatch(ZEUInt GroupCountX, ZEUInt GroupCountY, ZEUInt GroupCountZ);

		virtual void						ClearRenderTarget(ZEGRRenderTarget* RenderTarget, const ZEVector4& ClearColor);
		virtual void						ClearDepthStencilBuffer(ZEGRDepthStencilBuffer* DepthStencil, bool Depth, bool Stencil, float DepthValue, ZEUInt8 StencilValue);
		virtual void						ClearUnorderedAccessView(ZEGRTexture* Texture, const ZEVector4& ClearColor);
};
