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

#include "ZEGRBlendState.h"
#include "ZEGRSamplerState.h"
#include "ZEGRVertexLayout.h"
#include "ZEGRGraphicsModule.h"
#include "ZEGRRasterizerState.h"
#include "ZEGRDepthStencilState.h"
#include "ZEGRRenderState.h"

class ZEGRVertexLayout;
class ZEGRVertexBuffer;
class ZEGRIndexBuffer;
class ZEGRShader;
class ZEGRConstantBuffer;
class ZEGRTexture;
class ZEGRSamplerState;

class ZEGRContext
{
	protected:
								ZEGRContext();
		virtual					~ZEGRContext();

	public:
		virtual void			SetRenderState(ZEGRRenderStateData* State) = 0;

		virtual void			SetVertexBuffer(ZEUInt Index, ZEGRVertexBuffer* Buffer) = 0;
		virtual void			SetIndexBuffer(ZEGRIndexBuffer* Buffer) = 0;

		virtual void			SetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index, ZEGRConstantBuffer* Buffer) = 0;
		virtual void			SetTexture(ZEGRShaderType Shader, ZEUInt Index, ZEGRTexture* Texture) = 0;
		virtual void			SetSampler(ZEGRShaderType Shader, ZEUInt Index, const ZEGRSamplerState& Sampler) = 0;

		virtual void			SetRenderTarget(ZEUInt Count, ZEGRRenderTarget** RenderTargets) = 0;
		virtual void			SetDepthStencilBuffer(ZEGRDepthStencilBuffer* Buffer) = 0;

		virtual void			SetViewports(ZEUInt Count, const ZEGRViewport* ViewPorts) = 0;
		virtual void			SetScissorRects(ZEUInt Count, const ZEGRScissorRect* Rects) = 0;

		virtual void			SetStencilRef(ZEUInt Reference) = 0;
		virtual void			SetBlendFactors(ZEVector4& Factors) = 0;
		virtual void			SetBlendMask(ZEUInt Mask) = 0;

		virtual  void			Draw(ZEUInt PrimitiveCount, ZEUInt VertexOffset) = 0;
		virtual void			DrawInstanced(ZEUInt PrimitiveCount, ZEUInt VertexOffset, ZEUInt InstanceCount, ZEUInt InstanceOffset) = 0;
		
		virtual void			ClearRenderTarget(ZEGRRenderTarget* RenderTarget, const ZEVector4& ClearColor) = 0;
		virtual void			ClearDepthStencilBuffer(ZEGRDepthStencilBuffer* DepthStencil, bool Depth, bool Stencil, float DepthValue, ZEUInt8 StencilValue) = 0;
};
