//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGraphicsDevice.h
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

#ifndef __ZE_GRAPHICS_DEVICE_H__ 
#define __ZE_GRAPHICS_DEVICE_H__

#include "ZEBlendState.h"
#include "ZESamplerState.h"
#include "ZEVertexLayout.h"
#include "ZEGraphicsModule.h"
#include "ZERasterizerState.h"
#include "ZEDepthStencilState.h"
#include "ZEGraphicsDeviceState.h"

struct ZEMSAAMode
{
	ZEUInt		SampleCount;
	ZEUInt		SampleQuality;
};

class ZEGraphicsDevice
{
	protected:
		ZEUInt						ActiveAF;
		ZEArray<ZEUInt>				AFModeList;

		ZEUInt						ActiveMSAA;
		ZEArray<ZEMSAAMode>			MSAAModeList;

		ZEGraphicsDeviceState		CurrentState;
		ZEGraphicsDeviceHashState	OldState;

									ZEGraphicsDevice();
		virtual						~ZEGraphicsDevice();

	public:
		const ZEArray<ZEUInt>&		GetModesAF() const;
		const ZEArray<ZEMSAAMode>&	GetModesMSAA() const;

		bool						SetSampleCountAF(ZEUInt SampleCount);
		ZEUInt						GetSampleCountAF() const;

		bool						SetSampleCountMSAA(ZEUInt SampleCount);
		ZEUInt						GetSampleCountMSAA() const;

		bool						CheckSupportAF(ZEUInt SampleCount);
		bool						CheckSupportMSAA(ZEUInt SampleCount);

		void						SetVerticalSync(bool Enabled);
		bool						GetVerticalSync() const;

		void						SetVertexLayout(ZEGRVertexLayout* Layout);
		ZEGRVertexLayout*				GetVertexLayout();
		
		void						SetVertexBufferArray(ZEVertexBuffer* const Buffer[ZEGR_MAX_VERTEX_BUFFER_SLOT]);
		void						SetVertexBuffer(ZEUInt Index, ZEVertexBuffer* Buffer);
		const ZEVertexBuffer*		GetVertexBuffer(ZEUInt Index) const;
		
		void						SetIndexBuffer(ZEGRIndexBuffer* Buffer);
		const ZEGRIndexBuffer*		GetIndexBuffer() const;
		
		void						SetVertexShader(ZEGRShader* Shader);
		const ZEGRShader*				GetVertexShader() const;
		
		void						SetVertexShaderBuffer(ZEUInt Index, ZEGRConstantBuffer* Buffer);
		const ZEGRConstantBuffer*		GetVertexShaderBuffer(ZEUInt Index) const;
		
		void						SetVertexShaderTexture(ZEUInt Index, const ZEGRTexture* Texture);
		const ZEGRTexture*			GetVertexShaderTexture(ZEUInt Index) const;
		
		void						SetVertexShaderSampler(ZEUInt Index, ZEGRSamplerState& Sampler);
		ZEGRSamplerState&				GetVertexShaderSampler(ZEUInt Index);
		
		void						SetGeometryShader(ZEGRShader* Shader);
		const ZEGRShader*				GetGeometryShader() const;
		
		void						SetGeometryShaderBuffer(ZEUInt Index, ZEGRConstantBuffer* Buffer);
		const ZEGRConstantBuffer*		GetGeometryShaderBuffer(ZEUInt Index) const;
		
		void						SetGeometryShaderTexture(ZEUInt Index, const ZEGRTexture* Texture);
		const ZEGRTexture*			GetGeometryShaderTexture(ZEUInt Index) const;
		
		void						SetGeometryShaderSampler(ZEUInt Index, ZEGRSamplerState& Sampler);
		ZEGRSamplerState&				GetGeometryShaderSampler(ZEUInt Index);
		
		void						SetRasterizerState(ZEGRRasterizerState& State);
		ZEGRRasterizerState&			GetRasterizerState();
		
		void						SetViewport(ZEUInt Index, ZEViewport& ViewPort);
		ZEViewport&					GetViewport(ZEUInt Index);
		
		void						SetScissorRectangle(ZEUInt Index, ZEGRScissorRectangle& Rectangle);
		ZEGRScissorRectangle&			GetScissorRectangle(ZEUInt Index);
		
		void						SetPixelShader(ZEGRShader* Shader);
		const ZEGRShader*				GetPixelShader() const;
		
		void						SetPixelShaderBuffer(ZEUInt Index, ZEGRConstantBuffer* Buffer);
		const ZEGRConstantBuffer*		GetPixelShaderBuffer(ZEUInt Index) const;
		
		void						SetPixelShaderTexture(ZEUInt Index, const ZEGRTexture* Texture);
		const ZEGRTexture*			GetPixelShaderTexture(ZEUInt Index) const;
		
		void						SetPixelShaderSampler(ZEUInt Index, ZEGRSamplerState& SamplerState);
		ZEGRSamplerState&				GetPixelShaderSampler(ZEUInt Index);
		
		void						SetBlendState(ZEGRBlendState& State, ZEVector4& ComponentBlendFactors, ZEGRColorMask ComponentBlendMask);
		
		void						SetBlendState(ZEGRBlendState& State);
		ZEGRBlendState&				GetBlendState();
		
		void						SetComponentBlendMask(ColorBlendMask ComponentBlendMask);
		ColorBlendMask&				GetComponentBlendMask();

		void						SetComponentBlendFactors(ZEVector4* ComponentBlendFactors);
		ZEVector4&					GetComponentBlendFactors();

		void						SetDepthStencilState(ZEGRDepthStencilState& State, ZEUInt32 StencilReferance);
		
		void						SetDepthStencilState(ZEGRDepthStencilState& State);
		ZEGRDepthStencilState&		GetDepthStencilState();
		
		void						SetStencilReferance(ZEUInt32 StencilReferance);
		ZEUInt32&					GetStencilReferance();
		
		void						SetRenderTargetScreen(const ZEGRRenderTarget* FrameBuffer);
		bool						GetScreenWriteEnable() const;
		
		void						SetRenderTargetArray(const ZEGRRenderTarget* const Targets[ZEGR_MAX_RENDER_TARGET_SLOT]);
		void						SetRenderTarget(ZEUInt Index, const ZEGRRenderTarget* Target);
		const ZEGRRenderTarget*		GetRenderTarget(ZEUInt Index) const;
		
		void						SetDepthStencilBuffer(const ZEGRDepthStencilBuffer* Buffer);
		const ZEGRDepthStencilBuffer*	GetDepthStencilBuffer() const;
		
		void						SetDeviceState(ZEGraphicsDeviceState& State);
		ZEGraphicsDeviceState&		GetDeviceState();
		
		virtual void				ResetStates();
		
		virtual void				Draw(ZEPrimitiveType PrimitiveType, ZEUInt VertexCount, ZEUInt FirstVertex) = 0;
		virtual void				DrawIndexed(ZEPrimitiveType PrimitiveType, ZEUInt IndexCount, ZEUInt FirstIndex, ZEInt BaseVertex) = 0;
		virtual void				DrawInstanced(ZEPrimitiveType PrimitiveType, ZEUInt VertexCount, ZEUInt FirstVertex, ZEUInt InstanceCount, ZEUInt FirstInstance) = 0;
		virtual void				DrawIndexedInstanced(ZEPrimitiveType PrimitiveType, ZEUInt IndexCount, ZEUInt InstanceCount, ZEUInt FirstIndex, ZEInt BaseVertex, ZEUInt FirstInstance) = 0;
		
		virtual void				ClearRenderTarget(const ZEGRRenderTarget* RenderTarget, const ZEVector4& ClearColor) = 0;
		virtual void				ClearDepthStencilBuffer(const ZEGRDepthStencilBuffer* DepthStencil, bool Depth, bool Stencil, float DepthValue, ZEUInt8 StencilValue) = 0;
};

#endif
