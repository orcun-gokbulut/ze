//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEListIteratorsTests.cpp
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

ZE_TEST(ZEListIterator)
{
	ZE_TEST_ITEM("inline bool IsEnd() const")
	{
		ZEList<ZEListTestItem> List;
		ZEListIterator<ZEListTestItem> Iterator = List.GetIterator();
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), NULL);

		bool End = Iterator.IsEnd();
		ZE_TEST_CHECK_ERROR(End);

		ZE_TEST_CASE("for false")
		{
			ZEListTestItem Item;
			List.Insert(&Item);

			Iterator = List.GetIterator();
			ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), &Item);

			End = Iterator.IsEnd();
			ZE_TEST_CHECK_ERROR(!End);

			ZEListItem* Next = Iterator.MoveNext();
			ZE_TEST_CHECK_EQUAL(Next, NULL);

			End = Iterator.IsEnd();
			ZE_TEST_CHECK_ERROR(End);
		}
	}

	ZE_TEST_ITEM("inline const ZEType* GetItem() const")
	{
		ZEList<ZEListTestItem> List;
		ZEListIterator<ZEListTestItem> Iterator = List.GetIterator();

		const ZEListTestItem* Result = Iterator.GetItem();
		ZE_TEST_CHECK_EQUAL(Result, NULL);

		ZE_TEST_CASE("for not an empty list")
		{
			ZEListTestItem Item1;
			List.Insert(&Item1);

			Iterator = List.GetIterator();

			Result = Iterator.GetItem();
			ZE_TEST_CHECK_EQUAL(Result, &Item1);

			ZEListTestItem Item2;
			List.Insert(&Item2);
			Iterator = List.GetIterator();

			Result = Iterator.GetItem();
			ZE_TEST_CHECK_EQUAL(Result, &Item2);

			ZEListTestItem Item3;
			List.Append(&Item3);
			Iterator = List.GetIterator();

			Result = Iterator.GetItem();
			ZE_TEST_CHECK_EQUAL(Result, &Item2);
		}
	}

	ZE_TEST_ITEM("inline ZEType* MovePrevious()")
	{
		ZEList<ZEListItem> List;
		ZEListTestItem Item1;
		List.Insert(&Item1);

		ZEListIterator<ZEListItem> Iterator = List.GetIterator();

		ZEListItem* Previous = Iterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, NULL);

		ZEListTestItem Item2;
		List.Append(&Item2);

		Iterator = List.GetIterator();

		Previous = Iterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, NULL);

		ZEListTestItem Item3;
		List.Append(&Item3);

		Iterator = List.GetIterator();

		Previous = Iterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, NULL);

		ZEListTestItem Item4;
		List.Insert(&Item4);

		Iterator = List.GetIterator();

		Previous = Iterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, NULL);
	}

	ZE_TEST_ITEM("inline ZEType* MoveNext()")
	{
		ZEList<ZEListItem> List;
		ZEListTestItem Item1;
		List.Insert(&Item1);

		ZEListIterator<ZEListItem> Iterator = List.GetIterator();

		ZEListItem* Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, NULL);

		ZEListTestItem Item2;
		List.Append(&Item2);

		Iterator = List.GetIterator();

		Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, &Item2);

		ZEListTestItem Item3;
		List.Insert(&Item3);

		Iterator = List.GetIterator();

		Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, &Item1);
		Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, &Item2);
		Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, NULL);

		ZE_TEST_CASE("MoveNext and MovePrevious")
		{
			ZEList<ZEListItem> List1;
			ZEListTestItem Item1;
			ZEListTestItem Item2;
			ZEListTestItem Item3;

			List1.Insert(&Item1);

			Iterator = List1.GetIterator();

			Next = Iterator.MoveNext();
			ZE_TEST_CHECK_EQUAL(Next, NULL);

			List1.Append(&Item2);

			Iterator = List1.GetIterator();
			Next = Iterator.MoveNext();
			ZE_TEST_CHECK_EQUAL(Next, &Item2);
			ZEListItem* Previous = Iterator.MovePrevious();
			ZE_TEST_CHECK_EQUAL(Previous, &Item1);

			List1.Append(&Item3);
			Iterator = List1.GetIterator();

			Next = Iterator.MoveNext();
			ZE_TEST_CHECK_EQUAL(Next, &Item2);
			Next = Iterator.MoveNext();
			ZE_TEST_CHECK_EQUAL(Next, &Item3);
			Previous = Iterator.MovePrevious();
			ZE_TEST_CHECK_EQUAL(Previous, &Item2);
			Previous = Iterator.MovePrevious();
			ZE_TEST_CHECK_EQUAL(Previous, &Item1);
			Previous = Iterator.MovePrevious();
			ZE_TEST_CHECK_EQUAL(Previous, NULL);
		}
	}

	ZE_TEST_ITEM("inline ZESize GetIndex()")
	{
		ZEList<ZEListItem> List;
		ZEListIterator<ZEListItem> Iterator = List.GetIterator();

		ZESize Index = Iterator.GetIndex();
		ZE_TEST_CHECK_EQUAL(Index, 0);

		ZE_TEST_CASE("for not an empty list")
		{
			ZEListTestItem Item1;
			List.Insert(&Item1);

			Iterator = List.GetIterator();

			Index = Iterator.GetIndex();
			ZE_TEST_CHECK_EQUAL(Index, 0);

			ZEListTestItem Item2;
			List.Insert(&Item2);

			Index = Iterator.GetIndex();
			ZE_TEST_CHECK_EQUAL(Index, 1);

			ZEListTestItem Item3;
			List.Insert(&Item3);

			Index = Iterator.GetIndex();
			ZE_TEST_CHECK_EQUAL(Index, 2);

			Iterator = List.GetIterator();
			Index = Iterator.GetIndex();
			ZE_TEST_CHECK_EQUAL(Index, 0);

			ZEListTestItem Item4;
			List.Append(&Item4);

			Index = Iterator.GetIndex();
			ZE_TEST_CHECK_EQUAL(Index, 0);
		}
	}

	ZE_TEST_ITEM("class ZEListIterator mixed test")
	{
		ZEList<ZEListTestItem> List;
		ZEListTestItem Item1;
		ZEListTestItem Item2;
		ZEListTestItem Item3;
		ZEListTestItem Item4;

		ZEListIterator<ZEListTestItem> Iterator = List.GetIterator();
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), NULL);

		List.Insert(&Item1);
		Iterator = List.GetIterator();
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), &Item1);

		List.Insert(&Item2);
		Iterator = List.GetIterator();
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), &Item2);

		ZEListTestItem* Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, &Item1);

		ZEListTestItem* Previous = Iterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, &Item2);
		Previous = Iterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, NULL);

		Iterator = List.GetIterator();
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), &Item2);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);

		List.Insert(&Item3);
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), &Item2);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 1);

		List.Insert(&Item4);
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), &Item2);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 2);

		Previous = Iterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, &Item3);
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), &Item3);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 1);

		Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, &Item2);
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), &Item2);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 2);

		Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, &Item1);
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), &Item1);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 3);
		ZE_TEST_CHECK_ERROR(!Iterator.IsEnd());

		Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, NULL);
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), NULL);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);
		ZE_TEST_CHECK_ERROR(Iterator.IsEnd());

		Iterator = List.GetIterator();
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), &Item4);
	}
}

ZE_TEST(ZEListIteratorConst)
{
	ZE_TEST_ITEM("inline bool IsEnd() const")
	{
		ZEList<ZEListTestItem> List;
		ZEListIteratorConst<ZEListTestItem> ConstIterator = List.GetConstIterator();
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), NULL);

		bool End = ConstIterator.IsEnd();
		ZE_TEST_CHECK_ERROR(End);

		ZE_TEST_CASE("for false")
		{
			ZEListTestItem Item;
			List.Insert(&Item);

			ConstIterator = List.GetConstIterator();
			ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), &Item);

			End = ConstIterator.IsEnd();
			ZE_TEST_CHECK_ERROR(!End);
		}
	}

	ZE_TEST_ITEM("inline const ZEType* GetItem() const")
	{
		ZEList<ZEListTestItem> List;
		ZEListIteratorConst<ZEListTestItem> ConstIterator = List.GetConstIterator();

		const ZEListTestItem* Result = ConstIterator.GetItem();
		ZE_TEST_CHECK_EQUAL(Result, NULL);

		ZE_TEST_CASE("for not an empty list")
		{
			ZEListTestItem Item1;
			List.Insert(&Item1);

			ConstIterator = List.GetConstIterator();

			Result = ConstIterator.GetItem();
			ZE_TEST_CHECK_EQUAL(Result, &Item1);

			ZEListTestItem Item2;
			List.Insert(&Item2);

			ConstIterator = List.GetConstIterator();

			Result = ConstIterator.GetItem();
			ZE_TEST_CHECK_EQUAL(Result, &Item2);

			ZEListTestItem Item3;
			List.Append(&Item3);

			ConstIterator = List.GetConstIterator();

			Result = ConstIterator.GetItem();
			ZE_TEST_CHECK_EQUAL(Result, &Item2);
		}
	}

	ZE_TEST_ITEM("inline ZESize GetIndex()")
	{
		 ZEList<ZEListTestItem> List;
		 ZEListIteratorConst<ZEListTestItem> ConstIterator = List.GetConstIterator();
		 
		 ZESize Index = ConstIterator.GetIndex();
		 ZE_TEST_CHECK_EQUAL(Index, 0);
		 
		 ZE_TEST_CASE("for not an empty list")
		 {
			ZEListTestItem Item1;
			List.Insert(&Item1);

		 	ConstIterator = List.GetConstIterator();
		 
			ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), &Item1);
		 	Index = ConstIterator.GetIndex();
		 	ZE_TEST_CHECK_EQUAL(Index, 0);
		 
			ZEListTestItem Item2;
			List.Insert(&Item2);
		 
		 	Index = ConstIterator.GetIndex();
		 	ZE_TEST_CHECK_EQUAL(Index, 1);

			ZEListTestItem Item3;
			List.Insert(&Item3);

			Index = ConstIterator.GetIndex();
			ZE_TEST_CHECK_EQUAL(Index, 2);

			ConstIterator = List.GetConstIterator();
			ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), &Item3);
			Index = ConstIterator.GetIndex();
			ZE_TEST_CHECK_EQUAL(Index, 0);
		  }
	}

	ZE_TEST_ITEM("inline const ZEType* MovePrevious()")
	{
		ZEList<ZEListTestItem> List;
		ZEListTestItem Item1;
		List.Insert(&Item1);

 		ZEListIteratorConst<ZEListTestItem> ConstIterator = List.GetConstIterator();
 
		const ZEListTestItem* Previous = ConstIterator.MovePrevious();
 		ZE_TEST_CHECK_EQUAL(Previous, NULL);
 
		ZEListTestItem Item2;
		List.Insert(&Item2);
 
		ConstIterator = List.GetConstIterator();
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), &Item2);

 		Previous = ConstIterator.MovePrevious();
  		ZE_TEST_CHECK_EQUAL(Previous, NULL);
	}

	ZE_TEST_ITEM("inline const ZEType* MoveNext()")
	{ 		
		ZEList<ZEListItem> List;
		ZEListTestItem Item1;
		List.Insert(&Item1);

		ZEListIteratorConst<ZEListItem> ConstIterator = List.GetConstIterator();
	
		const ZEListItem* Next = ConstIterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, NULL);

		ZEListTestItem Item2;
		List.Append(&Item2);
 
		ConstIterator = List.GetConstIterator();

		Next = ConstIterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, &Item2);
	}

	ZE_TEST_ITEM("class ZEListIteratorConst mixed test")
	{
		ZEList<ZEListTestItem> List;
		ZEListTestItem Item1;
		ZEListTestItem Item2;
		ZEListTestItem Item3;
		ZEListTestItem Item4;

		ZEListIteratorConst<ZEListTestItem> ConstIterator = List.GetConstIterator();
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), NULL);

		List.Insert(&Item1);
		ConstIterator = List.GetConstIterator();
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), &Item1);

		List.Insert(&Item2);
		ConstIterator = List.GetConstIterator();
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), &Item2);

		const ZEListTestItem* Next = ConstIterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, &Item1);

		const ZEListTestItem* Previous = ConstIterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, &Item2);
		Previous = ConstIterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, NULL);

		ConstIterator = List.GetConstIterator();
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), &Item2);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);

		List.Insert(&Item3);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), &Item2);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 1);

		List.Insert(&Item4);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), &Item2);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 2);

		Previous = ConstIterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, &Item3);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), &Item3);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 1);

		Next = ConstIterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, &Item2);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), &Item2);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 2);

		Next = ConstIterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, &Item1);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), &Item1);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 3);
		ZE_TEST_CHECK_ERROR(!ConstIterator.IsEnd());

		Next = ConstIterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, NULL);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), NULL);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);
		ZE_TEST_CHECK_ERROR(ConstIterator.IsEnd());

		ConstIterator = List.GetConstIterator();
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), &Item4);
	}
}
