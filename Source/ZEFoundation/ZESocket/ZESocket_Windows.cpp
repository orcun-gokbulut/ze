//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESocket_Windows.cpp
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

#include "ZESocket.h"

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

static sockaddr_in CreateSockAddr4(const ZEIPAddress& IpAddress, ZEUInt16 Port)
{
	sockaddr_in Info;
	memset(&Info, 0, sizeof(Info));
	in_addr Addr;
	memset(&Addr, 0, sizeof(Addr));

	if(IpAddress == ZEIPAddress::IPv4Any)
	{
		Info.sin_family = AF_INET;
		Info.sin_port = htons(Port);
		Info.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	}
	else if(IpAddress.Type == ZE_IAT_IP_V4)
	{
		Addr.S_un.S_un_b.s_b1 = IpAddress.Address4[0];
		Addr.S_un.S_un_b.s_b2 = IpAddress.Address4[1];
		Addr.S_un.S_un_b.s_b3 = IpAddress.Address4[2];
		Addr.S_un.S_un_b.s_b4 = IpAddress.Address4[3];
		Info.sin_family = AF_INET;
		Info.sin_port = htons(Port);
		Info.sin_addr =  Addr;
	}

	return Info;
}

// ZESocketTCP

bool ZESocketTCP::Create(const ZEIPAddress& Address, ZEUInt16 Port)
{
	if(Address.Type == ZE_IAT_NONE)
		return false;

	if(Address.Type == ZE_IAT_IP_V4)
		Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(Socket == INVALID_SOCKET)
	{
		zeError("Can not create TCP Socket, WinSock Error code : %d", WSAGetLastError());
		return false;
	}

	if(Address.Type == ZE_IAT_IP_V4)
	{
		sockaddr_in LocalInfo = CreateSockAddr4(Address, Port);

		if(bind(Socket, (SOCKADDR*)&LocalInfo, sizeof(LocalInfo)) == SOCKET_ERROR)
		{
			zeError("Can not bind IPv4 TCP Socket, WinSock Error code : %d", WSAGetLastError());
			return false;
		}
	}

	IpAddress = Address;
	this->Port = Port;

	u_long IMode = 1;

	if(ioctlsocket(Socket, FIONBIO, &IMode) == SOCKET_ERROR)
	{
		zeError("Can not set TCP Socket non-blocking mode, WinSock Error code : %d", WSAGetLastError());
		return false;
	}

	return true;
}

bool ZESocketTCP::Close()
{
	if(closesocket(Socket) == SOCKET_ERROR)
	{
		zeError("Can not close TCP Listener Socket, WinSock Error code : %d", WSAGetLastError());
		return false;
	}

	return true;
}

ZESSize ZESocketTCP::Connect(const ZEIPAddress& Address, ZEUInt16 Port)
{
	if(Address.Type == ZE_IAT_IP_V4)
	{
		sockaddr_in LocalInfo = CreateSockAddr4(Address, Port);

		if(connect(Socket, (SOCKADDR*)&LocalInfo, sizeof(LocalInfo)) == SOCKET_ERROR)
		{
			ZEInt Error = WSAGetLastError();

			if(Error == WSAEWOULDBLOCK)
			{
				zeLog("WinSock notification code on connect() : %d", Error);
				return ZE_SR_RETRY;
			}

			else if(Error == WSAEALREADY)
			{
				zeLog("WinSock notification code on connect() : %d", Error);
				return ZE_SR_IN_PROGRESS;
			}

			else
			{
				zeError("Can not connect TCP IPv4 socket to specified address, WinSock Error code : %d", WSAGetLastError());
				return ZE_SR_ERROR;
			}
		}

		return ZE_SR_OK;
	}

	return ZE_SR_ERROR;

}

ZESSize ZESocketTCP::Send(const void* Buffer, ZESize BufferSize)
{
	ZESSize Result = 0;

	Result = send(Socket, (const char*)Buffer, (int)BufferSize, 0);

	if(Result == SOCKET_ERROR)
	{
		ZEInt Error = WSAGetLastError();

		if(Error == WSAEWOULDBLOCK)
		{
			zeLog("WinSock notification code on send() : %d", Error);
			return ZE_SR_RETRY;
		}

		else if(Error == WSAEALREADY)
		{
			zeLog("WinSock notification code on send() : %d", Error);
			return ZE_SR_IN_PROGRESS;
		}

		else
		{
			zeError("Can not send data from TCP Socket, WinSock Error code : %d", WSAGetLastError());
			return ZE_SR_ERROR;
		}
	}

	return Result;
}

ZESSize ZESocketTCP::Recieve(void* Buffer, ZESize BufferSize)
{
	ZESSize Result = 0;

	Result = recv(Socket, (char*)Buffer, (int)BufferSize, 0);

	if(Result >= 0)
		return Result;
	else
	{
		ZEInt Error = WSAGetLastError();

		if(Error == WSAEWOULDBLOCK)
		{
			//zeLog("WinSock notification code on recv() : %d", Error);
			return ZE_SR_RETRY;
		}

		else if(Error == WSAEALREADY)
		{
			//zeLog("WinSock notification code on recv() : %d", Error);
			return ZE_SR_IN_PROGRESS;
		}

		else
		{
			//zeError("Can not receive data TCP Socket, WinSock Error code : %d", WSAGetLastError());
			return ZE_SR_ERROR;
		}
	}
}

const ZEIPAddress& ZESocketTCP::GetIpAddress() const
{
	return IpAddress;
}

ZEUInt16 ZESocketTCP::GetPort() const
{
	return Port;
}

// ZESocketTCPListener

bool ZESocketTCPListener::Create(const ZEIPAddress& Address, ZEUInt16 Port)
{
	if(Address.Type == ZE_IAT_NONE)
		return false;

	if(Address.Type == ZE_IAT_IP_V4)
		Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(Socket == INVALID_SOCKET)
	{
		zeError("Can not create TCP Listener Socket, WinSock Error code : %d", WSAGetLastError());
		return false;
	}

	if(Address.Type == ZE_IAT_IP_V4)
	{
		sockaddr_in LocalInfo = CreateSockAddr4(Address, Port);

		if(bind(Socket, (SOCKADDR*)&LocalInfo, sizeof(LocalInfo)) == SOCKET_ERROR)
		{
			zeError("Can not bind IPv4 TCP Listener Socket, WinSock Error code : %d", WSAGetLastError());
			return false;
		}
	}

	IpAddress = Address;
	this->Port = Port;

	u_long IMode = 1;

	if(ioctlsocket(Socket, FIONBIO, &IMode) == SOCKET_ERROR)
	{
		zeError("Can not set TCP Listener Socket non-blocking mode, WinSock Error code : %d", WSAGetLastError());
		return false;
	}

	if(listen(Socket, SOMAXCONN) == SOCKET_ERROR)
	{
		zeError("Listen failed on TCP Listener., WinSock Error code : %d", WSAGetLastError());
		return false;
	}

	return true;
}

bool ZESocketTCPListener::Close()
{
	if(closesocket(Socket) == SOCKET_ERROR)
	{
		zeError("Can not close TCP Listener Socket, WinSock Error code : %d", WSAGetLastError());
		return false;
	}

	return true;
}

ZESocketTCP* ZESocketTCPListener::Accept()
{


	if(IpAddress.Type == ZE_IAT_IP_V4)
	{
		SOCKET Result = accept(Socket, NULL, NULL);
		if(Result == INVALID_SOCKET)
		{
			//zeError("Accepted socket is not valid. WinSock Error code : %d", WSAGetLastError());
			return NULL;
		}

		sockaddr_in Info;
		memset(&Info, 0, sizeof(Info));
		int InfoSize = sizeof(Info);
		if(getsockname(Result, (SOCKADDR*)&Info, &InfoSize) == SOCKET_ERROR)
		{
			closesocket(Socket);
			zeError("Can not get TCP v4 socket name, WinSock Error code : %d", WSAGetLastError());
			return NULL;
		}
		
		ZESocketTCP* ResultSocket = new ZESocketTCP();
		ResultSocket->Socket = Result;
		ResultSocket->IpAddress = ZEIPAddress(Info.sin_addr.S_un.S_un_b.s_b1, Info.sin_addr.S_un.S_un_b.s_b2, Info.sin_addr.S_un.S_un_b.s_b3, Info.sin_addr.S_un.S_un_b.s_b4);
		ResultSocket->Port = ntohs(Info.sin_port);
		
		u_long IMode = 1;
		if(ioctlsocket(Socket, FIONBIO, &IMode) == SOCKET_ERROR)
		{
			closesocket(Socket);
			zeError("Can not set TCP Socket non-blocking mode after accept, WinSock Error code : %d", WSAGetLastError());
			return NULL;
		}

		return ResultSocket;
	}

	return NULL;
}

const ZEIPAddress& ZESocketTCPListener::GetIpAddress() const
{
	return IpAddress;
}

ZEUInt16 ZESocketTCPListener::GetPort() const
{
	return Port;
}

// ZESocketUDP

bool ZESocketUDP::Create(const ZEIPAddress& Address, ZEUInt16 Port)
{
	if(Address.Type == ZE_IAT_IP_V4)
	{
		Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(Socket == INVALID_SOCKET)
		{
			zeError("Can not create UDP Socket, WinSock Error code : %d", WSAGetLastError());
			return false;
		}

		sockaddr_in LocalInfo = CreateSockAddr4(Address, Port);
		if(bind(Socket, (SOCKADDR*)&LocalInfo, sizeof(LocalInfo)) == SOCKET_ERROR)
		{
			closesocket(Socket);
			zeError("Can not bind IPv4 UDP Socket, WinSock Error code : %d", WSAGetLastError());
			return false;
		}

		IpAddress = Address;
		this->Port = Port;

		u_long IMode = 1;
		if(ioctlsocket(Socket, FIONBIO, &IMode) == SOCKET_ERROR)
		{
			closesocket(Socket);
			zeError("Can not set UDP Socket non-blocking mode, WinSock Error code : %d", WSAGetLastError());
			return false;
		}
	}

	return true;
}

bool ZESocketUDP::Close()
{
	if(closesocket(Socket) == SOCKET_ERROR)
	{
		zeError("Can not close UDP Socket, WinSock Error code : %d", WSAGetLastError());
		return false;
	}

	return true;
}

const ZEIPAddress&	ZESocketUDP::GetIpAddress() const
{
	return IpAddress;
}

ZEUInt16 ZESocketUDP::GetPort() const
{
	return Port;
}

ZESSize ZESocketUDP::SendTo(const ZEIPAddress* To, ZEUInt16 ToPort, const void* Buffer, ZESize BufferSize)
{
	ZESSize Result = 0;

	if(To == NULL)
	{
		zeError("To Ip address can not be NULL");
		return ZE_SR_ERROR;
	}

	if(To->Type == ZE_IAT_IP_V4)
	{
		sockaddr_in ToInfo = CreateSockAddr4(*To, ToPort);
		Result = sendto(Socket, (const char*)Buffer, (int)BufferSize, 0, (SOCKADDR*)&ToInfo, sizeof(ToInfo));

		if(Result == SOCKET_ERROR)
		{
			ZEInt Error = WSAGetLastError();

			if(Error == WSAEWOULDBLOCK)
			{
				zeLog("WinSock notification code on sendto() : %d", Error);
				return ZE_SR_RETRY;
			}

			else if(Error == WSAEALREADY)
			{
				zeLog("WinSock notification code on sendto() : %d", Error);
				return ZE_SR_IN_PROGRESS;
			}

			else
			{
				zeError("Can not send to Ipv4 UDP socket, WinSock Error code : %d", WSAGetLastError());
				return ZE_SR_ERROR;
			}
		}
	}

	return Result;
}

ZESSize ZESocketUDP::RecieveFrom(void* Buffer, ZESize BufferSize, const ZEIPAddress* From, ZEUInt16 FromPort)
{
	ZESSize Result = 0;

	if(From == NULL)
	{
		zeError("From Ip address can not be NULL");
		return ZE_SR_ERROR;
	}
	else
	{
		if(From->Type == ZE_IAT_IP_V4)
		{
			sockaddr_in FromInfo = CreateSockAddr4(*From, FromPort);			
			int FromInfoSize = sizeof(FromInfo);

			Result = recvfrom(Socket, (char*)Buffer, (int)BufferSize, 0, (SOCKADDR*)&FromInfo, &FromInfoSize);

			if(Result == SOCKET_ERROR)
			{
				ZEInt Error = WSAGetLastError();

				if(Error == WSAEWOULDBLOCK)
				{
					zeLog("WinSock notification code on recvfrom() : %d", Error);
					return ZE_SR_RETRY;
				}

				else if(Error == WSAEALREADY)
				{
					zeLog("WinSock notification code on recvfrom() : %d", Error);
					return ZE_SR_IN_PROGRESS;
				}

				else
				{
					zeError("Can not receive from Ipv4 UDP socket, WinSock Error code : %d", WSAGetLastError());
					return ZE_SR_ERROR;
				}
			}
		}
	}

	return Result;
}
