//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Texture2D.cpp
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

#include "ZED11Texture2D.h"

#include "ZEError.h"
#include "ZEPointer\ZEPointer.h"
#include "ZED11RenderTarget.h"
#include "ZED11DepthStencilBuffer.h"
#include "ZED11Module.h"

static DXGI_FORMAT ConvertDepthStencilFormat(ZEGRFormat Format)
{
	switch(Format)
	{
		default:
		case ZEGR_TF_NONE:
			return DXGI_FORMAT_UNKNOWN;

		case ZEGR_TF_D16_UNORM:
			return DXGI_FORMAT_R16_TYPELESS;

		case ZEGR_TF_D24_UNORM_S8_UINT:
			return DXGI_FORMAT_R24G8_TYPELESS;

		case ZEGR_TF_D32_FLOAT:
			return DXGI_FORMAT_R32_TYPELESS;
	}
}

static DXGI_FORMAT ConvertToDepthStencilViewFormat(ZEGRFormat Format)
{
	switch(Format)
	{
		default:
		case ZEGR_TF_NONE:
			return DXGI_FORMAT_UNKNOWN;

		case ZEGR_TF_D16_UNORM:
			return DXGI_FORMAT_D16_UNORM;

		case ZEGR_TF_D24_UNORM_S8_UINT:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;

		case ZEGR_TF_D32_FLOAT:
			return DXGI_FORMAT_D32_FLOAT;
	}
}

static DXGI_FORMAT ConvertToShaderResourceViewFormat(ZEGRFormat Format)
{
	switch(Format)
	{
		default:
		case ZEGR_TF_NONE:
			return DXGI_FORMAT_UNKNOWN;

		case ZEGR_TF_D16_UNORM:
			return DXGI_FORMAT_R16_UNORM;

		case ZEGR_TF_D24_UNORM_S8_UINT:
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

		case ZEGR_TF_D32_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;
	}
}

bool ZED11Texture2D::Initialize(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlags, ZEUInt ArrayCount, ZEUInt SampleCount, const void* Data)
{
	bool DepthStencil = BindFlags.GetFlags(ZEGR_RBF_DEPTH_STENCIL);

	DXGI_FORMAT TextureFormat = DepthStencil ? ConvertDepthStencilFormat(Format) : ConvertFormat(Format);

	D3D11_TEXTURE2D_DESC TextureDesc = {};
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.MipLevels = LevelCount;
	TextureDesc.ArraySize = ArrayCount;
	TextureDesc.Format = TextureFormat;
	TextureDesc.SampleDesc.Count = SampleCount;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = ConvertUsage(Usage);
	TextureDesc.BindFlags = ConvertBindFlags(BindFlags);
	TextureDesc.CPUAccessFlags = ConvertUsageToCpuAccessFlags(Usage);
	TextureDesc.MiscFlags = 0;

	ZEPointer<D3D11_SUBRESOURCE_DATA, ZEDeletorArray<D3D11_SUBRESOURCE_DATA>> SubresourceData;
	if (Data != NULL)
	{
		SubresourceData = new D3D11_SUBRESOURCE_DATA[ArrayCount * LevelCount];
		const ZEGRFormatDefinition* FormatInfo = ZEGRFormatDefinition::GetDefinition(Format);
		ZESize Offset = 0;
		for (ZEUInt J = 0; J < ArrayCount; J++)
		{
			ZEUInt SubresourceIndex = J * LevelCount;
			for (ZEUInt I = 0; I < LevelCount; I++)
			{
				ZEUInt NewWidth = Width >> I;
				ZEUInt NewHeight = Height >> I;

				NewWidth = (FormatInfo->Compressed && NewWidth < FormatInfo->BlockDimension) ? FormatInfo->BlockDimension : NewWidth;
				NewHeight = (FormatInfo->Compressed && NewHeight < FormatInfo->BlockDimension) ? FormatInfo->BlockDimension : NewHeight;

				ZESize RowPitch = (NewWidth / FormatInfo->BlockDimension) * FormatInfo->BlockSize;
				ZESize SlicePitch = RowPitch * (NewHeight / FormatInfo->BlockDimension);

				SubresourceData[SubresourceIndex].pSysMem = static_cast<const ZEBYTE*>(Data) + Offset;
				SubresourceData[SubresourceIndex].SysMemPitch = RowPitch;
				SubresourceData[SubresourceIndex].SysMemSlicePitch = SlicePitch;

				SubresourceIndex++;
				Offset += SlicePitch;
			}
		}
	}

	HRESULT Result = GetDevice()->CreateTexture2D(&TextureDesc, SubresourceData, &Texture2D);
	if (FAILED(Result))
	{
		zeError("Texture 2D creation failed. ErrorCode: %d.", Result);
		return false;
	}

	if (BindFlags.GetFlags(ZEGR_RBF_SHADER_RESOURCE))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceDesc = {};
		ShaderResourceDesc.Format = DepthStencil ? ConvertToShaderResourceViewFormat(Format) : TextureDesc.Format;
		if (ArrayCount > 1)
		{
			if (SampleCount > 1)
			{
				ShaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
				ShaderResourceDesc.Texture2DMSArray.ArraySize = ArrayCount;
				ShaderResourceDesc.Texture2DMSArray.FirstArraySlice = 0;
			}
			else
			{
				ShaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				ShaderResourceDesc.Texture2DArray.ArraySize = ArrayCount;
				ShaderResourceDesc.Texture2DArray.FirstArraySlice = 0;
				ShaderResourceDesc.Texture2DArray.MostDetailedMip = 0;
				ShaderResourceDesc.Texture2DArray.MipLevels = LevelCount;
			}
		}
		else
		{
			if (SampleCount > 1)
			{
				ShaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				ShaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				ShaderResourceDesc.Texture2D.MostDetailedMip = 0;
				ShaderResourceDesc.Texture2D.MipLevels = LevelCount;
			}
		}

		Result = GetDevice()->CreateShaderResourceView(Texture2D, &ShaderResourceDesc, &ShaderResourceView);
		if (FAILED(Result))
		{
			zeError("Texture 2D shader resource view creation failed. ErrorCode: %d.", Result);
			return false;
		}
	}

	if (BindFlags.GetFlags(ZEGR_RBF_UNORDERED_ACCESS))
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC UnorderedAccessDesc;
		UnorderedAccessDesc.Format = TextureDesc.Format;
		UnorderedAccessDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		UnorderedAccessDesc.Texture2D.MipSlice = 0;

		Result = GetDevice()->CreateUnorderedAccessView(Texture2D, &UnorderedAccessDesc, &UnorderedAccessView);
		if (FAILED(Result))
		{
			zeError("Texture 2D unordered access view creation failed. ErrorCode: %d.", Result);
			return false;
		}
	}

	return ZEGRTexture2D::Initialize(Width, Height, LevelCount, Format, Usage, BindFlags, ArrayCount, SampleCount, Data);
}

void ZED11Texture2D::Deinitialize()
{
	ZEGR_RELEASE(Texture2D);
	ZEGR_RELEASE(ShaderResourceView);
	ZEGR_RELEASE(UnorderedAccessView);

	ZEGRTexture2D::Deinitialize();
}

ID3D11Resource* ZED11Texture2D::GetResource() const
{
	ID3D11Resource* Resource = NULL;

	if (Texture2D != NULL)
		Texture2D->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(&Resource));

	return Resource;
}

ID3D11Texture2D* ZED11Texture2D::GetTexture() const
{
	return Texture2D;
}

ID3D11ShaderResourceView* ZED11Texture2D::GetShaderResourceView() const
{
	return ShaderResourceView;
}

ID3D11UnorderedAccessView* ZED11Texture2D::GetUnorderedAccessView() const
{
	return UnorderedAccessView;
}

ZED11Texture2D::ZED11Texture2D()
{
	Texture2D = NULL;
	ShaderResourceView = NULL;
	UnorderedAccessView = NULL;
}

ZED11Texture2D::~ZED11Texture2D()
{
	Deinitialize();
}

const ZEGRRenderTarget* ZED11Texture2D::GetRenderTarget(ZEUInt Level, ZEUInt ArrayIndex) const
{
	zeDebugCheck(Level >= GetLevelCount(), "Texture doesnt have specified Mipmap level");
	zeDebugCheck(ArrayIndex >= GetArrayCount(), "Array index is out of range");
	zeDebugCheck(!GetResourceBindFlags().GetFlags(ZEGR_RBF_RENDER_TARGET), "Texture had not been created with render target bind flag");
	
	ZEUInt Index = ArrayIndex * GetLevelCount() + Level;

	if (RenderTargets.GetCount() > Index)
		return RenderTargets[Index];

	D3D11_RENDER_TARGET_VIEW_DESC RenderTargetViewDesc = {};
	RenderTargetViewDesc.Format = ZED11ComponentBase::ConvertFormat(GetFormat());

	if (ArrayCount > 1)
	{
		if (SampleCount > 1)
		{
			RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
			RenderTargetViewDesc.Texture2DMSArray.ArraySize = 1;
			RenderTargetViewDesc.Texture2DMSArray.FirstArraySlice = ArrayIndex;
		}
		else
		{	
			RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			RenderTargetViewDesc.Texture2DArray.ArraySize = 1;
			RenderTargetViewDesc.Texture2DArray.FirstArraySlice = ArrayIndex;
			RenderTargetViewDesc.Texture2DArray.MipSlice = Level;
		}
	}
	else
	{
		if (SampleCount > 1)
		{
			RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			RenderTargetViewDesc.Texture2D.MipSlice = Level;
		}
	}

	ID3D11RenderTargetView* RenderTargetView = NULL;
	HRESULT Result = GetDevice()->CreateRenderTargetView(Texture2D, &RenderTargetViewDesc, &RenderTargetView);
	if (FAILED(Result))
	{
		zeError("Texture 2D render target creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	ZED11RenderTarget* RenderTarget = new ZED11RenderTarget(Width >> Level, Height >> Level, GetFormat(), RenderTargetView);
	RenderTarget->SetOwner(this);

	RenderTargets.Add(RenderTarget);

	return RenderTarget;
}

const ZEGRDepthStencilBuffer* ZED11Texture2D::GetDepthStencilBuffer(bool ReadOnly, ZEUInt ArrayIndex) const
{
	zeDebugCheck(ArrayIndex >= ArrayCount, "Array index is out of range");
	zeDebugCheck(!GetResourceBindFlags().GetFlags(ZEGR_RBF_DEPTH_STENCIL), "Texture had not been created with depth-stencil bind flag");

	UINT Flags = 0;

	ZEUInt Index = ArrayIndex * 2;
	if (!ReadOnly)
	{
		if (DepthStencilBuffers[Index] != NULL)
			return DepthStencilBuffers[Index];
	}
	else
	{
		Index++;
		if (DepthStencilBuffers[Index] != NULL)
			return DepthStencilBuffers[Index];

		Flags = D3D11_DSV_READ_ONLY_DEPTH;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	DepthStencilViewDesc.Format = ConvertToDepthStencilViewFormat(GetFormat());
	DepthStencilViewDesc.Flags = Flags;

	if (ArrayCount > 1)
	{
		if (SampleCount > 1)
		{
			DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
			DepthStencilViewDesc.Texture2DMSArray.ArraySize = 1;
			DepthStencilViewDesc.Texture2DMSArray.FirstArraySlice = ArrayIndex;
		}
		else
		{	
			DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			DepthStencilViewDesc.Texture2DArray.ArraySize = 1;
			DepthStencilViewDesc.Texture2DArray.FirstArraySlice = ArrayIndex;
			DepthStencilViewDesc.Texture2DArray.MipSlice = 0;
		}
	}
	else
	{
		if(SampleCount > 1)
		{
			DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			DepthStencilViewDesc.Texture2D.MipSlice = 0;
		}
	}

	ID3D11DepthStencilView* DepthStencilView = NULL;
	HRESULT Result = GetDevice()->CreateDepthStencilView(Texture2D, &DepthStencilViewDesc, &DepthStencilView);
	if(FAILED(Result))
	{
		zeError("Texture 2D depth stencil creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	ZED11DepthStencilBuffer* DepthStencilBuffer = new ZED11DepthStencilBuffer(Width, Height, GetFormat(), DepthStencilView);
	DepthStencilBuffer->SetOwner(this);
	DepthStencilBuffer->SetReadOnly(ReadOnly);

	DepthStencilBuffers[Index] = DepthStencilBuffer;

	return DepthStencilBuffer;
}

void ZED11Texture2D::UpdateSubResource(ZEUInt DestArrayIndex, ZEUInt DestLevel, ZERect* DestRect, const void* SrcData, ZESize SrcRowPitch) 
{
	zeDebugCheck(DestArrayIndex >= GetArrayCount(), "Destination array index is out of range.");
	zeDebugCheck(DestLevel >= GetLevelCount(), "There is no such a texture level.");
	
	if (DestRect != NULL)
	{
		D3D11_BOX D11Rect;
		D11Rect.left = DestRect->x;
		D11Rect.top = DestRect->y;
		D11Rect.right = DestRect->x + DestRect->Width;
		D11Rect.bottom = DestRect->y + DestRect->Height;
		D11Rect.front = 0;
		D11Rect.back = 1;

		LockContext();

		GetMainContext()->UpdateSubresource(Texture2D, DestArrayIndex * GetLevelCount() + DestLevel, &D11Rect, SrcData, SrcRowPitch, 0);
		
		UnlockContext();
	}
	else
	{
		LockContext();

		GetMainContext()->UpdateSubresource(Texture2D, DestArrayIndex * GetLevelCount() + DestLevel, NULL, SrcData, SrcRowPitch, 0);
		
		UnlockContext();
	}
}

bool ZED11Texture2D::Lock(void** Buffer, ZESize* RowPitch, ZESize* SlicePitch, ZEUInt ArrayIndex, ZEUInt Level)
{
	zeDebugCheck(Texture2D == NULL, "Texture is not initialized.");
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_GPU_READ_CPU_WRITE && (GetLevelCount() > 1 || GetArrayCount() > 1), "Dynamic texture having subresource cannot be mapped");
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_GPU_READ_ONLY || GetResourceUsage() == ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, "Texture had not been created for mapping");

	D3D11_MAP MapType;
	if (GetResourceUsage() == ZEGR_RU_CPU_READ_WRITE)
		MapType = D3D11_MAP_READ_WRITE;
	else if (GetResourceUsage() == ZEGR_RU_GPU_READ_CPU_WRITE)
		MapType = D3D11_MAP_WRITE_DISCARD;
	else
		return false;

	D3D11_MAPPED_SUBRESOURCE MapData = {};
	HRESULT Result = GetMainContext()->Map(Texture2D, ArrayIndex * GetLevelCount() + Level, MapType, 0, &MapData);
	if (FAILED(Result))
	{
		zeError("Cannot lock texture 2d resource.");
		return false;
	}

	*Buffer = MapData.pData;
	if (RowPitch != NULL)
		*RowPitch = MapData.RowPitch;
	if (SlicePitch != NULL)
		*SlicePitch = MapData.DepthPitch;

	return true;
}

void ZED11Texture2D::Unlock()
{
	zeDebugCheck(Texture2D == NULL, "Texture is not initialized.");

	GetMainContext()->Unmap(Texture2D, 0);
}
