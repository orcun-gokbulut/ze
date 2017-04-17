//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRTexture.cpp
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

#include "ZEGRTexture.h"

#include "ZEFile/ZEPathInfo.h"
#include "ZEMath/ZEMath.h"
#include "ZEPointer/ZEPointer.h"
#include "ZEGRGraphicsModule.h"
#include "ZEGRContext.h"
#include "ZEGRDepthStencilBuffer.h"
#include "ZEGRRenderTarget.h"
#include "ZEResource/ZERSTemplates.h"

#include <DirectXTex.h>

using namespace DirectX;


static DXGI_FORMAT ConvertFormat(ZEGRFormat Format)
{
	switch (Format)
	{
		default:
		case ZEGR_TF_NONE:
			return DXGI_FORMAT_UNKNOWN;

		case ZEGR_TF_R8_SINT:
			return DXGI_FORMAT_R8_SINT;

		case ZEGR_TF_R8_UINT:
			return DXGI_FORMAT_R8_UINT;

		case ZEGR_TF_R8_UNORM:
			return DXGI_FORMAT_R8_UNORM;

		case ZEGR_TF_R8_SNORM:
			return DXGI_FORMAT_R8_SNORM;



		case ZEGR_TF_R8G8_UINT:
			return DXGI_FORMAT_R8G8_UINT;

		case ZEGR_TF_R8G8_SINT:
			return DXGI_FORMAT_R8G8_SINT;

		case ZEGR_TF_R8G8_UNORM:
			return DXGI_FORMAT_R8G8_UNORM;

		case ZEGR_TF_R8G8_SNORM:
			return DXGI_FORMAT_R8G8_SNORM;



		case ZEGR_TF_R8G8B8A8_UINT:
			return DXGI_FORMAT_R8G8B8A8_UINT;

		case ZEGR_TF_R8G8B8A8_SINT:
			return DXGI_FORMAT_R8G8B8A8_SINT;

		case ZEGR_TF_R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;

		case ZEGR_TF_R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

		case ZEGR_TF_R8G8B8A8_SNORM:
			return DXGI_FORMAT_R8G8B8A8_SNORM;



		case ZEGR_TF_R16_UINT:
			return DXGI_FORMAT_R16_UINT;

		case ZEGR_TF_R16_SINT:
			return DXGI_FORMAT_R16_SINT;

		case ZEGR_TF_R16_UNORM:
			return DXGI_FORMAT_R16_UNORM;

		case ZEGR_TF_R16_SNORM:
			return DXGI_FORMAT_R16_SNORM;

		case ZEGR_TF_R16_FLOAT:
			return DXGI_FORMAT_R16_FLOAT;


		case ZEGR_TF_R16G16_UINT:
			return DXGI_FORMAT_R16G16_UINT;

		case ZEGR_TF_R16G16_SINT:
			return DXGI_FORMAT_R16G16_SINT;

		case ZEGR_TF_R16G16_UNORM:
			return DXGI_FORMAT_R16G16_UNORM;

		case ZEGR_TF_R16G16_SNORM:
			return DXGI_FORMAT_R16G16_SNORM;

		case ZEGR_TF_R16G16_FLOAT:
			return DXGI_FORMAT_R16G16_FLOAT;



		case ZEGR_TF_R16G16B16A16_UINT:
			return DXGI_FORMAT_R16G16B16A16_UINT;

		case ZEGR_TF_R16G16B16A16_SINT:
			return DXGI_FORMAT_R16G16B16A16_SINT;

		case ZEGR_TF_R16G16B16A16_UNORM:
			return DXGI_FORMAT_R16G16B16A16_UNORM;

		case ZEGR_TF_R16G16B16A16_SNORM:
			return DXGI_FORMAT_R16G16B16A16_SNORM;

		case ZEGR_TF_R16G16B16A16_FLOAT:
			return DXGI_FORMAT_R16G16B16A16_FLOAT;


		case ZEGR_TF_R32_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;

		case ZEGR_TF_R32_SINT:
			return DXGI_FORMAT_R32_SINT;

		case ZEGR_TF_R32_UINT:
			return DXGI_FORMAT_R32_UINT;


		case ZEGR_TF_R32G32_FLOAT:
			return DXGI_FORMAT_R32G32_FLOAT;

		case ZEGR_TF_R32G32_SINT:
			return DXGI_FORMAT_R32G32_SINT;

		case ZEGR_TF_R32G32_UINT:
			return DXGI_FORMAT_R32G32_UINT;


		case ZEGR_TF_R32G32B32A32_SINT:
			return DXGI_FORMAT_R32G32B32A32_SINT;

		case ZEGR_TF_R32G32B32A32_UINT:
			return DXGI_FORMAT_R32G32B32A32_UINT;

		case ZEGR_TF_R32G32B32A32_FLOAT:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;



		case ZEGR_TF_R10G10B10A2_UINT:
			return DXGI_FORMAT_R10G10B10A2_UINT;

		case ZEGR_TF_R10G10B10A2_UNORM:
			return DXGI_FORMAT_R10G10B10A2_UNORM;

		case ZEGR_TF_R11G11B10_FLOAT:
			return DXGI_FORMAT_R11G11B10_FLOAT;



		case ZEGR_TF_BC1_UNORM:
			return DXGI_FORMAT_BC1_UNORM;

		case ZEGR_TF_BC1_UNORM_SRGB:
			return DXGI_FORMAT_BC1_UNORM_SRGB;

		case ZEGR_TF_BC3_UNORM:
			return DXGI_FORMAT_BC3_UNORM;

		case ZEGR_TF_BC3_UNORM_SRGB:
			return DXGI_FORMAT_BC3_UNORM_SRGB;

		case ZEGR_TF_BC4_UNORM:
			return DXGI_FORMAT_BC4_UNORM;

		case ZEGR_TF_BC4_SNORM:
			return DXGI_FORMAT_BC4_SNORM;

		case ZEGR_TF_BC5_UNORM:
			return DXGI_FORMAT_BC5_UNORM;

		case ZEGR_TF_BC5_SNORM:
			return DXGI_FORMAT_BC5_SNORM;

		case ZEGR_TF_BC7_UNORM:
			return DXGI_FORMAT_BC7_UNORM;

		case ZEGR_TF_BC7_UNORM_SRGB:
			return DXGI_FORMAT_BC7_UNORM_SRGB;

		case ZEGR_TF_D16_UNORM:
			return DXGI_FORMAT_D16_UNORM;

		case ZEGR_TF_D24_UNORM_S8_UINT:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;

		case ZEGR_TF_D32_FLOAT:
			return DXGI_FORMAT_D32_FLOAT;

		case ZEGR_TF_D32_FLOAT_S8X24_UINT:
			return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

		case ZEGR_TF_R24_UNORM_X8:
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

		case ZEGR_TF_X24_G8_UINT:
			return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
	}
}

static ZEGRFormat ConvertDXGIFormat(DXGI_FORMAT Format)
{
	switch (Format)
	{
		default:
		case DXGI_FORMAT_UNKNOWN:
			return ZEGR_TF_NONE;

		case DXGI_FORMAT_R8_SINT:
			return ZEGR_TF_R8_SINT;

		case DXGI_FORMAT_R8_UINT:
			return ZEGR_TF_R8_UINT;

		case DXGI_FORMAT_R8_UNORM:
			return ZEGR_TF_R8_UNORM;

		case DXGI_FORMAT_R8_SNORM:
			return ZEGR_TF_R8_SNORM;



		case DXGI_FORMAT_R8G8_UINT:
			return ZEGR_TF_R8G8_UINT;

		case DXGI_FORMAT_R8G8_SINT:
			return ZEGR_TF_R8G8_SINT;

		case DXGI_FORMAT_R8G8_UNORM:
			return ZEGR_TF_R8G8_UNORM;

		case DXGI_FORMAT_R8G8_SNORM:
			return ZEGR_TF_R8G8_SNORM;



		case DXGI_FORMAT_R8G8B8A8_UINT:
			return ZEGR_TF_R8G8B8A8_UINT;

		case DXGI_FORMAT_R8G8B8A8_SINT:
			return ZEGR_TF_R8G8B8A8_SINT;

		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return ZEGR_TF_R8G8B8A8_UNORM;

		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return ZEGR_TF_R8G8B8A8_UNORM_SRGB;

		case DXGI_FORMAT_R8G8B8A8_SNORM:
			return ZEGR_TF_R8G8B8A8_SNORM;



		case DXGI_FORMAT_R16_UINT:
			return ZEGR_TF_R16_UINT;

		case DXGI_FORMAT_R16_SINT:
			return ZEGR_TF_R16_SINT;

		case DXGI_FORMAT_R16_UNORM:
			return ZEGR_TF_R16_UNORM;

		case DXGI_FORMAT_R16_SNORM:
			return ZEGR_TF_R16_SNORM;

		case DXGI_FORMAT_R16_FLOAT:
			return ZEGR_TF_R16_FLOAT;


		case DXGI_FORMAT_R16G16_UINT:
			return ZEGR_TF_R16G16_UINT;

		case DXGI_FORMAT_R16G16_SINT:
			return ZEGR_TF_R16G16_SINT;

		case DXGI_FORMAT_R16G16_UNORM:
			return ZEGR_TF_R16G16_UNORM;

		case DXGI_FORMAT_R16G16_SNORM:
			return ZEGR_TF_R16G16_SNORM;

		case DXGI_FORMAT_R16G16_FLOAT:
			return ZEGR_TF_R16G16_FLOAT;



		case DXGI_FORMAT_R16G16B16A16_UINT:
			return ZEGR_TF_R16G16B16A16_UINT;

		case DXGI_FORMAT_R16G16B16A16_SINT:
			return ZEGR_TF_R16G16B16A16_SINT;

		case DXGI_FORMAT_R16G16B16A16_UNORM:
			return ZEGR_TF_R16G16B16A16_UNORM;

		case DXGI_FORMAT_R16G16B16A16_SNORM:
			return ZEGR_TF_R16G16B16A16_SNORM;

		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			return ZEGR_TF_R16G16B16A16_FLOAT;


		case DXGI_FORMAT_R32_FLOAT:
			return ZEGR_TF_R32_FLOAT;

		case DXGI_FORMAT_R32_SINT:
			return ZEGR_TF_R32_SINT;

		case DXGI_FORMAT_R32_UINT:
			return ZEGR_TF_R32_UINT;


		case DXGI_FORMAT_R32G32_FLOAT:
			return ZEGR_TF_R32G32_FLOAT;

		case DXGI_FORMAT_R32G32_SINT:
			return ZEGR_TF_R32G32_SINT;

		case DXGI_FORMAT_R32G32_UINT:
			return ZEGR_TF_R32G32_UINT;


		case DXGI_FORMAT_R32G32B32A32_SINT:
			return ZEGR_TF_R32G32B32A32_SINT;

		case DXGI_FORMAT_R32G32B32A32_UINT:
			return ZEGR_TF_R32G32B32A32_UINT;

		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return ZEGR_TF_R32G32B32A32_FLOAT;



		case DXGI_FORMAT_R10G10B10A2_UINT:
			return ZEGR_TF_R10G10B10A2_UINT;

		case DXGI_FORMAT_R10G10B10A2_UNORM:
			return ZEGR_TF_R10G10B10A2_UNORM;

		case DXGI_FORMAT_R11G11B10_FLOAT:
			return ZEGR_TF_R11G11B10_FLOAT;



		case DXGI_FORMAT_BC1_UNORM:
			return ZEGR_TF_BC1_UNORM;

		case DXGI_FORMAT_BC1_UNORM_SRGB:
			return ZEGR_TF_BC1_UNORM_SRGB;

		case DXGI_FORMAT_BC3_UNORM:
			return ZEGR_TF_BC3_UNORM;

		case DXGI_FORMAT_BC3_UNORM_SRGB:
			return ZEGR_TF_BC3_UNORM_SRGB;

		case DXGI_FORMAT_BC4_UNORM:
			return ZEGR_TF_BC4_UNORM;

		case DXGI_FORMAT_BC4_SNORM:
			return ZEGR_TF_BC4_SNORM;

		case DXGI_FORMAT_BC5_UNORM:
			return ZEGR_TF_BC5_UNORM;

		case DXGI_FORMAT_BC5_SNORM:
			return ZEGR_TF_BC5_SNORM;

		case DXGI_FORMAT_BC7_UNORM:
			return ZEGR_TF_BC7_UNORM;

		case DXGI_FORMAT_BC7_UNORM_SRGB:
			return ZEGR_TF_BC7_UNORM_SRGB;
	}
}
ZEGRTextureOptions::ZEGRTextureOptions()
{
	Type = ZEGR_TT_NONE;
	HorizontalTileCount = 0;
	VerticalTileCount = 0;
	CompressionFormat = ZEGR_TF_NONE;
	MaximumMipmapLevel = 0;
	GenerateMipMaps = false;
	sRGB = false;
}

ZEGRTextureView::ZEGRTextureView()
{
	Texture = NULL;
	Format = ZEGR_TF_NONE;
	LevelMin = 0;
	LevelCount = 1;
	LayerMin = 0;
	LayerCount = 1;
}

ZEGRTextureView::ZEGRTextureView(ZEGRFormat Format, ZEUInt LevelMin, ZEUInt LevelCount, ZEUInt LayerMin, ZEUInt LayerCount)
{
	Texture = NULL;
	this->Format = Format;
	this->LevelMin = LevelMin;
	this->LevelCount = LevelCount;
	this->LayerMin = LayerMin;
	this->LayerCount = LayerCount;
}

const ZEGRTexture* ZEGRTextureView::GetTexture() const
{
	return Texture;
}

ZEGRFormat ZEGRTextureView::GetFormat() const
{
	return Format;
}

ZEUInt ZEGRTextureView::GetLevelMin() const
{
	return LevelMin;
}

ZEUInt ZEGRTextureView::GetLevelCount() const
{
	return LevelCount;
}

ZEUInt ZEGRTextureView::GetLayerMin() const
{
	return LayerMin;
}

ZEUInt ZEGRTextureView::GetLayerCount() const
{
	return LayerCount;
}

ZESize ZEGRTexture::CalculateSize()
{
	const ZEGRFormatDefinition* FormatDefinition = ZEGRFormatDefinition::GetDefinition(GetFormat());
	if (FormatDefinition->Format == ZEGR_TF_NONE)
		return 0;

	ZESize Size = 0;

	for (ZEUInt I = 0; I < LevelCount; I++)
	{
		ZEUInt LodWidth = Width >> I;
		ZEUInt LodHeight = Height >> I;

		if (FormatDefinition->Compressed)
		{
			LodWidth = (LodWidth < FormatDefinition->BlockDimension) ? FormatDefinition->BlockDimension : (LodWidth / FormatDefinition->BlockDimension);
			LodHeight = (LodHeight < FormatDefinition->BlockDimension) ? FormatDefinition->BlockDimension : (LodHeight / FormatDefinition->BlockDimension);
		}

		Size += LodWidth * LodHeight;
	}

	return Size * FormatDefinition->BlockSize * DepthOrArrayCount;
}

const ZEArray<ZEHolder<ZEGRRenderTarget>>& ZEGRTexture::GetRenderTargets() const
{
	return RenderTargets;
}

const ZEArray<ZEHolder<ZEGRDepthStencilBuffer>>& ZEGRTexture::GetDepthStencilBuffers() const
{
	return DepthStencilBuffers;
}

bool ZEGRTexture::Initialize(ZEGRTextureType TextureType, ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEUInt DepthOrArrayCount, ZEUInt SampleCount, const void* Data)
{
	this->Type = TextureType;
	this->Width = Width;
	this->Height = Height;
	this->LevelCount = LevelCount;
	this->SampleCount = SampleCount;
	this->DepthOrArrayCount = DepthOrArrayCount;
	SetResourceUsage(Usage);
	SetResourceBindFlags(BindFlags);
	SetFormat(Format);
	SetSize(CalculateSize());

	DepthStencilBuffers.SetCount(8);

	return true;
}

void ZEGRTexture::Deinitialize()
{

}

ZETaskResult ZEGRTexture::LoadInternal()
{
	if (GetFileName().IsEmpty())
		return ZE_TR_DONE;

	if (TextureOptions.Type == ZEGR_TT_NONE)
	{
		zeError("Cannot load texture. Unknown texture type. File Name: \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	ZEPathInfo PathInfo = GetFileName();
	ZEString Extension = PathInfo.GetExtension();

	if (Extension.IsEmpty())
	{
		zeError("Cannot load texture. Unknown file extension. File Name: \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	ZEString FileRealPath = PathInfo.GetRealPath().Path;

	HRESULT HR;
	TexMetadata FinalMetaData;
	ZEPointer<ScratchImage> FinalImage = new ScratchImage();

	if (Extension == ".dds")
	{
		HR = LoadFromDDSFile(FileRealPath, DDS_FLAGS_NONE, &FinalMetaData, *FinalImage);
		if (FAILED(HR))
		{
			zeError("Cannot load texture. Cannot read from file. Result: 0x%X, File Name: \"%s\".", HR, GetFileName().ToCString());
			return ZE_TR_FAILED;
		}

		if ((TextureOptions.Type == ZEGR_TT_3D && !FinalMetaData.IsVolumemap()) || (TextureOptions.Type == ZEGR_TT_CUBE && !FinalMetaData.IsCubemap()))
		{
			zeError("Cannot load texture. Object texture type and file texture type don't match. File Name: \"%s\".", GetFileName().ToCString());
			return ZE_TR_FAILED;
		}
	}
	else
	{
		ZEString DDSFile = PathInfo.GetName() + ".dds";
		ZEPathInfo DDSFilePathInfo = ZEPathInfo::CombineRelativePath(PathInfo.GetPath(), DDSFile);
		if (DDSFilePathInfo.GetModificationTime() > PathInfo.GetModificationTime())
		{
			HR = LoadFromDDSFile(DDSFilePathInfo.GetRealPath().Path, DDS_FLAGS_NONE, &FinalMetaData, *FinalImage);
			if (FAILED(HR))
			{
				zeError("Cannot load texture. Cannot read from file. Result: 0x%X, File Name: \"%s\".", HR, GetFileName().ToCString());
				return ZE_TR_FAILED;
			}
		}
		else
		{
			if (Extension == ".tga")
			{
				HR = LoadFromTGAFile(FileRealPath, &FinalMetaData, *FinalImage);
				if (FAILED(HR))
				{
					zeError("Cannot load texture. Cannot read from file. Result: 0x%X, File Name: \"%s\".", HR, GetFileName().ToCString());
					return ZE_TR_FAILED;
				}
			}
			else
			{
				HR = LoadFromWICFile(FileRealPath, WIC_FLAGS_FORCE_RGB | WIC_FLAGS_IGNORE_SRGB, &FinalMetaData, *FinalImage);
				if (FAILED(HR))
				{
					zeError("Cannot load texture. Cannot read from file. Result: 0x%X, File Name: \"%s\".", HR, GetFileName().ToCString());
					return ZE_TR_FAILED;
				}
			}

			if (TextureOptions.Type == ZEGR_TT_3D)
			{
				const Image* SrcImage = FinalImage->GetImage(0, 0, 0);
				ZEUInt TargetRowpitch = (ZEUInt)SrcImage->rowPitch / TextureOptions.HorizontalTileCount;
				ZEUInt TargetHeight = (ZEUInt)SrcImage->height / TextureOptions.VerticalTileCount;
				ZEUInt TargetDepth = TextureOptions.HorizontalTileCount * TextureOptions.VerticalTileCount;

				ZEPointer<ScratchImage> VolumeMap = new ScratchImage();
				VolumeMap->Initialize3D(FinalMetaData.format, FinalMetaData.width / TextureOptions.HorizontalTileCount, FinalMetaData.height / TextureOptions.VerticalTileCount, TargetDepth, 1);
				for (ZEUInt J = 0; J < TextureOptions.VerticalTileCount; J++)
				{
					ZESize Index = J * TextureOptions.HorizontalTileCount;
					ZESize Offset = J * TargetHeight * SrcImage->rowPitch;
					for (ZEUInt I = 0; I < TextureOptions.HorizontalTileCount; I++)
					{
						for (ZEUInt K = 0; K < TargetHeight; K++)
							memcpy(VolumeMap->GetImages()[Index + I].pixels + K * TargetRowpitch, SrcImage->pixels + (Offset + I * TargetRowpitch) + K * SrcImage->rowPitch, TargetRowpitch);
					}
				}

				FinalMetaData = VolumeMap->GetMetadata();
				FinalImage = VolumeMap.Transfer();
			}
			else if (TextureOptions.Type == ZEGR_TT_CUBE)
			{
				const Image* SrcImage = FinalImage->GetImage(0, 0, 0);
				ZEUInt FaceRowpitch = (ZEUInt)SrcImage->rowPitch / 3;
				ZEUInt FaceHeight = (ZEUInt)SrcImage->height / 2;

				ZESize Offsets[6] = 
				{															// Copy Order
					2 * FaceRowpitch,										// +X Face
					0,														// -X Face
					FaceHeight * SrcImage->rowPitch + 2 * FaceRowpitch,		// +Y Face
					FaceHeight * SrcImage->rowPitch + FaceRowpitch,			// -Y Face
					FaceRowpitch,											// +Z Face
					FaceHeight * SrcImage->rowPitch,						// -Z Face
				};

				ZEPointer<ScratchImage> CubeMap = new ScratchImage();
				CubeMap->InitializeCube(FinalMetaData.format, FinalMetaData.width / 3, FinalMetaData.height / 2, 1, 1);
				for (ZEUInt I = 0; I < 6; I++)
				{
					for (ZEUInt J = 0; J < FaceHeight; J++)
						memcpy(CubeMap->GetImages()[I].pixels + J * FaceRowpitch, SrcImage->pixels + Offsets[I] + J * SrcImage->rowPitch, FaceRowpitch);
				}

				FinalMetaData = CubeMap->GetMetadata();
				FinalImage = CubeMap.Transfer();
			}

			if (TextureOptions.sRGB)
			{
				FinalImage->OverrideFormat(MakeSRGB(FinalMetaData.format));
				FinalMetaData = FinalImage->GetMetadata();
			}

			if (TextureOptions.GenerateMipMaps)
			{
				if (TextureOptions.Type == ZEGR_TT_3D && !ZEMath::IsPowerOfTwo((ZEUInt)FinalMetaData.depth))
				{
					zeError("Cannot load texture. Cannot generate mipmaps for texture3d having non-power-of-2 depth. File Name: \"%s\".", GetFileName().ToCString());
					return ZE_TR_FAILED;
				}

				if (!ZEMath::IsPowerOfTwo((ZEUInt)FinalMetaData.width) || !ZEMath::IsPowerOfTwo((ZEUInt)FinalMetaData.height))
				{
					float PowerWidth = ZEMath::Log((float)FinalMetaData.width) / ZEMath::Log(2);
					float PowerHeight = ZEMath::Log((float)FinalMetaData.height) / ZEMath::Log(2);

					ZEUInt NearestPowerWidth = (ZEUInt)ZEMath::Floor(PowerWidth + 0.5f);
					ZEUInt NearestPowerHeight = (ZEUInt)ZEMath::Floor(PowerHeight + 0.5f);

					ZEUInt ResizedWidth = (ZEUInt)ZEMath::Power(2, (float)NearestPowerWidth);
					ZEUInt ResizedHeight = (ZEUInt)ZEMath::Power(2, (float)NearestPowerHeight);

					ZEPointer<ScratchImage> ResizedImage = new ScratchImage();
					HR = Resize(FinalImage->GetImages(), FinalImage->GetImageCount(), FinalMetaData, ResizedWidth, ResizedHeight, TEX_FILTER_DEFAULT, *ResizedImage);
					if (FAILED(HR))
					{
						zeError("Cannot load texture. Resizing to nearest power of two failed. Result: 0x%X, File Name: \"%s\".", HR, GetFileName().ToCString());
						return ZE_TR_FAILED;
					}

					FinalMetaData = ResizedImage->GetMetadata();
					FinalImage = ResizedImage.Transfer();
				}

				ZEPointer<ScratchImage> MipmapChain = new ScratchImage();

				if (TextureOptions.Type == ZEGR_TT_3D)
					HR = GenerateMipMaps3D(FinalImage->GetImages(), FinalImage->GetImageCount(), FinalMetaData, TEX_FILTER_DEFAULT, TextureOptions.MaximumMipmapLevel, *MipmapChain);
				else
					HR = GenerateMipMaps(FinalImage->GetImages(), FinalImage->GetImageCount(), FinalMetaData, TEX_FILTER_DEFAULT, TextureOptions.MaximumMipmapLevel, *MipmapChain);

				if (FAILED(HR))
				{
					zeError("Cannot load texture. Mip map generation failed. Result: 0x%X, File Name: \"%s\".", HR, GetFileName().ToCString());
					return ZE_TR_FAILED;
				}

				FinalMetaData = MipmapChain->GetMetadata();
				FinalImage = MipmapChain.Transfer();
			}

			const ZEGRFormatDefinition* FormatInfo = ZEGRFormatDefinition::GetDefinition(TextureOptions.CompressionFormat);
			if (FormatInfo->Compressed)
			{
				DXGI_FORMAT CompressionFormat = ConvertFormat(FormatInfo->Format);
				DWORD CompressionFlags = TEX_COMPRESS_DEFAULT;

				if (FormatInfo->Type == ZEGR_FT_UNORM_SRGB)
					CompressionFlags |= TEX_COMPRESS_SRGB;

				if (FormatInfo->Structure == ZEGR_FS_BC4)
					CompressionFlags |= TEX_COMPRESS_SRGB_IN;

				ZEPointer<ScratchImage> CompressedImage = new ScratchImage();
				HR = Compress(FinalImage->GetImages(), FinalImage->GetImageCount(), FinalMetaData, CompressionFormat, CompressionFlags, 0.5f, *CompressedImage);
				if (FAILED(HR))
				{
					zeError("Cannot load texture. Compression failed. Result: 0x%X, File Name: \"%s\".", HR, GetFileName().ToCString());
					return ZE_TR_FAILED;
				}

				FinalMetaData = CompressedImage->GetMetadata();
				FinalImage = CompressedImage.Transfer();
			}

			ZEString NewFilename = PathInfo.GetName() + ".dds";
			ZEString NewFileRealPath = ZEPathInfo::CombineRelativePath(FileRealPath, NewFilename);
			HR = SaveToDDSFile(FinalImage->GetImages(), FinalImage->GetImageCount(), FinalMetaData, DDS_FLAGS_NONE, NewFileRealPath);
			if (FAILED(HR))
			{
				zeError("Cannot load texture. Saving to dds file failed. Result: 0x%X, File Name: \"%s\".", HR, GetFileName().ToCString());
				return ZE_TR_FAILED;
			}
		}
	}

	if (!Initialize(
		TextureOptions.Type,
		(ZEUInt)FinalMetaData.width, 
		(ZEUInt)FinalMetaData.height, 
		(ZEUInt)FinalMetaData.mipLevels, 
		ConvertDXGIFormat(FinalMetaData.format), 
		ZEGR_RU_IMMUTABLE, 
		ZEGR_RBF_SHADER_RESOURCE, 
		(TextureOptions.Type != ZEGR_TT_3D) ? (ZEUInt)FinalMetaData.arraySize : (ZEUInt)FinalMetaData.depth, 
		1, 
		FinalImage->GetPixels()))
	{
		zeError("Cannot load texture. Initialization failed. File Name: \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	return ZE_TR_DONE;
}

ZETaskResult ZEGRTexture::UnloadInternal()
{
	Deinitialize();
	return ZE_TR_DONE;
}

ZEGRTexture::ZEGRTexture()
{
	Width = 0;
	Height = 0;
	LevelCount = 0;
	SampleCount = 0;
	DepthOrArrayCount = 0;
	Type = ZEGR_TT_NONE;

	Register();
}

ZEGRTexture::~ZEGRTexture()
{
	Unregister();

	ze_for_each(Srv, ShaderResourceViews)
		delete Srv.GetItem();

	ze_for_each(Uav, UnorderedAccessViews)
		delete Uav.GetItem();

	ShaderResourceViews.Clear();
	UnorderedAccessViews.Clear();

	RenderTargets.Clear();
	DepthStencilBuffers.Clear();
}

ZERSResource* ZEGRTexture::Instanciator(const void* Parameters)
{
	ZEGRGraphicsModule* GraphicsModule = ZEGRGraphicsModule::GetInstance();
	if (GraphicsModule == NULL)
		return NULL;

	ZEGRTexture* Texture = GraphicsModule->CreateTexture();

	if (Parameters != NULL && Texture != NULL)
		Texture->TextureOptions = *reinterpret_cast<const ZEGRTextureOptions*>(Parameters);

	return Texture;
}

ZEUInt ZEGRTexture::GetWidth() const
{
	return Width;
}

ZEUInt ZEGRTexture::GetHeight() const
{
	return Height;
}

ZEUInt ZEGRTexture::GetLevelCount() const
{
	return LevelCount;
}

ZEUInt ZEGRTexture::GetSampleCount() const
{
	return SampleCount;
}

ZEUInt ZEGRTexture::GetDepthOrArrayCount() const
{
	return DepthOrArrayCount;
}

ZEGRTextureType ZEGRTexture::GetTextureType() const
{
	return Type;
}

ZEGRResourceType ZEGRTexture::GetResourceType() const
{
	return ZEGR_RT_TEXTURE;
}

ZEHolder<ZEGRTexture> ZEGRTexture::CreateResource(ZEGRTextureType TextureType, ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEUInt DepthOrArrayCount, ZEUInt SampleCount, const void* Data)
{
	ZEHolder<ZEGRTexture> Texture = ZERSTemplates::CreateResource<ZEGRTexture>(Instanciator);
	if (Texture == NULL)
		return NULL;

	if (!Texture->Initialize(TextureType, Width, Height, LevelCount, Format, Usage, BindFlags, DepthOrArrayCount, SampleCount, Data))
	{
		Texture->StagingFailed();
		return NULL;
	}

	Texture->StagingRealized();

	return Texture;
}

ZEHolder<const ZEGRTexture> ZEGRTexture::CreateResourceShared(const ZEGUID& GUID, ZEGRTextureType TextureType, ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEUInt DepthOrArrayCount, ZEUInt SampleCount, const void* Data, ZEGRTexture** StagingResource)
{
	ZEGRTexture* StagingResourceTemp;
	ZEHolder<const ZEGRTexture> Texture = ZERSTemplates::CreateResourceShared<ZEGRTexture>(GUID, &StagingResourceTemp, Instanciator);
	if (Texture == NULL)
		return NULL;

	if (StagingResourceTemp != NULL)
	{
		if (!StagingResourceTemp->Initialize(TextureType, Width, Height, LevelCount, Format, Usage, BindFlags, DepthOrArrayCount, SampleCount, Data))
			return NULL;

		if (StagingResource != NULL)
			*StagingResource = StagingResourceTemp;
		else
			StagingResourceTemp->StagingRealized();
	}

	return Texture;
}

ZEHolder<ZEGRTexture> ZEGRTexture::LoadResource(const ZEString& FileName, const ZEGRTextureOptions& TextureOptions)
{
	return ZERSTemplates::LoadResource<ZEGRTexture>(FileName, Instanciator, &TextureOptions);
}

ZEHolder<const ZEGRTexture> ZEGRTexture::LoadResourceShared(const ZEString& FileName, const ZEGRTextureOptions& TextureOptions)
{
	return ZERSTemplates::LoadResourceShared<ZEGRTexture>(FileName, Instanciator, &TextureOptions);
}
