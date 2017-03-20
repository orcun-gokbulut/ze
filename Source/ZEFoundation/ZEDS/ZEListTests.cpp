//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEListTests.cpp
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


#include "ZEDS/ZEList.h"
#include "ZETest/ZETest.h"
#include "ZETest/ZETestCheck.h"

class ZEListTestItem : public ZEListItem
{
public:

    ZEListTestItem(ZEListItem* Next, ZEListItem* Prev)
    {
        ZEListItem* NextItem = Next;
        ZEListItem* PrevItem = Prev;
    }

	ZEListTestItem()
	{
		ZEListItem* NextItem = NULL;
		ZEListItem* PrevItem = NULL; 
	}
};

ZE_TEST(ZEList)
{
	ZE_TEST_ITEM("~ZEList()")
	{
		ZEList<ZEListTestItem> List;
		ZEListTestItem Item1;
		ZEListTestItem Item2;
		ZEListTestItem Item3;

		List.Append(&Item1);
		List.Append(&Item2);
		List.Append(&Item3);
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 3);

		List.~ZEList();
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 0);
	}

	ZE_TEST_ITEM("inline ZEType* Append(ZEType* Item)")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item;

		ZEListTestItem* Result = List.Append(&Item);
		ZE_TEST_CHECK_ERROR(Result == &Item);
	
		ZE_TEST_CASE("for FirstItem != NULL")
        {
 			ZEList<ZEListTestItem> List1;
			ZEListTestItem Item1;
			Result = List1.Insert(&Item1);
			ZE_TEST_CHECK_EQUAL(List1.GetFirstItem(), &Item1);

			ZEListTestItem Item2;
			Result = List1.Append(&Item2);
			ZE_TEST_CHECK_EQUAL(List1.GetFirstItem(), &Item1);
			ZE_TEST_CHECK_EQUAL(List1.GetLastItem(), &Item2);

			ZEListTestItem Item3;
			Result = List1.Append(&Item3);
			ZE_TEST_CHECK_EQUAL(List1.GetLastItem(), &Item3);

			ZEListTestItem Item4;
			Result = List1.Insert(&Item4);
			ZE_TEST_CHECK_EQUAL(List1.GetLastItem(), &Item3);
			ZE_TEST_CHECK_EQUAL(List1.GetFirstItem(), &Item4);
 		}
	}

	ZE_TEST_ITEM("inline ZEType* Dequeue()")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;

		ZEListTestItem* Item = List.Insert(&Item1);
		Item = List.Insert(&Item2);
		ZE_TEST_CHECK_EQUAL(List.GetFirstItem(), &Item2);
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 2);

		ZEListTestItem* Result = List.Dequeue();
		ZE_TEST_CHECK_EQUAL(Result, &Item2);
		ZE_TEST_CHECK_EQUAL(List.GetFirstItem(), &Item1);
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 1);
	}
 
	ZE_TEST_ITEM("inline void Enqueue(ZEType* Value)")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;

		List.Enqueue(&Item1);
		ZE_TEST_CHECK_EQUAL(List.GetLastItem(), &Item1);
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 1);

		ZE_TEST_CASE("for LastItem == NULL")
		{
			ZE_TEST_CHECK_EQUAL(List.GetLastItem(), &Item1);
			
			ZEListTestItem Item2;

			List.Enqueue(&Item2);
			ZE_TEST_CHECK_EQUAL(List.GetFirstItem(), &Item1);
			ZE_TEST_CHECK_EQUAL(List.GetLastItem(), &Item2);
			ZE_TEST_CHECK_EQUAL(List.GetCount(), 2);
		}
	}
 
	ZE_TEST_ITEM("inline bool Exists(ZEType* Item) const")
 	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item;
		
		bool Exist = List.Exists(&Item);
		ZE_TEST_CHECK_ERROR(!Exist);

		ZE_TEST_CASE("for Cursor != NULL")
		{
			ZEListTestItem Item1;
			ZEListTestItem* Result = List.Append(&Item1);

			Exist = List.Exists(&Item);
			ZE_TEST_CHECK_ERROR(!Exist);
			Exist = List.Exists(&Item1);
			ZE_TEST_CHECK_ERROR(Exist);
		}

		ZE_TEST_CASE("for true")
		{
			ZEListTestItem Item2;
			ZEListTestItem Item3;
			ZEListTestItem* Result = List.Append(&Item2);
			Result = List.Append(&Item3);

			Exist = List.Exists(&Item2);
			ZE_TEST_CHECK_ERROR(Exist);
			Exist = List.Exists(&Item3);
			ZE_TEST_CHECK_ERROR(Exist);
		}
 	}

	ZE_TEST_ITEM("inline ZESSize FindIndex(ZEType* Item, ZESize StartIndex = 0) const")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;
		ZEListTestItem Item3;

		ZEListTestItem* Item = List.Insert(0, &Item1);
		Item = List.Insert(1, &Item2);
		Item = List.Insert(2, &Item3);

		ZE_TEST_CHECK_EQUAL(List[0], &Item1);
		ZE_TEST_CHECK_EQUAL(List[1], &Item2);
		ZE_TEST_CHECK_EQUAL(List[2], &Item3);

		ZESSize Index = List.FindIndex(&Item1);
		ZE_TEST_CHECK_EQUAL(Index, 0);
		Index = List.FindIndex(&Item2);
		ZE_TEST_CHECK_EQUAL(Index, 1);
		Index = List.FindIndex(&Item3);
		ZE_TEST_CHECK_EQUAL(Index, 2);

		Index = List.FindIndex(&Item1, 1);
		ZE_TEST_CHECK_EQUAL(Index, -1);
		Index = List.FindIndex(&Item2, 1);
		ZE_TEST_CHECK_EQUAL(Index, 1);
		Index = List.FindIndex(&Item3, 1);
		ZE_TEST_CHECK_EQUAL(Index, 2);

		Index = List.FindIndex(&Item1, 2);
		ZE_TEST_CHECK_EQUAL(Index, -1);
		Index = List.FindIndex(&Item2, 2);
		ZE_TEST_CHECK_EQUAL(Index, -1);
		Index = List.FindIndex(&Item3, 2);
		ZE_TEST_CHECK_EQUAL(Index, 2);

		Index = List.FindIndex(&Item3, 3);
		//error: Index is out of range.
		ZE_TEST_CHECK_EQUAL(Index, -1);
	}

	ZE_TEST_ITEM("ConstIterator GetConstIterator() const")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;

		ZEListTestItem* Item = List.Insert(&Item1);
		Item = List.Append(&Item2);

		const ZEListIteratorConst<ZEListTestItem> ConstIterator = List.GetConstIterator();
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), &Item1);
	}
 
	ZE_TEST_ITEM("inline ZESize GetCount() const")
	{
		ZEList<ZEListTestItem> List;
		ZESize Count = List.GetCount();
		ZE_TEST_CHECK_EQUAL(Count, 0);

		ZE_TEST_CASE("for not empty list")
		{
			ZEListTestItem Item1;
			ZEListTestItem Item2;

			List.Insert(&Item1);

			Count = List.GetCount();
			ZE_TEST_CHECK_EQUAL(Count, 1);

			List.Insert(&Item2);

			Count = List.GetCount();
			ZE_TEST_CHECK_EQUAL(Count, 2);
		}
	}
 
	ZE_TEST_ITEM("inline const ZEType* GetFirstItem() const")
	{
		ZEList<ZEListTestItem> List;
		const ZEListTestItem *First = List.GetFirstItem();
		ZE_TEST_CHECK_EQUAL(First, NULL);

		ZE_TEST_CASE("for not empty list")
		{
			ZEListTestItem Item1;
			ZEListTestItem Item2;
		
			List.Insert(&Item1);

			First = List.GetFirstItem();
			ZE_TEST_CHECK_EQUAL(First, &Item1);

			List.Insert(&Item2);

			First = List.GetFirstItem();
			ZE_TEST_CHECK_EQUAL(First, &Item2);
		}
 	}

	ZE_TEST_ITEM("inline ZEType* GetFirstItem()")
	{
		ZEList<ZEListTestItem> List;
		ZEListTestItem* First = List.GetFirstItem();
		ZE_TEST_CHECK_EQUAL(First, NULL);

		ZE_TEST_CASE("for not empty list")
		{
			ZEListTestItem Item1;
			ZEListTestItem Item2;

			List.Insert(&Item1);

			First = List.GetFirstItem();
			ZE_TEST_CHECK_EQUAL(First, &Item1);

			List.Insert(&Item2);

			First = List.GetFirstItem();
			ZE_TEST_CHECK_EQUAL(First, &Item2);
		}
	}

	ZE_TEST_ITEM("inline const ZEType* GetItem(ZESize Index) const")
 	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;

		List.Insert(&Item1);

		const ZEListTestItem* Result = List.GetItem(0);
		ZE_TEST_CHECK_EQUAL(Result, &Item1);

		List.Insert(&Item2);

		Result = List.GetItem(0);
		ZE_TEST_CHECK_EQUAL(Result, &Item2);
		Result = List.GetItem(1);
		ZE_TEST_CHECK_EQUAL(Result, &Item1);
 	}

	ZE_TEST_ITEM("inline ZEType* GetItem(ZESize Index)")
	{
		ZEList<ZEListTestItem> List;
		ZEListTestItem* Result = List.GetItem(0);
		//index is out of range
		
		ZE_TEST_CASE("for not empty list")
		{
			ZEListTestItem Item1;
			ZEListTestItem Item2;

			List.Insert(&Item1);

			Result = List.GetItem(0);
			ZE_TEST_CHECK_EQUAL(Result, &Item1);

			List.Insert(&Item2);

			Result = List.GetItem(0);
			ZE_TEST_CHECK_EQUAL(Result, &Item2);
			Result = List.GetItem(1);
			ZE_TEST_CHECK_EQUAL(Result, &Item1);
		}
  	}

	ZE_TEST_ITEM("Iterator GetIterator()")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;

		ZEListTestItem* Item = List.Insert(&Item1);
		Item = List.Insert(&Item2);
		
		ZEListIterator<ZEListTestItem> Iterator = List.GetIterator();
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), &Item2);
	}
 
	ZE_TEST_ITEM("inline ZEType* GetLastItem()")
	{
		ZEList<ZEListTestItem> List;
		ZEListTestItem* Last = List.GetLastItem();
		ZE_TEST_CHECK_EQUAL(Last, NULL);

		ZE_TEST_CASE("for not empty list")
		{
			ZEListTestItem Item;
	
			List.Insert(&Item);

			Last = List.GetLastItem();
			ZE_TEST_CHECK_EQUAL(Last, &Item);
		}

		ZE_TEST_CASE("for List count > 1")
		{
			ZE_TEST_CHECK_EQUAL(List.GetCount(), 1);
			
			ZEListTestItem Item1;
			List.Append(&Item1);
			ZE_TEST_CHECK_EQUAL(List.GetCount(), 2);

			Last = List.GetLastItem();
			ZE_TEST_CHECK_EQUAL(Last, &Item1);
		}
	}
 
	ZE_TEST_ITEM("inline const ZEType* GetLastItem() const")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		List.Insert(&Item1);

		const ZEListTestItem* Last = List.GetLastItem();
		ZE_TEST_CHECK_EQUAL(Last, &Item1);
		
		ZEListTestItem Item2;
		List.Insert(&Item2);

		Last = List.GetLastItem();
		ZE_TEST_CHECK_EQUAL(Last, &Item1);
		ZE_TEST_CHECK_EQUAL(List.GetFirstItem(), &Item2);
	}

	ZE_TEST_ITEM("inline ZEType* Insert(ZEType* Item)")
	{
		ZEList<ZEListTestItem> List;

		ZE_TEST_CHECK_EQUAL(List.GetCount(), 0);
		
		ZEListTestItem Item;

 		ZEListTestItem* Result = List.Insert(&Item);
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 1);
		ZE_TEST_CHECK_EQUAL(Result, &Item);
		ZE_TEST_CHECK_EQUAL(List.GetFirstItem(), Result);

		ZE_TEST_CASE("for item is already in list")
		{
			bool Exist = List.Exists(&Item);
			ZE_TEST_CHECK_ERROR(Exist);

			ZEListTestItem Item1;
			Result = List.Insert(&Item1);
			ZE_TEST_CHECK_EQUAL(List.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(Result, &Item1);
			ZE_TEST_CHECK_EQUAL(List.GetFirstItem(), Result);

			List.Insert(&Item);
			//Item is either already associated with another list or corrupt
			//Item is already added to list
		}
	}
 
 	ZE_TEST_ITEM("inline ZEType* Insert(ZESize Index, ZEType* Item)")
 	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;
		ZEListTestItem Item3;
			
		ZEListTestItem* Result1 = List.Insert(0, &Item1);
		ZEListTestItem* Result2 = List.Insert(1, &Item2);
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 2);
		ZE_TEST_CHECK_EQUAL(List[0], &Item1);
		ZE_TEST_CHECK_EQUAL(Result1, &Item1);
		ZE_TEST_CHECK_EQUAL(List[1], &Item2);
		ZE_TEST_CHECK_EQUAL(Result2, &Item2);

		ZEListTestItem* Result3 = List.Insert(1, &Item3);
		ZE_TEST_CHECK_EQUAL(Result3, &Item3);
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 3);
		ZE_TEST_CHECK_EQUAL(List[1], &Item3);
		ZE_TEST_CHECK_EQUAL(List[2], &Item2);
		ZE_TEST_CHECK_EQUAL(List[0], &Item1);

		ZE_TEST_CASE("for dynamically created ZEListTestItems")
		{
			ZEList<ZEListTestItem> List1;

			ZEListTestItem* TestItem1 = new ZEListTestItem();
			ZEListTestItem* TestItem2 = new ZEListTestItem();
			ZEListTestItem* TestItem3 = new ZEListTestItem();

			ZEListTestItem* Res1 = List1.Insert(0, TestItem1);
			ZEListTestItem* Res2 = List1.Insert(1, TestItem2);
			ZE_TEST_CHECK_EQUAL(List1.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(List1[0], TestItem1);
			ZE_TEST_CHECK_EQUAL(Res1, TestItem1);
			ZE_TEST_CHECK_EQUAL(List1[1], TestItem2);
			ZE_TEST_CHECK_EQUAL(Res2, TestItem2);
			ZEListTestItem* Res3 = List1.Insert(1, TestItem3);
			ZE_TEST_CHECK_EQUAL(List1.GetCount(), 3);
			ZE_TEST_CHECK_EQUAL(Res3, TestItem3);
			ZE_TEST_CHECK_EQUAL(List1[0], TestItem1);
			ZE_TEST_CHECK_EQUAL(List1[1], TestItem3);
			ZE_TEST_CHECK_EQUAL(List1[2], TestItem2);
		}
	}
 
	ZE_TEST_ITEM("inline ZEType* operator[](ZESize Index)")
 	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;
		ZEListTestItem Item3;

		List.Insert(&Item1);
		List.Insert(&Item2);
		List.Insert(&Item3);

		ZEListTestItem* Result = List[0];
		ZE_TEST_CHECK_EQUAL(Result, &Item3);

		Result = List[1];
		ZE_TEST_CHECK_EQUAL(Result, &Item2);

		Result = List[2];
		ZE_TEST_CHECK_EQUAL(Result, &Item1);
 	}

	ZE_TEST_ITEM("inline const ZEType* operator[](ZESize Index) const")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;
		ZEListTestItem Item3;
		List.Insert(0, &Item1);
		List.Insert(1, &Item2);
		List.Insert(2, &Item3);

		const ZEListTestItem* Result = List[0];
		ZE_TEST_CHECK_EQUAL(Result, &Item1);

		Result = List[1];
		ZE_TEST_CHECK_EQUAL(Result, &Item2);

		Result = List[2];
		ZE_TEST_CHECK_EQUAL(Result, &Item3);
	}
 
	ZE_TEST_ITEM("inline ZEType Pop()")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;

		List.Insert(&Item1);
		List.Insert(&Item2);
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 2);
		ZE_TEST_CHECK_EQUAL(List[0], &Item2);
		ZE_TEST_CHECK_EQUAL(List[1], &Item1);

		ZEListTestItem* Value = List.Pop();
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 1);
		ZE_TEST_CHECK_EQUAL(Value, &Item1);
		ZE_TEST_CHECK_EQUAL(List[0], &Item2);		
	}
 
	ZE_TEST_ITEM("inline void Push(ZEType* Value)")
	{
		ZEList<ZEListTestItem> List;

		ZE_TEST_CHECK_EQUAL(List.GetCount(), 0);
		ZE_TEST_CHECK_EQUAL(List.GetFirstItem(), NULL);
	
		ZEListTestItem Item1;

		List.Push(&Item1);
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 1);
		ZE_TEST_CHECK_EQUAL(List.GetFirstItem(), &Item1);

		
		ZEListTestItem Item2;

		List.Push(&Item2);
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 2);
		ZE_TEST_CHECK_EQUAL(List.GetFirstItem(), &Item1);
		ZE_TEST_CHECK_EQUAL(List.GetLastItem(), &Item2);
	}

	ZE_TEST_ITEM("inline void Remove(ZEType* Item)")
 	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;

		List.Insert(&Item1);
		List.Insert(&Item2);
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 2);
		ZE_TEST_CHECK_EQUAL(List[0], &Item2);

		List.Remove(&Item2);
 		ZE_TEST_CHECK_EQUAL(List.GetCount(), 1);
		ZE_TEST_CHECK_EQUAL(List[0], &Item1);

		List.Remove(&Item1);
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 0);

		ZE_TEST_CASE("for Count > 2")
		{
			ZEList<ZEListTestItem> List1;
			
			ZEListTestItem Item3;
			List1.Insert(&Item1);
			List1.Insert(&Item2);
			List1.Insert(&Item3);
			ZE_TEST_CHECK_EQUAL(List1.GetCount(), 3);
			ZE_TEST_CHECK_EQUAL(List1[0], &Item3);
			ZE_TEST_CHECK_EQUAL(List1[1], &Item2);
			ZE_TEST_CHECK_EQUAL(List1[2], &Item1);

			List1.Remove(&Item3);
			ZE_TEST_CHECK_EQUAL(List1.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(List1[0], &Item2);
			ZE_TEST_CHECK_EQUAL(List1[1], &Item1);

			List1.Remove(&Item2);
			ZE_TEST_CHECK_EQUAL(List1.GetCount(), 1);
			ZE_TEST_CHECK_EQUAL(List1[0], &Item1);
		}

		ZE_TEST_CASE("for Count > 2, remove last item first")
		{
			ZEList<ZEListTestItem> List1;

			ZEListTestItem Item3;
			List1.Insert(&Item1);
			List1.Insert(&Item2);
			List1.Insert(&Item3);
			ZE_TEST_CHECK_EQUAL(List1.GetCount(), 3);
			ZE_TEST_CHECK_EQUAL(List1[0], &Item3);
			ZE_TEST_CHECK_EQUAL(List1[1], &Item2);
			ZE_TEST_CHECK_EQUAL(List1[2], &Item1);

			List1.Remove(&Item1);
			ZE_TEST_CHECK_EQUAL(List1.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(List1[0], &Item3);
			ZE_TEST_CHECK_EQUAL(List1[1], &Item2);

			List1.Remove(&Item3);
			ZE_TEST_CHECK_EQUAL(List1.GetCount(), 1);
			ZE_TEST_CHECK_EQUAL(List1[0], &Item2);
		}

		ZE_TEST_CASE("insert and remove same item respectively to an empty list")
		{
			ZEList<ZEListTestItem> List2;
			ZE_TEST_CHECK_EQUAL(List2.GetFirstItem(), NULL);
			ZE_TEST_CHECK_EQUAL(List2.GetLastItem(), NULL);
			
			ZEListTestItem Item;
			ZEListTestItem* Result = List2.Insert(&Item);
			ZE_TEST_CHECK_EQUAL(Result, &Item);
			ZE_TEST_CHECK_EQUAL(List2.GetFirstItem(), &Item);
			ZE_TEST_CHECK_EQUAL(List2.GetLastItem(), &Item);
			
			List2.Remove(&Item);
			ZE_TEST_CHECK_EQUAL(List2.GetFirstItem(), NULL);
			ZE_TEST_CHECK_EQUAL(List2.GetLastItem(), NULL);
			ZE_TEST_CHECK_EQUAL(List2.GetCount(), 0);
		}

		ZE_TEST_CASE("for dynamically created ZEListTestItems")
		{
			ZEList<ZEListTestItem> NewList;
			ZEListTestItem* NewItem1 = new ZEListTestItem();
			ZEListTestItem* NewItem2 = new ZEListTestItem();
			ZEListTestItem* NewItem3 = new ZEListTestItem();
			NewList.Insert(NewItem1);
			NewList.Append(NewItem2);
			NewList.Append(NewItem3);
			ZE_TEST_CHECK_EQUAL(NewList[0], NewItem1);
			ZE_TEST_CHECK_EQUAL(NewList[1], NewItem2);
			ZE_TEST_CHECK_EQUAL(NewList[2], NewItem3);

			NewList.Remove(NewItem2);
			ZE_TEST_CHECK_EQUAL(NewList[0], NewItem1);
			ZE_TEST_CHECK_EQUAL(NewList[1], NewItem3);

			NewList.Remove(NewItem1);
			ZE_TEST_CHECK_EQUAL(NewList[0], NewItem3);

			NewList.Remove(NewItem3);
			ZE_TEST_CHECK_EQUAL(NewList.GetCount(), 0);
		}
 	}
 
	ZE_TEST_ITEM("inline void RemoveAll()")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;

		List.Insert(&Item1);
		List.Insert(&Item2);
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 2);

		List.RemoveAll();
  		ZE_TEST_CHECK_EQUAL(List.GetCount(), 0);
	}

	ZE_TEST_ITEM("inline void RemoveAt(ZESize Index)")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;

		List.Insert(&Item1);
		List.Append(&Item2);
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 2);
		ZE_TEST_CHECK_EQUAL(List.GetItem(0), &Item1);

		List.RemoveAt(0);
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 1);
		ZE_TEST_CHECK_EQUAL(List.GetItem(0), &Item2);

		ZE_TEST_CASE("remove second item")
		{
			ZEList<ZEListTestItem> List1;

			ZEListTestItem Item3;
			List1.Insert(&Item1);
			List1.Insert(&Item2);
			List1.Insert(&Item3);
			ZE_TEST_CHECK_EQUAL(List1.GetCount(), 3);
			ZE_TEST_CHECK_EQUAL(List1[0], &Item3);
			ZE_TEST_CHECK_EQUAL(List1[1], &Item2);
			ZE_TEST_CHECK_EQUAL(List1[2], &Item1);

			List1.RemoveAt(1);
			ZE_TEST_CHECK_EQUAL(List1.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(List1[0], &Item3);
			ZE_TEST_CHECK_EQUAL(List1[1], &Item1);

			List1.RemoveAt(1);
			ZE_TEST_CHECK_EQUAL(List1.GetCount(), 1);
			ZE_TEST_CHECK_EQUAL(List1[0], &Item3);
		}
 	}

	ZE_TEST_ITEM("ZEList()")
	{
		ZEList<ZEListTestItem> List;
		
		ZE_TEST_CHECK_EQUAL(List.GetCount(), 0);
		ZE_TEST_CHECK_EQUAL(List.GetFirstItem(), NULL);
		ZE_TEST_CHECK_EQUAL(List.GetLastItem(), NULL);
	}
}
