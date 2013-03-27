//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPacketManagerServer.cpp
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

#include "ZEPacketManagerServer.h"
#include "ZESerialPort/ZESerialPort.h"
#include "ZEDS/ZEString.h"
#include "ZEConnection.h"
#include "ZESocket/ZESocket.h"
#include "ZESocket/ZEIPAddress.h"

#include <memory.h>
#include <string.h>

ZEPacketManagerServer::ZEPacketManagerServer()
{
	Handlers.Clear();
}

ZEPacketManagerServer::~ZEPacketManagerServer()
{
	Handlers.Clear();
}

bool ZEPacketManagerServer::RegisterHander(ZEPacketHandler* Handler)
{
	if(Handler == NULL)
		return false;

	ZESize HandlerId = Handler->GetHandlerId();

	if(HandlerId < Handlers.GetCount())
		if(Handlers[HandlerId] != NULL)
			return false;

	if(HandlerId >= Handlers.GetCount())
		Handlers.Resize(HandlerId + 1);

	Handlers[HandlerId] = Handler;
	return true;
}

bool ZEPacketManagerServer::UnRegisterHandler(ZEPacketHandler* Handler)
{
	if(Handler == NULL)
		return false;

	ZESize HandlerId = Handler->GetHandlerId();

	if(HandlerId > Handlers.GetCount())
		return false;

	Handlers[HandlerId] = NULL;
	return true;
}

void ZEPacketManagerServer::Process(float ElapsedTime, ZEArray<ZEConnection*>& Connections)
{
	for (ZESize I = 0; I < Connections.GetCount(); I++)
	{
		ZEConnection* CurrentConnection = Connections[I];
		CurrentConnection->Process(ElapsedTime);


		ZESize UsedBufferSize = 0;
		char* ReadBuffer = (char*)CurrentConnection->GetReadBuffer(UsedBufferSize);

		if(UsedBufferSize < sizeof(ZEPacketHeader))
			continue;

		ZEPacketHeader Header;
		memcpy(&Header, ReadBuffer, sizeof(ZEPacketHeader));

		if(Header.Identifier != ZE_COMMAND_PACKET_HEADER_IDENTIFIER)
		{
			zeError("Header Mismatch !!!");
			CurrentConnection->CleanReadBuffer();
			continue;
		}

		ZESize HeaderAndDataSize = sizeof(ZEPacketHeader) + Header.DataSize;

		if(UsedBufferSize < HeaderAndDataSize)
			continue;

		//Invalid Packet Handler ID Check
		ZEPacketHandler* Handler = Handlers[Header.CommandId];

		if(Handler == NULL)
		{
			zeError("No command found with ID : %d", Header.CommandId);
			CurrentConnection->CleanReadBuffer();
			continue;
		}

		if(Handler->GetCallback().IsNull())
		{
			zeError("Command callback is NULL command ID : %d", Header.CommandId);
			CurrentConnection->CleanReadBuffer();
			continue;
		}

		Handler->GetCallback()(ReadBuffer + sizeof(ZEPacketHeader), Header.DataSize, CurrentConnection);
		memmove(ReadBuffer, ReadBuffer + HeaderAndDataSize, CurrentConnection->GetReadBufferSize() - HeaderAndDataSize);
		CurrentConnection->FilledReadBufferSize -= sizeof(ZEPacketHeader) + Header.DataSize;
	}
}
