//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureTestUtils.cpp
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

#include "ZEBitmap.h"
#include "ZEPixel.h"
#include "ZEFile/ZEFile.h"
#include "ZETextureTestUtils.h"
#include "ZEDS/ZEFormat.h"
#include "ZETextureLoader.h"

ZESize ZETextureTestUtils::GetPixelSize(ZETexturePixelFormat Format)
{
	switch(Format)
	{
		case ZE_TPF_RGBA8:
			return 4;

		case ZE_TPF_RGBA16:
			return 8;

		case ZE_TPF_R16F:
			return 2;

		case ZE_TPF_RG16F:
			return 4;;

		case ZE_TPF_RGBA16F:
			return 16;

		case ZE_TPF_R32F:
			return 8;

		case ZE_TPF_RG32F:
			return 16;

		case ZE_TPF_RGBA32F:
			return 32;

		case ZE_TPF_L8:
			return 1;

		case ZE_TPF_LA8:
			return 2;

		case ZE_TPF_L16:
			return 2;

		case ZE_TPF_LA16:
			return 4;

		default:
			return 0;
	}
}

void a(const char* A)
{

}

void ZETextureTestUtils::DumpAsTGA(const char* FileName, ZETextureData* Data)
{
	for (ZESize I = 0; I < Data->GetSurfaces().GetCount(); I++)
		DumpAsTGA(ZEFormat::Format("{0}-surface{1}", FileName, I), &Data->GetSurfaces()[I]);
}

void ZETextureTestUtils::DumpAsTGA(const char* FileName, ZETextureSurface* Surface)
{
	for (ZESize I = 0; I < Surface->GetLevels().GetCount(); I++)
		DumpAsTGA(ZEFormat::Format("{0}-level{1}.tga", FileName, I), &Surface->GetLevels()[I]);
}

void ZETextureTestUtils::DumpAsTGA(const char* FileName, ZETextureLevel* Level)
{
	ZEBitmap Bitmap;
	Bitmap.Create(Level->GetWidth(), Level->GetHeight(), 32);
	ZEPixelColor* Destination = Bitmap.GetPixels();
	void* Source = Level->GetData();

	ZETexturePixelFormat Format = Level->GetOwner()->GetOwner()->GetPixelFormat();
	for (ZESize y = 0; y < Level->GetHeight(); y++)
	{
		for (ZESize x = 0; x < Level->GetWidth(); x++)
		{
			ZEPixelColor* Current = &Bitmap.GetPixel(x, y);
			switch(Format)
			{
				case ZE_TPF_RGBA8:
				{
					ZEPixelRGBA8* Pixel = (ZEPixelRGBA8*)Source + Level->GetWidth() * y + x;
					Current->a = Pixel->A;
					Current->r = Pixel->R;
					Current->g = Pixel->G;
					Current->b = Pixel->B;
					break;
				}

				case ZE_TPF_RGBA16:
				{
					ZEPixelRGBA16* Pixel = (ZEPixelRGBA16*)Source + Level->GetWidth() * y + x;
					Current->a = Pixel->A >> 8;
					Current->r = Pixel->R >> 8;
					Current->g = Pixel->G >> 8;
					Current->b = Pixel->B >> 8;
					break;
				}

				case ZE_TPF_R32F:
				{
					ZEPixelR32F* Pixel = (ZEPixelR32F*)Source + Level->GetWidth() * y + x;
					Current->a = 255;
					Current->r = ZEMath::Saturate(Pixel->R) * 255;
					Current->g = 0;
					Current->b = 0;
					break;
				}

				case ZE_TPF_RG32F:
				{
					ZEPixelRG32F* Pixel = (ZEPixelRG32F*)Source + Level->GetWidth() * y + x;
					Current->a = 0;
					Current->r = ZEMath::Saturate(Pixel->R) * 255;
					Current->g = ZEMath::Saturate(Pixel->G) * 255;
					Current->b = 0;
					break;
				}

				case ZE_TPF_RGBA32F:
				{
					ZEPixelRGBA32F* Pixel = (ZEPixelRGBA32F*)Source + Level->GetWidth() * y + x;
					Current->a = ZEMath::Saturate(Pixel->A) * 255;
					Current->r = ZEMath::Saturate(Pixel->R) * 255;
					Current->g = ZEMath::Saturate(Pixel->G) * 255;
					Current->b = ZEMath::Saturate(Pixel->B) * 255;
					break;
				}

				case ZE_TPF_L8:
				{
					ZEPixelL8* Pixel = (ZEPixelL8*)Source + Level->GetWidth() * y + x;
					Current->a = 255;
					Current->r = Pixel->L;
					Current->g = Pixel->L;
					Current->b = Pixel->L;
					break;
				}

				case ZE_TPF_LA8:
				{
					ZEPixelLA8* Pixel = (ZEPixelLA8*)Source + Level->GetWidth() * y + x;
					Current->a = Pixel->A;
					Current->r = Pixel->L;
					Current->g = Pixel->L;
					Current->b = Pixel->L;
					break;
				}

				case ZE_TPF_L16:
				{
					ZEPixelL16* Pixel = (ZEPixelL16*)Source + Level->GetWidth() * y + x;
					Current->a = 255;
					Current->r = Pixel->L >> 8;
					Current->g = Pixel->L >> 8;
					Current->b = Pixel->L >> 8;
					break;
				}

				case ZE_TPF_LA16:
				{
					ZEPixelLA16* Pixel = (ZEPixelLA16*)Source + Level->GetWidth() * y + x;
					Current->a = Pixel->A >> 8;
					Current->r = Pixel->L >> 8;
					Current->g = Pixel->L >> 8;
					Current->b = Pixel->L >> 8;
					break;
				}

				case ZE_TPF_R16F:
				case ZE_TPF_RG16F:
				case ZE_TPF_RGBA16F:
				default:
				{
					Current->a = 0;
					Current->r = 0;
					Current->g = 0;
					Current->b = 0;
					break;
				}
			}
		}
	}

	Bitmap.Save(FileName, ZE_BFF_TGA);
}

bool ZETextureTestUtils::Compare(ZETextureLevel* LevelA, ZETextureLevel* LevelB)
{
	if (LevelA->GetWidth() != LevelB->GetWidth() || 
		LevelA->GetHeight() != LevelB->GetHeight() ||
		LevelA->GetOwner()->GetOwner()->GetPixelFormat() != LevelB->GetOwner()->GetOwner()->GetPixelFormat())
		return false;

	return memcmp(LevelA->GetData(), LevelB->GetData(), LevelA->GetSize()) == 0;
}

bool ZETextureTestUtils::Compare(ZETextureSurface* SurfaceA, ZETextureSurface* SurfaceB)
{
	if (SurfaceA->GetLevels().GetCount() != SurfaceB->GetLevels().GetCount())
		return false;

	for (ZESize I = 0; I < SurfaceA->GetLevels().GetCount(); I++)
		if (!Compare(&SurfaceA->GetLevels()[I], &SurfaceB->GetLevels()[I]))
			return false;

	return true;
}

bool ZETextureTestUtils::Compare(const ZETextureDataInfo* InfoA, const ZETextureDataInfo* InfoB)
{
	return memcmp(InfoA, InfoB, sizeof(ZETextureDataInfo)) == 0;
}

bool ZETextureTestUtils::Compare(ZETextureData* Data, const ZEString& FileName)
{
	ZEFile File;
	if (!File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE))
		return false;

	ZETextureData DataB;
	if (!ZETextureLoader::LoadFromFile(&File, &DataB))
		return false;

	return Compare(Data, &DataB);
}

bool ZETextureTestUtils::Compare(ZETextureData* DataA, ZETextureData* DataB)
{
	if (!Compare(&DataA->GetInfo(), &DataB->GetInfo()))
		return false;

	for (ZESize I = 0; I < DataA->GetSurfaces().GetCount(); I++)
		if (!Compare(&DataA->GetSurfaces()[I], &DataB->GetSurfaces()[I]))
			return false;

	return true;
}
