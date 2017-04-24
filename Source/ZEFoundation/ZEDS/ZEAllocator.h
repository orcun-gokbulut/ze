//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAllocator.h
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

#ifndef NULL
#define NULL 0
#endif

#include "ZETypes.h"
#include <math.h>


template <typename ZEItemType>
class ZEAllocatorBase
{
	public:
		ZESize							Size;

	public:	
		inline static void				ObjectCopy(ZEItemType* Destination, const ZEItemType* Source, ZESize Count);

		inline ZESize					GetSize() const;
		inline bool						Allocate(ZEItemType** Pointer, ZESize NewSize);
		inline void						Reallocate(ZEItemType** Pointer, ZESize NewSize);
		inline void						Deallocate(ZEItemType** Pointer);
		
										ZEAllocatorBase();
};


template <typename ZEItemType, ZEInt ChunkSize>
class ZEChunkAllocator : public ZEAllocatorBase<ZEItemType>
{
	public:
		inline bool						Allocate(ZEItemType** Pointer, ZESize NewSize);
		inline void						Reallocate(ZEItemType** Pointer, ZESize NewSize);
};


template <typename ZEItemType, ZESize Exponent = 2>
class ZESmartAllocator : public ZEAllocatorBase<ZEItemType>
{
	private:
		ZESize LowerLimit;

	public:
		inline bool						Allocate(ZEItemType** Pointer, ZESize NewSize);
		inline void						Reallocate(ZEItemType** Pointer, ZESize NewSize);
};


// ZEAllocatorBase
//////////////////////////////////////////////////////////////////////////////////////

template <typename ZEItemType>
void ZEAllocatorBase<ZEItemType>::ObjectCopy(ZEItemType* Destination, const ZEItemType* Source, ZESize Count)
{
	const ZEItemType* SourceEnd = Source + Count;
	while(Source != SourceEnd)
	{
		*Destination = *Source;
		Destination++;
		Source++;
	} 
}

template <typename ZEItemType>
ZESize ZEAllocatorBase<ZEItemType>::GetSize() const
{
	return Size;
}

template <typename ZEItemType>
void ZEAllocatorBase<ZEItemType>::Deallocate(ZEItemType** Pointer)
{
	if (*Pointer == NULL)
		return;

	Size = 0;
	delete[] *Pointer;
	*Pointer = NULL;
}

template <typename ZEItemType>
bool ZEAllocatorBase<ZEItemType>::Allocate(ZEItemType** Pointer, ZESize NewSize)
{
	if (Size == NewSize)
		return false;

	if (NewSize == 0)
	{
		Deallocate(Pointer);
		return false;
	}

	Size = NewSize;
	*Pointer = new ZEItemType[NewSize];

	return true;
}

template <typename ZEItemType>
void ZEAllocatorBase<ZEItemType>::Reallocate(ZEItemType** Pointer, ZESize NewSize)
{
	if (Size == NewSize)
		return;

	if (NewSize == 0)
	{
		Deallocate(Pointer);
		return;
	}

	ZESize OldSize = Size;
	ZEItemType* NewPointer = new ZEItemType[NewSize];
	if (*Pointer != NULL)
	{
		ObjectCopy(NewPointer, *Pointer, (OldSize > NewSize ? NewSize : OldSize));
		delete[] *Pointer;
	}

	Size = NewSize;
	*Pointer = NewPointer;
}

template <typename ZEItemType>
ZEAllocatorBase<ZEItemType>::ZEAllocatorBase()
{
	Size = 0;
}


// ZEChunkAllocator
//////////////////////////////////////////////////////////////////////////////////////

template <typename ZEItemType, ZEInt ChunkSize>
bool ZEChunkAllocator<ZEItemType, ChunkSize>::Allocate(ZEItemType** Pointer, ZESize NewSize)
{
	if (NewSize != 0)
	{
		if ((this->Size < NewSize) || (this->Size - NewSize >= ChunkSize))
		{
			this->Size = ((NewSize + ChunkSize - 1) / ChunkSize) * ChunkSize;

			*Pointer = new ZEItemType[this->Size];
			return true;
		}
		else
			return false;
	}
	else
	{
		this->Size = 0;
		if (*Pointer != NULL)
		{
			delete[] *Pointer;
			*Pointer = NULL;
		}
		return false;
	}
}

template <typename ZEItemType, ZEInt ChunkSize>
void ZEChunkAllocator<ZEItemType, ChunkSize>::Reallocate(ZEItemType** Pointer, ZESize NewSize)
{
	ZEItemType* OldPointer = *Pointer;
	ZESize OldSize = this->Size;
	if (Allocate(Pointer, NewSize))
	{	
		if (OldPointer != NULL)		
		{
			ObjectCopy(*Pointer, OldPointer, (OldSize > NewSize ? NewSize : OldSize));
			delete[] OldPointer;
		}
	}
}


// ZESmartAllocator
//////////////////////////////////////////////////////////////////////////////////////

template <typename ZEItemType, ZESize Exponent>
bool ZESmartAllocator<ZEItemType, Exponent>::Allocate(ZEItemType** Pointer, ZESize NewSize)
{
	if (NewSize != 0)
	{
		if ((NewSize > this->Size) || (NewSize < LowerLimit))
		{
			this->Size = (ZESize)(powf((float)Exponent, (logf((float)NewSize) / logf((float)Exponent)) + 1.0f) + 0.5f);
			LowerLimit = this->Size / Exponent;
			*Pointer = new ZEItemType[this->Size];
			return true;
		}
		else
			return false;
	}
	else
	{
		this->Size = 0;
		if (*Pointer != NULL)
		{
			delete[] *Pointer;
			*Pointer = NULL;
		}
		return false;
	}
}

template <typename ZEItemType, ZESize Exponent>
void ZESmartAllocator<ZEItemType, Exponent>::Reallocate(ZEItemType** Pointer, ZESize NewSize)
{
	ZESize OldSize = this->Size;
	ZEItemType* OldPointer = *Pointer;
	if (Allocate(Pointer, NewSize))
	{	
		if (OldPointer != NULL)		
		{
			ZEAllocatorBase<ZEItemType>::ObjectCopy(*Pointer, OldPointer, (OldSize > NewSize ? NewSize : OldSize));
			delete[] OldPointer;
		}
	}
}
