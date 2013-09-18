//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D10Texture2D.cpp
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
#include "ZED3D10Texture2D.h"
#include "ZED3D10RenderTarget.h"
#include "ZED3D10GraphicsModule.h"
#include "ZETexture/ZETextureData.h"

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

bool ZED3D10Texture2D::UpdateWith(ZEUInt ShadowIndex)
{
	if (!ShadowCopy.GetChanged(ShadowIndex))
		return true;

	UINT SubResourceIndex = D3D11CalcSubresource(0, 0, 1);

	D3D11_MAPPED_SUBRESOURCE Mapped;
	HRESULT Result = D3DContexes[0]->Map(D3D10Texture2D, SubResourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
	if (FAILED(Result))
	{
		zeError("D3D10 dynamic texture 2D mapping failed. ErrorCode: %d.", Result);
		return false;
	}

	ZESize RowSize = ShadowCopy.GetRowSize();
	ZESize RowCount = ShadowCopy.GetRowCount();
	
	ZEUInt8* Destination = (ZEUInt8*)Mapped.pData;
	const ZEUInt8* Source = (const ZEUInt8*)ShadowCopy.GetConstData(ShadowIndex);
	
	for (ZESize RowN = 0; RowN < RowCount; ++RowN)
	{
		ZEUInt8* DestinationRow = Destination + RowN * Mapped.RowPitch;
		const ZEUInt8* SourceRow = Source + RowN * RowSize;
		
		memcpy(DestinationRow, SourceRow, RowSize);
	}

	D3DContexes[0]->Unmap(D3D10Texture2D, SubResourceIndex);

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Texture 2D contents updated. Texture2D: %p, ShadowCOpyIdnex: %u.", this, ShadowIndex);
#endif

	return ZETexture2D::UpdateWith(ShadowIndex);
}

const ID3D11Texture2D* ZED3D10Texture2D::GetD3D10Texture() const
{
	return D3D10Texture2D;
}

const ID3D11ShaderResourceView* ZED3D10Texture2D::GetD3D10ResourceView() const
{
	return D3D10ShaderResourceView;
}

ZERenderTarget* ZED3D10Texture2D::CreateRenderTarget(ZEUInt MipLevel) const
{
	zeDebugCheck(!GetIsCreated(), "Texture not created.");
	zeDebugCheck(!State.IsStatic, "Dynamic textures cannot be render target");
	zeDebugCheck(!IsRenderTarget, "Texture is not created with render target flag");
	zeDebugCheck(MipLevel >= LevelCount, "Texture dont have specified Mipmap level");
	
	// Create render target view
	D3D11_RENDER_TARGET_VIEW_DESC RenderTargetDesc;
	RenderTargetDesc.Format = ConvertPixelFormat(PixelFormat);
	RenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RenderTargetDesc.Texture2D.MipSlice = MipLevel;

	ID3D11RenderTargetView* D3D10RenderTargetView = NULL;
	HRESULT Result = D3DDevices[0]->CreateRenderTargetView(D3D10Texture2D, &RenderTargetDesc, &D3D10RenderTargetView);
	if(FAILED(Result))
	{
		zeError("D3D10 texture 2D render target creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	ZED3D10RenderTarget* RenderTarget = new ZED3D10RenderTarget(Width >> MipLevel, Height >> MipLevel, 1, PixelFormat, TextureType, D3D10RenderTargetView);
	
#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Render target view created. Texture2D: %p, MipLevel: %u, Width: %u, Height: %u", 
			this, MipLevel, RenderTarget->Width, RenderTarget->Height);
#endif

	return RenderTarget;
}

bool ZED3D10Texture2D::CreateDynamic(ZEUInt Width, ZEUInt Height, ZETexturePixelFormat PixelFormat, ZETextureData* Data)
{
	zeDebugCheck(GetIsCreated(), "Texture already created.");
	zeDebugCheck(Width == 0, "Width cannot be zero");
	zeDebugCheck(Height == 0, "Height cannot be zero");
	zeDebugCheck(PixelFormat == ZE_TPF_NOTSET, "PixelFormat must be set");
	zeDebugCheck(Width > 8192 || Height > 8192, "Texture2D dimensions exceeds the limits");

	D3D11_USAGE Usage;
	Usage = D3D11_USAGE_DYNAMIC;

	UINT CPUAccess = 0;
	CPUAccess |= D3D11_CPU_ACCESS_WRITE;
	
	UINT BindFlags = 0;
	BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	
	D3D11_TEXTURE2D_DESC TextureDesc;
	TextureDesc.MiscFlags = 0;
	TextureDesc.ArraySize = 1;
	TextureDesc.Usage = Usage;
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.BindFlags = BindFlags;
	TextureDesc.MipLevels = 1;
	TextureDesc.CPUAccessFlags = CPUAccess;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Format = ConvertPixelFormat(PixelFormat);
	
	D3D11_SUBRESOURCE_DATA InitialData;
	if (Data != NULL)
	{
		InitialData.pSysMem = Data->GetSurfaces()[0].GetLevels()[0].GetData();
		InitialData.SysMemPitch = (UINT)Data->GetSurfaces()[0].GetLevels()[0].GetPitch();
		InitialData.SysMemSlicePitch = 0;
	}

	HRESULT Result = D3DDevices[0]->CreateTexture2D(&TextureDesc, Data == NULL ? NULL : &InitialData, &D3D10Texture2D);
	if (FAILED(Result))
	{
		zeError("D3D10 dynamic texture 2D creation failed. ErrorCode: %d.", Result);
		return false;
	}

	// Create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC ResourceDesc;
	ResourceDesc.Format = TextureDesc.Format;
	ResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ResourceDesc.Texture2D.MostDetailedMip = 0;
	ResourceDesc.Texture2D.MipLevels = 1;
	
	Result = D3DDevices[0]->CreateShaderResourceView(D3D10Texture2D, &ResourceDesc, &D3D10ShaderResourceView);
	if(FAILED(Result))
	{
		zeError("D3D10 dynamic texture 2D shader resource view creation failed. ErrorCode: %d.", Result);
		return false;
	}
	
	return ZETexture2D::CreateDynamic(Width, Height, PixelFormat, Data);
}

bool ZED3D10Texture2D::CreateStatic(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZETexturePixelFormat PixelFormat, bool RenderTarget, ZETextureData* Data)
{
	zeDebugCheck(GetIsCreated(), "Texture already created.");
	zeDebugCheck(Width == 0, "Width cannot be zero");
	zeDebugCheck(Height == 0, "Height cannot be zero");
	zeDebugCheck(LevelCount == 0, "LevelCount cannot be zero");
	zeDebugCheck(PixelFormat == ZE_TPF_NOTSET, "PixelFormat must be valid");
	zeDebugCheck(RenderTarget && LevelCount != 1, "Render target's LevelCount must be one ");
	zeDebugCheck(Width > 8192 || Height > 8192, "Texture2D dimensions exceeds the limits.");
	
	D3D11_USAGE Usage;
	Usage = RenderTarget ? D3D11_USAGE_DEFAULT : D3D11_USAGE_IMMUTABLE;

	UINT CPUAccess = 0;
	UINT BindFlags = 0;
	BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	BindFlags |= RenderTarget ? D3D11_BIND_RENDER_TARGET : 0;

	// Create texture 2D
	D3D11_TEXTURE2D_DESC TextureDesc;
	TextureDesc.MiscFlags = 0;
	TextureDesc.ArraySize = 1;
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.MipLevels = LevelCount;
	TextureDesc.Usage = Usage;
	TextureDesc.BindFlags = BindFlags;
	TextureDesc.CPUAccessFlags = CPUAccess;
	TextureDesc.Format = ConvertPixelFormat(PixelFormat);
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;

	bool UseInitialData = !RenderTarget && Data != NULL;

	ZEArray<D3D11_SUBRESOURCE_DATA> InitialData;
	if (UseInitialData)
	{
		ZEUInt LevelCount = Data->GetLevelCount();
		InitialData.Resize(LevelCount);

		for (ZESize LevelN = 0; LevelN < LevelCount; ++ LevelN)
		{
			InitialData[LevelN].pSysMem = Data->GetSurfaces()[0].GetLevels()[LevelN].GetData();
			InitialData[LevelN].SysMemPitch = (UINT)Data->GetSurfaces()[0].GetLevels()[LevelN].GetPitch();
			InitialData[LevelN].SysMemSlicePitch = 0;
		}
	}

	HRESULT Result = D3DDevices[0]->CreateTexture2D(&TextureDesc, UseInitialData ? InitialData.GetConstCArray() : NULL, &D3D10Texture2D);
	if (FAILED(Result))
	{
		zeError("D3D10 static texture 2D creation failed. ErrorCode: %d.", Result);
		return false;
	}

	// Create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC ResourceDesc;
	ResourceDesc.Format = TextureDesc.Format;
	ResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ResourceDesc.Texture2D.MostDetailedMip = 0;
	ResourceDesc.Texture2D.MipLevels = LevelCount;
	
	Result = D3DDevices[0]->CreateShaderResourceView(D3D10Texture2D, &ResourceDesc, &D3D10ShaderResourceView);
	if(FAILED(Result))
	{
		zeError("D3D10 static texture 2D shader resource view creation failed. ErrorCode: %d.", Result);
		return false;
	}

	return ZETexture2D::CreateStatic(Width, Height, LevelCount, PixelFormat, RenderTarget, Data);
}

ZED3D10Texture2D::ZED3D10Texture2D()
{
	D3D10Texture2D = NULL;
	D3D10ShaderResourceView = NULL;
}

ZED3D10Texture2D::~ZED3D10Texture2D()
{
	ZED3D_RELEASE(D3D10ShaderResourceView);
	ZED3D_RELEASE(D3D10Texture2D);
}
