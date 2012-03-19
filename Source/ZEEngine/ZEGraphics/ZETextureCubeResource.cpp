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
	ZEUInt ConstLength = strlen("resources\\") - 1;

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

static void CopyToTextureCube(ZETextureCube* Output, ZETextureData* TextureData)
{
	ZEUInt LevelCount = TextureData->GetTextureLevelCount();
	ZEUInt SurfaceCount = TextureData->GetTextureSurfaceCount();

	// Copy texture data into ZETextureCube
	void* TargetBuffer = NULL;
	ZESize TargetPitch = 0;

	for(ZESize Surface = 0; Surface < SurfaceCount; ++Surface)
	{
		for(ZESize Level = 0; Level < LevelCount; ++Level)
		{
			Output->Lock((ZETextureCubeFace)Surface, Level, &TargetBuffer, &TargetPitch);
			TextureData->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).CopyTo(TargetBuffer, TargetPitch);
			Output->Unlock((ZETextureCubeFace)Surface, Level);
		}
	}
}

static void CopyCubeFaceTo(void* Destination, ZEUInt DestPitch, void* SourceBuffer, ZEUInt SourcePitch, ZEUInt EdgeLenght, ZEUInt OffsetX, ZEUInt OffsetY)
{
	for (ZEUInt I = 0; I < EdgeLenght; I++)
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
		zeError("Texture file not found. FileName : \"%s\"", NewPath.GetValue());
		return NULL;
	}
}

ZETextureCubeResource* ZETextureCubeResource::LoadResource(ZEFile* ResourceFile, const ZETextureOptions* UserOptions)
{
	if(UserOptions == NULL)
		UserOptions = zeGraphics->GetTextureOptions();


	ZEFileCache			FileCache;
	ZETextureOptions	FinalOptions;
	ZETextureData	TempTextureData;
	ZETextureData	ProcessedTextureData;
	ZEString			CachePath = "TextureCache.ZECACHE";

	bool CacheIt			= true;
	bool Process			= true;
	bool CheckCache			= true;
	bool CacheOpen			= false;
	bool Processed			= false;
	bool IdentifierExists	= false;

	// Decide final texture options
	ZETextureQualityManager::GetFinalTextureOptions(&FinalOptions, ResourceFile, UserOptions, 3, 2, ZE_TT_CUBE);
	
	// Create identifier
	ZETextureCacheDataIdentifier Identifier(ResourceFile->GetFilePath(), FinalOptions);

	if(UserOptions->FileCaching != ZE_TFC_DISABLED)
	{
		CacheIt				= true;
		CheckCache			= true;
		CacheOpen			= FileCache.Open(CachePath);
		IdentifierExists	= FileCache.IdentifierExists(&Identifier);
	}
	else
	{
		CacheIt				= false;
		CheckCache			= false;
		CacheOpen			= false;
		IdentifierExists	= false;
	}

	if (CheckCache && CacheOpen && IdentifierExists)
	{
		// If found in cache load from cache directly
		zeLog("Loading from file cache: \"%s\".", ResourceFile->GetFilePath().GetValue());

		ZEPartialFile PartialResourceFile;
		if (!FileCache.OpenData(&PartialResourceFile, &Identifier))
		{
			zeAssert(true, "Cannot open partial file on cache for file: \"%s\".", ResourceFile->GetFilePath().GetValue());
			return NULL;
		}

		// Load into TextureData
		if (!ZETextureLoader::Read(&PartialResourceFile, &ProcessedTextureData))
		{
			zeAssert(true, "Cannot read texture from cache. File: \"%s\".", ResourceFile->GetFilePath().GetValue());
			TempTextureData.DestroyTexture();
			ProcessedTextureData.DestroyTexture();
			return NULL;
		}

		PartialResourceFile.Close();
		FileCache.Close();
		Process = CacheIt = false;

	}
	else // If cache is not used then try to load from ZEPack / ZETextureFile / Image file
	{
		// Load into TextureData
		if (!ZETextureLoader::LoadFromFile(ResourceFile, &ProcessedTextureData))
		{
			zeAssert(true, "Cannot load image from file: \"%s\".", ResourceFile->GetFilePath().GetValue());
			TempTextureData.DestroyTexture();
			return NULL;
		}
	}

	// If not loaded properly
	if (ProcessedTextureData.IsEmpty())
	{
		zeError("Cannot load: \"%s\".", ResourceFile->GetFilePath().GetValue());
		TempTextureData.DestroyTexture();
		return NULL;
	}

	// Process the data
	if(Process)
	{
		zeLog("Processing texture \"%s\".", ResourceFile->GetFilePath().GetValue());
	
		// Convert to 6 surface texture data and clean ProcessedTextureData
		ZETextureData::ConvertToCubeTextureData(&TempTextureData, &ProcessedTextureData);
		ProcessedTextureData.DestroyTexture();

		Processed = ZETextureQualityManager::Process(&ProcessedTextureData, &TempTextureData, &FinalOptions);
		if (!Processed)
		{
			zeCriticalError("Cannot process texture: \"%s\".", ResourceFile->GetFilePath().GetValue());
			ProcessedTextureData.DestroyTexture();
			TempTextureData.DestroyTexture();
			return NULL;
		}
	}

	// Save to cache
	if(CacheIt && CacheOpen && !IdentifierExists)
	{
		ZEPartialFile PartialResourceFile;
		if (!FileCache.Allocate(&PartialResourceFile, &Identifier, ProcessedTextureData.GetSizeOnDisk()))
		{
			zeAssert(true, "Cache allocation failed for file: \"%s\".", ResourceFile->GetFilePath().GetValue());
		}

		if (!ZETextureLoader::Write((ZEFile*)&PartialResourceFile, &ProcessedTextureData))
		{
			zeAssert(true, "Cannot cache the texture: \"%s\".", ResourceFile->GetFilePath().GetValue());
		}

		PartialResourceFile.Close();
		FileCache.Close();
	}
	
	// Create TextureCubeResource 
	ZETextureCubeResource* TextureResource = new ZETextureCubeResource();
	ZETextureCube* Texture = TextureResource->Texture = ZETextureCube::CreateInstance();
	if (Texture == NULL)
	{
		delete TextureResource;
		ProcessedTextureData.DestroyTexture();
		TempTextureData.DestroyTexture();
		return NULL;
	}

	// Set Other Variables
	TextureResource->SetFileName(ResourceFile->GetFilePath().GetValue());
	TextureResource->Cached = false;
	TextureResource->Shared = false;

	// Create the Texture
	if (!Texture->Create(ProcessedTextureData.GetTextureWidth(), ProcessedTextureData.GetTextureLevelCount(), ProcessedTextureData.GetPixelFormat(), false))
	{
		zeError("Can not create texture resource. FileName : \"%s\"", ResourceFile->GetFilePath().GetValue());
		ProcessedTextureData.DestroyTexture();
		TempTextureData.DestroyTexture();
		delete TextureResource;
		return NULL;
	}

	CopyToTextureCube(Texture, &ProcessedTextureData);
	
	ProcessedTextureData.DestroyTexture();
	TempTextureData.DestroyTexture();
	
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




