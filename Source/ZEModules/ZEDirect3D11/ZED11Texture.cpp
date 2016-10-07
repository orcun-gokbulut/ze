//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Texture.cpp
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

#include "ZED11Texture.h"

#include "ZEPointer\ZEPointer.h"
#include "ZED11RenderTarget.h"
#include "ZED11DepthStencilBuffer.h"

static DXGI_FORMAT ConvertDepthStencilToTypelessFormat(ZEGRFormat Format)
{
	switch (Format)
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

static DXGI_FORMAT ConvertDepthStencilToShaderResourceViewFormat(ZEGRFormat Format)
{
	switch (Format)
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

bool ZED11Texture::Initialize2D(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEUInt ArrayCount, ZEUInt SampleCount, const void* Data)
{
	bool ShaderResourceDepthStencil = BindFlags.GetFlags(ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_DEPTH_STENCIL);

	D3D11_TEXTURE2D_DESC TextureDesc = {};
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.MipLevels = LevelCount;
	TextureDesc.ArraySize = ArrayCount;
	TextureDesc.Format = ShaderResourceDepthStencil ? ConvertDepthStencilToTypelessFormat(Format) : ConvertFormat(Format);;
	TextureDesc.SampleDesc.Count = SampleCount;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = ConvertUsage(Usage);
	TextureDesc.BindFlags = ConvertBindFlags(BindFlags);
	TextureDesc.CPUAccessFlags = ConvertUsageToCpuAccessFlags(Usage);
	TextureDesc.MiscFlags = (GetTextureType() == ZEGR_TT_CUBE) ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

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

	HRESULT Result = GetDevice()->CreateTexture2D(&TextureDesc, SubresourceData, reinterpret_cast<ID3D11Texture2D**>(&Resource));
	if (FAILED(Result))
	{
		zeError("Texture creation failed. ErrorCode: 0x%X.", Result);
		return false;
	}

	if (BindFlags.GetFlags(ZEGR_RBF_SHADER_RESOURCE))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceDesc = {};
		ShaderResourceDesc.Format = ShaderResourceDepthStencil ? ConvertDepthStencilToShaderResourceViewFormat(Format) : TextureDesc.Format;
		if (ArrayCount > 1)
		{
			if (SampleCount > 1)
			{
				ShaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
				ShaderResourceDesc.Texture2DMSArray.ArraySize = ArrayCount;
				ShaderResourceDesc.Texture2DMSArray.FirstArraySlice = 0;
			}
			else if (GetTextureType() == ZEGR_TT_CUBE)
			{
				ShaderResourceDesc.ViewDimension = D3D10_1_SRV_DIMENSION_TEXTURECUBE;
				ShaderResourceDesc.TextureCube.MostDetailedMip = 0;
				ShaderResourceDesc.TextureCube.MipLevels = LevelCount;
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

		Result = GetDevice()->CreateShaderResourceView(Resource, &ShaderResourceDesc, &ShaderResourceView);
		if (FAILED(Result))
		{
			zeError("Texture2D shader resource view creation failed. ErrorCode: 0x%X.", Result);
			return false;
		}
	}

	if (BindFlags.GetFlags(ZEGR_RBF_UNORDERED_ACCESS))
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC UnorderedAccessDesc = {};
		UnorderedAccessDesc.Format = TextureDesc.Format;
		if (ArrayCount > 1)
		{
			UnorderedAccessDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			UnorderedAccessDesc.Texture2DArray.FirstArraySlice = 0;
			UnorderedAccessDesc.Texture2DArray.ArraySize = ArrayCount;
			UnorderedAccessDesc.Texture2DArray.MipSlice = 0;
		}
		else
		{
			UnorderedAccessDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			UnorderedAccessDesc.Texture2D.MipSlice = 0;
		}

		Result = GetDevice()->CreateUnorderedAccessView(Resource, &UnorderedAccessDesc, &UnorderedAccessView);
		if (FAILED(Result))
		{
			zeError("Texture2D unordered access view creation failed. ErrorCode: 0x%X.", Result);
			return false;
		}
	}

	return true;
}

bool ZED11Texture::Initialize3D(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEUInt Depth, ZEUInt SampleCount, const void* Data)
{
	D3D11_TEXTURE3D_DESC TextureDesc = {};
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.Depth = Depth;
	TextureDesc.MipLevels = LevelCount;
	TextureDesc.Format = ConvertFormat(Format);
	TextureDesc.Usage = ConvertUsage(Usage);
	TextureDesc.BindFlags = ConvertBindFlags(BindFlags);
	TextureDesc.CPUAccessFlags = ConvertUsageToCpuAccessFlags(Usage);

	ZEPointer<D3D11_SUBRESOURCE_DATA, ZEDeletorArray<D3D11_SUBRESOURCE_DATA>> SubresourceData;
	if (Data != NULL)
	{
		SubresourceData = new D3D11_SUBRESOURCE_DATA[LevelCount];
		const ZEGRFormatDefinition* FormatInfo = ZEGRFormatDefinition::GetDefinition(Format);
		ZESize Offset = 0;
		for (ZEUInt I = 0; I < LevelCount; I++)
		{
			ZEUInt NewWidth = Width >> I;
			ZEUInt NewHeight = Height >> I;

			NewWidth = (FormatInfo->Compressed && NewWidth < FormatInfo->BlockDimension) ? FormatInfo->BlockDimension : NewWidth;
			NewHeight = (FormatInfo->Compressed && NewHeight < FormatInfo->BlockDimension) ? FormatInfo->BlockDimension : NewHeight;

			ZESize RowPitch = (NewWidth / FormatInfo->BlockDimension) * FormatInfo->BlockSize;
			ZESize SlicePitch = RowPitch * (NewHeight / FormatInfo->BlockDimension);

			SubresourceData[I].pSysMem = static_cast<const ZEBYTE*>(Data) + Offset;
			SubresourceData[I].SysMemPitch = RowPitch;
			SubresourceData[I].SysMemSlicePitch = SlicePitch;

			Offset += SlicePitch * (Depth >> I);
		}
	}

	HRESULT Result = GetDevice()->CreateTexture3D(&TextureDesc, SubresourceData, reinterpret_cast<ID3D11Texture3D**>(&Resource));
	if (FAILED(Result))
	{
		zeError("Texture3D creation failed. ErrorCode: %d.", Result);
		return false;
	}

	if (BindFlags.GetFlags(ZEGR_RBF_SHADER_RESOURCE))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceDesc = {};
		ShaderResourceDesc.Format = TextureDesc.Format;
		ShaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		ShaderResourceDesc.Texture3D.MostDetailedMip = 0;
		ShaderResourceDesc.Texture3D.MipLevels = LevelCount;

		Result = GetDevice()->CreateShaderResourceView(Resource, &ShaderResourceDesc, &ShaderResourceView);
		if (FAILED(Result))
		{
			zeError("Texture3D shader resource view creation failed. ErrorCode: 0x%X.", Result);
			return false;
		}
	}

	if (BindFlags.GetFlags(ZEGR_RBF_UNORDERED_ACCESS))
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC UnorderedAccessDesc = {};
		UnorderedAccessDesc.Format = TextureDesc.Format;
		UnorderedAccessDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
		UnorderedAccessDesc.Texture3D.FirstWSlice = 0;
		UnorderedAccessDesc.Texture3D.WSize = Depth;
		UnorderedAccessDesc.Texture3D.MipSlice = 0;

		Result = GetDevice()->CreateUnorderedAccessView(Resource, &UnorderedAccessDesc, &UnorderedAccessView);
		if(FAILED(Result))
		{
			zeError("Texture3D unordered access view creation failed. ErrorCode: 0x%X.", Result);
			return false;
		}
	}

	return true;
}

bool ZED11Texture::Initialize(ZEGRTextureType TextureType, ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEUInt DepthOrArrayCount, ZEUInt SampleCount, const void* Data)
{
	if (!ZEGRTexture::Initialize(TextureType, Width, Height, LevelCount, Format, Usage, BindFlags, DepthOrArrayCount, SampleCount, Data))
		return false;

	switch (TextureType)
	{
		case ZEGR_TT_2D:
		case ZEGR_TT_CUBE:
			return Initialize2D(Width, Height, LevelCount, Format, Usage, BindFlags, DepthOrArrayCount, SampleCount, Data);

		case ZEGR_TT_3D:
			return Initialize3D(Width, Height, LevelCount, Format, Usage, BindFlags, DepthOrArrayCount, SampleCount, Data);

		default:
			return false;
	}

	return true;
}

void ZED11Texture::Deinitialize()
{
	ZEGR_RELEASE(Resource);
	ZEGR_RELEASE(ShaderResourceView);
	ZEGR_RELEASE(UnorderedAccessView);

	ZEGRTexture::Deinitialize();
}

const ZEGRRenderTarget* ZED11Texture::GetRenderTarget(ZEUInt DepthOrArrayIndex, ZEUInt Level) const
{
	zeDebugCheck(Level >= GetLevelCount(), "Level is out of range");
	zeDebugCheck(DepthOrArrayIndex >= GetDepthOrArrayCount(), "Array index is out of range");
	zeDebugCheck(GetTextureType() == ZEGR_TT_3D && (DepthOrArrayIndex >= (GetDepthOrArrayCount() >> Level)), "Array index is out of range");
	zeDebugCheck(!GetResourceBindFlags().GetFlags(ZEGR_RBF_RENDER_TARGET), "Texture had not been created with render target bind flag");

	ZEUInt Index = 0;
	switch (GetTextureType())
	{
		case ZEGR_TT_2D:
		case ZEGR_TT_CUBE:
			{
				Index = DepthOrArrayIndex * GetLevelCount() + Level;
				
				break;
			}
		case ZEGR_TT_3D:
			{
				for (ZEUInt I = 0; I < Level; I++)
					Index += (GetDepthOrArrayCount() >> I);
				
				break;
			}
		default:
			break;
	}

	if (RenderTargets.GetCount() > Index)
		return RenderTargets[Index];

	D3D11_RENDER_TARGET_VIEW_DESC RenderTargetViewDesc = {};
	RenderTargetViewDesc.Format = ConvertFormat(GetFormat());

	if (GetDepthOrArrayCount() > 1)
	{
		if (GetSampleCount() > 1)
		{
			RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
			RenderTargetViewDesc.Texture2DMSArray.FirstArraySlice = DepthOrArrayIndex;
			RenderTargetViewDesc.Texture2DMSArray.ArraySize = 1;
		}
		else if (GetTextureType() == ZEGR_TT_3D)
		{
			RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
			RenderTargetViewDesc.Texture3D.FirstWSlice = DepthOrArrayIndex;
			RenderTargetViewDesc.Texture3D.WSize = 1;
			RenderTargetViewDesc.Texture3D.MipSlice = Level;
		}
		else
		{	
			RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			RenderTargetViewDesc.Texture2DArray.FirstArraySlice = DepthOrArrayIndex;
			RenderTargetViewDesc.Texture2DArray.ArraySize = 1;
			RenderTargetViewDesc.Texture2DArray.MipSlice = Level;
		}
	}
	else
	{
		if (GetSampleCount() > 1)
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
	HRESULT Result = GetDevice()->CreateRenderTargetView(Resource, &RenderTargetViewDesc, &RenderTargetView);
	if (FAILED(Result))
	{
		zeError("Texture render target creation failed. ErrorCode: 0x%X.", Result);
		return NULL;
	}

	ZED11RenderTarget* RenderTarget = new ZED11RenderTarget(GetWidth() >> Level, GetHeight() >> Level, GetFormat(), RenderTargetView);
	RenderTarget->SetOwner(this);

	RenderTargets.Add(RenderTarget);

	return RenderTarget;
}

const ZEGRDepthStencilBuffer* ZED11Texture::GetDepthStencilBuffer(bool ReadOnly, ZEUInt DepthOrArrayIndex) const
{
	zeDebugCheck(DepthOrArrayIndex >= GetDepthOrArrayCount(), "Array index is out of range");
	zeDebugCheck(!GetResourceBindFlags().GetFlags(ZEGR_RBF_DEPTH_STENCIL), "Texture had not been created with depth-stencil bind flag");

	UINT Flags = 0;

	ZEUInt Index = DepthOrArrayIndex * 2;
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
	DepthStencilViewDesc.Format = ConvertFormat(GetFormat());
	DepthStencilViewDesc.Flags = Flags;

	if (GetDepthOrArrayCount() > 1)
	{
		if (GetSampleCount() > 1)
		{
			DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
			DepthStencilViewDesc.Texture2DMSArray.FirstArraySlice = DepthOrArrayIndex;
			DepthStencilViewDesc.Texture2DMSArray.ArraySize = 1;
		}
		else
		{
			DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			DepthStencilViewDesc.Texture2DArray.FirstArraySlice = DepthOrArrayIndex;
			DepthStencilViewDesc.Texture2DArray.ArraySize = 1;
			DepthStencilViewDesc.Texture2DArray.MipSlice = 0;
		}
	}
	else
	{
		if (GetSampleCount() > 1)
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
	HRESULT Result = GetDevice()->CreateDepthStencilView(Resource, &DepthStencilViewDesc, &DepthStencilView);
	if (FAILED(Result))
	{
		zeError("Texture depth stencil creation failed. ErrorCode: 0x%X.", Result);
		return NULL;
	}

	ZED11DepthStencilBuffer* DepthStencilBuffer = new ZED11DepthStencilBuffer(GetWidth(), GetHeight(), GetFormat(), DepthStencilView);
	DepthStencilBuffer->SetOwner(this);
	DepthStencilBuffer->SetReadOnly(ReadOnly);

	DepthStencilBuffers[Index] = DepthStencilBuffer;

	return DepthStencilBuffer;
}

bool ZED11Texture::Map(ZEGRResourceMapType MapType, void** Buffer, ZESize* RowPitch, ZESize* DepthPitch, ZEUInt ArrayIndex, ZEUInt Level)
{
	zeDebugCheck(Resource == NULL, "Texture is not initialized.");
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_IMMUTABLE || GetResourceUsage() == ZEGR_RU_STATIC, "Texture is not mappable");
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_DYNAMIC && (GetLevelCount() > 1 || GetDepthOrArrayCount() > 1), "Dynamic texture2d having subresource or texture3d cannot be mapped");
	
	D3D11_MAP D11MapType = ConvertMapType(MapType);
	D3D11_MAPPED_SUBRESOURCE MapData = {};

	LockContext();
	HRESULT Result = GetMainContext()->Map(Resource, ArrayIndex * GetLevelCount() + Level, D11MapType, 0, &MapData);
	UnlockContext();
	if (FAILED(Result))
	{
		zeError("Mapping texture failed. Error: 0x%X", Result);
		return false;
	}

	*Buffer = MapData.pData;
	if (RowPitch != NULL)
		*RowPitch = MapData.RowPitch;
	if (DepthPitch != NULL)
		*DepthPitch = MapData.DepthPitch;

	return true;
}

void ZED11Texture::Unmap(ZEUInt ArrayIndex, ZEUInt Level)
{
	zeDebugCheck(Resource == NULL, "Texture is not initialized.");
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_IMMUTABLE || GetResourceUsage() == ZEGR_RU_STATIC, "Texture is not mappable");
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_DYNAMIC && (GetLevelCount() > 1 || GetDepthOrArrayCount() > 1), "Dynamic texture2d having subresource or texture3d cannot be mapped");

	LockContext();
	GetMainContext()->Unmap(Resource, 0);
	UnlockContext();
}

void ZED11Texture::Update(const void* SrcData, ZESize SrcRowPitch, ZESize SrcDepthPitch, ZEUInt DestArrayIndex, ZEUInt DestLevel, const ZERect* DestRect)
{
	zeDebugCheck(Resource == NULL, "Texture is not initialized.");
	zeDebugCheck(GetResourceBindFlags().GetFlags(ZEGR_RBF_DEPTH_STENCIL), "Depth texture cannot be updated");
	zeDebugCheck(GetSampleCount() > 1, "Multi-sampled texture cannot be update");
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_IMMUTABLE || GetResourceUsage() == ZEGR_RU_DYNAMIC || GetResourceUsage() == ZEGR_RU_STAGING, "Texture is not non-mappable");
	zeCheckError(DestArrayIndex >= GetDepthOrArrayCount(), ZE_VOID, "Destination array index is out of range.");
	zeCheckError(DestLevel >= GetLevelCount(), ZE_VOID, "Destination level is out of range.");

	D3D11_BOX* D11Box = NULL;
	if (DestRect != NULL)
	{
		D3D11_BOX DestBox = {};
		DestBox.left = DestRect->x;
		DestBox.top = DestRect->y;
		DestBox.right = DestRect->x + DestRect->Width;
		DestBox.bottom = DestRect->y + DestRect->Height;
		DestBox.front = 0;
		DestBox.back = 1;

		D11Box = &DestBox;
	}

	LockContext();
	GetMainContext()->UpdateSubresource(Resource, DestArrayIndex * GetLevelCount() + DestLevel, D11Box, SrcData, SrcRowPitch, SrcDepthPitch);
	UnlockContext();
}

ZED11Texture::ZED11Texture()
{
	Resource = NULL;
	ShaderResourceView = NULL;
	UnorderedAccessView = NULL;
}

ZED11Texture::~ZED11Texture()
{
	Deinitialize();
}
