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

int Compare (const void *F, const void *E)
{
	int A = *((int*)(F));
	int B = *((int*)(E));

	int ReturnValue = 0;

	if (A > B)
		ReturnValue = 1;
	else if (A == B)
		ReturnValue = 0;
	else if (A < B)
		ReturnValue = -1;

	return ReturnValue;
};



ZETestSuite(ZEArray)
{
	ZETest("~ZEArray()")
	{

	}
	ZETest("inline Type* Add()")
	{
		ZEArray<int> Array;

		int *p;

		p = Array.Add();
		*p = 5;

		p = Array.Add();
		*p = 2;
		ZETestCheckEqual(Array[0], 5);
		ZETestCheckEqual(Array[1], 2);

		ZETestCase("for ZEChunkArray")
		{
			ZEChunkArray<int, 2> ChunkArray;

			int *p;
			p = ChunkArray.Add();
			*p = 5;
			p = ChunkArray.Add();
			*p = 4;
			p = ChunkArray.Add();
			*p = 3;
			p = ChunkArray.Add();
			*p = 2;
			p = ChunkArray.Add();
			*p = 1;

			ZETestCheckEqual(ChunkArray.GetCount(), 5);
			ZETestCheckEqual(ChunkArray.GetSize(), 6);
		}

		ZETestCase("for ZESmartArray")
		{
			ZESmartArray<int, 2> SmartArray;

			int *p;
			p = SmartArray.Add();
			*p = 5;
			p = SmartArray.Add();
			*p = 4;
			p = SmartArray.Add();
			*p = 3;

			ZETestCheckEqual(SmartArray.GetCount(), 3);
			ZETestCheckEqual(SmartArray.GetSize(), 6);
		}
	}
	ZETest("inline Type* Add(Type NewItem)")
	{
		ZEArray<int> Array;

		int *p;

		p = Array.Add(1);
		p = Array.Add(2);
		ZETestCheckEqual(Array[0], 1);
		ZETestCheckEqual(Array[1], 2);
		ZETestCheckEqual(*p, 2);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 4> ChunkArray;

			int *p;

			for (int I = 10; I < 16; I++)
			{
				p = ChunkArray.Add(I);
			}

			ZETestCheckEqual(ChunkArray.GetCount(), 6);
			ZETestCheckEqual(ChunkArray.GetSize(), 8);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 2> SmartArray;

			int *p;

			for (int I = 10; I < 16; I++)
			{
				p = SmartArray.Add(I);
			}

			ZETestCheckEqual(SmartArray.GetCount(), 6);
			ZETestCheckEqual(SmartArray.GetSize(), 6);
		}
	}
	ZETest("inline Type* AddByRef(const Type& NewItem)")
	{
		ZEArray<int> Array;

		int NewItem = 3;
		int *p;

		p = Array.AddByRef(NewItem);
		p = Array.AddByRef(2);
		ZETestCheckEqual(Array[0], 3);
		ZETestCheckEqual(Array[1], 2);
		ZETestCheckEqual(*p, 2);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 5> ChunkArray;

			int *p;

			for (int NewItem = 10; NewItem < 16; NewItem++)
			{
				p = ChunkArray.AddByRef(NewItem);
			}
			ZETestCheckEqual(ChunkArray.GetCount(), 6);
			ZETestCheckEqual(ChunkArray.GetSize(), 10);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 5> SmartArray;

			int *p;

			for (int NewItem = 10; NewItem < 16; NewItem++)
			{
				p = SmartArray.AddByRef(NewItem);
			}
			ZETestCheckEqual(SmartArray.GetCount(), 6);		
			ZETestCheckEqual(SmartArray.GetSize(), 30); //29
		}
	}
	ZETest("inline int BinarySearch(const Type& Element, int (*CompareFunction)(Type*, Type*))")
	{
		/*ZEArray<int> Array;

		Array.Add(0);
		Array.Add(5);
		Array.Add(10);
		Array.Add(15);
		Array.Add(20);
		Array.Add(25);

		const int Element = 20;

		int Result = Array.BinarySearch(Element, &Compare);*/
	}
	ZETest("inline void Clear()")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);

		Array.Clear();
		ZETestCheckEqual(Array.GetCount(), 0);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);

			ZETestCheckEqual(ChunkArray.GetSize(), 4);

			ChunkArray.Clear();
			ZETestCheckEqual(ChunkArray.GetCount(), 0);
			ZETestCheckEqual(ChunkArray.GetSize(), 0);
		}
		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);

			ZETestCheckEqual(SmartArray.GetCount(), 3);
			ZETestCheckEqual(SmartArray.GetSize(), 6);

			SmartArray.Clear();
			ZETestCheckEqual(SmartArray.GetCount(), 0);
			ZETestCheckEqual(SmartArray.GetSize(), 0);
		}
	}
	ZETest("void Combine(const ZEArray<Type, Allocator_>& OtherArray)")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);

		ZEArray<int> OtherArray;

		OtherArray.Add(3);
		OtherArray.Add(4);

		Array.Combine(OtherArray);
		ZETestCheckEqual(Array[0], 1);
		ZETestCheckEqual(Array[1], 2);
		ZETestCheckEqual(Array[2], 3);
		ZETestCheckEqual(Array[3], 4);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);

			ZEChunkArray<int, 2> OtherChunkArray;

			OtherChunkArray.Add(4);
			OtherChunkArray.Add(5);
			OtherChunkArray.Add(6);

			ChunkArray.Combine(OtherChunkArray);

			ZETestCheckEqual(ChunkArray.GetCount(), 6);
			ZETestCheckEqual(ChunkArray.GetSize(), 8);
			ZETestCheckEqual(OtherChunkArray.GetSize(), 4);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);

			ZESmartArray<int, 2> OtherSmartArray;

			OtherSmartArray.Add(4);
			OtherSmartArray.Add(5);
			OtherSmartArray.Add(6);

			SmartArray.Combine(OtherSmartArray);

			ZETestCheckEqual(SmartArray.GetCount(), 6);
			ZETestCheckEqual(SmartArray.GetSize(), 6);
			ZETestCheckEqual(OtherSmartArray.GetSize(), 6);
		}
	}
	ZETest("void CopyFrom(const Type* OtherArray, size_t Count)")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEArray<int> OtherArray;

		OtherArray.Add(4);
		OtherArray.Add(5);
		OtherArray.Add(6);

		Array.CopyFrom(OtherArray.GetCArray(), 2);
		ZETestCheckEqual(Array.GetCount(), 2);
		ZETestCheckEqual(Array[0], 4);
		ZETestCheckEqual(Array[1], 5);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 3> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);

			ZEChunkArray<int, 3> OtherChunkArray;

			OtherChunkArray.Add(4);
			OtherChunkArray.Add(5);
			OtherChunkArray.Add(6);

			ChunkArray.CopyFrom(OtherChunkArray.GetCArray(), 2);

			ZETestCheckEqual(ChunkArray.GetCount(), 2);
			ZETestCheckEqual(ChunkArray.GetSize(), 3);
			ZETestCheckEqual(OtherChunkArray.GetSize(), 3);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 3> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);

			ZESmartArray<int, 3> OtherSmartArray;

			OtherSmartArray.Add(4);
			OtherSmartArray.Add(5);
			OtherSmartArray.Add(6);

			SmartArray.CopyFrom(OtherSmartArray.GetCArray(), 2);

			ZETestCheckEqual(SmartArray.GetCount(), 2);
			ZETestCheckEqual(SmartArray.GetSize(), 3);
			ZETestCheckEqual(OtherSmartArray.GetSize(), 3);
		}
	}
	ZETest("void CopyFrom(const ZEArray<Type, Allocator_>& OtherArray)")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEArray<int> OtherArray;

		OtherArray.Add(4);
		OtherArray.Add(5);

		Array.CopyFrom(OtherArray);
		ZETestCheckEqual(Array.GetCount(), 2);
		ZETestCheckEqual(Array[0], 4);
		ZETestCheckEqual(Array[1], 5);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);

			ZEChunkArray<int, 2> OtherChunkArray;

			OtherChunkArray.Add(4);
			OtherChunkArray.Add(5);

			ChunkArray.CopyFrom(OtherChunkArray);

			ZETestCheckEqual(ChunkArray.GetCount(), 2);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);
			ZETestCheckEqual(OtherChunkArray.GetSize(), 2);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			ZETestCheckEqual(SmartArray.GetSize(), 6);

			ZESmartArray<int, 2> OtherSmartArray;

			OtherSmartArray.Add(4);
			OtherSmartArray.Add(5);

			SmartArray.CopyFrom(OtherSmartArray);

			ZETestCheckEqual(SmartArray.GetCount(), 2);
			ZETestCheckEqual(SmartArray.GetSize(), 6); //4
			ZETestCheckEqual(OtherSmartArray.GetSize(), 2);
		}
	}
	ZETest("void CopyTo(Type* OtherArray, size_t Count) const")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEArray<int> OtherArray;

		OtherArray.Add(4);
		OtherArray.Add(5);
		OtherArray.Add(6);

		Array.CopyTo(OtherArray.GetCArray(), 2);
		ZETestCheckEqual(OtherArray.GetCount(), 3);
		ZETestCheckEqual(OtherArray[0], 1);
		ZETestCheckEqual(OtherArray[1], 2);
		ZETestCheckEqual(OtherArray[2], 6);
	}
	ZETest("void CopyTo(ZEArray<Type, Allocator_>& OtherArray) const")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEArray<int> OtherArray;

		OtherArray.Add(4);
		OtherArray.Add(5);
		OtherArray.Add(6);
		OtherArray.Add(7);

		Array.CopyTo(OtherArray);
		ZETestCheckEqual(OtherArray.GetCount(), 3);
		ZETestCheckEqual(OtherArray[0], 1);
		ZETestCheckEqual(OtherArray[1], 2);
		ZETestCheckEqual(OtherArray[2], 3);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);

			ZEChunkArray<int, 2> OtherChunkArray;

			OtherChunkArray.Add(4);
			OtherChunkArray.Add(5);
			OtherChunkArray.Add(6);
			OtherChunkArray.Add(7);
			ZETestCheckEqual(OtherChunkArray.GetSize(), 4);

			ChunkArray.CopyTo(OtherChunkArray);

			ZETestCheckEqual(OtherChunkArray.GetCount(), 3);
			ZETestCheckEqual(OtherChunkArray.GetSize(), 4);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			ZETestCheckEqual(SmartArray.GetSize(), 6);

			ZESmartArray<int, 2> OtherSmartArray;

			OtherSmartArray.Add(4);
			OtherSmartArray.Add(5);
			OtherSmartArray.Add(6);
			OtherSmartArray.Add(7);
			ZETestCheckEqual(OtherSmartArray.GetSize(), 6);

			SmartArray.CopyTo(OtherSmartArray);

			ZETestCheckEqual(OtherSmartArray.GetCount(), 3);
			ZETestCheckEqual(OtherSmartArray.GetSize(), 6);
		}

	}
	ZETest("inline void DeleteAt(size_t Index)")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		Array.DeleteAt(1);
		ZETestCheckEqual(Array.GetCount(), 2);
		ZETestCheckEqual(Array[0], 1);
		ZETestCheckEqual(Array[1], 3);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);

			ChunkArray.DeleteAt(1);

			ZETestCheckEqual(ChunkArray.GetCount(), 2);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			ZETestCheckEqual(SmartArray.GetSize(), 6);

			SmartArray.DeleteAt(1);

			ZETestCheckEqual(SmartArray.GetCount(), 2);
			ZETestCheckEqual(SmartArray.GetSize(), 6); //4
		}
	}
	ZETest("inline void DeleteValue(Type Value)")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);
		Array.Add(2);

		Array.DeleteValue(2);
		ZETestCheckEqual(Array.GetCount(), 2);
		ZETestCheckEqual(Array[0], 1);
		ZETestCheckEqual(Array[1], 3);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ChunkArray.Add(2);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);

			ChunkArray.DeleteValue(2);

			ZETestCheckEqual(ChunkArray.GetCount(), 2);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			SmartArray.Add(2);
			ZETestCheckEqual(SmartArray.GetSize(), 6);

			SmartArray.DeleteValue(2);

			ZETestCheckEqual(SmartArray.GetCount(), 2);
			ZETestCheckEqual(SmartArray.GetSize(), 6); //4
		}
	}
	ZETest("inline Type Dequeue()")
	{
		ZEArray<int> Array;

		Array.Add(3);
		Array.Add(2);

		int Element = Array.Dequeue();
		ZETestCheckEqual(Array.GetCount(), 1);
		ZETestCheckEqual(Array[0], 2);
		ZETestCheckEqual(Element, 3);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 2> ChunkArray;

			ChunkArray.Add(3);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ZETestCheckEqual(ChunkArray.GetSize(), 6);

			int Element = ChunkArray.Dequeue();

			ZETestCheckEqual(ChunkArray.GetCount(), 4);
			ZETestCheckEqual(ChunkArray.GetSize(), 6);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 2> SmartArray;

			SmartArray.Add(3);
			SmartArray.Add(2);
			SmartArray.Add(3);
			SmartArray.Add(2);
			SmartArray.Add(3);
			ZETestCheckEqual(SmartArray.GetSize(), 6);

			int Element = SmartArray.Dequeue();

			ZETestCheckEqual(SmartArray.GetCount(), 4);
			ZETestCheckEqual(SmartArray.GetSize(), 6);
		}
	}
	ZETest("inline void Enqueue(Type Value)")
	{
		ZEArray<int> Array;
		int Value = 3;

		Array.Add(1);
		Array.Add(2);

		Array.Enqueue(Value);
		ZETestCheckEqual(Array.GetCount(), 3);
		ZETestCheckEqual(Array[0], 1);
		ZETestCheckEqual(Array[1], 2);
		ZETestCheckEqual(Array[2], 3);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 2> ChunkArray;
			int Value = 3;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ZETestCheckEqual(ChunkArray.GetSize(), 2);

			ChunkArray.Enqueue(Value);

			ZETestCheckEqual(ChunkArray.GetCount(), 3);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 2> SmartArray;
			int Value = 3;

			SmartArray.Add(1);
			SmartArray.Add(2);
			ZETestCheckEqual(SmartArray.GetSize(), 2);

			SmartArray.Enqueue(Value);

			ZETestCheckEqual(SmartArray.GetCount(), 3);
			ZETestCheckEqual(SmartArray.GetSize(), 6);
		}
	}
	ZETest("inline bool Exists(Type& Value)")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);
		Array.Add(4);

		int Value1 = 2;
		int Value2 = 5;

		bool Result1 = Array.Exists(Value1);
		bool Result2 = Array.Exists(Value2);
		ZETestCheckEqual(Result1, true);
		ZETestCheckEqual(Result2, false);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ChunkArray.Add(4);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);

			int Value1 = 2;
			int Value2 = 5;

			bool Result1 = ChunkArray.Exists(Value1);
			bool Result2 = ChunkArray.Exists(Value2);
			ZETestCheckEqual(Result1, true);
			ZETestCheckEqual(Result2, false);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			SmartArray.Add(4);
			ZETestCheckEqual(SmartArray.GetSize(), 6);

			int Value1 = 2;
			int Value2 = 5;

			bool Result1 = SmartArray.Exists(Value1);
			bool Result2 = SmartArray.Exists(Value2);
			ZETestCheckEqual(Result1, true);
			ZETestCheckEqual(Result2, false);
		}
	}
	ZETest("inline void Fill(Type Value)")
	{
		ZEArray<int> Array;

		Array.SetCount(5);
		int Value = 7;

		Array.Fill(Value);
		ZETestCheckEqual(Array.GetCount(), 5);
		ZETestCheckEqual(Array[0], 7);
		ZETestCheckEqual(Array[1], 7);
		ZETestCheckEqual(Array[2], 7);
		ZETestCheckEqual(Array[3], 7);
		ZETestCheckEqual(Array[4], 7);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 4> ChunkArray;

			ChunkArray.SetCount(5);
			int Value = 7;

			ChunkArray.Fill(Value);

			ZETestCheckEqual(ChunkArray.GetCount(), 5);
			ZETestCheckEqual(ChunkArray.GetSize(), 8);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 4> SmartArray;

			SmartArray.SetCount(5);
			int Value = 7;

			SmartArray.Fill(Value);

			ZETestCheckEqual(SmartArray.GetCount(), 5);
			ZETestCheckEqual(SmartArray.GetSize(), 20); //19
		}
	}
	ZETest("inline int FindIndex(Type Item, int StartIndex = 0)")
	{
		ZEArray<int> Array;

		for (int I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		int Result = Array.FindIndex(12);
		ZETestCheckEqual(Result, 2);

		ZETestCase("FindIndex for searched value is not in the array")
		{
			int Result = Array.FindIndex(25);
			ZETestCheckEqual(Result, -1);
		}
		ZETestCase("FindIndex for searched value is in the array but before the specified starting index")
		{
			int Result = Array.FindIndex(12, 4);
			ZETestCheckEqual(Result, -1);
		}

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 6> ChunkArray;

			for (int I = 10; I < 20; I++)
			{
				ChunkArray.Add(I);
			}
			ZETestCheckEqual(ChunkArray.GetCount(), 10);
			ZETestCheckEqual(ChunkArray.GetSize(), 12);

			int Result = ChunkArray.FindIndex(12);
			ZETestCheckEqual(Result, 2);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 6> SmartArray;

			for (int I = 10; I < 20; I++)
			{
				SmartArray.Add(I);
			}
			ZETestCheckEqual(SmartArray.GetCount(), 10);
			ZETestCheckEqual(SmartArray.GetSize(), 42); //41

			int Result = SmartArray.FindIndex(12);
			ZETestCheckEqual(Result, 2);
		}
	}
	ZETest("inline Type* GetCArray()")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		int *p;

		p = Array.GetCArray();
		ZETestCheckEqual(*p, 1);

		p[0] = 5;
		ZETestCheckEqual(*p, 5);
		ZETestCheckEqual(Array[0], 5);
		ZETestCheckEqual(Array[1], 2);
		ZETestCheckEqual(Array[2], 3);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);

			int *p;

			p = ChunkArray.GetCArray();
			ZETestCheckEqual(*p, 1);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);

			int *p;

			p = SmartArray.GetCArray();
			ZETestCheckEqual(*p, 1);
			ZETestCheckEqual(SmartArray.GetSize(), 6);
		}
	}
	ZETest("inline const Type* GetConstCArray() const")
	{
		ZEArray<int> Array;

		for (int I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		const int *p;

		p = Array.GetConstCArray();
		ZETestCheckEqual(*p, 10);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 7> ChunkArray;

			for (int I = 10; I < 20; I++)
			{
				ChunkArray.Add(I);
			}

			const int *p;

			p = ChunkArray.GetConstCArray();
			ZETestCheckEqual(*p, 10);
			ZETestCheckEqual(ChunkArray.GetCount(), 10);
			ZETestCheckEqual(ChunkArray.GetSize(), 14);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 7> SmartArray;

			for (int I = 10; I < 20; I++)
			{
				SmartArray.Add(I);
			}

			const int *p;

			p = SmartArray.GetConstCArray();
			ZETestCheckEqual(*p, 10);
			ZETestCheckEqual(SmartArray.GetCount(), 10);
			ZETestCheckEqual(SmartArray.GetSize(), 56);
		}
	}
	ZETest("inline size_t GetCount() const")
	{
		ZEArray<int> Array;

		for (int I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		int Count = Array.GetCount();
		ZETestCheckEqual(Count, 10);

		ZETestCase("for ZEChunkArray")
		{
			ZEChunkArray<int, 5> ChunkArray;

			for (int I = 1; I < 7; I++)
			{
				ChunkArray.Add(I);
			}

			int Count = ChunkArray.GetCount();
			int Size = ChunkArray.GetSize();
			ZETestCheckEqual(Count, 6);
			ZETestCheckEqual(Size, 10);
		}

		ZETestCase("for ZESmartArray")
		{
			ZESmartArray<int, 2> SmartArray;

			for (int I = 1; I < 8; I++)
			{
				SmartArray.Add(I);
			}

			ZETestCheckEqual(SmartArray.GetCount(), 7);
			ZETestCheckEqual(SmartArray.GetSize(), 14);
		}
	}
	ZETest("inline Type& GetFirstItem()")
	{
		ZEArray<int> Array;

		for (int I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		int Result = Array.GetFirstItem();
		ZETestCheckEqual(Result, 10);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 8> ChunkArray;

			for (int I =10; I < 20; I++)
			{
				ChunkArray.Add(I);
			}

			int Result = ChunkArray.GetFirstItem();
			ZETestCheckEqual(Result, 10);
			ZETestCheckEqual(ChunkArray.GetCount(), 10);
			ZETestCheckEqual(ChunkArray.GetSize(), 16);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 8> SmartArray;

			for (int I = 10; I < 20; I++)
			{
				SmartArray.Add(I);
			}

			int Result = SmartArray.GetFirstItem();
			ZETestCheckEqual(Result, 10);
			ZETestCheckEqual(SmartArray.GetCount(), 10);
			ZETestCheckEqual(SmartArray.GetSize(), 72); //71
		}
	}
	ZETest("inline Type& GetItem(size_t Index)")
	{
		ZEArray<int> Array;

		for (int I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		int Result = Array.GetItem(2);
		ZETestCheckEqual(Result, 12);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 9> ChunkArray;

			for (int I = 10; I < 30; I++)
			{
				ChunkArray.Add(I);
			}

			int Result = ChunkArray.GetItem(13);
			ZETestCheckEqual(Result, 23);
			ZETestCheckEqual(ChunkArray.GetCount(), 20);
			ZETestCheckEqual(ChunkArray.GetSize(), 27);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 9> SmartArray;

			for (int I =10; I < 30; I++)
			{
				SmartArray.Add(I);
			}

			int Result = SmartArray.GetItem(13);
			ZETestCheckEqual(Result, 23);
			ZETestCheckEqual(SmartArray.GetCount(), 20);
			ZETestCheckEqual(SmartArray.GetSize(), 90);
		}
	}
	ZETest("inline const Type& GetItem(size_t Index) const")
	{
		ZEArray<int> Array;

		for (int I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		const int Result = Array.GetItem(6);
		ZETestCheckEqual(Result, 16);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 10> ChunkArray;

			for (int I = 10; I < 30; I++)
			{
				ChunkArray.Add(I);
			}

			const int Result = ChunkArray.GetItem(13);
			ZETestCheckEqual(Result, 23);
			ZETestCheckEqual(ChunkArray.GetCount(), 20);
			ZETestCheckEqual(ChunkArray.GetSize(), 20);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 10> SmartArray;

			for (int I = 10; I < 30; I++)
			{
				SmartArray.Add(I);
			}

			const int Result = SmartArray.GetItem(13);
			ZETestCheckEqual(Result, 23);
			ZETestCheckEqual(SmartArray.GetCount(), 20);
			ZETestCheckEqual(SmartArray.GetSize(), 110);
		}
	}
	ZETest("inline Type& GetLastItem()")
	{
		ZEArray<int> Array;

		for (int I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		int Result = Array.GetLastItem();
		ZETestCheckEqual(Result, 19);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 15> ChunkArray;

			for (int I = 10; I < 30; I++)
			{
				ChunkArray.Add(I);
			}
			int Result = ChunkArray.GetLastItem();
			ZETestCheckEqual(Result, 29);
			ZETestCheckEqual(ChunkArray.GetCount(), 20);
			ZETestCheckEqual(ChunkArray.GetSize(), 30);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 15> SmartArray;

			for (int I = 10; I < 30; I++)
			{
				SmartArray.Add(I);
			}
			int Result = SmartArray.GetLastItem();
			ZETestCheckEqual(Result, 29);
			ZETestCheckEqual(SmartArray.GetCount(), 20);
			ZETestCheckEqual(SmartArray.GetSize(), 240); //239
		}
	}
	ZETest("inline size_t GetSize() const")
	{
		ZEArray<int> Array;

		for (int I = 10; I < 15; I++)
		{
			Array.Add(I);
		}

		int Size = Array.GetSize();
		ZETestCheckEqual(Size, 5);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 3> ChunkArray;

			for (int I = 10; I < 15; I++)
			{
				ChunkArray.Add(I);
			}

			ZETestCheckEqual(ChunkArray.GetSize(), 6);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 3> SmartArray;

			for (int I = 10; I < 15; I++)
			{
				SmartArray.Add(I);
			}

			ZETestCheckEqual(SmartArray.GetSize(), 12); //11
		}
	}
	ZETest("inline Type* Insert(size_t Index)")
	{
		ZEArray<int> Array;

		int *p;

		Array.Insert(0, 0);
		Array.Insert(1, 1);

		p = Array.Insert(1);

		*p=5;

		ZETestCheckEqual(Array[0], 0);
		ZETestCheckEqual(Array[1], 5);
		ZETestCheckEqual(Array[2], 1);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 5> ChunkArray;

			int *p;

			for (int I = 1; I < 6; I++)
			{
				ChunkArray.Add(I);
			}
			ZETestCheckEqual(ChunkArray.GetSize(), 5);

			p = ChunkArray.Insert(0);

			*p = 8;

			ZETestCheckEqual(ChunkArray.GetCount(), 6);
			ZETestCheckEqual(ChunkArray.GetSize(), 10);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 5> SmartArray;

			int *p;

			for (int I = 1; I < 6; I++)
			{
				SmartArray.Add(I);
			}
			ZETestCheckEqual(SmartArray.GetSize(), 5);

			p = SmartArray.Insert(0);

			*p = 8;

			ZETestCheckEqual(SmartArray.GetCount(), 6);
			ZETestCheckEqual(SmartArray.GetSize(), 30); //29
		}
	}
	ZETest("inline Type* Insert(size_t Index, Type NewItem)")
	{
		ZEArray<int> Array;

		int *p;

		p = Array.Insert(0, 3);
		ZETestCheckEqual(*p, 3);
		p = Array.Insert(1, 4);
		ZETestCheckEqual(*p, 4);
		p = Array.Insert(2, 5);
		ZETestCheckEqual(*p, 5);

		ZETestCheckEqual(Array[0], 3);
		ZETestCheckEqual(Array[1], 4);
		ZETestCheckEqual(Array[2], 5);

		p = Array.Insert(1, 0);
		ZETestCheckEqual(*p, 0);

		ZETestCheckEqual(Array[0], 3);
		ZETestCheckEqual(Array[1], 0);
		ZETestCheckEqual(Array[2], 4);
		ZETestCheckEqual(Array[3], 5);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 3> ChunkArray;

			int *p;

			p = ChunkArray.Insert(0, 3);
			p = ChunkArray.Insert(1, 4);
			p = ChunkArray.Insert(2, 5);
			p = ChunkArray.Insert(1, 0);

			ZETestCheckEqual(ChunkArray.GetCount(), 4);
			ZETestCheckEqual(ChunkArray.GetSize(), 6);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 3> SmartArray;

			int *p;

			p = SmartArray.Insert(0, 3);
			p = SmartArray.Insert(1, 4);
			p = SmartArray.Insert(2, 5);
			p = SmartArray.Insert(1, 0);

			ZETestCheckEqual(SmartArray.GetCount(), 4);
			ZETestCheckEqual(SmartArray.GetSize(), 12); //11
		}
	}
	ZETest("inline Type* MassAdd(const Type* NewItems, size_t ItemCount)")
	{
		ZEArray<int> A;

		int *p;

		for (int I = 10; I < 20; I++)
		{
			A.Add(I);
		}

		ZEArray<int> B;

		B.Add(30);
		B.Add(40);
		B.Add(50);
		B.Add(60);

		p = A.MassAdd(B.GetCArray(), 2);
		ZETestCheckEqual(*p, 30);

		ZETestCheckEqual(A.GetCount(), 12);
		ZETestCheckEqual(A[9], 19);
		ZETestCheckEqual(A[10], 30);
		ZETestCheckEqual(A[11], 40);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 6> A;

			int *p;

			for (int I = 10; I < 20; I++)
			{
				A.Add(I);
			}
			ZETestCheckEqual(A.GetCount(), 10);
			ZETestCheckEqual(A.GetSize(), 12);

			ZEChunkArray<int, 3> B;

			B.Add(30);
			B.Add(40);
			B.Add(50);
			B.Add(60);
			ZETestCheckEqual(B.GetCount(), 4);
			ZETestCheckEqual(B.GetSize(), 6);

			p = A.MassAdd(B.GetCArray(), 2);

			ZETestCheckEqual(A.GetCount(), 12);
			ZETestCheckEqual(A.GetSize(), 12);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 6> A;

			int *p;

			for (int I = 10; I < 20; I++)
			{
				A.Add(I);
			}
			ZETestCheckEqual(A.GetCount(), 10);
			ZETestCheckEqual(A.GetSize(), 42); //41

			ZESmartArray<int, 3> B;

			B.Add(30);
			B.Add(40);
			B.Add(50);
			B.Add(60);
			ZETestCheckEqual(B.GetCount(), 4);
			ZETestCheckEqual(B.GetSize(), 12); //11

			p = A.MassAdd(B.GetCArray(), 2);

			ZETestCheckEqual(A.GetCount(), 12);
			ZETestCheckEqual(A.GetSize(), 42); //41
		}
	}
	ZETest("inline Type* MassAdd(size_t ItemCount)")
	{
		ZEArray<int> A;

		A.Add(1);
		A.Add(2);

		ZEArray<int> B;

		B.Add(30);
		B.Add(40);
		B.Add(50);
		B.Add(60);

		int *p;

		p = A.MassAdd(2);

		p[0] = B[0];
		p[1] = B[1];
		ZETestCheckEqual(A.GetCount(), 4);
		ZETestCheckEqual(A.GetSize(), 4);
		ZETestCheckEqual(A[0], 1);
		ZETestCheckEqual(A[1], 2);
		ZETestCheckEqual(A[2], 30);
		ZETestCheckEqual(A[3], 40);
		ZETestCheckEqual(*p, 30);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 2> A;

			A.Add(1);
			A.Add(2);
			ZETestCheckEqual(A.GetSize(), 2);

			ZEChunkArray<int, 2> B;

			B.Add(30);
			B.Add(40);
			B.Add(50);
			B.Add(60);
			ZETestCheckEqual(B.GetSize(), 4);

			int *p;

			p = A.MassAdd(2);

			p[0] = B[0];
			p[1] = B[1];
			ZETestCheckEqual(A.GetCount(), 4);
			ZETestCheckEqual(A.GetSize(), 4); //6
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 2> A;

			A.Add(1);
			A.Add(2);
			ZETestCheckEqual(A.GetSize(), 2);

			ZESmartArray<int, 2> B;

			B.Add(30);
			B.Add(40);
			B.Add(50);
			B.Add(60);
			ZETestCheckEqual(B.GetSize(), 6);

			int *p;

			p = A.MassAdd(2);

			ZETestCheckEqual(A.GetCount(), 4);
			ZETestCheckEqual(A.GetSize(), 6); //8
		}
	}
	ZETest("inline Type* MassInsert(size_t Index, size_t ItemCount)")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		int *p;

		p = Array.MassInsert(1, 3);
		ZETestCheckEqual(Array.GetCount(), 6);
		ZETestCheckEqual(Array.GetSize(), 6);
		ZETestCheckEqual(Array[0], 1);
		ZETestCheckEqual(Array[1], -842150451);
		ZETestCheckEqual(Array[2], -842150451);
		ZETestCheckEqual(Array[3], -842150451);
		ZETestCheckEqual(Array[4], 2);
		ZETestCheckEqual(Array[5], 3);
		ZETestCheckEqual(*p, Array[1]);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 3> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ZETestCheckEqual(ChunkArray.GetSize(), 3);

			int *p;

			p = ChunkArray.MassInsert(1, 3);
			ZETestCheckEqual(ChunkArray.GetCount(), 6);
			ZETestCheckEqual(ChunkArray.GetSize(), 6); //9
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 3> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			ZETestCheckEqual(SmartArray.GetSize(), 3);

			int *p;

			p = SmartArray.MassInsert(1, 3);
			ZETestCheckEqual(SmartArray.GetCount(), 6);
			ZETestCheckEqual(SmartArray.GetSize(), 12); //18
		}

	}
	ZETest("inline Type* MassInsert(size_t Index, Type* NewItems, size_t ItemCount)")
	{
		ZEArray<int> A;

		int *p;

		for (int I = 10; I < 20; I++)
		{
			A.Add(I);
		}

		ZEArray<int> B;

		B.Add(30);
		B.Add(40);
		B.Add(50);
		B.Add(60);

		p = A.MassInsert(5, B.GetCArray(), 2);

		ZETestCheckEqual(A.GetCount(), 12);
		ZETestCheckEqual(A[0], 10);
		ZETestCheckEqual(A[1], 11);
		ZETestCheckEqual(A[2], 12);
		ZETestCheckEqual(A[3], 13);
		ZETestCheckEqual(A[4], 14);
		ZETestCheckEqual(A[5], 30);
		ZETestCheckEqual(A[6], 40);
		ZETestCheckEqual(A[7], 15);
		ZETestCheckEqual(A[8], 16);
		ZETestCheckEqual(A[9], 17);
		ZETestCheckEqual(A[10], 18);
		ZETestCheckEqual(A[11], 19);
		ZETestCheckEqual(*p, 30);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 5> A;

			int *p;

			for (int I = 10; I < 20; I++)
			{
				A.Add(I);
			}
			ZETestCheckEqual(A.GetCount(), 10);
			ZETestCheckEqual(A.GetSize(), 10);

			ZEChunkArray<int, 5> B;

			B.Add(30);
			B.Add(40);
			B.Add(50);
			B.Add(60);
			ZETestCheckEqual(B.GetCount(), 4);
			ZETestCheckEqual(B.GetSize(), 5);

			p = A.MassInsert(5, B.GetCArray(), 2);

			ZETestCheckEqual(A.GetCount(), 12);
			ZETestCheckEqual(A.GetSize(), 15);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 5> A;

			int *p;

			for (int I = 10; I < 20; I++)
			{
				A.Add(I);
			}
			ZETestCheckEqual(A.GetCount(), 10);
			ZETestCheckEqual(A.GetSize(), 30); //29

			ZESmartArray<int, 5> B;

			B.Add(30);
			B.Add(40);
			B.Add(50);
			B.Add(60);
			ZETestCheckEqual(B.GetCount(), 4);
			ZETestCheckEqual(B.GetSize(), 5);

			p = A.MassInsert(5, B.GetCArray(), 2);

			ZETestCheckEqual(A.GetCount(), 12);
			ZETestCheckEqual(A.GetSize(), 30); //29
		}
	}
	ZETest("inline Type& operator[](size_t Index)")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);

		int Element = Array[0];
		ZETestCheckEqual(Element, 1);

		Array[0] = 3;
		ZETestCheckEqual(Array[0], 3);
		ZETestCheckEqual(Array[1], 2);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 2> ChunkArray;

			for (int I = 1; I < 6; I++)
			{
				ChunkArray.Add(I);
			}
			ZETestCheckEqual(ChunkArray.GetCount(), 5);
			ZETestCheckEqual(ChunkArray.GetSize(), 6);

			int Element = ChunkArray[2];
			ZETestCheckEqual(Element, 3);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 2> SmartArray;

			for (int I =1; I < 6; I++)
			{
				SmartArray.Add(I);
			}
			ZETestCheckEqual(SmartArray.GetCount(), 5);
			ZETestCheckEqual(SmartArray.GetSize(), 6);

			int Element = SmartArray[3];
			ZETestCheckEqual(Element, 4);
		}
	}
	ZETest("inline const Type& operator[](size_t Index) const")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);

		const int Element = Array[0];
		ZETestCheckEqual(Element, 1);

		Array[0] = 5;
		ZETestCheckEqual(Array[0], 5);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 4> ChunkArray;

			for (int I = 1; I < 20; I++)
			{
				ChunkArray.Add(I);
			}
			ZETestCheckEqual(ChunkArray.GetCount(), 19);
			ZETestCheckEqual(ChunkArray.GetSize(), 20);

			const int Element = ChunkArray[5];
			ZETestCheckEqual(Element, 6);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 4> SmartArray;

			for (int I = 1; I < 20; I++)
			{
				SmartArray.Add(I);
			}
			ZETestCheckEqual(SmartArray.GetCount(), 19);
			ZETestCheckEqual(SmartArray.GetSize(), 20); //19

			const int Element = SmartArray[5];
			ZETestCheckEqual(Element, 6);
		}
	}
	ZETest("ZEArray operator+(const ZEArray<Type, Allocator_>& OtherArray)")
	{
		ZEArray<int> Array;

		Array.Add(10);
		Array.Add(20);

		ZEArray<int> OtherArray;

		OtherArray.Add(30);
		OtherArray.Add(40);
		OtherArray.Add(50);
		OtherArray.Add(60);

		ZEArray<int> Result;

		Result = Array + OtherArray;
		ZETestCheckEqual(Result.GetCount(), 6);
		ZETestCheckEqual(Result[0], 10);
		ZETestCheckEqual(Result[1], 20);
		ZETestCheckEqual(Result[2], 30);
		ZETestCheckEqual(Result[3], 40);
		ZETestCheckEqual(Result[4], 50);
		ZETestCheckEqual(Result[5], 60);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 4> ChunkArray;

			ChunkArray.Add(10);
			ChunkArray.Add(20);

			ZETestCheckEqual(ChunkArray.GetCount(), 2);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);

			ZEChunkArray<int, 4> OtherChunkArray;

			OtherChunkArray.Add(30);
			OtherChunkArray.Add(40);
			OtherChunkArray.Add(50);
			OtherChunkArray.Add(60);

			ZETestCheckEqual(OtherChunkArray.GetCount(), 4);
			ZETestCheckEqual(OtherChunkArray.GetSize(), 4);

			ZEChunkArray<int, 4> Result;

			/*Result = */ChunkArray + OtherChunkArray;
			ZETestCheckEqual(Result.GetCount(), 6);
			ZETestCheckEqual(Result.GetSize(), 8);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 4> SmartArray;

			SmartArray.Add(10);
			SmartArray.Add(20);

			ZETestCheckEqual(SmartArray.GetCount(), 2);
			ZETestCheckEqual(SmartArray.GetSize(), 4);

			ZESmartArray<int, 4> OtherSmartArray;

			OtherSmartArray.Add(30);
			OtherSmartArray.Add(40);
			OtherSmartArray.Add(50);
			OtherSmartArray.Add(60);

			ZETestCheckEqual(SmartArray.GetCount(), 4);
			ZETestCheckEqual(SmartArray.GetSize(), 4);

			ZESmartArray<int, 4> Result;

			/*Result = */SmartArray + OtherSmartArray;
			ZETestCheckEqual(Result.GetCount(), 6);
			ZETestCheckEqual(Result.GetSize(), 20);
		}

	}
	ZETest("ZEArray& operator+=(const ZEArray<Type, Allocator_>& OtherArray)")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);

		ZEArray<int> OtherArray;

		OtherArray.Add(8);
		OtherArray.Add(9);

		Array += OtherArray;
		ZETestCheckEqual(Array.GetCount(), 4);
		ZETestCheckEqual(Array[0], 1);
		ZETestCheckEqual(Array[1], 2);
		ZETestCheckEqual(Array[2], 8);
		ZETestCheckEqual(Array[3], 9);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 3> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);

			ZETestCheckEqual(ChunkArray.GetCount(), 2);
			ZETestCheckEqual(ChunkArray.GetSize(), 3);

			ZEChunkArray<int, 3> OtherChunkArray;

			OtherChunkArray.Add(8);
			OtherChunkArray.Add(9);

			ZETestCheckEqual(OtherChunkArray.GetCount(), 2);
			ZETestCheckEqual(OtherChunkArray.GetSize(), 3);

			ChunkArray += OtherChunkArray;
			ZETestCheckEqual(ChunkArray.GetCount(), 4);
			ZETestCheckEqual(ChunkArray.GetSize(), 6);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 3> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);

			ZETestCheckEqual(SmartArray.GetCount(), 2);
			ZETestCheckEqual(SmartArray.GetSize(), 3);

			ZESmartArray<int, 3> OtherSmartArray;

			OtherSmartArray.Add(8);
			OtherSmartArray.Add(9);

			ZETestCheckEqual(SmartArray.GetCount(), 2);
			ZETestCheckEqual(SmartArray.GetSize(), 3);

			SmartArray += OtherSmartArray;
			ZETestCheckEqual(SmartArray.GetCount(), 4);
			ZETestCheckEqual(SmartArray.GetSize(), 12);
		}
	}
	ZETest("inline void operator=(const ZEArray<Type, Allocator_>& Other)")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEArray<int> Other;

		Other.Add(4);
		Other.Add(5);

		Array = Other;
		ZETestCheckEqual(Array.GetCount(), 2);
		ZETestCheckEqual(Array[0], 4);
		ZETestCheckEqual(Array[1], 5);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);

			ZETestCheckEqual(ChunkArray.GetCount(), 3);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);

			ZEChunkArray<int, 2> OtherChunkArray;

			OtherChunkArray.Add(4);
			OtherChunkArray.Add(5);

			ZETestCheckEqual(OtherChunkArray.GetCount(), 2);
			ZETestCheckEqual(OtherChunkArray.GetSize(), 2);

			ChunkArray = OtherChunkArray;
			ZETestCheckEqual(ChunkArray.GetCount(), 2);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);

			ZETestCheckEqual(SmartArray.GetCount(), 3);
			ZETestCheckEqual(SmartArray.GetSize(), 6);

			ZESmartArray<int, 2> OtherSmartArray;

			OtherSmartArray.Add(4);
			OtherSmartArray.Add(5);

			ZETestCheckEqual(OtherSmartArray.GetCount(), 2);
			ZETestCheckEqual(OtherSmartArray.GetSize(), 2);

			SmartArray = OtherSmartArray;
			ZETestCheckEqual(SmartArray.GetCount(), 2);
			ZETestCheckEqual(SmartArray.GetSize(), 6);
		}
	}
	ZETest("inline Type Pop()")
	{
		ZEArray<int> Array;

		Array.Insert(0, 0);
		Array.Insert(1, 3);

		int Result = Array.Pop();
		ZETestCheckEqual(Result, 3);
		ZETestCheckEqual(Array.GetCount(), 1);
		ZETestCheckEqual(Array[0], 0);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 4> ChunkArray;

			for (int I = 1; I < 22; I++)
			{
				ChunkArray.Add(I);
			}
			ZETestCheckEqual(ChunkArray.GetCount(), 21);
			ZETestCheckEqual(ChunkArray.GetSize(), 24);

			int Result = ChunkArray.Pop();
			ZETestCheckEqual(Result, 21);
			ZETestCheckEqual(ChunkArray.GetCount(), 20);
			ZETestCheckEqual(ChunkArray.GetSize(), 24);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 4> SmartArray;

			for (int I = 1; I < 22; I++)
			{
				SmartArray.Add(I);
			}
			ZETestCheckEqual(SmartArray.GetCount(), 21);
			ZETestCheckEqual(SmartArray.GetSize(), 84); //79

			int Result = SmartArray.Pop();
			ZETestCheckEqual(Result, 21);
			ZETestCheckEqual(SmartArray.GetCount(), 20);
			ZETestCheckEqual(SmartArray.GetSize(), 84); //79
		}
	}
	ZETest("inline void Push(Type Value)")
	{
		ZEArray<int> Array;

		Array.Push(3);
		Array.Push(5);
		ZETestCheckEqual(Array.GetCount(), 2);
		ZETestCheckEqual(Array[0], 3);
		ZETestCheckEqual(Array[1], 5);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 6> ChunkArray;

			for (int I = 1; I < 13; I++)
			{
				ChunkArray.Add(I);
			}
			ZETestCheckEqual(ChunkArray.GetCount(), 12);
			ZETestCheckEqual(ChunkArray.GetSize(), 12);

			ChunkArray.Push(13);
			ZETestCheckEqual(ChunkArray[12], 13);
			ZETestCheckEqual(ChunkArray.GetCount(), 13);
			ZETestCheckEqual(ChunkArray.GetSize(), 18);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 6> SmartArray;

			for (int I = 1; I < 13; I++)
			{
				SmartArray.Add(I);
			}
			ZETestCheckEqual(SmartArray.GetCount(), 12);
			ZETestCheckEqual(SmartArray.GetSize(), 42); //41

			SmartArray.Push(13);
			ZETestCheckEqual(SmartArray[12], 13);
			ZETestCheckEqual(SmartArray.GetCount(), 13);
			ZETestCheckEqual(SmartArray.GetSize(), 42); //41
		}
	}
	ZETest("inline void Resize(size_t Count)")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		Array.Resize(2);
		ZETestCheckEqual(Array.GetCount(), 2);
		ZETestCheckEqual(Array[0], 1);
		ZETestCheckEqual(Array[1], 2);

		ZETestCase("Resize for count value bigger than array size")
		{
			Array.Resize(5);
			ZETestCheckEqual(Array.GetCount(), 5);
		}

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 4> ChunkArray;

			for (int I = 1; I < 6; I++)
			{
				ChunkArray.Add(I);
			}
			ZETestCheckEqual(ChunkArray.GetCount(), 5);
			ZETestCheckEqual(ChunkArray.GetSize(), 8);

			ChunkArray.Resize(3);
			ZETestCheckEqual(ChunkArray.GetCount(), 3);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);

			ChunkArray.Resize(9);
			ZETestCheckEqual(ChunkArray.GetCount(), 9);
			ZETestCheckEqual(ChunkArray.GetSize(), 12);

			ChunkArray.Resize(12);
			ZETestCheckEqual(ChunkArray.GetCount(), 12);
			ZETestCheckEqual(ChunkArray.GetSize(), 12);

			ChunkArray.Resize(3);
			ZETestCheckEqual(ChunkArray.GetCount(), 3);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 4> SmartArray;

			for (int I = 1; I < 6; I++)
			{
				SmartArray.Add(I);
			}
			ZETestCheckEqual(SmartArray.GetCount(), 5);
			ZETestCheckEqual(SmartArray.GetSize(), 20); //19

			SmartArray.Resize(3);
			ZETestCheckEqual(SmartArray.GetCount(), 3);
			ZETestCheckEqual(SmartArray.GetSize(), 4); //12

			SmartArray.Resize(21);
			ZETestCheckEqual(SmartArray.GetCount(), 21);
			ZETestCheckEqual(SmartArray.GetSize(), 84);

			SmartArray.Resize(20);
			ZETestCheckEqual(SmartArray.GetCount(), 20);
			size_t Size = SmartArray.GetSize();	//79
		}
	}
	ZETest("inline void SetCount(size_t Count)")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZETestCheckEqual(Array.GetCount(), 3);

		Array.SetCount(2);

		ZETestCheckEqual(Array.GetCount(), 2);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 3> ChunkArray;

			ChunkArray.SetCount(2);
			ZETestCheckEqual(ChunkArray.GetCount(), 2);
			ZETestCheckEqual(ChunkArray.GetSize(), 3);

			ChunkArray.SetCount(4);
			ZETestCheckEqual(ChunkArray.GetCount(), 4);
			ZETestCheckEqual(ChunkArray.GetSize(), 6);

			ChunkArray.SetCount(7);
			ZETestCheckEqual(ChunkArray.GetCount(), 7);
			ZETestCheckEqual(ChunkArray.GetSize(), 9);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 3> SmartArray;

			SmartArray.SetCount(2);
			ZETestCheckEqual(SmartArray.GetCount(), 2);
			ZETestCheckEqual(SmartArray.GetSize(), 3); //5

			SmartArray.SetCount(4);
			ZETestCheckEqual(SmartArray.GetCount(), 4);
			ZETestCheckEqual(SmartArray.GetSize(), 12); //5

			SmartArray.SetCount(13);
			ZETestCheckEqual(SmartArray.GetCount(), 13);
			ZETestCheckEqual(SmartArray.GetSize(), 39);

		}
	}
	ZETest("inline void SetItem(size_t Index, Type Value)")
	{
		ZEArray<int> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		Array.SetItem(2, 7);
		ZETestCheckEqual(Array.GetCount(), 3);
		ZETestCheckEqual(Array[0], 1);
		ZETestCheckEqual(Array[1], 2);
		ZETestCheckEqual(Array[2], 7);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);

			ChunkArray.SetItem(2, 7);
			ZETestCheckEqual(ChunkArray[2], 7);
			ZETestCheckEqual(ChunkArray.GetCount(), 3);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);

			SmartArray.SetItem(2, 7);
			ZETestCheckEqual(SmartArray[2], 7);
			ZETestCheckEqual(SmartArray.GetCount(), 3);
			ZETestCheckEqual(SmartArray.GetSize(), 6);
		}
	}
	ZETest("inline void Sort(int (*CompareFunction)(const void*, const void*))")
	{
		ZEArray<int> Array;

		Array.Add(0);
		Array.Add(2);
		Array.Add(3);
		Array.Add(1);

		Array.Sort(&Compare);

		ZETestCheckEqual(Array[0], 0);
		ZETestCheckEqual(Array[1], 1);
		ZETestCheckEqual(Array[2], 2);
		ZETestCheckEqual(Array[3], 3);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<int, 4> ChunkArray;

			ChunkArray.Add(3);
			ChunkArray.Add(0);
			ChunkArray.Add(9);
			ChunkArray.Add(6);
			ChunkArray.Add(12);

			ChunkArray.Sort(&Compare);

			ZETestCheckEqual(ChunkArray.GetCount(), 5);
			ZETestCheckEqual(ChunkArray.GetSize(), 8);
			ZETestCheckEqual(ChunkArray[0], 0);
			ZETestCheckEqual(ChunkArray[1], 3);
			ZETestCheckEqual(ChunkArray[2], 6);
			ZETestCheckEqual(ChunkArray[3], 9);
			ZETestCheckEqual(ChunkArray[4], 12);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<int, 4> SmartArray;

			SmartArray.Add(3);
			SmartArray.Add(0);
			SmartArray.Add(9);
			SmartArray.Add(6);
			SmartArray.Add(12);

			SmartArray.Sort(&Compare);

			ZETestCheckEqual(SmartArray.GetCount(), 5);
			ZETestCheckEqual(SmartArray.GetSize(), 20); //19
			ZETestCheckEqual(SmartArray[0], 0);
			ZETestCheckEqual(SmartArray[1], 3);
			ZETestCheckEqual(SmartArray[2], 6);
			ZETestCheckEqual(SmartArray[3], 9);
			ZETestCheckEqual(SmartArray[4], 12);
		}
	}
	ZETest("ZEArray()")
	{

	}
	ZETest("ZEArray(const ZEArray<Type, Allocator_>& C)")
	{

	}
}
