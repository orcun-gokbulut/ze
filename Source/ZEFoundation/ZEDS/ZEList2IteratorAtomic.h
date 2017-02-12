//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEList2IteratorAtomic.h
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

#include "ZEIteratorAtomic.h"

#include "ZETypes.h"
#include "ZEThread/ZELock.h"

#define ZE_LIST2_ITERATOR_ATOMIC_TEMPLATE template<typename ZEItemType, typename ZELockType>
#define ZE_LIST2_ITERATOR_ATOMIC_SPEC ZEItemType, ZELockType

template<typename ZEItemType, typename ZELockType> class ZEList2;
template<typename ZEItemType> class ZELink;

ZE_LIST2_ITERATOR_ATOMIC_TEMPLATE
class ZEList2IteratorAtomic : public ZEIteratorAtomic<ZEItemType>
{
	friend class ZEList2<ZEItemType, ZELockType>;
	private:
		ZELink<ZEItemType>*						Link;
		ZELock									Lock;

	public:
		inline bool								IsValid() const;

		ZELink<ZEItemType>*						Next();
		ZELink<ZEItemType>*						Prev();

		ZEItemType*								NextItem();
		ZEItemType*								PrevItem();

												ZEList2IteratorAtomic(ZELink<ZEItemType>* Link);
};

ZE_LIST2_ITERATOR_ATOMIC_TEMPLATE
class ZEList2IteratorAtomicConst : public ZEIteratorAtomicConst<ZEItemType>
{
	friend class ZEList2<ZEItemType, ZELockType>;
	private:
		const ZELink<ZEItemType>*				Link;

	public:
		inline bool								IsValid() const;

		const ZELink<ZEItemType>*				Next();
		const ZELink<ZEItemType>*				Prev();

		ZEItemType*								NextItem();
		ZEItemType*								PrevItem();
													
												ZEList2IteratorAtomicConst(const ZELink<ZEItemType>* Link);
};


// ZEList2IteratorAtomic
/////////////////////////////////////////////////////////////////////////////

ZE_LIST2_ITERATOR_ATOMIC_TEMPLATE
bool ZEList2IteratorAtomic<ZE_LIST2_ITERATOR_ATOMIC_SPEC>::IsValid() const
{
	return Link != NULL;
}

ZE_LIST2_ITERATOR_ATOMIC_TEMPLATE
ZELink<ZEItemType>* ZEList2IteratorAtomic<ZE_LIST2_ITERATOR_ATOMIC_SPEC>::Prev()
{
	ZELink<ZEItemType>* Output;
	Lock.Lock();
	{
		Output = Link;
		if (Link != NULL)
			Link = Link->GetPrev();
	}
	Lock.Unlock();

	return Output;
}

ZE_LIST2_ITERATOR_ATOMIC_TEMPLATE
ZELink<ZEItemType>* ZEList2IteratorAtomic<ZE_LIST2_ITERATOR_ATOMIC_SPEC>::Next()
{	
	ZELink<ZEItemType>* Output;
	Lock.Lock();
	{
		Output = Link;
		if (Link != NULL)
			Link = Link->GetNext();
	}
	Lock.Unlock();

	return Output;
}

ZE_LIST2_ITERATOR_ATOMIC_TEMPLATE
ZEItemType* ZEList2IteratorAtomic<ZE_LIST2_ITERATOR_ATOMIC_SPEC>::PrevItem()
{
	ZELink<ZEItemType>* Link = Prev();
	if (Link == NULL)
		return NULL;

	return Link->GetItem();
}

ZE_LIST2_ITERATOR_ATOMIC_TEMPLATE
ZEItemType* ZEList2IteratorAtomic<ZE_LIST2_ITERATOR_ATOMIC_SPEC>::NextItem()
{	
	ZELink<ZEItemType>* Link = Next();
	if (Link == NULL)
		return NULL;

	return Link->GetItem();
}

ZE_LIST2_ITERATOR_ATOMIC_TEMPLATE
ZEList2IteratorAtomic<ZE_LIST2_ITERATOR_ATOMIC_SPEC>::ZEList2IteratorAtomic(ZELink<ZEItemType>* Link)
{
	this->Link = Link;
}

// ZEList2IteratorAtomicConst
/////////////////////////////////////////////////////////////////////////////

ZE_LIST2_ITERATOR_ATOMIC_TEMPLATE
bool ZEList2IteratorAtomicConst<ZE_LIST2_ITERATOR_ATOMIC_SPEC>::IsValid() const
{
	return Link != NULL;
} 

ZE_LIST2_ITERATOR_ATOMIC_TEMPLATE
const ZELink<ZEItemType>* ZEList2IteratorAtomicConst<ZE_LIST2_ITERATOR_ATOMIC_SPEC>::Prev()
{
	const ZELink<ZEItemType>* Output;
	Lock.Lock();
	{
		Output = Link;
		if (Link != NULL)
			Link = Link->GetPrev();
	}
	Lock.Unlock();
	
	return Output;
}

ZE_LIST2_ITERATOR_ATOMIC_TEMPLATE
const ZELink<ZEItemType>* ZEList2IteratorAtomicConst<ZE_LIST2_ITERATOR_ATOMIC_SPEC>::Next()
{	
	const ZELink<ZEItemType>* Output;
	Lock.Lock();
	{
		Output = Link;
		if (Link != NULL)
			Link = Link->GetNext();
	}
	Lock.Unlock();

	return Output;
}

ZE_LIST2_ITERATOR_ATOMIC_TEMPLATE
ZEItemType* ZEList2IteratorAtomicConst<ZE_LIST2_ITERATOR_ATOMIC_SPEC>::PrevItem()
{
	const ZELink<ZEItemType>* Link = Prev();
	if (Link == NULL)
		return NULL;

	return Link->GetItem();
}

ZE_LIST2_ITERATOR_ATOMIC_TEMPLATE
ZEItemType* ZEList2IteratorAtomicConst<ZE_LIST2_ITERATOR_ATOMIC_SPEC>::NextItem()
{	
	const ZELink<ZEItemType>* Link = Next();
	if (Link == NULL)
		return NULL;

	return Link->GetItem();
}

ZE_LIST2_ITERATOR_ATOMIC_TEMPLATE
ZEList2IteratorAtomicConst<ZE_LIST2_ITERATOR_ATOMIC_SPEC>::ZEList2IteratorAtomicConst(const ZELink<ZEItemType>* Link)
{
	this->Link = Link;
}
