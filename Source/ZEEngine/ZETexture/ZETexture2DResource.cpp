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

#include "ZECore/ZEConsole.h"
#include "ZECore/ZEResourceManager.h"
#include "ZEError.h"
#include "ZEFile/ZEFile.h"
#include "ZEFile/ZEFileCache.h"
#include "ZETextureData.h"
#include "ZETextureLoader.h"
#include "ZETextureQualityManager.h"
#include "ZETextureCacheDataIdentifier.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRTexture.h"

#include "ZEPointer/ZEPointer.h"
#include "ZEMath/ZEMath.h"

const char* ZETexture2DResource::GetResourceType() const
{
	return "Texture Resource";
}

ZEGRTextureType ZETexture2DResource::GetTextureType() const
{
	return ZEGR_TT_2D;
}

ZEGRTexture* ZETexture2DResource::GetTexture() const
{
	return Texture;
}

ZEGRTexture* ZETexture2DResource::GetTexture2D() const
{
	return Texture;
}

ZETexture2DResource::ZETexture2DResource()
{

}

ZETexture2DResource::~ZETexture2DResource()
{

}

ZETexture2DResource* ZETexture2DResource::LoadSharedResource(const ZEString& FileName, const ZETextureOptions* UserOptions, bool sRGB)
{	
	ZETexture2DResource* NewResource = (ZETexture2DResource*)zeResources->GetResource(FileName.GetValue());
	if(NewResource == NULL)
	{		
		if(UserOptions == NULL)
			UserOptions = ZEGRGraphicsModule::GetInstance()->GetTextureOptions();

		NewResource = LoadResource(FileName, UserOptions, sRGB);
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
}

void ZETexture2DResource::CacheResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	ZETexture2DResource* NewResource = (ZETexture2DResource*)zeResources->GetResource(FileName.GetValue());
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

ZETexture2DResource* ZETexture2DResource::LoadResource(const ZEString& FileName, const ZETextureOptions* UserOptions, bool sRGB)
{
	ZETexture2DResource* TextureResource;
	bool Result;
	ZEFile File;
	Result = File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE);
	if(Result)
	{
		if(UserOptions == NULL)
			UserOptions = ZEGRGraphicsModule::GetInstance()->GetTextureOptions();

		TextureResource = LoadResource(&File, UserOptions, sRGB);
		File.Close();

		return TextureResource;
	}
	else
	{
		zeError("Texture file not found. File Path : \"%s\"", FileName.GetValue());
		return NULL;
	}
}

ZETexture2DResource* ZETexture2DResource::LoadResource(ZEFile* ResourceFile, const ZETextureOptions* UserOptions, bool sRGB)
{
	if (UserOptions == NULL)
		UserOptions = ZEGRGraphicsModule::GetInstance()->GetTextureOptions();

	ZETextureData TextureData;
	ZETextureOptions FinalOptions;
	ZEFileCache FileCache;
	ZEString CachePath = "#S:/Caches/TextureCache.ZECache";
	
	bool CacheIt			= true;
	bool CheckCache			= true;
	bool CacheOpen			= false;
	bool IdentifierExists	= false;

	ZETextureQualityManager::GetFinalTextureOptions(&FinalOptions, ResourceFile, UserOptions, 1, 1, ZEGR_TT_2D);
	ZETextureCacheDataIdentifier Identifier(ResourceFile->GetPath(), FinalOptions);

	if (UserOptions->FileCaching != ZE_TFC_DISABLED)
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
		if (!ZETextureLoader::Read(&PartialResourceFile, &TextureData))
		{
			zeDebugCheck(true, "Cannot read texture from cache. File: \"%s\".", ResourceFile->GetPath().GetValue());
			TextureData.Destroy();
			return NULL;
		}
		
		PartialResourceFile.Close();
		FileCache.Close();
		CacheIt = false;
	}
	else // If cache is not used then try to load from ZEPack / ZETextureFile / Image file
	{
		if (!ZETextureLoader::LoadFromFile(ResourceFile, &TextureData))
		{
			zeDebugCheck(true, "Cannot load image from file: \"%s\".", ResourceFile->GetPath().GetValue());
			TextureData.Destroy();
			return NULL;
		}
	}

	if (TextureData.IsEmpty())
	{
		zeError("Cannot load: \"%s\".", ResourceFile->GetPath().GetValue());
		TextureData.Destroy();
		return NULL;
	}

	// Save to cache
	if (CacheIt && CacheOpen && !IdentifierExists)
	{
		ZEPartialFile PartialResourceFile;
		if (!FileCache.Allocate(&PartialResourceFile, &Identifier, TextureData.GetSizeOnDisk()))
		{
			zeDebugCheck(true, "Cache allocation failed for file: \"%s\".", ResourceFile->GetPath().GetValue());
		}

		if (!ZETextureLoader::Write((ZEFile*)&PartialResourceFile, &TextureData))
		{
			zeDebugCheck(true, "Cannot cache the texture: \"%s\".", ResourceFile->GetPath().GetValue());
		}

		PartialResourceFile.Close();
		FileCache.Close();
	}

	ZEGRFormat Format = sRGB ? ZEGR_TF_R8G8B8A8_UNORM_SRGB : ZEGR_TF_R8G8B8A8_UNORM;

	ZEPointer<ZETexture2DResource, ZEDeletorRelease<ZETexture2DResource>> TextureResource = new ZETexture2DResource();	
	TextureResource->Texture = ZEGRTexture::CreateResource(ZEGR_TT_2D, TextureData.GetWidth(), TextureData.GetHeight(), FinalOptions.MaximumMipmapLevel, Format, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE);
	if (TextureResource->Texture == NULL)
	{
		zeError("Can not create texture resource. FileName : \"%s\"", ResourceFile->GetPath().GetValue());
		TextureData.Destroy();
		return NULL;
	}

	ZETextureLevel& TextureLevel = TextureData.GetSurfaces()[0].GetLevels()[0];
	TextureResource->Texture->Update(TextureLevel.GetData(), TextureLevel.GetPitch());

	// Set Other Variables
	TextureResource->SetFileName(ResourceFile->GetPath());
	TextureResource->Cached = false;
	TextureResource->Shared = false;

	TextureData.Destroy();

	return TextureResource.Transfer();
}
