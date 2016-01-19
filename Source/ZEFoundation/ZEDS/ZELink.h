//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELink.h
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
#include "ZEList2Iterator.h"

template<typename ZEItemType> class ZEList2;

template<typename ZEItemType>
class ZELink
{
	friend class ZEList2<ZEItemType>;
	private:
		bool							InUse;
		ZEItemType*						Item;
		ZELink*							Prev;
		ZELink*							Next;

	public:
		ZEList2Iterator<ZEItemType>		GetIterator();
		ZEList2IteratorConst<ZEItemType> GetIteratorConst();

		ZEList2Iterator<ZEItemType>		GetIteratorEnd();
		ZEList2IteratorConst<ZEItemType> GetIteratorEndConst() const;

		bool							GetInUse() const;
		ZELink*							GetPrev();
		const ZELink*					GetPrev() const;
		ZELink*							GetNext();
		const ZELink*					GetNext() const;
		ZEItemType*						GetItem() const;

		// ZE_COPY_NO_ACTION
		ZELink&							operator=(const ZELink& Other);
										ZELink(const ZELink& Other);

										ZELink();
										ZELink(ZEItemType* Object);
										~ZELink();
};


// ZELink
/////////////////////////////////////////////////////////////////////////////

template<typename ZEItemType>
ZEList2Iterator<ZEItemType> ZELink<ZEItemType>::GetIterator()
{
	return ZEList2Iterator<ZEItemType>(this);
}

template<typename ZEItemType>
ZEList2IteratorConst<ZEItemType> ZELink<ZEItemType>::GetIteratorConst()
{
	return ZEList2IteratorConst<ZEItemType>(this);
}

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
ZELink<ZEItemType>& ZELink<ZEItemType>::operator=(const ZELink<ZEItemType>& Other)
{
	return *this;
}

template<typename ZEItemType>
ZELink<ZEItemType>::ZELink(const ZELink& Other)
{
	new (this)ZELink<ZEItemType>();
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
