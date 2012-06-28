//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESerialPort_Unix.cpp
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

#include "SerialPort.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

bool SerialPort::IsOpen()
{
	return Port != -1;
}

bool SerialPort::Open()
{
	SerialPort::Close();

	int BaudRateValue;
	switch(BaudRate)
	{
		case 50: 
			BaudRateValue = B50; 
			break;

		case 75:	
			BaudRateValue = B75; 
			break;

		case 110:		
			BaudRateValue = B110; 
			break;

		case 134:		
			BaudRateValue = B134; 
			break;

		case 150:		
			BaudRateValue = B150; 
			break;

		case 200:		
			BaudRateValue = B200; 
			break;

		case 300:		
			BaudRateValue = B300; 
			break;

		case 600:		
			BaudRateValue = B600; 
			break;

		case 1200:	
			BaudRateValue = B1200; 
			break;

		case 1800:	
			BaudRateValue = B1800; 
			break;

		case 2400:	
			BaudRateValue = B2400; 
			break;

		case 4800:	
			BaudRateValue = B4800; 
			break;

		case 9600:	
			BaudRateValue = B9600; 
			break;

		case 19200:	
			BaudRateValue = B19200; 
			break;

		case 38400:	
			BaudRateValue = B38400; 
			break;

		case 57600:	
			BaudRateValue = B57600; 
			break;

		case 115200:	
			BaudRateValue = B115200; 
			break;

		default:
			return false;
	}

	Port = open(PortName, O_RDWR | O_NOCTTY | O_NDELAY);
	if (Port == -1)
		return false;

	fcntl(Port, F_SETFL, FNDELAY);

	termios Options;
	tcgetattr(Port, &Options);

	// Baud Rate
	cfsetispeed(&Options, BaudRateValue);
	cfsetospeed(&Options, BaudRateValue);

	Options.c_cflag |= (CLOCAL | CREAD);
	Options.c_cflag &= ~PARENB;                 // Mask the character size to 8 bits, no parity
	Options.c_cflag &= ~CSTOPB;                 // One stop bit
	Options.c_cflag &= ~CSIZE;
	Options.c_cflag |=  CS8;                    // Select 8 data bits

	Options.c_cflag &= ~CRTSCTS;                // Disable hardware flow control
	Options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Enable data to be processed as raw input
	Options.c_oflag &= ~OPOST;
	Options.c_iflag &= ~(INPCK | ISTRIP);
	Options.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control

	if (tcsetattr(Port, TCSAFLUSH, &Options) < 0)
		return false;

	return true;
}

bool SerialPort::Read(void* Data, size_t DataSize, size_t &BytesRead)
{
	ssize_t Result = read(Port, Data, DataSize);

	if (Result < 0)
	{
		BytesRead = 0;
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return true;

		char Output[255];
		sprintf(Output, "Error : %d. Text : %s", errno, strerror(errno));
		return false;
	}
	else
	{
		BytesRead = Result;
		return true;
	}
}

bool SerialPort::Write(const void* Data, size_t DataSize, size_t &BytesWritten)
{
	ssize_t Result = write(Port, Data, DataSize);

	if (Result < 0)
	{
		BytesWritten = 0;
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return true;

		char Output[255];
		sprintf(Output, "Error : %d. Text : %s", errno, strerror(errno));
		return false;
	}
	else
	{
		BytesWritten = Result;
		return true;
	}
}

void SerialPort::Close()
{
	if (SerialPort::IsOpen())
	{
		close(Port);
		Port = -1;
	}

}

ZESerialPort::ZESerialPort()
{
	Port = -1;
	BaudRate = 0;
	TimeOut = 1000;
}

ZESerialPort::~ZESerialPort()
{
	Close();
}
