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

#include "ZEError.h"
#include "ZEPointer\ZEPointer.h"
#include "ZED11Module.h"
#include "ZED11RenderTarget.h"
#include "ZED11Texture2D.h"

bool ZED11TextureCube::Initialize(ZEUInt Length, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlags, const void* Data)
{
	zeDebugCheck(Texture != NULL, "Texture already created.");

	D3D11_TEXTURE2D_DESC TextureDesc;
	TextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	TextureDesc.ArraySize = 6;
	TextureDesc.Width = Length;
	TextureDesc.Height = Length;
	TextureDesc.MipLevels = LevelCount;
	TextureDesc.Format = ConvertFormat(Format);
	TextureDesc.Usage = ConvertUsage(Usage);
	TextureDesc.BindFlags = ConvertBindFlags(BindFlags);
	TextureDesc.CPUAccessFlags = ConvertUsageToCpuAccessFlags(Usage);
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;

	ZEPointer<D3D11_SUBRESOURCE_DATA, ZEDeletorArray<D3D11_SUBRESOURCE_DATA>> SubresourceData;
	if (Data != NULL)
	{
		SubresourceData = new D3D11_SUBRESOURCE_DATA[6];
		const ZEGRFormatDefinition* FormatInfo = ZEGRFormatDefinition::GetDefinition(Format);
		ZESize Offset = 0;
		for (ZEUInt J = 0; J < 6; J++)
		{
			ZEUInt SubresourceIndex = J * LevelCount;
			for (ZEUInt I = 0; I < LevelCount; I++)
			{
				ZEUInt NewWidth = Length >> I;
				ZEUInt NewHeight = Length >> I;

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

	HRESULT Result = GetDevice()->CreateTexture2D(&TextureDesc, SubresourceData, &Texture);
	if (FAILED(Result))
	{
		zeError("Texture cube creation failed. ErrorCode: %d.", Result);
		return false;
	}

	if (BindFlags.GetFlags(ZEGR_RBF_SHADER_RESOURCE))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC ResourceDesc;
		ResourceDesc.Format = TextureDesc.Format;
		ResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		ResourceDesc.TextureCube.MostDetailedMip = 0;
		ResourceDesc.TextureCube.MipLevels = LevelCount;

		Result = GetDevice()->CreateShaderResourceView(Texture, &ResourceDesc, &ShaderResourceView);
		if(FAILED(Result))
		{
			zeError("Texture cube shader resource view creation failed. ErrorCode: %d.", Result);
			return false;
		}
	}

	return ZEGRTextureCube::Initialize(Length, LevelCount, Format, Usage, BindFlags, Data);
}

void ZED11TextureCube::Deinitialize()
{
	ZEGR_RELEASE(Texture);
	ZEGR_RELEASE(ShaderResourceView);

	ZEGRTextureCube::Deinitialize();
}

ID3D11Texture2D* ZED11TextureCube::GetTexture() const
{
	return Texture;
}

ID3D11ShaderResourceView* ZED11TextureCube::GetShaderResourceView() const
{
	return ShaderResourceView;
}

ZED11TextureCube::ZED11TextureCube()
{
	Texture = NULL;
	ShaderResourceView = NULL;
}

ZED11TextureCube::~ZED11TextureCube()
{
	Deinitialize();
}

void ZED11TextureCube::UpdateSubResource(ZEGRTextureCubeFace DestFace, ZEUInt DestLevel, const void* SrcData, ZESize SrcRowPitch)
{
	zeDebugCheck(DestFace >= 6, "There is no such a cube texture face.");
	zeDebugCheck(DestLevel >= GetLevelCount(), "There is no such a texture level.");

	GetMainContext()->UpdateSubresource(Texture, DestFace * GetLevelCount() + DestLevel, NULL, SrcData, SrcRowPitch, 0);
}

const ZEGRRenderTarget* ZED11TextureCube::GetRenderTarget(ZEGRTextureCubeFace Face, ZEUInt Level) const
{
	zeDebugCheck(Texture == NULL, "Texture is not initialized.");
	zeCheckError(Face >= 6, NULL, "There is no such a cube texture face.");
	zeCheckError(Level >= GetLevelCount(), NULL, "Texture dont have specified Mipmap level");
	zeDebugCheck(!GetResourceBindFlags().GetFlags(ZEGR_RBF_RENDER_TARGET), "Texture had not been created with render target bind flag");

	ZEUInt Index = Face * GetLevelCount() + Level;

	if (RenderTargets.GetCount() > Index)
		return RenderTargets[Index];

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
		zeError("Texture cube render target creation failed. ErrorCode: %d.", Result);
		return NULL;
	}

	ZED11RenderTarget* RenderTarget = new ZED11RenderTarget(GetLength() >> Level, GetLength() >> Level, GetFormat(), RenderTargetView);
	RenderTarget->SetOwner(this);

	RenderTargets.Add(RenderTarget);

	return RenderTarget;
}
