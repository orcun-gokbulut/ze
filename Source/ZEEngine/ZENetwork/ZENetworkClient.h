//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZENetworkClient.h
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

#pragma once
#ifndef	__ZE_NETWORK_CLIENT_H__
#define __ZE_NETWORK_CLIENT_H__

#include "ZETypes.h"
#include "ZEDS\ZEArray.h"
#include "ZESocket\ZEIPAddress.h"
#include "ZEDS\ZEBuffer.h"
#include "ZESocket/ZESocketUDP.h"
#include "ZEDS\ZEDelegate.h"
#include "ZETexture\ZEPixel.h"

enum ZENetworkClientStatus
{
	ZE_NCS_DISCONNECTED,
	ZE_NCS_CONNECTING,
	ZE_NCS_CONNECTED
};

ZEPackStruct
(
	struct ZENetworkPacketHeader
	{
		ZEUInt8		Header;
		ZEUInt16	Sender;
		ZEUInt16	Receiver;
		ZEUInt16	PacketId;
	};
);

typedef ZEDelegate<void (const ZENetworkPacketHeader*, void*, ZESize)> ZENetworkPacketHandler;

struct ZENetworkPacketHandlerReg
{
	ZEUInt PacketId;
	ZENetworkPacketHandler Handler;
};

class ZENetworkObject;
class ZENetworkClient
{
	private:
		ZESocketUDP								ServerSocket;
		ZEUInt64								ServerTime;
		ZEUInt64								LastPackageTime;

		ZEBuffer								SendBufferHigh;
		ZEBuffer								SendBufferNormal;
		ZEBuffer								SendBufferLow;

		ZEUInt16								ClientId;
		ZEUInt16								ServerId;

		ZEArray<ZENetworkObject*>				Objects;
		ZEArray<ZENetworkPacketHandlerReg>		PacketHandlers;

		void									ProcessPacket(void* Packet, ZESize Size);
		void									ProcessPackets();

		void									ProcessObject(ZENetworkObject* Object);
		void									ProcessObjects();

	public:
		void									RegisterObject(ZENetworkObject* Object);
		void									UnregisterObject(ZENetworkObject* Object);

		void									AddPacketHandler(ZEUInt PacketId, const ZENetworkPacketHandler& Handler);
		void									RemovePacketHandler(ZEUInt PacketId);

		ZENetworkClientStatus					GetStatus();

		ZEUInt16								GetClientId();
		ZEUInt16								GetServerId();

		void									SetServerIPAddress();
		ZEIPAddress								GetServerIPAddress();

		void									SetServerPort(ZEUInt16 Port);
		ZEUInt16								GetServerPort();
		
		void									SendPacket(ZEUInt16 PacketId, void* Data, ZESize Size);

		bool									Connect();
		void									Disconnect();

		void									Process();
};


class ZENetworkConnection;
/*
class ZENetworkClient
{
	private:
		ZENetworkConnection*					Connection;
		ZENetworkClientStatus					Status;
		ZEIPAddress								ServerIPAddress;
		ZEUInt16								ServerPort;

	public:
		ZENetworkConnection*					GetConnection();
		ZENetworkClientStatus					GetStatus();

		void									SetServerIPAddress(const ZEIPAddress& ServerIp);
		const ZEIPAddress&						GetServerIPAddress();

		void									SetServerPort(ZEUInt16 Port);
		ZEUInt16								GetServerPort();

		void									Connect();
		void									Disconnect();		
};

enum ZENetworkPacketPriority
{
	ZE_NPP_REAL_TIME,
	ZE_NPP_NORMAL
};


struct ZENetworkPacket
{
	ZEInt										PacketId;
	ZENetworkPacketPriority						Priority;
	ZESize										Size;
};

class ZENetworkConnection
{
	private:
		ZESocketUDP								Socket;

		ZEArray<ZEUInt8>						Packets;
		ZESize									PacketStart;
		ZESize									PacketCount;

		ZEArray<ZEUInt8>						ReceivedPackets;
		ZESize									ReceivedPacketStart;
		ZESize									ReceivedPacketCount;


	public:
		bool									SendPacket(ZEUInt PacketId, ZENetworkPacketPriority Priorty, void* Packet, ZESize Size);
		bool									ReceivePacket(ZEUInt& PacketId, void* Packet, ZESize& Size);
};


typedef ZEDelegate<void (ZEUInt, void*, ZESize)> ZENetworkMessageHandler;

class ZENetworkObjectClient
{
	private:
		ZEArray<ZEUInt8>						LocalState;
		ZEArray<ZEUInt8>						LocalStateAcknowledged;
		ZEArray<ZEUInt8>						RemoteState;
		
		ZEUInt32								LocalStateNumber;
		ZEUInt32								LocalStateNumberAcknowledged;
		ZEUInt32								RemoteStateNumber;

		ZEArray<Me
		void									CreateDelta(void* Buffer, ZESize& Size);
		void									ApplyDelta(void* Buffer, ZESize& Size);

	public:
		void*									GetLocalState();
		void									SetLocalState(void* State);

		void									SetLocalStateSize(ZESize Size);
		ZESize									GetLocalStateSize();

		void*									GetRemoteState();
		ZESize									GetRemoteStateSize();

		void									Dirty();

		bool									SendMessage(ZEUInt32 MessageID, void* Message, ZESize MessageSize);
		bool									ReceiveMessage(ZEUInt32& MessageId, void* Message, ZESize& MessageSize);

		void									ProcessMessages();
		void									RegisterMessageHandler(ZEUInt MessageId, const ZENetworkMessageHandler& MessageHandler);
		void									UnregisterMessageHandler(ZEUInt MessageId);
};
*/
#endif
