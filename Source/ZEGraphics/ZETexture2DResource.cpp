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
static void						DownSample2x(void* DestinationData, unsigned int DestinationPitch, void* SourceData, unsigned int SourceWidth, unsigned int SourceHeight, unsigned int SourcePitch);
static unsigned int				GetMipmapCount(unsigned int Width, unsigned int Height);



static void Compress(ZETexture2DResource* TextureResource, ZEBYTE* Image, int Width, int Height, int Pitch, ZETextureCompressionType CType, ZETextureCompressionQuality CSpeed, unsigned int Level)
{	
	ATI_TC_FORMAT	Format;
	ATI_TC_Speed	Speed;
	int				CompressedPitch;	

	// Decide Compression Type
	switch(CType)
	{
		case ZE_TCT_DXT1:
			Format = ATI_TC_FORMAT_DXT1;
			CompressedPitch = Width / 2;
			break;
		case ZE_TCT_AUTO:
		case ZE_TCT_DXT3:
			Format = ATI_TC_FORMAT_DXT3;
			CompressedPitch = Width;
			break;
		case ZE_TCT_DXT5:
			Format = ATI_TC_FORMAT_DXT5;
			CompressedPitch = Width;
			break;
		case ZE_TCT_3DC:
			Format = ATI_TC_FORMAT_BC5;
			CompressedPitch = Width;
			break;
		default:
			break;
	};

	// Decide compression Speed
	switch(CSpeed)
	{
		case ZE_TCQ_HIGH:
			Speed = ATI_TC_Speed_Normal;
			break;
		case ZE_TCQ_NORMAL:
			Speed = ATI_TC_Speed_Fast;
			break;
		case ZE_TCQ_LOW:
			Speed = ATI_TC_Speed_SuperFast;
			break;
		default:
			break;
	};
	
	ZEBYTE* CompressedData = new ZEBYTE[Height * CompressedPitch];

	ATI_TC_Texture srcTexture;
	srcTexture.dwSize = sizeof(srcTexture);
	srcTexture.dwWidth = Width;
	srcTexture.dwHeight = Height;
	srcTexture.dwPitch = Pitch;
	srcTexture.format = ATI_TC_FORMAT_ARGB_8888;
	srcTexture.pData = Image;
	srcTexture.dwDataSize = ATI_TC_CalculateBufferSize(&srcTexture);

	ATI_TC_Texture destTexture;  
	destTexture.dwSize = sizeof(destTexture);
	destTexture.dwWidth = Width;
	destTexture.dwHeight = Height;
	destTexture.dwPitch = CompressedPitch;
	destTexture.format = Format;
	destTexture.pData = CompressedData;
	destTexture.dwDataSize = ATI_TC_CalculateBufferSize(&destTexture);

	ATI_TC_CompressOptions options;
	memset(&options, 0, sizeof(options));
	options.dwSize = sizeof(options);
	options.nCompressionSpeed = Speed;

	ATI_TC_ConvertTexture(&srcTexture, &destTexture, &options, NULL, NULL, NULL);

	unsigned int BitsPerBlock;
	if (CType == ZE_TCT_DXT1)
		BitsPerBlock = 64;
	else
		BitsPerBlock = 128;

	unsigned int WidthBlockNumber = Width / 4;
	unsigned int HeightBlockNumber = Height / 4;
	unsigned int CopySize = BitsPerBlock * WidthBlockNumber;
	
	void* Buffer = NULL;
	unsigned int DestinationPitch;

	TextureResource->Texture->Lock(&Buffer, &DestinationPitch, Level);
	for (int I = 0; I < HeightBlockNumber; I++)
		memcpy((unsigned char*)Buffer + (I * DestinationPitch), CompressedData + (Height - I - 1) * CopySize, CopySize);

	TextureResource->Texture->Unlock();
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
	//char CacheFileName[ZE_MAX_FILE_NAME_SIZE];
	//sprintf(CacheFileName, "%s.cache", FileName);


	//zeLog("Texture2D Resource", "SAVING texture to FILE CACHE \"%s\".", CacheFileName);

	//FILE* FileToWrite = NULL;
	//FileToWrite = fopen(CacheFileName, "wb+");

	////write width(4byte), height(4byte), BPP(4byte), Pitch(4Byte) and image data
	//fwrite(&Width, 4, 1, FileToWrite);
	//fwrite(&Height, 4, 1, FileToWrite);
	//fwrite(&BPP, 4, 1, FileToWrite);
	//fwrite(&Pitch, 4, 1, FileToWrite);
	//fwrite(Image, 1, Width*Height*4, FileToWrite);

	//fclose(FileToWrite);
}


// Değişecek
static ZETexture2DResource* LoadFromFileCache(const char *FileName)
{
	zeLog("Texture2D Resource", "LOADING texture from FILE CACHE \"%s\".", FileName);

	char ResourceFilePath [ZE_MAX_FILE_NAME_SIZE]; 
	sprintf(ResourceFilePath, "resources\\%s.cache", FileName);

	int Width;
	int Height;
	int BPP;
	int Pitch;

	FILE* FileToRead;
	FileToRead = fopen (ResourceFilePath, "rb+");

	//first 4 byte is Width, 2nd 4 byte is Height and the rest is image data
	fread(&Width, 4, 1, FileToRead);
	fread(&Height, 4, 1, FileToRead);
	fread(&BPP, 4, 1, FileToRead);
	fread(&Pitch, 4, 1, FileToRead);

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Performance Problem ! Unnecessary Memory Allocation.
	ZEBYTE* Image = new ZEBYTE [Height * Pitch];
	fread (Image, 1, Height * Pitch, FileToRead);

	fclose(FileToRead);

	ZETexture2DResource* TextureResource = new ZETexture2DResource();
	ZETexture2D* Texture  =  TextureResource->Texture = ZETexture2D::CreateInstance();

	TextureResource->SetFileName(FileName);
	TextureResource->Cached = false;
	TextureResource->Shared = false;

	if (!Texture->Create(Width, Height, ZE_TPF_RGBA_INT32))
	{
		zeError("Texture Resource", "Can not create texture resource. FileName : \"%s\"", FileName);
		delete TextureResource;
		Texture->Destroy();
		return NULL;
	}

	WriteToDevice(TextureResource, Image, Width, Height, BPP);

	zeLog("Texture2D Resource", "Texture file \"%s\" has been loaded from FILE CACHE.", FileName);

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Performance Problem ! Unnecessary Memory Allocation.
	delete [] Image;
	

	return TextureResource;
}


static ZETexture2DResource* LoadFromOriginalFile(ZEResourceFile* ResourceFile, ZETextureLoadOptions *UserOptions)
{
	char FileName[ZE_MAX_FILE_NAME_SIZE];
	sprintf(FileName, "%s", ResourceFile->GetFileName());
	zeLog("Texture2D Resource", "LOADING texture from ORIGINAL FILE  \"%s\".", FileName);

	bool					IsResizeable = true;
	bool					IsCompressible = true;
	unsigned int			Width;
	unsigned int			Height;
	unsigned int			BPP;
	unsigned int			Pitch;
	unsigned int			MaxMipmapNumber = 0;
	unsigned int			TextureQuality = zeGraphics->GetTextureQuality();
	unsigned char			*Image;
	ZETexturePixelFormat	PixelFormat;
	
	
	ZEResourceFile File;
	if (File.Open(FileName))
	{
		FreeImageIO Callbacks;
		Callbacks.read_proc = &FreeImageFile_Read_2D;
		Callbacks.seek_proc = &FreeImageFile_Seek_2D;
		Callbacks.tell_proc = &FreeImageFile_Tell_2D;

		FREE_IMAGE_FORMAT TextureFormat = FreeImage_GetFileTypeFromHandle(&Callbacks, ResourceFile);
		if (TextureFormat == FIF_UNKNOWN) 
		{
			zeError("Texture Resource","Unsupported image format. FileName : \"%s\"", FileName);
			return NULL;
		}

		FIBITMAP* Bitmap = FreeImage_LoadFromHandle(TextureFormat, &Callbacks, ResourceFile);

		FIBITMAP* ConvertedData = FreeImage_ConvertTo32Bits(Bitmap);

		Width = FreeImage_GetWidth(ConvertedData);
		Height = FreeImage_GetHeight(ConvertedData);
		BPP = FreeImage_GetBPP(ConvertedData) / 8;
		Pitch = FreeImage_GetPitch(ConvertedData);

		Image = (unsigned char*)malloc(Height * Pitch);
		if (Image == NULL)
		{
			zeError("Texture Resource","Cannot Allocate Memmory for the Texture. FileName : \"%s\"", FileName);
			return NULL;
		}
		FreeImage_ConvertToRawBits((BYTE*)Image, ConvertedData, Pitch, BPP * 8, 0x00FF0000, 0x0000FF00, 0x000000FF);

		FreeImage_Unload(Bitmap);
		FreeImage_Unload(ConvertedData);

		File.Close();
	}

	// SetTextureQuality
	if (TextureQuality < UserOptions->MinimumQuality)
		TextureQuality = UserOptions->MinimumQuality;
	if (TextureQuality > UserOptions-> MaximumQuality)
		TextureQuality = UserOptions-> MaximumQuality;
	
	// Check if it is Resizeable (power of 2)
	if ( ((Width & (Width - 1)) != 0) || ((Height & (Height - 1)) != 0) || UserOptions->DownSample == ZE_TDS_NONE )
	{
		IsResizeable = false;
		UserOptions->DownSample = ZE_TDS_NONE;
	}

	// Check if it is Compressible (divisible by 4)
	if (((Width % 4) != 0) || ((Height % 4) != 0) || UserOptions->CompressionType == ZE_TCT_NONE)
	{
		IsCompressible = false;
		UserOptions->CompressionType = ZE_TCT_NONE;
	}


	// Get max possible Mipmap levels
	if(IsResizeable && UserOptions->MipMapping)
	{
		if (TextureQuality == 2 || TextureQuality == 1)// with 4x down sample MaxMipmapNumber have to be 2 less than its value
			MaxMipmapNumber = GetMipmapCount(Width, Height) - 2;
		else if (TextureQuality == 3)// with 4x down sample MaxMipmapNumber have to be 1 less than its value
			MaxMipmapNumber = GetMipmapCount(Width, Height) - 1;
		else
			MaxMipmapNumber = GetMipmapCount(Width, Height);
	}

	// Set Maximum MipmapLevel
	if (UserOptions->MaximumMipmapLevel > MaxMipmapNumber)
	{
		UserOptions->MaximumMipmapLevel = MaxMipmapNumber;
	}

	// Create TextureResource 
	ZETexture2DResource* TextureResource = new ZETexture2DResource();
	ZETexture2D* Texture = TextureResource->Texture = ZETexture2D::CreateInstance();

	// Set Other Variables
	TextureResource->SetFileName(ResourceFile->GetFileName());
	TextureResource->Cached = false;
	TextureResource->Shared = false;

	//Decide Pixel Format
	switch(UserOptions->CompressionType)
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
	
	// Create the Texture
	if (!Texture->Create(Width, Height, PixelFormat, false, UserOptions->MaximumMipmapLevel))
	{
		zeError("Texture Resource", "Can not create texture resource. FileName : \"%s\"", ResourceFile->GetFileName());
		delete TextureResource;
		return NULL;
	}


	// Resize or Compress according to Texture Quality
	switch(TextureQuality)
	{
		case 1:
		case 2:
			// 4x4 Resize
			if (IsResizeable)
			{
				zeLog("Texture2D Resource", "RESIZING texture file:  \"%s\".", FileName);
				Rescale(Image, Image, Width, Height, Pitch, BPP, 2);
				Rescale(Image, Image, Width, Height, Pitch, BPP, 2);
			}
			CreateMipmaps(TextureResource, Image, Width, Height, BPP, Pitch, MaxMipmapNumber, UserOptions);
			break;

		case 3:
			// 2x2 Resize
			if (IsResizeable)
			{
				zeLog("Texture2D Resource", "RESIZING texture file:  \"%s\".", FileName);
				Rescale(Image, Image, Width, Height, Pitch, BPP, 2);
			}
			CreateMipmaps(TextureResource, Image, Width, Height, BPP, Pitch, MaxMipmapNumber, UserOptions);			
			break;

		case 4:
			// No Resize
			CreateMipmaps(TextureResource, Image, Width, Height, BPP, Pitch, MaxMipmapNumber, UserOptions);
			break;

		case 5:
			// No resize no compress
			CreateMipmaps(TextureResource, Image, Width, Height, BPP, Pitch, MaxMipmapNumber, UserOptions);
	}

	//	If not EmbededResource Save to file cache for future use
	/*if(!EmbededResource)
	SaveToFileCache(CompressedImage, Width, Height, BPP, Pitch, AbsolutePath);*/

	return TextureResource;
}


static void WriteToDevice(ZETexture2DResource* TextureResource, const unsigned char* SourceData, int Width, int Height, unsigned int Level)
{
	void* Buffer = NULL;
	unsigned int DestinationPitch;

	TextureResource->Texture->Lock(&Buffer, &DestinationPitch, Level);
	for (int I = 0; I < Height; I++)
		memcpy((unsigned char*)Buffer + (I * DestinationPitch), SourceData + (Height - I - 1) * DestinationPitch, DestinationPitch);

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

struct ZEColorARGB
{
	ZEBYTE Alpha;
	ZEBYTE Red;
	ZEBYTE Blue;
	ZEBYTE Green;
};


static void DownSample2x(void* DestinationData, unsigned int DestinationPitch, void* SourceData, unsigned int SourceWidth, unsigned int SourceHeight, unsigned int SourcePitch)
{
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


static void CreateMipmaps(ZETexture2DResource* TextureResource, const unsigned char* Image, unsigned int Width, unsigned int Height, unsigned int BPP, unsigned int Pitch, unsigned int Level, ZETextureLoadOptions *UserOptions)
{
	zeLog("Texture2D Resource", "Creeating MipMaps for the texture:  \"%s\".", TextureResource->GetFileName());

	// If Resizeable Then Create Mipmaps 
	if (UserOptions->DownSample != ZE_TDS_NONE)
	{
		// Storage For the Copy of Image
		unsigned char* Temp = new unsigned char[Height * Pitch];

		// Copy The Image
		for (int I = 0; I < Height; I++)
			memcpy( Temp + (I * Pitch), Image + (I * Pitch), Pitch);


		if (UserOptions->CompressionType != ZE_TCT_NONE)
		{
			
			// Create Mipmaps for level-2 (2x2 and 1x1 not compressible)
			for (int I = 0; I < Level-2; I++)
			{
				Compress(TextureResource,Temp, Width, Height, Pitch, UserOptions->CompressionType, UserOptions->CompressionQuality, I);

				
				Rescale(Temp, Temp, Width, Height, Pitch, BPP, 2);

				
			}
			// 2x2
			//WriteToDevice(TextureResource, Temp, Width, Height, Level - 2);
			// 1x1
			//Rescale(Temp, Temp, Width, Height, Pitch, BPP, 2);
			//WriteToDevice(TextureResource, CompressedImage, Width, Height, Level - 1);


		}
		else
		{
			// create Mipmaps without Compressing
			for (int I = 0; I < Level; I++)
			{
				// Write Mipmap to device level I
				WriteToDevice(TextureResource, Temp, Width, Height, I);

				Rescale(Temp, Temp, Width, Height, Pitch, BPP, 2);
			}
		}

		delete [] Temp;
	}
	else
	{
		// Write Mipmap to device level 0
		WriteToDevice(TextureResource, Image, Width, Height, 0);
	}

}

// DEGISTIR
static unsigned int GetMipmapCount(unsigned int Width, unsigned int Height)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Type Conversion Problem. User logf instead of log. Conversion must be double to int not int to double.
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

ZETexture2DResource* ZETexture2DResource::LoadResource(const char* FileName, ZETextureLoadOptions *UserOptions)
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

ZETexture2DResource* ZETexture2DResource::LoadResource(ZEResourceFile* ResourceFile, bool EmbededResource, ZETextureLoadOptions *UserOptions)
{	
	unsigned int	TextureQuality = zeGraphics->GetTextureQuality();

	if (TextureQuality < 5 && !EmbededResource)
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
	// If texture quality is 5 or if it is EmbededResource Load from original file
	else
	{
		
		return LoadFromOriginalFile(ResourceFile, UserOptions);
	}
}
	
ZETexture2DResource* ZETexture2DResource::LoadSharedResource(const char* FileName, ZETextureLoadOptions *UserOptions)
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
	else
		return NewResource;
}

// NOT NEEDED ANYMORE

//void ZETexture2DResource::CopyFromChunk(ZEBYTE* Output, const ZEBYTE* Input, unsigned int ChunkX, unsigned int ChunkY, unsigned int Pitch)
//{
//	unsigned int RowIndex = ChunkY * 4 * Pitch;
//	unsigned int Index = RowIndex + ChunkX * 4;
//
//	const ZEBYTE* Temp = Input + Index;
//
//	for (size_t Y = 0; Y < 4; Y++)
//		for (size_t X = 0; X < 4; X++)
//			*(int*)(Output + Y * 16 + X * 4) = *(int*)(Temp + Y * Pitch + X * 4);
//}
//
//void ZETexture2DResource::CopyToChunk(ZEBYTE* Output, const ZEBYTE* Input, unsigned int ChunkX, unsigned int ChunkY, unsigned int Pitch)
//{
//	unsigned int RowIndex = ChunkY * 4 * Pitch;
//	unsigned int Index = RowIndex + ChunkX * 4;
//	
//	const ZEBYTE* Temp = Output + Index;
//
//	for (size_t Y = 0; Y < 4; Y++)
//		for (size_t X = 0; X < 4; X++)
//			*(int*)(Temp + Y * Pitch + X * 4) = *(int*)(Input + Y * 16 + X * 4);
//}


