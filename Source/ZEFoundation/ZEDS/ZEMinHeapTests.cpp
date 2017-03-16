//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMinHeapTests.cpp
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

#include "ZETest/ZETest.h"
#include "ZEMinHeap.h"

bool ZE_TEST_CHECK_ERRORHeapTree(ZEMinHeap<ZEInt> Heap, ZESize Index = 0)
{
	if (Heap.GetFirstChildIndex(Index) < Heap.GetCount())
	{
		if (Heap.GetFirstChild(Index) > Heap.GetArray()[Index])
			return false;

		if (ZE_TEST_CHECK_ERRORHeapTree(Heap, Heap.GetFirstChildIndex(Index)))
			return false;
	}
	
	if (Heap.GetSecondChildIndex(Index) < Heap.GetCount())
	{
		if (Heap.GetSecondChild(Index) > Heap.GetArray()[Index])
			return false;

		if (ZE_TEST_CHECK_ERRORHeapTree(Heap, Heap.GetSecondChildIndex(Index)))
			return false;
	}

	return true;
}

ZE_TEST(ZEMinHeap)
{
	ZE_TEST_ITEM("GetMin")
	{
		
	}

	ZE_TEST_ITEM("Insert")
	{
		ZE_TEST_CASE("InsertOrdered")
		{
			ZEMinHeap<ZEInt> TestHeap;

			TestHeap.InsertValue(0);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 0);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.InsertValue(1);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 0);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.InsertValue(2);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 0);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.InsertValue(3);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 0);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.InsertValue(4);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 0);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.InsertValue(5);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 0);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);
		}

		ZE_TEST_CASE("InsertInverseOrdered")
		{
			ZEMinHeap<ZEInt> TestHeap;

			TestHeap.InsertValue(5);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 5);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.InsertValue(4);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 4);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.InsertValue(3);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 3);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.InsertValue(2);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 2);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.InsertValue(1);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 1);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.InsertValue(0);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 0);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);
		}

		ZE_TEST_CASE("InsertNotOrdered")
		{
			ZEMinHeap<ZEInt> TestHeap;

			TestHeap.InsertValue(5);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 5);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.InsertValue(10);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 5);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.InsertValue(15);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 5);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.InsertValue(0);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 0);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.InsertValue(10);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 0);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.InsertValue(15);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 0);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);
		}
	}

	ZE_TEST_ITEM("Remove")
	{

		ZE_TEST_CASE("RemoveFirstElement")
		{
			ZEMinHeap<ZEInt> TestHeap;
			TestHeap.InsertValue(1);
			TestHeap.InsertValue(2);
			TestHeap.InsertValue(3);
			TestHeap.InsertValue(4);
			TestHeap.InsertValue(5);
			TestHeap.InsertValue(6);

			TestHeap.Remove(0);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 2);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.Remove(0);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 3);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.Remove(0);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 4);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.Remove(0);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 5);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.Remove(0);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetMin(), 6);
			ZE_TEST_CHECK_ERRORHeapTree(TestHeap);

			TestHeap.Remove(0);
			ZE_TEST_CHECK_EQUAL(TestHeap.GetCount(), 0);
		}

	}	

	ZE_TEST_ITEM("GetCount")
	{
		ZEMinHeap<ZEInt> TestHeap;

		ZE_TEST_CHECK_EQUAL(TestHeap.GetCount(), 0);

		TestHeap.InsertValue(1);
		TestHeap.InsertValue(2);
		TestHeap.InsertValue(3);

		ZE_TEST_CHECK_EQUAL(TestHeap.GetCount(), 3);
	}


	ZE_TEST_ITEM("AssignmentAndCopyConstructor")
	{
		ZEMinHeap<ZEInt> TestHeap;
	}
}
