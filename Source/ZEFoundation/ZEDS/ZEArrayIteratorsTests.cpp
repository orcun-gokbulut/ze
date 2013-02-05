//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEArrayIteratorsTests.cpp
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

#include "ZEDS/ZEArray.h"
#include "ZETest/ZETest.h"
#include "ZETest/ZETestCheck.h"

ZETestSuite(ZEArrayIterator)
{
	ZETest("inline bool IsEnd() const")
	{
		ZEArray<ZEInt32> Array;
		ZEArrayIterator<ZEInt32, ZEAllocatorBase<ZEInt32>> Iterator = Array.GetIterator();
		ZETestCheckEqual(Iterator.GetIndex(), 0);

		bool End = Iterator.IsEnd();
		ZETestCheck(End);

		ZETestCase("for false")
		{
			Array.Add(1);
			Array.Add(2);
			Array.Add(3);
			Iterator = Array.GetIterator();
			ZETestCheckEqual(Iterator.GetIndex(), 0);

			End = Iterator.IsEnd();
			ZETestCheck(!End);

			Iterator.MoveNext();
			ZETestCheckEqual(Iterator.GetIndex(), 1);
			End = Iterator.IsEnd();
			ZETestCheck(!End);

			Iterator.MoveNext();
			ZETestCheckEqual(Iterator.GetIndex(), 2);
			End = Iterator.IsEnd();
			ZETestCheck(End);

			Iterator.MoveNext();
			ZETestCheckEqual(Iterator.GetIndex(), 2);
			End = Iterator.IsEnd();
			ZETestCheck(End);
		}
	}

	ZETest("inline ZEType* GetItem()")
	{
		ZEArray<ZEInt64> Array;
		ZEArrayIterator<ZEInt64, ZEAllocatorBase<ZEInt64>> Iterator = Array.GetIterator();

		ZEInt64* Item = Iterator.GetItem();
		//error Index is out of range.
		ZETestCheckEqual(Item, NULL);

		Array.Add(81985529216486895);
		Array.Add(11564982415960004267);
		Iterator = Array.GetIterator();

		Item = Iterator.GetItem();
		ZETestCheckEqual(*Item, 81985529216486895);
	}

	ZETest("inline ZEType* MovePrevious()")
	{
		ZEArray<ZEInt> Array;
		ZEArrayIterator<ZEInt, ZEAllocatorBase<ZEInt>> Iterator = Array.GetIterator();
		ZETestCheckEqual(Iterator.GetIndex(), 0);
		ZETestCheckEqual(Iterator.GetItem(), NULL);
		//error Index is out of range.
		ZETestCheck(Iterator.IsEnd());

		ZEInt* Previous = Iterator.MovePrevious();
		ZETestCheckEqual(Previous, NULL);
		ZETestCheckEqual(Iterator.GetIndex(), 0);
		ZETestCheckEqual(Iterator.GetItem(), NULL);
		//error Index is out of range.
		ZETestCheck(Iterator.IsEnd());

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZETestCheckEqual(*Iterator.GetItem(), 1);
		ZETestCheckEqual(Iterator.GetIndex(), 0);

		Previous = Iterator.MovePrevious();
		ZETestCheckEqual(Previous, NULL);
		ZETestCheckEqual(Iterator.GetIndex(), 0);
		ZETestCheckEqual(*Iterator.GetItem(), 1);

		Previous = Iterator.MovePrevious();
		ZETestCheckEqual(Previous, NULL);
		ZETestCheckEqual(Iterator.GetIndex(), 0);
		ZETestCheckEqual(*Iterator.GetItem(), 1);

		ZEInt* Next = Iterator.MoveNext();
		ZETestCheckEqual(*Next, 2);
		ZETestCheckEqual(Iterator.GetIndex(), 1);
		ZETestCheckEqual(*Iterator.GetItem(), 2);

		Next = Iterator.MoveNext();
		ZETestCheckEqual(*Next, 3);
		ZETestCheckEqual(Iterator.GetIndex(), 2);
		ZETestCheckEqual(*Iterator.GetItem(), 3);

		Next = Iterator.MoveNext();
		ZETestCheckEqual(Next, NULL);
		ZETestCheckEqual(Iterator.GetIndex(), 2);
		ZETestCheckEqual(*Iterator.GetItem(), 3);

		Next = Iterator.MoveNext();
		ZETestCheckEqual(Next, NULL);
		ZETestCheckEqual(Iterator.GetIndex(), 2);
		ZETestCheckEqual(*Iterator.GetItem(), 3);

		Previous = Iterator.MovePrevious();
		ZETestCheckEqual(*Previous, 2);
		ZETestCheckEqual(Iterator.GetIndex(), 1);
		ZETestCheckEqual(*Iterator.GetItem(), 2);

		Previous = Iterator.MovePrevious();
		ZETestCheckEqual(*Previous, 1);
		ZETestCheckEqual(Iterator.GetIndex(), 0);
		ZETestCheckEqual(*Iterator.GetItem(), 1);

		Previous = Iterator.MovePrevious();
		ZETestCheckEqual(Previous, NULL);
		ZETestCheckEqual(Iterator.GetIndex(), 0);
		ZETestCheckEqual(*Iterator.GetItem(), 1);
	}

	ZETest("inline ZEType* MoveNext()")
	{
		ZEArray<ZEInt> Array;
		ZEArrayIterator<ZEInt, ZEAllocatorBase<ZEInt>> Iterator = Array.GetIterator();
		ZETestCheckEqual(Iterator.GetIndex(), 0);
		ZETestCheckEqual(Iterator.GetItem(), NULL);
		//error Index is out of range.
		ZETestCheck(Iterator.IsEnd());

		ZEInt* Next = Iterator.MoveNext();
		ZETestCheckEqual(Next, NULL);
		ZETestCheckEqual(Iterator.GetIndex(), 0);
		ZETestCheckEqual(Iterator.GetItem(), NULL);
		//error Index is out of range.
		ZETestCheck(Iterator.IsEnd());

		Array.Add(1);
		Array.Add(2);

		ZETestCheckEqual(*Iterator.GetItem(), 1);
		ZETestCheckEqual(Iterator.GetIndex(), 0);
		ZETestCheck(!Iterator.IsEnd());

		Next = Iterator.MoveNext();
		ZETestCheckEqual(*Next, 2);
		ZETestCheckEqual(Iterator.GetIndex(), 1);
		ZETestCheckEqual(*Iterator.GetItem(), 2);
		ZETestCheck(Iterator.IsEnd());

		Array.Add(3);

		Next = Iterator.MoveNext();
		ZETestCheckEqual(*Next, 3);
		ZETestCheckEqual(Iterator.GetIndex(), 2);
		ZETestCheckEqual(*Iterator.GetItem(), 3);
		ZETestCheck(Iterator.IsEnd());

		Next = Iterator.MoveNext();
		ZETestCheckEqual(Next, NULL);
		ZETestCheckEqual(Iterator.GetIndex(), 2);
		ZETestCheckEqual(*Iterator.GetItem(), 3);
		ZETestCheck(Iterator.IsEnd());
	}

	ZETest("inline ZESize GetIndex()")
	{
		ZEArray<ZEInt8> Array;
		ZEArrayIterator<ZEInt8, ZEAllocatorBase<ZEInt8>> Iterator = Array.GetIterator();

		ZESize Index = Iterator.GetIndex();
		ZETestCheckEqual(Index, 0);

		Array.Add('z');
		Array.Add('e');
		Array.Add('Z');

		Index = Iterator.GetIndex();
		ZETestCheckEqual(Index, 0);

		Iterator.MoveNext();
		Index = Iterator.GetIndex();
		ZETestCheckEqual(Index, 1);

		Iterator.MoveNext();
		Index = Iterator.GetIndex();
		ZETestCheckEqual(Index, 2);
	}

	ZETest("class ZEArrayIterator mixed test")
	{
		ZEArray<ZEInt> Array;
		ZEArrayIterator<ZEInt, ZEAllocatorBase<ZEInt>> Iterator = Array.GetIterator();
		ZETestCheckEqual(Iterator.GetIndex(), 0);
		ZETestCheckEqual(Iterator.GetItem(), NULL);
		//error Index is out of range.
		ZETestCheck(Iterator.IsEnd());

		Array.Add(0);
		Array.Add(1);
		Array.Add(2);

		ZETestCheckEqual(Iterator.GetIndex(), 0);
		ZETestCheckEqual(*Iterator.GetItem(), 0);
		ZETestCheck(!Iterator.IsEnd());

		ZEInt* Next = Iterator.MoveNext();
		ZETestCheckEqual(*Next, 1);
		ZETestCheckEqual(Iterator.GetIndex(), 1);
		ZETestCheckEqual(*Iterator.GetItem(), 1);
		ZETestCheck(!Iterator.IsEnd());

		Next = Iterator.MoveNext();
		ZETestCheckEqual(*Next, 2);
		ZETestCheckEqual(Iterator.GetIndex(), 2);
		ZETestCheckEqual(*Iterator.GetItem(), 2);
		ZETestCheck(Iterator.IsEnd());

		Next = Iterator.MoveNext();
		ZETestCheckEqual(Next, NULL);
		ZETestCheckEqual(Iterator.GetIndex(), 2);
		ZETestCheckEqual(*Iterator.GetItem(), 2);
		ZETestCheck(Iterator.IsEnd());

		Next = Iterator.MoveNext();
		ZETestCheckEqual(Next, NULL);
		ZETestCheckEqual(Iterator.GetIndex(), 2);
		ZETestCheckEqual(*Iterator.GetItem(), 2);
		ZETestCheck(Iterator.IsEnd());

		ZEInt* Prev = Iterator.MovePrevious();
		ZETestCheckEqual(*Prev, 1);
		ZETestCheckEqual(Iterator.GetIndex(), 1);
		ZETestCheckEqual(*Iterator.GetItem(), 1);
		ZETestCheck(!Iterator.IsEnd());

		Prev = Iterator.MovePrevious();
		ZETestCheckEqual(*Prev, 0);
		ZETestCheckEqual(Iterator.GetIndex(), 0);
		ZETestCheckEqual(*Iterator.GetItem(), 0);
		ZETestCheck(!Iterator.IsEnd());

		Prev = Iterator.MovePrevious();
		ZETestCheckEqual(Prev, NULL);
		ZETestCheckEqual(Iterator.GetIndex(), 0);
		ZETestCheckEqual(*Iterator.GetItem(), 0);
		ZETestCheck(!Iterator.IsEnd());

		Prev = Iterator.MovePrevious();
		ZETestCheckEqual(Prev, NULL);
		ZETestCheckEqual(Iterator.GetIndex(), 0);
		ZETestCheckEqual(*Iterator.GetItem(), 0);
		ZETestCheck(!Iterator.IsEnd());

		Next = Iterator.MoveNext();
		ZETestCheckEqual(*Next, 1);
		ZETestCheckEqual(Iterator.GetIndex(), 1);
		ZETestCheckEqual(*Iterator.GetItem(), 1);
		ZETestCheck(!Iterator.IsEnd());
	}
}

ZETestSuite(ZEArrayIteratorConst)
{
	ZETest("inline bool IsEnd() const")
	{
		ZEArray<ZEString> Array;
		ZEArrayIteratorConst<ZEString, ZEAllocatorBase<ZEString>> ConstIterator = Array.GetConstIterator();
		ZETestCheckEqual(ConstIterator.GetIndex(), 0);

		bool End = ConstIterator.IsEnd();
		ZETestCheck(End);

		ZETestCase("for false")
		{
			Array.Add("New");
			Array.Add("Test");
			Array.Add("String");
			ConstIterator = Array.GetConstIterator();
			ZETestCheckEqual(ConstIterator.GetIndex(), 0);

			End = ConstIterator.IsEnd();
			ZETestCheck(!End);

			ConstIterator.MoveNext();
			ZETestCheckEqual(ConstIterator.GetIndex(), 1);
			End = ConstIterator.IsEnd();
			ZETestCheck(!End);

			ConstIterator.MoveNext();
			ZETestCheckEqual(ConstIterator.GetIndex(), 2);
			End = ConstIterator.IsEnd();
			ZETestCheck(End);

			ConstIterator.MoveNext();
			ZETestCheckEqual(ConstIterator.GetIndex(), 2);
			End = ConstIterator.IsEnd();
			ZETestCheck(End);
		}
	}

	ZETest("inline const ZEType* GetItem() const")
	{
		ZEArray<ZEInt16> Array;
		ZEArrayIteratorConst<ZEInt16, ZEAllocatorBase<ZEInt16>> ConstIterator = Array.GetConstIterator();

		const ZEInt16* Item = ConstIterator.GetItem();
		//error Index is out of range.
		ZETestCheckEqual(Item, NULL);

		Array.Add(1);
		Array.Add(2);
		ConstIterator = Array.GetConstIterator();

		Item = ConstIterator.GetItem();
		ZETestCheckEqual(*Item, 1);
	}

	ZETest("inline const ZEType* MovePrevious() const")
	{
		ZEArray<ZEInt> Array;
		ZEArrayIteratorConst<ZEInt, ZEAllocatorBase<ZEInt>> ConstIterator = Array.GetConstIterator();
		ZETestCheckEqual(ConstIterator.GetIndex(), 0);
		ZETestCheckEqual(ConstIterator.GetItem(), NULL);
		//error Index is out of range.

		const ZEInt* Previous = ConstIterator.MovePrevious();
		ZETestCheckEqual(Previous, NULL);
		ZETestCheckEqual(ConstIterator.GetIndex(), 0);
		ZETestCheckEqual(ConstIterator.GetItem(), NULL);
		//error Index is out of range.
		ZETestCheck(ConstIterator.IsEnd());

		Array.Add(1);
		Array.Add(2);

		ZETestCheckEqual(ConstIterator.GetIndex(), 0);
		ZETestCheckEqual(*ConstIterator.GetItem(), 1);

		Previous = ConstIterator.MovePrevious();
		ZETestCheckEqual(Previous, NULL);
		ZETestCheckEqual(ConstIterator.GetIndex(), 0);
		ZETestCheckEqual(*ConstIterator.GetItem(), 1);

		Array.Add(3);

		Previous = ConstIterator.MovePrevious();
		ZETestCheckEqual(Previous, NULL);
		ZETestCheckEqual(ConstIterator.GetIndex(), 0);
		ZETestCheckEqual(*ConstIterator.GetItem(), 1);
		ZETestCheck(!ConstIterator.IsEnd());

		const ZEInt* Next = ConstIterator.MoveNext();
		ZETestCheckEqual(*Next, 2);
		ZETestCheckEqual(ConstIterator.GetIndex(), 1);
		ZETestCheckEqual(*ConstIterator.GetItem(), 2);
		ZETestCheck(!ConstIterator.IsEnd());

		Next = ConstIterator.MoveNext();
		ZETestCheckEqual(*Next, 3);
		ZETestCheckEqual(ConstIterator.GetIndex(), 2);
		ZETestCheckEqual(*ConstIterator.GetItem(), 3);
		ZETestCheck(ConstIterator.IsEnd());

		Next = ConstIterator.MoveNext();
		ZETestCheckEqual(Next, NULL);
		ZETestCheckEqual(ConstIterator.GetIndex(), 2);
		ZETestCheckEqual(*ConstIterator.GetItem(), 3);
		ZETestCheck(ConstIterator.IsEnd());

		Previous = ConstIterator.MovePrevious();
		ZETestCheckEqual(*Previous, 2);
		ZETestCheckEqual(ConstIterator.GetIndex(), 1);
		ZETestCheckEqual(*ConstIterator.GetItem(), 2);
		ZETestCheck(!ConstIterator.IsEnd());

		Previous = ConstIterator.MovePrevious();
		ZETestCheckEqual(*Previous, 1);
		ZETestCheckEqual(ConstIterator.GetIndex(), 0);
		ZETestCheckEqual(*ConstIterator.GetItem(), 1);
		ZETestCheck(!ConstIterator.IsEnd());

		Previous = ConstIterator.MovePrevious();
		ZETestCheckEqual(Previous, NULL);
		ZETestCheckEqual(ConstIterator.GetIndex(), 0);
		ZETestCheckEqual(*ConstIterator.GetItem(), 1);
		ZETestCheck(!ConstIterator.IsEnd());
	}

	ZETest("inline const ZEType* MoveNext() const")
	{
		ZEArray<ZEInt> Array;
		ZEArrayIteratorConst<ZEInt, ZEAllocatorBase<ZEInt>> ConstIterator = Array.GetConstIterator();
		ZETestCheckEqual(ConstIterator.GetIndex(), 0);
		ZETestCheckEqual(ConstIterator.GetItem(), NULL);
		//error Index is out of range.

		const ZEInt* Next = ConstIterator.MoveNext();
		ZETestCheckEqual(Next, NULL);
		ZETestCheckEqual(ConstIterator.GetIndex(), 0);
		ZETestCheckEqual(ConstIterator.GetItem(), NULL);
		//error Index is out of range.
		ZETestCheck(ConstIterator.IsEnd());

		Array.Add(1);
		Array.Add(2);

		ZETestCheckEqual(ConstIterator.GetIndex(), 0);
		ZETestCheckEqual(*ConstIterator.GetItem(), 1);

		Next = ConstIterator.MoveNext();
		ZETestCheckEqual(*Next, 2);
		ZETestCheckEqual(ConstIterator.GetIndex(), 1);
		ZETestCheckEqual(*ConstIterator.GetItem(), 2);
		ZETestCheck(ConstIterator.IsEnd());

		Array.Add(3);

		Next = ConstIterator.MoveNext();
		ZETestCheckEqual(*Next, 3);
		ZETestCheckEqual(ConstIterator.GetIndex(), 2);
		ZETestCheckEqual(*ConstIterator.GetItem(), 3);
		ZETestCheck(ConstIterator.IsEnd());

		Next = ConstIterator.MoveNext();
		ZETestCheckEqual(Next, NULL);
		ZETestCheckEqual(ConstIterator.GetIndex(), 2);
		ZETestCheckEqual(*ConstIterator.GetItem(), 3);
		ZETestCheck(ConstIterator.IsEnd());
	}

	ZETest("inline ZESize GetIndex()")
	{
		ZEArray<const char*> Array;
		ZEArrayIteratorConst<const char*, ZEAllocatorBase<const char*>> ConstIterator = Array.GetConstIterator();

		ZESize Index = ConstIterator.GetIndex();
		ZETestCheckEqual(Index, 0);

		Array.Add("z");
		Array.Add("e");

		Index = ConstIterator.GetIndex();
		ZETestCheckEqual(Index, 0);

		ConstIterator.MoveNext();

		Index = ConstIterator.GetIndex();
		ZETestCheckEqual(Index, 1);

		ConstIterator.MoveNext();

		Index = ConstIterator.GetIndex();
		ZETestCheckEqual(Index, 1);
	}

	ZETest("class ZEArrayIteratorConst mixed test")
	{
		ZEArray<ZEInt> Array;
		ZEArrayIteratorConst<ZEInt, ZEAllocatorBase<ZEInt>> ConstIterator = Array.GetConstIterator();
		ZETestCheckEqual(ConstIterator.GetIndex(), 0);
		ZETestCheckEqual(ConstIterator.GetItem(), NULL);
		//error Index is out of range.
		ZETestCheck(ConstIterator.IsEnd());

		Array.Add(0);
		Array.Add(1);
		Array.Add(2);

		ZETestCheckEqual(ConstIterator.GetIndex(), 0);
		ZETestCheckEqual(*ConstIterator.GetItem(), 0);
		ZETestCheck(!ConstIterator.IsEnd());

		const ZEInt* Prev = ConstIterator.MovePrevious();
		ZETestCheckEqual(Prev, NULL);
		ZETestCheckEqual(ConstIterator.GetIndex(), 0);
		ZETestCheckEqual(*ConstIterator.GetItem(), 0);
		ZETestCheck(!ConstIterator.IsEnd());

		Prev = ConstIterator.MovePrevious();
		ZETestCheckEqual(Prev, NULL);
		ZETestCheckEqual(ConstIterator.GetIndex(), 0);
		ZETestCheckEqual(*ConstIterator.GetItem(), 0);
		ZETestCheck(!ConstIterator.IsEnd());

		const ZEInt* Next = ConstIterator.MoveNext();
		ZETestCheckEqual(*Next, 1);
		ZETestCheckEqual(ConstIterator.GetIndex(), 1);
		ZETestCheckEqual(*ConstIterator.GetItem(), 1);
		ZETestCheck(!ConstIterator.IsEnd());

		Next = ConstIterator.MoveNext();
		ZETestCheckEqual(*Next, 2);
		ZETestCheckEqual(ConstIterator.GetIndex(), 2);
		ZETestCheckEqual(*ConstIterator.GetItem(), 2);
		ZETestCheck(ConstIterator.IsEnd());

		Next = ConstIterator.MoveNext();
		ZETestCheckEqual(Next, NULL);
		ZETestCheckEqual(ConstIterator.GetIndex(), 2);
		ZETestCheckEqual(*ConstIterator.GetItem(), 2);
		ZETestCheck(ConstIterator.IsEnd());

		Next = ConstIterator.MoveNext();
		ZETestCheckEqual(Next, NULL);
		ZETestCheckEqual(ConstIterator.GetIndex(), 2);
		ZETestCheckEqual(*ConstIterator.GetItem(), 2);
		ZETestCheck(ConstIterator.IsEnd());

		Prev = ConstIterator.MovePrevious();
		ZETestCheckEqual(*Prev, 1);
		ZETestCheckEqual(ConstIterator.GetIndex(), 1);
		ZETestCheckEqual(*ConstIterator.GetItem(), 1);
		ZETestCheck(!ConstIterator.IsEnd());
	}
}
