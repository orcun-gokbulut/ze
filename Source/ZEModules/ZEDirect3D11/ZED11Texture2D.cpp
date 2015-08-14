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
#include "ZED11Module.h"

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

	ZED11RenderTarget* RenderTarget = new ZED11RenderTarget(Width >> Level, Height >> Level, GetFormat(), RenderTargetView);
	
	#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Render target view created. Texture2D: %p, MipLevel: %u, Width: %u, Height: %u", 
			this, MipLevel, RenderTarget->Width, RenderTarget->Height);
	#endif

	return RenderTarget;
}

bool ZED11Texture2D::Initialize(ZEUInt Width, ZEUInt Height, ZEUInt Level, ZEGRFormat Format, bool RenderTarget)
{
	D3D11_USAGE Usage;
	Usage = RenderTarget ? D3D11_USAGE_DEFAULT : D3D11_USAGE_IMMUTABLE;

	UINT CPUAccess = 0;
	UINT BindFlags = 0;
	BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	BindFlags |= RenderTarget ? D3D11_BIND_RENDER_TARGET : 0;

	D3D11_TEXTURE2D_DESC TextureDesc;
	TextureDesc.MiscFlags = 0;
	TextureDesc.ArraySize = 1;
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.MipLevels = Level;
	TextureDesc.Usage = Usage;
	TextureDesc.BindFlags = BindFlags;
	TextureDesc.CPUAccessFlags = CPUAccess;
	TextureDesc.Format = ZED11ComponentBase::ConvertFormat(Format);
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;

	HRESULT Result = GetDevice()->CreateTexture2D(&TextureDesc, NULL, &Texture2D);
	if (FAILED(Result))
	{
		zeError("Texture 2D creation failed. ErrorCode: %d.", Result);
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC ResourceDesc;
	ResourceDesc.Format = TextureDesc.Format;
	ResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ResourceDesc.Texture2D.MostDetailedMip = 0;
	ResourceDesc.Texture2D.MipLevels = Level;
	
	Result = GetDevice()->CreateShaderResourceView(Texture2D, &ResourceDesc, &ResourceView);
	if(FAILED(Result))
	{
		zeError("Texture 2D shader resource view creation failed. ErrorCode: %d.", Result);
		return false;
	}

	return ZEGRTexture2D::Initialize(Width, Height, Level, Format, RenderTarget);
}

void ZED11Texture2D::Deinitialize()
{
	ZEGR_RELEASE(ResourceView);
	ZEGR_RELEASE(Texture2D);
	ZEGRTexture2D::Deinitialize();
}

bool ZED11Texture2D::Lock(void** Buffer, ZESize* Pitch, ZEUInt Level)
{
	zeDebugCheck(Texture2D == NULL, "Texture is not initailized.");
	zeCheckError(Buffer == NULL || Pitch == NULL, false, "Buffer is NULL.");
	zeCheckError(Level >= GetLevelCount(), false, "There is no such a texture level.");

	D3D11_MAPPED_SUBRESOURCE MapData;
	HRESULT Result = GetMainContext()->Map(Texture2D, Level, D3D11_MAP_WRITE, D3D11_MAP_FLAG_DO_NOT_WAIT, &MapData);
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
