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
#include "ZETest/ZETest.h"
#include "ZEDS/ZEArrayIterators.h"

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


ZE_TEST(ZEArray)
{
	ZE_TEST_ITEM("~ZEArray()")
	{
		ZEArray<ZEInt> Array;
		Array.Add(1);
		Array.Add(2);
		Array.Add(3);
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 3);

		Array.~ZEArray();
	}

	ZE_TEST_ITEM("inline Type* Add()")
	{
		ZEArray<ZEInt> Array;

		ZEInt *p;

		p = Array.Add();
		*p = 5;

		p = Array.Add();
		*p = 2;
		ZE_TEST_CHECK_EQUAL(Array[0], 5);
		ZE_TEST_CHECK_EQUAL(Array[1], 2);

		ZE_TEST_CASE("for ZEChunkArray")
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

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 5);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 6);
		}

		ZE_TEST_CASE("for ZESmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			ZEInt *p;
			p = SmartArray.Add();
			*p = 5;
			p = SmartArray.Add();
			*p = 4;
			p = SmartArray.Add();
			*p = 3;

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 3);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);
		}
	}

	ZE_TEST_ITEM("inline Type* Add(Type NewItem)")
	{
		ZEArray<ZEInt> Array;

		ZEInt *p;

		p = Array.Add(1);
		p = Array.Add(2);
		ZE_TEST_CHECK_EQUAL(Array[0], 1);
		ZE_TEST_CHECK_EQUAL(Array[1], 2);
		ZE_TEST_CHECK_EQUAL(*p, 2);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 4> ChunkArray;

			ZEInt *p;

			for (ZEInt I = 10; I < 16; I++)
			{
				p = ChunkArray.Add(I);
			}

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 6);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 8);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			ZEInt *p;

			for (ZEInt I = 10; I < 16; I++)
			{
				p = SmartArray.Add(I);
			}

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 6);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);
		}
	}

	ZE_TEST_ITEM("inline Type* AddByRef(const Type& NewItem)")
	{
		ZEArray<ZEInt> Array;

		ZEInt NewItem = 3;
		ZEInt *p;

		p = Array.AddByRef(NewItem);
		p = Array.AddByRef(2);
		ZE_TEST_CHECK_EQUAL(Array[0], 3);
		ZE_TEST_CHECK_EQUAL(Array[1], 2);
		ZE_TEST_CHECK_EQUAL(*p, 2);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 5> ChunkArray;

			ZEInt *p;

			for (ZEInt NewItem = 10; NewItem < 16; NewItem++)
			{
				p = ChunkArray.AddByRef(NewItem);
			}
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 6);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 10);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 5> SmartArray;

			ZEInt *p;

			for (ZEInt NewItem = 10; NewItem < 16; NewItem++)
			{
				p = SmartArray.AddByRef(NewItem);
			}
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 6);		
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 30);
		}
	}

	ZE_TEST_ITEM("inline ZESSize BinarySearch(const Type& Element, ZEInt (*CompareFunction)(Type*, Type*))")
	{
		ZEArray<ZEInt> Array;

		Array.Add(0);
		Array.Add(5);
		Array.Add(10);
		Array.Add(15);
		Array.Add(20);
		Array.Add(25);

		const ZEInt Element = 20;

		ZESSize Result = Array.BinarySearch(Element, &Compare);
		ZE_TEST_CHECK_EQUAL(Result, 4);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 6> ChunkArray;

			ChunkArray.Add(0);
			ChunkArray.Add(5);
			ChunkArray.Add(10);
			ChunkArray.Add(15);
			ChunkArray.Add(20);
			ChunkArray.Add(25);

			const ZEInt Item = 15;

			Result = ChunkArray.BinarySearch(Item, &Compare);
			ZE_TEST_CHECK_EQUAL(Result, 3);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 6> SmartArray;

			SmartArray.Add(0);
			SmartArray.Add(5);
			SmartArray.Add(10);
			SmartArray.Add(15);
			SmartArray.Add(20);
			SmartArray.Add(25);

			const ZEInt Item = 25;

			Result = SmartArray.BinarySearch(Item, &Compare);
			ZE_TEST_CHECK_EQUAL(Result, 5);
		}
	}

	ZE_TEST_ITEM("inline ZESize Circular(ZESSize Index) const")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZESSize Index;
		ZESize Result;

		ZE_TEST_CASE("for Index < 0")
		{
			Index = -5;

			Result = Array.Circular(Index);
			ZE_TEST_CHECK_EQUAL(Result, 1);
		}

		ZE_TEST_CASE("for Index > 0")
		{
			Index = 5;

			Result = Array.Circular(Index);
			ZE_TEST_CHECK_EQUAL(Result, 2);
		}

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 3> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ChunkArray.Add(4);

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 6);

			Index = -4;

			Result = ChunkArray.Circular(Index);
			ZE_TEST_CHECK_EQUAL(Result, 0);

			Index = 4;

			Result = ChunkArray.Circular(Index);
			ZE_TEST_CHECK_EQUAL(Result, 0);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 6);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 3> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			SmartArray.Add(4);

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 12);

			Index = -4;

			Result = SmartArray.Circular(Index);
			ZE_TEST_CHECK_EQUAL(Result, 0);

			Index = 4;

			Result = SmartArray.Circular(Index);
			ZE_TEST_CHECK_EQUAL(Result, 0);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 12);
		}
	}

	ZE_TEST_ITEM("inline void Clear()")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);

		Array.Clear();
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 0);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);

			ChunkArray.Clear();
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 0);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 0);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 3);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);

			SmartArray.Clear();
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 0);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 0);
		}
	}

	ZE_TEST_ITEM("void Combine(const ZEArray<Type, Allocator_>& OtherArray)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);

		ZEArray<ZEInt> OtherArray;

		OtherArray.Add(3);
		OtherArray.Add(4);

		Array.Combine(OtherArray);
		ZE_TEST_CHECK_EQUAL(Array[0], 1);
		ZE_TEST_CHECK_EQUAL(Array[1], 2);
		ZE_TEST_CHECK_EQUAL(Array[2], 3);
		ZE_TEST_CHECK_EQUAL(Array[3], 4);

		ZE_TEST_CASE("for ChunkArray")
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

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 6);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 8);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray.GetSize(), 4);
		}

		ZE_TEST_CASE("for SmartArray")
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

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 6);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);
			ZE_TEST_CHECK_EQUAL(OtherSmartArray.GetSize(), 6);
		}
	}

	ZE_TEST_ITEM("void CopyFrom(const Type* OtherArray, ZESize Count)")
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
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 2);
		ZE_TEST_CHECK_EQUAL(Array[0], 4);
		ZE_TEST_CHECK_EQUAL(Array[1], 5);

		ZE_TEST_CASE("for ChunkArray")
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

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(ChunkArray[0], 4);
			ZE_TEST_CHECK_EQUAL(ChunkArray[1], 5);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 3);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray.GetSize(), 3);
		}

		ZE_TEST_CASE("for SmartArray")
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

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(SmartArray[0], 4);
			ZE_TEST_CHECK_EQUAL(SmartArray[1], 5);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 3);
			ZE_TEST_CHECK_EQUAL(OtherSmartArray.GetSize(), 3);
		}
	}

	ZE_TEST_ITEM("void CopyFrom(const ZEArray<Type, Allocator_>& OtherArray)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEArray<ZEInt> OtherArray;

		OtherArray.Add(4);
		OtherArray.Add(5);

		Array.CopyFrom(OtherArray);
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 2);
		ZE_TEST_CHECK_EQUAL(Array[0], 4);
		ZE_TEST_CHECK_EQUAL(Array[1], 5);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);

			ZEChunkArray<ZEInt, 2> OtherChunkArray;

			OtherChunkArray.Add(4);
			OtherChunkArray.Add(5);

			ChunkArray.CopyFrom(OtherChunkArray);

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(ChunkArray[0], 4);
			ZE_TEST_CHECK_EQUAL(ChunkArray[1], 5);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray.GetSize(), 2);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);

			ZESmartArray<ZEInt, 2> OtherSmartArray;

			OtherSmartArray.Add(4);
			OtherSmartArray.Add(5);

			SmartArray.CopyFrom(OtherSmartArray);

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(SmartArray[0], 4);
			ZE_TEST_CHECK_EQUAL(SmartArray[1], 5);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 4); //6
			ZE_TEST_CHECK_EQUAL(OtherSmartArray.GetSize(), 2);
		}
	}

	ZE_TEST_ITEM("void CopyTo(Type* OtherArray, ZESize Count) const")
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
		ZE_TEST_CHECK_EQUAL(OtherArray.GetCount(), 3);
		ZE_TEST_CHECK_EQUAL(OtherArray[0], 1);
		ZE_TEST_CHECK_EQUAL(OtherArray[1], 2);
		ZE_TEST_CHECK_EQUAL(OtherArray[2], 6);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);

			ZEChunkArray<ZEInt, 2> OtherChunkArray;

			OtherChunkArray.Add(4);
			OtherChunkArray.Add(5);
			OtherChunkArray.Add(6);
			OtherChunkArray.Add(7);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray.GetSize(), 4);

			ChunkArray.CopyTo(OtherChunkArray.GetCArray(), 2);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray[0], 1);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray[1], 2);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray[2], 6);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray[3], 7);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray.GetSize(), 4);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);

			ZESmartArray<ZEInt, 2>OtherSmartArray;

			OtherSmartArray.Add(4);
			OtherSmartArray.Add(5);
			OtherSmartArray.Add(6);
			OtherSmartArray.Add(7);
			ZE_TEST_CHECK_EQUAL(OtherSmartArray.GetSize(), 6);

			SmartArray.CopyTo(OtherSmartArray.GetCArray(), 2);
			ZE_TEST_CHECK_EQUAL(OtherSmartArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(OtherSmartArray[0], 1);
			ZE_TEST_CHECK_EQUAL(OtherSmartArray[1], 2);
			ZE_TEST_CHECK_EQUAL(OtherSmartArray[2], 6);
			ZE_TEST_CHECK_EQUAL(OtherSmartArray[3], 7);
			ZE_TEST_CHECK_EQUAL(OtherSmartArray.GetSize(), 6);
		}
	}

	ZE_TEST_ITEM("void CopyTo(ZEArray<Type, Allocator_>& OtherArray) const")
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
		ZE_TEST_CHECK_EQUAL(OtherArray.GetCount(), 3);
		ZE_TEST_CHECK_EQUAL(OtherArray[0], 1);
		ZE_TEST_CHECK_EQUAL(OtherArray[1], 2);
		ZE_TEST_CHECK_EQUAL(OtherArray[2], 3);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);

			ZEChunkArray<ZEInt, 2> OtherChunkArray;

			OtherChunkArray.Add(4);
			OtherChunkArray.Add(5);
			OtherChunkArray.Add(6);
			OtherChunkArray.Add(7);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray.GetSize(), 4);

			ChunkArray.CopyTo(OtherChunkArray);

			ZE_TEST_CHECK_EQUAL(OtherChunkArray.GetCount(), 3);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray[0], 1);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray[1], 2);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray[2], 3);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray.GetSize(), 4);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);

			ZESmartArray<ZEInt, 2> OtherSmartArray;

			OtherSmartArray.Add(4);
			OtherSmartArray.Add(5);
			OtherSmartArray.Add(6);
			OtherSmartArray.Add(7);
			ZE_TEST_CHECK_EQUAL(OtherSmartArray.GetSize(), 6);

			SmartArray.CopyTo(OtherSmartArray);

			ZE_TEST_CHECK_EQUAL(OtherSmartArray.GetCount(), 3);
			ZE_TEST_CHECK_EQUAL(OtherSmartArray[0], 1);
			ZE_TEST_CHECK_EQUAL(OtherSmartArray[1], 2);
			ZE_TEST_CHECK_EQUAL(OtherSmartArray[2], 3);
			ZE_TEST_CHECK_EQUAL(OtherSmartArray.GetSize(), 6);
		}
	}

	ZE_TEST_ITEM("inline void DeleteAt(ZESize Index)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		Array.DeleteAt(1);
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 2);
		ZE_TEST_CHECK_EQUAL(Array[0], 1);
		ZE_TEST_CHECK_EQUAL(Array[1], 3);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);

			ChunkArray.DeleteAt(1);

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);

			SmartArray.DeleteAt(1);

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 4); //6
		}
	}

	ZE_TEST_ITEM("inline void DeleteValue(Type Value)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);
		Array.Add(2);

		Array.DeleteValue(2);
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 2);
		ZE_TEST_CHECK_EQUAL(Array[0], 1);
		ZE_TEST_CHECK_EQUAL(Array[1], 3);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ChunkArray.Add(2);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);

			ChunkArray.DeleteValue(2);

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			SmartArray.Add(2);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);

			SmartArray.DeleteValue(2);

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 4); //6
		}
	}

	ZE_TEST_ITEM("inline Type Dequeue()")
	{
		ZEArray<ZEInt> Array;

		Array.Add(3);
		Array.Add(2);

		ZEInt Element = Array.Dequeue();
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 1);
		ZE_TEST_CHECK_EQUAL(Array[0], 2);
		ZE_TEST_CHECK_EQUAL(Element, 3);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(3);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 6);

			ZEInt Element = ChunkArray.Dequeue();

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 6);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(3);
			SmartArray.Add(2);
			SmartArray.Add(3);
			SmartArray.Add(2);
			SmartArray.Add(3);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);

			ZEInt Element = SmartArray.Dequeue();

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);
		}
	}

	ZE_TEST_ITEM("inline void Enqueue(Type Value)")
	{
		ZEArray<ZEInt> Array;
		ZEInt Value = 3;

		Array.Add(1);
		Array.Add(2);

		Array.Enqueue(Value);
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 3);
		ZE_TEST_CHECK_EQUAL(Array[0], 1);
		ZE_TEST_CHECK_EQUAL(Array[1], 2);
		ZE_TEST_CHECK_EQUAL(Array[2], 3);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;
			ZEInt Value = 3;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 2);

			ChunkArray.Enqueue(Value);

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 3);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;
			ZEInt Value = 3;

			SmartArray.Add(1);
			SmartArray.Add(2);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 2);

			SmartArray.Enqueue(Value);

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 3);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);
		}
	}

	ZE_TEST_ITEM("inline bool Exists(Type& Value)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);
		Array.Add(4);

		ZEInt Value1 = 2;
		ZEInt Value2 = 5;

		bool Exist = Array.Exists(Value1);
		ZE_TEST_CHECK_ERROR(Exist);
		Exist = Array.Exists(Value2);		
		ZE_TEST_CHECK_ERROR(!Exist);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ChunkArray.Add(4);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);

			ZEInt Value1 = 2;
			ZEInt Value2 = 5;

			Exist = ChunkArray.Exists(Value1);
			ZE_TEST_CHECK_ERROR(Exist);
			Exist = ChunkArray.Exists(Value2);
			ZE_TEST_CHECK_ERROR(!Exist);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			SmartArray.Add(4);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);

			ZEInt Value1 = 2;
			ZEInt Value2 = 5;

			Exist = SmartArray.Exists(Value1);
			ZE_TEST_CHECK_ERROR(Exist);
			Exist = SmartArray.Exists(Value2);
			ZE_TEST_CHECK_ERROR(!Exist);
		}
	}

	ZE_TEST_ITEM("inline void Fill(Type Value)")
	{
		ZEArray<ZEInt> Array;

		Array.SetCount(5);
		ZEInt Value = 7;

		Array.Fill(Value);
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 5);
		ZE_TEST_CHECK_EQUAL(Array[0], 7);
		ZE_TEST_CHECK_EQUAL(Array[1], 7);
		ZE_TEST_CHECK_EQUAL(Array[2], 7);
		ZE_TEST_CHECK_EQUAL(Array[3], 7);
		ZE_TEST_CHECK_EQUAL(Array[4], 7);

		ZE_TEST_CASE("for arrays store different types")
		{
			ZEArray<ZEString> StringArray;
			StringArray.SetCount(3);
			ZEString Item = "TestString";

			StringArray.Fill(Item);
			ZE_TEST_CHECK_EQUAL(StringArray.GetCount(), 3);
			ZE_TEST_CHECK_STRING_EQUAL(StringArray[0], "TestString");
			ZE_TEST_CHECK_STRING_EQUAL(StringArray[1], "TestString");
			ZE_TEST_CHECK_STRING_EQUAL(StringArray[2], "TestString");

			ZEArray<ZEInt8> CharString;
			CharString.SetCount(2);
			ZEInt8 Char = 'z';

			CharString.Fill(Char);
			ZE_TEST_CHECK_EQUAL(CharString.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(CharString[0], 'z');
			ZE_TEST_CHECK_EQUAL(CharString[1], 'z');
		}

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 4> ChunkArray;

			ChunkArray.SetCount(5);
			ZEInt Value = 7;

			ChunkArray.Fill(Value);

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 5);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 8);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 4> SmartArray;

			SmartArray.SetCount(5);
			ZEInt Value = 7;

			SmartArray.Fill(Value);

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 5);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 20);
		}
	}

	ZE_TEST_ITEM("inline ZESSize FindIndex(ZEType Item, ZESize StartIndex = 0) const")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		ZESSize Result = Array.FindIndex(12);
		ZE_TEST_CHECK_EQUAL(Result, 2);

		Result = Array.FindIndex(12, 1);
		ZE_TEST_CHECK_EQUAL(Result, 2);

		Result = Array.FindIndex(12, 2);
		ZE_TEST_CHECK_EQUAL(Result, 2);

		ZE_TEST_CASE("FindIndex for searched value is not in the array")
		{
			Result = Array.FindIndex(25);
			ZE_TEST_CHECK_EQUAL(Result, -1);
		}

		ZE_TEST_CASE("FindIndex for searched value is in the array but before the specified starting index")
		{
			Result = Array.FindIndex(12, 4);
			ZE_TEST_CHECK_EQUAL(Result, -1);
		}

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 6> ChunkArray;

			for (ZEInt I = 10; I < 20; I++)
			{
				ChunkArray.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 10);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 12);

			Result = ChunkArray.FindIndex(12);
			ZE_TEST_CHECK_EQUAL(Result, 2);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 6> SmartArray;

			for (ZEInt I = 10; I < 20; I++)
			{
				SmartArray.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 10);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 42);

			Result = SmartArray.FindIndex(12);
			ZE_TEST_CHECK_EQUAL(Result, 2);
		}
	}

	ZE_TEST_ITEM("inline Type* GetCArray()")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEInt *p;

		p = Array.GetCArray();
		ZE_TEST_CHECK_EQUAL(*p, 1);
		ZE_TEST_CHECK_EQUAL(p[0], 1);
		ZE_TEST_CHECK_EQUAL(p[1], 2);
		ZE_TEST_CHECK_EQUAL(p[2], 3);

		p[0] = 5;
		ZE_TEST_CHECK_EQUAL(*p, 5);
		ZE_TEST_CHECK_EQUAL(Array[0], 5);
		ZE_TEST_CHECK_EQUAL(Array[1], 2);
		ZE_TEST_CHECK_EQUAL(Array[2], 3);

		ZE_TEST_CASE("Array stores ZEStrings")
		{
			ZEArray<ZEString> StringArray;
			StringArray.Add("New");
			StringArray.Add("Test");
			StringArray.Add("String");

			ZEString* Items;

			Items = StringArray.GetCArray();
			ZE_TEST_CHECK_STRING_EQUAL(*Items, "New");
			ZE_TEST_CHECK_STRING_EQUAL(Items[0], "New");
			ZE_TEST_CHECK_STRING_EQUAL(Items[1], "Test");
			ZE_TEST_CHECK_STRING_EQUAL(Items[2], "String");
		}

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);

			ZEInt *p;

			p = ChunkArray.GetCArray();
			ZE_TEST_CHECK_EQUAL(*p, 1);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);

			ZEInt *p;

			p = SmartArray.GetCArray();
			ZE_TEST_CHECK_EQUAL(*p, 1);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);
		}
	}

	ZE_TEST_ITEM("inline const Type* GetConstCArray() const")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		const ZEInt *p;

		p = Array.GetConstCArray();
		ZE_TEST_CHECK_EQUAL(*p, 10);
		ZE_TEST_CHECK_EQUAL(p[0], 10);
		ZE_TEST_CHECK_EQUAL(p[1], 11);
		ZE_TEST_CHECK_EQUAL(p[9], 19);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 7> ChunkArray;

			for (ZEInt I = 10; I < 20; I++)
			{
				ChunkArray.Add(I);
			}

			const ZEInt *p;

			p = ChunkArray.GetConstCArray();
			ZE_TEST_CHECK_EQUAL(*p, 10);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 10);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 14);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 7> SmartArray;

			for (ZEInt I = 10; I < 20; I++)
			{
				SmartArray.Add(I);
			}

			const ZEInt *p;

			p = SmartArray.GetConstCArray();
			ZE_TEST_CHECK_EQUAL(*p, 10);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 10);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 56);
		}
	}

	ZE_TEST_ITEM("ConstIterator GetConstIterator() const")
	{
		ZEArray<ZEInt> Array;
		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEArrayIteratorConst<ZEInt, ZEAllocatorBase<ZEInt>> ConstIterator = Array.GetConstIterator();
		ZE_TEST_CHECK_EQUAL(ConstIterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*ConstIterator.GetItem(), 1);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt8, 4> ChunkArray;
			ChunkArray.Add('z');
			ChunkArray.Add('e');

			ZEArrayIteratorConst<ZEInt8, ZEChunkAllocator<ZEInt8, 4>> IteratorConst = ChunkArray.GetConstIterator();
			ZE_TEST_CHECK_EQUAL(IteratorConst.GetIndex(), 0);
			ZE_TEST_CHECK_EQUAL(*IteratorConst.GetItem(), 'z');
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEString, 2> SmartArray;
			SmartArray.Add("Test");
			SmartArray.Add("String");

			ZEArrayIteratorConst<ZEString, ZESmartAllocator<ZEString, 2>> IteratorConst = SmartArray.GetConstIterator();
			ZE_TEST_CHECK_EQUAL(IteratorConst.GetIndex(), 0);
			ZE_TEST_CHECK_STRING_EQUAL(*IteratorConst.GetItem(), "Test");
		}
	}

	ZE_TEST_ITEM("inline ZESize GetCount() const")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		ZESize Count = Array.GetCount();
		ZE_TEST_CHECK_EQUAL(Count, 10);

		ZE_TEST_CASE("for ZEChunkArray")
		{
			ZEChunkArray<ZEInt, 5> ChunkArray;

			for (ZEInt I = 1; I < 7; I++)
			{
				ChunkArray.Add(I);
			}

			Count = ChunkArray.GetCount();
			ZESize Size = ChunkArray.GetSize();
			ZE_TEST_CHECK_EQUAL(Count, 6);
			ZE_TEST_CHECK_EQUAL(Size, 10);
		}

		ZE_TEST_CASE("for ZESmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			for (ZEInt I = 1; I < 8; I++)
			{
				SmartArray.Add(I);
			}

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 7);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 14);
		}
	}

	ZE_TEST_ITEM("inline Type& GetFirstItem()")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		ZEInt Result = Array.GetFirstItem();
		ZE_TEST_CHECK_EQUAL(Result, 10);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 8> ChunkArray;

			for (ZEInt I =10; I < 20; I++)
			{
				ChunkArray.Add(I);
			}

			ZEInt Result = ChunkArray.GetFirstItem();
			ZE_TEST_CHECK_EQUAL(Result, 10);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 10);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 16);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 8> SmartArray;

			for (ZEInt I = 10; I < 20; I++)
			{
				SmartArray.Add(I);
			}

			ZEInt Result = SmartArray.GetFirstItem();
			ZE_TEST_CHECK_EQUAL(Result, 10);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 10);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 72);
		}
	}

	ZE_TEST_ITEM("inline const Type& GetFirstItem()const")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		const ZEInt Result = Array.GetFirstItem();
		ZE_TEST_CHECK_EQUAL(Result, 10);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 8> ChunkArray;

			for (ZEInt I =10; I < 20; I++)
			{
				ChunkArray.Add(I);
			}

			const ZEInt Result = ChunkArray.GetFirstItem();
			ZE_TEST_CHECK_EQUAL(Result, 10);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 10);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 16);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 8> SmartArray;

			for (ZEInt I = 10; I < 20; I++)
			{
				SmartArray.Add(I);
			}

			const ZEInt Result = SmartArray.GetFirstItem();
			ZE_TEST_CHECK_EQUAL(Result, 10);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 10);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 72);
		}
	}

	ZE_TEST_ITEM("inline Type& GetItem(ZESize Index)")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		ZEInt Result = Array.GetItem(2);
		ZE_TEST_CHECK_EQUAL(Result, 12);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 9> ChunkArray;

			for (ZEInt I = 10; I < 30; I++)
			{
				ChunkArray.Add(I);
			}

			ZEInt Result = ChunkArray.GetItem(13);
			ZE_TEST_CHECK_EQUAL(Result, 23);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 20);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 27);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 9> SmartArray;

			for (ZEInt I =10; I < 30; I++)
			{
				SmartArray.Add(I);
			}

			ZEInt Result = SmartArray.GetItem(13);
			ZE_TEST_CHECK_EQUAL(Result, 23);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 20);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 90);
		}
	}

	ZE_TEST_ITEM("inline const Type& GetItem(ZESize Index) const")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		const ZEInt Result = Array.GetItem(6);
		ZE_TEST_CHECK_EQUAL(Result, 16);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 10> ChunkArray;

			for (ZEInt I = 10; I < 30; I++)
			{
				ChunkArray.Add(I);
			}

			const ZEInt Result = ChunkArray.GetItem(13);
			ZE_TEST_CHECK_EQUAL(Result, 23);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 20);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 20);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 10> SmartArray;

			for (ZEInt I = 10; I < 30; I++)
			{
				SmartArray.Add(I);
			}

			const ZEInt Result = SmartArray.GetItem(13);
			ZE_TEST_CHECK_EQUAL(Result, 23);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 20);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 110);
		}
	}

	ZE_TEST_ITEM("Iterator GetIterator()")
	{
		ZEArray<ZEInt> Array;
		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEArrayIterator<ZEInt, ZEAllocatorBase<ZEInt>> Iterator = Array.GetIterator();
		ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);
		ZE_TEST_CHECK_EQUAL(*Iterator.GetItem(), 1);

		ZE_TEST_CASE("for an empty array")
		{
			ZEArray<ZEInt> A;

			Iterator = A.GetIterator();
			ZE_TEST_CHECK_EQUAL(Iterator.GetIndex(), 0);
			ZE_TEST_CHECK_EQUAL(Iterator.GetItem(), NULL);
			//error Index is out of range.
		}

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEString, 2> ChunkArray;
			ChunkArray.Add("Chunk");
			ChunkArray.Add("Array");

			ZEArrayIterator<ZEString, ZEChunkAllocator<ZEString, 2>> IteratorChunk = ChunkArray.GetIterator();
			ZE_TEST_CHECK_EQUAL(IteratorChunk.GetIndex(), 0);
			ZE_TEST_CHECK_STRING_EQUAL(*IteratorChunk.GetItem(), "Chunk");
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt8, 2> SmartArray;
			SmartArray.Add('Z');
			SmartArray.Add('E');

			ZEArrayIterator<ZEInt8, ZESmartAllocator<ZEInt8, 2>> IteratorSmart = SmartArray.GetIterator();
			ZE_TEST_CHECK_EQUAL(IteratorSmart.GetIndex(), 0);
			ZE_TEST_CHECK_EQUAL(*IteratorSmart.GetItem(), 'Z');
		}
	}

	ZE_TEST_ITEM("inline Type& GetLastItem()")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		ZEInt Result = Array.GetLastItem();
		ZE_TEST_CHECK_EQUAL(Result, 19);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 15> ChunkArray;

			for (ZEInt I = 10; I < 30; I++)
			{
				ChunkArray.Add(I);
			}
			ZEInt Result = ChunkArray.GetLastItem();
			ZE_TEST_CHECK_EQUAL(Result, 29);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 20);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 30);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 15> SmartArray;

			for (ZEInt I = 10; I < 30; I++)
			{
				SmartArray.Add(I);
			}
			ZEInt Result = SmartArray.GetLastItem();
			ZE_TEST_CHECK_EQUAL(Result, 29);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 20);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 240);
		}
	}

	ZE_TEST_ITEM("inline const Type& GetLastItem()const")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 20; I++)
		{
			Array.Add(I);
		}

		const ZEInt Result = Array.GetLastItem();
		ZE_TEST_CHECK_EQUAL(Result, 19);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 15> ChunkArray;

			for (ZEInt I = 10; I < 30; I++)
			{
				ChunkArray.Add(I);
			}
			const ZEInt Result = ChunkArray.GetLastItem();
			ZE_TEST_CHECK_EQUAL(Result, 29);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 20);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 30);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 15> SmartArray;

			for (ZEInt I = 10; I < 30; I++)
			{
				SmartArray.Add(I);
			}
			const ZEInt Result = SmartArray.GetLastItem();
			ZE_TEST_CHECK_EQUAL(Result, 29);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 20);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 240);
		}
	}

	ZE_TEST_ITEM("inline ZESize GetSize() const")
	{
		ZEArray<ZEInt> Array;

		for (ZEInt I = 10; I < 15; I++)
		{
			Array.Add(I);
		}

		ZEInt Size = Array.GetSize();
		ZE_TEST_CHECK_EQUAL(Size, 5);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 3> ChunkArray;

			for (ZEInt I = 10; I < 15; I++)
			{
				ChunkArray.Add(I);
			}

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 6);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 3> SmartArray;

			for (ZEInt I = 10; I < 15; I++)
			{
				SmartArray.Add(I);
			}

			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 12);
		}
	}

	ZE_TEST_ITEM("inline Type* Insert(ZESize Index)")
	{
		ZEArray<ZEInt> Array;

		ZEInt *p;

		Array.Insert(0, 0);
		Array.Insert(1, 1);
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 2);

		p = Array.Insert(1);
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 3);
		ZE_TEST_CHECK_EQUAL(Array[1], -842150451);

		*p=5;

		ZE_TEST_CHECK_EQUAL(Array[0], 0);
		ZE_TEST_CHECK_EQUAL(Array[1], 5);
		ZE_TEST_CHECK_EQUAL(Array[2], 1);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 5> ChunkArray;

			ZEInt *p;

			for (ZEInt I = 1; I < 6; I++)
			{
				ChunkArray.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(ChunkArray[0], 1);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 5);

			p = ChunkArray.Insert(0);
			ZE_TEST_CHECK_EQUAL(ChunkArray[0], -842150451);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 6);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 10);

			*p = 8;
			ZE_TEST_CHECK_EQUAL(ChunkArray[0], 8);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 5> SmartArray;

			ZEInt *p;

			for (ZEInt I = 1; I < 6; I++)
			{
				SmartArray.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(SmartArray[0], 1);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 5);

			p = SmartArray.Insert(0);
			ZE_TEST_CHECK_EQUAL(SmartArray[0], -842150451);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 6);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 30);

			*p = 8;
			ZE_TEST_CHECK_EQUAL(SmartArray[0], 8);
		}
	}

	ZE_TEST_ITEM("inline Type* Insert(ZESize Index, Type NewItem)")
	{
		ZEArray<ZEInt> Array;

		ZEInt *p;

		p = Array.Insert(0, 3);
		ZE_TEST_CHECK_EQUAL(*p, 3);
		p = Array.Insert(1, 4);
		ZE_TEST_CHECK_EQUAL(*p, 4);
		p = Array.Insert(2, 5);
		ZE_TEST_CHECK_EQUAL(*p, 5);

		ZE_TEST_CHECK_EQUAL(Array[0], 3);
		ZE_TEST_CHECK_EQUAL(Array[1], 4);
		ZE_TEST_CHECK_EQUAL(Array[2], 5);

		p = Array.Insert(1, 0);
		ZE_TEST_CHECK_EQUAL(*p, 0);

		ZE_TEST_CHECK_EQUAL(Array[0], 3);
		ZE_TEST_CHECK_EQUAL(Array[1], 0);
		ZE_TEST_CHECK_EQUAL(Array[2], 4);
		ZE_TEST_CHECK_EQUAL(Array[3], 5);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 3> ChunkArray;

			ZEInt *p;

			p = ChunkArray.Insert(0, 3);
			p = ChunkArray.Insert(1, 4);
			p = ChunkArray.Insert(2, 5);
			p = ChunkArray.Insert(1, 0);

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 6);
			ZE_TEST_CHECK_EQUAL(ChunkArray[0], 3);
			ZE_TEST_CHECK_EQUAL(ChunkArray[1], 0);
			ZE_TEST_CHECK_EQUAL(ChunkArray[2], 4);
			ZE_TEST_CHECK_EQUAL(ChunkArray[3], 5);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 3> SmartArray;

			ZEInt *p;

			p = SmartArray.Insert(0, 5);
			p = SmartArray.Insert(1, 2);
			p = SmartArray.Insert(2, 5);
			p = SmartArray.Insert(1, 0);

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 12);
			ZE_TEST_CHECK_EQUAL(SmartArray[0], 5);
			ZE_TEST_CHECK_EQUAL(SmartArray[1], 0);
			ZE_TEST_CHECK_EQUAL(SmartArray[2], 2);
			ZE_TEST_CHECK_EQUAL(SmartArray[3], 5);
		}
	}

	ZE_TEST_ITEM("inline Type* MassAdd(const Type* NewItems, ZESize ItemCount)")
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
		ZE_TEST_CHECK_EQUAL(*p, 30);

		ZE_TEST_CHECK_EQUAL(A.GetCount(), 12);
		ZE_TEST_CHECK_EQUAL(A[9], 19);
		ZE_TEST_CHECK_EQUAL(A[10], 30);
		ZE_TEST_CHECK_EQUAL(A[11], 40);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 6> A;

			ZEInt *p;

			for (ZEInt I = 10; I < 20; I++)
			{
				A.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(A.GetCount(), 10);
			ZE_TEST_CHECK_EQUAL(A.GetSize(), 12);

			ZEChunkArray<ZEInt, 3> B;

			B.Add(30);
			B.Add(40);
			B.Add(50);
			B.Add(60);
			ZE_TEST_CHECK_EQUAL(B.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(B.GetSize(), 6);

			p = A.MassAdd(B.GetCArray(), 2);

			ZE_TEST_CHECK_EQUAL(A.GetCount(), 12);
			ZE_TEST_CHECK_EQUAL(A.GetSize(), 12);
			ZE_TEST_CHECK_EQUAL(A[9], 19);
			ZE_TEST_CHECK_EQUAL(A[10], 30);
			ZE_TEST_CHECK_EQUAL(A[11], 40);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 6> A;

			ZEInt *p;

			for (ZEInt I = 10; I < 20; I++)
			{
				A.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(A.GetCount(), 10);
			ZE_TEST_CHECK_EQUAL(A.GetSize(), 42);

			ZESmartArray<ZEInt, 3> B;

			B.Add(30);
			B.Add(40);
			B.Add(50);
			B.Add(60);
			ZE_TEST_CHECK_EQUAL(B.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(B.GetSize(), 12);

			p = A.MassAdd(B.GetCArray(), 2);

			ZE_TEST_CHECK_EQUAL(A.GetCount(), 12);
			ZE_TEST_CHECK_EQUAL(A.GetSize(), 42);
			ZE_TEST_CHECK_EQUAL(A[9], 19);
			ZE_TEST_CHECK_EQUAL(A[10], 30);
			ZE_TEST_CHECK_EQUAL(A[11], 40);
		}
	}

	ZE_TEST_ITEM("inline Type* MassAdd(ZESize ItemCount)")
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

		ZE_TEST_CHECK_EQUAL(A.GetCount(), 4);
		ZE_TEST_CHECK_EQUAL(A.GetSize(), 4);
		ZE_TEST_CHECK_EQUAL(A[2], -842150451);
		ZE_TEST_CHECK_EQUAL(A[3], -842150451);

		p[0] = B[0];
		p[1] = B[1];
		ZE_TEST_CHECK_EQUAL(A[0], 1);
		ZE_TEST_CHECK_EQUAL(A[1], 2);
		ZE_TEST_CHECK_EQUAL(A[2], 30);
		ZE_TEST_CHECK_EQUAL(A[3], 40);
		ZE_TEST_CHECK_EQUAL(*p, 30);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> A;

			A.Add(1);
			A.Add(2);
			ZE_TEST_CHECK_EQUAL(A.GetSize(), 2);

			ZEChunkArray<ZEInt, 2> B;

			B.Add(30);
			B.Add(40);
			B.Add(50);
			B.Add(60);
			ZE_TEST_CHECK_EQUAL(B.GetSize(), 4);

			ZEInt *p;

			p = A.MassAdd(2);

			ZE_TEST_CHECK_EQUAL(A.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(A.GetSize(), 6);
			ZE_TEST_CHECK_EQUAL(A[2], -842150451);
			ZE_TEST_CHECK_EQUAL(A[3], -842150451);

			p[0] = B[0];
			p[1] = B[1];
			ZE_TEST_CHECK_EQUAL(A[0], 1);
			ZE_TEST_CHECK_EQUAL(A[1], 2);
			ZE_TEST_CHECK_EQUAL(A[2], 30);
			ZE_TEST_CHECK_EQUAL(A[3], 40);
			ZE_TEST_CHECK_EQUAL(*p, 30);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> A;

			A.Add(1);
			A.Add(2);
			ZE_TEST_CHECK_EQUAL(A.GetSize(), 2);

			ZEInt *p;

			p = A.MassAdd(2);

			ZE_TEST_CHECK_EQUAL(A.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(A.GetSize(), 8); //6
			ZE_TEST_CHECK_EQUAL(A[2], -842150451);
			ZE_TEST_CHECK_EQUAL(A[3], -842150451);
		}
	}

	ZE_TEST_ITEM("inline Type* MassInsert(ZESize Index, ZESize ItemCount)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEInt *p;

		p = Array.MassInsert(1, 3);
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 6);
		ZE_TEST_CHECK_EQUAL(Array.GetSize(), 6);
		ZE_TEST_CHECK_EQUAL(Array[0], 1);
		ZE_TEST_CHECK_EQUAL(Array[1], -842150451);
		ZE_TEST_CHECK_EQUAL(Array[2], -842150451);
		ZE_TEST_CHECK_EQUAL(Array[3], -842150451);
		ZE_TEST_CHECK_EQUAL(Array[4], 2);
		ZE_TEST_CHECK_EQUAL(Array[5], 3);
		ZE_TEST_CHECK_EQUAL(*p, Array[1]);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 3> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 3);

			ZEInt *p;

			p = ChunkArray.MassInsert(1, 3);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 6);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 9);
			ZE_TEST_CHECK_EQUAL(ChunkArray[0], 1);
			ZE_TEST_CHECK_EQUAL(ChunkArray[1], -842150451);
			ZE_TEST_CHECK_EQUAL(ChunkArray[2], -842150451);
			ZE_TEST_CHECK_EQUAL(ChunkArray[3], -842150451);
			ZE_TEST_CHECK_EQUAL(ChunkArray[4], 2);
			ZE_TEST_CHECK_EQUAL(ChunkArray[5], 3);
			ZE_TEST_CHECK_EQUAL(*p, ChunkArray[1]);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 3> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 3);

			ZEInt *p;

			p = SmartArray.MassInsert(1, 3);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 6);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 18); //12
			ZE_TEST_CHECK_EQUAL(SmartArray[0], 1);
			ZE_TEST_CHECK_EQUAL(SmartArray[1], -842150451);
			ZE_TEST_CHECK_EQUAL(SmartArray[2], -842150451);
			ZE_TEST_CHECK_EQUAL(SmartArray[3], -842150451);
			ZE_TEST_CHECK_EQUAL(SmartArray[4], 2);
			ZE_TEST_CHECK_EQUAL(SmartArray[5], 3);
			ZE_TEST_CHECK_EQUAL(*p, SmartArray[1]);
		}
	}

	ZE_TEST_ITEM("inline Type* MassInsert(ZESize Index, Type* NewItems, ZESize ItemCount)")
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

		ZE_TEST_CHECK_EQUAL(A.GetCount(), 12);
		ZE_TEST_CHECK_EQUAL(A[0], 10);
		ZE_TEST_CHECK_EQUAL(A[1], 11);
		ZE_TEST_CHECK_EQUAL(A[2], 12);
		ZE_TEST_CHECK_EQUAL(A[3], 13);
		ZE_TEST_CHECK_EQUAL(A[4], 14);
		ZE_TEST_CHECK_EQUAL(A[5], 30);
		ZE_TEST_CHECK_EQUAL(A[6], 40);
		ZE_TEST_CHECK_EQUAL(A[7], 15);
		ZE_TEST_CHECK_EQUAL(A[8], 16);
		ZE_TEST_CHECK_EQUAL(A[9], 17);
		ZE_TEST_CHECK_EQUAL(A[10], 18);
		ZE_TEST_CHECK_EQUAL(A[11], 19);
		ZE_TEST_CHECK_EQUAL(*p, 30);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 5> A;

			ZEInt *p;

			for (ZEInt I = 10; I < 20; I++)
			{
				A.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(A.GetCount(), 10);
			ZE_TEST_CHECK_EQUAL(A.GetSize(), 10);

			ZEChunkArray<ZEInt, 5> B;

			B.Add(30);
			B.Add(40);
			B.Add(50);
			B.Add(60);
			ZE_TEST_CHECK_EQUAL(B.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(B.GetSize(), 5);

			p = A.MassInsert(5, B.GetCArray(), 2);

			ZE_TEST_CHECK_EQUAL(A.GetCount(), 12);
			ZE_TEST_CHECK_EQUAL(A.GetSize(), 15);
			ZE_TEST_CHECK_EQUAL(A[4], 14);
			ZE_TEST_CHECK_EQUAL(A[5], 30);
			ZE_TEST_CHECK_EQUAL(A[6], 40);
			ZE_TEST_CHECK_EQUAL(A[7], 15);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 5> A;

			ZEInt *p;

			for (ZEInt I = 10; I < 20; I++)
			{
				A.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(A.GetCount(), 10);
			ZE_TEST_CHECK_EQUAL(A.GetSize(), 30);

			ZESmartArray<ZEInt, 5> B;

			B.Add(30);
			B.Add(40);
			B.Add(50);
			B.Add(60);
			ZE_TEST_CHECK_EQUAL(B.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(B.GetSize(), 5);

			p = A.MassInsert(5, B.GetCArray(), 2);

			ZE_TEST_CHECK_EQUAL(A.GetCount(), 12);
			ZE_TEST_CHECK_EQUAL(A.GetSize(), 30);
			ZE_TEST_CHECK_EQUAL(A[4], 14);
			ZE_TEST_CHECK_EQUAL(A[5], 30);
			ZE_TEST_CHECK_EQUAL(A[6], 40);
			ZE_TEST_CHECK_EQUAL(A[7], 15);
		}
	}

	ZE_TEST_ITEM("bool operator!=(const ZEArray<Type>& Other)")
	{
		ZE_TEST_CASE("for false")
		{
			ZEArray<ZEInt> Array;

			Array.Add(1);
			Array.Add(2);

			ZEArray<ZEInt> Other;

			Other.Add(1);
			Other.Add(2);

			bool Result = (Array != Other);
			ZE_TEST_CHECK_ERROR(!Result);
		}

		ZE_TEST_CASE("for true")
		{
			ZEArray<ZEInt> Array;

			Array.Add(1);
			Array.Add(2);

			ZEArray<ZEInt> Other;

			Other.Add(1);
			Other.Add(3);

			bool Result = (Array != Other);
			ZE_TEST_CHECK_ERROR(Result);
		}

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);

			ZEChunkArray<ZEInt, 2> ChunkOther;

			ChunkOther.Add(2);
			ChunkOther.Add(3);

			bool Result = (ChunkArray != ChunkOther);
			ZE_TEST_CHECK_ERROR(Result);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);

			ZESmartArray<ZEInt, 2> SmartOther;

			SmartOther.Add(1);
			SmartOther.Add(2);

			bool Result = (SmartArray != SmartOther);
			ZE_TEST_CHECK_ERROR(!Result);
		}
	}

	ZE_TEST_ITEM("inline Type& operator[](ZESize Index)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);

		ZEInt Element = Array[0];
		ZE_TEST_CHECK_EQUAL(Element, 1);

		Array[0] = 3;
		ZE_TEST_CHECK_EQUAL(Array[0], 3);
		ZE_TEST_CHECK_EQUAL(Array[1], 2);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			for (ZEInt I = 1; I < 6; I++)
			{
				ChunkArray.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 5);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 6);

			ZEInt Element = ChunkArray[2];
			ZE_TEST_CHECK_EQUAL(Element, 3);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			for (ZEInt I =1; I < 6; I++)
			{
				SmartArray.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 5);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);

			ZEInt Element = SmartArray[3];
			ZE_TEST_CHECK_EQUAL(Element, 4);
		}
	}

	ZE_TEST_ITEM("inline const Type& operator[](ZESize Index) const")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);

		const ZEInt Element = Array[0];
		ZE_TEST_CHECK_EQUAL(Element, 1);

		Array[0] = 5;
		ZE_TEST_CHECK_EQUAL(Array[0], 5);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 4> ChunkArray;

			for (ZEInt I = 1; I < 20; I++)
			{
				ChunkArray.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 19);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 20);

			const ZEInt Element = ChunkArray[5];
			ZE_TEST_CHECK_EQUAL(Element, 6);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 4> SmartArray;

			for (ZEInt I = 1; I < 20; I++)
			{
				SmartArray.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 19);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 20);

			const ZEInt Element = SmartArray[5];
			ZE_TEST_CHECK_EQUAL(Element, 6);
		}
	}

	ZE_TEST_ITEM("ZEArray operator+(const ZEArray<Type, Allocator_>& OtherArray)")
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
		ZE_TEST_CHECK_EQUAL(Result.GetCount(), 6);
		ZE_TEST_CHECK_EQUAL(Result[0], 10);
		ZE_TEST_CHECK_EQUAL(Result[1], 20);
		ZE_TEST_CHECK_EQUAL(Result[2], 30);
		ZE_TEST_CHECK_EQUAL(Result[3], 40);
		ZE_TEST_CHECK_EQUAL(Result[4], 50);
		ZE_TEST_CHECK_EQUAL(Result[5], 60);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 4> ChunkArray;

			ChunkArray.Add(10);
			ChunkArray.Add(20);

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);

			ZEChunkArray<ZEInt, 4> OtherChunkArray;

			OtherChunkArray.Add(30);
			OtherChunkArray.Add(40);
			OtherChunkArray.Add(50);
			OtherChunkArray.Add(60);

			ZE_TEST_CHECK_EQUAL(OtherChunkArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray.GetSize(), 4);

			ZE_TEST_CHECK_EQUAL((ChunkArray + OtherChunkArray).GetCount(), 6);
			ZE_TEST_CHECK_EQUAL((ChunkArray + OtherChunkArray).GetSize(), 8);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 4> SmartArray;

			SmartArray.Add(10);
			SmartArray.Add(20);

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 4);

			ZESmartArray<ZEInt, 4> OtherSmartArray;

			OtherSmartArray.Add(30);
			OtherSmartArray.Add(40);
			OtherSmartArray.Add(50);
			OtherSmartArray.Add(60);

			ZE_TEST_CHECK_EQUAL(OtherSmartArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(OtherSmartArray.GetSize(), 4);

			ZE_TEST_CHECK_EQUAL((SmartArray + OtherSmartArray).GetCount(), 6);
			ZE_TEST_CHECK_EQUAL((SmartArray + OtherSmartArray).GetSize(), 24); //20
		}
	}

	ZE_TEST_ITEM("ZEArray& operator+=(const ZEArray<Type, Allocator_>& OtherArray)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);

		ZEArray<ZEInt> OtherArray;

		OtherArray.Add(8);
		OtherArray.Add(9);

		Array += OtherArray;
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 4);
		ZE_TEST_CHECK_EQUAL(Array[0], 1);
		ZE_TEST_CHECK_EQUAL(Array[1], 2);
		ZE_TEST_CHECK_EQUAL(Array[2], 8);
		ZE_TEST_CHECK_EQUAL(Array[3], 9);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 3> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 3);

			ZEChunkArray<ZEInt, 3> OtherChunkArray;

			OtherChunkArray.Add(8);
			OtherChunkArray.Add(9);

			ZE_TEST_CHECK_EQUAL(OtherChunkArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray.GetSize(), 3);

			ChunkArray += OtherChunkArray;
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 6);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 3> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 3);

			ZESmartArray<ZEInt, 3> OtherSmartArray;

			OtherSmartArray.Add(8);
			OtherSmartArray.Add(9);

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 3);

			SmartArray += OtherSmartArray;
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 12);
		}
	}

	ZE_TEST_ITEM("inline void operator=(const ZEArray<Type, Allocator_>& Other)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		ZEArray<ZEInt> Other;

		Other.Add(4);
		Other.Add(5);

		Array = Other;
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 2);
		ZE_TEST_CHECK_EQUAL(Array[0], 4);
		ZE_TEST_CHECK_EQUAL(Array[1], 5);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 3);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);

			ZEChunkArray<ZEInt, 2> OtherChunkArray;

			OtherChunkArray.Add(4);
			OtherChunkArray.Add(5);

			ZE_TEST_CHECK_EQUAL(OtherChunkArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(OtherChunkArray.GetSize(), 2);

			ChunkArray = OtherChunkArray;
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 3);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);

			ZESmartArray<ZEInt, 2> OtherSmartArray;

			OtherSmartArray.Add(4);
			OtherSmartArray.Add(5);

			ZE_TEST_CHECK_EQUAL(OtherSmartArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(OtherSmartArray.GetSize(), 2);

			SmartArray = OtherSmartArray;
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 4); //6
		}
	}

	ZE_TEST_ITEM("bool operator==(const ZEArray<Type>& Other)")
	{
		ZE_TEST_CASE("for true")
		{
			ZEArray<ZEInt> Array;

			Array.Add(1);
			Array.Add(2);

			ZEArray<ZEInt> Other;

			Other.Add(1);
			Other.Add(2);

			bool Result = (Array == Other);
			ZE_TEST_CHECK_ERROR(Result);
		}

		ZE_TEST_CASE("for false")
		{
			ZEArray<ZEInt> Array;

			Array.Add(1);
			Array.Add(2);

			ZEArray<ZEInt> Other;

			Other.Add(1);
			Other.Add(3);

			bool Result = (Array == Other);
			ZE_TEST_CHECK_ERROR(!Result);
		}

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);

			ZEChunkArray<ZEInt, 2> ChunkOther;

			ChunkOther.Add(2);
			ChunkOther.Add(3);

			bool Result = (ChunkArray == ChunkOther);
			ZE_TEST_CHECK_ERROR(!Result);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);

			ZESmartArray<ZEInt, 2> SmartOther;

			SmartOther.Add(1);
			SmartOther.Add(2);

			bool Result = (SmartArray == SmartOther);
			ZE_TEST_CHECK_ERROR(Result);
		}	
	}

	ZE_TEST_ITEM("inline Type Pop()")
	{
		ZEArray<ZEInt> Array;

		Array.Insert(0, 0);
		Array.Insert(1, 3);

		ZEInt Result = Array.Pop();
		ZE_TEST_CHECK_EQUAL(Result, 3);
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 1);
		ZE_TEST_CHECK_EQUAL(Array[0], 0);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 4> ChunkArray;

			for (ZEInt I = 1; I < 22; I++)
			{
				ChunkArray.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 21);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 24);

			ZEInt Result = ChunkArray.Pop();
			ZE_TEST_CHECK_EQUAL(Result, 21);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 20);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 24);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 4> SmartArray;

			for (ZEInt I = 1; I < 22; I++)
			{
				SmartArray.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 21);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 84);

			ZEInt Result = SmartArray.Pop();
			ZE_TEST_CHECK_EQUAL(Result, 21);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 20);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 80);
		}
	}

	ZE_TEST_ITEM("inline void Push(Type Value)")
	{
		ZEArray<ZEInt> Array;

		Array.Push(3);
		Array.Push(5);
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 2);
		ZE_TEST_CHECK_EQUAL(Array[0], 3);
		ZE_TEST_CHECK_EQUAL(Array[1], 5);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 6> ChunkArray;

			for (ZEInt I = 1; I < 13; I++)
			{
				ChunkArray.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 12);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 12);

			ChunkArray.Push(13);
			ZE_TEST_CHECK_EQUAL(ChunkArray[12], 13);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 13);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 18);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 6> SmartArray;

			for (ZEInt I = 1; I < 13; I++)
			{
				SmartArray.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 12);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 42);

			SmartArray.Push(13);
			ZE_TEST_CHECK_EQUAL(SmartArray[12], 13);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 13);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 42);
		}
	}

	ZE_TEST_ITEM("inline void Resize(ZESize Count)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		Array.Resize(2);
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 2);
		ZE_TEST_CHECK_EQUAL(Array[0], 1);
		ZE_TEST_CHECK_EQUAL(Array[1], 2);

		ZE_TEST_CASE("Resize for count value bigger than array size")
		{
			Array.Resize(5);
			ZE_TEST_CHECK_EQUAL(Array.GetCount(), 5);
			ZE_TEST_CHECK_EQUAL(Array[0], 1);
			ZE_TEST_CHECK_EQUAL(Array[1], 2);
			ZE_TEST_CHECK_EQUAL(Array[2], -842150451);
			ZE_TEST_CHECK_EQUAL(Array[3], -842150451);
			ZE_TEST_CHECK_EQUAL(Array[4], -842150451);
		}

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 4> ChunkArray;

			for (ZEInt I = 1; I < 6; I++)
			{
				ChunkArray.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 5);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 8);

			ChunkArray.Resize(3);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 3);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);

			ChunkArray.Resize(9);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 9);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 12);

			ChunkArray.Resize(12);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 12);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 12);

			ChunkArray.Resize(3);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 3);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 4> SmartArray;

			for (ZEInt I = 1; I < 6; I++)
			{
				SmartArray.Add(I);
			}
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 5);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 20);

			SmartArray.Resize(3);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 3);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 12); //4

			SmartArray.Resize(21);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 21);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 84);

			SmartArray.Resize(20);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 20);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 80);
		}
	}

	ZE_TEST_ITEM("inline void SetCount(ZESize Count)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 3);

		Array.SetCount(2);
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 2);
		ZE_TEST_CHECK_EQUAL(Array[0], -842150451);
		ZE_TEST_CHECK_EQUAL(Array[1], -842150451);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 3> ChunkArray;

			ChunkArray.SetCount(2);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 3);

			ChunkArray.SetCount(4);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 6);

			ChunkArray.SetCount(7);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 7);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 9);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 3> SmartArray;

			SmartArray.SetCount(2);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6); //3

			SmartArray.SetCount(4);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6); //12

			SmartArray.SetCount(13);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 13);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 39);
		}
	}

	ZE_TEST_ITEM("inline void SetItem(ZESize Index, Type Value)")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);

		Array.SetItem(2, 7);
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 3);
		ZE_TEST_CHECK_EQUAL(Array[0], 1);
		ZE_TEST_CHECK_EQUAL(Array[1], 2);
		ZE_TEST_CHECK_EQUAL(Array[2], 7);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);

			ChunkArray.SetItem(2, 7);
			ZE_TEST_CHECK_EQUAL(ChunkArray[2], 7);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 3);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);

			SmartArray.SetItem(2, 7);
			ZE_TEST_CHECK_EQUAL(SmartArray[2], 7);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 3);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);
		}
	}

	ZE_TEST_ITEM("inline void Sort(ZEInt (*CompareFunction)(const Type*, const Type*))")
	{
		ZEArray<ZEInt> Array;

		Array.Add(0);
		Array.Add(2);
		Array.Add(3);
		Array.Add(1);

		Array.Sort(&Compare);

		ZE_TEST_CHECK_EQUAL(Array[0], 0);
		ZE_TEST_CHECK_EQUAL(Array[1], 1);
		ZE_TEST_CHECK_EQUAL(Array[2], 2);
		ZE_TEST_CHECK_EQUAL(Array[3], 3);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 4> ChunkArray;

			ChunkArray.Add(3);
			ChunkArray.Add(0);
			ChunkArray.Add(9);
			ChunkArray.Add(6);
			ChunkArray.Add(12);

			ChunkArray.Sort(&Compare);

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 5);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 8);
			ZE_TEST_CHECK_EQUAL(ChunkArray[0], 0);
			ZE_TEST_CHECK_EQUAL(ChunkArray[1], 3);
			ZE_TEST_CHECK_EQUAL(ChunkArray[2], 6);
			ZE_TEST_CHECK_EQUAL(ChunkArray[3], 9);
			ZE_TEST_CHECK_EQUAL(ChunkArray[4], 12);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 4> SmartArray;

			SmartArray.Add(3);
			SmartArray.Add(0);
			SmartArray.Add(9);
			SmartArray.Add(6);
			SmartArray.Add(12);

			SmartArray.Sort(&Compare);

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 5);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 20);
			ZE_TEST_CHECK_EQUAL(SmartArray[0], 0);
			ZE_TEST_CHECK_EQUAL(SmartArray[1], 3);
			ZE_TEST_CHECK_EQUAL(SmartArray[2], 6);
			ZE_TEST_CHECK_EQUAL(SmartArray[3], 9);
			ZE_TEST_CHECK_EQUAL(SmartArray[4], 12);
		}
	}

	ZE_TEST_ITEM("void Traverse()")
	{
		ZEArray<ZEInt> Array;

		Array.Add(1);
		Array.Add(2);
		Array.Add(3);
		Array.Add(4);

		Array.Traverse();

		ZE_TEST_CHECK_EQUAL(Array[0], 4);
		ZE_TEST_CHECK_EQUAL(Array[1], 3);
		ZE_TEST_CHECK_EQUAL(Array[2], 2);
		ZE_TEST_CHECK_EQUAL(Array[3], 1);

		ZE_TEST_CASE("for ChunkArray")
		{
			ZEChunkArray<ZEInt, 2> ChunkArray;

			ChunkArray.Add(1);
			ChunkArray.Add(2);
			ChunkArray.Add(3);
			ChunkArray.Add(4);

			ChunkArray.Traverse();

			ZE_TEST_CHECK_EQUAL(ChunkArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(ChunkArray.GetSize(), 4);
			ZE_TEST_CHECK_EQUAL(ChunkArray[0], 4);
			ZE_TEST_CHECK_EQUAL(ChunkArray[1], 3);
			ZE_TEST_CHECK_EQUAL(ChunkArray[2], 2);
			ZE_TEST_CHECK_EQUAL(ChunkArray[3], 1);
		}

		ZE_TEST_CASE("for SmartArray")
		{
			ZESmartArray<ZEInt, 2> SmartArray;

			SmartArray.Add(1);
			SmartArray.Add(2);
			SmartArray.Add(3);
			SmartArray.Add(4);

			SmartArray.Traverse();

			ZE_TEST_CHECK_EQUAL(SmartArray.GetCount(), 4);
			ZE_TEST_CHECK_EQUAL(SmartArray.GetSize(), 6);
			ZE_TEST_CHECK_EQUAL(SmartArray[0], 4);
			ZE_TEST_CHECK_EQUAL(SmartArray[1], 3);
			ZE_TEST_CHECK_EQUAL(SmartArray[2], 2);
			ZE_TEST_CHECK_EQUAL(SmartArray[3], 1);
		}
	}

	ZE_TEST_ITEM("ZEArray()")
	{
		ZEArray<ZEInt> Array;

		ZEInt *p = Array.GetCArray();
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 0);
		ZE_TEST_CHECK_EQUAL(p, NULL);
	}

	ZE_TEST_ITEM("ZEArray(const ZEArray<Type, Allocator_>& C)")
	{
		ZEArray<ZEInt> Other;

		ZEArray<ZEInt> Array(Other);
		ZE_TEST_CHECK_EQUAL(Array.GetCount(), 0);

		ZE_TEST_CASE("for array parameter's Items != NULL")
		{
			ZEArray<ZEInt> A;
			A.Add(1);
			A.Add(2);

			ZEArray<ZEInt> B(A);
			ZE_TEST_CHECK_EQUAL(B.GetCount(), 2);
			ZE_TEST_CHECK_EQUAL(B[0], 1);
			ZE_TEST_CHECK_EQUAL(B[1], 2);
		}
	}
}
