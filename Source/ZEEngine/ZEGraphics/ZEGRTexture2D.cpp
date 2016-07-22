//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRTexture2D.cpp
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

#include "ZEGRTexture2D.h"

#include "ZEGRGraphicsModule.h"
#include "ZETexture/ZETextureData.h"
#include "ZEMath/ZEMath.h"
#include "ZEFile/ZEPathInfo.h"
#include "ZEPointer/ZEPointer.h"

#include "ZEModules/ZEDirect3D11/ZED11ComponentBase.h"

#include "DirectXTex.h"
using namespace DirectX;

ZEGRResourceType ZEGRTexture2D::GetResourceType() const
{
	return ZEGR_RT_TEXTURE;
}

ZEGRTextureType ZEGRTexture2D::GetTextureType() const
{
	return ZEGR_TT_2D;
}

ZEUInt ZEGRTexture2D::GetWidth() const
{
	return Width;
}

ZEUInt ZEGRTexture2D::GetHeight() const
{
	return Height;
}

ZEUInt ZEGRTexture2D::GetArrayCount() const
{
	return ArrayCount;
}

ZEUInt ZEGRTexture2D::GetSampleCount() const
{
	return SampleCount;
}

ZEVector2 ZEGRTexture2D::GetPixelSize() const
{
	return ZEVector2(1.0f / Width, 1.0f / Height);
}

bool ZEGRTexture2D::Initialize(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlags, ZEUInt ArrayCount, ZEUInt SampleCount, const void* Data)
{
	this->Width = Width;
	this->Height = Height;
	this->ArrayCount = ArrayCount;
	this->SampleCount = SampleCount;
	SetFormat(Format);
	SetResourceUsage(Usage);
	SetResourceBindFlags(BindFlags);
	SetLevelCount(LevelCount);

	SetSize(CalculateSize(Width, Height, LevelCount, Format));
	ZEGR_COUNTER_RESOURCE_INCREASE(this, Texture2D, Texture);

	return true;
}

void ZEGRTexture2D::Deinitialize()
{
	SetSize(0);
	ZEGR_COUNTER_RESOURCE_DECREASE(this, Texture2D, Texture);
}

ZEGRTexture2D::ZEGRTexture2D()
{
	Width = 0;
	Height = 0;
	ArrayCount = 1;
	SampleCount = 1;
};

ZEHolder<ZEGRTexture2D> ZEGRTexture2D::CreateInstance(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlags, ZEUInt ArrayCount, ZEUInt SampleCount, const void* Data)
{
	zeDebugCheck(Width == 0, "Width cannot be 0.");
	zeDebugCheck(Height == 0, "Height cannot be 0.");
	zeDebugCheck(Width > ZEGR_MAX_TEXTURE2D_DIMENSION, "Width is too big.");
	zeDebugCheck(Height > ZEGR_MAX_TEXTURE2D_DIMENSION, "Height is too big.");
	zeDebugCheck(LevelCount == 0, NULL, "Level cannot be 0.");
	zeDebugCheck(LevelCount > 1 && (!ZEMath::IsPowerOfTwo(Width) || !ZEMath::IsPowerOfTwo(Height)), "Level must be 1 for non-power of two textures.");
	zeDebugCheck(Usage == ZEGR_RU_GPU_READ_ONLY && Data == NULL, "Immutable textures must be initialized with data");
	zeDebugCheck(Usage == ZEGR_RU_GPU_READ_ONLY && BindFlags.GetFlags(ZEGR_RBF_RENDER_TARGET | ZEGR_RBF_DEPTH_STENCIL), "Immutable textures cannot be bound as render target or depth-stencil");
	zeDebugCheck(Usage == ZEGR_RU_GPU_READ_CPU_WRITE && BindFlags.GetFlags(ZEGR_RBF_RENDER_TARGET | ZEGR_RBF_DEPTH_STENCIL),  "Dynamic textures cannot be bound as render target or depth-stencil");
	zeDebugCheck(Usage == ZEGR_RU_CPU_READ_WRITE && !BindFlags.GetFlags(ZEGR_RBF_NONE), "Staging textures cannot be bound to gpu");
	zeDebugCheck(BindFlags.GetFlags(ZEGR_RBF_RENDER_TARGET) && BindFlags.GetFlags(ZEGR_RBF_DEPTH_STENCIL), "Both render target and depth-stencil bind flags cannot be set");
	zeDebugCheck(ArrayCount > ZEGR_MAX_TEXTURE2D_ARRAY_LENGTH, "Array count is too much");
	zeDebugCheck(SampleCount > ZEGR_MAX_SAMPLE_COUNT, "Sample count is too much");

	ZEGRTexture2D* Texture = ZEGRGraphicsModule::GetInstance()->CreateTexture2D();
	if (Texture == NULL)
		return NULL;

	if (!Texture->Initialize(Width, Height, LevelCount, Format, Usage, BindFlags, ArrayCount, SampleCount, Data))
	{
		delete Texture;
		return NULL;
	}

	return Texture;
}

ZEHolder<ZEGRTexture2D> ZEGRTexture2D::CreateFromFile(const ZEString& Filename, const ZEGRTextureOptions& TextureOptions)
{
	if (Filename.IsEmpty())
		return NULL;
	
	ZEPathInfo PathInfo = Filename;
	ZEString Extension = PathInfo.GetExtension();

	if (Extension.IsEmpty())
		return NULL;

	ZEString FileRealPath = PathInfo.GetRealPath().Path;

	HRESULT HR;
	TexMetadata FinalMetaData;
	ZEPointer<ScratchImage> FinalImage = new ScratchImage();

	if (Extension == ".dds")
	{
		HR = LoadFromDDSFile(FileRealPath, DDS_FLAGS_NONE, &FinalMetaData, *FinalImage);
		if (FAILED(HR))
		{
			zeError("Loading from dds file failed (%x)\n", HR);
			return NULL;
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
				zeError("Loading from dds file failed (%x)\n", HR);
				return NULL;
			}
		}
		else
		{
			if (Extension == ".tga")
			{
				HR = LoadFromTGAFile(FileRealPath, &FinalMetaData, *FinalImage);
				if (FAILED(HR))
				{
					zeError("Loading from tga file failed (%x)\n", HR);
					return NULL;
				}
			}
			else
			{
				HR = LoadFromWICFile(FileRealPath, WIC_FLAGS_FORCE_RGB | WIC_FLAGS_IGNORE_SRGB, &FinalMetaData, *FinalImage);
				if (FAILED(HR))
				{
					zeError("Loading from %s file failed (%x)\n", Extension, HR);
					return NULL;
				}
			}

			if (TextureOptions.sRGB)
			{
				FinalImage->OverrideFormat(MakeSRGB(FinalMetaData.format));
				FinalMetaData = FinalImage->GetMetadata();
			}

			if (TextureOptions.GenerateMipMaps)
			{
				if (!ZEMath::IsPowerOfTwo(FinalMetaData.width) || !ZEMath::IsPowerOfTwo(FinalMetaData.height))
				{
					float PowerWidth = ZEMath::Log(FinalMetaData.width) / ZEMath::Log(2);
					float PowerHeight = ZEMath::Log(FinalMetaData.height) / ZEMath::Log(2);

					ZEUInt NearestPowerWidth = ZEMath::Floor(PowerWidth + 0.5f);
					ZEUInt NearestPowerHeight = ZEMath::Floor(PowerHeight + 0.5f);

					ZEUInt ResizedWidth = ZEMath::Power(2, NearestPowerWidth);
					ZEUInt ResizedHeight = ZEMath::Power(2, NearestPowerHeight);

					ZEPointer<ScratchImage> ResizedImage = new ScratchImage();
					HR = Resize(FinalImage->GetImages(), FinalImage->GetImageCount(), FinalMetaData, ResizedWidth, ResizedHeight, TEX_FILTER_DEFAULT, *ResizedImage);
					if (FAILED(HR))
					{
						zeError("Resizing to nearest power of two failed (%x)\n", HR);
						return NULL;
					}

					FinalMetaData = ResizedImage->GetMetadata();
					FinalImage = ResizedImage.Transfer();
				}

				ZEPointer<ScratchImage> MipmapChain = new ScratchImage();
				HR = GenerateMipMaps(FinalImage->GetImages(), FinalImage->GetImageCount(), FinalMetaData, TEX_FILTER_DEFAULT, TextureOptions.MaximumMipmapLevel, *MipmapChain);
				if (FAILED(HR))
				{
					zeError("Mip map generation failed (%x)\n", HR);
					return NULL;
				}

				FinalMetaData = MipmapChain->GetMetadata();
				FinalImage = MipmapChain.Transfer();
			}

			const ZEGRFormatDefinition* FormatInfo = ZEGRFormatDefinition::GetDefinition(TextureOptions.CompressionFormat);
			if (FormatInfo->Compressed)
			{
				DXGI_FORMAT CompressionFormat = ZED11ComponentBase::ConvertFormat(FormatInfo->Format);
				DWORD CompressionFlags = TEX_COMPRESS_DEFAULT;

				if (FormatInfo->Type == ZEGR_FT_UNORM_SRGB)
					CompressionFlags |= TEX_COMPRESS_SRGB;

				if (FormatInfo->Structure == ZEGR_FS_BC4)
					CompressionFlags |= TEX_COMPRESS_SRGB_IN;

				ZEPointer<ScratchImage> CompressedImage = new ScratchImage();
				HR = Compress(FinalImage->GetImages(), FinalImage->GetImageCount(), FinalMetaData, CompressionFormat, CompressionFlags, 0.5f, *CompressedImage);
				if (FAILED(HR))
				{
					zeError("Compression failed (%x)\n", HR);
					return NULL;
				}

				FinalMetaData = CompressedImage->GetMetadata();
				FinalImage = CompressedImage.Transfer();
			}

			ZEString NewFilename = PathInfo.GetName() + ".dds";
			ZEString NewFileRealPath = ZEPathInfo::CombineRelativePath(FileRealPath, NewFilename);
			HR = SaveToDDSFile(FinalImage->GetImages(), FinalImage->GetImageCount(), FinalMetaData, DDS_FLAGS_NONE, NewFileRealPath);
			if (FAILED(HR))
			{
				zeError("Saving to dds file failed (%x)\n", HR);
				return NULL;
			}
		}
	}

	return ZEGRTexture2D::CreateInstance(
										FinalMetaData.width, 
										FinalMetaData.height, 
										FinalMetaData.mipLevels, 
										ZED11ComponentBase::ConvertDXGIFormat(FinalMetaData.format), 
										ZEGR_RU_GPU_READ_ONLY, 
										ZEGR_RBF_SHADER_RESOURCE, 
										FinalMetaData.arraySize, 
										1, 
										FinalImage->GetPixels());
}
