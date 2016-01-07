//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEList2Iterator.h
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

template<typename ZEItemType>
class ZEList2;

template<typename ZEItemType>
class ZELink;

template<typename ZEItemType>
class ZEList2Iterator : public ZEIterator<ZEItemType>
{
	friend class ZEList2<ZEItemType>;
	private:
		ZELink<ZEItemType>*			Link;

	public:
		inline bool					IsValid() const;

		inline ZEItemType&			GetItem() const;

		inline void					Prev();
		inline void					Next();

		bool						operator==(const ZEList2Iterator& Iterator) const;

		ZEList2Iterator&			operator--();
		ZEList2Iterator&			operator++();

		ZEItemType&					operator*() const;
		ZEItemType*					operator->() const;
		
									ZEList2Iterator(ZELink<ZEItemType>* Link);
};

template<typename ZEItemType>
class ZEList2IteratorConst : public ZEIteratorConst<ZEItemType>
{
	friend class ZEList2<ZEItemType>;
	private:
		const ZELink<ZEItemType>*	Link;

	public:
		inline bool					IsValid() const;

		inline const ZEItemType&	GetItem() const;

		inline void					Prev();
		inline void					Next();

		bool						operator==(const ZEList2IteratorConst& Iterator) const;

		ZEList2IteratorConst&		operator--();
		ZEList2IteratorConst&		operator++();
		const ZEItemType&			operator*() const;
		const ZEItemType*			operator->() const;
									
									ZEList2IteratorConst(const ZELink<ZEItemType>* Link);
};


// ZEList2Iterator
/////////////////////////////////////////////////////////////////////////////

template<typename ZEItemType>
bool ZEList2Iterator<ZEItemType>::IsValid() const
{
	return Link != NULL;
}

template<typename ZEItemType>
ZEItemType& ZEList2Iterator<ZEItemType>::GetItem() const
{
	return *Link->GetItem();
}

template<typename ZEItemType>
void ZEList2Iterator<ZEItemType>::Prev()
{
	Link = Link->GetPrev();
}

template<typename ZEItemType>
void ZEList2Iterator<ZEItemType>::Next()
{	
	Link = Link->GetNext();
}

template<typename ZEItemType>
bool ZEList2Iterator<ZEItemType>::operator==(const ZEList2Iterator& Iterator) const
{
	return (Iterator.Link == Link);
}

template<typename ZEItemType>
ZEList2Iterator<ZEItemType>& ZEList2Iterator<ZEItemType>::operator--()
{
	Prev();
	return *this;
}

template<typename ZEItemType>
ZEList2Iterator<ZEItemType>& ZEList2Iterator<ZEItemType>::operator++()
{
	Next();
	return *this;
}

template<typename ZEItemType>
ZEItemType& ZEList2Iterator<ZEItemType>::operator*() const
{
	return GetItem();
}

template<typename ZEItemType>
ZEItemType* ZEList2Iterator<ZEItemType>::operator->() const
{
	return &GetItem();
}

template<typename ZEItemType>
ZEList2Iterator<ZEItemType>::ZEList2Iterator(ZELink<ZEItemType>* Link)
{
	this->Link = Link;
}

// ZEList2IteratorConst
/////////////////////////////////////////////////////////////////////////////

template<typename ZEItemType>
bool ZEList2IteratorConst<ZEItemType>::IsValid() const
{
	return Link != NULL;
} 

template<typename ZEItemType>
const ZEItemType& ZEList2IteratorConst<ZEItemType>::GetItem() const
{
	return *Link->GetItem();
}

template<typename ZEItemType>
void ZEList2IteratorConst<ZEItemType>::Prev()
{
	Link = Link->GetPrev();
}

template<typename ZEItemType>
void ZEList2IteratorConst<ZEItemType>::Next()
{	
	Link = Link->GetNext();
}

template<typename ZEItemType>
bool ZEList2IteratorConst<ZEItemType>::operator==(const ZEList2IteratorConst& Iterator) const
{
	return (Iterator.Link == Link);
}

template<typename ZEItemType>
ZEList2IteratorConst<ZEItemType>& ZEList2IteratorConst<ZEItemType>::operator--()
{
	Prev();
	return *this;
}

template<typename ZEItemType>
ZEList2IteratorConst<ZEItemType>& ZEList2IteratorConst<ZEItemType>::operator++()
{
	Next();
	return *this;
}

template<typename ZEItemType>
const ZEItemType& ZEList2IteratorConst<ZEItemType>::operator*() const
{
	return GetItem();
}

template<typename ZEItemType>
const ZEItemType* ZEList2IteratorConst<ZEItemType>::operator->() const
{
	return &GetItem();
}

template<typename ZEItemType>
ZEList2IteratorConst<ZEItemType>::ZEList2IteratorConst(const ZELink<ZEItemType>* Link)
{
	this->Link = Link;
}
