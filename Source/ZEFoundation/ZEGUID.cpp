//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGUID.cpp
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

#include "ZEGUID.h"
#include "ZEDS/ZEFormat.h"
#include "ZERandom.h"

ZEInt ZEGUID::Compare(const ZEGUID& Other) const
{
	if (Data1 == Other.Data1)
	{
		if (Data2 == Other.Data2)
		{
			if (Data3 == Other.Data3)
			{
				if (Data4 == Data4)
					return 0;

				return (Data4 > Other.Data4 ? 1 : -1);
			}

			return (Data3 > Other.Data3 ? 1 : -1);
		}

		return (Data2 > Other.Data2 ? 1 : -1);
	}

	return (Data1 > Other.Data1 ? 1 : -1);
}

bool ZEGUID::Equals(const ZEGUID& Other) const
{
	return Data1 == Other.Data1 && Data2 == Other.Data2 && Data3 == Other.Data3 && Data4 == Other.Data4;
}

bool ZEGUID::operator<(const ZEGUID& Other) const
{
	return Compare(Other) == -1;
}

bool ZEGUID::operator>(const ZEGUID& Other) const
{
	return Compare(Other) == 1;
}

bool ZEGUID::operator<=(const ZEGUID& Other) const
{
	ZEInt Result = Compare(Other);
	return Result == -1 || Result == 0;
}

bool ZEGUID::operator>=(const ZEGUID& Other) const
{
	ZEInt Result = Compare(Other);
	return Result == 1 || Result == 0;
}

bool ZEGUID::operator==(const ZEGUID& Other) const
{
	return Equals(Other);
}

bool ZEGUID::operator!=(const ZEGUID& Other) const
{
	return !Equals(Other);
}

ZEString ZEGUID::ToString()
{
	return ZEFormat::Format("{0:X:08}-{1:X:04}-{2:X:04}-{3:X:04}-{4:X:012}", Data1, Data2, Data3, Data4 >> 48, Data4 & (ZEUInt64)0x0000FFFFFFFFFFFF);
}

ZEGUID::ZEGUID()
{

}

ZEGUID::ZEGUID(ZEUInt32 Data1, ZEUInt16 Data2, ZEUInt16 Data3, ZEUInt64 Data4)
{
	this->Data1 = Data1;
	this->Data2 = Data2;
	this->Data3 = Data3;
	this->Data4 = Data4;
}

ZEGUID::ZEGUID(ZEUInt32 Data1, ZEUInt16 Data2, ZEUInt16 Data3, ZEUInt16 Data4FirstTwo, ZEUInt64 Data4Remaining)
{
	this->Data1 = Data1;
	this->Data2 = Data2;
	this->Data3 = Data3;
	this->Data4 = (Data4FirstTwo << 48) | (Data4 & (ZEUInt64)0x0000FFFFFFFFFFFFFF);
}

ZEGUID ZEGUID::Generate()
{
	ZEGUID GUID;

	GUID.Data1 = ZERandom::GetUInt32();
	GUID.Data2 = ZERandom::GetUInt16();
	GUID.Data3 = ZERandom::GetUInt16();
	GUID.Data4 = ZERandom::GetUInt64();

	return GUID;
}
