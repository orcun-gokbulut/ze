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

#include <d3d11.h>

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
#include "ZEGraphics/ZEGraphicsDefinitions.h"

inline DXGI_FORMAT ZEIndexBufferFormatToD3D10(ZEIndexBufferFormat BufferFormat)
{
	static const DXGI_FORMAT Values[] = 
	{
		DXGI_FORMAT_R16_UINT,
		DXGI_FORMAT_R32_UINT
	};
	return Values[BufferFormat];
}

inline D3D11_PRIMITIVE_TOPOLOGY ZEPrimitiveTypeToD3D10(ZEPrimitiveType PrimitiveType)
{
	static const D3D11_PRIMITIVE_TOPOLOGY Values[] = 
	{
		D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED,
		D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	};

	return Values[PrimitiveType];
}

void ZED3D10GraphicsDevice::ApplyInputStates()
{
	bool Commit = false;
	ZEStatePool* StatePool = zeGraphics->GetStatePool();

	static ZEVertexLayout StaticLayout;
		
	ZESize LayoutHash = 0;
	ZEVertexLayout*	LayoutPtr = NULL;
		
	if (CurrentState.VertexLayout == NULL)
	{
		// Auto create layout
		bool Result = ZEVertexLayout::GenerateLayout(StaticLayout, CurrentState.VertexBuffers, CurrentState.VertexShader);
		if (!Result)
		{
			zeWarning("Cannot assmeble vertex layout, check registered vertex buffer elements!");
		}

		LayoutPtr = &StaticLayout;
		LayoutHash = StaticLayout.GetHash();
	}
	else
	{
		// User defined layout
		LayoutPtr = CurrentState.VertexLayout;
		LayoutHash = CurrentState.VertexLayout->GetHash();
	}

	if (OldState.VertexLayoutHash != LayoutHash)
	{
		ID3D11InputLayout* D3D10Layout = (ID3D11InputLayout*)StatePool->GetState(LayoutPtr, CurrentState.VertexShader);
		D3DContexes[ContextIndex]->IASetInputLayout(D3D10Layout);

		OldState.VertexLayoutHash = LayoutHash;
	}
	
	if (CurrentState.IndexBuffer != NULL)
		((ZED3D10IndexBuffer*)CurrentState.IndexBuffer)->UpdateWith(0);

	if (CurrentState.IndexBuffer != OldState.IndexBuffer)
	{
		DXGI_FORMAT D3D10Format = DXGI_FORMAT_UNKNOWN;
		ID3D11Buffer* D3D10IndexBuffer = NULL;
		
		if (CurrentState.IndexBuffer != NULL)
		{
			D3D10IndexBuffer = ((const ZED3D10IndexBuffer*)CurrentState.IndexBuffer)->D3D10Buffer;
			D3D10Format = ZEIndexBufferFormatToD3D10(CurrentState.IndexBuffer->GetBufferFormat());
		}

		D3DContexes[ContextIndex]->IASetIndexBuffer(D3D10IndexBuffer, D3D10Format, 0);

		OldState.IndexBuffer = CurrentState.IndexBuffer;
	}
		
	for (ZESize I = 0; I < ZE_MAX_VERTEX_BUFFER_SLOT; ++I)
	{
		UINT D3D10VertexBufferOffset = 0;
		UINT D3D10VertexBufferStride = 0;
		ID3D11Buffer* D3D10VertexBuffer = NULL;

		if (CurrentState.VertexBuffers[I] != NULL)
			((ZED3D10VertexBuffer*)CurrentState.VertexBuffers[I])->UpdateWith(0);

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
			D3DContexes[ContextIndex]->IASetVertexBuffers((UINT)I, 1, &D3D10VertexBuffer, &D3D10VertexBufferStride, &D3D10VertexBufferOffset);
		}
	}
}

void ZED3D10GraphicsDevice::ApplyVertexShaderStates()
{
	bool Commit = false;
	ZEStatePool* StatePool = zeGraphics->GetStatePool();

	// Shader
	// ----------------------------------------------------------
	if (CurrentState.VertexShader != OldState.VertexShader)
	{
		ID3D11VertexShader* D3D10Shader = (CurrentState.VertexShader == NULL) ? NULL : ((ZED3D10VertexShader*)CurrentState.VertexShader)->D3D10VertexShader;
		D3DContexes[ContextIndex]->VSSetShader(D3D10Shader, NULL, 0);

		OldState.VertexShader = CurrentState.VertexShader;
	}

	// Buffers
	// ----------------------------------------------------------
	for (ZESize I = 0; I < ZE_MAX_CONSTANT_BUFFER_SLOT; ++I)
	{
		ID3D11Buffer* D3D10Buffer = NULL;
		
		if (CurrentState.VertexShaderBuffers[I] != NULL)
			((ZED3D10ConstantBuffer*)CurrentState.VertexShaderBuffers[I])->UpdateWith(0);
	
		if (CurrentState.VertexShaderBuffers[I] != OldState.VertexShaderBuffers[I])
		{
			D3D10Buffer = CurrentState.VertexShaderBuffers[I] == NULL ? NULL : ((ZED3D10ConstantBuffer*)CurrentState.VertexShaderBuffers[I])->D3D10Buffer;
			
			OldState.VertexShaderBuffers[I] = CurrentState.VertexShaderBuffers[I];
			D3DContexes[ContextIndex]->VSSetConstantBuffers((UINT)I, 1, &D3D10Buffer);
		}
	}

	// Samplers
	// ----------------------------------------------------------
	for (ZESize I = 0; I < ZE_MAX_SAMPLER_SLOT; ++I)
	{
		if (CurrentState.VertexShaderSamplers[I].GetHash() != OldState.VertexShaderSamplerHashes[I])
		{
			ID3D11SamplerState* D3D10Sampler = (ID3D11SamplerState*)StatePool->GetState(&CurrentState.VertexShaderSamplers[I]);
			
			OldState.VertexShaderSamplerHashes[I] = CurrentState.VertexShaderSamplers[I].GetHash();
			D3DContexes[ContextIndex]->VSSetSamplers((UINT)I, 1, &D3D10Sampler);
		}
	}

	// Textures
	// ----------------------------------------------------------
	for (ZESize I = 0; I < ZE_MAX_TEXTURE_SLOT; ++I)
	{
		ID3D11ShaderResourceView* D3D10Resource = NULL;
		const ZETexture* Texture = CurrentState.VertexShaderTextures[I];

		if ((Texture != OldState.VertexShaderTextures[I]) || (Texture != NULL && Texture->GetIsRenderTarget()))
		{
			if (Texture != NULL)
			{
				switch(Texture->GetTextureType())
				{
					case ZE_TT_2D:
						((ZED3D10Texture2D*)Texture)->UpdateWith(0);
						D3D10Resource = ((ZED3D10Texture2D*)Texture)->D3D10ShaderResourceView;
						break;
					case ZE_TT_3D:
						((ZED3D10Texture3D*)Texture)->UpdateWith(0);
						D3D10Resource = ((ZED3D10Texture3D*)Texture)->D3D10ShaderResourceView;
						break;
					case ZE_TT_CUBE:
						((ZED3D10TextureCube*)Texture)->UpdateWith(0);
						D3D10Resource = ((ZED3D10TextureCube*)Texture)->D3D10ShaderResourceView;
						break;
				}
			}

			OldState.VertexShaderTextures[I] = CurrentState.VertexShaderTextures[I];
			D3DContexes[ContextIndex]->VSSetShaderResources((UINT)I, 1, &D3D10Resource);
		}
	}
}

void ZED3D10GraphicsDevice::ApplyGeometryShaderStates()
{
	bool Commit = false;
	ZEStatePool* StatePool = zeGraphics->GetStatePool();

	// Shader
	// ----------------------------------------------------------
	if (CurrentState.GeometryShader != OldState.GeometryShader)
	{		
		ID3D11GeometryShader* D3D10Shader = (CurrentState.GeometryShader == NULL) ? NULL : ((ZED3D10GeometryShader*)CurrentState.GeometryShader)->D3D10GeometryShader;
		D3DContexes[ContextIndex]->GSSetShader(D3D10Shader, NULL, 0);

		OldState.GeometryShader = CurrentState.GeometryShader;
	}

	// Buffers
	// ----------------------------------------------------------
	for (ZESize I = 0; I < ZE_MAX_CONSTANT_BUFFER_SLOT; ++I)
	{		
		if (CurrentState.GeometryShaderBuffers[I] != NULL)
			((ZED3D10ConstantBuffer*)CurrentState.GeometryShaderBuffers[I])->UpdateWith(0);

		if (CurrentState.GeometryShaderBuffers[I] != OldState.GeometryShaderBuffers[I])
		{
			ID3D11Buffer* D3D10Buffer = CurrentState.GeometryShaderBuffers[I] == NULL ? NULL : ((ZED3D10ConstantBuffer*)CurrentState.GeometryShaderBuffers[I])->D3D10Buffer;
			
			OldState.GeometryShaderBuffers[I] = CurrentState.GeometryShaderBuffers[I];
			D3DContexes[ContextIndex]->GSSetConstantBuffers((UINT)I, 1, &D3D10Buffer);
		}
	}

	// Samplers
	// ----------------------------------------------------------
	for (ZESize I = 0; I < ZE_MAX_SAMPLER_SLOT; ++I)
	{
		if (CurrentState.GeometryShaderSamplers[I].GetHash() != OldState.GeometryShaderSamplerHashes[I])
		{
			ID3D11SamplerState* D3D10Sampler = (ID3D11SamplerState*)StatePool->GetState(&CurrentState.GeometryShaderSamplers[I]);
			
			OldState.GeometryShaderSamplerHashes[I] = CurrentState.GeometryShaderSamplers[I].GetHash();
			D3DContexes[ContextIndex]->GSSetSamplers((UINT)I, 1, &D3D10Sampler);
		}
	}

	// Textures
	// ----------------------------------------------------------
	for (ZESize I = 0; I < ZE_MAX_TEXTURE_SLOT; ++I)
	{
		ID3D11ShaderResourceView* D3D10Resource = NULL;
		const ZETexture* Texture = CurrentState.GeometryShaderTextures[I];

		if ((Texture != OldState.GeometryShaderTextures[I]) || (Texture != NULL && Texture->GetIsRenderTarget()))
		{
			if (Texture != NULL)
			{
				switch(Texture->GetTextureType())
				{
					case ZE_TT_2D:
						((ZED3D10Texture2D*)Texture)->UpdateWith(0);
						D3D10Resource = ((ZED3D10Texture2D*)Texture)->D3D10ShaderResourceView;
						break;
					case ZE_TT_3D:
						((ZED3D10Texture3D*)Texture)->UpdateWith(0);
						D3D10Resource = ((ZED3D10Texture3D*)Texture)->D3D10ShaderResourceView;
						break;
					case ZE_TT_CUBE:
						((ZED3D10TextureCube*)Texture)->UpdateWith(0);
						D3D10Resource = ((ZED3D10TextureCube*)Texture)->D3D10ShaderResourceView;
						break;
				}
			}

			OldState.GeometryShaderTextures[I] = CurrentState.GeometryShaderTextures[I];
			D3DContexes[ContextIndex]->GSSetShaderResources((UINT)I, 1, &D3D10Resource);
		}
	}
}

void ZED3D10GraphicsDevice::ApplyRasterizerStates()
{
	bool Commit = false;
	ZEStatePool* StatePool = zeGraphics->GetStatePool();

	// Rasterizer state
	// ----------------------------------------------------------
	if (CurrentState.RasterizerState.GetHash() != OldState.RasterizerStateHash)
	{
		ID3D11RasterizerState* D3D10State = (ID3D11RasterizerState*)StatePool->GetState(&CurrentState.RasterizerState);
		D3DContexes[ContextIndex]->RSSetState(D3D10State);

		OldState.RasterizerStateHash = CurrentState.RasterizerState.GetHash();
	}
	
	// Viewports
	// ----------------------------------------------------------
	Commit = false;
	D3D11_VIEWPORT D3D10Viewports[ZE_MAX_VIEWPORT_SLOT] = {0};
	for (ZESize I = 0; I < ZE_MAX_VIEWPORT_SLOT; ++I)
	{
		if (RenderTargetChanged || (CurrentState.ViewPorts[I].GetHash() != OldState.ViewPortHashes[I]))
		{
			D3D10Viewports[I].Width = CurrentState.ViewPorts[I].StateData.Width;
			D3D10Viewports[I].Height = CurrentState.ViewPorts[I].StateData.Height;
			D3D10Viewports[I].MinDepth = CurrentState.ViewPorts[I].StateData.MinDepth;
			D3D10Viewports[I].MaxDepth = CurrentState.ViewPorts[I].StateData.MaxDepth;
			D3D10Viewports[I].TopLeftX = CurrentState.ViewPorts[I].StateData.TopLeftX;
			D3D10Viewports[I].TopLeftY = CurrentState.ViewPorts[I].StateData.TopLeftY;

			OldState.ViewPortHashes[I] = CurrentState.ViewPorts[I].GetHash();
			Commit = true;
		}
	}
	if (Commit)
	{
		D3DContexes[ContextIndex]->RSSetViewports(ZE_MAX_VIEWPORT_SLOT, D3D10Viewports);
	}

	// Scissor Rectangles
	// ----------------------------------------------------------
	Commit = false;
	D3D11_RECT D3D10Rectangles[ZE_MAX_SCISSOR_SLOT] = {0};
	for (ZESize I = 0; I < ZE_MAX_SCISSOR_SLOT; ++I)
	{		
		if (RenderTargetChanged || (CurrentState.ScissorRects[I].GetHash() != OldState.ScissorRectHashes[I]))
		{
			D3D10Rectangles[I].top = CurrentState.ScissorRects[I].StateData.Top;
			D3D10Rectangles[I].left	= CurrentState.ScissorRects[I].StateData.Left;
			D3D10Rectangles[I].right = CurrentState.ScissorRects[I].StateData.Right;
			D3D10Rectangles[I].bottom = CurrentState.ScissorRects[I].StateData.Bottom;
				
			OldState.ScissorRectHashes[I] = CurrentState.ScissorRects[I].GetHash();
			Commit = true;
		}
	}
	if (Commit)
	{
		D3DContexes[ContextIndex]->RSSetScissorRects(ZE_MAX_SCISSOR_SLOT, D3D10Rectangles);
	}

	RenderTargetChanged = false;
}

void ZED3D10GraphicsDevice::ApplyPixelShaderStates()
{
	bool Commit = false;
	ZEStatePool* StatePool = zeGraphics->GetStatePool();

	// Shader
	// ----------------------------------------------------------
	if (CurrentState.PixelShader != OldState.PixelShader)
	{
		ID3D11PixelShader* D3D10Shader = (CurrentState.PixelShader == NULL) ? NULL : ((ZED3D10PixelShader*)CurrentState.PixelShader)->D3D10PixelShader;
			
		D3DContexes[ContextIndex]->PSSetShader(D3D10Shader, NULL, 0);
		OldState.PixelShader = CurrentState.PixelShader;
	}

	// Buffers
	// ----------------------------------------------------------
	for (ZESize I = 0; I < ZE_MAX_CONSTANT_BUFFER_SLOT; ++I)
	{
		if (CurrentState.PixelShaderBuffers[I] != NULL)
			((ZED3D10ConstantBuffer*)CurrentState.PixelShaderBuffers[I])->UpdateWith(0);

		if (CurrentState.PixelShaderBuffers[I] != OldState.PixelShaderBuffers[I])
		{
			ID3D11Buffer* D3D10Buffer = CurrentState.PixelShaderBuffers[I] == NULL ? NULL : ((ZED3D10ConstantBuffer*)CurrentState.PixelShaderBuffers[I])->D3D10Buffer;
			
			OldState.PixelShaderBuffers[I] = CurrentState.PixelShaderBuffers[I];
			D3DContexes[ContextIndex]->PSSetConstantBuffers((UINT)I, 1, &D3D10Buffer);
		}
	}

	// Samplers
	// ----------------------------------------------------------
	for (ZESize I = 0; I < ZE_MAX_SAMPLER_SLOT; ++I)
	{
		if (CurrentState.PixelShaderSamplers[I].GetHash() != OldState.PixelShaderSamplerHashes[I])
		{
			ID3D11SamplerState* D3D10Sampler = (ID3D11SamplerState*)StatePool->GetState(&CurrentState.PixelShaderSamplers[I]);

			OldState.PixelShaderSamplerHashes[I] = CurrentState.PixelShaderSamplers[I].GetHash();
			D3DContexes[ContextIndex]->PSSetSamplers((UINT)I, 1, &D3D10Sampler);
		}
	}
		
	// Textures
	// ----------------------------------------------------------
	for (ZESize I = 0; I < ZE_MAX_TEXTURE_SLOT; ++I)
	{
		ID3D11ShaderResourceView* D3D10Resource = NULL;
		const ZETexture* Texture = CurrentState.PixelShaderTextures[I];

		if ((Texture != OldState.GeometryShaderTextures[I]) || (Texture != NULL && Texture->GetIsRenderTarget()))
		{
			if (Texture != NULL)
			{
				switch(Texture->GetTextureType())
				{
					case ZE_TT_2D:
						((ZED3D10Texture2D*)Texture)->UpdateWith(0);
						D3D10Resource = ((ZED3D10Texture2D*)Texture)->D3D10ShaderResourceView;
						break;
					case ZE_TT_3D:
						((ZED3D10Texture3D*)Texture)->UpdateWith(0);
						D3D10Resource = ((ZED3D10Texture3D*)Texture)->D3D10ShaderResourceView;
						break;
					case ZE_TT_CUBE:
						((ZED3D10TextureCube*)Texture)->UpdateWith(0);
						D3D10Resource = ((ZED3D10TextureCube*)Texture)->D3D10ShaderResourceView;
						break;
				}
			}

			OldState.PixelShaderTextures[I] = CurrentState.PixelShaderTextures[I];
			D3DContexes[ContextIndex]->PSSetShaderResources((UINT)I, 1, &D3D10Resource);
		}
	}
}

void ZED3D10GraphicsDevice::ApplyOutputStates()
{
	bool Commit = false;
	ZEStatePool* StatePool = zeGraphics->GetStatePool();

	// Render targets
	// ----------------------------------------------------------
	ID3D11DepthStencilView* D3D10DepthStencil = NULL;
	ID3D11RenderTargetView* D3D10Targets[ZE_MAX_RENDER_TARGET_SLOT] = {NULL};

	D3D10DepthStencil = CurrentState.DepthStencilBuffer == NULL ? NULL : ((const ZED3D10DepthStencilBuffer*)CurrentState.DepthStencilBuffer)->D3D10DepthStencilView;

	if (OldState.DepthStencilBuffer != CurrentState.DepthStencilBuffer)
	{
		OldState.DepthStencilBuffer = CurrentState.DepthStencilBuffer;
		RenderTargetChanged = true;
		Commit = true;
	}

	if (CurrentState.ScreenWriteEnable)
	{
		if (CurrentState.RenderTargets[0] != OldState.RenderTargets[0])
		{
			D3D10Targets[0] = CurrentState.RenderTargets[0] == NULL ? NULL : ((const ZED3D10RenderTarget*)CurrentState.RenderTargets[0])->D3D10RenderTargetView;
			
			OldState.RenderTargets[0] = CurrentState.RenderTargets[0];
			RenderTargetChanged = true;
			Commit = true;
		}
		
		if (Commit)
		{
			D3DContexes[ContextIndex]->OMSetRenderTargets(ZE_MAX_RENDER_TARGET_SLOT, D3D10Targets, D3D10DepthStencil);
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
			{
				Commit = true;
				RenderTargetChanged = true;
			}

			OldState.RenderTargets[I] = CurrentState.RenderTargets[I];
		}

		if (Commit)
		{
			D3DContexes[ContextIndex]->OMSetRenderTargets(ZE_MAX_RENDER_TARGET_SLOT, D3D10Targets, D3D10DepthStencil);
		}
	}
	
	// Blend State
	// ----------------------------------------------------------
	if ( (CurrentState.BlendState.GetHash() != OldState.BlendStateHash) || 
		 (CurrentState.ComponentBlendMask != OldState.ComponentBlendMask) || 
		 (CurrentState.ComponentBlendFactors != OldState.ComponentBlendFactors))
	{
		ID3D11BlendState* D3D10State = (ID3D11BlendState*)StatePool->GetState(&CurrentState.BlendState);
		D3DContexes[ContextIndex]->OMSetBlendState(D3D10State, CurrentState.ComponentBlendFactors.M, 0xffffffff/*CurrentState.ComponentBlendMask.Value*/);

		OldState.BlendStateHash = CurrentState.BlendState.GetHash();
		OldState.ComponentBlendMask = CurrentState.ComponentBlendMask;
		OldState.ComponentBlendFactors = CurrentState.ComponentBlendFactors;
	}

	// Depth Stencil State
	// ----------------------------------------------------------
	if ( (CurrentState.DepthStencilState.GetHash() != OldState.DepthStencilStateHash) || 
		 (CurrentState.StencilReferance != OldState.StencilReferance))
	{
		ID3D11DepthStencilState* D3D10State = (ID3D11DepthStencilState*)StatePool->GetState(&CurrentState.DepthStencilState);
		D3DContexes[ContextIndex]->OMSetDepthStencilState(D3D10State, CurrentState.StencilReferance);

		OldState.DepthStencilStateHash = CurrentState.DepthStencilState.GetHash();
		OldState.StencilReferance = CurrentState.StencilReferance;
	}
}

void ZED3D10GraphicsDevice::ApplyStates()
{
	ApplyOutputStates();
	ApplyPixelShaderStates();
	ApplyRasterizerStates();
	ApplyGeometryShaderStates();
	ApplyVertexShaderStates();
	ApplyInputStates();
}

void ZED3D10GraphicsDevice::Draw(ZEPrimitiveType PrimitiveType, ZEUInt VertexCount, ZEUInt FirstVertex)
{
	ApplyStates();
	D3DContexes[ContextIndex]->IASetPrimitiveTopology(ZEPrimitiveTypeToD3D10(PrimitiveType));
	D3DContexes[ContextIndex]->Draw(VertexCount, FirstVertex);
}

void ZED3D10GraphicsDevice::DrawIndexed(ZEPrimitiveType PrimitiveType, ZEUInt IndexCount, ZEUInt FirstIndex, ZEInt BaseVertex)
{
	ApplyStates();
	D3DContexes[ContextIndex]->IASetPrimitiveTopology(ZEPrimitiveTypeToD3D10(PrimitiveType));
	D3DContexes[ContextIndex]->DrawIndexed(IndexCount, FirstIndex, BaseVertex);
}

void ZED3D10GraphicsDevice::DrawInstanced(ZEPrimitiveType PrimitiveType, ZEUInt VertexCount, ZEUInt FirstVertex, ZEUInt InstanceCount, ZEUInt FirstInstance)
{
	ApplyStates();
	D3DContexes[ContextIndex]->IASetPrimitiveTopology(ZEPrimitiveTypeToD3D10(PrimitiveType));
	D3DContexes[ContextIndex]->DrawInstanced(VertexCount, InstanceCount, FirstVertex, FirstInstance);
}

void ZED3D10GraphicsDevice::DrawIndexedInstanced(ZEPrimitiveType PrimitiveType, ZEUInt IndexCount, ZEUInt InstanceCount, ZEUInt FirstIndex, ZEInt BaseVertex, ZEUInt FirstInstance)
{
	ApplyStates();
	D3DContexes[ContextIndex]->IASetPrimitiveTopology(ZEPrimitiveTypeToD3D10(PrimitiveType));
	D3DContexes[ContextIndex]->DrawIndexedInstanced(IndexCount, InstanceCount, FirstIndex, BaseVertex, FirstInstance);
}

void ZED3D10GraphicsDevice::ClearRenderTarget(const ZERenderTarget* RenderTarget, const ZEVector4& ClearColor)
{
	D3DContexes[ContextIndex]->ClearRenderTargetView(((ZED3D10RenderTarget*)RenderTarget)->D3D10RenderTargetView, ClearColor.M);
}

void ZED3D10GraphicsDevice::ClearDepthStencilBuffer(const ZEDepthStencilBuffer* DepthStencil, bool Depth, bool Stencil, float DepthValue, ZEUInt8 StencilValue)
{
	UINT ClearFlag = 0;
	ClearFlag |= Depth ? D3D11_CLEAR_DEPTH : 0;
	ClearFlag |= Stencil ? D3D11_CLEAR_STENCIL : 0;

	D3DContexes[ContextIndex]->ClearDepthStencilView(((ZED3D10DepthStencilBuffer*)DepthStencil)->D3D10DepthStencilView, ClearFlag, DepthValue, StencilValue);
}

D3D_FEATURE_LEVEL ZED3D10GraphicsDevice::GetD3DFeatureLevel() const
{
	return FeatureLevel;
}

ZED3D10GraphicsDevice::ZED3D10GraphicsDevice(ID3D11Device* D3D10Device, ZEUInt ContextIndex) : ContextIndex(ContextIndex)
{
	RenderTargetChanged = false;

	ZEUInt CurrentSampleCount = 1;
	ZEUInt CurrentSampleQuality = 0;
	DXGI_FORMAT Format = ((ZED3D10GraphicsModule*)zeGraphics)->GetDXGIDisplayFormat();
	do
	{
		HRESULT Result = D3D10Device->CheckMultisampleQualityLevels(Format, CurrentSampleCount, &CurrentSampleQuality);
		if (FAILED(Result))
		{
			zeCriticalError("Cannot query sample count/quality support. Error: %d", Result);
			return;
		}

		if (CurrentSampleQuality != 0)
		{
			ZEMSAAMode CurrentMS;
			CurrentMS.SampleCount = CurrentSampleCount;
			CurrentMS.SampleQuality = CurrentSampleQuality - 1;

			MSAAModeList.AddByRef(CurrentMS);
		}

		CurrentSampleCount *= 2;

	} while(CurrentSampleCount <= ZE_MAX_MULTI_SAMPLE_COUNT);

	ZEUInt AnisotropyLevel = 1;
	do
	{
		AFModeList.Add(AnisotropyLevel);
		AnisotropyLevel *= 2;

	} while (AnisotropyLevel <= ZE_MAX_ANISOTROPY_LEVEL);

	FeatureLevel = D3D10Device->GetFeatureLevel();

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Multi sampling modes enumerated.");
#endif
}

ZED3D10GraphicsDevice::~ZED3D10GraphicsDevice()
{

}
