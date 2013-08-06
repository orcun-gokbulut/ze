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

static ZEInt Compare(const ZENetworkPacketHandlerReg& A, ZEInt B)
{
	if (A.PacketId < B)
		return -1;
	else if (A.PacketId > B)
		return 1;
	else
		return 0;
}
void ZENetworkClient::ProcessPacket(void* Packet, ZESize Size)
{
	if (Size < sizeof(ZENetworkPacketHeader))
		return;

	ZENetworkPacketHeader* Header = (ZENetworkPacketHeader*)Packet;
	if (Header->Receiver != ClientId)
		return;

	if (Header->Sender != ServerId)
		return;

	ZESSize Index = PacketHandlers.BinarySearch<ZEInt, Compare>(Header->PacketId);
	if (Index == -1)
		return;

	PacketHandlers[Index].Handler(Header, (ZEBYTE*)Packet + sizeof(ZENetworkPacketHeader), Size - sizeof(ZENetworkPacketHeader));
}

void ZENetworkClient::ProcessPackets()
{
	ZEBYTE PacketBuffer[ZE_MAX_UDP_PACKET_SIZE];
	while(true)
	{
		ZESSize Result = ServerSocket.Receive(PacketBuffer, ZE_MAX_UDP_PACKET_SIZE);
		if (Result <= 0)
			break;

		ProcessPacket(PacketBuffer, Result);
	}
}

void ZENetworkClient::ProcessObject(ZENetworkObject* Object)
{

}
	
void ZENetworkClient::ProcessObjects()
{
	for (ZESize I = 0; I < Objects.GetCount(); I++)
		ProcessObject(Objects[I]);
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
	while(true)
	{
		ZEBYTE PacketBuffer[ZE_MAX_UDP_PACKET_SIZE];
		ZESSize Result = ServerSocket.Receive(PacketBuffer, ZE_MAX_UDP_PACKET_SIZE);
		if (Result >= 0)
			break;

		ProcessPacket(PacketBuffer, Result);
	}
}
