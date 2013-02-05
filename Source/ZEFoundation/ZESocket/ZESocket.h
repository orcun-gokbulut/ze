//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESocket.h
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
#ifndef	__ZE_SOCKET_H__
#define __ZE_SOCKET_H__

#include "ZETypes.h"
#include "ZEDS/ZEString.h"
#include "ZEIPAddress.h"

#ifdef ZE_PLATFORM_WINDOWS
#include <Winsock2.h>
#endif

#ifdef ZE_PLATFORM_UNIX
#include <sys/socket.h>
#define SOCKET int
#endif

#define ZE_SR_OK			((ZESSize)0)
#define ZE_SR_ERROR			((ZESSize)-1)
#define ZE_SR_RETRY			((ZESSize)-2)
#define ZE_SR_IN_PROGRESS	((ZESSize)-3)

class ZESocket
{
	public:
		virtual ZESSize	Connect(const ZEIPAddress& Address, ZEUInt16 Port) = 0;
		virtual ZESSize	Recieve(void* Buffer, ZESize BufferSize) = 0;
};

class ZESocketTCP : public ZESocket
{
	friend class ZESocketTCPListener;

	private:
		SOCKET				Socket;
		ZEIPAddress			IpAddress;
		ZEUInt16			Port;

	public:
		bool				Create(const ZEIPAddress& Address, ZEUInt16 Port);
		bool				Close();

		ZESSize				Connect(const ZEIPAddress& Address, ZEUInt16 Port);
		virtual ZESSize		Send(const void* Buffer, ZESize BufferSize);
		virtual ZESSize		Recieve(void* Buffer, ZESize BufferSize);

		const ZEIPAddress&	GetIpAddress() const;
		ZEUInt16			GetPort() const;		
};

class ZESocketTCPListener
{
	private:
		SOCKET				Socket;
		ZEIPAddress			IpAddress;
		ZEUInt16			Port;

	public:
		const ZEIPAddress&	GetIpAddress() const;
		ZEUInt16			GetPort() const;

		bool				Create(const ZEIPAddress& Address, ZEUInt16 Port);
		bool				Close();

		ZESocketTCP*		Accept();
};

class ZESocketUDP : public ZESocket
{
	private:
		SOCKET				Socket;
		ZEIPAddress			IpAddress;
		ZEUInt16			Port;

		ZEIPAddress			ToIpAddress;
		ZEUInt16			ToPort;

		ZEIPAddress			FromIpAddress;
		ZEUInt16			FromPort;

	public:
		bool				Create(const ZEIPAddress& Address, ZEUInt16 Port);
		bool				Close();

		const ZEIPAddress&	GetIpAddress() const;
		ZEUInt16			GetPort() const;

		void				SetToIpAddress(ZEIPAddress Address);
		const ZEIPAddress&	GetToIpAddress();

		void				SetToPort(ZEUInt16 Port);
		ZEUInt16			GetToPort();

		void				SetFromIpAddress(ZEIPAddress Address);
		const ZEIPAddress&	GetFromIpAddress();

		void				SetFromPort(ZEUInt16 Port);
		ZEUInt16			GetFromPort();

		virtual ZESSize		Send(const void* Buffer, ZESize BufferSize);
		virtual ZESSize		Recieve(void* Buffer, ZESize BufferSize);
};

#endif
