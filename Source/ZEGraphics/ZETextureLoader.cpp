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

#include "ZEGraphics\ZETextureLoader.h"
#include "ZECore\ZEError.h"
#include "ZECore\ZEConsole.h"

#define FREEIMAGE_LIB
#include <freeimage.h>
#include <sys/stat.h>
#include <stdio.h>
#include <math.h>


static unsigned DLL_CALLCONV	FreeImageFile_Read_2D(void *buffer, unsigned size, unsigned count, fi_handle handle);
static int DLL_CALLCONV			FreeImageFile_Seek_2D(fi_handle handle, long offset, int origin);
static long DLL_CALLCONV		FreeImageFile_Tell_2D(fi_handle handle);


ZETextureLoaderInfo ZETextureLoaderInfo::operator=( const ZETextureLoaderInfo &Info)
{
	TextureWidth = Info.TextureWidth;
	TextureHeight = Info.TextureHeight;
	TexturePitch = Info.TexturePitch;
	BitsPerPixel = Info.BitsPerPixel;
	TextureType = Info.TextureType;
	PixelFormat = Info.PixelFormat;

	ZETextureLoaderInfo ReturnInfo = {TextureWidth, TextureHeight, TexturePitch, BitsPerPixel, TextureType, PixelFormat};
	return ReturnInfo;
}

bool ZETextureLoaderInfo::operator!=( const ZETextureLoaderInfo &Info)
{
	return (TextureWidth != Info.TextureWidth &&
			TextureHeight != Info.TextureHeight &&
			TexturePitch != Info.TexturePitch &&
			BitsPerPixel != Info.BitsPerPixel &&
			TextureType != Info.TextureType &&
			PixelFormat != Info.PixelFormat) ? true : false;
}

ZETextureLoader::ZETextureLoader()
{
	/* Empty */
}
ZETextureLoader::~ZETextureLoader()
{
	/* Empty */
}


// Texture type is not set in this function. User have to specify the texture type data for LoadTexture() function
bool ZETextureLoader::GetTextureInfo(ZETextureLoaderInfo& TextureInfo, ZEResourceFile* ResourceFile)
{
	/* Check if file exists */
	/*char CompletePath[ZE_MAX_FILE_NAME_SIZE];
	sprintf(CompletePath, "\\resources\\%s", ResourceFile->GetFileName());

	int Stat;
	struct stat FileInfo;
	
	Stat = stat(CompletePath, &FileInfo);

	if (Stat != 0)
	{
		zeLog("Texture Loader", "File Does Not Exists. FileName: \"%s\".", ResourceFile->GetFileName());
		return false;
	}*/

	FreeImageIO Callbacks;
	Callbacks.read_proc = &FreeImageFile_Read_2D;
	Callbacks.seek_proc = &FreeImageFile_Seek_2D;
	Callbacks.tell_proc = &FreeImageFile_Tell_2D;
	
	/* Get Texture information */
	FREE_IMAGE_FORMAT ImageFormat = FreeImage_GetFileTypeFromHandle(&Callbacks, ResourceFile);
	if (ImageFormat == FIF_UNKNOWN) 
	{
		zeError("Texture Loader","Unsupported image format. FileName : \"%s\"", ResourceFile->GetFileName());
		return false;
	}

	// If possible only load the image header data
	FIBITMAP *Bitmap = FreeImage_LoadFromHandle(ImageFormat, &Callbacks, ResourceFile, FIF_LOAD_NOPIXELS);


	TextureInfo.BitsPerPixel = FreeImage_GetBPP(Bitmap);
	TextureInfo.TextureWidth = FreeImage_GetWidth(Bitmap);
	TextureInfo.TextureHeight = FreeImage_GetHeight(Bitmap);
	TextureInfo.TexturePitch = FreeImage_GetPitch(Bitmap);
	
	FREE_IMAGE_TYPE PixelFormat = FreeImage_GetImageType(Bitmap);

	switch(PixelFormat)
	{
		case FIT_BITMAP:
			{
				switch(TextureInfo.BitsPerPixel)
				{
					case 1:
					case 4:
					case 8:
					case 16:
					case 32:
					default:
						TextureInfo.PixelFormat = ZE_TPF_A8R8G8B8;
						break;
				}
			}
			break;
		case FIT_UINT16:
		case FIT_INT16:
		case FIT_UINT32:
		case FIT_INT32:
		case FIT_FLOAT:
		case FIT_DOUBLE:
		case FIT_COMPLEX:
		case FIT_RGB16:
		case FIT_RGBA16:
		case FIT_RGBAF:
		case FIT_RGBF:
		default:
			TextureInfo.PixelFormat = ZE_TPF_NOTSET;
			break;
	}


	FreeImage_Unload(Bitmap);
	ResourceFile->Seek(0, ZE_SF_BEGINING);
	Bitmap = NULL;
	return true;
}


bool ZETextureLoader::LoadTexture(void* DestBuffer, ZEResourceFile* ResourceFile, ZETextureLoaderInfo& TextureInfo)
{
	// If Texture info passed and texture format
	if(TextureInfo.PixelFormat == ZE_TPF_NOTSET)
	{
		zeError("Texture Loader","Cannot load, Unsupported file format. FileName : \"%s\"", ResourceFile->GetFileName());
		return false;
	}

	unsigned int		ImageWidth;
	unsigned int		ImageHeight;
	unsigned int		ImageBitsPP;
	unsigned int		ImagePitch;
	FIBITMAP			*Bitmap;
	FIBITMAP			*Bitmap32;
	
	
	//Set callbacks
	FreeImageIO Callbacks;
	Callbacks.read_proc = &FreeImageFile_Read_2D;
	Callbacks.seek_proc = &FreeImageFile_Seek_2D;
	Callbacks.tell_proc = &FreeImageFile_Tell_2D;

	zeLog("Texture Loader", "LOADING texture from file  \"%s\".", ResourceFile->GetFileName());
	
	
	FREE_IMAGE_FORMAT FileFormat = FreeImage_GetFileTypeFromHandle(&Callbacks, ResourceFile);
	if (FileFormat == FIF_UNKNOWN)
	{
		zeError("Texture Loader","Cannot load, Unsupported file format. FileName : \"%s\"", ResourceFile->GetFileName());
		return false;
	}
	Bitmap = FreeImage_LoadFromHandle(FileFormat, &Callbacks, ResourceFile);
	if(!Bitmap)
	{
		zeError("Texture Loader","Unknown error ocured while loading the texture. FileName : \"%s\"", ResourceFile->GetFileName());
		return false;
	}

	ImageWidth	= TextureInfo.TextureWidth;
	ImageHeight = TextureInfo.TextureHeight;
	ImageBitsPP = TextureInfo.BitsPerPixel;
	ImagePitch	= TextureInfo.TexturePitch;
	

	// if its not in A8R8G8B8 then convert it to 32 bit
	if(ImageBitsPP != 32)
	{
		Bitmap32 = FreeImage_ConvertTo32Bits(Bitmap);
		FreeImage_Unload(Bitmap);
		ImagePitch  = TextureInfo.TexturePitch = FreeImage_GetPitch(Bitmap32); 
		ImageBitsPP = TextureInfo.BitsPerPixel = FreeImage_GetBPP(Bitmap32);
	}
	else
	{
		Bitmap32 = Bitmap;
	}

	FreeImage_ConvertToRawBits((BYTE*)DestBuffer, Bitmap32, ImagePitch, ImageBitsPP, 0x00FF0000, 0x0000FF00, 0x000000FF, true);
	FreeImage_Unload(Bitmap32);
	Bitmap32 = NULL;
	Bitmap = NULL;

	ResourceFile->Seek(0, ZE_SF_BEGINING);
	return true;
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
