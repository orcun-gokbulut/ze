//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureCubeResource.cpp
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
#include "ZETextureCube.h"
#include "ZETextureData.h"
#include "ZETextureLoader.h"
#include "ZECore/ZEConsole.h"
#include "ZEGraphicsModule.h"
#include "ZEFile/ZEFileCache.h"
#include "ZETextureCubeResource.h"
#include "ZETextureQualityManager.h"
#include "ZECore/ZEResourceManager.h"
#include "ZETextureCacheDataIdentifier.h"


static ZEString ConstructResourcePath(const ZEString& Path)
{
	ZEString NewString = Path;
	unsigned int ConstLength = strlen("resources\\") - 1;

	if (Path[0] == '\\' || Path[0] == '/')
		NewString = NewString.SubString(1, Path.GetLength() - 1);

	// If it is guaranteed that there is no "resources\\" string in beginning
	if (NewString.GetLength() - 1 < ConstLength)
	{
		NewString.Insert(0, "resources\\");
		return NewString;
	}
	// Else check if there is "resources\\" in the beginning
	else if (_stricmp("resources\\", Path.SubString(0, ConstLength)) != 0)
	{
		NewString.Insert(0, "resources\\");
		return NewString;
	}

	return NewString;
}

static void CopyCubeFaceTo(void* Destination, unsigned int DestPitch, void* SourceBuffer, unsigned int SourcePitch, unsigned int EdgeLenght, unsigned int OffsetX, unsigned int OffsetY)
{
	for (unsigned int I = 0; I < EdgeLenght; I++)
		memcpy((unsigned char*)Destination + (I * DestPitch), (unsigned char*)SourceBuffer + SourcePitch * (OffsetY + I) + OffsetX * 4, EdgeLenght * 4);

}

void ZETextureCubeResource::CacheResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	ZEString NewPath = ConstructResourcePath(FileName);
	ZETextureCubeResource* NewResource = (ZETextureCubeResource*)zeResources->GetResource(NewPath);
	
	if (NewResource == NULL)
	{
		if(UserOptions == NULL)
			UserOptions = zeGraphics->GetTextureOptions();

		NewResource = LoadResource(NewPath, UserOptions);
		if (NewResource != NULL)
		{
			NewResource->Cached = true;
			NewResource->ReferenceCount = 0;
			zeResources->AddResource(NewResource);
		}
	}
}

ZETextureCubeResource* ZETextureCubeResource::LoadSharedResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	ZEString NewPath = ConstructResourcePath(FileName);
	ZETextureCubeResource* NewResource =(ZETextureCubeResource*)zeResources->GetResource(NewPath.GetValue());
	
	if (NewResource == NULL)
	{
		if(UserOptions == NULL)
			UserOptions = zeGraphics->GetTextureOptions();

		NewResource = LoadResource(NewPath, UserOptions);
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

ZETextureCubeResource* ZETextureCubeResource::LoadResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	ZETextureCubeResource* TextureResource;
	ZEString NewPath = ConstructResourcePath(FileName);
	

	ZEFile* File = ZEFile::Open(NewPath);
	if (File != NULL && File->IsOpen())
	{
		if(UserOptions == NULL)
			UserOptions = zeGraphics->GetTextureOptions();

		TextureResource = LoadResource(File, UserOptions);
		File->Close();
		delete File;

		return TextureResource;
	}
	else
	{
		zeError("Cube Texture Resource", "Texture file not found. FileName : \"%s\"", NewPath.GetValue());
		return NULL;
	}
}

ZETextureCubeResource* ZETextureCubeResource::LoadResource(ZEFile* ResourceFile, const ZETextureOptions* UserOptions)
{
	if(UserOptions == NULL)
		UserOptions = zeGraphics->GetTextureOptions();

	ZETextureData		TextureData;
	ZETextureOptions	FinalOptions;
	ZEFileCache			FileCache;
	ZEString			CachePath = "TextureCache.ZECACHE";

	bool CheckCache		= true;
	bool CacheIt		= true;
	bool Process		= true;

	// Decide final texture options
	ZETextureQualityManager::GetFinalTextureOptions(&FinalOptions, ResourceFile, UserOptions);

	if(UserOptions->FileCaching != ZE_TFC_DISABLED && zeGraphics->GetTextureQuality() != ZE_TQ_VERY_HIGH)
		CheckCache = CacheIt = true;
	else
		CheckCache = CacheIt = false;


	// Create identifier and check FileCache first
	ZETextureCacheDataIdentifier Identifier(ResourceFile->GetFilePath(), FinalOptions);
	if(CheckCache && FileCache.Open(CachePath) && FileCache.IdentifierExists(&Identifier))
	{
		// If found in cache load from cache directly
		zeLog("Cube Texture Resource", "Loading from file cache: \"%s\".", ResourceFile->GetFilePath().GetValue());

		ZEPartialFile PartialResourceFile;
		FileCache.OpenData(&PartialResourceFile, &Identifier);
		ZETextureLoader::Read(&PartialResourceFile, &TextureData);
		PartialResourceFile.Close();
		FileCache.Close();
		Process = CacheIt = false;

	}
	else // if it is not cache try to load from ZEPack / ZETextureFile / Image file
	{
		ZETextureLoader::LoadFromFile(ResourceFile, &TextureData);
	}

	if(TextureData.IsEmpty())
	{
		zeError("Cube Texture Resource", "Cannot load: \"%s\".", ResourceFile->GetFilePath().GetValue());
		TextureData.DestroyTexture();
		return NULL;
	}

	if(TextureData.GetDepth() == 1)
	{
		// All the 6 texture data is in one surface
		// Convert it to 6 surface TextureData

		ZETexturePixelFormat PixelFormat = TextureData.GetPixelFormat();
		unsigned int MipmapCount = TextureData.GetMipmapCount();
		unsigned int RowSize = TextureData.GetMipmapRowSize(0, 0) / 3;
		unsigned int RowCount = TextureData.GetMipmapRowCount(0, 0) / 2;
		unsigned int Width = TextureData.GetWidth() / 3;
		unsigned int Height = TextureData.GetHeight() / 2;
		unsigned int Depth = 6; 

		// Check if texture dimensions are right
		if (Width != Height)
		{
			zeError("Cube Texture Resource", "File does not have correct dimensions. (FileName : \"%s\")", ResourceFile->GetFilePath().GetValue());
			TextureData.DestroyTexture();
			return NULL;
		}

		// Copy Texture data
		void* Buffer = malloc(TextureData.GetMipmapDataSize(0, 0));
		unsigned int BufferPitch = TextureData.GetMipmapRowSize(0, 0);
		TextureData.CopyMipmapDataTo(0, 0, Buffer, BufferPitch);

		TextureData.DestroyTexture();
		TextureData.CreateTexture(PixelFormat, Depth, MipmapCount, Width, Height);

		unsigned int Offset = Width;

		// Surf0 Mip0 = +X
		TextureData.AllocateMipmap(0, 0, RowSize, RowCount);
		CopyCubeFaceTo(TextureData.GetMipmapData(0, 0), RowSize, Buffer, BufferPitch, Width, 2 * Width, 0);

		// Surf1 Mip0 = -X
		TextureData.AllocateMipmap(1, 0, RowSize, RowCount);
		CopyCubeFaceTo(TextureData.GetMipmapData(1, 0), RowSize, Buffer, BufferPitch, Width, 0 * Width, 0);

		// Surf2 Mip0 = +Y
		TextureData.AllocateMipmap(2, 0, RowSize, RowCount);
		CopyCubeFaceTo(TextureData.GetMipmapData(2, 0), RowSize, Buffer, BufferPitch, Width, 2 * Width, Width);

		// Surf3 Mip0 = -Y
		TextureData.AllocateMipmap(3, 0, RowSize, RowCount);
		CopyCubeFaceTo(TextureData.GetMipmapData(3, 0), RowSize, Buffer, BufferPitch, Width, 1 * Width, Width);

		// Surf4 Mip0 = +Z
		TextureData.AllocateMipmap(4, 0, RowSize, RowCount);
		CopyCubeFaceTo(TextureData.GetMipmapData(4, 0), RowSize, Buffer, BufferPitch, Width, 1 * Width, 0);

		// Surf5 Mip0 = -Z
		TextureData.AllocateMipmap(5, 0, RowSize, RowCount);
		CopyCubeFaceTo(TextureData.GetMipmapData(5, 0), RowSize, Buffer, BufferPitch, Width, 0 * Width, Width);

		free(Buffer);
	}

	// Process the data
	if(Process)
	{
		zeLog("Cube Texture Resource", "Processing texture \"%s\".", ResourceFile->GetFilePath().GetValue());
		ZETextureQualityManager::Process(&TextureData, &FinalOptions);
	}

	// Save to cache
	if(CacheIt && !FileCache.IdentifierExists(&Identifier))
	{
		ZEPartialFile PartialResourceFile;
		FileCache.Allocate(&PartialResourceFile, &Identifier, TextureData.GetSizeOnDisk());
		ZETextureLoader::Write(&PartialResourceFile, &TextureData);
		PartialResourceFile.Close();
		FileCache.Close();
	}
	
	// Create TextureCubeResource 
	ZETextureCubeResource* TextureResource = new ZETextureCubeResource();
	ZETextureCube* Texture = TextureResource->Texture = ZETextureCube::CreateInstance();
	if (Texture == NULL)
	{
		delete TextureResource;
		TextureData.DestroyTexture();
		return NULL;
	}

	// Set Other Variables
	TextureResource->SetFileName(ResourceFile->GetFilePath().GetValue());
	TextureResource->Cached = false;
	TextureResource->Shared = false;

	// Create the Texture
	if (!Texture->Create(TextureData.GetWidth(), TextureData.GetPixelFormat(), false))
	{
		zeError("Cube Texture Resource", "Can not create texture resource. FileName : \"%s\"", ResourceFile->GetFilePath().GetValue());
		TextureData.DestroyTexture();
		delete TextureResource;
		return NULL;
	}

	// Copy Data to texture
	unsigned int MipCount = TextureData.GetMipmapCount();
	unsigned int SurfaceCount = TextureData.GetDepth();

	// Copy texture data into ZETexture2D
	void* TargetBuffer = NULL;
	unsigned int TargetPitch = 0;

	for(unsigned int I = 0; I < SurfaceCount; I++)
	{
		for(unsigned int J = 0; J < MipCount; J++)
		{
			Texture->Lock((ZETextureCubeFace)I, &TargetBuffer, &TargetPitch);
			TextureData.CopyMipmapDataTo(I, J, (unsigned char*)TargetBuffer, TargetPitch);
			Texture->Unlock((ZETextureCubeFace)I);
		}
	}

	TextureData.DestroyTexture();
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




