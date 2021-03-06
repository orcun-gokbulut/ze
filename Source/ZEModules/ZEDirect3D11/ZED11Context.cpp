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

#include "ZERandom.h"

#include "ZED11Module.h"
#include "ZED11Shader.h"
#include "ZED11Buffer.h"
#include "ZED11Texture.h"
#include "ZED11Sampler.h"
#include "ZED11StatePool.h"
#include "ZED11RenderTarget.h"
#include "ZED11DepthStencilBuffer.h"
#include "ZED11RenderStateData.h"

#include "ZEGraphics/ZEGRDefinitions.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRScissorRect.h"

#include <d3d9.h>

#pragma comment(lib, "d3d9.lib")

#define ZEGR_CDF_BLEND_STATE			1
#define ZEGR_CDF_DEPTHSTENCIL_STATE		2

void ZED11Context::Initialize(ID3D11DeviceContext1* Context)
{
	this->Context = Context;

#ifdef ZE_EDITION_DEVELOPMENT
	HRESULT HR = this->Context->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), reinterpret_cast<void**>(&UserDefinedAnnotation));
	if (FAILED(HR))
	{
		zeError("Context initialize failed. QueryInterface error: 0x%X", HR);
		return;
	}

	if (UserDefinedAnnotation->GetStatus())
		ProfilingEnabled = true;
#endif

	GraphicsState = static_cast<ZED11RenderStateData*>(GetModule()->CreateRenderStateData());
	ComputeState = static_cast<ZED11ComputeRenderStateData*>(GetModule()->CreateComputeRenderStateData());
}

void ZED11Context::Deinitialize()
{
	ZEGR_RELEASE(Context);
	ZEGR_RELEASE(UserDefinedAnnotation);

	DirtyFlags.RaiseAll();

	GraphicsState.Release();
	ComputeState.Release();
}

void ZED11Context::UpdateGraphicsState()
{
	if (DirtyFlags.GetFlags(ZEGR_CDF_BLEND_STATE))
	{
		LockContext();

		Context->OMSetBlendState(GraphicsState->BlendState->GetInterface(), BlendFactors.M, BlendMask);

		UnlockContext();

		DirtyFlags.UnraiseFlags(ZEGR_CDF_BLEND_STATE);
	}

	if (DirtyFlags.GetFlags(ZEGR_CDF_DEPTHSTENCIL_STATE))
	{
		LockContext();

		Context->OMSetDepthStencilState(GraphicsState->DepthStencilState->GetInterface(), StencilRef);

		UnlockContext();

		DirtyFlags.UnraiseFlags(ZEGR_CDF_DEPTHSTENCIL_STATE);
	}
}

ID3D11DeviceContext1* ZED11Context::GetContext() const
{
	return Context;
}

ZED11Context::ZED11Context()
{
	Context = NULL;
	UserDefinedAnnotation = NULL;

	DirtyFlags.RaiseAll();

	BlendFactors = ZEVector4::One;
	BlendMask = 0xFFFFFFFF;
	StencilRef = 0;
}

ZED11Context::~ZED11Context()
{
	Deinitialize();
}

void ZED11Context::SetRenderState(const ZEGRRenderStateData* State)
{
	if (State == NULL)
		return;

	const ZED11RenderStateData* D11State = static_cast<const ZED11RenderStateData*>(State);

	for (ZEUInt I = 0; I < ZEGR_SHADER_TYPE_COUNT; I++)
	{
		const ZED11Shader* SrcShader = D11State->Shaders[(ZEGRShaderType)I];
		const ZED11Shader* DestShader = GraphicsState->Shaders[(ZEGRShaderType)I];
		bool SrcShaderExist = (SrcShader != NULL);
		bool DestShaderExist = (DestShader != NULL);

		if (!SrcShaderExist && !DestShaderExist)
			continue;

		if (SrcShaderExist && DestShaderExist)
		{	
			const ZEArray<ZEBYTE>& DestShaderByteCode= DestShader->GetByteCode();
			const ZEArray<ZEBYTE>& SrcShaderByteCode= SrcShader->GetByteCode();

			ZESize SrcShaderByteCodeLength = SrcShaderByteCode.GetCount();
			if (SrcShaderByteCodeLength == DestShaderByteCode.GetCount() &&
				memcmp(SrcShaderByteCode.GetConstCArray(), DestShaderByteCode.GetConstCArray(), SrcShaderByteCodeLength) == 0)
				continue;
		}

		bool SetShaderNull = (!SrcShaderExist && DestShaderExist);

		LockContext();

		switch ((ZEGRShaderType)I)
		{
			case ZEGR_ST_VERTEX:
				Context->VSSetShader(SetShaderNull ? NULL : SrcShader->GetVertexShader(), NULL, 0);
				break;

			case ZEGR_ST_DOMAIN:
				Context->DSSetShader(SetShaderNull ? NULL : SrcShader->GetDomainShader(), NULL, 0);
				break;

			case ZEGR_ST_HULL:
				Context->HSSetShader(SetShaderNull ? NULL : SrcShader->GetHullShader(), NULL, 0);
				break;

			case ZEGR_ST_GEOMETRY:
				Context->GSSetShader(SetShaderNull ? NULL : SrcShader->GetGeometryShader(), NULL, 0);
				break;

			case ZEGR_ST_PIXEL:
				Context->PSSetShader(SetShaderNull ? NULL : SrcShader->GetPixelShader(), NULL, 0);
				break;

			default:
				break;
		}
		
		UnlockContext();

		GraphicsState->Shaders[(ZEGRShaderType)I] = D11State->Shaders[(ZEGRShaderType)I];
	}

	if (D11State->BlendState != NULL && GraphicsState->BlendState != D11State->BlendState)
	{
		GraphicsState->BlendState = D11State->BlendState;
		DirtyFlags.RaiseFlags(ZEGR_CDF_BLEND_STATE);
	}

	if (D11State->RasterizerState != NULL && GraphicsState->RasterizerState != D11State->RasterizerState)
	{
		LockContext();
		Context->RSSetState(D11State->RasterizerState->GetInterface());
		UnlockContext();

		GraphicsState->RasterizerState = D11State->RasterizerState;
	}

	if (D11State->DepthStencilState != NULL && GraphicsState->DepthStencilState != D11State->DepthStencilState)
	{
		GraphicsState->DepthStencilState = D11State->DepthStencilState;
		DirtyFlags.RaiseFlags(ZEGR_CDF_DEPTHSTENCIL_STATE);
	}

	if (D11State->VertexLayout != NULL && GraphicsState->VertexLayout != D11State->VertexLayout)
	{
		LockContext();
		Context->IASetInputLayout(D11State->VertexLayout->GetInterface());
		UnlockContext();

		GraphicsState->VertexLayout = D11State->VertexLayout;
	}

	if (GraphicsState->PrimitiveTopology != D11State->PrimitiveTopology)
	{
		LockContext();
		Context->IASetPrimitiveTopology(D11State->PrimitiveTopology);
		UnlockContext();

		GraphicsState->PrimitiveTopology = D11State->PrimitiveTopology;
	}
}

void ZED11Context::SetComputeRenderState(const ZEGRComputeRenderStateData* State)
{
	if (State == NULL)
		return;

	const ZED11ComputeRenderStateData* D11State = static_cast<const ZED11ComputeRenderStateData*>(State);

	const ZED11Shader* DestShader = ComputeState->ComputeShader;
	const ZED11Shader* SrcShader = D11State->ComputeShader;

	if (DestShader != NULL && SrcShader != NULL)
	{	
		const ZEArray<ZEBYTE>& DestShaderByteCode= DestShader->GetByteCode();
		const ZEArray<ZEBYTE>& SrcShaderByteCode= SrcShader->GetByteCode();

		ZESize SrcShaderByteCodeLength = SrcShaderByteCode.GetCount();
		if (SrcShaderByteCodeLength == DestShaderByteCode.GetCount() &&
			memcmp(SrcShaderByteCode.GetConstCArray(), DestShaderByteCode.GetConstCArray(), SrcShaderByteCodeLength) == 0)
			return;
	}

	LockContext();
	Context->CSSetShader(SrcShader->GetComputeShader(), NULL, 0);
	UnlockContext();

	ComputeState->ComputeShader = D11State->ComputeShader;
}

void ZED11Context::SetVertexBuffers(ZEUInt Index, ZEUInt Count, const ZEGRBuffer*const* Buffers, const ZEUInt* Offsets)
{
	if (!CheckVertexBuffers(Index, Count, Buffers))
		return;

	UINT StridesInBytes[ZEGR_MAX_VERTEX_BUFFER_SLOT] = {};
	UINT OffsetsInBytes[ZEGR_MAX_VERTEX_BUFFER_SLOT] = {};
	ID3D11Buffer* NativeBuffers[ZEGR_MAX_VERTEX_BUFFER_SLOT] = {};

	for (ZEUInt I = 0; I < Count; I++)
	{
		ZEUInt Slot = Index + I;
		if (VertexBuffers[Slot] != NULL)
		{
			StridesInBytes[I] = static_cast<const ZED11Buffer*>(VertexBuffers[Slot])->StrideInBytes;
			NativeBuffers[I] = static_cast<const ZED11Buffer*>(VertexBuffers[Slot])->Buffer;

			if (Offsets != NULL)
				OffsetsInBytes[I] = Offsets[I];
		}
	}

	LockContext();
	Context->IASetVertexBuffers(Index, Count, NativeBuffers, StridesInBytes, OffsetsInBytes);
	UnlockContext();
}

void ZED11Context::SetIndexBuffer(const ZEGRBuffer* Buffer, ZEUInt Offset)
{
	if (!CheckIndexBuffer(Buffer))
		return;

	ID3D11Buffer* NativeBuffer = NULL;
	DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;
	
	if (IndexBuffer != NULL)
	{
		Format = ConvertFormat(IndexBuffer->GetFormat());
		NativeBuffer = static_cast<const ZED11Buffer*>(IndexBuffer)->Buffer;
	}

	LockContext();
	Context->IASetIndexBuffer(NativeBuffer, Format, 0);
	UnlockContext();
}

void ZED11Context::SetConstantBuffers(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRBuffer*const* Buffers, const ZEUInt* FirstConstant, const ZEUInt* NumConstants)
{
	if (!CheckConstantBuffers(Shader, Index, Count, Buffers, FirstConstant, NumConstants))
		return;

	ID3D11Buffer* NativeBuffers[ZEGR_MAX_CONSTANT_BUFFER_SLOT] = {};
	ZEUInt ShaderIndex = (Shader == ZEGR_ST_ALL) ? 0 : Shader;

	for (ZEUInt I = 0; I < Count; I++)
	{
		ZEUInt Slot = Index + I;

		if (ConstantBuffers[ShaderIndex][Slot] != NULL)
			NativeBuffers[I] = static_cast<const ZED11Buffer*>(ConstantBuffers[ShaderIndex][Slot])->Buffer;
	}

	LockContext();

	switch (Shader)
	{
		case ZEGR_ST_VERTEX:
			Context->VSSetConstantBuffers1(Index, Count, NativeBuffers, FirstConstant, NumConstants);
			break;

		case ZEGR_ST_PIXEL:
			Context->PSSetConstantBuffers1(Index, Count, NativeBuffers, FirstConstant, NumConstants);
			break;

		case ZEGR_ST_GEOMETRY:
			Context->GSSetConstantBuffers1(Index, Count, NativeBuffers, FirstConstant, NumConstants);
			break;

		case ZEGR_ST_DOMAIN:
			Context->DSSetConstantBuffers1(Index, Count, NativeBuffers, FirstConstant, NumConstants);
			break;

		case ZEGR_ST_HULL:
			Context->HSSetConstantBuffers1(Index, Count, NativeBuffers, FirstConstant, NumConstants);
			break;

		case ZEGR_ST_COMPUTE:
			Context->CSSetConstantBuffers1(Index, Count, NativeBuffers, FirstConstant, NumConstants);
			break;

		case ZEGR_ST_ALL:
			Context->VSSetConstantBuffers1(Index, Count, NativeBuffers, FirstConstant, NumConstants);
			Context->PSSetConstantBuffers1(Index, Count, NativeBuffers, FirstConstant, NumConstants);
			Context->GSSetConstantBuffers1(Index, Count, NativeBuffers, FirstConstant, NumConstants);
			Context->DSSetConstantBuffers1(Index, Count, NativeBuffers, FirstConstant, NumConstants);
			Context->HSSetConstantBuffers1(Index, Count, NativeBuffers, FirstConstant, NumConstants);
			Context->CSSetConstantBuffers1(Index, Count, NativeBuffers, FirstConstant, NumConstants);
			break;

		default:
			break;
	}

	UnlockContext();
}

void ZED11Context::SetTextureView(ZEGRShaderType Shader, ZEUInt Index, const ZEGRTextureView* TextureView)
{
	if (TextureView == NULL)
		return;

	ID3D11ShaderResourceView* ShaderResourceView = static_cast<const ZED11ShaderResourceView*>(TextureView)->View;//static_cast<const ZED11Texture*>(TextureView->GetTexture())->GetSrv(*TextureView);

	LockContext();

	switch (Shader)
	{
		case ZEGR_ST_VERTEX:
			Context->VSSetShaderResources(Index, 1, &ShaderResourceView);
			break;

		case ZEGR_ST_PIXEL:
			Context->PSSetShaderResources(Index, 1, &ShaderResourceView);
			break;

		case ZEGR_ST_GEOMETRY:
			Context->GSSetShaderResources(Index, 1, &ShaderResourceView);
			break;

		case ZEGR_ST_DOMAIN:
			Context->DSSetShaderResources(Index, 1, &ShaderResourceView);
			break;

		case ZEGR_ST_HULL:
			Context->HSSetShaderResources(Index, 1, &ShaderResourceView);
			break;

		case ZEGR_ST_COMPUTE:
			Context->CSSetShaderResources(Index, 1, &ShaderResourceView);
			break;

		case ZEGR_ST_ALL:
			Context->VSSetShaderResources(Index, 1, &ShaderResourceView);
			Context->PSSetShaderResources(Index, 1, &ShaderResourceView);
			Context->GSSetShaderResources(Index, 1, &ShaderResourceView);
			Context->DSSetShaderResources(Index, 1, &ShaderResourceView);
			Context->HSSetShaderResources(Index, 1, &ShaderResourceView);
			Context->CSSetShaderResources(Index, 1, &ShaderResourceView);
			break;

		default:
			break;
	}

	UnlockContext();
}

void ZED11Context::SetRWTextureView(ZEUInt Index, const ZEGRTextureView* RWTextureView)
{
	if (RWTextureView == NULL)
		return;
	
	ID3D11UnorderedAccessView* UnorderedAccessView = static_cast<const ZED11UnorderedAccessView*>(RWTextureView)->View;

	LockContext();
	Context->CSSetUnorderedAccessViews(Index, 1, &UnorderedAccessView, NULL);
	UnlockContext();
}

void ZED11Context::SetBuffers(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRBuffer*const* Buffers)
{
	if (!CheckShaderResources(Shader, Index, Count, reinterpret_cast<const ZEGRResource*const*>(Buffers)))
		return;

	ID3D11ShaderResourceView* NativeShaderResources[ZEGR_MAX_TEXTURE_SLOT] = {};
	ZEUInt ShaderIndex = (Shader == ZEGR_ST_ALL) ? 0 : Shader;

	for (ZEUInt I = 0; I < Count; I++)
	{
		ZEUInt Slot = Index + I;

		const ZEGRResource* Resource = ShaderResources[ShaderIndex][Slot];
		if (Resource == NULL)
			continue;

		switch (Resource->GetResourceType())
		{
			case ZEGR_RT_BUFFER:
				NativeShaderResources[I] = static_cast<const ZED11Buffer*>(Resource)->ShaderResourceView;
				break;

			case ZEGR_RT_TEXTURE:
				{
					const ZED11Texture* Texture = static_cast<const ZED11Texture*>(Resource);
					NativeShaderResources[I] = static_cast<ZED11ShaderResourceView*>(Texture->ShaderResourceViews[0])->View;
				}
				break;

			default:
				break;
		}
	}

	LockContext();

	switch (Shader)
	{
		case ZEGR_ST_VERTEX:
			Context->VSSetShaderResources(Index, Count, NativeShaderResources);
			break;

		case ZEGR_ST_PIXEL:
			Context->PSSetShaderResources(Index, Count, NativeShaderResources);
			break;

		case ZEGR_ST_GEOMETRY:
			Context->GSSetShaderResources(Index, Count, NativeShaderResources);
			break;

		case ZEGR_ST_DOMAIN:
			Context->DSSetShaderResources(Index, Count, NativeShaderResources);
			break;

		case ZEGR_ST_HULL:
			Context->HSSetShaderResources(Index, Count, NativeShaderResources);
			break;

		case ZEGR_ST_COMPUTE:
			Context->CSSetShaderResources(Index, Count, NativeShaderResources);
			break;

		case ZEGR_ST_ALL:
			Context->VSSetShaderResources(Index, Count, NativeShaderResources);
			Context->PSSetShaderResources(Index, Count, NativeShaderResources);
			Context->GSSetShaderResources(Index, Count, NativeShaderResources);
			Context->DSSetShaderResources(Index, Count, NativeShaderResources);
			Context->HSSetShaderResources(Index, Count, NativeShaderResources);
			Context->CSSetShaderResources(Index, Count, NativeShaderResources);
			break;

		default:
			break;
	}

	UnlockContext();
}

void ZED11Context::SetTextures(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRTexture*const* Textures)
{
	if (!CheckShaderResources(Shader, Index, Count, reinterpret_cast<const ZEGRResource*const*>(Textures)))
		return;

	ID3D11ShaderResourceView* NativeShaderResources[ZEGR_MAX_TEXTURE_SLOT] = {};
	ZEUInt ShaderIndex = (Shader == ZEGR_ST_ALL) ? 0 : Shader;

	for (ZEUInt I = 0; I < Count; I++)
	{
		ZEUInt Slot = Index + I;

		const ZEGRResource* Resource = static_cast<const ZEGRTexture*>(ShaderResources[ShaderIndex][Slot]);

		if (Resource == NULL)
			continue;

		switch (Resource->GetResourceType())
		{
			case ZEGR_RT_BUFFER:
				NativeShaderResources[I] = static_cast<const ZED11Buffer*>(Resource)->ShaderResourceView;
				break;

			case ZEGR_RT_TEXTURE:
				{
					const ZED11Texture* Texture = static_cast<const ZED11Texture*>(Resource);
					NativeShaderResources[I] = static_cast<ZED11ShaderResourceView*>(Texture->ShaderResourceViews[0])->View;
				}
				break;

			default:
				break;
		}
	}

	LockContext();

	switch (Shader)
	{
		case ZEGR_ST_VERTEX:
			Context->VSSetShaderResources(Index, Count, NativeShaderResources);
			break;

		case ZEGR_ST_PIXEL:
			Context->PSSetShaderResources(Index, Count, NativeShaderResources);
			break;

		case ZEGR_ST_GEOMETRY:
			Context->GSSetShaderResources(Index, Count, NativeShaderResources);
			break;

		case ZEGR_ST_DOMAIN:
			Context->DSSetShaderResources(Index, Count, NativeShaderResources);
			break;

		case ZEGR_ST_HULL:
			Context->HSSetShaderResources(Index, Count, NativeShaderResources);
			break;

		case ZEGR_ST_COMPUTE:
			Context->CSSetShaderResources(Index, Count, NativeShaderResources);
			break;

		case ZEGR_ST_ALL:
			Context->VSSetShaderResources(Index, Count, NativeShaderResources);
			Context->PSSetShaderResources(Index, Count, NativeShaderResources);
			Context->GSSetShaderResources(Index, Count, NativeShaderResources);
			Context->DSSetShaderResources(Index, Count, NativeShaderResources);
			Context->HSSetShaderResources(Index, Count, NativeShaderResources);
			Context->CSSetShaderResources(Index, Count, NativeShaderResources);
			break;

		default:
			break;
	}

	UnlockContext();
}

void ZED11Context::SetSamplers(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRSampler*const* Samplers)
{
	if (!CheckSamplers(Shader, Index, Count, Samplers))
		return;

	ID3D11SamplerState* NativeSamplers[ZEGR_MAX_SAMPLER_SLOT] = {NULL};
	ZEUInt ShaderIndex = (Shader == ZEGR_ST_ALL) ? 0 : Shader;

	for (ZEUInt I = 0; I < Count; I++)
	{
		ZEUInt Slot = Index + I;

		if (this->Samplers[ShaderIndex][Slot] != NULL)
			NativeSamplers[I] = static_cast<const ZED11Sampler*>(this->Samplers[ShaderIndex][Slot])->GetInterface();
	}

	LockContext();

	switch (Shader)
	{
		case ZEGR_ST_VERTEX:
			Context->VSSetSamplers(Index, Count, NativeSamplers);
			break;

		case ZEGR_ST_PIXEL:
			Context->PSSetSamplers(Index, Count, NativeSamplers);
			break;

		case ZEGR_ST_GEOMETRY:
			Context->GSSetSamplers(Index, Count, NativeSamplers);
			break;

		case ZEGR_ST_DOMAIN:
			Context->DSSetSamplers(Index, Count, NativeSamplers);
			break;

		case ZEGR_ST_HULL:
			Context->HSSetSamplers(Index, Count, NativeSamplers);
			break;

		case ZEGR_ST_COMPUTE:
			Context->CSSetSamplers(Index, Count, NativeSamplers);
			break;

		case ZEGR_ST_ALL:
			Context->VSSetSamplers(Index, Count, NativeSamplers);
			Context->PSSetSamplers(Index, Count, NativeSamplers);
			Context->GSSetSamplers(Index, Count, NativeSamplers);
			Context->DSSetSamplers(Index, Count, NativeSamplers);
			Context->HSSetSamplers(Index, Count, NativeSamplers);
			Context->CSSetSamplers(Index, Count, NativeSamplers);
			break;

		default:
			break;
	}

	UnlockContext();
}

void ZED11Context::SetRenderTargets(ZEUInt Count, const ZEGRRenderTarget*const* RenderTargets, const ZEGRDepthStencilBuffer* DepthStencilBuffer)
{
	if (!CheckRenderTargetsAndDepthStencilBuffer(Count, RenderTargets, DepthStencilBuffer))
		return;

	ID3D11RenderTargetView* NativeRenderTargets[ZEGR_MAX_RENDER_TARGET_SLOT] = {};
	ID3D11DepthStencilView*	NativeDepthStencil = NULL;

	for (ZEUInt I = 0; I < Count; I++)
	{
		if (this->RenderTargets[I] != NULL)
			NativeRenderTargets[I] = static_cast<const ZED11RenderTarget*>(this->RenderTargets[I])->GetView();
	}

	if (this->DepthStencilBuffer != NULL)
		NativeDepthStencil = static_cast<const ZED11DepthStencilBuffer*>(this->DepthStencilBuffer)->GetView();
	
	LockContext();
	Context->OMSetRenderTargets(Count, (Count == 0) ? NULL : NativeRenderTargets, NativeDepthStencil);
	UnlockContext();
}

void ZED11Context::SetScissorRects(ZEUInt Count, const ZEGRScissorRect* Rects)
{
	if (!CheckScissorRects(Count, Rects))
		return;

	D3D11_RECT NativeRects[ZEGR_MAX_SCISSOR_SLOT];

	for (ZEUInt I = 0; I < Count; I++)
	{
		NativeRects[I].left = this->ScissorRects[I].GetLeft();
		NativeRects[I].right = this->ScissorRects[I].GetRight();
		NativeRects[I].top = this->ScissorRects[I].GetTop();
		NativeRects[I].bottom = this->ScissorRects[I].GetBottom();
	}
	
	LockContext();
	Context->RSSetScissorRects(Count, NativeRects);
	UnlockContext();
}

void ZED11Context::SetViewports(ZEUInt Count, const ZEGRViewport* Viewports)
{
	if (!CheckViewports(Count, Viewports))
		return;

	D3D11_VIEWPORT NativeViewports[ZEGR_MAX_VIEWPORT_SLOT];

	for (ZEUInt I = 0; I < Count; I++)
	{
		NativeViewports[I].Width = this->Viewports[I].GetWidth();
		NativeViewports[I].Height = this->Viewports[I].GetHeight();
		NativeViewports[I].TopLeftX = this->Viewports[I].GetX();
		NativeViewports[I].TopLeftY = this->Viewports[I].GetY();
		NativeViewports[I].MinDepth = this->Viewports[I].GetMinDepth();
		NativeViewports[I].MaxDepth = this->Viewports[I].GetMaxDepth();
	}
	
	LockContext();
	Context->RSSetViewports(Count, NativeViewports);
	UnlockContext();
}

void ZED11Context::SetStencilRef(ZEUInt Reference)
{
	if (StencilRef == Reference)
		return;

	StencilRef = Reference;
	DirtyFlags.RaiseFlags(ZEGR_CDF_DEPTHSTENCIL_STATE);
}

void ZED11Context::SetBlendFactors(const ZEVector4& Factors)
{
	if (BlendFactors == Factors)
		return;

	BlendFactors = Factors;
	DirtyFlags.RaiseFlags(ZEGR_CDF_BLEND_STATE);
}

void ZED11Context::SetBlendMask(ZEUInt Mask)
{
	if (BlendMask == Mask)
		return;

	BlendMask = Mask;
	DirtyFlags.RaiseFlags(ZEGR_CDF_BLEND_STATE);
}

void ZED11Context::SetRWBuffers(ZEUInt Index, ZEUInt Count, const ZEGRBuffer*const* Buffers)
{
	if (!CheckUnorderedAccesses(Index, Count, reinterpret_cast<const ZEGRResource*const*>(Buffers)))
		return;

	ID3D11UnorderedAccessView* NativeUnorderedAccesses[ZEGR_MAX_RWTEXTURE_SLOT] = {};

	for (ZEUInt I = 0; I < Count; I++)
	{
		ZEUInt Slot = Index + I;

		const ZEGRResource* Resource = UnorderedAccesses[Slot];
		if (Resource != NULL)
		{
			switch (Resource->GetResourceType())
			{
				case ZEGR_RT_BUFFER:
					NativeUnorderedAccesses[I] = static_cast<const ZED11Buffer*>(Resource)->UnorderedAccessView;
					break;

				case ZEGR_RT_TEXTURE:
					{
						const ZED11Texture* Texture = static_cast<const ZED11Texture*>(Resource);
						NativeUnorderedAccesses[I] = static_cast<ZED11UnorderedAccessView*>(Texture->UnorderedAccessViews[0])->View;
					}
					break;

				default:
					break;
			}
		}
	}
	
	LockContext();
	Context->CSSetUnorderedAccessViews(Index, Count, NativeUnorderedAccesses, NULL);
	UnlockContext();
}

void ZED11Context::SetRWTextures(ZEUInt Index, ZEUInt Count, const ZEGRTexture*const* Textures)
{
	if (!CheckUnorderedAccesses(Index, Count, reinterpret_cast<const ZEGRResource*const*>(Textures)))
		return;
	
	ID3D11UnorderedAccessView* NativeUnorderedAccesses[ZEGR_MAX_RWTEXTURE_SLOT] = {NULL};

	for (ZEUInt I = 0; I < Count; I++)
	{
		ZEUInt Slot = Index + I;

		const ZEGRResource* Resource = UnorderedAccesses[Slot];
		if (Resource != NULL)
		{
			switch (Resource->GetResourceType())
			{
				case ZEGR_RT_BUFFER:
					NativeUnorderedAccesses[I] = static_cast<const ZED11Buffer*>(Resource)->UnorderedAccessView;
					break;

				case ZEGR_RT_TEXTURE:
					{
						const ZED11Texture* Texture = static_cast<const ZED11Texture*>(Resource);
						NativeUnorderedAccesses[I] = static_cast<ZED11UnorderedAccessView*>(Texture->UnorderedAccessViews[0])->View;
					}
					break;

				default:
					break;
			}
		}
	}

	LockContext();
	Context->CSSetUnorderedAccessViews(Index, Count, NativeUnorderedAccesses, NULL);
	UnlockContext();
}

void ZED11Context::CopyResource(const ZEGRResource* DestResource, const ZEGRResource* SrcResource)
{
	zeDebugCheck(DestResource == NULL || SrcResource == NULL, "Source or destination resource cannot be NULL");

	ID3D11Resource* DestNativeResource = NULL;
	ID3D11Resource* SrcNativeResource = NULL;

	switch (DestResource->GetResourceType())
	{
		case ZEGR_RT_BUFFER:
			DestNativeResource = static_cast<const ZED11Buffer*>(DestResource)->Buffer;
			break;

		case ZEGR_RT_TEXTURE:
			DestNativeResource = static_cast<const ZED11Texture*>(DestResource)->Resource;
			break;

		default:
			return;
	}

	switch (SrcResource->GetResourceType())
	{
		case ZEGR_RT_BUFFER:
			SrcNativeResource = static_cast<const ZED11Buffer*>(SrcResource)->Buffer;
			break;

		case ZEGR_RT_TEXTURE:
			SrcNativeResource = static_cast<const ZED11Texture*>(SrcResource)->Resource;
			break;

		default:
			return;
	}

	LockContext();
	GetMainContext()->CopyResource(DestNativeResource, SrcNativeResource);
	UnlockContext();
}

void ZED11Context::ResolveSubresource(const ZEGRTexture* DestTexture, ZEUInt DestSubresource, const ZEGRTexture* SrcTexture, ZEUInt SrcSubresource, ZEGRFormat Format)
{
	zeDebugCheck(DestTexture == NULL || SrcTexture == NULL, "Source or destination texture cannot be NULL");

	ID3D11Resource* DestNativeResource = static_cast<const ZED11Texture*>(DestTexture)->Resource;
	ID3D11Resource* SrcNativeResource = static_cast<const ZED11Texture*>(SrcTexture)->Resource;

	LockContext();
	GetMainContext()->ResolveSubresource(DestNativeResource, DestSubresource, SrcNativeResource, SrcSubresource, ConvertFormat(Format));
	UnlockContext();
}

void ZED11Context::Draw(ZEUInt VertexCount, ZEUInt FirstVertex)
{
	UpdateGraphicsState();

	LockContext();
	GetMainContext()->Draw(VertexCount, FirstVertex);
	UnlockContext();
}

void ZED11Context::DrawIndexed(ZEUInt IndexCount, ZEUInt IndexOffset, ZEUInt VertexOffset)
{
	UpdateGraphicsState();

	LockContext();
	GetMainContext()->DrawIndexed(IndexCount, IndexOffset, VertexOffset);
	UnlockContext();
}

void ZED11Context::DrawInstanced(ZEUInt VertexCount, ZEUInt FirstVertex, ZEUInt InstanceCount, ZEUInt FirstInstance)
{
	UpdateGraphicsState();

	LockContext();
	GetMainContext()->DrawInstanced(VertexCount, InstanceCount, FirstVertex, FirstInstance);
	UnlockContext();
}

void ZED11Context::DrawIndexedInstanced(ZEUInt IndexCount, ZEUInt IndexOffset, ZEUInt VertexOffset, ZEUInt InstanceCount, ZEUInt InstanceOffset)
{
	UpdateGraphicsState();

	LockContext();
	GetMainContext()->DrawIndexedInstanced(IndexCount, InstanceCount, IndexOffset, VertexOffset, InstanceOffset);
	UnlockContext();
}

void ZED11Context::Dispatch(ZEUInt GroupCountX, ZEUInt GroupCountY, ZEUInt GroupCountZ)
{
	LockContext();
	GetMainContext()->Dispatch(GroupCountX, GroupCountY, GroupCountZ);
	UnlockContext();
}

void ZED11Context::ClearRenderTarget(const ZEGRRenderTarget* RenderTarget, const ZEVector4& ClearColor)
{
	LockContext();
	GetMainContext()->ClearRenderTargetView(static_cast<const ZED11RenderTarget*>(RenderTarget)->GetView(), ClearColor.M);
	UnlockContext();
}

void ZED11Context::ClearDepthStencilBuffer(const ZEGRDepthStencilBuffer* DepthStencil, bool Depth, bool Stencil, float DepthValue, ZEUInt8 StencilValue)
{
	UINT ClearFlag = 0;
	ClearFlag |= Depth ? D3D11_CLEAR_DEPTH : 0;
	ClearFlag |= Stencil ? D3D11_CLEAR_STENCIL : 0;

	LockContext();
	GetMainContext()->ClearDepthStencilView(static_cast<const ZED11DepthStencilBuffer*>(DepthStencil)->GetView(), ClearFlag, DepthValue, StencilValue);
	UnlockContext();
}

void ZED11Context::ClearRWView(const ZEGRTextureView* RWView, const ZEVector4& ClearColor)
{
	ID3D11UnorderedAccessView* NativeView = static_cast<const ZED11UnorderedAccessView*>(RWView)->View;

	LockContext();
	GetMainContext()->ClearUnorderedAccessViewFloat(NativeView, ClearColor.M);
	UnlockContext();
}

void ZED11Context::ClearState()
{
	ZEGRContext::ClearState();

	GraphicsState->BlendState = NULL;
	GraphicsState->DepthStencilState = NULL;
	GraphicsState->PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	GraphicsState->RasterizerState = NULL;

	for (ZESize I = 0; I < ZEGR_SHADER_TYPE_COUNT; I++)
		GraphicsState->Shaders[I] = NULL;

	GraphicsState->VertexLayout = NULL;

	ComputeState->ComputeShader = NULL;

	GetMainContext()->ClearState();
}

void ZED11Context::BeginEvent(const ZEString& Name)
{
	//if (ProfilingEnabled)
	{
		LockContext();
		//UserDefinedAnnotation->BeginEvent(Name.ToWCString());
		D3DPERF_BeginEvent(D3DCOLOR_XRGB(ZERandom::GetUInt8(), ZERandom::GetUInt8(), ZERandom::GetUInt8()), Name.ToWCString());
		//Context->BeginEventInt(Name.ToWCString(), 0);
		UnlockContext();
	}
}

void ZED11Context::EndEvent()
{
	//if (ProfilingEnabled)
	{
		LockContext();
		//UserDefinedAnnotation->EndEvent();
		D3DPERF_EndEvent();
		//Context->EndEvent();
		UnlockContext();
	}
}

void ZED11Context::SetMarker(const ZEString& Name)
{
	LockContext();
	D3DPERF_SetMarker(D3DCOLOR_XRGB(ZERandom::GetUInt8(), ZERandom::GetUInt8(), ZERandom::GetUInt8()), Name.ToWCString());
	UnlockContext();
}
