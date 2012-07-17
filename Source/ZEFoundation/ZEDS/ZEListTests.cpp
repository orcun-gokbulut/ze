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

ZETestSuite(ZEList)
{
	ZETest("~ZEList()")
	{
		ZEList<ZEListTestItem> List;
		ZEListTestItem Item1;
		ZEListTestItem Item2;
		ZEListTestItem Item3;

		List.Append(&Item1);
		List.Append(&Item2);
		List.Append(&Item3);
		ZETestCheckEqual(List.GetCount(), 3);

		List.~ZEList();
		ZETestCheckEqual(List.GetCount(), 0);
	}

	ZETest("inline ZEType* Append(ZEType* Item)")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item;

		ZEListTestItem* Result = List.Append(&Item);
		ZETestCheck(Result == &Item);
	
		ZETestCase("for FirstItem != NULL")
        {
 			ZEList<ZEListTestItem> List1;
			ZEListTestItem Item1;
			Result = List1.Insert(&Item1);
			ZETestCheckEqual(List1.GetFirstItem(), &Item1);

			ZEListTestItem Item2;
			Result = List1.Append(&Item2);
			ZETestCheckEqual(List1.GetFirstItem(), &Item1);
			ZETestCheckEqual(List1.GetLastItem(), &Item2);

			ZEListTestItem Item3;
			Result = List1.Append(&Item3);
			ZETestCheckEqual(List1.GetLastItem(), &Item3);

			ZEListTestItem Item4;
			Result = List1.Insert(&Item4);
			ZETestCheckEqual(List1.GetLastItem(), &Item3);
			ZETestCheckEqual(List1.GetFirstItem(), &Item4);
 		}
	}

	ZETest("inline ZEType* Dequeue()")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;

		ZEListTestItem* Item = List.Insert(&Item1);
		Item = List.Insert(&Item2);
		ZETestCheckEqual(List.GetFirstItem(), &Item2);
		ZETestCheckEqual(List.GetCount(), 2);

		ZEListTestItem* Result = List.Dequeue();
		ZETestCheckEqual(Result, &Item2);
		ZETestCheckEqual(List.GetCount(), 1);
	}
 
	ZETest("inline void Enqueue(ZEType* Value)")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;

		List.Enqueue(&Item1);
		ZETestCheckEqual(List.GetLastItem(), &Item1);
		ZETestCheckEqual(List.GetCount(), 1);

		ZETestCase("for LastItem == NULL")
		{
			ZETestCheckEqual(List.GetLastItem(), &Item1);
			
			ZEListTestItem Item2;

			List.Enqueue(&Item2);
			ZETestCheckEqual(List.GetFirstItem(), &Item1);
			ZETestCheckEqual(List.GetLastItem(), &Item2);
			ZETestCheckEqual(List.GetCount(), 2);
		}
	}
 
	ZETest("inline bool Exists(ZEType* Item) const")
 	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item;
		
		bool Exist = List.Exists(&Item);
		ZETestCheck(!Exist);

		ZETestCase("for Cursor != NULL")
		{
			ZEListTestItem Item1;
			ZEListTestItem* Result = List.Append(&Item1);

			Exist = List.Exists(&Item);
			ZETestCheck(!Exist);
			Exist = List.Exists(&Item1);
			ZETestCheck(Exist);
		}

		ZETestCase("for true")
		{
			ZEListTestItem Item2;
			ZEListTestItem Item3;
			ZEListTestItem* Result = List.Append(&Item2);
			Result = List.Append(&Item3);

			Exist = List.Exists(&Item2);
			ZETestCheck(Exist);
			Exist = List.Exists(&Item3);
			ZETestCheck(Exist);
		}
 	}

	ZETest("inline ZESSize FindIndex(ZEType* Item, ZESize StartIndex = 0) const")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;
		ZEListTestItem Item3;

		ZEListTestItem* Item = List.Insert(0, &Item1);
		Item = List.Insert(1, &Item2);
		Item = List.Insert(2, &Item3);

		ZETestCheckEqual(List[0], &Item1);
		ZETestCheckEqual(List[1], &Item2);
		ZETestCheckEqual(List[2], &Item3);

		ZESSize Index = List.FindIndex(&Item1);
		ZETestCheckEqual(Index, 0);
		Index = List.FindIndex(&Item2);
		ZETestCheckEqual(Index, 1);
		Index = List.FindIndex(&Item3);
		ZETestCheckEqual(Index, 2);
	}

	ZETest("ConstIterator GetConstIterator() const")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;

		ZEListTestItem* Item = List.Insert(&Item1);
		Item = List.Append(&Item2);

		const ZEListIteratorConst<ZEListTestItem> ConstIterator = List.GetConstIterator();
		ZETestCheckEqual(ConstIterator.GetItem(), &Item1);
	}
 
	ZETest("inline ZESize GetCount() const")
	{
		ZEList<ZEListTestItem> List;
		ZESize Count = List.GetCount();
		ZETestCheckEqual(Count, 0);

		ZETestCase("for not empty list")
		{
			ZEListTestItem Item1;
			ZEListTestItem Item2;

			List.Insert(&Item1);

			Count = List.GetCount();
			ZETestCheckEqual(Count, 1);

			List.Insert(&Item2);

			Count = List.GetCount();
			ZETestCheckEqual(Count, 2);
		}
	}
 
	ZETest("inline const ZEType* GetFirstItem() const")
	{
		ZEList<ZEListTestItem> List;
		const ZEListTestItem *First = List.GetFirstItem();
		ZETestCheckEqual(First, NULL);

		ZETestCase("for not empty list")
		{
			ZEListTestItem Item1;
			ZEListTestItem Item2;
		
			List.Insert(&Item1);

			First = List.GetFirstItem();
			ZETestCheckEqual(First, &Item1);

			List.Insert(&Item2);

			First = List.GetFirstItem();
			ZETestCheckEqual(First, &Item2);
		}
 	}

	ZETest("inline ZEType* GetFirstItem()")
	{
		ZEList<ZEListTestItem> List;
		ZEListTestItem* First = List.GetFirstItem();
		ZETestCheckEqual(First, NULL);

		ZETestCase("for not empty list")
		{
			ZEListTestItem Item1;
			ZEListTestItem Item2;

			List.Insert(&Item1);

			First = List.GetFirstItem();
			ZETestCheckEqual(First, &Item1);

			List.Insert(&Item2);

			First = List.GetFirstItem();
			ZETestCheckEqual(First, &Item2);
		}
	}

	ZETest("inline const ZEType* GetItem(ZESize Index) const")
 	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;

		List.Insert(&Item1);

		const ZEListTestItem* Result = List.GetItem(0);
		ZETestCheckEqual(Result, &Item1);

		List.Insert(&Item2);

		Result = List.GetItem(0);
		ZETestCheckEqual(Result, &Item2);
		Result = List.GetItem(1);
		ZETestCheckEqual(Result, &Item1);
 	}

	ZETest("inline ZEType* GetItem(ZESize Index)")
	{
		ZEList<ZEListTestItem> List;
		ZEListTestItem* Result = List.GetItem(0);
		//index is out of range
		
		ZETestCase("for not empty list")
		{
			ZEListTestItem Item1;
			ZEListTestItem Item2;

			List.Insert(&Item1);

			Result = List.GetItem(0);
			ZETestCheckEqual(Result, &Item1);

			List.Insert(&Item2);

			Result = List.GetItem(0);
			ZETestCheckEqual(Result, &Item2);
			Result = List.GetItem(1);
			ZETestCheckEqual(Result, &Item1);
		}
  	}

	ZETest("Iterator GetIterator()")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;

		ZEListTestItem* Item = List.Insert(&Item1);
		Item = List.Insert(&Item2);
		
		ZEListIterator<ZEListTestItem> Iterator = List.GetIterator();
		ZETestCheckEqual(Iterator.GetItem(), &Item2);
	}
 
	ZETest("inline ZEType* GetLastItem()")
	{
		ZEList<ZEListTestItem> List;
		ZEListTestItem* Last = List.GetLastItem();
		ZETestCheckEqual(Last, NULL);

		ZETestCase("for not empty list")
		{
			ZEListTestItem Item;
	
			List.Insert(&Item);

			Last = List.GetLastItem();
			ZETestCheckEqual(Last, &Item);
		}

		ZETestCase("for List count > 1")
		{
			ZETestCheckEqual(List.GetCount(), 1);
			
			ZEListTestItem Item1;
			List.Append(&Item1);
			ZETestCheckEqual(List.GetCount(), 2);

			Last = List.GetLastItem();
			ZETestCheckEqual(Last, &Item1);
		}
	}
 
	ZETest("inline const ZEType* GetLastItem() const")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		List.Insert(&Item1);

		const ZEListTestItem* Last = List.GetLastItem();
		ZETestCheckEqual(Last, &Item1);
		
		ZEListTestItem Item2;
		List.Insert(&Item2);

		Last = List.GetLastItem();
		ZETestCheckEqual(Last, &Item1);
		ZETestCheckEqual(List.GetFirstItem(), &Item2);
	}

	ZETest("inline ZEType* Insert(ZEType* Item)")
	{
		ZEList<ZEListTestItem> List;

		ZETestCheckEqual(List.GetCount(), 0);
		
		ZEListTestItem Item;

 		ZEListTestItem* Result = List.Insert(&Item);
		ZETestCheckEqual(List.GetCount(), 1);
		ZETestCheckEqual(Result, &Item);
		ZETestCheckEqual(List.GetFirstItem(), Result);

		ZETestCase("for item is already in list")
		{
			bool Exist = List.Exists(&Item);
			ZETestCheck(Exist);

			ZEListTestItem Item1;
			Result = List.Insert(&Item1);
			ZETestCheckEqual(List.GetCount(), 2);
			ZETestCheckEqual(Result, &Item1);
			ZETestCheckEqual(List.GetFirstItem(), Result);

			List.Insert(&Item);
			//Item is either already associated with another list or corrupt
			//Item is already added to list
		}
	}
 
 	ZETest("inline ZEType* Insert(ZESize Index, ZEType* Item)")
 	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;
		ZEListTestItem Item3;
			
		ZEListTestItem* Result1 = List.Insert(0, &Item1);
		ZEListTestItem* Result2 = List.Insert(1, &Item2);
		ZETestCheckEqual(List.GetCount(), 2);
		ZETestCheckEqual(List[0], &Item1);
		ZETestCheckEqual(Result1, &Item1);
		ZETestCheckEqual(List[1], &Item2);
		ZETestCheckEqual(Result2, &Item2);

		ZEListTestItem* Result3 = List.Insert(1, &Item3);
		ZETestCheckEqual(Result3, &Item3);
		ZETestCheckEqual(List.GetCount(), 3);
		ZETestCheckEqual(List[1], &Item3);
		ZETestCheckEqual(List[2], &Item2);
		ZETestCheckEqual(List[0], &Item1);	
	}
 
	ZETest("inline ZEType* operator[](ZESize Index)")
 	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;
		ZEListTestItem Item3;

		List.Insert(&Item1);
		List.Insert(&Item2);
		List.Insert(&Item3);

		ZEListTestItem* Result = List[0];
		ZETestCheckEqual(Result, &Item3);

		Result = List[1];
		ZETestCheckEqual(Result, &Item2);

		Result = List[2];
		ZETestCheckEqual(Result, &Item1);
 	}

	ZETest("inline const ZEType* operator[](ZESize Index) const")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;
		ZEListTestItem Item3;
		List.Insert(0, &Item1);
		List.Insert(1, &Item2);
		List.Insert(2, &Item3);

		const ZEListTestItem* Result = List[0];
		ZETestCheckEqual(Result, &Item1);

		Result = List[1];
		ZETestCheckEqual(Result, &Item2);

		Result = List[2];
		ZETestCheckEqual(Result, &Item3);
	}
 
	ZETest("inline ZEType Pop()")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;

		List.Insert(&Item1);
		List.Insert(&Item2);
		ZETestCheckEqual(List.GetCount(), 2);
		ZETestCheckEqual(List[0], &Item2);
		ZETestCheckEqual(List[1], &Item1);

		ZEListTestItem* Value = List.Pop();
		ZETestCheckEqual(List.GetCount(), 1);
		ZETestCheckEqual(Value, &Item2);
		ZETestCheckEqual(List[0], &Item1);		
	}
 
	ZETest("inline void Push(ZEType* Value)")
	{
		ZEList<ZEListTestItem> List;

		ZETestCheckEqual(List.GetCount(), 0);
		ZETestCheckEqual(List.GetFirstItem(), NULL);
	
		ZEListTestItem Item1;

		List.Push(&Item1);
		ZETestCheckEqual(List.GetCount(), 1);
		ZETestCheckEqual(List.GetFirstItem(), &Item1);

		
		ZEListTestItem Item2;

		List.Push(&Item2);
		ZETestCheckEqual(List.GetCount(), 2);
		ZETestCheckEqual(List.GetFirstItem(), &Item2);
		ZETestCheckEqual(List.GetLastItem(), &Item1);
	}

	ZETest("inline void Remove(ZEType* Item)")
 	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;

		List.Insert(&Item1);
		List.Insert(&Item2);
		ZETestCheckEqual(List.GetCount(), 2);
		ZETestCheckEqual(List[0], &Item2);

		List.Remove(&Item2);
 		ZETestCheckEqual(List.GetCount(), 1);
		ZETestCheckEqual(List[0], &Item1);

		List.Remove(&Item1);
		ZETestCheckEqual(List.GetCount(), 0);

		ZETestCase("for Count > 2")
		{
			ZEList<ZEListTestItem> List1;
			
			ZEListTestItem Item3;
			List1.Insert(&Item1);
			List1.Insert(&Item2);
			List1.Insert(&Item3);
			ZETestCheckEqual(List1.GetCount(), 3);
			ZETestCheckEqual(List1[0], &Item3);
			ZETestCheckEqual(List1[1], &Item2);
			ZETestCheckEqual(List1[2], &Item1);

			List1.Remove(&Item3);
			ZETestCheckEqual(List1.GetCount(), 2);
			ZETestCheckEqual(List1[0], &Item2);
			ZETestCheckEqual(List1[1], &Item1);

			List1.Remove(&Item2);
			ZETestCheckEqual(List1.GetCount(), 1);
			ZETestCheckEqual(List1[0], &Item1);
		}

		ZETestCase("for Count > 2, remove last item first")
		{
			ZEList<ZEListTestItem> List1;

			ZEListTestItem Item3;
			List1.Insert(&Item1);
			List1.Insert(&Item2);
			List1.Insert(&Item3);
			ZETestCheckEqual(List1.GetCount(), 3);
			ZETestCheckEqual(List1[0], &Item3);
			ZETestCheckEqual(List1[1], &Item2);
			ZETestCheckEqual(List1[2], &Item1);

			List1.Remove(&Item1);
			ZETestCheckEqual(List1.GetCount(), 2);
			ZETestCheckEqual(List1[0], &Item3);
			ZETestCheckEqual(List1[1], &Item2);

			List1.Remove(&Item3);
			ZETestCheckEqual(List1.GetCount(), 1);
			ZETestCheckEqual(List1[0], &Item2);
		}

		ZETestCase("insert and remove same item respectively to an empty list")
		{
			ZEList<ZEListTestItem> List2;
			ZETestCheckEqual(List2.GetFirstItem(), NULL);
			ZETestCheckEqual(List2.GetLastItem(), NULL);
			
			ZEListTestItem Item;
			ZEListTestItem* Result = List2.Insert(&Item);
			ZETestCheckEqual(Result, &Item);
			ZETestCheckEqual(List2.GetFirstItem(), &Item);
			ZETestCheckEqual(List2.GetLastItem(), &Item);
			
			List2.Remove(&Item);
			ZETestCheckEqual(List2.GetFirstItem(), NULL);
			ZETestCheckEqual(List2.GetLastItem(), NULL);
			ZETestCheckEqual(List2.GetCount(), 0);
		}
 	}
 
	ZETest("inline void RemoveAll()")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;

		List.Insert(&Item1);
		List.Insert(&Item2);
		ZETestCheckEqual(List.GetCount(), 2);

		List.RemoveAll();
  		ZETestCheckEqual(List.GetCount(), 0);
	}

	ZETest("inline void RemoveAt(ZESize Index)")
	{
		ZEList<ZEListTestItem> List;

		ZEListTestItem Item1;
		ZEListTestItem Item2;

		List.Insert(&Item1);
		List.Append(&Item2);
		ZETestCheckEqual(List.GetCount(), 2);
		ZETestCheckEqual(List.GetItem(0), &Item1);

		List.RemoveAt(0);
		ZETestCheckEqual(List.GetCount(), 1);
		ZETestCheckEqual(List.GetItem(0), &Item2);

		ZETestCase("remove second item")
		{
			ZEList<ZEListTestItem> List1;

			ZEListTestItem Item3;
			List1.Insert(&Item1);
			List1.Insert(&Item2);
			List1.Insert(&Item3);
			ZETestCheckEqual(List1.GetCount(), 3);
			ZETestCheckEqual(List1[0], &Item3);
			ZETestCheckEqual(List1[1], &Item2);
			ZETestCheckEqual(List1[2], &Item1);

			List1.RemoveAt(1);
			ZETestCheckEqual(List1.GetCount(), 2);
			ZETestCheckEqual(List1[0], &Item3);
			ZETestCheckEqual(List1[1], &Item1);

			List1.RemoveAt(1);
			ZETestCheckEqual(List1.GetCount(), 1);
			ZETestCheckEqual(List1[0], &Item3);
		}
 	}

	ZETest("ZEList()")
	{
		ZEList<ZEListTestItem> List;
		
		ZETestCheckEqual(List.GetCount(), 0);
		ZETestCheckEqual(List.GetFirstItem(), NULL);
		ZETestCheckEqual(List.GetLastItem(), NULL);
	}
}
