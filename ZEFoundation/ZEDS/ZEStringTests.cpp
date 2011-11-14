//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEStringTests.cpp
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

#include "ZEDS/ZEString.h"

#include "ZETest.h"

ZETestSuiteAdd(ZEString)
{
	ZETestItemAdd(ZEString)
	{
		ZEString A, B;

		A.Buffer = "Orcun";
		B.Buffer = "Orcun";
		CHECK(A.Equals(B));
		CHECK(A == B);
		
		B.Buffer = "nucrO";
		CHECK(!A.Equals(B));
		CHECK(A != B);

		CHECK(A == "Orcun");
		CHECK(B != "Orcun");
		A.Buffer = NULL;
		B.Buffer = NULL;

		CHECK(A == B);
		CHECK(A == "");
	}

	ZETestItemAdd(Assignments)
	{
		ZEString String;
		String.SetValue("Testing Testing");
		CHECK_EQUAL(String, "Testing Testing");
		CHECK_EQUAL(String, "Testing Testing");

		String.SetValue("ASCII TEST ascii test");
		CHECK_EQUAL(String, "ASCII TEST ascii test");

		String.SetValue("UTF8 T�rk�e karakterler Test");
		CHECK_EQUAL(String, "UTF8 T�rk�e karakterler Test");

		String = "Testing Testing";
		CHECK_EQUAL(String, "Testing Testing");

		String = "String Test 123";
		CHECK_EQUAL(String, "String Test 123");

		ZEString A("Constructor");
		CHECK_EQUAL(A, "Constructor");

		ZEString B("Wide Constructor");
		CHECK_EQUAL(B, "Wide Constructor");

		ZEString C("Copy Constructor");
		ZEString D(C);

		CHECK_EQUAL(D, "Copy Constructor");

		String = A;
		CHECK_EQUAL(String, A.GetValue());

		String = "Operator Assignment";
		CHECK_EQUAL(String, "Operator Assignment");
		
		String = "Char Operator Assignment";
		CHECK_EQUAL(String, "Char Operator Assignment");

		ZEString X;
		ZEString Y;
		X = Y;
		CHECK_EQUAL(X, "");
		
		ZEString W(X);
		CHECK_EQUAL(W, "");
	}

	ZETestItemAdd(ClearIsEmpty)
	{
		ZEString A;
		CHECK(A.IsEmpty());

		A = "Denerenrmenre";
		A.Clear();
		CHECK_EQUAL(A.GetLength(), 0);
		CHECK_EQUAL(A.GetSize(), 0);
		CHECK(A.IsEmpty());		
	}

	ZETestItemAdd(Buffer)
	{
		char TestString[2000];
		for(size_t I = 0; I < 2000; I++)
			TestString[I] = 'x';
		TestString[1999] = '\0';

		ZEString A;
		CHECK(A.Buffer == NULL);
		
		TestString[500] = '\0';
		A.SetValue(TestString);
		CHECK_EQUAL(A.GetLength(), 500);
		CHECK(A.GetSize() >=  500);

		TestString[500] = 'x';
		TestString[1000] = '\0';
		A = TestString;
		CHECK_EQUAL(A.GetLength(), 1000);
		CHECK(A.GetSize() >= 1000);
		
		TestString[1000] = 'x';
		TestString[1500] = '\0';
		A = TestString;
		CHECK_EQUAL(A.GetLength(), 1500);
		CHECK(A.GetSize() >= 1500);

		TestString[250] = '\0';
		A = TestString;
		CHECK_EQUAL(A.GetLength(), 250);
		CHECK(A.GetSize() >= 250);

		A.Clear();
		CHECK_EQUAL(A.GetSize(), 0);
		CHECK_EQUAL(A.GetLength(), 0);
		CHECK(A.Buffer == NULL);

		A = "String SetSize Testing";
		CHECK_EQUAL(A, "String SetSize Testing");
		A.Compact();
		CHECK_EQUAL(A, "String SetSize Testing");
	}

	ZETestItemAdd(Insert)
	{
		ZEString A = "Insertable Text";
		A.Insert("Pre");
		CHECK_EQUAL(A, "PreInsertable Text");

		ZEString B = "Insertable Text";
		B.Insert(ZEString("Pre"));
		CHECK_EQUAL(B, "PreInsertable Text");

		ZEString C = "Empty String";
		C.Insert("");
		CHECK_EQUAL(C, "Empty String");
		
		ZEString D;
		D.Insert("New String");
		CHECK_EQUAL(D, "New String");
	}

	ZETestItemAdd(InsertStart)
	{
		ZEString A = "Insertable Text";
		A.Insert(0, "Pre");
		CHECK_EQUAL(A, "PreInsertable Text");

		ZEString B = "Empty String";
		B.Insert(0, "");
		CHECK_EQUAL(B, "Empty String");
		
		ZEString C;
		C.Insert(0, "New String");
		CHECK_EQUAL(C, "New String");

		ZEString D;
		ZEString E;
		D.Insert(E);
	}

	ZETestItemAdd(InsertMiddle)
	{
		ZEString A = "$%%$";
		A.Insert(2, "Testing");
		CHECK_EQUAL(A, "$%Testing%$");

		ZEString B = "$%%$";
		B.Insert(2, "");
		CHECK_EQUAL(B, "$%%$");
	}

	ZETestItemAdd(InsertEnd)
	{
		ZEString A = "EndPoint";
		A.Insert(8, "Location");
		CHECK_EQUAL(A, "EndPointLocation");

		ZEString B = "EndPoint";
		B.Insert(8, "Location");
		CHECK_EQUAL(B, "EndPointLocation");
	}

	ZETestItemAdd(Append)
	{
		//void Append(const ZEString& String);
		ZEString A = "StringA";
		A.Append("StringB");
		CHECK_EQUAL(A, "StringAStringB");

		A = "Orcun";
		A.Append("Rulz");
		CHECK_EQUAL(A, "OrcunRulz");
		A.Append(ZEString(" "));
		CHECK_EQUAL(A, "OrcunRulz ");
		
		A = "StringA";
		CHECK_EQUAL(A + "StringB", "StringAStringB");
		CHECK_EQUAL(A + "StringB", "StringAStringB");
		CHECK_EQUAL(A + ZEString("StringB"), "StringAStringB");

		A = "StringA";
		A += ZEString("StringB");
		CHECK_EQUAL(A, "StringAStringB");

		A = "";
		A.Append("");
		CHECK_EQUAL(A, "");

		A = "StringA";
		A += "";
		CHECK_EQUAL(A, "StringA");
		
		A = "XYZ";
		A += ZEString("");
		CHECK_EQUAL(A, "XYZ");
	}

	ZETestItemAdd(Remove)
	{
		ZEString A = "";
		A.Remove(0, 0);
		
		A = "1234567890";
		A.Remove(0, 0);
		CHECK_EQUAL(A, "1234567890");

		A = "1234567890";
		A.Remove(0, 1);
		CHECK_EQUAL(A, "234567890");

		A = "1234567890";
		A.Remove(0, 5);
		CHECK_EQUAL(A, "67890");

		A = "1234567890";
		A.Remove(1, 1);
		CHECK_EQUAL(A, "134567890");

		A = "1234567890";
		A.Remove(1, 5);
		CHECK_EQUAL(A, "17890");

		A = "1234567890";
		A.Remove(9, 1);
		CHECK_EQUAL(A, "123456789");

		A = "1234567890";
		A.Remove(8, 2);
		CHECK_EQUAL(A, "12345678");

		A = "1234567890";
		A.Remove(3, 4);
		CHECK_EQUAL(A, "123890");
	}	

	ZETestItemAdd(Contains)
	{

	}

	ZETestItemAdd(Replace)
	{

	}

	ZETestItemAdd(Delete)
	{

	}

	ZETestItemAdd(Left)
	{
		ZEString A;
		CHECK_EQUAL(A.Left(0), "");

		A = "1234Test5678";
		CHECK_EQUAL(A.Left(0), "");
		CHECK_EQUAL(A.Left(1), "1");
		CHECK_EQUAL(A.Left(4), "1234");
		CHECK_EQUAL(A.Left(12), "1234Test5678");
	}
	
	ZETestItemAdd(Right)
	{
		ZEString A;
		CHECK_EQUAL(A.Right(0), "");

		A = "1234Test5678";
		CHECK_EQUAL(A.Right(0), "");
		CHECK_EQUAL(A.Right(1), "8");
		CHECK_EQUAL(A.Right(4), "5678");
		CHECK_EQUAL(A.Right(12), "1234Test5678");
	}

	ZETestItemAdd(Middle)
	{
		ZEString A;
		CHECK_EQUAL(A.Middle(0,0), "");
		
		A = "1234Test5678";
		CHECK_EQUAL(A.Middle(0, 0), "");
		CHECK_EQUAL(A.Middle(5, 0), "");
		CHECK_EQUAL(A.Middle(11, 0), "");

		// Front
		CHECK_EQUAL(A.Middle(0, 0), "");
		CHECK_EQUAL(A.Middle(0, 1), "1");
		CHECK_EQUAL(A.Middle(0, 4), "1234");
		CHECK_EQUAL(A.Middle(0, 12), "1234Test5678");

		// Back
		CHECK_EQUAL(A.Middle(12, 0), "");
		CHECK_EQUAL(A.Middle(11, 1), "8");
		CHECK_EQUAL(A.Middle(8, 4), "5678");

		// Middle
		CHECK_EQUAL(A.Middle(4, 0), "");
		CHECK_EQUAL(A.Middle(4, 1), "T");
		CHECK_EQUAL(A.Middle(4, 4), "Test");
	}

	ZETestItemAdd(SubString)
	{
		ZEString A;
		CHECK_EQUAL(A.SubString(0, 0), "");

		A = "1234Test5678";
		CHECK_EQUAL(A.SubString(0, 0), "1");
		CHECK_EQUAL(A.SubString(0, 1), "12");
		CHECK_EQUAL(A.SubString(0, 3), "1234");

		CHECK_EQUAL(A.SubString(4, 4), "T");
		CHECK_EQUAL(A.SubString(4, 5), "Te");
		CHECK_EQUAL(A.SubString(4, 7), "Test");

		CHECK_EQUAL(A.SubString(8, 8), "5");
		CHECK_EQUAL(A.SubString(8, 9), "56");
		CHECK_EQUAL(A.SubString(8, 11), "5678");
	}

	ZETestItemAdd(UpperLower)
	{
		ZEString A = "LOWERCASE";
		CHECK_EQUAL(A.Lower(), "lowercase");
		
		A = ">>*loWeR <<%12CaSe\t";
		CHECK_EQUAL(A.Lower(), ">>*lower <<%12case\t");

		A = "uppercase";
		CHECK_EQUAL(A.Upper(), "UPPERCASE");

		A = ">>*UpPeR <<%12CaSe\t";
		CHECK_EQUAL(A.Upper(), ">>*UPPER <<%12CASE\t");
	}

	ZETestItemAdd(Trim)
	{
		ZEString A;
		CHECK_EQUAL(A.TrimLeft(), "");
		CHECK_EQUAL(A.TrimRight(), "");
		CHECK_EQUAL(A.Trim(), "");

		A = "    ";
		CHECK_EQUAL(A.TrimLeft(), "");
		CHECK_EQUAL(A.TrimRight(), "");
		CHECK_EQUAL(A.Trim(), "");

		A = "Trim Trim";
		CHECK_EQUAL(A.TrimLeft(), "Trim Trim");
		CHECK_EQUAL(A.TrimRight(), "Trim Trim");
		CHECK_EQUAL(A.Trim(), "Trim Trim");

		A = " Trim Trim ";
		CHECK_EQUAL(A.TrimLeft(), "Trim Trim ");
		CHECK_EQUAL(A.TrimRight(), " Trim Trim");
		CHECK_EQUAL(A.Trim(), "Trim Trim");

		A = "    Trim Trim    ";
		CHECK_EQUAL(A.TrimLeft(), "Trim Trim    ");
		CHECK_EQUAL(A.TrimRight(), "    Trim Trim");
		CHECK_EQUAL(A.Trim(), "Trim Trim");

		A = "\t\tTrim Trim\t\t";
		CHECK_EQUAL(A.TrimLeft(), "Trim Trim\t\t");
		CHECK_EQUAL(A.TrimRight(), "\t\tTrim Trim");
		CHECK_EQUAL(A.Trim(), "Trim Trim");

		A = " \t \tTrim Trim \t \t";
		CHECK_EQUAL(A.TrimLeft(), "Trim Trim \t \t");
		CHECK_EQUAL(A.TrimRight(), " \t \tTrim Trim");
		CHECK_EQUAL(A.Trim(), "Trim Trim");
	}

	ZETestItemAdd(CopyOnWrite)
	{
	}
}
