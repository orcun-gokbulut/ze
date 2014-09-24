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
#include "ZEFontResourceBitmap.h"
#include "ZECore/ZEConsole.h"
#include "ZEFile/ZEPartialFile.h"
#include "ZEGraphics/ZEGraphicsModule.h"
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
	for(ZESize I = 0; I < Characters.GetCount(); I++)
	{
		if(Characters[I].Character == Character)
			return Characters[I];
	}

	return ZEFontCharacter();
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

	ZEFontResourceBitmap* NewResource = new ZEFontResourceBitmap();
	
	ZEMLSerialReader* Reader = new ZEMLSerialReader(ResourceFile);
	if(Reader->Read())
	{
		if(Reader->GetItemName() != "ZEFont")
			zeError("File is not a ZEFont file.");
	}

	if(Reader->Read())
	{
		if(Reader->GetItemName() != "Textures")
			zeError("Can not find Textures node.");
	}

	ZESize TextureCount = Reader->GetSubItemCount();

	NewResource->TextureResources.SetCount(TextureCount);
	NewResource->Textures.SetCount(TextureCount);

	for(ZESize I = 0; I < TextureCount; I++)
	{
		if(Reader->Read())
		{
			if(Reader->GetItemName() != "Texture")
				zeError("Can not read Texture node.");

			Reader->Read();
			ZEString TextureFileName = Reader->GetItemValue().GetString();

			ZEString TexturePath = "../" + ZEDirectoryInfo::GetParentDirectory(ResourceFile->GetPath()) + "/" + TextureFileName;

			NewResource->TextureResources[I] = ZETexture2DResource::LoadSharedResource(TexturePath);
			NewResource->Textures[I] = NewResource->TextureResources[I]->GetTexture();
		}
	}

	if(Reader->Read())
	{
		if(Reader->GetItemName() != "FontInformation")
			zeError("Can not find FontInformation node.");

		ZESize FontInformationSubItemCount = Reader->GetSubItemCount();
		for(ZESize I = 0; I < FontInformationSubItemCount; I++)
		{
			Reader->Read();
		}
	}

	if(Reader->Read())
	{
		if(Reader->GetItemName() != "Characters")
			zeError("Can not find Characters node.");
	}

	ZESize CharacterCount = Reader->GetSubItemCount();

	NewResource->Characters.SetCount(CharacterCount);

	for(ZESize I = 0; I < CharacterCount; I++)
	{
		if(Reader->Read())
		{
			if(Reader->GetItemName() != "Character")
				zeError("Can not read Character node.");

			Reader->Read();
			if(Reader->GetItemName() != "Value")
				zeError("Can not read Value node.");
			NewResource->Characters[I].Character = Reader->GetItemValue().GetInt32();

			Reader->Read();
			if(Reader->GetItemName() != "GlyphIndex")
				zeError("Can not read GlyphIndex node.");
			NewResource->Characters[I].GlyphIndex = Reader->GetItemValue().GetUInt32();

			Reader->Read();
			if(Reader->GetItemName() != "FontSize")
				zeError("Can not read FontSize node.");
			NewResource->Characters[I].CharacterMetric.FontSize = Reader->GetItemValue().GetUInt32();

			Reader->Read();
			if(Reader->GetItemName() != "Height")
				zeError("Can not read Height node.");
			NewResource->Characters[I].CharacterMetric.Height = Reader->GetItemValue().GetInt32();

			Reader->Read();
			if(Reader->GetItemName() != "Width")
				zeError("Can not read Width node.");
			NewResource->Characters[I].CharacterMetric.Width = Reader->GetItemValue().GetInt32();

			Reader->Read();
			if(Reader->GetItemName() != "HorizontalAdvance")
				zeError("Can not read HorizontalAdvance node.");
			NewResource->Characters[I].CharacterMetric.HorizontalAdvance = Reader->GetItemValue().GetInt32();

			Reader->Read();
			if(Reader->GetItemName() != "VerticalAdvance")
				zeError("Can not read VerticalAdvance node.");
			NewResource->Characters[I].CharacterMetric.VerticalAdvance = Reader->GetItemValue().GetInt32();

			Reader->Read();
			if(Reader->GetItemName() != "HorizontalBearingX")
				zeError("Can not read HorizontalBearingX node.");
			NewResource->Characters[I].CharacterMetric.HorizontalBearingX = Reader->GetItemValue().GetInt32();

			Reader->Read();
			if(Reader->GetItemName() != "HorizontalBearingY")
				zeError("Can not read HorizontalBearingY node.");
			NewResource->Characters[I].CharacterMetric.HorizontalBearingY = Reader->GetItemValue().GetInt32();

			Reader->Read();
			if(Reader->GetItemName() != "VerticalBearingX")
				zeError("Can not read VerticalBearingX node.");
			NewResource->Characters[I].CharacterMetric.VerticalBearingX = Reader->GetItemValue().GetInt32();

			Reader->Read();
			if(Reader->GetItemName() != "VerticalBearingY")
				zeError("Can not read VerticalBearingY node.");
			NewResource->Characters[I].CharacterMetric.VerticalBearingY = Reader->GetItemValue().GetInt32();
			
			Reader->Read();
			if(Reader->GetItemName() != "LeftUp")
				zeError("Can not read LeftUp node.");
			NewResource->Characters[I].CoordinateRectangle.LeftUp = Reader->GetItemValue().GetVector2();

			Reader->Read();
			if(Reader->GetItemName() != "RightDown")
				zeError("Can not read RightDown node.");
			NewResource->Characters[I].CoordinateRectangle.RightDown = Reader->GetItemValue().GetVector2();

			Reader->Read();
			if(Reader->GetItemName() != "TextureID")
				zeError("Can not read TextureID node.");
			NewResource->Characters[I].Texture = NewResource->Textures[Reader->GetItemValue().GetUInt32()];
		}
	}

	zeLog("Font file \"%s\" has been loaded.", ResourceFile->GetPath().GetValue());

	return NewResource;
}
