//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETexture2DResource.cpp
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

#include "ZETexture2D.h"
#include "ZETexture2DResource.h"
#include "ZECore\ZEError.h"
#include "ZECore\ZEConsole.h"
#include "ZECore\ZEResourceManager.h"
#include "ZECore\ZEResourceFile.h"
#include "ZETextureLoader.h"
#include "ZEGraphics\ZETextureTools.h"


#define FREEIMAGE_LIB
#include <freeimage.h>
#include <sys/stat.h>
#include <stdio.h>
#include <math.h>

static unsigned DLL_CALLCONV	FreeImageFile_Read_2D(void *buffer, unsigned size, unsigned count, fi_handle handle);
static int DLL_CALLCONV			FreeImageFile_Seek_2D(fi_handle handle, long offset, int origin);
static long DLL_CALLCONV		FreeImageFile_Tell_2D(fi_handle handle);


const char* ZETexture2DResource::GetResourceType() const
{
	return "Texture Resource";
}

ZETextureType ZETexture2DResource::GetTextureType() const 
{
	return ZE_TT_2D;
}

const ZETexture2D* ZETexture2DResource::GetTexture() const
{
	return Texture;
}

ZETexture2DResource::ZETexture2DResource()
{
	Texture = NULL;
};

ZETexture2DResource::~ZETexture2DResource()
{
	if (Texture != NULL)
		Texture->Release();
};

ZETexture2DResource* ZETexture2DResource::LoadResource(const char* FileName, const ZETextureOptions *UserOptions)
{
	ZETexture2DResource* TextureResource;
	ZEResourceFile File;

	if (File.Open(FileName))
	{
		TextureResource = LoadResource(&File, false, UserOptions);
		File.Close();
		return TextureResource;
	}
	else
	{
		zeError("TextureResource", "Texture file not found. FileName : \"%s\"", FileName);
		return NULL;
	}
}
	
ZETexture2DResource* ZETexture2DResource::LoadSharedResource(const char* FileName, const ZETextureOptions *UserOptions)
{	
	ZETexture2DResource* NewResource =(ZETexture2DResource*)zeResources->GetResource(FileName);
	if (NewResource == NULL)
	{		
		NewResource = LoadResource(FileName, UserOptions);
		if (NewResource != NULL)
		{
			NewResource->Shared = true;
			NewResource->Cached = false;
			NewResource->ReferenceCount = 1;
			zeResources->AddResource(NewResource);
			return NewResource;
		}
		else
			return NULL;
	}
}

ZETexture2DResource* ZETexture2DResource::LoadResource(ZEResourceFile* ResourceFile, bool EmbededResource, const ZETextureOptions *UserOptions)
{
	if ((zeGraphics->GetTextureQuality() != ZE_TQ_VERY_HIGH) && (!EmbededResource))
	{
		if (CheckInFileCache(ResourceFile->GetFileName()))
		{
			return LoadFromFileCache(ResourceFile->GetFileName());
		}
		else
		{			
			return LoadFromFile(ResourceFile, UserOptions);
		}
	}
	else // If Texture Quality Is Max Or It Is EmbededResource Load With Special Options
	{
		//ZETextureLoadOptions AutoTextureQuality = {UserOptions->MinimumQuality, UserOptions->MaximumQuality, false, {ZE_TCT_NONE, ZE_TCQ_NORMAL, ZE_TDS_NONE, true, true, 0}};
		return LoadFromFile(ResourceFile, UserOptions);
	}
}

bool ZETexture2DResource::CheckInFileCache(const char *FileName)
{
	char CompletePath[ZE_MAX_FILE_NAME_SIZE];
	sprintf(CompletePath, "resources\\%s.cache", FileName);

	int Stat;
	struct stat FileInfo;

	Stat = stat(CompletePath, &FileInfo);

	if (Stat == 0)
		return true;

	return false;
}

ZETexture2DResource* ZETexture2DResource::LoadFromFile(ZEResourceFile* ResourceFile, const ZETextureOptions* UserOptions)
{
	zeLog("Texture2D Resource", "LOADING texture from ORIGINAL FILE  \"%s\".", ResourceFile->GetFileName());	

	ZETextureOptions *DefaultOptions = zeGraphics->GetTextureOptions();

	if (UserOptions == NULL)
		UserOptions = zeGraphics->GetTextureOptions();

	// Combining engine options with user options. 
	// User options that are other than auto overrides engine options.
	ZETextureOptions		Options;
	Options.CompressionType		= UserOptions->CompressionType == ZE_TCT_AUTO ? DefaultOptions->CompressionType : UserOptions->CompressionType;
	Options.CompressionQuality	= UserOptions->CompressionQuality == ZE_TCQ_AUTO ? DefaultOptions->CompressionQuality : UserOptions->CompressionQuality;
	Options.DownSample			= UserOptions->DownSample == ZE_TDS_AUTO ? DefaultOptions->DownSample : Options.DownSample = UserOptions->DownSample;
	Options.MipMapping			= UserOptions->MipMapping == ZE_TMM_AUTO ? DefaultOptions->MipMapping : UserOptions->MipMapping;
	Options.FileCaching			= UserOptions->FileCaching == ZE_TFC_AUTO ? DefaultOptions->FileCaching : UserOptions->FileCaching;

	ZETextureLoaderInfo	TextureInfo;
	ZETextureLoader::GetTextureInfo(TextureInfo, ResourceFile);

	unsigned char	*RawTexture;
	RawTexture = (unsigned char*)malloc(TextureInfo.TextureHeight * TextureInfo.TextureWidth * 4);
	if (RawTexture == NULL)
	{
		zeError("Texture Resource","Cannot Allocate Memmory for the Texture. FileName : \"%s\"", ResourceFile->GetFileName());
		return NULL;
	}

	ZETextureLoader::LoadTexture((void*)RawTexture, ResourceFile, TextureInfo);
	

	bool	IsResizeable = true;
	bool	IsCompressible = true;

	// Creating texture options constrains
	// Check if it is Resizeable (power of 2)
	if (!ZETextureTools::IsResizeable(TextureInfo))
	{
		IsResizeable = false;
		Options.DownSample = ZE_TDS_NONE;
		Options.MipMapping = ZE_TMM_DISABLED;
		Options.MaximumMipmapLevel = 0;
	}

	// Check if it is Compressible (divisible by 4)
	if (!ZETextureTools::IsCompressible(TextureInfo))
	{
		IsCompressible = false;
		Options.CompressionType = ZE_TCT_NONE;
	}

	// MipMapping
	unsigned int	PossibleMaxMipmapNumber;
	unsigned int	MipMapCount = ZETextureTools::GetMaxMipmapCount(TextureInfo);

	switch (Options.DownSample)
	{
		case ZE_TDS_8X:
			PossibleMaxMipmapNumber = MipMapCount - 3; // With 4x down sample MaxMipmapNumber have to be 2 less than its normal value
			break;
		case ZE_TDS_4X:
			PossibleMaxMipmapNumber = MipMapCount - 2; // With 4x down sample MaxMipmapNumber have to be 2 less than its normal value	
			break;
		case ZE_TDS_2X:
			PossibleMaxMipmapNumber = MipMapCount - 1; // With 2x down sample MaxMipmapNumber have to be 1 less than its normal value
			break;
		default:
			PossibleMaxMipmapNumber = MipMapCount;
			break;
	}

	// If Users MipMapping Level is Valid Use It
	if (Options.MipMapping != ZE_TMM_DISABLED)
		Options.MaximumMipmapLevel = UserOptions->MaximumMipmapLevel > PossibleMaxMipmapNumber ? PossibleMaxMipmapNumber : UserOptions->MaximumMipmapLevel;
	else
		Options.MaximumMipmapLevel = 0;

	//Decide Pixel Format
	ZETexturePixelFormat	PixelFormat;
	switch(Options.CompressionType)
	{
		case ZE_TCT_AUTO:
		case ZE_TCT_DXT3:
			PixelFormat = ZE_TPF_RGBA_DXT3;
			break;

		case ZE_TCT_NONE:
			PixelFormat = ZE_TPF_RGBA_INT32;
			break;

		case ZE_TCT_DXT1:
			PixelFormat = ZE_TPF_RGBA_DXT1;
			break;

		case ZE_TCT_DXT5:
			PixelFormat = ZE_TPF_RGBA_DXT5;
			break;
	}

	// Downsample if it is requested by Loading Options
	switch (Options.DownSample)
	{
		case ZE_TDS_8X:
			zeLog("Texture2D Resource", "RESIZING texture by 8X:  \"%s\".", ResourceFile->GetFileName());
			ZETextureTools::DownSample2x(RawTexture, TextureInfo.TexturePitch, RawTexture, TextureInfo.TexturePitch, TextureInfo.TextureWidth, TextureInfo.TextureHeight);
			TextureInfo.TextureWidth /= 2;
			TextureInfo.TextureHeight /= 2;
			ZETextureTools::DownSample2x(RawTexture, TextureInfo.TexturePitch, RawTexture, TextureInfo.TexturePitch, TextureInfo.TextureWidth, TextureInfo.TextureHeight);
			TextureInfo.TextureWidth /= 2;
			TextureInfo.TextureHeight /= 2;
			ZETextureTools::DownSample2x(RawTexture, TextureInfo.TexturePitch, RawTexture, TextureInfo.TexturePitch, TextureInfo.TextureWidth, TextureInfo.TextureHeight);
			TextureInfo.TextureWidth /= 2;
			TextureInfo.TextureHeight /= 2;
			break;

		case ZE_TDS_4X:
			zeLog("Texture2D Resource", "RESIZING texture by 4X:  \"%s\".", ResourceFile->GetFileName());
			ZETextureTools::DownSample2x(RawTexture, TextureInfo.TexturePitch, RawTexture, TextureInfo.TexturePitch, TextureInfo.TextureWidth, TextureInfo.TextureHeight);
			TextureInfo.TextureWidth /= 2;
			TextureInfo.TextureHeight /= 2;
			ZETextureTools::DownSample2x(RawTexture, TextureInfo.TexturePitch, RawTexture, TextureInfo.TexturePitch, TextureInfo.TextureWidth, TextureInfo.TextureHeight);
			TextureInfo.TextureWidth /= 2;
			TextureInfo.TextureHeight /= 2;
			break;

		case ZE_TDS_2X:
			zeLog("Texture2D Resource", "RESIZING texture by 2X:  \"%s\".", ResourceFile->GetFileName());
			ZETextureTools::DownSample2x(RawTexture, TextureInfo.TexturePitch , RawTexture, TextureInfo.TexturePitch, TextureInfo.TextureWidth, TextureInfo.TextureHeight);
			TextureInfo.TextureWidth /= 2;
			TextureInfo.TextureHeight /= 2;
			break;

		case ZE_TDS_NONE:
		default:
			break;
	}

	// Create TextureResource 
	ZETexture2DResource* TextureResource = new ZETexture2DResource();
	ZETexture2D* Texture = TextureResource->Texture = ZETexture2D::CreateInstance();

	// Set Other Variables
	TextureResource->SetFileName(ResourceFile->GetFileName());
	TextureResource->Cached = false;
	TextureResource->Shared = false;

	// Create the Texture
	if (!Texture->Create(TextureInfo.TextureWidth, TextureInfo.TextureHeight, PixelFormat, false, Options.MaximumMipmapLevel))
	{
		zeError("Texture Resource", "Can not create texture resource. FileName : \"%s\"", ResourceFile->GetFileName());
		delete TextureResource;
		return NULL;
	}


	// Decide the Case for MipMap Creation
	unsigned int Case;

	//Compression && Mipmapping
	if (Options.CompressionType != ZE_TCT_NONE && Options.MipMapping != ZE_TMM_DISABLED)
		Case = 1;
	//No Compression && Mipmapping
	else if (Options.CompressionType == ZE_TCT_NONE && Options.MipMapping != ZE_TMM_DISABLED)
		Case = 2;
	//Compression && No Mipmapping case
	else if (Options.CompressionType != ZE_TCT_NONE && Options.MipMapping == ZE_TMM_DISABLED)
		Case = 3;
	//No Compression && No Mipmapping
	else 
		Case = 4;

	void* Buffer = NULL;
	unsigned int DestinationPitch;
	unsigned int Levels = Options.MaximumMipmapLevel;
	switch(Case)
	{
		case 1://Compression && Mipmapping case
			// Create Mipmaps for level-2 (2x2 and 1x1 not compressible)
			for (size_t I = 0; I < Levels - 2; I++)
			{
				TextureResource->Texture->Lock(&Buffer, &DestinationPitch, I);
				ZETextureTools::CompressTexture(Buffer, DestinationPitch, RawTexture, TextureInfo.TexturePitch, TextureInfo.TextureWidth, TextureInfo.TextureHeight, &Options);
				TextureResource->Texture->Unlock();
				ZETextureTools::DownSample2x(RawTexture, TextureInfo.TexturePitch , RawTexture, TextureInfo.TexturePitch, TextureInfo.TextureWidth, TextureInfo.TextureHeight);
				TextureInfo.TextureWidth /= 2;
				TextureInfo.TextureHeight /= 2;
			}

			// 2x2 Mipmap
			TextureResource->Texture->Lock(&Buffer, &DestinationPitch, Levels - 2);
			for (size_t I = 0; I < TextureInfo.TextureHeight; I++)
				memcpy((unsigned char*)Buffer + (I * DestinationPitch), RawTexture + (I * TextureInfo.TexturePitch), TextureInfo.TextureWidth * TextureInfo.BitsPerPixel/8);
			TextureResource->Texture->Unlock();

			// 1x1 Mipmap
			TextureResource->Texture->Lock(&Buffer, &DestinationPitch, Levels - 1);
			ZETextureTools::DownSample2x(Buffer, DestinationPitch , RawTexture, TextureInfo.TexturePitch, TextureInfo.TextureWidth, TextureInfo.TextureHeight);
			TextureResource->Texture->Unlock();
			break;

		case 2://No Compression && Mipmapping
			for (size_t I = 0; I < Levels; I++)
			{
				// Write Mipmap to Device Level I
				ZETextureType Type;
				Type = TextureResource->GetTextureType();

				TextureResource->Texture->Lock(&Buffer, &DestinationPitch, I);
				for (size_t K = 0; K < TextureInfo.TextureHeight; K++)
					memcpy((unsigned char*)Buffer + (K * DestinationPitch), RawTexture + (K * TextureInfo.TexturePitch), TextureInfo.TextureWidth * TextureInfo.BitsPerPixel / 8);
				TextureResource->Texture->Unlock();
				ZETextureTools::DownSample2x(RawTexture, TextureInfo.TexturePitch , RawTexture, TextureInfo.TexturePitch, TextureInfo.TextureWidth, TextureInfo.TextureHeight);
				TextureInfo.TextureWidth /= 2;
				TextureInfo.TextureHeight /= 2;
			}
			break;

		case 3://Compression && No Mipmapping case
			TextureResource->Texture->Lock(&Buffer, &DestinationPitch, 0);
			ZETextureTools::CompressTexture(Buffer, DestinationPitch, RawTexture, TextureInfo.TexturePitch, TextureInfo.TextureWidth, TextureInfo.TextureHeight, &Options);
			TextureResource->Texture->Unlock();
			break;

		case 4://No Compression && No Mipmapping
			TextureResource->Texture->Lock(&Buffer, &DestinationPitch, 0);
			for (size_t I = 0; I < TextureInfo.TextureHeight; I++)
				memcpy((unsigned char*)Buffer + (I * DestinationPitch), RawTexture + I * (TextureInfo.TextureWidth * TextureInfo.BitsPerPixel/8), TextureInfo.TextureWidth * TextureInfo.BitsPerPixel/8);
			TextureResource->Texture->Unlock();
			break;

		default:
			break;
	}

	/* Yazılacak */
	if(Options.FileCaching != ZE_TFC_DISABLED)
		SaveToFileCache();

	free(RawTexture);

	return TextureResource;
}
ZETexture2DResource* ZETexture2DResource::LoadFromFileCache(const char *FileName)
{
	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */

	return false;
}
bool ZETexture2DResource::SaveToFileCache()
{
	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */

	return false;
}
bool ZETexture2DResource::CreateMipmaps(ZETexture2DResource* TextureResource, unsigned char* Image, unsigned int Width, unsigned int Height, unsigned int BPP, unsigned int Pitch, bool IsResizeable, const ZETextureOptions* MipmapOptions)
{
	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */

	return false;
}

static unsigned DLL_CALLCONV FreeImageFile_Read_2D(void *buffer, unsigned size, unsigned count, fi_handle handle) 
{
	return (unsigned int)((ZEResourceFile*)handle)->Read(buffer, size, count);
}

static int DLL_CALLCONV FreeImageFile_Seek_2D(fi_handle handle, long offset, int origin) 
{
	ZESeekFrom OriginNorm;
	switch(origin)
	{
	case SEEK_SET:
		OriginNorm = ZE_SF_BEGINING;
		break;
	case SEEK_CUR:
		OriginNorm = ZE_SF_CURRENT;
		break;
	case SEEK_END:
		OriginNorm = ZE_SF_END;
		break;
	}

	return ((ZEResourceFile*)handle)->Seek(offset, OriginNorm);
}

static long DLL_CALLCONV FreeImageFile_Tell_2D(fi_handle handle) 
{
	return (long)((ZEResourceFile*)handle)->Tell();
}
