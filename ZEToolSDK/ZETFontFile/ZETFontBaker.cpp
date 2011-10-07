//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETFontBaker.cpp
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

#include "ZETFontBaker.h"

#define WINDOWS_MEAN_AND_LEAN
#include <windows.h>

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
                    sizeof(RGBQUAD) * (1<< cClrBits)); 
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

void DumpTexture(ZEResourceInternalFile* TextureFile, HBITMAP Bitmap, HDC DC) 
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

	TextureFile->AllocateBuffer(hdr.bfSize);

    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof (RGBQUAD); 

	unsigned char* Data = TextureFile->GetFileBuffer();
	memcpy(Data, &hdr, sizeof(BITMAPFILEHEADER));
	Data += sizeof(BITMAPFILEHEADER);

 	memcpy(Data, pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD));
	Data += sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD);

	dwTotal = cb = pbih->biSizeImage; 
 
	struct ARGB
	{
		unsigned char b;
		unsigned char g;
		unsigned char r;
		unsigned char a;
	} *color;

	hp = lpBits; 
	
	for (size_t I = 0; I < cb / 4; I++)
	{
		color = ((ARGB*)hp) + I;
		color->r = color->g = color->b = color->a = (color->r + color->g + color->b) / 3;
		//(color->r != 0 ? 255 : 0);
	}
 	
	memcpy(Data, hp, cb);
	Data += cb;

    GlobalFree((HGLOBAL)lpBits);
}

bool ZEFontBaker:: BakeFont(ZEFont* Output, 
							const char* FontName, int FontSize,
							bool FontItalic, bool FontBold,	bool FontUnderLine, bool FontStrikeOut,
							int TextureWidth, int TextureHeight, 
							int StartCharacter, int EndCharacter,
							int CharacterSpacingX, int CharacterSpacingY,
							int LeftMargin, int TopMargin, int RightMargin, int BottomMargin, 
							bool GenerateCoordsOnly )
{
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

	int OffsetX = LeftMargin + CharacterSpacingX;
	int OffsetY = TopMargin + CharacterSpacingY;

	int* CharacterWidths = new int[EndCharacter - StartCharacter];
	GetCharWidth(dc, StartCharacter, EndCharacter, CharacterWidths);

	TEXTMETRIC	Metric;
	GetTextMetrics(dc, &Metric);

	Output->Textures.Clear();

	int BitmapId = 0;
	RECT CharacterRectangle;
	for (int I = 0; I <= 255; I++)
	{
		if (I < StartCharacter || I > EndCharacter)
			ZeroMemory(&Output->Characters[I], sizeof(ZEFontCharacter));
	}

	for (int I = StartCharacter; I <= EndCharacter; I++)
	{
		char Character = I;
		if (OffsetX + CharacterWidths[I - StartCharacter] + RightMargin >= TextureWidth)
		{
			OffsetX = RightMargin + CharacterSpacingX;
			OffsetY += Metric.tmHeight + CharacterSpacingY;
			if (OffsetY + Metric.tmHeight + BottomMargin >= TextureHeight)
			{
				if (!GenerateCoordsOnly)
				{
					ZEResourceInternalFile* NewTexture = Output->Textures.Add();
					DumpTexture(NewTexture, Bitmap, dc);
					DeleteObject(Bitmap);
					Bitmap = CreateBitmap(TextureWidth, TextureHeight, 1, 32, NULL);
					SelectObject(dc, Bitmap);
					OffsetX = LeftMargin + CharacterSpacingX;
					OffsetY = TopMargin + CharacterSpacingY;
				}
				BitmapId++;
			}
		}
		Output->Characters[I].TextureId = BitmapId;

		Output->Characters[I].Coordinates = ZERectangle(ZEVector2(ZEFONT_PIXELTOUV(OffsetX, TextureWidth), ZEFONT_PIXELTOUV(OffsetY, TextureHeight)), ZEFONT_PIXELTOUV(CharacterWidths[I - StartCharacter], TextureWidth), ZEFONT_PIXELTOUV(Metric.tmHeight, TextureHeight));
		CharacterRectangle.left = OffsetX;
		CharacterRectangle.top = OffsetY;
		CharacterRectangle.right = OffsetX + CharacterWidths[I - StartCharacter];
		CharacterRectangle.bottom = OffsetY + Metric.tmHeight;
		
		if (!GenerateCoordsOnly)
			DrawText(dc, &Character, 1, &CharacterRectangle, NULL);

		OffsetX += CharacterWidths[I - StartCharacter] + CharacterSpacingX;
	}

	if (!GenerateCoordsOnly)
	{
		ZEResourceInternalFile* NewTexture = Output->Textures.Add();
		DumpTexture(NewTexture, Bitmap, dc);
		DeleteObject(Bitmap);
	}
	DeleteDC(dc);
	return true;
}
