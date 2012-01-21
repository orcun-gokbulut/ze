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

#include "ZETypes.h"
#include "ZEAllocator.h"
#include "ZEError.h"

#include <stdlib.h>

typedef ZEInt FunctionPointerCaster(const void*, const void*);

template<typename Type, typename Allocator_= ZEAllocatorBase<Type> >
class ZEArray
{
	private:
		ZESize		Count;
		Allocator_	Allocator;
		Type*		Items;

	public:
		inline ZESize Circular(ZESSize Index) const
		{
			if (Index < 0)
			{
				ZESSize CircularIndex = ((-Index) % Count);
				return (CircularIndex == 0 ? 0 : Count - CircularIndex);
			}
			else
				return Index % Count;
		}

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
			Type Temp = Items[Count - 1];
			DeleteAt(Count - 1);
			return Temp;
		}

		inline void SetItem(ZESize Index, Type Value)
		{
			zeAssert(Index < 0 || Index >= Count, "ZEArray::SetItem operaion failed. Index is out of range. (0 <= Index < Count)");
			this->Items[Index] = Value;
		}

		inline Type* MassAdd(ZESize ItemCount)
		{
			Resize(Count + ItemCount);

			return &Items[Count - ItemCount];
		}

		inline Type* MassAdd(const Type* NewItems, ZESize ItemCount)
		{
			Resize(Count + ItemCount);
			ZEAllocatorBase<Type>::ObjectCopy(this->Items + Count - ItemCount, NewItems, ItemCount);

			ZEDebugCheckMemory();

			return &Items[Count - ItemCount];
		}
		
		inline Type* MassInsert(ZESize Index, ZESize ItemCount)
		{
			Type* NewBuffer;
			if (Allocator.Allocate(&NewBuffer, Count + ItemCount))
			{
				ZEAllocatorBase<Type>::ObjectCopy(NewBuffer, this->Items, Index);
				ZEAllocatorBase<Type>::ObjectCopy(NewBuffer + Index + ItemCount, this->Items + Index, Count - Index);
				delete[] Items;
				Items = NewBuffer;
			}
			else
				ZEAllocatorBase<Type>::ObjectCopy(this->Items + Index + ItemCount, this->Items + Index, Count - Index);

			Count += ItemCount;

			ZEDebugCheckMemory();

			return &Items[Index];
		}

		inline Type* MassInsert(ZESize Index, Type* NewItems, ZESize ItemCount)
		{
			Type* NewBuffer;
			if (Allocator.Allocate(&NewBuffer, Count + ItemCount))
			{
				ZEAllocatorBase<Type>::ObjectCopy(NewBuffer, this->Items, Index);
				ZEAllocatorBase<Type>::ObjectCopy(NewBuffer + Index + ItemCount, this->Items + Index, Count - Index);
				delete[] Items;
				Items = NewBuffer;
			}
			else
				ZEAllocatorBase<Type>::ObjectCopy(this->Items + Index + ItemCount, this->Items + Index, Count - Index);

			ZEAllocatorBase<Type>::ObjectCopy(this->Items + Index, NewItems, ItemCount);

			Count += ItemCount;

			ZEDebugCheckMemory();

			return &Items[Index];
		}

		inline void Fill(Type Value)
		{
			for (ZESize I = 0; I < Count; I++)
				Items[I] = Value;
		}

		inline ZESSize FindIndex(Type Item, ZESize StartIndex = 0) const
		{
			for(ZESize I = StartIndex; I < Count; I++)
				if (Items[I] == Item)
					return I;

			return -1;	
		}

		void CopyTo(ZEArray<Type, Allocator_>& OtherArray) const
		{
			OtherArray.SetCount(Count);
			ZEAllocatorBase<Type>::ObjectCopy(OtherArray.Items, this->Items, Count);
			ZEDebugCheckMemory();
		}

		void CopyTo(Type* OtherArray, ZESize Count) const
		{
			ZEAllocatorBase<Type>::ObjectCopy(OtherArray, this->Items, Count > this->Count ? this->Count : Count);
			ZEDebugCheckMemory();
		}

		void CopyFrom(const Type* OtherArray, ZESize Count)
		{
			this->SetCount(Count);
			ZEAllocatorBase<Type>::ObjectCopy(this->Items, OtherArray, Count);	

			ZEDebugCheckMemory();
		}

		void CopyFrom(const ZEArray<Type, Allocator_>& OtherArray)
		{
			CopyFrom(OtherArray.GetConstCArray(), OtherArray.Count);
		}
		
		void Combine(const ZEArray<Type, Allocator_>& OtherArray)
		{
			MassAdd(OtherArray.Items, OtherArray.Count);
		}

		inline Type* Add()
		{
			Count++;
			Allocator.Reallocate(&Items, Count); 

			ZEDebugCheckMemory();
			
			return &Items[Count - 1];		
		}

		inline Type* AddByRef(const Type& NewItem)
		{
			Count++;
			Allocator.Reallocate(&Items, Count);
			Items[Count - 1] = NewItem;

			ZEDebugCheckMemory();

			return &Items[Count - 1];
		}

		inline Type* Add(Type NewItem)
		{
			Count++;
			Allocator.Reallocate(&Items, Count);
			Items[Count - 1] = NewItem;

			ZEDebugCheckMemory();

			return &Items[Count - 1];
		}
		
		inline bool Exists(Type& Value)
		{
			for (ZESize I = 0; I < Count; I++)
				if (Items[I] == Value)
					return true;

			return false;
		}

		inline Type* Insert(ZESize Index)
		{
			zeAssert(Index < 0 || Index > Count, "ZEArray::Insert operation failed. Index is out of range. (0 <= Index <= Count)");
			ZESize N = 0;
			Type* TempPointer = this->Items;
			bool Changed = Allocator.Allocate(&Items, Count + 1);

			for(ZESize I = Index; I < Count; I++)
				Items[I + 1] = TempPointer[I];

			Count++;

			if (Changed)
			{
				for(ZESize I = 0; I < Index; I++)
					Items[I] = TempPointer[I];
				delete[] TempPointer;
			}

			ZEDebugCheckMemory();

			return &Items[Index];
		}

		inline Type* Insert(ZESize Index, Type NewItem)
		{
			
			Insert(Index);
			Items[Index] = NewItem;

			ZEDebugCheckMemory();

			return &Items[Index];
		}

		inline void DeleteAt(ZESize Index)
		{
			zeAssert(Index < 0 || Index >= Count, "ZEArray::DeleteAt operation failed. Index is out of range. (0 <= Index < Count)");
			ZESize NewCount = Count, N = 0;
			Type* TempPointer = this->Items;
			bool Changed;
			
			if (Count > 1)
				Changed = Allocator.Allocate(&Items, Count - 1);
			else
			{
				Allocator.Deallocate(&Items);
				Count = 0;
				return;
			}

			for(ZESize I = Index; I < Count - 1; I++)
				Items[I] = TempPointer[I + 1];

			if (Changed && Index != 0)
			{
				for(ZESize I = 0; I < Index; I++)
					Items[I] = TempPointer[I];
				delete[] TempPointer;
			}

			Count--;

			ZEDebugCheckMemory();
		}

		inline void DeleteValue(Type Value)
		{
			ZESize N = 0, OldCount = Count;

			for(Type* I = Items; I < Items + OldCount; I++)
				if (*I != Value)
				{
					Items[N++] = *I;
				}
				else
					Count--;

			Allocator.Reallocate(&Items, Count);

			ZEDebugCheckMemory();
		}
		
		inline void SetCount(ZESize Count)
		{
			zeAssert(Count < 0, "ZEArray::SetCount operation failed. Wrong Count value. (Count >= 0)");

			if (Count == this->Count)
				return;

			this->Count = Count;

			Type* OldItems = Items;
			if (Allocator.Allocate(&Items, Count))
				if (OldItems != NULL)
					delete[] OldItems;

			ZEDebugCheckMemory();
		}

		inline void Resize(ZESize Count)
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

			ZEDebugCheckMemory();
		}
		
		inline Type* GetCArray()
		{
			return Items;
		}
		
		inline const Type* GetConstCArray() const
		{
			return Items;
		}

		inline ZESize GetCount() const
		{
			return Count;
		}

		inline ZESize GetSize() const
		{
			return Allocator.GetSize();
		}

		inline void Clear()
		{
			Allocator.Deallocate(&Items);
			Count = 0;
		}

		inline void Clear(bool PreserveAllocation)
		{
			if (PreserveAllocation)
				Allocator.Deallocate(&Items);
			
			Count = 0;
		}

		inline void Sort(ZEInt (*CompareFunction)(const Type*, const Type*))
		{
			qsort(Items, Count, sizeof(Type), (FunctionPointerCaster*)(CompareFunction));
		}

		void Traverse()
		{
			for (ZESize I = 0; I < Count / 2; I++)
			{
				Type Temp = Items[I];
				Items[I] = Items[Count - I - 1];
				Items[Count - I - 1] = Temp;
			}
		}

		inline ZESSize BinarySearch(const Type& Element, ZEInt (*CompareFunction)(const Type*, const Type*))
		{
			void* Result = bsearch(&Element, &Items, Count, sizeof(Type), (FunctionPointerCaster*)(CompareFunction));
			if (Result == NULL)
				return -1;
			else
				return ((Type*)Result - Items);
		}

		inline const Type& GetItem(ZESize Index) const
		{
			zeAssert(Index >= Count, "ZEArray::GetItem operation failed. Index is out of range. (0 <= Index < Count)");
			return Items[Index];
		}

		inline Type& GetItem(ZESize Index)
		{
			zeAssert(Index >= Count, "ZEArray::GetItem operation failed. Index is out of range. (0 <= Index < Count)");
			return Items[Index];
		}

		inline const Type& GetFirstItem() const
		{
			return Items[0];
		}

		inline Type& GetFirstItem()
		{
			return Items[0];
		}

		inline Type& GetLastItem()
		{
			return Items[Count - 1];
		}

		inline const Type& GetLastItem() const
		{
			return Items[Count - 1];
		}

		inline Type& operator[](ZESize Index)
		{
			zeAssert(Index < 0 || Index >= Count, "ZEArray::operator[] operation failed. Index is out of range. (0 <= Index < Count)");
			return Items[Index];
		}
		
		inline const Type& operator[](ZESize Index) const
		{
			zeAssert(Index < 0 || Index >= Count, "ZEArray::operator[] operation failed. Index is out of range. (0 <= Index < Count)");
			return Items[Index];
		}

		ZEArray operator+(const ZEArray<Type, Allocator_>& OtherArray)
		{
			ZEArray Temp;
			Temp.MassAdd(Items, Count);
			Temp.MassAdd(OtherArray.Items, OtherArray.Count);
			return Temp;
		}

		ZEArray& operator+=(const ZEArray<Type, Allocator_>& OtherArray)
		{
			this->MassAdd(OtherArray.Items, OtherArray.Count);
			return *this;
		}

		bool operator==(const ZEArray<Type, Allocator_>& Other)
		{
			if (Count != Other.Count)
				return false;

			for (ZESize I = 0; I < Count; I++)
				if (Items[I] != Other.Items[I])
					return false;

			return true;
		}


		bool operator!=(const ZEArray<Type, Allocator_>& Other)
		{
			if (Count != Other.Count)
				return true;

			for (ZESize I = 0; I < Count; I++)
				if (Items[I] != Other.Items[I])
					return true;

			return false;
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

		ZEArray(const ZEArray<Type, Allocator_>& C)
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

template <typename Type, ZEInt Exponent = 2>
class ZESmartArray : public ZEArray<Type, ZESmartAllocator<Type, Exponent> >
{};

template <typename Type, ZEInt ChunkSize>
class ZEChunkArray : public ZEArray<Type, ZEChunkAllocator<Type, ChunkSize> >
{};

#endif