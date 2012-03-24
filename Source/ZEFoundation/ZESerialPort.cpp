//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESerialPort.cpp
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

#include "ZESerialPort.h"
#include "ZEError.h"

#include <windows.h>
#include <time.h>

static clock_t ClockMs()
{
	return GetTickCount();
}

static clock_t GetNextTimeOut(ZEUInt32 TimeOut)
{
	return ClockMs() + TimeOut;
}

void ZESerialPort::SetTimeOut(ZEUInt32 Milliseconds)
{
	TimeOut = Milliseconds;
}

ZEUInt32 ZESerialPort::GetTimeOut()
{
	return TimeOut;
}

bool ZESerialPort::IsOpen()
{
	return Handle != NULL;
}

bool ZESerialPort::Open(const ZEString& PortName, ZEUInt32 BaudRate)
{
	if (Handle != NULL)
		Close();

	Handle = CreateFile((const char*)PortName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (Handle == INVALID_HANDLE_VALUE)
	{
		//zeError("Can not create serial port.");
		Close();
		return false;
	}

	if(SetupComm(Handle, 1024, 1024) == 0)
	{
		//zeError("Can not configure serial port.");
		Close();
		return false;
	}

	DCB dcbConfig;
	if (GetCommState(Handle, &dcbConfig) == 0)
	{
		//zeError("Can not configure serial port.");
		Close();
		return false;
	}

	dcbConfig.BaudRate = BaudRate;
	dcbConfig.ByteSize = 8;
	dcbConfig.Parity = NOPARITY;
	dcbConfig.StopBits = ONESTOPBIT;
	dcbConfig.fBinary = TRUE;
	dcbConfig.fParity = TRUE;

	if(!SetCommState(Handle, &dcbConfig))
	{
		//zeError("Can not configure serial port.");
		Close();
		return false;
	}

	COMMTIMEOUTS CommTimeout;
	if (!GetCommTimeouts(Handle, &CommTimeout))
	{
		//zeError("Can not configure serial port.");
		Close();
		return false;
	}

	CommTimeout.ReadIntervalTimeout = MAXDWORD;
	CommTimeout.ReadTotalTimeoutConstant = 0;
	CommTimeout.ReadTotalTimeoutMultiplier = 0;
	CommTimeout.WriteTotalTimeoutConstant = 0;
	CommTimeout.WriteTotalTimeoutMultiplier = 0;

	if (!SetCommTimeouts(Handle, &CommTimeout))
	{
		//zeError("Can not configure serial port.");
		Close();
		return false;
	}

	PurgeComm (Handle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	return true;
}

void ZESerialPort::Close()
{
	if (Handle == NULL)
		return;

	PurgeComm(Handle, PURGE_TXCLEAR | PURGE_RXCLEAR);
	CloseHandle(Handle);
	Handle = NULL;
	BaudRate = 0;
	PortName = "";
}

bool ZESerialPort::ReadPackage(void* Packet, ZESize PackageSize)
{
	ZESize TotalBytesRead = 0;
	clock_t NextTimeOut = ClockMs() + TimeOut;

	while (PackageSize != TotalBytesRead)
	{
		ZESize BytesRead = 0;

		if (!ZESerialPort::Read(((char*)Packet + TotalBytesRead), PackageSize - TotalBytesRead, BytesRead))
			return false;
		
		if (BytesRead != 0)
		{
			NextTimeOut = ClockMs() + TimeOut;
			TotalBytesRead += BytesRead;
		}
		else
		{
			clock_t CurrentTime = ClockMs();
			if (NextTimeOut < CurrentTime)
				return false;
		}
	}

	return true;
}

bool ZESerialPort::WritePackage(const void* Packet, ZESize PackageSize)
{
	ZESize TotalBytesWritten = 0;
	clock_t NextTimeOut = ClockMs() + TimeOut;

	while (PackageSize != TotalBytesWritten)
	{
		ZESize BytesWritten = 0;

		if (!ZESerialPort::Write(((const char*)Packet + TotalBytesWritten), PackageSize - TotalBytesWritten, BytesWritten))
			return false;

		if (BytesWritten != 0)
		{
			NextTimeOut = ClockMs() + TimeOut;
			TotalBytesWritten += BytesWritten;
		}
		else
		{
			if (NextTimeOut < ClockMs())
				return false;
		}
	}

	return true;
}

void ZESerialPort::Clear()
{
	ZESize BytesRead = 0;
	do 
	{
        char Temp[100];
        if (!ZESerialPort::Read(&Temp, 100, BytesRead))
			break;
	}
	while (BytesRead != 0);
}


bool ZESerialPort::Read(void* Data, ZESize DataSize, ZESize &BytesRead)
{
	unsigned long Temp;
	if (!ReadFile(Handle, Data, (DWORD)DataSize, &Temp, NULL))
		return false;

	BytesRead = (ZESize)Temp;

	return true;
}

bool ZESerialPort::Write(const void* Data, ZESize DataSize, ZESize &BytesWritten)
{
	DWORD Temp;
	if (!WriteFile(Handle, Data, (DWORD)DataSize, &Temp, NULL))
		return false;

	BytesWritten = (ZESize)Temp;

	return true;
}

ZESerialPort::ZESerialPort()
{
	Handle = NULL;
	BaudRate = 0;
	TimeOut = 1000;
}

ZESerialPort::~ZESerialPort()
{
	Close();
}
