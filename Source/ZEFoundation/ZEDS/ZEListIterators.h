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
template<typename ZEType>
class ZEList;

template<typename ZEType>
class ZEListIterator
{
	template<typename ZEType> friend class ZEList;
	private:
		ZEType* Item;

		ZEListIterator(ZEList<ZEType>* List)
		{
			Item = List->GetFirstItem();
		}

	public:
		inline bool IsEnd() const
		{
			return Item == NULL;
		}

		inline ZEType* GetItem()
		{
			return Item;
		}

		inline ZEType* MovePrevious()
		{
			Item = Item->PrevItem;
			return Item;
		}

		inline ZEType* MoveNext()
		{
			Item = Item->NextItem;
			return Item;			
		}

		inline ZESize GetIndex()
		{
			ZESize Index = 0;
			ZEType* Current = Item;
			while(Current != NULL)
			{
				Current = Current->PrevItem;
				Index++;
			}
			
			return Index;
		}
};

template<typename ZEType>
class ZEListIteratorConst
{
	template<typename ZEType> friend class ZEList;
	private:
		const ZEType* Item;
				
		ZEListIteratorConst(const ZEList<ZEType>* List)
		{
			Item = List->GetFirstItem();
		}

	public:
		inline bool IsEnd() const
		{
			return Item == NULL;
		}

		inline const ZEType* GetItem() const
		{
			return Item;
		}

		inline ZESize GetIndex()
		{
			ZESize Index = 0;
			const ZEType* Current = Item;
			while(Current != NULL)
			{
				Current = Current->PrevItem;
				Index++;
			}

			return Index;
		}

		inline const ZEType* MovePrevious() const
		{
			Item = Item->PrevItem;
			return Item;
		}

		inline const ZEType* MoveNext() const
		{
			Item = Item->NextItem;
			return Item;			
		}
};

#endif
