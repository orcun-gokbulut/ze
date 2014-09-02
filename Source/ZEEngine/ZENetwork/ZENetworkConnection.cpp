//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZENetworkConnection.cpp
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

#include "ZENetworkConnection.h"
#include "ZESocket/ZESocketUDP.h"
#include "ZEPacking.h"

ZEPackStruct
(
	struct ZENetworkIncommingPacketBlock
	{
		ZETimeStamp				ReceiveTime;
		bool					IsDelivered;
	}
);

ZEPackStruct
(
	struct ZENetworkOutgoingPacket
	{
		ZEUInt					Priority;
		ZETimeStamp				SentTime;
	}
);

ZENetworkConnection::ZENetworkConnection()
{
	Reset();
}

ZENetworkConnection::~ZENetworkConnection()
{
}

bool ZENetworkConnection::Send(void* Data, ZESize Size)
{
	ZESSize Result = Socket->Send(RemoteAddress, RemotePort, Data, Size);
	if (Result < 0)
	{
		if (Result == ZE_SR_ERROR)
			Status = ZE_NCS_ERROR;

		return false;
	}

	return true;
}

void ZENetworkConnection::Receive(void* Data, ZESize Size)
{
	ZENetworkPacketHeader* PacketHeader = (ZENetworkPacketHeader*)Data;
	ZEBYTE* PacketData = (ZEBYTE*)Data + sizeof(ZENetworkPacketHeader);
	ZESize PacketSize = Size - sizeof(ZENetworkPacketHeader);

	if (PacketHeader->Flags & ZE_NPF_ACKNOWLEDGEMENT)
	{
		ReceivedAcknowledgement(*PacketHeader);
	}
	else
	{
		Lock.Lock();
		AddReceivedPacket(*PacketHeader, PacketData, PacketSize);
		Lock.Unlock();
	}
}

void ZENetworkConnection::SendConnectionPacket()
{
	ZENetworkPacketHeader ConnectionPacket;
	ConnectionPacket.Id = ZE_NPI_CONNECT;
	ConnectionPacket.Squence = 0;
	ConnectionPacket.Flags = ZE_NPF_NONE;
	Send(&ConnectionPacket, sizeof(ZENetworkPacketHeader));
}

void ZENetworkConnection::SendDisconnectPacket()
{
	ZENetworkPacketHeader DisconnectPacket;
	DisconnectPacket.Id = ZE_NPI_DISCONNECT;
	DisconnectPacket.Squence = 0;
	DisconnectPacket.Flags = ZE_NPF_NONE;
	Send(&DisconnectPacket, sizeof(ZENetworkPacketHeader));
}

void ZENetworkConnection::SendPingPacket()
{
	ZENetworkPacketHeader PingPacket;
	PingPacket.Id = ZE_NPI_PING;
	PingPacket.Squence = 0;
	PingPacket.Flags = ZE_NPF_NONE;
	Send(&PingPacket, sizeof(ZENetworkPacketHeader));
}

void ZENetworkConnection::SendAcknowledgementPacket(const ZENetworkPacketHeader& PacketHeader)
{
	ZENetworkPacketHeader AcknowledgementPacket = PacketHeader;
	AcknowledgementPacket.Flags |= ZE_NPF_ACKNOWLEDGEMENT;
	Send(&AcknowledgementPacket, sizeof(ZENetworkPacketHeader));
}

void ZENetworkConnection::ProcessIncommingPackets()
{
	IncommingBuffer.Defrag();

	ZEBufferBlockPointer Current = IncommingBuffer.GetFirstBlock();
	while(Current != ZE_BP_NONE)
	{
		ZENetworkIncommingPacketBlock* PacketWarpper = (ZENetworkIncommingPacketBlock*)((ZEBYTE*)OutgoingBuffer.GetBlockData(Current));
		ZENetworkPacketHeader* PacketHeader = (ZENetworkPacketHeader*)((ZEBYTE*)OutgoingBuffer.GetBlockData(Current) + sizeof(ZENetworkIncommingPacketBlock));
		if (PacketHeader->Flags | ZE_NPF_RELIABLE)
			SendAcknowledgementPacket(*PacketHeader);
	}
}

void ZENetworkConnection::ProcessOutgoingPackets()
{
	ZEBufferBlockPointer Current = OutgoingBuffer.GetFirstBlock();
	while(Current != ZE_BP_NONE)
	{
		ZENetworkOutgoingPacket* PacketBlock = (ZENetworkOutgoingPacket*)OutgoingBuffer.GetBlockData(Current);
		ZENetworkPacketHeader* PacketHeader = (ZENetworkPacketHeader*)((ZEBYTE*)OutgoingBuffer.GetBlockData(Current) + sizeof(ZENetworkOutgoingPacket));
		if ((PacketHeader->Flags & ZE_NPF_RELIABLE) == 0)
			OutgoingBuffer.DeleteBlock(Current);
		else
			PacketBlock->Priority--;

		Send((ZEBYTE*)OutgoingBuffer.GetBlockData(Current) + sizeof(ZENetworkOutgoingPacket), OutgoingBuffer.GetBlockSize(Current) - sizeof(ZENetworkOutgoingPacket));

		Current = OutgoingBuffer.MoveNextBlock(Current);
	}

	OutgoingBuffer.Defrag();
}

void ZENetworkConnection::ProcessPing()
{
	if (LastSentTime + ZETimeStamp::CreateTime(0, 0, 0, 500) < ZETimeStamp::Now())
		return;

	SendPingPacket();
}

void ZENetworkConnection::ReceivedAcknowledgement(const ZENetworkPacketHeader& Header)
{
	ZEBufferBlockPointer Current = OutgoingBuffer.GetFirstBlock();
	while(Current != ZE_BP_NONE)
	{
		ZENetworkPacketHeader* CurrentHeader = (ZENetworkPacketHeader*)((ZEBYTE*)OutgoingBuffer.GetBlockData(Current) + sizeof(ZENetworkOutgoingPacket));
		if (CurrentHeader->Squence == Header.Squence &&
			CurrentHeader->Id == Header.Id &&
			CurrentHeader->Flags == (Header.Flags & ~ZE_NPF_ACKNOWLEDGEMENT))
		{
			OutgoingBuffer.DeleteBlock(Current);
			break;
		}
	}
}

void ZENetworkConnection::AddReceivedPacket(const ZENetworkPacketHeader& Header, void* Data, ZESize DataSize)
{
	ZEBufferBlockPointer Current = IncommingBuffer.GetFirstBlock();
	while(Current != ZE_BP_NONE)
	{
		ZENetworkPacketHeader* CurrentHeader = (ZENetworkPacketHeader*)IncommingBuffer.GetBlockData(Current);
		if (CurrentHeader->Id == Header.Id && CurrentHeader->Flags == Header.Flags && CurrentHeader->Squence == Header.Squence)
			return;
	}

	ZEBufferBlockPointer Pointer = IncommingBuffer.AddBlock(DataSize + sizeof(ZENetworkPacketHeader));
	if (Pointer == ZE_BP_NONE)
		return;

	memcpy(IncommingBuffer.GetBlockData(Pointer), &Header, sizeof(ZENetworkPacketHeader));
	memcpy((ZEBYTE*)IncommingBuffer.GetBlockData(Pointer) + sizeof(ZENetworkPacketHeader), Data, DataSize);
}

ZENetworkConnectionStatus ZENetworkConnection::GetStatus()
{
	return Status;
}

const ZEIPAddress& ZENetworkConnection::GetRemoteAddress()
{
	return RemoteAddress;
}

ZEUInt16 ZENetworkConnection::GetRemotePort()
{
	return RemotePort;
}

bool ZENetworkConnection::HasReceivedPacket()
{
	return IncommingBuffer.GetBlockCount() != 0;
}

bool ZENetworkConnection::SendPacket(ZEUInt16 Id, void* Data, ZESize Size, ZENetworkPacketReliability Reliablility, ZENetworkPacketPriority Priority)
{
	if (Status == ZE_NCS_CONNECTING || Status == ZE_NCS_ERROR || Status == ZE_NCS_DISCONNECTED)
		return false;

	Lock.Lock();

	ZEBufferBlockPointer Pointer = OutgoingBuffer.AddBlock(sizeof(ZENetworkOutgoingPacket) + sizeof(ZENetworkPacketHeader) + Size);
	if (Pointer == ZE_BP_NONE)
	{
		Lock.Unlock();
		return false;
	}

	ZENetworkOutgoingPacket* PacketBlock = (ZENetworkOutgoingPacket*)((ZEBYTE*)OutgoingBuffer.GetBlockData(Pointer));
	ZENetworkPacketHeader* PacketHeader = (ZENetworkPacketHeader*)((ZEBYTE*)OutgoingBuffer.GetBlockData(Pointer) + sizeof(ZENetworkOutgoingPacket));
	void* PacketData = (ZEBYTE*)OutgoingBuffer.GetBlockData(Pointer) + sizeof(ZENetworkOutgoingPacket) + sizeof(ZENetworkPacketHeader);
	
	PacketBlock->SentTime = CurrentTime;
	PacketBlock->Priority = Priority * 5;
	PacketHeader->Id = Id;
	switch(Reliablility)
	{
		case ZE_NPR_UNRELIABLE:
			PacketHeader->Flags = 0;
			PacketHeader->Squence = 0;
			break;

		case ZE_NPR_RELIABLE:
			PacketHeader->Flags = ZE_NPF_RELIABLE;
			PacketHeader->Squence = LastSendSquenceNumber;
			LastSendSquenceNumber++;
			break;

		case ZE_NPR_RELIABLE_ORDERED:
			PacketHeader->Flags = ZE_NPF_ORDERED;
			PacketHeader->Squence = LastSendOrderNumber;
			LastSendOrderNumber++;
			break;
	}

	memcpy(PacketData, Data, Size);

	Lock.Unlock();

	return true;
}

bool ZENetworkConnection::ReceivePacket(ZEUInt16 Id, void* Data, ZESize& Size)
{
	if (Status == ZE_NCS_CONNECTING || Status == ZE_NCS_ERROR || Status == ZE_NCS_DISCONNECTED)
		return false;

	Lock.Lock();

	ZENetworkPacketHeader* PacketHeader = NULL;	
	ZEBufferBlockPointer Pointer = IncommingBuffer.GetFirstBlock();
	while(Pointer != ZE_BP_NONE)
	{
		PacketHeader = (ZENetworkPacketHeader*)IncommingBuffer.GetBlockData(Pointer);
		if (PacketHeader->Flags == ZE_NPF_ORDERED)
		{
			if (PacketHeader->Squence == LastReceivedOrderNumber)
				break;
		}
		else
		{
			break;
		}
		Pointer = IncommingBuffer.MoveNextBlock(Pointer);
	}
	
	if (Pointer == ZE_BP_NONE)
	{
		Lock.Unlock();
		return false;
	}
	
	if (Data == NULL)
	{
		Id = ((ZENetworkPacketHeader*)IncommingBuffer.GetBlockData(Pointer))->Id;
		Size = IncommingBuffer.GetBlockSize(Pointer) - sizeof(ZENetworkPacketHeader);
		Lock.Unlock();
		return true;
	}

	if (Size < IncommingBuffer.GetBlockCount())
	{
		Lock.Unlock();
		return false;
	}

	Id = ((ZENetworkPacketHeader*)IncommingBuffer.GetBlockData(Pointer))->Id;
	Size = IncommingBuffer.GetBlockSize(Pointer) - sizeof(ZENetworkPacketHeader);
	memcpy(Data, (ZEBYTE*)IncommingBuffer.GetBlockData(Pointer) + sizeof(ZENetworkPacketHeader), Size);

	IncommingBuffer.DeleteBlock(Pointer);

	Lock.Unlock();
	return true;
}

void ZENetworkConnection::Reset()
{
	Status = ZE_NCS_DISCONNECTED;
	RemoteAddress = ZEIPAddress::Any;
	RemotePort = ZE_SP_ANY;
	OutgoingBuffer.Clear();
	IncommingBuffer.Clear();
	LastSendSquenceNumber = 0;
	LastSendOrderNumber = 0;
	LastReceivedOrderNumber = 0;
	LastReceivedTime = 0;
	LastSentTime = 0;
	CurrentTime = 0;
}

void ZENetworkConnection::Process()
{
	CurrentTime = ZETimeStamp::Now();
	if (Status == ZE_NCS_CONNECTED)
	{
		Lock.Lock();
		ProcessIncommingPackets();
		ProcessOutgoingPackets();
		ProcessPing();
		Lock.Unlock();
	}
}
