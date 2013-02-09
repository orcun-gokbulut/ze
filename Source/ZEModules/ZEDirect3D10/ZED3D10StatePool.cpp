//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D10StatePool.cpp
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

#include "ZETypes.h"
#include "ZED3D10Shader.h"
#include "ZEGraphics/ZEShader.h"
#include "ZED3D10StatePool.h"
#include "ZEDS/ZEListIterators.h"
#include "ZED3D10GraphicsModule.h"
#include "ZEGraphics/ZEBlendState.h"
#include "ZEGraphics/ZEVertexLayout.h"
#include "ZEGraphics/ZESamplerState.h"
#include "ZEGraphics/ZERasterizerState.h"
#include "ZEGraphics/ZEDepthStencilState.h"

#include <d3d10.h>

inline D3D10_BLEND ZEBlendOptionToD3D10(ZEBlendOption BlendOption)
{
	static const D3D10_BLEND Values[] =
	{
		(D3D10_BLEND)-1,
		D3D10_BLEND_ZERO,				// ZE_BO_ZERO				= 1,
		D3D10_BLEND_ONE,				// ZE_BO_ONE				= 2,
		D3D10_BLEND_SRC_COLOR,			// ZE_BO_SRC_COLOR			= 3,
		D3D10_BLEND_INV_SRC_COLOR,		// ZE_BO_INV_SRC_COLOR		= 4,
		D3D10_BLEND_SRC_ALPHA,			// ZE_BO_SRC_ALPHA			= 5,
		D3D10_BLEND_INV_SRC_ALPHA,		// ZE_BO_INV_SRC_ALPHA		= 6,
		D3D10_BLEND_DEST_ALPHA,			// ZE_BO_DEST_ALPHA			= 7,
		D3D10_BLEND_INV_DEST_ALPHA,		// ZE_BO_INV_DEST_ALPHA		= 8,
		D3D10_BLEND_DEST_COLOR,			// ZE_BO_DEST_COLOR			= 9,
		D3D10_BLEND_INV_DEST_COLOR,		// ZE_BO_INV_DEST_COLOR		= 10,
		D3D10_BLEND_SRC_ALPHA_SAT,		// ZE_BO_SRC_ALPHA_SAT		= 11
		D3D10_BLEND_BLEND_FACTOR,		// ZE_BO_BLEND_FACTOR		= 12,
		D3D10_BLEND_INV_BLEND_FACTOR,	// ZE_BO_INV_BLEND_FACTOR	= 13,
		D3D10_BLEND_SRC1_COLOR,			// ZE_BO_SRC1_COLOR			= 14,
		D3D10_BLEND_INV_SRC1_COLOR,		// ZE_BO_INV_SRC1_COLOR		= 15,
		D3D10_BLEND_SRC1_ALPHA,			// ZE_BO_SRC1_ALPHA			= 16,
		D3D10_BLEND_INV_SRC1_ALPHA		// ZE_BO_INV_SRC1_ALPHA		= 17
	};

	return Values[BlendOption];
}

inline D3D10_BLEND_OP ZEBlendEquationToD3D10(ZEBlendEquation BlendEquation)
{
	static const D3D10_BLEND_OP Values[] =
	{
		(D3D10_BLEND_OP)-1,
		D3D10_BLEND_OP_ADD,				// ZE_BE_ADD			= 1,
		D3D10_BLEND_OP_SUBTRACT,		// ZE_BE_SUBTRACT		= 2,
		D3D10_BLEND_OP_REV_SUBTRACT,	// ZE_BE_REV_SUBTRACT	= 3,
		D3D10_BLEND_OP_MIN,				// ZE_BE_MIN			= 4,
		D3D10_BLEND_OP_MAX				// ZE_BE_MAX			= 5 
	};

	return Values[BlendEquation];
}

inline D3D10_COMPARISON_FUNC ZEComparisionFunctionToD3D10(ZEComparisonFunction CompFunc)
{
	static const D3D10_COMPARISON_FUNC Values[] =
	{
		(D3D10_COMPARISON_FUNC)0,			
		D3D10_COMPARISON_NEVER,				// ZE_CF_NEVER			= 1,
		D3D10_COMPARISON_LESS,				// ZE_CF_LESS			= 2,
		D3D10_COMPARISON_EQUAL, 			// ZE_CF_EQUAL			= 3,
		D3D10_COMPARISON_LESS_EQUAL, 		// ZE_CF_LESS_EQUAL		= 4,
		D3D10_COMPARISON_GREATER, 			// ZE_CF_GREATER		= 5,
		D3D10_COMPARISON_NOT_EQUAL, 		// ZE_CF_NOT_EQUAL		= 6,
		D3D10_COMPARISON_GREATER_EQUAL, 	// ZE_CF_GREATER_EQUAL	= 7,
		D3D10_COMPARISON_ALWAYS				// ZE_CF_ALWAYS			= 8 
	};

	return Values[CompFunc];
}

inline D3D10_TEXTURE_ADDRESS_MODE ZETextureAdressModeToD3D10(ZETextureAddressMode TextureAddress)
{
	static const D3D10_TEXTURE_ADDRESS_MODE Values[] =
	{
		(D3D10_TEXTURE_ADDRESS_MODE)-1,	
		D3D10_TEXTURE_ADDRESS_WRAP,		// ZE_TAM_WRAP		= 1,
		D3D10_TEXTURE_ADDRESS_MIRROR,	// ZE_TAM_MIRROR	= 2,
		D3D10_TEXTURE_ADDRESS_CLAMP,	// ZE_TAM_CLAMP		= 3,
		D3D10_TEXTURE_ADDRESS_BORDER	// ZE_TAM_BORDER	= 4
	};
	return Values[TextureAddress];
}

inline D3D10_FILTER_TYPE ZETextureFilterModeToD3D10FilterType(ZETextureFilterMode FilterMode)
{
	static const D3D10_FILTER_TYPE Values[] =
	{
		(D3D10_FILTER_TYPE)-1,		
		D3D10_FILTER_TYPE_POINT,	// ZE_TFM_POINT			= 1,
		D3D10_FILTER_TYPE_LINEAR	// ZE_TFM_LINEAR		= 2,
	};

	return Values[FilterMode];
}

inline D3D10_FILTER ZETextureFilterModeToD3D10(ZETextureFilterMode Min, ZETextureFilterMode Mag, ZETextureFilterMode Mip)
{
	// TODO: Try to avoid if check
	if (Min == ZE_TFM_ANISOTROPY || Mag == ZE_TFM_ANISOTROPY || Mip == ZE_TFM_ANISOTROPY)
		return D3D10_FILTER_ANISOTROPIC;

	return (D3D10_FILTER)(	(ZETextureFilterModeToD3D10FilterType(Min) << D3D10_MIN_FILTER_SHIFT) | 
							(ZETextureFilterModeToD3D10FilterType(Mag) << D3D10_MAG_FILTER_SHIFT) | 
							(ZETextureFilterModeToD3D10FilterType(Mip) << D3D10_MIP_FILTER_SHIFT));
}

inline D3D10_FILL_MODE ZEFillModeToD3D10(ZEFillMode FillMode)
{
	static const D3D10_FILL_MODE Values[] = 
	{
		(D3D10_FILL_MODE)-1,
		D3D10_FILL_WIREFRAME,	// ZE_FM_WIREFRAME	= 1,
		D3D10_FILL_SOLID,		// ZE_FM_SOLID		= 2
	};

	return Values[FillMode];
}

inline D3D10_CULL_MODE ZECullDirectionToD3D10(ZECullDirection CullDirection)
{
	static const D3D10_CULL_MODE Values[] = 
	{
		(D3D10_CULL_MODE)-1,
		D3D10_CULL_NONE,		// ZE_CD_NONE				= 1,
		D3D10_CULL_FRONT,		// ZE_CD_CLOCKWISE 			= 2,
		D3D10_CULL_BACK			// ZE_CD_COUNTER_CLOCKWISE	= 3
	};

	return Values[CullDirection];
}

inline D3D10_STENCIL_OP ZEStencilOperationToD3D10(ZEStencilOperation StencilOperation)
{
	static const D3D10_STENCIL_OP Values[] = 
	{ 
		(D3D10_STENCIL_OP)-1,
		D3D10_STENCIL_OP_KEEP,		//	ZE_SO_KEEP		= 1,
		D3D10_STENCIL_OP_ZERO, 		//	ZE_SO_ZERO		= 2,
		D3D10_STENCIL_OP_REPLACE,	//	ZE_SO_REPLACE	= 3,
		D3D10_STENCIL_OP_INCR_SAT, 	//	ZE_SO_INCR_SAT	= 4,
		D3D10_STENCIL_OP_DECR_SAT, 	//	ZE_SO_DECR_SAT	= 5,
		D3D10_STENCIL_OP_INVERT,	//	ZE_SO_INVERT	= 6,
		D3D10_STENCIL_OP_INCR, 		//	ZE_SO_INCR		= 7,
		D3D10_STENCIL_OP_DECR		//	ZE_SO_DECR		= 8 
	};

	return Values[StencilOperation];
}

inline static DXGI_FORMAT ZEVertexElementTypeToD3D10(ZEVertexElementType ElementType)
{
	static const DXGI_FORMAT Values[] = 
	{ 
		DXGI_FORMAT_UNKNOWN,				//	ZE_VET_NONE		= 0,
		DXGI_FORMAT_R32_SINT, 				//	ZE_VET_INT		= 1,
		DXGI_FORMAT_R32G32_SINT, 			//	ZE_VET_INT2		= 2,
		DXGI_FORMAT_R32G32B32_SINT,			//	ZE_VET_INT3		= 3,
		DXGI_FORMAT_R32G32B32A32_SINT, 		//	ZE_VET_INT4		= 4,
		DXGI_FORMAT_R32_UINT, 				//	ZE_VET_UINT		= 5,
		DXGI_FORMAT_R32G32_UINT,			//	ZE_VET_UINT2	= 6,
		DXGI_FORMAT_R32G32B32_UINT, 		//	ZE_VET_UINT3	= 7,
		DXGI_FORMAT_R32G32B32A32_UINT,		//	ZE_VET_UINT4	= 8,
		DXGI_FORMAT_R32_FLOAT,				//	ZE_VET_FLOAT	= 9,
		DXGI_FORMAT_R32G32_FLOAT,			//	ZE_VET_FLOAT2	= 10,
		DXGI_FORMAT_R32G32B32_FLOAT,		//	ZE_VET_FLOAT3	= 11,
		DXGI_FORMAT_R32G32B32A32_FLOAT		//	ZE_VET_FLOAT4	= 12
	};

	return Values[ElementType];
}

inline static D3D10_INPUT_CLASSIFICATION ZEVertexUsageToD3D10(ZEVertexUsage Usage)
{
	static const D3D10_INPUT_CLASSIFICATION Values[] =
	{
		D3D10_INPUT_PER_VERTEX_DATA,		//	ZE_VU_PER_VERTEX	= 0,
		D3D10_INPUT_PER_INSTANCE_DATA 		//	ZE_VU_PER_INSTANCE	= 1,
	};

	return Values[Usage];
}

ID3D10BlendState* ZED3D10StatePool::CreateD3D10BlendState(const ZEBlendState* BlendState)
{
	if (BlendStatePool.GetCount() >= ZE_D3D10_STATE_CACHE_CAPACITY)
	{
		zeError("Max blend state count reached.");
		return NULL;
	}

	D3D10_BLEND_DESC BlendDesc;
	BlendDesc.AlphaToCoverageEnable = BlendState->GetAlphaToCoverageEnable();
	BlendDesc.SrcBlend = ZEBlendOptionToD3D10(BlendState->GetSourceBlendOption());
	BlendDesc.DestBlend = ZEBlendOptionToD3D10(BlendState->GetDestinationBlendOption());
	BlendDesc.BlendOp = ZEBlendEquationToD3D10(BlendState->GetBlendEquation());
	BlendDesc.SrcBlendAlpha = ZEBlendOptionToD3D10(BlendState->GetSourceBlendAlphaOption());
	BlendDesc.DestBlendAlpha = ZEBlendOptionToD3D10(BlendState->GetDestinationBlendAlphaOption());
	BlendDesc.BlendOpAlpha = ZEBlendEquationToD3D10(BlendState->GetBlendAlphaEquation());

	for (ZESize I = 0; I < ZE_MAX_RENDER_TARGET_SLOT; ++I)
	{
		BlendDesc.BlendEnable[I] = BlendState->GetBlendEnable(I);
		BlendDesc.RenderTargetWriteMask[I] = BlendState->GetColorChannelMask(I);
	}

	ID3D10BlendState* D3D10State = NULL;
	if (FAILED(D3D10Device->CreateBlendState(&BlendDesc, &D3D10State)))
	{
		zeError("cannot create depth stencil state");
		return NULL;
	}

	BlendStatePool.Append(new ZEBlendStateEntry(D3D10State, BlendState->Hash, 1));
	
	return D3D10State;
}

ID3D10SamplerState* ZED3D10StatePool::CreateD3D10SamplerState(const ZESamplerState* SamplerState)
{
	if (SamplerStatePool.GetCount() >= ZE_D3D10_STATE_CACHE_CAPACITY)
	{
		zeError("Max sampler state count reached.");
		return NULL;
	}

	ZEVector4* BorderColor = &SamplerState->GetBorderColor();

	D3D10_SAMPLER_DESC SamplerDesc;
	SamplerDesc.BorderColor[0] = BorderColor->x;
	SamplerDesc.BorderColor[1] = BorderColor->y;
	SamplerDesc.BorderColor[2] = BorderColor->z;
	SamplerDesc.BorderColor[3] = BorderColor->w;
	SamplerDesc.ComparisonFunc = D3D10_COMPARISON_NEVER;
	SamplerDesc.MinLOD = SamplerState->GetMinLOD();
	SamplerDesc.MaxLOD = SamplerState->GetMaxLOD();
	SamplerDesc.MipLODBias = SamplerState->GetMipLODBias();
	SamplerDesc.MaxAnisotropy = SamplerState->GetMaxAnisotrophy();
	SamplerDesc.AddressU = ZETextureAdressModeToD3D10(SamplerState->GetAddressU());
	SamplerDesc.AddressV = ZETextureAdressModeToD3D10(SamplerState->GetAddressV());
	SamplerDesc.AddressW = ZETextureAdressModeToD3D10(SamplerState->GetAddressW());
	SamplerDesc.Filter = ZETextureFilterModeToD3D10(SamplerState->GetMinFilter(), SamplerState->GetMagFilter(), SamplerState->GetMipFilter());
	
	ID3D10SamplerState* D3D10State = NULL;
	if (FAILED(D3D10Device->CreateSamplerState(&SamplerDesc, &D3D10State)))
	{
		zeError("Cannot create depth stencil state");
		return NULL;
	}

	SamplerStatePool.Append(new ZESamplerStateEntry(D3D10State, SamplerState->Hash, 1));

	return D3D10State;
}

ID3D10RasterizerState* ZED3D10StatePool::CreateD3D10RasterizerState(const ZERasterizerState* RasterizerState)
{
	if (RasterizerStatePool.GetCount() >= ZE_D3D10_STATE_CACHE_CAPACITY)
	{
		zeError("Max rasterizer state count reached.");
		return NULL;
	}

	D3D10_RASTERIZER_DESC RasterizerDesc;
	RasterizerDesc.FillMode = ZEFillModeToD3D10(RasterizerState->GetFillMode());
	RasterizerDesc.CullMode = ZECullDirectionToD3D10(RasterizerState->GetCullDirection());
	RasterizerDesc.FrontCounterClockwise = RasterizerState->GetFrontIsCounterClockwise();
	RasterizerDesc.ScissorEnable = false;
	RasterizerDesc.DepthClipEnable = true;
	RasterizerDesc.MultisampleEnable = false;
	RasterizerDesc.AntialiasedLineEnable = false;
	RasterizerDesc.DepthBias = 0;
	RasterizerDesc.DepthBiasClamp = 0.0f;
	RasterizerDesc.SlopeScaledDepthBias = 0.0f;

	ID3D10RasterizerState* D3D10State = NULL;
	if (FAILED(D3D10Device->CreateRasterizerState(&RasterizerDesc, &D3D10State)))
	{
		zeError("Cannot create depth stencil state");
		return NULL;
	}

	RasterizerStatePool.Append(new ZERasterizerStateEntry(D3D10State, RasterizerState->Hash, 1));

	return D3D10State;
}

ID3D10DepthStencilState* ZED3D10StatePool::CreateD3D10DepthStencilState(const ZEDepthStencilState* DepthStencilState)
{
	if (DepthStencilStatePool.GetCount() >= ZE_D3D10_STATE_CACHE_CAPACITY)
	{
		zeError("Max depth stencil state count reached.");
		return NULL;
	}

	D3D10_DEPTH_STENCIL_DESC DepthStencilDesc;
	DepthStencilDesc.DepthEnable = DepthStencilState->GetZTestEnable();
	DepthStencilDesc.DepthFunc = ZEComparisionFunctionToD3D10(DepthStencilState->GetZFunction());
	DepthStencilDesc.DepthWriteMask = DepthStencilState->GetZWriteEnable() ? D3D10_DEPTH_WRITE_MASK_ALL : D3D10_DEPTH_WRITE_MASK_ZERO;
	DepthStencilDesc.StencilEnable = DepthStencilState->GetStencilTestEnable();
	DepthStencilDesc.StencilReadMask = DepthStencilState->GetStencilReadMask();
	DepthStencilDesc.StencilWriteMask = DepthStencilState->GetStencilWriteMask();
	DepthStencilDesc.BackFace.StencilPassOp = ZEStencilOperationToD3D10(DepthStencilState->GetBackStencilPass());
	DepthStencilDesc.BackFace.StencilFailOp = ZEStencilOperationToD3D10(DepthStencilState->GetBackStencilFail());
	DepthStencilDesc.BackFace.StencilDepthFailOp = ZEStencilOperationToD3D10(DepthStencilState->GetBackZFail());
	DepthStencilDesc.BackFace.StencilFunc = ZEComparisionFunctionToD3D10(DepthStencilState->GetBackStencilFunction());
	DepthStencilDesc.FrontFace.StencilPassOp = ZEStencilOperationToD3D10(DepthStencilState->GetFrontStencilPass());
	DepthStencilDesc.FrontFace.StencilFailOp = ZEStencilOperationToD3D10(DepthStencilState->GetFrontStencilFail());
	DepthStencilDesc.FrontFace.StencilDepthFailOp = ZEStencilOperationToD3D10(DepthStencilState->GetFrontZFail());
	DepthStencilDesc.FrontFace.StencilFunc = ZEComparisionFunctionToD3D10(DepthStencilState->GetFrontStencilFunction());

	ID3D10DepthStencilState* D3D10State = NULL;
	if (FAILED(D3D10Device->CreateDepthStencilState(&DepthStencilDesc, &D3D10State)))
	{
		zeError("Cannot create depth stencil state");
		return NULL;
	}

	DepthStencilStatePool.Append(new ZEDepthStencilStateEntry(D3D10State, DepthStencilState->Hash, 1));

	return D3D10State;
}

ID3D10InputLayout* ZED3D10StatePool::CreateD3D10VertexLayout(const ZEVertexLayout* VertexLayout, ID3D10Blob* ByteCode)
{
	if (VertexLayoutPool.GetCount() >= ZE_D3D10_STATE_CACHE_CAPACITY)
	{
		zeError("Max input layout count reached.");
		return NULL;
	}

	ZEUInt ElementCount = VertexLayout->GetElementCount();
	const ZEVertexElement* Elements = VertexLayout->GetLayout();

	D3D10_INPUT_ELEMENT_DESC ElementDesc[16];
	for (ZESize I = 0; I < ElementCount; ++I)
	{
		ElementDesc[I].SemanticName = Elements[I].Semantic;
		ElementDesc[I].SemanticIndex = Elements[I].SemanticIndex;
		ElementDesc[I].Format = ZEVertexElementTypeToD3D10(Elements[I].Type);
		ElementDesc[I].InputSlot = Elements[I].StreamSlot;
		ElementDesc[I].AlignedByteOffset = Elements[I].ByteOffset;
		ElementDesc[I].InstanceDataStepRate = Elements[I].InstanceCount;
		ElementDesc[I].InputSlotClass = ZEVertexUsageToD3D10(Elements[I].Usage);
	}

	ID3D10InputLayout* D3D10State = NULL;
	SIZE_T Lenght = ByteCode->GetBufferSize();
	const void* Code = ByteCode->GetBufferPointer();
	if (FAILED(D3D10Device->CreateInputLayout(ElementDesc, ElementCount, Code, Lenght, &D3D10State)))
	{
		zeError("Can not create automatic vertex declaration.");
		return false;
	}

	VertexLayoutPool.Append(new ZEVertexLayoutEntry(D3D10State, VertexLayout->Hash, 1));

	return D3D10State;
}

ZEList<ZEBlendStateEntry>		 ZED3D10StatePool::BlendStatePool;
ZEList<ZEVertexLayoutEntry>		 ZED3D10StatePool::VertexLayoutPool;
ZEList<ZESamplerStateEntry>		 ZED3D10StatePool::SamplerStatePool;
ZEList<ZERasterizerStateEntry>	 ZED3D10StatePool::RasterizerStatePool;
ZEList<ZEDepthStencilStateEntry> ZED3D10StatePool::DepthStencilStatePool;

ZED3D10StatePool::ZED3D10StatePool()
{

}

ZED3D10StatePool::~ZED3D10StatePool()
{

}

void ZED3D10StatePool::ClearStates()
{
	ZESize Count = 0;

	Count = BlendStatePool.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		ZEBlendStateEntry* Entry = BlendStatePool.Dequeue();
		delete Entry;
	}

	Count = VertexLayoutPool.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		ZEVertexLayoutEntry* Entry = VertexLayoutPool.Dequeue();
		delete Entry;
	}

	Count = SamplerStatePool.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		ZESamplerStateEntry* Entry = SamplerStatePool.Dequeue();
		delete Entry;
	}

	Count = RasterizerStatePool.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		ZERasterizerStateEntry* Entry = RasterizerStatePool.Dequeue();
		delete Entry;
	}

	Count = DepthStencilStatePool.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		ZEDepthStencilStateEntry* Entry = DepthStencilStatePool.Dequeue();
		delete Entry;
	}
}

void ZED3D10StatePool::ClearStatistics()
{
	zeCriticalError("Not implemented yet");
	// Use iterator for accessing entries
}

ZEUInt ZED3D10StatePool::GetStateCount()
{
	return (ZEUInt)(BlendStatePool.GetCount() + 
					VertexLayoutPool.GetCount() +
					SamplerStatePool.GetCount() + 
					RasterizerStatePool.GetCount() + 
					DepthStencilStatePool.GetCount());
}

ZEUInt ZED3D10StatePool::GetBlendStateCount()
{
	return (ZEUInt)BlendStatePool.GetCount();
}

ZEUInt ZED3D10StatePool::GetVertexLayoutCount()
{
	return (ZEUInt)VertexLayoutPool.GetCount();
}

ZEUInt ZED3D10StatePool::GetSamplerStateCount()
{
	return (ZEUInt)SamplerStatePool.GetCount();
}

ZEUInt ZED3D10StatePool::GetRasterizerStateCount()
{
	return (ZEUInt)RasterizerStatePool.GetCount();
}

ZEUInt ZED3D10StatePool::GetDepthStencilStateCount()
{
	return (ZEUInt)DepthStencilStatePool.GetCount();
}

ID3D10BlendState* ZED3D10StatePool::GetBlendState(const ZEBlendState* BlendState)
{
	ZESize Index = 0;
	ZEUInt64 Hash = 0;
	ZEBlendStateEntry* Entry = NULL;

	ZEListIterator<ZEBlendStateEntry> Iterator = BlendStatePool.GetIterator();
	
	Entry = Iterator.GetItem();
	Hash = BlendState->Hash;

	while (!Iterator.IsEnd())
	{
		if (Entry->Hash == Hash)
		{
			Entry->AccessCount++;
			ID3D10BlendState* State = Entry->BlendState;

			if (Index > 0)
			{
				// Swap with previous
			}

			return State;
		}

		++Index;
		Entry = Iterator.MoveNext();
	}

	return CreateD3D10BlendState(BlendState);
}

ID3D10SamplerState* ZED3D10StatePool::GetSamplerState(const ZESamplerState* SamplerState)
{
	ZESize Index = 0;
	ZEUInt64 Hash = 0;
	ZESamplerStateEntry* Entry = NULL;

	ZEListIterator<ZESamplerStateEntry> Iterator = SamplerStatePool.GetIterator();
	
	Entry = Iterator.GetItem();
	Hash = SamplerState->Hash;

	while (!Iterator.IsEnd())
	{
		if (Entry->Hash == Hash)
		{
			Entry->AccessCount++;
			ID3D10SamplerState* State = Entry->SamplerState;

			if (Index > 0)
			{
				// Swap with previous
			}

			return State;
		}

		++Index;
		Entry = Iterator.MoveNext();
	}

	return CreateD3D10SamplerState(SamplerState);
}

ID3D10RasterizerState* ZED3D10StatePool::GetRasterizerState(const ZERasterizerState* RasterizerState)
{
	ZESize Index = 0;
	ZEUInt64 Hash = 0;
	ZERasterizerStateEntry* Entry = NULL;

	ZEListIterator<ZERasterizerStateEntry> Iterator = RasterizerStatePool.GetIterator();
	
	Entry = Iterator.GetItem();
	Hash = RasterizerState->Hash;

	while (!Iterator.IsEnd())
	{
		if (Entry->Hash == Hash)
		{
			Entry->AccessCount++;
			ID3D10RasterizerState* State = Entry->RasterizerState;

			if (Index > 0)
			{
				// Swap with previous
			}

			return State;
		}

		++Index;
		Entry = Iterator.MoveNext();
	}

	return CreateD3D10RasterizerState(RasterizerState);
}

ID3D10DepthStencilState* ZED3D10StatePool::GetDepthStencilState(const ZEDepthStencilState* DepthStencilState)
{
	ZESize Index = 0;
	ZEUInt64 Hash = 0;
	ZEDepthStencilStateEntry* Entry = NULL;

	ZEListIterator<ZEDepthStencilStateEntry> Iterator = DepthStencilStatePool.GetIterator();
	
	Entry = Iterator.GetItem();
	Hash = DepthStencilState->Hash;

	while (!Iterator.IsEnd())
	{
		if (Entry->Hash == Hash)
		{
			Entry->AccessCount++;
			ID3D10DepthStencilState* State = Entry->DepthStencilState;

			if (Index > 0)
			{
				// Swap with previous
			}

			return State;
		}

		++Index;
		Entry = Iterator.MoveNext();
	}

	return CreateD3D10DepthStencilState(DepthStencilState);
}

ID3D10InputLayout* ZED3D10StatePool::GetVertexLayout(const ZEVertexLayout* VertexLayout, const ZEShader* VertexShader)
{
	ZEUInt Index = 0;
	ZEUInt64 Hash = 0;
	ZEVertexLayoutEntry* Entry = NULL;

	ZEListIterator<ZEVertexLayoutEntry> Iterator = VertexLayoutPool.GetIterator();
	
	Entry = Iterator.GetItem();
	Hash = VertexLayout->Hash;

	while (!Iterator.IsEnd())
	{
		if (Entry->Hash == Hash)
		{
			Entry->AccessCount++;
			ID3D10InputLayout* State = Entry->VertexLayout;

			if (Index > 0)
			{
				// Swap with previous
			}

			return State;
		}

		Index++;
		Entry = Iterator.MoveNext();
	}

	return CreateD3D10VertexLayout(VertexLayout, ((ZED3D10Shader*)VertexShader)->D3D10ByteCode);
}
