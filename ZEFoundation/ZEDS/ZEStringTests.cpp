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
#include <String>
#include "ZETest.h"

ZETestSuite(ZEString)
{
	ZETest("void ZEString::Append(const ZEString & String)")
	{
		ZEString StringA = "Lorem";
		ZEString StringB = " Ipsum";
		unsigned int ExpectedSize = 12;
		unsigned int ExpectedLength = 11;

		StringA.Append(StringB);

		ZETestCheckEqual(StringA, "Lorem Ipsum");
		ZETestCheckEqual(StringA.GetSize(), ExpectedSize);
		ZETestCheckEqual(StringA.GetLength(), ExpectedLength);
	}
	ZETest("void ZEString::Append(const char * String)")
	{
		ZETestCase("char *string")
		{
			ZEString StringA = "Lorem";
			const char* StringB = " Ipsum";
			unsigned int ExpectedSize = 12;
			unsigned int ExpectedLength = 11;

			StringA.Append(StringB);

			ZETestCheckEqual(StringA, "Lorem Ipsum");
			ZETestCheckEqual(StringA.GetSize(), ExpectedSize);
			ZETestCheckEqual(StringA.GetLength(), ExpectedLength);
		}
	}

	ZETest("void ZEString::Clear()")
	{
		ZEString String = "Lorem Ipsum";

		String.Clear();

		ZETestCheckEqual(String.GetLength(), 0);
		ZETestCheckEqual(String.GetSize(), 0);
	}

	ZETest("void ZEString::Compact()")
	{
		ZETestCheck(false);
	}

	ZETest("void ZEString::CopyFrom(const ZEString & String)")
	{
		ZETestCase("Copying a string to a previously non-empty ZEString")
		{
			ZEString StringA = "Lorem";
			ZEString StringB = "Ipsum";
			unsigned int ExpectedSize = 5;
			unsigned int ExpectedLength = 6;

			StringA.CopyFrom(StringB);

			ZETestCheckEqual(StringA, "Ipsum");
			ZETestCheckEqual(StringA.GetLength(), 5);
			ZETestCheckEqual(StringA.GetSize(), 6);
		}

		ZETestCase("Copying a string to a previously empty ZEString")
		{
			ZEString StringA = "Lorem Ipsum";
			ZEString StringB;
			unsigned int ExpectedSize = 12;
			unsigned int ExpectedLength = 11;

			StringB.CopyFrom(StringA);

			ZETestCheckEqual(StringB, "Lorem Ipsum");
			ZETestCheckEqual(StringB.GetLength(), ExpectedLength);
			ZETestCheckEqual(StringB.GetSize(), ExpectedSize);
		}
	}

	ZETest("void ZEString::CopyTo(ZEString & String) const")
		{
			ZETestCase("Copying a string to a previously empty ZEString")
			{
				ZEString StringA = "Lorem Ipsum";
				ZEString StringB;
				unsigned int ExpectedSize = 12;
				unsigned int ExpectedLength = 11;

				StringA.CopyTo(StringB);

				ZETestCheckEqual(StringB, "Lorem Ipsum");
				ZETestCheckEqual(StringB.GetLength(), ExpectedLength);
				ZETestCheckEqual(StringB.GetSize(), ExpectedSize);
			}

			ZETestCase("Copying a string to a previously non-empty ZEString")
			{
				ZEString StringA = "Lorem";
				ZEString StringB = "Ipsum Dolor";
				unsigned int ExpectedSize = 6;
				unsigned int ExpectedLength = 5;

				StringA.CopyTo(StringB);

				ZETestCheckEqual(StringB, "Lorem");
				ZETestCheckEqual(StringB.GetLength(), ExpectedLength);
				ZETestCheckEqual(StringB.GetSize(), ExpectedSize);
			}
		}

	ZETest("bool ZEString::Equals(const ZEString & String) const")
	{
		const ZEString StringA = "Lorem Ipsum";
		const ZEString StringB = "Lorem Ipsum";

		ZETestCheck(StringA.Equals(StringB));

		ZETestCase("Two ZEStrings are not equal")
		{
			const ZEString StringA = "Lorem Ipsum";
			const ZEString StringB = "Dolor Sit";
			ZETestCheck(!(StringA.Equals(StringB)));
		}
	}

	ZETest("bool ZEString::Equals(const char * String) const")
	{
		const ZEString StringA = "Lorem Ipsum";
		const char* StringB = "Lorem Ipsum";

		ZETestCheck(StringA.Equals(StringB));
		ZETestCheck(StringA.Equals("Lorem Ipsum"));

		ZETestCase("Two ZEStrings are not equal")
		{
			const ZEString StringA = "Lorem Ipsum";
			const char* StringB = "Dolor Sit";
			ZETestCheck(!(StringA.Equals(StringB)));
		}
	}

	ZETest("ZEString ZEString::Format(const char* Format, ...)")
	{
		ZETestCase("char")
		{
			ZEString StringA = ZEString::Format("%corem %cpsum", 'L', 'I');
			ZETestCheckEqual(StringA, "Lorem Ipsum");
		}

		ZETestCase("int")
		{
			ZEString StringB = ZEString::Format("c%dcTestc%dc", 1, 1);
			ZETestCheckEqual(StringB, "c1cTestc1c");
		}
	}

	ZETest("ZEString ZEString::FromBool(bool Value, const char* TrueText, const char* FalseText)")
	{
		ZETestCase("True")
		{
			ZEString StringA;
			ZETestCheckEqual(StringA.FromBool(1,"True","False"), "True");
		}

		ZETestCase("False")
		{
			ZEString StringA;
			ZETestCheckEqual(StringA.FromBool(0,"True","False"), "False");
		}
	}

	ZETest("ZEString ZEString::FromChar(char Value)")
	{
		char C = 'a';

		ZEString StringA = ZEString::FromChar(C);

		ZETestCheckEqual(StringA, "a");
	}

	ZETest("ZEString ZEString::FromCString(const char* Value)")
	{
		const char* String = "Lorem Ipsum";

		ZEString StringB = ZEString::FromCString(String);

		ZETestCheckEqual(StringB, "Lorem Ipsum");
	}

	ZETest("ZEString ZEString::FromFloat(float Value, ZEUInt Digits)")
	{
		float Value = 845756.88781;
		ZEUInt Digits = 7;

		ZEString String = ZEString::FromFloat(Value, Digits);

		ZETestCheckEqual(String, "845756.9");
	}

	ZETest("ZEString ZEString::FromInt(ZEInt Value, ZEUInt Base)")
	{
		ZEInt Value = 10;
		ZEUInt Base = 5;

		ZEString String = ZEString::FromInt(Value, Base);

		ZETestCheckEqual(String, "20");
	}

	ZETest("ZEString ZEString::FromStdString(const std::string& Value)")
	{
		std::string Example = "Lorem Ipsum";

		ZEString String = ZEString::FromStdString(Example);

		ZETestCheckEqual(String, "Lorem Ipsum");

	}

	ZETest("ZEString ZEString::FromUInt(ZEUInt Value, ZEUInt Base)")
	{
		ZEUInt Base = 2;
		ZEUInt Value = 120;

		ZEString String = ZEString::FromUInt(Value, Base);

		ZETestCheckEqual(String, "1111000");

	}

	ZETest("char ZEString::GetCharacter(size_t Position) const")
	{
		ZEString String = "Lorem Ipsum";

		char Result = String.GetCharacter(4);

		ZETestCheckEqual(Result, 'm');
	}

	ZETest("size_t ZEString::GetLength() const")
	{
		ZEString String = "Lorem Ipsum";
		unsigned int ExpectedLength = 11;

		ZETestCheckEqual(String.GetLength(), ExpectedLength);
	}

	ZETest("size_t ZEString::GetSize() const")
	{
		ZEString String = "Lorem Ipsum Dolor Sit Amet";
		unsigned int ExpectedSize = 27;

		ZETestCheckEqual(String.GetSize(), ExpectedSize);
	}

	ZETest("const char* ZEString::GetValue() const")
	{
		ZEString String = "Lorem";
		const char * C;

		C = String.GetValue();
		ZETestCheckEqual(C[0], 'L');
		ZETestCheckEqual(C[1], 'o');
		ZETestCheckEqual(C[2], 'r');
		ZETestCheckEqual(C[3], 'e');
		ZETestCheckEqual(C[4], 'm');
	}

	ZETest("void ZEString::Insert(const ZEString & String);")
	{
		ZEString StringA = "Ipsum";
		ZEString StringB = "Lorem ";

		StringA.Insert(StringB);
		ZETestCheckEqual(StringA, "Lorem Ipsum");

	}

	ZETest("void ZEString::Insert(const char * String);")
	{
		ZEString StringA = "Ipsum";
		const char* StringB = "Lorem ";

		StringA.Insert(StringB);
		ZETestCheckEqual(StringA, "Lorem Ipsum");
	}

	ZETest("void ZEString::Insert(size_t Position, const ZEString & String)")
	{
		ZETestCase("Inserting a ZEString inside another ZEString")
		{
			ZEString StringA = "Lorem Sit Amet";
			ZEString StringB = " Ipsum Dolor";
			unsigned int ExpectedLength = 26;

			StringA.Insert(5, StringB);

			ZETestCheckEqual(StringA, "Lorem Ipsum Dolor Sit Amet");
			ZETestCheckEqual(StringA.GetLength(), ExpectedLength);
		}

		ZETestCase("Inserting an empty ZEString inside another ZEString")
		{
			ZEString StringC = "Lorem";
			ZEString StringD = "";
			unsigned int ExpectedLength = 5;

			StringC.Insert(3, StringD);

			ZETestCheckEqual(StringC, "Lorem");
			ZETestCheckEqual(StringC.GetLength(), ExpectedLength);
		}

		ZETestCase("Inserting a ZESTring towards the end of the current ZEString")
		{
			ZEString StringE = "Lorem ";
			unsigned int ExpectedLength = 11;

			StringE.Insert(6, "Ipsum");

			ZETestCheckEqual(StringE, "Lorem Ipsum");
			ZETestCheckEqual(StringE.GetLength(), ExpectedLength);
		}
	}

	ZETest("void ZEString::Insert(size_t Position, const char * String)")
	{
		ZEString StringA = "Lorem ";
		const char* StringB = "Ipsum";
		unsigned int ExpectedLength = 11;
		size_t Position = 6;

		StringA.Insert(Position, StringB);

		ZETestCheckEqual(StringA, "Lorem Ipsum");
		ZETestCheckEqual(StringA.GetLength(), ExpectedLength);
	}

	ZETest("bool ZEString::IsEmpty() const")
	{
		ZETestCase("ZEString is empty")
		{
			ZEString StringA;
			ZEString StringB = "Lorem";
			StringB.Clear();

			ZETestCheck(StringA.IsEmpty());
			ZETestCheck(StringB.IsEmpty());
		}

		ZETestCase("ZEString is not empty")
		{
			ZEString StringC = "Lorem Ipsum";

			ZETestCheck(!(StringC.IsEmpty()));
		}
	}

	ZETest("ZEString ZEString::Left(size_t Count) const")
	{
		ZEString String;

		ZETestCase("ZEString is empty")
		{
			ZETestCheckEqual(String.Left(0), "");
		}
		ZETestCase("ZEString has a value")
		{
			String = "1234Test5678";

			ZETestCheckEqual(String.Left(0), "");
			ZETestCheckEqual(String.Left(1), "1");
			ZETestCheckEqual(String.Left(4), "1234");
			ZETestCheckEqual(String.Left(12), "1234Test5678");
		}
	}

	ZETest("ZEString ZEString::Lower() const")
	{
		ZEString StringA;
		ZEString StringB;
		ZEString StringC;

		StringA = "lorem";
		StringB = "IPSUM";
		StringC = "DoLoR";

		ZETestCheckEqual(StringA.Lower(), "lorem");
		ZETestCheckEqual(StringB.Lower(), "ipsum");
		ZETestCheckEqual(StringC.Lower(), "dolor");

		StringA = "";

		ZETestCheckEqual(StringA.Lower(), "");
	}

	ZETest("ZEString ZEString::Middle(size_t Position, size_t Count) const")
	{
		ZEString String;

		ZETestCase("ZEString is empty")
		{
			ZETestCheckEqual(String.Middle(0, 0), "");
		}

		ZETestCase("ZEString has a value")
		{
			String = "1234Test5678";

			ZETestCheckEqual(String.Middle(0, 0), "");
			ZETestCheckEqual(String.Middle(2, 8), "34Test56");
			ZETestCheckEqual(String.Middle(8, 4), "5678");
			ZETestCheckEqual(String.Middle(0, 12), "1234Test5678");
		}
	}

	ZETest("bool ZEString::operator!=(const ZEString & String) const")
	{
		ZETestCase("ZEStrings are not equal")
		{
			const ZEString StringA = "Lorem";
			const ZEString StringB = "Ipsum";

			ZETestCheck(StringA != StringB);
		}

		ZETestCase("ZEStrings are equal")
		{
			const ZEString StringC = "Lorem";
			const ZEString StringD = "Lorem";

			ZETestCheck(!(StringC != StringD));
		}
	}

	ZETest("bool ZEString::operator!=(const char * String) const")
	{

		ZETestCase("Strings are not equal")
		{
			const ZEString StringA = "Lorem";
			const char* StringB = "Ipsum";

			ZETestCheck(StringA != StringB);
		}

		ZETestCase("Strings are equal")
		{
			const ZEString StringC = "Lorem";
			const char* StringD = "Lorem";

			ZETestCheck(!(StringC != StringD));
		}
	}

	ZETest("char & ZEString::operator[](int Index)")
	{
		ZEString StringA = "Lorem";

		char C = StringA[0];

		ZETestCheckEqual(C, 'L');
	}


	ZETest("const char & ZEString::operator[](int Index) const")
	{
		const ZEString String = "Lorem";

		const char C = String[3];

		ZETestCheck(C, 'r');
	}

	ZETest("ZEString ZEString::operator+(const ZEString & String)")
	{
		ZEString StringA = "Lorem ";
		ZEString StringB = "Ipsum";

		ZEString Result = StringA + StringB;

		ZETestCheckEqual(Result, "Lorem Ipsum");

	}

	ZETest("ZEString ZEString::operator+(const char * String)")
	{
		ZEString StringA = "Lorem";
		const char* StringB = " Ipsum";

		ZEString Result = StringA + StringB;

		ZETestCheckEqual(Result, "Lorem Ipsum");
	}

	ZETest("ZEString & ZEString::operator+=(const ZEString & String)")
	{
		ZEString StringA = "Lorem ";
		ZEString StringB = "Ipsum";

		StringA += StringB;
		ZETestCheckEqual(StringA, "Lorem Ipsum");
	}

	ZETest("ZEString & ZEString::operator+=(const char * String)")
	{
		ZEString StringA = "Lorem ";
		const char* StringB = "Ipsum";

		StringA += StringB;
		ZETestCheckEqual(StringA, "Lorem Ipsum");
	}

	ZETest("ZEString & ZEString::operator=(const ZEString & String)")
	{
		ZEString StringA = "Lorem";
		ZEString StringB = "Ipsum";

		StringA = StringB;

		ZETestCheckEqual(StringA, "Ipsum");
	}

	ZETest("ZEString & ZEString::operator=(const char * String)")
	{
		ZEString StringA = "Lorem";
		const char* StringB = "Ipsum";

		StringA = StringB;

		ZETestCheckEqual(StringA, "Ipsum");
	}

	ZETest(" bool ZEString::operator==(const ZEString & String) const")
	{
		ZETestCase("ZEStrings are not equal")
		{
			const ZEString StringA = "Lorem";
			const ZEString StringB = "Ipsum";

			ZETestCheck(!(StringA == StringB));
		}
		ZETestCase("ZEStrings are equal")
		{
			const ZEString StringC = "Lorem Ipsum";
			const ZEString StringD = "Lorem Ipsum";

			ZETestCheck(StringC == StringD);
		}
	}

	ZETest("bool ZEString::operator==(const char * String) const")
	{
		ZETestCase("Strings are not equal")
		{
			const ZEString StringA = "Lorem";
			const char* StringB = "Ipsum";

			ZETestCheck(!(StringA == StringB));
		}

		ZETestCase("Strings are equal")
		{
			const ZEString StringA = "Lorem Ipsum";
			const char* StringB = "Lorem Ipsum";

			ZETestCheck(StringA == StringB);
		}
    }

	ZETest("ZEString::operator const char*() const")
	{
		ZEString StringA = "Lorem Ipsum";
		const char* StringB = (const char*)StringA;

		ZETestCheck(strcmp(StringB, StringA.GetValue()) == 0);
		
	}

	ZETest("ZEString::operator std::string() const")
	{
		ZEString StringA = "Lorem Ipsum";
		std::string ExpectedResult = "Lorem Ipsum";

		std::string StringB = (std::string)StringA;

		//ZETestCheck(strcmp(StringB, StringA.GetValue()) == 0);
		ZETestCheckEqual(StringB, ExpectedResult);
	}

	ZETest("void ZEString::Remove(size_t Position, unsigned int Count = 1)")
	{
		ZEString String = "1234567890";
		size_t Position = 3;
		unsigned int Count = 5;

		String.Remove(Position, Count);
		ZETestCheckEqual(String, "12390");
		ZETestCheckEqual(String.GetLength(), 5);
	}

	ZETest("ZEString ZEString::Right(size_t Count) const")
	{
		ZEString String;

		ZETestCase("ZEString is empty")
		{
			ZETestCheckEqual(String.Right(0), "");
		}
		
		ZETestCase("ZEString has a value")
		{
			String = "1234Test5678";

			ZETestCheckEqual(String.Right(0), "");
			ZETestCheckEqual(String.Right(1), "8");
			ZETestCheckEqual(String.Right(4), "5678");
			ZETestCheckEqual(String.Right(12), "1234Test5678");
		}
	}

	ZETest("void ZEString::SetCharacter(size_t Position, char Value)")
	{
		ZEString String = "0123456789";
		char Value = 'X';
		size_t Position = 5;

		String.SetCharacter(Position, Value);
		ZETestCheckEqual(String, "01234X6789");
	}

	ZETest("void ZEString::SetValue(bool Value, const char* TrueText, const char* FalseText)")
	{
		ZETestCase("ZEString value is set to True")
		{
			ZEString StringA;

			StringA.SetValue(1, "True", "False");

			ZETestCheckEqual(StringA, "True");
		}
		ZETestCase("ZEString value is set to False")
		{
			ZEString StringB;

			StringB.SetValue(0, "True", "False");

			ZETestCheckEqual(StringB, "False");
		}
		
	}

	ZETest("void ZEString::SetValue(const char* String)")
	{
		ZEString StringA;
		const char* StringB = "Lorem";

		StringA.SetValue(StringB);

		ZETestCheckEqual(StringA, "Lorem");
	}

	ZETest("void ZEString::SetValue(float Value, ZEUInt Digits)")
	{
		float Value = 400.5525252f;
		ZEString String;
		ZEUInt Digits = 6;
		String.SetValue(Value, Digits);
		ZETestCheckEqual(String, "400.553");
	}

	ZETest("void ZEString::SetValue(ZEInt Value, ZEUInt Base)")
	{
		ZEString String;
		ZEInt Value = 10;
		ZEUInt Base = 2;

		String.SetValue(Value, Base);
		ZETestCheckEqual(String, "1010");
	}

	ZETest("void ZEString::SetValue(ZEUInt Value, ZEUInt Base)")
	{
		ZEString String;
		ZEUInt Value = 6;
		ZEUInt Base = 2;

		String.SetValue(Value, Base);

		ZETestCheckEqual(String, ZEString("110"));
	}

	ZETest("ZEString::ZEString(const char * String)")
	{
		const char* StringA = "Lorem Ipsum";

		ZEString StringB(StringA);

		ZETestCheckEqual(StringB, "Lorem Ipsum");
	}

	ZETest("ZEString::ZEString(const ZEString & String)")
	{
		ZEString StringA = "Lorem Ipsum";

		ZEString StringB(StringA);

		ZETestCheckEqual(StringB, "Lorem Ipsum");

	}

	ZETest("ZEString ZEString::SubString(size_t StartPosition, size_t EndPosition) const")
	{
		ZEString String;

		ZETestCase("ZEString is empty")
		{
			ZETestCheckEqual(String.SubString(0, 0), "");
		}

		ZETestCase("ZEString has a value")
		{
			String = "1234Test5678";
			ZETestCheckEqual(String.SubString(0, 0), "1");
			ZETestCheckEqual(String.SubString(0, 1), "12");
			ZETestCheckEqual(String.SubString(0, 3), "1234");

			ZETestCheckEqual(String.SubString(4, 4), "T");
			ZETestCheckEqual(String.SubString(4, 5), "Te");
			ZETestCheckEqual(String.SubString(4, 7), "Test");

			ZETestCheckEqual(String.SubString(8, 8), "5");
			ZETestCheckEqual(String.SubString(8, 9), "56");
			ZETestCheckEqual(String.SubString(8, 11), "5678");
		}
	}

	ZETest("const char* ZEString::ToCString() const")
	{
		ZEString StringA = "Lorem Ipsum";
		const char* StringB;

		StringB = StringA.ToCString();

		ZETestCheck(strcmp(StringB, StringA.GetValue()) == 0);
	}

	ZETest("std::string ZEString::ToStdString() const")
	{
		ZEString StringA = "Lorem Ipsum";
		std::string StringB;
		std::string ExpectedResult = "Lorem Ipsum";

		StringB = StringA.ToStdString();

		//ZETestCheck(strncmp(a.c_str(), A, A.GetSize()) == 0);
		ZETestCheckEqual(StringB, ExpectedResult);
	}

	ZETest("ZEUINT32 ZEString::ToUInt() const")
	{
		ZEString String = "123";
		ZEUInt32 ExpectedValue = 123;

		ZEUInt32 Value = String.ToUInt();

		ZETestCheckEqual(Value, ExpectedValue);
	}

	ZETest("ZEString ZEString::Trim() const")
	{
		ZEString String;
		ZETestCheckEqual(String.Trim(), "");

		String = "    ";
		ZETestCheckEqual(String.Trim(), "");

		String = "Lorem Ipsum";
		ZETestCheckEqual(String.Trim(), "Lorem Ipsum");

		String = "   Lorem Ipsum   ";
		ZETestCheckEqual(String.Trim(), "Lorem Ipsum");

		String = "\t\tLorem Ipsum\t\t";
		ZETestCheckEqual(String.Trim(), "Lorem Ipsum");

		String = " \t \tLorem Ipsum \t \t";
		ZETestCheckEqual(String.Trim(), "Lorem Ipsum");
	}

	ZETest("ZEString ZEString::TrimLeft() const")
	{
		ZEString String;
		ZETestCheckEqual(String.TrimLeft(), "");

		String = "    ";
		ZETestCheckEqual(String.TrimLeft(), "");

		String = "Lorem Ipsum";
		ZETestCheckEqual(String.TrimLeft(), "Lorem Ipsum");

		String = "   Lorem Ipsum   ";
		ZETestCheckEqual(String.TrimLeft(), "Lorem Ipsum   ");

		String = "\t\tLorem Ipsum\t\t";
		ZETestCheckEqual(String.TrimLeft(), "Lorem Ipsum\t\t");

		String = " \t \tLorem Ipsum \t \t";
		ZETestCheckEqual(String.TrimLeft(), "Lorem Ipsum \t \t");
	}

	ZETest("ZEString ZEString::TrimRight() const")
	{
		ZEString String;
		ZETestCheckEqual(String.TrimRight(), "");

		String = "    ";
		ZETestCheckEqual(String.TrimRight(), "");

		String = "Lorem Ipsum";
		ZETestCheckEqual(String.TrimRight(), "Lorem Ipsum");

		String = "   Lorem Ipsum   ";
		ZETestCheckEqual(String.TrimRight(), "   Lorem Ipsum");

		String = "\t\tLorem Ipsum\t\t";
		ZETestCheckEqual(String.TrimRight(), "\t\tLorem Ipsum");

		String = " \t \tLorem Ipsum \t \t";
		ZETestCheckEqual(String.TrimRight(), " \t \tLorem Ipsum");
	}

	ZETest(" ZEString ZEString::Upper() const")
	{
		ZEString StringA;
		ZEString StringB;
		ZEString StringC;
		ZEString StringD;

		StringA = "Lorem";
		StringB = "IPSUM";
		StringC = "DoLoR";
		StringD = "";

		ZETestCheckEqual(StringA.Upper(), "LOREM");
		ZETestCheckEqual(StringB.Upper(), "IPSUM");
		ZETestCheckEqual(StringC.Upper(), "DOLOR");
		ZETestCheckEqual(StringD.Upper(), "");
	}

	ZETest("float ZEString::ToFloat()")
	{
		ZEString String = "-255.046";

		float Value = String.ToFloat();

		ZETestCheckClose(Value, -255.04601f);

	}

	ZETest("int ZEString::ToInt()")
	{
		ZEString StringA = "255";
		ZEString StringB = "-255.043";

		int ValueA = StringA.ToInt();
		int ValueB = StringB.ToInt();

		ZETestCheckEqual(ValueA, 255);
		ZETestCheckEqual(ValueB, -255);
	}

	ZETest("ZEString operator+(const char* String1, const ZEString& String2)")
	{
		ZEString StringA = "Ipsum";
		const char* StringB = "Lorem ";
		ZEString Result;

		Result = StringB + StringA;

		ZETestCheckEqual(Result, "Lorem Ipsum");
	}
}
