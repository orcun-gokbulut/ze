//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZENetworkClient - Packet Manager.cpp
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

#include "ZENetworkClient.h"

static ZEInt Compare(const ZENetworkPacketHandlerReg& A, ZEInt B)
{
	if (A.PacketId < B)
		return -1;
	else if (A.PacketId > B)
		return 1;
	else
		return 0;
}


void ZENetworkClient::RouteIncommingPackets()
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
				Disconnect();
			break;
		}

		if (Address != Connection.GetRemoteAddress() || Port != Connection.GetRemotePort())
			continue;

		Connection.Receive(Buffer, Result);
	}
}


void ZENetworkClient::RouteIncommingPackets()
{
	ZEBYTE PacketBuffer[ZE_MAX_UDP_PACKET_SIZE];
	while(true)
	{
		ZESSize Result = Socket.Receive(PacketBuffer, ZE_MAX_UDP_PACKET_SIZE);
		if (Result <= 0)
			break;

		if (Result < sizeof(ZENetworkPacketHeader))
			return;

		ZENetworkPacketHeader* Header = (ZENetworkPacketHeader*)PacketBuffer;
		ZESSize Index = PacketHandlers.BinarySearch<ZEInt, Compare>(Header->Id);
		if (Index == -1)
			continue;

		PacketHandlers[Index].Handler(Header, (ZEBYTE*)PacketBuffer + sizeof(ZENetworkPacketHeader), Result - sizeof(ZENetworkPacketHeader));
	}
}
	
void ZENetworkClient::ProcessObjects()
{
	for (ZESize I = 0; I < Objects.GetCount(); I++)
	{

	}
}

void ZENetworkClient::RegisterObject(ZENetworkObject* Object)
{
	if (Objects.Exists(Object))
		return;

	Objects.Add(Object);
}

void ZENetworkClient::UnregisterObject(ZENetworkObject* Object)
{
	Objects.DeleteValue(Object);
}

void ZENetworkClient::AddPacketHandler(ZEUInt PacketId, const ZENetworkPacketHandler& Handler)
{
	for (ZESize I = 0; I < PacketHandlers.GetCount(); I++)
	{
		if (PacketHandlers[I].PacketId == PacketId)
		{
			return;
		}
		else if (PacketHandlers[I].PacketId > PacketId)
		{
			PacketHandlers.Insert(I);
			PacketHandlers[I].PacketId = PacketId;
			PacketHandlers[I].Handler = Handler;
		}
	}
}

void ZENetworkClient::RemovePacketHandler(ZEUInt PacketId)
{
	for (ZESize I = 0; I < PacketHandlers.GetCount(); I++)
	{
		if (PacketHandlers[I].PacketId == PacketId)
		{
			PacketHandlers.DeleteAt(I);
			return;
		}
	}
}

void ZENetworkClient::Process()
{
	if (Connection.GetStatus() == ZE_NCS_CONNECTED)
	{
		RouteIncommingPackets();
		Connection.Process();
		ProcessPackets();
	}
	else if (Connection.GetStatus() == ZE_NCS_CONNECTING)
	{
		ProcessHandshake();
	}
}

void ZENetworkClient::ProcessHandshake()
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
				Disconnect();
			break;
		}

		if (Address != Connection.GetRemoteAddress() || Port != Connection.GetRemotePort())
			continue;

		ZENetworkPacketHeader* Header = (ZENetworkPacketHeader*)Buffer;

		if (HandshakePhase == 0)
		{
			if (Header->Id == ZE_NPI_CONNECT && Header->Flags == ZE_NPF_ACKNOWLEDGEMENT)
				HandshakePhase = 1;
		}
	}

	if (HandshakeTime + ZETimeStamp::CreateTime(0, 0, 0, 500) < ZETimeStamp::Now())
		return;

	ZENetworkPacketHeader ConnectionPacket;
	ConnectionPacket.Squence = 0;
	if (HandshakePhase == 0)
	{
		ConnectionPacket.Id = ZE_NPI_CONNECT;
		ConnectionPacket.Flags = ZE_NPF_NONE;
	}
	else if (HandshakePhase == 1)
	{
		ConnectionPacket.Id = ZE_NPI_CONNECT;
		ConnectionPacket.Flags = ZE_NPF_ACKNOWLEDGEMENT;
	}

	ZESSize Result = Socket.Send(GetServerAddress(), GetServerPort(), &ConnectionPacket, sizeof(ZENetworkPacketHeader));
	if (Result < 0)
		if (Result == ZE_SR_ERROR)
			Disconnect();
}

void ZENetworkClient::SetServerAddress(const ZEIPAddress& Address)
{
	ServerAddress = Address;
}

const ZEIPAddress& ZENetworkClient::GetServerAddress()
{
	return ServerAddress;
}

void ZENetworkClient::SetServerPort(ZEUInt16 Port)
{
	ServerPort = Port;
}

ZEUInt16 ZENetworkClient::GetServerPort()
{
	return ServerPort;
}

 bool ZENetworkClient::Connect()
 {
	 if (Connection.GetStatus() == ZE_NCS_CONNECTED || Connection.GetStatus() == ZE_NCS_CONNECTING)
		 return true;

	 Socket.SetRemoteIPAddress(Connection.RemoteAddress);
	 Socket.SetRemotePort(Connection.RemotePort);
	 return Socket.Open();
 }

 void ZENetworkClient::Disconnect()
 {
	 Socket.Close();
	 Connection.Reset();
 }
