//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZENetworkServer - Packet Manager.cpp
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

#include "ZENetworkServer.h"

ZENetworkConnection* ZENetworkServer::GetConnection(const ZEIPAddress& Address, ZEUInt16 Port)
{
	for (ZESize I = 0; I < Connections.GetCount(); I++)
		if (Connections[I].ClientAddress == Address && Connections[I].ClientPort == Port)
			return &Connections[I];

	return NULL;
}

void ZENetworkServer::RouteIncommingPackets()
{
	ZEBYTE Buffer[ZE_MAX_NETWORK_PACKET_SIZE + sizeof(ZENetworkPacketHeader)];
	ZEIPAddress Address;
	ZEUInt16 Port;
	while(true)
	{
		ZESSize Result = Socket.Receive(Address, Port, Buffer, sizeof(Buffer));
		if (Result < 0)
		{
			if (Result == ZE_SR_ERROR)
				Stop();

			return;
		}

		ZENetworkConnection* Connection = GetConnection(Address, Port);
		ZENetworkPacketHeader* PacketHeader = (ZENetworkPacketHeader*)Buffer;

		if (PacketHeader->Id != ZE_NPI_CONNECT)
		{
			if (Connection == NULL)
				continue;

			for (ZESize I = 0; I < PacketHandlers.GetCount(); I++)
				if (PacketHandlers[I].Id == PacketHeader->Id)
				{
					PacketHandlers[I].Callback(*Connection, PacketHeader->Id, Buffer + sizeof(ZENetworkPacketHeader), Result - sizeof(ZENetworkPacketHeader));
					break;
				}
		}
		else
		{
			if (Connection == NULL)
			{
				PacketHeader->Flags |= ZE_NPF_ACKNOWLEDGEMENT;
				Socket.Send(Address, Port, &PacketHeader, sizeof(ZENetworkPacketHeader));
				Connection = Connections.Add();
				Connection->Socket = &Socket;
				Connection->RemoteAddress = Address;
				Connection->RemotePort = Port;
				Connection->Status = ZE_NCS_CONNECTING;
				continue;
			}
			else
			{
				if ((PacketHeader->Flags & ZE_NPF_ACKNOWLEDGEMENT) != 0)
				{
					Connection->Status = ZE_NCS_CONNECTED;
					continue;
				}
				else
				{
					PacketHeader->Flags |= ZE_NPF_ACKNOWLEDGEMENT;
					Socket.Send(Address, Port, &PacketHeader, sizeof(ZENetworkPacketHeader));
				}
			}
		}
	}
}

void ZENetworkServer::ProcessConnections()
{
	for (ZESize I = 0; Connections.GetCount(); I++)
	{
		if (Connections[I].GetStatus() == ZE_NCS_CONNECTED)
		{
			Connections[I].Process();
		}
		else if (Connections[I].GetStatus() == ZE_NCS_CONNECTING)
		{
			ZENetworkPacketHeader Packet;
			Packet.Id = ZE_NPI_CONNECT;
			Packet.Flags = ZE_NPF_ACKNOWLEDGEMENT;
			Socket.Send(Connections[I].GetStatus(), Connection[I].GetIPort, &Packet, sizeof(ZENetworkPacketHeader));
		}
	}		
}

void ZENetworkServer::SetServerAddress(const ZEIPAddress& Address)
{
	ServerAddress = Address;
}

const ZEIPAddress& ZENetworkServer::GetServerAddress()
{
	return ServerAddress;
}

void ZENetworkServer::SetServerPort(ZEUInt16 Port)
{
	ServerPort = Port;
}

ZEUInt16 ZENetworkServer::GetServerPort()
{
	return ServerPort;
}

static int PacketHandlerCompare(const ZENetworkServerPacketHandler* A, const ZENetworkServerPacketHandler* B)
{
	if (A->Id > B->Id)
		return 1;
	else if (A->Id < B->Id)
		return -1;
	else 
		return 0;
}

void ZENetworkServer::AddPacketHandler(ZEUInt16 PacketId, const ZENetworkServerPacketHandler& Handler)
{
	PacketHandlers.AddOrderedInc<PacketHandlerCompare>(Handler);
}

void ZENetworkServer::RemovePacketHandler(ZEUInt16 PacketId)
{
	for (ZESize I = 0; I < PacketHandlers.GetCount(); I++)
		if (PacketHandlers[I].Id == PacketId)
		{
			PacketHandlers.DeleteAt(I);
			return;
		}
}

bool ZENetworkServer::Start()
{
	return Socket.Open();
}

void ZENetworkServer::Stop()
{
	for (ZESize I = 0; I < Connections.GetCount(); I++)
		Connections[I].Disconnect();

	Socket.Close();
}

void ZENetworkServer::Process()
{
	RouteIncommingPackets();
	ProcessHandshakes();
	PrcoessConnections();
}
