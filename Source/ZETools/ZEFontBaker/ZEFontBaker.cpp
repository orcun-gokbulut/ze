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
#define WINDOWS_MEAN_AND_LEAN
#include <windows.h>
#include "ZEFile\ZEFile.h"
#include "ZEML\ZEMLWriter.h"
#include "ZETexture\ZETextureData.h"
#include "ZETexture\ZETextureLoader.h"
#include "ZEFile\ZEDirectoryInfo.h"
#include "ZETexture\ZEBitmap.h"

#define ZEFONT_PIXELTOUV(Pixel, Dimention) ((float)Pixel * (1.0f / (float)Dimention))
#define ZEFONT_UVTOPIXEL(UV, Dimention) ((float)UV * (float)Dimention)

PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
{ 
	BITMAP bmp; 
	PBITMAPINFO pbmi; 
	WORD    cClrBits; 

	if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) 
		printf("GetObject"); 

	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
	if (cClrBits == 1) 
		cClrBits = 1; 
	else if (cClrBits <= 4) 
		cClrBits = 4; 
	else if (cClrBits <= 8) 
		cClrBits = 8; 
	else if (cClrBits <= 16) 
		cClrBits = 16; 
	else if (cClrBits <= 24) 
		cClrBits = 24; 
	else cClrBits = 32; 

	if (cClrBits != 24) 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
		sizeof(BITMAPINFOHEADER) + 
		sizeof(RGBQUAD) * (ZESize)(1<< cClrBits));
	else 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
		sizeof(BITMAPINFOHEADER)); 

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	pbmi->bmiHeader.biWidth = bmp.bmWidth; 
	pbmi->bmiHeader.biHeight = bmp.bmHeight; 
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
	if (cClrBits < 24) 
		pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

	pbmi->bmiHeader.biCompression = BI_RGB; 
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
		* pbmi->bmiHeader.biHeight; 
	pbmi->bmiHeader.biClrImportant = 0; 
	return pbmi; 
} 

void DumpTexture(const ZEString& FontFile, ZEUInt32 TextureNumber, HBITMAP Bitmap, HDC DC) 
{ 
	BITMAPFILEHEADER hdr;
	PBITMAPINFOHEADER pbih;
	LPBYTE lpBits;
	DWORD dwTotal; 
	DWORD cb; 
	BYTE *hp;

	PBITMAPINFO pbi = CreateBitmapInfoStruct(Bitmap);

	pbih = (PBITMAPINFOHEADER) pbi; 
	lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits) 
	{}  //printf("GlobalAlloc"); 

	if (!GetDIBits(DC, Bitmap, 0, (WORD) pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS)) 
	{
		//printf("GetDIBits"); 
	}
	hdr.bfType = 0x4d42;
	hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage); 
	hdr.bfReserved1 = 0; 
	hdr.bfReserved2 = 0; 


	unsigned char* TextureBuffer = new unsigned char[(ZESize)hdr.bfSize];
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof (RGBQUAD); 

	void* Data = TextureBuffer;
	memcpy(Data, &hdr, sizeof(BITMAPFILEHEADER));
	Data = (ZEUInt8*)Data + sizeof(BITMAPFILEHEADER);
	memcpy(Data, pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD));
	Data = (ZEUInt8*)Data + sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD);
	dwTotal = cb = pbih->biSizeImage; 

	struct ARGB
	{
		unsigned char b;
		unsigned char g;
		unsigned char r;
		unsigned char a;
	}*color;

	hp = lpBits; 

	for (ZESize I = 0; I < (ZESize)cb / 4; I++)
	{
		color = ((ARGB*)hp) + I;
		color->r = color->g = color->b = color->a = (color->r + color->g + color->b) / 3;
		//(color->r != 0 ? 255 : 0);
	}

	memcpy(Data, hp, (ZESize)cb);
	Data = (ZEUInt8*)Data + (ZESize)cb;

	ZEFile TextureFile;
	TextureFile.Open(ZEString(ZEDirectoryInfo(FontFile).GetParentDirectory() + "\\" + TextureNumber + ".bmp"), ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);

	if(TextureFile.Write(TextureBuffer , cb, 1) != 1)
		zeError("Can not write texture %d to file. File name : ", TextureNumber, FontFile.ToCString());

	if(TextureFile.IsOpen())
		TextureFile.Close();

	delete[] TextureBuffer;
	TextureBuffer = NULL;

	GlobalFree((HGLOBAL)lpBits);
}

ZEUInt32 ZEFontBaker::TextureCount = 0;

bool ZEFontBaker::BakeFont(const char* FileName, 
	const char* FontName, ZEInt FontSize,
	bool FontItalic, bool FontBold,	bool FontUnderLine, bool FontStrikeOut,
	ZEInt TextureWidth, ZEInt TextureHeight, 
	ZEInt StartCharacter, ZEInt EndCharacter,
	ZEInt CharacterSpacingX, ZEInt CharacterSpacingY,
	ZEInt LeftMargin, ZEInt TopMargin, ZEInt RightMargin, ZEInt BottomMargin, 
	bool GenerateCoordsOnly)
{
	ZEFile FontFile;
	TextureCount = 0;

	if(!FontFile.Open(FileName, ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE))
	{
		zeError("Can not open font file to write. File name : %s", FileName);
		return false;
	}

	ZEMLWriter FontWriter;

	if (!FontWriter.Open(&FontFile))
	{
		zeError("Can not write font file.");
		return false;
	}

	ZEMLWriterNode FontRootNode = FontWriter.WriteRootNode("ZEFont");
	ZEMLWriterNode TexturesNode;

	if(!GenerateCoordsOnly)
		TexturesNode = FontRootNode.OpenSubNode("Textures");

	HDC dc;
	HBITMAP Bitmap;
	HFONT Font;
	Font = CreateFont(FontSize, 0, 0,  0, (FontBold ? FW_BOLD : FW_NORMAL), FontItalic, FontUnderLine, FontStrikeOut,     
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, FontName);

	if (!GenerateCoordsOnly)
	{
		dc = CreateCompatibleDC(NULL);
		Bitmap = CreateBitmap(TextureWidth, TextureHeight, 1, 32, NULL);
		SelectObject(dc, Bitmap);
	}
	SelectObject(dc, Font);
	SetMapMode(dc, MM_TEXT);
	SetBkMode(dc, TRANSPARENT);
	SetTextColor(dc, RGB(255, 255, 255));

	ZEInt OffsetX = LeftMargin + CharacterSpacingX;
	ZEInt OffsetY = TopMargin + CharacterSpacingY;

	ZEInt* CharacterWidths = new ZEInt[(ZESize)(EndCharacter - StartCharacter)];
	GetCharWidth(dc, StartCharacter, EndCharacter, CharacterWidths);

	TEXTMETRIC	Metric;
	GetTextMetrics(dc, &Metric);

	ZEArray<ZEFontCharacter> Characters;
	Characters.SetCount(EndCharacter - StartCharacter);

	ZEInt BitmapId = 0;
	RECT CharacterRectangle;

	for (ZESize I = 0; I < Characters.GetCount(); I++)
		ZeroMemory(&Characters[I], sizeof(ZEFontCharacter));

	for (ZESize I = 0; I < Characters.GetCount(); I++)
	{
		Characters[I].Value = StartCharacter + I;
		
		if (OffsetX + CharacterWidths[I] + RightMargin >= TextureWidth)
		{
			OffsetX = RightMargin + CharacterSpacingX;
			OffsetY += Metric.tmHeight + CharacterSpacingY;
			if (OffsetY + Metric.tmHeight + BottomMargin >= TextureHeight)
			{
				if (!GenerateCoordsOnly)
				{
					ZEMLWriterNode TextureNode = TexturesNode.OpenSubNode("Texture");
					TextureNode.WriteString("FileName", ZEString(TextureCount) + ".bmp");
					DumpTexture(FileName, TextureCount, Bitmap, dc);
					DeleteObject(Bitmap);
					Bitmap = CreateBitmap(TextureWidth, TextureHeight, 1, 32, NULL);
					SelectObject(dc, Bitmap);
					OffsetX = LeftMargin + CharacterSpacingX;
					OffsetY = TopMargin + CharacterSpacingY;
					TextureNode.CloseNode();
					TextureCount++;
				}
				BitmapId++;
			}
		}

		Characters[I].TextureId = BitmapId;
		Characters[I].Coordinates = ZERectangle(ZEVector2(ZEFONT_PIXELTOUV(OffsetX, TextureWidth), ZEFONT_PIXELTOUV(OffsetY, TextureHeight)), ZEFONT_PIXELTOUV(CharacterWidths[I], TextureWidth), ZEFONT_PIXELTOUV(Metric.tmHeight, TextureHeight));
		CharacterRectangle.left = OffsetX;
		CharacterRectangle.top = OffsetY;
		CharacterRectangle.right = OffsetX + CharacterWidths[I];
		CharacterRectangle.bottom = OffsetY + Metric.tmHeight;

		if (!GenerateCoordsOnly)
			DrawText(dc, (char*)(&Characters[I].Value), 1, &CharacterRectangle, NULL);

		OffsetX += CharacterWidths[I] + CharacterSpacingX;
	}

	if (!GenerateCoordsOnly)
	{
		ZEMLWriterNode TextureNode = TexturesNode.OpenSubNode("Texture");
		TextureNode.WriteString("FileName", ZEString(TextureCount) + ".bmp");
		DumpTexture(FileName, TextureCount, Bitmap, dc);
		DeleteObject(Bitmap);
		TextureNode.CloseNode();
		TextureCount++;

		TexturesNode.CloseNode();
	}

	FontRootNode.WriteUInt32("TextureCount", TextureCount);
	FontRootNode.WriteData("Characters", Characters.GetCArray(), Characters.GetCount() * sizeof(ZEFontCharacter));

	FontRootNode.CloseNode();
	FontWriter.Close();
	FontFile.Close();

	DeleteDC(dc);
	return true;
}
