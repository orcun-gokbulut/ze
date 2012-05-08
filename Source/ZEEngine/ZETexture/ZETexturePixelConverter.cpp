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

void ZETexturePixelConverter::ConvertIndexed(ZEARGB32* Destination, void* Source, ZESize Count, ZEARGB32* Palette)
{
	for (ZESize N = 0; N < Count; N++)
		Destination[N] = Palette[((ZEUInt8*)Source)[N]];	
}

void ZETexturePixelConverter::ConvertG8(ZEARGB32* Destination, void* Source, ZESize Count)
{
	for (ZESize I = 0; I < Count; I++)
		((ZEUInt32*)Destination)[I] = (0xFF000000 | (ZEUInt32)((ZEUInt8*)Source)[I] << 8 | (ZEUInt32)((ZEUInt8*)Source)[I] << 8 | ((ZEUInt8*)Source)[I]);
}

void ZETexturePixelConverter::ConvertG16(ZEARGB32* Destination, void* Source, ZESize Count)
{
	for (ZESize I = 0; I < Count; I++)
	{
		ZEUInt16 Value = *(ZEUInt16*)((ZEUInt8*)Source) >> 8;
		((ZEUInt32*)Destination)[I] = (0xFF000000 | (ZEUInt32)Value << 16 | (ZEUInt32)Value << 8 | Value);
	}
}

void ZETexturePixelConverter::ConvertGA16(ZEARGB32* Destination, void* Source, ZESize Count)
{

}

void ZETexturePixelConverter::ConvertAG16(ZEARGB32* Destination, void* Source, ZESize Count)
{

}

void ZETexturePixelConverter::ConvertGA32(ZEARGB32* Destination, void* Source, ZESize Count)
{

}

void ZETexturePixelConverter::ConvertAG32(ZEARGB32* Destination, void* Source, ZESize Count)
{

}

void ZETexturePixelConverter::ConvertBGR15(ZEARGB32* Destination, void* Source, ZESize Count)
{
	ZEARGB32* End = Destination + Count;
	while(Destination < End)
	{
		#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
			Destination->R = *(ZEUInt16*)Source >> 7;
			Destination->G = *(ZEUInt16*)Source >> 2;
			Destination->B = *(ZEUInt16*)Source & 0xF8;
			Destination->A = 255;			
		#else		
			Destination->R = (((ZEUInt8*)Source)[1] << 1) & 0xF8;
			Destination->G = ((((ZEUInt8*)Source)[1] << 6) | (((ZEUInt8*)Source)[0] >> 2)) & 0xF8; 
			Destination->B = ((ZEUInt8*)Source)[0] << 3;
			Destination->A = 255;
		#endif
		Destination++;
		Source = (ZEUInt8*)Source + 2;
	}
}

void ZETexturePixelConverter::ConvertRGB15(ZEARGB32* Destination, void* Source, ZESize Count)
{
	ZEARGB32* End = Destination + Count;
	while(Destination < End)
	{
		#ifdef ZE_PLATFORM_ENDIANNESS_BIG
			Destination->R = *(ZEUInt16*)Source >> 7;
			Destination->G = *(ZEUInt16*)Source >> 2;
			Destination->B = *(ZEUInt16*)Source & 0xF8;
			Destination->A = 255;			
		#else		
			Destination->R = (((ZEUInt8*)Source)[0] << 1) & 0xF8;
			Destination->G = ((((ZEUInt8*)Source)[0] << 6) | (((ZEUInt8*)Source)[1] >> 2)) & 0xF8; 
			Destination->B = ((ZEUInt8*)Source)[1] << 3;
			Destination->A = 255;
		#endif
		Destination++;
		Source = (ZEUInt8*)Source + 2;
	}
}

void ZETexturePixelConverter::ConvertBGRA16(ZEARGB32* Destination, void* Source, ZESize Count)
{
	ZEARGB32* End = Destination + Count;
	while(Destination < End)
	{
		#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
			Destination->R = (*(ZEUInt16*)Source >> 7) & 0xF8;
			Destination->G = (*(ZEUInt16*)Source >> 2) & 0xF8;
			Destination->B = *(ZEUInt16*)Source & 0xF8;
			Destination->A = (*(ZEUInt16*)Source & 0x8000) ? 255 : 0;			
		#else
			Destination->R = (((ZEUInt8*)Source)[1] << 1) & 0xF8;
			Destination->G = ((((ZEUInt8*)Source)[1] << 6) | (((ZEUInt8*)Source)[0] >> 2)) & 0xF8; 
			Destination->B = ((ZEUInt8*)Source)[0] << 3;
			Destination->A = (((ZEUInt8*)Source)[0] & 0x80) ? 255 : 0;
		#endif
		Source = (ZEUInt8*)Source + 2;
		Destination++;
	}
}

void ZETexturePixelConverter::ConvertARGB16(ZEARGB32* Destination, void* Source, ZESize Count)
{
	ZEARGB32* End = Destination + Count;
	while(Destination < End)
	{
		#ifdef ZE_PLATFORM_ENDIANNESS_BIG
			Destination->R = (*(ZEUInt16*)Source >> 7) & 0xF8;
			Destination->G = (*(ZEUInt16*)Source >> 2) & 0xF8;
			Destination->B = *(ZEUInt16*)Source & 0xF8;
			Destination->A = (*(ZEUInt16*) & 0x8000) ? 255 : 0;			
		#else
			Destination->R = (((ZEUInt8*)Source)[0] << 1) & 0xF8;
			Destination->G = ((((ZEUInt8*)Source)[0] << 6) | (((ZEUInt8*)Source)[1] >> 2)) & 0xF8; 
			Destination->B = ((ZEUInt8*)Source)[1] << 3;
			Destination->A = (((ZEUInt8*)Source)[0] & 0x80) ? 255 : 0;
		#endif

		Source = (ZEUInt8*)Source + 2;
		Destination++;
	}
}

void ZETexturePixelConverter::ConvertBGR16(ZEARGB32* Destination, void* Source, ZESize Count)
{
	ZEARGB32* End = Destination + Count;
	while(Destination < End)
	{
		#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
			Destination->R = (*(ZEUInt16*)Source >> 7) & 0xF8;
			Destination->G = (*(ZEUInt16*)Source >> 2) & 0xF8;
			Destination->B = *(ZEUInt16*)Source & 0xF8;
			Destination->A = 255;			
		#else
			Destination->R = (((ZEUInt8*)Source)[1] << 1) & 0xF8;
			Destination->G = ((((ZEUInt8*)Source)[1] << 6) | (((ZEUInt8*)Source)[0] >> 2)) & 0xF8; 
			Destination->B = ((ZEUInt8*)Source)[0] << 3;
			Destination->A = 255;
		#endif

		Source = (ZEUInt8*)Source + 2;
		Destination++;
	}
}

void ZETexturePixelConverter::ConvertRGB16(ZEARGB32* Destination, void* Source, ZESize Count)
{
	ZEARGB32* End = Destination + Count;
	while(Destination < End)
	{
		#ifdef ZE_PLATFORM_ENDIANNESS_BIG
			Destination->R = (*(ZEUInt16*)Source >> 7) & 0xF8;
			Destination->G = (*(ZEUInt16*)Source >> 2) & 0xF8;
			Destination->B = *(ZEUInt16*)Source & 0xF8;
			Destination->A = 255;			
		#else
			Destination->R = (((ZEUInt8*)Source)[0] << 1) & 0xF8;
			Destination->G = ((((ZEUInt8*)Source)[0] << 6) | (((ZEUInt8*)Source)[1] >> 2)) & 0xF8; 
			Destination->B = ((ZEUInt8*)Source)[1] << 3;
			Destination->A = 255;
		#endif

		Source = (ZEUInt8*)Source + 2;
		Destination++;
	}
}

void ZETexturePixelConverter::ConvertBGR24(ZEARGB32* Destination, void* Source, ZESize Count)
{
	ZEARGB32* End = Destination + Count;
	while(Destination < End)
	{
		#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
			*(ZEUInt32*)Destination  = *(ZEUInt32*)Source;
			Destination[3].A = 255;
		#else
			Destination->B = ((ZEUInt8*)Source)[0];
			Destination->G = ((ZEUInt8*)Source)[1];
			Destination->R = ((ZEUInt8*)Source)[2];
			Destination->A = 255;
		#endif

		Source = (ZEUInt8*)Source + 3;
		Destination++;
	}
}

void ZETexturePixelConverter::ConvertRGB24(ZEARGB32* Destination, void* Source, ZESize Count)
{
	ZEARGB32* End = Destination + Count;
	while(Destination < End)
	{
		#ifdef ZE_PLATFORM_ENDIANNESS_BIG
			*(ZEUInt32*)Destination  = *(ZEUInt32*)Source;
		#else
			Destination->B = ((ZEUInt8*)Source)[2];
			Destination->G = ((ZEUInt8*)Source)[1];
			Destination->R = ((ZEUInt8*)Source)[0];
			Destination->A = 255;
		#endif

		Source = (ZEUInt8*)Source + 4;
		Destination++;
	}
}

void ZETexturePixelConverter::ConvertBGRA32(ZEARGB32* Destination, void* Source, ZESize Count)
{
	ZEARGB32* End = Destination + Count;
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

void ZETexturePixelConverter::ConvertARGB32(ZEARGB32* Destination, void* Source, ZESize Count)
{
	ZEARGB32* End = Destination + Count;
	while(Destination < End)
	{
		#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
			*(ZEUInt32*)Destination  = *(ZEUInt32*)Source;
		#else
			Destination->A = ((ZEUInt8*)Source)[3];
			Destination->B = ((ZEUInt8*)Source)[2];
			Destination->G = ((ZEUInt8*)Source)[1];
			Destination->R = ((ZEUInt8*)Source)[0];
		#endif

		Source = (ZEUInt8*)Source + 4;
		Destination++;
	}
}

void ZETexturePixelConverter::ConvertABGR32(ZEARGB32* Destination, void* Source, ZESize Count)
{
	ZEARGB32* End = Destination + Count;
	while(Destination < End)
	{
		Destination->A = ((ZEUInt8*)Source)[0];
		Destination->B = ((ZEUInt8*)Source)[1];
		Destination->G = ((ZEUInt8*)Source)[2];
		Destination->R = ((ZEUInt8*)Source)[3];

		Source = (ZEUInt8*)Source + 4;
		Destination++;
	}
}

void ZETexturePixelConverter::ConvertRGBA32(ZEARGB32* Destination, void* Source, ZESize Count)
{
	ZEARGB32* End = Destination + Count;
	while(Destination < End)
	{
		Destination->R = ((ZEUInt8*)Source)[0];
		Destination->B = ((ZEUInt8*)Source)[2];
		Destination->G = ((ZEUInt8*)Source)[3];
		Destination->A = ((ZEUInt8*)Source)[4];

		Source = (ZEUInt8*)Source + 4;
		Destination++;
	}
}
