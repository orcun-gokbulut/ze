//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEArrayIterator.h
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
#include "ZEIterator.h"

template<typename ZEItemType, typename ZEAllocatorType>
class ZEArray;

template<typename ZEItemType, typename ZEAllocatorType>
class ZEArrayIterator : public ZEIterator<ZEItemType>
{
	friend class ZEArray<ZEItemType, ZEAllocatorType>;
	private:
		ZEArray<ZEItemType, ZEAllocatorType>* Array;
		ZESize						Index;

									ZEArrayIterator(ZEArray<ZEItemType, ZEAllocatorType>& Array, ZESize Index);

	public:
		inline bool					IsBegin() const;
		inline bool					IsEnd() const;

		inline ZEItemType&			GetItem() const;
		inline ZESize				GetIndex() const;

		inline void					Prev();
		inline void					Next();

		bool						operator==(const ZEArrayIterator& Iterator) const;

		ZEArrayIterator&			operator--();
		ZEArrayIterator&			operator++();

		ZEItemType&					operator*() const;
		ZEItemType*					operator->() const;
};

template<typename ZEItemType, typename ZEAllocatorType>
class ZEArrayIteratorConst : public ZEIteratorConst<ZEItemType>
{
	friend class ZEArray<ZEItemType, ZEAllocatorType>;
	private:
		const ZEArray<ZEItemType, ZEAllocatorType>* Array;
		ZESize						Index;

									ZEArrayIteratorConst(const ZEArray<ZEItemType, ZEAllocatorType>& Array, ZESize Index);

	public:
		inline bool					IsBegin() const;
		inline bool					IsEnd() const;

		inline const ZEItemType&	GetItem() const;
		inline ZESize				GetIndex() const;

		inline void					Prev();
		inline void					Next();

		bool						operator==(const ZEArrayIteratorConst& Iterator) const;

		ZEArrayIteratorConst&		operator--();
		ZEArrayIteratorConst&		operator++();
		const ZEItemType&			operator*() const;
		const ZEItemType*			operator->() const;
};


// ZEArrayIterator
/////////////////////////////////////////////////////////////////////////////

template<typename ZEItemType, typename ZEAllocatorType>
ZEArrayIterator<ZEItemType, ZEAllocatorType>::ZEArrayIterator(ZEArray<ZEItemType, ZEAllocatorType>& Array, ZESize Index)
{
	this->Array = &Array;
	this->Index = Index;
}

template<typename ZEItemType, typename ZEAllocatorType>
bool ZEArrayIterator<ZEItemType, ZEAllocatorType>::IsBegin() const
{
	return Index == 0;
}

template<typename ZEItemType, typename ZEAllocatorType>
bool ZEArrayIterator<ZEItemType, ZEAllocatorType>::IsEnd() const
{
	return (ZESSize)Index >= (ZESSize)Array->GetCount();
} 

template<typename ZEItemType, typename ZEAllocatorType>
ZEItemType& ZEArrayIterator<ZEItemType, ZEAllocatorType>::GetItem() const
{
	return Array->GetItem(Index);
}

template<typename ZEItemType, typename ZEAllocatorType>
ZESize ZEArrayIterator<ZEItemType, ZEAllocatorType>::GetIndex() const
{
	return Index;
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArrayIterator<ZEItemType, ZEAllocatorType>::Prev()
{
	if (Index == 0)
		return;

	Index--;
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArrayIterator<ZEItemType, ZEAllocatorType>::Next()
{	
	if ((ZESSize)Index >= (ZESSize)Array->GetCount())
		return;
	
	Index++;
}

template<typename ZEItemType, typename ZEAllocatorType>
bool ZEArrayIterator<ZEItemType, ZEAllocatorType>::operator==(const ZEArrayIterator& Iterator) const
{
	return (Iterator.Array == Array && Iterator.Index == Index);
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEArrayIterator<ZEItemType, ZEAllocatorType>& ZEArrayIterator<ZEItemType, ZEAllocatorType>::operator--()
{
	Prev();
	return *this;
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEArrayIterator<ZEItemType, ZEAllocatorType>& ZEArrayIterator<ZEItemType, ZEAllocatorType>::operator++()
{
	Next();
	return *this;
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEItemType& ZEArrayIterator<ZEItemType, ZEAllocatorType>::operator*() const
{
	return GetItem();
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEItemType* ZEArrayIterator<ZEItemType, ZEAllocatorType>::operator->() const
{
	return &GetItem();
}


// ZEArrayIteratorConst
/////////////////////////////////////////////////////////////////////////////

template<typename ZEItemType, typename ZEAllocatorType>
ZEArrayIteratorConst<ZEItemType, ZEAllocatorType>::ZEArrayIteratorConst(const ZEArray<ZEItemType, ZEAllocatorType>& Array, ZESize Index)
{
	this->Array = &Array;
	this->Index = Index;
}

template<typename ZEItemType, typename ZEAllocatorType>
bool ZEArrayIteratorConst<ZEItemType, ZEAllocatorType>::IsBegin() const
{
	return Index == 0;
}

template<typename ZEItemType, typename ZEAllocatorType>
bool ZEArrayIteratorConst<ZEItemType, ZEAllocatorType>::IsEnd() const
{
	return (ZESSize)Index >= (ZESSize)Array->GetCount();
} 

template<typename ZEItemType, typename ZEAllocatorType>
const ZEItemType& ZEArrayIteratorConst<ZEItemType, ZEAllocatorType>::GetItem() const
{
	return Array->GetItem(Index);
}

template<typename ZEItemType, typename ZEAllocatorType>
ZESize ZEArrayIteratorConst<ZEItemType, ZEAllocatorType>::GetIndex() const
{
	return Index;
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArrayIteratorConst<ZEItemType, ZEAllocatorType>::Prev()
{
	if (Index == 0)
		return;

	Index--;
}

template<typename ZEItemType, typename ZEAllocatorType>
void ZEArrayIteratorConst<ZEItemType, ZEAllocatorType>::Next()
{	
	if ((ZESSize)Index >= (ZESSize)Array->GetCount())
		return;

	Index++;
}

template<typename ZEItemType, typename ZEAllocatorType>
bool ZEArrayIteratorConst<ZEItemType, ZEAllocatorType>::operator==(const ZEArrayIteratorConst& Iterator) const
{
	return (Iterator.Array == Array && Iterator.Index == Index);
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEArrayIteratorConst<ZEItemType, ZEAllocatorType>& ZEArrayIteratorConst<ZEItemType, ZEAllocatorType>::operator--()
{
	Prev();
	return *this;
}

template<typename ZEItemType, typename ZEAllocatorType>
ZEArrayIteratorConst<ZEItemType, ZEAllocatorType>& ZEArrayIteratorConst<ZEItemType, ZEAllocatorType>::operator++()
{
	Next();
	return *this;
}

template<typename ZEItemType, typename ZEAllocatorType>
const ZEItemType& ZEArrayIteratorConst<ZEItemType, ZEAllocatorType>::operator*() const
{
	return GetItem();
}

template<typename ZEItemType, typename ZEAllocatorType>
const ZEItemType* ZEArrayIteratorConst<ZEItemType, ZEAllocatorType>::operator->() const
{
	return &GetItem();
}
