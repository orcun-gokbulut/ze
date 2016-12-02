//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEList2.h
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

#include "ZECommon.h"
#include "ZETypes.h"
#include "ZEError.h"
#include "ZELink.h"
#include "ZEList2Iterator.h"
#include "ZEThread/ZELockRW.h"

#define ZE_LIST_TEMPLATE template<typename ZEItemType, typename ZELockType>
#define ZE_LIST_SPECIALIZATION ZEItemType, ZELockType

template<typename ZEItemType, typename ZELockType = ZELockRW>
class ZEList2
{
	private:
		ZELink<ZEItemType>*					First;
		ZELink<ZEItemType>*					Last;
		ZESize								Count;
		mutable ZELockType					Lock;

	public:
		ZEList2Iterator<ZE_LIST_SPECIALIZATION>			GetIterator();
		ZEList2Iterator<ZE_LIST_SPECIALIZATION>			GetIterator(ZELink<ZEItemType>* Item);
		ZEList2IteratorConst<ZE_LIST_SPECIALIZATION>	GetIterator() const;
		ZEList2IteratorConst<ZE_LIST_SPECIALIZATION>	GetIterator(const ZELink<ZEItemType>* Item) const;
		ZEList2Iterator<ZE_LIST_SPECIALIZATION>			GetIteratorEnd();
		ZEList2IteratorConst<ZE_LIST_SPECIALIZATION>	GetIteratorEnd() const;

		ZESize								GetCount() const;

		ZELink<ZEItemType>*					GetFirst();
		const ZELink<ZEItemType>*			GetFirst() const;
		ZELink<ZEItemType>*					GetLast();
		const ZELink<ZEItemType>*			GetLast() const;

		ZEItemType*							GetItem(ZESize Index) const;
		ZELink<ZEItemType>*					GetLink(ZESize Index);
		const ZELink<ZEItemType>*			GetLink(ZESize Index) const;

		ZELink<ZEItemType>*					Find(const ZEItemType* Item, const ZELink<ZEItemType>* StartLink = NULL);
		const ZELink<ZEItemType>*			Find(const ZEItemType* Item, const ZELink<ZEItemType>* StartLink = NULL) const;

		ZESSize								FindIndex(const ZEItemType* Item) const;
		ZESSize								FindIndex(const ZELink<ZEItemType>* Link) const;

		bool								Exists(const ZEItemType* Item) const;
		bool								Exists(const ZELink<ZEItemType>* Link) const;

		void								AddBegin(ZELink<ZEItemType>* Link);
		void								AddEnd(ZELink<ZEItemType>* Link);
		
		void								InsertAfter(ZELink<ZEItemType>* Link, ZELink<ZEItemType>* NewLink);
		void								InsertBefore(ZELink<ZEItemType>* Link, ZELink<ZEItemType>* NewLink);
		
		void								Remove(ZELink<ZEItemType>* Link);
		void								Swap(ZELink<ZEItemType>* A, ZELink<ZEItemType>* B);

		template<typename ZELockTypeOther>
		void								MergeBegin(ZEList2<ZEItemType, ZELockTypeOther>& OtherList);
		template<typename ZELockTypeOther>
		void								MergeEnd(ZEList2<ZEItemType, ZELockTypeOther>& OtherList);

		void								Reverse();
		template<ZEInt CompareFunction(const ZEItemType*, const ZEItemType*)>
		void								Sort();
		void								Clear();

		void								LockRead() const;
		void								UnlockRead() const;

		void								LockWrite();
		void								UnlockWrite();

		const ZEList2<const ZEItemType>&	ToInspector() const;

		ZEItemType*							operator[](ZESize Index) const;

											//ZE_COPY_NO_ACTION								
											template<typename ZELockTypeOther>
											ZEList2& operator=(const ZEList2<ZEItemType, ZELockTypeOther>& Other);
											template<typename ZELockTypeOther>
											ZEList2(const ZEList2<ZEItemType, ZELockTypeOther>& Other);

											ZEList2();
											~ZEList2();
};


// ZEList2
/////////////////////////////////////////////////////////////////////////////

ZE_LIST_TEMPLATE
ZEList2Iterator<ZE_LIST_SPECIALIZATION> ZEList2<ZE_LIST_SPECIALIZATION>::GetIterator()
{
	return ZEList2Iterator<ZE_LIST_SPECIALIZATION>(GetFirst());
}

ZE_LIST_TEMPLATE
ZEList2Iterator<ZE_LIST_SPECIALIZATION> ZEList2<ZE_LIST_SPECIALIZATION>::GetIterator(ZELink<ZEItemType>* Item)
{
	return ZEList2Iterator<ZE_LIST_SPECIALIZATION>(Item);
}

ZE_LIST_TEMPLATE
ZEList2IteratorConst<ZE_LIST_SPECIALIZATION> ZEList2<ZE_LIST_SPECIALIZATION>::GetIterator() const
{
	return ZEList2IteratorConst<ZE_LIST_SPECIALIZATION>(GetFirst());
}

ZE_LIST_TEMPLATE
ZEList2IteratorConst<ZE_LIST_SPECIALIZATION> ZEList2<ZE_LIST_SPECIALIZATION>::GetIterator(const ZELink<ZEItemType>* Item) const
{
	return ZEList2IteratorConst<ZE_LIST_SPECIALIZATION>(Item);
}

ZE_LIST_TEMPLATE
ZEList2Iterator<ZE_LIST_SPECIALIZATION> ZEList2<ZE_LIST_SPECIALIZATION>::GetIteratorEnd()
{
	return ZEList2Iterator<ZE_LIST_SPECIALIZATION>(GetLast());
}

ZE_LIST_TEMPLATE
ZEList2IteratorConst<ZE_LIST_SPECIALIZATION> ZEList2<ZE_LIST_SPECIALIZATION>::GetIteratorEnd() const
{
	return ZEList2IteratorConst<ZE_LIST_SPECIALIZATION>(GetLast());
}

ZE_LIST_TEMPLATE
ZELink<ZEItemType>* ZEList2<ZE_LIST_SPECIALIZATION>::GetFirst()
{
	return First;
}

ZE_LIST_TEMPLATE
const ZELink<ZEItemType>* ZEList2<ZE_LIST_SPECIALIZATION>::GetFirst() const
{
	return First;
}

ZE_LIST_TEMPLATE
ZELink<ZEItemType>* ZEList2<ZE_LIST_SPECIALIZATION>::GetLast()
{
	return Last;
}

ZE_LIST_TEMPLATE
const ZELink<ZEItemType>* ZEList2<ZE_LIST_SPECIALIZATION>::GetLast() const
{
	return Last;
}


ZE_LIST_TEMPLATE
ZESize ZEList2<ZE_LIST_SPECIALIZATION>::GetCount() const
{
	return Count;
}

ZE_LIST_TEMPLATE
ZELink<ZEItemType>* ZEList2<ZE_LIST_SPECIALIZATION>::Find(const ZEItemType* Item, const ZELink<ZEItemType>* StartLink)
{
	Lock.LockRead();

	zeDebugCheck(StartLink != NULL && !Exists(StartLink), "Start link is not in this list.");

	ZELink<ZEItemType>* Cursor = First;
	if (StartLink != NULL)
		Cursor = StartLink;

	while(Cursor != NULL)
	{
		if (Cursor->GetItem() == Item)
		{
			Lock.UnlockRead();
			return Cursor;
		}

		Cursor = Cursor->Next;
	}

	Lock.UnlockRead();
	return NULL;
}

ZE_LIST_TEMPLATE
const ZELink<ZEItemType>* ZEList2<ZE_LIST_SPECIALIZATION>::Find(const ZEItemType* Item, const ZELink<ZEItemType>* StartLink) const
{
	Lock.LockRead();

	zeDebugCheck(StartLink != NULL && !Exists(StartLink), "Start link is not in this list.");

	const ZELink<ZEItemType>* Cursor = First;
	if (StartLink != NULL)
		Cursor = StartLink;

	while(Cursor != NULL)
	{
		if (Cursor->GetItem() == Item)
		{
			Lock.UnlockRead();
			return Cursor;
		}

		Cursor = Cursor->Next;
	}

	Lock.UnlockRead();
	return NULL;
}

ZE_LIST_TEMPLATE
ZESSize ZEList2<ZE_LIST_SPECIALIZATION>::FindIndex(const ZELink<ZEItemType>* Link) const
{
	Lock.LockRead();

	if (!Link->InUse)
	{
		Lock.UnlockRead();
		return -1;
	}

	ZESize Index = 0;
	const ZELink<ZEItemType>* Cursor = First;
	while(Cursor != NULL)
	{
		if (Cursor == Link)
		{
			Lock.UnlockRead();
			return Index;
		}

		Cursor = Cursor->Next;
		Index++;
	}

	Lock.UnlockRead();
	return -1;
}

ZE_LIST_TEMPLATE
ZESSize ZEList2<ZE_LIST_SPECIALIZATION>::FindIndex(const ZEItemType* Item) const
{
	Lock.LockRead();

	if (Item == NULL)
	{
		Lock.UnlockRead();
		return -1;
	}

	ZESize Index = 0;
	const ZELink<ZEItemType>* Cursor = First;
	while(Cursor != NULL)
	{
		if (Cursor->GetItem() == Item)
		{
			Lock.UnlockRead();
			return Index;
		}

		Cursor = Cursor->Next;
		Index++;
	}

	Lock.UnlockRead();
	return -1;
}

ZE_LIST_TEMPLATE
bool ZEList2<ZE_LIST_SPECIALIZATION>::Exists(const ZEItemType* Item) const
{
	return FindIndex(Item) != -1;
}

ZE_LIST_TEMPLATE
bool ZEList2<ZE_LIST_SPECIALIZATION>::Exists(const ZELink<ZEItemType>* Link) const
{
	return FindIndex(Link) != -1;
}

ZE_LIST_TEMPLATE
ZELink<ZEItemType>* ZEList2<ZE_LIST_SPECIALIZATION>::GetLink(ZESize Index)
{
	Lock.LockRead();

	zeDebugCheck(Index >= Count, "Index parameter is out of range.");
	ZELink<ZEItemType>* Cursor = First;
	while(Index != 0)
	{
		Cursor = Cursor->Next;
		Index--;
	}

	Lock.UnlockRead();
	return Cursor;
}

ZE_LIST_TEMPLATE
const ZELink<ZEItemType>* ZEList2<ZE_LIST_SPECIALIZATION>::GetLink(ZESize Index) const
{
	Lock.LockRead();

	zeDebugCheck(Index >= Count, "Index parameter is out of range.");
	const ZELink<ZEItemType>* Cursor = First;
	while(Index != 0)
	{
		Cursor = Cursor->Next;
		Index--;
	}

	Lock.UnlockRead();
	return Cursor;
}

ZE_LIST_TEMPLATE
ZEItemType* ZEList2<ZE_LIST_SPECIALIZATION>::GetItem(ZESize Index) const
{
	return GetLink(Index)->GetItem();
}

ZE_LIST_TEMPLATE
void ZEList2<ZE_LIST_SPECIALIZATION>::AddBegin(ZELink<ZEItemType>* Link)
{
	Lock.LockWriteNested();

	zeDebugCheck(Link == NULL, "Link parameter cannot be null.");
	zeDebugCheck(Link->InUse, "Link already cointained in a list.");
	zeDebugCheck(Link->List != NULL, "Link already cointained in a list.");

	if (First == NULL)
	{
		First = Link;
		Last = Link;
		Link->InUse = true;
		#ifdef ZE_DEBUG_ENABLE
		Link->List = this;
		#endif
		Count++;
	}
	else
	{
		InsertBefore(First, Link);
	}

	Lock.UnlockWrite();
}

ZE_LIST_TEMPLATE
void ZEList2<ZE_LIST_SPECIALIZATION>::AddEnd(ZELink<ZEItemType>* Link)
{
	Lock.LockWriteNested();

	zeDebugCheck(Link == NULL, "Link parameter cannot be null.");
	zeDebugCheck(Link->InUse, "Link already cointained in a list.");
	zeDebugCheck(Link->List != NULL, "Link already cointained in a list.");

	if (Last == NULL)
	{
		First = Link;
		Last = Link;
		Link->InUse = true;
		#ifdef ZE_DEBUG_ENABLE
		Link->List = this;
		#endif

		Count++;
	}
	else
	{
		InsertAfter(Last, Link);
	}

	Lock.UnlockWrite();
}

ZE_LIST_TEMPLATE
void ZEList2<ZE_LIST_SPECIALIZATION>::InsertBefore(ZELink<ZEItemType>* Link, ZELink<ZEItemType>* NewLink)
{
	Lock.LockWriteNested();

	zeDebugCheck(Link == NULL, "Link parameter cannot be null.");
	zeDebugCheck(Link->List != this, "Link is not contained in this list.");
	zeDebugCheck(NewLink == NULL, "NewLink parameter cannot be null.");
	zeDebugCheck(NewLink->InUse, "NewLink already cointained in a list.");
	zeDebugCheck(NewLink->List != NULL, "NewLink already cointained in a list.");
	
	NewLink->Prev = Link->Prev;
	NewLink->Next = Link;
	NewLink->InUse = true;
	#ifdef ZE_DEBUG_ENABLE
	NewLink->List = this;
	#endif

	if (Link->Prev == NULL)
		First = NewLink;
	else
		Link->Prev->Next = NewLink;

	Link->Prev = NewLink;

	Count++;

	Lock.UnlockWrite();
}

ZE_LIST_TEMPLATE
void ZEList2<ZE_LIST_SPECIALIZATION>::InsertAfter(ZELink<ZEItemType>* Link, ZELink<ZEItemType>* NewLink)
{
	Lock.LockWriteNested();

	zeDebugCheck(Link == NULL, "Link parameter cannot be null.");
	zeDebugCheck(Link->List != this, "Link is not contained in this list.");
	zeDebugCheck(NewLink == NULL, "NewLink parameter cannot be null.");
	zeDebugCheck(NewLink->InUse, "NewLink already cointained in a list.");
	zeDebugCheck(NewLink->List != NULL, "NewLink already cointained in a list.");

	NewLink->Prev = Link;
	NewLink->Next = Link->Next;
	NewLink->InUse = true;
	#ifdef ZE_DEBUG_ENABLE
	NewLink->List = this;
	#endif

	if (Link->Next == NULL)
		Last = NewLink;
	else
		Link->Next->Prev = NewLink;

	Link->Next = NewLink;

	Count++;

	Lock.UnlockWrite();
}

ZE_LIST_TEMPLATE
void ZEList2<ZE_LIST_SPECIALIZATION>::Remove(ZELink<ZEItemType>* Link)
{
	Lock.LockWriteNested();

	zeDebugCheck(Link == NULL, "Link parameter cannot be null.");
	zeDebugCheck(Link->List != this, "Link is not contained in this list.");

	if (Link->Prev == NULL)
		First = Link->Next;
	else
		Link->Prev->Next = Link->Next;

	if (Link->Next == NULL)
		Last = Link->Prev;
	else
		Link->Next->Prev = Link->Prev;

	Link->Prev = NULL;
	Link->Next = NULL;
	Link->InUse = false;
	#ifdef ZE_DEBUG_ENABLE
	Link->List = NULL;
	#endif

	Count--;

	Lock.UnlockWrite();
}

ZE_LIST_TEMPLATE
void ZEList2<ZE_LIST_SPECIALIZATION>::Swap(ZELink<ZEItemType>* A, ZELink<ZEItemType>* B)
{
	Lock.LockWriteNested();

	zeDebugCheck(A == NULL, "A parameter cannot be null.");
	zeDebugCheck(!Exists(A), "A link is not in this list.");

	zeDebugCheck(B == NULL, "B parameter cannot be null.");
	zeDebugCheck(!Exists(B), "B link is not in this list.");

	ZELink<ZEItemType>* Temp;
	Temp = A->Prev;
	A->Prev = B->Prev;
	B->Prev = Temp;

	Temp = A->Next;
	A->Next = B->Next;
	B->Next = Temp;

	if (First == A)
		First = B;
	else if (First == B)
		First = A;

	if (Last == A)
		Last = B;
	else if (Last == B)
		Last = A;

	Lock.UnlockWrite();
}

ZE_LIST_TEMPLATE
template<typename ZELockTypeOther>
void ZEList2<ZE_LIST_SPECIALIZATION>::MergeBegin(ZEList2<ZEItemType, ZELockTypeOther>& OtherList)
{
	Lock.LockWriteNested();

	if (OtherList.GetFirst() == NULL)
	{
		Lock.UnlockWrite();
		return;
	}

	#ifdef ZE_DEBUG_ENABLE
		ZELink<ZEItemType>* Link = OtherList.GetFirst();
		while(Link != NULL)
		{
			Link->List = this;
			Link = Link->Next;	
		}
	#endif

	if (First != NULL)
	{
		First->Prev = OtherList.Last;
		OtherList.Last->Next = First;
		First = OtherList.First;
	}
	else
	{
		First = OtherList.First;
		Last = OtherList.Last;
	}

	Count += OtherList.Count;
	
	OtherList.First = NULL;
	OtherList.Last = NULL;
	OtherList.Count = 0;

	Lock.UnlockWrite();
}

ZE_LIST_TEMPLATE
template<typename ZELockTypeOther>
void ZEList2<ZE_LIST_SPECIALIZATION>::MergeEnd(ZEList2<ZEItemType, ZELockTypeOther>& OtherList)
{
	Lock.LockWriteNested();

	if (OtherList.GetLast() == NULL)
	{
		Lock.UnlockWrite();
		return;
	}

	#ifdef ZE_DEBUG_ENABLE
		ZELink<ZEItemType>* Link = OtherList.GetFirst();
		while(Link != NULL)
		{
			Link->List = this;
			Link = Link->Next;	
		}
	#endif

	if (Last != NULL)
	{
		Last->Next = OtherList.First;
		OtherList.First->Prev = Last;
		Last = OtherList.Last;
	}
	else
	{
		First = OtherList.First;
		Last = OtherList.Last;
	}

	Count += OtherList.Count;
	
	OtherList.First = NULL;
	OtherList.Last = NULL;
	OtherList.Count = 0;

	Lock.UnlockWrite();
}

ZE_LIST_TEMPLATE
inline void ZEList2<ZE_LIST_SPECIALIZATION>::Clear()
{
	Lock.LockWriteNested();

	ZELink<ZEItemType>* Link = First;
	while(Link != NULL)
	{
		ZELink<ZEItemType>* Temp = Link->Next;
		Link->Prev = NULL;
		Link->Next = NULL;
		Link->InUse = false;
		#ifdef ZE_DEBUG_ENABLE
		Link->List = NULL;
		#endif
		Link = Temp;
	}

	First = NULL;
	Last = NULL;
	Count = 0;

	Lock.UnlockWrite();
}

ZE_LIST_TEMPLATE
void ZEList2<ZE_LIST_SPECIALIZATION>::LockRead() const
{
	Lock.LockRead();
}

ZE_LIST_TEMPLATE
void ZEList2<ZE_LIST_SPECIALIZATION>::UnlockRead() const
{
	Lock.UnlockRead();
}

ZE_LIST_TEMPLATE
void ZEList2<ZE_LIST_SPECIALIZATION>::LockWrite()
{
	Lock.LockWrite();
}

ZE_LIST_TEMPLATE
void ZEList2<ZE_LIST_SPECIALIZATION>::UnlockWrite()
{
	Lock.UnlockWrite();
}

ZE_LIST_TEMPLATE
const ZEList2<const ZEItemType>& ZEList2<ZE_LIST_SPECIALIZATION>::ToInspector() const
{
	return *reinterpret_cast<const ZEList2<const ZEItemType>*>(this);
}

ZE_LIST_TEMPLATE
void ZEList2<ZE_LIST_SPECIALIZATION>::Reverse()
{
	Lock.LockWriteNested();

	ZELink<ZEItemType>* Temp;

	ZELink<ZEItemType>* Link = First;
	while(Link != NULL)
	{
		Temp = Link->Prev;
		Link->Prev = Link->Next;
		Link->Next = Temp;

		Link = Link->Prev;
	}

	Temp = First;
	First = Last;
	Last = Temp;

	Lock.UnlockWrite();
}

ZE_LIST_TEMPLATE
ZEItemType* ZEList2<ZE_LIST_SPECIALIZATION>::operator[](ZESize Index) const
{
	return GetItem(Index);
}

ZE_LIST_TEMPLATE
template<typename ZELockTypeOther>
ZEList2<ZE_LIST_SPECIALIZATION>& ZEList2<ZE_LIST_SPECIALIZATION>::operator=(const ZEList2<ZEItemType, ZELockTypeOther>& Other)
{
	return *this;
}

ZE_LIST_TEMPLATE
template<typename ZELockTypeOther>
ZEList2<ZE_LIST_SPECIALIZATION>::ZEList2(const ZEList2<ZEItemType, ZELockTypeOther>& Other)
{
	new (this)ZEList2<ZE_LIST_SPECIALIZATION>();
}

ZE_LIST_TEMPLATE
ZEList2<ZE_LIST_SPECIALIZATION>::ZEList2()
{
	First = NULL;
	Last = NULL;
	Count = 0;
}

ZE_LIST_TEMPLATE
ZEList2<ZE_LIST_SPECIALIZATION>::~ZEList2()
{
	Clear();
}

ZE_LIST_TEMPLATE
template<ZEInt CompareFunction(const ZEItemType*, const ZEItemType*)>
void ZEList2<ZE_LIST_SPECIALIZATION>::Sort()
{
	Lock.LockWriteNested();

	// Source: ideasman42/linked_list_sort.c - https://gist.github.com/ideasman42/5921b0edfc6aa41a9ce0
	int BlockSize = 1, BlockCount;
	do 
	{
		ZELink<ZEItemType>* Left = First;
		ZELink<ZEItemType>* Right = First;

		First = NULL;
		Last = First;

		BlockCount = 0;
		while (Left != NULL) 
		{
			ZESize LeftSize;
			ZESize RightSize = BlockSize;

			BlockCount++;
			for (LeftSize = 0; (LeftSize < BlockSize) && (Right != NULL); LeftSize++) 
				Right = Right->Next;

			bool LeftEmpty = (LeftSize == 0);
			bool RightEmpty = (RightSize == 0 || Right == NULL);
			while (!LeftEmpty || !RightEmpty) 
			{
				ZELink<ZEItemType>* Selected;
				if (RightEmpty || (!LeftEmpty && CompareFunction(Left->GetItem(), Right->GetItem()) <= 0)) 
				{
					Selected = Left;
					Left = Left->Next;
					LeftSize--;
					LeftEmpty = (LeftSize == 0);
				}
				else 
				{
					Selected = Right;
					Right = Right->Next;
					RightSize--;
					RightEmpty = (RightSize == 0) || (Right == NULL);
				}

				if (Last != NULL)
					Last->Next = Selected;
				else
					First = Selected;
				Selected->Prev = Last;
				Last = Selected;
			}

			Left = Right;
		}

		if (Last != NULL)
			Last->Next = NULL;

		BlockSize <<= 1;
	} 
	while (BlockCount > 1);

	Lock.UnlockWrite();
}
