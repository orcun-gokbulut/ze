//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESocketUDP.cpp
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

#include "ZESocketUDP.h"

void ZESocketUDP::SetBindIPAddress(const ZEIPAddress& IPAddress)
{
	BindIPAddress = IPAddress;
}

const ZEIPAddress& ZESocketUDP::GetBindIPAddress() const
{
	return BindIPAddress;
}

void ZESocketUDP::SetBindPort(ZEUInt16 Port)
{
	BindPort = Port;
}

ZEUInt16 ZESocketUDP::GetBindPort() const
{
	return BindPort;
}

void ZESocketUDP::SetRemoteIPAddress(const ZEIPAddress& IPAddress)
{
	RemoteIPAddress = IPAddress;
}

const ZEIPAddress& ZESocketUDP::GetRemoteIPAddress() const
{
	return RemoteIPAddress;
}

void ZESocketUDP::SetRemotePort(ZEUInt16 Port)
{
	RemotePort = Port;
}

ZEUInt16 ZESocketUDP::GetRemotePort()
{
	return RemotePort;
}

ZESSize ZESocketUDP::Send(const void* Buffer, ZESize BufferSize)
{
	return Send(RemoteIPAddress, (RemotePort != -1 ? RemotePort : BindPort), Buffer, BufferSize);
}

ZESSize ZESocketUDP::Receive(void* Buffer, ZESize BufferSize)
{
	ZEIPAddress Address;
	ZEUInt16 Port;
	ZESSize Result = Receive(Address, Port, Buffer, BufferSize);

	if (Result < 0)
		return Result;
	
	if (Address != RemoteIPAddress)
		return ZE_SR_PACKET_DROPPED;

	if (RemotePort != -1 && Port != RemotePort)
		return ZE_SR_PACKET_DROPPED;

	return Result;
}

ZESocketUDP::ZESocketUDP()
{
	Socket = NULL;
	Status = ZE_SUS_CLOSED;
	BindIPAddress = ZEIPAddress::Any;
	BindPort = 0;
	RemoteIPAddress = ZEIPAddress::Any;
	RemotePort = -1;
}

ZESocketUDP::~ZESocketUDP()
{
	Close();
}
