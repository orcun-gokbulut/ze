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

#include "ZEError.h"
#include "ZETexture2D.h"
#include "ZETextureData.h"
#include "ZEFile/ZEFile.h"
#include "ZECore/ZEConsole.h"
#include "ZEFile/ZEFileCache.h"
#include "ZETexture2DResource.h"
#include "ZETextureQualityManager.h"
#include "ZECore/ZEResourceManager.h"
#include "ZEGraphics/ZETextureLoader.h"
#include "ZEGraphics/ZETextureCacheDataIdentifier.h"


#include <sys/stat.h>
#include <stdio.h>
#include <string.h>


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

ZETexture2DResource* ZETexture2DResource::LoadSharedResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{	
	ZEString NewPath = ConstructResourcePath(FileName);

	ZETexture2DResource* NewResource =(ZETexture2DResource*)zeResources->GetResource(NewPath.GetValue());
	if(NewResource == NULL)
	{		
		if(UserOptions == NULL)
			UserOptions = zeGraphics->GetTextureOptions();

		NewResource = LoadResource(NewPath, UserOptions);
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
}

void ZETexture2DResource::CacheResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	ZETexture2DResource* NewResource = (ZETexture2DResource*)zeResources->GetResource(NewPath.GetValue());
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

ZETexture2DResource* ZETexture2DResource::LoadResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	ZETexture2DResource* TextureResource;
	ZEString NewPath = ConstructResourcePath(FileName);
	
	ZEFile* File = ZEFile::Open(NewPath);
	if(File != NULL && File->IsOpen())
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
		zeError("Texture file not found. File Path : \"%s\"", NewPath.GetValue());
		return NULL;
	}
}

ZETexture2DResource* ZETexture2DResource::LoadResource(ZEFile* ResourceFile, const ZETextureOptions* UserOptions)
{
	if(UserOptions == NULL)
		UserOptions = zeGraphics->GetTextureOptions();

	ZETextureData		TextureData;
	ZETextureOptions	FinalOptions;
	ZEFileCache			FileCache;
	ZEString			CachePath = "TextureCache.ZECACHE";

	bool Open		= false;
	bool Result		= false;
	bool IdentifierExists = false;

	bool CheckCache	= true;
	bool CacheIt	= true;
	bool Process	= true;

	// Decide final texture options
	if (!ZETextureQualityManager::GetFinalTextureOptions(&FinalOptions, ResourceFile, UserOptions))
	{	
		zeAssert(true, "Cannot get final texture options for file: \"%s\".", ResourceFile->GetFilePath().GetValue());
		return NULL;
	}
	
	if(UserOptions->FileCaching != ZE_TFC_DISABLED && zeGraphics->GetTextureQuality() != ZE_TQ_VERY_HIGH)
		CheckCache = CacheIt = true;
	else
		CheckCache = CacheIt = false;

	// Create identifier and check FileCache first
	ZETextureCacheDataIdentifier Identifier(ResourceFile->GetFilePath(), FinalOptions);
	Open = FileCache.Open(CachePath);
	IdentifierExists = FileCache.IdentifierExists(&Identifier);
	
	if(CheckCache && Open && IdentifierExists)
	{
		ZEPartialFile PartialResourceFile;
		if (!FileCache.OpenData(&PartialResourceFile, &Identifier))
		{
			zeAssert(true, "Cannot open partial file on cache for file: \"%s\".", ResourceFile->GetFilePath().GetValue());
			return NULL;
		}
		
		if (!ZETextureLoader::Read(&PartialResourceFile, &TextureData))
		{
			zeAssert(true, "Cannot read texture from cache. File: \"%s\".", ResourceFile->GetFilePath().GetValue());
			TextureData.DestroyTexture();
			return NULL;
		}
		
		PartialResourceFile.Close();
		FileCache.Close();
		Process = CacheIt = false;
	}
	else // if it is not cache try to load from ZEPack / ZETextureFile / Image file
	{
		if (!ZETextureLoader::LoadFromFile(ResourceFile, &TextureData))
		{
			zeAssert(true, "Cannot load image from file: \"%s\".", ResourceFile->GetFilePath().GetValue());
			TextureData.DestroyTexture();
			return NULL;
		}
		
	}

	if(TextureData.IsEmpty())
	{
		zeError("Cannot load because of an unknown error: \"%s\".", ResourceFile->GetFilePath().GetValue());
		TextureData.DestroyTexture();
		return NULL;
	}
	
	// Process the data
	if(Process)
	{
		zeLog("Processing texture \"%s\".", ResourceFile->GetFilePath().GetValue());
		if (!ZETextureQualityManager::Process(&TextureData, &FinalOptions))
		{
			zeAssert(true, "Cannot process texture: \"%s\".", ResourceFile->GetFilePath().GetValue());
			TextureData.DestroyTexture();
			return NULL;
		}
		
	}

	// Save to cache
	// Do not stop if an error occurs, just do not save it to cache
	if(CacheIt && Open && !IdentifierExists)
	{
		ZEPartialFile PartialResourceFile;
		if (!FileCache.Allocate(&PartialResourceFile, &Identifier, TextureData.GetSizeOnDisk()))
		{
			zeAssert(true, "Cache allocation failed for file: \"%s\".", ResourceFile->GetFilePath().GetValue());
		}
		
		if (!ZETextureLoader::Write((ZEFile*)&PartialResourceFile, &TextureData))
		{
			zeAssert(true, "Cannot cache the texture: \"%s\".", ResourceFile->GetFilePath().GetValue());
		}
		
		PartialResourceFile.Close();
		FileCache.Close();
	}

	// Create TextureResource 
	ZETexture2DResource* TextureResource = new ZETexture2DResource();
	ZETexture2D* Texture = TextureResource->Texture = ZETexture2D::CreateInstance();

	// Set Other Variables
	TextureResource->SetFileName(ResourceFile->GetFilePath());
	TextureResource->Cached = false;
	TextureResource->Shared = false;

	// Create the Texture
	if(!Texture->Create(TextureData.GetWidth(), TextureData.GetHeight(), TextureData.GetPixelFormat(), false, TextureData.GetMipmapCount()))
	{
		zeError("Can not create texture resource. FileName : \"%s\"", ResourceFile->GetFilePath().GetValue());
		TextureData.DestroyTexture();
		delete TextureResource;
		return NULL;
	}

	ZEUInt MipCount = TextureData.GetMipmapCount();
	ZEUInt SurfaceCount = TextureData.GetDepth();

	// Copy texture data into ZETexture2D
	void* TargetBuffer = NULL;
	ZESize TargetPitch = 0;
	
	for(ZESize I = 0; I < SurfaceCount; I++)
	{
		for(ZESize J = 0; J < MipCount; J++)
		{
			Texture->Lock(&TargetBuffer, &TargetPitch, J);
			TextureData.CopyMipmapDataTo(I, J, (unsigned char*)TargetBuffer, TargetPitch);
			Texture->Unlock(J);
		}
	}

	TextureData.DestroyTexture();
	return TextureResource;
}
