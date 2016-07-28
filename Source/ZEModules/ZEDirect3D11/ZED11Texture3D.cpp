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

#include "ZEError.h"
#include "ZEPointer\ZEPointer.h"
#include "ZED11RenderTarget.h"

bool ZED11Texture3D::Initialize(ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZEUInt Level, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlags, const void* Data)
{
	zeDebugCheck(Texture3D != NULL, "Texture already created.");

	D3D11_TEXTURE3D_DESC TextureDesc = {};
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.Depth = Depth;
	TextureDesc.MipLevels = Level;
	TextureDesc.Format = ConvertFormat(Format);
	TextureDesc.Usage = ConvertUsage(Usage);
	TextureDesc.BindFlags = ConvertBindFlags(BindFlags);
	TextureDesc.CPUAccessFlags = ConvertUsageToCpuAccessFlags(Usage);

	ZEPointer<D3D11_SUBRESOURCE_DATA, ZEDeletorArray<D3D11_SUBRESOURCE_DATA>> SubresourceData;
	if (Data != NULL)
	{
		SubresourceData = new D3D11_SUBRESOURCE_DATA[Level];
		const ZEGRFormatDefinition* FormatInfo = ZEGRFormatDefinition::GetDefinition(Format);
		ZESize Offset = 0;
		for (ZEUInt I = 0; I < Level; I++)
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

	HRESULT Result = GetDevice()->CreateTexture3D(&TextureDesc, SubresourceData, &Texture3D);
	if (FAILED(Result))
	{
		zeError("Texture 3D creation failed. ErrorCode: %d.", Result);
		return false;
	}

	if (BindFlags.GetFlags(ZEGR_RBF_SHADER_RESOURCE))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceDesc;
		ShaderResourceDesc.Format = TextureDesc.Format;
		ShaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		ShaderResourceDesc.Texture3D.MostDetailedMip = 0;
		ShaderResourceDesc.Texture3D.MipLevels = Level;

		Result = GetDevice()->CreateShaderResourceView(Texture3D, &ShaderResourceDesc, &ShaderResourceView);
		if (FAILED(Result))
		{
			zeError("Texture 3D shader resource view creation failed. ErrorCode: %d.", Result);
			return false;
		}
	}

	if (BindFlags.GetFlags(ZEGR_RBF_UNORDERED_ACCESS))
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC UnorderedAccessDesc;
		UnorderedAccessDesc.Format = TextureDesc.Format;
		UnorderedAccessDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
		UnorderedAccessDesc.Texture3D.FirstWSlice = 0;
		UnorderedAccessDesc.Texture3D.WSize = Depth;
		UnorderedAccessDesc.Texture3D.MipSlice = 0;

		Result = GetDevice()->CreateUnorderedAccessView(Texture3D, &UnorderedAccessDesc, &UnorderedAccessView);
		if(FAILED(Result))
		{
			zeError("Texture 3D unordered access view creation failed. ErrorCode: %d.", Result);
			return false;
		}
	}

	return ZEGRTexture3D::Initialize(Width, Height, Depth, Level, Format, Usage, BindFlags, Data);
}

void ZED11Texture3D::Deinitialize()
{
	ZEGR_RELEASE(Texture3D);
	ZEGR_RELEASE(ShaderResourceView);
	ZEGR_RELEASE(UnorderedAccessView);

	ZEGRTexture3D::Deinitialize();
}

ID3D11Resource* ZED11Texture3D::GetResource() const
{
	ID3D11Resource* Resource = NULL;

	if (Texture3D != NULL)
		Texture3D->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(&Resource));

	return Resource;
}

ID3D11Texture3D* ZED11Texture3D::GetTexture() const
{
	return Texture3D;
}

ID3D11ShaderResourceView* ZED11Texture3D::GetShaderResourceView() const
{
	return ShaderResourceView;
}

ID3D11UnorderedAccessView* ZED11Texture3D::GetUnorderedAccessView() const
{
	return UnorderedAccessView;
}

ZED11Texture3D::ZED11Texture3D()
{
	Texture3D = NULL;
	ShaderResourceView = NULL;
	UnorderedAccessView = NULL;
}

ZED11Texture3D::~ZED11Texture3D()
{
	Deinitialize();
}

void ZED11Texture3D::UpdateSubResource(ZEUInt DestLevel, const void* SrcData, ZESize SrcRowPitch, ZESize SrcDepthPitch)
{
	zeDebugCheck(DestLevel >= GetLevelCount(), "There is no such a texture level.");

	GetMainContext()->UpdateSubresource(Texture3D, DestLevel, NULL, SrcData, SrcRowPitch, SrcDepthPitch);
}

const ZEGRRenderTarget* ZED11Texture3D::GetRenderTarget(ZEUInt Depth, ZEUInt Level) const
{
	zeDebugCheck(Texture3D == NULL, "Texture not created.");
	zeDebugCheck(Level >= GetLevelCount(), "Texture doesnt have specified Mipmap level.");
	zeDebugCheck(Depth >= GetDepth(), "Depth is out of range");
	zeDebugCheck(!GetResourceBindFlags().GetFlags(ZEGR_RBF_RENDER_TARGET), "Texture had not been created with render target bind flag");

	ZEUInt Index = Depth * GetLevelCount() + Level;

	if (RenderTargets.GetCount() > Index)
		return RenderTargets[Index];

	// Create render target view
	D3D11_RENDER_TARGET_VIEW_DESC RenderTargetDesc;
	RenderTargetDesc.Format = ZED11Texture3D::ConvertFormat(GetFormat());
	RenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
	RenderTargetDesc.Texture3D.WSize = 1;
	RenderTargetDesc.Texture3D.FirstWSlice = Depth;
	RenderTargetDesc.Texture3D.MipSlice = Level;

	ID3D11RenderTargetView* RenderTargetView = NULL;
	HRESULT Result = GetDevice()->CreateRenderTargetView(Texture3D, &RenderTargetDesc, &RenderTargetView);
	if(FAILED(Result))
	{
		zeError("Texture 3D render target creation failed. ErrorCode: %d.", Result);
		return NULL;
	}
	
	ZED11RenderTarget* RenderTarget = new ZED11RenderTarget(GetWidth() >> Level, GetHeight() >> Level, GetFormat(), RenderTargetView);
	RenderTarget->SetOwner(this);

	RenderTargets.Add(RenderTarget);

	return RenderTarget;
}

bool ZED11Texture3D::Lock(void** Buffer, ZESize* RowPitch, ZESize* SlicePitch, ZEUInt Level)
{
	zeDebugCheck(Texture3D == NULL, "Texture is not initialized.");
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_GPU_READ_CPU_WRITE && GetLevelCount() > 1, "Dynamic texture having subresource cannot be mapped");
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_GPU_READ_ONLY || GetResourceUsage() == ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, "Texture had not been created for mapping");

	D3D11_MAP MapType;
	if (GetResourceUsage() == ZEGR_RU_CPU_READ_WRITE)
		MapType = D3D11_MAP_READ_WRITE;
	else if (GetResourceUsage() == ZEGR_RU_GPU_READ_CPU_WRITE)
		MapType = D3D11_MAP_WRITE_DISCARD;
	else
		return false;

	LockContext();

	D3D11_MAPPED_SUBRESOURCE MapData = {};
	HRESULT Result = GetMainContext()->Map(Texture3D, Level, MapType, 0, &MapData);

	UnlockContext();

	if (FAILED(Result))
	{
		zeError("Cannot lock texture 3d resource.");
		return false;
	}

	*Buffer = MapData.pData;
	if (RowPitch != NULL)
		*RowPitch = MapData.RowPitch;
	if (SlicePitch != NULL)
		*SlicePitch = MapData.DepthPitch;

	return true;
}

void ZED11Texture3D::Unlock(ZEUInt Level)
{
	zeDebugCheck(Texture3D == NULL, "Texture is not initialized.");

	LockContext();

	GetMainContext()->Unmap(Texture3D, Level);

	UnlockContext();
}
