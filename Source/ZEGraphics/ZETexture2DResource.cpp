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
#include "ZECore/ZEError.h"
#include "ZECore/ZEConsole.h"
#include "ZECore/ZEResourceManager.h"
#include "ZECore/ZEResourceFile.h"
#include "ZEGraphics/ZETextureLoader.h"
#include "ZEGraphics/ZETextureTools.h"
#include "ZEGraphics/ZETextureCacheChunkIdentifier.h"
#include "ZERealTimeTextureCompressor.h"
#include "ZEDS/ZEFileCache.h"

#include <sys/stat.h>
#include <stdio.h>
#include <math.h>

#define	ZE_TC_BLOCK_WIDTH	4
#define ZE_TC_BLOCK_HEIGHT	4


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
	
ZETexture2DResource* ZETexture2DResource::LoadSharedResource(const char* FileName, const ZETextureOptions* UserOptions)
{	
	ZETexture2DResource* NewResource =(ZETexture2DResource*)zeResources->GetResource(FileName);
	if (NewResource == NULL)
	{		
		if(!UserOptions)
			UserOptions = zeGraphics->GetTextureOptions();

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
		{
			zeError("TextureResource", "Texture file not found. FileName : \"%s\"", FileName);
			return NULL;
		}
	}
	else
		return NewResource;
}

ZETexture2DResource* ZETexture2DResource::LoadResource(const char* FileName, const ZETextureOptions* UserOptions)
{
	ZETexture2DResource* TextureResource;
	ZEResourceFile File;

	if (File.Open(FileName))
	{
		if(!UserOptions)
			UserOptions = zeGraphics->GetTextureOptions();

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

ZETexture2DResource* ZETexture2DResource::LoadResource(ZEResourceFile* ResourceFile, bool EmbededResource, const ZETextureOptions* UserOptions)
{
	if(!UserOptions)
		UserOptions = zeGraphics->GetTextureOptions();

	ZETexture2DResource* TextureResource = NULL;
	if(UserOptions->FileCaching != ZE_TFC_DISABLED && zeGraphics->GetTextureQuality() != ZE_TQ_VERY_HIGH && !EmbededResource)
	{
		TextureResource = LoadFromFileCache(ResourceFile, ResourceFile->GetFileName(), UserOptions);
		if(TextureResource == NULL)
			zeLog("Texture 2D Resource", "Coudnt laod the texture from cache. \"%s\". Trying to load it from original file.", ResourceFile->GetFileName());
	}
	if(TextureResource == NULL)
		TextureResource = LoadFromFile(ResourceFile, UserOptions);

	return TextureResource;
}

void ZETexture2DResource::GetFinalTextureOptions(ZETextureOptions& FinalOptions, ZEResourceFile* ResourceFile, const ZETextureOptions* UserOptions)
{
	ZETextureOptions *DefaultOptions = zeGraphics->GetTextureOptions();

	if(UserOptions == NULL)
		UserOptions = zeGraphics->GetTextureOptions();

	FinalOptions.CompressionType		= UserOptions->CompressionType == ZE_TCT_AUTO		? DefaultOptions->CompressionType : UserOptions->CompressionType;
	FinalOptions.CompressionQuality		= UserOptions->CompressionQuality == ZE_TCQ_AUTO	? DefaultOptions->CompressionQuality : UserOptions->CompressionQuality;
	FinalOptions.DownSample				= UserOptions->DownSample == ZE_TDS_AUTO			? DefaultOptions->DownSample :  UserOptions->DownSample;
	FinalOptions.MipMapping				= UserOptions->MipMapping == ZE_TMM_AUTO			? DefaultOptions->MipMapping : UserOptions->MipMapping;
	FinalOptions.FileCaching			= UserOptions->FileCaching == ZE_TFC_AUTO			? DefaultOptions->FileCaching : UserOptions->FileCaching;

	ZETextureLoaderInfo	TextureInfo;
	ZETextureLoader::GetTextureInfo(TextureInfo, ResourceFile);

	bool	IsResizeable = true;
	bool	IsCompressible = true;

	// Creating texture options constrains
	// Check if it is Resizeable (power of 2)
	if (!ZETextureTools::IsResizeable(TextureInfo))
	{
		IsResizeable = false;
		FinalOptions.DownSample = ZE_TDS_NONE;
		FinalOptions.MipMapping = ZE_TMM_DISABLED;
		FinalOptions.MaximumMipmapLevel = 0;
	}

	// Check if it is Compressible (divisible by 4)
	if (!ZETextureTools::IsCompressible(TextureInfo))
	{
		IsCompressible = false;
		FinalOptions.CompressionType = ZE_TCT_NONE;
	}

	// MipMapping
	unsigned int	PossibleMaxMipmapNumber;
	unsigned int	MipMapCount = ZETextureTools::GetMaxMipmapCount(TextureInfo);

	switch (FinalOptions.DownSample)
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
	if (FinalOptions.MipMapping != ZE_TMM_DISABLED)
		FinalOptions.MaximumMipmapLevel = UserOptions->MaximumMipmapLevel > PossibleMaxMipmapNumber ? PossibleMaxMipmapNumber : UserOptions->MaximumMipmapLevel;
	else
		FinalOptions.MaximumMipmapLevel = 1;

}

ZETexture2DResource* ZETexture2DResource::LoadFromFile(ZEResourceFile* ResourceFile, const ZETextureOptions* UserOptions)
{
	zeLog("Texture2D Resource", "LOADING texture from ORIGINAL FILE  \"%s\".", ResourceFile->GetFileName());	

	ZETextureLoaderInfo	TextureInfo;
	ZETextureLoader::GetTextureInfo(TextureInfo, ResourceFile);

	ZETextureOptions Options;
	GetFinalTextureOptions(Options, ResourceFile, UserOptions);

	unsigned char	*RawTexture;
	RawTexture = (unsigned char*)malloc(TextureInfo.TextureHeight * TextureInfo.TextureWidth * 4);
	if (RawTexture == NULL)
	{
		zeError("Texture Resource","Cannot Allocate Memmory for the Texture. FileName : \"%s\"", ResourceFile->GetFileName());
		return NULL;
	}

	ZETextureLoader::LoadTexture((void*)RawTexture, ResourceFile, TextureInfo);
	

	//Decide Pixel Format
	ZETexturePixelFormat	PixelFormat;
	switch(Options.CompressionType)
	{
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

	// If caching is on
	bool			CacheIt = false;
	ZEPartialFile	PartialFile;
	ZEFileCache		TextureFileCache;
	unsigned int	TotalTextureSize = 0;
	unsigned int	DownsampleMultiplier = 1;
	unsigned int	CompressionMultiplier = 1;
	unsigned int	CompressionBlockSize = 1;
	

	if(Options.FileCaching != ZE_TFC_DISABLED)
	{
		// Open the cache
		TextureFileCache.OpenCache("resources\\TextureFileCache.ZEFileCache");
		// Create the identifier
		ZETextureCacheChunkIdentifier Identifier(ResourceFile->GetFileName(), Options, 0);
		//Check if texture is already in cache
		if(!TextureFileCache.ChunkExists(&Identifier))
		{
			// If chunk does not exists we will cache it
			CacheIt = true;

			// If there is resizing
			switch(Options.DownSample)
			{
				case ZE_TDS_2X:
					DownsampleMultiplier *= 4;
					break;
				case ZE_TDS_4X:
					DownsampleMultiplier *= 16;
					break;
				case ZE_TDS_8X:
					DownsampleMultiplier *= 64;
					break;
				case ZE_TDS_NONE:
				default:
					break;
			}

			// If there is compression
			switch(Options.CompressionType)
			{
				case ZE_TCT_DXT1:
					CompressionMultiplier *= 8;
					CompressionBlockSize *= 8;
					break;

				case ZE_TCT_DXT5:
				case ZE_TCT_DXT3:
					CompressionMultiplier *= 4;
					CompressionBlockSize *= 16;
					break;

				case ZE_TCT_NONE:
				default:
					break;
			}

			//Calculate the total texture size for caching
			unsigned int LevelZeroSize = (TextureInfo.TextureWidth * TextureInfo.TextureHeight * (TextureInfo.BitsPerPixel / 8)) / (CompressionMultiplier);
			TotalTextureSize += LevelZeroSize;

			// If there is mipmapping
			if(Options.MipMapping == ZE_TMM_ENABLED)
			{
				

				unsigned int Levels = Options.MaximumMipmapLevel - 3;
				while(Levels > 0)
				{
					TotalTextureSize += LevelZeroSize /= 4;
					Levels--;
				}

				// if there is compression the 2x2 and 1x1 mipmap levels will be the same with 4x4 level
				if(Options.CompressionType != ZE_TCT_NONE)
				{
					TotalTextureSize += 32;
				}
				else // if there is no compression there wont be any exceptions about last 2 mipmap levels
				{
					TotalTextureSize += LevelZeroSize /= 4;
					TotalTextureSize += LevelZeroSize /= 4;
				}
			}

			// Create a new chunk as a partial resource file
			TextureFileCache.CreateChunk(PartialFile, &Identifier, TotalTextureSize);
		}
	}

	

	void* Buffer = NULL;
	unsigned int DestinationPitch;

	// For test purpose
	unsigned int WriteCount = 0;
	unsigned int WriteSize = 0;
	unsigned int RowSizeToWrite = 0;
	unsigned int TotalWriteSize = 0;

	//Compression && Mipmapping Case
	if (Options.CompressionType != ZE_TCT_NONE && Options.MipMapping != ZE_TMM_DISABLED)
	{
		// Create Mipmaps for level-2 (2x2 and 1x1 not compressible use the same data of 4x4 compressed texture)
		for (size_t I = 0; I < Options.MaximumMipmapLevel - 2; I++)
		{
			TextureResource->Texture->Lock(&Buffer, &DestinationPitch, I);
			ZETextureTools::CompressTexture(Buffer, DestinationPitch, RawTexture, TextureInfo.TexturePitch, TextureInfo.TextureWidth, TextureInfo.TextureHeight, &Options);
			//ZERealTimeTextureCompressor::Compress(Buffer, DestinationPitch, );


			if (CacheIt)
			{
				WriteSize = 0;
				RowSizeToWrite = (TextureInfo.TextureWidth / ZE_TC_BLOCK_WIDTH) * CompressionBlockSize;

				for(size_t J = 0; J < TextureInfo.TextureHeight / ZE_TC_BLOCK_HEIGHT; J++)
				{
					WriteCount = PartialFile.Write((unsigned char*)Buffer + J * DestinationPitch, RowSizeToWrite, 1);
					if(WriteCount)
					{
						WriteSize += RowSizeToWrite;
						WriteCount = 0;
					}

				}
			}
			TotalWriteSize += WriteSize;

			TextureResource->Texture->Unlock(I);

			// Do not downsample when 4x4 size reached
			if(TextureInfo.TextureWidth > ZE_TC_BLOCK_WIDTH && TextureInfo.TextureHeight > ZE_TC_BLOCK_HEIGHT)
			{
				ZETextureTools::DownSample2x(RawTexture, TextureInfo.TexturePitch , RawTexture, TextureInfo.TexturePitch, TextureInfo.TextureWidth, TextureInfo.TextureHeight);
				TextureInfo.TextureWidth /= 2;
				TextureInfo.TextureHeight /= 2;
			}
		}

		// 2x2 Mipmap
		// Compress the last 4x4 texture and put it to 2x2 mipmap level
		TextureResource->Texture->Lock(&Buffer, &DestinationPitch, Options.MaximumMipmapLevel - 2);
		ZETextureTools::CompressTexture(Buffer, DestinationPitch, RawTexture, TextureInfo.TexturePitch, TextureInfo.TextureWidth, TextureInfo.TextureHeight, &Options);
		
		
		//2x2 mipmap will be just one block
		if (CacheIt)
		{
			WriteSize = 0;
			RowSizeToWrite = CompressionBlockSize;

			WriteCount = PartialFile.Write((unsigned char*)Buffer, RowSizeToWrite, 1);
			if(WriteCount)
			{
				WriteSize += RowSizeToWrite;
				WriteCount = 0;
			}
		}
		TotalWriteSize += WriteSize;


		TextureResource->Texture->Unlock(Options.MaximumMipmapLevel - 2);

		// 1x1 Mipmap
		// Compress the last 4x4 texture and put it to 1x1 mipmap level
		TextureResource->Texture->Lock(&Buffer, &DestinationPitch, Options.MaximumMipmapLevel - 1);
		ZETextureTools::CompressTexture(Buffer, DestinationPitch, RawTexture, TextureInfo.TexturePitch, TextureInfo.TextureWidth, TextureInfo.TextureHeight, &Options);
		

		//1x1 mipmap will be just one block
		if (CacheIt)
		{
			WriteSize = 0;
			RowSizeToWrite = CompressionBlockSize;

			WriteCount = PartialFile.Write((unsigned char*)Buffer, RowSizeToWrite, 1);
			if(WriteCount)
			{
				WriteSize += RowSizeToWrite;
				WriteCount = 0;
			}
		}
		TotalWriteSize += WriteSize;
		
		TextureResource->Texture->Unlock(Options.MaximumMipmapLevel - 1);

	}
	//No Compression && Mipmapping  Case
	else if (Options.CompressionType == ZE_TCT_NONE && Options.MipMapping != ZE_TMM_DISABLED)	
	{
		for (size_t I = 0; I < Options.MaximumMipmapLevel; I++)
		{
			// Write Mipmap to Device Level I
			TextureResource->Texture->Lock(&Buffer, &DestinationPitch, I);
			for (size_t K = 0; K < TextureInfo.TextureHeight; K++)
				memcpy((unsigned char*)Buffer + (K * DestinationPitch), RawTexture + (K * TextureInfo.TexturePitch), TextureInfo.TextureWidth * TextureInfo.BitsPerPixel / 8);


			if (CacheIt)
			{
				WriteSize = 0;
				RowSizeToWrite = TextureInfo.TextureWidth * TextureInfo.BitsPerPixel / 8;

				for(size_t J = 0; J < TextureInfo.TextureHeight; J++)
				{
					// Read it from the raw texture, not from d3d pointer
					WriteCount = PartialFile.Write(RawTexture + (J * TextureInfo.TexturePitch), RowSizeToWrite, 1);
					if(WriteCount)
					{
						WriteSize += RowSizeToWrite;
						WriteCount = 0;
					}

				}
			}
			TotalWriteSize += WriteSize;


			TextureResource->Texture->Unlock(I);
			ZETextureTools::DownSample2x(RawTexture, TextureInfo.TexturePitch , RawTexture, TextureInfo.TexturePitch, TextureInfo.TextureWidth, TextureInfo.TextureHeight);
			TextureInfo.TextureWidth /= 2;
			TextureInfo.TextureHeight /= 2;
		}
	}
	//Compression && No Mipmapping case  Case
	else if (Options.CompressionType != ZE_TCT_NONE && Options.MipMapping == ZE_TMM_DISABLED)
	{
		TextureResource->Texture->Lock(&Buffer, &DestinationPitch, 0);
		ZETextureTools::CompressTexture(Buffer, DestinationPitch, RawTexture, TextureInfo.TexturePitch, TextureInfo.TextureWidth, TextureInfo.TextureHeight, &Options);


		if (CacheIt)
		{
			WriteSize = 0;
			RowSizeToWrite = (TextureInfo.TextureWidth / ZE_TC_BLOCK_WIDTH) * CompressionBlockSize;

			for(size_t J = 0; J < TextureInfo.TextureHeight / ZE_TC_BLOCK_HEIGHT; J++)
			{
				WriteCount = PartialFile.Write((unsigned char*)Buffer + J * DestinationPitch, RowSizeToWrite, 1);
				if(WriteCount)
				{
					WriteSize += RowSizeToWrite;
					WriteCount = 0;
				}

			}
		}
		TotalWriteSize += WriteSize;


		TextureResource->Texture->Unlock(0);
	}
	//No Compression && No Mipmapping  Case
	else 
	{
		TextureResource->Texture->Lock(&Buffer, &DestinationPitch, 0);
		for (size_t I = 0; I < TextureInfo.TextureHeight; I++)
			memcpy((unsigned char*)Buffer + (I * DestinationPitch), RawTexture + I * (TextureInfo.TextureWidth * TextureInfo.BitsPerPixel/8), TextureInfo.TextureWidth * TextureInfo.BitsPerPixel/8);


		if (CacheIt)
		{
			WriteSize = 0;
			RowSizeToWrite = TextureInfo.TextureWidth * TextureInfo.BitsPerPixel / 8;

			for(size_t J = 0; J < TextureInfo.TextureHeight; J++)
			{
				// Read it from the raw texture, not from d3d pointer
				WriteCount = PartialFile.Write(RawTexture + (J * TextureInfo.TexturePitch), RowSizeToWrite, 1);
				if(WriteCount)
				{
					WriteSize += RowSizeToWrite;
					WriteCount = 0;
				}

			}
		}
		TotalWriteSize += WriteSize;


		TextureResource->Texture->Unlock(0);
	}


	if(CacheIt)
	{
		PartialFile.Close();
		TextureFileCache.CloseCache();
	}

	free(RawTexture);

	return TextureResource;
}

ZETexture2DResource* ZETexture2DResource::LoadFromFileCache(ZEResourceFile* ResourceFile, const char* TextureFileName, const ZETextureOptions *Options)
{
	ZETextureOptions FinalTextureOptions;
	GetFinalTextureOptions(FinalTextureOptions, ResourceFile, Options);

	// Create the chunk identifier
	ZETextureCacheChunkIdentifier TextureCacheIdentifier(TextureFileName, FinalTextureOptions, 0);

	// Create and open the file cache
	ZEFileCache FileCache;
	FileCache.OpenCache("resources\\TextureFileCache.ZEFileCache");

	// Check if the requested texture is in the file cache
	if (!FileCache.ChunkExists(&TextureCacheIdentifier)) // LoadFrom Cache
	{
		zeLog("Texture 2D Resource", "Item not found in file cache with the name:  \"%s\".", ResourceFile->GetFileName());
		//zeError("Texture 2D Resource", "The chunk specified by the identifier is not found in file cache: \"%s\".", FileCache.GetCacheFileName());
		FileCache.CloseCache();
		return NULL;
	}
		
	// Create the Texture info
	ZETextureLoaderInfo TextureInfo;
	memset(&TextureInfo, 0, sizeof(ZETextureLoaderInfo));


	unsigned int TextureWidth	= 0;
	unsigned int TextureHeight	= 0;
	unsigned int MipMapCount	= 0;
	unsigned int TextureSize	= 0;
	unsigned int Multiplier		= 1;
	ZETexturePixelFormat PixelFormat = ZE_TPF_RGBA_INT32;

	// Get the original texture's options
	if(!ZETextureLoader::GetTextureInfo(TextureInfo, ResourceFile))
	{
		zeLog("Texture 2D Resource", "Cannot get texture info from the file: \"%s\".", TextureFileName);
		FileCache.CloseCache();
		return NULL;
	}

	// Little hack here
	// Get texture info function returns texture's real BPP
	// Since all textures are loaded as 32bits just set it to 32 bit
	TextureInfo.BitsPerPixel = 32;
	TextureInfo.TexturePitch = TextureInfo.TextureWidth * TextureInfo.BitsPerPixel / 8;

	unsigned int	TotalTextureSize = 0;
	unsigned int	DownsampleMultiplier = 1;
	unsigned int	CompressionMultiplier = 1;
	unsigned int	CompressionBlockSize = 1;
	
	// Calculate the multiplier if there is resizing
	if(ZETextureTools::IsResizeable(TextureInfo))
	{
		switch(Options->DownSample)
		{
			case ZE_TDS_2X:
				DownsampleMultiplier *= 4;
				break;

			case ZE_TDS_4X:
				DownsampleMultiplier *= 16;
				break;

			case ZE_TDS_8X:
				DownsampleMultiplier *= 64;
				break;

			case ZE_TDS_NONE:
			default:
				break;
		}
	}
		
	// Calculate the multiplier if there is compression
	if(ZETextureTools::IsCompressible(TextureInfo))
	{
		switch (Options->CompressionType)
		{
			case ZE_TCT_DXT1:
				CompressionMultiplier *= 8;
				CompressionBlockSize *= 8;
				PixelFormat = ZE_TPF_RGBA_DXT1;
				break;

			case ZE_TCT_DXT3:
				CompressionMultiplier *= 4;
				CompressionBlockSize *= 16;
				PixelFormat = ZE_TPF_RGBA_DXT3;
				break;

			case ZE_TCT_DXT5:
				CompressionMultiplier *= 4;
				CompressionBlockSize *= 16;
				PixelFormat = ZE_TPF_RGBA_DXT5;
				break;

			case ZE_TCT_NONE:
			default:
				PixelFormat = ZE_TPF_RGBA_INT32;
				break;
		}
	}
	
	else
		PixelFormat = ZE_TPF_RGBA_INT32;

	TextureWidth = TextureInfo.TextureWidth / (unsigned int)sqrt((double)DownsampleMultiplier);
	TextureHeight = TextureInfo.TextureHeight / (unsigned int)sqrt((double)DownsampleMultiplier);
	MipMapCount = (FinalTextureOptions.MipMapping != ZE_TMM_DISABLED) ? FinalTextureOptions.MaximumMipmapLevel : 1;
	TextureSize = (TextureInfo.TextureWidth * TextureInfo.TextureHeight * (TextureInfo.BitsPerPixel / 8)) / (CompressionMultiplier * DownsampleMultiplier);

	// Open chunk for reading
	ZEPartialResourceFile PartialResourceFile;
	FileCache.OpenChunk(PartialResourceFile, &TextureCacheIdentifier);

	// Create TextureResource and texture
	ZETexture2DResource* TextureResource = new ZETexture2DResource();
	ZETexture2D* Texture = TextureResource->Texture = ZETexture2D::CreateInstance();
	TextureResource->SetFileName(TextureFileName);
	TextureResource->Cached = false;
	TextureResource->Shared = false;

	if(!Texture->Create(TextureWidth, TextureHeight, PixelFormat, false, MipMapCount))
	{
		zeLog("Texture 2D Resource", "Can not create texture");
		return NULL;
	}
		
	void* Buffer = NULL;
	unsigned int DestinationPitch;
	unsigned int RowSizeToRead = 0;
	unsigned int ReadSize = 0;
	unsigned int TotalReadSize = 0;
	unsigned int ReadCount = 0;

	

	//Compression && Mipmapping Case
	if (FinalTextureOptions.CompressionType != ZE_TCT_NONE && FinalTextureOptions.MipMapping != ZE_TMM_DISABLED)
	{
		// Create Mipmaps for level-2 (2x2 and 1x1 not compressible use the same data of 4x4 compressed texture)
		for (size_t I = 0; I < FinalTextureOptions.MaximumMipmapLevel - 2; I++)
		{
			TextureResource->Texture->Lock(&Buffer, &DestinationPitch, I);
			
			RowSizeToRead = (TextureWidth / ZE_TC_BLOCK_WIDTH) * CompressionBlockSize;

			for(size_t J = 0; J < TextureHeight / ZE_TC_BLOCK_HEIGHT; J++)
			{
				ReadCount = PartialResourceFile.Read((unsigned char*)Buffer + J * DestinationPitch, RowSizeToRead, 1);
				if(ReadCount)
				{
					ReadSize += RowSizeToRead;
					ReadCount = 0;
				}

			}
			
			TotalReadSize += ReadSize;
			TextureResource->Texture->Unlock(I);

			// Prepare to read for the next mipmap
			TextureWidth /= 2;
			TextureHeight /= 2;
		}

		// 2x2 Mipmap
		// Compress the last 4x4 texture and put it to 2x2 mipmap level
		TextureResource->Texture->Lock(&Buffer, &DestinationPitch, FinalTextureOptions.MaximumMipmapLevel - 2);
		
		ReadSize = 0;
		RowSizeToRead = CompressionBlockSize;

		ReadCount = PartialResourceFile.Read((unsigned char*)Buffer, RowSizeToRead, 1);
		if(ReadCount)
		{
			ReadSize += RowSizeToRead;
			ReadCount = 0;
		}

		TotalReadSize += ReadSize;


		TextureResource->Texture->Unlock(FinalTextureOptions.MaximumMipmapLevel - 2);

		// 1x1 Mipmap
		// Compress the last 4x4 texture and put it to 1x1 mipmap level
		TextureResource->Texture->Lock(&Buffer, &DestinationPitch, FinalTextureOptions.MaximumMipmapLevel - 1);
		
		//1x1 mipmap will be just one block
		ReadSize = 0;
		RowSizeToRead = CompressionBlockSize;

		ReadCount = PartialResourceFile.Read((unsigned char*)Buffer, RowSizeToRead, 1);
		if(ReadCount)
		{
			ReadSize += RowSizeToRead;
			ReadCount = 0;
		}
		
		TotalReadSize += ReadSize;

		TextureResource->Texture->Unlock(FinalTextureOptions.MaximumMipmapLevel - 1);

	}

	//No Compression && Mipmapping  Case
	else if (FinalTextureOptions.CompressionType == ZE_TCT_NONE && FinalTextureOptions.MipMapping != ZE_TMM_DISABLED)	
	{
		for (size_t I = 0; I < FinalTextureOptions.MaximumMipmapLevel; I++)
		{
			// Write Mipmap to Device Level I
			TextureResource->Texture->Lock(&Buffer, &DestinationPitch, I);
			
			ReadSize = 0;
			RowSizeToRead = TextureWidth * TextureInfo.BitsPerPixel / 8;

			for(size_t J = 0; J < TextureHeight; J++)
			{
				// Read it from the raw texture, not from d3d pointer
				ReadCount = PartialResourceFile.Read((unsigned char*)Buffer + J * DestinationPitch, RowSizeToRead, 1);
				if(ReadCount)
				{
					ReadSize += RowSizeToRead;
					ReadCount = 0;
				}

			}
			
			TotalReadSize += ReadSize;

			TextureResource->Texture->Unlock(I);
			
			TextureWidth /= 2;
			TextureHeight /= 2;
		}
	}
	//Compression && No Mipmapping case  Case
	else if (FinalTextureOptions.CompressionType != ZE_TCT_NONE && FinalTextureOptions.MipMapping == ZE_TMM_DISABLED)
	{
		TextureResource->Texture->Lock(&Buffer, &DestinationPitch, 0);
		
		ReadSize = 0;
		RowSizeToRead = (TextureWidth / ZE_TC_BLOCK_WIDTH) * CompressionBlockSize;

		for(size_t J = 0; J < TextureHeight / ZE_TC_BLOCK_HEIGHT; J++)
		{
			ReadCount = PartialResourceFile.Read((unsigned char*)Buffer + J * DestinationPitch, RowSizeToRead, 1);
			if(ReadCount)
			{
				ReadSize += RowSizeToRead;
				ReadCount = 0;
			}

		}
		
		TotalReadSize += ReadSize;


		TextureResource->Texture->Unlock(0);
	}

	//No Compression && No Mipmapping  Case
	else 
	{
		TextureResource->Texture->Lock(&Buffer, &DestinationPitch, 0);
		
		ReadSize = 0;
		RowSizeToRead = TextureWidth * TextureInfo.BitsPerPixel / 8;

		for(size_t J = 0; J < TextureHeight; J++)
		{
			// Read it from the raw texture, not from d3d pointer
			ReadCount = PartialResourceFile.Read((unsigned char*)Buffer + J * DestinationPitch, RowSizeToRead, 1);
			if(ReadCount)
			{
				ReadSize += RowSizeToRead;
				ReadCount = 0;
			}
		}
		
		TotalReadSize += ReadSize;


		TextureResource->Texture->Unlock(0);
	}



	zeLog("ZE Texture 2D", "Texture loaded from cache file: \"%s\".", ResourceFile->GetFileName());


	PartialResourceFile.Close();
	FileCache.CloseCache();

	return TextureResource;
}
