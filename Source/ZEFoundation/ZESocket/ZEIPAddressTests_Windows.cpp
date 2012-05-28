//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEIPAddressWinTests.cpp
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

#include "ZETest/ZETest.h"
#include "ZEIPAddress.h"
#include <Winsock2.h>
#include <Ws2ipdef.h>
#include <Ws2tcpip.h>
#include <Mstcpip.h>

static bool InitializeWinSock()
{
	int FResult;
	WSAData WSADat;

	FResult = WSAStartup(MAKEWORD(2,2), &WSADat);

	if(FResult != 0)
		zeError("Error Initializing Winsock!!!");
	else
		zeLog("Winsock Initialized.\n\n");

	return true;
}

static void DeinitializeWinSock()
{
	WSACleanup();
}

ZETestSuite(ZEIPAddress)
{
	ZETest("ZEIPAddress")
	{
		ZETestCase("Constructor IPv4")
		{
			ZEIPAddress TestIP = ZEIPAddress(127, 1, 2, 3);

			ZETestCheckEqual(TestIP.Address4[0], 127);
			ZETestCheckEqual(TestIP.Address4[1], 1);
			ZETestCheckEqual(TestIP.Address4[2], 2);
			ZETestCheckEqual(TestIP.Address4[3], 3);
			ZETestCheckEqual(TestIP.Type, ZE_IAT_IP_V4);
		}

		ZETestCase("Constructor IPv6")
		{
			ZEIPAddress TestIP = ZEIPAddress(0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF, 0x000A, 0x000B);

			ZETestCheckEqual(TestIP.Address6[0], 0xAAAA);
			ZETestCheckEqual(TestIP.Address6[1], 0xBBBB);
			ZETestCheckEqual(TestIP.Address6[2], 0xCCCC);
			ZETestCheckEqual(TestIP.Address6[3], 0xDDDD);
			ZETestCheckEqual(TestIP.Address6[4], 0xEEEE);
			ZETestCheckEqual(TestIP.Address6[5], 0xFFFF);
			ZETestCheckEqual(TestIP.Address6[6], 0x000A);
			ZETestCheckEqual(TestIP.Address6[7], 0x000B);
			ZETestCheckEqual(TestIP.Type, ZE_IAT_IP_V6);
		}

		ZETestCase("Parse IPv4")
		{
			ZEIPAddress TestIP;

			TestIP = ZEIPAddress::Parse(ZEString("127.1.2.3"));

			ZETestCheckEqual(TestIP.Address4[0], 127);
			ZETestCheckEqual(TestIP.Address4[1], 1);
			ZETestCheckEqual(TestIP.Address4[2], 2);
			ZETestCheckEqual(TestIP.Address4[3], 3);
			ZETestCheckEqual(TestIP.Type, ZE_IAT_IP_V4);
		}

		ZETestCase("Parse IPv6")
		{
			ZEIPAddress TestIP;

			TestIP = ZEIPAddress::Parse(ZEString("AAAA:BBBB:CCCC:DDDD:EEEE:FFFF:000A:000B"));

			ZETestCheckEqual(TestIP.Address6[0], 0xAAAA);
			ZETestCheckEqual(TestIP.Address6[1], 0xBBBB);
			ZETestCheckEqual(TestIP.Address6[2], 0xCCCC);
			ZETestCheckEqual(TestIP.Address6[3], 0xDDDD);
			ZETestCheckEqual(TestIP.Address6[4], 0xEEEE);
			ZETestCheckEqual(TestIP.Address6[5], 0xFFFF);
			ZETestCheckEqual(TestIP.Address6[6], 0x000A);
			ZETestCheckEqual(TestIP.Address6[7], 0x000B);
			ZETestCheckEqual(TestIP.Type, ZE_IAT_IP_V6);
		}

		ZETestCase("== Operator")
		{
			ZEIPAddress TestIP, TestIP2;

			TestIP = ZEIPAddress(127, 1, 2, 3);
			TestIP2 = ZEIPAddress(127, 1, 2, 3);
			ZETestCheck(TestIP == TestIP2);

			TestIP2 = ZEIPAddress(127, 1, 2, 4);
			ZETestCheck(!(TestIP == TestIP2));

			TestIP = ZEIPAddress(0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF, 0x000A, 0x000B);
			TestIP2 = ZEIPAddress(0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF, 0x000A, 0x000B);
			ZETestCheck(TestIP == TestIP2);
			
			TestIP2 = ZEIPAddress(0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF, 0x000A, 0xAAAA);
			ZETestCheck(!(TestIP == TestIP2));
		}

		ZETestCase("!= Operator")
		{
			ZEIPAddress TestIP, TestIP2;

			TestIP = ZEIPAddress(127, 1, 2, 3);
			TestIP2 = ZEIPAddress(127, 1, 2, 4);
			ZETestCheck(TestIP != TestIP2);

			TestIP2 = ZEIPAddress(127, 1, 2, 3);
			ZETestCheck(!(TestIP != TestIP2));

			TestIP = ZEIPAddress(0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF, 0x000A, 0x000B);
			TestIP2 = ZEIPAddress(0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF, 0x000A, 0xAAAA);
			ZETestCheck(TestIP != TestIP2);

			TestIP2 = ZEIPAddress(0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF, 0x000A, 0x000B);
			ZETestCheck(!(TestIP != TestIP2));
		}
	}
}
