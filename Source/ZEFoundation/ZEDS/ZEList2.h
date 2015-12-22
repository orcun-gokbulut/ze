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

template<typename ZEItemType> class ZEList2;

template<typename ZEItemType>
class ZELink
{
	//ZE_DISALLOW_COPY(ZELink);
	friend class ZEList2<ZEItemType>;
	private:
		bool						InUse;
		ZEItemType*					Item;
		ZELink*						Prev;
		ZELink*						Next;

	public:
		bool						GetInUse() const;
		ZELink*						GetPrev();
		const ZELink*				GetPrev() const;
		ZELink*						GetNext();
		const ZELink*				GetNext() const;
		ZEItemType*					GetItem() const;

		void						InsertBefore(ZELink<ZEItemType>* Item);
		void						InsertAfter(ZELink<ZEItemType>* Item);
		void						Remove();

									ZELink();
									ZELink(ZEItemType* Object);
									~ZELink();
};

template<typename ZEItemType>
class ZEList2
{
	//ZE_DISALLOW_COPY(ZEList2)
	private:
		ZELink<ZEItemType>*			First;
		ZELink<ZEItemType>*			Last;
		ZESize						Count;

	public:
		ZESize						GetCount() const;

		ZELink<ZEItemType>*			GetFirst();
		const ZELink<ZEItemType>*	GetFirst() const;
		ZELink<ZEItemType>*			GetLast();
		const ZELink<ZEItemType>*	GetLast() const;

		ZEItemType*					GetItem(ZESize Index);
		ZELink<ZEItemType>*			GetLink(ZESize Index);
		const ZELink<ZEItemType>*	GetLink(ZESize Index) const;

		ZELink<ZEItemType>*			Find(const ZEItemType* Item, ZELink<ZEItemType>* StartLink = NULL);
		const ZELink<ZEItemType>*	Find(const ZEItemType* Item, const ZELink<ZEItemType>* StartLink = NULL) const;
		ZESSize						FindIndex(ZELink<ZEItemType>* Link) const;

		bool						Exists(ZELink<ZEItemType>* Link);

		void						AddBegin(ZELink<ZEItemType>* Link);
		void						AddEnd(ZELink<ZEItemType>* Link);
		
		void						InsertAfter(ZELink<ZEItemType>* Link, ZELink<ZEItemType>* NewLink);
		void						InsertBefore(ZELink<ZEItemType>* Link, ZELink<ZEItemType>* NewLink);
		
		void						Remove(ZELink<ZEItemType>* Link);
		void						Swap(ZELink<ZEItemType>* A, ZELink<ZEItemType>* B);


		void						Reverse();

		template<ZEInt CompareFunction(const ZEItemType*, const ZEItemType*)>
		void						Sort();

		void						Clean();

		ZEItemType*					operator[](ZESize Index) const;

									ZEList2();
									~ZEList2();
};

template<typename ZEItemType>
bool ZELink<ZEItemType>::GetInUse() const
{
	return InUse;
}


template<typename ZEItemType>
ZELink<ZEItemType>* ZELink<ZEItemType>::GetPrev()
{
	return Prev;
}

template<typename ZEItemType>
const ZELink<ZEItemType>* ZELink<ZEItemType>::GetPrev() const
{
	return Prev;
}

template<typename ZEItemType>
ZELink<ZEItemType>* ZELink<ZEItemType>::GetNext()
{
	return Next;
}

template<typename ZEItemType>
const ZELink<ZEItemType>* ZELink<ZEItemType>::GetNext() const
{
	return Next;
}

template<typename ZEItemType>
ZEItemType* ZELink<ZEItemType>::GetItem() const
{
	return Item;
}

template<typename ZEItemType>
ZELink<ZEItemType>::ZELink()
{
	InUse = false;
	Prev = NULL;
	Next = NULL;
	Item = NULL;
}

template<typename ZEItemType>
ZELink<ZEItemType>::ZELink(ZEItemType* Object)
{
	 InUse = false;
	 Prev = NULL;
	 Next = NULL;
	 this->Item = Object;
}

template<typename ZEItemType>
ZELink<ZEItemType>::~ZELink()
{
	zeDebugCheck(InUse, "Link has destructed before it it has been removed from the list.");
}

template<typename ZEItemType>
ZELink<ZEItemType>* ZEList2<ZEItemType>::GetFirst()
{
	return First;
}

template<typename ZEItemType>
const ZELink<ZEItemType>* ZEList2<ZEItemType>::GetFirst() const
{
	return First;
}

template<typename ZEItemType>
ZELink<ZEItemType>* ZEList2<ZEItemType>::GetLast()
{
	return Last;
}

template<typename ZEItemType>
const ZELink<ZEItemType>* ZEList2<ZEItemType>::GetLast() const
{
	return Last;
}


template<typename ZEItemType>
ZESize ZEList2<ZEItemType>::GetCount() const
{
	return Count;
}

template<typename ZEItemType>
ZELink<ZEItemType>* ZEList2<ZEItemType>::Find(const ZEItemType* Item, ZELink<ZEItemType>* StartLink)
{
	zeDebugCheck(StartLink != NULL && !Exists(StartLink), "Start link is not in this list.");

	ZELink<ZEItemType>* Cursor = First;
	if (StartLink != NULL)
		Cursor = StartLink;

	while(Cursor != NULL)
	{
		if (Cursor->GetItem() == Item)
			return Cursor;

		Cursor = Cursor->Next;
	}

	return NULL;
}

template<typename ZEItemType>
const ZELink<ZEItemType>* ZEList2<ZEItemType>::Find(const ZEItemType* Item, const ZELink<ZEItemType>* StartLink) const
{
	zeDebugCheck(StartLink != NULL && !Exists(StartLink), "Start link is not in this list.");

	const ZELink<ZEItemType>* Cursor = First;
	if (StartLink != NULL)
		Cursor = StartLink;

	while(Cursor != NULL)
	{
		if (Cursor->GetItem() == Item)
			return Cursor;

		Cursor = Cursor->Next;
	}

	return NULL;
}

template<typename ZEItemType>
ZESSize ZEList2<ZEItemType>::FindIndex(ZELink<ZEItemType>* Link) const
{
	if (!Link->InUse)
		return -1;

	ZESize Index = 0;
	const ZELink<ZEItemType>* Cursor = First;
	while(Cursor != NULL)
	{
		if (Cursor == Link)
			return Index;

		Cursor = Cursor->Next;
		Index++;
	}

	return -1;
}

template<typename ZEItemType>
bool ZEList2<ZEItemType>::Exists(ZELink<ZEItemType>* Link)
{
	return FindIndex(Link) != -1;
}

template<typename ZEItemType>
ZELink<ZEItemType>* ZEList2<ZEItemType>::GetLink(ZESize Index)
{
	zeDebugCheck(Index >= Count, "Index parameter is out of range.");
	ZELink<ZEItemType>* Cursor = First;
	while(Index != 0)
	{
		Cursor = Cursor->Next;
		Index--;
	}

	return Cursor;
}

template<typename ZEItemType>
const ZELink<ZEItemType>* ZEList2<ZEItemType>::GetLink(ZESize Index) const
{
	zeDebugCheck(Index >= Count, "Index parameter is out of range.");
	const ZELink<ZEItemType>* Cursor = First;
	while(Index != 0)
	{
		Cursor = Cursor->Next;
		Index--;
	}

	return Cursor;
}

template<typename ZEItemType>
ZEItemType* ZEList2<ZEItemType>::GetItem(ZESize Index)
{
	return GetLink(Index)->GetItem();
}

template<typename ZEItemType>
void ZEList2<ZEItemType>::AddBegin(ZELink<ZEItemType>* Link)
{
	zeDebugCheck(Link == NULL, "Link parameter cannot be null.");
	zeDebugCheck(Link->InUse, "Link already cointained in a list.");

	if (First == NULL)
	{
		Link->InUse = true;
		First = Link;
		Last = Link;
		Count++;
	}
	else
	{
		InsertBefore(First, Link);
	}
}

template<typename ZEItemType>
void ZEList2<ZEItemType>::AddEnd(ZELink<ZEItemType>* Link)
{
	zeDebugCheck(Link == NULL, "Link parameter cannot be null.");
	zeDebugCheck(Link->InUse, "Link already cointained in a list.");

	if (Last == NULL)
	{
		Link->InUse = true;
		First = Link;
		Last = Link;
		Count++;
	}
	else
	{
		InsertAfter(Last, Link);
	}
}

template<typename ZEItemType>
void ZEList2<ZEItemType>::InsertBefore(ZELink<ZEItemType>* Link, ZELink<ZEItemType>* NewLink)
{
	zeDebugCheck(Link == NULL, "Link parameter cannot be null.");
	zeDebugCheck(!Exists(Link), "Link parameter is not in this list.");
	zeDebugCheck(NewLink == NULL, "NewLink parameter cannot be null.");
	zeDebugCheck(NewLink->InUse, "NewLink already cointained in a list.");

	NewLink->Prev = Link->Prev;
	NewLink->Next = Link;
	NewLink->InUse = true;

	if (Link->Prev == NULL)
		First = NewLink;
	else
		Link->Prev->Next = NewLink;

	Link->Prev = NewLink;

	Count++;
}

template<typename ZEItemType>
void ZEList2<ZEItemType>::InsertAfter(ZELink<ZEItemType>* Link, ZELink<ZEItemType>* NewLink)
{
	zeDebugCheck(Link == NULL, "Link parameter cannot be null.");
	zeDebugCheck(!Exists(Link), "Link parameter is not in this list.");
	zeDebugCheck(NewLink == NULL, "NewLink parameter cannot be null.");
	zeDebugCheck(NewLink->InUse, "NewLink already cointained in a list.");

	NewLink->Prev = Link;
	NewLink->Next = Link->Next;
	NewLink->InUse = true;

	if (Link->Next == NULL)
		Last = NewLink;
	else
		Link->Next->Prev = NewLink;

	Link->Next = NewLink;

	Count++;
}

template<typename ZEItemType>
void ZEList2<ZEItemType>::Remove(ZELink<ZEItemType>* Link)
{
	zeDebugCheck(Link == NULL, "Link parameter cannot be null.");
	zeDebugCheck(!Exists(Link), "Link is not in this list.");

	if (Link->Prev == NULL)
		First = Link->Next;
	else
		Link->Prev->Next = Link->Next;

	if (Link->Next == NULL)
		Last = Link->Prev;
	else
		Link->Next->Prev = Link->Prev;

	Link->InUse = false;
	Link->Prev = NULL;
	Link->Next = NULL;

	Count--;
}

template<typename ZEItemType>
void ZEList2<ZEItemType>::Swap(ZELink<ZEItemType>* A, ZELink<ZEItemType>* B)
{
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
}


template<typename ZEItemType>
inline void ZEList2<ZEItemType>::Clean()
{
	ZELink<ZEItemType>* Link = First;
	while(Link != NULL)
	{
		ZELink<ZEItemType>* Temp = Link->Next;
		Link->Prev = NULL;
		Link->Next = NULL;
		Link->InUse = false;
		Link = Temp;
	}

	First = NULL;
	Last = NULL;
	Count = 0;
}

template<typename ZEItemType>
void ZEList2<ZEItemType>::Reverse()
{
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
}

template<typename ZEItemType>
ZEItemType* ZEList2<ZEItemType>::operator[](ZESize Index) const
{
	return GetItem(Index);
}

template<typename ZEItemType>
ZEList2<ZEItemType>::ZEList2()
{
	First = NULL;
	Last = NULL;
	Count = 0;
}

template<typename ZEItemType>
ZEList2<ZEItemType>::~ZEList2()
{
	Clean();
}

template<typename ZEItemType>
template<ZEInt CompareFunction(const ZEItemType*, const ZEItemType*)>
void ZEList2<ZEItemType>::Sort()
{
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
}
