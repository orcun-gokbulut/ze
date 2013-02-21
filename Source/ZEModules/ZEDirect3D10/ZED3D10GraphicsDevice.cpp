//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D10GraphicsDevice.cpp
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

#include "ZED3D10Shader.h"
#include "ZED3D10Texture2D.h"
#include "ZED3D10Texture3D.h"
#include "ZED3D10TextureCube.h"
#include "ZED3D10IndexBuffer.h"
#include "ZED3D10RenderTarget.h"
#include "ZED3D10VertexBuffer.h"
#include "ZED3D10GraphicsDevice.h"
#include "ZED3D10GraphicsModule.h"
#include "ZED3D10ConstantBuffer.h"
#include "ZED3D10DepthStencilBuffer.h"

#include <D3D10.h>


inline DXGI_FORMAT ZEIndexBufferFormatToD3D10(ZEIndexBufferFormat BufferFormat)
{
	static const DXGI_FORMAT Values[] = 
	{
		DXGI_FORMAT_R16_UINT,
		DXGI_FORMAT_R32_UINT
	};
	return Values[BufferFormat];
}

inline D3D10_PRIMITIVE_TOPOLOGY ZEPrimitiveTypeToD3D10(ZEPrimitiveType PrimitiveType)
{
	static const D3D10_PRIMITIVE_TOPOLOGY Values[] = 
	{
		D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED,
		D3D10_PRIMITIVE_TOPOLOGY_POINTLIST,
		D3D10_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	};

	return Values[PrimitiveType];
}

void ZED3D10GraphicsDevice::ApplyStates()
{
	bool Commit = false;

	/************************************************************************/
	/*                         Output Stage                                 */
	/************************************************************************/
	{
		// Render targets
		// NOTE: Set outputs first to avoid binding the same resource both as rt and shader resource
		// ----------------------------------------------------------

		Commit = false;
		ID3D10DepthStencilView* D3D10DepthStencil = NULL;
		ID3D10RenderTargetView* D3D10Targets[ZE_MAX_RENDER_TARGET_SLOT] = {NULL};

		D3D10DepthStencil = CurrentState.DepthStencilBuffer == NULL ? NULL : ((const ZED3D10DepthStencilBuffer*)CurrentState.DepthStencilBuffer)->D3D10DepthStencilView;

		if (OldState.DepthStencilBuffer != CurrentState.DepthStencilBuffer)
		{
			OldState.DepthStencilBuffer = CurrentState.DepthStencilBuffer;
			Commit = true;
		}

		if (CurrentState.ScreenWriteEnable)
		{
			if (CurrentState.RenderTargets[0] != OldState.RenderTargets[0])
			{
				D3D10Targets[0] = CurrentState.RenderTargets[0] == NULL ? NULL : ((const ZED3D10RenderTarget*)CurrentState.RenderTargets[0])->D3D10RenderTargetView;
			
				OldState.RenderTargets[0] = CurrentState.RenderTargets[0];
				Commit = true;
			}
		
			if (Commit)
			{
				D3D10Device->OMSetRenderTargets(ZE_MAX_RENDER_TARGET_SLOT, D3D10Targets, D3D10DepthStencil);
			}
		
			OldState.ScreenWriteEnable = CurrentState.ScreenWriteEnable;
		}
		else
		{
			for (ZESize I = 0; I < ZE_MAX_RENDER_TARGET_SLOT; ++I)
			{
				// Use all render targets without comparing to old ones
				D3D10Targets[I] = CurrentState.RenderTargets[I] == NULL ? NULL : ((const ZED3D10RenderTarget*)CurrentState.RenderTargets[I])->D3D10RenderTargetView;

				if (OldState.RenderTargets[I] != CurrentState.RenderTargets[I])
					Commit = true;

				OldState.RenderTargets[I] = CurrentState.RenderTargets[I];
			}

			if (Commit)
			{
				D3D10Device->OMSetRenderTargets(ZE_MAX_RENDER_TARGET_SLOT, D3D10Targets, D3D10DepthStencil);
			}
		}
	
		// Blend State
		// ----------------------------------------------------------
		CurrentState.BlendState.UpdateHash();
		if (CurrentState.BlendState.Hash != OldState.BlendState.Hash || 
			CurrentState.ComponentBlendMask != OldState.ComponentBlendMask || 
			CurrentState.ComponentBlendFactors != OldState.ComponentBlendFactors)
		{
			//static const ZEVector4 BlendFactor = ZEVector4::One;

			ID3D10BlendState* D3D10State = ZED3D10StatePool::GetBlendState(&CurrentState.BlendState);
			D3D10Device->OMSetBlendState(D3D10State, CurrentState.ComponentBlendFactors.M, CurrentState.ComponentBlendMask.Value);

			OldState.BlendState = CurrentState.BlendState;
			CurrentState.ComponentBlendMask = OldState.ComponentBlendMask;
			CurrentState.ComponentBlendFactors = OldState.ComponentBlendFactors;
		}

		// Depth Stencil State
		// ----------------------------------------------------------
		CurrentState.DepthStencilState.UpdateHash();
		if (CurrentState.DepthStencilState.Hash != OldState.DepthStencilState.Hash || 
			CurrentState.StencilReferance != OldState.StencilReferance)
		{
			ID3D10DepthStencilState* D3D10State = ZED3D10StatePool::GetDepthStencilState(&CurrentState.DepthStencilState);
			D3D10Device->OMSetDepthStencilState(D3D10State, CurrentState.StencilReferance);

			OldState.DepthStencilState = CurrentState.DepthStencilState;
			CurrentState.StencilReferance = OldState.StencilReferance;
		}
	}
	
	/************************************************************************/
	/*                      Pixel Shader Stage                              */
	/************************************************************************/
	{
		// Shader
		// ----------------------------------------------------------
		if (CurrentState.PixelShader != OldState.PixelShader)
		{
			ID3D10PixelShader* D3D10Shader = (CurrentState.PixelShader == NULL) ? NULL : ((ZED3D10PixelShader*)CurrentState.PixelShader)->D3D10PixelShader;
			
			D3D10Device->PSSetShader(D3D10Shader);
			OldState.PixelShader = CurrentState.PixelShader;
		}

		// Buffers
		// ----------------------------------------------------------
		for (ZESize I = 0; I < ZE_MAX_BUFFER_SLOT; ++I)
		{
			if (CurrentState.PixelShaderBuffers[I] != NULL)
			{
				CurrentState.PixelShaderBuffers[I]->UpdateData();
			}

			if (CurrentState.PixelShaderBuffers[I] != OldState.PixelShaderBuffers[I])
			{
				ID3D10Buffer* D3D10Buffer = CurrentState.PixelShaderBuffers[I] == NULL ? NULL : ((ZED3D10ConstantBuffer*)CurrentState.PixelShaderBuffers[I])->D3D10Buffer;
			
				OldState.PixelShaderBuffers[I] = CurrentState.PixelShaderBuffers[I];
				D3D10Device->PSSetConstantBuffers((UINT)I, 1, &D3D10Buffer);
			}
		}

		// Samplers
		// ----------------------------------------------------------
		for (ZESize I = 0; I < ZE_MAX_SAMPLER_SLOT; ++I)
		{
			CurrentState.PixelShaderSamplers[I].UpdateHash();

			if (CurrentState.PixelShaderSamplers[I].Hash != OldState.PixelShaderSamplers[I].Hash)
			{
				ID3D10SamplerState* D3D10Sampler = ZED3D10StatePool::GetSamplerState(&CurrentState.PixelShaderSamplers[I]);

				OldState.PixelShaderSamplers[I] = CurrentState.PixelShaderSamplers[I];
				D3D10Device->PSSetSamplers((UINT)I, 1, &D3D10Sampler);
			}
		}
		
		// Textures
		// ----------------------------------------------------------
		for (ZESize I = 0; I < ZE_MAX_TEXTURE_SLOT; ++I)
		{
			ID3D10ShaderResourceView* D3D10Resource = NULL;
			const ZETexture* Texture = CurrentState.PixelShaderTextures[I];

			if ((Texture != OldState.GeometryShaderTextures[I]) || (Texture != NULL && Texture->RenderTarget))
			{
				if (Texture != NULL)
				{
					switch(Texture->GetTextureType())
					{
						case ZE_TT_2D:
							D3D10Resource = ((ZED3D10Texture2D*)Texture)->ShaderResourceView;
							break;
						case ZE_TT_3D:
							D3D10Resource = ((ZED3D10Texture3D*)Texture)->ShaderResourceView;
							break;
						case ZE_TT_CUBE:
							D3D10Resource = ((ZED3D10TextureCube*)Texture)->ShaderResourceView;
							break;
					}
				}

				OldState.PixelShaderTextures[I] = CurrentState.PixelShaderTextures[I];
				D3D10Device->PSSetShaderResources((UINT)I, 1, &D3D10Resource);
			}
		}
	}
	
	/************************************************************************/
	/*                     Rasterizer Stage                                 */
	/************************************************************************/
	{
		// Rasterizer state
		// ----------------------------------------------------------
		CurrentState.RasterizerState.UpdateHash();
		if (CurrentState.RasterizerState.Hash != OldState.RasterizerState.Hash)
		{
			ID3D10RasterizerState* D3D10State = ZED3D10StatePool::GetRasterizerState(&CurrentState.RasterizerState);
			D3D10Device->RSSetState(D3D10State);

			OldState.RasterizerState = CurrentState.RasterizerState;
		}

		// Viewports
		// ----------------------------------------------------------
		Commit = false;
		D3D10_VIEWPORT D3D10Viewports[ZE_MAX_VIEWPORT_SLOT] = {0};
		for (ZESize I = 0; I < ZE_MAX_VIEWPORT_SLOT; ++I)
		{
			CurrentState.ViewPorts[I].UpdateHash();
		
			if (CurrentState.ViewPorts[I].Hash != OldState.ViewPorts[I].Hash)
			{
				D3D10Viewports[I].Width		= CurrentState.ViewPorts[I].ViewportData.Width;
				D3D10Viewports[I].Height	= CurrentState.ViewPorts[I].ViewportData.Height;
				D3D10Viewports[I].MinDepth	= CurrentState.ViewPorts[I].ViewportData.MinDepth;
				D3D10Viewports[I].MaxDepth	= CurrentState.ViewPorts[I].ViewportData.MaxDepth;
				D3D10Viewports[I].TopLeftX	= CurrentState.ViewPorts[I].ViewportData.TopLeftX;
				D3D10Viewports[I].TopLeftY	= CurrentState.ViewPorts[I].ViewportData.TopLeftY;

				OldState.ViewPorts[I] = CurrentState.ViewPorts[I];
				Commit = true;
			}
		}
		if (Commit)
		{
			D3D10Device->RSSetViewports(ZE_MAX_VIEWPORT_SLOT, D3D10Viewports);
		}
	
		// Scissor Rectangles
		// ----------------------------------------------------------
		Commit = false;
		D3D10_RECT D3D10Rectangles[ZE_MAX_SCISSOR_SLOT] = {0};
		for (ZESize I = 0; I < ZE_MAX_SCISSOR_SLOT; ++I)
		{
			CurrentState.ScissorRectangles[I].UpdateHash();
		
			if (CurrentState.ScissorRectangles[I].Hash != OldState.ScissorRectangles[I].Hash)
			{
				D3D10Rectangles[I].top	 = CurrentState.ScissorRectangles[I].ScissorRectangleData.Top;
				D3D10Rectangles[I].left	 = CurrentState.ScissorRectangles[I].ScissorRectangleData.Left;
				D3D10Rectangles[I].right	 = CurrentState.ScissorRectangles[I].ScissorRectangleData.Right;
				D3D10Rectangles[I].bottom = CurrentState.ScissorRectangles[I].ScissorRectangleData.Bottom;
				
				OldState.ScissorRectangles[I] = CurrentState.ScissorRectangles[I];
				Commit = true;
			}
		}
		if (Commit)
		{
			D3D10Device->RSSetScissorRects(ZE_MAX_SCISSOR_SLOT, D3D10Rectangles);
		}
	}
	/************************************************************************/
	/*                       Geometry Shader                                */
	/************************************************************************/
	{
		// Shader
		// ----------------------------------------------------------
		if (CurrentState.GeometryShader != OldState.GeometryShader)
		{		
			ID3D10GeometryShader* D3D10Shader = (CurrentState.GeometryShader == NULL) ? NULL : ((ZED3D10GeometryShader*)CurrentState.GeometryShader)->D3D10GeometryShader;
			D3D10Device->GSSetShader(D3D10Shader);

			OldState.GeometryShader = CurrentState.GeometryShader;
		}

		// Buffers
		// ----------------------------------------------------------
		for (ZESize I = 0; I < ZE_MAX_BUFFER_SLOT; ++I)
		{
			if (CurrentState.GeometryShaderBuffers[I] != NULL)
			{
				CurrentState.GeometryShaderBuffers[I]->UpdateData();
			}
		
			if (CurrentState.GeometryShaderBuffers[I] != OldState.GeometryShaderBuffers[I])
			{
				ID3D10Buffer* D3D10Buffer = CurrentState.GeometryShaderBuffers[I] == NULL ? NULL : ((ZED3D10ConstantBuffer*)CurrentState.GeometryShaderBuffers[I])->D3D10Buffer;
			
				OldState.GeometryShaderBuffers[I] = CurrentState.GeometryShaderBuffers[I];
				D3D10Device->GSSetConstantBuffers((UINT)I, 1, &D3D10Buffer);
			}
		}

		// Samplers
		// ----------------------------------------------------------
		for (ZESize I = 0; I < ZE_MAX_SAMPLER_SLOT; ++I)
		{
			CurrentState.GeometryShaderSamplers[I].UpdateHash();

			if (CurrentState.GeometryShaderSamplers[I].Hash != OldState.GeometryShaderSamplers[I].Hash)
			{
				ID3D10SamplerState* D3D10Sampler = ZED3D10StatePool::GetSamplerState(&CurrentState.GeometryShaderSamplers[I]);
			
				OldState.GeometryShaderSamplers[I] = CurrentState.GeometryShaderSamplers[I];
				D3D10Device->GSSetSamplers((UINT)I, 1, &D3D10Sampler);
			}
		}

		// Textures
		// ----------------------------------------------------------
		for (ZESize I = 0; I < ZE_MAX_TEXTURE_SLOT; ++I)
		{
			ID3D10ShaderResourceView* D3D10Resource = NULL;
			const ZETexture* Texture = CurrentState.GeometryShaderTextures[I];

			if ((Texture != OldState.GeometryShaderTextures[I]) || (Texture != NULL && Texture->RenderTarget))
			{
				if (Texture != NULL)
				{
					switch(Texture->GetTextureType())
					{
						case ZE_TT_2D:
							D3D10Resource = ((ZED3D10Texture2D*)Texture)->ShaderResourceView;
							break;
						case ZE_TT_3D:
							D3D10Resource = ((ZED3D10Texture3D*)Texture)->ShaderResourceView;
							break;
						case ZE_TT_CUBE:
							D3D10Resource = ((ZED3D10TextureCube*)Texture)->ShaderResourceView;
							break;
					}
				}

				OldState.GeometryShaderTextures[I] = CurrentState.GeometryShaderTextures[I];
				D3D10Device->GSSetShaderResources((UINT)I, 1, &D3D10Resource);
			}
		}
	}

	/************************************************************************/
	/*                     Vertex Shader Stage                              */
	/************************************************************************/
	{
		// Shader
		// ----------------------------------------------------------
		if (CurrentState.VertexShader != OldState.VertexShader)
		{
			ID3D10VertexShader* D3D10Shader = (CurrentState.VertexShader == NULL) ? NULL : ((ZED3D10VertexShader*)CurrentState.VertexShader)->D3D10VertexShader;
			D3D10Device->VSSetShader(D3D10Shader);

			OldState.VertexShader = CurrentState.VertexShader;
		}

		// Buffers
		// ----------------------------------------------------------
		for (ZESize I = 0; I < ZE_MAX_BUFFER_SLOT; ++I)
		{
			ID3D10Buffer* D3D10Buffer = NULL;

			if (CurrentState.VertexShaderBuffers[I] != NULL)
			{
				CurrentState.VertexShaderBuffers[I]->UpdateData();
			}
		
			if (CurrentState.VertexShaderBuffers[I] != OldState.VertexShaderBuffers[I])
			{
				D3D10Buffer = CurrentState.VertexShaderBuffers[I] == NULL ? NULL : ((ZED3D10ConstantBuffer*)CurrentState.VertexShaderBuffers[I])->D3D10Buffer;
			
				OldState.VertexShaderBuffers[I] = CurrentState.VertexShaderBuffers[I];
				D3D10Device->VSSetConstantBuffers((UINT)I, 1, &D3D10Buffer);
			}
		}

		// Samplers
		// ----------------------------------------------------------
		for (ZESize I = 0; I < ZE_MAX_SAMPLER_SLOT; ++I)
		{
			ID3D10SamplerState* D3D10Sampler = NULL;

			CurrentState.VertexShaderSamplers[I].UpdateHash();

			if (CurrentState.VertexShaderSamplers[I].Hash != OldState.VertexShaderSamplers[I].Hash)
			{
				D3D10Sampler = ZED3D10StatePool::GetSamplerState(&CurrentState.VertexShaderSamplers[I]);
			
				OldState.VertexShaderSamplers[I] = CurrentState.VertexShaderSamplers[I];
				D3D10Device->VSSetSamplers((UINT)I, 1, &D3D10Sampler);
			}
		}

		// Textures
		// ----------------------------------------------------------
		for (ZESize I = 0; I < ZE_MAX_TEXTURE_SLOT; ++I)
		{
			ID3D10ShaderResourceView* D3D10Resource = NULL;
			const ZETexture* Texture = CurrentState.VertexShaderTextures[I];

			if ((Texture != OldState.VertexShaderTextures[I]) || (Texture != NULL && Texture->RenderTarget))
			{
				if (Texture != NULL)
				{
					switch(Texture->GetTextureType())
					{
						case ZE_TT_2D:
							D3D10Resource = ((ZED3D10Texture2D*)Texture)->ShaderResourceView;
							break;
						case ZE_TT_3D:
							D3D10Resource = ((ZED3D10Texture3D*)Texture)->ShaderResourceView;
							break;
						case ZE_TT_CUBE:
							D3D10Resource = ((ZED3D10TextureCube*)Texture)->ShaderResourceView;
							break;
					}
				}

				OldState.VertexShaderTextures[I] = CurrentState.VertexShaderTextures[I];
				D3D10Device->VSSetShaderResources((UINT)I, 1, &D3D10Resource);
			}
		}
	}
	/************************************************************************/
	/*                    Input Stage                                       */
	/************************************************************************/
	{
		// No need to set to old state
		ID3D10InputLayout* Layout = NULL;
		static ID3D10InputLayout* D3D10VertexLayout = NULL;

		CurrentState.VertexLayout.UpdateHash();
		if (CurrentState.VertexLayout.Hash != OldState.VertexLayout.Hash && !CurrentState.VertexLayout.IsEmpty())
		{
			// User defined vertex layout
			Layout = ZED3D10StatePool::GetVertexLayout(&CurrentState.VertexLayout, CurrentState.VertexShader);
		}
		else
		{
			// Automatic layout
			Layout = ZED3D10StatePool::GetVertexLayout(&CurrentState.VertexShader->DefaultVertexLayout, CurrentState.VertexShader);
		}
	
		// If layout is different
		if (Layout != D3D10VertexLayout)
		{
			D3D10VertexLayout = Layout;
			D3D10Device->IASetInputLayout(D3D10VertexLayout);
		}

		if (CurrentState.IndexBuffer != OldState.IndexBuffer)
		{	
			DXGI_FORMAT D3D10Format = DXGI_FORMAT_UNKNOWN;
			ID3D10Buffer* D3D10IndexBuffer = NULL;
		
			if (CurrentState.IndexBuffer != NULL)
			{
				D3D10IndexBuffer = ((const ZED3D10IndexBuffer*)CurrentState.IndexBuffer)->D3D10Buffer;
				D3D10Format = ZEIndexBufferFormatToD3D10(CurrentState.IndexBuffer->Format);
			}

			D3D10Device->IASetIndexBuffer(D3D10IndexBuffer, D3D10Format, 0);

			OldState.IndexBuffer = CurrentState.IndexBuffer;
		}
	
		for (ZESize I = 0; I < ZE_MAX_VERTEX_BUFFER_SLOT; ++I)
		{
			UINT D3D10VertexBufferOffset = 0;
			UINT D3D10VertexBufferStride = 0;
			ID3D10Buffer* D3D10VertexBuffer = NULL;

			if (CurrentState.VertexBuffers[I] != OldState.VertexBuffers[I])
			{
				const ZED3D10VertexBuffer* VertexBuffer = (ZED3D10VertexBuffer*)CurrentState.VertexBuffers[I];
				if (VertexBuffer != NULL)
				{
					D3D10VertexBuffer = VertexBuffer->D3D10Buffer;
					D3D10VertexBufferOffset = 0;
					D3D10VertexBufferStride = (UINT)VertexBuffer->VertexSize;
				}

				OldState.VertexBuffers[I] = CurrentState.VertexBuffers[I];
				D3D10Device->IASetVertexBuffers((UINT)I, 1, &D3D10VertexBuffer, &D3D10VertexBufferStride, &D3D10VertexBufferOffset);
			}
		}
	}
}

void ZED3D10GraphicsDevice::Draw(ZEPrimitiveType PrimitiveType, ZEUInt VertexCount, ZEUInt FirstVertex)
{
	ApplyStates();

	D3D10Device->IASetPrimitiveTopology(ZEPrimitiveTypeToD3D10(PrimitiveType));
	D3D10Device->Draw(VertexCount, FirstVertex);
}

void ZED3D10GraphicsDevice::DrawIndexed(ZEPrimitiveType PrimitiveType, ZEUInt IndexCount, ZEUInt FirstIndex, ZEInt BaseVertex)
{
	ApplyStates();
	D3D10Device->IASetPrimitiveTopology(ZEPrimitiveTypeToD3D10(PrimitiveType));
	D3D10Device->DrawIndexed(IndexCount, FirstIndex, BaseVertex);
}

void ZED3D10GraphicsDevice::DrawInstanced(ZEPrimitiveType PrimitiveType, ZEUInt VertexCount, ZEUInt FirstVertex, ZEUInt InstanceCount, ZEUInt FirstInstance)
{
	ApplyStates();
	D3D10Device->IASetPrimitiveTopology(ZEPrimitiveTypeToD3D10(PrimitiveType));
	D3D10Device->DrawInstanced(VertexCount, InstanceCount, FirstVertex, FirstInstance);
}

void ZED3D10GraphicsDevice::DrawIndexedInstanced(ZEPrimitiveType PrimitiveType, ZEUInt IndexCount, ZEUInt InstanceCount, ZEUInt FirstIndex, ZEInt BaseVertex, ZEUInt FirstInstance)
{
	ApplyStates();
	D3D10Device->IASetPrimitiveTopology(ZEPrimitiveTypeToD3D10(PrimitiveType));
	D3D10Device->DrawIndexedInstanced(IndexCount, InstanceCount, FirstIndex, BaseVertex, FirstInstance);
}

bool ZED3D10GraphicsDevice::Present() const
{
	UINT SynchInterval = zeGraphics->GetVerticalSync() ? 1 : 0;

	if (FAILED(GraphicsModule->GetDXGISwapChain()->Present(SynchInterval, 0)))
		return false;

	return false;
}

void ZED3D10GraphicsDevice::ClearRenderTarget(const ZERenderTarget* RenderTarget, const ZEVector4& ClearColor)
{
	D3D10Device->ClearRenderTargetView(((ZED3D10RenderTarget*)RenderTarget)->D3D10RenderTargetView, ClearColor.M);
}

void ZED3D10GraphicsDevice::ClearDepthStencilBuffer(const ZEDepthStencilBuffer* DepthStencil, bool Depth, bool Stencil, float DepthValue, ZEUInt8 StencilValue)
{
	UINT ClearFlag = 0;
	ClearFlag |= Depth ? D3D10_CLEAR_DEPTH : 0;
	ClearFlag |= Stencil ? D3D10_CLEAR_STENCIL : 0;

	D3D10Device->ClearDepthStencilView(((ZED3D10DepthStencilBuffer*)DepthStencil)->D3D10DepthStencilView, ClearFlag, DepthValue, StencilValue);
}

ZED3D10GraphicsDevice::ZED3D10GraphicsDevice()
{

}

ZED3D10GraphicsDevice::~ZED3D10GraphicsDevice()
{

}
