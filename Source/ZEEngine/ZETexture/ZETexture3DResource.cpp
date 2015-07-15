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
#include "ZEGraphics/ZEGRTexture3D.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
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

static void CopyToTexture3D(ZEGRTexture3D* Texture, ZETextureData* TextureData)
{
	// Copy texture data into ZETexture3D
	void* TargetBuffer	= NULL;
	ZESize SlicePitch	= 0;
	ZESize RowPitch		= 0;

	// Get texture specs
	ZESize SurfaceLevelCount	= (ZESize)TextureData->GetLevelCount();
	ZESize TextureSurfaceCount	= (ZESize)TextureData->GetSurfaceCount();
	
	/*for (ZESize Level = 0,  SurfaceIncrement = 1; Level < SurfaceLevelCount; ++Level, SurfaceIncrement *= 2)
	{
		Texture->Lock(&TargetBuffer, &RowPitch, &SlicePitch, (ZEUInt)Level);

		for (ZESize Surface = 0, SurfaceCopyCount = 0; Surface < TextureSurfaceCount; Surface += SurfaceIncrement, ++SurfaceCopyCount)
		{
			TextureData->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).CopyTo((void*)((ZEUInt8*)TargetBuffer + SlicePitch * SurfaceCopyCount), RowPitch);
		}

		Texture->Unlock((ZEUInt	)Level);
	}*/
}

const char* ZETexture3DResource::GetResourceType() const
{
	return "Volume Texture Resource";
}

ZEGRTextureType ZETexture3DResource::GetTextureType() const
{
	return ZEGR_TT_3D;
}

ZEGRTexture* ZETexture3DResource::GetTexture() const
{
	return Texture;
}

ZEGRTexture3D* ZETexture3DResource::GetTexture3D() const
{
	return Texture;
}

ZETexture3DResource::ZETexture3DResource()
{
	Texture = NULL;
}

ZETexture3DResource::~ZETexture3DResource()
{

}

ZETexture3DResource* ZETexture3DResource::LoadSharedResource(const ZEString& FileName, ZEUInt HorizTileCount, ZEUInt VertTileCount, const ZETextureOptions* UserOptions)
{
	ZETexture3DResource* NewResource =(ZETexture3DResource*)zeResources->GetResource(FileName.GetValue());
	if(NewResource == NULL)
	{		
		if(UserOptions == NULL)
			UserOptions = ZEGRGraphicsModule::GetInstance()->GetTextureOptions();

		NewResource = LoadResource(FileName, HorizTileCount, VertTileCount, UserOptions);
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
			zeError("Texture file not found. File Path : \"%s\"", FileName.GetValue());
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
	ZETexture3DResource* NewResource = (ZETexture3DResource*)zeResources->GetResource(FileName.GetValue());
	if (NewResource == NULL)
	{
		if(UserOptions == NULL)
			UserOptions = ZEGRGraphicsModule::GetInstance()->GetTextureOptions();

		NewResource = LoadResource(FileName, HorizTileCount, VertTileCount, UserOptions);
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
	bool Result;
	ZEFile File;
	ZETexture3DResource* TextureResource;
	Result = File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE);
	if(Result)
	{
		if(UserOptions == NULL)
			UserOptions = ZEGRGraphicsModule::GetInstance()->GetTextureOptions();

		TextureResource = LoadResource(&File, HorizTileCount, VertTileCount, UserOptions);
		File.Close();

		return TextureResource;
	}
	else
	{
		zeError("Texture file not found. File Path : \"%s\"", FileName.GetValue());
		return NULL;
	}

	return NULL;
}

ZETexture3DResource* ZETexture3DResource::LoadResource(ZEFile* ResourceFile, ZEUInt HorizTileCount, ZEUInt VertTileCount, const ZETextureOptions* UserOptions)
{
	if(UserOptions == NULL)
		UserOptions = ZEGRGraphicsModule::GetInstance()->GetTextureOptions();

	ZETextureData	TempTextureData;
	ZETextureData	ProcessedTextureData;
	ZETextureOptions	FinalOptions;
	ZEFileCache			FileCache;
	ZEString			CachePath = "#S:/Caches/TextureCache.ZECache";

	bool CacheIt			= true;
	bool Process			= true;
	bool CheckCache			= true;
	bool CacheOpen			= false;
	bool Processed			= false;
	bool IdentifierExists	= false;

	// Decide final texture options
	ZETextureQualityManager::GetFinalTextureOptions(&FinalOptions, ResourceFile, UserOptions, HorizTileCount, VertTileCount, ZEGR_TT_3D);

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
	
	if(CheckCache && CacheOpen && IdentifierExists)
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
			zeDebugCheck(true, "Cannot load image from file: \"%s\".", ResourceFile->GetPath().GetValue());
			ProcessedTextureData.Destroy();
			return NULL;
		}
	}

	if(ProcessedTextureData.IsEmpty())
	{
		zeError("Cannot load: \"%s\".", ResourceFile->GetPath().GetValue());
		ProcessedTextureData.Destroy();
		return NULL;
	}

	// Process the data
	if(Process)
	{
		zeLog("Processing texture \"%s\".", ResourceFile->GetPath().GetValue());

		// Convert to VertTileCount * HorizTileCount tile count surfaced texture data
		ZETextureData::ConvertToVolumeTextureData(&TempTextureData, &ProcessedTextureData, HorizTileCount, VertTileCount);
		ProcessedTextureData.Destroy();

		// Send the unprocessed texture data to process function
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

	// Create Texture3DResource 
	ZETexture3DResource* TextureResource = new ZETexture3DResource();
	ZEGRTexture3D* Texture = TextureResource->Texture = ZEGRTexture3D::Create(ProcessedTextureData.GetWidth(), ProcessedTextureData.GetHeight(), ProcessedTextureData.GetSurfaceCount(), ProcessedTextureData.GetLevelCount(), ProcessedTextureData.GetPixelFormat());
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

	CopyToTexture3D(Texture, &ProcessedTextureData);

	TempTextureData.Destroy();
	ProcessedTextureData.Destroy();
	
	return TextureResource;
}
