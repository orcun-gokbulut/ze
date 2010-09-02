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
#include "ATI_Compress.h"




unsigned DLL_CALLCONV FreeImageFile_Read_2D(void *buffer, unsigned size, unsigned count, fi_handle handle) 
{
	return (unsigned int)((ZEResourceFile*)handle)->Read(buffer, size, count);
}

int DLL_CALLCONV FreeImageFile_Seek_2D(fi_handle handle, long offset, int origin) 
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

long DLL_CALLCONV FreeImageFile_Tell_2D(fi_handle handle) 
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
	FreeImage_Initialise();
	Texture = NULL;
};

ZETexture2DResource::~ZETexture2DResource()
{
	FreeImage_DeInitialise();
	if (Texture != NULL)
		Texture->Release();
};

ZETexture2DResource* ZETexture2DResource::LoadResource(const char* FileName)
{
	ZETexture2DResource* TextureResource;
	ZEResourceFile File;
	if (File.Open(FileName))
	{
		TextureResource = LoadResource(&File, false);
		File.Close();
		return TextureResource;
	}
	else
	{
		zeError("TextureResource", "Texture file not found. FileName : \"%s\"", FileName);
		return NULL;
	}
}

ZETexture2DResource* ZETexture2DResource::LoadResource(ZEResourceFile* ResourceFile, bool EmbededResource, bool CreateMipMap, unsigned int MinTextureQuality)
{
	bool			IsResizeable = TRUE;
	bool			IsCompressible = TRUE;
	unsigned int	Width;
	unsigned int	Height;
	unsigned int	BPP;
	unsigned int	Pitch;
	unsigned int	MaxMipmapNumber = 0;
	unsigned int	TextureQuality = zeGraphics->GetTextureQuality();
	unsigned char*	Image = NULL;
	
	char AbsolutePath[ZE_MAX_FILE_NAME_SIZE];
	sprintf(AbsolutePath, "resources\\%s", ResourceFile->GetFileName());
	
	
	if (TextureQuality < 4 && !EmbededResource)
	{
		if (CheckInFileCache(AbsolutePath))
		{
			return LoadFromFileCache(AbsolutePath);
		}
		else
		{			
			LoadFromOriginalFile(ResourceFile, Image, Width, Height, BPP, Pitch);
			if(Image == NULL)
			{
				zeError("Texture2D Resource", "Cannot Load Resource From Original File: \"%s\".", ResourceFile->GetFileName());
				return NULL;
			}

			// Check if it is Resizeable (power of 2)
			if ( ((Width & (Width - 1)) != 0) || ((Height & (Height - 1)) != 0) )
				IsResizeable = FALSE;

			// Check if it is Compressible (divisible by 4)
			if (((Width % 4) != 0) || ((Height % 4) != 0))
				IsCompressible = FALSE;

			// Get max possible Mipmap levels
			if(IsResizeable)
			{
				if (TextureQuality == 1)// with 4x down sample MaxMipmapNumber have to be 2 less than its value
					MaxMipmapNumber = GetMipmapCount(Width, Height) - 2;
				else if (TextureQuality == 2)
					MaxMipmapNumber = GetMipmapCount(Width, Height) - 1;
				else
					MaxMipmapNumber = GetMipmapCount(Width, Height);
			}
			
			ZETexture2DResource* TextureResource = new ZETexture2DResource();
			ZETexture2D* Texture = TextureResource->Texture = ZETexture2D::CreateInstance();

			TextureResource->SetFileName(ResourceFile->GetFileName());
			TextureResource->Cached = false;
			TextureResource->Shared = false;

			if (!Texture->Create(Width, Height, ZE_TPF_RGBA_INT32, false, MaxMipmapNumber))
			{
				zeError("Texture Resource", "Can not create texture resource. FileName : \"%s\"", ResourceFile->GetFileName());
				delete TextureResource;
				return NULL;
			}

			// Resize or Compress according to Texture Quality
			switch(TextureQuality)
			{
				case 1:
					// 4x4 Resize
					if (IsResizeable)
					{
						zeLog("Texture2D Resource", "RESIZING texture file:  \"%s\".", AbsolutePath);
						Rescale(Image, Image, Width, Height, Pitch, BPP, 4);
					}
					CreateMipmaps(TextureResource, Image, Width, Height, BPP, Pitch, MaxMipmapNumber, IsResizeable, IsCompressible, ZE_T2DR_Dxt3, ZE_T2DR_Normal);
					break;
				
				case 2:
					// 2x2 Resize
					if (IsResizeable)
					{
						zeLog("Texture2D Resource", "RESIZING texture file:  \"%s\".", AbsolutePath);
						Rescale(Image, Image, Width, Height, Pitch, BPP, 2);
					}
					CreateMipmaps(TextureResource, Image, Width, Height, BPP, Pitch, MaxMipmapNumber, IsResizeable, IsCompressible, ZE_T2DR_Dxt3, ZE_T2DR_Normal);
					break;
				
				case 3:
					// No Resize
					CreateMipmaps(TextureResource, Image, Width, Height, BPP, Pitch, MaxMipmapNumber, IsResizeable, IsCompressible, ZE_T2DR_Dxt3, ZE_T2DR_Normal);
					break;
			}

			//	If not EmbededResource Save to file cache for future use
			//  Mipmapler oturana kadar file cache e yazma
			/*if(!EmbededResource)
				SaveToFileCache(CompressedImage, Width, Height, BPP, Pitch, AbsolutePath);*/
			
			free(Image);
			
			return TextureResource;
		}
	}
	// If texture quality is 4 or if it is EmbededResource Load from original file
	else
	{
		LoadFromOriginalFile(ResourceFile, Image, Width, Height, BPP, Pitch);
		if (Image == NULL)
		{
			zeError("Texture2D Resource", "Cannot Load Resource From Original File: \"%s\".", ResourceFile->GetFileName());
			return NULL;
		}

		// Check if it is Resizeable (power of 2)
		if ( ((Width & (Width - 1)) != 0) || ((Height & (Height - 1)) != 0) )
			IsResizeable = FALSE;

		ZETexture2DResource* TextureResource = new ZETexture2DResource();
		ZETexture2D* Texture = TextureResource->Texture = ZETexture2D::CreateInstance();

		TextureResource->SetFileName(ResourceFile->GetFileName());
		TextureResource->Cached = false;
		TextureResource->Shared = false;

		if(IsResizeable)
			MaxMipmapNumber = GetMipmapCount(Width, Height);
		
		if (!Texture->Create(Width, Height, ZE_TPF_RGBA_INT32, false, MaxMipmapNumber))
		{
			zeError("Texture Resource", "Can not create texture resource. FileName : \"%s\"", ResourceFile->GetFileName());
			delete TextureResource;
			return NULL;
		}

		CreateMipmaps(TextureResource, Image, Width, Height, BPP, Pitch, MaxMipmapNumber, IsResizeable, IsCompressible, ZE_T2DR_Dxt3, ZE_T2DR_Normal);

		free(Image);
		
		return TextureResource;
	}
}
	
ZETexture2DResource* ZETexture2DResource::LoadSharedResource(const char* FileName)
{	
	
	ZETexture2DResource* NewResource =(ZETexture2DResource*)zeResources->GetResource(FileName);
	if (NewResource == NULL)
	{
		NewResource = LoadResource(FileName);
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



/*********************************************GOKAY*************************************************/

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


// OK
ZEBYTE* ZETexture2DResource::Compress(ZEBYTE* Image, int Width, int Height, int Pitch, CompressionType CType, CompressionSpeed CSpeed)
{	
	ATI_TC_FORMAT	Format;
	ATI_TC_Speed	Speed;
	int				CompressedPitch;	
	
	// Decide Compression Type
	switch(CType)
	{
		case ZE_T2DR_Dxt1:
			Format = ATI_TC_FORMAT_DXT1;
			CompressedPitch = Width / 2;
			break;
		case ZE_T2DR_Dxt3:
			Format = ATI_TC_FORMAT_DXT3;
			CompressedPitch = Width;
			break;
		case ZE_T2DR_Dxt5:
			Format = ATI_TC_FORMAT_DXT5;
			CompressedPitch = Width;
			break;
		case ZE_T2DR_BC5:
			Format = ATI_TC_FORMAT_BC5;
			CompressedPitch = Width;
			break;
		default:
			break;
	};

	// Decide compression Speed
	switch(CSpeed)
	{
		case ZE_T2DR_Normal:
			Speed = ATI_TC_Speed_Normal;
			break;
		case ZE_T2DR_Fast:
			Speed = ATI_TC_Speed_Fast;
			break;
		case ZE_T2DR_SuperFast:
			Speed = ATI_TC_Speed_SuperFast;
			break;
		default:
			break;
	};
	///////////////////////////////////////COMPRESS///////////////////////////////////
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
	
	
	///////////////////////////////////DECOMPRESS(TEMP)//////////////////////////

	ZEBYTE* DecompressedImage = new ZEBYTE[Height * Pitch];
	
	ATI_TC_Texture srcTexture1;
	srcTexture1.dwWidth = Width;
	srcTexture1.dwHeight = Height;
	srcTexture1.dwPitch = CompressedPitch;
	srcTexture1.format = Format;
	srcTexture1.pData = CompressedData;
	srcTexture1.dwSize = sizeof(srcTexture1);
	srcTexture1.dwDataSize = ATI_TC_CalculateBufferSize(&srcTexture1);
	
	ATI_TC_Texture destTexture1;
	destTexture1.dwWidth = Width;
	destTexture1.dwHeight = Height;
	destTexture1.dwPitch = Pitch;
	destTexture1.format = ATI_TC_FORMAT_ARGB_8888;
	destTexture1.pData = DecompressedImage;
	destTexture1.dwSize = sizeof(destTexture1);
	destTexture1.dwDataSize = ATI_TC_CalculateBufferSize(&destTexture1);
	
	ATI_TC_CompressOptions options1;
	options1.nCompressionSpeed = Speed;
	options1.dwSize = sizeof(options1);
	memset(&options1, 0, options1.dwSize);
	
	ATI_TC_ConvertTexture(&srcTexture1, &destTexture1, &options1, NULL, NULL, NULL);

	///////////////////////////END OF DECOMPRESS(TEMP)////////////////////////////
		
	delete [] CompressedData;

	return DecompressedImage;
}


// OK
bool ZETexture2DResource::CheckInFileCache(const char *FileName)
{	
	char *CompletePath = new char[strlen(FileName) + strlen(".cache") + 1];
	strcpy (CompletePath, FileName);
	strcat (CompletePath, ".cache");

	int Stat;
	struct stat FileInfo;
	std::string FName =  std::string(CompletePath);

	Stat = stat(CompletePath, &FileInfo);
	
	delete[] CompletePath;

	if (Stat == 0)
		return TRUE;

	return FALSE;
}

// DEGISECEK
void ZETexture2DResource::SaveToFileCache(const ZEBYTE* Image, int Width, int Height, int BPP, int Pitch, const char* FileName)
{
	char CacheFileName[ZE_MAX_FILE_NAME_SIZE];
	sprintf(CacheFileName, "%s.cache", FileName);
	

	zeLog("Texture2D Resource", "SAVING texture to FILE CACHE \"%s\".", CacheFileName);

	FILE* FileToWrite = NULL;
	FileToWrite = fopen(CacheFileName, "wb+");

	//write width(4byte), height(4byte), BPP(4byte), Pitch(4Byte) and image data
	fwrite(&Width, 4, 1, FileToWrite);
	fwrite(&Height, 4, 1, FileToWrite);
	fwrite(&BPP, 4, 1, FileToWrite);
	fwrite(&Pitch, 4, 1, FileToWrite);
	fwrite(Image, 1, Width*Height*4, FileToWrite);
	
	fclose(FileToWrite);
}

// DEGISECEK
ZETexture2DResource* ZETexture2DResource::LoadFromFileCache(const char *FileName)
{
	zeLog("Texture2D Resource", "LOADING texture from FILE CACHE \"%s\".", FileName);
	char* ResourceFilePath  = new char [ZE_MAX_FILE_NAME_SIZE];
	sprintf(ResourceFilePath, "%s.cache", FileName);
	
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

	WriteToDevice(TextureResource, Image, Width, Height, BPP, Pitch);
	
	zeLog("Texture2D Resource", "Texture file \"%s\" has been loaded from FILE CACHE.", FileName);
	
	delete [] Image;
	delete [] ResourceFilePath;
	
	return TextureResource;
}

// OK
bool ZETexture2DResource::LoadFromOriginalFile(ZEResourceFile* ResourceFile, unsigned char* &Image, unsigned int &Width, unsigned int &Height, unsigned int &BPP, unsigned int &Pitch)
{
	const char *FileName = ResourceFile->GetFileName();
	zeLog("Texture2D Resource", "LOADING texture from ORIGINAL FILE  \"%s\".", FileName);
	
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
			return FALSE;
		}
		
		FIBITMAP* Bitmap = FreeImage_LoadFromHandle(TextureFormat, &Callbacks, ResourceFile);
		
		FIBITMAP* ConvertedData = FreeImage_ConvertTo32Bits(Bitmap);
		
		Width = FreeImage_GetWidth(ConvertedData);
		Height = FreeImage_GetHeight(ConvertedData);
		BPP = FreeImage_GetBPP(ConvertedData) / 8;
		Pitch = FreeImage_GetPitch(ConvertedData);
		
		Image = (unsigned char*)malloc(Height * Pitch);
		if (!Image)
		{
			zeError("Texture Resource","Cannot Allocate Memmory for the Texture. FileName : \"%s\"", FileName);
			return FALSE;
		}
		FreeImage_ConvertToRawBits((BYTE*)Image, ConvertedData, Pitch, BPP * 8, 0x00FF0000, 0x0000FF00, 0x000000FF);
		
		FreeImage_Unload(Bitmap);
		FreeImage_Unload(ConvertedData);

		File.Close();

		return TRUE;
	}
}

// OK
void ZETexture2DResource::WriteToDevice(ZETexture2DResource *TextureResource, const unsigned char *Source, int Width, int Height, int BPP, unsigned int Pitch, unsigned int Level)
{
	void* Buffer = NULL;
	
	TextureResource->Texture->Lock(&Buffer, &Pitch, Level);
	for (int I = 0; I < Height; I++)
		memcpy((unsigned char*)Buffer + (I * Width * BPP), Source + (Height - I - 1) * Width * BPP, Width * BPP);

	TextureResource->Texture->Unlock();
}

// OK
bool ZETexture2DResource::Rescale(const unsigned char* DestinationData, const unsigned char* SourceData, unsigned int &SourceWidth, unsigned int &SourceHeight, unsigned int &SourcePitch, unsigned int &SourceBPP, unsigned int RescaleRatio)
{
	int DestinationHeight = SourceHeight / RescaleRatio;
	int DestinationWidth = SourceWidth / RescaleRatio;
	int DestinationPitch = SourcePitch / RescaleRatio;
	int destinationBPP = SourceBPP * 8;

	FIBITMAP* Bitmap = FreeImage_ConvertFromRawBits((BYTE*)SourceData, SourceWidth, SourceHeight, SourcePitch, SourceBPP * 8, 0x00FF0000, 0x0000FF00, 0x000000FF);
	FIBITMAP* ResizedBitmap = FreeImage_Rescale(Bitmap, DestinationWidth, DestinationHeight, FILTER_BOX);

	SourceHeight =  SourceHeight / RescaleRatio;
	SourceWidth = SourceWidth / RescaleRatio;
	SourcePitch = SourcePitch / RescaleRatio;
	
	FreeImage_ConvertToRawBits((BYTE*)DestinationData, ResizedBitmap, DestinationPitch, destinationBPP, 0x00FF0000, 0x0000FF00, 0x000000FF);

	FreeImage_Unload(ResizedBitmap);
	FreeImage_Unload(Bitmap);
	
	return TRUE;
}

// OK
void ZETexture2DResource::CreateMipmaps(ZETexture2DResource* TextureResource, const ZEBYTE* Image, unsigned int Width, unsigned int Height, unsigned int BPP, unsigned int Pitch, unsigned int Level, bool DoResize, bool DoCompress, CompressionType Type, CompressionSpeed Speed)
{
	zeLog("Texture2D Resource", "Creeating MipMaps for the texture:  \"%s\".", TextureResource->GetFileName());
	unsigned char* CompressedImage = NULL;
	
	// If Resizeable Then Create Mipmaps 
	if (DoResize)
	{
		// Storage For the Copy of Image
		unsigned char* Temp = new unsigned char[Height * Pitch];
		
		// Copy The Image
		for (int I = 0; I < Height; I++)
			memcpy( Temp + (I * Pitch), Image + (I * Pitch), Pitch);

		
		if (DoCompress)
		{
			// Create Mipmaps for level-2 (2x2 and 1x1 not compressible)
			for (int I = 0; I < Level-2; I++)
			{
				CompressedImage = Compress(Temp, Width, Height, Pitch, Type, Speed);

				/*FIBITMAP* bibi = FreeImage_ConvertFromRawBits(CompressedImage, Width, Height, Pitch, 32, 0x00FF0000, 0x0000FF00, 0x000000FF);
				FreeImage_Save(FIF_TARGA, bibi, "test.tga", 0);*/
				
				// Write Mipmap to device level I
				WriteToDevice(TextureResource, CompressedImage, Width, Height, BPP, Pitch, I);
				Rescale(Temp, Temp, Width, Height, Pitch, BPP, 2);
				
				delete [] CompressedImage;
			}
			// 2x2
			WriteToDevice(TextureResource, Temp, Width, Height, BPP, Pitch, Level - 2);
			// 1x1
			Rescale(Temp, Temp, Width, Height, Pitch, BPP, 2);
			WriteToDevice(TextureResource, CompressedImage, Width, Height, BPP, Pitch, Level - 1);
			

		}
		else
		{
			// create Mipmaps without Compressing
			for (int I = 0; I < Level; I++)
			{
				// Write Mipmap to device level I
				WriteToDevice(TextureResource, Temp, Width, Height, BPP, Pitch, I);
				
				Rescale(Temp, Temp, Width, Height, Pitch, BPP, 2);
			}
		}

		delete [] Temp;
	}
	else
	{
		// Write Mipmap to device level 0
		WriteToDevice(TextureResource, Image, Width, Height, BPP, Pitch, 0);
	}
	
}

// OK
unsigned int ZETexture2DResource::GetMipmapCount(unsigned int Width, unsigned int Height)
{
	unsigned int WidthCount = (unsigned int)(log((double)Width) / log(2.0));
	unsigned int HeightCount = (unsigned int)(log((double)Height) / log(2.0));

	return WidthCount >= HeightCount ? HeightCount + 1 : WidthCount + 1;
}
