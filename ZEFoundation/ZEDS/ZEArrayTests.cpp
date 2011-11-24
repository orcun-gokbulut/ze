//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEArrayTests.cpp
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
#include "ZETest.h"

int Compare1 (const void *F, const void *E)
{
	int A = *((int*)(F));
	int B = *((int*)(E));
	if (A > B)
		return 1;
	else if (A == B)
		return 0;
	else if (A < B)
		return -1;

};



ZETestSuite(ZEArray)
{
	ZETest("Rambo")//getCount,setcount,getitem,setitem,findIndex
	{

		ZEArray<int> TestItem;
		for (int i=0;i<8;i++)
		{
			TestItem.Add(i);
		}
		ZETestCheckEqual(TestItem.GetCount(), 8);
		TestItem.SetItem(2,3);
		ZETestCheckEqual(TestItem[2], 3);
		int x;
		x = TestItem.GetItem(0);
		ZETestCheckEqual(x, 0);
		TestItem.SetCount(4);
		ZETestCheckEqual(TestItem.GetCount(), 4);

		ZEArray<int> TestItem2;
		TestItem2.Insert(0, 0);
		TestItem2.Insert(1, 1);
		TestItem2.Insert(2, 2);
		//start
		ZETestCheckEqual(TestItem2.FindIndex(0), 0);
		//middle
		ZETestCheckEqual(TestItem2.FindIndex(1), 1);
		//end
		ZETestCheckEqual(TestItem2.FindIndex(2), 2);

		ZEArray<int> TestItem3;
		TestItem3.SetCount(3);
		ZETestCheckEqual(TestItem3.GetCount(), 3);


	}
	ZETest("ADD")
	{
		ZEArray<int> TestItem1;
		ZEArray<int> TestItem2;
		for (int i=0;i<8;i++)
		{
			TestItem1.Add(i);
		}
		TestItem2.Add(1);
		ZETestCheckEqual(TestItem2[0], 1);
		TestItem1.Add(8);
		ZETestCheckEqual(TestItem1[8], 8);

		//add by ref
		int TestItem3 = 5;
		TestItem2.AddByRef(TestItem3);
		ZETestCheckEqual(TestItem2[0], 1);
		ZETestCheckEqual(TestItem2[1], 5);


	}
	ZETest("MassADD")
	{
		//MassAdd(Type* OtherArray, size_t Count)
		//TestItem1 filled one, TestItem2 filled one, TestItem3 empty
		//ZEArray<int> TestItem1[] = {0,1,2,3,4,5};
		ZEArray<int> TestItem1;
		for (int i=0;i<6;i++)
		{
			TestItem1.Add(i);
		}
		int* TestItem2 = new int[2];
		TestItem2[0] = 6;
		TestItem2[1] = 7;
		ZEArray<int> TestItem3;
		TestItem1.MassAdd(TestItem2, 2);
		ZETestCheckEqual(TestItem1.GetCount(), 8);
		TestItem3.MassAdd(TestItem2, 2);
		ZETestCheckEqual(TestItem3.GetCount(), 2);

		/*/MassAdd(Type* OtherArray, size_t Offset, size_t Count)
		TestItem1.MassAdd(TestItem2,0,2);
		CHECK_EQUAL(TestItem1[6], 6);
		CHECK_EQUAL(TestItem1[7], 7);
		CHECK_EQUAL(TestItem1.GetCount(), 10);
		
		TestItem1.MassAdd(TestItem2,1,1);
		CHECK_EQUAL(TestItem1[8], 6);
		CHECK_EQUAL(TestItem1.GetCount(), 11);*/

		// MassAdd(size_t ItemCount)
		ZEArray<int> TestITem4;
		for (int I = 10; I < 20; I++)
		{
			TestITem4.Add(I);
		}

		ZEArray<int> TestItem5;

		TestItem5.Add(30);
		TestItem5.Add(40);
		TestItem5.Add(50);
		TestItem5.Add(60);

		int *p;

		p = TestITem4.MassAdd(2);

		p[0] = TestItem5[0];
		p[1] = TestItem5[1];

		//Value of A.GetCount() : 12
		//Value of A[10] : 30
		//Value of A[11] : 40
		
	}

	ZETest("insert")
	{
		//insert start
		ZEArray<int> TestItem1;				
		TestItem1.Insert(0, 1);
		ZETestCheckEqual(TestItem1[0], 1);

		//insert middle
		ZEArray<int> TestItem2;				
		TestItem2.Insert(0, 3);
		TestItem2.Insert(1, 4);
		TestItem2.Insert(2, 5);
		ZETestCheckEqual(TestItem2[0], 3);
		ZETestCheckEqual(TestItem2[1], 4);
		ZETestCheckEqual(TestItem2[2], 5);
		TestItem2.Insert(1,0);                 
		ZETestCheckEqual(TestItem2[0], 3);
		ZETestCheckEqual(TestItem2[1], 0);
		ZETestCheckEqual(TestItem2[2], 4);
		ZETestCheckEqual(TestItem2[3], 5);

		//insert end
		ZEArray<int> TestItem3;				
		TestItem3.Insert(0, 1);
		TestItem3.Insert(1, 2);             

		ZETestCheckEqual(TestItem3[0], 1);
		ZETestCheckEqual(TestItem3[1], 2);

		//insert with pointer
		ZEArray<int> TestItem4;				
		int *p;
		TestItem4.Insert(0, 0);
		TestItem4.Insert(1, 1);
		p=TestItem4.Insert(1);
		*p=5;
		ZETestCheckEqual(TestItem4[0], 0);
		ZETestCheckEqual(TestItem4[1], 5);
		ZETestCheckEqual(TestItem4[2], 1);

	}

	ZETest("MassInsert")
	{
		/*MassInsert(size_t Index, Type* OtherArray, size_t Offset, size_t Count)
		ZEArray<int> TestItem1;
		TestItem1.Add(0);
		TestItem1.Add(1);
		TestItem1.Add(2);
		int* TestItem2 = new int[2];
		TestItem2[0] = 3;
		TestItem2[1] = 4;

		//insert to end
		TestItem1.MassInsert(1,TestItem2,0,2);
  		CHECK_EQUAL(TestItem1[0], 0);
		CHECK_EQUAL(TestItem1[1], 1);
		CHECK_EQUAL(TestItem1[2], 2);
		CHECK_EQUAL(TestItem1[3], 0);
		CHECK_EQUAL(TestItem1[4], 1);
		//insert to start
		TestItem1.MassInsert(0,TestItem2,0,2);
		CHECK_EQUAL(TestItem1[0], 0);
		CHECK_EQUAL(TestItem1[1], 1);
		CHECK_EQUAL(TestItem1[2], 0);
		CHECK_EQUAL(TestItem1[3], 1);
		CHECK_EQUAL(TestItem1[4], 2);
		CHECK_EQUAL(TestItem1[5], 0);
		CHECK_EQUAL(TestItem1[6], 1);
		//insert to middle
		TestItem1.MassInsert(4,TestItem2,0,2);
		CHECK_EQUAL(TestItem1[0], 0);
		CHECK_EQUAL(TestItem1[1], 1);
		CHECK_EQUAL(TestItem1[2], 0);
		CHECK_EQUAL(TestItem1[3], 1);
		CHECK_EQUAL(TestItem1[4], 0);
		CHECK_EQUAL(TestItem1[5], 1);
		CHECK_EQUAL(TestItem1[6], 2);
		CHECK_EQUAL(TestItem1[7], 0);
		CHECK_EQUAL(TestItem1[8], 1);
		//insert from item to TestItem3[2]
		ZEArray<int> TestItem3;
		TestItem3.Add(0);
		TestItem3.Add(1);
		TestItem3.Add(2);
		TestItem3.MassInsert(2,TestItem2,1,2);
		CHECK_EQUAL(TestItem3[0], 0);
		CHECK_EQUAL(TestItem3[1], 1);
		CHECK_EQUAL(TestItem3[2], 3);
		CHECK_EQUAL(TestItem3[3], 2);
		//MassInsert(size_t Index, Type* OtherArray, size_t Count)
		ZEArray<int> TestItem4;
		TestItem4.Add(0);
		TestItem4.Add(1);
		TestItem4.Add(2);
		TestItem4.MassInsert(0,TestItem2,5);
		CHECK_EQUAL(TestItem4[0], 0);
		CHECK_EQUAL(TestItem4[1], 1);
		CHECK_EQUAL(TestItem4[2], 2);
		CHECK_EQUAL(TestItem4[3], 3);
		CHECK_EQUAL(TestItem4[4], 4);
		*/
		//MassInsert(size_t Index, size_t ItemCount)





	}

	ZETest("Enqueue")
	{
		ZEArray<int> TestItem1;
		TestItem1.Enqueue(1);
		ZETestCheckEqual(TestItem1[0], 1);
		TestItem1.Add(0);
		TestItem1.Enqueue(3);
		ZETestCheckEqual(TestItem1[2], 3);
		ZETestCheckEqual(TestItem1.GetCount(), 3);
	}

	ZETest("Dequeue")
	{
		ZEArray<int> TestItem1;
		TestItem1.Add(1);
		TestItem1.Add(2);
		TestItem1.Dequeue();
		ZETestCheckEqual(TestItem1[0], 2);
	}

	ZETest("DeleteAt")
	{
		ZEArray<int> TestItem1;
		TestItem1.Add(1);
		TestItem1.Add(2);
		TestItem1.Add(3);
		TestItem1.Add(4);
		TestItem1.Add(5);
		TestItem1.Add(6);
		TestItem1.DeleteAt(0);
		//delete at the start
		ZETestCheckEqual(TestItem1[0], 2);
		//delete at the end
		TestItem1.DeleteAt(2);
		ZETestCheckEqual(TestItem1[2], 5);
		//delete at the middle
		TestItem1.DeleteAt(1);
		ZETestCheckEqual(TestItem1[1], 5);

	}

	ZETest("PopPush")
	{
		//push item
		ZEArray<int> TestItem1;
		TestItem1.Push(0);
		TestItem1.Push(3);
		//push to empty one
		ZETestCheckEqual(TestItem1[0], 0);
		//push to filled one
		ZETestCheckEqual(TestItem1[1], 3);

		//pop item
		TestItem1.Pop();
		ZETestCheckEqual(TestItem1.GetCount(), 1);
		ZETestCheckEqual(TestItem1[0], 0);
	}

	ZETest("DeleteValue")
	{
		ZEArray<int> TestItem1;
		TestItem1.Add(0);
		TestItem1.Add(1);
		//deletevalue at the end
		TestItem1.DeleteValue(1);
		ZETestCheckEqual(TestItem1[0], 0);

		TestItem1.Add(2);
		TestItem1.Add(3);
		//after add item then delete value at the middle
		TestItem1.DeleteValue(2);
		ZETestCheckEqual(TestItem1[0], 0);
		ZETestCheckEqual(TestItem1[1], 3);
		//delete at the first one
		TestItem1.DeleteValue(0);
		ZETestCheckEqual(TestItem1[0], 3);

	}
	ZETest("Existence")
	{
		ZEArray<int> TestItem1;
		int x;
		TestItem1.Add(0);
		TestItem1.Add(1);
		TestItem1.Add(2);
		//check exists one at the start
		x = 0;
		TestItem1.Exists(x);
		//check exists one at the middle
		x = 1;
		TestItem1.Exists(x);
		//check exists one at the end
		x = 2;
		TestItem1.Exists(x);
	}

	ZETest("Operators")
	{
		// =
		ZEArray<int> TestItem1;
		ZEArray<int> TestItem2;
		TestItem1.Add(0);
		TestItem1.Add(1);
		TestItem2 = TestItem1;
		ZETestCheckEqual(TestItem1[0], TestItem2[0]);
		ZETestCheckEqual(TestItem1[1], TestItem2[1]);
		// []
		ZETestCheckEqual(TestItem1[0], 0);

	}

	ZETest("GetCArray")
	{
		ZEArray<int> TestItem1;
		int *TestItem2;
		TestItem1.Add(1);
		TestItem1.Add(2);
		TestItem1.Add(3);
		TestItem2 = TestItem1.GetCArray();
		ZETestCheckEqual(TestItem2[0], 1);
		ZETestCheckEqual(TestItem2[1], 2);
		ZETestCheckEqual(TestItem2[2], 3);

	}

	ZETest("fill")
	{
		ZEArray<int> TestItem1;
		TestItem1.SetCount(3);
		TestItem1.Fill(3);
		ZETestCheckEqual(TestItem1[0], 3);
		ZETestCheckEqual(TestItem1[1], 3);
		ZETestCheckEqual(TestItem1[2], 3);

	}

	/*ZETestItemAdd(fill)
	{
		ZEArray<int> TestItem1;
		ZEArray<int> TestItem2;
		ZEArray<int> TestItem3;
		//fill to empty
		TestItem2.Add(0);
		TestItem2.Add(1);
		int* TestItem4 = new int[2];
		TestItem4[0] = 0;
		TestItem4[1] = 1;
		TestItem3.Fill(TestItem4, 2);
		CHECK_EQUAL(TestItem3[0], 0);
		CHECK_EQUAL(TestItem3[1], 1);
		//fill to !empty
		TestItem1.Add(2);
		TestItem1.Add(3);
		TestItem1.Resize(5);
		TestItem1.Fill(TestItem4, 2);
		CHECK_EQUAL(TestItem1[0], 0);
		CHECK_EQUAL(TestItem1[1], 1);
		CHECK_EQUAL(TestItem1.GetCount(), 2);

	}
*/
	ZETest("Resize")
	{
		ZEArray<int> TestItem1;
		TestItem1.Add(0);
		TestItem1.Add(1);
		TestItem1.Add(2);
		TestItem1.Resize(2);
		ZETestCheckEqual(TestItem1.GetCount(), 2);
		//after set count
		TestItem1.SetCount(4);
		TestItem1.Resize(1);
		ZETestCheckEqual(TestItem1.GetCount(), 1);

	}

	ZETest("GetSize")
	{
		//after operation check with getcount
		ZEArray<int> TestItem1;
		TestItem1.SetCount(3);
		ZETestCheckEqual(TestItem1.GetSize(), TestItem1.GetCount());
		//after insert operation check size
		ZEArray<int> TestItem2;
		TestItem2.Add(0);
		TestItem2.Add(1);
		ZETestCheckEqual(TestItem2.GetSize(), 2);

	}

	ZETest("CopyTo")
	{
		ZEArray<int> TestItem1;
		ZEArray<int> TestItem2;
		//to empty
		TestItem1.Add(0);
		TestItem1.Add(1);
		TestItem1.CopyTo(TestItem2);
		ZETestCheckEqual(TestItem2[0], 0);
		ZETestCheckEqual(TestItem2[1], 1);
		// to !empty
		ZEArray<int> TestItem3;
		TestItem3.Add(2);
		TestItem3.Add(3);
		TestItem3.CopyTo(TestItem2);
		ZETestCheckEqual(TestItem2[0], 2);
		ZETestCheckEqual(TestItem2[1], 3);
		ZETestCheckEqual(TestItem2.GetCount(), 2);

	}
	ZETest("CopyFrom")
	{
		ZEArray<int> TestItem1;
		ZEArray<int> TestItem2;
		//from empty
		TestItem1.Insert(0, 0);
		TestItem1.Insert(1, 1);
		TestItem2.CopyFrom(TestItem1);
		ZETestCheckEqual(TestItem2[0], 0);
		ZETestCheckEqual(TestItem2[1], 1);
		ZETestCheckEqual(TestItem2.GetCount(), 2);

		// C[2] from empty
		ZEArray<int> TestItem3;
		TestItem3.Insert(0,2);
		TestItem3.Insert(1,3);
		TestItem3.CopyFrom(TestItem1);
		ZETestCheckEqual(TestItem3[0], 0);
		ZETestCheckEqual(TestItem3[1], 1);

		// empty to D[2]
		ZEArray<int> TestItem4;
		TestItem4.CopyFrom(TestItem1);
		ZETestCheckEqual(TestItem4[0], 0);
		ZETestCheckEqual(TestItem4[1], 1);
		ZETestCheckEqual(TestItem4.GetCount(), 2);

	}
	ZETest("Combine")
	{
		ZEArray<int> TestItem1;
		ZEArray<int> TestItem2;
		TestItem1.Add(0);
		TestItem1.Add(1);
		TestItem2.Add(1);
		TestItem1.Combine(TestItem2);
		ZETestCheckEqual(TestItem1.GetCount(), 3);
		ZETestCheckEqual(TestItem1[0], 0);
		ZETestCheckEqual(TestItem1[1], 1);
		ZETestCheckEqual(TestItem1[2], 1);

	}

	ZETest("Clear")
	{

		ZEArray<int> TestItem1;
		TestItem1.Add(0);
		TestItem1.Add(1);
		ZETestCheckEqual(TestItem1[0], 0);
		ZETestCheckEqual(TestItem1[1], 1);
		TestItem1.Clear();
		ZETestCheckEqual(TestItem1.GetCount(), 0);
	}

	ZETest("sort")
	{
		//						return
		//A[]={0,2,2,1}  0,2	(-1)
		//				 2,2	(0)
		//				 2,1	(1)

		ZEArray<int> TestItem1;
		TestItem1.Insert(0, 0);
		TestItem1.Insert(1, 2);
		TestItem1.Insert(2, 2);
		TestItem1.Insert(3, 1);
		TestItem1.Sort(&Compare1);

		ZETestCheckEqual(TestItem1[0], 0);
		ZETestCheckEqual(TestItem1[1], 1);
		ZETestCheckEqual(TestItem1[2], 2);
		ZETestCheckEqual(TestItem1[3], 2);
	}

	ZETest("binary_search")
	{

		ZEArray<int> TestItem1;
		TestItem1.Add(0);
		TestItem1.Add(5);
		TestItem1.Add(10);
		TestItem1.Add(15);
		TestItem1.Add(20);
		TestItem1.Add(30);
		TestItem1.Add(35);
		const int a = 20;
		int b;
	//	b = TestItem1.BinarySearch(a, /*(int (_cdecl *)(const int*, const int*))*/&Compare1);

	}
}
