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
		ID3D11DeviceContext*		Context;
		
		bool						DirtyBlendState;
		ID3D11BlendState*			BlendState;
		ZEVector4					BlendFactors;
		ZEUInt						BlendMask;

		bool						DirtyRenderTargets;
		ZEUInt						RenderTargetCount;
		ID3D11RenderTargetView*		RenderTargets[ZEGR_MAX_RENDER_TARGET_SLOT];
		ID3D11DepthStencilState*	DepthStencilState;
		ID3D11DepthStencilView*		DepthStencilBuffer;

		bool						DirtyStencilState;
		ZEUInt						StencilRef;

		ZED11RenderStateData*		RenderState;

		void						UpdateBlendState();
		void						UpdateRenderTargets();
		void						UpdateDepthStencilState();

		void						Initialize(ID3D11DeviceContext* Device);
		void						Deinitialize();

									ZED11Context();
		virtual						~ZED11Context();

	public:
		ID3D11DeviceContext*		GetContext();

		virtual void				SetRenderState(ZEGRRenderStateData* State);
		
		virtual void				SetVertexBuffer(ZEUInt Index, ZEGRVertexBuffer* Buffer);
		virtual void				SetIndexBuffer(ZEGRIndexBuffer* Buffer);
		
		virtual void				SetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index, ZEGRConstantBuffer* Buffer);
		virtual void				SetSampler(ZEGRShaderType Shader, ZEUInt Index, const ZEGRSamplerState& Sampler);
		virtual void				SetTexture(ZEGRShaderType Shader, ZEUInt Index, ZEGRTexture* Texture);
	
		virtual void				SetRenderTarget(ZEUInt Count, ZEGRRenderTarget** RenderTarget);
		virtual void				SetDepthStencilBuffer(ZEGRDepthStencilBuffer* Buffer);

		virtual void				SetViewports(ZEUInt Count, const ZEGRViewport* ViewPorts);
		virtual void				SetScissorRects(ZEUInt Count, const ZEGRScissorRect* Rects);

		virtual void				SetStencilRef(ZEUInt Reference);
		virtual void				SetBlendFactors(ZEVector4& Factors);
		virtual void				SetBlendMask(ZEUInt Mask);

		virtual void				Draw(ZEUInt VertexCount, ZEUInt VertexOffset);
		virtual void				DrawInstanced(ZEUInt VertexCount, ZEUInt VertexOffset, ZEUInt InstanceCount, ZEUInt InstanceOffset);

		virtual void				ClearRenderTarget(ZEGRRenderTarget* RenderTarget, const ZEVector4& ClearColor);
		virtual void				ClearDepthStencilBuffer(ZEGRDepthStencilBuffer* DepthStencil, bool Depth, bool Stencil, float DepthValue, ZEUInt8 StencilValue);
};
