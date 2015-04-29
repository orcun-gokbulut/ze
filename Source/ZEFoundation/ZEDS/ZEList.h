//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEList.h
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

#ifndef __ZEDS_LIST_H__
#define __ZEDS_LIST_H__

#include "ZETypes.h"
#include "ZEError.h"
#include "ZEListIterators.h"
#include "ZEError.h"

class ZEListItem
{
	template<typename ZEType> friend class ZEList;
	template<typename ZEType> friend class ZEListIterator;
	template<typename ZEType> friend class ZEListIteratorConst;
	private:
		ZEListItem* NextItem;
		ZEListItem* PrevItem;

	protected:
		ZEListItem()
		{
			NextItem = NULL;
			PrevItem = NULL;
		}
};

template<typename ZEType>
class ZEList
{
	private:
		ZESize Count;
		ZEListItem* FirstItem;
		ZEListItem* LastItem;

		ZEList(const ZEList& Other)
		{
		}
		
		void operator=(const ZEList& Other)
		{
		}

	public:
		typedef ZEListIterator<ZEType> Iterator;
		Iterator GetIterator()
		{
			return Iterator(this);
		}

		typedef ZEListIterator<ZEType> ConstIterator;
		Iterator GetConstIterator() const
		{
			return Iterator(const_cast<ZEList<ZEType>*>(this));
		}

		inline ZESize GetCount() const
		{
			return Count;
		}

		inline ZEType* Append(ZEType* Item)
		{
			zeDebugCheck(Item->PrevItem != NULL || Item->NextItem != NULL, "Item is eighter already associated with another list or corrupt.");
			zeDebugCheck(Exists(Item), "Item is already added to list.");

			Item->NextItem = NULL;
			Item->PrevItem = LastItem;

			if (FirstItem == NULL)
				FirstItem = Item;

			if (LastItem != NULL)
				LastItem->NextItem = Item;

			LastItem = Item;

			Count++;

			return Item;
		}

		inline ZEType* Insert(ZEType* Item)
		{
			zeDebugCheck(Item->PrevItem != NULL || Item->NextItem != NULL, "Item is eighter already associated with another list or corrupt.");
			zeDebugCheck(Exists(Item), "Item is already added to list.");

			Item->NextItem = FirstItem;
			Item->PrevItem = NULL;

			if (LastItem == NULL)
				LastItem = Item;

			if (FirstItem != NULL)
				FirstItem->PrevItem = Item;

			FirstItem = Item;


			Count++;

			return Item;
		}

		inline ZEType* Insert(ZESize Index, ZEType* Item)
		{
			zeDebugCheck(Index > Count, "Index out of range.");
			zeDebugCheck(Item->PrevItem != NULL || Item->NextItem != NULL, "Item is eighter already associated with another list or corrupt.");
			zeDebugCheck(Exists(Item), "Item is already added to list.");

			if (Index == 0)
				return Insert(Item);
			else if (Index == Count)
				return Append(Item);
			else
			{
				ZEType* OldItem = GetItem(Index);
				ZEType* PreviousItem = (ZEType*)OldItem->PrevItem;
				Item->PrevItem = OldItem->PrevItem;
				Item->NextItem = OldItem;
				OldItem->PrevItem = Item;
				PreviousItem->NextItem = Item;
				Count++;
				return Item;
			}
		}

		inline void Remove(ZEType* Item)
		{
			zeDebugCheck(!Exists(Item), "Item is not in the list.");

			if (Item->PrevItem == NULL)
			{
				FirstItem = Item->NextItem;

				if (FirstItem != NULL)
					FirstItem->PrevItem = NULL;			
			}

			else
			{
				ZEType* Previous = (ZEType*)Item->PrevItem;
				Previous->NextItem = Item->NextItem;
			}
			
			if (Item->NextItem == NULL)
			{
				LastItem = Item->PrevItem;

				if (LastItem != NULL)
					LastItem->NextItem = NULL;
			}

			else
			{
				ZEType* Next = (ZEType*)Item->NextItem;
				Next->PrevItem = Item->PrevItem;
			}
				
			Item->PrevItem = NULL;
			Item->NextItem = NULL;

			Count--;
		}

		inline void RemoveAt(ZESize Index)
		{
			ZEType* Item = GetItem(Index);
			Remove(Item);
		}

		inline void RemoveAll()
		{
			ZEType* Cursor = (ZEType*)FirstItem;
			while(Cursor != NULL)
			{
				ZEType* Temp = (ZEType*)Cursor->NextItem;
				Cursor->PrevItem = NULL;
				Cursor->NextItem = NULL;
				Cursor = Temp;
			}
  			Count = 0;
		}

		inline const ZEType* GetItem(ZESize Index) const
		{
			zeDebugCheck(Index >= Count, "Index is out of range.");
			
			const ZEType* Cursor = (ZEType*)FirstItem;
			while(Index-- != 0)
				Cursor = (ZEType*)Cursor->NextItem;
			
			return Cursor;
		}

		inline ZEType* GetItem(ZESize Index)
		{
			zeDebugCheck(Index >= Count, "Index is out of range.");

			ZEType* Cursor = (ZEType*)FirstItem;
			while(Index-- != 0)
				Cursor = (ZEType*)Cursor->NextItem;

			return Cursor;
		}

		inline ZEType* GetFirstItem()
		{
			return (ZEType*)FirstItem;
		}

		inline const ZEType* GetFirstItem() const
		{
			return (const ZEType*)FirstItem;
		}

		inline ZEType* GetLastItem()
		{
			return (ZEType*)LastItem;
		}

		inline const ZEType* GetLastItem() const
		{
			return (const ZEType*)LastItem;
		}

		inline ZEType* operator[](ZESize Index)
		{
			zeDebugCheck(Index >= Count, "Index is out of range.");
			return GetItem(Index);
		}

		inline const ZEType* operator[](ZESize Index) const
		{
			zeDebugCheck(Index >= Count, "Index is out of range.");
			return GetItem(Index);
		}

		inline void Enqueue(ZEType* Value)
		{
			Append(Value);
		}

		inline ZEType* Dequeue()
		{
			ZEType* Item = (ZEType*)FirstItem;
			Remove((ZEType*)FirstItem);

			return Item;
		}

		inline void Push(ZEType* Value)
		{
			Append(Value);
		}

		inline ZEType* Pop()
		{
			ZEType* Item = (ZEType*)LastItem;
			Remove((ZEType*)LastItem);

			return Item;
		}

		inline bool Exists(ZEType* Item) const
		{
			ZEType* Cursor = (ZEType*)FirstItem;
			while(Cursor != NULL)
			{
				if (Cursor == Item)
					return true;
				Cursor = (ZEType*)Cursor->NextItem;
			}
			return false;
		}

		inline ZESSize FindIndex(ZEType* Item, ZESize StartIndex = 0) const
		{
			ZESize Index = StartIndex;
			ZEType* Cursor = (ZEType*)GetItem(Index);
			while(Cursor != NULL)
			{
				if (Cursor == Item)
					return Index;

				Index++;
				Cursor = (ZEType*)Cursor->NextItem;
			}

			return -1;
		}

		ZEList()
		{
			FirstItem = NULL;
			LastItem = NULL;
			Count = 0;
		}
		
		~ZEList()
		{
			RemoveAll();
		}
};

#endif
