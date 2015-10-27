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

#pragma once
#ifndef	__ZEDS_ARRAY_H__
#define __ZEDS_ARRAY_H__

#include "ZETypes.h"
#include "ZEAllocator.h"
#include "ZEError.h"
#include "ZEArrayIterator.h"

#include <stdlib.h>

template<typename ZEType, typename Allocator_= ZEAllocatorBase<ZEType> >
class ZEArray
{
	private:
		ZESize		Count;
		Allocator_	Allocator;
		ZEType*		Items;

	public:
		typedef ZEArrayIterator<ZEType, Allocator_> Iterator;
		typedef ZEArrayIteratorConst<ZEType, Allocator_> IteratorConst;

		Iterator GetIterator()
		{
			return Iterator(*this, 0);
		}

		IteratorConst GetIterator() const
		{
			return IteratorConst(*this, 0);
		}

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

		inline void Enqueue(ZEType Value)
		{
			Add(Value);
		}

		inline ZEType Dequeue()
		{
			zeDebugCheck(Count == 0, "There is no item in the queue.");
			ZEType Temp = Items[0];
			Remove(0);
			return Temp;			
		}

		inline void Push(ZEType Value)
		{
			Add(Value);
		}

		inline ZEType Pop()
		{
			zeDebugCheck(Count == 0, "There is no item in the stack.");
			ZEType Temp = Items[Count - 1];
			Remove(Count - 1);
			return Temp;
		}

		inline void SetItem(ZESize Index, ZEType Value)
		{
			zeDebugCheck(Index >= Count, "Index is out of range.");
			this->Items[Index] = Value;
		}

		inline ZEType* MassAdd(ZESize ItemCount)
		{
			Resize(Count + ItemCount);

			return &Items[Count - ItemCount];
		}

		inline ZEType* MassAdd(const ZEType* NewItems, ZESize ItemCount)
		{
			Resize(Count + ItemCount);
			ZEAllocatorBase<ZEType>::ObjectCopy(this->Items + Count - ItemCount, NewItems, ItemCount);

			ZEDebugCheckMemory();

			return &Items[Count - ItemCount];
		}
		
		inline ZEType* MassInsert(ZESize Index, ZESize ItemCount)
		{
			ZEType* NewBuffer;
			if (Allocator.Allocate(&NewBuffer, Count + ItemCount))
			{
				ZEAllocatorBase<ZEType>::ObjectCopy(NewBuffer, this->Items, Index);
				ZEAllocatorBase<ZEType>::ObjectCopy(NewBuffer + Index + ItemCount, this->Items + Index, Count - Index);
				delete[] Items;
				Items = NewBuffer;
			}
			else
				ZEAllocatorBase<ZEType>::ObjectCopy(this->Items + Index + ItemCount, this->Items + Index, Count - Index);

			Count += ItemCount;

			ZEDebugCheckMemory();

			return &Items[Index];
		}

		inline ZEType* MassInsert(ZESize Index, ZEType* NewItems, ZESize ItemCount)
		{
			ZEType* NewBuffer;
			if (Allocator.Allocate(&NewBuffer, Count + ItemCount))
			{
				ZEAllocatorBase<ZEType>::ObjectCopy(NewBuffer, this->Items, Index);
				ZEAllocatorBase<ZEType>::ObjectCopy(NewBuffer + Index + ItemCount, this->Items + Index, Count - Index);
				delete[] Items;
				Items = NewBuffer;
			}
			else
				ZEAllocatorBase<ZEType>::ObjectCopy(this->Items + Index + ItemCount, this->Items + Index, Count - Index);

			ZEAllocatorBase<ZEType>::ObjectCopy(this->Items + Index, NewItems, ItemCount);

			Count += ItemCount;

			ZEDebugCheckMemory();

			return &Items[Index];
		}

		inline void Fill(ZEType Value)
		{
			for (ZESize I = 0; I < Count; I++)
				Items[I] = Value;
		}

		inline ZESSize FindIndex(ZEType Item, ZESize StartIndex = 0) const
		{
			for(ZESize I = StartIndex; I < Count; I++)
				if (Items[I] == Item)
					return I;

			return -1;	
		}

		void CopyTo(ZEArray<ZEType, Allocator_>& OtherArray) const
		{
			OtherArray.SetCount(Count);
			ZEAllocatorBase<ZEType>::ObjectCopy(OtherArray.Items, this->Items, Count);
			ZEDebugCheckMemory();
		}

		void CopyTo(ZEType* OtherArray, ZESize Count) const
		{
			ZEAllocatorBase<ZEType>::ObjectCopy(OtherArray, this->Items, Count > this->Count ? this->Count : Count);
			ZEDebugCheckMemory();
		}

		void CopyFrom(const ZEType* OtherArray, ZESize Count)
		{
			this->SetCount(Count);
			ZEAllocatorBase<ZEType>::ObjectCopy(this->Items, OtherArray, Count);	

			ZEDebugCheckMemory();
		}

		void CopyFrom(const ZEArray<ZEType, Allocator_>& OtherArray)
		{
			CopyFrom(OtherArray.GetConstCArray(), OtherArray.Count);
		}
		
		void Combine(const ZEArray<ZEType, Allocator_>& OtherArray)
		{
			MassAdd(OtherArray.Items, OtherArray.Count);
		}

		inline ZEType* Add()
		{
			Count++;
			Allocator.Reallocate(&Items, Count); 

			ZEDebugCheckMemory();
			
			return &Items[Count - 1];		
		}

		inline ZEType* AddByRef(const ZEType& NewItem)
		{
			Count++;
			Allocator.Reallocate(&Items, Count);
			Items[Count - 1] = NewItem;

			ZEDebugCheckMemory();

			return &Items[Count - 1];
		}

		inline ZEType* Add(ZEType NewItem)
		{
			Count++;
			Allocator.Reallocate(&Items, Count);
			Items[Count - 1] = NewItem;

			ZEDebugCheckMemory();

			return &Items[Count - 1];
		}

		inline ZEType* AddOrderedInc(ZEType NewItem)
		{
			if (Count == 0)
				return Add(NewItem);
	
			ZESize Index = 0;
			while(Index < Count && Items[Index] < NewItem)
				Index++;

			return Insert(Index + 1, NewItem);
		}

		template<ZEInt CompareFunction(const ZEType*, const ZEType*)>
		inline ZEType* AddOrderedInc(ZEType NewItem)
		{
			if (Count == 0)
				return Add(NewItem);

			ZESize Index = 0;
			while(Index < Count && CompareFunction(&Items[Index], &NewItem) == -1)
				Index++;

			return Insert(Index + 1, NewItem);
		}

		inline ZEType* AddOrderedDec(ZEType Type)
		{
			if (Count == 0)
			return Add(NewItem);

			ZESize Index = 0;
			while(Index < Count && Items[Index] > NewItem)
				Index++;

			return Insert(Index + 1, NewItem);
		}

		template<ZEInt CompareFunction(const ZEType*, const ZEType*)>
		inline ZEType* AddOrderedDec(ZEType NewItem)
		{
			if (Count == 0)
				return Add(NewItem);

			ZESize Index = 0;
			while(Index < Count && CompareFunction(&Items[Index], &NewItem) == 1)
				Index++;

			return Insert(Index + 1, NewItem);
		}

		inline bool Exists(ZEType Value) const
		{
			for (ZESize I = 0; I < Count; I++)
				if (Items[I] == Value)
					return true;

			return false;
		}

		inline ZEType* Insert(ZESize Index)
		{
			zeDebugCheck(Index > Count, "Index is out of range.");
			ZEType* TempPointer = this->Items;
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

		inline ZEType* Insert(ZESize Index, ZEType NewItem)
		{
			
			Insert(Index);
			Items[Index] = NewItem;

			ZEDebugCheckMemory();

			return &Items[Index];
		}

		inline void Remove(ZESize Index)
		{
			zeDebugCheck(Index >= Count, "Index is out of range.");
			ZEType* TempPointer = this->Items;
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

		inline void RemoveValue(ZEType Value)
		{
			ZESize N = 0, OldCount = Count;

			for(ZEType* I = Items; I < Items + OldCount; I++)
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
			if (Count == this->Count)
				return;

			this->Count = Count;

			ZEType* OldItems = Items;
			if (Allocator.Allocate(&Items, Count))
				if (OldItems != NULL)
					delete[] OldItems;

			ZEDebugCheckMemory();
		}

		inline void Resize(ZESize Count)
		{
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
		
		inline ZEType* GetCArray()
		{
			return Items;
		}
		
		inline const ZEType* GetConstCArray() const
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

		void Traverse()
		{
			for (ZESize I = 0; I < Count / 2; I++)
			{
				ZEType Temp = Items[I];
				Items[I] = Items[Count - I - 1];
				Items[Count - I - 1] = Temp;
			}
		}

		template<ZEInt CompareFunction(const ZEType*, const ZEType*)>
		inline void Sort()
		{
			qsort(Items, Count, sizeof(ZEType), (int (*)(const void*, const void*))CompareFunction);
		}

		template<ZEInt CompareFunction(const ZEType&, const ZEType&)>
		inline void Sort2()
		{
			qsort(Items, Count, sizeof(ZEType), (int (*)(const void*, const void*))CompareFunction);
		}

		template<typename ZESearchValueType, ZEInt CompareFunction(const ZEType&, ZESearchValueType)>
		ZESSize BinarySearch(ZESearchValueType TargetValue) const
		{
			ZESSize Low = 0;
			ZESSize High = Count;
			while (Low <= High) 
			{
				ZESSize Middle = Low + (High - Low) / 2;
				ZEInt Result = CompareFunction(Items[Middle], TargetValue);
				if (Result > 0)
					Low = Middle + 1;
				else if (Result < 0)
					High = Middle - 1;
				else
					return Middle;
			}
			return -1;
		}

		inline void Sort(ZEInt (*CompareFunction)(const ZEType*, const ZEType*))
		{
			qsort(Items, Count, sizeof(ZEType), (int (*)(const void*, const void*))CompareFunction);
		}

		inline ZESSize BinarySearch(const ZEType& Element, ZEInt (*CompareFunction)(const ZEType*, const ZEType*))
		{
			void* Result = bsearch(&Element, Items, Count, sizeof(ZEType), (int (*)(const void*, const void*))(CompareFunction));
			if (Result == NULL)
				return -1;
			else
				return ((ZEType*)Result - Items);
		}

		inline const ZEType& GetItem(ZESize Index) const
		{
			zeDebugCheck(Index >= Count, "Index is out of range.");
			return Items[Index];
		}

		inline ZEType& GetItem(ZESize Index)
		{
			zeDebugCheck(Index >= Count, "Index is out of range.");
			return Items[Index];
		}

		inline const ZEType& GetFirstItem() const
		{
			return Items[0];
		}

		inline ZEType& GetFirstItem()
		{
			return Items[0];
		}

		inline ZEType& GetLastItem()
		{
			return Items[Count - 1];
		}

		inline const ZEType& GetLastItem() const
		{
			return Items[Count - 1];
		}

		inline ZEType& operator[](ZESize Index)
		{
			zeDebugCheck(Index >= Count, "Index is out of range.");
			return Items[Index];
		}
		
		inline const ZEType& operator[](ZESize Index) const
		{
			zeDebugCheck(Index >= Count, "Index is out of range.");
			return Items[Index];
		}

		ZEArray operator+(const ZEArray<ZEType, Allocator_>& OtherArray)
		{
			ZEArray Temp;
			Temp.MassAdd(Items, Count);
			Temp.MassAdd(OtherArray.Items, OtherArray.Count);
			return Temp;
		}

		ZEArray& operator+=(const ZEArray<ZEType, Allocator_>& OtherArray)
		{
			this->MassAdd(OtherArray.Items, OtherArray.Count);
			return *this;
		}

		bool operator==(const ZEArray<ZEType, Allocator_>& Other)
		{
			if (Count != Other.Count)
				return false;

			for (ZESize I = 0; I < Count; I++)
				if (Items[I] != Other.Items[I])
					return false;

			return true;
		}


		bool operator!=(const ZEArray<ZEType, Allocator_>& Other)
		{
			if (Count != Other.Count)
				return true;

			for (ZESize I = 0; I < Count; I++)
				if (Items[I] != Other.Items[I])
					return true;

			return false;
		}

		inline void operator=(const ZEArray<ZEType, Allocator_>& Other)
		{
			CopyFrom(Other);
		}

		ZEArray()
		{
			Items = NULL;
			Count = 0;
		}

		ZEArray(const ZEArray<ZEType, Allocator_>& C)
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


template <typename ZEType, ZEInt Exponent = 2>
class ZESmartArray : public ZEArray<ZEType, ZESmartAllocator<ZEType, Exponent> >
{};

template <typename ZEType, ZEInt ChunkSize>
class ZEChunkArray : public ZEArray<ZEType, ZEChunkAllocator<ZEType, ChunkSize> >
{};

#endif
