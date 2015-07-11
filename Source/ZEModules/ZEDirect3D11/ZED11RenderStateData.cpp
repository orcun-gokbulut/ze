//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11RenderStateData.cpp
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

#include "ZED11RenderStateData.h"

#include "ZEGraphics\ZEGRRenderState.h"
#include "ZED11Context.h"
#include "ZED11VertexBuffer.h"
#include "ZED11IndexBuffer.h"
#include "ZED11ConstantBuffer.h"
#include "ZED11RenderTarget.h"
#include "ZED11Shader.h"
#include "ZED11StatePool.h"
#include "ZED11DepthStencilBuffer.h"
#include "ZED11Direct3D11Module.h"
#include "ZEGraphics/ZEGRTexture.h"

#include <d3d11.h>

inline DXGI_FORMAT ConverFormat(ZEGRIndexBufferFormat Format)
{
	switch(Format)
	{
		default:
		case ZEGR_IBF_NONE:
			return DXGI_FORMAT_UNKNOWN;

		case ZEGR_IBF_INDEX16:
			return DXGI_FORMAT_R16_UINT;

		case ZEGR_IBF_INDEX32:
			return DXGI_FORMAT_R32_UINT;
	}
}

void ZED11RenderStateData::Initialize(const ZEGRRenderState& RenderState)
{
	ZED11StatePool* StatePool = GetModule()->GetStatePool();

	VertexLayout = StatePool->GetState(RenderState.GetVertexLayout(), RenderState.GetShader(ZEGR_ST_VERTEX));

	VertexBuffersStart = ZEGR_MAX_VERTEX_BUFFER_SLOT;
	VertexBuffersCount = 0;
	for (ZESize I = 0; I < ZEGR_MAX_VERTEX_BUFFER_SLOT; I++)
	{
		VertexBuffers[I] = VertexBuffers[I].Cast<ZED11VertexBuffer>();
		if (VertexBuffers[I] == NULL)
		{
			VertexBuffersCount = I + 1;	
			if (I < VertexBuffersStart)
				VertexBuffersStart = I;	
		}
	}

	IndexBuffer = (ZED11IndexBuffer*)RenderState.GetIndexBuffer();

	for (ZEUInt I = 0; I < ZEGR_SHADER_TYPE_COUNT; I++)
	{
		ConstantBuffersStart[I] = ZEGR_MAX_CONSTANT_BUFFER_SLOT;
		ConstantBuffersCount[I] = 0;
		Shaders[I] = RenderState.GetShader((ZEGRShaderType)I);
		for (ZEUInt N = 0; N < ZEGR_MAX_CONSTANT_BUFFER_SLOT; I++)
		{
			ConstantBuffers[I][N] = (ZED11ConstantBuffer*)RenderState.GetConstantBuffer((ZEGRShaderType)I, N);
			if (ConstantBuffers[I][N] == NULL)
			{
				ConstantBuffersCount[I] = N + 1;	
				if (N < ConstantBuffersStart[I])
					ConstantBuffersStart[I] = N;	
			}
		}

		TexturesStart[I] = ZEGR_MAX_TEXTURE_SLOT;
		TexturesCount[I] = 0;
		for (ZEUInt N = 0; N < ZEGR_MAX_TEXTURE_SLOT; I++)
		{
			Textures[I][N] = RenderState.GetTexture((ZEGRShaderType)I, N);
			if (Textures[I][N] == NULL)
			{
				TexturesCount[I] = N + 1;	
				if (N < TexturesStart[I])
					TexturesStart[I] = N;	
			}
		}

		SamplersStart[I] = ZEGR_MAX_SAMPLER_SLOT;
		SamplersCount[I] = 0;
		for (ZEUInt N = 0; N < ZEGR_MAX_SAMPLER_SLOT; I++)
		{
			Samplers[I][N] = StatePool->GetState(RenderState.GetSampler((ZEGRShaderType)I, N));
			if (Samplers[I][N] == NULL)
			{
				SamplersCount[I] = N + 1;	
				if (N < SamplersStart[I])
					SamplersStart[I] = N;	
			}
		}
	}

	RasterizerState = StatePool->GetState(RenderState.GetRasterizerState());
	DepthStencilState = StatePool->GetState(RenderState.GetDepthStencilState());
	DepthStencilBuffer = (ZED11DepthStencilBuffer*)RenderState.GetDepthStencilBuffer();
	BlendState = StatePool->GetState(RenderState.GetBlendState(0));

	for (ZEUInt I = 0; I < ZEGR_MAX_RENDER_TARGET_SLOT; I++)
	{
		RenderTargets[I] = (ZED11RenderTarget*)RenderState.GetRenderTarget(I);
		if (RenderTargets[I] == NULL)
		{
			RenderTargetsCount = I + 1;	
			if (I < RenderTargetsStart)
				RenderTargetsStart = I;	
		}
	}

	ViewportCount = RenderState.GetViewportCount();
	for (ZEUInt I = 0; I < ViewportCount; I++)
	{
		const ZEGRViewport& Viewport  = RenderState.GetViewport(I);
		Viewports[I].TopLeftY = Viewport.GetY();
		Viewports[I].TopLeftX = Viewport.GetX();
		Viewports[I].Width = Viewport.GetWidth();
		Viewports[I].Height = Viewport.GetHeight();
		Viewports[I].MinDepth = Viewport.GetMinDepth();
		Viewports[I].MaxDepth = Viewport.GetMaxDepth();
	}


	ScissorRectsCount = RenderState.GetScissorRectCount();
	for (ZEUInt I = 0; I < ScissorRectsCount; I++)
	{
		const ZEGRScissorRect& ScissorRect  = RenderState.GetScissorRect(I);
		ScissorRects[I].left = ScissorRect.GetLeft();
		ScissorRects[I].right = ScissorRect.GetRight();
		ScissorRects[I].top = ScissorRect.GetTop();
		ScissorRects[I].bottom = ScissorRect.GetBottom();
	}
}

void ZED11RenderStateData::Setup(ZEGRContext* Context)
{
	ID3D11DeviceContext* NativeContext = ((ZED11Context*)Context)->GetContext();

	NativeContext->IASetInputLayout(VertexLayout->GetInterface());
	NativeContext->IASetVertexBuffers(VertexBuffersStart, VertexBuffersCount, NativeVertexBuffers, VertexBuffersStrides, VertexBuffersOffsets);
	NativeContext->IASetIndexBuffer(IndexBuffer->GetBuffer(), IndexBufferFormat, 0);
	NativeContext->IASetPrimitiveTopology(PrimitiveTopology);

	if (Shaders[ZEGR_ST_VERTEX] == NULL)
	{
		NativeContext->VSSetShader(NULL, NULL, 0);
	}
	else
	{
		NativeContext->VSSetShader(Shaders[ZEGR_ST_VERTEX].Cast<ZED11Shader>()->GetVertexShader(), NULL, 0);
		NativeContext->VSSetSamplers(SamplersStart[ZEGR_ST_VERTEX], SamplersCount[ZEGR_ST_VERTEX], NativeSamplers[ZEGR_ST_VERTEX]);
		NativeContext->VSSetConstantBuffers(ConstantBuffersStart[ZEGR_ST_VERTEX], ConstantBuffersCount[ZEGR_ST_VERTEX], NativeConstantBuffers[ZEGR_ST_VERTEX]);
		NativeContext->VSSetShaderResources(SamplersStart[ZEGR_ST_VERTEX], SamplersCount[ZEGR_ST_VERTEX], NativeTextures[ZEGR_ST_VERTEX]);
	}

	if (Shaders[ZEGR_ST_PIXEL] == NULL)
	{
		NativeContext->PSSetShader(NULL, NULL, 0);
	}
	else
	{
		NativeContext->PSSetShader(Shaders[ZEGR_ST_PIXEL].Cast<ZED11Shader>()->GetPixelShader(), NULL, 0);
		NativeContext->PSSetSamplers(SamplersStart[ZEGR_ST_PIXEL], SamplersCount[ZEGR_ST_PIXEL], NativeSamplers[ZEGR_ST_PIXEL]);
		NativeContext->PSSetConstantBuffers(ConstantBuffersStart[ZEGR_ST_PIXEL], ConstantBuffersCount[ZEGR_ST_PIXEL], NativeConstantBuffers[ZEGR_ST_PIXEL]);
		NativeContext->PSSetShaderResources(SamplersStart[ZEGR_ST_PIXEL], SamplersCount[ZEGR_ST_VERTEX], NativeTextures[ZEGR_ST_PIXEL]);
	}

	if (Shaders[ZEGR_ST_GEOMETRY] == NULL)
	{
		NativeContext->GSSetShader(NULL, NULL, 0);
	}
	else
	{
		NativeContext->GSSetShader(Shaders[ZEGR_ST_GEOMETRY].Cast<ZED11Shader>()->GetGeometryShader(), NULL, 0);
		NativeContext->GSSetSamplers(SamplersStart[ZEGR_ST_GEOMETRY], SamplersCount[ZEGR_ST_GEOMETRY], NativeSamplers[ZEGR_ST_GEOMETRY]);
		NativeContext->GSSetConstantBuffers(ConstantBuffersStart[ZEGR_ST_GEOMETRY], ConstantBuffersCount[ZEGR_ST_GEOMETRY], NativeConstantBuffers[ZEGR_ST_GEOMETRY]);
		NativeContext->GSSetShaderResources(SamplersStart[ZEGR_ST_GEOMETRY], SamplersCount[ZEGR_ST_GEOMETRY], NativeTextures[ZEGR_ST_GEOMETRY]);
	}

	if (Shaders[ZEGR_ST_DOMAIN] == NULL)
	{
		NativeContext->DSSetShader(NULL, NULL, 0);
	}
	else
	{
		NativeContext->DSSetShader(Shaders[ZEGR_ST_DOMAIN].Cast<ZED11Shader>()->GetDomainShader(), NULL, 0);
		NativeContext->DSSetSamplers(SamplersStart[ZEGR_ST_DOMAIN], SamplersCount[ZEGR_ST_DOMAIN], NativeSamplers[ZEGR_ST_DOMAIN]);
		NativeContext->DSSetConstantBuffers(ConstantBuffersStart[ZEGR_ST_DOMAIN], ConstantBuffersCount[ZEGR_ST_DOMAIN], NativeConstantBuffers[ZEGR_ST_DOMAIN]);
		NativeContext->DSSetShaderResources(SamplersStart[ZEGR_ST_DOMAIN], SamplersCount[ZEGR_ST_DOMAIN], NativeTextures[ZEGR_ST_DOMAIN]);
	}

	if (Shaders[ZEGR_ST_HULL] == NULL)
	{
		NativeContext->HSSetShader(NULL, NULL, 0);
	}
	else
	{
		NativeContext->HSSetShader(Shaders[ZEGR_ST_HULL].Cast<ZED11Shader>()->GetHullShader(), NULL, 0);
		NativeContext->HSSetSamplers(SamplersStart[ZEGR_ST_HULL], SamplersCount[ZEGR_ST_HULL], NativeSamplers[ZEGR_ST_HULL]);
		NativeContext->HSSetConstantBuffers(ConstantBuffersStart[ZEGR_ST_HULL], ConstantBuffersCount[ZEGR_ST_HULL], NativeConstantBuffers[ZEGR_ST_HULL]);
		NativeContext->HSSetShaderResources(SamplersStart[ZEGR_ST_HULL], SamplersCount[ZEGR_ST_HULL], NativeTextures[ZEGR_ST_HULL]);
	}

	if (Shaders[ZEGR_ST_COMPUTE] == NULL)
	{
		NativeContext->CSSetShader(NULL, NULL, 0);
	}
	else
	{
		NativeContext->CSSetShader(Shaders[ZEGR_ST_COMPUTE].Cast<ZED11Shader>()->GetComputeShader(), NULL, 0);
		NativeContext->CSSetSamplers(SamplersStart[ZEGR_ST_COMPUTE], SamplersCount[ZEGR_ST_VERTEX], NativeSamplers[ZEGR_ST_VERTEX]);
		NativeContext->CSSetConstantBuffers(ConstantBuffersStart[ZEGR_ST_COMPUTE], ConstantBuffersCount[ZEGR_ST_VERTEX], NativeConstantBuffers[ZEGR_ST_VERTEX]);
		NativeContext->CSSetShaderResources(SamplersStart[ZEGR_ST_COMPUTE], SamplersCount[ZEGR_ST_VERTEX], NativeTextures[ZEGR_ST_VERTEX]);
	}

	NativeContext->RSSetScissorRects(ScissorRectsCount, ScissorRects);
	NativeContext->RSSetViewports(ViewportCount, Viewports);
	NativeContext->RSSetState(RasterizerState->GetInterface());
	NativeContext->OMSetBlendState(BlendState->GetInterface(), (FLOAT*)&BlendFactors, BlendMask);
	NativeContext->OMSetDepthStencilState(DepthStencilState->GetInterface(), StencilRef);
	NativeContext->OMSetRenderTargets(RenderTargetsCount, NativeRenderTargets, DepthStencilBuffer->GetView());
}
