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
#include "ZERasterizerState.h"
#include "ZESamplerState.h"
#include "ZEShader.h"
#include "ZEStencilZState.h"
#include "ZEVertexDeclaration.h"
#include "ZERenderTarget.h"
#include "ZERenderCommand.h"
#include "ZEDepthBuffer.h"

#define ZE_MAX_SAMPLER_ATTACHMENT	16	// PLACEHOLDER
#define ZE_MAX_RENDER_TARGETS		4

class ZEGraphicsDevice
{
	// Device Tag in Variable names represents the current State of the GPU.
	// Requested Tag in the Variable names represents the state that needed by the application.
	protected:
		// Current RenderState
		ZEBlendState			RequestedBlendState;
		ZERasterizerState		RequestedRasterizerState;
		ZEStencilZState			RequestedStencilZState;

		ZEBlendState			DeviceBlendState;
		ZERasterizerState		DeviceRasterizerState;
		ZEStencilZState			DeviceStencilZState;

		// Shader Sampler States
		ZESamplerState			RequestedSamplerStates[ZE_MAX_SAMPLER_ATTACHMENT];
		ZESamplerState			DeviceSamplerStates[ZE_MAX_SAMPLER_ATTACHMENT];

		// Vertex Buffer
		ZEVertexBuffer*			RequestedVertexBuffer;
		ZEVertexBuffer*			DeviceVertexBuffer;

		// Index Buffer
		ZEIndexBuffer*			RequestedIndexBuffer;
		ZEIndexBuffer*			DeviceIndexBuffer;

		// Shader
		ZEShader*				RequestedVertexShader;
		ZEShader*				DeviceVertexShader;

		ZEShader*				RequestedPixelShader;
		ZEShader*				DevicePixelShader;

		// Frame buffer
		ZERenderTarget*			RequestedRenderTargets[ZE_MAX_RENDER_TARGETS];
		ZERenderTarget*			DeviceRenderTargets[ZE_MAX_RENDER_TARGETS];
		ZEDepthBuffer*			RequestedDepthBuffer;
		ZEDepthBuffer*			DeviceDepthBuffer;
		bool					ScreenWriteEnable;

		virtual void			InitDefaultState() = 0;

		virtual void			ApplyRequestedBlendState() = 0;
		virtual void			ApplyReqestedSamplerStates() = 0;
		virtual void			ApplyRequestedShaders() = 0;
		virtual void			ApplyRequestedStencilZState() = 0;
		virtual void			ApplyRequestedRasterizerState() = 0;
		virtual void			ApplyRequestedVertexBuffer() = 0;
		virtual void			ApplyRequestedIndexBuffer() = 0;
		virtual void			ApplyRequestedRenderTargets() = 0;

		virtual void			ApplyAllRequestedStates() = 0;

	public:
		// Render States (Can Arbitrary Change)
		void					SetBlendState(const ZEBlendState &NewBlendState);
		ZEBlendState&			GetBlendStateEdit();
		void					SetRasterizerState(const ZERasterizerState &NewRasterizerState);
		ZERasterizerState&		GetRasterizerStateEdit();
		void					SetSamplerState(int index, const ZESamplerState &NewSamplerState);
		ZESamplerState&			GetSamplerStateEdit(int index);
		void					SetStencilZState(ZEStencilZState NewStencilZState);
		ZEStencilZState&		GetStencilZStateEdit();
		
		// Per Render Command Changes
		void					SetVertexBuffer(ZEVertexBuffer* VertexBuffer);
		ZEVertexBuffer*			GetVertexBuffer() const;
		void					SetIndexBuffer(ZEIndexBuffer* IndexBuffer);
		ZEIndexBuffer*			GetIndexBuffer() const;

		// Shader Change
		void					SetVertexShader(ZEShader* VertexShader);
		ZEShader*				GetVertexShader() const;
		void					SetPixelShader(ZEShader* PixelShader);
		ZEShader*				GetPixelShader() const;
		
		// Output (Aka. RenderTarget Change)
		void					SetRenderTarget(int index, ZERenderTarget* RenderTarget);
		ZERenderTarget*			GetRenderTarget(int index) const;
		void					SetRenderTargetScreen(ZERenderTarget* RenderTarget);
		bool					GetScreenWriteEnable() const;

		// Commits the Requested State then Draws
		virtual void			Draw(ZEROPrimitiveType PrimitiveType, ZEUInt32 StartVertex, ZEUInt32 VertexCount) = 0;
		virtual void			DrawIndexed(ZEROPrimitiveType PrimitiveType, int BaseVertexIndex, ZEUInt32 MinIndex, 
											ZEUInt32 VertexCount, ZEUInt32 StartIndex, ZEUInt32 PrimitiveCount) = 0;
		
		// Commit State to GPU without Draw Call
		void					CommitRequestedState();

								ZEGraphicsDevice();
		virtual					~ZEGraphicsDevice();
};
#endif
