//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZENetworkConnection.h
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
#ifndef	__ZE_NETWORK_CONNECTION_H__
#define __ZE_NETWORK_CONNECTION_H__

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEBuffer.h"
#include "ZESocket/ZEIPAddress.h"
#include "ZETimeStamp.h"
#include "ZEPacking.h"

class ZESocket;
struct ZENetworkPacketHeader;

enum ZENetworkConnectionStatus
{
	ZE_NCS_CONNECTED,
	ZE_NCS_CONNECTING,
	ZE_NCS_ERROR,
	ZE_NCS_DISCONNECTED
};

enum ZENetworkPacketReliability 
{
	ZE_NPR_UNRELIABLE,
	ZE_NPR_RELIABLE,
	ZE_NPR_RELIABLE_ORDERED
};

enum ZENetworkPacketPriority
{
	ZE_NPP_ULTRA_LOW,
	ZE_NPP_LOW,
	ZE_NPP_NORMAL,
	ZE_NPP_HIGH,
	ZE_NPP_ULTRA_HIGH,
	ZE_BPP_REALTIME
};

#define ZE_NPI_CONNECT			0
#define ZE_NPI_DISCONNECT		1
#define ZE_NPI_PING				2


typedef ZEUInt8 ZENetworkPacketFlags;
#define ZE_NPF_NONE				0
#define ZE_NPF_UNRELIABLE		0
#define ZE_NPF_RELIABLE			1
#define ZE_NPF_ORDERED			2
#define ZE_NPF_ACKNOWLEDGEMENT	4

#define ZE_MAX_NETWORK_PACKET_SIZE (0xFFFF - sizeof(ZENetworkPacketHeader))

ZEPackStruct
(
	struct ZENetworkPacketHeader
	{
		ZEUInt16				Id;
		ZEUInt8					SessionId;
		ZENetworkPacketFlags	Flags;
		ZEUInt32				Squence;
	}
);

class ZESocketUDP;

class ZENetworkConnection
{
	friend class ZENetworkClient;
	friend class ZENetworkServer;
	private:
		ZESocketUDP*					Socket;
		ZEIPAddress						RemoteAddress;
		ZEUInt16						RemotePort;

		ZELock							Lock;
		ZENetworkConnectionStatus		Status;

		ZEBuffer						OutgoingBuffer;
		ZEBuffer						IncommingBuffer;

		ZEUInt							LastSendSquenceNumber;
		ZEUInt							LastSendOrderNumber;
		ZEUInt							LastReceivedOrderNumber;

		ZETimeStamp						LastReceivedTime;
		ZETimeStamp						LastSentTime;
		ZETimeStamp						CurrentTime;

		ZEInt							ClientId;

		bool							Send(void* Data, ZESize Size);
		void							Receive(void* Data, ZESize Size);

		void							SendConnectionPacket();
		void							SendDisconnectPacket();
		void							SendPingPacket();
		void							SendAcknowledgementPacket(const ZENetworkPacketHeader& PacketHeader);

		void							ProcessIncommingPackets();
		void							ProcessOutgoingPackets();

		void							ReceivedAcknowledgement(const ZENetworkPacketHeader& Header);						

		void							SendPackets();
		void							AddReceivedPacket(const ZENetworkPacketHeader& Header, void* Data, ZESize DataSize);

		void							ProcessPackets();
		void							ProcessPing();

		void							Reset();

										ZENetworkConnection();
										~ZENetworkConnection();

	public:
		ZENetworkConnectionStatus		GetStatus();
		ZEInt							GetClientId();
		ZEInt							GetSessionId();

		const ZEIPAddress&				GetRemoteAddress();
		ZEUInt16						GetRemotePort();

		bool							HasReceivedPacket();
		bool							SendPacket(ZEUInt16 Id, void* Data, ZESize Size, ZENetworkPacketReliability Reliablility, ZENetworkPacketPriority Priority);
		bool							ReceivePacket(ZEUInt16 Id, void* Data, ZESize& Size);

		void							Process();
};

#endif
