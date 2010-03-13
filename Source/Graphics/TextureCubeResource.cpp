//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - TextureCubeResource.cpp
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

#include "TextureCubeResource.h"
#include "TextureCube.h"
#include "Core/Error.h"
#include "Core/ResourceManager.h"
#include <freeimage.h>

unsigned DLL_CALLCONV FreeImageFile_Read_Cube(void *buffer, unsigned size, unsigned count, fi_handle handle) 
{
	return (unsigned int)((ZEResourceFile*)handle)->Read(buffer, size, count);
}

int DLL_CALLCONV FreeImageFile_Seek_Cube(fi_handle handle, long offset, int origin) 
{
	return ((ZEResourceFile*)handle)->Seek(offset, (ZESeekFrom)origin);
}

long DLL_CALLCONV FreeImageFile_Tell_Cube(fi_handle handle) 
{
	return (long)((ZEResourceFile*)handle)->Tell();
}

void ZETextureCubeResource::CacheResource(const char* FileName)
{
	ZETextureCubeResource* NewResource = (ZETextureCubeResource*)zeResources->GetResource(FileName);
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

ZETextureCubeResource* ZETextureCubeResource::LoadSharedResource(const char* FileName)
{
	ZETextureCubeResource* NewResource =(ZETextureCubeResource*)zeResources->GetResource(FileName);
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

bool LoadCubeFace(ZETextureCube* Texture, ZETextureCubeFace Face, unsigned char* FIBuffer, int EdgeLenght, int OffsetX, int OffsetY, int BPP)
{
	void* Buffer;
	int Pitch;

	Texture->Lock(Face, &Buffer, &Pitch);
	if (Buffer == NULL)
	{
		zeError("Cube Texture Resource", "Can not lock texture resource. (Face : %d)", Face);
		return false;
	}

	for (int I = 0; I < EdgeLenght; I++)
		memcpy((unsigned char*)Buffer + (I * Pitch), 
		FIBuffer + (EdgeLenght * 3 * (OffsetY + EdgeLenght - I - 1) + OffsetX) * BPP, EdgeLenght * BPP);

	Texture->Unlock(Face);
	return true;
}

ZETextureCubeResource* ZETextureCubeResource::LoadResource(const char* FileName)
{
	ZETextureCubeResource* TextureResource;
	ZEResourceFile File;
	if (File.Open(FileName))
	{
		TextureResource = LoadResource(&File);
		File.Close();
		return TextureResource;
	}
	else
	{
		zeError("Cube Texture Resource", "Texture file not found. FileName : \"%s\"", FileName);
		return NULL;
	}
}

ZETextureCubeResource* ZETextureCubeResource::LoadResource(ZEResourceFile* ResourceFile)
{
	FreeImageIO Callbacks;
	Callbacks.read_proc = &FreeImageFile_Read_Cube;
	Callbacks.seek_proc = &FreeImageFile_Seek_Cube;
	Callbacks.tell_proc = &FreeImageFile_Tell_Cube;

	FREE_IMAGE_FORMAT TextureFormat = FreeImage_GetFileTypeFromHandle(&Callbacks, ResourceFile);
	if (TextureFormat == FIF_UNKNOWN) 
	{
		zeError("Cube Texture Resource","Unsported image format. FileName : \"%s\"", ResourceFile->GetFileName());
		return NULL;
	}

	ZETextureCubeResource* TextureResource = new ZETextureCubeResource();
	ZETextureCube* Texture = TextureResource->Texture = ZETextureCube::CreateInstance();

	if (Texture == NULL)
	{
		delete TextureResource;
		return NULL;
	}

	FIBITMAP* Data = FreeImage_LoadFromHandle(TextureFormat, &Callbacks, ResourceFile);
	FIBITMAP* ConvertedData;

	int Width = FreeImage_GetWidth(Data);
	int Height = FreeImage_GetHeight(Data);

	if (Width / 3 != Height / 2)
	{
		zeError("Cube Texture Resource", "File does not have correct dimensions. (FileName : \"%s\")", ResourceFile->GetFileName());
		return NULL;
	}
	
	int EdgeLenght = Width / 3;

	ZETexturePixelFormat Format;

	switch(FreeImage_GetImageType(Data))
	{
		case FIT_BITMAP:
			switch (FreeImage_GetBPP(Data))
			{
				case 16:
					zeWarning("Texture Resource", "There is performance hit converting 16 bit image to 32 bit image. Use 16 bit or 32 bit images. FileName : \"%s\"", ResourceFile->GetFileName());
					Format = ZE_TPF_ARGB32; 
					ConvertedData = FreeImage_ConvertTo32Bits(Data);
					FreeImage_Unload(Data);
					Data = ConvertedData;
					break;
				case 8:
					zeWarning("Texture Resource", "There is performance hit converting 8 bit image to 32 bit image. Use 16 bit or 32 bit images. FileName : \"%s\"", ResourceFile->GetFileName());
					Format = ZE_TPF_ARGB32;
					ConvertedData = FreeImage_ConvertTo32Bits(Data);
					FreeImage_Unload(Data);
					Data = ConvertedData;
					break;
				case 24:
					zeWarning("Texture Resource", "There is performance hit converting 24 bit image to 32 bit image. Use 16 bit or 32 bit images. FileName : \"%s\"", ResourceFile->GetFileName());
					Format = ZE_TPF_ARGB32;
					ConvertedData = FreeImage_ConvertTo32Bits(Data);
					FreeImage_Unload(Data);
					Data = ConvertedData;
					break;
				case 32:
					Format = ZE_TPF_ARGB32;
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

	if (!Texture->Create(EdgeLenght, Format))
	{
		zeError("Cube Texture Resource", "Can not create texture resource. FileName : \"%s\"", ResourceFile->GetFileName());
		delete TextureResource;
		return NULL;
	}

	void* FIBuffer;
	int BPP = FreeImage_GetBPP(Data) / 8;
	unsigned char* Bits = FreeImage_GetBits((FIBITMAP*)Data);

	if (!LoadCubeFace(Texture, ZE_CTF_NEGATIVEX, Bits, EdgeLenght, 0,				EdgeLenght, BPP) ||
		!LoadCubeFace(Texture, ZE_CTF_POSITIVEZ, Bits, EdgeLenght, EdgeLenght,		EdgeLenght, BPP) ||
		!LoadCubeFace(Texture, ZE_CTF_POSITIVEX, Bits, EdgeLenght, EdgeLenght * 2,	EdgeLenght, BPP) ||
		!LoadCubeFace(Texture, ZE_CTF_NEGATIVEZ, Bits, EdgeLenght, 0,				0, BPP) ||
		!LoadCubeFace(Texture, ZE_CTF_NEGATIVEY, Bits, EdgeLenght, EdgeLenght,		0, BPP) ||
		!LoadCubeFace(Texture, ZE_CTF_POSITIVEY, Bits, EdgeLenght, EdgeLenght * 2,	0, BPP))
	{
		zeError("Cube Texture Resource", "Can not create texture resource. FileName : \"%s\"", ResourceFile->GetFileName());
		delete TextureResource;
		return NULL;		
	}

	FreeImage_Unload(Data);
	TextureResource->SetFileName(ResourceFile->GetFileName());
	TextureResource->Cached = false;
	TextureResource->Shared = false;
	return TextureResource;	
}

const char* ZETextureCubeResource::GetResourceType() const
{
	return "Cube Texture Resource";
}


ZETextureType ZETextureCubeResource::GetTextureType() const
{
	return ZE_TT_CUBE;
}

const ZETextureCube* ZETextureCubeResource::GetTexture() const
{
	return Texture;
}


ZETextureCubeResource::ZETextureCubeResource()
{
	Texture = NULL;
};

ZETextureCubeResource::~ZETextureCubeResource()
{
	if (Texture != NULL)
		Texture->Destroy();
};
