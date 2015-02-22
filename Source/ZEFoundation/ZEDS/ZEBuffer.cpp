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

#include <memory.h>
#include "ZEMath\ZEMath.h"

ZESSize ZEBuffer::RawBlockSize(ZEBufferBlockPointer Pointer) const
{
	return *(ZESSize*)&Buffer[Pointer];
}

bool ZEBuffer::IsFragmented()
{
	return Fragmented;
}

void ZEBuffer::SetSize(ZESize Size)
{
	Buffer.SetCount(Size);
}

ZESize ZEBuffer::GetSize() const
{
	return Buffer.GetCount();
}

ZESize ZEBuffer::GetBlockCount() const
{
	return BlockCount;
}

ZESize ZEBuffer::GetUsedSpace() const
{
	return UsedSpace;
}

ZESize ZEBuffer::GetFreeSpace() const
{
	if (BufferStart > BufferEnd)
		return ZEMath::ClampLower((ZESSize)BufferEnd - (ZESSize)BufferStart - (ZESSize)sizeof(ZESSize), (ZESSize)0);
	else
		return ZEMath::ClampLower((ZESSize)ZEMath::Max(BufferStart, (Buffer.GetSize() - BufferEnd))  - (ZESSize)sizeof(ZESSize),  (ZESSize)0);
}

ZEBufferBlockPointer ZEBuffer::AddBlock(ZESize Size)
{
	ZESize BlockSize = Size + sizeof(ZESSize);
	ZESize FreeSpaceStart;
	if (BufferStart > BufferEnd)
	{
		if (BufferEnd - BufferStart < BlockSize)
			return ZE_BP_NONE;
		FreeSpaceStart = BufferEnd;
	}
	else
	{
		if (BlockSize > Buffer.GetCount() - BufferEnd)
		{
			if (BlockSize > BufferStart)
				return ZE_BP_NONE;
			FreeSpaceStart = BufferStart;
		}
		else
		{
			FreeSpaceStart = BufferEnd;
			BufferRemainingEnd = BufferEnd + BlockSize;
		}
	}

	*(ZESSize*)&Buffer[FreeSpaceStart] = Size;
	UsedSpace += BlockSize;
	BlockCount++;
	BufferEnd = FreeSpaceStart + BlockSize;
	return FreeSpaceStart;
}

ZEBufferBlockPointer ZEBuffer::AddBlock(const void* Data, ZESize Size)
{
	ZEBufferBlockPointer BlockPointer = AddBlock(Size);
	if (BlockPointer == ZE_BP_NONE)
		return ZE_BP_NONE;

	memcpy((ZEBYTE*)&Buffer[BlockPointer] + sizeof(ZESSize), Data, Size);
	return BlockPointer;
}		


bool ZEBuffer::DeleteBlock(ZEBufferBlockPointer Pointer)
{
	if (BlockCount == 0)
		return false;

	ZESSize BlockSize = *(ZESSize*)&Buffer[BufferStart];
	if (BlockSize <= 0)
		return false;

	if (Pointer == BufferStart)
	{
		BufferStart += BlockSize;
		if (BufferStart == BufferRemainingEnd)
			BufferStart = 0;
	}
	else if (Pointer + *(ZESSize*)&Buffer[Pointer] == BufferEnd)
	{
		BufferEnd -= BlockSize;
		if (BufferEnd == 0)
			BufferEnd = BufferRemainingEnd;
	}
	else
	{
		*(ZESSize*)&Buffer[Pointer] = -BlockSize;
		Fragmented = true;
	}

	UsedSpace -= BlockSize;
	BlockCount--;

	return true;
}

ZEBufferBlockPointer ZEBuffer::GetFirstBlock() const
{
	if (BlockCount == 0)
		return ZE_BP_NONE;

	return BufferStart;
}

ZEBufferBlockPointer ZEBuffer::MoveNextBlock(ZEBufferBlockPointer PrevPointer) const
{
	if (PrevPointer == ZE_BP_NONE)
		return ZE_BP_NONE;

	if (BlockCount == 0)
		return ZE_BP_NONE;

	if (PrevPointer == BufferEnd)
		return ZE_BP_NONE;
	
	PrevPointer += RawBlockSize(PrevPointer) + sizeof(ZESSize);
	if (PrevPointer == BufferEnd)
		return ZE_BP_NONE;

	while(RawBlockSize(PrevPointer) < 0)
	{
		if (PrevPointer == BufferEnd)
			return ZE_BP_NONE;

		PrevPointer += -RawBlockSize(PrevPointer) + sizeof(ZESSize);
		if (PrevPointer == BufferEnd)
			return ZE_BP_NONE;
		else if (PrevPointer == BufferRemainingEnd)
			PrevPointer = 0;
	}

	return PrevPointer;
}

void* ZEBuffer::GetBlockData(ZEBufferBlockPointer Pointer)
{
	return &Buffer[Pointer + sizeof(ZESSize)];
}

ZESSize ZEBuffer::GetBlockSize(ZEBufferBlockPointer Pointer) const
{
	ZESSize Size = *(ZESSize*)&Buffer[Pointer];
	return Size;
}

void ZEBuffer::Clear()
{
	BufferStart = 0;
	BufferEnd = 0;
	BlockCount = 0;
	BufferRemainingEnd = 0;
	UsedSpace = 0;
	Fragmented = false;
}

void ZEBuffer::Defrag()
{
	if (!Fragmented)
		return;

	ZESSize Destination = -1;
	if (BufferStart < BufferEnd)
	{
		if (BufferStart > 0)
			Destination = 0;
	}

	ZEBufferBlockPointer Current = BufferStart;
	while(Current != BufferEnd)
	{
		ZESSize BlockSize = RawBlockSize(Current);
		if (BlockSize < 0)
		{
			if (Destination == -1)
				Destination = Current;

			Current += -BlockSize;
			if (Current == Buffer.GetCount() || Current == BufferRemainingEnd)
				Current = 0;
	
			continue;
		}

		BlockSize += sizeof(ZESSize);
		if (Destination != -1)
		{
			memmove(Buffer.GetCArray() + Destination, Buffer.GetCArray() + Current, BlockSize);
			Destination += BlockSize + sizeof(ZESSize);
			if (Destination > Buffer.GetCount())
			{
				if (Current == Destination)
					Destination = -1;
				else
					Destination = 0;
			}
		}

		Current += -BlockSize + sizeof(ZESSize);
		if (Current == Buffer.GetCount() || Current == BufferRemainingEnd)
			Current = 0;
	}

	Fragmented = false;
}

ZEBuffer::ZEBuffer()
{
	Clear();
}
