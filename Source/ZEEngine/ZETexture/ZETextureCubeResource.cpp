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
#include "ZEGraphics/ZEGRTextureCube.h"
#include "ZETextureData.h"
#include "ZETextureLoader.h"
#include "ZECore/ZEConsole.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEFile/ZEFileCache.h"
#include "ZETextureCubeResource.h"
#include "ZETextureQualityManager.h"
#include "ZECore/ZEResourceManager.h"
#include "ZETextureCacheDataIdentifier.h"


static void CopyToTextureCube(ZEGRTextureCube* Output, ZETextureData* TextureData)
{
	ZEUInt LevelCount = TextureData->GetLevelCount();
	ZEUInt SurfaceCount = TextureData->GetSurfaceCount();

	// Copy texture data into ZETextureCube
	void* TargetBuffer = NULL;
	ZESize TargetPitch = 0;

	for(ZESize Surface = 0; Surface < (ZESize)SurfaceCount; ++Surface)
	{
		for(ZESize Level = 0; Level < (ZESize)LevelCount; ++Level)
		{
			Output->Lock((ZETextureCubeFace)Surface, (ZEUInt)Level, &TargetBuffer, &TargetPitch);
			TextureData->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).CopyTo(TargetBuffer, TargetPitch);
			Output->Unlock((ZETextureCubeFace)Surface, (ZEUInt)Level);
		}
	}
}

static void CopyCubeFaceTo(void* Destination, ZESize DestPitch, void* SourceBuffer, ZESize SourcePitch, ZEUInt EdgeLenght, ZEUInt OffsetX, ZEUInt OffsetY)
{
	for (ZESize I = 0; I < (ZESize)EdgeLenght; I++)
		memcpy((unsigned char*)Destination + (I * DestPitch), (unsigned char*)SourceBuffer + SourcePitch * ((ZESize)OffsetY + I) + (ZESize)OffsetX * 4, (ZESize)EdgeLenght * 4);

}

void ZETextureCubeResource::CacheResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	ZETextureCubeResource* NewResource = (ZETextureCubeResource*)zeResources->GetResource(FileName);
	
	if (NewResource == NULL)
	{
		if(UserOptions == NULL)
			UserOptions = zeGraphics->GetTextureOptions();

		NewResource = LoadResource(FileName, UserOptions);
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
	ZETextureCubeResource* NewResource =(ZETextureCubeResource*)zeResources->GetResource(FileName.GetValue());
	
	if (NewResource == NULL)
	{
		if(UserOptions == NULL)
			UserOptions = zeGraphics->GetTextureOptions();

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

ZETextureCubeResource* ZETextureCubeResource::LoadResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	ZETextureCubeResource* TextureResource;

	bool Result;
	ZEFile File;
	Result = File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE);
	if (Result)
	{
		if(UserOptions == NULL)
			UserOptions = zeGraphics->GetTextureOptions();

		TextureResource = LoadResource(&File, UserOptions);
		File.Close();
		
		return TextureResource;
	}
	else
	{
		zeError("Texture file not found. FileName : \"%s\"", FileName.GetValue());
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
	ZEString			CachePath = "#S:/Caches/TextureCache.ZECache";

	bool CacheIt			= true;
	bool Process			= true;
	bool CheckCache			= true;
	bool CacheOpen			= false;
	bool Processed			= false;
	bool IdentifierExists	= false;

	// Decide final texture options
	ZETextureQualityManager::GetFinalTextureOptions(&FinalOptions, ResourceFile, UserOptions, 3, 2, ZEGR_TT_CUBE);
	
	// Create identifier
	ZETextureCacheDataIdentifier Identifier(ResourceFile->GetPath(), FinalOptions);

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
		zeLog("Loading from file cache: \"%s\".", ResourceFile->GetPath().GetValue());

		ZEPartialFile PartialResourceFile;
		if (!FileCache.OpenData(&PartialResourceFile, &Identifier))
		{
			zeDebugCheck(true, "Cannot open partial file on cache for file: \"%s\".", ResourceFile->GetPath().GetValue());
			return NULL;
		}

		// Load into TextureData
		if (!ZETextureLoader::Read(&PartialResourceFile, &ProcessedTextureData))
		{
			zeDebugCheck(true, "Cannot read texture from cache. File: \"%s\".", ResourceFile->GetPath().GetValue());
			TempTextureData.Destroy();
			ProcessedTextureData.Destroy();
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
			zeDebugCheck(true, "Cannot load image from file: \"%s\".", ResourceFile->GetPath().GetValue());
			TempTextureData.Destroy();
			return NULL;
		}
	}

	// If not loaded properly
	if (ProcessedTextureData.IsEmpty())
	{
		zeError("Cannot load: \"%s\".", ResourceFile->GetPath().GetValue());
		TempTextureData.Destroy();
		return NULL;
	}

	// Process the data
	if(Process)
	{
		zeLog("Processing texture \"%s\".", ResourceFile->GetPath().GetValue());
	
		// Convert to 6 surface texture data and clean ProcessedTextureData
		ZETextureData::ConvertToCubeTextureData(&TempTextureData, &ProcessedTextureData);
		ProcessedTextureData.Destroy();

		Processed = ZETextureQualityManager::Process(&ProcessedTextureData, &TempTextureData, &FinalOptions);
		if (!Processed)
		{
			zeCriticalError("Cannot process texture: \"%s\".", ResourceFile->GetPath().GetValue());
			ProcessedTextureData.Destroy();
			TempTextureData.Destroy();
			return NULL;
		}
	}

	// Save to cache
	if(CacheIt && CacheOpen && !IdentifierExists)
	{
		ZEPartialFile PartialResourceFile;
		if (!FileCache.Allocate(&PartialResourceFile, &Identifier, ProcessedTextureData.GetSizeOnDisk()))
		{
			zeDebugCheck(true, "Cache allocation failed for file: \"%s\".", ResourceFile->GetPath().GetValue());
		}

		if (!ZETextureLoader::Write((ZEFile*)&PartialResourceFile, &ProcessedTextureData))
		{
			zeDebugCheck(true, "Cannot cache the texture: \"%s\".", ResourceFile->GetPath().GetValue());
		}

		PartialResourceFile.Close();
		FileCache.Close();
	}
	
	// Create TextureCubeResource 
	ZETextureCubeResource* TextureResource = new ZETextureCubeResource();
	ZEGRTextureCube* Texture = TextureResource->Texture = ZEGRTextureCube::CreateInstance();
	if (Texture == NULL)
	{
		delete TextureResource;
		ProcessedTextureData.Destroy();
		TempTextureData.Destroy();
		return NULL;
	}

	// Set Other Variables
	TextureResource->SetFileName(ResourceFile->GetPath().GetValue());
	TextureResource->Cached = false;
	TextureResource->Shared = false;

	// Create the Texture
	if (!Texture->Create(ProcessedTextureData.GetWidth(), ProcessedTextureData.GetLevelCount(), ProcessedTextureData.GetPixelFormat(), false))
	{
		zeError("Can not create texture resource. FileName : \"%s\"", ResourceFile->GetPath().GetValue());
		ProcessedTextureData.Destroy();
		TempTextureData.Destroy();
		delete TextureResource;
		return NULL;
	}

	CopyToTextureCube(Texture, &ProcessedTextureData);
	
	ProcessedTextureData.Destroy();
	TempTextureData.Destroy();
	
	return TextureResource;
}

const char* ZETextureCubeResource::GetResourceType() const
{
	return "Cube Texture Resource";
}


ZEGRTextureType ZETextureCubeResource::GetTextureType() const
{
	return ZEGR_TT_CUBE;
}

const ZEGRTextureCube* ZETextureCubeResource::GetTexture() const
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
