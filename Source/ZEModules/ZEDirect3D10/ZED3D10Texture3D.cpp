//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D10Texture3D.cpp
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
#include "ZED3D10Texture3D.h"
#include "ZED3D10GraphicsModule.h"
#include "ZETexture/ZETextureData.h"

#include <D3D10.h>

inline static float GetAveragePixelSize(ZETexturePixelFormat PixelFormat)
{
	float Size = 0.0f;
	switch (PixelFormat)
	{
		case ZE_TPF_R32F: // 4 byte
		case ZE_TPF_RGBA8:
		case ZE_TPF_RG16F:
		case ZE_TPF_RG16:
		case ZE_TPF_INTZ:
			Size = 4.0f;
			break;

		case ZE_TPF_R16F: // 2 byte
		case ZE_TPF_R16:	
		case ZE_TPF_LA8:
			Size = 2.0f;
			break;

		case ZE_TPF_RG32F: // 8 byte
		case ZE_TPF_RGBA16F:
		case ZE_TPF_RGBA16:	
			Size = 8.0f;
			break;

		case ZE_TPF_L8: // 1 byte
			Size = 1.0f;
			break;
			
		case ZE_TPF_NULL: // 0 byte
			Size = 0.0f;
			break;

		case ZE_TPF_DXT1: // 8 byte per 4x4 block
			Size = 8.0f / 16.0f;
			break;
		case ZE_TPF_DXT3: // 16 byte per 4x4 block
			Size = 16.0f / 16.0f;
			break;
		case ZE_TPF_DXT5: // 16 byte per 4x4 block
			Size = 16.0f / 16.0f;
			break;

	};
	return Size;
}

static ZESize CalculateTexture3DSize(ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZEUInt LevelCount, ZETexturePixelFormat PixelFormat)
{
	float TotalSize = 0;
	float PixelSize = GetAveragePixelSize(PixelFormat);
	
	for (ZESize LevelN = LevelCount; LevelCount > 0; --LevelN)
	{
		ZEUInt32 LevelIndex = (ZEUInt32)LevelN - 1;

		float PixelCount = (float)((Width >> LevelIndex) * (Height >> LevelIndex) * (Depth >> LevelIndex));
		TotalSize += PixelCount * PixelSize;
	}
	return (ZESize)TotalSize;
}

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

ZESize		ZED3D10Texture3D::GlobalSize = 0;
ZEUInt16	ZED3D10Texture3D::GlobalCount = 0;

ZED3D10Texture3D::ZED3D10Texture3D()
{
	D3D10Texture3D = NULL;
	D3D10ShaderResourceView = NULL;
}

ZED3D10Texture3D::~ZED3D10Texture3D()
{
	ZED3D_RELEASE(D3D10ShaderResourceView);
	ZED3D_RELEASE(D3D10Texture3D);

	GlobalSize -= CalculateTexture3DSize(Width, Height, Depth, LevelCount, PixelFormat);
	GlobalCount--;
}

bool ZED3D10Texture3D::IsEmpty() const
{
	return D3D10Texture3D == NULL;
}

const ID3D10Texture3D* ZED3D10Texture3D::GetTexture() const
{
	return D3D10Texture3D;
}

const ID3D10ShaderResourceView* ZED3D10Texture3D::GetResourceView() const
{
	return D3D10ShaderResourceView;
}

ZERenderTarget* ZED3D10Texture3D::CreateRenderTarget(ZEUInt MipLevel) const
{
	zeDebugCheck(IsEmpty(), "Texture not created!");
	zeDebugCheck(!Static, "Dynamic textures cannot be render target");
	zeDebugCheck(!RenderTarget, "Texture not created with render target flag");
	zeDebugCheck(MipLevel >= LevelCount, "Texture dont have specified Mipmap level");

	// Create render target view
	D3D10_RENDER_TARGET_VIEW_DESC RenderTargetDesc;
	RenderTargetDesc.Format = ConvertPixelFormat(PixelFormat);
	RenderTargetDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE3D;
	RenderTargetDesc.Texture3D.MipSlice = MipLevel;
	RenderTargetDesc.Texture3D.FirstWSlice = 0;
	RenderTargetDesc.Texture3D.WSize = -1;

	ID3D10RenderTargetView* D3D10RenderTargetView = NULL;
	if(FAILED(D3D10Device->CreateRenderTargetView(D3D10Texture3D, &RenderTargetDesc, &D3D10RenderTargetView)))
	{
		zeError("Cannot create render target view for static texture 3D.");
		return false;
	}

	ZED3D10RenderTarget* RenderTarget = new ZED3D10RenderTarget(Width >> MipLevel, Height >> MipLevel, PixelSize, PixelFormat, TextureType, D3D10RenderTargetView);
	
#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Render target view created. Texture3D: %p, MipLevel: %u, Width: %u, Height: %u", 
			this, MipLevel, RenderTarget->Width, RenderTarget->Height);
#endif

	return RenderTarget;
}

bool ZED3D10Texture3D::CreateDynamic(ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZEUInt LevelCount, ZETexturePixelFormat PixelFormat)
{
	zeDebugCheck(D3D10Texture3D, "Texture alread created");
	zeDebugCheck(Width == 0, "Width cannot be zero");
	zeDebugCheck(Height == 0, "Height cannot be zero");
	zeDebugCheck(Depth == 0, "Depth cannot be zero");
	zeDebugCheck(LevelCount != 1, "LevelCount must be one");
	zeDebugCheck(PixelFormat == ZE_TPF_NOTSET, "PixelFormat must be set");
	zeDebugCheck(Width > 2047 || Height > 2047 || Depth > 2047, "Texture dimensions exceeds the limits, 0-2047");
	
	D3D10_USAGE Usage;
	Usage = D3D10_USAGE_DYNAMIC;

	UINT CPUAccess = 0;
	CPUAccess |= D3D10_CPU_ACCESS_WRITE;
	
	UINT BindFlags = 0;
	BindFlags |= D3D10_BIND_SHADER_RESOURCE;

	D3D10_TEXTURE3D_DESC TextureDesc;
	TextureDesc.MiscFlags = 0;
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.Depth = Depth;
	TextureDesc.MipLevels = LevelCount;
	TextureDesc.Usage = Usage;
	TextureDesc.BindFlags = BindFlags;
	TextureDesc.CPUAccessFlags = CPUAccess;
	TextureDesc.Format = ConvertPixelFormat(PixelFormat);

	if (FAILED(D3D10Device->CreateTexture3D(&TextureDesc, NULL, &D3D10Texture3D)))
	{
		zeError("Can not create dynamic texture 3d.");
		return false;
	}

	// Create shader resource view
	D3D10_SHADER_RESOURCE_VIEW_DESC ResourceDesc;
	ResourceDesc.Format = TextureDesc.Format;
	ResourceDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE3D;
	ResourceDesc.Texture3D.MostDetailedMip = 0;
	ResourceDesc.Texture3D.MipLevels = LevelCount;
	
	if(FAILED(D3D10Device->CreateShaderResourceView(D3D10Texture3D, &ResourceDesc, &D3D10ShaderResourceView)))
	{
		zeError("Cannot create shader resource view for dynamic texture 3D.");
		return false;
	}

	this->Width = Width;
	this->Height = Height;
	this->Depth	= Depth;
	this->Static = false;
	this->RenderTarget = false;
	this->PixelFormat = PixelFormat;
	this->LevelCount = LevelCount;
	this->PixelSize = ZEVector3(1.0f / Width, 1.0f / Height, 1.0f / Depth);

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Dynamic cube texture created. Texture3D: %p, Width: %u, Height: %u, Depth: %u, LevelCount: %u, PixelFormat: %u", 
			this, Width, Height, Depth, LevelCount, PixelFormat);
#endif

	GlobalSize += CalculateTexture3DSize(Width, Height, Depth, LevelCount, PixelFormat);
	GlobalCount++;
	
	return true;
}

bool ZED3D10Texture3D::CreateStatic(ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZEUInt LevelCount, ZETexturePixelFormat PixelFormat, bool RenderTarget, ZETextureData* Data)
{
	zeDebugCheck(D3D10Texture3D, "Texture alread created");
	zeDebugCheck(Width == 0, "Width cannot be zero");
	zeDebugCheck(Height == 0, "Height cannot be zero");
	zeDebugCheck(Depth == 0, "Depth cannot be zero");
	zeDebugCheck(LevelCount == 0, "LevelCount cannot be zero");
	zeDebugCheck(PixelFormat == ZE_TPF_NOTSET, "PixelFormat must be valid");
	zeDebugCheck(RenderTarget && LevelCount != 1, "Render target's LevelCount must be one ");
	zeDebugCheck(RenderTarget && Data != NULL, "Render target specified, ignoring intial data.");
	zeDebugCheck(Width > 2047 || Height > 2047 || Depth > 2047, "Texture dimensions exceeds the limits, 0-2047");

	D3D10_USAGE Usage;
	Usage = RenderTarget ? D3D10_USAGE_DEFAULT : D3D10_USAGE_IMMUTABLE;

	UINT CPUAccess = 0;
	
	UINT BindFlags = 0;
	BindFlags |= D3D10_BIND_SHADER_RESOURCE;
	BindFlags |= RenderTarget ? D3D10_BIND_RENDER_TARGET : 0;

	D3D10_TEXTURE3D_DESC TextureDesc;
	TextureDesc.MiscFlags = 0;
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.Depth = Depth;
	TextureDesc.MipLevels = LevelCount;
	TextureDesc.Usage = Usage;
	TextureDesc.BindFlags = BindFlags;
	TextureDesc.CPUAccessFlags = CPUAccess;
	TextureDesc.Format = ConvertPixelFormat(PixelFormat);
	
	if (FAILED(D3D10Device->CreateTexture3D(&TextureDesc, NULL, &D3D10Texture3D)))
	{
		zeError("Can not create dynamic texture 3d.");
		return false;
	}

	// Create shader resource view
	D3D10_SHADER_RESOURCE_VIEW_DESC ResourceDesc;
	ResourceDesc.Format = TextureDesc.Format;
	ResourceDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE3D;
	ResourceDesc.Texture3D.MostDetailedMip = 0;
	ResourceDesc.Texture3D.MipLevels = LevelCount;
	
	if(FAILED(D3D10Device->CreateShaderResourceView(D3D10Texture3D, &ResourceDesc, &D3D10ShaderResourceView)))
	{
		zeError("Cannot create shader resource view for dynamic texture 3D.");
		return false;
	}

	this->Width = Width;
	this->Height = Height;
	this->Depth	= Depth;
	this->Static = true;
	this->RenderTarget = RenderTarget;
	this->PixelFormat = PixelFormat;
	this->LevelCount = LevelCount;
	this->PixelSize = ZEVector3(1.0f / Width, 1.0f / Height, 1.0f / Depth);

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Static cube texture created. Texture3D: %p, Width: %u, Height: %u, Depth: %u, LevelCount: %u, PixelFormat: %u", 
			this, Width, Height, Depth, LevelCount, PixelFormat);
#endif

	GlobalSize += CalculateTexture3DSize(Width, Height, Depth, LevelCount, PixelFormat);
	GlobalCount++;
	
	return true;
}

bool ZED3D10Texture3D::Lock(void** Buffer, ZESize* RowPitch, ZESize* SlicePitch, ZEUInt Level)
{
	zeDebugCheck(*Buffer == NULL, "Buffer cannot be null");
	zeDebugCheck(RowPitch == NULL, "RowPitch cannot be null");
	zeDebugCheck(SlicePitch == NULL, "SlicePitch cannot be null");
	zeDebugCheck(D3D10Texture3D == NULL, "Cannot lock empty texture 3D");
	zeDebugCheck(Static, "Texture must be dynamic to lock or unlock");
	zeDebugCheck(Level >= LevelCount, "Level is greater than LevelCount");

	D3D10_MAPPED_TEXTURE3D Mapped;
	if (FAILED(D3D10Texture3D->Map(D3D10CalcSubresource(Level, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &Mapped)))
	{
		zeError("Cannot lock texture 3d.");
		return false;
	}
	
	*Buffer = Mapped.pData;
	*RowPitch = (ZESize)Mapped.RowPitch;
	*SlicePitch = (ZESize)Mapped.DepthPitch;

	return true;
}

bool ZED3D10Texture3D::Unlock(ZEUInt Level)
{
	zeDebugCheck(Static, "Texture must be dynamic to lock or unlock");
	zeDebugCheck(D3D10Texture3D == NULL, "Cannot unlock empty texture 3D");
	zeDebugCheck(Level >= LevelCount, "Level is greater than LevelCount");

	D3D10Texture3D->Unmap(D3D10CalcSubresource(Level, 0, 1));
	
	return true;
}
