//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11StatePool.cpp
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

#include "ZED11StatePool.h"

#include "ZETypes.h"
#include "ZEDS/ZEListIterators.h"

#include "ZEGraphics/ZEGRDefinitions.h"
#include "ZEGraphics/ZEGRBlendState.h"
#include "ZEGraphics/ZEGRVertexLayout.h"
#include "ZEGraphics/ZEGRRasterizerState.h"
#include "ZEGraphics/ZEGRDepthStencilState.h"
#include "ZEGraphics/ZEGRVertexLayout.h"
#include "ZEGraphics/ZEGRShader.h"

#include "ZED11Shader.h"
#include "ZED11Module.h"
#include "ZED11Context.h"
#include "ZED11RenderStateData.h"

static D3D11_BLEND Convert(ZEGRBlend BlendOption)
{
	switch(BlendOption)
	{
		default:
			zeDebugCheck(true, "Wrong ZEGRBlendOptionc value.");

		case ZEGR_BO_ZERO:
			return D3D11_BLEND_ZERO;

		case ZEGR_BO_ONE:
			return D3D11_BLEND_ONE;

		case ZEGR_BO_SRC_COLOR:
			return D3D11_BLEND_SRC_COLOR;

		case ZEGR_BO_INV_SRC_COLOR:
			return D3D11_BLEND_INV_SRC_COLOR;

		case ZEGR_BO_SRC_ALPHA:
			return D3D11_BLEND_SRC_ALPHA;

		case ZEGR_BO_INV_SRC_ALPHA:
			return D3D11_BLEND_INV_SRC_ALPHA;

		case ZEGR_BO_DEST_ALPHA:
			return D3D11_BLEND_DEST_ALPHA;

		case ZEGR_BO_INV_DEST_ALPHA:
			return D3D11_BLEND_INV_DEST_ALPHA;

		case ZEGR_BO_DEST_COLOR:
			return D3D11_BLEND_DEST_COLOR;

		case ZEGR_BO_INV_DEST_COLOR:
			return D3D11_BLEND_INV_DEST_COLOR;

		case ZEGR_BO_SRC_ALPHA_SAT:
			return D3D11_BLEND_SRC_ALPHA_SAT;

		case ZEGR_BO_BLEND_FACTOR:
			return D3D11_BLEND_BLEND_FACTOR;

		case ZEGR_BO_INV_BLEND_FACTOR:
			return D3D11_BLEND_INV_BLEND_FACTOR;

		case ZEGR_BO_SRC1_COLOR:
			return D3D11_BLEND_SRC1_COLOR;

		case ZEGR_BO_INV_SRC1_COLOR:
			return D3D11_BLEND_INV_SRC1_COLOR;

		case ZEGR_BO_SRC1_ALPHA:
			return D3D11_BLEND_SRC1_ALPHA;

		case ZEGR_BO_INV_SRC1_ALPHA:
			return D3D11_BLEND_INV_SRC1_ALPHA;
	};
}

static D3D11_BLEND_OP Convert(ZEGRBlendOperation BlendEquation)
{
	switch (BlendEquation)
	{
		default:
			zeDebugCheck(true, "Wrong ZEGRBlendEquation value.");

		case ZEGR_BE_ADD:
			return D3D11_BLEND_OP_ADD;

		case ZEGR_BE_SUBTRACT:
			return D3D11_BLEND_OP_SUBTRACT;

		case ZEGR_BE_REV_SUBTRACT:
			return D3D11_BLEND_OP_REV_SUBTRACT;

		case ZEGR_BE_MIN:
			return D3D11_BLEND_OP_MIN;

		case ZEGR_BE_MAX:
			return D3D11_BLEND_OP_MAX;
	}
}

static D3D11_FILL_MODE Convert(ZEGRFillMode FillMode)
{
	switch (FillMode)
	{
		default:
			zeDebugCheck(true, "Wrong ZEGRFillMode value.");

		case ZEGR_FM_WIREFRAME:
			return D3D11_FILL_WIREFRAME;

		case ZEGR_FM_SOLID:
			return D3D11_FILL_SOLID;
	}

}

static D3D11_CULL_MODE Convert(ZEGRCullMode CullDirection)
{
	switch (CullDirection)
	{
		default:
			zeDebugCheck(true, "Wrong ZEGRCullMode value.");

		case ZEGR_CMD_NONE:
			return D3D11_CULL_NONE;

		case ZEGR_CMD_FRONT:
			return D3D11_CULL_FRONT;

		case ZEGR_CMD_BACK:
			return D3D11_CULL_BACK;
	}
}

static D3D11_STENCIL_OP Convert(ZEGRStencilOperation StencilOperation)
{
	switch (StencilOperation)
	{
		default:
			zeDebugCheck(true, "Wrong ZEGRStencilOperation value.");

		case ZEGR_SO_KEEP:
			return D3D11_STENCIL_OP_KEEP;

		case ZEGR_SO_ZERO:
			return D3D11_STENCIL_OP_ZERO;

		case ZEGR_SO_REPLACE:
			return D3D11_STENCIL_OP_REPLACE;

		case ZEGR_SO_INCREASE_SATURATE:
			return D3D11_STENCIL_OP_INCR_SAT;

		case ZEGR_SO_DECREASE_SATURATE:
			return D3D11_STENCIL_OP_DECR_SAT;

		case ZEGR_SO_INVERT:
			return D3D11_STENCIL_OP_INVERT;

		case ZEGR_SO_INCREASE:
			return D3D11_STENCIL_OP_INCR;

		case ZEGR_SO_DECREASE:
			return D3D11_STENCIL_OP_DECR;
	}
}

static D3D11_COMPARISON_FUNC Convert(ZEGRComparisonFunction CompFunc)
{
	switch (CompFunc)
	{
	default:
		zeDebugCheck(true, "Wrong ZEGRComparisonFunction value.");

	case ZEGR_CF_NEVER:
		return D3D11_COMPARISON_NEVER;

	case ZEGR_CF_LESS:
		return D3D11_COMPARISON_LESS;

	case ZEGR_CF_EQUAL:
		return D3D11_COMPARISON_EQUAL;

	case ZEGR_CF_LESS_EQUAL:
		return D3D11_COMPARISON_LESS_EQUAL;

	case ZEGR_CF_GREATER:
		return D3D11_COMPARISON_GREATER;

	case ZEGR_CF_NOT_EQUAL:
		return D3D11_COMPARISON_NOT_EQUAL;

	case ZEGR_CF_GREATER_EQUAL:
		return D3D11_COMPARISON_GREATER_EQUAL;

	case ZEGR_CF_ALWAYS:
		return D3D11_COMPARISON_ALWAYS;
	}
}

static DXGI_FORMAT Convert(ZEGRVertexElementType ElementType)
{
	switch(ElementType)
	{ 
		default:
			zeDebugCheck(true, "Wrong ZEGRVertexElementType value.");

		case ZEGR_VET_NONE:
			return DXGI_FORMAT_UNKNOWN;

		case ZEGR_VET_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;

		case ZEGR_VET_FLOAT_2:
			return DXGI_FORMAT_R32G32_FLOAT;

		case ZEGR_VET_FLOAT_3:
			return DXGI_FORMAT_R32G32B32_FLOAT;

		case ZEGR_VET_FLOAT_4:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;


		case ZEGR_VET_INT8:
			return DXGI_FORMAT_R8_SINT;

		case ZEGR_VET_INT8_NORM:
			return DXGI_FORMAT_R8_SNORM;

		case ZEGR_VET_INT8_2:
			return DXGI_FORMAT_R8G8_SINT;

		case ZEGR_VET_INT8_2_NORM:
			return DXGI_FORMAT_R8G8_SNORM;

		case ZEGR_VET_INT8_4:
			return DXGI_FORMAT_R8G8B8A8_SINT;

		case ZEGR_VET_INT8_4_NORM:
			return DXGI_FORMAT_R8G8B8A8_SNORM;
			

		case ZEGR_VET_UINT8:
			return DXGI_FORMAT_R8_UINT;

		case ZEGR_VET_UINT8_NORM:
			return DXGI_FORMAT_R8_UNORM;

		case ZEGR_VET_UINT8_2:
			return DXGI_FORMAT_R8G8_UINT;

		case ZEGR_VET_UINT8_2_NORM:
			return DXGI_FORMAT_R8G8_UNORM;

		case ZEGR_VET_UINT8_4:
			return DXGI_FORMAT_R8G8B8A8_UINT;

		case ZEGR_VET_UINT8_4_NORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;


		case ZEGR_VET_INT16:
			return DXGI_FORMAT_R16_SINT;

		case ZEGR_VET_INT16_NORM:
			return DXGI_FORMAT_R16_SNORM;

		case ZEGR_VET_INT16_2:
			return DXGI_FORMAT_R16G16_SINT;

		case ZEGR_VET_INT16_2_NORM:
			return DXGI_FORMAT_R16G16_SNORM;

		case ZEGR_VET_INT16_4:
			return DXGI_FORMAT_R16G16B16A16_SINT;

		case ZEGR_VET_INT16_4_NORM:
			return DXGI_FORMAT_R16G16B16A16_SNORM;


		case ZEGR_VET_UINT16:
			return DXGI_FORMAT_R16_UINT;

		case ZEGR_VET_UINT16_NORM:
			return DXGI_FORMAT_R16_UNORM;

		case ZEGR_VET_UINT16_2:
			return DXGI_FORMAT_R16G16_UINT;

		case ZEGR_VET_UINT16_2_NORM:
			return DXGI_FORMAT_R16G16_UNORM;

		case ZEGR_VET_UINT16_4:
			return DXGI_FORMAT_R16G16B16A16_UINT;

		case ZEGR_VET_UINT16_4_NORM:
			return DXGI_FORMAT_R16G16B16A16_UNORM;

		
		case ZEGR_VET_INT32:
			return DXGI_FORMAT_R32_SINT;

		case ZEGR_VET_INT32_2:
			return DXGI_FORMAT_R32G32_SINT;

		case ZEGR_VET_INT32_3:
			return DXGI_FORMAT_R32G32B32_SINT;

		case ZEGR_VET_INT32_4:
			return DXGI_FORMAT_R32G32B32A32_SINT;


		case ZEGR_VET_UINT32:
			return DXGI_FORMAT_R32_UINT;

		case ZEGR_VET_UINT32_2:
			return DXGI_FORMAT_R32G32_UINT;

		case ZEGR_VET_UINT32_3:
			return DXGI_FORMAT_R32G32B32_UINT;

		case ZEGR_VET_UINT32_4:
			return DXGI_FORMAT_R32G32B32A32_UINT;
	};
}

static D3D11_INPUT_CLASSIFICATION Convert(ZEGRVertexUsage Usage)
{
	switch (Usage)
	{
		default:
			zeDebugCheck(true, "Wrong ZEGRVertexUsage value.");

		case ZEGR_VU_PER_VERTEX:
			return D3D11_INPUT_PER_VERTEX_DATA;

		case ZEGR_VU_PER_INSTANCE:
			return D3D11_INPUT_PER_INSTANCE_DATA;
	}
}

ID3D11BlendState* ZED11StatePool::CreateBlendState(const ZEGRBlendState& BlendState) const
{
	if (BlendStatePool.GetCount() >= ZEGR_STATE_POOL_CACHE_CAPACITY)
	{
		zeError("Max blend state count reached.");
		return NULL;
	}
	
	D3D11_BLEND_DESC BlendDesc;
	memset(&BlendDesc, 0, sizeof(D3D11_BLEND_DESC));

	BlendDesc.AlphaToCoverageEnable = BlendState.GetAlphaToCoverageEnable();

	if (!BlendState.GetBlendEnable())
	{
		BlendDesc.IndependentBlendEnable = FALSE;
		BlendDesc.RenderTarget[0].BlendEnable = FALSE;
		BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;

		BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

		BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	else
	{
		BlendDesc.IndependentBlendEnable = BlendState.GetIndividualBlendEnable();

		ZESize RenderTargetCount = (!BlendState.GetIndividualBlendEnable() ? 1 : ZEGR_MAX_RENDER_TARGET_SLOT);
		for (ZESize I = 0; I < RenderTargetCount; I++)
		{
			const ZEGRBlendRenderTarget& Current = BlendState.GetRenderTarget((ZEUInt)I);

			BlendDesc.RenderTarget[I].BlendEnable = Current.GetBlendEnable();

			BlendDesc.RenderTarget[I].BlendOp = Convert(Current.GetOperation());
			BlendDesc.RenderTarget[I].SrcBlend = Convert(Current.GetSource());
			BlendDesc.RenderTarget[I].DestBlend = Convert(Current.GetDestination());

			BlendDesc.RenderTarget[I].BlendOpAlpha = Convert(Current.GetAlphaOperation());
			BlendDesc.RenderTarget[I].SrcBlendAlpha = Convert(Current.GetSourceAlpha());
			BlendDesc.RenderTarget[I].DestBlendAlpha = Convert(Current.GetDestinationAlpha());

			BlendDesc.RenderTarget[I].RenderTargetWriteMask = Current.GetWriteMask().Value;
		}
	}

	ID3D11BlendState* NativeState = NULL;
	HRESULT Result = GetDevice()->CreateBlendState(&BlendDesc, &NativeState);
	if (FAILED(Result))
	{
		zeError("Blend state creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	return NativeState;
}

ID3D11RasterizerState* ZED11StatePool::CreateRasterizerState(const ZEGRRasterizerState& RasterizerState) const
{
	if (RasterizerStatePool.GetCount() >= ZEGR_STATE_POOL_CACHE_CAPACITY)
	{
		zeError("Max rasterizer state count reached.");
		return NULL;
	}

	D3D11_RASTERIZER_DESC RasterizerDesc;
	memset(&RasterizerDesc, 0, sizeof(D3D11_RASTERIZER_DESC));

	RasterizerDesc.DepthBias = RasterizerState.GetDepthBias();
	RasterizerDesc.DepthBiasClamp = RasterizerState.GetDepthBiasClamp();
	RasterizerDesc.SlopeScaledDepthBias = RasterizerState.GetSlopeScaledDepthBias();
	RasterizerDesc.ScissorEnable = false;
	RasterizerDesc.DepthClipEnable = RasterizerState.GetDepthClipEnable();
	RasterizerDesc.MultisampleEnable = false;
	RasterizerDesc.AntialiasedLineEnable = false;
	RasterizerDesc.FillMode = Convert(RasterizerState.GetFillMode());
	RasterizerDesc.CullMode = Convert(RasterizerState.GetCullMode());
	RasterizerDesc.FrontCounterClockwise = RasterizerState.GetFrontIsCounterClockwise();

	ID3D11RasterizerState* NativeState = NULL;
	HRESULT Result = GetDevice()->CreateRasterizerState(&RasterizerDesc, &NativeState);
	if (FAILED(Result))
	{
		zeError("Rasterizer state creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Rasterizer state created. Hash: %Iu.", (ZEUInt64)RasterizerState->GetHash());
	#endif

	return NativeState;
}

ID3D11DepthStencilState* ZED11StatePool::CreateDepthStencilState(const ZEGRDepthStencilState& DepthStencilState) const
{
	if (DepthStencilStatePool.GetCount() >= ZEGR_STATE_POOL_CACHE_CAPACITY)
	{
		zeError("Max depth stencil state count reached.");
		return NULL;
	}
	
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	memset(&DepthStencilDesc, 0, sizeof(D3D11_DEPTH_STENCIL_DESC));

	DepthStencilDesc.DepthEnable = DepthStencilState.GetDepthTestEnable();
	DepthStencilDesc.DepthFunc = Convert(DepthStencilState.GetDepthFunction());
	DepthStencilDesc.DepthWriteMask = DepthStencilState.GetDepthWriteEnable() ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	DepthStencilDesc.StencilEnable = DepthStencilState.GetStencilTestEnable();
	DepthStencilDesc.StencilReadMask = DepthStencilState.GetStencilReadMask();
	DepthStencilDesc.StencilWriteMask = DepthStencilState.GetStencilWriteMask();
	DepthStencilDesc.BackFace.StencilPassOp = Convert(DepthStencilState.GetBackStencilPass());
	DepthStencilDesc.BackFace.StencilFailOp = Convert(DepthStencilState.GetBackStencilFail());
	DepthStencilDesc.BackFace.StencilDepthFailOp = Convert(DepthStencilState.GetBackZFail());
	DepthStencilDesc.BackFace.StencilFunc = Convert(DepthStencilState.GetBackStencilFunction());
	DepthStencilDesc.FrontFace.StencilPassOp = Convert(DepthStencilState.GetFrontStencilPass());
	DepthStencilDesc.FrontFace.StencilFailOp = Convert(DepthStencilState.GetFrontStencilFail());
	DepthStencilDesc.FrontFace.StencilDepthFailOp = Convert(DepthStencilState.GetFrontStencilDepthFail());
	DepthStencilDesc.FrontFace.StencilFunc = Convert(DepthStencilState.GetFrontStencilFunction());

	ID3D11DepthStencilState* NativeState = NULL;
	HRESULT Result = GetDevice()->CreateDepthStencilState(&DepthStencilDesc, &NativeState);
	if (FAILED(Result))
	{
		zeError("Depth stencil state creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Depth stencil state created. Hash: %Iu.", (ZEUInt64)DepthStencilState->GetHash());
	#endif

	return NativeState;
}

ID3D11InputLayout* ZED11StatePool::CreateVertexLayout(const ZEGRVertexLayout& VertexLayout, const ZEGRShader* Shader) const
{
	if (Shader == NULL || Shader->GetShaderType() != ZEGR_ST_VERTEX)
	{
		zeError("Vertex Layout creation failed. Wrong shader type or empty vertex shader.");
		return false;
	}

	if (VertexLayoutPool.GetCount() >= ZEGR_STATE_POOL_CACHE_CAPACITY)
	{
		zeError("Max input layout count reached.");
		return NULL;
	}

	ZEUInt ElementCount = VertexLayout.GetElementCount();
	const ZEGRVertexElement* Elements = VertexLayout.GetElements();

	D3D11_INPUT_ELEMENT_DESC ElementDesc[16];
	char* SemanticType = NULL;
	for (ZESize I = 0; I < ElementCount; I++)
	{
		switch(Elements[I].Semantic)
		{
			case ZEGR_VES_UNKNOWN:
				ElementDesc[I].SemanticName = "UNKNOWN";
				break;

			case ZEGR_VES_POSITION:
				ElementDesc[I].SemanticName = "POSITION";
				break;

			case ZEGR_VES_NORMAL:
				ElementDesc[I].SemanticName = "NORMAL";
				break;

			case ZEGR_VES_BINORMAL:
				ElementDesc[I].SemanticName = "BINORMAL";
				break;

			case ZEGR_VES_TANGENT:
				ElementDesc[I].SemanticName = "TANGENT";
				break;

			case ZEGR_VES_TEXCOORD:
				ElementDesc[I].SemanticName = "TEXCOORD";
				break;

			case ZEGR_VES_COLOR:
				ElementDesc[I].SemanticName = "COLOR";
				break;

			case ZEGR_VES_BLEND_INDICES:
				ElementDesc[I].SemanticName = "BLENDINDICES";
				break;

			case ZEGR_VES_BLEND_WEIGHTS:
				ElementDesc[I].SemanticName = "BLENDWEIGHTS";
				break;

			case ZEGR_VES_CUSTOM:
				ElementDesc[I].SemanticName = "CUSTOM";
				break;
		}

		ElementDesc[I].SemanticIndex = Elements[I].Index;
		ElementDesc[I].Format = Convert(Elements[I].Type);
		ElementDesc[I].InputSlot = Elements[I].Stream;
		ElementDesc[I].AlignedByteOffset = Elements[I].Offset;
		ElementDesc[I].InstanceDataStepRate = Elements[I].InstanceCount;
		ElementDesc[I].InputSlotClass = Convert(Elements[I].Usage);
	}

	const ZED11Shader* D11Shader = static_cast<const ZED11Shader*>(Shader);
	ID3D11InputLayout* NativeState = NULL;
	HRESULT Result = GetDevice()->CreateInputLayout(
		ElementDesc, ElementCount, 
		D11Shader->GetByteCode().GetConstCArray(), D11Shader->GetByteCode().GetCount(), 
		&NativeState);
	if (FAILED(Result))
	{
		zeError("Vertex layout creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Vertex layout created. Hash: %Iu.", (ZEUInt64)VertexLayout->GetHash());
	#endif

	return NativeState;
}

ZED11StateBase* ZED11StatePool::FindPoolEntry(ZEList2<ZED11StateBase>& Pool, const ZEGRState& State) const
{
	Pool.LockRead();
	ze_for_each(Entry, Pool)
	{
		if (Entry->GetStateBase().GetHash() == State.GetHash() && State.Equals(Entry->GetStateBase()))
		{
			Pool.UnlockRead();
			return &(*Entry);
		}
	}
	Pool.UnlockRead();

	return NULL;
}

ZED11StatePool::ZED11StatePool()
{

}

ZED11StatePool::~ZED11StatePool()
{
}

ZEHolder<const ZED11BlendState> ZED11StatePool::GetBlendState(const ZEGRBlendState& BlendState) const
{
	ZEHolder<ZED11BlendState> Entry = static_cast<ZED11BlendState*>(FindPoolEntry(BlendStatePool, BlendState));
	if (Entry == NULL)
	{
		Entry = new ZED11BlendState();
		Entry->Owner = this;
		Entry->State = BlendState;
		Entry->Interface = CreateBlendState(BlendState);

		BlendStatePool.AddEnd(Entry->Link);
	}

	return Entry;
}

ZEHolder<const ZED11RasterizerState> ZED11StatePool::GetRasterizerState(const ZEGRRasterizerState& RasterizerState) const
{
	ZED11RasterizerState* Entry = static_cast<ZED11RasterizerState*>(FindPoolEntry(RasterizerStatePool, RasterizerState));
	if (Entry == NULL)
	{
		Entry = new ZED11RasterizerState();
		Entry->Owner = this;
		Entry->State = RasterizerState;
		Entry->Interface = CreateRasterizerState(RasterizerState);

		RasterizerStatePool.AddEnd(Entry->Link);
	}
	
	return Entry;
}

ZEHolder<const ZED11DepthStencilState> ZED11StatePool::GetDepthStencilState(const ZEGRDepthStencilState& DepthStencilState) const
{
	ZED11DepthStencilState* Entry = static_cast<ZED11DepthStencilState*>(FindPoolEntry(DepthStencilStatePool, DepthStencilState));
	if (Entry == NULL)
	{
		Entry = new ZED11DepthStencilState();
		Entry->Owner = this;
		Entry->State = DepthStencilState;
		Entry->Interface = CreateDepthStencilState(DepthStencilState);

		DepthStencilStatePool.AddEnd(Entry->Link);
	}

	return Entry;
}

ZEHolder<const ZED11VertexLayout> ZED11StatePool::GetVertexLayout(const ZEGRVertexLayout& VertexLayout, const ZEGRShader* VertexShader) const
{
	zeCheckCriticalError(VertexShader == NULL, NULL, "Vertex shader cannot be NULL");
	zeCheckCriticalError(VertexShader->GetShaderType() != ZEGR_ST_VERTEX, NULL, "Wrong shader type.");

	ZED11VertexLayout* Entry = NULL;
	if (VertexLayout.GetElementCount() > 0)
	{
		Entry = static_cast<ZED11VertexLayout*>(FindPoolEntry(VertexLayoutPool, VertexLayout));
		if (Entry == NULL)
		{
			Entry = new ZED11VertexLayout();
			Entry->Owner = this;
			Entry->State = VertexLayout;
			Entry->Interface =  CreateVertexLayout(VertexLayout, VertexShader);

			VertexLayoutPool.AddEnd(Entry->Link);
		}
	}

	return Entry;
}
