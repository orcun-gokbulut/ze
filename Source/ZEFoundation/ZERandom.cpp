//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERandom.cpp
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

#include "ZERandom.h"
#include <stdlib.h>
#include <process.h>
#include <time.h>

float ZERandom::GetFloat()
{
	return (float)rand() / (float)RAND_MAX - (float)rand() / (float)RAND_MAX; //-V501
}

float ZERandom::GetFloatPositive()
{
	return (float)rand() / (float)RAND_MAX;
}

float ZERandom::GetFloatRange(float Min, float Max)
{
	return Min + GetFloatPositive() * (Max - Min);
}

ZEInt ZERandom::GetInt()
{
	return rand() - rand();
}

ZEUInt ZERandom::GetUInt()
{
	return rand();
}

ZEInt8 ZERandom::GetInt8()
{
	return rand();
}

ZEInt16 ZERandom::GetInt16()
{
	return rand();
}

ZEInt32 ZERandom::GetInt32()
{
	return GetUInt32();
}

ZEInt64 ZERandom::GetInt64()
{
	return GetUInt64();
}

ZEUInt8 ZERandom::GetUInt8()
{
	return rand();
}

ZEUInt16 ZERandom::GetUInt16()
{
	return rand();
}

ZEUInt32 ZERandom::GetUInt32()
{
	return (rand() << 16) | rand();
}

ZEUInt64 ZERandom::GetUInt64()
{
	return ((ZEUInt64)GetUInt32() << 32) | (ZEUInt64)GetUInt32();
}

bool ZERandom::GetBool()
{
	return rand() % 2 == 1;
}

static int Mix(int a, int b, int c)
{
	a = a - b;  a = a - c;  a = a ^ (c >> 13);
	b = b - c;  b = b - a;  b = b ^ (a << 8);
	c = c - a;  c = c - b;  c = c ^ (b >> 13);
	a = a - b;  a = a - c;  a = a ^ (c >> 12);
	b = b - c;  b = b - a;  b = b ^ (a << 16);
	c = c - a;  c = c - b;  c = c ^ (b >> 5);
	a = a - b;  a = a - c;  a = a ^ (c >> 3);
	b = b - c;  b = b - a;  b = b ^ (a << 10);
	c = c - a;  c = c - b;  c = c ^ (b >> 15);
	return c;
}

void ZERandom::Reset()
{
	srand(Mix(getpid(), time(NULL), clock()));
}

void ZERandom::SetSeed(ZEUInt Seed)
{
	srand(Seed);
}

class ZERandomInitializer
{
	public:
		ZERandomInitializer()
		{
			ZERandom::Reset();
		}
};

static ZERandomInitializer RandomInitializer;
