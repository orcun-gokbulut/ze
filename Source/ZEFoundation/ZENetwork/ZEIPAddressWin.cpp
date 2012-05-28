//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEIPAddressWin.cpp
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

#include <Winsock2.h>
#include <Ws2ipdef.h>
#include <Ws2tcpip.h>
#include <Mstcpip.h>
#include <memory.h>

#include "ZETypes.h"
#include "ZEError.h"

const ZEIPAddress ZEIPAddress::IPv4Any = ZEIPAddress(0, 0, 0, 0);
const ZEIPAddress ZEIPAddress::IPv6Any = ZEIPAddress(0, 0, 0, 0, 0, 0, 0, 0);

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

ZEIPAddress::ZEIPAddress(ZEUInt16 Word0, ZEUInt16 Word1, ZEUInt16 Word2, ZEUInt16 Word3, ZEUInt16 Word4, ZEUInt16 Word5, ZEUInt16 Word6, ZEUInt16 Word7)
{
	Type = ZE_IAT_IP_V6;
	Address6[0] = Word0;
	Address6[1] = Word1;
	Address6[2] = Word2;
	Address6[3] = Word3;
	Address6[4] = Word4;
	Address6[5] = Word5;
	Address6[6] = Word6;
	Address6[7] = Word7;
}

ZEIPAddress ZEIPAddress::Parse(ZEString& String)
{
	ZEIPAddressType Type = ZE_IAT_IP_V4;

	for (ZESize I = 0; I < String.GetLength(); I++)
	{
		if(String[I] == ':')
		{
			Type = ZE_IAT_IP_V6;
			break;
		}
	}

	if(Type == ZE_IAT_IP_V4)
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

		return ZEIPAddress((ZEUInt8)Part0.ToInt(), (ZEUInt8)Part1.ToInt(), (ZEUInt8)Part2.ToInt(), (ZEUInt8)Part3.ToInt());
	}

	else if(Type == ZE_IAT_IP_V6)
	{
		ZEString Part0, Part1, Part2, Part3, Part4, Part5, Part6, Part7;
		ZEUInt PartIndex = 0;

		for (ZESize I = 0; I < String.GetLength(); I++)
		{
			if(String[I] == ':')
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
			else if(PartIndex == 4)
				Part4.Append(ZEString::FromChar(String.GetCharacter(I)));
			else if(PartIndex == 5)
				Part5.Append(ZEString::FromChar(String.GetCharacter(I)));
			else if(PartIndex == 6)
				Part6.Append(ZEString::FromChar(String.GetCharacter(I)));
			else if(PartIndex == 7)
				Part7.Append(ZEString::FromChar(String.GetCharacter(I)));
		}

		char* Error = NULL;

		return ZEIPAddress( (ZEUInt16)strtol(Part0, &Error, 16), (ZEUInt16)strtol(Part1, &Error, 16), (ZEUInt16)strtol(Part2, &Error, 16), (ZEUInt16)strtol(Part3, &Error, 16), 
							(ZEUInt16)strtol(Part4, &Error, 16), (ZEUInt16)strtol(Part5, &Error, 16), (ZEUInt16)strtol(Part6, &Error, 16), (ZEUInt16)strtol(Part7, &Error, 16));
	}


// 	for (ZESize I = 0; I < String.GetLength(); I++)
// 	{
// 		if (String[I] == '.')
// 		{
// 			in_addr IP4Adress;
// 			if (RtlIpv4StringToAddress(String, false, '\0', &IP4Adress) != NO_ERROR)
// 			{
// 				return ZEIPAddress(
// 					IP4Adress.S_un.S_un_b.s_b1, IP4Adress.S_un.S_un_b.s_b1, 
// 					IP4Adress.S_un.S_un_b.s_b1, IP4Adress.S_un.S_un_b.s_b1);
// 			}
// 
// 			return ZEIPAddress();
// 		}
// 		else if (String[I] == ':')
// 		{
// 			in6_addr IP6Adress;
// 			if (RtlIpv6StringToAddress(String, '\0', &IP6Adress) != NO_ERROR)
// 			{
// 				return ZEIPAddress(
// 					IP6Adress.u.Word[0], IP6Adress.u.Word[1], IP6Adress.u.Word[2], IP6Adress.u.Word[3],
// 					IP6Adress.u.Word[4], IP6Adress.u.Word[5], IP6Adress.u.Word[6], IP6Adress.u.Word[7]);
// 			}
// 
// 			return ZEIPAddress();
// 		}
// 	}

	return ZEIPAddress();
}

ZEArray<ZEIPAddress> ZEIPAddress::Lookup(ZEString& String)
{
	ZEArray<ZEIPAddress> Temp;

	addrinfo Hints;
	memset(&Hints, 0, sizeof(addrinfo));

	addrinfo* Address;
	getaddrinfo(String, NULL, &Hints, &Address);
	while(Address != NULL)
	{
		if (Address->ai_family == AF_INET)
		{
			sockaddr_in* IP4Adress = (struct sockaddr_in *)Address->ai_addr;
			Temp.Add(ZEIPAddress(
				IP4Adress->sin_addr.S_un.S_un_b.s_b1, IP4Adress->sin_addr.S_un.S_un_b.s_b2, 
				IP4Adress->sin_addr.S_un.S_un_b.s_b3, IP4Adress->sin_addr.S_un.S_un_b.s_b4));
		}
		else if (Address->ai_family == AF_INET6)
		{
			sockaddr_in6* IP6Adress = (struct sockaddr_in6 *)Address->ai_addr;
			Temp.Add(ZEIPAddress(
				IP6Adress->sin6_addr.u.Word[0], IP6Adress->sin6_addr.u.Word[1], IP6Adress->sin6_addr.u.Word[2], IP6Adress->sin6_addr.u.Word[3],
				IP6Adress->sin6_addr.u.Word[4], IP6Adress->sin6_addr.u.Word[5], IP6Adress->sin6_addr.u.Word[6], IP6Adress->sin6_addr.u.Word[7]));
		}
		Address = Address->ai_next;
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
	zeAssert(Type != RightOperand.Type, "Ip type mismatch.");
	zeAssert((Type == ZE_IAT_NONE) || (RightOperand.Type == ZE_IAT_NONE), "Typeless Ip's are not comparable.");

	if(Type != RightOperand.Type)
		return false;

	if(Type == ZE_IAT_NONE || RightOperand.Type == ZE_IAT_NONE)
		return false;

	if(Type == ZE_IAT_IP_V4)
		return ((Address4[0] == RightOperand.Address4[0]) &&
				(Address4[1] == RightOperand.Address4[1]) &&
				(Address4[2] == RightOperand.Address4[2]) &&
				(Address4[3] == RightOperand.Address4[3]));
	
	else if(Type == ZE_IAT_IP_V6)
		return ((Address6[0] == RightOperand.Address6[0]) &&
				(Address6[1] == RightOperand.Address6[1]) &&
				(Address6[2] == RightOperand.Address6[2]) &&
				(Address6[3] == RightOperand.Address6[3]) &&
				(Address6[4] == RightOperand.Address6[4]) &&
				(Address6[5] == RightOperand.Address6[5]) &&
				(Address6[6] == RightOperand.Address6[6]) &&
				(Address6[7] == RightOperand.Address6[7]));
}

bool ZEIPAddress::operator != (const ZEIPAddress &RightOperand) const
{
	zeAssert(Type != RightOperand.Type, "Ip type mismatch.");
	zeAssert((Type == ZE_IAT_NONE) || (RightOperand.Type == ZE_IAT_NONE), "Typeless Ip's are not comparable.");

	if(Type != RightOperand.Type)
		return false;

	if(Type == ZE_IAT_NONE || RightOperand.Type == ZE_IAT_NONE)
		return false;

	if(Type == ZE_IAT_IP_V4)
		return ((Address4[0] != RightOperand.Address4[0]) ||
				(Address4[1] != RightOperand.Address4[1]) ||
				(Address4[2] != RightOperand.Address4[2]) ||
				(Address4[3] != RightOperand.Address4[3]));

	else if(Type == ZE_IAT_IP_V6)
		return ((Address6[0] != RightOperand.Address6[0]) ||
				(Address6[1] != RightOperand.Address6[1]) ||
				(Address6[2] != RightOperand.Address6[2]) ||
				(Address6[3] != RightOperand.Address6[3]) ||
				(Address6[4] != RightOperand.Address6[4]) ||
				(Address6[5] != RightOperand.Address6[5]) ||
				(Address6[6] != RightOperand.Address6[6]) ||
				(Address6[7] != RightOperand.Address6[7]));
}
