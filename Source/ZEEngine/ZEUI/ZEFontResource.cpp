//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFontResource.cpp
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
#include "ZEFontResource.h"
#include "ZECore/ZEConsole.h"
#include "ZEFile/ZEPartialFile.h"
#include "ZEGraphics/ZEUIMaterial.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZETexture/ZETextureOptions.h"
#include "ZECore/ZEResourceManager.h"
#include "ZEML/ZEMLSerialReader.h"
#include "ZEFile/ZEDirectoryInfo.h"


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

ZEFontResource::ZEFontResource()
{
}

ZEFontResource::~ZEFontResource()
{
	for (ZESize I = 0; I < Materials.GetCount(); I++)
	{
		Materials[I]->Destroy();
	}

	for (ZESize I = 0; I < TextureResources.GetCount(); I++)
	{
		TextureResources[I]->Release();
	}
}

const char* ZEFontResource::GetResourceType() const
{
	return "Font Resource";
}

const ZEFontResourceCharacter& ZEFontResource::GetCharacter(char Character)
{
	ZESize Index = Character - Characters[0].Value;
	return Characters[Index];
}

ZEFontResource* ZEFontResource::LoadSharedResource(const ZEString& FilePath, const ZETextureOptions* UserOptions)
{
	ZEString NewPath = ConstructResourcePath(FilePath);

	ZEFontResource* NewResource =(ZEFontResource*)zeResources->GetResource(NewPath.GetValue());
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

void ZEFontResource::CacheResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	ZEFontResource* NewResource = (ZEFontResource*)zeResources->GetResource(NewPath.GetValue());
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

ZEFontResource* ZEFontResource::LoadResource(const ZEString& FilePath, const ZETextureOptions* UserOptions)
{
	ZEString NewPath = ConstructResourcePath(FilePath);

	bool Result;
	ZEFile File;
	ZEFontResource* FontResource;
	
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

ZEFontResource* ZEFontResource::LoadResource(ZEFile* ResourceFile, const ZETextureOptions* UserOptions)
{
	zeLog("Loading font file \"%s\".", ResourceFile->GetPath().GetValue());

	ZEMLSerialReader Reader(ResourceFile);

	if(!Reader.Read())
	{
		zeError("Can not read fot file. File path : %s", ResourceFile->GetPath());
		return NULL;
	}

	if(Reader.GetItemName() != "ZEFont")
	{
		zeError("Given file is not a font file. File path : %s", ResourceFile->GetPath());
		return NULL;
	}

	if(UserOptions == NULL)
		UserOptions = zeGraphics->GetTextureOptions();

	// Font files should not be resized or mipmapped
	ZETextureOptions ModifiedOptions = {UserOptions->CompressionType, 
										UserOptions->CompressionQuality,
										ZE_TDS_NONE, 
										ZE_TFC_DISABLED,
										ZE_TMM_DISABLED,
										UserOptions->MaximumMipmapLevel};

	ZEFontResource* NewResource = new ZEFontResource();
	NewResource->SetFileName(ResourceFile->GetPath());

	ZEVariant TextureCount;
	ZEMLSerialPointer CharactersPointer, TexturesPointer;

	ZEMLSerialListItem List[] = {
		ZEML_LIST_DATA("Characters",		CharactersPointer,	true),
		ZEML_LIST_NODE("Textures",			TexturesPointer,	false),
		ZEML_LIST_PROPERTY("TextureCount",	TextureCount, ZE_VRT_UNSIGNED_INTEGER_32, true)
	};

	Reader.ReadPropertyList(List, 3);

	if(TexturesPointer != -1)
	{
		NewResource->TextureResources.SetCount(TextureCount.GetUInt32());
		NewResource->Materials.SetCount(TextureCount.GetUInt32());
		Reader.SeekPointer(TexturesPointer);

		for (ZESize I = 0; I < (ZESize)TextureCount.GetUInt32(); I++)
		{
			if(!Reader.Read())
			{
				zeError("Can not read texture file.");
				delete NewResource;
				return NULL;
			}

			if(Reader.GetItemName() != "Texture")
			{
				zeError("Can not read texture file.");
				delete NewResource;
				return NULL;
			}

			if(!Reader.Read())
			{
				zeError("Can not read texture file name.");
				delete NewResource;
				return NULL;
			}

			if(Reader.GetItemName() != "FileName")
			{
				zeError("Can not read texture file name.");
				delete NewResource;
				return NULL;
			}

			ZEString TextureFilePath = ZEDirectoryInfo::GetParentDirectory(ResourceFile->GetPath()) + "\\" + Reader.GetItemValue().GetString();

			ZETexture2DResource* CurrentTexture = ZETexture2DResource::LoadResource(TextureFilePath, &ModifiedOptions);
			if (CurrentTexture == NULL)
			{
				zeError("Can not read texture from the file. (FilePath : \"%s\", Texture Index : %d)", TextureFilePath.ToCString(), I);
				delete NewResource;
				return NULL;
			}

			NewResource->TextureResources[I] = CurrentTexture;

			NewResource->Materials[I] = ZEUIMaterial::CreateInstance();
			NewResource->Materials[I]->SetWireFrame(false);
			NewResource->Materials[I]->SetTexture(NewResource->TextureResources[I]->GetTexture());
			NewResource->Materials[I]->UpdateMaterial();
		}
	}
	else
		zeWarning("Font file does not contain any textures.");

	Reader.SeekPointer(CharactersPointer);
	ZEArray<ZEFontFileCharacter> FileCharacters;
	FileCharacters.SetCount(Reader.GetDataSize() / sizeof(ZEFontFileCharacter));
	Reader.GetData(FileCharacters.GetCArray(), Reader.GetDataSize());
	NewResource->Characters.SetCount(FileCharacters.GetCount());

	for (ZESize I = 0; I < NewResource->Characters.GetCount(); I++)
	{
		NewResource->Characters[I].CoordinateRectangle = FileCharacters[I].Coordinates;
		NewResource->Characters[I].Texture = NewResource->TextureResources[FileCharacters[I].TextureId]->GetTexture();
		
		if(TexturesPointer != -1)
			NewResource->Characters[I].Material = NewResource->Materials[FileCharacters[I].TextureId];
		else
			NewResource->Characters[I].Material = NULL;
		
		NewResource->Characters[I].Value = FileCharacters[I].Value;
	}

	zeLog("Font file \"%s\" has been loaded.", ResourceFile->GetPath().GetValue());

	return NewResource;
}
