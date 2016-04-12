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

ZEHolder<const ZEGRRenderTarget> ZED11Texture2D::GetRenderTarget(ZEUInt Level) const
{
	zeDebugCheck(!GetIsRenderTarget(), "Texture is not created with render target flag");
	zeDebugCheck(Level >= GetLevelCount(), "Texture dont have specified Mipmap level");
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_CPU_READ_WRITE, "Texture cannot be bound as input or output");

	if (RenderTargets[Level] == NULL)
	{
		D3D11_TEXTURE2D_DESC TextureDesc;
		Texture2D->GetDesc(&TextureDesc);
		if((TextureDesc.BindFlags & D3D11_BIND_RENDER_TARGET) != D3D11_BIND_RENDER_TARGET)
			return NULL;

		D3D11_RENDER_TARGET_VIEW_DESC RenderTargetDesc = {};
		RenderTargetDesc.Format = ZED11ComponentBase::ConvertFormat(GetFormat());
		if((TextureDesc.SampleDesc.Count > 1) )
		{
			RenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			RenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			RenderTargetDesc.Texture2D.MipSlice = Level;
		}

		ID3D11RenderTargetView* RenderTargetView = NULL;
		HRESULT Result = GetDevice()->CreateRenderTargetView(Texture2D, &RenderTargetDesc, &RenderTargetView);
		if(FAILED(Result))
		{
			zeError("Texture 2D render target creation failed. ErrorCode: %d.", Result);
			return NULL;
		}

		RenderTargets[Level] = new ZED11RenderTarget(Width >> Level, Height >> Level, GetFormat(), RenderTargetView);
	}

	return RenderTargets[Level];
}

ZEHolder<const ZEGRDepthStencilBuffer> ZED11Texture2D::GetDepthStencilBuffer(bool ReadOnly, ZEUInt ArrayIndex) const
{
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_CPU_READ_WRITE, "Texture cannot be bound as input or output");

	UINT Flags = 0;
	ZEUInt BufferIndex = 0;

	if (!ReadOnly)
	{
		if (DepthStencilBuffers[ArrayIndex][0] != NULL)
			return DepthStencilBuffers[ArrayIndex][0];
	}
	else
	{
		if (DepthStencilBuffers[ArrayIndex][1] != NULL)
			return DepthStencilBuffers[ArrayIndex][1];

		BufferIndex = 1;
		Flags = D3D11_DSV_READ_ONLY_DEPTH  | D3D11_DSV_READ_ONLY_STENCIL;
	}

	D3D11_TEXTURE2D_DESC TextureDesc;
	Texture2D->GetDesc(&TextureDesc);
	if ((TextureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL) != D3D11_BIND_DEPTH_STENCIL)
		return NULL;

	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	DepthStencilViewDesc.Format = ConvertToDepthStencilViewFormat(GetFormat());
	DepthStencilViewDesc.Flags = Flags;

	if (TextureDesc.ArraySize > 1)
	{
		if (TextureDesc.SampleDesc.Count > 1)
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
		if(TextureDesc.SampleDesc.Count > 1)
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

	DepthStencilBuffers[ArrayIndex][BufferIndex] = new ZED11DepthStencilBuffer(Width, Height, GetFormat(), DepthStencilView);
	
	return DepthStencilBuffers[ArrayIndex][BufferIndex];
}

void ZED11Texture2D::GenerateMipMaps()
{
	GetMainContext()->GenerateMips(ShaderResourceView);
}

bool ZED11Texture2D::Initialize(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlags, ZEUInt ArrayCount, ZEUInt SampleCount)
{
	bool DepthStencil = BindFlags.GetFlags(ZEGR_RBF_DEPTH_STENCIL);
	bool UnorderedAccess = BindFlags.GetFlags(ZEGR_RBF_UNORDERED_ACCESS);
	bool RenderTarget = BindFlags.GetFlags(ZEGR_RBF_RENDER_TARGET);
	bool ShaderResource = BindFlags.GetFlags(ZEGR_RBF_SHADER_RESOURCE);

	DXGI_FORMAT TextureFormat = (Format == ZEGR_TF_R8G8B8A8_UNORM) ? DXGI_FORMAT_B8G8R8A8_UNORM : ConvertFormat(Format);	//format of .tga
	TextureFormat = (RenderTarget || UnorderedAccess) ?  ConvertFormat(Format) : TextureFormat;
	TextureFormat = DepthStencil ? ConvertDepthStencilFormat(Format) : TextureFormat;

	D3D11_TEXTURE2D_DESC TextureDesc = {};
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.MipLevels = LevelCount;
	TextureDesc.ArraySize = ArrayCount;
	TextureDesc.Format = TextureFormat;
	TextureDesc.SampleDesc.Count = SampleCount;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = ConvertUsage(Usage);
	TextureDesc.BindFlags = (ShaderResource && !RenderTarget) ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : ConvertBindFlags(BindFlags);
	TextureDesc.CPUAccessFlags = (Usage == ZEGR_RU_CPU_READ_WRITE) ? (D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE) : (Usage == ZEGR_RU_GPU_READ_CPU_WRITE) ? D3D11_CPU_ACCESS_WRITE : 0;
	TextureDesc.MiscFlags = (ShaderResource && !RenderTarget) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

	HRESULT Result = GetDevice()->CreateTexture2D(&TextureDesc, NULL, &Texture2D);
	if (FAILED(Result))
	{
		zeError("Texture 2D creation failed. ErrorCode: %d.", Result);
		return false;
	}

	if (Usage != ZEGR_RU_CPU_READ_WRITE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC ResourceDesc = {};
		ResourceDesc.Format = DepthStencil ? ConvertToShaderResourceViewFormat(Format) : TextureDesc.Format;
		if (ArrayCount > 1)
		{
			if (SampleCount > 1)
			{
				ResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
				ResourceDesc.Texture2DMSArray.ArraySize = ArrayCount;
				ResourceDesc.Texture2DMSArray.FirstArraySlice = 0;
			}
			else
			{
				ResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				ResourceDesc.Texture2DArray.ArraySize = ArrayCount;
				ResourceDesc.Texture2DArray.FirstArraySlice = 0;
				ResourceDesc.Texture2DArray.MostDetailedMip = 0;
				ResourceDesc.Texture2DArray.MipLevels = LevelCount;
			}
		}
		else
		{
			if (SampleCount > 1)
			{
				ResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				ResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				ResourceDesc.Texture2D.MostDetailedMip = 0;
				ResourceDesc.Texture2D.MipLevels = LevelCount;
			}
		}

		Result = GetDevice()->CreateShaderResourceView(Texture2D, &ResourceDesc, &ShaderResourceView);
		if (FAILED(Result))
		{
			zeError("Texture 2D shader resource view creation failed. ErrorCode: %d.", Result);
			return false;
		}

		if (UnorderedAccess)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC UnorderedAccessViewDesc;
			UnorderedAccessViewDesc.Format = TextureDesc.Format;
			UnorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			UnorderedAccessViewDesc.Texture2D.MipSlice = 0;

			Result = GetDevice()->CreateUnorderedAccessView(Texture2D, &UnorderedAccessViewDesc, &UnorderedAccessView);
			if (FAILED(Result))
			{
				zeError("Texture 2D unordered access view creation failed. ErrorCode: %d.", Result);
				return false;
			}
		}
	}

	return ZEGRTexture2D::Initialize(Width, Height, LevelCount, Format, Usage, BindFlags, ArrayCount, SampleCount);
}

void ZED11Texture2D::Deinitialize()
{
	ZEGR_RELEASE(Texture2D);
	ZEGR_RELEASE(ShaderResourceView);
	ZEGR_RELEASE(UnorderedAccessView);

	for(ZEUInt I = 0; I < 15; I++)
		RenderTargets[I].Release();

	for(ZEUInt I = 0; I < 8; I++)
	{
		DepthStencilBuffers[I][0].Release();
		DepthStencilBuffers[I][1].Release();
	}

	ZEGRTexture2D::Deinitialize();
}

bool ZED11Texture2D::UpdateSubResource(ZEUInt DestArrayIndex, ZEUInt DestLevel, ZERect* DestRect, const void* SrcData, ZESize SrcRowPitch) 
{
	zeCheckError(DestArrayIndex >= GetArrayCount(), false, "Destination array index is out of range.");
	zeCheckError(DestLevel >= GetLevelCount(), false, "There is no such a texture level.");
	
	if (DestRect != NULL)
	{
		D3D11_BOX D11Rect;
		D11Rect.left = DestRect->x;
		D11Rect.top = DestRect->y;
		D11Rect.right = DestRect->x + DestRect->Width;
		D11Rect.bottom = DestRect->y + DestRect->Height;
		D11Rect.front = 0;
		D11Rect.back = 1;
		GetMainContext()->UpdateSubresource(Texture2D, DestArrayIndex * GetLevelCount() + DestLevel, &D11Rect, SrcData, SrcRowPitch, 0);
	}
	else
	{
		GetMainContext()->UpdateSubresource(Texture2D, DestArrayIndex * GetLevelCount() + DestLevel, NULL, SrcData, SrcRowPitch, 0);
	}

	return true;
}

bool ZED11Texture2D::Lock(void** Buffer, ZESize* RowPitch)
{
	zeCheckError(Texture2D == NULL, false, "Texture is not initialized.");
	zeCheckError(GetLevelCount() > 1 || GetArrayCount() > 1, false, "Texture having subresource cannot be mapped");
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_GPU_READ_ONLY || GetResourceUsage() == ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, "Texture has not been created for mapping");

	D3D11_MAP MapType;
	if (GetResourceUsage() == ZEGR_RU_CPU_READ_WRITE)
		MapType = D3D11_MAP_READ_WRITE;
	else if (GetResourceUsage() == ZEGR_RU_GPU_READ_CPU_WRITE)
		MapType = D3D11_MAP_WRITE_DISCARD;
	else
		return false;

	D3D11_MAPPED_SUBRESOURCE MapData = {};
	HRESULT Result = GetMainContext()->Map(Texture2D, 0, MapType, 0, &MapData);
	if (FAILED(Result))
	{
		zeError("Cannot lock texture 2d resource.");
		return false;
	}

	*Buffer = MapData.pData;
	if (RowPitch != NULL)
		*RowPitch = MapData.RowPitch;

	return true;
}

void ZED11Texture2D::Unlock()
{
	zeDebugCheck(Texture2D == NULL, "Texture is not initailized.");

	GetMainContext()->Unmap(Texture2D, 0);
}

ZED11Texture2D::ZED11Texture2D()
{
	Texture2D = NULL;
	ShaderResourceView = NULL;
	UnorderedAccessView = NULL;
}

ZED11Texture2D::~ZED11Texture2D()
{

}
