//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11TextureCube.cpp
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

#include "ZED11TextureCube.h"
#include "ZED11Module.h"
#include "ZED11RenderTarget.h"
#include "ZED11Texture2D.h"

#include "ZEError.h"

ID3D11Texture2D* ZED11TextureCube::GetTexture()
{
	return Texture;
}

ID3D11ShaderResourceView* ZED11TextureCube::GetResourceView()
{
	return ResourceView;
}

ZEGRRenderTarget* ZED11TextureCube::GetRenderTarget(ZEGRTextureCubeFace Face, ZEUInt Level)
{
	zeDebugCheck(Texture == NULL, "Empty texture.");
	zeCheckError(!GetIsRenderTarget(), NULL, "Texture not created with render target flag");
	zeCheckError(Level >= GetLevelCount(), NULL, "Texture dont have specified Mipmap level");

	// Create render target view
	D3D11_RENDER_TARGET_VIEW_DESC RenderTargetDesc;
	RenderTargetDesc.Format = ZED11Texture2D::ConvertFormat(GetFormat());
	RenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	RenderTargetDesc.Texture2DArray.MipSlice = Level;
	RenderTargetDesc.Texture2DArray.FirstArraySlice = Face;
	RenderTargetDesc.Texture2DArray.ArraySize = 6;
		
	ID3D11RenderTargetView* RenderTargetView = NULL;
	HRESULT Result = GetDevice()->CreateRenderTargetView(Texture, &RenderTargetDesc, &RenderTargetView);
	if(FAILED(Result))
	{
		zeError("D3D10 texture cube render target creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	ZED11RenderTarget* RenderTarget = new ZED11RenderTarget(GetLength() >> Level, GetLength() >> Level, GetFormat(), RenderTargetView);

	#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Render target view created. TextureCube: %p, MipLevel: %u, Width: %u, Height: %u, Depth: %u", 
			this, Level, RenderTarget->Width, RenderTarget->Height, RenderTarget->Depth);
	#endif

	return RenderTarget;
}

bool ZED11TextureCube::Initialize(ZEUInt Length, ZEUInt LevelCount, ZEGRFormat Format, bool RenderTarget)
{
	zeDebugCheck(Texture != NULL, "Texture already created.");
	zeCheckError(ZED11Texture2D::ConvertFormat(Format) == DXGI_FORMAT_UNKNOWN, false, "Unknown pixel format.");

	D3D11_USAGE Usage = D3D11_USAGE_DEFAULT;

	UINT CPUAccess = 0;
	
	UINT BindFlags = 0;
	BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	BindFlags |= RenderTarget ? D3D11_BIND_RENDER_TARGET : 0;
	
	UINT MiscFlags = 0;
	MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

	D3D11_TEXTURE2D_DESC TextureDesc;
	TextureDesc.MiscFlags = MiscFlags;
	TextureDesc.ArraySize = 6;
	TextureDesc.Width = Length;
	TextureDesc.Height = Length;
	TextureDesc.MipLevels = LevelCount;
	TextureDesc.Usage = Usage;
	TextureDesc.BindFlags = BindFlags;
	TextureDesc.CPUAccessFlags = CPUAccess;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;	//ZED11Texture2D::ConvertFormat(Format);

	HRESULT Result = GetDevice()->CreateTexture2D(&TextureDesc, NULL, &Texture);
	if (FAILED(Result))
	{
		zeError("Texture cube creation failed. ErrorCode: %d.", Result);
		return false;
	}

	// Create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC ResourceDesc;
	ResourceDesc.Format = TextureDesc.Format;
	ResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	ResourceDesc.TextureCube.MostDetailedMip = 0;
	ResourceDesc.TextureCube.MipLevels = LevelCount;
	
	Result = GetDevice()->CreateShaderResourceView(Texture, &ResourceDesc, &ResourceView);
	if(FAILED(Result))
	{
		zeError("Texture cube shader resource view creation failed. ErrorCode: %d.", Result);
		return false;
	}

	return ZEGRTextureCube::Initialize(Length, LevelCount, Format, RenderTarget);
}

void ZED11TextureCube::Deinitialize()
{
	ZEGR_RELEASE(ResourceView);
	ZEGR_RELEASE(Texture);
	ZEGRTextureCube::Deinitialize();
}

bool ZED11TextureCube::UpdateSubResource(void* Data, ZESize RowPitch, ZEGRTextureCubeFace Face, ZEUInt Level)
{
	zeCheckError(Data == NULL, false, "Data is NULL.");
	zeCheckError(Level >= GetLevelCount(), false, "There is no such a texture level.");
	zeCheckError(Face >= 6, false, "There is no such a cube texture face.");

	GetMainContext()->UpdateSubresource(Texture, Face * GetLevelCount() + Level, NULL, Data, RowPitch, 0);

	return true;
}

bool ZED11TextureCube::Lock(void** Buffer, ZESize* Pitch, ZEGRTextureCubeFace Face, ZEUInt Level)
{
	zeDebugCheck(Texture == NULL, "Texture is not initailized.");
	zeCheckError(Buffer == NULL || Pitch == NULL, false, "Buffer is NULL.");
	zeCheckError(Level >= GetLevelCount(), false, "There is no such a texture level.");
	zeCheckError(Face >= 6, false, "There is no such a cube texture face.");

	D3D11_MAPPED_SUBRESOURCE MapData;
	HRESULT Result = GetMainContext()->Map(Texture, Face * GetLevelCount() + Level, D3D11_MAP_WRITE_DISCARD, 0, &MapData);
	*Buffer = MapData.pData;
	*Pitch = MapData.RowPitch;

	if (FAILED(Result) && Result != D3D11_MAP_FLAG_DO_NOT_WAIT)
	{
		zeError("Cannot lock texture 2d resource.");
		return false;
	}

	return SUCCEEDED(Result);
}

void ZED11TextureCube::Unlock(ZEGRTextureCubeFace Face, ZEUInt Level)
{
	zeDebugCheck(Texture == NULL, "Texture is not initailized.");
	zeCheckError(Level >= GetLevelCount(), ZE_VOID, "There is no such a texture level.");
	zeCheckError(Face >= 6, ZE_VOID, "There is no such a cube texture face.");

	GetMainContext()->Unmap(Texture, Face * GetLevelCount() + Level);
}
ZED11TextureCube::ZED11TextureCube()
{
	Texture = NULL;
	ResourceView = NULL;
}

