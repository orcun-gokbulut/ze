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
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZERenderer/ZEUIMaterial.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZETexture/ZETextureOptions.h"
#include "ZECore/ZEResourceManager.h"
#include "ZEML/ZEMLReader.h"
#include "ZEFile/ZEDirectoryInfo.h"

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

ZEFontResourceBitmap* ZEFontResourceBitmap::LoadSharedResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	ZEFontResourceBitmap* NewResource = (ZEFontResourceBitmap*)zeResources->GetResource(FileName.GetValue());
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
		{
			return NULL;
		}
	}
	else
		return NewResource;
}

void ZEFontResourceBitmap::CacheResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	ZEFontResourceBitmap* NewResource = (ZEFontResourceBitmap*)zeResources->GetResource(FileName.GetValue());
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

ZEFontResourceBitmap* ZEFontResourceBitmap::LoadResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	bool Result;
	ZEFile File;
	ZEFontResourceBitmap* FontResource;

	Result = File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE);
	if (Result)
	{
		if(UserOptions == NULL)
			UserOptions = ZEGRGraphicsModule::GetInstance()->GetTextureOptions();

		FontResource = LoadResource(&File, UserOptions);
		File.Close();

		return FontResource;
	}
	else
	{
		zeError("Texture file not found. FilePath : \"%s\"", FileName.GetValue());
		return NULL;
	}
}

ZEFontResourceBitmap* ZEFontResourceBitmap::LoadResource(ZEFile* ResourceFile, const ZETextureOptions* UserOptions)
{
	zeLog("Loading font file \"%s\".", ResourceFile->GetPath().GetValue());

	if(UserOptions == NULL)
		UserOptions = ZEGRGraphicsModule::GetInstance()->GetTextureOptions();

	// Font files should not be resized or mipmapped
	ZETextureOptions ModifiedOptions = {UserOptions->CompressionType, 
		UserOptions->CompressionQuality,
		ZE_TDS_NONE, 
		UserOptions->FileCaching,
		ZE_TMM_DISABLED, 
		UserOptions->MaximumMipmapLevel};

	ZEFontResourceBitmap* NewResource = new ZEFontResourceBitmap();
	
	ZEMLReader FontReader;
	FontReader.Open(ResourceFile);
	ZEMLReaderNode RootNode = FontReader.GetRootNode();

	if(RootNode.IsValid())
	{
		if(RootNode.GetName() != "ZEFont")
			zeError("File is not a ZEFont file.");
	}

	if (!RootNode.IsSubNodeExists("Textures"))
		zeError("Can not find Textures node.");

	ZEMLReaderNode TexturesNode = RootNode.GetSubNode("Textures");

	ZESize TextureCount = TexturesNode.GetSubNodeCount("Texture");

	NewResource->TextureResources.SetCount(TextureCount);
	NewResource->Textures.SetCount(TextureCount);

	for(ZESize I = 0; I < TextureCount; I++)
	{
		ZEMLReaderNode TextureNode = TexturesNode.GetSubNode("Texture", I);

		if(!TextureNode.IsValid())
		{
			zeError("Can not read Texture node.");
			break;
		}

		ZEString TextureFileName = TextureNode.ReadString("FileName");

		ZEString TexturePath = "../" + ZEDirectoryInfo(ResourceFile->GetPath()).GetParentDirectory() + "/" + TextureFileName;

		NewResource->TextureResources[I] = ZETexture2DResource::LoadSharedResource(TexturePath);
		NewResource->Textures[I] = NewResource->TextureResources[I]->GetTexture2D();
	}

	ZEMLReaderNode CharactersNode = RootNode.GetSubNode("Characters");

	if(!CharactersNode.IsValid())
		zeError("Can not find Characters node.");


	ZESize CharacterCount = CharactersNode.GetSubNodeCount("Character");

	NewResource->Characters.SetCount(CharacterCount);

	for(ZESize I = 0; I < CharacterCount; I++)
	{
		ZEMLReaderNode CharacterNode = CharactersNode.GetSubNode("Character", I);

		if(CharacterNode.IsValid())
		{
			NewResource->Characters[I].Character = CharacterNode.ReadInt32("Value");
			NewResource->Characters[I].GlyphIndex = CharacterNode.ReadUInt32("GlyphIndex");
			NewResource->Characters[I].CharacterMetric.FontSize = CharacterNode.ReadUInt32("FontSize");
			NewResource->Characters[I].CharacterMetric.Height = CharactersNode.ReadInt32("Height");
			NewResource->Characters[I].CharacterMetric.Width = CharactersNode.ReadInt32("Width");
			NewResource->Characters[I].CharacterMetric.HorizontalAdvance = CharactersNode.ReadInt32("HorizontalAdvance");
			NewResource->Characters[I].CharacterMetric.VerticalAdvance = CharactersNode.ReadInt32("VerticalAdvance");
			NewResource->Characters[I].CharacterMetric.HorizontalBearingX = CharactersNode.ReadInt32("HorizontalBearingX");
			NewResource->Characters[I].CharacterMetric.HorizontalBearingY = CharactersNode.ReadInt32("HorizontalBearingY");
			NewResource->Characters[I].CharacterMetric.VerticalBearingX = CharactersNode.ReadInt32("VerticalBearingX");
			NewResource->Characters[I].CharacterMetric.VerticalBearingY = CharactersNode.ReadInt32("VerticalBearingY");
			NewResource->Characters[I].CoordinateRectangle.LeftUp = CharactersNode.ReadVector2("LeftUp");
			NewResource->Characters[I].CoordinateRectangle.RightDown = CharactersNode.ReadVector2("RightDown");
			NewResource->Characters[I].Texture = NewResource->Textures[CharactersNode.ReadUInt32("TextureID")];
		}
	}

	FontReader.Close();

	zeLog("Font file \"%s\" has been loaded.", ResourceFile->GetPath().GetValue());

	return NewResource;
}
