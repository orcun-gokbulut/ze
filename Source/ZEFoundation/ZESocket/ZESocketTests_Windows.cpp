//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESocketTests_Windows.cpp
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
#include "ZESocket.h"
#include <concrt.h>
#include <winsock.h>

#define DefaultPort		27015
#define DefaultPort2	27016
#define DefaultPort3	27017

#define LocalHostIP ZEIPAddress(127, 0, 0, 1)

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

ZETestSuite(ZESocketUDP)
{
	ZETest("UDP Socket")
	{
		InitializeWinSock();

		ZETestCase("Create")
		{
			ZESocketUDP* UDPSocket = new ZESocketUDP();
			bool Result = UDPSocket->Create(LocalHostIP, DefaultPort);
			UDPSocket->Close();

			ZETestCheck(Result);
		}

		ZETestCase("Close")
		{
			ZESocketUDP* UDPSocket = new ZESocketUDP();
			bool Result = UDPSocket->Create(LocalHostIP, DefaultPort);

			ZETestCheck(UDPSocket->Close());
		}

		ZETestCase("SendTo")
		{
			ZESocketUDP* UDPSocketSender = new ZESocketUDP();
			UDPSocketSender->Create(LocalHostIP, DefaultPort);

			ZESocketUDP* UDPSocketReciever = new ZESocketUDP();
			UDPSocketReciever->Create(LocalHostIP, DefaultPort2);

			const char* Buffer = "Test_Buffer_Test";
			ZESize BufferSize = 16;

			ZESSize SentDataSize = UDPSocketSender->SendTo(&UDPSocketReciever->GetIpAddress(), UDPSocketReciever->GetPort(), Buffer, BufferSize);

			UDPSocketSender->Close();
			UDPSocketReciever->Close();

			ZETestCheckEqual(16, SentDataSize);
		}

		ZETestCase("RecieveFrom")
		{
			ZESocketUDP* UDPSocketSender = new ZESocketUDP();
			UDPSocketSender->Create(LocalHostIP, DefaultPort);

			ZESocketUDP* UDPSocketReciever = new ZESocketUDP();
			UDPSocketReciever->Create(LocalHostIP, DefaultPort2);

			const char* Buffer = "Test_Buffer_Test";
			ZESize BufferSize = 16;
			char* ReceiveBuffer = new char[BufferSize];

			ZESSize SentDataSize = UDPSocketSender->SendTo(&UDPSocketReciever->GetIpAddress(), UDPSocketReciever->GetPort(), Buffer, BufferSize);
			ZESSize ReceivedDataSize = UDPSocketReciever->RecieveFrom(ReceiveBuffer, BufferSize, &UDPSocketSender->GetIpAddress(), UDPSocketSender->GetPort());

			UDPSocketSender->Close();
			UDPSocketReciever->Close();

			ZETestCheckEqual(16, ReceivedDataSize);
		}

		ZETestCase("GetIpAddress")
		{
			ZESocketUDP* UDPSocket = new ZESocketUDP();
			bool Result = UDPSocket->Create(LocalHostIP, DefaultPort);
			UDPSocket->Close();

			ZETestCheck(LocalHostIP == UDPSocket->GetIpAddress());
		}

		ZETestCase("GetPort")
		{
			ZESocketUDP* UDPSocket = new ZESocketUDP();
			bool Result = UDPSocket->Create(LocalHostIP, DefaultPort);
			UDPSocket->Close();

			ZETestCheck(DefaultPort == UDPSocket->GetPort());
		}

		DeinitializeWinSock();
	}
}

ZETestSuite(ZESocketTCPListener)
{
	ZETest("TCPListener Socket")
	{
		InitializeWinSock();

		ZETestCase("Create")
		{
			ZESocketTCPListener* Listener = new ZESocketTCPListener();
			bool Result = Listener->Create(LocalHostIP, DefaultPort);
			Listener->Close();

			ZETestCheck(Result);
		}

		ZETestCase("Close")
		{
			ZESocketTCPListener* Listener = new ZESocketTCPListener();
			bool Result = Listener->Create(LocalHostIP, DefaultPort);
			
			ZETestCheck(Listener->Close());
		}

		ZETestCase("GetIpAddress")
		{
			ZESocketTCPListener* Listener = new ZESocketTCPListener();
			bool Result = Listener->Create(LocalHostIP, DefaultPort);
			Listener->Close();

			ZETestCheck(LocalHostIP == Listener->GetIpAddress());
		}

		ZETestCase("GetPort")
		{
			ZESocketTCPListener* Listener = new ZESocketTCPListener();
			bool Result = Listener->Create(LocalHostIP, DefaultPort);
			Listener->Close();

			ZETestCheck(DefaultPort == Listener->GetPort());
		}

		ZETestCase("Accept")
		{
			ZESocketTCPListener* Listener = new ZESocketTCPListener();
			Listener->Create(LocalHostIP, DefaultPort);
		
			ZESocketTCP* TCPSocket = new ZESocketTCP();
			TCPSocket->Create(LocalHostIP, 0);

			ZESSize Result = TCPSocket->Connect(LocalHostIP, DefaultPort);

			ZESocketTCP* AcceptSocket = NULL;

			AcceptSocket = Listener->Accept();

			Listener->Close();
			TCPSocket->Close();
			AcceptSocket->Close();

			ZETestCheck((AcceptSocket != NULL && AcceptSocket->GetIpAddress() == LocalHostIP && AcceptSocket->GetPort() == DefaultPort));
		}

		DeinitializeWinSock();
	}
}

ZETestSuite(ZESocketTCP)
{
	ZETest("TCP Socket")
	{
		InitializeWinSock();

		ZETestCase("Create")
		{
			ZESocketTCP* TCPSocket = new ZESocketTCP();
			bool Result = TCPSocket->Create(LocalHostIP, DefaultPort);
			TCPSocket->Close();

			ZETestCheck(Result);
		}

		ZETestCase("Close")
		{
			ZESocketTCP* TCPSocket = new ZESocketTCP();
			TCPSocket->Create(LocalHostIP, DefaultPort);

			ZETestCheck(TCPSocket->Close());
 		}

// 		ZETestCase("Receive")
// 		{
// 			ZESocketTCP* TCPSocket = new ZESocketTCP();
// 			TCPSocket->Create(ZEIPAddress(192,168,1,12), DefaultPort2);
// 			TCPSocket->Connect(ZEIPAddress(192,168,1,12), DefaultPort);
// 
// 			Concurrency::wait(2000);
// 
// 			ZESize BufferSize = 32;
// 			char* ReceiveBuffer = new char[BufferSize];
// 			ZESSize  ReceivedDataSize = ZE_SR_RETRY;
// 
// 			while(ReceivedDataSize == ZE_SR_RETRY)
// 				ReceivedDataSize = TCPSocket->Recieve(ReceiveBuffer, BufferSize);
// 
// 			TCPSocket->Close();
// 
// 			ZETestCheckEqual(ReceivedDataSize, BufferSize);
// 		}

// 		ZETestCase("Send")
// 		{
// 			ZESocketTCP* TCPSocket = new ZESocketTCP();
// 			TCPSocket->Create(ZEIPAddress(192,168,1,12), DefaultPort2);
// 			TCPSocket->Connect(ZEIPAddress(192,168,1,12), DefaultPort);
// 
// 			Concurrency::wait(2000);
// 
// 			ZESize BufferSize = 32;
// 			const char* SendBuffer = "Test_Buffer_TestTest_Buffer_Test";
// 			ZESSize  SentDataSize = 0;
// 
// 			while(SentDataSize != BufferSize)
// 				SentDataSize = TCPSocket->Send(SendBuffer, BufferSize);
// 
// 			TCPSocket->Close();
//  		}

		ZETestCase("GetIpAddress")
		{
			ZESocketTCP* TCPSocket = new ZESocketTCP();
			bool Result = TCPSocket->Create(LocalHostIP, DefaultPort);
			TCPSocket->Close();

			ZETestCheckEqual(TCPSocket->GetIpAddress(), LocalHostIP);
		}

		ZETestCase("GetPort")
		{
			ZESocketTCP* TCPSocket = new ZESocketTCP();
			bool Result = TCPSocket->Create(LocalHostIP, DefaultPort);
			TCPSocket->Close();

			ZETestCheckEqual(TCPSocket->GetPort(), DefaultPort);
		}

		DeinitializeWinSock();
	}
}
