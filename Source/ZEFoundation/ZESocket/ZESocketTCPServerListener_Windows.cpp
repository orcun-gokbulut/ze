//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESocketTCPServerListener_Windows.cpp
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

#include "ZESocketTCPServerListener.h"

#include <Winsock2.h>
#include "ZESocketTCPServer.h"

ZESocketTCPServerListenerStatus ZESocketTCPServerListener::GetStatus()
{
	return Status;
}

bool ZESocketTCPServerListener::Open()
{
	if (Status != ZE_STSLS_NOT_LISTENING)
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
			zeError("Can not create TCP listener socket. WinSock Error code : %d", WSAGetLastError());
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

	listen(Socket, QueueLength);

	Status = ZE_STSLS_LISTENING;
	return true;
}

bool ZESocketTCPServerListener::Accept(ZESocketTCPServer& ServerSocket)
{
	sockaddr_in Address;
	int AddressSize = sizeof(sockaddr_in);
	SOCKET ConnectionSocket = accept(this->Socket, (sockaddr*)&Address, &AddressSize);
	if (ConnectionSocket == INVALID_SOCKET)
	{
		int Error = WSAGetLastError();
		if (Error != WSAEWOULDBLOCK)
			Close();
		return false;
	}
	
	ServerSocket.Socket = ConnectionSocket;
	ServerSocket.BindIPAddress = BindIPAddress;
	ServerSocket.Port = Port;
	ZEIPAddress::FromSockaddr_in(ServerSocket.ClientIPAddress, &Address);
	
	return true;
}

void ZESocketTCPServerListener::Close()
{
	if (Socket != NULL)
		return;

	closesocket(Socket);
	Socket = NULL;

	Status = ZE_STSLS_NOT_LISTENING;
}
