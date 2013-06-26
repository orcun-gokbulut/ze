//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEServer.cpp
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

#include "ZEServer.h"
#include "ZESocket/ZESocket.h"
#include "ZEConnection.h"

#define TCP_PORT_NO	26100

ZEServer::ZEServer()
{
	Listener = new ZESocketTCPListener();
	if(!Listener->Create(ZEIPAddress::IPv4Any, TCP_PORT_NO))
		zeError("Can not create listener.");
}

ZEServer::~ZEServer()
{

}

void ZEServer::AcceptConnections()
{
	ZESocketTCP* NewTCP = NULL;
	NewTCP = Listener->Accept();

	if(NewTCP != NULL)
		AddConnection(new ZEConnection(NewTCP));
}

bool ZEServer::AddConnection(ZEConnection* Connection)
{
	if(Connection == NULL)
		return false;

	if(Connections.Exists(Connection))
		return false;

	Connections.Add(Connection);
	return true;
}

bool ZEServer::RemoveConnection(ZEConnection* Connection)
{
	if(Connection == NULL)
		return false;

	if(!Connections.Exists(Connection))
		return false;

	Connections.DeleteValue(Connection);
	return true;
}

void ZEServer::Process(float ElapsedTime)
{
	AcceptConnections();
	PacketManager.Process(ElapsedTime, Connections);
}

ZEPacketManagerServer* ZEServer::GetPacketManager()
{
	return &PacketManager;
}

bool ZEServer::SendPacket(ZEUInt16 PacketId, void* Data, ZESize DataSize, ZEConnection* Connection)
{
	bool  Result = false;
	ZEPacketHeader Header;
	Header.CommandId = PacketId;
	Header.Identifier = ZE_COMMAND_PACKET_HEADER_IDENTIFIER;
	Header.DataSize = DataSize;

	Result = SendData(&Header, sizeof(ZEPacketHeader), Connection);
	if(!Result)
		return false;

	return SendData(Data, DataSize, Connection);
}

bool ZEServer::SendData(void* Data, ZESize DataSize, ZEConnection* Connection)
{
	return Connection->SendData(Data, DataSize);
}

void ZEServer::BroadCast(void* Data, ZESize DataSize)
{
	for (ZESize I = 0; I < Connections.GetCount(); I++)
		Connections[I]->SendData(Data, DataSize);
}

void ZEServer::BroadCastPacket(ZEUInt16 PacketId, void* Data, ZESize DataSize)
{
	ZEPacketHeader Header;
	Header.CommandId = PacketId;
	Header.Identifier = ZE_COMMAND_PACKET_HEADER_IDENTIFIER;
	Header.DataSize = DataSize;

	for (ZESize I = 0; I < Connections.GetCount(); I++)
	{
		Connections[I]->SendData(&Header, sizeof(ZEPacketHeader));
		Connections[I]->SendData(Data, DataSize);
	}
}
