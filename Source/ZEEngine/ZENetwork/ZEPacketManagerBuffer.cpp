//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPacketManagerBuffer.cpp
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

#include "ZEPacketManagerBuffer.h"

#include <memory.h>
#include "ZEError.h"

ZEPacketManagerBuffer::ZEPacketManagerBuffer()
{
	BufferStart = NULL;
	BufferEnd = NULL;
	BufferSize = 0;
	Head = NULL;
	Tail = NULL;
}

ZEPacketManagerBuffer::~ZEPacketManagerBuffer()
{
	if(BufferStart != NULL)
		delete []BufferStart;

	Head = NULL;
	Tail = NULL;
	BufferEnd = NULL;
	BufferSize = 0;
}

void ZEPacketManagerBuffer::Create(ZESize BufferSize)
{
	if(BufferStart != NULL)
	{
		delete []BufferStart;
		Head = NULL;
		Tail = NULL;
		BufferEnd = NULL;
		BufferSize = 0;
	}

	BufferStart = new char[BufferSize];
	Head = BufferStart;
	Tail = BufferStart;
	BufferEnd = BufferStart + BufferSize;
	BufferSize = 0;
}

void ZEPacketManagerBuffer::Release()
{
	if(BufferStart != NULL)
		delete []BufferStart;

	Head = NULL;
	Tail = NULL;
	BufferSize = 0;
}

bool ZEPacketManagerBuffer::AddData(const void* Data, ZESize Size)
{
	if(Size + DataSize > BufferSize)
		return false;

	DataSize += Size;

	if(Head == BufferStart)
	{
		memcpy(Head, Data, Size);
		Tail = Head + DataSize;
		return true;
	}
	else if(Head + Size < BufferEnd)
	{
		memcpy(Tail + sizeof(char), Data, Size);
		Tail = Head + DataSize;
		return true;
	}
	else
	{
		ZESize FirstPartDataSize = BufferEnd - Tail;
		ZESize SecondPartDataSize = Size - FirstPartDataSize;
		memcpy(Tail + sizeof(char), Data, FirstPartDataSize);
		memcpy(BufferStart, (char*)Data + FirstPartDataSize, SecondPartDataSize);
		Tail = BufferStart + SecondPartDataSize;
		return true;
	}
}

ZESize ZEPacketManagerBuffer::PopData(ZESize Size, void* ToBuffer)
{
	if(Size > DataSize)
	{
		zeError("Pop size exceeds filled buffer size.");
		return 0;
	}
	else if(Head + Size <= BufferEnd)
	{
		memcpy(ToBuffer, Head, Size);
		Head = Head + Size;
		return Size;
	}	
	else
	{
		ZESize FirstPartSize = BufferEnd - Head;
		memcpy(ToBuffer, Head, FirstPartSize);
		ZESize SecondPartSize = Size - FirstPartSize;
		memcpy((unsigned char*)ToBuffer + FirstPartSize, BufferStart, SecondPartSize);
		Head = BufferStart + SecondPartSize;
		return Size;
	}

	if(Tail == Head)
	{
		DataSize = 0;
		Head = BufferStart;
		Tail = BufferStart;
		return (ZESize)0;
	}
}

void ZEPacketManagerBuffer::Clear()
{
	DataSize = 0;
	Head = BufferStart;
	Tail = BufferStart;
}
