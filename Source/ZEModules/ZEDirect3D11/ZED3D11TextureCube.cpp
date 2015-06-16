//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D11TextureCube.cpp
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

#include "ZELog.h"
#include "ZEError.h"
#include "ZED3D11TextureCube.h"
#include "ZED3D11RenderTarget.h"
#include "ZED3D11GraphicsModule.h"
#include "ZETexture/ZETextureData.h"
#include "ZEGraphics/ZEGraphicsDefinitions.h"

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

bool ZED3D11TextureCube::UpdateWith(ZEUInt ShadowIndex)
{
	if (!ShadowCopy.GetChanged(ShadowIndex))
		return true;

	ZESize RowSize = ShadowCopy.GetRowSize();
	ZESize RowCount = ShadowCopy.GetRowCount();
	ZESize SliceSize = ShadowCopy.GetSliceSize();
	ZESize SliceCount = ShadowCopy.GetSliceCount();
	const void* Source = (const void*)ShadowCopy.GetConstData(ShadowIndex);

	//! Until the changes per subreasource is being logged, it is not possible to update single face

	for (ZESize SliceN = 0; SliceN < SliceCount; ++SliceN)
	{
		UINT SubResourceIndex = D3D10CalcSubresource(0, (UINT)SliceN, 1);

		D3D11_MAPPED_SUBRESOURCE Mapped;
		HRESULT Result = D3DContexes[0]->Map(D3D10TextureCube, SubResourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
		if (FAILED(Result))
		{
			zeError("D3D10 dynamic texture 2D mapping failed. ErrorCode: %d.", Result);
			return false;
		}
	
		ZEUInt8* DestinationSlice = (ZEUInt8*)Mapped.pData;
		ZEUInt8* SourceSlice = (ZEUInt8*)Source + SliceN * SliceSize;
		
		for (ZESize RowN = 0; RowN < RowCount; ++RowN)
		{
			ZEUInt8* DestinationRow = DestinationSlice + RowN * Mapped.RowPitch;
			const ZEUInt8* SourceRow = SourceSlice + RowN * RowSize;
		
			memcpy(DestinationRow, SourceRow, RowSize);
		}

		D3DContexes[0]->Unmap(D3D10TextureCube, SubResourceIndex);
	}
	
#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Texture cube contents updated. TextureCube: %p, ShadowCOpyIdnex: %u.", this, ShadowIndex);
#endif

	return ZETextureCube::UpdateWith(ShadowIndex);
}

const ID3D11Texture2D* ZED3D11TextureCube::GetD3D10Texture() const
{
	return D3D10TextureCube;
}

const ID3D11ShaderResourceView* ZED3D11TextureCube::GetD3D10ResourceView() const
{
	return D3D10ShaderResourceView;
}

ZERenderTarget* ZED3D11TextureCube::CreateRenderTarget(ZEUInt MipLevel) const
{
	zeDebugCheck(GetIsCreated(), "Texture already created.");
	zeDebugCheck(!GetIsStatic(), "Dynamic textures cannot be render target");
	zeDebugCheck(!GetIsRenderTarget(), "Texture not created with render target flag");
	zeDebugCheck(MipLevel >= LevelCount, "Texture dont have specified Mipmap level");

	// Create render target view
	D3D11_RENDER_TARGET_VIEW_DESC RenderTargetDesc;
	RenderTargetDesc.Format = ConvertPixelFormat(PixelFormat);
	RenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	RenderTargetDesc.Texture2DArray.MipSlice = MipLevel;
	RenderTargetDesc.Texture2DArray.FirstArraySlice = 0;
	RenderTargetDesc.Texture2DArray.ArraySize = 6;
		
	ID3D11RenderTargetView* D3D10RenderTargetView = NULL;
	HRESULT Result = D3DDevices[0]->CreateRenderTargetView(D3D10TextureCube, &RenderTargetDesc, &D3D10RenderTargetView);
	if(FAILED(Result))
	{
		zeError("D3D10 texture cube render target creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	ZED3D11RenderTarget* RenderTarget = new ZED3D11RenderTarget(EdgeLength >> MipLevel, EdgeLength >> MipLevel, 1, PixelFormat, TextureType, D3D10RenderTargetView);

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Render target view created. TextureCube: %p, MipLevel: %u, Width: %u, Height: %u, Depth: %u", 
			this, MipLevel, RenderTarget->Width, RenderTarget->Height, RenderTarget->Depth);
#endif

	return RenderTarget;
}

bool ZED3D11TextureCube::CreateDynamic(ZEUInt EdgeLength, ZETexturePixelFormat PixelFormat, ZETextureData* InitialData)
{
	zeDebugCheck(GetIsCreated(), "Texture already created.");
	zeDebugCheck(EdgeLength == 0, "EdgeLength cannot be zero");
	zeDebugCheck(EdgeLength > 8192, "EdgeLength exceeds the limits.");
	zeDebugCheck(PixelFormat == ZE_TPF_NOTSET, "PixelFormat must be set");
	
	D3D11_USAGE Usage;
	Usage = D3D11_USAGE_DYNAMIC;

	UINT CPUAccess = 0;
	CPUAccess |= D3D11_CPU_ACCESS_WRITE;
	
	UINT BindFlags = 0;
	BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	
	UINT MiscFlags = 0;
	MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

	// Create texture
	D3D11_TEXTURE2D_DESC TextureDesc;
	TextureDesc.MiscFlags = MiscFlags;
	TextureDesc.ArraySize = 6;
	TextureDesc.Width = EdgeLength;
	TextureDesc.Height = EdgeLength;
	TextureDesc.MipLevels = 1;
	TextureDesc.Usage = Usage;
	TextureDesc.BindFlags = BindFlags;
	TextureDesc.CPUAccessFlags = CPUAccess;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Format = ConvertPixelFormat(PixelFormat);

	ZEArray<D3D11_SUBRESOURCE_DATA> Data;
	if (InitialData != NULL)
	{
		Data.Resize(6);

		for (ZESize SurfaceN = 0; SurfaceN < 6; ++ SurfaceN)
		{
			ZETextureLevel* Level = &InitialData->GetSurfaces()[SurfaceN].GetLevels()[0];

			Data[SurfaceN].pSysMem = Level->GetData();
			Data[SurfaceN].SysMemPitch = (UINT)Level->GetPitch();
 			Data[SurfaceN].SysMemSlicePitch = 0;
		}
	}

	HRESULT Result = D3DDevices[0]->CreateTexture2D(&TextureDesc, InitialData == NULL ? NULL : Data.GetConstCArray(), &D3D10TextureCube);
	if (FAILED(Result))
	{
		zeError("D3D10 dynamic texture cube creation failed. ErrorCode: %d.", Result);
		return false;
	}

	// Create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC ResourceDesc;
	ResourceDesc.Format = TextureDesc.Format;
	ResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	ResourceDesc.TextureCube.MostDetailedMip = 0;
	ResourceDesc.TextureCube.MipLevels = 1;
	
	Result = D3DDevices[0]->CreateShaderResourceView(D3D10TextureCube, &ResourceDesc, &D3D10ShaderResourceView);
	if(FAILED(Result))
	{
		zeError("D3D10 dynamic texture cube shader resource view creation failed. ErrorCode: %d.", Result);
		return false;
	}
	
	return ZETextureCube::CreateDynamic(EdgeLength, PixelFormat, InitialData);
}

bool ZED3D11TextureCube::CreateStatic(ZEUInt EdgeLength, ZEUInt LevelCount, ZETexturePixelFormat PixelFormat, bool RenderTarget, ZETextureData* InitialData)
{
	zeDebugCheck(GetIsCreated(), "Texture already created.");
	zeDebugCheck(EdgeLength == 0, "EdgeLength cannot be zero");
	zeDebugCheck(LevelCount == 0, "LevelCount cannot be zero");
	zeDebugCheck(EdgeLength > 8192, "EdgeLength exceeds the limits.");
	zeDebugCheck(PixelFormat == ZE_TPF_NOTSET, "PixelFormat must be set");
	zeDebugCheck(RenderTarget && LevelCount != 1, "Render target's LevelCount must be one.");

	D3D11_USAGE Usage;
	Usage = RenderTarget ? D3D11_USAGE_DEFAULT : D3D11_USAGE_IMMUTABLE;

	UINT CPUAccess = 0;
	
	UINT BindFlags = 0;
	BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	BindFlags |= RenderTarget ? D3D11_BIND_RENDER_TARGET : 0;
	
	UINT MiscFlags = 0;
	MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

	D3D11_TEXTURE2D_DESC TextureDesc;
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

	bool UseInitialData = !RenderTarget && InitialData != NULL;

	ZEArray<D3D11_SUBRESOURCE_DATA> Data;
	if (UseInitialData)
	{
		Data.Resize(LevelCount * 6);

		for (ZESize SurfaceN = 0; SurfaceN < 6; ++ SurfaceN)
		{
			ZETextureSurface* Surface = &InitialData->GetSurfaces()[SurfaceN];

			for (ZESize LevelN = 0; LevelN < LevelCount; ++ LevelN)
			{	
				ZETextureLevel* Level = &Surface->GetLevels()[LevelN];
 				
				Data[SurfaceN * LevelCount + LevelN].pSysMem = Level->GetData();
				Data[SurfaceN * LevelCount + LevelN].SysMemPitch = (UINT)Level->GetPitch();
 				Data[SurfaceN * LevelCount + LevelN].SysMemSlicePitch = 0;
			}
		}
	}

	HRESULT Result = D3DDevices[0]->CreateTexture2D(&TextureDesc, UseInitialData ? Data.GetConstCArray() : NULL, &D3D10TextureCube);
	if (FAILED(Result))
	{
		zeError("D3D10 static texture cube creation failed. ErrorCode: %d.", Result);
		return false;
	}

	// Create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC ResourceDesc;
	ResourceDesc.Format = TextureDesc.Format;
	ResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	ResourceDesc.TextureCube.MostDetailedMip = 0;
	ResourceDesc.TextureCube.MipLevels = LevelCount;
	
	Result = D3DDevices[0]->CreateShaderResourceView(D3D10TextureCube, &ResourceDesc, &D3D10ShaderResourceView);
	if(FAILED(Result))
	{
		zeError("D3D10 static texture cube shader resource view creation failed. ErrorCode: %d.", Result);
		return false;
	}

	return ZETextureCube::CreateStatic(EdgeLength, LevelCount, PixelFormat, RenderTarget, InitialData);
}

ZED3D11TextureCube::ZED3D11TextureCube()
{
	D3D10TextureCube = NULL;
	D3D10ShaderResourceView = NULL;
}

ZED3D11TextureCube::~ZED3D11TextureCube()
{
	ZED3D_RELEASE(D3D10ShaderResourceView);
	ZED3D_RELEASE(D3D10TextureCube);
}
