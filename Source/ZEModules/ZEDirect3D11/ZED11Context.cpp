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

#include <d3d11.h>
#include "ZED11StatePool.h"

inline D3D11_PRIMITIVE_TOPOLOGY ConvertPrimitiveType(ZEGRPrimitiveType PrimitiveType)
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

void ZED11Context::UpdateBlendState()
{
	if (!DirtyBlendState)
		return;

	Context->OMSetBlendState(RenderState->NativeBlendState, (float*)&BlendFactors, BlendMask);
}

void ZED11Context::UpdateDepthStencilState()
{
	if (!DirtyStencilState)
		return;

	Context->OMSetDepthStencilState(RenderState->NativeDepthStencilState, StencilRef);
}

void ZED11Context::Initialize(ID3D11DeviceContext* Context)
{
	this->Context = Context;
}

void ZED11Context::Deinitialize()
{
	ZEGR_RELEASE(Context);
}

void ZED11Context::UpdateRenderTargets()
{
	if (!DirtyRenderTargets)
		return;

	Context->OMSetRenderTargets(RenderTargetCount, RenderTargets, DepthStencilBuffer);
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

	Context->RSSetState(RenderState->RasterizerState->GetInterface());

	if (BlendState != RenderState->NativeBlendState)
		DirtyBlendState = true;

	if (DepthStencilState != RenderState->NativeDepthStencilState)
		DirtyBlendState = true;

	DirtyRenderTargets = true;
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
		Stride = (ZEUInt)Buffer->GetVertexSize();
		NativeBuffer = D11Buffer->GetBuffer();
	}
		
	Context->IASetVertexBuffers(Index, 1, &NativeBuffer, &Stride, &Offset);
}

void ZED11Context::SetIndexBuffer(ZEGRIndexBuffer* Buffer)
{
	ID3D11Buffer* NativeBuffer = NULL;
	ZED11IndexBuffer* D11Buffer = static_cast<ZED11IndexBuffer*>(Buffer);
	if (Buffer != NULL)
		NativeBuffer = D11Buffer->GetBuffer();

	Context->IASetIndexBuffer(NativeBuffer, ConverIndexBufferFormat(Buffer->GetFormat()), 0);
}

void ZED11Context::SetRenderTarget(ZEUInt Count, ZEGRRenderTarget** RenderTargets)
{
	zeDebugCheck(Count > ZEGR_MAX_RENDER_TARGET_SLOT, "RenderTarget count is too much.");
	
	if (Count > ZEGR_MAX_RENDER_TARGET_SLOT)
		Count = ZEGR_MAX_RENDER_TARGET_SLOT;

	RenderTargetCount = Count;
	for (ZESize I = 0; I < RenderTargetCount; I++)
		this->RenderTargets[I] = static_cast<ZED11RenderTarget*>(RenderTargets[I])->GetView();

	DirtyRenderTargets = true;
}

void ZED11Context::SetStencilRef(ZEUInt Reference)
{
	if (StencilRef == Reference)
		return;

	StencilRef = Reference;
	DirtyStencilState = true;
}

void ZED11Context::SetBlendFactors(ZEVector4& Factors)
{
	DirtyBlendState = true;
	BlendFactors = Factors;
}

void ZED11Context::SetBlendMask(ZEUInt Mask)
{
	if (BlendMask == Mask)
		return;

	BlendMask = Mask;
	DirtyBlendState = true;
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

void ZED11Context::SetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index, ZEGRConstantBuffer* Buffer)
{
	ID3D11Buffer* NativeConstants = NULL;
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
	ID3D11SamplerState* NativeSampler = GetModule()->GetStatePool()->GetState(Sampler)->GetInterface();

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

void ZED11Context::SetDepthStencilBuffer(ZEGRDepthStencilBuffer* Buffer)
{
	ID3D11DepthStencilView* NativeView = NULL;
	ZED11DepthStencilBuffer* D11Buffer = static_cast<ZED11DepthStencilBuffer*>(Buffer);
	if (D11Buffer == NULL)
		NativeView = D11Buffer->GetView();

	if (DepthStencilBuffer == NativeView)
		return;

	DepthStencilBuffer = NativeView;
	DirtyRenderTargets = true;
}

void ZED11Context::Draw(ZEUInt VertexCount, ZEUInt FirstVertex)
{
	UpdateRenderTargets();
	UpdateBlendState();
	UpdateDepthStencilState();

	GetMainContext()->Draw(VertexCount, FirstVertex);
}

void ZED11Context::DrawInstanced(ZEUInt VertexCount, ZEUInt FirstVertex, ZEUInt InstanceCount, ZEUInt FirstInstance)
{
	UpdateRenderTargets();
	UpdateBlendState();
	UpdateDepthStencilState();

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

	DirtyBlendState = true;
	BlendState = NULL;
	BlendFactors = ZEVector4::One;
	BlendMask = 0xFFFFFFFF;

	DirtyRenderTargets = true;
	RenderTargetCount = 0;
	memset(RenderTargets, 0, ZEGR_MAX_RENDER_TARGET_SLOT * sizeof(ID3D11RenderTargetView*));
	DepthStencilState = NULL;
	DepthStencilBuffer = NULL;

	DirtyStencilState = true;
	StencilRef = 0;

	RenderState = NULL;
}

ZED11Context::~ZED11Context()
{
	Deinitialize();
}
