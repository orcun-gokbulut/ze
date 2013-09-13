//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZENetworkObjectMessageManager.h
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
#ifndef	__ZE_NETWORK_OBJECT_MESSAGE_MANAGER_H__
#define __ZE_NETWORK_OBJECT_MESSAGE_MANAGER_H__

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEDelegate.h"
#include "ZEDS/ZEBuffer.h"

typedef ZEDelegate<void (ZEUInt32, const void*, ZESize)> ZENetworkObjectMessageHandler;

struct ZENetworkObjectMessage
{
	ZEUInt32			Id;
	ZESize				Size;
	void*				Data;
};

class ZENetworkObjectMessageManager
{
	private:
		ZEUInt32								SquenceNumber;
		ZEUInt32								LastSequenceNumber;
	
		ZEBuffer								ReceiveBuffer;
		ZEBuffer								SendBuffer;

		ZENetworkObjectMessageHandler			GenericHandler;
		ZEArray<ZENetworkObjectMessageHandler>	Handlers;

	
	public:
		void									SetGenericHandler(const ZENetworkObjectMessageHandler& GenericHandler);
		const ZENetworkObjectMessageHandler&	GetGenericHandler();

		void									RegisterHandler(ZEUInt32 MessageId, const ZENetworkObjectMessageHandler& Handler);
		void									UnregisterHandler(ZEUInt32 MessageId, const ZENetworkObjectMessageHandler& EventHandler);

		void									SendMessage(const ZENetworkObjectMessage* Message);
		
		void									DistributeMessages();
		void									ClearMessages();

		const ZENetworkObjectMessage*			GetReceivedMessage();
		void									RemoveReceivedMessage();
};

#endif