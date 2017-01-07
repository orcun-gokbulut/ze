//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFontBaker.cpp
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

#include "ZEFontBaker.h"

#include "ft2build.h"
#include "freetype\freetype.h"
#include "freetype\ftglyph.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEFile/ZEFile.h"

#define UTF8_SPACE_CHARACTER 0x00020

struct ZEFTFontCharMetrics
{
	char		Character;
	ZEUInt32	GlyphIndex;
	ZEUInt32	FontSize;

	ZEInt32		Height;
	ZEInt32		Width;

	ZEInt32		HorizontalAdvance;
	ZEInt32		VerticalAdvance;

	ZEInt32		HorizontalBearingX;
	ZEInt32		HorizontalBearingY;

	ZEInt32		VerticalBearingX;
	ZEInt32		VerticalBearingY;

	ZEVector2	LeftUp;
	ZEVector2	RightDown;

	ZEUInt32	TextureID;
};

static ZEBitmap* CreateBitmap(ZEUInt32 TextureWidth, ZEUInt32 TextureHeight)
{
	ZEBitmap* Bitmap = new ZEBitmap();
	Bitmap->Create(TextureWidth, TextureHeight, ZE_BPF_GRAYSCALE);
	Bitmap->Fill(0);
	return Bitmap;
}

bool ZEFontBaker::BakeFont(ZEString CharacterSequence, ZEString FontFilePath, ZEString OutputFilePath, ZEString OutputFileName, ZEUInt32 FontSize, 
									ZEUInt32 HorizontalOutputDPI, ZEUInt32 VerticalOutputDPI, ZEUInt32 PointFactor, 
									ZEUInt32 TextureWidth, ZEUInt32 TextureHeight,
									ZEUInt32 MarginTop, ZEUInt32 MarginBottom, ZEUInt32 MarginLeft, ZEUInt32 MarginRight,
									ZEUInt32 HorizontalPadding, ZEUInt32 VerticalPadding)
{
	CharacterSequence.Append(ZEString::FromChar(UTF8_SPACE_CHARACTER));

	FT_Library Library;
	FT_Face	Face;

	FT_Init_FreeType(&Library);
	FT_New_Face(Library, FontFilePath, 0, &Face);

	ZEUInt32 CurrentGlyphIndex;

	ZEArray<ZEFTFontCharMetrics> FontCharacters;
	FontCharacters.SetCount(CharacterSequence.GetLength());

	ZEArray<FT_Bitmap> FTBitmaps;
	FTBitmaps.SetCount(CharacterSequence.GetLength());

	for(ZESize I = 0; I < CharacterSequence.GetLength(); I++)
	{
		char CurrentCharacter = CharacterSequence.GetCharacter(I).GetValue()[0];
		FontCharacters[I].Character = CurrentCharacter;

		if(isspace(CurrentCharacter))
			CurrentGlyphIndex = FT_Get_Char_Index(Face, UTF8_SPACE_CHARACTER);
		else
			CurrentGlyphIndex = FT_Get_Char_Index(Face, CurrentCharacter);

		//Character size in 1/64th of points (PointFactor of 64)
		FT_Set_Char_Size(Face, 0, FontSize * PointFactor, HorizontalOutputDPI, VerticalOutputDPI);
		FT_Load_Glyph(Face, CurrentGlyphIndex, FT_LOAD_DEFAULT);

		FontCharacters[I].GlyphIndex			= CurrentGlyphIndex;
		FontCharacters[I].FontSize				= FontSize;
		FontCharacters[I].Height				= Face->glyph->metrics.height		/ PointFactor;
		FontCharacters[I].Width					= Face->glyph->metrics.width		/ PointFactor;
		FontCharacters[I].HorizontalAdvance		= Face->glyph->metrics.horiAdvance	/ PointFactor;
		FontCharacters[I].VerticalAdvance		= Face->glyph->metrics.vertAdvance	/ PointFactor;
		FontCharacters[I].HorizontalBearingX	= Face->glyph->metrics.horiBearingX / PointFactor;
		FontCharacters[I].HorizontalBearingY	= Face->glyph->metrics.horiBearingY / PointFactor;
		FontCharacters[I].VerticalBearingX		= Face->glyph->metrics.vertBearingX / PointFactor;
		FontCharacters[I].VerticalBearingY		= Face->glyph->metrics.vertBearingY / PointFactor;

		FT_Glyph Glyph;
		FT_Get_Glyph(Face->glyph, &Glyph);
		FT_Glyph_To_Bitmap(&Glyph, FT_RENDER_MODE_NORMAL, 0, 1);
		FTBitmaps[I] = ((FT_BitmapGlyph)Glyph)->bitmap;
	}

	ZEArray<ZEBitmap*> Bitmaps;
	Bitmaps.Add(CreateBitmap(TextureWidth, TextureHeight));

	ZEUInt32 TextureId = 0;
	ZEUInt32 CurrentX = MarginLeft, CurrentY = MarginTop;

	ZEString TextureOutputFile = OutputFilePath;
	TextureOutputFile.Append(ZEString::FromUInt32(TextureId));
	TextureOutputFile.Append(".png");

	for(ZESize I = 0; I < FTBitmaps.GetCount(); I++)
	{
		if(CurrentX + MarginRight + FTBitmaps[I].width > TextureWidth)
		{
			CurrentX = MarginLeft;
			CurrentY += FontSize + VerticalPadding;

			if(CurrentY + MarginBottom + FontSize > TextureHeight)
			{
				Bitmaps[TextureId]->Save(TextureOutputFile.ToCString(), ZE_BFF_PNG);

				CurrentX = MarginLeft;
				CurrentY = MarginTop;

				TextureId++;
				Bitmaps.Add(CreateBitmap(TextureWidth, TextureHeight));

				TextureOutputFile.Remove(TextureOutputFile.GetLength() - 5, 5);
				TextureOutputFile.Append(ZEString::FromUInt32(TextureId));
				TextureOutputFile.Append(".png");
			}
		}

		if(FontCharacters[I].Character != UTF8_SPACE_CHARACTER)
			Bitmaps[TextureId]->CopyFrom(FTBitmaps[I].buffer, FTBitmaps[I].pitch, FTBitmaps[I].width, FTBitmaps[I].rows, 0, 0, CurrentX, CurrentY);

		FontCharacters[I].TextureID = TextureId;

		float U = (float)CurrentX / (float)TextureWidth;
		float V = (float)CurrentY / (float)TextureHeight;
		FontCharacters[I].LeftUp = ZEVector2(U, V);

		CurrentX += FTBitmaps[I].width;

		U = (float)CurrentX / (float)TextureWidth;
		V = ((float)CurrentY + (float)FTBitmaps[I].rows) / (float)TextureHeight;
		FontCharacters[I].RightDown = ZEVector2(U, V);

		CurrentX += HorizontalPadding;
	}

	Bitmaps[TextureId]->Save(TextureOutputFile.ToCString(), ZE_BFF_PNG);

	for(ZESize I = 0; I < Bitmaps.GetCount(); I++)
		delete Bitmaps[I];

	Bitmaps.Clear();

	ZEString DataOutputFile = OutputFilePath;
	DataOutputFile.Append(OutputFileName);
	DataOutputFile.Append(".ZEFont");

	ZEFile* CharacterMetricsOutput = new ZEFile();
	CharacterMetricsOutput->Open(DataOutputFile, ZE_FOM_WRITE, ZE_FCM_OVERWRITE);

	ZEMLWriter FreeTypeFontWriter;
	FreeTypeFontWriter.Open(CharacterMetricsOutput);

	ZEMLWriterNode FreeTypeRootNode;
	FreeTypeFontWriter.OpenRootNode("ZEFont", FreeTypeRootNode);

	ZEMLWriterNode TexturesNode;
	FreeTypeRootNode.OpenNode("Textures", TexturesNode);

	ZEMLWriterNode TextureNode;

	for(ZESize I = 0; I <= TextureId; I++)
	{
		TexturesNode.OpenNode("Texture", TextureNode);
		ZEString TextureFileName;
		TextureFileName.Append(ZEString::FromUInt32(I));
		TextureFileName.Append(".png");
		TextureNode.WriteString("FileName", TextureFileName);
		TextureNode.CloseNode();
	}

	TexturesNode.CloseNode();

	ZEMLWriterNode FontInformationNode;
	FreeTypeRootNode.OpenNode("FontInformation", FontInformationNode);
	FontInformationNode.WriteString("CharacterSequence", CharacterSequence);
	FontInformationNode.WriteUInt32("TextureCount", TextureId + 1);
	FontInformationNode.WriteUInt32("FontSize", FontSize);
	FontInformationNode.WriteUInt32("HorizontalOutputDPI", HorizontalOutputDPI);
	FontInformationNode.WriteUInt32("VerticalOutputDPI", VerticalOutputDPI);
	FontInformationNode.WriteUInt32("PointFactor", PointFactor);
	FontInformationNode.WriteUInt32("TextureWidth", TextureWidth);
	FontInformationNode.WriteUInt32("TextureHeight", TextureHeight);
	FontInformationNode.WriteUInt32("MarginTop", MarginTop);
	FontInformationNode.WriteUInt32("MarginBottom", MarginBottom);
	FontInformationNode.WriteUInt32("MarginLeft", MarginLeft);
	FontInformationNode.WriteUInt32("MarginRight", MarginRight);
	FontInformationNode.WriteUInt32("HorizontalPadding", HorizontalPadding);
	FontInformationNode.WriteUInt32("VerticalPadding", VerticalPadding);
	FontInformationNode.CloseNode();

	ZEMLWriterNode CharactersNode;
	FreeTypeRootNode.OpenNode("Characters", CharactersNode);

	ZEMLWriterNode CharacterMetricNode;

	for(ZESize I = 0; I < FontCharacters.GetCount(); I++)
	{
		CharacterMetricNode;
		CharactersNode.OpenNode("Character", CharacterMetricNode);
		CharacterMetricNode.WriteInt32("Value", FontCharacters[I].Character);
		CharacterMetricNode.WriteUInt32("GlyphIndex", FontCharacters[I].GlyphIndex);
		CharacterMetricNode.WriteUInt32("FontSize", FontCharacters[I].FontSize);
		CharacterMetricNode.WriteInt32("Height", FontCharacters[I].Height);
		CharacterMetricNode.WriteInt32("Width", FontCharacters[I].Width);
		CharacterMetricNode.WriteInt32("HorizontalAdvance", FontCharacters[I].HorizontalAdvance);
		CharacterMetricNode.WriteInt32("VerticalAdvance", FontCharacters[I].VerticalAdvance);
		CharacterMetricNode.WriteInt32("HorizontalBearingX", FontCharacters[I].HorizontalBearingX);
		CharacterMetricNode.WriteInt32("HorizontalBearingY", FontCharacters[I].HorizontalBearingY);
		CharacterMetricNode.WriteInt32("VerticalBearingX", FontCharacters[I].VerticalBearingX);
		CharacterMetricNode.WriteInt32("VerticalBearingY", FontCharacters[I].VerticalBearingY);
		CharacterMetricNode.WriteVector2("LeftUp", FontCharacters[I].LeftUp);
		CharacterMetricNode.WriteVector2("RightDown", FontCharacters[I].RightDown);
		CharacterMetricNode.WriteUInt32("TextureID", FontCharacters[I].TextureID);
		CharacterMetricNode.CloseNode();
	}

	CharactersNode.CloseNode();
	FreeTypeRootNode.CloseNode();
	FreeTypeFontWriter.Close();
	CharacterMetricsOutput->Close();

	return true;
}

bool ZEFontBaker::BakeFont(const char StartCharacter, const char EndCharacter, 
									ZEString FontFilePath, ZEString OutputFilePath, ZEString OutputFileName, ZEUInt32 FontSize, 
									ZEUInt32 HorizontalOutputDPI, ZEUInt32 VerticalOutputDPI, ZEUInt32 PointFactor, 
									ZEUInt32 TextureWidth, ZEUInt32 TextureHeight,
									ZEUInt32 MarginTop, ZEUInt32 MarginBottom, ZEUInt32 MarginLeft, ZEUInt32 MarginRight,
									ZEUInt32 HorizontalPadding, ZEUInt32 VerticalPadding)
{
	ZEString CharacterSequence;

	if(StartCharacter < EndCharacter)
		for(ZESize I = StartCharacter; I <= EndCharacter; I++)
			CharacterSequence.Append(ZEString::FromChar(I));
	else
		for(ZESize I = EndCharacter; I <= StartCharacter; I++)
			CharacterSequence.Append(ZEString::FromChar(I));


	return BakeFont(CharacterSequence, FontFilePath, OutputFilePath, OutputFileName, FontSize, 
					HorizontalOutputDPI, VerticalOutputDPI, PointFactor, 
					TextureWidth, TextureHeight, 
					MarginTop, MarginBottom, MarginLeft, MarginRight, 
					HorizontalPadding, VerticalPadding);
}
