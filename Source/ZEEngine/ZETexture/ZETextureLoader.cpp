//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureLoader.cpp
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

#include "ZEError.h"
#include "ZEFile/ZEFile.h"
#include "ZETextureData.h"
#include "ZETextureLoader.h"
#include "ZECore/ZEConsole.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZETextureFileFormat.h"


#define WIN32_LEAN_AND_MEAN

//#define FREEIMAGE_LIB
#include <FreeImage.h>



static unsigned DLL_CALLCONV	FreeImageFile_Write_2D(void *buffer, unsigned size, unsigned count, fi_handle handle);
static unsigned DLL_CALLCONV	FreeImageFile_Read_2D(void *buffer, unsigned size, unsigned count, fi_handle handle);
static ZEInt	DLL_CALLCONV	FreeImageFile_Seek_2D(fi_handle handle, long offset, ZEInt origin);
static long		DLL_CALLCONV	FreeImageFile_Tell_2D(fi_handle handle);


// Checks if the file is in ZE texture file format or not
bool ZETextureLoader::IsZETextureFile(const ZEString& FilePath)
{
	ZEFile File;
	if(!File.Open(FilePath, ZE_FOM_READ, ZE_FCM_NONE))
	{
		zeError("Cannot open file: \"%s\".", FilePath.GetValue());
		return false;
	}

	ZETextureFileHeader Header;
	if(File.Read(&Header, sizeof(ZETextureFileHeader), 1) != 1)
	{
		zeError("Cannot read header of file: \"%s\".", FilePath.GetValue());
		return false;
	}
	File.Close();

	if(Header.ChunkId == ZE_TXTF_HEADER)
		return true;

	return false;
}

// Checks the format of a previously opened file
bool ZETextureLoader::IsZETextureFile(ZEFile* File)
{
	if(!File->IsOpen())
		return false;

	ZEUInt64 Cursor = File->Tell();
	File->Seek(0, ZE_SF_BEGINING);
		
	ZETextureFileHeader	FileHeader;
	if(File->Read(&FileHeader, sizeof(ZETextureFileHeader), 1) == 1 && FileHeader.ChunkId == ZE_TXTF_HEADER)
	{
		File->Seek(Cursor, ZE_SF_BEGINING);
		return true;
	}

	File->Seek(Cursor, ZE_SF_BEGINING);
	return false;
}

// Loads a known image format as a surface of ZETexture data
bool ZETextureLoader::LoadFromImageFile(ZEFile* File, ZETextureData* TextureData)
{
	FIBITMAP	*Bitmap;
	FIBITMAP	*Bitmap32;

	zeLog("Loading texture from image file : \"%s\".", File->GetPath().GetValue());

	//Set callbacks
	FreeImageIO Callbacks;
	Callbacks.write_proc = &FreeImageFile_Write_2D;
	Callbacks.read_proc = &FreeImageFile_Read_2D;
	Callbacks.seek_proc = &FreeImageFile_Seek_2D;
	Callbacks.tell_proc = &FreeImageFile_Tell_2D;

	File->Seek(0, ZE_SF_BEGINING);

	FREE_IMAGE_FORMAT FileFormat = FreeImage_GetFileTypeFromHandle(&Callbacks, (fi_handle)File);
	if (FileFormat == FIF_UNKNOWN)
	{
		zeError("Unsupported format. File name: \"%s\"", File->GetPath().GetValue());
		return false;
	}

	Bitmap = FreeImage_LoadFromHandle(FileFormat, &Callbacks, (fi_handle)File);
	if(Bitmap == NULL)
	{
		zeError("Unknown error while loading the image. File name : \"%s\"", File->GetPath().GetValue());
		return false;
	}

	if(FreeImage_GetBPP(Bitmap) != 32)
	{
		Bitmap32 = FreeImage_ConvertTo32Bits(Bitmap);
		FreeImage_Unload(Bitmap);
	}
	else
	{
		Bitmap32 = Bitmap;
	}

	ZEUInt BPP			= FreeImage_GetBPP(Bitmap32);
	ZEUInt Width		= FreeImage_GetWidth(Bitmap32);
	ZEUInt Height		= FreeImage_GetHeight(Bitmap32);
	ZEUInt PixelSize	= BPP / 8;
	ZEUInt RowSize		= Width * PixelSize;
	ZEUInt RowCount		= Height;

	TextureData->Create(ZE_TT_2D, ZE_TPF_I8_4, 1, 1, Width, Height);
	BYTE* TargetData = (BYTE*)TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetData();

	FreeImage_ConvertToRawBits(TargetData, Bitmap32, RowSize, BPP, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
	
	// Swap red blue channels
	ZEUInt8 Red = 0, Blue = 0;
	for (ZESize H = 0; H < Height; H++)
	{
		for (ZESize W = 0; W < Width; W++)
		{
			BYTE* PixelData = TargetData + H * Width * PixelSize + W * PixelSize;

			Red = PixelData[0];
			Blue = PixelData[2];

			PixelData[0] = Blue;
			PixelData[2] = Red;
		}
	}
	
	zeLog("Image file loaded successfully: \"%s\".", File->GetPath().GetValue());

	FreeImage_Unload(Bitmap32);
	Bitmap32 = Bitmap = NULL;
	return true;

}

// Saves level 0 of surface 0 to specified file in ".TGA" format
bool ZETextureLoader::SaveAsImageFile(ZEFile* File, ZETextureData* TextureData, ZESize Surface, ZESize Level)
{
	if(TextureData->GetPixelFormat() != ZE_TPF_I8_4)
	{
		zeError("Pixel Format is not supported for saving.");
		return false;
	}

	zeLog("Saving texture as image file : \"%s\".", File->GetPath().GetValue());
	 
	//Set callbacks
	FreeImageIO Callbacks;
	Callbacks.write_proc = &FreeImageFile_Write_2D;
	Callbacks.read_proc	 = &FreeImageFile_Read_2D;
	Callbacks.seek_proc  = &FreeImageFile_Seek_2D;
	Callbacks.tell_proc  = &FreeImageFile_Tell_2D;

	File->Seek(0, ZE_SF_BEGINING);

	ZEUInt BPP			= 32;
	BYTE* SourceData	= (BYTE*)TextureData->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetData();
	ZESize Pitch		= TextureData->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetPitch();
	ZEUInt Height		= TextureData->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetHeight();
	ZEUInt Width		= TextureData->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetWidth();

	FIBITMAP* Bitmap;
	Bitmap = FreeImage_ConvertFromRawBits(SourceData, (ZEInt)Width, (ZEInt)Height, (ZEInt)Pitch, BPP, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
	if(Bitmap == NULL)
	{
		zeError("Error during conversion, cannot save image to \"%s\".", File->GetPath().GetValue());
		FreeImage_Unload(Bitmap);
		return false;
	}

	if(!FreeImage_SaveToHandle(FIF_TARGA, Bitmap, &Callbacks, (fi_handle)File))
	{
		zeError("Unknown error, Cannot save image to \"%s\".", File->GetPath().GetValue());
		FreeImage_Unload(Bitmap);
		return false;
	}

	zeLog("Image file saved successfully : \"%s\".", File->GetPath().GetValue());

	FreeImage_Unload(Bitmap);
	Bitmap = NULL;
	return true;

}

// File pointer must be at the beginning of the ZETexture file
bool ZETextureLoader::Read(ZEFile* File, ZETextureData* TextureData)
{
	zeLog("Loading texture from ZETexture file : \"%s\".", File->GetPath().GetValue());

	// Get file header
	ZETextureFileHeader	FileHeader;
	if(File->Read(&FileHeader, sizeof(ZETextureFileHeader), 1) != 1)
	{
		zeCriticalError("Cannot read file header from disk! File name: \"&s\".", File->GetPath().GetValue());
		TextureData->Destroy();
		return false;
	}

	// Check File header is correct
	if(FileHeader.ChunkId != ZE_TXTF_HEADER)
	{
		zeCriticalError("File header chunk id mismatch. Possible corruption. File name: \"&s\".", File->GetPath().GetValue());
		TextureData->Destroy();
		return false;
	}

	// Create TextureData
	TextureData->Create(	(ZETextureType)FileHeader.TextureType, 
								(ZETexturePixelFormat)FileHeader.PixelFormat, 
								FileHeader.SurfaceCount,
								FileHeader.LevelCount, 
								FileHeader.Width, 
								FileHeader.Height );

	// For every surface
	for(ZESize I = 0; I < (ZESize)FileHeader.SurfaceCount; ++I)
	{
		// Get surface header
		ZETextureFileSurfaceChunk	SurfaceChunk;
		if(File->Read(&SurfaceChunk, sizeof(ZETextureFileSurfaceChunk), 1) != 1)
		{
			zeCriticalError("Cannot read surface header from disk! File name: \"&s\".", File->GetPath().GetValue());
			TextureData->Destroy();
			return false;
		}

		// Check surface header is correct
		if(SurfaceChunk.ChunkId != ZE_TXTF_SURF_CHUNKID)
		{
			zeCriticalError("Surface chunk id mismatch. Possible corruption. File name: \"&s\".", File->GetPath().GetValue());
			TextureData->Destroy();
			return false;
		}

		// Get surface data
		// There is no surface data

		// For every level
		for(ZESize J = 0; J < (ZESize)FileHeader.LevelCount; ++J)
		{
			ZETExtureFileMipmapChunk MipmapChunk;
			if(File->Read(&MipmapChunk, sizeof(ZETExtureFileMipmapChunk), 1) != 1)
			{
				zeCriticalError("Cannot read level header from disk! File name: \"&s\".", File->GetPath().GetValue());
				TextureData->Destroy();
				return false;
			}

			// Check level header is correct
			if(MipmapChunk.ChunkId != ZE_TXTF_MIP_CHUNKID)
			{
				zeCriticalError("Level chunk id mismatch. Possible corruption. File name: \"&s\".", File->GetPath().GetValue());
				TextureData->Destroy();
				return false;
			}

			ZETextureLevel* CurrentLevel = &TextureData->GetSurfaces().GetItem(I).GetLevels().GetItem(J);

			if (CurrentLevel->GetRowCount() != MipmapChunk.RowCount || CurrentLevel->GetPitch() != (ZESize)MipmapChunk.RowSize )
			{
				zeCriticalError("Unexpected level data red from file. File name: \"&s\".", File->GetPath().GetValue());
				TextureData->Destroy();
				return false;
			}

			// Do not read if level data is empty
			if (CurrentLevel->GetSize() != 0)
			{
				if(File->Read(CurrentLevel->GetData(), CurrentLevel->GetSize(), 1) != 1)
				{
					zeCriticalError("Cannot read level data from disk! File name: \"&s\".", File->GetPath().GetValue());
					TextureData->Destroy();
					return false;
				}
			}
			
			if (File->Eof())
			{
				zeCriticalError("Eof reached... Possible corruption! File name: \"&s\".", File->GetPath().GetValue());
				TextureData->Destroy();
				return false;
			}
		}
	}

	zeLog("Texture loaded successfully: \"%s\".", File->GetPath().GetValue());
	return true;
}

bool ZETextureLoader::Write(ZEFile* File, ZETextureData* TextureData)
{
	// Create file header
	ZETextureFileHeader	FileHeader;
	FileHeader.ChunkId		= ZE_TXTF_HEADER;
	FileHeader.Version		= ZE_TXTF_VERSION;
	FileHeader.TextureType	= TextureData->GetType();
	FileHeader.PixelFormat	= TextureData->GetPixelFormat();
	FileHeader.SurfaceCount	= TextureData->GetSurfaceCount();
	FileHeader.LevelCount	= TextureData->GetLevelCount();
	FileHeader.Width		= TextureData->GetWidth();
	FileHeader.Height		= TextureData->GetHeight();

	// Write file header
	if(File->Write(&FileHeader, sizeof(ZETextureFileHeader), 1) != 1)
	{
		zeCriticalError("Cannot write file header to disk!");
		return false;
	}

	// For every surface
	for(ZESize I = 0; I < (ZESize)FileHeader.SurfaceCount; ++I)
	{
		// Create and write surface header
		ZETextureFileSurfaceChunk	SurfaceChunk;
		SurfaceChunk.ChunkId = ZE_TXTF_SURF_CHUNKID;

		if(File->Write(&SurfaceChunk, sizeof(ZETextureFileSurfaceChunk), 1) != 1)
		{
			zeCriticalError("Cannot write surface header to disk! File name: \"&s\".", File->GetPath().GetValue());
			return false;
		}

		// For every level
		for(ZESize J = 0; J < (ZESize)FileHeader.LevelCount; ++J)
		{
			// Create and write level header
			ZETExtureFileMipmapChunk	MipmapChunk;
			MipmapChunk.ChunkId		= ZE_TXTF_MIP_CHUNKID;
			MipmapChunk.RowCount	= (ZEUInt32)TextureData->GetSurfaces().GetItem(I).GetLevels().GetItem(J).GetRowCount();
			MipmapChunk.RowSize		= (ZEUInt32)TextureData->GetSurfaces().GetItem(I).GetLevels().GetItem(J).GetPitch();

			if(File->Write(&MipmapChunk, sizeof(ZETExtureFileMipmapChunk), 1) != 1)
			{
				zeCriticalError("Cannot write level header to disk! File name: \"&s\".", File->GetPath().GetValue());
				return false;
			}

			ZETextureLevel* CurrentLevel = &TextureData->GetSurfaces().GetItem(I).GetLevels().GetItem(J);
			// Do not write if level data is empty
			if (CurrentLevel->GetSize() != 0)
			{
				if(File->Write(CurrentLevel->GetData(), CurrentLevel->GetSize(), 1) != 1)
				{
					zeCriticalError("Cannot write level data! File name: \"&s\".", File->GetPath().GetValue());
					return false;
				}
			}

			if (File->Eof())
			{
				zeCriticalError("Eof reached... Possible corruption! File name: \"&s\".", File->GetPath().GetValue());
				TextureData->Destroy();
				return false;
			}
		}
	}

	File->Flush();
	zeLog("Saved successfully : \"%s\".", ZEFileInfo::GetFileName(File->GetPath()).ToCString());
	return true;
}

bool ZETextureLoader::LoadFromFile(ZEFile* File, ZETextureData* TextureData)
{
	bool Result = false;

	if(ZETextureLoader::IsZETextureFile(File))	
	{
		// Load from pack or original texture file 
		// Where data is stored in ZETextureFile format in both cases
		Result = ZETextureLoader::Read(File, TextureData);
	}
	else 
	{
		// Else assume it is image file with a known extension and try to load it
		Result = ZETextureLoader::LoadFromImageFile(File, TextureData);
	}

	return Result;
}

bool ZETextureLoader::GetImageInfo(ZETextureDataInfo* TextureInfo, ZEFile* File)
{
	zeLog("Getting image info: \"%s\".", File->GetPath().GetValue());

	ZEUInt64 Cursor = File->Tell();
	File->Seek(0, ZE_SF_BEGINING);

	FreeImageIO Callbacks;
	Callbacks.write_proc	= &FreeImageFile_Write_2D;
	Callbacks.read_proc		= &FreeImageFile_Read_2D;
	Callbacks.seek_proc		= &FreeImageFile_Seek_2D;
	Callbacks.tell_proc		= &FreeImageFile_Tell_2D;

	// Get image info
	FREE_IMAGE_FORMAT ImageFormat = FreeImage_GetFileTypeFromHandle(&Callbacks, (fi_handle)File);
	if (ImageFormat == FIF_UNKNOWN)
	{
		zeError("Unsupported image format. File Path : \"%s\"", File->GetPath().GetValue());
		return false;
	}

	// If possible, only load the image header
	FIBITMAP *Bitmap = FreeImage_LoadFromHandle(ImageFormat, &Callbacks, (fi_handle)File, FIF_LOAD_NOPIXELS);
	if(Bitmap == NULL)
	{
		zeError("Cannot get image info. File Path : \"%s\"", File->GetPath().GetValue());
		return false;
	}

	// Fill texture info
	TextureInfo->LevelCount		= 1;
	TextureInfo->SurfaceCount	= 1;
	TextureInfo->PixelFormat	= ZE_TPF_I8_4;
	TextureInfo->Width			= FreeImage_GetWidth(Bitmap);
	TextureInfo->Height			= FreeImage_GetHeight(Bitmap);
	
	FreeImage_Unload(Bitmap);
	Bitmap = NULL;

	zeLog("Image info gathered successfully: \"%s\".", File->GetPath().GetValue());

	File->Seek(Cursor, ZE_SF_BEGINING);
	return true;
}

bool ZETextureLoader::GetTextureInfo(ZETextureDataInfo* TextureInfo, ZEFile* File)
{
	// Go to the beginning of file
	ZEUInt64 Cursor = File->Tell();
	File->Seek(0, ZE_SF_BEGINING);

	zeLog("Getting texture info: \"%s\".", File->GetPath().GetValue());

	ZETextureFileHeader FileHeader;
	if(File->Read(&FileHeader, sizeof(ZETextureFileHeader), 1) != 1)
		return false;

	if (FileHeader.ChunkId != ZE_TXTF_HEADER)
		return false;
	
	// Fill texture info
	TextureInfo->Width			= FileHeader.Width;
	TextureInfo->Height			= FileHeader.Height;
	TextureInfo->SurfaceCount	= FileHeader.SurfaceCount;
	TextureInfo->LevelCount		= FileHeader.LevelCount;
	TextureInfo->PixelFormat	= (ZETexturePixelFormat)FileHeader.PixelFormat;
	
	zeLog("Texture info gathered successfully: \"%s\".", File->GetPath().GetValue());

	File->Seek(Cursor, ZE_SF_BEGINING);
	return true;
}

ZETextureLoader::ZETextureLoader()
{
	// Empty
}

ZETextureLoader::~ZETextureLoader()
{
	// Empty
}

static unsigned DLL_CALLCONV FreeImageFile_Write_2D(void *buffer, unsigned size, unsigned count, fi_handle handle)
{
	return (ZEUInt)((ZEFile*)handle)->Write(buffer, size, count);;
}

static unsigned DLL_CALLCONV FreeImageFile_Read_2D(void *buffer, unsigned size, unsigned count, fi_handle handle) 
{
	return (ZEUInt)((ZEFile*)handle)->Read(buffer, size, count);
}

static ZEInt DLL_CALLCONV FreeImageFile_Seek_2D(fi_handle handle, long offset, ZEInt origin)
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

	return ((ZEFile*)handle)->Seek(offset, OriginNorm);
}

static long DLL_CALLCONV FreeImageFile_Tell_2D(fi_handle handle) 
{
	return (long)((ZEFile*)handle)->Tell();
}
