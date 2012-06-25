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

};

ZETestSuite(ZEList)
{
	ZETest("~ZEList()")
	{
		
	}

	ZETest("inline ZEType* Append(ZEType* Item)")
	{
		ZEList<ZEListTestItem> List;	
		ZEListTestItem* Item = new ZEListTestItem(NULL, NULL);
		
		ZEListTestItem* Result = List.Append(Item);
   		ZETestCheck(Result == Item);
	
		ZETestCase("for FirstItem != NULL")
		{
 			ZEList<ZEListTestItem> List1;
			ZEListTestItem* Item1 = new ZEListTestItem(NULL, NULL);
			ZEListTestItem* Result1 = List1.Insert(Item1);
			ZETestCheckEqual(List1.GetFirstItem(), Item1);
			ZEListTestItem* Item2 = new ZEListTestItem(NULL, NULL);
			ZEListTestItem* Result2 = List1.Append(Item2);
			ZETestCheckEqual(List1.GetFirstItem(), Item1);
			ZETestCheckEqual(List1.GetLastItem(), Item2);
			ZEInt val = 1;
			ZEListTestItem* Item3 = new ZEListTestItem(reinterpret_cast<ZEListItem*> (&val), NULL);
			ZEListTestItem* Result3 = List1.Append(Item3);
			ZETestCheckEqual(List1.GetLastItem(), Item3);
			ZEInt val1 = 2;
			ZEInt val2 = 3;
			ZEListTestItem* Item4 = new ZEListTestItem(reinterpret_cast<ZEListItem*> (&val1), reinterpret_cast<ZEListItem*> (&val2));
			ZEListTestItem* Result4 = List1.Append(Item4);
			ZETestCheckEqual(List1.GetLastItem(), Item4);
 		}

		ZETestCase("for LastItem != NULL")
		{
			ZEList<ZEListTestItem> List2;
			ZEListTestItem* Value = new ZEListTestItem(NULL, NULL);
			ZETestCheckEqual(List2.GetLastItem(), NULL);
			ZEInt a = 1;
			ZEListTestItem* Value1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&a), NULL);
			ZEListTestItem* Res = List2.Append(Value1);
			ZEListTestItem* Last = List2.GetLastItem();
			ZEInt b = 3;
			ZEListTestItem* Item = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&b), NULL);
			Res = List2.Append(Item);
			ZETestCheckEqual(Res, Item);
			ZETestCheckEqual(List2.GetLastItem(), Res);
		}
	}

	ZETest("inline ZEType* Dequeue()")
	{
		ZEList<ZEListTestItem> List;
		ZEInt a = 1;
		ZEInt b = 2;
		ZEListTestItem* Item = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&a), NULL);
		ZEListTestItem* Item1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&b), NULL);
		ZEListTestItem* Res = List.Insert(Item);
		ZEListTestItem* Res1 = List.Insert(Item1);
		ZETestCheckEqual(List.GetFirstItem(), Item1);

		ZEListTestItem *Result = List.Dequeue();
 		ZETestCheckEqual(Result, Res1);
	}
 
	ZETest("inline void Enqueue(ZEType* Value)")
	{
		ZEList<ZEListTestItem> List;
		ZEInt val = 7;
		ZEListTestItem* Value = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&val), NULL);

		List.Enqueue(Value);
		ZETestCheckEqual(List.GetFirstItem(), Value);
		ZETestCheckEqual(List.GetLastItem(), Value);

		ZETestCase("for LastItem == NULL")
		{
			ZETestCheckEqual(List.GetLastItem(), Value);
			ZEInt val1 = 5;
			ZEListTestItem* Value1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&val1), NULL);

			List.Enqueue(Value1);
			ZETestCheckEqual(List.GetFirstItem(), Value);
			ZETestCheckEqual(List.GetLastItem(), Value1);
		}
	}
 
	ZETest("inline bool Exists(ZEType* Item) const")
 	{
		ZEList<ZEListTestItem> List;
		ZEInt Val = 1;
		ZEListTestItem* Item = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val), NULL);
		
		bool Exist = List.Exists(Item);
		ZETestCheck(!Exist);

		ZETestCase("for Cursor != NULL")
		{
			ZEInt a = 2;
			ZEListTestItem* Value = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&a), NULL);
			List.Append(Value);

			Exist = List.Exists(Item);
			ZETestCheck(!Exist);
			Exist = List.Exists(Value);
			ZETestCheck(Exist);
		}

		ZETestCase("for true")
		{
			ZEListTestItem* Item1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val), NULL);
			ZEListTestItem* Res = List.Append(Item1);
			ZEInt Value = 3;
			ZEListTestItem* Item2 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Value), NULL);
			List.Append(Item2);
			
			Exist = List.Exists(Item1);
 			ZETestCheck(Exist);
			Exist = List.Exists(Item2);
			ZETestCheck(Exist);
		}
 	}

	ZETest("inline ZESSize FindIndex(ZEType* Item, ZESize StartIndex = 0) const")
	{
		ZEList<ZEListTestItem> List;
		ZEInt Val1 = 1;
		ZEInt Val2 = 2;
		ZEInt Val3 = 3;
		ZEListTestItem* Item1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val1), NULL);
		ZEListTestItem* Item2 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val2), NULL);
		ZEListTestItem* Item3 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val3), NULL);
		List.Insert(0, Item1);
		List.Insert(1, Item2);
		List.Insert(2, Item3);
		ZETestCheckEqual(List[0], Item1);
		ZETestCheckEqual(List[1], Item2);
		ZETestCheckEqual(List[2], Item3);

		ZESSize Index = List.FindIndex(Item1);
		ZETestCheckEqual(Index, 0);
		Index = List.FindIndex(Item2);
		ZETestCheckEqual(Index, 1);
		Index = List.FindIndex(Item3);
		ZETestCheckEqual(Index, 2);
	}

	ZETest("ConstIterator GetConstIterator() const")
	{
		ZEList<ZEListTestItem> List;
		ZEInt a = 1;
		ZEInt b = 2;
		ZEListTestItem* A = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&a), NULL);
		ZEListTestItem* B = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&b), NULL);
		List.Insert(A);
		List.Append(B);

		const ZEListIteratorConst<ZEListTestItem> Item = List.GetConstIterator();
		
	}
 
	ZETest("inline ZESize GetCount() const")
	{
		ZEList<ZEListTestItem> List;
		ZESize count = List.GetCount();
		ZETestCheckEqual(count, 0);

		ZETestCase("for not empty list")
		{
			ZEInt val = 2;
			ZEListTestItem* Item = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&val), reinterpret_cast<ZEListTestItem*> (&val));
			List.Insert(Item);

			count = List.GetCount();
			ZETestCheckEqual(count, 1);
		}
	}
 
	ZETest("inline const ZEType* GetFirstItem() const")
	{
		ZEList<ZEListTestItem> List;
		const ZEListTestItem *First = List.GetFirstItem();
		ZETestCheckEqual(First, NULL);

		ZETestCase("for not empty list")
		{
			ZEInt a = 1;
			ZEListTestItem* Item1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&a), NULL);
			List.Insert(Item1);

			const ZEListTestItem *FirstValue = List.GetFirstItem();
			ZETestCheckEqual(FirstValue, Item1);
		}
 	}

	ZETest("inline ZEType* GetFirstItem()")
	{
		ZEList<ZEListTestItem> List;
		ZEListTestItem* First = List.GetFirstItem();
		ZETestCheckEqual(First, NULL);

		ZETestCase("for not empty list")
		{
			ZEInt a = 4;
			ZEListTestItem* Item = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&a), NULL);
			List.Insert(Item);

			ZEListTestItem *FirstValue = List.GetFirstItem();
			ZETestCheckEqual(FirstValue, Item);
		}
	}

	ZETest("inline const ZEType* GetItem(ZESize Index) const")
 	{
		ZEList<ZEListTestItem> List;
		ZEInt a = 5;
		ZEListTestItem* Item = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&a), NULL);
		List.Insert(Item);

		const ZEListTestItem* Result = List.GetItem(0);
		ZETestCheckEqual(Result, Item);		
 	}

	ZETest("inline ZEType* GetItem(ZESize Index)")
	{
		ZEList<ZEListTestItem> List;
		ZEListTestItem* i = List.GetItem(0);
		//index is out of range
		
		ZETestCase("for not empty list")
		{
			ZEInt Val1 = 2;
			ZEListTestItem* Item1 = new ZEListTestItem(NULL, reinterpret_cast<ZEListTestItem*> (&Val1));
			List.Insert(Item1);
			ZEInt Val2 = 4;
			ZEListTestItem* Item2 = new ZEListTestItem(NULL, reinterpret_cast<ZEListTestItem*> (&Val2));
			List.Insert(Item2);

			ZEListTestItem* Result = List.GetItem(0);
			ZETestCheckEqual(Result, Item2);
			Result = List.GetItem(1);
			ZETestCheckEqual(Result, Item1);
		}
  	}

	ZETest("Iterator GetIterator()")
	{
		ZEList<ZEListTestItem> List;
		ZEInt a = 12;
		ZEListTestItem* A = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&a), NULL);
 		List.Insert(A);
		ZEInt b = 3;
		ZEListTestItem* B = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&b), NULL);
		List.Insert(B);
		
		ZEListIterator<ZEListTestItem> Item = List.GetIterator();
		
	}
 
	ZETest("inline ZEType* GetLastItem()")
	{
		ZEList<ZEListTestItem> List;
		ZEListTestItem* Last = List.GetLastItem();
		ZETestCheckEqual(Last, NULL);

		ZETestCase("for not empty list")
		{
			ZEInt a = 8;
			ZEInt b = 10;
			ZEListTestItem* Value = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&a), reinterpret_cast<ZEListTestItem*> (&b));
			List.Insert(Value);

			Last = List.GetLastItem();
			ZETestCheckEqual(Last, Value);
		}

		ZETestCase("for List count > 1")
		{
			ZETestCheckEqual(List.GetCount(), 1);
			ZEInt c = 9;
			ZEListTestItem* Value1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&c), NULL);
			List.Append(Value1);
			ZETestCheckEqual(List.GetCount(), 2);

			Last = List.GetLastItem();
			ZETestCheckEqual(Last, Value1);
		}
	}
 
	ZETest("inline const ZEType* GetLastItem() const")
	{
		ZEList<ZEListTestItem> List;
		ZEListTestItem* Value = new ZEListTestItem(NULL, NULL);
		List.Insert(Value);

		const ZEListTestItem* Last = List.GetLastItem();
		ZETestCheckEqual(Last, Value);
		
		ZEInt a = 9;
		ZEListTestItem* Value1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Value1), NULL);
		List.Insert(Value1);
		Last = List.GetLastItem();
		ZETestCheckEqual(Last, Value);
		ZETestCheckEqual(List.GetFirstItem(), Value1);
	}

	ZETest("inline ZEType* Insert(ZEType* Item)")
	{
		ZEList<ZEListTestItem> List;
		ZETestCheckEqual(List.GetCount(), 0);
		ZEInt Value = 5;
		ZEListTestItem* Item = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Value), NULL);

 		ZEListTestItem* p = List.Insert(Item);
		ZETestCheckEqual(List.GetCount(), 1);
		ZETestCheckEqual(p, Item);

		ZETestCase("for item is already in list")
		{
			bool e = List.Exists(Item);
			ZEInt Val = 4;
			ZEListTestItem* Item1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val), NULL);
			List.Insert(Item1);
			List.Insert(Item);
			//Item is either already associated with another list or corrupt
			//Item is already added to list
			ZEInt Val1 = 3;
			ZEListTestItem* Item2 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val1), NULL);
			List.Append(Item2);
		}
	}
 
 	ZETest("inline ZEType* Insert(ZESize Index, ZEType* Item)")
 	{
		ZEList<ZEListTestItem> List;
		ZEInt Value1 = 7;
		ZEInt Value2 = 8;
		ZEInt Value3 = 9;
		ZEListTestItem* Item1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Value1), NULL);
		ZEListTestItem* Item2 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Value2), NULL);
		ZEListTestItem* Item3 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Value3), NULL);
			
		ZEListTestItem* Result1 = List.Insert(0, Item1);
		ZEListTestItem* Result2 = List.Insert(1, Item2);
		ZETestCheckEqual(List.GetCount(), 2);
		ZETestCheckEqual(List[0], Item1);
		ZETestCheckEqual(List[1], Item2);

		ZEListTestItem* Result3 = List.Insert(1, Item3);
		ZETestCheckEqual(List.GetCount(), 3);
		ZETestCheckEqual(List[1], Item3);
		ZETestCheckEqual(List[2], Item2);
		ZETestCheckEqual(List[0], Item1);	
	}
 
	ZETest("inline ZEType* operator[](ZESize Index)")
 	{
		ZEList<ZEListTestItem> List;
		ZEInt val1 = 1;
		ZEInt val2 = 2;
 		ZEInt val3 = 3;
		ZEListTestItem* Item1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&val1), NULL);
		ZEListTestItem* Item2 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&val2), NULL);
		ZEListTestItem* Item3 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&val3), NULL);
		List.Insert(Item1);
		List.Insert(Item2);
		List.Insert(Item3);

		ZEListTestItem* Result = List[0];
		ZETestCheckEqual(Result, Item3);
		Result = List[1];
		ZETestCheckEqual(Result, Item2);
		Result = List[2];
		ZETestCheckEqual(Result, Item1);
 	}

	ZETest("inline const ZEType* operator[](ZESize Index) const")
	{
		ZEList<ZEListTestItem> List;
		ZEInt val1 = 1;
		ZEInt val2 = 2;
		ZEInt val3 = 3;
		ZEListTestItem* Item1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&val1), NULL);
		ZEListTestItem* Item2 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&val2), NULL);
		ZEListTestItem* Item3 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&val3), NULL);
		List.Insert(0, Item1);
		List.Insert(1, Item2);
		List.Insert(2, Item3);

		const ZEListTestItem* Result = List[0];
		ZETestCheckEqual(Result, Item1);
		Result = List[1];
		ZETestCheckEqual(Result, Item2);
		Result = List[2];
		ZETestCheckEqual(Result, Item3);
	}
 
	ZETest("inline ZEType Pop()")
	{
		ZEList<ZEListTestItem> List;
		ZEInt val1 = 1;
		ZEInt val2 = 2;
		ZEListTestItem* Value1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&val1), NULL);
		ZEListTestItem* Value2 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&val2), NULL);
		List.Insert(Value1);
		List.Insert(Value2);
		ZETestCheckEqual(List.GetCount(), 2);
		ZETestCheckEqual(List[0], Value2);
		ZETestCheckEqual(List[1], Value1);

		ZEListTestItem* Value = List.Pop();
		ZETestCheckEqual(List.GetCount(), 1);
		ZETestCheckEqual(Value, Value2);
		ZETestCheckEqual(List[0], Value1);		
	}
 
	ZETest("inline void Push(ZEType* Value)")
	{
		ZEList<ZEListTestItem> List;
		ZETestCheckEqual(List.GetCount(), 0);
		ZETestCheckEqual(List.GetFirstItem(), NULL);
		ZEInt val = 3;
		ZEListTestItem* Value = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&val), NULL);

		List.Push(Value);
		ZETestCheckEqual(List.GetCount(), 1);
		ZETestCheckEqual(List.GetFirstItem(), Value);

		ZEInt val1 = 4;
		ZEListTestItem* Value1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&val1), NULL);

		List.Push(Value1);
		ZETestCheckEqual(List.GetCount(), 2);
		ZETestCheckEqual(List.GetFirstItem(), Value1);
		ZETestCheckEqual(List.GetLastItem(), Value);
	}

	ZETest("inline void Remove(ZEType* Item)")
 	{
		ZEList<ZEListTestItem> List;
		ZEInt Val1 = 1;
		ZEInt Val2 = 2;
		ZEListTestItem* Item1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val1), NULL);
		ZEListTestItem* Item2 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val2), NULL);
		List.Insert(Item1);
		List.Insert(Item2);
		ZETestCheckEqual(List.GetCount(), 2);
		ZETestCheckEqual(List[0], Item2);

		List.Remove(Item2);
 		ZETestCheckEqual(List.GetCount(), 1);
		ZETestCheckEqual(List[0], Item1);
		List.Remove(Item1);
		ZETestCheckEqual(List.GetCount(), 0);

		ZETestCase("")
		{
			ZEList<ZEListTestItem> List1;
			ZEInt Val3 = 3;
			ZEListTestItem* Item3 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val3), NULL);
			List1.Insert(Item1);
			List1.Insert(Item2);
			List1.Insert(Item3);
			ZETestCheckEqual(List1[0], Item3);
			ZETestCheckEqual(List1[1], Item2);
			ZETestCheckEqual(List1[2], Item1);

			List1.Remove(Item3);
			ZETestCheckEqual(List1[0], Item2);
			ZETestCheckEqual(List1[1], Item1);
			List1.Remove(Item2);
			ZETestCheckEqual(List1[0], Item1);
		}

		ZETestCase("insert and remove same item respectively to an empty list")
		{
			ZEList<ZEListTestItem> L;
			ZETestCheckEqual(L.GetFirstItem(), NULL);
			ZETestCheckEqual(L.GetLastItem(), NULL);
			ZEInt i = 5;
			ZEListTestItem* k = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&i), NULL);
			ZEListTestItem* p = L.Insert(k);
			ZETestCheckEqual(p, k);
			ZETestCheckEqual(L.GetFirstItem(), k);
			ZETestCheckEqual(L.GetLastItem(), k);
			
			L.Remove(k);
			ZETestCheckEqual(L.GetFirstItem(), NULL);
			ZETestCheckEqual(L.GetLastItem(), NULL);
			ZETestCheckEqual(L.GetCount(), 0);
		}
 	}
 
	ZETest("inline void RemoveAll()")
	{
		ZEList<ZEListTestItem> List;
		ZEInt Val1 = 1;
		ZEInt Val2 = 2;
		ZEListTestItem* Item1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val1), NULL);
		ZEListTestItem* Item2 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val2), NULL);
		List.Insert(Item1);
		List.Insert(Item2);
		ZETestCheckEqual(List.GetCount(), 2);

		List.RemoveAll();
  		ZETestCheckEqual(List.GetCount(), 0);
	}

	ZETest("inline void RemoveAt(ZESize Index)")
	{
		ZEList<ZEListTestItem> List;
		ZEInt Val1 = 3;
		ZEInt Val2 = 5;
		ZEListTestItem* Item1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val1), NULL);
		ZEListTestItem* Item2 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val2), NULL);
		List.Insert(Item1);
		List.Append(Item2);
		ZETestCheckEqual(List.GetCount(), 2);
		ZETestCheckEqual(List.GetItem(0), Item1);

		List.RemoveAt(0);
		ZETestCheckEqual(List.GetCount(), 1);
		ZETestCheckEqual(List.GetItem(0), Item2);
 	}

	ZETest("ZEList()")
	{
		ZEList<ZEListTestItem> List;
		
		ZETestCheckEqual(List.GetCount(), 0);
		ZETestCheckEqual(List.GetFirstItem(), NULL);
		ZETestCheckEqual(List.GetLastItem(), NULL);
	}


	//ZEListIterators Tests//
//class ZEListIterator

	ZETest("inline bool IsEnd() const")
	{
		ZEList<ZEListTestItem> List;
		ZEListIterator<ZEListTestItem> Item = List.GetIterator();

		bool End = Item.IsEnd();
		ZETestCheck(End);

		ZETestCase("for false")
		{
			ZEInt Val = 1;
			ZEListTestItem* Value  = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val), NULL);
			List.Insert(Value);
			Item = List.GetIterator();

			End = Item.IsEnd();
			ZETestCheck(!End);
		}
	}

	ZETest("inline const ZEType* GetItem() const")
	{
		ZEList<ZEListTestItem> List;
		ZEListIterator<ZEListTestItem> Item = List.GetIterator();

		const ZEListTestItem* Result = Item.GetItem();
		ZETestCheckEqual(Result, NULL);

		ZETestCase("for not an empty list")
		{
			ZEInt Val1 = 1;
			ZEListTestItem* Value1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val1), NULL);
			List.Insert(Value1);
			Item = List.GetIterator();

			Result = Item.GetItem();
			ZETestCheckEqual(Result, Value1);

			ZEInt Val2 = 2;
			ZEListTestItem* Value2 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val2), NULL);
			List.Insert(Value2);
			Item = List.GetIterator();

			Result = Item.GetItem();
			ZETestCheckEqual(Result, Value2);
		}
	}

	ZETest("inline ZEType* MovePrevious()")
	{
		ZEList<ZEListItem> List;
		ZEInt Val1 = 1;
		ZEListTestItem* Value1 = new ZEListTestItem(reinterpret_cast<ZEListItem*> (&Val1), NULL);
		List.Insert(Value1);
		ZEListIterator<ZEListItem> Item = List.GetIterator();

		ZEListItem* Previous = Item.MovePrevious();
		ZETestCheckEqual(Previous, NULL);

		ZEInt Val2 = 2;
		ZEListTestItem* Value2 = new ZEListTestItem(reinterpret_cast<ZEListItem*> (&Val2), NULL);
		List.Append(Value2);
		Item = List.GetIterator();

		Previous = Item.MovePrevious();
		ZETestCheckEqual(Previous, NULL);
	}

	ZETest("inline ZEType* MoveNext()")
	{
		ZEList<ZEListItem> List;
		ZEInt Val1 = 1;
		ZEListTestItem* Value1 = new ZEListTestItem(reinterpret_cast<ZEListItem*> (&Val1), NULL);
		List.Insert(Value1);
		ZEListIterator<ZEListItem> Item = List.GetIterator();

		ZEListItem* Next = Item.MoveNext();
		ZETestCheckEqual(Next, NULL);

		ZEInt Val2 = 2;
		ZEListTestItem* Value2 = new ZEListTestItem(reinterpret_cast<ZEListItem*> (&Val2), NULL);
		List.Append(Value2);
		Item = List.GetIterator();

		Next = Item.MoveNext();
		ZETestCheckEqual(Next, Value2);
	}

	ZETest("inline ZESize GetIndex()")
	{
		ZEList<ZEListItem> List;
		ZEListIterator<ZEListItem> Item = List.GetIterator();

		ZESize Index = Item.GetIndex();
		ZETestCheckEqual(Index, 0);

		ZETestCase("for not an empty list")
		{
			ZEInt Val1 = 1;
			ZEListTestItem* Value1 = new ZEListTestItem(reinterpret_cast<ZEListItem*> (&Val1), NULL);
			List.Insert(Value1);
			Item = List.GetIterator();

			Index = Item.GetIndex();
			ZETestCheckEqual(Index, 1);

			ZEInt Val2 = 2;
			ZEListTestItem* Value2 = new ZEListTestItem(reinterpret_cast<ZEListItem*> (&Val2), NULL);
			List.Insert(Value2);

			Index = Item.GetIndex();
			ZETestCheckEqual(Index, 2);
		}
	}

//class ZEListIteratorConst

	ZETest("inline bool IsEnd() const")
	{
		ZEList<ZEListTestItem> List;
		const ZEListIteratorConst<ZEListTestItem> Item = List.GetConstIterator();

		bool End = Item.IsEnd();
		ZETestCheck(End);

		ZETestCase("for false")
		{
			ZEInt Val = 1;
			ZEListTestItem* Value = new ZEListTestItem(reinterpret_cast<ZEListItem*> (&Val), NULL);
			List.Insert(Value);
			const ZEListIteratorConst<ZEListTestItem> Item = List.GetConstIterator();

			End = Item.IsEnd();
			ZETestCheck(!End);
		}
	}

	ZETest("inline const ZEType* GetItem() const")
	{
		ZEList<ZEListTestItem> List;
		const ZEListIteratorConst<ZEListTestItem> Item = List.GetConstIterator();

		const ZEListTestItem* Result = Item.GetItem();
		ZETestCheckEqual(Result, NULL);

		ZETestCase("for not an empty list")
		{
			ZEInt Val1 = 1;
			ZEListTestItem* Value1 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val1), NULL);
			List.Insert(Value1);
			const ZEListIteratorConst<ZEListTestItem> Item = List.GetConstIterator();

			Result = Item.GetItem();
			ZETestCheckEqual(Result, Value1);

			ZEInt Val2 = 2;
			ZEListTestItem* Value2 = new ZEListTestItem(reinterpret_cast<ZEListTestItem*> (&Val2), NULL);
			List.Insert(Value2);
			const ZEListIteratorConst<ZEListTestItem> Item1 = List.GetConstIterator();

			Result = Item1.GetItem();
			ZETestCheckEqual(Result, Value2);
		}
	}

	ZETest("inline ZESize GetIndex()")
	{
// 		ZEList<ZEListItem> List;
// 		ZEListIteratorConst<ZEListItem> Item = List.GetConstIterator();
// 
// 		ZESize Index = Item.GetIndex();
// 		ZETestCheckEqual(Index, 0);
// 
// 		ZETestCase("for not an empty list")
// 		{
// 			ZEInt Val1 = 1;
// 			ZEListTestItem* Value1 = new ZEListTestItem(reinterpret_cast<ZEListItem*> (&Val1), NULL);
// 			List.Insert(Value1);
// 			ZEListIteratorConst<ZEListItem> Item = List.GetConstIterator();
// 
// 			Index = Item.GetIndex();
// 			ZETestCheckEqual(Index, 1);
// 
// 			ZEInt Val2 = 2;
// 			ZEListTestItem* Value2 = new ZEListTestItem(reinterpret_cast<ZEListItem*> (&Val2), NULL);
// 			List.Insert(Value2);
// 
// 			Index = Item.GetIndex();
// 			ZETestCheckEqual(Index, 2);
//  		}
	}

	ZETest("inline const ZEType* MovePrevious() const")
	{
// 		ZEList<ZEListTestItem> List;
// 		ZEInt Val1 = 1;
// 		ZEListTestItem* Value1 = new ZEListTestItem(reinterpret_cast<ZEListItem*> (&Val1), NULL);
// 		List.Insert(Value1);
// 		const ZEListIteratorConst<ZEListTestItem> Item = List.GetConstIterator();
// 
// 		const ZEListTestItem* Previous = Item.MovePrevious();
// 		ZETestCheckEqual(Previous, NULL);
// 
// 		ZEInt Val2 = 2;
// 		ZEListTestItem* Value2 = new ZEListTestItem(reinterpret_cast<ZEListItem*> (&Val2), NULL);
// 		List.Append(Value2);
// 
// 		Previous = Item.MovePrevious();
//  		ZETestCheckEqual(Previous, NULL);
	}

	ZETest("inline const ZEType* MoveNext() const")
	{
// 		ZEList<ZEListItem> List;
// 		ZEInt Val1 = 1;
// 		ZEListTestItem* Value1 = new ZEListTestItem(reinterpret_cast<ZEListItem*> (&Val1), NULL);
// 		List.Insert(Value1);
// 		const ZEListIteratorConst<ZEListItem> Item = List.GetConstIterator();
// 
// 		const ZEListItem* Next = Item.MoveNext();
// 		ZETestCheckEqual(Next, NULL);
// 
// 		ZEInt Val2 = 2;
// 		ZEListTestItem* Value2 = new ZEListTestItem(reinterpret_cast<ZEListItem*> (&Val2), NULL);
// 		List.Append(Value2);
// 		const ZEListIteratorConst<ZEListItem> Item1 = List.GetConstIterator();
// 
// 		Next = Item.MoveNext();
// 		ZETestCheckEqual(Next, Value2);
	}
}
