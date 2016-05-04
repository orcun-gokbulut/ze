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
#include "ZEGraphics/ZEGRStructuredBuffer.h"

#define ZEGR_CDF_BLEND_STATE			1
#define ZEGR_CDF_DEPTHSTENCIL_STATE		2

static DXGI_FORMAT ConvertIndexBufferFormat(ZEGRIndexBufferFormat Format)
{
	switch (Format)
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

void ZED11Context::Initialize(ID3D11DeviceContext* Context)
{
	this->Context = Context;

	GraphicsState = static_cast<ZED11RenderStateData*>(GetModule()->CreateRenderStateData());
	ComputeState = static_cast<ZED11ComputeRenderStateData*>(GetModule()->CreateComputeRenderStateData());
}

void ZED11Context::Deinitialize()
{
	ZEGR_RELEASE(Context);
	
	DirtyFlags.RaiseAll();

	GraphicsState.Release();
	ComputeState.Release();
}

void ZED11Context::UpdateGraphicsState()
{
	if (DirtyFlags.GetFlags(ZEGR_CDF_BLEND_STATE))
	{
		Context->OMSetBlendState(GraphicsState->BlendState->GetInterface(), BlendFactors.M, BlendMask);
		DirtyFlags.UnraiseFlags(ZEGR_CDF_BLEND_STATE);
	}

	if (DirtyFlags.GetFlags(ZEGR_CDF_DEPTHSTENCIL_STATE))
	{
		Context->OMSetDepthStencilState(GraphicsState->DepthStencilState->GetInterface(), StencilRef);
		DirtyFlags.UnraiseFlags(ZEGR_CDF_DEPTHSTENCIL_STATE);
	}
}

ID3D11DeviceContext* ZED11Context::GetContext() const
{
	return Context;
}

ZED11Context::ZED11Context()
{
	Context = NULL;

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

		GraphicsState->Shaders[(ZEGRShaderType)I] = D11State->Shaders[(ZEGRShaderType)I];
	}

	if (D11State->BlendState != NULL && GraphicsState->BlendState != D11State->BlendState)
	{
		GraphicsState->BlendState = D11State->BlendState;
		DirtyFlags.RaiseFlags(ZEGR_CDF_BLEND_STATE);
	}

	if (D11State->RasterizerState != NULL && GraphicsState->RasterizerState != D11State->RasterizerState)
	{
		Context->RSSetState(D11State->RasterizerState->GetInterface());
		GraphicsState->RasterizerState = D11State->RasterizerState;
	}

	if (D11State->DepthStencilState != NULL && GraphicsState->DepthStencilState != D11State->DepthStencilState)
	{
		GraphicsState->DepthStencilState = D11State->DepthStencilState;
		DirtyFlags.RaiseFlags(ZEGR_CDF_DEPTHSTENCIL_STATE);
	}

	if (D11State->VertexLayout != NULL && GraphicsState->VertexLayout != D11State->VertexLayout)
	{
		Context->IASetInputLayout(D11State->VertexLayout->GetInterface());
		GraphicsState->VertexLayout = D11State->VertexLayout;
	}

	if (GraphicsState->PrimitiveTopology != D11State->PrimitiveTopology)
	{
		Context->IASetPrimitiveTopology(D11State->PrimitiveTopology);
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

	Context->CSSetShader(SrcShader->GetComputeShader(), NULL, 0);
	ComputeState->ComputeShader = D11State->ComputeShader;
}

void ZED11Context::SetVertexBuffers(ZEUInt Index, ZEUInt Count, const ZEGRVertexBuffer*const* Buffers)
{
	if (!CheckVertexBuffers(Index, Count, Buffers))
		return;

	UINT Strides[ZEGR_MAX_VERTEX_BUFFER_SLOT] = {0};
	UINT Offsets[ZEGR_MAX_VERTEX_BUFFER_SLOT] = {0};
	ID3D11Buffer* NativeBuffers[ZEGR_MAX_VERTEX_BUFFER_SLOT] = {NULL};

	for (ZEUInt I = 0; I < Count; I++)
	{
		ZEUInt Slot = Index + I;
		if (VertexBuffers[Slot] != NULL)
		{
			Strides[I] = static_cast<const ZED11VertexBuffer*>(VertexBuffers[Slot])->GetVertexStride();
			NativeBuffers[I] = static_cast<const ZED11VertexBuffer*>(VertexBuffers[Slot])->GetBuffer();
		}
	}

	Context->IASetVertexBuffers(Index, Count, NativeBuffers, Strides, Offsets);
}

void ZED11Context::SetIndexBuffer(const ZEGRIndexBuffer* Buffer)
{
	if (!CheckIndexBuffer(Buffer))
		return;

	ID3D11Buffer* NativeBuffer = NULL;
	DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;
	
	if (IndexBuffer != NULL)
	{
		Format = ConvertIndexBufferFormat(Buffer->GetFormat());
		NativeBuffer = static_cast<const ZED11IndexBuffer*>(Buffer)->GetBuffer();
	}

	Context->IASetIndexBuffer(NativeBuffer, Format, 0);
}

void ZED11Context::SetConstantBuffers(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRConstantBuffer*const* Buffers)
{
	if (!CheckConstantBuffers(Shader, Index, Count, Buffers))
		return;

	ID3D11Buffer* NativeBuffers[ZEGR_MAX_CONSTANT_BUFFER_SLOT] = {NULL};
	ZEUInt ShaderIndex = (Shader == ZEGR_ST_ALL) ? 0 : Shader;

	for (ZEUInt I = 0; I < Count; I++)
	{
		ZEUInt Slot = Index + I;

		if (ConstantBuffers[ShaderIndex][Slot] != NULL)
			NativeBuffers[I] = static_cast<const ZED11ConstantBuffer*>(ConstantBuffers[ShaderIndex][Slot])->GetBuffer();
	}

	switch (Shader)
	{
		case ZEGR_ST_VERTEX:
			Context->VSSetConstantBuffers(Index, Count, NativeBuffers);
			break;

		case ZEGR_ST_PIXEL:
			Context->PSSetConstantBuffers(Index, Count, NativeBuffers);
			break;

		case ZEGR_ST_GEOMETRY:
			Context->GSSetConstantBuffers(Index, Count, NativeBuffers);
			break;

		case ZEGR_ST_DOMAIN:
			Context->DSSetConstantBuffers(Index, Count, NativeBuffers);
			break;

		case ZEGR_ST_HULL:
			Context->HSSetConstantBuffers(Index, Count, NativeBuffers);
			break;

		case ZEGR_ST_COMPUTE:
			Context->CSSetConstantBuffers(Index, Count, NativeBuffers);
			break;

		case ZEGR_ST_ALL:
			Context->VSSetConstantBuffers(Index, Count, NativeBuffers);
			Context->PSSetConstantBuffers(Index, Count, NativeBuffers);
			Context->GSSetConstantBuffers(Index, Count, NativeBuffers);
			Context->DSSetConstantBuffers(Index, Count, NativeBuffers);
			Context->HSSetConstantBuffers(Index, Count, NativeBuffers);
			Context->CSSetConstantBuffers(Index, Count, NativeBuffers);
			break;

		default:
			break;
	}
}

void ZED11Context::SetStructuredBuffers(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRStructuredBuffer*const* Buffers)
{
	if (!CheckShaderResources(Shader, Index, Count, reinterpret_cast<const ZEGRResource*const*>(Buffers)))
		return;

	ID3D11ShaderResourceView* NativeShaderResources[ZEGR_MAX_TEXTURE_SLOT] = {NULL};
	ZEUInt ShaderIndex = (Shader == ZEGR_ST_ALL) ? 0 : Shader;

	for (ZEUInt I = 0; I < Count; I++)
	{
		ZEUInt Slot = Index + I;

		const ZEGRResource* Resource = ShaderResources[ShaderIndex][Slot];
		if (Resource == NULL)
			continue;

		switch (Resource->GetResourceType())
		{
			case ZEGR_RT_STRUCTURED_BUFFER:
				{
					NativeShaderResources[I] = static_cast<const ZED11StructuredBuffer*>(Resource)->GetShaderResourceView();
				}
				break;

			case ZEGR_RT_TEXTURE:
				{
					const ZEGRTexture* Texture = static_cast<const ZEGRTexture*>(Resource);
					switch (Texture->GetTextureType())
					{
						case ZEGR_TT_2D:
							NativeShaderResources[I] = static_cast<const ZED11Texture2D*>(Texture)->GetShaderResourceView();
							break;

						case ZEGR_TT_3D:
							NativeShaderResources[I] = static_cast<const ZED11Texture3D*>(Texture)->GetShaderResourceView();
							break;

						case ZEGR_TT_CUBE:
							NativeShaderResources[I] = static_cast<const ZED11TextureCube*>(Texture)->GetShaderResourceView();
							break;

						default:
							break;
					}
				}
				break;

			default:
				break;
		}
	}

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
}

void ZED11Context::SetTextures(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRTexture*const* Textures)
{
	if (!CheckShaderResources(Shader, Index, Count, reinterpret_cast<const ZEGRResource*const*>(Textures)))
		return;

	ID3D11ShaderResourceView* NativeShaderResources[ZEGR_MAX_TEXTURE_SLOT] = {NULL};
	ZEUInt ShaderIndex = (Shader == ZEGR_ST_ALL) ? 0 : Shader;

	for (ZEUInt I = 0; I < Count; I++)
	{
		ZEUInt Slot = Index + I;

		const ZEGRResource* Resource = static_cast<const ZEGRTexture*>(ShaderResources[ShaderIndex][Slot]);
		if (Resource == NULL)
			continue;

		switch (Resource->GetResourceType())
		{
			case ZEGR_RT_STRUCTURED_BUFFER:
				{
					NativeShaderResources[I] = static_cast<const ZED11StructuredBuffer*>(Resource)->GetShaderResourceView();
				}
				break;

			case ZEGR_RT_TEXTURE:
				{
					const ZEGRTexture* Texture = static_cast<const ZEGRTexture*>(Resource);
					switch (Texture->GetTextureType())
					{
						case ZEGR_TT_2D:
							NativeShaderResources[I] = static_cast<const ZED11Texture2D*>(Texture)->GetShaderResourceView();
							break;

						case ZEGR_TT_3D:
							NativeShaderResources[I] = static_cast<const ZED11Texture3D*>(Texture)->GetShaderResourceView();
							break;

						case ZEGR_TT_CUBE:
							NativeShaderResources[I] = static_cast<const ZED11TextureCube*>(Texture)->GetShaderResourceView();
							break;

						default:
							break;
					}
				}
				break;

			default:
				break;
		}

	}

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
}

void ZED11Context::SetRenderTargets(ZEUInt Count, const ZEGRRenderTarget*const* RenderTargets, const ZEGRDepthStencilBuffer* DepthStencilBuffer)
{
	if (!CheckRenderTargetsAndDepthStencilBuffer(Count, RenderTargets, DepthStencilBuffer))
		return;

	ID3D11RenderTargetView* NativeRenderTargets[ZEGR_MAX_RENDER_TARGET_SLOT] = {NULL};
	ID3D11DepthStencilView*	NativeDepthStencil = NULL;

	for (ZEUInt I = 0; I < Count; I++)
	{
		if (this->RenderTargets[I] != NULL)
			NativeRenderTargets[I] = static_cast<const ZED11RenderTarget*>(this->RenderTargets[I])->GetView();
	}

	if (this->DepthStencilBuffer != NULL)
		NativeDepthStencil = static_cast<const ZED11DepthStencilBuffer*>(this->DepthStencilBuffer)->GetView();
	
	Context->OMSetRenderTargets(Count, (Count == 0) ? NULL : NativeRenderTargets, NativeDepthStencil);
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
	
	Context->RSSetScissorRects(Count, NativeRects);
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
	
	Context->RSSetViewports(Count, NativeViewports);
}

void ZED11Context::SetStencilRef(ZEUInt Reference)
{
	if (StencilRef == Reference)
		return;

	StencilRef = Reference;
	DirtyFlags.RaiseBit(ZEGR_CDF_DEPTHSTENCIL_STATE);
}

void ZED11Context::SetBlendFactors(const ZEVector4& Factors)
{
	if (BlendFactors == Factors)
		return;

	BlendFactors = Factors;
	DirtyFlags.RaiseBit(ZEGR_CDF_BLEND_STATE);
}

void ZED11Context::SetBlendMask(ZEUInt Mask)
{
	if (BlendMask == Mask)
		return;

	BlendMask = Mask;
	DirtyFlags.RaiseBit(ZEGR_CDF_BLEND_STATE);
}

void ZED11Context::SetRWStructuredBuffers(ZEUInt Index, ZEUInt Count, const ZEGRStructuredBuffer*const* Buffers)
{
	if (!CheckUnorderedAccesses(Index, Count, reinterpret_cast<const ZEGRResource*const*>(Buffers)))
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
				case ZEGR_RT_STRUCTURED_BUFFER:
					{
						NativeUnorderedAccesses[I] = static_cast<const ZED11StructuredBuffer*>(Resource)->GetUnorderedAccessView();
					}
					break;

				case ZEGR_RT_TEXTURE:
					{
						const ZEGRTexture* Texture = static_cast<const ZEGRTexture*>(Resource);
						switch (Texture->GetTextureType())
						{
							case ZEGR_TT_2D:
								NativeUnorderedAccesses[I] = static_cast<const ZED11Texture2D*>(Texture)->GetUnorderedAccessView();
								break;

							case ZEGR_TT_3D:
								NativeUnorderedAccesses[I] = static_cast<const ZED11Texture3D*>(Texture)->GetUnorderedAccessView();
								break;

							default:
								break;
						}
					}
					break;

				default:
					break;
			}
		}
	}
	
	Context->CSSetUnorderedAccessViews(Index, Count, NativeUnorderedAccesses, NULL);
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
				case ZEGR_RT_STRUCTURED_BUFFER:
					{
						NativeUnorderedAccesses[I] = static_cast<const ZED11StructuredBuffer*>(Resource)->GetUnorderedAccessView();
					}
					break;

				case ZEGR_RT_TEXTURE:
					{
						const ZEGRTexture* Texture = static_cast<const ZEGRTexture*>(Resource);
						switch (Texture->GetTextureType())
						{
							case ZEGR_TT_2D:
								NativeUnorderedAccesses[I] = static_cast<const ZED11Texture2D*>(Texture)->GetUnorderedAccessView();
								break;

							case ZEGR_TT_3D:
								NativeUnorderedAccesses[I] = static_cast<const ZED11Texture3D*>(Texture)->GetUnorderedAccessView();
								break;

							default:
								break;
						}
					}
					break;

				default:
					break;
			}
		}
	}

	Context->CSSetUnorderedAccessViews(Index, Count, NativeUnorderedAccesses, NULL);
}

void ZED11Context::CopyResource(ZEGRResource* DestResource, ZEGRResource* SrcResource)
{
	zeDebugCheck(DestResource == NULL || SrcResource == NULL, "Source or destination resource cannot be NULL");

	ID3D11Resource* DestNativeResource = NULL;
	ID3D11Resource* SrcNativeResource = NULL;

	switch (DestResource->GetResourceType())
	{
		case ZEGR_RT_TEXTURE:
		{
			ZEGRTexture* Texture = static_cast<ZEGRTexture*>(DestResource);
			if (Texture->GetTextureType() == ZEGR_TT_2D)
				DestNativeResource = static_cast<ZED11Texture2D*>(Texture)->GetResource();
		}
		break;

		default:
			return;
	}

	switch (SrcResource->GetResourceType())
	{
		case ZEGR_RT_TEXTURE:
		{
			ZEGRTexture* Texture = static_cast<ZEGRTexture*>(SrcResource);
			if (Texture->GetTextureType() == ZEGR_TT_2D)
				SrcNativeResource = static_cast<ZED11Texture2D*>(Texture)->GetResource();
		}
		break;

		default:
			return;
	}

	GetMainContext()->CopyResource(DestNativeResource, SrcNativeResource);
}

void ZED11Context::Draw(ZEUInt VertexCount, ZEUInt FirstVertex)
{
	UpdateGraphicsState();
	GetMainContext()->Draw(VertexCount, FirstVertex);
}

void ZED11Context::DrawInstanced(ZEUInt VertexCount, ZEUInt FirstVertex, ZEUInt InstanceCount, ZEUInt FirstInstance)
{
	UpdateGraphicsState();
	GetMainContext()->DrawInstanced(VertexCount, InstanceCount, FirstVertex, FirstInstance);
}

void ZED11Context::Dispatch(ZEUInt GroupCountX, ZEUInt GroupCountY, ZEUInt GroupCountZ)
{
	GetMainContext()->Dispatch(GroupCountX, GroupCountY, GroupCountZ);
}

void ZED11Context::GenerateMipMaps(const ZEGRTexture* Texture)
{
	ID3D11ShaderResourceView* NativeShaderResource = NULL;

	switch (Texture->GetTextureType())
	{
		case ZEGR_TT_2D:
			NativeShaderResource = static_cast<const ZED11Texture2D*>(Texture)->GetShaderResourceView();
			break;

		case ZEGR_TT_3D:
			NativeShaderResource = static_cast<const ZED11Texture3D*>(Texture)->GetShaderResourceView();
			break;

		case ZEGR_TT_CUBE:
			NativeShaderResource = static_cast<const ZED11TextureCube*>(Texture)->GetShaderResourceView();
			break;

		default:
			break;
	}

	if (NativeShaderResource != NULL)
		GetMainContext()->GenerateMips(NativeShaderResource);
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
	{
		if(Texture->GetTextureType() == ZEGR_TT_3D)
			NativeView = static_cast<const ZED11Texture3D*>(Texture)->GetUnorderedAccessView();

		else if (Texture->GetTextureType() == ZEGR_TT_2D)
			NativeView = static_cast<const ZED11Texture2D*>(Texture)->GetUnorderedAccessView();
	}

	GetMainContext()->ClearUnorderedAccessViewFloat(NativeView, ClearColor.M);
}
