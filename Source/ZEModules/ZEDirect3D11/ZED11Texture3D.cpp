//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Texture3D.cpp
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

#include "ZED11Texture3D.h"

#include "ZED11Module.h"
#include "ZED11RenderTarget.h"
#include "ZED11Texture2D.h"
#include "ZEError.h"

#include <d3d11.h>

ID3D11Texture3D* ZED11Texture3D::GetTexture()
{
	return Texture3D;
}

ID3D11ShaderResourceView* ZED11Texture3D::GetResourceView()
{
	return ResourceView;
}

ZEGRRenderTarget* ZED11Texture3D::GetRenderTarget(ZEUInt Depth, ZEUInt Level)
{
	zeDebugCheck(Texture3D == NULL, "Texture not created.");
	zeCheckError(!GetIsRenderTarget(), NULL, "Texture is not created as a render target.");
	zeCheckError(Level >= GetLevelCount(), NULL, "Texture dont have specified Mipmap level.");

	// Create render target view
	D3D11_RENDER_TARGET_VIEW_DESC RenderTargetDesc;
	RenderTargetDesc.Format = ZED11Texture2D::ConvertFormat(GetFormat());
	RenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
	RenderTargetDesc.Texture3D.MipSlice = Level;
	RenderTargetDesc.Texture3D.FirstWSlice = 0;
	RenderTargetDesc.Texture3D.WSize = -1;

	ID3D11RenderTargetView* RenderTargetView = NULL;
	HRESULT Result = GetDevice()->CreateRenderTargetView(Texture3D, &RenderTargetDesc, &RenderTargetView);
	if(FAILED(Result))
	{
		zeError("D3D10 texture 3D render target creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	ZED11RenderTarget* RenderTarget = new ZED11RenderTarget(Width >> Level, Height >> Level, GetFormat(), RenderTargetView);
	
	#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Render target view created. Texture3D: %p, MipLevel: %u, Width: %u, Height: %u, Depth: %u", 
			this, Level, RenderTarget->Width, RenderTarget->Height, RenderTarget->Depth);
	#endif

	return RenderTarget;
}


bool ZED11Texture3D::Initialize(ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZEUInt Level, ZEGRFormat Format, bool RenderTarget)
{
	zeDebugCheck(Texture3D != NULL, "Texture already created.");
	zeCheckError(ZED11Texture2D::ConvertFormat(Format) == DXGI_FORMAT_UNKNOWN, false, "Unknown texture format.");

	D3D11_USAGE Usage;
	Usage = RenderTarget ? D3D11_USAGE_DEFAULT : D3D11_USAGE_IMMUTABLE;

	UINT CPUAccess = 0;
	UINT BindFlags = 0;
	BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	BindFlags |= RenderTarget ? D3D11_BIND_RENDER_TARGET : 0;

	D3D11_TEXTURE3D_DESC TextureDesc;
	TextureDesc.MiscFlags = 0;
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.Depth = Depth;
	TextureDesc.MipLevels = Level;
	TextureDesc.Usage = Usage;
	TextureDesc.BindFlags = BindFlags;
	TextureDesc.CPUAccessFlags = CPUAccess;
	TextureDesc.Format = ZED11Texture2D::ConvertFormat(Format);

	HRESULT Result = GetDevice()->CreateTexture3D(&TextureDesc, NULL, &Texture3D);
	if (FAILED(Result))
	{
		zeError("Texture 3D creation failed. ErrorCode: %d.", Result);
		return false;
	}

	// Create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC ResourceDesc;
	ResourceDesc.Format = TextureDesc.Format;
	ResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	ResourceDesc.Texture3D.MostDetailedMip = 0;
	ResourceDesc.Texture3D.MipLevels = Level;

	Result = GetDevice()->CreateShaderResourceView(Texture3D, &ResourceDesc, &ResourceView);
	if(FAILED(Result))
	{
		zeError("Texture 3D shader resource view creation failed. ErrorCode: %d.", Result);
		return false;
	}

	return ZEGRTexture3D::Initialize(Width, Height, Depth, Level, Format, RenderTarget);
}

void ZED11Texture3D::Deinitialize()
{
	ZEGR_RELEASE(ResourceView);
	ZEGR_RELEASE(Texture3D);

	ZEGRTexture3D::Deinitialize();
}

bool ZED11Texture3D::Lock(void** Buffer, ZESize* Pitch, ZEUInt Depth, ZEUInt Level)
{
	zeDebugCheck(Texture3D == NULL, "Texture is not initailized.");
	zeCheckError(Buffer == NULL || Pitch == NULL, false, "Buffer is NULL.");
	zeCheckError(Depth >= GetDepth(), false, "There is no such a texture depth.");
	zeCheckError(Level >= GetLevelCount(), false, "There is no such a texture level.");

	D3D11_MAPPED_SUBRESOURCE MapData;
	HRESULT Result = GetMainContext()->Map(Texture3D, Level , D3D11_MAP_WRITE, D3D11_MAP_FLAG_DO_NOT_WAIT, &MapData);
	*Buffer = (ZEBYTE*)MapData.pData + MapData.DepthPitch * Depth;
	*Pitch = MapData.RowPitch;

	return SUCCEEDED(Result);
}

void ZED11Texture3D::Unlock(ZEUInt Level)
{
	zeDebugCheck(Texture3D == NULL, "Texture is not initailized.");
	zeCheckError(Level >= GetLevelCount(), ZE_VOID, "There is no such a texture level.");

	GetMainContext()->Unmap(Texture3D, Level);
}

ZED11Texture3D::ZED11Texture3D()
{
	Texture3D = NULL;
	ResourceView = NULL;
}
