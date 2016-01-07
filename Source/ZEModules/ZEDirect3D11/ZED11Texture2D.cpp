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

inline static DXGI_FORMAT ConvertDepthStencilFormat(ZEGRFormat Format)
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

inline static DXGI_FORMAT ConvertToDepthStencilViewFormat(ZEGRFormat Format)
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

inline static DXGI_FORMAT ConvertToShaderResourceViewFormat(ZEGRFormat Format)
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

ID3D11Texture2D* ZED11Texture2D::GetTexture()
{
	return Texture2D;
}

ID3D11ShaderResourceView* ZED11Texture2D::GetResourceView()
{
	return ResourceView;
}

ZEGRRenderTarget* ZED11Texture2D::GetRenderTarget(ZEUInt Level)
{
	zeDebugCheck(!GetIsRenderTarget(), "Texture is not created with render target flag");
	zeDebugCheck(Level >= GetLevelCount(), "Texture dont have specified Mipmap level");
	
	if (RenderTargets[Level] == NULL)
	{
		D3D11_RENDER_TARGET_VIEW_DESC RenderTargetDesc;
		RenderTargetDesc.Format = ZED11ComponentBase::ConvertFormat(GetFormat());
		RenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		RenderTargetDesc.Texture2D.MipSlice = Level;

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

ZEGRDepthStencilBuffer* ZED11Texture2D::GetDepthStencilBuffer(ZEUInt ArrayIndex)
{
	if(DepthStencilBuffers[ArrayIndex] == NULL)
	{
		D3D11_TEXTURE2D_DESC TextureDesc;
		Texture2D->GetDesc(&TextureDesc);
		if((TextureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL) != D3D11_BIND_DEPTH_STENCIL)
			return NULL;

		D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
		DepthStencilViewDesc.Format = ConvertToDepthStencilViewFormat(GetFormat());
		if(TextureDesc.ArraySize > 0)
		{
			DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			DepthStencilViewDesc.Texture2DArray.ArraySize = 1;
			DepthStencilViewDesc.Texture2DArray.FirstArraySlice = ArrayIndex;
			DepthStencilViewDesc.Texture2DArray.MipSlice = 0;
			DepthStencilViewDesc.Flags = 0;
		}
		else
		{
			DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			DepthStencilViewDesc.Texture2D.MipSlice = 0;
			DepthStencilViewDesc.Flags = 0;
		}

		ID3D11DepthStencilView* DepthStencilView = NULL;
		HRESULT Result = GetDevice()->CreateDepthStencilView(Texture2D, &DepthStencilViewDesc, &DepthStencilView);
		if(FAILED(Result))
		{
			zeError("Texture 2D depth stencil creation failed. ErrorCode: %d.", Result);
			return NULL;
		}

		DepthStencilBuffers[ArrayIndex] = new ZED11DepthStencilBuffer(Width, Height, GetFormat(), DepthStencilView);
	}

	return DepthStencilBuffers[ArrayIndex];
}

void ZED11Texture2D::GenerateMipMaps()
{
	GetMainContext()->GenerateMips(ResourceView);
}

bool ZED11Texture2D::Initialize(ZEUInt Width, ZEUInt Height, ZEUInt ArrayCount, ZEUInt LevelCount, ZEGRFormat Format, bool RenderTarget, bool DepthStencil)
{
	UINT BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	BindFlags = DepthStencil ? ( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL) : BindFlags;

	DXGI_FORMAT TextureFormat = (Format == ZEGR_TF_R8G8B8A8_UNORM) ? DXGI_FORMAT_B8G8R8A8_UNORM : ConvertFormat(Format);	//format of .tga
	TextureFormat = RenderTarget ?  ConvertFormat(Format) : TextureFormat;
	TextureFormat = DepthStencil ? ConvertDepthStencilFormat(Format) : TextureFormat;

	D3D11_TEXTURE2D_DESC TextureDesc;
	TextureDesc.MiscFlags = DepthStencil ? 0 : D3D11_RESOURCE_MISC_GENERATE_MIPS;
	TextureDesc.ArraySize = ArrayCount;
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.MipLevels = LevelCount;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = BindFlags;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.Format = TextureFormat;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;

	HRESULT Result = GetDevice()->CreateTexture2D(&TextureDesc, NULL, &Texture2D);
	if (FAILED(Result))
	{
		zeError("Texture 2D creation failed. ErrorCode: %d.", Result);
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC ResourceDesc;
	ResourceDesc.Format = DepthStencil ? ConvertToShaderResourceViewFormat(Format) :  TextureDesc.Format;
	if(ArrayCount > 1)
	{
		ResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		ResourceDesc.Texture2DArray.ArraySize = ArrayCount;
		ResourceDesc.Texture2DArray.FirstArraySlice = 0;
		ResourceDesc.Texture2DArray.MostDetailedMip = 0;
		ResourceDesc.Texture2DArray.MipLevels = LevelCount;
	}
	else
	{
		ResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		ResourceDesc.Texture2D.MostDetailedMip = 0;
		ResourceDesc.Texture2D.MipLevels = LevelCount;
	}

	Result = GetDevice()->CreateShaderResourceView(Texture2D, &ResourceDesc, &ResourceView);
	if(FAILED(Result))
	{
		zeError("Texture 2D shader resource view creation failed. ErrorCode: %d.", Result);
		return false;
	}

	return ZEGRTexture2D::Initialize(Width, Height, ArrayCount, LevelCount, Format, RenderTarget, DepthStencil);
}

void ZED11Texture2D::Deinitialize()
{
	ZEGR_RELEASE(ResourceView);
	ZEGR_RELEASE(Texture2D);

	for(ZEUInt I = 0; I < 14; I++)
		RenderTargets[I].Release();

	for(ZEUInt I = 0; I < 14; I++)
		DepthStencilBuffers[I].Release();

	ZEGRTexture2D::Deinitialize();
}

bool ZED11Texture2D::UpdateSubResource(void* Data, ZEUInt ArrayIndex, ZEUInt Level, ZESize RowPitch) 
{
	zeCheckError(Data == NULL, false, "Data is NULL.");
	zeCheckError(Level >= GetLevelCount(), false, "There is no such a texture level.");

	GetMainContext()->UpdateSubresource(Texture2D, ArrayIndex * GetLevelCount() + Level, NULL, Data, RowPitch, 0);

	return true;
}

bool ZED11Texture2D::Lock(void** Buffer, ZESize* Pitch, ZEUInt Level)
{
	zeDebugCheck(Texture2D == NULL, "Texture is not initailized.");
	zeCheckError(Buffer == NULL || Pitch == NULL, false, "Buffer is NULL.");
	zeCheckError(Level >= GetLevelCount(), false, "There is no such a texture level.");

	D3D11_MAPPED_SUBRESOURCE MapData;
	HRESULT Result = GetMainContext()->Map(Texture2D, Level, D3D11_MAP_WRITE_DISCARD, 0, &MapData);
	*Buffer = MapData.pData;
	*Pitch = MapData.RowPitch;

	if (FAILED(Result) && Result != D3D11_MAP_FLAG_DO_NOT_WAIT)
	{
		zeError("Cannot lock texture 2d resource.");
		return false;
	}
	
	return SUCCEEDED(Result);
}

void ZED11Texture2D::Unlock(ZEUInt Level)
{
	zeDebugCheck(Texture2D == NULL, "Texture is not initailized.");
	zeCheckError(Level >= GetLevelCount(), ZE_VOID, "There is no such a texture level.");
	GetMainContext()->Unmap(Texture2D, Level);
}

ZED11Texture2D::ZED11Texture2D()
{
	Texture2D = NULL;
	ResourceView = NULL;
}

ZED11Texture2D::~ZED11Texture2D()
{
	Deinitialize();
}
