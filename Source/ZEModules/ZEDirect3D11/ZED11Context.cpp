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
#include "ZED11StatePool.h"
#include "ZED11Sampler.h"

#include "ZEGraphics/ZEGRDefinitions.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRScissorRect.h"

#include <d3d11_1.h>

#define ZEGR_CONTEXT_DIRTY_BLEND_STATE		1
#define ZEGR_CONTEXT_DIRTY_STENCIL_STATE	2

static DXGI_FORMAT ConverIndexBufferFormat(ZEGRIndexBufferFormat Format)
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

	if (DirtyFlags.GetFlags(ZEGR_CONTEXT_DIRTY_BLEND_STATE))
	{
		Context->OMSetBlendState(RenderState->BlendState->GetInterface(), (float*)&BlendFactors, BlendMask);
		DirtyFlags.UnraiseFlags(ZEGR_CONTEXT_DIRTY_BLEND_STATE);
	}


	if (DirtyFlags.GetFlags(ZEGR_CONTEXT_DIRTY_STENCIL_STATE))
	{
		Context->OMSetDepthStencilState(RenderState->DepthStencilState->GetInterface(), StencilRef);
		DirtyFlags.UnraiseFlags(ZEGR_CONTEXT_DIRTY_STENCIL_STATE);
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

ID3D11DeviceContext1* ZED11Context::GetContext() const
{
	return Context;
}

void ZED11Context::SetRenderState(const ZEGRRenderStateData* State)
{
	if (State == NULL)
		return;

	RenderState = static_cast<const ZED11RenderStateData*>(State);

	if(RenderState->VertexLayout != NULL)
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

	if (BlendState != RenderState->BlendState->GetInterface())
		DirtyFlags.RaiseFlags(ZEGR_CONTEXT_DIRTY_BLEND_STATE);

	if (DepthStencilState != RenderState->DepthStencilState->GetInterface())
		DirtyFlags.RaiseFlags(ZEGR_CONTEXT_DIRTY_STENCIL_STATE);
}

void ZED11Context::SetVertexBuffers(ZEUInt Index, ZEUInt Count, const ZEGRVertexBuffer*const* Buffers)
{
	zeDebugCheck(Index >= D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, "Vertex buffer index exceeds the max slot count.");
	zeDebugCheck(Count >= (D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - Index), "Vertex buffer count is too much.");

	ZEUInt Strides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
	ZEUInt Offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
	ID3D11Buffer* NativeBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

	memset(Strides, 0, sizeof(ZEUInt) * Count);
	memset(Offsets, 0, sizeof(ZEUInt) * Count);
	memset(NativeBuffers, NULL, sizeof(ID3D11Buffer*) * Count);

	for(ZEUInt I = 0; I < Count; ++I)
	{
		if (Buffers[I] != NULL)
		{
			const ZED11VertexBuffer* D11Buffer = static_cast<const ZED11VertexBuffer*>(Buffers[I]);
			Strides[I] = (ZEUInt)D11Buffer->GetVertexSize();
			NativeBuffers[I] = D11Buffer->GetBuffer();
		}
	}

	Context->IASetVertexBuffers(Index, Count, NativeBuffers, Strides, Offsets);
}

void ZED11Context::SetIndexBuffer(const ZEGRIndexBuffer* Buffer)
{
	const ZED11IndexBuffer* D11Buffer = static_cast<const ZED11IndexBuffer*>(Buffer);

	ID3D11Buffer* NativeBuffer = NULL;
	ZEGRIndexBufferFormat Format = ZEGRIndexBufferFormat::ZEGR_IBF_NONE;
	if (D11Buffer != NULL)
	{
		NativeBuffer = D11Buffer->GetBuffer();
		Format = D11Buffer->GetFormat();
	}

	Context->IASetIndexBuffer(NativeBuffer, ConverIndexBufferFormat(Format), 0);
}

void ZED11Context::SetRenderTargets(ZEUInt Count, const ZEGRRenderTarget*const* RenderTargets, const ZEGRDepthStencilBuffer* DepthStencilBuffer)
{
	zeDebugCheck(Count >= ZEGR_MAX_RENDER_TARGET_SLOT, "RenderTarget count is too much.");
	
	if (Count >= ZEGR_MAX_RENDER_TARGET_SLOT)
		Count = ZEGR_MAX_RENDER_TARGET_SLOT;

	ID3D11RenderTargetView* NativeRenderTargets[ZEGR_MAX_RENDER_TARGET_SLOT];
	memset(&NativeRenderTargets[0], NULL, sizeof(ID3D11RenderTargetView*) * Count);

	for (ZESize I = 0; I < Count; I++)
	{
		if (RenderTargets[I] != NULL)
			NativeRenderTargets[I] = static_cast<const ZED11RenderTarget*>(RenderTargets[I])->GetView();
	}

	ID3D11DepthStencilView* NativeDepthStencil = NULL;
	if (DepthStencilBuffer != NULL)
		NativeDepthStencil = static_cast<const ZED11DepthStencilBuffer*>(DepthStencilBuffer)->GetView();

	Context->OMSetRenderTargets(Count, NativeRenderTargets, NativeDepthStencil);
}

void ZED11Context::GetRenderTargets(ZEUInt Count, ZEGRRenderTarget** RenderTargets, ZEGRDepthStencilBuffer** DepthStencilBuffer)
{
	zeDebugCheck(Count >= ZEGR_MAX_RENDER_TARGET_SLOT, "RenderTarget count is too much.");
	zeCheckError(RenderTargets == NULL && DepthStencilBuffer == NULL, ZE_VOID, "Both render target and depth-stencil buffer cannot be NULL.");

	if (Count >= ZEGR_MAX_RENDER_TARGET_SLOT)
		Count = ZEGR_MAX_RENDER_TARGET_SLOT;

	ID3D11RenderTargetView* NativeRenderTargets[ZEGR_MAX_RENDER_TARGET_SLOT];
	memset(&NativeRenderTargets[0], NULL, sizeof(ID3D11RenderTargetView*) * Count);
	ID3D11DepthStencilView* NativeDepthStencil = NULL;

	Context->OMGetRenderTargets(Count, NativeRenderTargets, &NativeDepthStencil);

	ZED11RenderTarget* _RenderTargets[ZEGR_MAX_RENDER_TARGET_SLOT];
	ZED11DepthStencilBuffer* _DepthStencilBuffer;

	for(ZEUInt I = 0; I < Count; I++)
	{
		_RenderTargets[I] = new ZED11RenderTarget();
		_RenderTargets[I]->View = NativeRenderTargets[I];
	}

	_DepthStencilBuffer = new ZED11DepthStencilBuffer();
	_DepthStencilBuffer->View = NativeDepthStencil;

	if(RenderTargets != NULL)
		memcpy(RenderTargets, _RenderTargets, sizeof(ZEGRRenderTarget*) * Count);
	if(DepthStencilBuffer != NULL)
		*DepthStencilBuffer = _DepthStencilBuffer;
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
	zeDebugCheck(Count > ZEGR_MAX_VIEWPORT_SLOT, "Viewport count is too much.");

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

void ZED11Context::SetStructuredBuffer(ZEGRShaderType Shader, ZEUInt Index, const ZEGRStructuredBuffer* Buffer)
{
	ID3D11ShaderResourceView* ResourceView = NULL;
	if(Buffer != NULL)
	{
		ResourceView = static_cast<const ZED11StructuredBuffer*>(Buffer)->GetResourceView();
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

void ZED11Context::SetUnorderedAccessView(ZEGRShaderType Shader, ZEUInt Index, const ZEGRTexture* Texture)
{
	ID3D11UnorderedAccessView* NativeView = NULL;

	if (Texture != NULL)
	{
		switch (Texture->GetTextureType())
		{
		case ZEGR_TT_3D:
			NativeView = static_cast<const ZED11Texture3D*>(Texture)->GetUnorderedAccessView();
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

	case ZEGR_ST_COMPUTE:
		Context->CSSetUnorderedAccessViews(Index, 1, &NativeView, NULL);
		break;
	}
}

void ZED11Context::SetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index, const ZEGRConstantBuffer* Buffer)
{
	ID3D11Buffer* NativeConstantBuffer = NULL;

	if (Buffer != NULL)
		 NativeConstantBuffer = static_cast<const ZED11ConstantBuffer*>(Buffer)->GetBuffer();

	switch(Shader)
	{
		default:
		case ZEGR_ST_NONE:
			break;

		case ZEGR_ST_VERTEX:
			Context->VSSetConstantBuffers(Index, 1, &NativeConstantBuffer);
			break;

		case ZEGR_ST_PIXEL:
			Context->PSSetConstantBuffers(Index, 1, &NativeConstantBuffer);
			break;

		case ZEGR_ST_GEOMETRY:
			Context->GSSetConstantBuffers(Index, 1, &NativeConstantBuffer);
			break;

		case ZEGR_ST_DOMAIN:
			Context->DSSetConstantBuffers(Index, 1, &NativeConstantBuffer);
			break;

		case ZEGR_ST_HULL:
			Context->HSSetConstantBuffers(Index, 1, &NativeConstantBuffer);
			break;

		case ZEGR_ST_COMPUTE:
			Context->CSSetConstantBuffers(Index, 1, &NativeConstantBuffer);
			break;

		case ZEGR_ST_ALL:
			Context->VSSetConstantBuffers(Index, 1, &NativeConstantBuffer);
			Context->PSSetConstantBuffers(Index, 1, &NativeConstantBuffer);
			Context->GSSetConstantBuffers(Index, 1, &NativeConstantBuffer);
			Context->DSSetConstantBuffers(Index, 1, &NativeConstantBuffer);
			Context->HSSetConstantBuffers(Index, 1, &NativeConstantBuffer);
			Context->CSSetConstantBuffers(Index, 1, &NativeConstantBuffer);
			break;
	}
}

void ZED11Context::GetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index, ZEGRConstantBuffer** Buffer)
{
	zeCheckError(Buffer == NULL, ZE_VOID,"Constant buffer cannot be NULL.");
	zeCheckError(Shader == ZEGR_ST_ALL, ZE_VOID, "Shader type cannot be ZEGR_ST_ALL.");

	ID3D11Buffer* NativeBuffer = NULL;

	switch (Shader)
	{
	default:
	case ZEGR_ST_NONE:
		*Buffer = NULL;
		break;
	case ZEGR_ST_VERTEX:
		Context->VSGetConstantBuffers(Index, 1, &NativeBuffer);
		break;
	case ZEGR_ST_PIXEL:
		Context->PSGetConstantBuffers(Index, 1, &NativeBuffer);
		break;
	case ZEGR_ST_GEOMETRY:
		Context->GSGetConstantBuffers(Index, 1, &NativeBuffer);
		break;
	case ZEGR_ST_DOMAIN:
		Context->DSGetConstantBuffers(Index, 1, &NativeBuffer);
		break;
	case ZEGR_ST_HULL:
		Context->HSGetConstantBuffers(Index, 1, &NativeBuffer);
		break;
	case ZEGR_ST_COMPUTE:
		Context->CSGetConstantBuffers(Index, 1, &NativeBuffer);
		break;
	}

	ZED11ConstantBuffer* ConstantBuffer = new ZED11ConstantBuffer();
	ConstantBuffer->Buffer = NativeBuffer;

	*Buffer = ConstantBuffer;
}

void ZED11Context::SetTexture(ZEGRShaderType Shader, ZEUInt Index, const ZEGRTexture* Texture)
{
	ID3D11ShaderResourceView* NativeTexture = NULL;

	if (Texture != NULL)
	{
		switch (Texture->GetTextureType())
		{
			case ZEGR_TT_2D:
				NativeTexture = static_cast<const ZED11Texture2D*>(Texture)->GetResourceView();
				break;

			case ZEGR_TT_3D:
				NativeTexture = static_cast<const ZED11Texture3D*>(Texture)->GetResourceView();
				break;

			case ZEGR_TT_CUBE:
				NativeTexture = static_cast<const ZED11TextureCube*>(Texture)->GetResourceView();
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

		case ZEGR_ST_ALL:
			Context->VSSetShaderResources(Index, 1, &NativeTexture);
			Context->GSSetShaderResources(Index, 1, &NativeTexture);
			Context->HSSetShaderResources(Index, 1, &NativeTexture);
			Context->DSSetShaderResources(Index, 1, &NativeTexture);
			Context->PSSetShaderResources(Index, 1, &NativeTexture);
			Context->CSSetShaderResources(Index, 1, &NativeTexture);
			break;
	}
}

void ZED11Context::GetTexture(ZEGRShaderType Shader, ZEUInt Index, ZEGRTexture** Texture)
{
	zeCheckError(Texture == NULL, ZE_VOID, "Texture cannot be NULL");
	zeCheckError(Shader == ZEGR_ST_ALL, ZE_VOID, "Shader type cannot be ZEGR_ST_ALL");

	ID3D11ShaderResourceView* ResourceView = NULL;

	switch(Shader)
	{
		default:
		case ZEGR_ST_NONE:
			break;

		case ZEGR_ST_VERTEX:
			Context->VSGetShaderResources(Index, 1, &ResourceView);
			break;

		case ZEGR_ST_PIXEL:
			Context->PSGetShaderResources(Index, 1, &ResourceView);
			break;

		case ZEGR_ST_GEOMETRY:
			Context->GSGetShaderResources(Index, 1, &ResourceView);
			break;

		case ZEGR_ST_DOMAIN:
			Context->DSGetShaderResources(Index, 1, &ResourceView);
			break;

		case ZEGR_ST_HULL:
			Context->HSGetShaderResources(Index, 1, &ResourceView);
			break;

		case ZEGR_ST_COMPUTE:
			Context->CSGetShaderResources(Index, 1, &ResourceView);
			break;
	}

	if(ResourceView != NULL)
	{
		ID3D11Resource* Resource;
		ResourceView->GetResource(&Resource);
		D3D11_RESOURCE_DIMENSION ResourceDimension;
		Resource->GetType(&ResourceDimension);

		if(ResourceDimension == D3D11_RESOURCE_DIMENSION_TEXTURE2D)
		{
			ZED11Texture2D *Texture2D = new ZED11Texture2D();
			Texture2D->ResourceView = ResourceView;

			*Texture = Texture2D;
		}
		else if(ResourceDimension == D3D11_RESOURCE_DIMENSION_TEXTURE3D)
		{
			ZED11Texture3D *Texture3D = new ZED11Texture3D();
			Texture3D->ResourceView = ResourceView;

			*Texture = Texture3D;
		}
	}
	else
	{
		*Texture = NULL;
	}
}

void ZED11Context::SetSampler(ZEGRShaderType Shader, ZEUInt Index, const ZEGRSampler* Sampler)
{
	const ZED11Sampler* D11Sampler = static_cast<const ZED11Sampler*>(Sampler);

	ID3D11SamplerState* NativeSampler = NULL;
	if (D11Sampler != NULL)
		NativeSampler = D11Sampler->GetInterface();

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

void ZED11Context::Dispatch(ZEUInt GroupCountX, ZEUInt GroupCountY, ZEUInt GroupCountZ)
{
	GetMainContext()->Dispatch(GroupCountX, GroupCountY, GroupCountZ);
}

void ZED11Context::ClearRenderTarget(const ZEGRRenderTarget* RenderTarget, const ZEVector4& ClearColor)
{
	GetMainContext()->ClearRenderTargetView(static_cast<const ZED11RenderTarget*>(RenderTarget)->GetView(), ClearColor.M);
}

void ZED11Context::ClearDepthStencilBuffer(const ZEGRDepthStencilBuffer* DepthStencil, bool Depth, bool Stencil, float DepthValue, ZEUInt8 StencilValue)
{
	UINT ClearFlag = 0;
	ClearFlag |= Depth ? D3D11_CLEAR_DEPTH : 0;
	ClearFlag |= Stencil ? D3D11_CLEAR_STENCIL : 0;

	GetMainContext()->ClearDepthStencilView(static_cast<const ZED11DepthStencilBuffer*>(DepthStencil)->GetView(), ClearFlag, DepthValue, StencilValue);
}

void ZED11Context::ClearUnorderedAccessView(const ZEGRTexture* Texture, const ZEVector4& ClearColor)
{
	ID3D11UnorderedAccessView* NativeView = NULL;
	if(Texture != NULL)
		if(Texture->GetTextureType() == ZEGR_TT_3D)
			NativeView = static_cast<const ZED11Texture3D*>(Texture)->GetUnorderedAccessView();

	GetMainContext()->ClearUnorderedAccessViewFloat(NativeView, ClearColor.M);
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
