//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZENetworkConnectionClient.cpp
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

#include "ZENetworkConnectionClient.h"

bool ZENetworkConnectionClient::Send(void* Data, ZESize Size)
{

	ZESSize Result = Socket.Send(Data, Size);
	if (Result < 0)
	{
		if (Result == ZE_SR_ERROR)
			Disconnect();

		return false;
	}

	return true;
}

void ZENetworkConnectionClient::SetServerAddress(const ZEIPAddress& Address)
{
	Socket.SetBindIPAddress(Address);
}

const ZEIPAddress& ZENetworkConnectionClient::GetServerAddress()
{
	return Socket.GetBindIPAddress();
}

void ZENetworkConnectionClient::SetServerPort(ZEUInt16 Port)
{
	Socket.SetRemotePort(Port);
}

ZEUInt16 ZENetworkConnectionClient::GetServerPort()
{
	return Socket.GetRemotePort();
}

void ZENetworkConnectionClient::Connect()
{
	if (Status == ZE_NCS_CONNECTING || Status == ZE_NCS_CONNECTED)
		return;

	if (Socket.Open())
	{
		Status = ZE_NCS_ERROR;
		return;
	}
}

void ZENetworkConnectionClient::Disconnect()
{
	if (Status == ZE_NCS_CONNECTING || Status == ZE_NCS_ERROR)
	{
		Socket.Close();
	}
	else
	{
		ZENetworkPacketHeader DisconnectPacket;
		DisconnectPacket.Id = ZE_NPI_DISCONNECT;
		DisconnectPacket.Flags = ZE_NPF_NONE;
		DisconnectPacket.Squence = 0;
		Socket.Send(&DisconnectPacket, sizeof(ZENetworkPacketHeader));
		Socket.Close();
	}
}

void ZENetworkConnectionClient::Process()
{
	CurrentTime = ZETimeStamp::Now();
	if (Status == ZE_NCS_CONNECTED)
	{
		ReceivePackets();
		Lock.Lock();
		ProcessPackets();
		ProcessPing();
		Lock.Unlock();
	}
}
