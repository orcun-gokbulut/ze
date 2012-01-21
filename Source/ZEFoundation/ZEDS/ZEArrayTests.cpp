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

ZEInt Compare(const ZEInt *F, const ZEInt *E)
{
	ZEInt A = *F;
	ZEInt B = *E;

	ZEInt ReturnValue = 0;

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
		ZEArray<ZEInt> Array;

		ZEInt *p;

		p = Array.Add();
		*p = 5;

		p = Array.Add();
		*p = 2;
		ZETestCheckEqual(Array[0], 5);
		ZETestCheckEqual(Array[1], 2);

		ZETestCase("for ZEChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ZEInt *p;
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
			ZESmartArray<ZEInt, 2> SmartArray;

			ZEInt *p;
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
		ZEArray<ZEInt> Array;

		ZEInt *p;

		p = Array.Add(1);
		p = Array.Add(2);
		ZETestCheckEqual(Array[0], 1);
		ZETestCheckEqual(Array[1], 2);
		ZETestCheckEqual(*p, 2);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 4> ChunkArray;

			ZEInt *p;

			for (ZEInt I = 10; I < 16; I++)
			{
				p = ChunkArray.Add(I);
			}

			ZETestCheckEqual(ChunkArray.GetCount(), 6);
			ZETestCheckEqual(ChunkArray.GetSize(), 8);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			ZEInt *p;

			for (ZEInt I = 10; I < 16; I++)
			{
				p = SmartArray.Add(I);
			}

			ZETestCheckEqual(SmartArray.GetCount(), 6);
			ZETestCheckEqual(SmartArray.GetSize(), 6);
		}
	}
	ZETest("inline Type* AddByRef(const Type& NewItem)")
	{
		ZEArray<ZEInt> Array;

		ZEInt NewItem = 3;
		ZEInt *p;

		p = Array.AddByRef(NewItem);
		p = Array.AddByRef(2);
		ZETestCheckEqual(Array[0], 3);
		ZETestCheckEqual(Array[1], 2);
		ZETestCheckEqual(*p, 2);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 5> ChunkArray;

			ZEInt *p;

			for (ZEInt NewItem = 10; NewItem < 16; NewItem++)
			{
				p = ChunkArray.AddByRef(NewItem);
			}
			ZETestCheckEqual(ChunkArray.GetCount(), 6);
			ZETestCheckEqual(ChunkArray.GetSize(), 10);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 5> SmartArray;

			ZEInt *p;

			for (ZEInt NewItem = 10; NewItem < 16; NewItem++)
			{
				p = SmartArray.AddByRef(NewItem);
			}
			ZETestCheckEqual(SmartArray.GetCount(), 6);		
			ZETestCheckEqual(SmartArray.GetSize(), 30);
		}
	}
	ZETest("inline ZEInt BinarySearch(const Type& Element, ZEInt (*CompareFunction)(Type*, Type*))")
	{
		ZEArray<ZEInt> Array;

		Array.Add(0);
		Array.Add(5);
		Array.Add(10);
		Array.Add(15);
		Array.Add(20);
		Array.Add(25);

		const ZEInt Element = 20;

		ZEInt Result = Array.BinarySearch(Element, &Compare);
		ZETestCheckEqual(Result, 4);
	}
	ZETest("inline ZESize Circular(ZEInt Index) const")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);
		
		ZETestCase("for Index < 0")
		{
			ZEInt Index = -5;

			ZESize Result = Array.Circular(Index);
			ZETestCheckEqual(Result, 1);
		}
		ZETestCase("for Index > 0")
		{
			ZEInt Index = 5;

			ZESize Result = Array.Circular(Index);
			ZETestCheckEqual(Result, 2);
		}
		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 3> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ChunkArray.Add(4);

			ZETestCheckEqual(ChunkArray.GetCount(), 4);
			ZETestCheckEqual(ChunkArray.GetSize(), 6);

			ZEInt Index = -4;

			ZESize Result = ChunkArray.Circular(Index);
			ZETestCheckEqual(Result, 0);

			Index = 4;

			Result = ChunkArray.Circular(Index);
			ZETestCheckEqual(Result, 0);
			ZETestCheckEqual(ChunkArray.GetCount(), 4);
			ZETestCheckEqual(ChunkArray.GetSize(), 6);
		}
		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 3> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			SmartArray.Add(4);

			ZETestCheckEqual(SmartArray.GetCount(), 4);
			ZETestCheckEqual(SmartArray.GetSize(), 12);

			ZEInt Index = -4;

			ZESize Result = SmartArray.Circular(Index);
			ZETestCheckEqual(Result, 0);

			Index = 4;

			Result = SmartArray.Circular(Index);
			ZETestCheckEqual(Result, 0);
			ZETestCheckEqual(SmartArray.GetCount(), 4);
			ZETestCheckEqual(SmartArray.GetSize(), 12);
		}
	}
	ZETest("inline void Clear()")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);

		Array.Clear();
		ZETestCheckEqual(Array.GetCount(), 0);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

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
			ZESmartArray<ZEInt, 2> SmartArray;

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
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);

		ZEArray<ZEInt> OtherArray;

		OtherArray.Add(3);
		OtherArray.Add(4);

		Array.Combine(OtherArray);
		ZETestCheckEqual(Array[0], 1);
		ZETestCheckEqual(Array[1], 2);
		ZETestCheckEqual(Array[2], 3);
		ZETestCheckEqual(Array[3], 4);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);

			ZEChunkArray<ZEInt, 2> OtherChunkArray;

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
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);

			ZESmartArray<ZEInt, 2> OtherSmartArray;

			OtherSmartArray.Add(4);
			OtherSmartArray.Add(5);
			OtherSmartArray.Add(6);

			SmartArray.Combine(OtherSmartArray);

			ZETestCheckEqual(SmartArray.GetCount(), 6);
			ZETestCheckEqual(SmartArray.GetSize(), 6);
			ZETestCheckEqual(OtherSmartArray.GetSize(), 6);
		}
	}
	ZETest("void CopyFrom(const Type* OtherArray, ZESize Count)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEArray<ZEInt> OtherArray;

		OtherArray.Add(4);
		OtherArray.Add(5);
		OtherArray.Add(6);

		Array.CopyFrom(OtherArray.GetCArray(), 2);
		ZETestCheckEqual(Array.GetCount(), 2);
		ZETestCheckEqual(Array[0], 4);
		ZETestCheckEqual(Array[1], 5);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 3> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);

			ZEChunkArray<ZEInt, 3> OtherChunkArray;

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
			ZESmartArray<ZEInt, 3> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);

			ZESmartArray<ZEInt, 3> OtherSmartArray;

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
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEArray<ZEInt> OtherArray;

		OtherArray.Add(4);
		OtherArray.Add(5);

		Array.CopyFrom(OtherArray);
		ZETestCheckEqual(Array.GetCount(), 2);
		ZETestCheckEqual(Array[0], 4);
		ZETestCheckEqual(Array[1], 5);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);

			ZEChunkArray<ZEInt, 2> OtherChunkArray;

			OtherChunkArray.Add(4);
			OtherChunkArray.Add(5);

			ChunkArray.CopyFrom(OtherChunkArray);

			ZETestCheckEqual(ChunkArray.GetCount(), 2);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);
			ZETestCheckEqual(OtherChunkArray.GetSize(), 2);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			ZETestCheckEqual(SmartArray.GetSize(), 6);

			ZESmartArray<ZEInt, 2> OtherSmartArray;

			OtherSmartArray.Add(4);
			OtherSmartArray.Add(5);

			SmartArray.CopyFrom(OtherSmartArray);

			ZETestCheckEqual(SmartArray.GetCount(), 2);
			ZETestCheckEqual(SmartArray.GetSize(), 6); //4
			ZETestCheckEqual(OtherSmartArray.GetSize(), 2);
		}
	}
	ZETest("void CopyTo(Type* OtherArray, ZESize Count) const")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEArray<ZEInt> OtherArray;

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
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEArray<ZEInt> OtherArray;

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
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);

			ZEChunkArray<ZEInt, 2> OtherChunkArray;

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
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			ZETestCheckEqual(SmartArray.GetSize(), 6);

			ZESmartArray<ZEInt, 2> OtherSmartArray;

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
	ZETest("inline void DeleteAt(ZESize Index)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		Array.DeleteAt(1);
		ZETestCheckEqual(Array.GetCount(), 2);
		ZETestCheckEqual(Array[0], 1);
		ZETestCheckEqual(Array[1], 3);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

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
			ZESmartArray<ZEInt, 2> SmartArray;

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
		ZEArray<ZEInt> Array;

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
			ZEChunkArray<ZEInt, 2> ChunkArray;

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
			ZESmartArray<ZEInt, 2> SmartArray;

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
		ZEArray<ZEInt> Array;

		Array.Add(3);
		Array.Add(2);

		ZEInt Element = Array.Dequeue();
		ZETestCheckEqual(Array.GetCount(), 1);
		ZETestCheckEqual(Array[0], 2);
		ZETestCheckEqual(Element, 3);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(3);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ZETestCheckEqual(ChunkArray.GetSize(), 6);

			ZEInt Element = ChunkArray.Dequeue();

			ZETestCheckEqual(ChunkArray.GetCount(), 4);
			ZETestCheckEqual(ChunkArray.GetSize(), 6);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(3);
			SmartArray.Add(2);
			SmartArray.Add(3);
			SmartArray.Add(2);
			SmartArray.Add(3);
			ZETestCheckEqual(SmartArray.GetSize(), 6);

			ZEInt Element = SmartArray.Dequeue();

			ZETestCheckEqual(SmartArray.GetCount(), 4);
			ZETestCheckEqual(SmartArray.GetSize(), 6);
		}
	}
	ZETest("inline void Enqueue(Type Value)")
	{
		ZEArray<ZEInt> Array;
		ZEInt Value = 3;

		Array.Add(1);
		Array.Add(2);

		Array.Enqueue(Value);
		ZETestCheckEqual(Array.GetCount(), 3);
		ZETestCheckEqual(Array[0], 1);
		ZETestCheckEqual(Array[1], 2);
		ZETestCheckEqual(Array[2], 3);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;
			ZEInt Value = 3;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ZETestCheckEqual(ChunkArray.GetSize(), 2);

			ChunkArray.Enqueue(Value);

			ZETestCheckEqual(ChunkArray.GetCount(), 3);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;
			ZEInt Value = 3;

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
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);
		Array.Add(4);

		ZEInt Value1 = 2;
		ZEInt Value2 = 5;

		bool Result1 = Array.Exists(Value1);
		bool Result2 = Array.Exists(Value2);
		ZETestCheckEqual(Result1, true);
		ZETestCheckEqual(Result2, false);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ChunkArray.Add(4);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);

			ZEInt Value1 = 2;
			ZEInt Value2 = 5;

			bool Result1 = ChunkArray.Exists(Value1);
			bool Result2 = ChunkArray.Exists(Value2);
			ZETestCheckEqual(Result1, true);
			ZETestCheckEqual(Result2, false);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			SmartArray.Add(4);
			ZETestCheckEqual(SmartArray.GetSize(), 6);

			ZEInt Value1 = 2;
			ZEInt Value2 = 5;

			bool Result1 = SmartArray.Exists(Value1);
			bool Result2 = SmartArray.Exists(Value2);
			ZETestCheckEqual(Result1, true);
			ZETestCheckEqual(Result2, false);
		}
	}
	ZETest("inline void Fill(Type Value)")
	{
		ZEArray<ZEInt> Array;

		Array.SetCount(5);
		ZEInt Value = 7;

		Array.Fill(Value);
		ZETestCheckEqual(Array.GetCount(), 5);
		ZETestCheckEqual(Array[0], 7);
		ZETestCheckEqual(Array[1], 7);
		ZETestCheckEqual(Array[2], 7);
		ZETestCheckEqual(Array[3], 7);
		ZETestCheckEqual(Array[4], 7);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 4> ChunkArray;

			ChunkArray.SetCount(5);
			ZEInt Value = 7;

			ChunkArray.Fill(Value);

			ZETestCheckEqual(ChunkArray.GetCount(), 5);
			ZETestCheckEqual(ChunkArray.GetSize(), 8);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 4> SmartArray;

			SmartArray.SetCount(5);
			ZEInt Value = 7;

			SmartArray.Fill(Value);

			ZETestCheckEqual(SmartArray.GetCount(), 5);
			ZETestCheckEqual(SmartArray.GetSize(), 20);
		}
	}
	ZETest("inline ZEInt FindIndex(Type Item, ZEInt StartIndex = 0)const")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		ZEInt Result = Array.FindIndex(12);
		ZETestCheckEqual(Result, 2);

		ZETestCase("FindIndex for searched value is not in the array")
		{
			ZEInt Result = Array.FindIndex(25);
			ZETestCheckEqual(Result, -1);
		}
		ZETestCase("FindIndex for searched value is in the array but before the specified starting index")
		{
			ZEInt Result = Array.FindIndex(12, 4);
			ZETestCheckEqual(Result, -1);
		}

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 6> ChunkArray;

			for (ZEInt I = 10; I < 20; I++)
			{
				ChunkArray.Add(I);
			}
			ZETestCheckEqual(ChunkArray.GetCount(), 10);
			ZETestCheckEqual(ChunkArray.GetSize(), 12);

			ZEInt Result = ChunkArray.FindIndex(12);
			ZETestCheckEqual(Result, 2);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 6> SmartArray;

			for (ZEInt I = 10; I < 20; I++)
			{
				SmartArray.Add(I);
			}
			ZETestCheckEqual(SmartArray.GetCount(), 10);
			ZETestCheckEqual(SmartArray.GetSize(), 42);

			ZEInt Result = SmartArray.FindIndex(12);
			ZETestCheckEqual(Result, 2);
		}
	}
	ZETest("inline Type* GetCArray()")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEInt *p;

		p = Array.GetCArray();
		ZETestCheckEqual(*p, 1);

		p[0] = 5;
		ZETestCheckEqual(*p, 5);
		ZETestCheckEqual(Array[0], 5);
		ZETestCheckEqual(Array[1], 2);
		ZETestCheckEqual(Array[2], 3);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);

			ZEInt *p;

			p = ChunkArray.GetCArray();
			ZETestCheckEqual(*p, 1);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);

			ZEInt *p;

			p = SmartArray.GetCArray();
			ZETestCheckEqual(*p, 1);
			ZETestCheckEqual(SmartArray.GetSize(), 6);
		}
	}
	ZETest("inline const Type* GetConstCArray() const")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		const ZEInt *p;

		p = Array.GetConstCArray();
		ZETestCheckEqual(*p, 10);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 7> ChunkArray;

			for (ZEInt I = 10; I < 20; I++)
			{
				ChunkArray.Add(I);
			}

			const ZEInt *p;

			p = ChunkArray.GetConstCArray();
			ZETestCheckEqual(*p, 10);
			ZETestCheckEqual(ChunkArray.GetCount(), 10);
			ZETestCheckEqual(ChunkArray.GetSize(), 14);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 7> SmartArray;

			for (ZEInt I = 10; I < 20; I++)
			{
				SmartArray.Add(I);
			}

			const ZEInt *p;

			p = SmartArray.GetConstCArray();
			ZETestCheckEqual(*p, 10);
			ZETestCheckEqual(SmartArray.GetCount(), 10);
			ZETestCheckEqual(SmartArray.GetSize(), 56);
		}
	}
	ZETest("inline ZESize GetCount() const")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		ZEInt Count = Array.GetCount();
		ZETestCheckEqual(Count, 10);

		ZETestCase("for ZEChunkArray")
		{
			ZEChunkArray<ZEInt, 5> ChunkArray;

			for (ZEInt I = 1; I < 7; I++)
			{
				ChunkArray.Add(I);
			}

			ZEInt Count = ChunkArray.GetCount();
			ZEInt Size = ChunkArray.GetSize();
			ZETestCheckEqual(Count, 6);
			ZETestCheckEqual(Size, 10);
		}

		ZETestCase("for ZESmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			for (ZEInt I = 1; I < 8; I++)
			{
				SmartArray.Add(I);
			}

			ZETestCheckEqual(SmartArray.GetCount(), 7);
			ZETestCheckEqual(SmartArray.GetSize(), 14);
		}
	}
	ZETest("inline Type& GetFirstItem()")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		ZEInt Result = Array.GetFirstItem();
		ZETestCheckEqual(Result, 10);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 8> ChunkArray;

			for (ZEInt I =10; I < 20; I++)
			{
				ChunkArray.Add(I);
			}

			ZEInt Result = ChunkArray.GetFirstItem();
			ZETestCheckEqual(Result, 10);
			ZETestCheckEqual(ChunkArray.GetCount(), 10);
			ZETestCheckEqual(ChunkArray.GetSize(), 16);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 8> SmartArray;

			for (ZEInt I = 10; I < 20; I++)
			{
				SmartArray.Add(I);
			}

			ZEInt Result = SmartArray.GetFirstItem();
			ZETestCheckEqual(Result, 10);
			ZETestCheckEqual(SmartArray.GetCount(), 10);
			ZETestCheckEqual(SmartArray.GetSize(), 72);
		}
	}
	ZETest("inline const Type& GetFirstItem()const")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		const ZEInt Result = Array.GetFirstItem();
		ZETestCheckEqual(Result, 10);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 8> ChunkArray;

			for (ZEInt I =10; I < 20; I++)
			{
				ChunkArray.Add(I);
			}

			const ZEInt Result = ChunkArray.GetFirstItem();
			ZETestCheckEqual(Result, 10);
			ZETestCheckEqual(ChunkArray.GetCount(), 10);
			ZETestCheckEqual(ChunkArray.GetSize(), 16);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 8> SmartArray;

			for (ZEInt I = 10; I < 20; I++)
			{
				SmartArray.Add(I);
			}

			const ZEInt Result = SmartArray.GetFirstItem();
			ZETestCheckEqual(Result, 10);
			ZETestCheckEqual(SmartArray.GetCount(), 10);
			ZETestCheckEqual(SmartArray.GetSize(), 72);
		}
	}
	ZETest("inline Type& GetItem(ZESize Index)")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		ZEInt Result = Array.GetItem(2);
		ZETestCheckEqual(Result, 12);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 9> ChunkArray;

			for (ZEInt I = 10; I < 30; I++)
			{
				ChunkArray.Add(I);
			}

			ZEInt Result = ChunkArray.GetItem(13);
			ZETestCheckEqual(Result, 23);
			ZETestCheckEqual(ChunkArray.GetCount(), 20);
			ZETestCheckEqual(ChunkArray.GetSize(), 27);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 9> SmartArray;

			for (ZEInt I =10; I < 30; I++)
			{
				SmartArray.Add(I);
			}

			ZEInt Result = SmartArray.GetItem(13);
			ZETestCheckEqual(Result, 23);
			ZETestCheckEqual(SmartArray.GetCount(), 20);
			ZETestCheckEqual(SmartArray.GetSize(), 90);
		}
	}
	ZETest("inline const Type& GetItem(ZESize Index) const")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		const ZEInt Result = Array.GetItem(6);
		ZETestCheckEqual(Result, 16);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 10> ChunkArray;

			for (ZEInt I = 10; I < 30; I++)
			{
				ChunkArray.Add(I);
			}

			const ZEInt Result = ChunkArray.GetItem(13);
			ZETestCheckEqual(Result, 23);
			ZETestCheckEqual(ChunkArray.GetCount(), 20);
			ZETestCheckEqual(ChunkArray.GetSize(), 20);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 10> SmartArray;

			for (ZEInt I = 10; I < 30; I++)
			{
				SmartArray.Add(I);
			}

			const ZEInt Result = SmartArray.GetItem(13);
			ZETestCheckEqual(Result, 23);
			ZETestCheckEqual(SmartArray.GetCount(), 20);
			ZETestCheckEqual(SmartArray.GetSize(), 110);
		}
	}
	ZETest("inline Type& GetLastItem()")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		ZEInt Result = Array.GetLastItem();
		ZETestCheckEqual(Result, 19);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 15> ChunkArray;

			for (ZEInt I = 10; I < 30; I++)
			{
				ChunkArray.Add(I);
			}
			ZEInt Result = ChunkArray.GetLastItem();
			ZETestCheckEqual(Result, 29);
			ZETestCheckEqual(ChunkArray.GetCount(), 20);
			ZETestCheckEqual(ChunkArray.GetSize(), 30);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 15> SmartArray;

			for (ZEInt I = 10; I < 30; I++)
			{
				SmartArray.Add(I);
			}
			ZEInt Result = SmartArray.GetLastItem();
			ZETestCheckEqual(Result, 29);
			ZETestCheckEqual(SmartArray.GetCount(), 20);
			ZETestCheckEqual(SmartArray.GetSize(), 240);
		}
	}
	ZETest("inline const Type& GetLastItem()const")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		const ZEInt Result = Array.GetLastItem();
		ZETestCheckEqual(Result, 19);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 15> ChunkArray;

			for (ZEInt I = 10; I < 30; I++)
			{
				ChunkArray.Add(I);
			}
			const ZEInt Result = ChunkArray.GetLastItem();
			ZETestCheckEqual(Result, 29);
			ZETestCheckEqual(ChunkArray.GetCount(), 20);
			ZETestCheckEqual(ChunkArray.GetSize(), 30);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 15> SmartArray;

			for (ZEInt I = 10; I < 30; I++)
			{
				SmartArray.Add(I);
			}
			const ZEInt Result = SmartArray.GetLastItem();
			ZETestCheckEqual(Result, 29);
			ZETestCheckEqual(SmartArray.GetCount(), 20);
			ZETestCheckEqual(SmartArray.GetSize(), 240);
		}
	}
	ZETest("inline ZESize GetSize() const")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 15; I++)
		{
			Array.Add(I);
		}

		ZEInt Size = Array.GetSize();
		ZETestCheckEqual(Size, 5);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 3> ChunkArray;

			for (ZEInt I = 10; I < 15; I++)
			{
				ChunkArray.Add(I);
			}

			ZETestCheckEqual(ChunkArray.GetSize(), 6);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 3> SmartArray;

			for (ZEInt I = 10; I < 15; I++)
			{
				SmartArray.Add(I);
			}

			ZETestCheckEqual(SmartArray.GetSize(), 12);
		}
	}
	ZETest("inline Type* Insert(ZESize Index)")
	{
		ZEArray<ZEInt> Array;

		ZEInt *p;

		Array.Insert(0, 0);
		Array.Insert(1, 1);

		p = Array.Insert(1);

		*p=5;

		ZETestCheckEqual(Array[0], 0);
		ZETestCheckEqual(Array[1], 5);
		ZETestCheckEqual(Array[2], 1);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 5> ChunkArray;

			ZEInt *p;

			for (ZEInt I = 1; I < 6; I++)
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
			ZESmartArray<ZEInt, 5> SmartArray;

			ZEInt *p;

			for (ZEInt I = 1; I < 6; I++)
			{
				SmartArray.Add(I);
			}
			ZETestCheckEqual(SmartArray.GetSize(), 5);

			p = SmartArray.Insert(0);

			*p = 8;

			ZETestCheckEqual(SmartArray.GetCount(), 6);
			ZETestCheckEqual(SmartArray.GetSize(), 30);
		}
	}
	ZETest("inline Type* Insert(ZESize Index, Type NewItem)")
	{
		ZEArray<ZEInt> Array;

		ZEInt *p;

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
			ZEChunkArray<ZEInt, 3> ChunkArray;

			ZEInt *p;

			p = ChunkArray.Insert(0, 3);
			p = ChunkArray.Insert(1, 4);
			p = ChunkArray.Insert(2, 5);
			p = ChunkArray.Insert(1, 0);

			ZETestCheckEqual(ChunkArray.GetCount(), 4);
			ZETestCheckEqual(ChunkArray.GetSize(), 6);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 3> SmartArray;

			ZEInt *p;

			p = SmartArray.Insert(0, 3);
			p = SmartArray.Insert(1, 4);
			p = SmartArray.Insert(2, 5);
			p = SmartArray.Insert(1, 0);

			ZETestCheckEqual(SmartArray.GetCount(), 4);
			ZETestCheckEqual(SmartArray.GetSize(), 12);
		}
	}
	ZETest("inline Type* MassAdd(const Type* NewItems, ZESize ItemCount)")
	{
		ZEArray<ZEInt> A;

		ZEInt *p;

		for (ZEInt I = 10; I < 20; I++)
		{
			A.Add(I);
		}

		ZEArray<ZEInt> B;

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
			ZEChunkArray<ZEInt, 6> A;

			ZEInt *p;

			for (ZEInt I = 10; I < 20; I++)
			{
				A.Add(I);
			}
			ZETestCheckEqual(A.GetCount(), 10);
			ZETestCheckEqual(A.GetSize(), 12);

			ZEChunkArray<ZEInt, 3> B;

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
			ZESmartArray<ZEInt, 6> A;

			ZEInt *p;

			for (ZEInt I = 10; I < 20; I++)
			{
				A.Add(I);
			}
			ZETestCheckEqual(A.GetCount(), 10);
			ZETestCheckEqual(A.GetSize(), 42);

			ZESmartArray<ZEInt, 3> B;

			B.Add(30);
			B.Add(40);
			B.Add(50);
			B.Add(60);
			ZETestCheckEqual(B.GetCount(), 4);
			ZETestCheckEqual(B.GetSize(), 12);

			p = A.MassAdd(B.GetCArray(), 2);

			ZETestCheckEqual(A.GetCount(), 12);
			ZETestCheckEqual(A.GetSize(), 42);
		}
	}
	ZETest("inline Type* MassAdd(ZESize ItemCount)")
	{
		ZEArray<ZEInt> A;

		A.Add(1);
		A.Add(2);

		ZEArray<ZEInt> B;

		B.Add(30);
		B.Add(40);
		B.Add(50);
		B.Add(60);

		ZEInt *p;

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
			ZEChunkArray<ZEInt, 2> A;

			A.Add(1);
			A.Add(2);
			ZETestCheckEqual(A.GetSize(), 2);

			ZEChunkArray<ZEInt, 2> B;

			B.Add(30);
			B.Add(40);
			B.Add(50);
			B.Add(60);
			ZETestCheckEqual(B.GetSize(), 4);

			ZEInt *p;

			p = A.MassAdd(2);

			p[0] = B[0];
			p[1] = B[1];
			ZETestCheckEqual(A.GetCount(), 4);
			ZETestCheckEqual(A.GetSize(), 6);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> A;

			A.Add(1);
			A.Add(2);
			ZETestCheckEqual(A.GetSize(), 2);

			ZESmartArray<ZEInt, 2> B;

			B.Add(30);
			B.Add(40);
			B.Add(50);
			B.Add(60);
			ZETestCheckEqual(B.GetSize(), 6);

			ZEInt *p;

			p = A.MassAdd(2);

			ZETestCheckEqual(A.GetCount(), 4);
			ZETestCheckEqual(A.GetSize(), 6); //8
		}
	}
	ZETest("inline Type* MassInsert(ZESize Index, ZESize ItemCount)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEInt *p;

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
			ZEChunkArray<ZEInt, 3> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ZETestCheckEqual(ChunkArray.GetSize(), 3);

			ZEInt *p;

			p = ChunkArray.MassInsert(1, 3);
			ZETestCheckEqual(ChunkArray.GetCount(), 6);
			ZETestCheckEqual(ChunkArray.GetSize(), 9);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 3> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			ZETestCheckEqual(SmartArray.GetSize(), 3);

			ZEInt *p;

			p = SmartArray.MassInsert(1, 3);
			ZETestCheckEqual(SmartArray.GetCount(), 6);
			ZETestCheckEqual(SmartArray.GetSize(), 12); //18
		}
	}
	ZETest("inline Type* MassInsert(ZESize Index, Type* NewItems, ZESize ItemCount)")
	{
		ZEArray<ZEInt> A;

		ZEInt *p;

		for (ZEInt I = 10; I < 20; I++)
		{
			A.Add(I);
		}

		ZEArray<ZEInt> B;

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
			ZEChunkArray<ZEInt, 5> A;

			ZEInt *p;

			for (ZEInt I = 10; I < 20; I++)
			{
				A.Add(I);
			}
			ZETestCheckEqual(A.GetCount(), 10);
			ZETestCheckEqual(A.GetSize(), 10);

			ZEChunkArray<ZEInt, 5> B;

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
			ZESmartArray<ZEInt, 5> A;

			ZEInt *p;

			for (ZEInt I = 10; I < 20; I++)
			{
				A.Add(I);
			}
			ZETestCheckEqual(A.GetCount(), 10);
			ZETestCheckEqual(A.GetSize(), 30);

			ZESmartArray<ZEInt, 5> B;

			B.Add(30);
			B.Add(40);
			B.Add(50);
			B.Add(60);
			ZETestCheckEqual(B.GetCount(), 4);
			ZETestCheckEqual(B.GetSize(), 5);

			p = A.MassInsert(5, B.GetCArray(), 2);

			ZETestCheckEqual(A.GetCount(), 12);
			ZETestCheckEqual(A.GetSize(), 30);
		}
	}
	ZETest("bool operator!=(const ZEArray<Type>& Other)")
	{
		ZETestCase("for false")
		{
			ZEArray<ZEInt> Array;

			Array.Add(1);
			Array.Add(2);

			ZEArray<ZEInt> Other;

			Other.Add(1);
			Other.Add(2);

			bool Result = (Array != Other);
			ZETestCheck(Result == false);
		}
		ZETestCase("for true")
		{
			ZEArray<ZEInt> Array;

			Array.Add(1);
			Array.Add(2);

			ZEArray<ZEInt> Other;

			Other.Add(1);
			Other.Add(3);

			bool Result = (Array != Other);
			ZETestCheck(Result == true);
		}
		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);

			ZEChunkArray<ZEInt, 2> ChunkOther;

			ChunkOther.Add(2);
			ChunkOther.Add(3);

			bool Result = (ChunkArray != ChunkOther);
			ZETestCheck(Result == true);
		}
		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);

			ZESmartArray<ZEInt, 2> SmartOther;

			SmartOther.Add(1);
			SmartOther.Add(2);

			bool Result = (SmartArray != SmartOther);
			ZETestCheck(Result == false);
		}
	}
	ZETest("inline Type& operator[](ZESize Index)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);

		ZEInt Element = Array[0];
		ZETestCheckEqual(Element, 1);

		Array[0] = 3;
		ZETestCheckEqual(Array[0], 3);
		ZETestCheckEqual(Array[1], 2);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			for (ZEInt I = 1; I < 6; I++)
			{
				ChunkArray.Add(I);
			}
			ZETestCheckEqual(ChunkArray.GetCount(), 5);
			ZETestCheckEqual(ChunkArray.GetSize(), 6);

			ZEInt Element = ChunkArray[2];
			ZETestCheckEqual(Element, 3);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			for (ZEInt I =1; I < 6; I++)
			{
				SmartArray.Add(I);
			}
			ZETestCheckEqual(SmartArray.GetCount(), 5);
			ZETestCheckEqual(SmartArray.GetSize(), 6);

			ZEInt Element = SmartArray[3];
			ZETestCheckEqual(Element, 4);
		}
	}
	ZETest("inline const Type& operator[](ZESize Index) const")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);

		const ZEInt Element = Array[0];
		ZETestCheckEqual(Element, 1);

		Array[0] = 5;
		ZETestCheckEqual(Array[0], 5);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 4> ChunkArray;

			for (ZEInt I = 1; I < 20; I++)
			{
				ChunkArray.Add(I);
			}
			ZETestCheckEqual(ChunkArray.GetCount(), 19);
			ZETestCheckEqual(ChunkArray.GetSize(), 20);

			const ZEInt Element = ChunkArray[5];
			ZETestCheckEqual(Element, 6);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 4> SmartArray;

			for (ZEInt I = 1; I < 20; I++)
			{
				SmartArray.Add(I);
			}
			ZETestCheckEqual(SmartArray.GetCount(), 19);
			ZETestCheckEqual(SmartArray.GetSize(), 20);

			const ZEInt Element = SmartArray[5];
			ZETestCheckEqual(Element, 6);
		}
	}
	ZETest("ZEArray operator+(const ZEArray<Type, Allocator_>& OtherArray)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(10);
		Array.Add(20);

		ZEArray<ZEInt> OtherArray;

		OtherArray.Add(30);
		OtherArray.Add(40);
		OtherArray.Add(50);
		OtherArray.Add(60);

		ZEArray<ZEInt> Result;

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
			ZEChunkArray<ZEInt, 4> ChunkArray;

			ChunkArray.Add(10);
			ChunkArray.Add(20);

			ZETestCheckEqual(ChunkArray.GetCount(), 2);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);

			ZEChunkArray<ZEInt, 4> OtherChunkArray;

			OtherChunkArray.Add(30);
			OtherChunkArray.Add(40);
			OtherChunkArray.Add(50);
			OtherChunkArray.Add(60);

			ZETestCheckEqual(OtherChunkArray.GetCount(), 4);
			ZETestCheckEqual(OtherChunkArray.GetSize(), 4);

			ZEChunkArray<ZEInt, 4> Result;

			/*Result = */ChunkArray + OtherChunkArray;
			ZETestCheckEqual(Result.GetCount(), 6);
			ZETestCheckEqual(Result.GetSize(), 8);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 4> SmartArray;

			SmartArray.Add(10);
			SmartArray.Add(20);

			ZETestCheckEqual(SmartArray.GetCount(), 2);
			ZETestCheckEqual(SmartArray.GetSize(), 4);

			ZESmartArray<ZEInt, 4> OtherSmartArray;

			OtherSmartArray.Add(30);
			OtherSmartArray.Add(40);
			OtherSmartArray.Add(50);
			OtherSmartArray.Add(60);

			ZETestCheckEqual(SmartArray.GetCount(), 4);
			ZETestCheckEqual(SmartArray.GetSize(), 4);

			ZESmartArray<ZEInt, 4> Result;

			/*Result = */SmartArray + OtherSmartArray;
			ZETestCheckEqual(Result.GetCount(), 6);
			ZETestCheckEqual(Result.GetSize(), 20);
		}

	}
	ZETest("ZEArray& operator+=(const ZEArray<Type, Allocator_>& OtherArray)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);

		ZEArray<ZEInt> OtherArray;

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
			ZEChunkArray<ZEInt, 3> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);

			ZETestCheckEqual(ChunkArray.GetCount(), 2);
			ZETestCheckEqual(ChunkArray.GetSize(), 3);

			ZEChunkArray<ZEInt, 3> OtherChunkArray;

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
			ZESmartArray<ZEInt, 3> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);

			ZETestCheckEqual(SmartArray.GetCount(), 2);
			ZETestCheckEqual(SmartArray.GetSize(), 3);

			ZESmartArray<ZEInt, 3> OtherSmartArray;

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
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEArray<ZEInt> Other;

		Other.Add(4);
		Other.Add(5);

		Array = Other;
		ZETestCheckEqual(Array.GetCount(), 2);
		ZETestCheckEqual(Array[0], 4);
		ZETestCheckEqual(Array[1], 5);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);

			ZETestCheckEqual(ChunkArray.GetCount(), 3);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);

			ZEChunkArray<ZEInt, 2> OtherChunkArray;

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
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);

			ZETestCheckEqual(SmartArray.GetCount(), 3);
			ZETestCheckEqual(SmartArray.GetSize(), 6);

			ZESmartArray<ZEInt, 2> OtherSmartArray;

			OtherSmartArray.Add(4);
			OtherSmartArray.Add(5);

			ZETestCheckEqual(OtherSmartArray.GetCount(), 2);
			ZETestCheckEqual(OtherSmartArray.GetSize(), 2);

			SmartArray = OtherSmartArray;
			ZETestCheckEqual(SmartArray.GetCount(), 2);
			ZETestCheckEqual(SmartArray.GetSize(), 6); //4
		}
	}
	ZETest("bool operator==(const ZEArray<Type>& Other)")
	{
		ZETestCase("for true")
		{
			ZEArray<ZEInt> Array;

			Array.Add(1);
			Array.Add(2);

			ZEArray<ZEInt> Other;

			Other.Add(1);
			Other.Add(2);

			bool Result = (Array == Other);
			ZETestCheck(Result == true);
		}
		ZETestCase("for false")
		{
			ZEArray<ZEInt> Array;

			Array.Add(1);
			Array.Add(2);

			ZEArray<ZEInt> Other;

			Other.Add(1);
			Other.Add(3);

			bool Result = (Array == Other);
			ZETestCheck(Result == false);
		}
		/*ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);

			ZEChunkArray<ZEInt, 2> ChunkOther;

			ChunkOther.Add(2);
			ChunkOther.Add(3);

			bool Result = (ChunkArray == ChunkOther);
			ZETestCheck(Result == false);
		}
		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);

			ZESmartArray<ZEInt, 2> SmartOther;

			SmartOther.Add(1);
			SmartOther.Add(2);

			bool Result = (SmartArray == SmartOther);
			ZETestCheck(Result == true);
		}*/
	}
	ZETest("inline Type Pop()")
	{
		ZEArray<ZEInt> Array;

		Array.Insert(0, 0);
		Array.Insert(1, 3);

		ZEInt Result = Array.Pop();
		ZETestCheckEqual(Result, 3);
		ZETestCheckEqual(Array.GetCount(), 1);
		ZETestCheckEqual(Array[0], 0);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 4> ChunkArray;

			for (ZEInt I = 1; I < 22; I++)
			{
				ChunkArray.Add(I);
			}
			ZETestCheckEqual(ChunkArray.GetCount(), 21);
			ZETestCheckEqual(ChunkArray.GetSize(), 24);

			ZEInt Result = ChunkArray.Pop();
			ZETestCheckEqual(Result, 21);
			ZETestCheckEqual(ChunkArray.GetCount(), 20);
			ZETestCheckEqual(ChunkArray.GetSize(), 24);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 4> SmartArray;

			for (ZEInt I = 1; I < 22; I++)
			{
				SmartArray.Add(I);
			}
			ZETestCheckEqual(SmartArray.GetCount(), 21);
			ZETestCheckEqual(SmartArray.GetSize(), 84);

			ZEInt Result = SmartArray.Pop();
			ZETestCheckEqual(Result, 21);
			ZETestCheckEqual(SmartArray.GetCount(), 20);
			ZETestCheckEqual(SmartArray.GetSize(), 80);
		}
	}
	ZETest("inline void Push(Type Value)")
	{
		ZEArray<ZEInt> Array;

		Array.Push(3);
		Array.Push(5);
		ZETestCheckEqual(Array.GetCount(), 2);
		ZETestCheckEqual(Array[0], 3);
		ZETestCheckEqual(Array[1], 5);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 6> ChunkArray;

			for (ZEInt I = 1; I < 13; I++)
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
			ZESmartArray<ZEInt, 6> SmartArray;

			for (ZEInt I = 1; I < 13; I++)
			{
				SmartArray.Add(I);
			}
			ZETestCheckEqual(SmartArray.GetCount(), 12);
			ZETestCheckEqual(SmartArray.GetSize(), 42);

			SmartArray.Push(13);
			ZETestCheckEqual(SmartArray[12], 13);
			ZETestCheckEqual(SmartArray.GetCount(), 13);
			ZETestCheckEqual(SmartArray.GetSize(), 42);
		}
	}
	ZETest("inline void Resize(ZESize Count)")
	{
		ZEArray<ZEInt> Array;

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
			ZEChunkArray<ZEInt, 4> ChunkArray;

			for (ZEInt I = 1; I < 6; I++)
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
			ZESmartArray<ZEInt, 4> SmartArray;

			for (ZEInt I = 1; I < 6; I++)
			{
				SmartArray.Add(I);
			}
			ZETestCheckEqual(SmartArray.GetCount(), 5);
			ZETestCheckEqual(SmartArray.GetSize(), 20);

			SmartArray.Resize(3);
			ZETestCheckEqual(SmartArray.GetCount(), 3);
			ZETestCheckEqual(SmartArray.GetSize(), 4); //12

			SmartArray.Resize(21);
			ZETestCheckEqual(SmartArray.GetCount(), 21);
			ZETestCheckEqual(SmartArray.GetSize(), 84);

			SmartArray.Resize(20);
			ZETestCheckEqual(SmartArray.GetCount(), 20);
			ZETestCheckEqual(SmartArray.GetSize(), 80);
		}
	}
	ZETest("inline void SetCount(ZESize Count)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZETestCheckEqual(Array.GetCount(), 3);

		Array.SetCount(2);

		ZETestCheckEqual(Array.GetCount(), 2);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 3> ChunkArray;

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
			ZESmartArray<ZEInt, 3> SmartArray;

			SmartArray.SetCount(2);
			ZETestCheckEqual(SmartArray.GetCount(), 2);
			ZETestCheckEqual(SmartArray.GetSize(), 3); //6

			SmartArray.SetCount(4);
			ZETestCheckEqual(SmartArray.GetCount(), 4);
			ZETestCheckEqual(SmartArray.GetSize(), 12); //6

			SmartArray.SetCount(13);
			ZETestCheckEqual(SmartArray.GetCount(), 13);
			ZETestCheckEqual(SmartArray.GetSize(), 39);

		}
	}
	ZETest("inline void SetItem(ZESize Index, Type Value)")
	{
		ZEArray<ZEInt> Array;

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
			ZEChunkArray<ZEInt, 2> ChunkArray;

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
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);

			SmartArray.SetItem(2, 7);
			ZETestCheckEqual(SmartArray[2], 7);
			ZETestCheckEqual(SmartArray.GetCount(), 3);
			ZETestCheckEqual(SmartArray.GetSize(), 6);
		}
	}
	ZETest("inline void Sort(ZEInt (*CompareFunction)(const Type*, const Type*))")
	{
		ZEArray<ZEInt> Array;

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
			ZEChunkArray<ZEInt, 4> ChunkArray;

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
			ZESmartArray<ZEInt, 4> SmartArray;

			SmartArray.Add(3);
			SmartArray.Add(0);
			SmartArray.Add(9);
			SmartArray.Add(6);
			SmartArray.Add(12);

			SmartArray.Sort(&Compare);

			ZETestCheckEqual(SmartArray.GetCount(), 5);
			ZETestCheckEqual(SmartArray.GetSize(), 20);
			ZETestCheckEqual(SmartArray[0], 0);
			ZETestCheckEqual(SmartArray[1], 3);
			ZETestCheckEqual(SmartArray[2], 6);
			ZETestCheckEqual(SmartArray[3], 9);
			ZETestCheckEqual(SmartArray[4], 12);
		}
	}
	ZETest("void Traverse()")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);
		Array.Add(4);

		Array.Traverse();

		ZETestCheckEqual(Array[0], 4);
		ZETestCheckEqual(Array[1], 3);
		ZETestCheckEqual(Array[2], 2);
		ZETestCheckEqual(Array[3], 1);

		ZETestCase("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ChunkArray.Add(4);

			ChunkArray.Traverse();

			ZETestCheckEqual(ChunkArray.GetCount(), 4);
			ZETestCheckEqual(ChunkArray.GetSize(), 4);
			ZETestCheckEqual(ChunkArray[0], 4);
			ZETestCheckEqual(ChunkArray[1], 3);
			ZETestCheckEqual(ChunkArray[2], 2);
			ZETestCheckEqual(ChunkArray[3], 1);
		}

		ZETestCase("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			SmartArray.Add(4);

			SmartArray.Traverse();

			ZETestCheckEqual(SmartArray.GetCount(), 4);
			ZETestCheckEqual(SmartArray.GetSize(), 6);
			ZETestCheckEqual(SmartArray[0], 4);
			ZETestCheckEqual(SmartArray[1], 3);
			ZETestCheckEqual(SmartArray[2], 2);
			ZETestCheckEqual(SmartArray[3], 1);
		}
	}
	ZETest("ZEArray()")
	{

	}
	ZETest("ZEArray(const ZEArray<Type, Allocator_>& C)")
	{

	}
}
