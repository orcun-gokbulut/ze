//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETexture3DResource.cpp
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
#include "ZEGraphics/ZETexture3D.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZETexture3DResource.h"
#include "ZECore/ZEResourceManager.h"
#include "ZEFile/ZEFile.h"
#include "ZETextureOptions.h"
#include "ZETextureData.h"
#include "ZEFile/ZEFileCache.h"
#include "ZETextureQualityManager.h"
#include "ZETextureCacheDataIdentifier.h"
#include "ZETextureLoader.h"
#include "ZEMath/ZEMath.h"
#include "ZETextureTools.h"


static ZEString ConstructResourcePath(const ZEString& Path)
{
	ZEString NewString = Path;
	ZESize ConstLength = strlen("resources\\") - 1;

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

static void CopyToTexture3D(ZETexture3D* Texture, ZETextureData* TextureData)
{
	// Copy texture data into ZETexture3D
	void* TargetBuffer	= NULL;
	ZESize SlicePitch	= 0;
	ZESize RowPitch		= 0;

	// Get texture specs
	ZESize SurfaceLevelCount	= (ZESize)TextureData->GetLevelCount();
	ZESize TextureSurfaceCount	= (ZESize)TextureData->GetSurfaceCount();


	for (ZESize Level = 0,  SurfaceIncrement = 1; Level < SurfaceLevelCount; ++Level, SurfaceIncrement *= 2)
	{
		Texture->Lock(&TargetBuffer, &RowPitch, &SlicePitch, (ZEUInt	)Level);

		for (ZESize Surface = 0, SurfaceCopyCount = 0; Surface < TextureSurfaceCount; Surface += SurfaceIncrement, ++SurfaceCopyCount)
		{
			TextureData->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).CopyTo((void*)((ZEUInt8*)TargetBuffer + SlicePitch * SurfaceCopyCount), RowPitch);
		}

		Texture->Unlock((ZEUInt	)Level);
	}

}

const char* ZETexture3DResource::GetResourceType() const
{
	return "Volume Texture Resource";
}

ZETextureType ZETexture3DResource::GetTextureType() const
{
	return ZE_TT_3D;
}

const ZETexture3D* ZETexture3DResource::GetTexture() const
{
	return Texture;
}

ZETexture3DResource::ZETexture3DResource()
{
	Texture = NULL;
}

ZETexture3DResource::~ZETexture3DResource()
{
	if (Texture != NULL)
		Texture->Destroy();
}

ZETexture3DResource* ZETexture3DResource::LoadSharedResource(const ZEString& FileName, ZEUInt HorizTileCount, ZEUInt VertTileCount, const ZETextureOptions* UserOptions)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	ZETexture3DResource* NewResource =(ZETexture3DResource*)zeResources->GetResource(NewPath.GetValue());
	if(NewResource == NULL)
	{		
		if(UserOptions == NULL)
			UserOptions = zeGraphics->GetTextureOptions();

		NewResource = LoadResource(NewPath, HorizTileCount, VertTileCount, UserOptions);
		if(NewResource != NULL)
		{
			NewResource->Shared = true;
			NewResource->Cached = false;
			NewResource->ReferenceCount = 1;
			zeResources->AddResource(NewResource);
			return NewResource;
		}
		else
		{
			zeError("Texture file not found. File Path : \"%s\"", NewPath.GetValue());
			return NULL;
		}
	}
	else
	{
		return NewResource;
	}

	return NULL;
}

void ZETexture3DResource::CacheResource(const ZEString& FileName, ZEUInt HorizTileCount, ZEUInt VertTileCount, const ZETextureOptions* UserOptions)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	ZETexture3DResource* NewResource = (ZETexture3DResource*)zeResources->GetResource(NewPath.GetValue());
	if (NewResource == NULL)
	{
		if(UserOptions == NULL)
			UserOptions = zeGraphics->GetTextureOptions();

		NewResource = LoadResource(NewPath, HorizTileCount, VertTileCount, UserOptions);
		if (NewResource != NULL)
		{
			NewResource->Cached = true;
			NewResource->ReferenceCount = 0;
			zeResources->AddResource(NewResource);
		}
	}
}

ZETexture3DResource* ZETexture3DResource::LoadResource(const ZEString& FileName, ZEUInt HorizTileCount, ZEUInt VertTileCount, const ZETextureOptions* UserOptions)
{
	ZETexture3DResource* TextureResource;
	ZEString NewPath = ConstructResourcePath(FileName);

	ZEFile* File = ZEFile::Open(NewPath, false);
	if(File != NULL && File->IsOpen())
	{
		if(UserOptions == NULL)
			UserOptions = zeGraphics->GetTextureOptions();

		TextureResource = LoadResource(File, HorizTileCount, VertTileCount, UserOptions);
		File->Close();
		delete File;

		return TextureResource;
	}
	else
	{
		zeError("Texture file not found. File Path : \"%s\"", NewPath.GetValue());
		return NULL;
	}

	return NULL;
}

ZETexture3DResource* ZETexture3DResource::LoadResource(ZEFile* ResourceFile, ZEUInt HorizTileCount, ZEUInt VertTileCount, const ZETextureOptions* UserOptions)
{
	if(UserOptions == NULL)
		UserOptions = zeGraphics->GetTextureOptions();

	ZETextureData	TempTextureData;
	ZETextureData	ProcessedTextureData;
	ZETextureOptions	FinalOptions;
	ZEFileCache			FileCache;
	ZEString			CachePath = "TextureCache.ZECACHE";

	bool CacheIt			= true;
	bool Process			= true;
	bool CheckCache			= true;
	bool CacheOpen			= false;
	bool Processed			= false;
	bool IdentifierExists	= false;

	// Decide final texture options
	ZETextureQualityManager::GetFinalTextureOptions(&FinalOptions, ResourceFile, UserOptions, HorizTileCount, VertTileCount, ZE_TT_3D);

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
	
	if(CheckCache && CacheOpen && IdentifierExists)
	{
		// If found in cache load from cache directly
		zeLog("Loading from file cache: \"%s\".", ResourceFile->GetFilePath().GetValue());

		ZEPartialFile PartialResourceFile;
		if (!FileCache.OpenData(&PartialResourceFile, &Identifier))
		{
			zeDebugCheck(true, "Cannot open partial file on cache for file: \"%s\".", ResourceFile->GetFilePath().GetValue());
			return NULL;
		}

		// Load into TextureData
		if (!ZETextureLoader::Read(&PartialResourceFile, &ProcessedTextureData))
		{
			zeDebugCheck(true, "Cannot read texture from cache. File: \"%s\".", ResourceFile->GetFilePath().GetValue());
			ProcessedTextureData.Destroy();
			return NULL;
		}

		PartialResourceFile.Close();
		FileCache.Close();
		Process = CacheIt = false;

	}
	else // if it is not cache try to load from ZEPack / ZETextureFile / Image file
	{
		// Load into TextureData
		if (!ZETextureLoader::LoadFromFile(ResourceFile, &ProcessedTextureData))
		{
			zeDebugCheck(true, "Cannot load image from file: \"%s\".", ResourceFile->GetFilePath().GetValue());
			ProcessedTextureData.Destroy();
			return NULL;
		}
	}

	if(ProcessedTextureData.IsEmpty())
	{
		zeError("Cannot load: \"%s\".", ResourceFile->GetFilePath().GetValue());
		ProcessedTextureData.Destroy();
		return NULL;
	}

	// Process the data
	if(Process)
	{
		zeLog("Processing texture \"%s\".", ResourceFile->GetFilePath().GetValue());

		// Convert to VertTileCount * HorizTileCount tile count surfaced texture data
		ZETextureData::ConvertToVolumeTextureData(&TempTextureData, &ProcessedTextureData, HorizTileCount, VertTileCount);
		ProcessedTextureData.Destroy();

		// Send the unprocessed texture data to process function
		Processed = ZETextureQualityManager::Process(&ProcessedTextureData, &TempTextureData, &FinalOptions);
		if (!Processed)
		{
			zeCriticalError("Cannot process texture: \"%s\".", ResourceFile->GetFilePath().GetValue());
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
			zeDebugCheck(true, "Cache allocation failed for file: \"%s\".", ResourceFile->GetFilePath().GetValue());
		}

		if (!ZETextureLoader::Write((ZEFile*)&PartialResourceFile, &ProcessedTextureData))
		{
			zeDebugCheck(true, "Cannot cache the texture: \"%s\".", ResourceFile->GetFilePath().GetValue());
		}

		PartialResourceFile.Close();
		FileCache.Close();
	}

	// Create Texture3DResource 
	ZETexture3DResource* TextureResource = new ZETexture3DResource();
	ZETexture3D* Texture = TextureResource->Texture = ZETexture3D::CreateInstance();
	if (Texture == NULL)
	{
		delete TextureResource;
		ProcessedTextureData.Destroy();
		TempTextureData.Destroy();
		return NULL;
	}

	// Set Other Variables
	TextureResource->SetFileName(ResourceFile->GetFilePath().GetValue());
	TextureResource->Cached = false;
	TextureResource->Shared = false;

	// Create the Texture
	if (!Texture->Create(ProcessedTextureData.GetWidth(), ProcessedTextureData.GetHeight(), ProcessedTextureData.GetSurfaceCount(), ProcessedTextureData.GetLevelCount(), ProcessedTextureData.GetPixelFormat()))
	{
		zeError("Can not create texture resource. FileName : \"%s\"", ResourceFile->GetFilePath().GetValue());
		ProcessedTextureData.Destroy();
		TempTextureData.Destroy();
		delete TextureResource;
		return NULL;
	}

	CopyToTexture3D(Texture, &ProcessedTextureData);

	TempTextureData.Destroy();
	ProcessedTextureData.Destroy();
	
	return TextureResource;
}
