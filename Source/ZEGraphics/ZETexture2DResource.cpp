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

#include <math.h>
#include "ZETexture2DResource.h"
#include "ZECore\ZEError.h"
#include "ZECore\ZEConsole.h"
#include "ZECore\ZEResourceManager.h"
#include "ZECore\ZEResourceFile.h"
#include "ZETexture2D.h"
#include <windows.h>
#include <ATI_Compress.h>
#include <freeimage.h>
#include <sys/stat.h> 
#include <stdio.h>


static unsigned DLL_CALLCONV	FreeImageFile_Read_2D(void *buffer, unsigned size, unsigned count, fi_handle handle);
static int DLL_CALLCONV			FreeImageFile_Seek_2D(fi_handle handle, long offset, int origin);
static long DLL_CALLCONV		FreeImageFile_Tell_2D(fi_handle handle); 

static bool						CheckInFileCache(const char *FileName);
static void						SaveToFileCache(const ZEBYTE* Image, int Width, int Height, int BPP, int Pitch, const char* FileName);

static bool						Rescale(const unsigned char* DestinationData, const unsigned char* SourceData, unsigned int &SourceWidth, unsigned int &SourceHeight, unsigned int &SourcePitch, unsigned int &SourceBPP, unsigned int RescaleRatio);
//static void						DownSample2x(void* DestinationData, unsigned int DestinationPitch, void* SourceData, unsigned int &SourceWidth, unsigned int &SourceHeight, unsigned int &SourcePitch);
static unsigned int				GetMipmapCount(unsigned int Width, unsigned int Height);


static void DownSample2x(void* DestinationData, unsigned int DestinationPitch, void* SourceData, unsigned int SourcePitch, unsigned int SourceWidth, unsigned int SourceHeight)
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
			ZEColorARGB* Source = (ZEColorARGB*)((ZEBYTE*)SourceData + SourcePitch * y + x * 4);

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
			Source = (ZEColorARGB*)((ZEBYTE*)Source + SourcePitch - 2 * 4);

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

static void Compress(void* DestinationData, unsigned int DestinationPitch, void* SourceData, unsigned int SourcePitch, unsigned int SourceWidth, unsigned int SourceHeight, const ZETextureOptions* Options)
{	
	ATI_TC_FORMAT	Format;
	ATI_TC_Speed	Speed;
	int				CompressedPitch;	

	// Decide Compression Type
	switch(Options->CompressionType)
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
	switch(Options->CompressionQuality)
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

// Ok
static bool CheckInFileCache(const char *FileName)
{	
	char CompletePath[ZE_MAX_FILE_NAME_SIZE];
	sprintf(CompletePath, "resources\\%s.cache", FileName);

	int Stat;
	struct stat FileInfo;
	
	Stat = stat(CompletePath, &FileInfo);

	if (Stat == 0)
		return TRUE;

	return FALSE;
}


// Değisecek
static void SaveToFileCache(const ZEBYTE* Image, int Width, int Height, int BPP, int Pitch, const char* FileName)
{
	
}


// Değisecek
static ZETexture2DResource* LoadFromFileCache(const char *FileName)
{
	return NULL;
}


static ZETexture2DResource* LoadFromOriginalFile(ZEResourceFile* ResourceFile, const ZETextureOptions* UserOptions)
{
	zeLog("Texture2D Resource", "LOADING texture from ORIGINAL FILE  \"%s\".", ResourceFile->GetFileName());

	bool					IsResizeable = true;
	bool					IsCompressible = true;
	unsigned int			Width;
	unsigned int			Height;
	unsigned int			BPP;
	unsigned int			Pitch;
	unsigned int			PossibleMaxMipmapNumber = 0;
	unsigned int			TextureQuality = zeGraphics->GetTextureQuality();
	unsigned char*			Image;
	ZETexturePixelFormat	PixelFormat;

	ZETextureOptions*	EngineOptions = zeGraphics->GetTextureOptions();
	
	if (UserOptions == NULL)
		UserOptions = zeGraphics->GetTextureOptions();

	// Combining engine options with user options. 
	// User options that are other than auto overrides engine options.
	ZETextureOptions		Options;
	Options.CompressionType		= UserOptions->CompressionType == ZE_TCT_AUTO ? EngineOptions->CompressionType : UserOptions->CompressionType;
	Options.CompressionQuality	= UserOptions->CompressionQuality == ZE_TCQ_AUTO ? EngineOptions->CompressionQuality : UserOptions->CompressionQuality;
	Options.DownSample			= UserOptions->DownSample == ZE_TCT_AUTO ? EngineOptions->DownSample : Options.DownSample = UserOptions->DownSample;
	Options.MipMapping			= UserOptions->MipMapping == ZE_TMM_AUTO ? EngineOptions->MipMapping : UserOptions->MipMapping;
	Options.FileCaching			= UserOptions->FileCaching == ZE_TFC_AUTO ? EngineOptions->FileCaching : UserOptions->FileCaching;

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

	Width = FreeImage_GetWidth(Bitmap);
	Height = FreeImage_GetHeight(Bitmap);
	BPP = FreeImage_GetBPP(Bitmap) / 8;
	Pitch = FreeImage_GetPitch(Bitmap);

	// Creating texture options constrains
	// Check if it is Resizeable (power of 2)
	if (Width & 0x00000001 != 0 || Height & 0x00000001 != 0)
		IsResizeable = false;

	// Check if it is Compressible (divisible by 4)
	if ((Width % 4 != 0) || (Height % 4 != 0))
		IsCompressible = false;

	// Applying Constraints
	// Compression
	if (!IsCompressible)
		Options.CompressionType = ZE_TCT_NONE;

	// Down Sampling
	if (!IsResizeable)
	{
		Options.DownSample = ZE_TDS_NONE;
		Options.MipMapping = ZE_TMM_DISABLED;
	}

	// MipMapping
	int MipMapCount = GetMipmapCount(Width, Height);
	switch (Options.DownSample)
	{
		case ZE_TDS_16X:
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
	}

	// If Users MipMapping Level is Valid Use It
	Options.MaximumMipmapLevel = UserOptions->MaximumMipmapLevel > PossibleMaxMipmapNumber ? PossibleMaxMipmapNumber : UserOptions->MaximumMipmapLevel;

	//Decide Pixel Format
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

////
	Image = (unsigned char*)malloc(Height * Pitch);
	if (Image == NULL)
	{
		zeError("Texture Resource","Cannot Allocate Memmory for the Texture. FileName : \"%s\"", ResourceFile->GetFileName());
		return NULL;
	}
	FreeImage_ConvertToRawBits((BYTE*)Image, Bitmap, Pitch, BPP * 8, 0x00FF0000, 0x0000FF00, 0x000000FF, true);

	// Resize According to Loading Options
	switch (Options.DownSample)
	{
		case ZE_TDS_16X:
			zeLog("Texture2D Resource", "RESIZING texture by 4X:  \"%s\".", ResourceFile->GetFileName());
			//DownSample16x(Image, Image, Width, Height, Pitch, BPP, 2);
			break;
		case ZE_TDS_4X:
			zeLog("Texture2D Resource", "RESIZING texture by 4X:  \"%s\".", ResourceFile->GetFileName());
			//DownSample4x(Image, Image, Width, Height, Pitch, BPP, 2);
			break;

		case ZE_TDS_2X:
			zeLog("Texture2D Resource", "RESIZING texture by 2X:  \"%s\".", ResourceFile->GetFileName());
			DownSample2x(Image, Image, Width, Height, Pitch, BPP, 2);
			break;
	}
///

	//if(EngineLoadingOptions->CacheFile)
		//SaveToFileCache();
	
	return TextureResource;
}

/*
static void WriteToDevice(ZETexture2DResource* TextureResource, const unsigned char* SourceData, unsigned int Width, unsigned int Height, unsigned int BPP, unsigned int Level, ZETextureCompressionType CompressionType)
{
	void* Buffer = NULL;
	unsigned int DestinationPitch;

	unsigned int BytesPerBlock;
	unsigned int BlocksInWidth; 
	unsigned int BlocksInHeight; 
	unsigned int CopySize;
	
	TextureResource->Texture->Lock(&Buffer, &DestinationPitch, Level);
	
	switch(CompressionType)
	{
		case ZE_TCT_3DC:
		case ZE_TCT_DXT1:
		case ZE_TCT_DXT3:
		case ZE_TCT_DXT5:
			
			if (CompressionType == ZE_TCT_DXT1)
				BytesPerBlock = 8;
			else
				BytesPerBlock = 16;

			BlocksInWidth = Width / 4;
			BlocksInHeight = Height / 4;
			CopySize = BytesPerBlock * BlocksInWidth;

			for (int I = 0; I < BlocksInHeight; I++)
				// Bunda Texture Kayması var
				memcpy((unsigned char*)Buffer + (I * DestinationPitch), SourceData + I * CopySize, CopySize);
				// Texturelar Ters çıkıyor.
				// ORJ: memcpy((unsigned char*)Buffer + (I * DestinationPitch), SourceData + (BlocksInHeight - I - 1) * CopySize, CopySize);
				
			
			break;
		
		case ZE_TCT_NONE:
			
			BlocksInWidth = 0;
			for (int I = 0; I < Height; I++)
				// Bunda Texture Kayması Var
				memcpy((unsigned char*)Buffer + (I * DestinationPitch), SourceData + I * (Width * BPP), Width * BPP);
				//memcpy((unsigned char*)Buffer + (I * DestinationPitch), SourceData + (Height - I - 1) * (Width * BPP), Width * BPP);
			
			break;
	}
	
	TextureResource->Texture->Unlock();
}


// Downsample2x() i kullan
static bool Rescale(const unsigned char* DestinationData, const unsigned char* SourceData, unsigned int &SourceWidth, unsigned int &SourceHeight, unsigned int &SourcePitch, unsigned int &SourceBPP, unsigned int RescaleRatio)
{
	int DestinationHeight = SourceHeight / RescaleRatio;
	int DestinationWidth = SourceWidth / RescaleRatio;
	int DestinationPitch = DestinationWidth * 4;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Performance Problem ! Unnecessary Memory Allocation.
	FIBITMAP* Bitmap = FreeImage_ConvertFromRawBits((BYTE*)SourceData, SourceWidth, SourceHeight, SourcePitch, 32, 0x00FF0000, 0x0000FF00, 0x000000FF);
	FIBITMAP* ResizedBitmap = FreeImage_Rescale(Bitmap, DestinationWidth, DestinationHeight, FILTER_BOX);

	SourceHeight =  SourceHeight / RescaleRatio;
	SourceWidth = SourceWidth / RescaleRatio;
	SourcePitch = SourcePitch / RescaleRatio;

	FreeImage_ConvertToRawBits((BYTE*)DestinationData, ResizedBitmap, DestinationPitch, 32, 0x00FF0000, 0x0000FF00, 0x000000FF);

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Performance Problem ! Unnecessary Memory Allocation.
	FreeImage_Unload(ResizedBitmap);
	FreeImage_Unload(Bitmap);

	return TRUE;
}

static void CreateMipmaps(ZETexture2DResource* TextureResource, unsigned char* Image, unsigned int Width, unsigned int Height, unsigned int BPP, unsigned int Pitch, bool IsResizeable, const ZETextureOptions *LoadingOptions)
{
	zeLog("Texture2D Resource", "Creeating MipMaps for the texture:  \"%s\".", TextureResource->GetFileName());
	
	unsigned int MipmapLevels = LoadingOptions->MaximumMipmapLevel;
	ZETextureCompressionType CompType = LoadingOptions->CompressionType;
	unsigned char* Temp;
	
	// If Resizeable(Mipmapping is On) Then Create Mipmaps 
	if (LoadingOptions->MipMapping)
	{
		// Storage For the Copy of Image
		Temp = new unsigned char[Height * Pitch];

		// Copy The Image
		for (int I = 0; I < Height; I++)
			memcpy( Temp + (I * Pitch), Image + (I * Pitch), Width * BPP);
		
		
		// Compressible
		if ((LoadingOptions->CompressionType != ZE_TCT_NONE) && (MipmapLevels > 2))
		{
			// Create Mipmaps for level-2 (2x2 and 1x1 not compressible)
			for (int I = 0; I < MipmapLevels - 2; I++)
			{
				Compress(TextureResource,Temp, Width, Height, Pitch,BPP, I, LoadingOptions);
				Rescale(Temp, Temp, Width, Height, Pitch, BPP, 2);
			}
			
			// 2x2
			WriteToDevice(TextureResource, Temp, Width, Height, BPP, MipmapLevels - 2, CompType);
			// 1x1
			Rescale(Temp, Temp, Width, Height, Pitch, BPP, 2);
			WriteToDevice(TextureResource, Temp, Width, Height, BPP, MipmapLevels - 1, CompType);
		}
		// Not Compressible
		else
		{
			// create Mipmaps without Compressing
			for (int I = 0; I < MipmapLevels; I++)
			{
				// Write Mipmap to Device Level I
				WriteToDevice(TextureResource, Temp, Width, Height, BPP, I, CompType);
				Rescale(Temp, Temp, Width, Height, Pitch, BPP, 2);
			}
		}

		delete [] Temp;
	}
	else
	{
		// If Compressible
		if(LoadingOptions->CompressionType != ZE_TCT_NONE)
			Compress(TextureResource, Image, Width, Height, Pitch, BPP, 0, LoadingOptions);
		else
			// Write Mipmap to device level 0
			WriteToDevice(TextureResource, Image, Width, Height, BPP, 0, CompType);
	}
}*/

// DEGISTIR
static unsigned int GetMipmapCount(unsigned int Width, unsigned int Height)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// You can use own log base 2 with switch statements. Texture size only varies in power of 2
	
	unsigned int WidthCount = (unsigned int)(logf((float)Width) / logf(2.0f));
	unsigned int HeightCount = (unsigned int)(logf((float)Height) / logf(2.0f));

	return WidthCount >= HeightCount ? HeightCount + 1 : WidthCount + 1;
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

ZETexture2DResource* ZETexture2DResource::LoadResource(ZEResourceFile* ResourceFile, bool EmbededResource, const ZETextureOptions *UserOptions)
{
	if ((zeGraphics->GetTextureQuality() < 5) && (!EmbededResource))
	{
		if (CheckInFileCache(ResourceFile->GetFileName()))
		{
			return LoadFromFileCache(ResourceFile->GetFileName());
		}
		else
		{			
			return LoadFromOriginalFile(ResourceFile, UserOptions);
		}
	}
	else // If texture quality is 5 or if it is EmbededResource Load from original file
	{
		// Load Directly(no compressing, no resizing) but mipmapping
		//ZETextureLoadOptions AutoTextureQuality = {UserOptions->MinimumQuality, UserOptions->MaximumQuality, false, {ZE_TCT_NONE, ZE_TCQ_NORMAL, ZE_TDS_NONE, true, true, 0}};
		
		return LoadFromOriginalFile(ResourceFile, UserOptions);
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
