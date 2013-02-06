//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D10TextureCube.cpp
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

#include "ZEError.h"
#include "ZED3D10TextureCube.h"
#include "ZED3D10RenderTarget.h"
#include "ZED3D10GraphicsModule.h"
#include "ZETexture/ZETextureData.h"

#include <D3D10.h>

inline static DXGI_FORMAT ConvertPixelFormat(ZETexturePixelFormat Format)
{
	switch(Format)
	{
		case ZE_TPF_I8:
			return DXGI_FORMAT_R8_UNORM;
		case ZE_TPF_I8_4:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case ZE_TPF_I16:
			return DXGI_FORMAT_R16_UNORM;
		case ZE_TPF_I16_2:
			return DXGI_FORMAT_R16G16_UNORM;

		case ZE_TPF_F16:
			return DXGI_FORMAT_R16_FLOAT;
		case ZE_TPF_F16_2:
			return DXGI_FORMAT_R16G16_FLOAT;
		case ZE_TPF_F16_4:
			return DXGI_FORMAT_R16G16B16A16_FLOAT;

		case ZE_TPF_F32:
			return DXGI_FORMAT_R32_FLOAT;
		case ZE_TPF_F32_2:
			return DXGI_FORMAT_R32G32_FLOAT;
		case ZE_TPF_F32_4:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;

		case ZE_TPF_DXT1:
			return DXGI_FORMAT_BC1_UNORM;
		case ZE_TPF_DXT3:
			return DXGI_FORMAT_BC2_UNORM;
		case ZE_TPF_DXT5:
			return DXGI_FORMAT_BC3_UNORM;
			
		default:
			zeWarning("Unknown pixel format.");
			return (DXGI_FORMAT)0;
	}
}

ZED3D10TextureCube::ZED3D10TextureCube()
{
	TextureCube = NULL;
	ShaderResourceView = NULL;
}

ZED3D10TextureCube::~ZED3D10TextureCube()
{
	ZED3D_RELEASE(TextureCube);
	ZED3D_RELEASE(ShaderResourceView);
}

bool ZED3D10TextureCube::IsEmpty() const
{
	return TextureCube == NULL;
}

const ID3D10Texture2D* ZED3D10TextureCube::GetD3D10Texture() const
{
	return TextureCube;
}

const ID3D10ShaderResourceView* ZED3D10TextureCube::GetD3D10ResourceView() const
{
	return ShaderResourceView;
}

ZERenderTarget* ZED3D10TextureCube::CreateRenderTarget(ZEUInt MipLevel) const
{
	zeDebugCheck(!Static, "Dynamic textures cannot be render target");
	zeDebugCheck(!RenderTarget, "Texture not created with render target flag");
	zeDebugCheck(MipLevel >= LevelCount, "Texture dont have specified Mipmap level");

	// Create render target view
	D3D10_RENDER_TARGET_VIEW_DESC RenderTargetDesc;
	RenderTargetDesc.Format = ConvertPixelFormat(PixelFormat);
	RenderTargetDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DARRAY;
	RenderTargetDesc.Texture2DArray.MipSlice = MipLevel;
	RenderTargetDesc.Texture2DArray.FirstArraySlice = 0;
	RenderTargetDesc.Texture2DArray.ArraySize = 6;
		
	ID3D10RenderTargetView* D3D10RenderTargetView = NULL;
	if(FAILED(D3D10Device->CreateRenderTargetView(TextureCube, &RenderTargetDesc, &D3D10RenderTargetView)))
	{
		zeError("Cannot create render target view for static texture cube.");
		return NULL;
	}

	ZED3D10RenderTarget* RenderTarget = new ZED3D10RenderTarget();
	RenderTarget->D3D10RenderTargetView = D3D10RenderTargetView;
	RenderTarget->RenderTargetType = TextureType;
	RenderTarget->PixelFormat = PixelFormat;
	RenderTarget->Width = EdgeLength >> MipLevel;
	RenderTarget->Height = EdgeLength >> MipLevel;

	return RenderTarget;
}

bool ZED3D10TextureCube::CreateDynamic(ZEUInt EdgeLength, ZEUInt LevelCount, ZETexturePixelFormat PixelFormat)
{
	zeDebugCheck(TextureCube, "Texture alread created");
	zeDebugCheck(EdgeLength == 0, "EdgeLength cannot be zero");
	zeDebugCheck(LevelCount != 1, "LevelCount must be one");
	zeDebugCheck(PixelFormat == ZE_TPF_NOTSET, "PixelFormat must be set");
	zeDebugCheck(EdgeLength > 8191, "EdgeLength exceeds the limits, 0-8191");
	
	D3D10_USAGE Usage;
	Usage = D3D10_USAGE_DYNAMIC;

	UINT CPUAccess = 0;
	CPUAccess |= D3D10_CPU_ACCESS_WRITE;
	
	UINT BindFlags = 0;
	BindFlags |= D3D10_BIND_SHADER_RESOURCE;
	
	UINT MiscFlags = 0;
	MiscFlags |= D3D10_RESOURCE_MISC_TEXTURECUBE;

	// Create texture
	D3D10_TEXTURE2D_DESC TextureDesc;
	TextureDesc.MiscFlags = MiscFlags;
	TextureDesc.ArraySize = 6;
	TextureDesc.Width = EdgeLength;
	TextureDesc.Height = EdgeLength;
	TextureDesc.MipLevels = LevelCount;
	TextureDesc.Usage = Usage;
	TextureDesc.BindFlags = BindFlags;
	TextureDesc.CPUAccessFlags = CPUAccess;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Format = ConvertPixelFormat(PixelFormat);

	if (FAILED(D3D10Device->CreateTexture2D(&TextureDesc, NULL, &TextureCube)))
	{
		zeError("Can not create dynamic texture cube.");
		return false;
	}

	// Create shader resource view
	D3D10_SHADER_RESOURCE_VIEW_DESC ResourceDesc;
	ResourceDesc.Format = TextureDesc.Format;
	ResourceDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
	ResourceDesc.TextureCube.MostDetailedMip = 0;
	ResourceDesc.TextureCube.MipLevels = LevelCount;
	
	if(FAILED(D3D10Device->CreateShaderResourceView(TextureCube, &ResourceDesc, &ShaderResourceView)))
	{
		zeError("Cannot create shader resource view for dynamic texture cube.");
		return false;
	}

	this->EdgeLength = EdgeLength;
	this->LevelCount = LevelCount;
	this->PixelFormat = PixelFormat;
	this->RenderTarget = false;
	this->Static = false;

	return true;
}

bool ZED3D10TextureCube::CreateStatic(ZEUInt EdgeLength, ZEUInt LevelCount, ZETexturePixelFormat PixelFormat, bool RenderTarget, ZETextureData* Data)
{
	zeDebugCheck(TextureCube, "Texture alread created");
	zeDebugCheck(EdgeLength == 0, "EdgeLength cannot be zero");
	zeDebugCheck(LevelCount == 0, "LevelCount cannot be zero");
	zeDebugCheck(PixelFormat == ZE_TPF_NOTSET, "PixelFormat must be valid");
	zeDebugCheck(RenderTarget && LevelCount != 1, "Render target's LevelCount must be one ");
	zeDebugCheck(RenderTarget && Data != NULL, "Render target specified, ignoring intial data.");
	zeDebugCheck(EdgeLength > 8191, "EdgeLength exceeds the limits, 0-8191.");

	D3D10_USAGE Usage;
	Usage = RenderTarget ? D3D10_USAGE_DEFAULT : D3D10_USAGE_IMMUTABLE;

	UINT CPUAccess = 0;
	
	UINT BindFlags = 0;
	BindFlags |= D3D10_BIND_SHADER_RESOURCE;
	BindFlags |= RenderTarget ? D3D10_BIND_RENDER_TARGET : 0;
	
	UINT MiscFlags = 0;
	MiscFlags |= D3D10_RESOURCE_MISC_TEXTURECUBE;

	// Create texture
	D3D10_TEXTURE2D_DESC TextureDesc;
	TextureDesc.MiscFlags = MiscFlags;
	TextureDesc.ArraySize = 6;
	TextureDesc.Width = EdgeLength;
	TextureDesc.Height = EdgeLength;
	TextureDesc.MipLevels = LevelCount;
	TextureDesc.Usage = Usage;
	TextureDesc.BindFlags = BindFlags;
	TextureDesc.CPUAccessFlags = CPUAccess;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Format = ConvertPixelFormat(PixelFormat);

	ZEArray<D3D10_SUBRESOURCE_DATA> InitialData;
	if (!RenderTarget && Data != NULL)
	{
		ZEUInt LevelCount = Data->GetLevelCount();
		ZEUInt SurfaceCount = Data->GetSurfaceCount();
		InitialData.Resize(LevelCount * SurfaceCount);

		for (ZESize SurfaceN = 0; SurfaceN < SurfaceCount; ++ SurfaceN)
		{
			for (ZESize LevelN = 0; LevelN < LevelCount; ++ LevelN)
			{	
 				InitialData[SurfaceN * LevelCount + LevelN].pSysMem = Data->GetSurfaces()[SurfaceN].GetLevels()[LevelN].GetData();
				InitialData[SurfaceN * LevelCount + LevelN].SysMemPitch = (UINT)Data->GetSurfaces()[SurfaceN].GetLevels()[LevelN].GetPitch();
 				InitialData[SurfaceN * LevelCount + LevelN].SysMemSlicePitch = 0;
			}
		}
	}

	if (FAILED(D3D10Device->CreateTexture2D(&TextureDesc, InitialData.GetConstCArray(), &TextureCube)))
	{
		zeError("Can not create static texture cube.");
		return false;
	}

	// Create shader resource view
	D3D10_SHADER_RESOURCE_VIEW_DESC ResourceDesc;
	ResourceDesc.Format = TextureDesc.Format;
	ResourceDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
	ResourceDesc.TextureCube.MostDetailedMip = 0;
	ResourceDesc.TextureCube.MipLevels = LevelCount;
	
	if(FAILED(D3D10Device->CreateShaderResourceView(TextureCube, &ResourceDesc, &ShaderResourceView)))
	{
		zeError("Cannot create shader resource view for dynamic texture cube.");
		return false;
	}

	this->EdgeLength = EdgeLength;
	this->LevelCount = LevelCount;
	this->PixelFormat = PixelFormat;
	this->RenderTarget = RenderTarget;
	this->Static = true;

	return true;
}

bool ZED3D10TextureCube::Lock(void** Buffer, ZESize* Pitch, ZETextureCubeFace Face, ZEUInt Level)
{
	zeDebugCheck(Pitch == NULL, "Pitch cannot be null");
	zeDebugCheck(*Buffer == NULL, "Buffer cannot be null");
	zeDebugCheck(Static, "Texture must be dynamic to lock or unlock");
	zeDebugCheck(TextureCube == NULL, "Cannot lock empty texture cube");
	zeDebugCheck(Level >= LevelCount, "Level is greater than LevelCount");

	D3D10_MAPPED_TEXTURE2D Mapped;
	if (FAILED(TextureCube->Map(D3D10CalcSubresource(Level, (ZEUInt)Face, 1), D3D10_MAP_WRITE_DISCARD, 0, &Mapped)))
	{
		zeError("Cannot lock dynamic texture cube.");
		return false;
	}
	
	*Buffer = Mapped.pData;
	*Pitch = (ZESize)Mapped.RowPitch;

	return true;
}

bool ZED3D10TextureCube::Unlock(ZETextureCubeFace Face, ZEUInt Level)
{
	zeDebugCheck(Static, "Texture must be dynamic to lock or unlock");
	zeDebugCheck(TextureCube == NULL, "Cannot unlock empty texture 2D");
	zeDebugCheck(Level >= LevelCount, "Level is greater than LevelCount");

	TextureCube->Unmap(D3D10CalcSubresource(Level, (ZEUInt)Face, 1));
	
	return true;
}




