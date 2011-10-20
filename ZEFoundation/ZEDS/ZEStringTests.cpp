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
		ZETestCheck(A.Equals(B));
		ZETestCheck(A == B);
		
		B.Buffer = "nucrO";
		ZETestCheck(!A.Equals(B));
		ZETestCheck(A != B);

		ZETestCheck(A == "Orcun");
		ZETestCheck(B != "Orcun");
		A.Buffer = NULL;
		B.Buffer = NULL;

		ZETestCheck(A == B);
		ZETestCheck(A == "");
	}

	ZETestItemAdd(Assignments)
	{
		ZEString String;
		String.SetValue("Testing Testing");
		ZETestCheckEqual(String, "Testing Testing");
		ZETestCheckEqual(String, "Testing Testing");

		String.SetValue("ASCII TEST ascii test");
		ZETestCheckEqual(String, "ASCII TEST ascii test");

		String.SetValue("UTF8 T�rk�e karakterler Test");
		ZETestCheckEqual(String, "UTF8 T�rk�e karakterler Test");

		String = "Testing Testing";
		ZETestCheckEqual(String, "Testing Testing");

		String = "String Test 123";
		ZETestCheckEqual(String, "String Test 123");

		ZEString A("Constructor");
		ZETestCheckEqual(A, "Constructor");

		ZEString B("Wide Constructor");
		ZETestCheckEqual(B, "Wide Constructor");

		ZEString C("Copy Constructor");
		ZEString D(C);

		ZETestCheckEqual(D, "Copy Constructor");

		String = A;
		ZETestCheckEqual(String, A.GetValue());

		String = "Operator Assignment";
		ZETestCheckEqual(String, "Operator Assignment");
		
		String = "Char Operator Assignment";
		ZETestCheckEqual(String, "Char Operator Assignment");

		ZEString X;
		ZEString Y;
		X = Y;
		ZETestCheckEqual(X, "");
		
		ZEString W(X);
		ZETestCheckEqual(W, "");
	}

	ZETestItemAdd(ClearIsEmpty)
	{
		ZEString A;
		ZETestCheck(A.IsEmpty());

		A = "Denerenrmenre";
		A.Clear();
		ZETestCheckEqual(A.GetLength(), 0);
		ZETestCheckEqual(A.GetSize(), 0);
		ZETestCheck(A.IsEmpty());		
	}

	ZETestItemAdd(Buffer)
	{
		char TestString[2000];
		for(size_t I = 0; I < 2000; I++)
			TestString[I] = 'x';
		TestString[1999] = '\0';

		ZEString A;
		ZETestCheck(A.Buffer == NULL);
		
		TestString[500] = '\0';
		A.SetValue(TestString);
		ZETestCheckEqual(A.GetLength(), 500);
		ZETestCheck(A.GetSize() >=  500);

		TestString[500] = 'x';
		TestString[1000] = '\0';
		A = TestString;
		ZETestCheckEqual(A.GetLength(), 1000);
		ZETestCheck(A.GetSize() >= 1000);
		
		TestString[1000] = 'x';
		TestString[1500] = '\0';
		A = TestString;
		ZETestCheckEqual(A.GetLength(), 1500);
		ZETestCheck(A.GetSize() >= 1500);

		TestString[250] = '\0';
		A = TestString;
		ZETestCheckEqual(A.GetLength(), 250);
		ZETestCheck(A.GetSize() >= 250);

		A.Clear();
		ZETestCheckEqual(A.GetSize(), 0);
		ZETestCheckEqual(A.GetLength(), 0);
		ZETestCheck(A.Buffer == NULL);

		A = "String SetSize Testing";
		ZETestCheckEqual(A, "String SetSize Testing");
		A.Compact();
		ZETestCheckEqual(A, "String SetSize Testing");
	}

	ZETestItemAdd(Insert)
	{
		ZEString A = "Insertable Text";
		A.Insert("Pre");
		ZETestCheckEqual(A, "PreInsertable Text");

		ZEString B = "Insertable Text";
		B.Insert(ZEString("Pre"));
		ZETestCheckEqual(B, "PreInsertable Text");

		ZEString C = "Empty String";
		C.Insert("");
		ZETestCheckEqual(C, "Empty String");
		
		ZEString D;
		D.Insert("New String");
		ZETestCheckEqual(D, "New String");
	}

	ZETestItemAdd(InsertStart)
	{
		ZEString A = "Insertable Text";
		A.Insert(0, "Pre");
		ZETestCheckEqual(A, "PreInsertable Text");

		ZEString B = "Empty String";
		B.Insert(0, "");
		ZETestCheckEqual(B, "Empty String");
		
		ZEString C;
		C.Insert(0, "New String");
		ZETestCheckEqual(C, "New String");

		ZEString D;
		ZEString E;
		D.Insert(E);
	}

	ZETestItemAdd(InsertMiddle)
	{
		ZEString A = "$%%$";
		A.Insert(2, "Testing");
		ZETestCheckEqual(A, "$%Testing%$");

		ZEString B = "$%%$";
		B.Insert(2, "");
		ZETestCheckEqual(B, "$%%$");
	}

	ZETestItemAdd(InsertEnd)
	{
		ZEString A = "EndPoint";
		A.Insert(8, "Location");
		ZETestCheckEqual(A, "EndPointLocation");

		ZEString B = "EndPoint";
		B.Insert(8, "Location");
		ZETestCheckEqual(B, "EndPointLocation");
	}

	ZETestItemAdd(Append)
	{
		//void Append(const ZEString& String);
		ZEString A = "StringA";
		A.Append("StringB");
		ZETestCheckEqual(A, "StringAStringB");

		A = "Orcun";
		A.Append("Rulz");
		ZETestCheckEqual(A, "OrcunRulz");
		A.Append(ZEString(" "));
		ZETestCheckEqual(A, "OrcunRulz ");
		
		A = "StringA";
		ZETestCheckEqual(A + "StringB", "StringAStringB");
		ZETestCheckEqual(A + "StringB", "StringAStringB");
		ZETestCheckEqual(A + ZEString("StringB"), "StringAStringB");

		A = "StringA";
		A += ZEString("StringB");
		ZETestCheckEqual(A, "StringAStringB");

		A = "";
		A.Append("");
		ZETestCheckEqual(A, "");

		A = "StringA";
		A += "";
		ZETestCheckEqual(A, "StringA");
		
		A = "XYZ";
		A += ZEString("");
		ZETestCheckEqual(A, "XYZ");
	}

	ZETestItemAdd(Remove)
	{
		ZEString A = "";
		A.Remove(0, 0);
		
		A = "1234567890";
		A.Remove(0, 0);
		ZETestCheckEqual(A, "1234567890");

		A = "1234567890";
		A.Remove(0, 1);
		ZETestCheckEqual(A, "234567890");

		A = "1234567890";
		A.Remove(0, 5);
		ZETestCheckEqual(A, "67890");

		A = "1234567890";
		A.Remove(1, 1);
		ZETestCheckEqual(A, "134567890");

		A = "1234567890";
		A.Remove(1, 5);
		ZETestCheckEqual(A, "17890");

		A = "1234567890";
		A.Remove(9, 1);
		ZETestCheckEqual(A, "123456789");

		A = "1234567890";
		A.Remove(8, 2);
		ZETestCheckEqual(A, "12345678");

		A = "1234567890";
		A.Remove(3, 4);
		ZETestCheckEqual(A, "123890");
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
		ZETestCheckEqual(A.Left(0), "");

		A = "1234Test5678";
		ZETestCheckEqual(A.Left(0), "");
		ZETestCheckEqual(A.Left(1), "1");
		ZETestCheckEqual(A.Left(4), "1234");
		ZETestCheckEqual(A.Left(12), "1234Test5678");
	}
	
	ZETestItemAdd(Right)
	{
		ZEString A;
		ZETestCheckEqual(A.Right(0), "");

		A = "1234Test5678";
		ZETestCheckEqual(A.Right(0), "");
		ZETestCheckEqual(A.Right(1), "8");
		ZETestCheckEqual(A.Right(4), "5678");
		ZETestCheckEqual(A.Right(12), "1234Test5678");
	}

	ZETestItemAdd(Middle)
	{
		ZEString A;
		ZETestCheckEqual(A.Middle(0,0), "");
		
		A = "1234Test5678";
		ZETestCheckEqual(A.Middle(0, 0), "");
		ZETestCheckEqual(A.Middle(5, 0), "");
		ZETestCheckEqual(A.Middle(11, 0), "");

		// Front
		ZETestCheckEqual(A.Middle(0, 0), "");
		ZETestCheckEqual(A.Middle(0, 1), "1");
		ZETestCheckEqual(A.Middle(0, 4), "1234");
		ZETestCheckEqual(A.Middle(0, 12), "1234Test5678");

		// Back
		ZETestCheckEqual(A.Middle(12, 0), "");
		ZETestCheckEqual(A.Middle(11, 1), "8");
		ZETestCheckEqual(A.Middle(8, 4), "5678");

		// Middle
		ZETestCheckEqual(A.Middle(4, 0), "");
		ZETestCheckEqual(A.Middle(4, 1), "T");
		ZETestCheckEqual(A.Middle(4, 4), "Test");
	}

	ZETestItemAdd(SubString)
	{
		ZEString A;
		ZETestCheckEqual(A.SubString(0, 0), "");

		A = "1234Test5678";
		ZETestCheckEqual(A.SubString(0, 0), "1");
		ZETestCheckEqual(A.SubString(0, 1), "12");
		ZETestCheckEqual(A.SubString(0, 3), "1234");

		ZETestCheckEqual(A.SubString(4, 4), "T");
		ZETestCheckEqual(A.SubString(4, 5), "Te");
		ZETestCheckEqual(A.SubString(4, 7), "Test");

		ZETestCheckEqual(A.SubString(8, 8), "5");
		ZETestCheckEqual(A.SubString(8, 9), "56");
		ZETestCheckEqual(A.SubString(8, 11), "5678");
	}

	ZETestItemAdd(UpperLower)
	{
		ZEString A = "LOWERCASE";
		ZETestCheckEqual(A.ToLower(), "lowercase");
		
		A = ">>*loWeR <<%12CaSe\t";
		ZETestCheckEqual(A.ToLower(), ">>*lower <<%12case\t");

		A = "uppercase";
		ZETestCheckEqual(A.ToUpper(), "UPPERCASE");

		A = ">>*UpPeR <<%12CaSe\t";
		ZETestCheckEqual(A.ToUpper(), ">>*UPPER <<%12CASE\t");
	}

	ZETestItemAdd(Trim)
	{
		ZEString A;
		ZETestCheckEqual(A.TrimLeft(), "");
		ZETestCheckEqual(A.TrimRight(), "");
		ZETestCheckEqual(A.Trim(), "");

		A = "    ";
		ZETestCheckEqual(A.TrimLeft(), "");
		ZETestCheckEqual(A.TrimRight(), "");
		ZETestCheckEqual(A.Trim(), "");

		A = "Trim Trim";
		ZETestCheckEqual(A.TrimLeft(), "Trim Trim");
		ZETestCheckEqual(A.TrimRight(), "Trim Trim");
		ZETestCheckEqual(A.Trim(), "Trim Trim");

		A = " Trim Trim ";
		ZETestCheckEqual(A.TrimLeft(), "Trim Trim ");
		ZETestCheckEqual(A.TrimRight(), " Trim Trim");
		ZETestCheckEqual(A.Trim(), "Trim Trim");

		A = "    Trim Trim    ";
		ZETestCheckEqual(A.TrimLeft(), "Trim Trim    ");
		ZETestCheckEqual(A.TrimRight(), "    Trim Trim");
		ZETestCheckEqual(A.Trim(), "Trim Trim");

		A = "\t\tTrim Trim\t\t";
		ZETestCheckEqual(A.TrimLeft(), "Trim Trim\t\t");
		ZETestCheckEqual(A.TrimRight(), "\t\tTrim Trim");
		ZETestCheckEqual(A.Trim(), "Trim Trim");

		A = " \t \tTrim Trim \t \t";
		ZETestCheckEqual(A.TrimLeft(), "Trim Trim \t \t");
		ZETestCheckEqual(A.TrimRight(), " \t \tTrim Trim");
		ZETestCheckEqual(A.Trim(), "Trim Trim");
	}

	ZETestItemAdd(CopyOnWrite)
	{
	}
}
