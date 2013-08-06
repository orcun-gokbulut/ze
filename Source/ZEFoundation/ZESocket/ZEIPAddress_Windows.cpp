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

#include "ZETypes.h"
#include <memory.h>

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

void ZEIPAddress::ToSockaddr_in(void* Buffer, const ZEIPAddress& IPAddress)
{
	sockaddr_in* SocketAddress = (sockaddr_in*)Buffer;
	memset(SocketAddress, 0, sizeof(sockaddr_in));

	SocketAddress->sin_family = AF_INET;
	SocketAddress->sin_port = htons(0);

	if(IPAddress == ZEIPAddress::Any)
	{
		SocketAddress->sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	}
	else
	{
		SocketAddress->sin_addr.S_un.S_un_b.s_b1 = IPAddress.Address[0];
		SocketAddress->sin_addr.S_un.S_un_b.s_b2 = IPAddress.Address[1];
		SocketAddress->sin_addr.S_un.S_un_b.s_b3 = IPAddress.Address[2];
		SocketAddress->sin_addr.S_un.S_un_b.s_b4 = IPAddress.Address[3];
	}
}

void ZEIPAddress::FromSockaddr_in(ZEIPAddress& IPAddress, const void* Buffer)
{
	sockaddr_in* SocketAddress = (sockaddr_in*)Buffer;
	IPAddress.Address[0] = SocketAddress->sin_addr.S_un.S_un_b.s_b1;
	IPAddress.Address[1] = SocketAddress->sin_addr.S_un.S_un_b.s_b2;
	IPAddress.Address[2] = SocketAddress->sin_addr.S_un.S_un_b.s_b3;
	IPAddress.Address[3] = SocketAddress->sin_addr.S_un.S_un_b.s_b4;
}
