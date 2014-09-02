//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZENetworkClient.cpp
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

ZENetworkClient::ZENetworkClient()
{
	HandshakeTime = 0;
	HandshakePhase = 0;

	ServerAddress = ZEIPAddress::Any;
	ServerPort = ZE_SP_ANY;
}

ZENetworkClient::~ZENetworkClient()
{
	Disconnect();
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

void ZENetworkClient::Process()
{
	if (Connection.GetStatus() == ZE_NCS_CONNECTED)
	{
		RouteIncommingPackets();
		Connection.Process();
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
	 bool Result = Socket.Open();
	 if (!Result)
		 return false;

	 Connection.Status = ZE_NCS_CONNECTING;
	 
	 HandshakePhase = 0;
	 HandshakeTime = 0;

	 ProcessHandshake();

	 return true;
 }

 void ZENetworkClient::Disconnect()
 {
	 Socket.Close();
	 Connection.Reset();
 }
