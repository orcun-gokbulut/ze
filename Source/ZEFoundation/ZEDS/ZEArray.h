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

#include "ZETypes.h"
#include "ZEAllocator.h"
#include "ZEArrayIterator.h"
#include "ZEError.h"

#include <stdlib.h>

#define ZE_ARRAY_FRIEND template<typename _Type, typename _Allocator> friend class ZEArray; template<typename _Type> friend class ZEAllocatorBase;
#define ZE_ARRAY_FRIEND_SPECIFIC(Allocator) template<typename _Type, typename _Allocator> friend class ZEArray; template<typename _Type> friend class Allocator;

template<typename ZEItemType, typename ZEAllocatorType= ZEAllocatorBase<ZEItemType> >
class ZEArray
{
	private:
		ZESize							Count;
		ZEAllocatorType					Allocator;
		ZEItemType*						Items;

	public:
		inline void						SetCount(ZESize Count);
		inline ZESize					GetCount() const;
		inline void						Resize(ZESize Count);
		inline ZESize					GetSize() const;

		inline void						SetItem(ZESize Index, ZEItemType Value);
		inline const ZEItemType&		GetItem(ZESize Index) const;
		inline ZEItemType&				GetItem(ZESize Index);

		inline ZEItemType&				GetFirstItem();
		inline const ZEItemType&		GetFirstItem() const;
		inline ZEItemType&				GetLastItem();
		inline const ZEItemType&		GetLastItem() const;

		ZEArrayIterator<ZEItemType, ZEAllocatorType>			GetIterator();
		ZEArrayIteratorConst <ZEItemType, ZEAllocatorType> 		GetIterator() const;
		ZEArrayIterator<ZEItemType, ZEAllocatorType> 			GetIteratorEnd();
		ZEArrayIteratorConst<ZEItemType, ZEAllocatorType> 		GetIteratorEnd() const;

		inline ZEItemType*				GetCArray();
		inline const ZEItemType*		GetConstCArray() const;

		inline ZESSize					FindIndex(ZEItemType Item, ZESize StartIndex = 0) const;
		inline bool						Exists(ZEItemType Value) const;
		inline ZESize					Circular(ZESSize Index) const;

		inline ZEItemType*				Add();
		inline void						AddByRef(const ZEItemType& NewItem);
		inline void						Add(ZEItemType NewItem);
		inline ZEItemType*				MassAdd(ZESize ItemCount);
		inline ZEItemType*				MassAdd(const ZEItemType* NewItems, ZESize ItemCount);

		inline void						Insert(ZESize Index);
		inline void						Insert(ZESize Index, ZEItemType NewItem);
		inline ZEItemType*				MassInsert(ZESize Index, ZESize ItemCount);
		inline ZEItemType*				MassInsert(ZESize Index, ZEItemType* NewItems, ZESize ItemCount);

		inline void						Remove(ZESize Index);
		inline void						RemoveRange(ZESize Index, ZESize Count);
		inline void						RemoveValue(ZEItemType Value);
		
		inline void						Clear();
		inline void						Clear(bool PreserveAllocation);
		
		inline void						Enqueue(ZEItemType Value);
		inline ZEItemType				Dequeue();

		inline void						Push(ZEItemType Value);
		inline ZEItemType				Pop();

		inline void						Fill(ZEItemType Value);
		void							Traverse();

		template<ZEInt CompareFunction(const ZEItemType*, const ZEItemType*)>
		inline void						Sort();
		template<ZEInt CompareFunction(const ZEItemType&, const ZEItemType&)>
		inline void						Sort2();
		inline void						Sort(ZEInt (*CompareFunction)(const ZEItemType*, const ZEItemType*));

		template<typename ZESearchValueType, ZEInt CompareFunction(const ZEItemType&, ZESearchValueType)>
		ZESSize							BinarySearch(ZESearchValueType TargetValue) const;
		inline ZESSize					BinarySearch(const ZEItemType& Element, ZEInt (*CompareFunction)(const ZEItemType*, const ZEItemType*));

		void							CopyTo(ZEArray<ZEItemType, ZEAllocatorType>& OtherArray) const;
		void							CopyTo(ZEItemType* OtherArray, ZESize Count) const;
		void							CopyFrom(const ZEItemType* OtherArray, ZESize Count);
		void							CopyFrom(const ZEArray<ZEItemType, ZEAllocatorType>& OtherArray);
		void							Combine(const ZEArray<ZEItemType, ZEAllocatorType>& OtherArray);
		
		inline ZEItemType&				operator[](ZESize Index);	
		inline const ZEItemType&		operator[](ZESize Index) const;

		inline ZEArray					operator+(const ZEArray<ZEItemType, ZEAllocatorType>& OtherArray);
		inline ZEArray&					operator+=(const ZEArray<ZEItemType, ZEAllocatorType>& OtherArray);

		inline bool						operator==(const ZEArray<ZEItemType, ZEAllocatorType>& Other);
		inline bool						operator!=(const ZEArray<ZEItemType, ZEAllocatorType>& Other);

		inline void						operator=(const ZEArray<ZEItemType, ZEAllocatorType>& Other);

										ZEArray();
										ZEArray(const ZEArray<ZEItemType, ZEAllocatorType>& C);
										~ZEArray();
};


template <typename ZEItemType, ZEInt Exponent = 2>
class ZESmartArray : public ZEArray<ZEItemType, ZESmartAllocator<ZEItemType, Exponent> >
{};

template <typename ZEItemType, ZEInt ChunkSize>
class ZEChunkArray : public ZEArray<ZEItemType, ZEChunkAllocator<ZEItemType, ChunkSize> >
{};


// IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////////

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::SetCount(ZESize Count)
{
	if (Count == this->Count)
		return;

	this->Count = Count;

	ZEItemType* OldItems = Items;
	if (Allocator.Allocate(&Items, Count))
		if (OldItems != NULL)
			delete[] OldItems;

	ZEDebugCheckMemory();
}

template<typename ZEItemType, typename ZEAllocatorType>
ZESize ZEArray<ZEItemType, ZEAllocatorType>::GetCount() const
{
	return Count;
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::Resize(ZESize Count)
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

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::SetItem(ZESize Index, ZEItemType Value)
{
	zeDebugCheck(Index >= Count, "Index is out of range.");
	this->Items[Index] = Value;
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEItemType& ZEArray<ZEItemType, ZEAllocatorType>::GetItem(ZESize Index)
{
	zeDebugCheck(Index >= Count, "Index is out of range.");
	return Items[Index];
}

template<typename ZEItemType, typename ZEAllocatorType>
const ZEItemType& ZEArray<ZEItemType, ZEAllocatorType>::GetItem(ZESize Index) const
{
	zeDebugCheck(Index >= Count, "Index is out of range.");
	return Items[Index];
}

template<typename ZEItemType, typename ZEAllocatorType>
inline ZEItemType& ZEArray<ZEItemType, ZEAllocatorType>::GetFirstItem()
{
	return Items[0];
}

template<typename ZEItemType, typename ZEAllocatorType>
const ZEItemType& ZEArray<ZEItemType, ZEAllocatorType>::GetFirstItem() const
{
	return Items[0];
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEItemType& ZEArray<ZEItemType, ZEAllocatorType>::GetLastItem()
{
	return Items[Count - 1];
}

template<typename ZEItemType, typename ZEAllocatorType>
const ZEItemType& ZEArray<ZEItemType, ZEAllocatorType>::GetLastItem() const
{
	return Items[Count - 1];
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEArrayIterator<ZEItemType, ZEAllocatorType> ZEArray<ZEItemType, ZEAllocatorType>::GetIterator()
{
	return ZEArrayIterator<ZEItemType, ZEAllocatorType>(*this, 0);
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEArrayIteratorConst<ZEItemType, ZEAllocatorType> ZEArray<ZEItemType, ZEAllocatorType>::GetIterator() const
{
	return ZEArrayIteratorConst<ZEItemType, ZEAllocatorType>(*this, 0);
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEArrayIterator<ZEItemType, ZEAllocatorType> ZEArray<ZEItemType, ZEAllocatorType>::GetIteratorEnd()
{
	return ZEArrayIterator<ZEItemType, ZEAllocatorType>(*this, (Count == 0 ? 0 : Count - 1));
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEArrayIteratorConst<ZEItemType, ZEAllocatorType> ZEArray<ZEItemType, ZEAllocatorType>::GetIteratorEnd() const
{
	return ZEArrayIteratorConst<ZEItemType, ZEAllocatorType>(*this, (Count == 0 ? 0 : Count - 1));
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEItemType* ZEArray<ZEItemType, ZEAllocatorType>::GetCArray()
{
	return Items;
}

template<typename ZEItemType, typename ZEAllocatorType>
ZESize ZEArray<ZEItemType, ZEAllocatorType>::GetSize() const
{
	return Allocator.GetSize();
}

template<typename ZEItemType, typename ZEAllocatorType>
const ZEItemType* ZEArray<ZEItemType, ZEAllocatorType>::GetConstCArray() const
{
	return Items;
}

template<typename ZEItemType, typename ZEAllocatorType>
ZESSize ZEArray<ZEItemType, ZEAllocatorType>::FindIndex(ZEItemType Item, ZESize StartIndex = 0) const
{
	for(ZESize I = StartIndex; I < Count; I++)
		if (Items[I] == Item)
			return I;

	return -1;	
}

template<typename ZEItemType, typename ZEAllocatorType>
bool ZEArray<ZEItemType, ZEAllocatorType>::Exists(ZEItemType Value) const
{
	return FindIndex(Value) != -1;
}

template<typename ZEItemType, typename ZEAllocatorType>
ZESize ZEArray<ZEItemType, ZEAllocatorType>::Circular(ZESSize Index) const
{
	if (Index < 0)
	{
		ZESSize CircularIndex = ((-Index) % Count);
		return (CircularIndex == 0 ? 0 : Count - CircularIndex);
	}
	else
		return Index % Count;
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEItemType* ZEArray<ZEItemType, ZEAllocatorType>::Add()
{
	Count++;
	Allocator.Reallocate(&Items, Count); 

	ZEDebugCheckMemory();

	return &Items[Count - 1];
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::Add(ZEItemType NewItem)
{
	Count++;
	Allocator.Reallocate(&Items, Count);
	Items[Count - 1] = NewItem;
	ZEDebugCheckMemory();
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::AddByRef(const ZEItemType& NewItem)
{
	Count++;
	Allocator.Reallocate(&Items, Count);
	Items[Count - 1] = NewItem;

	ZEDebugCheckMemory();
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEItemType* ZEArray<ZEItemType, ZEAllocatorType>::MassAdd(ZESize ItemCount)
{
	Resize(Count + ItemCount);

	return &Items[Count - ItemCount];
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEItemType* ZEArray<ZEItemType, ZEAllocatorType>::MassAdd(const ZEItemType* NewItems, ZESize ItemCount)
{
	Resize(Count + ItemCount);
	ZEAllocatorBase<ZEItemType>::ObjectCopy(this->Items + Count - ItemCount, NewItems, ItemCount);

	ZEDebugCheckMemory();

	return &Items[Count - ItemCount];
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::Insert(ZESize Index)
{
	zeDebugCheck(Index > Count, "Index is out of range.");
	ZEItemType* TempPointer = this->Items;
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
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::Insert(ZESize Index, ZEItemType NewItem)
{

	Insert(Index);
	Items[Index] = NewItem;

	ZEDebugCheckMemory();
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEItemType* ZEArray<ZEItemType, ZEAllocatorType>::MassInsert(ZESize Index, ZESize ItemCount)
{
	ZEItemType* NewBuffer;
	if (Allocator.Allocate(&NewBuffer, Count + ItemCount))
	{
		ZEAllocatorBase<ZEItemType>::ObjectCopy(NewBuffer, this->Items, Index);
		ZEAllocatorBase<ZEItemType>::ObjectCopy(NewBuffer + Index + ItemCount, this->Items + Index, Count - Index);
		delete[] Items;
		Items = NewBuffer;
	}
	else
		ZEAllocatorBase<ZEItemType>::ObjectCopy(this->Items + Index + ItemCount, this->Items + Index, Count - Index);

	Count += ItemCount;

	ZEDebugCheckMemory();

	return &Items[Index];
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEItemType* ZEArray<ZEItemType, ZEAllocatorType>::MassInsert(ZESize Index, ZEItemType* NewItems, ZESize ItemCount)
{
	ZEItemType* NewBuffer;
	if (Allocator.Allocate(&NewBuffer, Count + ItemCount))
	{
		ZEAllocatorBase<ZEItemType>::ObjectCopy(NewBuffer, this->Items, Index);
		ZEAllocatorBase<ZEItemType>::ObjectCopy(NewBuffer + Index + ItemCount, this->Items + Index, Count - Index);
		delete[] Items;
		Items = NewBuffer;
	}
	else
		ZEAllocatorBase<ZEItemType>::ObjectCopy(this->Items + Index + ItemCount, this->Items + Index, Count - Index);

	ZEAllocatorBase<ZEItemType>::ObjectCopy(this->Items + Index, NewItems, ItemCount);

	Count += ItemCount;

	ZEDebugCheckMemory();

	return &Items[Index];
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::Remove(ZESize Index)
{
	zeDebugCheck(Index >= Count, "Index is out of range.");
	ZEItemType* TempPointer = this->Items;
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

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::RemoveRange(ZESize Index, ZESize Count)
{
	zeDebugCheck(Index + Count >= Count, "Index and Count is out of range.");

	ZEItemType* TempPointer = this->Items;
	bool Changed;

	if (this->Count > 1)
	{
		Changed = Allocator.Allocate(&Items, this->Count - Count);
	}
	else
	{
		Allocator.Deallocate(&Items);
		this->Count = 0;
		return;
	}

	for(ZESize I = Index + Count; I < this->Count - Count; I++)
		Items[I] = TempPointer[I + 1];

	if (Changed && Index != 0)
	{
		for(ZESize I = 0; I < Index; I++)
			Items[I] = TempPointer[I];
		delete[] TempPointer;
	}

	this->Count -= Count;

	ZEDebugCheckMemory();
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::RemoveValue(ZEItemType Value)
{
	ZESize N = 0, OldCount = Count;

	for(ZEItemType* I = Items; I < Items + OldCount; I++)
		if (*I != Value)
		{
			Items[N++] = *I;
		}
		else
			Count--;

	Allocator.Reallocate(&Items, Count);

	ZEDebugCheckMemory();
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::Clear()
{
	Allocator.Deallocate(&Items);
	Count = 0;
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::Clear(bool PreserveAllocation)
{
	if (PreserveAllocation)
		Allocator.Deallocate(&Items);

	Count = 0;
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::Enqueue(ZEItemType Value)
{
	Add(Value);
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEItemType ZEArray<ZEItemType, ZEAllocatorType>::Dequeue()
{
	zeDebugCheck(Count == 0, "There is no item in the queue.");
	ZEItemType Temp = Items[0];
	Remove(0);
	return Temp;			
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::Push(ZEItemType Value)
{
	Add(Value);
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEItemType ZEArray<ZEItemType, ZEAllocatorType>::Pop()
{
	zeDebugCheck(Count == 0, "There is no item in the stack.");
	ZEItemType Temp = Items[Count - 1];
	Remove(Count - 1);
	return Temp;
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::Fill(ZEItemType Value)
{
	for (ZESize I = 0; I < Count; I++)
		Items[I] = Value;
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::Traverse()
{
	for (ZESize I = 0; I < Count / 2; I++)
	{
		ZEItemType Temp = Items[I];
		Items[I] = Items[Count - I - 1];
		Items[Count - I - 1] = Temp;
	}
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::CopyTo(ZEArray<ZEItemType, ZEAllocatorType>& OtherArray) const
{
	OtherArray.SetCount(Count);
	ZEAllocatorBase<ZEItemType>::ObjectCopy(OtherArray.Items, this->Items, Count);
	ZEDebugCheckMemory();
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::CopyTo(ZEItemType* OtherArray, ZESize Count) const
{
	ZEAllocatorBase<ZEItemType>::ObjectCopy(OtherArray, this->Items, Count > this->Count ? this->Count : Count);
	ZEDebugCheckMemory();
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::CopyFrom(const ZEItemType* OtherArray, ZESize Count)
{
	this->SetCount(Count);
	ZEAllocatorBase<ZEItemType>::ObjectCopy(this->Items, OtherArray, Count);	

	ZEDebugCheckMemory();
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::CopyFrom(const ZEArray<ZEItemType, ZEAllocatorType>& OtherArray)
{
	CopyFrom(OtherArray.GetConstCArray(), OtherArray.Count);
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::Combine(const ZEArray<ZEItemType, ZEAllocatorType>& OtherArray)
{
	MassAdd(OtherArray.Items, OtherArray.Count);
}

template<typename ZEItemType, typename ZEAllocatorType>
template<ZEInt CompareFunction(const ZEItemType*, const ZEItemType*)>
void ZEArray<ZEItemType, ZEAllocatorType>::Sort()
{
	qsort(Items, Count, sizeof(ZEItemType), (int (*)(const void*, const void*))CompareFunction);
}

template<typename ZEItemType, typename ZEAllocatorType>
template<ZEInt CompareFunction(const ZEItemType&, const ZEItemType&)>
void ZEArray<ZEItemType, ZEAllocatorType>::Sort2()
{
	qsort(Items, Count, sizeof(ZEItemType), (int (*)(const void*, const void*))CompareFunction);
}

template<typename ZEItemType, typename ZEAllocatorType>
template<typename ZESearchValueType, ZEInt CompareFunction(const ZEItemType&, ZESearchValueType)>
ZESSize ZEArray<ZEItemType, ZEAllocatorType>::BinarySearch(ZESearchValueType TargetValue) const
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

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::Sort(ZEInt (*CompareFunction)(const ZEItemType*, const ZEItemType*))
{
	qsort(Items, Count, sizeof(ZEItemType), (int (*)(const void*, const void*))CompareFunction);
}

template<typename ZEItemType, typename ZEAllocatorType>
ZESSize ZEArray<ZEItemType, ZEAllocatorType>::BinarySearch(const ZEItemType& Element, ZEInt (*CompareFunction)(const ZEItemType*, const ZEItemType*))
{
	void* Result = bsearch(&Element, Items, Count, sizeof(ZEItemType), (int (*)(const void*, const void*))(CompareFunction));
	if (Result == NULL)
		return -1;
	else
		return ((ZEItemType*)Result - Items);
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEItemType& ZEArray<ZEItemType, ZEAllocatorType>::operator[](ZESize Index)
{
	zeDebugCheck(Index >= Count, "Index is out of range.");
	return Items[Index];
}

template<typename ZEItemType, typename ZEAllocatorType>
const ZEItemType& ZEArray<ZEItemType, ZEAllocatorType>::operator[](ZESize Index) const
{
	zeDebugCheck(Index >= Count, "Index is out of range.");
	return Items[Index];
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEArray<ZEItemType, ZEAllocatorType> ZEArray<ZEItemType, ZEAllocatorType>::operator+(const ZEArray<ZEItemType, ZEAllocatorType>& OtherArray)
{
	ZEArray Temp;
	Temp.MassAdd(Items, Count);
	Temp.MassAdd(OtherArray.Items, OtherArray.Count);
	return Temp;
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEArray<ZEItemType, ZEAllocatorType>& ZEArray<ZEItemType, ZEAllocatorType>::operator+=(const ZEArray<ZEItemType, ZEAllocatorType>& OtherArray)
{
	this->MassAdd(OtherArray.Items, OtherArray.Count);
	return *this;
}

template<typename ZEItemType, typename ZEAllocatorType>
bool ZEArray<ZEItemType, ZEAllocatorType>::operator==(const ZEArray<ZEItemType, ZEAllocatorType>& Other)
{
	if (Count != Other.Count)
		return false;

	for (ZESize I = 0; I < Count; I++)
		if (Items[I] != Other.Items[I])
			return false;

	return true;
}

template<typename ZEItemType, typename ZEAllocatorType>
bool ZEArray<ZEItemType, ZEAllocatorType>::operator!=(const ZEArray<ZEItemType, ZEAllocatorType>& Other)
{
	if (Count != Other.Count)
		return true;

	for (ZESize I = 0; I < Count; I++)
		if (Items[I] != Other.Items[I])
			return true;

	return false;
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArray<ZEItemType, ZEAllocatorType>::operator=(const ZEArray<ZEItemType, ZEAllocatorType>& Other)
{
	CopyFrom(Other);
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEArray<ZEItemType, ZEAllocatorType>::ZEArray()
{
	Items = NULL;
	Count = 0;
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEArray<ZEItemType, ZEAllocatorType>::ZEArray(const ZEArray<ZEItemType, ZEAllocatorType>& C)
{
	Items = NULL;
	Count = 0;
	CopyFrom(C);
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEArray<ZEItemType, ZEAllocatorType>::~ZEArray()
{
	Allocator.Deallocate(&Items);
}
