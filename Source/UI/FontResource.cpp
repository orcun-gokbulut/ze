//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - FontResource.cpp
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

#include "FontResource.h"
#include "Graphics/Texture2DResource.h"
#include "Core/ResourceManager.h"
#include "Core/Error.h"
#include "Core/ResourceFile.h"
#include "FontFile.h"

ZEFontResource::ZEFontResource()
{
}

ZEFontResource::~ZEFontResource()
{
	for (int I = 0; I < TextureResources.GetCount(); I++)
		TextureResources[I]->Release();
}

const char* ZEFontResource::GetResourceType() const
{
	return "Font Resource";
}

const ZEFontCharacter& ZEFontResource::GetCharacter(char Character)
{
	return Characters[Character];
}


ZEFontResource* ZEFontResource::LoadResource(ZEResourceFile* ResourceFile)
{
	ZEFontFileHeader FileHeader;

	ResourceFile->Read(&FileHeader, sizeof(ZEFontFileHeader), 1);
	if (FileHeader.Header != ZE_FONT_FILE_HEADER)
	{
		zeError("Font Resource", "Unknown ZEFont file format. (FileName : \"%s\")", ResourceFile->GetFileName());
		return NULL;
	}

	ZEFontResource* NewResource = new ZEFontResource();

	NewResource->TextureResources.SetCount(FileHeader.TextureCount);
	for (size_t I = 0; I < FileHeader.TextureCount; I++)
	{
		ZETexture2DResource* CurrentTexture = ZETexture2DResource::LoadResource(ResourceFile);
		if (CurrentTexture == NULL)
		{
			zeError("Font Resource", "Can not read texture from the file. (FileName : \"%s\", Texture Index : %d)",  ResourceFile->GetFileName(), I);
			delete NewResource;
			return NULL;
		}
		NewResource->TextureResources[I] = CurrentTexture;
	}

	for (size_t I = 0; I < ZE_FONT_FILE_CHARACTER_COUNT; I++)
	{
		NewResource->Characters[I].CoordinateRectangle = FileHeader.Characters[I].Coordinates;
		NewResource->Characters[I].Texture = NewResource->TextureResources[I]->GetTexture();
	}

	return NewResource;
}

ZEFontResource* ZEFontResource::LoadResource(const char* FileName)
{
	ZEFontResource* TextureResource;
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

ZEFontResource* ZEFontResource::LoadSharedResource(const char* FileName)
{
	ZEFontResource* NewResource =(ZEFontResource*)zeResources->GetResource(FileName);
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

void ZEFontResource::CacheResource(const char* FileName)
{
	ZEFontResource* NewResource = (ZEFontResource*)zeResources->GetResource(FileName);
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
