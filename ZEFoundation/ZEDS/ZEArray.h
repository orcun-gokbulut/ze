//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEArray.h
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

// Program Files\Microsoft Visual Studio 8\Common7\Packages\Debugger\autoexp.dat
/*
;------------------------------------------------------------------------------
;  ZEArray<*> specialization
;------------------------------------------------------------------------------

ZEArray<*>{
	children
	(
		#
		(
			Members: [$c,!],
			Size: [$c.Count,i],
			#array
			(
				expr :	($c.Items)[$i],  
				size :	$c.Count
			)
		)
	)
}

*/

#pragma once
#ifndef	__ZEDS_ARRAY_H__
#define __ZEDS_ARRAY_H__

#include <stdlib.h>
#include "ZETypes.h"
#include "ZEAllocator.h"
#include "ZEError.h"

#ifdef ZE_DEBUG_CHECK_MEMORY
#include <crtdbg.h>
#endif

template<typename Type, typename Allocator_= ZEAllocatorBase<Type> >
class ZEArray
{
	private:
		size_t		Count;
		Allocator_	Allocator;
		Type*		Items;

	public:
		inline void Enqueue(Type Value)
		{
			Add(Value);
		}

		inline Type Dequeue()
		{
			zeAssert(Count == 0, "ZEArray::Dequeue operaion failed. There is no item in the queue.");
			Type Temp = Items[0];
			DeleteAt(0);
			return Temp;			
		}

		inline void Push(Type Value)
		{
			Add(Value);
		}

		inline Type Pop()
		{
			zeAssert(Count == 0, "ZEArray::Pop operaion failed. There is no item in the stack.");
			Type Temp = Items[0];
			DeleteAt(0);
			return Temp;
		}

		inline void SetItem(size_t Index, Type Value)
		{
			zeAssert(Index < 0 || Index >= Count, "ZEArray::SetItem operaion failed. Index is out of range. (0 <= Index < Count)");
			this->Items[Index] = Value;
		}

		void MassAdd(Type* OtherArray, size_t Count)
		{
			Resize(this->Count + Count);
			ZEAllocatorBase<Type>::ObjectCopy(this->Items + Count, OtherArray, Count);
		}

		void MassAdd(Type* OtherArray, size_t Offset, size_t Count)
		{
			Resize(this->Count + Count);
			ZEAllocatorBase<Type>::ObjectCopy(this->Items + Count, OtherArray + Offset, Count);
		}
		
		void MassInsert(size_t Index, Type* OtherArray, size_t Count)
		{
			Resize(this->Count + Count);
			ZEAllocatorBase<Type>::ObjectCopy(this->Items + Index, this->Items + Index + Count, Count);
			ZEAllocatorBase<Type>::ObjectCopy(this->Items + Index, OtherArray, Count);
		}

		void MassInsert(size_t Index, Type* OtherArray, size_t Offset, size_t Count)
		{
			Resize(this->Count + Count);
			ZEAllocatorBase<Type>::ObjectCopy(this->Items + Index, this->Items + Index + Count, Count);
			ZEAllocatorBase<Type>::ObjectCopy(this->Items + Index, OtherArray + Offset, Count);
		}

		inline void FillWith(Type Value)
		{
			for (size_t I =0 ; I < Count; I++)
				Items[I] = Value;
		}

		inline void Fill(Type* OtherArray, size_t Count)
		{
			if (this->Count != Count)
				this->SetCount(Count);
			ZEAllocatorBase<Type>::ObjectCopy(this->Items, OtherArray, Count);
		}

		inline int FindIndex(Type Item, int Index = 0)
		{
			for(size_t I = 0; I < Count; I++)
			{
				if (Items[I] == Item)
				{
					Index--;

					if (Index < 0)
						return I;
				}
			}
			return -1;	
		}

		void CopyTo(ZEArray<Type, Allocator_>& OtherArray) const
		{
			if (this->Count != OtherArray.Count)
				OtherArray.SetCount(Count);
			ZEAllocatorBase<Type>::ObjectCopy(OtherArray.Items, this->Items, Count);	
		}

		void CopyFrom(const ZEArray<Type, Allocator_>& OtherArray)
		{
			if (this->Count != OtherArray.Count)
				this->SetCount(OtherArray.Count);
			ZEAllocatorBase<Type>::ObjectCopy(this->Items, OtherArray.Items, Count);	
			#ifdef ZE_DEBUG_CHECK_MEMORY
					zeAssert(!_CrtCheckMemory(), "Heap problem");
			#endif
		}
		
		void Combine(const ZEArray<Type, Allocator_>& OtherArray)
		{
			Resize(Count + OtherArray.Count);
			MassInsert(Count, OtherArray.Items, OtherArray.Count);
		}

		inline Type* Add()
		{
			Count++;
			Allocator.Reallocate(&Items, Count); 

			#ifdef ZE_DEBUG_CHECK_MEMORY
				zeAssert(!_CrtCheckMemory(), "Heap problem");
			#endif
			
			return &Items[Count - 1];		
		}

		inline Type* MassAdd(size_t ItemCount)
		{
			Allocator.Reallocate(&Items, Count + ItemCount);
			Count += ItemCount;
			return &Items[Count - ItemCount];
		}

		inline Type* MassAdd(const Type* NewItems, size_t ItemCount)
		{
			Allocator.Reallocate(&Items, Count + ItemCount);
			ZEAllocatorBase<Type>::ObjectCopy(Items + Count, NewItems, ItemCount);
			Count += ItemCount;

			#ifdef ZE_DEBUG_CHECK_MEMORY
				zeAssert(!_CrtCheckMemory(), "Heap problem");
			#endif

			return &Items[Count - ItemCount];
		}

		inline Type* AddByRef(const Type& NewItem)
		{
			Count++;
			Allocator.Reallocate(&Items, Count);
			Items[Count - 1] = NewItem;

			#ifdef ZE_DEBUG_CHECK_MEMORY
				zeAssert(!_CrtCheckMemory(), "Heap problem");
			#endif

			return &Items[Count - 1];
		}

		inline Type* Add(Type NewItem)
		{
			Count++;
			Allocator.Reallocate(&Items, Count);
			Items[Count - 1] = NewItem;

			#ifdef ZE_DEBUG_CHECK_MEMORY
				zeAssert(!_CrtCheckMemory(), "Heap problem");
			#endif

			return &Items[Count - 1];
		}
		
		inline bool Exists(Type& Value)
		{
			for (size_t I = 0; I < Count; I++)
				if (Items[I] == Value)
					return true;

			return false;
		}

		inline Type* Insert(size_t Index)
		{
			zeAssert(Index < 0 || Index > Count, "ZEArray::Insert operation failed. Index is out of range. (0 <= Index <= Count)");
			size_t N = 0;
			Type* TempPointer = this->Items;
			bool Changed = Allocator.Allocate(&Items, Count + 1);

			for(size_t I = Index; I < Count; I++)
				Items[I + 1] = TempPointer[I];

			Count++;

			if (Changed)
			{
				for(size_t I = 0; I < Index; I++)
					Items[I] = TempPointer[I];
				delete[] TempPointer;
			}

			#ifdef ZE_DEBUG_CHECK_MEMORY
				zeAssert(!_CrtCheckMemory(), "Heap problem");
			#endif

			return &Items[Index];
		}

		inline Type* Insert(size_t Index, Type NewItem)
		{
			
			Insert(Index);
			Items[Index] = NewItem;

			#ifdef ZE_DEBUG_CHECK_MEMORY
				zeAssert(!_CrtCheckMemory(), "Heap problem");
			#endif

			return &Items[Index];
		}

		inline void DeleteAt(size_t Index)
		{
			zeAssert(Index < 0 || Index >= Count, "ZEArray::DeleteAt operation failed. Index is out of range. (0 <= Index < Count)");
			size_t NewCount = Count, N = 0;
			Type* TempPointer = this->Items;
			bool Changed;
			
			if (Count - 1 > 0)
				Changed = Allocator.Allocate(&Items, Count - 1);
			else
			{
				Allocator.Deallocate(&Items);
				Count = 0;
				return;
			}

			for(size_t I = Index; I < Count - 1; I++)
				Items[I] = TempPointer[I + 1];

			if (Changed && Index != 0)
			{
				for(size_t I = 0; I < Index; I++)
					Items[I] = TempPointer[I];
				delete[] TempPointer;
			}

			Count--;

			#ifdef ZE_DEBUG_CHECK_MEMORY
				zeAssert(!_CrtCheckMemory(), "Heap problem");
			#endif
		}

		inline void DeleteValue(Type Value)
		{
			size_t N = 0, OldCount = Count;

			for(Type* I = Items; I < Items + OldCount; I++)
				if (*I != Value)
				{
					Items[N++] = *I;
				}
				else
					Count--;

			Allocator.Reallocate(&Items, Count);

			#ifdef ZE_DEBUG_CHECK_MEMORY
				zeAssert(!_CrtCheckMemory(), "Heap problem");
			#endif
		}
		
		inline void SetCount(size_t Count)
		{
			zeAssert(Count < 0, "ZEArray::SetCount operation failed. Wrong Count value. (Count >= 0)");

			if (Count == this->Count)
				return;

			this->Count = Count;

			Type* OldItems = Items;
			if (Allocator.Allocate(&Items, Count))
				if (OldItems != NULL)
					delete[] OldItems;

			#ifdef ZE_DEBUG_CHECK_MEMORY
				zeAssert(!_CrtCheckMemory(), "Heap problem");
			#endif
		}

		inline void Resize(size_t Count)
		{
			zeAssert(Count < 0, "ZEArray::Resize operation failed. Wrong Count value. (Count >= 0)");
			
			if (Count == this->Count)
				return;

			if (Count == 0)
			{
				SetCount(0);
				return;
			}

			Allocator.Reallocate(&Items, Count);

			this->Count = Count;

			#ifdef ZE_DEBUG_CHECK_MEMORY
				zeAssert(!_CrtCheckMemory(), "Heap problem");
			#endif
		}
		
		inline Type* GetCArray()
		{
			return Items;
		}
		
		inline const Type* GetConstCArray() const
		{
			return Items;
		}

		inline size_t GetCount() const
		{
			return Count;
		}

		inline size_t GetSize() const
		{
			return Allocator.GetSize();
		}

		inline void Clear()
		{
			#ifdef ZE_DEBUG_CHECK_MEMORY
				zeAssert(!_CrtCheckMemory(), "Heap problem");
			#endif

			Allocator.Deallocate(&Items);
			Count = 0;
		}

		inline void Clear(bool PreserveAllocation)
		{
			if (PreserveAllocation)
				Allocator.Deallocate(&Items);
			
			Count = 0;
		}

		inline void Sort(int (*CompareFunction)(const void*, const void*))
		{
			qsort(Items, Count, sizeof(Type), CompareFunction);
		}

		inline void BinarySearch(const Type& Element, int (*CompareFunction)(Type*, Type*))
		{
			bsearch(&Element, &Element, Count, sizeof(Type), CompareFunction);
		}

		inline const Type& GetItem(size_t Index) const
		{
			zeAssert(Index < 0 || Index >= Count, "ZEArray::GetItem operation failed. Index is out of range. (0 <= Index < Count)");
			return Items[Index];
		}

		inline Type& GetItem(size_t Index)
		{
			zeAssert(Index < 0 || Index >= Count, "ZEArray::GetItem operation failed. Index is out of range. (0 <= Index < Count)");
			return Items[Index];
		}
		
		inline Type& GetFirstItem()
		{
			return Items[0];
		}

		inline Type& GetLastItem()
		{
			return Items[Count - 1];
		}

		inline Type& operator[](size_t Index)
		{
			zeAssert(Index < 0 || Index >= Count, "ZEArray::operator[] operation failed. Index is out of range. (0 <= Index < Count)");
			return Items[Index];
		}
		
		inline const Type& operator[](size_t Index) const
		{
			zeAssert(Index < 0 || Index >= Count, "ZEArray::operator[] operation failed. Index is out of range. (0 <= Index < Count)");
			return Items[Index];
		}

		inline void operator=(const ZEArray<Type, Allocator_>& Other)
		{
			CopyFrom(Other);
		}

		ZEArray()
		{
			Items = NULL;
			Count = 0;
		}

		ZEArray(const ZEArray<Type, Allocator_>& C) //Assingment Constructor
		{
			Items = NULL;
			Count = 0;
			CopyFrom(C);
		}

		~ZEArray()
		{
			Allocator.Deallocate(&Items);
		}
};

template <typename Type, int Exponent = 2>
class ZESmartArray : public ZEArray<Type, ZESmartAllocator<Type, Exponent> >
{};

template <typename Type, int ChunkSize>
class ZEChunkArray : public ZEArray<Type, ZEChunkAllocator<Type, ChunkSize> >
{};

#endif
