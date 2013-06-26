//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEConnection.cpp
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

#include "ZEConnection.h"
#include "ZESocket/ZESocket.h"

#define DEFAULT_BUFFER_SIZE 1024

ZEConnection::ZEConnection(ZESocket* Socket)
{
	this->Socket = Socket;

	TempBufferSize = DEFAULT_BUFFER_SIZE;
	SendBufferSize = DEFAULT_BUFFER_SIZE;
	ReadBufferSize = DEFAULT_BUFFER_SIZE;

	TempBuffer = new char[DEFAULT_BUFFER_SIZE];
	SendBuffer = new char[DEFAULT_BUFFER_SIZE];
	ReadBuffer = new char[DEFAULT_BUFFER_SIZE];

	memset(TempBuffer, 0, TempBufferSize);
	memset(SendBuffer, 0, SendBufferSize);
	memset(ReadBuffer, 0, ReadBufferSize);

	FilledSendBufferSize = 0;
	FilledReadBufferSize = 0;
}

ZEConnection::ZEConnection()
{
	Socket = NULL;

	TempBufferSize = DEFAULT_BUFFER_SIZE;
	SendBufferSize = DEFAULT_BUFFER_SIZE;
	ReadBufferSize = DEFAULT_BUFFER_SIZE;

	TempBuffer = new char[DEFAULT_BUFFER_SIZE];
	SendBuffer = new char[DEFAULT_BUFFER_SIZE];
	ReadBuffer = new char[DEFAULT_BUFFER_SIZE];

	memset(TempBuffer, 0, TempBufferSize);
	memset(SendBuffer, 0, SendBufferSize);
	memset(ReadBuffer, 0, ReadBufferSize);

	FilledSendBufferSize = 0;
	FilledReadBufferSize = 0;
}

ZEConnection::~ZEConnection()
{
	delete []TempBuffer;
	delete []SendBuffer;
	delete []ReadBuffer;

	TempBuffer = NULL;
	SendBuffer = NULL;
	ReadBuffer = NULL;

	if(Socket != NULL)
	{
		Socket->Close();
		delete Socket;
		Socket = NULL;
	}
}

void ZEConnection::SetSocket(ZESocket* Socket)
{
	if(Socket == NULL)
		return;

	this->Socket->Close();
	delete this->Socket;
	this->Socket = Socket;
}

const ZESocket* ZEConnection::GetSocket() const
{
	return Socket;
}

bool ZEConnection::SendData(const void* Data, ZESize Size)
{
	if(Size + FilledSendBufferSize >= SendBufferSize || Socket == NULL)
		return false;

	memcpy(SendBuffer + FilledSendBufferSize, Data, Size);
	FilledSendBufferSize += Size;
	return true;
}
void* ZEConnection::GetReadBuffer(ZESize& UsedSize)
{
	UsedSize = FilledReadBufferSize;
	return ReadBuffer;
}

void ZEConnection::ResizeBuffer(void* Buffer, ZESize OldSize, ZESize NewSize)
{
	if(Buffer == NULL)
		return;

	void* OldBuffer = Buffer;

	Buffer = new char[NewSize];

	if(OldSize <= NewSize)
		memcpy(Buffer, OldBuffer, OldSize);

	delete []OldBuffer;
	OldBuffer = NULL;
}

void ZEConnection::SetReadBufferSize(ZESize BufferSize)
{
	ResizeBuffer(ReadBuffer, ReadBufferSize, BufferSize);
	ReadBufferSize = BufferSize;
}

ZESize ZEConnection::GetReadBufferSize() const
{
	return ReadBufferSize;
}

void ZEConnection::SetSendBufferSize(ZESize BufferSize)
{
	ResizeBuffer(SendBuffer, SendBufferSize, BufferSize);
	SendBufferSize = BufferSize;
}

ZESize ZEConnection::GetSendBufferSize() const
{
	return SendBufferSize;
}

void ZEConnection::SetTemporaryBufferSize(ZESize BufferSize)
{
	ResizeBuffer(TempBuffer, TempBufferSize, BufferSize);
	TempBufferSize = BufferSize;
}

ZESize ZEConnection::GetTemporaryBufferSize() const
{
	return TempBufferSize;
}

void ZEConnection::CleanSendBuffer()
{
	memset(SendBuffer, 0, SendBufferSize);
	FilledSendBufferSize = 0;
}

void ZEConnection::CleanReadBuffer()
{
	memset(ReadBuffer, 0, ReadBufferSize);
	FilledReadBufferSize = 0;
}

bool ZEConnection::Process(float ElapsedTime)
{
	if(Socket == NULL)
		return true;

	if(FilledSendBufferSize > 0)
	{
		ZESSize SentDataSize = Socket->Send(SendBuffer, FilledSendBufferSize);

		if(SentDataSize > 0)
		{
			memmove(SendBuffer, SendBuffer + SentDataSize, SendBufferSize - SentDataSize);
			FilledSendBufferSize -= SentDataSize;
		}

	}

	ZESSize ReadDataSize = Socket->Recieve(TempBuffer, TempBufferSize);
	if (ReadDataSize == 0)
		return false;

	if(ReadDataSize > (ZESSize)(ReadBufferSize - FilledReadBufferSize))
	{
		zeWarning("Read data size is bigger than remaining read buffer space. Dropping Packet!!!");
		return true;
	}

	if(ReadDataSize > 0)
	{
		memcpy(ReadBuffer + FilledReadBufferSize, TempBuffer, ReadDataSize);
		FilledReadBufferSize += ReadDataSize;
	}

	return true;
}
