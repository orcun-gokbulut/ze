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

#pragma warning(push)
#pragma warning(disable: 4348)

#include "ZETypes.h"
#include "ZEError.h"
#include "ZEAllocator.h"
#include "ZEArrayIterator.h"
#include "ZEThread/ZELockRW.h"
#include "ZEMeta/ZEMTCollection.h"
#include "ZEMeta/ZEMTTypeGeneratorDynamic.h"

#include <stdlib.h>

#define ZE_ARRAY_FRIEND template<typename _Type, typename _Allocator, typename _Lock> friend class ZEArray; template<typename _Type> friend class ZEAllocatorBase;

#define ZE_ARRAY_TEMPLATE template<typename ZEItemType, typename ZEAllocatorType, typename ZELockType>
#define ZE_ARRAY_SPECIALIZATION ZEItemType, ZEAllocatorType, ZELockType

#define ZE_ARRAY_OTHER_TEMPLATE template<typename ZEAllocatorTypeOther, typename ZELockTypeOther>
#define ZE_ARRAY_OTHER_SPEC ZEItemType, ZEAllocatorTypeOther, ZELockTypeOther

template<typename ZEItemType, typename ZEAllocatorType = ZEAllocatorBase<ZEItemType>, typename ZELockType = ZELockRW>
class ZEArray : public ZEMTCollection
{
	friend class ZEMTType;
	private:
		ZEItemType*											Items;
		ZESize												Count;
		mutable ZEMTType									Type;
		mutable ZELockRW									Lock;
		ZEAllocatorType										Allocator;

		virtual ZESize										GetCountRaw() const override;
		virtual bool										RemoveRaw(ZESize Index) override;
		virtual bool										InsertRaw(ZESize Index, const void* ItemPointer) override;
		virtual bool										SetItemRaw(ZESize Index, const void* ItemPointer) override;
		virtual bool										GetItemRaw(ZESize Index, void* ItemPointer) const override;

	public:
		virtual const ZEMTType&								GetType() const override;

		inline void											SetCount(ZESize Count);
		inline ZESize										GetCount() const;
		inline void											Resize(ZESize Count);
		inline ZESize										GetSize() const;

		inline void											SetItem(ZESize Index, ZEItemType Value);
		inline const ZEItemType&							GetItem(ZESize Index) const;
		inline ZEItemType&									GetItem(ZESize Index);

		inline ZEItemType&									GetFirstItem();
		inline const ZEItemType&							GetFirstItem() const;
		inline ZEItemType&									GetLastItem();
		inline const ZEItemType&							GetLastItem() const;

		ZEArrayIterator<ZE_ARRAY_SPECIALIZATION>			GetIterator();
		ZEArrayIterator<ZE_ARRAY_SPECIALIZATION>			GetIterator(ZESize Index);
		ZEArrayIteratorConst <ZE_ARRAY_SPECIALIZATION> 		GetIterator() const;
		ZEArrayIteratorConst <ZE_ARRAY_SPECIALIZATION> 		GetIterator(ZESize Index) const;
		ZEArrayIterator<ZE_ARRAY_SPECIALIZATION> 			GetIteratorEnd();
		ZEArrayIteratorConst<ZE_ARRAY_SPECIALIZATION> 		GetIteratorEnd() const;

		inline ZEItemType*									GetCArray();
		inline const ZEItemType*							GetConstCArray() const;

		inline ZESSize										FindIndex(ZEItemType Item, ZESize StartIndex = 0) const;
		inline bool											Exists(ZEItemType Value) const;
		inline ZESize										Circular(ZESSize Index) const;

		inline ZEItemType*									Add();
		inline void											AddByRef(const ZEItemType& NewItem);
		inline void											Add(ZEItemType NewItem);
		inline ZEItemType*									AddMultiple(ZESize ItemCount);
		inline void											AddMultiple(const ZEItemType* NewItems, ZESize ItemCount);
		ZE_ARRAY_OTHER_TEMPLATE
		inline void											AddMultiple(const ZEArray<ZE_ARRAY_OTHER_SPEC>& OtherArray);

		inline ZEItemType*									Insert(ZESize Index);
		inline void											Insert(ZESize Index, ZEItemType NewItem);
		inline ZEItemType*									InsertMultiple(ZESize Index, ZESize ItemCount);
		inline void											InsertMultiple(ZESize Index, ZEItemType* NewItems, ZESize ItemCount);
		ZE_ARRAY_OTHER_TEMPLATE
		inline void											InsertMultiple(ZESize Index, const ZEArray<ZE_ARRAY_OTHER_SPEC>& OtherArray);

		inline void											Remove(ZESize Index);
		inline void											RemoveMultiple(ZESize Index, ZESize Count);
		inline void											RemoveValue(ZEItemType Value);
		
		ZE_ARRAY_OTHER_TEMPLATE
		inline void											Copy(const ZEArray<ZEItemType, ZEAllocatorTypeOther, ZELockTypeOther>& OtherArray);

		inline void											Clear();
		
		inline void											Enqueue(ZEItemType Value);
		inline ZEItemType									Dequeue();

		inline void											Push(ZEItemType Value);
		inline ZEItemType									Pop();

		inline void											Fill(ZEItemType Value);
		void												Traverse();

		template<ZEInt CompareFunction(const ZEItemType*, const ZEItemType*)>
		inline void											Sort();
		template<ZEInt CompareFunction(const ZEItemType&, const ZEItemType&)>
		inline void											Sort2();
		inline void											Sort(ZEInt (*CompareFunction)(const ZEItemType*, const ZEItemType*));

		template<typename ZESearchValueType, ZEInt CompareFunction(const ZEItemType&, ZESearchValueType)>
		ZESSize												BinarySearch(ZESearchValueType TargetValue) const;
		inline ZESSize										BinarySearch(const ZEItemType& Element, ZEInt (*CompareFunction)(const ZEItemType*, const ZEItemType*));
	
		inline ZEItemType&									operator[](ZESize Index);	
		inline const ZEItemType&							operator[](ZESize Index) const;

		ZE_ARRAY_OTHER_TEMPLATE
		inline ZEArray										operator+(const ZEArray<ZE_ARRAY_OTHER_SPEC>& OtherArray) const;
		ZE_ARRAY_OTHER_TEMPLATE
		inline ZEArray&										operator+=(const ZEArray<ZE_ARRAY_OTHER_SPEC>& OtherArray);

		ZE_ARRAY_OTHER_TEMPLATE
		inline bool											operator==(const ZEArray<ZE_ARRAY_OTHER_SPEC>& OtherArray) const;
		ZE_ARRAY_OTHER_TEMPLATE
		inline bool											operator!=(const ZEArray<ZE_ARRAY_OTHER_SPEC>& OtherArray) const;

		ZE_ARRAY_OTHER_TEMPLATE
		inline void											operator=(const ZEArray<ZE_ARRAY_OTHER_SPEC>& OtherArray);
		inline void											operator=(const ZEArray<ZE_ARRAY_SPECIALIZATION>& OtherArray);

		void												LockRead() const;
		void												UnlockRead() const;

		void												LockWrite();
		void												LockWriteNested();
		void												UnlockWrite();

															ZEArray();
		explicit						ZEArray(ZESize Count);
															ZE_ARRAY_OTHER_TEMPLATE
															ZEArray(const ZEArray<ZE_ARRAY_OTHER_SPEC>& OtherArray);
															ZEArray(const ZEArray<ZE_ARRAY_SPECIALIZATION>& OtherArray);
															~ZEArray();
};

template <typename ZEItemType, typename ZEAllocatorType = ZEAllocatorBase<ZEItemType>>
class ZEArrayMT : public ZEArray<ZEItemType, ZEAllocatorType, ZELockRW>
{};

template <typename ZEItemType, ZEInt Exponent = 2, typename ZELockType = ZELockRW>
class ZESmartArray : public ZEArray<ZEItemType, ZESmartAllocator<ZEItemType, Exponent>, ZELockType>
{};

template <typename ZEItemType, ZEInt Exponent = 2>
class ZESmartArrayMT : public ZEArray<ZEItemType, ZESmartAllocator<ZEItemType, Exponent>, ZELockRW>
{};

template <typename ZEItemType, ZEInt ChunkSize, typename ZELockType = ZELockRW>
class ZEChunkArray : public ZEArray<ZEItemType, ZEChunkAllocator<ZEItemType, ChunkSize>, ZELockType>
{};

template <typename ZEItemType, ZEInt ChunkSize>
class ZEChunkArrayMT : public ZEArray<ZEItemType, ZEChunkAllocator<ZEItemType, ChunkSize>, ZELockRW>
{};

// IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////////

ZE_ARRAY_TEMPLATE
const ZEMTType& ZEArray<ZE_ARRAY_SPECIALIZATION>::GetType() const
{
	if (!Type.IsBinded())
	{
		Type = ZEMT_TYPE_GENERATOR_DYNAMIC(ZEItemType);
		Type.SetBinded(true);

		if (!Type.IsValid() || Type.IsReference() || Type.IsCollection())
			return Type;

		if (Type.IsConst() && (Type.GetBaseType() != ZEMT_BT_OBJECT_PTR && Type.GetBaseType() != ZEMT_BT_OBJECT_HOLDER))
			return Type;

		Type.SetCollectionType(ZEMT_CT_ARRAY);
		Type.SetCollectionQualifier(ZEMT_TQ_VALUE);
	}

	return Type;
}

ZE_ARRAY_TEMPLATE
ZESize ZEArray<ZE_ARRAY_SPECIALIZATION>::GetCountRaw() const
{
	return GetCount();
}

ZE_ARRAY_TEMPLATE
bool ZEArray<ZE_ARRAY_SPECIALIZATION>::RemoveRaw(ZESize Index)
{
	if (Index >= Count)
		return false;


	Remove(Index);
	return true;
}

ZE_ARRAY_TEMPLATE
bool ZEArray<ZE_ARRAY_SPECIALIZATION>::InsertRaw(ZESize Index, const void* ItemPointer)
{
	if (Index >= Count)
		return false;

	Insert(Index, *(ZEItemType*)(ItemPointer));
	return true;
}

ZE_ARRAY_TEMPLATE
bool ZEArray<ZE_ARRAY_SPECIALIZATION>::SetItemRaw(ZESize Index, const void* ItemPointer)
{
	if (Index >= Count)
		return false;

	Items[Index] = *(ZEItemType*)(ItemPointer);
	return true;
}

ZE_ARRAY_TEMPLATE
bool ZEArray<ZE_ARRAY_SPECIALIZATION>::GetItemRaw(ZESize Index, void* ItemPointer) const
{
	if (Index >= Count)
		return false;

	*static_cast<ZEItemType*>(ItemPointer) = Items[Index];
	return true;
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::SetCount(ZESize Count)
{
	Lock.LockWriteNested();
	{
		if (Count == this->Count)
		{
			Lock.UnlockWrite();
			return;
		}

		this->Count = Count;

		ZEItemType* OldItems = Items;
		if (Allocator.Allocate(&Items, Count))
		{
			if (OldItems != NULL)
				delete[] OldItems;
		}

		ZEDebugCheckMemory();
	}
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
ZESize ZEArray<ZE_ARRAY_SPECIALIZATION>::GetCount() const
{
	return Count;
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::Resize(ZESize Count)
{
	Lock.LockWriteNested();
	{
		if (Count == this->Count)
		{
			Lock.UnlockWrite();
			return;
		}

		if (Count == 0)
		{
			SetCount(0);
			Lock.UnlockWrite();
			return;
		}

		Allocator.Reallocate(&Items, Count);
		this->Count = Count;

		ZEDebugCheckMemory();
	}
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
ZESize ZEArray<ZE_ARRAY_SPECIALIZATION>::GetSize() const
{
	return Allocator.GetSize();
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::SetItem(ZESize Index, ZEItemType Value)
{
	Lock.LockWriteNested();
	{
		zeDebugCheck(Index >= Count, "Index is out of range.");
		this->Items[Index] = Value;
	}
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
ZEItemType& ZEArray<ZE_ARRAY_SPECIALIZATION>::GetItem(ZESize Index)
{
	zeDebugCheck(Index >= Count, "Index is out of range.");
	return Items[Index];
}

ZE_ARRAY_TEMPLATE
const ZEItemType& ZEArray<ZE_ARRAY_SPECIALIZATION>::GetItem(ZESize Index) const
{
	zeDebugCheck(Index >= Count, "Index is out of range.");
	return Items[Index];
}

ZE_ARRAY_TEMPLATE
inline ZEItemType& ZEArray<ZE_ARRAY_SPECIALIZATION>::GetFirstItem()
{
	return Items[0];
}

ZE_ARRAY_TEMPLATE
const ZEItemType& ZEArray<ZE_ARRAY_SPECIALIZATION>::GetFirstItem() const
{
	return Items[0];
}

ZE_ARRAY_TEMPLATE
ZEItemType& ZEArray<ZE_ARRAY_SPECIALIZATION>::GetLastItem()
{
	return Items[Count - 1];
}

ZE_ARRAY_TEMPLATE
const ZEItemType& ZEArray<ZE_ARRAY_SPECIALIZATION>::GetLastItem() const
{
	return Items[Count - 1];
}

ZE_ARRAY_TEMPLATE
ZEArrayIterator<ZE_ARRAY_SPECIALIZATION> ZEArray<ZE_ARRAY_SPECIALIZATION>::GetIterator()
{
	return ZEArrayIterator<ZE_ARRAY_SPECIALIZATION>(*this, 0);
}

ZE_ARRAY_TEMPLATE
ZEArrayIteratorConst<ZE_ARRAY_SPECIALIZATION> ZEArray<ZE_ARRAY_SPECIALIZATION>::GetIterator() const
{
	return ZEArrayIteratorConst<ZE_ARRAY_SPECIALIZATION>(*this, 0);
}

ZE_ARRAY_TEMPLATE
ZEArrayIteratorConst<ZE_ARRAY_SPECIALIZATION> ZEArray<ZE_ARRAY_SPECIALIZATION>::GetIterator(ZESize Index) const
{
	return ZEArrayIteratorConst<ZE_ARRAY_SPECIALIZATION>(*this, Index);
}

ZE_ARRAY_TEMPLATE
ZEArrayIterator<ZE_ARRAY_SPECIALIZATION> ZEArray<ZE_ARRAY_SPECIALIZATION>::GetIteratorEnd()
{
	return ZEArrayIterator<ZE_ARRAY_SPECIALIZATION>(*this, (Count == 0 ? 0 : Count - 1));
}

ZE_ARRAY_TEMPLATE
ZEArrayIteratorConst<ZE_ARRAY_SPECIALIZATION> ZEArray<ZE_ARRAY_SPECIALIZATION>::GetIteratorEnd() const
{
	return ZEArrayIteratorConst<ZE_ARRAY_SPECIALIZATION>(*this, (Count == 0 ? 0 : Count - 1));
}

ZE_ARRAY_TEMPLATE
ZEItemType* ZEArray<ZE_ARRAY_SPECIALIZATION>::GetCArray()
{
	return Items;
}

ZE_ARRAY_TEMPLATE
const ZEItemType* ZEArray<ZE_ARRAY_SPECIALIZATION>::GetConstCArray() const
{
	return Items;
}

ZE_ARRAY_TEMPLATE
ZESSize ZEArray<ZE_ARRAY_SPECIALIZATION>::FindIndex(ZEItemType Item, ZESize StartIndex = 0) const
{
	Lock.LockRead();
	{
		for(ZESize I = StartIndex; I < Count; I++)
		{
			if (Items[I] == Item)
			{
				Lock.UnlockRead();
				return I;
			}
		}
	}
	Lock.UnlockRead();

	return -1;	
}

ZE_ARRAY_TEMPLATE
bool ZEArray<ZE_ARRAY_SPECIALIZATION>::Exists(ZEItemType Value) const
{
	return FindIndex(Value) != -1;
}

ZE_ARRAY_TEMPLATE
ZESize ZEArray<ZE_ARRAY_SPECIALIZATION>::Circular(ZESSize Index) const
{
	if (Index < 0)
	{
		ZESSize CircularIndex = ((-Index) % Count);
		return (CircularIndex == 0 ? 0 : Count - CircularIndex);
	}
	else
	{
		return Index % Count;
	}
}

ZE_ARRAY_TEMPLATE
ZEItemType* ZEArray<ZE_ARRAY_SPECIALIZATION>::Add()
{
	Lock.LockWriteNested();
	{
		Count++;
		Allocator.Reallocate(&Items, Count);

		ZEDebugCheckMemory();
	}
	Lock.UnlockWrite();

	return &Items[Count - 1];
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::Add(ZEItemType NewItem)
{
	Lock.LockWriteNested();
	{
		Count++;
		Allocator.Reallocate(&Items, Count); 
		Items[Count - 1] = NewItem;

		ZEDebugCheckMemory();
	}
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::AddByRef(const ZEItemType& NewItem)
{
	Lock.LockWriteNested();
	{
		Count++;
		Allocator.Reallocate(&Items, Count); 
		Items[Count - 1] = NewItem;

		ZEDebugCheckMemory();
	}
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
ZEItemType* ZEArray<ZE_ARRAY_SPECIALIZATION>::AddMultiple(ZESize ItemCount)
{
	Resize(Count + ItemCount);
	return &Items[Count - ItemCount];
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::AddMultiple(const ZEItemType* NewItems, ZESize ItemCount)
{
	Lock.LockWriteNested();
	{
		Resize(Count + ItemCount);
		ZEAllocatorBase<ZEItemType>::ObjectCopy(this->Items + Count - ItemCount, NewItems, ItemCount);

		ZEDebugCheckMemory();
	}
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
ZE_ARRAY_OTHER_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::AddMultiple(const ZEArray<ZE_ARRAY_OTHER_SPEC>& OtherArray)
{
	OtherArray.LockRead();
	{
		AddMultiple(OtherArray.GetConstCArray(), OtherArray.GetCount());
	}
	OtherArray.UnlockRead();
}

ZE_ARRAY_TEMPLATE
ZEItemType* ZEArray<ZE_ARRAY_SPECIALIZATION>::Insert(ZESize Index)
{
	Lock.LockWriteNested();
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
	Lock.UnlockWrite();

	return &Items[Index];
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::Insert(ZESize Index, ZEItemType NewItem)
{
	Lock.LockWriteNested();
	{
		Insert(Index);
		Items[Index] = NewItem;

		ZEDebugCheckMemory();
	}
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
ZEItemType* ZEArray<ZE_ARRAY_SPECIALIZATION>::InsertMultiple(ZESize Index, ZESize ItemCount)
{
	Lock.LockWriteNested();
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
		{
			ZEAllocatorBase<ZEItemType>::ObjectCopy(this->Items + Index + ItemCount, this->Items + Index, Count - Index);
		}

		Count += ItemCount;

		ZEDebugCheckMemory();
	}
	Lock.UnlockWrite();

	return &Items[Index];
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::InsertMultiple(ZESize Index, ZEItemType* NewItems, ZESize ItemCount)
{
	Lock.LockWriteNested();
	{
		InsertMultiple(Index, ItemCount);
		ZEAllocatorBase<ZEItemType>::ObjectCopy(this->Items + Index, NewItems, ItemCount);

		ZEDebugCheckMemory();
	}
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
ZE_ARRAY_OTHER_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::InsertMultiple(ZESize Index, const ZEArray<ZE_ARRAY_OTHER_SPEC>& OtherArray)
{
	OtherArray.LockRead();
	{
		InsertMultiple(Index, OtherArray.GetConstCArray(), OtherArray.GetCount());
	}
	OtherArray.UnlockRead();
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::Remove(ZESize Index)
{
	Lock.LockWriteNested();
	{
		zeDebugCheck(Index >= Count, "Index is out of range.");
		ZEItemType* TempPointer = this->Items;
		bool Changed;

		if (Count > 1)
		{
			Changed = Allocator.Allocate(&Items, Count - 1);
		}
		else
		{
			Allocator.Deallocate(&Items);
			Count = 0;
			ZEDebugCheckMemory();
			Lock.UnlockWrite();
		
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
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::RemoveMultiple(ZESize Index, ZESize Count)
{
	Lock.LockWriteNested();
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
			ZEDebugCheckMemory();
			Lock.UnlockWrite();

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
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::RemoveValue(ZEItemType Value)
{
	Lock.LockWriteNested();
	{
		ZESize N = 0, OldCount = Count;

		for (ZEItemType* I = Items; I < Items + OldCount; I++)
		{
			if (*I != Value)
				Items[N++] = *I;
			else
				Count--;
		}

		Allocator.Reallocate(&Items, Count);

		ZEDebugCheckMemory();
	}
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
ZE_ARRAY_OTHER_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::Copy(const ZEArray<ZE_ARRAY_OTHER_SPEC>& OtherArray)
{
	OtherArray.LockRead();
	Lock.LockWriteNested();
	{
		SetCount(OtherArray.GetCount());
		ZEAllocatorBase<ZEItemType>::ObjectCopy(Items, OtherArray.GetConstCArray(), OtherArray.GetCount());

		ZEDebugCheckMemory();
	}
	Lock.UnlockWrite();
	OtherArray.UnlockRead();
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::Clear()
{
	Lock.LockWriteNested();
	{
		Allocator.Deallocate(&Items);
		Count = 0;

		ZEDebugCheckMemory();
	}
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::Enqueue(ZEItemType Value)
{
	Add(Value);
}

ZE_ARRAY_TEMPLATE
ZEItemType ZEArray<ZE_ARRAY_SPECIALIZATION>::Dequeue()
{
	Lock.LockWriteNested();
	//{
		zeDebugCheck(Count == 0, "There is no item in the queue.");
		ZEItemType Temp = Items[0];
		Remove(0);
	//}
	Lock.UnlockWrite();

	return Temp;			
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::Push(ZEItemType Value)
{
	Add(Value);
}

ZE_ARRAY_TEMPLATE
ZEItemType ZEArray<ZE_ARRAY_SPECIALIZATION>::Pop()
{
	Lock.LockWriteNested();
	//{
		zeDebugCheck(Count == 0, "There is no item in the stack.");
		ZEItemType Temp = Items[Count - 1];
		Remove(Count - 1);
	//}
	Lock.UnlockWrite();

	return Temp;
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::Fill(ZEItemType Value)
{
	Lock.LockWriteNested();
	{
		for (ZESize I = 0; I < Count; I++)
			Items[I] = Value;
	}
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::Traverse()
{
	Lock.LockWriteNested();
	{
		for (ZESize I = 0; I < Count / 2; I++)
		{
			ZEItemType Temp = Items[I];
			Items[I] = Items[Count - I - 1];
			Items[Count - I - 1] = Temp;
		}
	}
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
template<ZEInt CompareFunction(const ZEItemType*, const ZEItemType*)>
void ZEArray<ZE_ARRAY_SPECIALIZATION>::Sort()
{
	Lock.LockWriteNested();
	{
		qsort(Items, Count, sizeof(ZEItemType), (int (*)(const void*, const void*))CompareFunction);
	}
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
template<ZEInt CompareFunction(const ZEItemType&, const ZEItemType&)>
void ZEArray<ZE_ARRAY_SPECIALIZATION>::Sort2()
{
	Lock.LockWriteNested();
	{
		qsort(Items, Count, sizeof(ZEItemType), (int (*)(const void*, const void*))CompareFunction);
	}
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::Sort(ZEInt (*CompareFunction)(const ZEItemType*, const ZEItemType*))
{
	Lock.LockWriteNested();
	{
		qsort(Items, Count, sizeof(ZEItemType), (int (*)(const void*, const void*))CompareFunction);
	}
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
template<typename ZESearchValueType, ZEInt CompareFunction(const ZEItemType&, ZESearchValueType)>
ZESSize ZEArray<ZE_ARRAY_SPECIALIZATION>::BinarySearch(ZESearchValueType TargetValue) const
{
	Lock.LockRead();
	{
		ZESSize Low = 0;
		ZESSize High = Count;
		while (Low <= High) 
		{
			ZESSize Middle = Low + (High - Low) / 2;
			ZEInt Result = CompareFunction(Items[Middle], TargetValue);
			if (Result > 0)
			{
				Low = Middle + 1;
			}
			else if (Result < 0)
			{
				High = Middle - 1;
			}
			else
			{
				Lock.UnlockRead();
				return Middle;
			}
		}
	}
	Lock.UnlockRead();

	return -1;
}

ZE_ARRAY_TEMPLATE
ZESSize ZEArray<ZE_ARRAY_SPECIALIZATION>::BinarySearch(const ZEItemType& Element, ZEInt (*CompareFunction)(const ZEItemType*, const ZEItemType*))
{
	Lock.LockRead();
	void* Result = bsearch(&Element, Items, Count, sizeof(ZEItemType), (int (*)(const void*, const void*))(CompareFunction));
	if (Result == NULL)
	{
		Lock.UnlockRead();<
		return -1;
	}
	else
	{
		ZESize Output = ((ZEItemType*)Result - Items); 
		Lock.UnlockRead();
		return Output;
	}
}

ZE_ARRAY_TEMPLATE
ZEItemType& ZEArray<ZE_ARRAY_SPECIALIZATION>::operator[](ZESize Index)
{
	zeDebugCheck(Index >= Count, "Index is out of range.");
	return Items[Index];
}

ZE_ARRAY_TEMPLATE
const ZEItemType& ZEArray<ZE_ARRAY_SPECIALIZATION>::operator[](ZESize Index) const
{
	zeDebugCheck(Index >= Count, "Index is out of range.");
	return Items[Index];
}

ZE_ARRAY_TEMPLATE
ZE_ARRAY_OTHER_TEMPLATE
ZEArray<ZE_ARRAY_SPECIALIZATION> ZEArray<ZE_ARRAY_SPECIALIZATION>::operator+(const ZEArray<ZE_ARRAY_OTHER_SPEC>& OtherArray) const
{
	ZEArray Temp;

	Lock.LockRead();
	OtherArray.LockRead();
	{
		Temp.MassAdd(*this);
		Temp.MassAdd(OtherArray);
	}
	OtherArray.UnlockRead();
	Lock.UnlockRead();

	return Temp;
}

ZE_ARRAY_TEMPLATE
ZE_ARRAY_OTHER_TEMPLATE
ZEArray<ZE_ARRAY_SPECIALIZATION>& ZEArray<ZE_ARRAY_SPECIALIZATION>::operator+=(const ZEArray<ZE_ARRAY_OTHER_SPEC>& OtherArray)
{
	AddMultiple(OtherArray);
	return *this;
}

ZE_ARRAY_TEMPLATE
ZE_ARRAY_OTHER_TEMPLATE
bool ZEArray<ZE_ARRAY_SPECIALIZATION>::operator==(const ZEArray<ZE_ARRAY_OTHER_SPEC>& OtherArray) const
{
	Lock.LockRead();
	{
		if (Count != OtherArray.Count)
		{
			Lock.UnlockRead();
			return false;
		}

		for (ZESize I = 0; I < Count; I++)
		{
			if (Items[I] == OtherArray.Items[I])
				continue;

			Lock.UnlockRead();
			return false;
		}
	}
	Lock.UnlockRead();

	return true;
}

ZE_ARRAY_TEMPLATE
ZE_ARRAY_OTHER_TEMPLATE
bool ZEArray<ZE_ARRAY_SPECIALIZATION>::operator!=(const ZEArray<ZE_ARRAY_OTHER_SPEC>& OtherArray) const
{
	Lock.LockRead();
	{
		if (Count != OtherArray.Count)
		{
			Lock.UnlockRead();
			return true;
		}

		for (ZESize I = 0; I < Count; I++)
		{
			if (Items[I] != OtherArray.Items[I])
				continue;

			Lock.UnlockRead();
			return false;
		}
	}
	Lock.UnlockRead();

	return true;
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::LockRead() const
{
	Lock.LockRead();
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::UnlockRead() const
{
	Lock.UnlockRead();
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::LockWrite()
{
	Lock.LockWrite();
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::LockWriteNested()
{
	Lock.LockWriteNested();
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::UnlockWrite()
{
	Lock.UnlockWrite();
}

ZE_ARRAY_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::operator=(const ZEArray<ZE_ARRAY_SPECIALIZATION>& OtherArray)
{
	Copy(OtherArray);
}

ZE_ARRAY_TEMPLATE
ZE_ARRAY_OTHER_TEMPLATE
void ZEArray<ZE_ARRAY_SPECIALIZATION>::operator=(const ZEArray<ZE_ARRAY_OTHER_SPEC>& OtherArray)
{
	Copy(OtherArray);
}

ZE_ARRAY_TEMPLATE
ZEArray<ZE_ARRAY_SPECIALIZATION>::ZEArray()
{
	Items = NULL;
	Count = 0;
}

ZE_ARRAY_TEMPLATE
ZEArray<ZE_ARRAY_SPECIALIZATION>::ZEArray(ZESize Count)
{
	Items = NULL;
	this->Count = 0;
	SetCount(Count);
}

ZE_ARRAY_TEMPLATE
ZEArray<ZE_ARRAY_SPECIALIZATION>::ZEArray(const ZEArray<ZE_ARRAY_SPECIALIZATION>& OtherArray)
{
	Items = NULL;
	Count = 0;
	Copy(OtherArray);
}

ZE_ARRAY_TEMPLATE
ZE_ARRAY_OTHER_TEMPLATE
ZEArray<ZE_ARRAY_SPECIALIZATION>::ZEArray(const ZEArray<ZE_ARRAY_OTHER_SPEC>& OtherArray)
{
	Items = NULL;
	Count = 0;
	Copy(OtherArray);
}

ZE_ARRAY_TEMPLATE
ZEArray<ZE_ARRAY_SPECIALIZATION>::~ZEArray()
{
	Clear();
}

#pragma warning(pop)
