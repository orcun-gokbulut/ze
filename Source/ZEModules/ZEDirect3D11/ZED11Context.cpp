//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Context.cpp
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

#include "ZED11Context.h"

#include "ZEGraphics/ZEGRDefinitions.h"
#include "ZED11Module.h"
#include "ZED11Shader.h"
#include "ZED11Texture2D.h"
#include "ZED11Texture3D.h"
#include "ZED11TextureCube.h"
#include "ZED11IndexBuffer.h"
#include "ZED11RenderTarget.h"
#include "ZED11VertexBuffer.h"
#include "ZED11ConstantBuffer.h"
#include "ZED11DepthStencilBuffer.h"
#include "ZED11RenderStateData.h"
#include "ZED11StructuredBuffer.h"

#include <d3d11_1.h>
#include "ZED11StatePool.h"

#define ZEGR_CONTEXT_DIRTY_BLEND_STATE		1
#define ZEGR_CONTEXT_DIRTY_STENCIL_STATE	2

inline DXGI_FORMAT ConverIndexBufferFormat(ZEGRIndexBufferFormat Format)
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

void ZED11Context::UpdateContext()
{
	if (RenderState == NULL)
		return;

	if (DirtyFlags.GetBit(ZEGR_CONTEXT_DIRTY_BLEND_STATE))
	{
		Context->OMSetBlendState(RenderState->NativeBlendState, (float*)&BlendFactors, BlendMask);
		DirtyFlags.UnraiseBit(ZEGR_CONTEXT_DIRTY_BLEND_STATE);
	}


	if (DirtyFlags.GetBit(ZEGR_CONTEXT_DIRTY_STENCIL_STATE))
	{
		Context->OMSetDepthStencilState(RenderState->NativeDepthStencilState, StencilRef);
		DirtyFlags.UnraiseBit(ZEGR_CONTEXT_DIRTY_STENCIL_STATE);
	}
}

void ZED11Context::Initialize(ID3D11DeviceContext1* Context)
{
	this->Context = Context;
}

void ZED11Context::Deinitialize()
{
	ZEGR_RELEASE(Context);
}

ID3D11DeviceContext1* ZED11Context::GetContext()
{
	return Context;
}

void ZED11Context::SetRenderState(ZEGRRenderStateData* State)
{
	if (RenderState == State)
		return;

	if (State == NULL)
		return;

	RenderState = static_cast<ZED11RenderStateData*>(State);

	Context->IASetInputLayout(RenderState->VertexLayout->GetInterface());
	Context->IASetPrimitiveTopology(RenderState->PrimitiveTopology);

	#define _SetupShader(Prefix, Type, Getter) \
		if (RenderState->Shaders[Type] == NULL) \
			Context->Prefix##SetShader(NULL, NULL, 0); \
		else \
			Context->Prefix##SetShader(RenderState->Shaders[Type].Cast<ZED11Shader>()->Getter(), NULL, 0);

	_SetupShader(VS, ZEGR_ST_VERTEX, GetVertexShader)
	_SetupShader(PS, ZEGR_ST_PIXEL, GetPixelShader)
	_SetupShader(GS, ZEGR_ST_GEOMETRY, GetGeometryShader)
	_SetupShader(DS, ZEGR_ST_DOMAIN, GetDomainShader)
	_SetupShader(HS, ZEGR_ST_HULL, GetHullShader)
	_SetupShader(CS, ZEGR_ST_COMPUTE, GetComputeShader)
	
	#undef _SetupShader
	Context->RSSetState(RenderState->NativeRasterizerState);

	if (BlendState != RenderState->NativeBlendState)
		DirtyFlags.RaiseBit(ZEGR_CONTEXT_DIRTY_BLEND_STATE);

	if (DepthStencilState != RenderState->NativeDepthStencilState)
		DirtyFlags.RaiseBit(ZEGR_CONTEXT_DIRTY_STENCIL_STATE);
}

void ZED11Context::SetVertexBuffer(ZEUInt Index, ZEGRVertexBuffer* Buffer)
{
	zeDebugCheck(Index >= ZEGR_MAX_VERTEX_BUFFER_SLOT, "Vertex buffer count is too much.");

	ZEUInt Stride = 0;
	ZEUInt Offset = 0;
	ID3D11Buffer* NativeBuffer = NULL;

	ZED11VertexBuffer* D11Buffer = static_cast<ZED11VertexBuffer*>(Buffer);
	if (D11Buffer != NULL)
	{
		Stride = (ZEUInt)D11Buffer->GetVertexSize();
		NativeBuffer = D11Buffer->GetBuffer();
	}
		
	Context->IASetVertexBuffers(Index, D11Buffer != NULL ? 1 : 0, &NativeBuffer, &Stride, &Offset);
}

void ZED11Context::SetIndexBuffer(ZEGRIndexBuffer* Buffer)
{
	ID3D11Buffer* NativeBuffer = NULL;
	ZED11IndexBuffer* D11Buffer = static_cast<ZED11IndexBuffer*>(Buffer);
	if (D11Buffer != NULL)
		NativeBuffer = D11Buffer->GetBuffer();

	Context->IASetIndexBuffer(NativeBuffer, ConverIndexBufferFormat(Buffer->GetFormat()), 0);
}

void ZED11Context::SetRenderTargets(ZEUInt Count, ZEGRRenderTarget** RenderTargets, ZEGRDepthStencilBuffer* DepthStencilBuffer)
{
	zeDebugCheck(Count >= ZEGR_MAX_RENDER_TARGET_SLOT, "RenderTarget index is too much.");
	
	if (Count >= ZEGR_MAX_RENDER_TARGET_SLOT)
		Count = ZEGR_MAX_RENDER_TARGET_SLOT;

	ID3D11RenderTargetView* NativeRenderTargets[ZEGR_MAX_RENDER_TARGET_SLOT];
	for (ZESize I = 0; I < Count; I++)
	{
		NativeRenderTargets[I] = NULL;
		if (RenderTargets[I] != NULL)
			NativeRenderTargets[I] = static_cast<ZED11RenderTarget*>(RenderTargets[I])->GetView();
	}

	ID3D11DepthStencilView* NativeDepthStencil = NULL;
	if (DepthStencilBuffer != NULL)
		NativeDepthStencil = static_cast<ZED11DepthStencilBuffer*>(DepthStencilBuffer)->GetView();

	Context->OMSetRenderTargets(Count, (Count != 0 ? NativeRenderTargets : NULL), NativeDepthStencil);
}

void ZED11Context::SetStencilRef(ZEUInt Reference)
{
	if (StencilRef == Reference)
		return;

	StencilRef = Reference;
	DirtyFlags.RaiseBit(ZEGR_CONTEXT_DIRTY_STENCIL_STATE);
}

void ZED11Context::SetBlendFactors(ZEVector4& Factors)
{
	if (BlendFactors == Factors)
		return;

	BlendFactors = Factors;
	DirtyFlags.RaiseBit(ZEGR_CONTEXT_DIRTY_BLEND_STATE);
}

void ZED11Context::SetBlendMask(ZEUInt Mask)
{
	if (BlendMask == Mask)
		return;

	BlendMask = Mask;
	DirtyFlags.RaiseBit(ZEGR_CONTEXT_DIRTY_BLEND_STATE);
}

void ZED11Context::SetScissorRects(ZEUInt Count, const ZEGRScissorRect* Rects)
{
	zeDebugCheck(Count > ZEGR_MAX_SCISSOR_SLOT, "Scisors Rect count is too much.");

	if (Count > ZEGR_MAX_SCISSOR_SLOT)
		Count = ZEGR_MAX_SCISSOR_SLOT;
	
	D3D11_RECT NativeRects[ZEGR_MAX_SCISSOR_SLOT];
	for (ZESize I = 0; I < Count; I++)
	{
		NativeRects[I].left = Rects[I].GetLeft();
		NativeRects[I].right = Rects[I].GetRight();
		NativeRects[I].top = Rects[I].GetTop();
		NativeRects[I].bottom = Rects[I].GetBottom();
	}
	
	Context->RSSetScissorRects(Count, NativeRects);
}

void ZED11Context::SetViewports(ZEUInt Count, const ZEGRViewport* Viewports)
{
	zeDebugCheck(Count > ZEGR_MAX_SCISSOR_SLOT, "Viewport count is too much.");

	if (Count > ZEGR_MAX_VIEWPORT_SLOT)
		Count = ZEGR_MAX_VIEWPORT_SLOT;

	D3D11_VIEWPORT NativeViewports[ZEGR_MAX_VIEWPORT_SLOT];
	for (ZESize I = 0; I < Count; I++)
	{
		NativeViewports[I].Width = Viewports[I].GetWidth();
		NativeViewports[I].Height = Viewports[I].GetHeight();
		NativeViewports[I].TopLeftX = Viewports[I].GetX();
		NativeViewports[I].TopLeftY = Viewports[I].GetY();
		NativeViewports[I].MinDepth = Viewports[I].GetMinDepth();
		NativeViewports[I].MaxDepth = Viewports[I].GetMaxDepth();
	}
	
	Context->RSSetViewports(Count, NativeViewports);
}

void ZED11Context::SetStructuredBuffer(ZEGRShaderType Shader, ZEUInt Index, ZEGRStructuredBuffer* Buffer)
{
	ID3D11ShaderResourceView* ResourceView = NULL;
	if(Buffer != NULL)
	{
		ResourceView = static_cast<ZED11StructuredBuffer*>(Buffer)->GetResourceView();
	}
	
	switch(Shader)
	{
		default:
		case ZEGR_ST_NONE:
			break;

		case ZEGR_ST_VERTEX:
			Context->VSSetShaderResources(Index, 1, &ResourceView);
			break;

		case ZEGR_ST_PIXEL:
			Context->PSSetShaderResources(Index, 1, &ResourceView);
			break;

		case ZEGR_ST_GEOMETRY:
			Context->GSSetShaderResources(Index, 1, &ResourceView);
			break;

		case ZEGR_ST_DOMAIN:
			Context->DSSetShaderResources(Index, 1, &ResourceView);
			break;

		case ZEGR_ST_HULL:
			Context->HSSetShaderResources(Index, 1, &ResourceView);
			break;

		case ZEGR_ST_COMPUTE:
			Context->CSSetShaderResources(Index, 1, &ResourceView);
			break;

		case ZEGR_ST_ALL:
			Context->VSSetShaderResources(Index, 1, &ResourceView);
			Context->PSSetShaderResources(Index, 1, &ResourceView);
			Context->GSSetShaderResources(Index, 1, &ResourceView);
			Context->DSSetShaderResources(Index, 1, &ResourceView);
			Context->HSSetShaderResources(Index, 1, &ResourceView);
			Context->CSSetShaderResources(Index, 1, &ResourceView);
			break;
	}
}

void ZED11Context::SetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index, ZEGRConstantBuffer* Buffer, ZEUInt StartOffset, ZEUInt Size)
{
	zeDebugCheck((StartOffset % 16) != 0, "StartOffset must be multiple of 16.");
	zeDebugCheck((Size % 16) != 0, "Size must be multiple of 16.");

	ID3D11Buffer* NativeConstants = NULL;
	UINT* FirstConstant = NULL;
	UINT* NumberOfConstant = NULL;
	if (Size != 0)
	{
		FirstConstant = &StartOffset;
		NumberOfConstant = &Size;
	}

	if (Buffer != NULL)
		 NativeConstants = ((ZED11ConstantBuffer*)Buffer)->GetBuffer();

	/*switch(Shader)
	{
		default:
		case ZEGR_ST_NONE:
			break;

		case ZEGR_ST_VERTEX:
			Context->VSSetConstantBuffers1(Index, 1, &NativeConstants, FirstConstant, NumberOfConstant);
			break;

		case ZEGR_ST_PIXEL:
			Context->PSSetConstantBuffers1(Index, 1, &NativeConstants, FirstConstant, NumberOfConstant);
			break;

		case ZEGR_ST_GEOMETRY:
			Context->GSSetConstantBuffers1(Index, 1, &NativeConstants, FirstConstant, NumberOfConstant);
			break;

		case ZEGR_ST_DOMAIN:
			Context->DSSetConstantBuffers1(Index, 1, &NativeConstants, FirstConstant, NumberOfConstant);
			break;

		case ZEGR_ST_HULL:
			Context->HSSetConstantBuffers1(Index, 1, &NativeConstants, FirstConstant, NumberOfConstant);
			break;

		case ZEGR_ST_COMPUTE:
			Context->CSSetConstantBuffers1(Index, 1, &NativeConstants, FirstConstant, NumberOfConstant);
			break;

		case ZEGR_ST_ALL:
			Context->VSSetConstantBuffers1(Index, 1, &NativeConstants, FirstConstant, NumberOfConstant);
			Context->PSSetConstantBuffers1(Index, 1, &NativeConstants, FirstConstant, NumberOfConstant);
			Context->GSSetConstantBuffers1(Index, 1, &NativeConstants, FirstConstant, NumberOfConstant);
			Context->DSSetConstantBuffers1(Index, 1, &NativeConstants, FirstConstant, NumberOfConstant);
			Context->HSSetConstantBuffers1(Index, 1, &NativeConstants, FirstConstant, NumberOfConstant);
			Context->CSSetConstantBuffers1(Index, 1, &NativeConstants, FirstConstant, NumberOfConstant);
			break;
	}*/

	switch(Shader)
	{
		default:
		case ZEGR_ST_NONE:
			break;

		case ZEGR_ST_VERTEX:
			Context->VSSetConstantBuffers(Index, 1, &NativeConstants);
			break;

		case ZEGR_ST_PIXEL:
			Context->PSSetConstantBuffers(Index, 1, &NativeConstants);
			break;

		case ZEGR_ST_GEOMETRY:
			Context->GSSetConstantBuffers(Index, 1, &NativeConstants);
			break;

		case ZEGR_ST_DOMAIN:
			Context->DSSetConstantBuffers(Index, 1, &NativeConstants);
			break;

		case ZEGR_ST_HULL:
			Context->HSSetConstantBuffers(Index, 1, &NativeConstants);
			break;

		case ZEGR_ST_COMPUTE:
			Context->CSSetConstantBuffers(Index, 1, &NativeConstants);
			break;

		case ZEGR_ST_ALL:
			Context->VSSetConstantBuffers(Index, 1, &NativeConstants);
			Context->PSSetConstantBuffers(Index, 1, &NativeConstants);
			Context->GSSetConstantBuffers(Index, 1, &NativeConstants);
			Context->DSSetConstantBuffers(Index, 1, &NativeConstants);
			Context->HSSetConstantBuffers(Index, 1, &NativeConstants);
			Context->CSSetConstantBuffers(Index, 1, &NativeConstants);
			break;
	}
}

void ZED11Context::SetTexture(ZEGRShaderType Shader, ZEUInt Index, ZEGRTexture* Texture)
{
	ID3D11ShaderResourceView* NativeTexture = NULL;
	if (Texture != NULL)
	{
		switch (Texture->GetTextureType())
		{
			case ZEGR_TT_2D:
				NativeTexture = static_cast<ZED11Texture2D*>(Texture)->GetResourceView();
				break;

			case ZEGR_TT_3D:
				NativeTexture = static_cast<ZED11Texture3D*>(Texture)->GetResourceView();
				break;

			case ZEGR_TT_CUBE:
				NativeTexture = static_cast<ZED11TextureCube*>(Texture)->GetResourceView();
				break;

			default:
				break;
		}
	}

	switch(Shader)
	{
		default:
		case ZEGR_ST_NONE:
			break;

		case ZEGR_ST_VERTEX:
			Context->VSSetShaderResources(Index, 1, &NativeTexture);
			break;

		case ZEGR_ST_PIXEL:
			Context->PSSetShaderResources(Index, 1, &NativeTexture);
			break;

		case ZEGR_ST_GEOMETRY:
			Context->GSSetShaderResources(Index, 1, &NativeTexture);
			break;

		case ZEGR_ST_DOMAIN:
			Context->DSSetShaderResources(Index, 1, &NativeTexture);
			break;

		case ZEGR_ST_HULL:
			Context->HSSetShaderResources(Index, 1, &NativeTexture);
			break;

		case ZEGR_ST_COMPUTE:
			Context->CSSetShaderResources(Index, 1, &NativeTexture);
			break;
	}
}

void ZED11Context::SetSampler(ZEGRShaderType Shader, ZEUInt Index, const ZEGRSamplerState& Sampler)
{
	ID3D11SamplerState* NativeSampler = GetModule()->GetStatePool()->GetSamplerState(Sampler)->GetInterface();

	switch(Shader)
	{
		default:
		case ZEGR_ST_NONE:
			break;

		case ZEGR_ST_VERTEX:
			Context->VSSetSamplers(Index, 1, &NativeSampler);
			break;

		case ZEGR_ST_PIXEL:
			Context->PSSetSamplers(Index, 1, &NativeSampler);
			break;

		case ZEGR_ST_GEOMETRY:
			Context->GSSetSamplers(Index, 1, &NativeSampler);
			break;

		case ZEGR_ST_DOMAIN:
			Context->DSSetSamplers(Index, 1, &NativeSampler);
			break;

		case ZEGR_ST_HULL:
			Context->HSSetSamplers(Index, 1, &NativeSampler);
			break;

		case ZEGR_ST_COMPUTE:
			Context->CSSetSamplers(Index, 1, &NativeSampler);
			break;
	}
}

void ZED11Context::Draw(ZEUInt VertexCount, ZEUInt FirstVertex)
{
	UpdateContext();
	GetMainContext()->Draw(VertexCount, FirstVertex);
}

void ZED11Context::DrawInstanced(ZEUInt VertexCount, ZEUInt FirstVertex, ZEUInt InstanceCount, ZEUInt FirstInstance)
{
	UpdateContext();
	GetMainContext()->DrawInstanced(VertexCount, InstanceCount, FirstVertex, FirstInstance);
}

void ZED11Context::ClearRenderTarget(ZEGRRenderTarget* RenderTarget, const ZEVector4& ClearColor)
{
	GetMainContext()->ClearRenderTargetView(((ZED11RenderTarget*)RenderTarget)->GetView(), ClearColor.M);
}

void ZED11Context::ClearDepthStencilBuffer(ZEGRDepthStencilBuffer* DepthStencil, bool Depth, bool Stencil, float DepthValue, ZEUInt8 StencilValue)
{
	UINT ClearFlag = 0;
	ClearFlag |= Depth ? D3D11_CLEAR_DEPTH : 0;
	ClearFlag |= Stencil ? D3D11_CLEAR_STENCIL : 0;

	GetMainContext()->ClearDepthStencilView(((ZED11DepthStencilBuffer*)DepthStencil)->GetView(), ClearFlag, DepthValue, StencilValue);
}

ZED11Context::ZED11Context()
{
	Context = NULL;

	DirtyFlags.UnraiseAll();

	BlendState = NULL;
	BlendFactors = ZEVector4::One;
	BlendMask = 0xFFFFFFFF;
	DepthStencilState = NULL;
	StencilRef = 0;

	RenderState = NULL;
}

ZED11Context::~ZED11Context()
{
	Deinitialize();
}
