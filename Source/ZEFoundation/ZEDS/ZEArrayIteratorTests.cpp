//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEArrayIteratorTests.cpp
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

ZE_TEST(ZEArrayIterator)
{
	ZE_TEST_ITEM("inline bool IsEnd() const")
	{
		ZEArray<ZEInt32> Array;
		ZEArrayIterator<ZEInt32, ZEAllocatorBase<ZEInt32>> Iterator = Array.GetIterator();
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);

		bool End = Iterator.IsEnd();
		ZE_TEST_CHECK_ERROR(End);

		ZE_TEST_CASE("for false")
		{
			Array.Add(1);
			Array.Add(2);
			Array.Add(3);
			Iterator = Array.GetIterator();
			ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);

			End = Iterator.IsEnd();
			ZE_TEST_CHECK_ERROR(!End);

			Iterator.MoveNext();
			ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 1);
			End = Iterator.IsEnd();
			ZE_TEST_CHECK_ERROR(!End);

			Iterator.MoveNext();
			ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 2);
			End = Iterator.IsEnd();
			ZE_TEST_CHECK_ERROR(End);

			Iterator.MoveNext();
			ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 2);
			End = Iterator.IsEnd();
			ZE_TEST_CHECK_ERROR(End);
		}
	}

	ZE_TEST_ITEM("inline ZEType* GetItem()")
	{
		ZEArray<ZEInt64> Array;
		ZEArrayIterator<ZEInt64, ZEAllocatorBase<ZEInt64>> Iterator = Array.GetIterator();

		ZEInt64* Item = Iterator.GetItem();
		//error Index is out of range.
		ZE_TEST_CHECK_EQUAL(Item, NULL);

		Array.Add(81985529216486895);
		Array.Add(11564982415960004267);
		Iterator = Array.GetIterator();

		Item = Iterator.GetItem();
		ZE_TEST_CHECK_EQUAL(*Item, 81985529216486895);
	}

	ZE_TEST_ITEM("inline ZEType* MovePrevious()")
	{
		ZEArray<ZEInt> Array;
		ZEArrayIterator<ZEInt, ZEAllocatorBase<ZEInt>> Iterator = Array.GetIterator();
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), NULL);
		//error Index is out of range.
		ZE_TEST_CHECK_ERROR(Iterator.IsEnd());

		ZEInt* Previous = Iterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, NULL);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), NULL);
		//error Index is out of range.
		ZE_TEST_CHECK_ERROR(Iterator.IsEnd());

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 1);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);

		Previous = Iterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, NULL);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 1);

		Previous = Iterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, NULL);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 1);

		ZEInt* Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(*Next, 2);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 1);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 2);

		Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(*Next, 3);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 2);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 3);

		Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, NULL);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 2);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 3);

		Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, NULL);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 2);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 3);

		Previous = Iterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(*Previous, 2);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 1);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 2);

		Previous = Iterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(*Previous, 1);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 1);

		Previous = Iterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, NULL);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 1);
	}

	ZE_TEST_ITEM("inline ZEType* MoveNext()")
	{
		ZEArray<ZEInt> Array;
		ZEArrayIterator<ZEInt, ZEAllocatorBase<ZEInt>> Iterator = Array.GetIterator();
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), NULL);
		//error Index is out of range.
		ZE_TEST_CHECK_ERROR(Iterator.IsEnd());

		ZEInt* Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, NULL);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), NULL);
		//error Index is out of range.
		ZE_TEST_CHECK_ERROR(Iterator.IsEnd());

		Array.Add(1);
		Array.Add(2);

		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 1);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);
		ZE_TEST_CHECK_ERROR(!Iterator.IsEnd());

		Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(*Next, 2);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 1);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 2);
		ZE_TEST_CHECK_ERROR(Iterator.IsEnd());

		Array.Add(3);

		Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(*Next, 3);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 2);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 3);
		ZE_TEST_CHECK_ERROR(Iterator.IsEnd());

		Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, NULL);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 2);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 3);
		ZE_TEST_CHECK_ERROR(Iterator.IsEnd());
	}

	ZE_TEST_ITEM("inline ZESize GetIndex()")
	{
		ZEArray<ZEInt8> Array;
		ZEArrayIterator<ZEInt8, ZEAllocatorBase<ZEInt8>> Iterator = Array.GetIterator();

		ZESize Index = Iterator.GetIndex();
		ZE_TEST_CHECK_EQUAL(Index, 0);

		Array.Add('z');
		Array.Add('e');
		Array.Add('Z');

		Index = Iterator.GetIndex();
		ZE_TEST_CHECK_EQUAL(Index, 0);

		Iterator.MoveNext();
		Index = Iterator.GetIndex();
		ZE_TEST_CHECK_EQUAL(Index, 1);

		Iterator.MoveNext();
		Index = Iterator.GetIndex();
		ZE_TEST_CHECK_EQUAL(Index, 2);
	}

	ZE_TEST_ITEM("class ZEArrayIterator mixed test")
	{
		ZEArray<ZEInt> Array;
		ZEArrayIterator<ZEInt, ZEAllocatorBase<ZEInt>> Iterator = Array.GetIterator();
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), NULL);
		//error Index is out of range.
		ZE_TEST_CHECK_ERROR(Iterator.IsEnd());

		Array.Add(0);
		Array.Add(1);
		Array.Add(2);

		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 0);
		ZE_TEST_CHECK_ERROR(!Iterator.IsEnd());

		ZEInt* Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(*Next, 1);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 1);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 1);
		ZE_TEST_CHECK_ERROR(!Iterator.IsEnd());

		Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(*Next, 2);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 2);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 2);
		ZE_TEST_CHECK_ERROR(Iterator.IsEnd());

		Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, NULL);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 2);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 2);
		ZE_TEST_CHECK_ERROR(Iterator.IsEnd());

		Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, NULL);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 2);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 2);
		ZE_TEST_CHECK_ERROR(Iterator.IsEnd());

		ZEInt* Prev = Iterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(*Prev, 1);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 1);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 1);
		ZE_TEST_CHECK_ERROR(!Iterator.IsEnd());

		Prev = Iterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(*Prev, 0);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 0);
		ZE_TEST_CHECK_ERROR(!Iterator.IsEnd());

		Prev = Iterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Prev, NULL);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 0);
		ZE_TEST_CHECK_ERROR(!Iterator.IsEnd());

		Prev = Iterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Prev, NULL);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 0);
		ZE_TEST_CHECK_ERROR(!Iterator.IsEnd());

		Next = Iterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(*Next, 1);
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 1);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 1);
		ZE_TEST_CHECK_ERROR(!Iterator.IsEnd());
	}
}

ZE_TEST(ZEArrayIteratorConst)
{
	ZE_TEST_ITEM("inline bool IsEnd() const")
	{
		ZEArray<ZEString> Array;
		ZEArrayIteratorConst<ZEString, ZEAllocatorBase<ZEString>> ConstIterator = Array.GetConstIterator();
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);

		bool End = ConstIterator.IsEnd();
		ZE_TEST_CHECK_ERROR(End);

		ZE_TEST_CASE("for false")
		{
			Array.Add("New");
			Array.Add("Test");
			Array.Add("String");
			ConstIterator = Array.GetConstIterator();
			ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);

			End = ConstIterator.IsEnd();
			ZE_TEST_CHECK_ERROR(!End);

			ConstIterator.MoveNext();
			ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 1);
			End = ConstIterator.IsEnd();
			ZE_TEST_CHECK_ERROR(!End);

			ConstIterator.MoveNext();
			ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 2);
			End = ConstIterator.IsEnd();
			ZE_TEST_CHECK_ERROR(End);

			ConstIterator.MoveNext();
			ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 2);
			End = ConstIterator.IsEnd();
			ZE_TEST_CHECK_ERROR(End);
		}
	}

	ZE_TEST_ITEM("inline const ZEType* GetItem() const")
	{
		ZEArray<ZEInt16> Array;
		ZEArrayIteratorConst<ZEInt16, ZEAllocatorBase<ZEInt16>> ConstIterator = Array.GetConstIterator();

		const ZEInt16* Item = ConstIterator.GetItem();
		//error Index is out of range.
		ZE_TEST_CHECK_EQUAL(Item, NULL);

		Array.Add(1);
		Array.Add(2);
		ConstIterator = Array.GetConstIterator();

		Item = ConstIterator.GetItem();
		ZE_TEST_CHECK_EQUAL(*Item, 1);
	}

	ZE_TEST_ITEM("inline const ZEType* MovePrevious() const")
	{
		ZEArray<ZEInt> Array;
		ZEArrayIteratorConst<ZEInt, ZEAllocatorBase<ZEInt>> ConstIterator = Array.GetConstIterator();
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), NULL);
		//error Index is out of range.

		const ZEInt* Previous = ConstIterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, NULL);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), NULL);
		//error Index is out of range.
		ZE_TEST_CHECK_ERROR(ConstIterator.IsEnd());

		Array.Add(1);
		Array.Add(2);

		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 1);

		Previous = ConstIterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, NULL);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 1);

		Array.Add(3);

		Previous = ConstIterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, NULL);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 1);
		ZE_TEST_CHECK_ERROR(!ConstIterator.IsEnd());

		const ZEInt* Next = ConstIterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(*Next, 2);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 1);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 2);
		ZE_TEST_CHECK_ERROR(!ConstIterator.IsEnd());

		Next = ConstIterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(*Next, 3);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 2);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 3);
		ZE_TEST_CHECK_ERROR(ConstIterator.IsEnd());

		Next = ConstIterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, NULL);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 2);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 3);
		ZE_TEST_CHECK_ERROR(ConstIterator.IsEnd());

		Previous = ConstIterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(*Previous, 2);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 1);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 2);
		ZE_TEST_CHECK_ERROR(!ConstIterator.IsEnd());

		Previous = ConstIterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(*Previous, 1);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 1);
		ZE_TEST_CHECK_ERROR(!ConstIterator.IsEnd());

		Previous = ConstIterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Previous, NULL);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 1);
		ZE_TEST_CHECK_ERROR(!ConstIterator.IsEnd());
	}

	ZE_TEST_ITEM("inline const ZEType* MoveNext() const")
	{
		ZEArray<ZEInt> Array;
		ZEArrayIteratorConst<ZEInt, ZEAllocatorBase<ZEInt>> ConstIterator = Array.GetConstIterator();
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), NULL);
		//error Index is out of range.

		const ZEInt* Next = ConstIterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, NULL);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), NULL);
		//error Index is out of range.
		ZE_TEST_CHECK_ERROR(ConstIterator.IsEnd());

		Array.Add(1);
		Array.Add(2);

		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 1);

		Next = ConstIterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(*Next, 2);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 1);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 2);
		ZE_TEST_CHECK_ERROR(ConstIterator.IsEnd());

		Array.Add(3);

		Next = ConstIterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(*Next, 3);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 2);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 3);
		ZE_TEST_CHECK_ERROR(ConstIterator.IsEnd());

		Next = ConstIterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, NULL);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 2);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 3);
		ZE_TEST_CHECK_ERROR(ConstIterator.IsEnd());
	}

	ZE_TEST_ITEM("inline ZESize GetIndex()")
	{
		ZEArray<const char*> Array;
		ZEArrayIteratorConst<const char*, ZEAllocatorBase<const char*>> ConstIterator = Array.GetConstIterator();

		ZESize Index = ConstIterator.GetIndex();
		ZE_TEST_CHECK_EQUAL(Index, 0);

		Array.Add("z");
		Array.Add("e");

		Index = ConstIterator.GetIndex();
		ZE_TEST_CHECK_EQUAL(Index, 0);

		ConstIterator.MoveNext();

		Index = ConstIterator.GetIndex();
		ZE_TEST_CHECK_EQUAL(Index, 1);

		ConstIterator.MoveNext();

		Index = ConstIterator.GetIndex();
		ZE_TEST_CHECK_EQUAL(Index, 1);
	}

	ZE_TEST_ITEM("class ZEArrayIteratorConst mixed test")
	{
		ZEArray<ZEInt> Array;
		ZEArrayIteratorConst<ZEInt, ZEAllocatorBase<ZEInt>> ConstIterator = Array.GetConstIterator();
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetItem(), NULL);
		//error Index is out of range.
		ZE_TEST_CHECK_ERROR(ConstIterator.IsEnd());

		Array.Add(0);
		Array.Add(1);
		Array.Add(2);

		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 0);
		ZE_TEST_CHECK_ERROR(!ConstIterator.IsEnd());

		const ZEInt* Prev = ConstIterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Prev, NULL);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 0);
		ZE_TEST_CHECK_ERROR(!ConstIterator.IsEnd());

		Prev = ConstIterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(Prev, NULL);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 0);
		ZE_TEST_CHECK_ERROR(!ConstIterator.IsEnd());

		const ZEInt* Next = ConstIterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(*Next, 1);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 1);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 1);
		ZE_TEST_CHECK_ERROR(!ConstIterator.IsEnd());

		Next = ConstIterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(*Next, 2);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 2);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 2);
		ZE_TEST_CHECK_ERROR(ConstIterator.IsEnd());

		Next = ConstIterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, NULL);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 2);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 2);
		ZE_TEST_CHECK_ERROR(ConstIterator.IsEnd());

		Next = ConstIterator.MoveNext();
		ZE_TEST_CHECK_EQUAL(Next, NULL);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 2);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 2);
		ZE_TEST_CHECK_ERROR(ConstIterator.IsEnd());

		Prev = ConstIterator.MovePrevious();
		ZE_TEST_CHECK_EQUAL(*Prev, 1);
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 1);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 1);
		ZE_TEST_CHECK_ERROR(!ConstIterator.IsEnd());
	}
}
