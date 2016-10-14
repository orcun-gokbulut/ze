//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEBase64.cpp
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

#include "ZEBase64.h"

#include <string>
#include "ZEMath\ZEMath.h"
#include "ZEDS\ZEArray.h"

static char EncodingTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static char DecodingTable[128] = 
{
	/*         0   1   2   3   4   5   6   7   8   9  */
	/*  00 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	/*  10 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	/*  20 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	/*  30 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	/*  40 */ -1, -1, -1, 62, -1, -1, -1, 63, 52, 53,
	/*  50 */ 54, 55, 56, 57, 58, 59, 60, 61, -1, -1,
	/*  60 */ -1, -1, -1, -1, -1,  0,  1,  2,  3,  4,
	/*  70 */  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	/*  80 */ 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
	/*  90 */ 25, -1, -1, -1, -1, -1, -1, 26, 27, 28,
	/* 100 */ 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
	/* 110 */ 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
	/* 120 */ 49, 50, 51, -1, -1, -1, -1, -1
};

bool ZEBase64::IsBase64(ZEBYTE Character)
{
	return (Character < 128) && (DecodingTable[Character] != -1);
}

bool ZEBase64::IsBase64(void* Data, ZESize Size)
{
	if (Size != 0 && (Size % 4) != 0)
		return false;

	for (ZESize I = 0; I < Size - 2; I++)
		if (!IsBase64(((ZEBYTE*)Data)[I]))
			return false;

	char A = ((ZEBYTE*)Data)[Size - 2];
	char B = ((ZEBYTE*)Data)[Size - 1];

	if ((A != '=' || B != '=') &&
		(!IsBase64(A) || B != '=') &&
		(!IsBase64(A) || !IsBase64(B)))
	{
		return false;
	}

	return true;
}

ZESize ZEBase64::EncodeSize(ZESize Size)
{
	return ((Size + 2) / 3) * 4; // Round Up Division of (Size / 3) * 4
}

ZESize ZEBase64::DecodeSize(void* Data, ZESize Size)
{
	if (Size == 0 || (Size % 4) != 0)
		return 0;

	return (Size / 4) * 3 + 
		(((ZEBYTE*)Data)[Size - 1] == '=' ? -1 : 0) +
		(((ZEBYTE*)Data)[Size - 2] == '=' ? -1 : 0);
}

void ZEBase64::Encode(void* Output, const void* Input, ZESize InputSize)
{

	ZESize I;
	for (I = 0; I < InputSize / 3; I++)
	{
		ZEBYTE* InputByte = (ZEBYTE*)Input + I * 3;
		ZEBYTE* OutputByte = (ZEBYTE*)Output + I * 4;

		OutputByte[0] = EncodingTable[InputByte[0] >> 2];
		OutputByte[1] = EncodingTable[(InputByte[0] << 4) & 0x3F | (InputByte[1] >> 4)];
		OutputByte[2] = EncodingTable[(InputByte[1] << 2) & 0x3F | (InputByte[2] >> 6)];
		OutputByte[3] = EncodingTable[InputByte[2] & 0x3F];
	}
	
	if ((InputSize % 3) == 0)
		return;

	ZEBYTE* InputByte = (ZEBYTE*)Input + I * 3;
	ZEBYTE* OutputByte = (ZEBYTE*)Output + I * 4;

	if ((InputSize % 3) == 1)
	{
		OutputByte[0] = EncodingTable[InputByte[0] >> 2];
		OutputByte[1] = EncodingTable[(InputByte[0] << 4) & 0x3F];
		OutputByte[2] = '=';
		OutputByte[3] = '=';
	}
	else if ((InputSize % 3) == 2)
	{
		OutputByte[0] = EncodingTable[InputByte[0] >> 2];
		OutputByte[1] = EncodingTable[(InputByte[0] << 4) & 0x3F | (InputByte[1] >> 4)];
		OutputByte[2] = EncodingTable[(InputByte[1] << 2) & 0x3F];
		OutputByte[3] = '=';
	}
}

bool ZEBase64::Decode(void* Output, const void* Input, ZESize InputSize)
{
	if (InputSize == 0)
		return true;

	ZEBYTE* OutputByte = reinterpret_cast<ZEBYTE*>(Output);
	const ZEBYTE* Current = reinterpret_cast<const ZEBYTE*>(Input);
	const ZEBYTE* End = Current + InputSize;

	ZEUInt Index = 0;
	ZEBYTE Buffer[4];
	while(Current < End)
	{
		if (*Current == ' ' || *Current == '\t' || *Current == '\r' || *Current == '\n')
		{
			Current++;
			continue;
		}

		Buffer[Index] = *Current;
		Current++;
		Index++;

		if (Index == 4)
		{
			Index = 0;
			if (Buffer[3] == '=' && Buffer[2] == '=')
			{
				if (!IsBase64(Buffer[0]) || !IsBase64(Buffer[1]))
					return false;

				OutputByte[0] = (DecodingTable[Buffer[0]] << 2) | (DecodingTable[Buffer[1]] >> 4);
				OutputByte++;
			}
			else if (Buffer[3] == '=')
			{
				if (!IsBase64(Buffer[0]) || !IsBase64(Buffer[1]) || !IsBase64(Buffer[2]))
					return false;

				OutputByte[0] = (DecodingTable[Buffer[0]] << 2) | (DecodingTable[Buffer[1]] >> 4);
				OutputByte[1] = (DecodingTable[Buffer[1]] << 4) | (DecodingTable[Buffer[2]] >> 2);
				OutputByte += 2;
			}
			else
			{
				if (!IsBase64(Buffer[0]) || !IsBase64(Buffer[1]) || !IsBase64(Buffer[2]) || !IsBase64(Buffer[3]))
					return false;

				OutputByte[0] = (DecodingTable[Buffer[0]] << 2) | (DecodingTable[Buffer[1]] >> 4);
				OutputByte[1] = (DecodingTable[Buffer[1]] << 4) | (DecodingTable[Buffer[2]] >> 2);
				OutputByte[2] = (DecodingTable[Buffer[2]] << 6) | (DecodingTable[Buffer[3]]);
				OutputByte += 3;
			}
		}
	}

	if (Index != 0)
		return false;
	
	return true;
}
