//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETexturePixelConverter.cpp
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

#include "ZETexturePixelConverter.h"
#include "ZETextureData.h"

void ZETexturePixelConverter::ConvertIndexed(ZEPixelRGBA8* Destination, void* Source, ZESize Count, ZEPixelRGBA8* Palette)
{
	for (ZESize N = 0; N < Count; N++)
		Destination[N] = Palette[((ZEUInt8*)Source)[N]];	
}
void ZETexturePixelConverter::ConvertIndexed16(ZEPixelRGBA8* Destination, void* Source, ZESize Count, ZEPixelRGBA8* Palette)
{
	for (ZESize N = 0; N < Count; N++)
		if (((ZEUInt16*)Source)[N] > 0xFF)
			Destination[N].R = Destination[N].G = Destination[N].B = Destination[N].A = 0;
		else
			Destination[N] = Palette[((ZEUInt16*)Source)[N]];	
}

void ZETexturePixelConverter::ConvertL16_BE(ZEPixelL16* Destination, void* Source, ZESize Count)
{
	ZEPixelL16* End = Destination + Count;
	while(Destination < End)
	{
		Destination->L = ZEEndian::Little(*(ZEUInt16*)Source);
		Source = (ZEUInt8*)Source + 2;
	}
}

void ZETexturePixelConverter::ConvertLA16_BE(ZEPixelLA16* Destination, void* Source, ZESize Count)
{
	ZEPixelLA16* End = Destination + Count;
	while(Destination < End)
	{
		Destination->L = ZEEndian::Little(((ZEUInt16*)Source)[1]);
		Destination->A = ZEEndian::Little(((ZEUInt16*)Source)[0]);
		Source = (ZEUInt8*)Source + 4;
	}
}

void ZETexturePixelConverter::ConvertAL8(ZEPixelLA8* Destination, void* Source, ZESize Count)
{
	ZEPixelLA8* End = Destination + Count;
	while(Destination < End)
	{
		Destination->L = ZEEndian::Little(((ZEUInt8*)Source)[1]);
		Destination->A = ZEEndian::Little(((ZEUInt8*)Source)[0]);
		Source = (ZEUInt8*)Source + 2;
	}
}

void ZETexturePixelConverter::ConvertAL16(ZEPixelLA16* Destination, void* Source, ZESize Count)
{
	ZEPixelLA16* End = Destination + Count;
	while(Destination < End)
	{
		Destination->L = ZEEndian::Little(((ZEUInt16*)Source)[1]);
		Destination->A = ZEEndian::Little(((ZEUInt16*)Source)[0]);
		Source = (ZEUInt8*)Source + 4;
	}
}

void ZETexturePixelConverter::ConvertBGR555(ZEPixelRGBA8* Destination, void* Source, ZESize Count)
{
	ZEPixelRGBA8* End = Destination + Count;
	while(Destination < End)
	{
		#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
			Destination->R = *(ZEUInt16*)Source >> 7;
			Destination->G = *(ZEUInt16*)Source >> 2;
			Destination->B = (*(ZEUInt16*)Source << 3) & 0xF8;
			Destination->A = 0xFF;			
		#else	
			Destination->R = (((ZEUInt8*)Source)[1] << 1) & 0xF8;
			Destination->G = ((((ZEUInt8*)Source)[1] << 6) | (((ZEUInt8*)Source)[0] >> 2)) & 0xF8; 
			Destination->B = ((ZEUInt8*)Source)[0] << 3;
			Destination->A = 0xFF;
		#endif
		Destination++;
		Source = (ZEUInt8*)Source + 2;
	}
}

void ZETexturePixelConverter::ConvertBGRA1555(ZEPixelRGBA8* Destination, void* Source, ZESize Count)
{
	ZEPixelRGBA8* End = Destination + Count;
	while(Destination < End)
	{
		#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
			Destination->R = (*(ZEUInt16*)Source >> 7) & 0xF8;
			Destination->G = (*(ZEUInt16*)Source >> 2) & 0xF8;
			Destination->B = (*(ZEUInt16*)Source << 3) & 0xF8;
			Destination->A = (*(ZEUInt16*)Source & 0x8000) ? 0xFF : 0;			
		#else
			Destination->R = (((ZEUInt8*)Source)[1] << 1) & 0xF8;
			Destination->G = ((((ZEUInt8*)Source)[1] << 6) | (((ZEUInt8*)Source)[0] >> 2)) & 0xF8; 
			Destination->B = ((ZEUInt8*)Source)[0] << 3;
			Destination->A = (((ZEUInt8*)Source)[0] & 0x80) ? 0xFF : 0;
		#endif
		Source = (ZEUInt8*)Source + 2;
		Destination++;
	}
}

void ZETexturePixelConverter::ConvertBGRX1555(ZEPixelRGBA8* Destination, void* Source, ZESize Count)
{
	ZETexturePixelConverter::ConvertBGRA1555(Destination, Source, Count);
}

void ZETexturePixelConverter::ConvertBGR565(ZEPixelRGBA8* Destination, void* Source, ZESize Count)
{
	ZEPixelRGBA8* End = Destination + Count;
	while(Destination < End)
	{
		#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
			Destination->R = (*(ZEUInt16*)Source >> 8) & 0xF8;
			Destination->G = (*(ZEUInt16*)Source >> 3) & 0xFC;
			Destination->B = (*(ZEUInt16*)Source << 3) & 0xF8;
		#else
			Destination->R = (((ZEUInt8*)Source)[1]) & 0xF8;
			Destination->G = ((((ZEUInt8*)Source)[1] << 5) | (((ZEUInt8*)Source)[0] >> 3)) & 0xFC;
			Destination->B = ((ZEUInt8*)Source)[0] << 3;
		#endif
		Destination->A = 0xFF;

		Source = (ZEUInt8*)Source + 2;
		Destination++;
	}
}

void ZETexturePixelConverter::ConvertRGB8(ZEPixelRGBA8* Destination, void* Source, ZESize Count)
{
	ZEPixelRGBA8* End = Destination + Count;
	while(Destination < End)
	{
		Destination->B = ((ZEUInt8*)Source)[2];
		Destination->G = ((ZEUInt8*)Source)[1];
		Destination->R = ((ZEUInt8*)Source)[0];
		Destination->A = 0xFF;

		Source = (ZEUInt8*)Source + 3;
		Destination++;
	}
}

void ZETexturePixelConverter::ConvertBGR8(ZEPixelRGBA8* Destination, void* Source, ZESize Count)
{
	ZEPixelRGBA8* End = Destination + Count;
	int Valid = 0;
	while(Destination < End)
	{
		Destination->B = ((ZEUInt8*)Source)[0];
		Destination->G = ((ZEUInt8*)Source)[1];
		Destination->R = ((ZEUInt8*)Source)[2];
		Destination->A = 0xFF;

		Source = (ZEUInt8*)Source + 3;
		Destination++;
		Valid++;
	}
}

void ZETexturePixelConverter::ConvertBGRA8(ZEPixelRGBA8* Destination, void* Source, ZESize Count)
{
	ZEPixelRGBA8* End = Destination + Count;
	while(Destination < End)
	{
		#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
			*(ZEUInt32*)Destination  = *(ZEUInt32*)Source;
		#else
			Destination->B = ((ZEUInt8*)Source)[0];
			Destination->G = ((ZEUInt8*)Source)[1];
			Destination->R = ((ZEUInt8*)Source)[2];
			Destination->A = ((ZEUInt8*)Source)[4];
		#endif

		Source = (ZEUInt8*)Source + 4;
		Destination++;
	}
}

void ZETexturePixelConverter::ConvertBGRX8(ZEPixelRGBA8* Destination, void* Source, ZESize Count)
{
	ConvertBGRA8(Destination, Source, Count);
}

void ZETexturePixelConverter::ConvertRGB16(ZEPixelRGBA16* Destination, void* Source, ZESize Count)
{
	ZEPixelRGBA16* End = Destination + Count;
	while(Destination < End)
	{
		Destination->R = ((ZEUInt16*)Source)[0];
		Destination->G = ((ZEUInt16*)Source)[1];
		Destination->B = ((ZEUInt16*)Source)[2];
		Destination->A = 0xFFFF;

		Source = (ZEUInt8*)Source + 6;
		Destination++;
	}
}

void ZETexturePixelConverter::ConvertRGB16_BE(ZEPixelRGBA16* Destination, void* Source, ZESize Count)
{
	ZEPixelRGBA16* End = Destination + Count;
	while(Destination < End)
	{
		Destination->R = ZEEndian::Little(((ZEUInt16*)Source)[0]);
		Destination->G = ZEEndian::Little(((ZEUInt16*)Source)[1]);
		Destination->B = ZEEndian::Little(((ZEUInt16*)Source)[2]);
		Destination->A = 0xFFFF;

		Source = (ZEUInt8*)Source + 6;
		Destination++;
	}
}

void ZETexturePixelConverter::ConvertRGBA16_BE(ZEPixelRGBA16* Destination, void* Source, ZESize Count)
{
	ZEPixelRGBA16* End = Destination + Count;
	while(Destination < End)
	{
		Destination->R = ZEEndian::Little(((ZEUInt16*)Source)[0]);
		Destination->G = ZEEndian::Little(((ZEUInt16*)Source)[1]);
		Destination->B = ZEEndian::Little(((ZEUInt16*)Source)[2]);
		Destination->A = ZEEndian::Little(((ZEUInt16*)Source)[3]);

		Source = (ZEUInt8*)Source + 8;
		Destination++;
	}
}
