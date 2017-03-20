//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEListIterators.h
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

#ifndef __ZEDS_LIST_ITERATORS_H__
#define __ZEDS_LIST_ITERATORS_H__

#include "ZETypes.h"
template<typename ZEMTType>
class ZEList;

template<typename ZEMTType>
class ZEListIterator
{
	friend class ZEList<ZEMTType>;
	private:
		ZEMTType* Item;

		ZEListIterator(ZEList<ZEMTType>* List)
		{
			Item = List->GetFirstItem();
		}

	public:
		inline bool IsEnd() const
		{
			return Item == NULL;
		}

		inline ZEMTType* GetItem()
		{
			return Item;
		}

		inline ZEMTType* MovePrevious()
		{
            Item = (ZEMTType*)Item->PrevItem;
            return Item;
		}

		inline ZEMTType* MoveNext()
		{
            Item = (ZEMTType*)Item->NextItem;
            return Item;
		}

		inline ZESize GetIndex()
		{
			ZESize Index = 0;
			ZEMTType* Current = Item;
			while(Current != NULL)
			{
				Current = (ZEMTType*)Current->PrevItem;
				if (Current == NULL)
				{
					return Index;
				}

				Index++;
			}
			
			return Index;
		}

		operator ZEMTType*()
		{
			return Item;
		}

		operator const ZEMTType*() const 
		{
			return Item;
		}
		
		ZEMTType& operator*()
		{
			return *Item;
		}
		
		const ZEMTType& operator*() const
		{
			return *Item;
		}

		ZEMTType* operator->()
		{
			return Item;
		}

		const ZEMTType* operator->() const
		{
			return Item;
		}

		ZEListIterator operator++(int)
		{
			ZEListIterator Temp = *this;
			MoveNext();
			return Temp;
		}

		ZEListIterator& operator++()
		{
			MoveNext();
			return *this;
		}

		ZEListIterator operator--(int)
		{
			ZEListIterator Temp = *this;
			MovePrevious();
			return Temp;
		}

		ZEListIterator& operator--()
		{
			MovePrevious();
			return *this;
		}
};

template<typename ZEMTType>
class ZEListIteratorConst
{
	friend class ZEList<ZEMTType>;
	private:
		const ZEMTType* Item;
				
		ZEListIteratorConst(const ZEList<ZEMTType>* List)
		{
			Item = List->GetFirstItem();
		}

	public:
		inline bool IsEnd() const
		{
			return Item == NULL;
		}

		inline const ZEMTType* GetItem() const
		{
			return Item;
		}

		inline ZESize GetIndex()
		{
			ZESize Index = 0;
			const ZEMTType* Current = Item;
			while(Current != NULL)
			{
				Current = (const ZEMTType*)Current->PrevItem;
				if (Current == NULL)
				{
					return Index;
				}

				Index++;
			}

			return Index;
		}

		inline const ZEMTType* MovePrevious()
		{
			Item = (const ZEMTType*)Item->PrevItem;
			return Item;
		}

		inline const ZEMTType* MoveNext()
		{
			Item = (const ZEMTType*)Item->NextItem;
 			return Item;			
		}
};

#endif
