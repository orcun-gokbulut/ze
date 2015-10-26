//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEHashGenerator.cpp
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

#include "ZEHashGenerator.h"

#if (defined(__GNUC__) && defined(__i386__)) || defined(_MSC_VER)
	#define Convert16Bit(d) (*((const ZEUInt16*) (d)))
#else
	#define Convert16Bit(d) ((((ZEUInt32)(((const ZEUInt8*)(d))[1])) << 8) + (uint32_t)(((const ZEUInt8*)(d))[0]))
#endif

ZEUInt32 ZEHashGenerator::Hash(const void* Input, ZESize Size) 
{
	ZEBYTE* Data = (ZEBYTE*)Input;
	ZEUInt32 Output = Size;

	ZEUInt32 Temp;
	ZEInt32 Reminder;

	if (Data == NULL || Size == 0) 
		return 0;

	Reminder = Size & 3;
	Size >>= 2;

	for (; Size > 0; Size--) 
	{
		Output += Convert16Bit(Data);
		Temp = (Convert16Bit(Data + 2) << 11) ^ Output;
		Output = (Output << 16) ^ Temp;
		Output += Output >> 11;
		Data += 2 * sizeof(ZEUInt16);

	}

	switch (Reminder) 
	{
		case 3: 
			Output += Convert16Bit (Data);
			Output ^= Output << 16;
			Output ^= ((ZEInt8)Data[sizeof (ZEUInt16)]) << 18;
			Output += Output >> 11;
			break;

		case 2: 
			Output += Convert16Bit(Data);
			Output ^= Output << 11;
			Output += Output >> 17;
			break;

		case 1: 
			Output += (ZEInt8)*Data;
			Output ^= Output << 10;
			Output += Output >> 1;
	}

	Output ^= Output << 3;
	Output += Output >> 5;
	Output ^= Output << 4;
	Output += Output >> 17;
	Output ^= Output << 25;
	Output += Output >> 6;

	return Output;
}

/*
// Slow old code
ZESize ZEHashGenerator::Hash(void* Value, ZESize Size)
{
	ZEUInt32 Hash = 0;
	for (ZESize I = 0; I < Size; I++)
		Hash = (ZEUInt)((ZEUInt8*)Value)[I] + (Hash << 6) + (Hash << 16) - Hash;

	return Hash;
}*/

ZEUInt32 ZEHashGenerator::Hash(const char* String)
{
	if (String == NULL)
		return 0;

	ZEUInt32 Hash = 0;
	while(*String != '\0')
	{
		Hash = (ZEUInt)*String + (Hash << 6) + (Hash << 16) - Hash;
		String++;
	}

	return Hash;
}

ZEUInt32 ZEHashGenerator::Hash(const ZEString& String)
{
	return Hash(String.ToCString());
}
