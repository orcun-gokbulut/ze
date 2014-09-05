//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESocketUDP_Windows.cpp
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

#include "ZESocketUDP.h"

#include <Winsock2.h>

ZESocketUDPStatus ZESocketUDP::GetStatus()
{
	return Status;
}

bool ZESocketUDP::Open()
{
	if (Status == ZE_SUS_OPEN)
		return true;

	Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(Socket == INVALID_SOCKET)
	{
		if (WSAGetLastError() == WSANOTINITIALISED)
		{
			int iResult;
			WSADATA wsaData;
			iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
			if (iResult != 0) 
			{
				zeError("WSAStartup failed. WinSock Error code : %d", iResult);
				return false;
			}

			return Open();
		}
		else
		{
			zeError("Can not create UDP socket. WinSock Error code : %d", WSAGetLastError());
			return false;
		}
	}

	if (!BindIPAddress.IsAny() || BindPort != ZE_SP_ANY)
	{
		sockaddr_in LocalInfo;
		ZEIPAddress::ToSockaddr_in(&LocalInfo, BindIPAddress);
		LocalInfo.sin_port = htons(BindPort);

		if(bind(Socket, (SOCKADDR*)&LocalInfo, sizeof(LocalInfo)) == SOCKET_ERROR)
		{
			zeError("Can not bind UDP Socket, WinSock Error code : %d", WSAGetLastError());
			Close();
			return false;
		}
	}

	u_long IMode = 1;
	if(ioctlsocket(Socket, FIONBIO, &IMode) == SOCKET_ERROR)
	{
		zeError("Can not set TCP Socket non-blocking mode, WinSock Error code : %d", WSAGetLastError());
		Close();
		return false;
	}

	Status = ZE_SUS_OPEN;
	return true;
}

void ZESocketUDP::Close()
{
	if (Socket != NULL)
		return;

	closesocket(Socket);
	Socket = NULL;

	Status = ZE_SUS_CLOSED;
}

ZESSize ZESocketUDP::Send(const ZEIPAddress& RemoteAddress, ZEUInt16 RemotePort, const void* Buffer, ZESize BufferSize)
{
	if (GetStatus() != ZE_SUS_OPEN)
		return ZE_SR_ERROR;

	sockaddr_in SocketAddress;
	ZEIPAddress::ToSockaddr_in(&SocketAddress, RemoteAddress);
	SocketAddress.sin_port = htons(RemotePort);

	ZESSize Result = sendto(Socket, (const char*)Buffer, (int)BufferSize, 0, (sockaddr*)&SocketAddress, sizeof(sockaddr_in));
	if(Result < 0)
	{
		ZEInt Error = WSAGetLastError();
		if (Error == WSAEWOULDBLOCK)
		{
			return BufferSize;
		}
		else if (Error == WSAEMSGSIZE)
		{
			return ZE_SR_PACKET_SIZE;
		}
		else if (Error == WSAENOBUFS)
		{
			return ZE_SR_BUFFER_FULL;
		}
		else
		{
			Close();
			return ZE_SR_ERROR;
		}
	}

	return Result;
}

ZESSize ZESocketUDP::Receive(ZEIPAddress& RemoteAddress, ZEUInt16& RemotePort, void* Buffer, ZESize BufferSize)
{
	if (GetStatus() != ZE_SUS_OPEN)
		return ZE_SR_ERROR;

	sockaddr_in SocketAddress;
	int SocketAddressSize = sizeof(sockaddr_in);

	ZESSize Result = recvfrom(Socket, (char*)Buffer, (int)BufferSize, 0, (sockaddr*)&SocketAddress, &SocketAddressSize);
	if (Result < 0)
	{
		ZEInt Error = WSAGetLastError();
		if (Error == WSAEWOULDBLOCK)
		{
			return ZE_SR_NO_PACKET;
		}
		else if (Error == WSAEMSGSIZE)
		{
			return ZE_SR_PACKET_SIZE;
		}
		else
		{
			Close();
			return ZE_SR_ERROR;
		}
	}

	ZEIPAddress::FromSockaddr_in(RemoteAddress, &SocketAddress);
	RemotePort = htons(SocketAddress.sin_port);

	return Result;
}
