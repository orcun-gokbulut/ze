//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEIPAddress.h
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
#ifndef	__ZE_IP_ADDRESS_H__
#define __ZE_IP_ADDRESS_H__

#include "ZETypes.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"

class ZEIPAddress
{
	public:
		static const ZEIPAddress		Any;
		static const ZEIPAddress		Broadcast;

		ZEUInt8							Address[4];
		
		bool							IsAny() const;
		bool							IsBroadcast() const;

		bool							operator == (const ZEIPAddress &RightOperand) const;
		bool							operator != (const ZEIPAddress &RightOperand) const;

										ZEIPAddress();
										ZEIPAddress(ZEUInt8 Byte0, ZEUInt8 Byte1, ZEUInt8 Byte2, ZEUInt8 Byte3);

		static ZEIPAddress				Parse(const ZEString& String);
		static ZEArray<ZEIPAddress>		Lookup(const ZEString& String);
		static ZEArray<ZEIPAddress>		HostIPs();

		static void						ToSockaddr_in(void* Buffer, const ZEIPAddress& IPAddress);
		static void						FromSockaddr_in(ZEIPAddress& IPAddress, const void* Buffer);
};

#endif
