//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRDevice.h
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
#include "ZEGRDeviceState.h"

class ZEGRVertexLayout;
class ZEGRVertexBuffer;
class ZEGRIndexBuffer;
class ZEGRShader;
class ZEGRConstantBuffer;
class ZEGRTexture;
class ZEGRSamplerState;

class ZEGRDevice
{
	protected:
										ZEGRDevice();
		virtual							~ZEGRDevice();

	public:
		void							SetVertexLayout(ZEGRVertexLayout* Layout);
		ZEGRVertexLayout*				GetVertexLayout();
		
		void							SetVertexBuffer(ZEUInt Index, ZEVertexBuffer* Buffer);
		const ZEVertexBuffer*			GetVertexBuffer(ZEUInt Index) const;
		
		void							SetIndexBuffer(ZEGRIndexBuffer* Buffer);
		const ZEGRIndexBuffer*			GetIndexBuffer() const;
		
		void							SetShader(ZEGRShaderType Type, ZEGRShader* Shader);
		const ZEGRShader*				GetVertexShader(ZEGRShaderType Type) const;
		
		void							SetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index, ZEGRConstantBuffer* Buffer);
		const ZEGRConstantBuffer*		GetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index) const;
		
		void							SetTexture(ZEGRShaderType Shader, ZEUInt Index, const ZEGRTexture* Texture);
		const ZEGRTexture*				GetTexture(ZEGRShaderType Shader, ZEUInt Index) const;
		
		void							SetSampler(ZEGRShaderType Shader, ZEUInt Index, ZEGRSamplerState& Sampler);
		ZEGRSamplerState&				GetSampler(ZEGRShaderType Shader, ZEUInt Index);
		
		void							SetRasterizerState(ZEGRRasterizerState& State);
		ZEGRRasterizerState&			GetRasterizerState();
		
		void							SetViewport(ZEUInt Index, ZEViewport& ViewPort);
		ZEViewport&						GetViewport(ZEUInt Index);
				
		void							SetBlendState(ZEGRBlendState& State);
		ZEGRBlendState&					GetBlendState();
		
		void							SetDepthStencilState(ZEGRDepthStencilState& State);
		ZEGRDepthStencilState&			GetDepthStencilState();
		
		void							SetRenderTarget(ZEUInt Index, const ZEGRRenderTarget* Target);
		const ZEGRRenderTarget*			GetRenderTarget(ZEUInt Index) const;
		
		void							SetDepthStencilBuffer(const ZEGRDepthStencilBuffer* Buffer);
		const ZEGRDepthStencilBuffer*	GetDepthStencilBuffer() const;
		
		virtual void					Draw(ZEGRPrimitiveType PrimitiveType, ZEUInt VertexCount, ZEUInt FirstVertex) = 0;
		virtual void					DrawIndexed(ZEGRPrimitiveType PrimitiveType, ZEUInt IndexCount, ZEUInt FirstIndex, ZEInt BaseVertex) = 0;
		virtual void					DrawInstanced(ZEGRPrimitiveType PrimitiveType, ZEUInt VertexCount, ZEUInt FirstVertex, ZEUInt InstanceCount, ZEUInt FirstInstance) = 0;
		virtual void					DrawIndexedInstanced(ZEGRPrimitiveType PrimitiveType, ZEUInt IndexCount, ZEUInt InstanceCount, ZEUInt FirstIndex, ZEInt BaseVertex, ZEUInt FirstInstance) = 0;
		
		virtual void					ClearRenderTarget(const ZEGRRenderTarget* RenderTarget, const ZEVector4& ClearColor) = 0;
		virtual void					ClearDepthStencilBuffer(const ZEGRDepthStencilBuffer* DepthStencil, bool Depth, bool Stencil, float DepthValue, ZEUInt8 StencilValue) = 0;
};
