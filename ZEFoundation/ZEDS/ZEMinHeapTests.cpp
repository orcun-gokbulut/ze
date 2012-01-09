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

#include "ZETest.h"
#include "ZEMinHeap.h"

bool ZETestCheckHeapTree(ZEMinHeap<int> Heap, size_t Index = 0)
{
	if (Heap.GetFirstChildIndex(Index) < Heap.GetCount())
	{
		if (Heap.GetFirstChild(Index) > Heap.GetArray()[Index])
			return false;

		if (ZETestCheckHeapTree(Heap, Heap.GetFirstChildIndex(Index)))
			return false;
	}
	
	if (Heap.GetSecondChildIndex(Index) < Heap.GetCount())
	{
		if (Heap.GetSecondChild(Index) > Heap.GetArray()[Index])
			return false;

		if (ZETestCheckHeapTree(Heap, Heap.GetSecondChildIndex(Index)))
			return false;
	}

	return true;
}

ZETestSuite(ZEMinHeap)
{
	ZETest("GetMin")
	{
		
	}

	ZETest("Insert")
	{
		ZETestCase("InsertOrdered")
		{
			ZEMinHeap<int> TestHeap;

			TestHeap.InsertValue(0);
			ZETestCheckEqual(TestHeap.GetMin(), 0);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.InsertValue(1);
			ZETestCheckEqual(TestHeap.GetMin(), 0);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.InsertValue(2);
			ZETestCheckEqual(TestHeap.GetMin(), 0);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.InsertValue(3);
			ZETestCheckEqual(TestHeap.GetMin(), 0);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.InsertValue(4);
			ZETestCheckEqual(TestHeap.GetMin(), 0);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.InsertValue(5);
			ZETestCheckEqual(TestHeap.GetMin(), 0);
			ZETestCheckHeapTree(TestHeap);
		}

		ZETestCase("InsertInverseOrdered")
		{
			ZEMinHeap<int> TestHeap;

			TestHeap.InsertValue(5);
			ZETestCheckEqual(TestHeap.GetMin(), 5);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.InsertValue(4);
			ZETestCheckEqual(TestHeap.GetMin(), 4);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.InsertValue(3);
			ZETestCheckEqual(TestHeap.GetMin(), 3);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.InsertValue(2);
			ZETestCheckEqual(TestHeap.GetMin(), 2);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.InsertValue(1);
			ZETestCheckEqual(TestHeap.GetMin(), 1);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.InsertValue(0);
			ZETestCheckEqual(TestHeap.GetMin(), 0);
			ZETestCheckHeapTree(TestHeap);
		}

		ZETestCase("InsertNotOrdered")
		{
			ZEMinHeap<int> TestHeap;

			TestHeap.InsertValue(5);
			ZETestCheckEqual(TestHeap.GetMin(), 5);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.InsertValue(10);
			ZETestCheckEqual(TestHeap.GetMin(), 5);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.InsertValue(15);
			ZETestCheckEqual(TestHeap.GetMin(), 5);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.InsertValue(0);
			ZETestCheckEqual(TestHeap.GetMin(), 0);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.InsertValue(10);
			ZETestCheckEqual(TestHeap.GetMin(), 0);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.InsertValue(15);
			ZETestCheckEqual(TestHeap.GetMin(), 0);
			ZETestCheckHeapTree(TestHeap);
		}
	}

	ZETest("Remove")
	{

		ZETestCase("RemoveFirstElement")
		{
			ZEMinHeap<int> TestHeap;
			TestHeap.InsertValue(1);
			TestHeap.InsertValue(2);
			TestHeap.InsertValue(3);
			TestHeap.InsertValue(4);
			TestHeap.InsertValue(5);
			TestHeap.InsertValue(6);

			TestHeap.Remove(0);
			ZETestCheckEqual(TestHeap.GetMin(), 2);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.Remove(0);
			ZETestCheckEqual(TestHeap.GetMin(), 3);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.Remove(0);
			ZETestCheckEqual(TestHeap.GetMin(), 4);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.Remove(4);
			ZETestCheckEqual(TestHeap.GetMin(), 5);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.Remove(5);
			ZETestCheckEqual(TestHeap.GetMin(), 6);
			ZETestCheckHeapTree(TestHeap);

			TestHeap.Remove(6);
			ZETestCheckEqual(TestHeap.GetCount(), 0);
		}

	}	

	ZETest("GetCount")
	{
		ZEMinHeap<int> TestHeap;

		ZETestCheckEqual(TestHeap.GetCount(), 0);

		TestHeap.InsertValue(1);
		TestHeap.InsertValue(2);
		TestHeap.InsertValue(3);

		ZETestCheckEqual(TestHeap.GetCount(), 3);
	}


	ZETest("AssignmentAndCopyConstructor")
	{
		ZEMinHeap<int> TestHeap;
	}
}
