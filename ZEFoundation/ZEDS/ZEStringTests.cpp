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
	ZETestItemAdd(ZEString)//Equals, =
	{
		ZEString TestItem1, TestItem2;

		TestItem1 = "Orcun";
		TestItem2 = "Orcun";
		CHECK(TestItem1.Equals(TestItem2));
		CHECK(TestItem1 == TestItem2);

		TestItem2 = "nucrO";
		CHECK(!TestItem1.Equals(TestItem2));
		CHECK(TestItem1 != TestItem2);

		CHECK(TestItem1 == "Orcun");
		CHECK(TestItem2 == "nucrO");
	}

	ZETestItemAdd(Assigments)
	{

		ZEString TestItem1;
		TestItem1.SetValue("Testing Testing");
		CHECK_EQUAL(TestItem1, "Testing Testing");

		TestItem1.SetValue("ASCII TEST ascii test");
		CHECK_EQUAL(TestItem1, "ASCII TEST ascii test");

		TestItem1.SetValue("UTF8 T�rk�e karakterler Test");
		CHECK_EQUAL(TestItem1, "UTF8 T�rk�e karakterler Test");

		TestItem1 = "Testing Testing";
		CHECK_EQUAL(TestItem1, "Testing Testing");

		TestItem1 = "Testing 123";
		CHECK_EQUAL(TestItem1, "Testing 123");

		ZEString TestItem2("Constructor");
		CHECK_EQUAL(TestItem2, "Constructor");

		ZEString TestItem3("Wide Constructor");
		CHECK_EQUAL(TestItem3, "Wide Constructor");

		ZEString TestItem4("Copy Constructor");
		CHECK_EQUAL(TestItem4, "Copy Constructor");

		ZEString TestItem10 = "Operator Assignment";
		CHECK_EQUAL(TestItem10, "Operator Assignment");

		ZEString TestItem5;
		ZEString TestItem6;
		TestItem5 = TestItem6;
		CHECK_EQUAL(TestItem5, "");

		ZEString TestItem7(TestItem5);
		CHECK_EQUAL(TestItem7, "");

	}
	ZETestItemAdd(OneTestArmy)//getlength,getvalue,getsize,setvalue,getcharacter,setcharacter
	{
		ZEString TestItem1;
		ZEString TestItem2;
		char TestItem3[2000];
		ZEString TestItem4;
		ZEString TestItem6;
		for (size_t I = 0; I < 2000; I++)
		{
			TestItem3[I] = 'x';
			TestItem3[1999] = '\0';
		}
		TestItem1 = "vivident";
		CHECK_EQUAL(TestItem1.GetLength(), 8);
		CHECK_EQUAL(TestItem1.GetSize(), 9);
		TestItem1.Clear();
		CHECK_EQUAL(TestItem1.GetLength(), 0);
		CHECK_EQUAL(TestItem1.GetSize(), 0);

		ZEString TestItem5("Constructor");
		TestItem2 = TestItem5;
		CHECK_EQUAL(TestItem2, TestItem5.GetValue());

		TestItem3[500] = '\0';
		CHECK(TestItem4 == "");

		TestItem3[500] = '\0';
		TestItem4.SetValue(TestItem3);
		CHECK_EQUAL(TestItem4.GetLength(), 500);
		CHECK(TestItem4.GetSize() >= 500);

		TestItem3[1000] = 'x';
		TestItem3[1500] = '\0';
		TestItem6 = TestItem3;
		CHECK_EQUAL(TestItem6.GetLength(), 500);
		CHECK(TestItem6.GetSize() >= 500);

		TestItem4.Clear();
		CHECK_EQUAL(TestItem4.GetSize(), 0);
		CHECK_EQUAL(TestItem4.GetLength(), 0);
		CHECK(TestItem4 == "");

		ZEString TestItem7;
		TestItem7 = "Whazaa";
		CHECK_EQUAL((TestItem7.GetCharacter(1)), 'h');
		//TestItem7 = "";
		//ZETestCheckEqual(TestItem7.GetCharacter(0), '');
		// If we do this operation to an empty string, we got empty character constant error C2137 
		TestItem7.SetCharacter(0, 'a');
		CHECK_EQUAL(TestItem7.GetCharacter(0), 'a');
		TestItem7.SetCharacter(0, 'C');
		CHECK_EQUAL(TestItem7.GetCharacter(0), 'C');

	}

	ZETestItemAdd(ClearIsEmpty)
	{
		ZEString TestItem1;
		CHECK(TestItem1.IsEmpty());

		TestItem1 = "Zaaaa";
		TestItem1.Clear();
		CHECK_EQUAL(TestItem1.GetLength(), 0);
		CHECK_EQUAL(TestItem1.GetSize(), 0);
		CHECK(TestItem1.IsEmpty());

	}

	ZETestItemAdd(insert)
	{
		ZEString TestItem1 = "Insertable Text";
		TestItem1.Insert("Pre");
		CHECK_EQUAL(TestItem1, "PreInsertable Text");

		ZEString TestItem2 = "Insertable Text";
		TestItem2.Insert(ZEString("Pre"));
		CHECK_EQUAL(TestItem2, "PreInsertable Text");

		//InsertMiddle

		ZEString TestItem3 = "cCccCc";
		TestItem3.Insert(3, "EGEMENBASKAN");
		CHECK_EQUAL(TestItem3, "cCcEGEMENBASKANcCc");

		ZEString TestItem4 = "cCccCc";
		TestItem4.Insert(3, "");
		CHECK_EQUAL(TestItem4, "cCccCc");

		//insert end

		ZEString TestItem5 = "EndPoint";
		TestItem5.Insert(8, "Location");
		CHECK_EQUAL(TestItem5, "EndPointLocation");

	}

	ZETestItemAdd(AppendAndOperators)
	{
		// void Append(const ZEString& String
		ZEString TestItem1 = "StringA";
		TestItem1.Append("StringB");
		CHECK_EQUAL(TestItem1, "StringAStringB");

		TestItem1 = "LayLayLay";
		TestItem1.Append("LomLomLom");
		CHECK_EQUAL(TestItem1, "LayLayLayLomLomLom");
		TestItem1.Append(ZEString(" "));
		CHECK_EQUAL(TestItem1, "LayLayLayLomLomLom ");

		TestItem1 = "";
		TestItem1.Append("");
		CHECK_EQUAL(TestItem1, "");

		TestItem1 = "StringA";
		TestItem1 += ZEString("StringB");
		CHECK_EQUAL(TestItem1, "StringAStringB");

		TestItem1 = "StringA";
		CHECK_EQUAL(TestItem1 + "StringB", "StringAStringB");
		CHECK_EQUAL(TestItem1 +ZEString("StringB"), "StringAStringB");

		//[]
		ZEString TestItem2;
		TestItem2.Append("012");
		CHECK_EQUAL(TestItem2[0], '0');
		CHECK_EQUAL(TestItem2[1], '1');
		CHECK_EQUAL(TestItem2[2], '2');

		//!=
		int a;
		if(TestItem2 != "01")

			a=1;
		else
			a=2;
		CHECK_EQUAL(a, 1);

		int b;

		if (TestItem2 != "012")

			b = 1;
		else
			b = 2;

		CHECK_EQUAL(b, 2);

	}	

	ZETestItemAdd(Remove)
	{
		ZEString TestItem1 = "";
		TestItem1.Remove(0, 0);

		TestItem1 = "1234567890";
		TestItem1.Remove(0, 0);
		CHECK_EQUAL(TestItem1, "1234567890");

		TestItem1 = "1234567890";
		TestItem1.Remove(0, 1);
		CHECK_EQUAL(TestItem1, "234567890");

		TestItem1 = "1234567890";
		TestItem1.Remove(0, 5);
		CHECK_EQUAL(TestItem1, "67890");
		char c;
		c = TestItem1[1];
		CHECK_EQUAL(c, '7');
		CHECK_EQUAL(TestItem1.GetSize(), 6);
		CHECK_EQUAL(TestItem1.GetLength(), 5);
		TestItem1 = "1234567890";;
		TestItem1.Remove(1, 1);
		CHECK_EQUAL(TestItem1, "134567890");

		TestItem1 = "1234567890";
		TestItem1.Remove(1, 5);
		CHECK_EQUAL(TestItem1, "17890");

		TestItem1 = "1234567890";
		TestItem1.Remove(9, 1);
		CHECK_EQUAL(TestItem1, "123456789");

		TestItem1 = "1234567890";
		TestItem1.Remove(3, 4);
		CHECK_EQUAL(TestItem1, "123890");

	}

	ZETestItemAdd(Left)
	{
		ZEString TestItem1;
		CHECK_EQUAL(TestItem1.Left(0), "");

		TestItem1 = "1234Test5678";
		CHECK_EQUAL(TestItem1.Left(0), "");
		CHECK_EQUAL(TestItem1.Left(1), "1");
		CHECK_EQUAL(TestItem1.Left(4), "1234");
		CHECK_EQUAL(TestItem1.Left(12), "1234Test5678");

	}

	ZETestItemAdd(Right)
	{
		ZEString TestItem1;
		CHECK_EQUAL(TestItem1.Right(0), "");

		TestItem1 = "1234Test5678";
		CHECK_EQUAL(TestItem1.Right(0), "");
		CHECK_EQUAL(TestItem1.Right(1), "8");
		CHECK_EQUAL(TestItem1.Right(4), "5678");
		CHECK_EQUAL(TestItem1.Right(12), "1234Test5678");

	}

	ZETestItemAdd(Middle)
	{

		ZEString TestItem1;
		CHECK_EQUAL(TestItem1.Middle(0, 0), "");

		TestItem1 = "1234Test5678";
		CHECK_EQUAL(TestItem1.Middle(0, 0), "");
		CHECK_EQUAL(TestItem1.Middle(5, 0), "");
		CHECK_EQUAL(TestItem1.Middle(11, 0), "");

		//Front
		CHECK_EQUAL(TestItem1.Middle(0, 0), "");
		CHECK_EQUAL(TestItem1.Middle(0, 1), "1");
		CHECK_EQUAL(TestItem1.Middle(0, 4), "1234");
		CHECK_EQUAL(TestItem1.Middle(0, 12), "1234Test5678");

		//Back
		CHECK_EQUAL(TestItem1.Middle(12,0), "");
		CHECK_EQUAL(TestItem1.Middle(11,1), "8");
		CHECK_EQUAL(TestItem1.Middle(8, 4), "5678");

		//Middle
		CHECK_EQUAL(TestItem1.Middle(4, 0), "");
		CHECK_EQUAL(TestItem1.Middle(4, 1), "T");
		CHECK_EQUAL(TestItem1.Middle(4, 4), "Test");


	}

	ZETestItemAdd(SubString)
	{

		ZEString TestItem1;
		CHECK_EQUAL(TestItem1.SubString(0, 0), "");

		TestItem1 = "1234Test5678";
		CHECK_EQUAL(TestItem1.SubString(0, 0), "1");
		CHECK_EQUAL(TestItem1.SubString(0, 1), "12");
		CHECK_EQUAL(TestItem1.SubString(0, 3), "1234");

		CHECK_EQUAL(TestItem1.SubString(4, 4), "T");
		CHECK_EQUAL(TestItem1.SubString(4, 5), "Te");
		CHECK_EQUAL(TestItem1.SubString(4, 7), "Test");

		CHECK_EQUAL(TestItem1.SubString(8, 8), "5");
		CHECK_EQUAL(TestItem1.SubString(8, 9), "56");
		CHECK_EQUAL(TestItem1.SubString(8, 11), "5678");

	}

	ZETestItemAdd(ToLower)
	{
		ZEString TestItem1;
		ZEString TestItem2;
		ZEString TestItem3;

		TestItem1 = "name";
		TestItem2 = "HEALTH";
		TestItem3 = "WoUnD";

		CHECK_EQUAL(TestItem1.Lower(), "name");
		CHECK_EQUAL(TestItem2.Lower(), "health");
		CHECK_EQUAL(TestItem3.Lower(), "wound");

		TestItem1 = "";
		CHECK_EQUAL(TestItem1.Lower(), "");

	}

	ZETestItemAdd(ToUpper)
	{
		ZEString TestItem1;
		ZEString TestItem2;
		ZEString TestItem3;
		ZEString TestItem4;

		TestItem1 = "Name";
		TestItem2 = "HEALTH";
		TestItem3 = "WoUnD";
		TestItem4 = "";

		CHECK_EQUAL(TestItem1.Upper(), "NAME");
		CHECK_EQUAL(TestItem2.Upper(), "HEALTH");
		CHECK_EQUAL(TestItem3.Upper(), "WOUND");
		CHECK_EQUAL(TestItem4.Upper(), "");

	}

	ZETestItemAdd(CopyOnWrite)
	{
		ZEString TestItem1 = "MADAO";
		ZEString TestItem2 = "ERZA";
		ZEString TestItem3 = "NATSU";
		ZEString TestItem4 = "";
		TestItem1.CopyFrom(TestItem2);
		TestItem3.CopyTo(TestItem4);

		CHECK_EQUAL(TestItem1, "ERZA");
		CHECK_EQUAL(TestItem4, "NATSU");

		TestItem4 = "";
		TestItem1.CopyFrom(TestItem4);
		TestItem3.CopyTo(TestItem2);

		CHECK_EQUAL(TestItem4, "");
		CHECK_EQUAL(TestItem2, "NATSU");
	}

	ZETestItemAdd(Trim)
	{

		ZEString TestItem1;
		CHECK_EQUAL(TestItem1.TrimLeft(), "");
		CHECK_EQUAL(TestItem1.TrimRight(), "");
		CHECK_EQUAL(TestItem1.Trim(), "");

		TestItem1 = "    ";
		CHECK_EQUAL(TestItem1.TrimLeft(), "");
		CHECK_EQUAL(TestItem1.TrimRight(), "");
		CHECK_EQUAL(TestItem1.Trim(), "");

		TestItem1 = "Trim Trim";
		CHECK_EQUAL(TestItem1.TrimLeft(), "Trim Trim");
		CHECK_EQUAL(TestItem1.TrimRight(), "Trim Trim");
		CHECK_EQUAL(TestItem1.Trim(), "Trim Trim");

		TestItem1 = "   Trim Trim   ";
		CHECK_EQUAL(TestItem1.TrimLeft(), "Trim Trim   ");
		CHECK_EQUAL(TestItem1.TrimRight(), "   Trim Trim");
		CHECK_EQUAL(TestItem1.Trim(), "Trim Trim");

		TestItem1 = "\t\tTrim Trim\t\t";
		CHECK_EQUAL(TestItem1.TrimLeft(), "Trim Trim\t\t");
		CHECK_EQUAL(TestItem1.TrimRight(), "\t\tTrim Trim");
		CHECK_EQUAL(TestItem1.Trim(), "Trim Trim");

		TestItem1 = " \t \tTrim Trim \t \t";
		CHECK_EQUAL(TestItem1.TrimLeft(), "Trim Trim \t \t");
		CHECK_EQUAL(TestItem1.TrimRight(), " \t \tTrim Trim");
		CHECK_EQUAL(TestItem1.Trim(), "Trim Trim");

	}

	ZETestItemAdd(Castings)
	{
		//tointeger
		ZEString TestItem1;
		
			
	
		//todobule
		ZEString TestItem2;



		//tofloat
		ZEString TestITem3;



		//toUnsignedinteger
		ZEString TestItem4;

		

	}
		
}
