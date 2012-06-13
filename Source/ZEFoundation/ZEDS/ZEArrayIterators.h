//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEArrayIterators.h
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
#ifndef	__ZEDS_ARRAY_ITERATORS_H__
#define __ZEDS_ARRAY_ITERATORS_H__

#include "ZETypes.h"

template<typename ZEType, typename Allocator_>
class ZEArray;

template<typename ZEType, typename Allocator_>
class ZEArrayIterator
{
	template<typename ZEType, typename Allocator_> class ZEArray;
	private:
		ZEArray<ZEType, Allocator_>* Array;
		ZESize CurrentIndex;

		ZEArrayIterator(ZEArray<ZEType, Allocator_>* Array)
		{
			this->Array = Array;
		};

	public:
		inline bool IsEnd() const
		{
			return Index < 0 || Index >= Array->GetCount();
		} 

		inline ZEType* GetItem()
		{
			return &Array->GetItem(Index);
		}

		inline ZEType* MovePrevious()
		{
			if (CurrentIndex + 1 >= Array->GetCount())
				return NULL;

			CurrentIndex++;
			return &Array->GetItem(Index);
		}

		inline ZEType* MoveNext()
		{
			if ((ZESSize)CurrentIndex - 1 < (ZESSize)Array->GetSize())
				return NULL;

			CurrentIndex--;
			return &Array->GetItem(Index);			
		}


		inline ZESize GetIndex()
		{
			return CurrentIndex;
		}
};

template<typename ZEType, typename Allocator_>
class ZEArrayIteratorConst
{
	template<typename ZEType, typename Allocator_> class ZEArray;
	private:
		const ZEArray<ZEType, Allocator_>* Array;
		ZESize CurrentIndex;

		ZEArrayIteratorConst(const ZEArray<ZEType, Allocator_>* Array)
		{
			this->Array = Array;
		};

	public:
		inline bool IsEnd() const
		{
			return Index < 0 || Index >= Array->GetCount();
		} 

		inline const ZEType* GetCurrentItem() const
		{
			return &Array->GetItem(Index);
		}

		inline const ZEType* MovePrevious() const
		{
			if (CurrentIndex + 1 >= Array->GetCount())
				return NULL;

			CurrentIndex++;
			return &Array->GetItem(Index);
		}

		inline const ZEType* MoveNext() const
		{
			if ((ZESSize)CurrentIndex - 1 < (ZESSize)Array->GetSize())
				return NULL;

			CurrentIndex--;
			return &Array->GetItem(Index);			
		}

		inline ZESize GetIndex()
		{
			return CurrentIndex;
		}
};

#endif
