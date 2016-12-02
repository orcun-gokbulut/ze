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

#define ZE_ARRAY_ITER_TEMPLATE template<typename ZEItemType, typename ZEAllocatorType, typename ZELockType>
#define ZE_ARRAY_ITER_SPEC ZEItemType, ZEAllocatorType, ZELockType

template<typename ZEItemType, typename ZEAllocatorType, typename ZELockType>
class ZEArray;

ZE_ARRAY_ITER_TEMPLATE
class ZEArrayIterator : public ZEIterator<ZEItemType>
{
	friend class ZEArray<ZEItemType, ZEAllocatorType, ZELockType>;
	private:
		ZEArray<ZE_ARRAY_ITER_SPEC>*		Array;
		ZESSize								Index;

											ZEArrayIterator(ZEArray<ZE_ARRAY_ITER_SPEC>& Array, ZESize Index);

	public:
		inline bool							IsValid() const;

		inline ZEItemType&					GetItem() const;
		inline ZESize						GetIndex() const;

		inline void							Prev();
		inline void							Next();

		bool								operator==(const ZEArrayIterator& Iterator) const;

		ZEArrayIterator&					operator--();
		ZEArrayIterator&					operator++();

		ZEItemType&							operator*() const;
		ZEItemType*							operator->() const;
};

ZE_ARRAY_ITER_TEMPLATE
class ZEArrayIteratorConst : public ZEIteratorConst<ZEItemType>
{
	friend class ZEArray<ZEItemType, ZEAllocatorType, ZELockType>;
	private:
		const ZEArray<ZE_ARRAY_ITER_SPEC>*	Array;
		ZESSize								Index;

											ZEArrayIteratorConst(const ZEArray<ZE_ARRAY_ITER_SPEC>& Array, ZESize Index);

	public:
		inline bool							IsValid() const;

		inline const ZEItemType&			GetItem() const;
		inline ZESize						GetIndex() const;

		inline void							Prev();
		inline void							Next();

		bool								operator==(const ZEArrayIteratorConst& Iterator) const;

		ZEArrayIteratorConst&				operator--();
		ZEArrayIteratorConst&				operator++();
		const ZEItemType&					operator*() const;
		const ZEItemType*					operator->() const;
};


// ZEArrayIterator
/////////////////////////////////////////////////////////////////////////////

ZE_ARRAY_ITER_TEMPLATE
ZEArrayIterator<ZE_ARRAY_ITER_SPEC>::ZEArrayIterator(ZEArray<ZE_ARRAY_ITER_SPEC>& Array, ZESize Index)
{
	this->Array = &Array;
	this->Index = Index;
}

ZE_ARRAY_ITER_TEMPLATE
bool ZEArrayIterator<ZE_ARRAY_ITER_SPEC>::IsValid() const
{
	return (Array != NULL && Index >= 0 && Index < (ZESSize)Array->GetCount());
}

ZE_ARRAY_ITER_TEMPLATE
ZEItemType& ZEArrayIterator<ZE_ARRAY_ITER_SPEC>::GetItem() const
{
	return Array->GetItem((ZESize)Index);
}

ZE_ARRAY_ITER_TEMPLATE
ZESize ZEArrayIterator<ZE_ARRAY_ITER_SPEC>::GetIndex() const
{
	return (ZESize)Index;
}

ZE_ARRAY_ITER_TEMPLATE
void ZEArrayIterator<ZE_ARRAY_ITER_SPEC>::Prev()
{
	Index++;
	if (Index < 0)
		Index = -1;
}

ZE_ARRAY_ITER_TEMPLATE
void ZEArrayIterator<ZE_ARRAY_ITER_SPEC>::Next()
{	
	Index++;
	if (Index >= (ZESSize)Array->GetCount())
		Index = (ZESSize)Array->GetCount();
}

ZE_ARRAY_ITER_TEMPLATE
bool ZEArrayIterator<ZE_ARRAY_ITER_SPEC>::operator==(const ZEArrayIterator& Iterator) const
{
	return (Iterator.Array == Array && Iterator.Index == Index);
}

ZE_ARRAY_ITER_TEMPLATE
ZEArrayIterator<ZE_ARRAY_ITER_SPEC>& ZEArrayIterator<ZE_ARRAY_ITER_SPEC>::operator--()
{
	Prev();
	return *this;
}

ZE_ARRAY_ITER_TEMPLATE
ZEArrayIterator<ZE_ARRAY_ITER_SPEC>& ZEArrayIterator<ZE_ARRAY_ITER_SPEC>::operator++()
{
	Next();
	return *this;
}

ZE_ARRAY_ITER_TEMPLATE
ZEItemType& ZEArrayIterator<ZE_ARRAY_ITER_SPEC>::operator*() const
{
	return GetItem();
}

ZE_ARRAY_ITER_TEMPLATE
ZEItemType* ZEArrayIterator<ZE_ARRAY_ITER_SPEC>::operator->() const
{
	return &GetItem();
}


// ZEArrayIteratorConst
/////////////////////////////////////////////////////////////////////////////

ZE_ARRAY_ITER_TEMPLATE
ZEArrayIteratorConst<ZE_ARRAY_ITER_SPEC>::ZEArrayIteratorConst(const ZEArray<ZE_ARRAY_ITER_SPEC>& Array, ZESize Index)
{
	this->Array = &Array;
	this->Index = (ZESize)Index;
}

ZE_ARRAY_ITER_TEMPLATE
bool ZEArrayIteratorConst<ZE_ARRAY_ITER_SPEC>::IsValid() const
{
	return (Array != NULL && Index >= 0 && Index < (ZESSize)Array->GetCount());
}

ZE_ARRAY_ITER_TEMPLATE
const ZEItemType& ZEArrayIteratorConst<ZE_ARRAY_ITER_SPEC>::GetItem() const
{
	return Array->GetItem((ZESize)Index);
}

ZE_ARRAY_ITER_TEMPLATE
ZESize ZEArrayIteratorConst<ZE_ARRAY_ITER_SPEC>::GetIndex() const
{
	return (ZESize)Index;
}

ZE_ARRAY_ITER_TEMPLATE
void ZEArrayIteratorConst<ZE_ARRAY_ITER_SPEC>::Prev()
{
	Index--;
	if (Index < 0)
		Index = -1;
}

ZE_ARRAY_ITER_TEMPLATE
void ZEArrayIteratorConst<ZE_ARRAY_ITER_SPEC>::Next()
{	
	Index++;
	if (Index >= (ZESSize)Array->GetCount())
		Index = (ZESSize)Array->GetCount();
}

ZE_ARRAY_ITER_TEMPLATE
bool ZEArrayIteratorConst<ZE_ARRAY_ITER_SPEC>::operator==(const ZEArrayIteratorConst& Iterator) const
{
	return (Iterator.Array == Array && Iterator.Index == Index);
}

ZE_ARRAY_ITER_TEMPLATE
ZEArrayIteratorConst<ZE_ARRAY_ITER_SPEC>& ZEArrayIteratorConst<ZE_ARRAY_ITER_SPEC>::operator--()
{
	Prev();
	return *this;
}

ZE_ARRAY_ITER_TEMPLATE
ZEArrayIteratorConst<ZE_ARRAY_ITER_SPEC>& ZEArrayIteratorConst<ZE_ARRAY_ITER_SPEC>::operator++()
{
	Next();
	return *this;
}

ZE_ARRAY_ITER_TEMPLATE
const ZEItemType& ZEArrayIteratorConst<ZE_ARRAY_ITER_SPEC>::operator*() const
{
	return GetItem();
}

ZE_ARRAY_ITER_TEMPLATE
const ZEItemType* ZEArrayIteratorConst<ZE_ARRAY_ITER_SPEC>::operator->() const
{
	return &GetItem();
}
