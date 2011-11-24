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

ZETestSuite(ZEString)
{
	ZETest("ZEString")//Equals, =
	{
		ZEString TestItem1, TestItem2;

		TestItem1 = "Orcun";
		TestItem2 = "Orcun";
		ZETestCheck(TestItem1.Equals(TestItem2));
		ZETestCheck(TestItem1 == TestItem2);

		TestItem2 = "nucrO";
		ZETestCheck(!TestItem1.Equals(TestItem2));
		ZETestCheck(TestItem1 != TestItem2);

		ZETestCheck(TestItem1 == "Orcun");
		ZETestCheck(TestItem2 == "nucrO");
	}

	ZETest("Assigments")
	{

		ZEString TestItem1;
		TestItem1.SetValue("Testing Testing");
		ZETestCheckEqual(TestItem1, "Testing Testing");

		TestItem1.SetValue("ASCII TEST ascii test");
		ZETestCheckEqual(TestItem1, "ASCII TEST ascii test");

		TestItem1.SetValue("UTF8 T�rk�e karakterler Test");
		ZETestCheckEqual(TestItem1, "UTF8 T�rk�e karakterler Test");

		TestItem1 = "Testing Testing";
		ZETestCheckEqual(TestItem1, "Testing Testing");

		TestItem1 = "Testing 123";
		ZETestCheckEqual(TestItem1, "Testing 123");

		ZEString TestItem2("Constructor");
		ZETestCheckEqual(TestItem2, "Constructor");

		ZEString TestItem3("Wide Constructor");
		ZETestCheckEqual(TestItem3, "Wide Constructor");

		ZEString TestItem4("Copy Constructor");
		ZETestCheckEqual(TestItem4, "Copy Constructor");

		ZEString TestItem10 = "Operator Assignment";
		ZETestCheckEqual(TestItem10, "Operator Assignment");

		ZEString TestItem5;
		ZEString TestItem6;
		TestItem5 = TestItem6;
		ZETestCheckEqual(TestItem5, "");

		ZEString TestItem7(TestItem5);
		ZETestCheckEqual(TestItem7, "");

	}
	ZETest("OneTestArmy")//getlength,getvalue,getsize,setvalue,getcharacter,setcharacter
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
		ZETestCheckEqual(TestItem1.GetLength(), 8);
		ZETestCheckEqual(TestItem1.GetSize(), 9);
		TestItem1.Clear();
		ZETestCheckEqual(TestItem1.GetLength(), 0);
		ZETestCheckEqual(TestItem1.GetSize(), 0);

		ZEString TestItem5("Constructor");
		TestItem2 = TestItem5;
		ZETestCheckEqual(TestItem2, TestItem5.GetValue());

		TestItem3[500] = '\0';
		ZETestCheck(TestItem4 == "");

		TestItem3[500] = '\0';
		TestItem4.SetValue(TestItem3);
		ZETestCheckEqual(TestItem4.GetLength(), 500);
		ZETestCheck(TestItem4.GetSize() >= 500);

		TestItem3[1000] = 'x';
		TestItem3[1500] = '\0';
		TestItem6 = TestItem3;
		ZETestCheckEqual(TestItem6.GetLength(), 500);
		ZETestCheck(TestItem6.GetSize() >= 500);

		TestItem4.Clear();
		ZETestCheckEqual(TestItem4.GetSize(), 0);
		ZETestCheckEqual(TestItem4.GetLength(), 0);
		ZETestCheck(TestItem4 == "");

		ZEString TestItem7;
		TestItem7 = "Whazaa";
		ZETestCheckEqual((TestItem7.GetCharacter(1)), 'h');
		//TestItem7 = "";
		//ZETestCheckEqual(TestItem7.GetCharacter(0), '');
		// If we do this operation to an empty string, we got empty character constant error C2137 
		TestItem7.SetCharacter(0, 'a');
		ZETestCheckEqual(TestItem7.GetCharacter(0), 'a');
		TestItem7.SetCharacter(0, 'C');
		ZETestCheckEqual(TestItem7.GetCharacter(0), 'C');

	}

	ZETest("ClearIsEmpty")
	{
		ZEString TestItem1;
		ZETestCheck(TestItem1.IsEmpty());

		TestItem1 = "Zaaaa";
		TestItem1.Clear();
		ZETestCheckEqual(TestItem1.GetLength(), 0);
		ZETestCheckEqual(TestItem1.GetSize(), 0);
		ZETestCheck(TestItem1.IsEmpty());

	}

	ZETest("insert")
	{
		ZEString TestItem1 = "Insertable Text";
		TestItem1.Insert("Pre");
		ZETestCheckEqual(TestItem1, "PreInsertable Text");

		ZEString TestItem2 = "Insertable Text";
		TestItem2.Insert(ZEString("Pre"));
		ZETestCheckEqual(TestItem2, "PreInsertable Text");

		//InsertMiddle

		ZEString TestItem3 = "cCccCc";
		TestItem3.Insert(3, "EGEMENBASKAN");
		ZETestCheckEqual(TestItem3, "cCcEGEMENBASKANcCc");

		ZEString TestItem4 = "cCccCc";
		TestItem4.Insert(3, "");
		ZETestCheckEqual(TestItem4, "cCccCc");

		//insert end

		ZEString TestItem5 = "EndPoint";
		TestItem5.Insert(8, "Location");
		ZETestCheckEqual(TestItem5, "EndPointLocation");

	}

	ZETest("AppendAndOperators")
	{
		// void Append(const ZEString& String
		ZEString TestItem1 = "StringA";
		TestItem1.Append("StringB");
		ZETestCheckEqual(TestItem1, "StringAStringB");

		TestItem1 = "LayLayLay";
		TestItem1.Append("LomLomLom");
		ZETestCheckEqual(TestItem1, "LayLayLayLomLomLom");
		TestItem1.Append(ZEString(" "));
		ZETestCheckEqual(TestItem1, "LayLayLayLomLomLom ");

		TestItem1 = "";
		TestItem1.Append("");
		ZETestCheckEqual(TestItem1, "");

		TestItem1 = "StringA";
		TestItem1 += ZEString("StringB");
		ZETestCheckEqual(TestItem1, "StringAStringB");

		TestItem1 = "StringA";
		ZETestCheckEqual(TestItem1 + "StringB", "StringAStringB");
		ZETestCheckEqual(TestItem1 +ZEString("StringB"), "StringAStringB");

		//[]
		ZEString TestItem2;
		TestItem2.Append("012");
		ZETestCheckEqual(TestItem2[0], '0');
		ZETestCheckEqual(TestItem2[1], '1');
		ZETestCheckEqual(TestItem2[2], '2');

		//!=
		int a;
		if(TestItem2 != "01")

			a=1;
		else
			a=2;
		ZETestCheckEqual(a, 1);

		int b;

		if (TestItem2 != "012")

			b = 1;
		else
			b = 2;

		ZETestCheckEqual(b, 2);

	}	

	ZETest("Remove")
	{
		ZEString TestItem1 = "";
		TestItem1.Remove(0, 0);

		TestItem1 = "1234567890";
		TestItem1.Remove(0, 0);
		ZETestCheckEqual(TestItem1, "1234567890");

		TestItem1 = "1234567890";
		TestItem1.Remove(0, 1);
		ZETestCheckEqual(TestItem1, "234567890");

		TestItem1 = "1234567890";
		TestItem1.Remove(0, 5);
		ZETestCheckEqual(TestItem1, "67890");
		char c;
		c = TestItem1[1];
		ZETestCheckEqual(c, '7');
		ZETestCheckEqual(TestItem1.GetSize(), 6);
		ZETestCheckEqual(TestItem1.GetLength(), 5);
		TestItem1 = "1234567890";;
		TestItem1.Remove(1, 1);
		ZETestCheckEqual(TestItem1, "134567890");

		TestItem1 = "1234567890";
		TestItem1.Remove(1, 5);
		ZETestCheckEqual(TestItem1, "17890");

		TestItem1 = "1234567890";
		TestItem1.Remove(9, 1);
		ZETestCheckEqual(TestItem1, "123456789");

		TestItem1 = "1234567890";
		TestItem1.Remove(3, 4);
		ZETestCheckEqual(TestItem1, "123890");

	}

	ZETest("Left")
	{
		ZEString TestItem1;
		ZETestCheckEqual(TestItem1.Left(0), "");

		TestItem1 = "1234Test5678";
		ZETestCheckEqual(TestItem1.Left(0), "");
		ZETestCheckEqual(TestItem1.Left(1), "1");
		ZETestCheckEqual(TestItem1.Left(4), "1234");
		ZETestCheckEqual(TestItem1.Left(12), "1234Test5678");

	}

	ZETest("Right")
	{
		ZEString TestItem1;
		ZETestCheckEqual(TestItem1.Right(0), "");

		TestItem1 = "1234Test5678";
		ZETestCheckEqual(TestItem1.Right(0), "");
		ZETestCheckEqual(TestItem1.Right(1), "8");
		ZETestCheckEqual(TestItem1.Right(4), "5678");
		ZETestCheckEqual(TestItem1.Right(12), "1234Test5678");

	}

	ZETest("Middle")
	{

		ZEString TestItem1;
		ZETestCheckEqual(TestItem1.Middle(0, 0), "");

		TestItem1 = "1234Test5678";
		ZETestCheckEqual(TestItem1.Middle(0, 0), "");
		ZETestCheckEqual(TestItem1.Middle(5, 0), "");
		ZETestCheckEqual(TestItem1.Middle(11, 0), "");

		//Front
		ZETestCheckEqual(TestItem1.Middle(0, 0), "");
		ZETestCheckEqual(TestItem1.Middle(0, 1), "1");
		ZETestCheckEqual(TestItem1.Middle(0, 4), "1234");
		ZETestCheckEqual(TestItem1.Middle(0, 12), "1234Test5678");

		//Back
		ZETestCheckEqual(TestItem1.Middle(12,0), "");
		ZETestCheckEqual(TestItem1.Middle(11,1), "8");
		ZETestCheckEqual(TestItem1.Middle(8, 4), "5678");

		//Middle
		ZETestCheckEqual(TestItem1.Middle(4, 0), "");
		ZETestCheckEqual(TestItem1.Middle(4, 1), "T");
		ZETestCheckEqual(TestItem1.Middle(4, 4), "Test");


	}

	ZETest("SubString")
	{

		ZEString TestItem1;
		ZETestCheckEqual(TestItem1.SubString(0, 0), "");

		TestItem1 = "1234Test5678";
		ZETestCheckEqual(TestItem1.SubString(0, 0), "1");
		ZETestCheckEqual(TestItem1.SubString(0, 1), "12");
		ZETestCheckEqual(TestItem1.SubString(0, 3), "1234");

		ZETestCheckEqual(TestItem1.SubString(4, 4), "T");
		ZETestCheckEqual(TestItem1.SubString(4, 5), "Te");
		ZETestCheckEqual(TestItem1.SubString(4, 7), "Test");

		ZETestCheckEqual(TestItem1.SubString(8, 8), "5");
		ZETestCheckEqual(TestItem1.SubString(8, 9), "56");
		ZETestCheckEqual(TestItem1.SubString(8, 11), "5678");

	}

	ZETest("ToLower")
	{
		ZEString TestItem1;
		ZEString TestItem2;
		ZEString TestItem3;

		TestItem1 = "name";
		TestItem2 = "HEALTH";
		TestItem3 = "WoUnD";

		ZETestCheckEqual(TestItem1.Lower(), "name");
		ZETestCheckEqual(TestItem2.Lower(), "health");
		ZETestCheckEqual(TestItem3.Lower(), "wound");

		TestItem1 = "";
		ZETestCheckEqual(TestItem1.Lower(), "");

	}

	ZETest("ToUpper")
	{
		ZEString TestItem1;
		ZEString TestItem2;
		ZEString TestItem3;
		ZEString TestItem4;

		TestItem1 = "Name";
		TestItem2 = "HEALTH";
		TestItem3 = "WoUnD";
		TestItem4 = "";

		ZETestCheckEqual(TestItem1.Upper(), "NAME");
		ZETestCheckEqual(TestItem2.Upper(), "HEALTH");
		ZETestCheckEqual(TestItem3.Upper(), "WOUND");
		ZETestCheckEqual(TestItem4.Upper(), "");

	}

	ZETest("CopyOnWrite")
	{
		ZEString TestItem1 = "MADAO";
		ZEString TestItem2 = "ERZA";
		ZEString TestItem3 = "NATSU";
		ZEString TestItem4 = "";
		TestItem1.CopyFrom(TestItem2);
		TestItem3.CopyTo(TestItem4);

		ZETestCheckEqual(TestItem1, "ERZA");
		ZETestCheckEqual(TestItem4, "NATSU");

		TestItem4 = "";
		TestItem1.CopyFrom(TestItem4);
		TestItem3.CopyTo(TestItem2);

		ZETestCheckEqual(TestItem4, "");
		ZETestCheckEqual(TestItem2, "NATSU");
	}

	ZETest("Trim")
	{

		ZEString TestItem1;
		ZETestCheckEqual(TestItem1.TrimLeft(), "");
		ZETestCheckEqual(TestItem1.TrimRight(), "");
		ZETestCheckEqual(TestItem1.Trim(), "");

		TestItem1 = "    ";
		ZETestCheckEqual(TestItem1.TrimLeft(), "");
		ZETestCheckEqual(TestItem1.TrimRight(), "");
		ZETestCheckEqual(TestItem1.Trim(), "");

		TestItem1 = "Trim Trim";
		ZETestCheckEqual(TestItem1.TrimLeft(), "Trim Trim");
		ZETestCheckEqual(TestItem1.TrimRight(), "Trim Trim");
		ZETestCheckEqual(TestItem1.Trim(), "Trim Trim");

		TestItem1 = "   Trim Trim   ";
		ZETestCheckEqual(TestItem1.TrimLeft(), "Trim Trim   ");
		ZETestCheckEqual(TestItem1.TrimRight(), "   Trim Trim");
		ZETestCheckEqual(TestItem1.Trim(), "Trim Trim");

		TestItem1 = "\t\tTrim Trim\t\t";
		ZETestCheckEqual(TestItem1.TrimLeft(), "Trim Trim\t\t");
		ZETestCheckEqual(TestItem1.TrimRight(), "\t\tTrim Trim");
		ZETestCheckEqual(TestItem1.Trim(), "Trim Trim");

		TestItem1 = " \t \tTrim Trim \t \t";
		ZETestCheckEqual(TestItem1.TrimLeft(), "Trim Trim \t \t");
		ZETestCheckEqual(TestItem1.TrimRight(), " \t \tTrim Trim");
		ZETestCheckEqual(TestItem1.Trim(), "Trim Trim");

	}

	ZETest("Castings")
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
