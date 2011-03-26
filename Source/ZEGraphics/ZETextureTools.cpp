//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureTools.cpp
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 YiÄŸit OrÃ§un GÃ–KBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "YiÄŸit  OrÃ§un  GÃ–KBULUT") and
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
  Name: YiÄŸit OrÃ§un GÃ–KBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#include <windows.h>
#include <ATI_Compress.h>
#include <freeimage.h>
#include <sys/stat.h>
#include <stdio.h>
#include <math.h>

#include "ZETexture2D.h"
#include "ZETexture2DResource.h"
#include "ZECore\ZEError.h"
#include "ZECore\ZEConsole.h"
#include "ZECore\ZEResourceManager.h"
#include "ZECore\ZEResourceFile.h"
#include "ZETextureTools.h"

/* FOrward declarations of free image library functions */
static unsigned DLL_CALLCONV	FreeImageFile_Read_2D(void *buffer, unsigned size, unsigned count, fi_handle handle);
static int DLL_CALLCONV			FreeImageFile_Seek_2D(fi_handle handle, long offset, int origin);
static long DLL_CALLCONV		FreeImageFile_Tell_2D(fi_handle handle);

bool ZETextureTools::CheckInFileCache(const char *FileName)
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


unsigned int ZETextureTools::GetMaxMipmapCount(unsigned int Width, unsigned int Height)
{
	unsigned int WidthCount = (unsigned int)(logf((float)Width) / logf(2.0f));
	unsigned int HeightCount = (unsigned int)(logf((float)Height) / logf(2.0f));

	/* also counts the original (level 0) texture as a mipmap */
	return WidthCount >= HeightCount ? HeightCount + 1 : WidthCount + 1;
}


bool ZETextureTools::CreateMipmaps(ZETexture2DResource*	TextureResource, unsigned char*	Image, unsigned int	Width, unsigned int	Height, unsigned int BPP, unsigned int Pitch, bool IsResizeable, const ZETextureOptions* MipmapOptions = NULL)
{
	/* load resource içinde yaratýlan mipmapler bu fonksyon çaðrýlarak yaratýlacak */

	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */
}

bool ZETextureTools::CompressTexture(void* DestinationAdress, unsigned int DestinationPitch, void* SourceData, unsigned int	SourcePitch, unsigned int SourceWidth, unsigned int SourceHeight, const ZETextureOptions* CompressionOptions = NULL)
{
	ATI_TC_FORMAT	Format;
	ATI_TC_Speed	Speed;
	int				CompressedPitch;	

	// Decide Compression Type
	switch(CompressionOptions->CompressionType)
	{
	case ZE_TCT_DXT1:
		Format = ATI_TC_FORMAT_DXT1;
		break;
	case ZE_TCT_DXT3:
		Format = ATI_TC_FORMAT_DXT3;
		break;
	case ZE_TCT_DXT5:
		Format = ATI_TC_FORMAT_DXT5;
		break;
	case ZE_TCT_3DC:
		Format = ATI_TC_FORMAT_BC5;
		break;
	default:
		break;
	};

	// Decide compression Speed
	switch(CompressionOptions->CompressionQuality)
	{
	case ZE_TCQ_HIGH:
		Speed = ATI_TC_Speed_Normal;
		break;
	default:
	case ZE_TCQ_NORMAL:
		Speed = ATI_TC_Speed_Fast;
		break;
	case ZE_TCQ_LOW:
		Speed = ATI_TC_Speed_SuperFast;
		break;
	};

	ATI_TC_Texture srcTexture;
	srcTexture.dwSize = sizeof(srcTexture);
	srcTexture.dwWidth = SourceWidth;
	srcTexture.dwHeight = SourceHeight;
	srcTexture.dwPitch = SourcePitch;
	srcTexture.format = ATI_TC_FORMAT_ARGB_8888;
	srcTexture.pData = (ATI_TC_BYTE*)SourceData;
	srcTexture.dwDataSize = ATI_TC_CalculateBufferSize(&srcTexture);

	ATI_TC_Texture destTexture;  
	destTexture.dwSize = sizeof(destTexture);
	destTexture.dwWidth = SourceWidth;
	destTexture.dwHeight = SourceHeight;
	destTexture.dwPitch = DestinationPitch;
	destTexture.format = Format;
	destTexture.pData = (ATI_TC_BYTE*)DestinationData;
	destTexture.dwDataSize = ATI_TC_CalculateBufferSize(&destTexture);

	ATI_TC_CompressOptions options;
	memset(&options, 0, sizeof(options));
	options.dwSize = sizeof(options);
	options.nCompressionSpeed = Speed;

	ATI_TC_ConvertTexture(&srcTexture, &destTexture, &options, NULL, NULL, NULL);
}

bool ZETextureTools::DownSample2x(void* DestinationAdress, unsigned int DestinationPitch, void* SourceData, unsigned int SourcePitch, unsigned int SourceWidth, unsigned int SourceHeight)
{
	struct ZEColorARGB
	{
		ZEBYTE Alpha;
		ZEBYTE Red;
		ZEBYTE Blue;
		ZEBYTE Green;
	};

	int DestinationHeight = SourceHeight / 2;
	int DestinationWidth = SourceWidth / 2;

	for (size_t y = 0; y < DestinationHeight; y++)
	{
		for (size_t x = 0; x < DestinationWidth; x++)
		{
			ZEColorARGB* Source = (ZEColorARGB*)((ZEBYTE*)SourceData + SourcePitch * y * 2 + x * 8);

			ZEWORD Red, Green, Blue, Alpha;
			Alpha = Source->Alpha;
			Red   = Source->Red;
			Green = Source->Green;
			Blue  = Source->Blue;
			Source++;

			Alpha += Source->Alpha;
			Red   += Source->Red;
			Green += Source->Green;
			Blue  += Source->Blue;

			Source = (ZEColorARGB*)((ZEBYTE*)Source + SourcePitch - 1 * 4);
			Alpha += Source->Alpha;
			Red   += Source->Red;
			Green += Source->Green;
			Blue  += Source->Blue;
			Source++;

			Alpha += Source->Alpha;
			Red   += Source->Red;
			Green += Source->Green;
			Blue  += Source->Blue;

			ZEColorARGB* Destination = (ZEColorARGB*)((ZEBYTE*)DestinationData + DestinationPitch * y + x * 4);
			Destination->Alpha = Alpha / 4;
			Destination->Red   = Red   / 4;
			Destination->Green = Green / 4;
			Destination->Blue  = Blue  / 4;
		}
	}
}

ZETexture2DResource*	ZETextureTools::LoadFromFileCache(const char *FileName)
{
	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */
}

ZETexture2DResource*	ZETextureTools::LoadFromOriginalFile(ZEResourceFile* ResourceFile, const ZETextureOptions* UserOptions = NULL)
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

	// Load Image
	FreeImageIO Callbacks;
	Callbacks.read_proc = &FreeImageFile_Read_2D;
	Callbacks.seek_proc = &FreeImageFile_Seek_2D;
	Callbacks.tell_proc = &FreeImageFile_Tell_2D;

	FREE_IMAGE_FORMAT TextureFormat = FreeImage_GetFileTypeFromHandle(&Callbacks, ResourceFile);
	if (TextureFormat == FIF_UNKNOWN) 
	{
		zeError("Texture Resource","Unsupported image format. FileName : \"%s\"", ResourceFile->GetFileName());
		return NULL;
	}

	FIBITMAP* Bitmap = FreeImage_LoadFromHandle(TextureFormat, &Callbacks, ResourceFile);
	unsigned int BPP = FreeImage_GetBPP(Bitmap) / 8;


	// Avoid Bitmap(Bitmap32) Creation and Conversion For the Images That Are Already 32 Bits Per Pixel
	if (BPP != 4)
	{
		FIBITMAP* Bitmap32 = FreeImage_ConvertTo32Bits(Bitmap); 
		FreeImage_Unload(Bitmap);
		Bitmap = Bitmap32;
		BPP = FreeImage_GetBPP(Bitmap) / 8;
	}

	// Get Image Properties
	unsigned int	Width;
	unsigned int	Height;
	unsigned int	Pitch;
	Width = FreeImage_GetWidth(Bitmap);
	Height = FreeImage_GetHeight(Bitmap);
	Pitch = FreeImage_GetPitch(Bitmap);

	unsigned char* Image;
	Image = (unsigned char*)malloc(Height * Pitch);
	if (Image == NULL)
	{
		zeError("Texture Resource","Cannot Allocate Memmory for the Texture. FileName : \"%s\"", ResourceFile->GetFileName());
		return NULL;
	}
	FreeImage_ConvertToRawBits((BYTE*)Image, Bitmap, Pitch, BPP * 8, 0x00FF0000, 0x0000FF00, 0x000000FF, true);
	FreeImage_Unload(Bitmap);

	bool	IsResizeable = true;
	bool	IsCompressible = true;
	// Creating texture options constrains
	// Check if it is Resizeable (power of 2)

	if (((Width & (Width - 1)) != 0) || ((Height & (Height - 1)) != 0))
	{
		IsResizeable = false;
		Options.DownSample = ZE_TDS_NONE;
		Options.MipMapping = ZE_TMM_DISABLED;
		Options.MaximumMipmapLevel = 0;
	}

	// Check if it is Compressible (divisible by 4)
	if ((Width % 4 != 0) || (Height % 4 != 0))
	{
		IsCompressible = false;
		Options.CompressionType = ZE_TCT_NONE;
	}

	// MipMapping
	unsigned int	PossibleMaxMipmapNumber;
	unsigned int	MipMapCount = GetMaxMipmapCount(Width, Height);
	switch (Options.DownSample)
	{
		// 16x Fazla?
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

	// Resize According to Loading Options
	switch (Options.DownSample)
	{
	case ZE_TDS_8X:
		zeLog("Texture2D Resource", "RESIZING texture by 8X:  \"%s\".", ResourceFile->GetFileName());
		DownSample2x(Image, Pitch, Image, Pitch, Width, Height);
		Width /= 2;
		Height /= 2;
		DownSample2x(Image, Pitch, Image, Pitch, Width, Height);
		Width /= 2;
		Height /= 2;
		DownSample2x(Image, Pitch, Image, Pitch, Width, Height);
		Width /= 2;
		Height /= 2;
		break;
	case ZE_TDS_4X:
		zeLog("Texture2D Resource", "RESIZING texture by 4X:  \"%s\".", ResourceFile->GetFileName());
		DownSample2x(Image, Pitch, Image, Pitch, Width, Height);
		Width /= 2;
		Height /= 2;
		DownSample2x(Image, Pitch, Image, Pitch, Width, Height);
		Width /= 2;
		Height /= 2;
		break;

	case ZE_TDS_2X:
		zeLog("Texture2D Resource", "RESIZING texture by 2X:  \"%s\".", ResourceFile->GetFileName());
		DownSample2x(Image, Pitch , Image, Pitch, Width, Height);
		Width /= 2;
		Height /= 2;
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
	if (!Texture->Create(Width, Height, PixelFormat, false, Options.MaximumMipmapLevel))
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
				CompressTexture(Buffer, DestinationPitch, Image, Width, Height, Pitch, &Options);
				TextureResource->Texture->Unlock();
				DownSample2x(Image, Pitch , Image, Pitch, Width, Height);
				Width /= 2;
				Height /= 2;
			}

			// 2x2 Mipmap
			TextureResource->Texture->Lock(&Buffer, &DestinationPitch, Levels - 2);
			for (size_t I = 0; I < Height; I++)
				memcpy((unsigned char*)Buffer + (I * DestinationPitch), Image + (I * Pitch), Width * BPP);
			TextureResource->Texture->Unlock();

			// 1x1 Mipmap
			TextureResource->Texture->Lock(&Buffer, &DestinationPitch, Levels - 1);
			DownSample2x(Buffer, DestinationPitch , Image, Pitch, Width, Height);
			TextureResource->Texture->Unlock();
			break;

		case 2://No Compression && Mipmapping
			for (size_t I = 0; I < Levels; I++)
			{
				// Write Mipmap to Device Level I
				TextureResource->Texture->Lock(&Buffer, &DestinationPitch, I);
				for (size_t K = 0; K < Height; K++)
					memcpy((unsigned char*)Buffer + (K * DestinationPitch), Image + (K * Pitch), Width * BPP);
				TextureResource->Texture->Unlock();
				DownSample2x(Image, Pitch , Image, Pitch, Width, Height);
				Width /= 2;
				Height /= 2;
			}
			break;

		case 3://Compression && No Mipmapping case
			TextureResource->Texture->Lock(&Buffer, &DestinationPitch, 0);
			CompressTexture(Buffer, DestinationPitch, Image, Width, Height, Pitch, &Options);
			TextureResource->Texture->Unlock();
			break;

		case 4://No Compression && No Mipmapping
			TextureResource->Texture->Lock(&Buffer, &DestinationPitch, 0);
			for (size_t I = 0; I < Height; I++)
				memcpy((unsigned char*)Buffer + (I * DestinationPitch), Image + I * (Width * BPP), Width * BPP);
			TextureResource->Texture->Unlock();
			break;
		default:
			break;
	}


	//if(Options.FileCaching != ZE_TFC_DISABLED)
		//SaveToFileCache();

	free(Image);
	return TextureResource;
}

bool ZETextureTools::SaveToFileCache(const char* DestinationFile, const ZEBYTE* SourceData, unsigned int SourceWidth, unsigned int SourceHeight, unsigned int SourceBPP, unsigned int SourcePitch)
{
	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */
	/* YAZILACAK */
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
