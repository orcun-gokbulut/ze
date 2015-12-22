//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESocketTCPClient_Windows.cpp
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

#include "ZESocketTCPClient.h"

#include <Winsock2.h>

void ZESocketTCPClient::UpdateStatus()
{
	if (Status == ZE_STCS_CONNECTING)
	{
		sockaddr_in Info;
		ZEIPAddress::ToSockaddr_in(&Info, ServerIPAddress);
		Info.sin_port = htons(Port);

		int Result = connect(Socket, (SOCKADDR*)&Info, sizeof(sockaddr_in));
		if (Result == 0)
			Status = ZE_STCS_CONNECTED;
		else if (Result != EALREADY)
			Close();
	}
}

ZESocketTCPClientStatus ZESocketTCPClient::GetStatus()
{
	UpdateStatus();
	return Status;
}

SOCKET_TYPE ZESocketTCPClient::GetSocket()
{
	return Socket;
}

bool ZESocketTCPClient::Open(SOCKET_TYPE Socket)
{
	if (this->Socket == Socket)
		return true;

	Close();

	if (Socket == INVALID_SOCKET)
		return false;

	this->Socket = Socket;

	u_long IMode = 1;
	if(ioctlsocket(Socket, FIONBIO, &IMode) == SOCKET_ERROR)
	{
		zeError("Can not set TCP Socket non-blocking mode, WinSock Error code : %d", WSAGetLastError());
		Close();
		return false;
	}

	sockaddr_in ClientInfo;
	int ClientInfoSize = sizeof(sockaddr_in);
	getpeername(Socket, (SOCKADDR*)&ClientInfo, &ClientInfoSize);
	ZEIPAddress::ToSockaddr_in(&ClientInfo, BindIPAddress);
	Port = htons(ClientInfo.sin_port);

	sockaddr_in ServerInfo;
	ZEIPAddress::ToSockaddr_in(&ServerInfo, ServerIPAddress);
	ServerInfo.sin_port = htons(Port);

	Status = ZE_STCS_CONNECTED;
	return true;
}

bool ZESocketTCPClient::Open()
{
	if (Status != ZE_STCS_DISCONNECTED)
		return true;

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
			zeError("Can not create TCP client socket. WinSock Error code : %d", WSAGetLastError());
			return false;
		}
	}

	if (BindIPAddress != ZEIPAddress::Any)
	{
		sockaddr_in LocalInfo;
		ZEIPAddress::ToSockaddr_in(&LocalInfo, BindIPAddress);
		LocalInfo.sin_port = htons(Port);

		if(bind(Socket, (SOCKADDR*)&LocalInfo, sizeof(LocalInfo)) == SOCKET_ERROR)
		{
			zeError("Can not bind IPv4 TCP Socket, WinSock Error code : %d", WSAGetLastError());
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

	sockaddr_in ServerInfo;
	ZEIPAddress::ToSockaddr_in(&ServerInfo, ServerIPAddress);
	ServerInfo.sin_port = htons(Port);
	if(connect(Socket, (SOCKADDR*)&ServerInfo, sizeof(ServerInfo)) == SOCKET_ERROR)
	{
		ZEInt Error = WSAGetLastError();
		if(Error == WSAEWOULDBLOCK || Error == WSAEALREADY)
		{
			Status = ZE_STCS_CONNECTING;
			return true;
		}
		else if(Error == WSAEISCONN)
		{
			Status = ZE_STCS_CONNECTED;
			return true;
		}
		else
		{	
			Status = ZE_STCS_DISCONNECTED;
			Close();
		}	
	}

	Status = ZE_STCS_CONNECTED;
	return true;
}

void ZESocketTCPClient::Close()
{
	if (Socket != NULL)
		return;

	closesocket(Socket);
	Socket = NULL;

	Status = ZE_STCS_DISCONNECTED;
}

ZESSize ZESocketTCPClient::Send(const void* Buffer, ZESize BufferSize)
{
	if (GetStatus() != ZE_STCS_CONNECTED)
		return ZE_SR_ERROR;

	ZESSize Result = send(Socket, (const char*)Buffer, (int)BufferSize, 0);
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
			return BufferSize;
		}
	}

	return Result;
}

ZESSize ZESocketTCPClient::Receive(void* Buffer, ZESize BufferSize)
{
	if (GetStatus() != ZE_STCS_CONNECTED)
		return ZE_SR_ERROR;

	ZESSize Result = recv(Socket, (char*)Buffer, (int)BufferSize, 0);
	if(Result < 0)
	{
		ZEInt Error = WSAGetLastError();
		if (Error == WSAEWOULDBLOCK)
		{
			return ZE_SR_NO_PACKET;
		}
		else
		{
			Close();
			return ZE_SR_ERROR;
		}
	}

	return Result;
}
