//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFontResourceBitmap.cpp
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
#include "ZEUI/ZEFontFile.h"
#include "ZEFontResourceBitmap.h"
#include "ZECore/ZEConsole.h"
#include "ZEFile/ZEPartialFile.h"
#include "ZEGraphics/ZEUIMaterial.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZETexture/ZETextureOptions.h"
#include "ZECore/ZEResourceManager.h"


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

ZEFontResourceBitmap::ZEFontResourceBitmap()
{
	Characters.SetCount(256);
}

ZEFontResourceBitmap::~ZEFontResourceBitmap()
{
	for (ZESize I = 0; I < TextureResources.GetCount(); I++)
	{
		TextureResources[I]->Release();
	}
}

const char* ZEFontResourceBitmap::GetResourceType() const
{
	return "ZEFontResourceBitmap";
}

ZEFontResourceType ZEFontResourceBitmap::GetFontResourceType() const
{
	return ZE_FRT_BITMAP;
}

const ZEFontCharacter& ZEFontResourceBitmap::GetCharacter(char Character)
{
	return Characters[Character];
}

const ZEFontCharacter& ZEFontResourceBitmap::GetCharacter(char CurrentChar, char NextChar, ZEInt64& KerningDistance)
{
	KerningDistance = 0;
	return GetCharacter(CurrentChar);
}

ZEFontResourceBitmap* ZEFontResourceBitmap::LoadSharedResource(const ZEString& FilePath, const ZETextureOptions* UserOptions)
{
	ZEString NewPath = ConstructResourcePath(FilePath);

	ZEFontResourceBitmap* NewResource = (ZEFontResourceBitmap*)zeResources->GetResource(NewPath.GetValue());
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
		{
			return NULL;
		}
	}
	else
		return NewResource;
}

void ZEFontResourceBitmap::CacheResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	ZEFontResourceBitmap* NewResource = (ZEFontResourceBitmap*)zeResources->GetResource(NewPath.GetValue());
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

ZEFontResourceBitmap* ZEFontResourceBitmap::LoadResource(const ZEString& FilePath, const ZETextureOptions* UserOptions)
{
	ZEString NewPath = ConstructResourcePath(FilePath);

	bool Result;
	ZEFile File;
	ZEFontResourceBitmap* FontResource;

	Result = File.Open(NewPath, ZE_FOM_READ, ZE_FCM_NONE);
	if (Result)
	{
		if(UserOptions == NULL)
			UserOptions = zeGraphics->GetTextureOptions();

		FontResource = LoadResource(&File, UserOptions);
		File.Close();

		return FontResource;
	}
	else
	{
		zeError("Texture file not found. FilePath : \"%s\"", FilePath.GetValue());
		return NULL;
	}
}

ZEFontResourceBitmap* ZEFontResourceBitmap::LoadResource(ZEFile* ResourceFile, const ZETextureOptions* UserOptions)
{
	zeLog("Loading font file \"%s\".", ResourceFile->GetPath().GetValue());

	if(UserOptions == NULL)
		UserOptions = zeGraphics->GetTextureOptions();

	// Font files should not be resized or mipmapped
	ZETextureOptions ModifiedOptions = {UserOptions->CompressionType, 
		UserOptions->CompressionQuality,
		ZE_TDS_NONE, 
		UserOptions->FileCaching,
		ZE_TMM_DISABLED, 
		UserOptions->MaximumMipmapLevel};

	ZEFontFileHeader FileHeader;
	ResourceFile->Read(&FileHeader, sizeof(ZEFontFileHeader), 1);
	if (FileHeader.Header != ZE_FONT_FILE_HEADER)
	{
		zeError("Unknown ZEFont file format. (FilePath : \"%s\")", ResourceFile->GetPath().GetValue());
		return NULL;
	}

	ZEFontResourceBitmap* NewResource = new ZEFontResourceBitmap();
	NewResource->SetFileName(ResourceFile->GetPath());
	NewResource->TextureResources.SetCount((ZESize)FileHeader.TextureCount);

	for (ZESize I = 0; I < (ZESize)FileHeader.TextureCount; I++)
	{
		ZEUInt32 FileCursor, TextureFileSize;

		ResourceFile->Read(&TextureFileSize, sizeof(ZEUInt32), 1);
		FileCursor = (ZEUInt32)ResourceFile->Tell();

		ZEPartialFile TextureResourceFile;
		TextureResourceFile.Open(ResourceFile, FileCursor, TextureFileSize);

		ZETexture2DResource* CurrentTexture = ZETexture2DResource::LoadResource((ZEFile*)&TextureResourceFile, &ModifiedOptions);
		if (CurrentTexture == NULL)
		{
			zeError("Can not read texture from the file. (FilePath : \"%s\", Texture Index : %d)", ResourceFile->GetPath().GetValue(), I);
			TextureResourceFile.Close();
			delete NewResource;
			return NULL;
		}
		TextureResourceFile.Close();

		NewResource->TextureResources[I] = CurrentTexture;
		ResourceFile->Seek(FileCursor + TextureFileSize, ZE_SF_BEGINING);
	}

	for (ZESize I = 0; I < ZE_FONT_FILE_CHARACTER_COUNT; I++)
	{
		NewResource->Characters[I].CoordinateRectangle = FileHeader.Characters[I].Coordinates;
		NewResource->Characters[I].Texture = NewResource->TextureResources[(ZESize)FileHeader.Characters[I].TextureId]->GetTexture();
	}

	zeLog("Font file \"%s\" has been loaded.", ResourceFile->GetPath().GetValue());

	return NewResource;
}