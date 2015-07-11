//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFontResourceDynamic.cpp
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

#include "ZEFontResourceDynamic.h"
#include "ZECore\ZEResourceManager.h"
#include "ZEError.h"
#include "ZEFile\zepartialfile.h"
#include "ZEFreeType.h"

#define UTF8_SPACE_CHARACTER 0x00020
#define CHARACTER_SPACING 4

struct ZEFontPixel
{
	ZEUInt8	R;
	ZEUInt8	G;
	ZEUInt8	B;
	ZEUInt8	A;
};

void ZEFontResourceDynamic::SetFontFile(ZEString FontFilePath)
{
	this->FontFile = FontFilePath;

	FT_Init_FreeType(&FreeType->Library);
	FT_New_Face(FreeType->Library, FontFilePath, 0, &FreeType->Face);
	FontSupportsKerning = FT_HAS_KERNING(FreeType->Face);
}

void ZEFontResourceDynamic::SetFontSize(ZEUInt32 FontSize)
{
	this->FontSize = FontSize;
}

void ZEFontResourceDynamic::CreateNewTexture(ZEUInt32 Width, ZEUInt32 Height)
{
	Textures.Add();
	Textures.GetLastItem() = ZEGRTexture2D::CreateInstance();
	Textures.GetLastItem()->Create(Width, Height, 1, ZEGR_TF_R8G8B8A8);
}

const char* ZEFontResourceDynamic::GetResourceType() const
{
	return "ZEFontResourceDynamic";
}

ZEFontResourceType ZEFontResourceDynamic::GetFontResourceType() const
{
	return ZE_FRT_DYNAMIC;
}

ZEUInt32 ZEFontResourceDynamic::GetFontSize() const
{
	return this->FontSize;
}

const ZEFontCharacter& ZEFontResourceDynamic::GetCharacter(char Character)
{
	ZEInt64 KerningDistance;
	return GetCharacter(Character, Character, KerningDistance);
}

const ZEFontCharacter& ZEFontResourceDynamic::GetCharacter(char Character, char PreviousCharacter, ZEInt64& KerningDistance)
{
	ZEFontPixel* Buffer;
	ZESize TexturePitch;
	ZEUInt32 LastItem = 0;
	ZEUInt32 CurrentGlyphIndex, PreviousGlyphIndex;
	KerningDistance = 0;

	for(ZESize I = 0; I < FontCharacters.GetCount(); I++)
	{
		if(FontCharacters[I].Character == Character)
		{
			if(isspace(Character))
				CurrentGlyphIndex = FT_Get_Char_Index(FreeType->Face, UTF8_SPACE_CHARACTER);
			else
				CurrentGlyphIndex = FT_Get_Char_Index(FreeType->Face, Character);

			if(isspace(PreviousCharacter))
				PreviousGlyphIndex = FT_Get_Char_Index(FreeType->Face, UTF8_SPACE_CHARACTER);
			else
				PreviousGlyphIndex = FT_Get_Char_Index(FreeType->Face, PreviousCharacter);

			if(FontSupportsKerning)
			{
				FT_Vector Delta;
				FT_Get_Kerning(FreeType->Face, PreviousGlyphIndex, CurrentGlyphIndex, FT_KERNING_DEFAULT, &Delta);
				//26.6 formatting 26 bits for pixel, 6 bits for subpixel
				KerningDistance = Delta.x >> 6;
			}
			return FontCharacters[I];
		}
	}

	FontCharacters.Add();
	LastItem = FontCharacters.GetCount() - 1;
	FontCharacters[LastItem].Character = Character;

	if(isspace(Character))
		CurrentGlyphIndex = FT_Get_Char_Index(FreeType->Face, UTF8_SPACE_CHARACTER);
	else
		CurrentGlyphIndex = FT_Get_Char_Index(FreeType->Face, Character);

	//Character size in 1/64th of points (PointFactor of 64)
	FT_Set_Char_Size(FreeType->Face, 0, FontSize * PointFactor, HorizontalOutputDPI, VerticalOutputDPI);
	FT_Load_Glyph(FreeType->Face, CurrentGlyphIndex, FT_LOAD_DEFAULT);

	FontCharacters[LastItem].GlyphIndex = CurrentGlyphIndex;
		
	FontCharacters[LastItem].CharacterMetric.FontSize			= FontSize;
	FontCharacters[LastItem].CharacterMetric.Height				= FreeType->Face->glyph->metrics.height		/ PointFactor;
	FontCharacters[LastItem].CharacterMetric.Width				= FreeType->Face->glyph->metrics.width			/ PointFactor;
	FontCharacters[LastItem].CharacterMetric.HorizontalAdvance	= FreeType->Face->glyph->metrics.horiAdvance	/ PointFactor;
	FontCharacters[LastItem].CharacterMetric.VerticalAdvance	= FreeType->Face->glyph->metrics.vertAdvance	/ PointFactor;
	FontCharacters[LastItem].CharacterMetric.HorizontalBearingX	= FreeType->Face->glyph->metrics.horiBearingX / PointFactor;
	FontCharacters[LastItem].CharacterMetric.HorizontalBearingY	= FreeType->Face->glyph->metrics.horiBearingY / PointFactor;
	FontCharacters[LastItem].CharacterMetric.VerticalBearingX	= FreeType->Face->glyph->metrics.vertBearingX / PointFactor;
	FontCharacters[LastItem].CharacterMetric.VerticalBearingY	= FreeType->Face->glyph->metrics.vertBearingY / PointFactor;

	FT_Glyph Glyph;
	FT_Get_Glyph(FreeType->Face->glyph, &Glyph);
	FT_Glyph_To_Bitmap(&Glyph, FT_RENDER_MODE_NORMAL, 0, 1);
	FT_Bitmap FTBitmap = ((FT_BitmapGlyph)Glyph)->bitmap;

	//Warning! Pot. bug
	//////////////////////////////////////////////////////////////////////////
	FT_Load_Glyph(FreeType->Face, FT_Get_Char_Index(FreeType->Face, 'O'), FT_LOAD_DEFAULT);
	FontCharacters[LastItem].CharacterMetric.MaximumHeight = FreeType->Face->glyph->metrics.height / PointFactor;

	if(isspace(PreviousCharacter))
		PreviousGlyphIndex = FT_Get_Char_Index(FreeType->Face, UTF8_SPACE_CHARACTER);
	else
		PreviousGlyphIndex = FT_Get_Char_Index(FreeType->Face, PreviousCharacter);

	if(LastCharacterPosition.x + FTBitmap.width > Textures[LastTextureId]->GetWidth())
	{
		LastTextureLine++;
		LastCharacterPosition.x = 0;
	}

	if(FontSize * LastTextureLine + FontSize > Textures[LastTextureId]->GetHeight())
	{
		LastTextureLine = 0;
		LastCharacterPosition.x = 0;
		CreateNewTexture(512, 512);
		LastTextureId++;
	}

	LastCharacterPosition.y = FontSize * LastTextureLine;

	Textures[LastTextureId]->Lock((void**)&Buffer, &TexturePitch, 0);
	ZEFontPixel* CurrentCharPix = Buffer + (ZEUInt32)LastCharacterPosition.x + ((ZEUInt32)(LastCharacterPosition.y * TexturePitch / 4));
	for (ZESize y = 0; y < FTBitmap.rows; y++)
	{
		ZEFontPixel* CurrentLine = CurrentCharPix + y * (TexturePitch / 4);

		for (ZESize x = 0; x <  FTBitmap.width; x++)
		{
			if(FTBitmap.buffer[FTBitmap.pitch * y + x] != 0)
			{
				ZEFontPixel* CurrentPixel = CurrentLine + x;

				unsigned char Value = FTBitmap.buffer[FTBitmap.pitch * y + x];
				CurrentPixel->R = Value;
				CurrentPixel->G = Value;
				CurrentPixel->B = Value;
				CurrentPixel->A = Value;
			}
		}
	}
	Textures[LastTextureId]->Unlock(0);

	FontCharacters[LastItem].CoordinateRectangle.LeftUp.x = LastCharacterPosition.x  / Textures[LastTextureId]->GetWidth();
	FontCharacters[LastItem].CoordinateRectangle.LeftUp.y = (LastCharacterPosition.y) / Textures[LastTextureId]->GetHeight();

	FontCharacters[LastItem].CoordinateRectangle.RightDown.x = (LastCharacterPosition.x + FontCharacters[LastItem].CharacterMetric.HorizontalAdvance) / Textures[LastTextureId]->GetWidth();
	FontCharacters[LastItem].CoordinateRectangle.RightDown.y = (LastCharacterPosition.y + FontCharacters[LastItem].CharacterMetric.Height) / Textures[LastTextureId]->GetHeight();

	if(FontSupportsKerning)
	{
		FT_Vector Delta;
		FT_Get_Kerning(FreeType->Face, PreviousGlyphIndex, CurrentGlyphIndex, FT_KERNING_DEFAULT, &Delta);
		//26.6 formatting 26 bits for pixel, 6 bits for subpixel
		KerningDistance = Delta.x >> 6;
	}

	LastCharacterPosition.x += FontCharacters[LastItem].CharacterMetric.HorizontalAdvance + CHARACTER_SPACING;

	FontCharacters[LastItem].Texture = Textures[LastTextureId];

	return FontCharacters[LastItem];
}

ZEGRTexture2D* ZEFontResourceDynamic::GetTexture(ZEUInt32 TextureId)
{
	return Textures[TextureId];
}

ZEFontResourceDynamic* ZEFontResourceDynamic::LoadSharedResource(const ZEString& FileName, ZEUInt32 FontSize)
{
	ZEFontResourceDynamic* NewResource = (ZEFontResourceDynamic*)zeResources->GetResource(FileName.GetValue());
	if (NewResource == NULL)
	{
		NewResource = LoadResource(FileName, FontSize);
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

void ZEFontResourceDynamic::CacheResource(const ZEString& FileName, ZEUInt32 FontSize)
{
	ZEFontResourceDynamic* NewResource = (ZEFontResourceDynamic*)zeResources->GetResource(FileName.GetValue());
	if (NewResource == NULL)
	{
		NewResource = LoadResource(FileName, FontSize);
		if (NewResource != NULL)
		{
			NewResource->Cached = true;
			NewResource->ReferenceCount = 0;
			zeResources->AddResource(NewResource);
		}
	}
}

ZEFontResourceDynamic* ZEFontResourceDynamic::LoadResource(const ZEString& FileName, ZEUInt32 FontSize)
{
	bool Result;
	ZEFontResourceDynamic* FontResource;

	ZEFile File;
	Result = File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE);
	if (Result)
	{
		FontResource = LoadResource(&File, FontSize);
		File.Close();

		return FontResource;
	}
	else
	{
		zeError("Font file not found. FilePath : \"%s\"", FileName.GetValue());
		return NULL;
	}
}

ZEFontResourceDynamic* ZEFontResourceDynamic::LoadResource(ZEFile* ResourceFile, ZEUInt32 FontSize)
{
	zeLog("Loading font file \"%s\".", ResourceFile->GetPath().GetValue());

	ZEFontResourceDynamic* NewResource = new ZEFontResourceDynamic();
	NewResource->SetFileName(ResourceFile->GetPath());

	NewResource->SetFontFile(ResourceFile->GetPath());
	NewResource->SetFontSize(FontSize);

	zeLog("Font file \"%s\" has been loaded.", ResourceFile->GetPath().GetValue());

	return NewResource;
}

ZEFontResourceDynamic::ZEFontResourceDynamic()
{
	FreeType = new ZEFreeType();
	FreeType->Face = NULL;
	FreeType->Library = NULL;

	HorizontalOutputDPI = 72;
	VerticalOutputDPI = 72;
	PointFactor = 64;
	LastCharacterPosition.x = 0;
	LastCharacterPosition.y = 0;

	LastTextureId = 0;
	LastTextureLine = 0;

	CreateNewTexture(512, 512);
}

ZEFontResourceDynamic::~ZEFontResourceDynamic()
{
	delete FreeType;
	FreeType = 0;
}
