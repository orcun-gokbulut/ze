//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - StringTests.cpp
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

 
#include "ZEDS/String.h"
#include "UnitTest/UnitTest++.h"

SUITE(ZEString)
{
	TEST_FIXTURE(ZEString, Comparasion)
	{
		ZEString A, B;

		A.Buffer = L"Orcun";
		B.Buffer = L"Orcun";
		CHECK(A.Equals(B));
		CHECK(A == B);
		
		B.Buffer = L"nucrO";
		CHECK(!A.Equals(B));
		CHECK(A != B);

		CHECK(A == L"Orcun");
		CHECK(B != L"Orcun");
		A.Buffer = NULL;
		B.Buffer = NULL;

		CHECK(A == B);
		CHECK(A == L"");
	}

	TEST(Assignments)
	{
		ZEString String;
		String.SetValue(L"Testing Testing");
		CHECK_EQUAL(String, L"Testing Testing");
		CHECK_EQUAL(String, "Testing Testing");

		String.SetValue("ASCII TEST ascii test");
		CHECK_EQUAL(String, L"ASCII TEST ascii test");

		String.SetValue("UTF8 T�rk�e karakterler Test");
		CHECK_EQUAL(String, L"UTF8 T�rk�e karakterler Test");

		String = L"Testing Testing";
		CHECK_EQUAL(String, L"Testing Testing");

		String = "String Test 123";
		CHECK_EQUAL(String, L"String Test 123");

		ZEString A("Constructor");
		CHECK_EQUAL(A, L"Constructor");

		ZEString B(L"Wide Constructor");
		CHECK_EQUAL(B, L"Wide Constructor");

		ZEString C(L"Copy Constructor");
		ZEString D(C);

		CHECK_EQUAL(D, L"Copy Constructor");

		String = A;
		CHECK_EQUAL(String, A.GetValue());

		String = L"Operator Assignment";
		CHECK_EQUAL(String, L"Operator Assignment");
		
		String = L"Char Operator Assignment";
		CHECK_EQUAL(String, L"Char Operator Assignment");

		ZEString X;
		ZEString Y;
		X = Y;
		CHECK_EQUAL(X, L"");
		
		ZEString W(X);
		CHECK_EQUAL(W, L"");
	}

	TEST(ClearIsEmpty)
	{
		ZEString A;
		CHECK(A.IsEmpty());

		A = L"Denerenrmenre";
		A.Clear();
		CHECK_EQUAL(A.GetLength(), 0);
		CHECK_EQUAL(A.GetSize(), 0);
		CHECK(A.IsEmpty());		
	}

	TEST(Buffer)
	{
		wchar_t TestString[2000];
		for(size_t I = 0; I < 2000; I++)
			TestString[I] = L'x';
		TestString[1999] = L'\0';

		ZEString A;
		CHECK(A.Buffer == NULL);
		
		TestString[500] = L'\0';
		A.SetValue(TestString);
		CHECK_EQUAL(A.GetLength(), 500);
		CHECK(A.GetSize() >=  500);

		TestString[500] = L'x';
		TestString[1000] = L'\0';
		A = TestString;
		CHECK_EQUAL(A.GetLength(), 1000);
		CHECK(A.GetSize() >= 1000);
		
		TestString[1000] = L'x';
		TestString[1500] = L'\0';
		A = TestString;
		CHECK_EQUAL(A.GetLength(), 1500);
		CHECK(A.GetSize() >= 1500);

		TestString[250] = L'\0';
		A = TestString;
		CHECK_EQUAL(A.GetLength(), 250);
		CHECK(A.GetSize() >= 250);

		A.Clear();
		CHECK_EQUAL(A.GetSize(), 0);
		CHECK_EQUAL(A.GetLength(), 0);
		CHECK(A.Buffer == NULL);

		A = L"String SetSize Testing";
		CHECK_EQUAL(A, "String SetSize Testing");
		A.Compact();
		CHECK_EQUAL(A, "String SetSize Testing");
	}

	TEST(Insert)
	{
		ZEString A = L"Insertable Text";
		A.Insert(L"Pre");
		CHECK_EQUAL(A, L"PreInsertable Text");

		ZEString B = L"Insertable Text";
		B.Insert(ZEString(L"Pre"));
		CHECK_EQUAL(B, L"PreInsertable Text");

		ZEString C = L"Empty String";
		C.Insert(L"");
		CHECK_EQUAL(C, L"Empty String");
		
		ZEString D;
		D.Insert(L"New String");
		CHECK_EQUAL(D, L"New String");
	}

	TEST(InsertStart)
	{
		ZEString A = L"Insertable Text";
		A.Insert(0, L"Pre");
		CHECK_EQUAL(A, L"PreInsertable Text");

		ZEString B = L"Empty String";
		B.Insert(0, L"");
		CHECK_EQUAL(B, L"Empty String");
		
		ZEString C;
		C.Insert(0, L"New String");
		CHECK_EQUAL(C, L"New String");

		ZEString D;
		ZEString E;
		D.Insert(E);
	}

	TEST(InsertMiddle)
	{
		ZEString A = L"$%%$";
		A.Insert(2, L"Testing");
		CHECK_EQUAL(A, L"$%Testing%$");

		ZEString B = L"$%%$";
		B.Insert(2, L"");
		CHECK_EQUAL(B, L"$%%$");
	}

	TEST(InsertEnd)
	{
		ZEString A = L"EndPoint";
		A.Insert(8, L"Location");
		CHECK_EQUAL(A, L"EndPointLocation");

		ZEString B = L"EndPoint";
		B.Insert(8, L"Location");
		CHECK_EQUAL(B, L"EndPointLocation");
	}

	TEST(Append)
	{
		//void Append(const ZEString& String);
		ZEString A = L"StringA";
		A.Append(L"StringB");
		CHECK_EQUAL(A, L"StringAStringB");

		A = "Orcun";
		A.Append("Rulz");
		CHECK_EQUAL(A, "OrcunRulz");
		A.Append(ZEString(" "));
		CHECK_EQUAL(A, L"OrcunRulz ");
		
		A = L"StringA";
		CHECK_EQUAL(A + L"StringB", L"StringAStringB");
		CHECK_EQUAL(A + "StringB", L"StringAStringB");
		CHECK_EQUAL(A + ZEString(L"StringB"), L"StringAStringB");

		A = L"StringA";
		A += ZEString(L"StringB");
		CHECK_EQUAL(A, L"StringAStringB");

		A = "";
		A.Append("");
		CHECK_EQUAL(A, "");

		A = L"StringA";
		A += L"";
		CHECK_EQUAL(A, L"StringA");
		
		A = L"XYZ";
		A += ZEString(L"");
		CHECK_EQUAL(A, L"XYZ");
	}

	TEST(Remove)
	{
		ZEString A = L"";
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

	TEST(Contains)
	{

	}

	TEST(Replace)
	{

	}

	TEST(Delete)
	{

	}

	TEST(Left)
	{
		ZEString A;
		CHECK_EQUAL(A.Left(0), L"");

		A = L"1234Test5678";
		CHECK_EQUAL(A.Left(0), L"");
		CHECK_EQUAL(A.Left(1), L"1");
		CHECK_EQUAL(A.Left(4), L"1234");
		CHECK_EQUAL(A.Left(12), L"1234Test5678");
	}
	
	TEST(Right)
	{
		ZEString A;
		CHECK_EQUAL(A.Right(0), L"");

		A = L"1234Test5678";
		CHECK_EQUAL(A.Right(0), L"");
		CHECK_EQUAL(A.Right(1), L"8");
		CHECK_EQUAL(A.Right(4), L"5678");
		CHECK_EQUAL(A.Right(12), L"1234Test5678");
	}

	TEST(Middle)
	{
		ZEString A;
		CHECK_EQUAL(A.Middle(0,0), L"");
		
		A = L"1234Test5678";
		CHECK_EQUAL(A.Middle(0, 0), L"");
		CHECK_EQUAL(A.Middle(5, 0), L"");
		CHECK_EQUAL(A.Middle(11, 0), L"");

		// Front
		CHECK_EQUAL(A.Middle(0, 0), L"");
		CHECK_EQUAL(A.Middle(0, 1), L"1");
		CHECK_EQUAL(A.Middle(0, 4), L"1234");
		CHECK_EQUAL(A.Middle(0, 12), L"1234Test5678");

		// Back
		CHECK_EQUAL(A.Middle(12, 0), L"");
		CHECK_EQUAL(A.Middle(11, 1), L"8");
		CHECK_EQUAL(A.Middle(8, 4), L"5678");

		// Middle
		CHECK_EQUAL(A.Middle(4, 0), L"");
		CHECK_EQUAL(A.Middle(4, 1), L"T");
		CHECK_EQUAL(A.Middle(4, 4), L"Test");
	}

	TEST(SubString)
	{
		ZEString A;
		CHECK_EQUAL(A.SubString(0, 0), L"");

		A = L"1234Test5678";
		CHECK_EQUAL(A.SubString(0, 0), L"1");
		CHECK_EQUAL(A.SubString(0, 1), L"12");
		CHECK_EQUAL(A.SubString(0, 3), L"1234");

		CHECK_EQUAL(A.SubString(4, 4), L"T");
		CHECK_EQUAL(A.SubString(4, 5), L"Te");
		CHECK_EQUAL(A.SubString(4, 7), L"Test");

		CHECK_EQUAL(A.SubString(8, 8), L"5");
		CHECK_EQUAL(A.SubString(8, 9), L"56");
		CHECK_EQUAL(A.SubString(8, 11), L"5678");
	}

	TEST(UpperLower)
	{
		ZEString A = L"LOWERCASE";
		CHECK_EQUAL(A.ToLower(), L"lowercase");
		
		A = L">>*loWeR <<%12CaSe\t";
		CHECK_EQUAL(A.ToLower(), L">>*lower <<%12case\t");

		A = L"uppercase";
		CHECK_EQUAL(A.ToUpper(), L"UPPERCASE");

		A = L">>*UpPeR <<%12CaSe\t";
		CHECK_EQUAL(A.ToUpper(), L">>*UPPER <<%12CASE\t");
	}

	TEST(Trim)
	{
		ZEString A;
		CHECK_EQUAL(A.TrimLeft(), L"");
		CHECK_EQUAL(A.TrimRight(), L"");
		CHECK_EQUAL(A.Trim(), L"");

		A = "    ";
		CHECK_EQUAL(A.TrimLeft(), L"");
		CHECK_EQUAL(A.TrimRight(), L"");
		CHECK_EQUAL(A.Trim(), L"");

		A = L"Trim Trim";
		CHECK_EQUAL(A.TrimLeft(), L"Trim Trim");
		CHECK_EQUAL(A.TrimRight(), L"Trim Trim");
		CHECK_EQUAL(A.Trim(), L"Trim Trim");

		A = L" Trim Trim ";
		CHECK_EQUAL(A.TrimLeft(), L"Trim Trim ");
		CHECK_EQUAL(A.TrimRight(), L" Trim Trim");
		CHECK_EQUAL(A.Trim(), L"Trim Trim");

		A = L"    Trim Trim    ";
		CHECK_EQUAL(A.TrimLeft(), L"Trim Trim    ");
		CHECK_EQUAL(A.TrimRight(), L"    Trim Trim");
		CHECK_EQUAL(A.Trim(), L"Trim Trim");

		A = L"\t\tTrim Trim\t\t";
		CHECK_EQUAL(A.TrimLeft(), L"Trim Trim\t\t");
		CHECK_EQUAL(A.TrimRight(), L"\t\tTrim Trim");
		CHECK_EQUAL(A.Trim(), L"Trim Trim");

		A = L" \t \tTrim Trim \t \t";
		CHECK_EQUAL(A.TrimLeft(), L"Trim Trim \t \t");
		CHECK_EQUAL(A.TrimRight(), L" \t \tTrim Trim");
		CHECK_EQUAL(A.Trim(), L"Trim Trim");
	}

	TEST(CopyOnWrite)
	{
	}
}
