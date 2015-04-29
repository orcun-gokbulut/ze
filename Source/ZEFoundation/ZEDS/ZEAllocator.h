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
#ifndef __ZE_ALLOCATOR_H__
#define __ZE_ALLOCATOR_H__

#ifndef NULL
#define NULL 0
#endif

#include "ZETypes.h"
#include <math.h>

template <typename Type>
class ZEAllocatorBase
{
	public:
		ZESize				Size;

	public:	
		inline static void ObjectCopy(Type* Destination, const Type* Source, ZESize Count)
		{
			if (Count == 0)
				return;

			Count--;
			do
			{
				Destination[Count] = Source[Count];
			} while (Count--);
		}

		inline ZESize GetSize() const
		{
			return Size;
		}

		inline void Deallocate(Type** Pointer)
		{
			if (*Pointer != NULL)
			{
				Size = 0;
				delete[] *Pointer;
				*Pointer = NULL;
			}
		}
	
		inline bool Allocate(Type** Pointer, ZESize NewSize)
		{
			if (NewSize != 0)
			{
				if (Size != NewSize)
				{
					Size = NewSize;
					*Pointer = new Type[NewSize];
					return true;
				}
				else
					return false;
			}
			else
			{
				Size = 0;
				if (*Pointer != NULL)
				{
					delete[] *Pointer;
					*Pointer = NULL;
				}
				return false;
			}

		}

		inline void Reallocate(Type** Pointer, ZESize NewSize)
		{
			if (NewSize != 0)
			{
				if (Size != NewSize)
				{
					ZESize OldSize = Size;
					Type* NewPointer = new Type[NewSize];
					if (*Pointer != NULL)
					{
						ObjectCopy(NewPointer, *Pointer, (OldSize > NewSize ? NewSize : OldSize));
						delete[] *Pointer;
					}
					Size = NewSize;
					*Pointer = NewPointer;
				}
			}
			else
				Deallocate(Pointer);
		}
		
		ZEAllocatorBase()
		{
			Size = 0;
		}
};

template <typename Type, ZEInt ChunkSize>
class ZEChunkAllocator : public ZEAllocatorBase<Type>
{
	public:
		inline bool Allocate(Type** Pointer, ZESize NewSize)
		{
			ZESize OldSize;
			if (NewSize != 0)
			{
				if ((this->Size < NewSize) || (this->Size - NewSize > ChunkSize))
				{
					OldSize = this->Size;
					if (NewSize < ChunkSize)
						this->Size = ChunkSize;
					else
						this->Size = ((NewSize / ChunkSize) + 1) * ChunkSize;

					*Pointer = new Type[this->Size];
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

		inline void Reallocate(Type** Pointer, ZESize NewSize)
		{
			Type* OldPointer = *Pointer;
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
};

template <typename Type, ZESize Exponent = 2>
class ZESmartAllocator : public ZEAllocatorBase<Type>
{
	private:
		ZESize LowerLimit;

	public:
		inline bool Allocate(Type** Pointer, ZESize NewSize)
		{
			if (NewSize != 0)
			{
				if ((NewSize > this->Size) || (NewSize < LowerLimit))
				{
					this->Size = (ZESize)(powf((float)Exponent, (logf((float)NewSize) / logf((float)Exponent)) + 1.0f) + 0.5f);
					LowerLimit = this->Size / Exponent;
					*Pointer = new Type[this->Size];
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

		inline void Reallocate(Type** Pointer, ZESize NewSize)
		{
			ZESize OldSize = this->Size;
			Type* OldPointer = *Pointer;
			if (Allocate(Pointer, NewSize))
			{	
				if (OldPointer != NULL)		
				{
                    ZEAllocatorBase<Type>::ObjectCopy(*Pointer, OldPointer, (OldSize > NewSize ? NewSize : OldSize));
					delete[] OldPointer;
				}
			}
		}
};
#endif
