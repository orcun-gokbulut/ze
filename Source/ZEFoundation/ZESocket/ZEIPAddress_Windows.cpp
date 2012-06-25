//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEIPAddress_Windows.cpp
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

#ifdef ZE_PLATFORM_COMPILER_GCC
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x501
#endif

#include <Winsock2.h>
#include <Ws2tcpip.h>

#ifdef ZE_PLATFORM_COMPILER_MSVC
#include <Mstcpip.h>
#include <Ws2ipdef.h>
#endif

#include <memory.h>

#include "ZETypes.h"
#include "ZEError.h"



const ZEIPAddress ZEIPAddress::IPv4Any = ZEIPAddress(0, 0, 0, 0);

ZEIPAddress::ZEIPAddress()
{
	memset(this, 0, sizeof(ZEIPAddress));
	Type = ZE_IAT_NONE;
}

ZEIPAddress::ZEIPAddress(ZEUInt8 Byte0, ZEUInt8 Byte1, ZEUInt8 Byte2, ZEUInt8 Byte3)
{
	Type = ZE_IAT_IP_V4;
	Address4[0] = Byte0;
	Address4[1] = Byte1;
	Address4[2] = Byte2;
	Address4[3] = Byte3;
}

ZEIPAddress ZEIPAddress::Parse(const ZEString& String)
{
	ZEIPAddressType Type = ZE_IAT_IP_V4;

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

	return ZEIPAddress((ZEUInt8)Part0.ToInt(), (ZEUInt8)Part1.ToInt(), (ZEUInt8)Part2.ToInt(), (ZEUInt8)Part3.ToInt());
}

ZEArray<ZEIPAddress> ZEIPAddress::Lookup(const ZEString& String)
{
	ZEArray<ZEIPAddress> Temp;

	addrinfo Hints;
	memset(&Hints, 0, sizeof(addrinfo));

	addrinfo* Address;
	getaddrinfo(String, NULL, &Hints, &Address);
	while(Address != NULL)
	{
		sockaddr_in* IP4Adress = (struct sockaddr_in *)Address->ai_addr;
		Temp.Add(ZEIPAddress(
			IP4Adress->sin_addr.S_un.S_un_b.s_b1, IP4Adress->sin_addr.S_un.S_un_b.s_b2, 
			IP4Adress->sin_addr.S_un.S_un_b.s_b3, IP4Adress->sin_addr.S_un.S_un_b.s_b4));
	}

	return Temp;
}

ZEArray<ZEIPAddress> ZEIPAddress::HostIPs()
{
	char HostName[256];
	gethostname(HostName, 255);
	return Lookup(ZEString(HostName));
}

bool ZEIPAddress::operator == (const ZEIPAddress &RightOperand) const
{
	zeDebugCheck(Type != RightOperand.Type, "Ip type mismatch.");
	zeDebugCheck((Type == ZE_IAT_NONE) || (RightOperand.Type == ZE_IAT_NONE), "Typeless Ip's are not comparable.");

	if(Type != RightOperand.Type)
		return false;

	if(Type == ZE_IAT_NONE || RightOperand.Type == ZE_IAT_NONE)
		return false;

	return ((Address4[0] == RightOperand.Address4[0]) &&
			(Address4[1] == RightOperand.Address4[1]) &&
			(Address4[2] == RightOperand.Address4[2]) &&
			(Address4[3] == RightOperand.Address4[3]));

	return false;
}

bool ZEIPAddress::operator != (const ZEIPAddress &RightOperand) const
{
	zeDebugCheck(Type != RightOperand.Type, "Ip type mismatch.");
	zeDebugCheck((Type == ZE_IAT_NONE) || (RightOperand.Type == ZE_IAT_NONE), "Typeless Ip's are not comparable.");

	if(Type != RightOperand.Type)
		return false;

	if(Type == ZE_IAT_NONE || RightOperand.Type == ZE_IAT_NONE)
		return false;

	if(Type == ZE_IAT_IP_V4)
		return ((Address4[0] != RightOperand.Address4[0]) ||
				(Address4[1] != RightOperand.Address4[1]) ||
				(Address4[2] != RightOperand.Address4[2]) ||
				(Address4[3] != RightOperand.Address4[3]));

	return false;
}
