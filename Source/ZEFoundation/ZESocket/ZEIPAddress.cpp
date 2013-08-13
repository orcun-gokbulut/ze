//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEIPAddress.cpp
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

#include "ZEIPAddress.h"

#include "ZETypes.h"
#include <memory.h>

const ZEIPAddress ZEIPAddress::Any = ZEIPAddress(0, 0, 0, 0);
const ZEIPAddress ZEIPAddress::Broadcast = ZEIPAddress(255, 255, 255, 255);

ZEIPAddress::ZEIPAddress()
{
	*this = ZEIPAddress::Any;
}

ZEIPAddress::ZEIPAddress(ZEUInt8 Byte0, ZEUInt8 Byte1, ZEUInt8 Byte2, ZEUInt8 Byte3)
{
	Address[0] = Byte0;
	Address[1] = Byte1;
	Address[2] = Byte2;
	Address[3] = Byte3;
}

ZEIPAddress ZEIPAddress::Parse(const ZEString& String)
{
	ZEString Part0, Part1, Part2, Part3;
	ZEUInt PartIndex = 0;

	for (ZESize I = 0; I < String.GetLength(); I++)
	{
		if(String[I] == '.')
		{
			PartIndex++;
			continue;
		}

		if(PartIndex == 0)
			Part0.Append(ZEString::FromChar(String.GetCharacter(I)));
		else if(PartIndex == 1)
			Part1.Append(ZEString::FromChar(String.GetCharacter(I)));
		else if(PartIndex == 2)
			Part2.Append(ZEString::FromChar(String.GetCharacter(I)));
		else if(PartIndex == 3)
			Part3.Append(ZEString::FromChar(String.GetCharacter(I)));
	}

	return ZEIPAddress((ZEUInt8)Part0.ToInt32(), (ZEUInt8)Part1.ToInt32(), (ZEUInt8)Part2.ToInt32(), (ZEUInt8)Part3.ToInt32());
}

bool ZEIPAddress::operator == (const ZEIPAddress &RightOperand) const
{
	return ((Address[0] == RightOperand.Address[0]) &&
			(Address[1] == RightOperand.Address[1]) &&
			(Address[2] == RightOperand.Address[2]) &&
			(Address[3] == RightOperand.Address[3]));

	return false;
}

bool ZEIPAddress::operator != (const ZEIPAddress &RightOperand) const
{
	return ((Address[0] != RightOperand.Address[0]) ||
			(Address[1] != RightOperand.Address[1]) ||
			(Address[2] != RightOperand.Address[2]) ||
			(Address[3] != RightOperand.Address[3]));
}
