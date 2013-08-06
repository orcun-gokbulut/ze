//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEBuffer.cpp
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

#include "ZEBuffer.h"

void ZEBuffer::SetSize(ZESize Size)
{
	Buffer.SetCount(Size);
}

ZESize ZEBuffer::GetSize()
{
	return Buffer.GetCount();
}

ZESize ZEBuffer::GetBlockCount()
{
	return BlockCount;
}

ZESize ZEBuffer::GetUsedSpace()
{
	return UsedSpace;
}

ZESize ZEBuffer::GetFreeSpace()
{
	ZESSize FreeSpace = (ZESSize)Buffer.GetSize() - (ZESSize)UsedSpace - sizeof(ZESize);
	return (FreeSpace > 0 ? FreeSpace : 0);
}
		
bool ZEBuffer::AddBlock(const void* Data, ZESize Size)
{
	ZESize BlockSize = Size + sizeof(ZESize);

	// Basic Tests
	if (BlockSize + UsedSpace > Buffer.GetSize())
		return false;

	// Normal
	if (BufferEnd + BlockSize <= Buffer.GetSize())
	{
		*(ZESize*)(Buffer.GetCArray() + BufferEnd) = Size;
		memcpy(Buffer.GetCArray() + BufferEnd + sizeof(ZESize), Data, Size);
		BufferEnd = Buffer.Circular(BufferEnd + BlockSize);
	}
	else
	{
		if (BufferEnd + sizeof(ZESize) <= Buffer.GetCount())
		{
			*(ZESize*)(Buffer.GetCArray() + BufferEnd) = Size;
			BufferEnd += sizeof(ZESize);
		}
		else
		{
			for (ZESize I = 0; I < sizeof(ZESize); I++)
				Buffer[Buffer.Circular(BufferEnd + I)] = ((ZEUInt8*)&Size)[I];
			BufferEnd = Buffer.Circular(BufferEnd + sizeof(ZESize));
		}

		ZESize FirstPartSize = Buffer.GetCount() - BufferEnd;
		memcpy(Buffer.GetCArray() + BufferEnd, Data, FirstPartSize);
		memcpy(Buffer.GetCArray(), (ZEUInt8*)Data + FirstPartSize, Size - FirstPartSize);
		BufferEnd = Size - FirstPartSize;
	}

	UsedSpace += BlockSize;
	BlockCount++;
}

bool ZEBuffer::DeleteBlock()
{
	if (BlockCount == 0)
		return false;
	
	ZESize BlockSize = GetBlockSize() + sizeof(ZESize);

	BufferStart = Buffer.Circular(BufferStart + BlockSize);

	UsedSpace -= BlockSize;
	BlockCount--;

	return true;
}

ZEBufferBlockPointer ZEBuffer::GetFirstBlock()
{
	if (BlockCount == 0)
		return ZE_BP_NONE;

	if (BufferStart == BufferEnd)
		return ZE_BP_NONE;

	return BufferStart;
}

ZEBufferBlockPointer ZEBuffer::MoveNextBlock(ZEBufferBlockPointer PrevPointer)
{
	if (BlockCount == 0)
		return ZE_BP_NONE;

	if (PrevPointer == BufferEnd)
		return ZE_BP_NONE;
	
	ZESize Size = GetBlockSize(PrevPointer);
	if (Size == 0)
		return ZE_BP_NONE;

	ZEBufferBlockPointer NewPointer = Buffer.Circular(PrevPointer + Size + sizeof(ZESize));

	if (PrevPointer == BufferEnd)
		return false;

	return NewPointer;
}

bool ZEBuffer::GetBlockData(void* Data)
{
	return GetBlockData(Data, GetBlockSize());
}

bool ZEBuffer::GetBlockData(void* Data, ZESize Size)
{
	return GetBlockData(Data, Size);
}

ZESize ZEBuffer::GetBlockSize()
{
	return GetBlockSize(BufferStart);
}

bool ZEBuffer::GetBlockData(ZEBufferBlockPointer Pointer, void* Data)
{
	return GetBlockData(Data, GetBlockSize(Pointer));
}

bool ZEBuffer::GetBlockData(ZEBufferBlockPointer Pointer, void* Data, ZESize Size)
{
	if (BlockCount)
		return false;

	ZESize BlockSize = Size + sizeof(ZESize);
	if (Pointer + BlockSize <= Buffer.GetCount())
	{
		memcpy(Data, Buffer.GetCArray() + sizeof(ZESize) + Pointer, Size);
	}
	else
	{
		ZESize FirstPartSize = Buffer.GetCount() - Pointer;
		memcpy(Data, Buffer.GetCArray() + Pointer, FirstPartSize);
		memcpy((ZEUInt8*)Data + FirstPartSize, Buffer.GetCArray(), Size - FirstPartSize);
	}

	return true;
}

ZESize ZEBuffer::GetBlockSize(ZEBufferBlockPointer Pointer)
{
	if (BlockCount == 0)
		return 0;
	
	if (Pointer + sizeof(ZESize) > Buffer.GetCount())
	{
		ZESize BlockSize;
		for (ZESize I = 0; I < sizeof(ZESize); I++)
			((ZEUInt8*)&BlockSize)[I] = Buffer[Buffer.Circular(Pointer + I)];
		return BlockSize;
	}
	else
		return *(ZESize*)&Buffer[Pointer];
}

void ZEBuffer::Reset()
{
	BufferStart = 0;
	BufferEnd = 0;
	BlockCount = 0;
	UsedSpace = 0;
}

ZEBuffer::ZEBuffer()
{
	Reset();
}
