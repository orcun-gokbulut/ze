//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZENetworkServer.h
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
#ifndef	__ZE_NETWORK_SERVER_H__
#define __ZE_NETWORK_SERVER_H__

#include "ZETypes.h"
#include "ZEDS\ZEArray.h"
#include "ZESocket\ZEIPAddress.h"
#include "ZESocket\ZESocketUDP.h"

enum ZENetworkServerStatus
{
	ZE_NSS_NOT_RUNNING,
	ZE_NSS_RUNNING
};

class ZENetworkConnection;
class ZENetworkObject;

class ZENetworkServer
{
	private:
		ZEArray<ZENetworkConnection*>			Connections;
		ZEArray<ZENetworkObject*>				Objects;

		ZEUInt									ServerId;
		ZEUInt									NextClientId;

		ZESocketUDP								Socket;
		ZENetworkServerStatus					Status;

		bool									Enabled;

		void									ProcessNewConnections();
		void									ProcessConnections();
		void									ProcessObjects();

	public:
		const ZEArray<ZENetworkConnection*>&	GetConnections();

		void									RegisterObject(ZENetworkObject* Object);
		void									UnregisterObject(ZENetworkObject* Object);

		void									SetIPAddress(const ZEIPAddress& Address);
		const ZEIPAddress&						GetIPAddress();

		void									SetPort(ZEUInt16 Port);
		ZEUInt16								GetPort();

		ZENetworkServerStatus					GetStatus();

		void									Start();
		void									Stop();

		void									Disconnect(ZEUInt ClientId);

		void									Process();

		ZENetworkServer();

		static ZENetworkServer*					GetInstance();
};


#endif
