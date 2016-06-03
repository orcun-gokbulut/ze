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
#include "ZECore/ZEConsole.h"
#include "ZECore/ZEResourceManager.h"
#include "ZEFile/ZEFileCache.h"
#include "ZEPointer/ZEPointer.h"
#include "ZETextureData.h"
#include "ZETextureLoader.h"
#include "ZETextureCubeResource.h"
#include "ZETextureQualityManager.h"
#include "ZETextureCacheDataIdentifier.h"
#include "ZEGraphics/ZEGRTextureCube.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"

void ZETextureCubeResource::CacheResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	ZETextureCubeResource* NewResource = (ZETextureCubeResource*)zeResources->GetResource(FileName);
	
	if (NewResource == NULL)
	{
		if(UserOptions == NULL)
			UserOptions = ZEGRGraphicsModule::GetInstance()->GetTextureOptions();

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
			UserOptions = ZEGRGraphicsModule::GetInstance()->GetTextureOptions();

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
			UserOptions = ZEGRGraphicsModule::GetInstance()->GetTextureOptions();

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
	if (UserOptions == NULL)
		UserOptions = ZEGRGraphicsModule::GetInstance()->GetTextureOptions();

	ZETextureData TextureDataCube;
	ZETextureOptions FinalOptions;
	ZEFileCache FileCache;
	ZEString CachePath = "#S:/Caches/TextureCache.ZECache";

	bool CacheIt			= true;
	bool Process			= true;
	bool CheckCache			= true;
	bool CacheOpen			= false;
	bool Processed			= false;
	bool IdentifierExists	= false;

	ZETextureQualityManager::GetFinalTextureOptions(&FinalOptions, ResourceFile, UserOptions, 3, 2, ZEGR_TT_CUBE);
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
		zeLog("Loading from file cache: \"%s\".", ResourceFile->GetPath().GetValue());

		ZEPartialFile PartialResourceFile;
		if (!FileCache.OpenData(&PartialResourceFile, &Identifier))
		{
			zeDebugCheck(true, "Cannot open partial file on cache for file: \"%s\".", ResourceFile->GetPath().GetValue());
			return NULL;
		}

		if (!ZETextureLoader::Read(&PartialResourceFile, &TextureDataCube))
		{
			zeDebugCheck(true, "Cannot read texture from cache. File: \"%s\".", ResourceFile->GetPath().GetValue());
			TextureDataCube.Destroy();
			return NULL;
		}

		PartialResourceFile.Close();
		FileCache.Close();
		Process = CacheIt = false;

	}
	else
	{
		ZETextureData TextureData;
		if (!ZETextureLoader::LoadFromFile(ResourceFile, &TextureData))
		{
			zeDebugCheck(true, "Cannot load image from file: \"%s\".", ResourceFile->GetPath().GetValue());
			TextureData.Destroy();
			return NULL;
		}

		ZETextureData::ConvertToCubeTextureData(&TextureDataCube, &TextureData);
		TextureData.Destroy();
	}

	if (TextureDataCube.IsEmpty())
	{
		zeError("Cannot load: \"%s\".", ResourceFile->GetPath().GetValue());
		TextureDataCube.Destroy();
		return NULL;
	}

	if(CacheIt && CacheOpen && !IdentifierExists)
	{
		ZEPartialFile PartialResourceFile;
		if (!FileCache.Allocate(&PartialResourceFile, &Identifier, TextureDataCube.GetSizeOnDisk()))
		{
			zeDebugCheck(true, "Cache allocation failed for file: \"%s\".", ResourceFile->GetPath().GetValue());
		}

		if (!ZETextureLoader::Write((ZEFile*)&PartialResourceFile, &TextureDataCube))
		{
			zeDebugCheck(true, "Cannot cache the texture: \"%s\".", ResourceFile->GetPath().GetValue());
		}

		PartialResourceFile.Close();
		FileCache.Close();
	}

	ZEUInt Length = TextureDataCube.GetWidth();
	ZEUInt PixelSize = ZEGRFormatDefinition::GetDefinition(TextureDataCube.GetPixelFormat())->BlockSize;

	ZEBYTE* Data = new ZEBYTE[Length * Length * PixelSize * 6];
	ZESize RowPitch = Length * PixelSize;
	ZESize SlicePitch = RowPitch * Length;

	for (ZESize Surface = 0; Surface < 6; Surface++)
		TextureDataCube.GetSurfaces()[Surface].GetLevels()[0].CopyTo(Data + Surface * SlicePitch, RowPitch);

	// Create TextureCubeResource 
	ZEPointer<ZETextureCubeResource, ZEDeletorRelease<ZETextureCubeResource>> TextureResource = new ZETextureCubeResource();
	TextureResource->Texture = ZEGRTextureCube::CreateInstance(
															TextureDataCube.GetWidth(), 
															TextureDataCube.GetLevelCount(), 
															TextureDataCube.GetPixelFormat(), 
															ZEGR_RU_GPU_READ_ONLY, 
															ZEGR_RBF_SHADER_RESOURCE, 
															Data);

	if (TextureResource->Texture == NULL)
	{
		zeError("Can not create texture resource. FileName : \"%s\"", ResourceFile->GetPath().GetValue());
		TextureDataCube.Destroy();
		delete[] Data;
		return NULL;
	}

	delete[] Data;

	TextureResource->SetFileName(ResourceFile->GetPath().GetValue());
	TextureResource->Cached = false;
	TextureResource->Shared = false;
	
	TextureDataCube.Destroy();

	return TextureResource.Transfer();
}

const char* ZETextureCubeResource::GetResourceType() const
{
	return "Cube Texture Resource";
}

ZEGRTextureType ZETextureCubeResource::GetTextureType() const
{
	return ZEGR_TT_CUBE;
}

ZEGRTexture* ZETextureCubeResource::GetTexture() const
{
	return Texture;
}

ZEGRTextureCube* ZETextureCubeResource::GetTextureCube() const
{
	return Texture;
}

ZETextureCubeResource::ZETextureCubeResource()
{
	Texture = NULL;
};

ZETextureCubeResource::~ZETextureCubeResource()
{

};
