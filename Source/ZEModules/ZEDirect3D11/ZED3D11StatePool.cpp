//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D11StatePool.cpp
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
#include "ZED3D11Shader.h"
#include "ZED3D11StatePool.h"
#include "ZEGraphics/ZEShader.h"
#include "ZEDS/ZEListIterators.h"
#include "ZED3D11GraphicsModule.h"
#include "ZEGraphics/ZEBlendState.h"
#include "ZEGraphics/ZEVertexLayout.h"
#include "ZEGraphics/ZESamplerState.h"
#include "ZEGraphics/ZERasterizerState.h"
#include "ZEGraphics/ZEDepthStencilState.h"
#include "ZEGraphics/ZEGraphicsDefinitions.h"

inline static D3D11_BLEND ZEBlendOptionToD3D10(ZEBlendOption BlendOption)
{
	static const D3D11_BLEND Values[] =
	{
		(D3D11_BLEND)-1,
		D3D11_BLEND_ZERO,				// ZE_BO_ZERO				= 1,
		D3D11_BLEND_ONE,				// ZE_BO_ONE				= 2,
		D3D11_BLEND_SRC_COLOR,			// ZE_BO_SRC_COLOR			= 3,
		D3D11_BLEND_INV_SRC_COLOR,		// ZE_BO_INV_SRC_COLOR		= 4,
		D3D11_BLEND_SRC_ALPHA,			// ZE_BO_SRC_ALPHA			= 5,
		D3D11_BLEND_INV_SRC_ALPHA,		// ZE_BO_INV_SRC_ALPHA		= 6,
		D3D11_BLEND_DEST_ALPHA,			// ZE_BO_DEST_ALPHA			= 7,
		D3D11_BLEND_INV_DEST_ALPHA,		// ZE_BO_INV_DEST_ALPHA		= 8,
		D3D11_BLEND_DEST_COLOR,			// ZE_BO_DEST_COLOR			= 9,
		D3D11_BLEND_INV_DEST_COLOR,		// ZE_BO_INV_DEST_COLOR		= 10,
		D3D11_BLEND_SRC_ALPHA_SAT,		// ZE_BO_SRC_ALPHA_SAT		= 11,
		D3D11_BLEND_BLEND_FACTOR,		// ZE_BO_BLEND_FACTOR		= 12,
		D3D11_BLEND_INV_BLEND_FACTOR,	// ZE_BO_INV_BLEND_FACTOR	= 13,
		D3D11_BLEND_SRC1_COLOR,			// ZE_BO_SRC1_COLOR			= 14,
		D3D11_BLEND_INV_SRC1_COLOR,		// ZE_BO_INV_SRC1_COLOR		= 15,
		D3D11_BLEND_SRC1_ALPHA,			// ZE_BO_SRC1_ALPHA			= 16,
		D3D11_BLEND_INV_SRC1_ALPHA		// ZE_BO_INV_SRC1_ALPHA		= 17
	};

	return Values[BlendOption];
}

inline static D3D11_BLEND_OP ZEBlendEquationToD3D10(ZEBlendEquation BlendEquation)
{
	static const D3D11_BLEND_OP Values[] =
	{
		(D3D11_BLEND_OP)-1,
		D3D11_BLEND_OP_ADD,				// ZE_BE_ADD			= 1,
		D3D11_BLEND_OP_SUBTRACT,		// ZE_BE_SUBTRACT		= 2,
		D3D11_BLEND_OP_REV_SUBTRACT,	// ZE_BE_REV_SUBTRACT	= 3,
		D3D11_BLEND_OP_MIN,				// ZE_BE_MIN			= 4,
		D3D11_BLEND_OP_MAX				// ZE_BE_MAX			= 5 
	};

	return Values[BlendEquation];
}

inline static D3D11_COMPARISON_FUNC ZEComparisionFunctionToD3D10(ZEComparisonFunction CompFunc)
{
	static const D3D11_COMPARISON_FUNC Values[] =
	{
		(D3D11_COMPARISON_FUNC)0,			
		D3D11_COMPARISON_NEVER,				// ZE_CF_NEVER			= 1,
		D3D11_COMPARISON_LESS,				// ZE_CF_LESS			= 2,
		D3D11_COMPARISON_EQUAL, 			// ZE_CF_EQUAL			= 3,
		D3D11_COMPARISON_LESS_EQUAL, 		// ZE_CF_LESS_EQUAL		= 4,
		D3D11_COMPARISON_GREATER, 			// ZE_CF_GREATER		= 5,
		D3D11_COMPARISON_NOT_EQUAL, 		// ZE_CF_NOT_EQUAL		= 6,
		D3D11_COMPARISON_GREATER_EQUAL, 	// ZE_CF_GREATER_EQUAL	= 7,
		D3D11_COMPARISON_ALWAYS				// ZE_CF_ALWAYS			= 8 
	};

	return Values[CompFunc];
}

inline static D3D11_TEXTURE_ADDRESS_MODE ZETextureAdressModeToD3D10(ZETextureAddressMode TextureAddress)
{
	static const D3D11_TEXTURE_ADDRESS_MODE Values[] =
	{
		(D3D11_TEXTURE_ADDRESS_MODE)-1,	
		D3D11_TEXTURE_ADDRESS_WRAP,		// ZE_TAM_WRAP		= 1,
		D3D11_TEXTURE_ADDRESS_MIRROR,	// ZE_TAM_MIRROR	= 2,
		D3D11_TEXTURE_ADDRESS_CLAMP,	// ZE_TAM_CLAMP		= 3,
		D3D11_TEXTURE_ADDRESS_BORDER	// ZE_TAM_BORDER	= 4
	};
	return Values[TextureAddress];
}

inline static D3D11_FILTER_TYPE ZETextureFilterModeToD3D10FilterType(ZETextureFilterMode FilterMode)
{
	static const D3D11_FILTER_TYPE Values[] =
	{
		(D3D11_FILTER_TYPE)-1,		
		D3D11_FILTER_TYPE_POINT,	// ZE_TFM_POINT			= 1,
		D3D11_FILTER_TYPE_LINEAR	// ZE_TFM_LINEAR		= 2,
	};

	return Values[FilterMode];
}

inline static D3D11_FILTER ZETextureFilterModeToD3D10(ZETextureFilterMode Min, ZETextureFilterMode Mag, ZETextureFilterMode Mip)
{
	if (Min == ZE_TFM_ANISOTROPY || Mag == ZE_TFM_ANISOTROPY || Mip == ZE_TFM_ANISOTROPY)
		return D3D11_FILTER_ANISOTROPIC;

	return (D3D11_FILTER)(	(ZETextureFilterModeToD3D10FilterType(Min) << D3D11_MIN_FILTER_SHIFT) | 
							(ZETextureFilterModeToD3D10FilterType(Mag) << D3D11_MAG_FILTER_SHIFT) | 
							(ZETextureFilterModeToD3D10FilterType(Mip) << D3D11_MIP_FILTER_SHIFT));
}

inline static D3D11_FILL_MODE ZEFillModeToD3D10(ZEFillMode FillMode)
{
	static const D3D11_FILL_MODE Values[] = 
	{
		(D3D11_FILL_MODE)-1,
		D3D11_FILL_WIREFRAME,	// ZE_FM_WIREFRAME	= 1,
		D3D11_FILL_SOLID,		// ZE_FM_SOLID		= 2
	};

	return Values[FillMode];
}

inline static D3D11_CULL_MODE ZECullDirectionToD3D10(ZECullDirection CullDirection)
{
	static const D3D11_CULL_MODE Values[] = 
	{
		(D3D11_CULL_MODE)-1,
		D3D11_CULL_NONE,		// ZE_CD_NONE				= 1,
		D3D11_CULL_FRONT,		// ZE_CD_CLOCKWISE 			= 2,
		D3D11_CULL_BACK			// ZE_CD_COUNTER_CLOCKWISE	= 3
	};

	return Values[CullDirection];
}

inline static D3D11_STENCIL_OP ZEStencilOperationToD3D10(ZEStencilOperation StencilOperation)
{
	static const D3D11_STENCIL_OP Values[] = 
	{ 
		(D3D11_STENCIL_OP)-1,
		D3D11_STENCIL_OP_KEEP,		//	ZE_SO_KEEP		= 1,
		D3D11_STENCIL_OP_ZERO, 		//	ZE_SO_ZERO		= 2,
		D3D11_STENCIL_OP_REPLACE,	//	ZE_SO_REPLACE	= 3,
		D3D11_STENCIL_OP_INCR_SAT, 	//	ZE_SO_INCR_SAT	= 4,
		D3D11_STENCIL_OP_DECR_SAT, 	//	ZE_SO_DECR_SAT	= 5,
		D3D11_STENCIL_OP_INVERT,	//	ZE_SO_INVERT	= 6,
		D3D11_STENCIL_OP_INCR, 		//	ZE_SO_INCR		= 7,
		D3D11_STENCIL_OP_DECR		//	ZE_SO_DECR		= 8 
	};

	return Values[StencilOperation];
}

inline static DXGI_FORMAT ZEVertexElementTypeToD3D10(ZEGRVertexElementType ElementType)
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

inline static D3D11_INPUT_CLASSIFICATION ZEVertexUsageToD3D10(ZEGRVertexUsage Usage)
{
	static const D3D11_INPUT_CLASSIFICATION Values[] =
	{
		D3D11_INPUT_PER_VERTEX_DATA,		//	ZE_VU_PER_VERTEX	= 0,
		D3D11_INPUT_PER_INSTANCE_DATA 		//	ZE_VU_PER_INSTANCE	= 1,
	};

	return Values[Usage];
}

ID3D11BlendState* ZED3D11StatePool::CreateD3D10State(ZEBlendState* BlendState)
{
	if (BlendStatePool.GetCount() >= ZE_STATE_POOL_CACHE_CAPACITY)
	{
		zeError("Max blend state count reached.");
		return NULL;
	}
	
	D3D11_BLEND_DESC BlendDesc;

	BlendDesc.IndependentBlendEnable = true;
	BlendDesc.AlphaToCoverageEnable = BlendState->GetAlphaToCoverageEnable();

	for (ZESize I = 0; I < ZE_MAX_RENDER_TARGET_SLOT; ++I)
	{
		BlendDesc.RenderTarget[I].BlendEnable = BlendState->GetBlendEnable((ZEUInt)I) ? 1 : 0;
		BlendDesc.RenderTarget[I].RenderTargetWriteMask = BlendState->GetComponentWriteMask((ZEUInt)I).Value;

		BlendDesc.RenderTarget[I].BlendOp = ZEBlendEquationToD3D10(BlendState->GetBlendEquation());
		BlendDesc.RenderTarget[I].SrcBlend = ZEBlendOptionToD3D10(BlendState->GetSourceBlendOption());
		BlendDesc.RenderTarget[I].DestBlend = ZEBlendOptionToD3D10(BlendState->GetDestinationBlendOption());
		
		BlendDesc.RenderTarget[I].BlendOpAlpha = ZEBlendEquationToD3D10(BlendState->GetBlendAlphaEquation());
		BlendDesc.RenderTarget[I].SrcBlendAlpha = ZEBlendOptionToD3D10(BlendState->GetSourceBlendAlphaOption());
		BlendDesc.RenderTarget[I].DestBlendAlpha = ZEBlendOptionToD3D10(BlendState->GetDestinationBlendAlphaOption());
	}

	ID3D11BlendState* D3D10State = NULL;
	HRESULT Result = D3DDevices[0]->CreateBlendState(&BlendDesc, &D3D10State);
	if (FAILED(Result))
	{
		zeError("D3D10 Blend state creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	BlendStatePool.Append(new ZEBlendStateEntry(D3D10State, BlendState->GetHash(), 1));
	BlendStateCount++;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Blend state created. Hash: %Iu.", (ZEUInt64)BlendState->GetHash());
#endif

	return D3D10State;
}

ID3D11SamplerState* ZED3D11StatePool::CreateD3D10State(ZESamplerState* SamplerState)
{
	if (SamplerStatePool.GetCount() >= ZE_STATE_POOL_CACHE_CAPACITY)
	{
		zeError("Max sampler state count reached.");
		return NULL;
	}

	ZEVector4 BorderColor = SamplerState->GetBorderColor();

	D3D11_SAMPLER_DESC SamplerDesc;
	SamplerDesc.BorderColor[0] = BorderColor.x;
	SamplerDesc.BorderColor[1] = BorderColor.y;
	SamplerDesc.BorderColor[2] = BorderColor.z;
	SamplerDesc.BorderColor[3] = BorderColor.w;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc.MinLOD = SamplerState->GetMinLOD();
	SamplerDesc.MaxLOD = SamplerState->GetMaxLOD();
	SamplerDesc.MipLODBias = SamplerState->GetMipLODBias();
	SamplerDesc.MaxAnisotropy = SamplerState->GetMaxAnisotrophy();
	SamplerDesc.AddressU = ZETextureAdressModeToD3D10(SamplerState->GetAddressU());
	SamplerDesc.AddressV = ZETextureAdressModeToD3D10(SamplerState->GetAddressV());
	SamplerDesc.AddressW = ZETextureAdressModeToD3D10(SamplerState->GetAddressW());
	SamplerDesc.Filter = ZETextureFilterModeToD3D10(SamplerState->GetMinFilter(), SamplerState->GetMagFilter(), SamplerState->GetMipFilter());
	
	ID3D11SamplerState* D3D10State = NULL;
	HRESULT Result = D3DDevices[0]->CreateSamplerState(&SamplerDesc, &D3D10State);
	if (FAILED(Result))
	{
		zeError("D3D10 Sampler state creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	SamplerStatePool.Append(new ZESamplerStateEntry(D3D10State, SamplerState->GetHash(), 1));
	SamplerStateCount++;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Sampler state created. Hash: %Iu.", (ZEUInt64)SamplerState->GetHash());
#endif

	return D3D10State;
}

ID3D11RasterizerState* ZED3D11StatePool::CreateD3D10State(ZERasterizerState* RasterizerState)
{
	if (RasterizerStatePool.GetCount() >= ZE_STATE_POOL_CACHE_CAPACITY)
	{
		zeError("Max rasterizer state count reached.");
		return NULL;
	}

	D3D11_RASTERIZER_DESC RasterizerDesc;
	RasterizerDesc.DepthBias = 0;
	RasterizerDesc.DepthBiasClamp = 0.0f;
	RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	RasterizerDesc.ScissorEnable = false;
	RasterizerDesc.DepthClipEnable = true;
	RasterizerDesc.MultisampleEnable = false;
	RasterizerDesc.AntialiasedLineEnable = false;
	RasterizerDesc.FillMode = ZEFillModeToD3D10(RasterizerState->GetFillMode());
	RasterizerDesc.CullMode = ZECullDirectionToD3D10(RasterizerState->GetCullDirection());
	RasterizerDesc.FrontCounterClockwise = RasterizerState->GetFrontIsCounterClockwise();


	ID3D11RasterizerState* D3D10State = NULL;
	HRESULT Result = D3DDevices[0]->CreateRasterizerState(&RasterizerDesc, &D3D10State);
	if (FAILED(Result))
	{
		zeError("D3D10 Rasterizer state creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	RasterizerStatePool.Append(new ZERasterizerStateEntry(D3D10State, RasterizerState->GetHash(), 1));
	RasterizerStateCount++;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Rasterizer state created. Hash: %Iu.", (ZEUInt64)RasterizerState->GetHash());
#endif

	return D3D10State;
}

ID3D11DepthStencilState* ZED3D11StatePool::CreateD3D10State(ZEDepthStencilState* DepthStencilState)
{
	if (DepthStencilStatePool.GetCount() >= ZE_STATE_POOL_CACHE_CAPACITY)
	{
		zeError("Max depth stencil state count reached.");
		return NULL;
	}
	
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	DepthStencilDesc.DepthEnable = DepthStencilState->GetZTestEnable();
	DepthStencilDesc.DepthFunc = ZEComparisionFunctionToD3D10(DepthStencilState->GetZFunction());
	DepthStencilDesc.DepthWriteMask = DepthStencilState->GetZWriteEnable() ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
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

	ID3D11DepthStencilState* D3D10State = NULL;
	HRESULT Result = D3DDevices[0]->CreateDepthStencilState(&DepthStencilDesc, &D3D10State);
	if (FAILED(Result))
	{
		zeError("D3D10 Depth stencil state creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	DepthStencilStatePool.Append(new ZEDepthStencilStateEntry(D3D10State, DepthStencilState->GetHash(), 1));
	DepthStencilStateCount++;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Depth stencil state created. Hash: %Iu.", (ZEUInt64)DepthStencilState->GetHash());
#endif

	return D3D10State;
}

ID3D11InputLayout* ZED3D11StatePool::CreateD3D10State(ZEGRVertexLayout* VertexLayout, ID3DBlob* ByteCode)
{
	if (VertexLayoutPool.GetCount() >= ZE_STATE_POOL_CACHE_CAPACITY)
	{
		zeError("Max input layout count reached.");
		return NULL;
	}

	ZEUInt ElementCount = VertexLayout->GetElementCount();
	const ZEGRVertexElement* Elements = VertexLayout->GetElements();

	D3D11_INPUT_ELEMENT_DESC ElementDesc[16];
	for (ZESize I = 0; I < ElementCount; ++I)
	{
		ElementDesc[I].SemanticName = Elements[I].Semantic;
		ElementDesc[I].SemanticIndex = Elements[I].Index;
		ElementDesc[I].Format = ZEVertexElementTypeToD3D10(Elements[I].Type);
		ElementDesc[I].InputSlot = Elements[I].Stream;
		ElementDesc[I].AlignedByteOffset = Elements[I].Offset;
		ElementDesc[I].InstanceDataStepRate = Elements[I].InstanceCount;
		ElementDesc[I].InputSlotClass = ZEVertexUsageToD3D10(Elements[I].Usage);
	}

	ID3D11InputLayout* D3D10State = NULL;
	SIZE_T Lenght = ByteCode->GetBufferSize();
	const void* Code = ByteCode->GetBufferPointer();
	
	HRESULT Result = D3DDevices[0]->CreateInputLayout(ElementDesc, ElementCount, Code, Lenght, &D3D10State);
	if (FAILED(Result))
	{
		zeError("D3D10 Vertex layout creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	VertexLayoutPool.Append(new ZEVertexLayoutEntry(D3D10State, VertexLayout->GetHash(), 1));
	VertexLayoutCount++;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Vertex layout created. Hash: %Iu.", (ZEUInt64)VertexLayout->GetHash());
#endif

	return D3D10State;
}

ZEStatePoolEntry* ZED3D11StatePool::FindPoolEntry(ZEList<ZEStatePoolEntry>& Pool, ZESize Hash)
{
	ZEStatePoolEntry* Entry = NULL;
	ZEListIterator<ZEStatePoolEntry> Iterator = Pool.GetIterator();
	
	Entry = Iterator.GetItem();
	while (!Iterator.IsEnd())
	{
		if (Entry->Hash == Hash)
			return Entry;

		Entry = Iterator.MoveNext();
	}

	return NULL;
}

ZED3D11StatePool::ZED3D11StatePool()
{

}

ZED3D11StatePool::~ZED3D11StatePool()
{

}

void ZED3D11StatePool::ClearStates()
{
	ZESize Count = 0;

	Count = BlendStatePool.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		ZEStatePoolEntry* Entry = BlendStatePool.Dequeue();
		delete Entry;
	}

	Count = VertexLayoutPool.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		ZEStatePoolEntry* Entry = VertexLayoutPool.Dequeue();
		delete Entry;
	}

	Count = SamplerStatePool.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		ZEStatePoolEntry* Entry = SamplerStatePool.Dequeue();
		delete Entry;
	}

	Count = RasterizerStatePool.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		ZEStatePoolEntry* Entry = RasterizerStatePool.Dequeue();
		delete Entry;
	}

	Count = DepthStencilStatePool.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		ZEStatePoolEntry* Entry = DepthStencilStatePool.Dequeue();
		delete Entry;
	}

	ZEStatePool::ClearStates();
}

void* ZED3D11StatePool::GetState(ZEBlendState* BlendState)
{
	zeDebugCheck(BlendState == NULL, "NULL pointer.");

	ZEStatePoolEntry* Entry = FindPoolEntry(BlendStatePool, BlendState->GetHash());
	return Entry == NULL ? CreateD3D10State(BlendState) : ((ZEBlendStateEntry*)Entry)->BlendState;
}

void* ZED3D11StatePool::GetState(ZESamplerState* SamplerState)
{
	zeDebugCheck(SamplerState == NULL, "NULL pointer.");

	ZEStatePoolEntry* Entry = FindPoolEntry(SamplerStatePool, SamplerState->GetHash());
	return Entry == NULL ? CreateD3D10State(SamplerState) : ((ZESamplerStateEntry*)Entry)->SamplerState;
}

void* ZED3D11StatePool::GetState(ZERasterizerState* RasterizerState)
{
	zeDebugCheck(RasterizerState == NULL, "NULL pointer.");

	ZEStatePoolEntry* Entry = FindPoolEntry(RasterizerStatePool, RasterizerState->GetHash());
	return Entry == NULL ? CreateD3D10State(RasterizerState) : ((ZERasterizerStateEntry*)Entry)->RasterizerState;
}

void* ZED3D11StatePool::GetState(ZEDepthStencilState* DepthStencilState)
{
	zeDebugCheck(DepthStencilState == NULL, "NULL pointer.");

	ZEStatePoolEntry* Entry = FindPoolEntry(DepthStencilStatePool, DepthStencilState->GetHash());
	return Entry == NULL ? CreateD3D10State(DepthStencilState) : ((ZEDepthStencilStateEntry*)Entry)->DepthStencilState;
}

void* ZED3D11StatePool::GetState(ZEGRVertexLayout* VertexLayout, const ZEShader* VertexShader)
{
	zeDebugCheck(VertexLayout == NULL, "NULL pointer.");
	zeDebugCheck(VertexShader == NULL, "NULL pointer.");
	zeDebugCheck(VertexShader->GetShaderType() != ZE_ST_VERTEX, "Wrong shader type.");

	ZEStatePoolEntry* Entry = FindPoolEntry(VertexLayoutPool, VertexLayout->GetHash());
	return Entry == NULL ? CreateD3D10State(VertexLayout, ((ZED3D11Shader*)VertexShader)->D3D10ByteCode) : ((ZEVertexLayoutEntry*)Entry)->VertexLayout;
}
