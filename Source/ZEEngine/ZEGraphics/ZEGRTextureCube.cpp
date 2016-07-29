//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRTextureCube.cpp
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

#include "ZEGRTextureCube.h"

#include "ZEError.h"
#include "ZEMath/ZEMath.h"
#include "ZEFile/ZEPathInfo.h"
#include "ZEPointer/ZEPointer.h"
#include "ZEGRGraphicsModule.h"
#include "ZEResource/ZERSTemplates.h"
#include "ZEModules/ZEDirect3D11/ZED11ComponentBase.h"

#include "DirectXTex.h"

using namespace DirectX;

bool ZEGRTextureCube::CheckParameters(ZEUInt Length, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlags, const void* Data)
{
	zeDebugCheck(Length == 0, "length cannot be 0.");
	zeDebugCheck(Length > ZEGR_MAX_TEXTURECUBE_DIMENSION, "length is too big.");
	zeDebugCheck(LevelCount == 0, "Level cannot be 0.");
	zeDebugCheck(LevelCount > 1 && !ZEMath::IsPowerOfTwo(Length), NULL, "Level must be 1 for non-power of two textures.");
	zeDebugCheck(Usage == ZEGR_RU_GPU_READ_ONLY && Data == NULL, "Immutable textures must be initialized with data");
	zeDebugCheck(Usage == ZEGR_RU_GPU_READ_ONLY && BindFlags.GetFlags(ZEGR_RBF_RENDER_TARGET | ZEGR_RBF_DEPTH_STENCIL), "Immutable textures cannot be bound as render target or depth-stencil");
	zeDebugCheck(Usage == ZEGR_RU_GPU_READ_CPU_WRITE && BindFlags.GetFlags(ZEGR_RBF_RENDER_TARGET | ZEGR_RBF_DEPTH_STENCIL),  "Dynamic textures cannot be bound as render target or depth-stencil");
	zeDebugCheck(Usage == ZEGR_RU_CPU_READ_WRITE && !BindFlags.GetFlags(ZEGR_RBF_NONE), "Staging textures cannot be bound to gpu");
	zeDebugCheck(BindFlags.GetFlags(ZEGR_RBF_RENDER_TARGET) && BindFlags.GetFlags(ZEGR_RBF_DEPTH_STENCIL), "Both render target and depth-stencil bind flags cannot be set");

	return true;
}

bool ZEGRTextureCube::Initialize(ZEUInt Length, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlags, const void* Data)
{
	this->Length = Length;
	SetLevelCount(LevelCount);
	SetFormat(Format);
	SetResourceUsage(Usage);
	SetResourceBindFlags(BindFlags);

	SetSize(CalculateSize(Length, Length, LevelCount, Format));

	return true;
}

void ZEGRTextureCube::Deinitialize()
{
	SetSize(0);
}


ZETaskResult ZEGRTextureCube::LoadInternal()
{
	if (GetFileName().IsEmpty())
		return ZE_TR_DONE;

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

			if (TextureOptions.sRGB)
			{
				FinalImage->OverrideFormat(MakeSRGB(FinalMetaData.format));
				FinalMetaData = FinalImage->GetMetadata();
			}

			if (TextureOptions.GenerateMipMaps)
			{
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
		(ZEUInt)FinalMetaData.width, 
		(ZEUInt)FinalMetaData.mipLevels, 
		ZED11ComponentBase::ConvertDXGIFormat(FinalMetaData.format), 
		ZEGR_RU_GPU_READ_ONLY, 
		ZEGR_RBF_SHADER_RESOURCE, 
		FinalImage->GetPixels()))
	{
		zeError("Cannot load texture. Initialization failed. File Name: \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	return ZE_TR_DONE;

}

ZETaskResult ZEGRTextureCube::UnloadInternal()
{
	Deinitialize();
	return ZE_TR_DONE;
}

ZEGRResourceType ZEGRTextureCube::GetResourceType() const
{
	return ZEGR_RT_TEXTURE;
}

ZEGRTextureType ZEGRTextureCube::GetTextureType() const
{
	return ZEGR_TT_CUBE;
}

ZEUInt ZEGRTextureCube::GetLength() const
{
	return Length;
}

float ZEGRTextureCube::GetPixelSize() const
{
	return 1.0f / (float)GetLength();
}

ZEGRTextureCube::ZEGRTextureCube()
{
	Length = 0;
	Register();
}

ZEGRTextureCube::~ZEGRTextureCube()
{
	Unregister();
}

ZERSResource* ZEGRTextureCube::Instanciator(const void* Parameters)
{
	ZEGRTextureCube* Resource = ZEGRGraphicsModule::GetInstance()->CreateTextureCube();
	
	if (Parameters != NULL && Resource != NULL)
		Resource->TextureOptions = *reinterpret_cast<const ZEGRTextureCubeOptions*>(Parameters);

	return Resource;
}

ZEHolder<ZEGRTextureCube> ZEGRTextureCube::CreateResource(ZEUInt Length, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlags, const void* Data)
{
	if (!CheckParameters(Length, LevelCount, Format, Usage, BindFlags, Data))
		return NULL;

	ZEHolder<ZEGRTextureCube> Texture = ZERSTemplates::CreateResource<ZEGRTextureCube>(Instanciator);
	if (Texture == NULL)
		return NULL;

	if (!Texture->Initialize(Length, LevelCount, Format, Usage, BindFlags, Data))
	{
		Texture->StagingFailed();
		return NULL;
	}

	Texture->StagingRealized();

	return Texture;
}

ZEHolder<const ZEGRTextureCube> ZEGRTextureCube::CreateResourceShared(const ZEGUID& GUID, ZEUInt Length, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlags, const void* Data, ZEGRTextureCube** StagingResource)
{
	if (!CheckParameters(Length, LevelCount, Format, Usage, BindFlags, Data))
		return NULL;

	ZEGRTextureCube* StagingResourceTemp = NULL;
	ZEHolder<const ZEGRTextureCube> Texture = ZERSTemplates::CreateResourceShared<ZEGRTextureCube>(GUID, &StagingResourceTemp, Instanciator);
	if (Texture == NULL)
		return NULL;

	if (StagingResourceTemp != NULL)
	{
		if (!StagingResourceTemp->Initialize(Length, LevelCount, Format, Usage, BindFlags, Data))
		{
			StagingResourceTemp->StagingFailed();
			return NULL;
		}

		StagingResourceTemp->StagingRealized();
	}

	if (*StagingResource != NULL)
		*StagingResource = StagingResourceTemp;

	return Texture;
}

ZEHolder<ZEGRTextureCube> ZEGRTextureCube::LoadResource(const ZEString& FileName, const ZEGRTextureCubeOptions& TextureOptions)
{
	return ZERSTemplates::LoadResource<ZEGRTextureCube>(FileName, Instanciator, &TextureOptions);
}

ZEHolder<const ZEGRTextureCube> ZEGRTextureCube::LoadResourceShared(const ZEString& FileName, const ZEGRTextureCubeOptions& TextureOptions)
{

	return ZERSTemplates::LoadResourceShared<ZEGRTextureCube>(FileName, Instanciator, &TextureOptions);
}
