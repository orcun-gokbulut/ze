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

#include "ZETexture2DResource.h"
#include "ZECore\ZEError.h"
#include "ZECore\ZEConsole.h"
#include "ZECore\ZEResourceManager.h"
#include "ZECore\ZEResourceFile.h"
#include "ZETexture2D.h"

#include <freeimage.h>

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
	Texture = NULL;
};

ZETexture2DResource::~ZETexture2DResource()
{
	if (Texture != NULL)
		Texture->Release();
};

ZETexture2DResource* ZETexture2DResource::LoadResource(const char* FileName)
{
	ZETexture2DResource* TextureResource;
	ZEResourceFile File;
	if (File.Open(FileName))
	{
		TextureResource = LoadResource(&File);
		File.Close();
		return TextureResource;
	}
	else
	{
		zeError("TextureResource", "Texture file not found. FileName : \"%s\"", FileName);
		return NULL;
	}
}

ZETexture2DResource* ZETexture2DResource::LoadResource(ZEResourceFile* ResourceFile)
{
	zeLog("Texture2D Resource", "Loading texture file \"%s\".", ResourceFile->GetFileName());

	FreeImageIO Callbacks;
	Callbacks.read_proc = &FreeImageFile_Read_2D;
	Callbacks.seek_proc = &FreeImageFile_Seek_2D;
	Callbacks.tell_proc = &FreeImageFile_Tell_2D;

	FREE_IMAGE_FORMAT TextureFormat = FreeImage_GetFileTypeFromHandle(&Callbacks, ResourceFile);
	if (TextureFormat == FIF_UNKNOWN) 
	{
		zeError("Texture Resource","Unsported image format. FileName : \"%s\"", ResourceFile->GetFileName());
		return NULL;
	}

	ZETexture2DResource* TextureResource = new ZETexture2DResource();
	ZETexture2D* Texture = TextureResource->Texture = ZETexture2D::CreateInstance();

	if (Texture == NULL)
	{
		delete TextureResource;
		return NULL;
	}

	FIBITMAP* Data = FreeImage_LoadFromHandle(TextureFormat, &Callbacks, ResourceFile);
	FIBITMAP* ConvertedData;

	int Width = FreeImage_GetWidth(Data);
	int Height = FreeImage_GetHeight(Data);
	ZETexturePixelFormat Format;

	switch(FreeImage_GetImageType(Data))
	{
		case FIT_BITMAP:
			switch (FreeImage_GetBPP(Data))
			{
				case 16:
					zeWarning("Texture Resource", "There is performance hit converting 16 bit image to 32 bit image. Use 16 bit or 32 bit images. FileName : \"%s\"", ResourceFile->GetFileName());
					Format = ZE_TPF_RGBA_INT32; 
					ConvertedData = FreeImage_ConvertTo32Bits(Data);
					FreeImage_Unload(Data);
					Data = ConvertedData;
					break;
				case 8:
					zeWarning("Texture Resource", "There is performance hit converting 8 bit image to 32 bit image. Use 16 bit or 32 bit images. FileName : \"%s\"", ResourceFile->GetFileName());
					Format = ZE_TPF_RGBA_INT32;
					ConvertedData = FreeImage_ConvertTo32Bits(Data);
					FreeImage_Unload(Data);
					Data = ConvertedData;
					break;
				case 24:
					zeWarning("Texture Resource", "There is performance hit converting 24 bit image to 32 bit image. Use 16 bit or 32 bit images. FileName : \"%s\"", ResourceFile->GetFileName());
					Format = ZE_TPF_RGBA_INT32;
					ConvertedData = FreeImage_ConvertTo32Bits(Data);
					FreeImage_Unload(Data);
					Data = ConvertedData;
					break;
				case 32:
					Format = ZE_TPF_RGBA_INT32;
					break;
				default:
					zeError("Texture Resource", "Image pixel format is not supported. FileName : \"%s\"", ResourceFile->GetFileName());
					delete TextureResource;
					return NULL;
					break;			
			}
			break;
		default:
			zeError("Texture Resource", "Image pixel format is not supported. FileName : \"%s\"", ResourceFile->GetFileName());
			delete TextureResource;
			return NULL;
			break;			
	}

	if (!Texture->Create(Width, Height, Format))
	{
		zeError("Texture Resource", "Can not create texture resource. FileName : \"%s\"", ResourceFile->GetFileName());
		delete TextureResource;
		return NULL;
	}

	void* Buffer;
	int Pitch;

	Texture->Lock(&Buffer, &Pitch);
	if (Buffer == NULL)
	{
		zeError("Texture Resource", "Can not lock texture resource. FileName : \"%s\"", ResourceFile->GetFileName());
		delete TextureResource;
		return NULL;
	}
	void* FIBuffer;
	int BPP = FreeImage_GetBPP(Data) / 8;
	unsigned char* Bits = FreeImage_GetBits(Data);
	for (int I = 0; I < Height; I++)
		memcpy((unsigned char*)Buffer + (I * Pitch), Bits + (Height - I - 1) * Width * BPP, Width * BPP);

	Texture->Unlock();

	FreeImage_Unload(Data);
	TextureResource->SetFileName(ResourceFile->GetFileName());
	TextureResource->Cached = false;
	TextureResource->Shared = false;

	zeLog("Texture2D Resource", "Texture file \"%s\" has been loaded.", ResourceFile->GetFileName());

	return TextureResource;	
}

void ZETexture2DResource::CacheResource(const char* FileName)
{
	ZETexture2DResource* NewResource = (ZETexture2DResource*)zeResources->GetResource(FileName);
	if (NewResource == NULL)
	{
		NewResource = LoadResource(FileName);
		if (NewResource != NULL)
		{
			NewResource->Cached = true;
			NewResource->ReferenceCount = 0;
			zeResources->AddResource(NewResource);
		}
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




