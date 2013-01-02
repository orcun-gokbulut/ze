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
#include <string>
#include "ZETest/ZETest.h"

ZETestSuite(ZEString)
{
	ZETest("void ZEString::Append(const ZEString & String)")
	{
		ZEString StringA = "Lorem";
		ZEString StringB = " Ipsum";
		ZEUInt ExpectedSize = 12;
		ZEUInt ExpectedLength = 11;

		StringA.Append(StringB);

		ZETestCheck(StringA == "Lorem Ipsum");
		ZETestCheck(StringA.GetSize() == ExpectedSize);
		ZETestCheck(StringA.GetLength() == ExpectedLength);

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Ilık";
			ZEString StringD = " Süt İç";

			StringC.Append(StringD);

			ZETestCheck(StringC == "Işık Ilık Süt İç");
			ZETestCheck(StringC.GetSize() ==  23);
			ZETestCheck(StringC.GetLength() == 16);
		}
	}
	ZETest("void ZEString::Append(const char * String)")
	{
		ZEString StringA = "Lorem";
		const char* StringB = " Ipsum";
		ZEUInt ExpectedSize = 12;
		ZEUInt ExpectedLength = 11;

		StringA.Append(StringB);

		ZETestCheck(StringA == "Lorem Ipsum");
		ZETestCheck(StringA.GetSize() == ExpectedSize);
		ZETestCheck(StringA.GetLength() == ExpectedLength);

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Ilık";
			const char* StringD = " Süt İç";

			StringC.Append(StringD);

			ZETestCheck(StringC == "Işık Ilık Süt İç");
			ZETestCheck(StringC.GetSize() == 23);
			ZETestCheck(StringC.GetLength() == 16);
		}
	}

	ZETest("void ZEString::Clear()")
	{
		ZEString String = "Lorem Ipsum";

		String.Clear();

		ZETestCheck(String.GetLength() == 0);
		ZETestCheck(String.GetSize() == 0);
	}

	ZETest("void ZEString::Compact()")
	{
		ZEString String("ZE");
		ZETestCheck(String.GetSize() == 3);

		String.SetSize(7);
		ZETestCheck(String == "ZE");
		ZETestCheck(String.GetSize() == 7);

		String.Compact();
		ZETestCheck(String == "ZE");
		ZETestCheck(String.GetSize() == 3);
	}

	ZETest("void ZEString::CopyFrom(const ZEString & String)")
	{
		ZETestCase("Copying a string to a previously non-empty ZEString")
		{
			ZEString StringA = "Lorem";
			ZEString StringB = "Ipsum";
			ZEUInt ExpectedSize = 5;
			ZEUInt ExpectedLength = 6;

			StringA.CopyFrom(StringB);

			ZETestCheck(StringA == "Ipsum");
			ZETestCheck(StringA.GetLength() == 5);
			ZETestCheck(StringA.GetSize() == 6);
		}

		ZETestCase("Copying a string to a previously empty ZEString")
		{
			ZEString StringA = "Lorem Ipsum";
			ZEString StringB;
			ZEUInt ExpectedSize = 12;
			ZEUInt ExpectedLength = 11;

			StringB.CopyFrom(StringA);

			ZETestCheck(StringB == "Lorem Ipsum");
			ZETestCheck(StringB.GetLength() == ExpectedLength);
			ZETestCheck(StringB.GetSize() == ExpectedSize);
		}

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Ilık";
			ZEString StringD = "Süt İç";

			StringC.CopyFrom(StringD);

			ZETestCheck(StringC == "Süt İç");
			ZETestCheck(StringC.GetSize() == 10);
			ZETestCheck(StringC.GetLength() == 6);
		}
	}

	ZETest("void ZEString::CopyTo(ZEString & String) const")
		{
			ZETestCase("Copying a string to a previously empty ZEString")
			{
				ZEString StringA = "Lorem Ipsum";
				ZEString StringB;
				ZEUInt ExpectedSize = 12;
				ZEUInt ExpectedLength = 11;

				StringA.CopyTo(StringB);

				ZETestCheck(StringB == "Lorem Ipsum");
				ZETestCheck(StringB.GetLength() == ExpectedLength);
				ZETestCheck(StringB.GetSize() == ExpectedSize);
			}

			ZETestCase("Copying a string to a previously non-empty ZEString")
			{
				ZEString StringA = "Lorem";
				ZEString StringB = "Ipsum Dolor";
				ZEUInt ExpectedSize = 6;
				ZEUInt ExpectedLength = 5;

				StringA.CopyTo(StringB);

				ZETestCheck(StringB == "Lorem");
				ZETestCheck(StringB.GetLength() == ExpectedLength);
				ZETestCheck(StringB.GetSize() == ExpectedSize);
			}

			ZETestCase("UTF-8 encoding compatibility test")
			{
				ZEString StringC = "Işık Ilık";
				ZEString StringD = "Süt İç";

				StringC.CopyTo(StringD);

				ZETestCheck(StringD == "Işık Ilık");
				ZETestCheck(StringD.GetSize() == 13);
				ZETestCheck(StringD.GetLength() == 9);
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

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Ilık";
			ZEString StringD = "Süt İç";
			ZEString StringE = "Işık Ilık";

			ZETestCheck(StringC.Equals(StringE));
			ZETestCheck(!(StringC.Equals(StringD)));
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

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Ilık";
			const char* StringD = "Süt İç";
			const char* StringE = "Işık Ilık";

			ZETestCheck(StringC.Equals(StringE));
			ZETestCheck(!(StringC.Equals(StringD)));
		}
	}

	ZETest("ZEString ZEString::FromChar(char Value)")
	{
		char C = 'a';

		ZEString StringA = ZEString::FromChar(C);

		ZETestCheck(StringA == "a");
	}

	ZETest("ZEString ZEString::FromCString(const char* Value)")
	{
		const char* String = "Lorem Ipsum";

		ZEString StringA = ZEString::FromCString(String);

		ZETestCheck(StringA == "Lorem Ipsum");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			const char* StringB = "Işık Ilık Süt İç";

			ZEString StringC = ZEString::FromCString(StringB);

			ZETestCheck(StringC == "Işık Ilık Süt İç");
		}
	}

	ZETest("ZEString ZEString::FromDouble(double Value, const char* Format)")
	{
		double Value = 845756.88781;

		ZEString String = ZEString::FromDouble(Value);

		ZETestCheck(String == "845756.875000");

		ZETestCase("for Format != NULL")
		{
			String = ZEString::FromDouble(Value, ".");
			ZETestCheck(String == "845757");
		}
	}

	ZETest("ZEString ZEString::FromFloat(float Value, const char* Format)")
	{
		float Value = 845756.88781f;

		ZEString String = ZEString::FromFloat(Value);

		ZETestCheck(String == "845756.875000");

		ZETestCase("for Format != NULL")
		{
			String = ZEString::FromFloat(Value, "12.1");
			ZETestCheck(String == "    845756.9");
		}
	}

	ZETest("ZEString ZEString::FromInt16(ZEInt16 Value, const char* Format)")
	{
		ZEInt16 Value = -150;

		ZEString String = ZEString::FromInt16(Value);

		ZETestCheck(String == "-150");

		ZETestCase("for Format != NULL")
		{
			String = ZEString::FromInt16(Value, "d:.6");
			ZETestCheck(String == "-000150");
		}
	}

	ZETest("ZEString ZEString::FromInt32(ZEInt32 Value, const char* Format)")
	{
		ZEInt32 Value = 845756;

		ZEString String = ZEString::FromInt32(Value);

		ZETestCheck(String == "845756");

		ZETestCase("for Format != NULL")
		{
			String = ZEString::FromInt32(Value, "x:08");
			ZETestCheck(String == "000ce7bc");
		}
	}

	ZETest("ZEString ZEString::FromInt64(ZEInt64 Value, const char* Format)")
	{
		ZEInt64 Value = -845756;

		ZEString String = ZEString::FromInt64(Value);

		ZETestCheck(String == "-845756");

		ZETestCase("for Format != NULL")
		{
			String = ZEString::FromInt64(Value, "d:-11.8");
			ZETestCheck(String == "-00845756  ");
		}
	}

	ZETest("ZEString ZEString::FromInt8(ZEInt8 Value, const char* Format)")
	{
		ZEInt8 Value = -20;

		ZEString String = ZEString::FromInt8(Value);

		ZETestCheck(String == "-20");

		ZETestCase("for Format != NULL")
		{
			String = ZEString::FromInt8(Value, "d:6.3");
			ZETestCheck(String == "  -020");
		}
	}

	ZETest("ZEString ZEString::FromStdString(const std::string& Value)")
	{
		std::string Example = "Lorem Ipsum";

		ZEString StringA = ZEString::FromStdString(Example);

		ZETestCheck(StringA == "Lorem Ipsum");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			std::string Example2 = "Işık Ilık Süt İç";

			ZEString StringB = ZEString::FromStdString(Example2);

			ZETestCheck(StringB == "Işık Ilık Süt İç");
		}
	}

	ZETest("ZEString ZEString::FromUInt16(ZEUInt16 Value, const char* Format)")
	{
		ZEUInt16 Value = 150;

		ZEString String = ZEString::FromUInt16(Value);

		ZETestCheck(String == "150");

		ZETestCase("for Format != NULL")
		{
			String = ZEString::FromUInt16(Value, "x:04");
			ZETestCheck(String == "0096");
		}
	}

	ZETest("ZEString ZEString::FromUInt32(ZEUInt32 Value, const char* Format)")
	{
		ZEUInt32 Value = 845756;

		ZEString String = ZEString::FromUInt32(Value);

		ZETestCheck(String == "845756");

		ZETestCase("for Format != NULL")
		{
			String = ZEString::FromUInt32(Value, "X:-10.8");
			ZETestCheck(String = "000CE7BC  ");
		}
	}

	ZETest("ZEString ZEString::FromUInt64(ZEUInt64 Value, const char* Format)")
	{
		ZEUInt64 Value = 845756.88781;

		ZEString String = ZEString::FromUInt64(Value);

		ZETestCheck(String == "845756");

		ZETestCase("for Format != NULL")
		{
			String = ZEString::FromUInt64(Value, "d:8.7");
			ZETestCheck(String == " 0845756");
		}
	}

	ZETest("ZEString ZEString::FromUInt8(ZEUInt8 Value, const char* Format)")
	{
		ZEUInt8 Value = 122;

		ZEString String = ZEString::FromUInt8(Value);

		ZETestCheck(String == "122");

		ZETestCase("for Format != NULL")
		{
			String = ZEString::FromUInt8(Value, "c:-4");
			ZETestCheck(String == "z   ");
		}
	}

	ZETest("ZEString ZEString::FromWChar(wchar_t Value)")
	{
		wchar_t WideCharacter = L'a';

		ZEString StringA = ZEString::FromWChar(WideCharacter);

		ZETestCheck(StringA == L"a");
	}

	ZETest("ZEString ZEString::FromWCString(const wchar_t* Value)")
	{
		wchar_t Example[] = {8364, 0};

		ZEString StringA = ZEString::FromWCString(Example);

		ZETestCheck(StringA == "€");
	}

	ZETest("ZEString ZEString::FromWStdString(const std::wstring& Value)")
	{
		wchar_t CString[] = {8364, 0};

		ZEString StringA = ZEString::FromWStdString(std::wstring(CString));

		ZETestCheck(StringA == "€");
	}

	ZETest("char ZEString::GetCharacter(ZESize Position) const")
	{
		ZEString StringA = "Lorem Ipsum";

		char Result1 = StringA.GetCharacter(4);

		ZETestCheck(Result1 == 'm');

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringB = "Işık Ilık Süt İç";

			ZECharacter Result2 = StringB.GetCharacter(3);
			ZECharacter Result3 = StringB.GetCharacter(11);

			ZETestCheck(Result2 == "k");
			ZETestCheck(Result3 == "ü");
		}
	}

	ZETest("ZESize ZEString::GetLength() const")
	{
		ZEString String = "Lorem Ipsum";
		ZEUInt ExpectedLength = 11;

		ZETestCheck(String.GetLength() == ExpectedLength);

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringB = "Işık Ilık Süt İç";

			ZETestCheck(StringB.GetLength() == 16);
		}
	}

	ZETest("ZESize ZEString::GetSize() const")
	{
		ZEString String = "Lorem Ipsum Dolor Sit Amet";
		ZEUInt ExpectedSize = 27;

		ZETestCheck(String.GetSize() == ExpectedSize);

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringB = "Işık Ilık Süt İç";

			ZETestCheck(StringB.GetSize() == 23);
		}
	}

	ZETest("const char* ZEString::GetValue() const")
	{
		ZEString String = "Lorem";
		const char* Value = String.GetValue();

		ZETestCheck(strcmp(Value, "Lorem") == 0);

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringB = "Işık";
			const char* Value2 = StringB.GetValue();

			ZETestCheck(strcmp(Value2, "Işık") == 0);
		}
	}

	ZETest("void ZEString::Insert(const ZEString & String);")
	{
		ZEString StringA = "Ipsum";
		ZEString StringB = "Lorem ";

		StringA.Insert(StringB);
		ZETestCheck(StringA == "Lorem Ipsum");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Süt İç";
			ZEString StringD = "Işık Ilık ";

			StringC.Insert(StringD);

			ZETestCheck(StringC == "Işık Ilık Süt İç");
		}
	}

	ZETest("void ZEString::Insert(const char * String);")
	{
		ZEString StringA = "Ipsum";
		const char* StringB = "Lorem ";

		StringA.Insert(StringB);
		ZETestCheck(StringA == "Lorem Ipsum");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Süt İç";
			const char* StringD = "Işık Ilık ";

			StringC.Insert(StringD);

			ZETestCheck(StringC == "Işık Ilık Süt İç");
		}
	}

	ZETest("void ZEString::Insert(ZESize Position, const ZEString & String)")
	{
		ZETestCase("Inserting a ZEString inside another ZEString")
		{
			ZEString StringA = "Lorem Sit Amet";
			ZEString StringB = " Ipsum Dolor";
			ZEUInt ExpectedLength = 26;

			StringA.Insert(5, StringB);

			ZETestCheck(StringA == "Lorem Ipsum Dolor Sit Amet");
			ZETestCheck(StringA.GetLength() == ExpectedLength);
		}

		ZETestCase("Inserting an empty ZEString inside another ZEString")
		{
			ZEString StringC = "Lorem";
			ZEString StringD = "";
			ZEUInt ExpectedLength = 5;

			StringC.Insert(3, StringD);

			ZETestCheck(StringC == "Lorem");
			ZETestCheck(StringC.GetLength() == ExpectedLength);
		}

		ZETestCase("Inserting a ZESTring towards the end of the current ZEString")
		{
			ZEString StringE = "Lorem ";
			ZEUInt ExpectedLength = 11;

			StringE.Insert(6, "Ipsum");

			ZETestCheck(StringE == "Lorem Ipsum");
			ZETestCheck(StringE.GetLength() == ExpectedLength);
		}

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Süt İç";
			ZEString StringD = "Ilık ";

			StringC.Insert(5, StringD);

			ZETestCheck(StringC == "Işık Ilık Süt İç");
		}
	}

	ZETest("void ZEString::Insert(ZESize Position, const char * String)")
	{
		ZEString StringA = "Lorem ";
		const char* StringB = "Ipsum";
		ZEUInt ExpectedLength = 11;
		ZESize Position = 6;

		StringA.Insert(Position, StringB);

		ZETestCheck(StringA == "Lorem Ipsum");
		ZETestCheck(StringA.GetLength() == ExpectedLength);

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Süt İç";
			const char* StringD = "Ilık ";

			StringC.Insert(5, StringD);

			ZETestCheck(StringC == "Işık Ilık Süt İç");
		}
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

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringD = "Işık Ilık Süt İç";
			StringD.Clear();

			ZETestCheck(StringD.IsEmpty());
		}
	}

	ZETest("ZEString ZEString::Left(ZESize Count) const")
	{
		ZEString String;

		ZETestCase("ZEString is empty")
		{
			ZETestCheck(String.Left(0) == "");
		}

		ZETestCase("ZEString has a value")
		{
			String = "1234Test5678";

			ZETestCheck(String.Left(0) == "");
			ZETestCheck(String.Left(1) == "1");
			ZETestCheck(String.Left(4) == "1234");
			ZETestCheck(String.Left(12) == "1234Test5678");
		}

		ZETestCase("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";

			ZETestCheck(String.Left(0) == "");
			ZETestCheck(String.Left(1) == "I");
			ZETestCheck(String.Left(4) == "Işık");
			ZETestCheck(String.Left(13) == "Işık Ilık Süt");
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

		ZETestCheck(StringA.Lower() == "lorem");
		ZETestCheck(StringB.Lower() == "ipsum");
		ZETestCheck(StringC.Lower() == "dolor");

		StringA = "";

		ZETestCheck(StringA.Lower() == "");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			//NO NON-ASCII Character Support at the moment.
			ZETestCheck(false);
		}
	}

	ZETest("void ZEString::LowerSelf()")
	{
		ZEString StringA = "lorem";
		ZEString StringB = "IPSUM";
		ZEString StringC = "DoLoR";

		StringA.LowerSelf();
		StringB.LowerSelf();
		StringC.LowerSelf();

		ZETestCheck(StringA == "lorem");
		ZETestCheck(StringB == "ipsum");
		ZETestCheck(StringC == "dolor");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			//NO NON-ASCII Character Support at the moment.
			ZETestCheck(false);
		}
	}

	ZETest("ZEString ZEString::Middle(ZESize Position, ZESize Count) const")
	{
		ZEString String;

		ZETestCase("ZEString is empty")
		{
			ZETestCheck(String.Middle(0, 0) == "");
		}

		ZETestCase("ZEString has a value")
		{
			String = "1234Test5678";

			ZETestCheck(String.Middle(0, 0) == "");
			ZETestCheck(String.Middle(2, 8) == "34Test56");
			ZETestCheck(String.Middle(8, 4) == "5678");
			ZETestCheck(String.Middle(0, 12) == "1234Test5678");
		}

		ZETestCase("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";

			ZETestCheck(String.Middle(0, 0) == "");
			ZETestCheck(String.Middle(2, 8) == "ık Ilık ");
			ZETestCheck(String.Middle(10, 6) == "Süt İç");
			ZETestCheck(String.Middle(0, 13) == "Işık Ilık Süt");
		}
	}

	ZETest("ZEString::operator const char*() const")
	{
		ZEString StringA = "Lorem Ipsum";
		const char* StringB = StringA;

		ZETestCheck(strcmp(StringB, StringA.GetValue()) == 0);

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Ilık Süt İç";
			const char* StringD = StringC;

			ZETestCheck(strcmp(StringD, StringC.GetValue()) == 0);
		}

	}

	ZETest("ZEString::operator std::string() const")
	{
		ZEString StringA = "Lorem Ipsum";

		std::string StringB = StringA;

		ZETestCheck(strcmp(StringB.c_str(), StringA.GetValue()) == 0);

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Ilık Süt İç";
			std::string StringD = StringC;

			ZETestCheck(strcmp(StringD.c_str(), StringC.GetValue()) == 0);
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

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringE = "Işık Ilık";
			ZEString StringF = " Süt İç";
			ZEString StringG = "Işık Ilık";

			ZETestCheck(StringE != StringF);
			ZETestCheck(!(StringE != StringG));
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

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringE = "Işık Ilık";
			const char* StringF = " Süt İç";
			const char* StringG = "Işık Ilık";

			ZETestCheck(StringE != StringF);
			ZETestCheck(!(StringE != StringG));
		}
	}

	ZETest("bool ZEString::operator!=(const wchar_t* String) const")
	{
		ZETestCase("Strings are not equal")
		{
			const ZEString StringA = "A";
			const wchar_t* StringB = L"B";

			ZETestCheck(StringA != StringB);
		}

		ZETestCase("Strings are equal")
		{
			const ZEString StringC = L"A";
			const wchar_t* StringD = L"A";

			ZETestCheck(!(StringC != StringD));
		}

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringE = "Işık Ilık Süt İç";
			wchar_t StringF[] = {8364, 0};
			const wchar_t* StringG = StringE.ToWCString();

			ZETestCheck(StringE != StringF);
			ZETestCheck(!(StringE != StringG));
		}
	}

	ZETest("bool ZEString::operator!=(const std::wstring& String) const")
	{
		ZETestCase("Strings are not equal")
		{
			const ZEString StringA = "A";
			std::wstring StringB = L"B";

			ZETestCheck(StringA != StringB);
		}

		ZETestCase("Strings are equal")
		{
			const ZEString StringC = L"A";
			std::wstring StringD = L"A";

			ZETestCheck(!(StringC != StringD));
		}

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringE = "Işık Ilık Süt İç";
			wchar_t Temp[] = {8364, 0};
			std::wstring StringF = std::wstring(Temp);
			std::wstring StringG = StringE.ToWCString();

			ZETestCheck(StringE != StringF);
			ZETestCheck(!(StringE != StringG));
		}
	}

	ZETest("char & ZEString::operator[](ZEInt Index)")
	{
		ZEString StringA = "Lorem";

		ZETestCheck(StringA[0] == 'L');

		ZETestCase("Changing a character of ZEString by index operator");
		{
			StringA[4] = 'M';

			ZETestCheck(StringA == "LoreM");
		}

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık Süt İç";

			ZETestCheck(StringA[1] == "ş");

			StringA[1] = "€";

			ZETestCheck(StringA[1] == "€");
		}
	}

	ZETest("const char & ZEString::operator[](ZEInt Index) const")
	{
		const ZEString StringA = "Lorem";

		const char C = StringA[2];

		ZETestCheck(StringA[2] == C);
		ZETestCheck(C == 'r');
	}

	ZETest("ZEString ZEString::operator+(const ZEString & String)")
	{
		ZEString StringA = "Lorem ";
		ZEString StringB = "Ipsum";

		ZEString Result = StringA + StringB;

		ZETestCheckEqual(Result, "Lorem Ipsum");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık ";
			StringB = "Süt İç";

			ZEString Result = StringA + StringB;

			ZETestCheck(Result == "Işık Ilık Süt İç");
		}
	}

	ZETest("ZEString ZEString::operator+(const char * String)")
	{
		ZEString StringA = "Lorem";
		const char* StringB = " Ipsum";

		ZEString Result = StringA + StringB;

		ZETestCheck(Result == "Lorem Ipsum");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık ";
			StringB = "Süt İç";

			ZEString Result = StringA + StringB;

			ZETestCheck(Result == "Işık Ilık Süt İç");
		}
	}

	ZETest("ZEString ZEString::operator+(const std::string& String)")
	{
		ZEString StringA = "Lorem";
		std::string StringB = " Ipsum";

		ZEString Result = StringA + StringB;

		ZETestCheck(Result == "Lorem Ipsum");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık ";
			StringB = "Süt İç";

			ZEString Result = StringA + StringB;

			ZETestCheck(Result == "Işık Ilık Süt İç");
		}
	}

	ZETest("ZEString ZEString::operator+(const wchar_t* String)")
	{
		ZEString StringA = "Lorem";
		const wchar_t* StringB = L"a";

		ZEString Result = StringA + StringB;

		ZETestCheck(Result == "Lorema");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "₭";
			wchar_t StringC[] = {8364, 0};

			ZEString Result = StringA + StringC;

			ZETestCheck(Result == "₭€");
		}
	}

	ZETest("ZEString ZEString::operator+(const std::wstring& String)")
	{
		ZEString StringA = "Lorem";
		std::wstring StringB = L"a";

		ZEString Result = StringA + StringB;

		ZETestCheck(Result == "Lorema");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "₭";
			wchar_t Temp[] = {8364, 0};
			std::wstring StringC = std::wstring(Temp);

			ZEString Result = StringA + StringC;

			ZETestCheck(Result == "₭€");
		}
	}

	ZETest("ZEString ZEString::operator+(ZEInt8 Value)")
	{
		ZEString String = "€ ";
		ZEInt8 Value = -120;

		ZEString Result = String + Value;

		ZETestCheck(Result == "€ -120");
	}

	ZETest("ZEString ZEString::operator+(ZEInt16 Value)")
	{
		ZEString String = "€ ";
		ZEInt16 Value = -6320;

		ZEString Result = String + Value;

		ZETestCheck(Result == "€ -6320");
	}

	ZETest("ZEString ZEString::operator+(ZEInt32 Value)")
	{
		ZEString String = "€ ";
		ZEInt32 Value = -987654;

		ZEString Result = String + Value;

		ZETestCheck(Result == "€ -987654");
	}

	ZETest("ZEString ZEString::operator+(ZEInt64 Value)")
	{
		ZEString String = "€ ";
		ZEInt64 Value = -987654321;

		ZEString Result = String + Value;

		ZETestCheck(Result == "€ -987654321");
	}

	ZETest("ZEString ZEString::operator+(ZEUInt8 Value)")
	{
		ZEString String = "€ ";
		ZEUInt8 Value = 120;

		ZEString Result = String + Value;

		ZETestCheck(Result == "€ 120");
	}

	ZETest("ZEString ZEString::operator+(ZEUInt16 Value)")
	{
		ZEString String = "€ ";
		ZEUInt16 Value = 6320;

		ZEString Result = String + Value;

		ZETestCheck(Result == "€ 6320");
	}

	ZETest("ZEString ZEString::operator+(ZEUInt32 Value)")
	{
		ZEString String = "€ ";
		ZEInt32 Value = 987654;

		ZEString Result = String + Value;

		ZETestCheck(Result == "€ 987654");
	}

	ZETest("ZEString ZEString::operator+(ZEUInt64 Value)")
	{
		ZEString String = "€ ";
		ZEInt64 Value = -987654321;

		ZEString Result = String + Value;

		ZETestCheck(Result == "€ -987654321");
	}

	ZETest("ZEString ZEString::operator+(float Value)")
	{
		ZEString String = "€ ";
		float Value = 98765.8564f;

		ZEString Result = String + Value;

		ZETestCheck(Result == "€ 98765.859375");
	}

	ZETest("ZEString ZEString::operator+(double Value)")
	{
		ZEString String = "€ ";
		double Value = 98765.8564;

		ZEString Result = String + Value;

		ZETestCheck(Result == "€ 98765.859375");
	}

	ZETest("ZEString & ZEString::operator+=(const ZEString & String)")
	{
		ZEString StringA = "Lorem ";
		ZEString StringB = "Ipsum";

		StringA += StringB;
		ZETestCheckEqual(StringA, "Lorem Ipsum");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık ";
			StringB = "Süt İç";

			StringA += StringB;

			ZETestCheck(StringA == "Işık Ilık Süt İç");
		}
	}

	ZETest("ZEString & ZEString::operator+=(const char * String)")
	{
		ZEString StringA = "Lorem ";
		const char* StringB = "Ipsum";

		StringA += StringB;
		ZETestCheck(StringA == "Lorem Ipsum");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık ";
			StringB = "Süt İç";

			StringA += StringB;

			ZETestCheck(StringA == "Işık Ilık Süt İç");
		}
	}

	ZETest("ZEString& ZEString::operator+=(const std::string& String)")
	{
		ZEString StringA = "Lorem ";
		std::string StringB = "Ipsum";

		StringA += StringB;
		ZETestCheck(StringA == "Lorem Ipsum");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık ";
			StringB = "Süt İç";

			StringA += StringB;

			ZETestCheck(StringA == "Işık Ilık Süt İç");
		}
	}

	ZETest("ZEString& ZEString::operator+=(const wchar_t* String)")
	{
		ZEString StringA = "Lorem";
		const wchar_t* StringB = L"a";

		StringA += StringB;

		ZETestCheck(StringA == "Lorema");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "₭";
			wchar_t StringC[] = {8364, 0};

			StringA += StringC;

			ZETestCheck(StringA == "₭€");
		}
	}

	ZETest("ZEString& ZEString::operator+=(const std::wstring& String)")
	{
		ZEString StringA = "Lorem";
		std::wstring StringB = L"a";

		StringA += StringB;

		ZETestCheck(StringA == "Lorema");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "₭";
			wchar_t Temp[] = {8364, 0};
			std::wstring StringC = std::wstring(Temp);

			StringA += StringC;

			ZETestCheck(StringA == "₭€");
		}
	}

	ZETest("ZEString& ZEString::operator+=(ZEInt8 Value)")
	{
		ZEString String = "€ ";
		ZEInt8 Value = -120;

		String += Value;

		ZETestCheck(String == "€ -120");
	}

	ZETest("ZEString& ZEString::operator+=(ZEInt16 Value)")
	{
		ZEString String = "€ ";
		ZEInt16 Value = -6320;

		String += Value;

		ZETestCheck(String == "€ -6320");
	}

	ZETest("ZEString& ZEString::operator+=(ZEInt32 Value)")
	{
		ZEString String = "€ ";
		ZEInt32 Value = -987654;

		String += Value;

		ZETestCheck(String == "€ -987654");
	}

	ZETest("ZEString& ZEString::operator+=(ZEInt64 Value)")
	{
		ZEString String = "€ ";
		ZEInt64 Value = -987654321;

		String += Value;

		ZETestCheck(String == "€ -987654321");
	}

	ZETest("ZEString& ZEString::operator+=(ZEUInt8 Value)")
	{
		ZEString String = "€ ";
		ZEUInt8 Value = 120;

		String += Value;

		ZETestCheck(String == "€ 120");
	}

	ZETest("ZEString& ZEString::operator+=(ZEUInt16 Value)")
	{
		ZEString String = "€ ";
		ZEUInt16 Value = 6320;

		String += Value;

		ZETestCheck(String == "€ 6320");
	}

	ZETest("ZEString& ZEString::operator+=(ZEUInt32 Value)")
	{
		ZEString String = "€ ";
		ZEInt32 Value = 987654;

		String += Value;

		ZETestCheck(String == "€ 987654");
	}

	ZETest("ZEString& ZEString::operator+=(ZEUInt64 Value)")
	{
		ZEString String = "€ ";
		ZEInt64 Value = 987654321;

		String += Value;

		ZETestCheck(String == "€ 987654321");
	}

	ZETest("ZEString& ZEString::operator+=(float Value)")
	{
		ZEString String = "€ ";
		float Value = 98765.8564f;

		String += Value;

		ZETestCheck(String == "€ 98765.859375");
	}

	ZETest("ZEString& ZEString::operator+=(double Value)")
	{
		ZEString String = "€ ";
		float Value = 98765.8564f;

		String += Value;

		ZETestCheck(String == "€ 98765.859375");
	}

	ZETest("ZEString & ZEString::operator=(const ZEString & String)")
	{
		ZEString StringA = "Lorem";
		ZEString StringB = "Ipsum";

		StringA = StringB;

		ZETestCheck(StringA == "Ipsum");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık";
			StringB = "Süt İç";

			StringA = StringB;

			ZETestCheck(StringA == "Süt İç");
		}
	}

	ZETest("ZEString & ZEString::operator=(const char * String)")
	{
		ZEString StringA = "Lorem";
		const char* StringB = "Ipsum";

		StringA = StringB;

		ZETestCheck(StringA == "Ipsum");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık";
			StringB = "Süt İç";

			StringA = StringB;

			ZETestCheck(StringA == "Süt İç");
		}
	}

	ZETest("ZEString& ZEString::operator=(const std::string& String)")
	{
		ZEString StringA = "Lorem";
		std::string StringB = "Ipsum";

		StringA = StringB;

		ZETestCheck(StringA == "Ipsum");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık";
			StringB = "Süt İç";

			StringA = StringB;

			ZETestCheck(StringA == "Süt İç");
		}
	}

	ZETest("ZEString& ZEString::operator=(const wchar_t* String)")
	{
		ZEString StringA = "Lorem";
		const wchar_t* StringB = L"a";

		StringA = StringB;

		ZETestCheck(StringA == "a");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "₭";
			wchar_t StringC[] = {8364, 0};

			StringA = StringC;

			ZETestCheck(StringA == "€");
		}
	}

	ZETest("ZEString& ZEString::operator=(const std::wstring& String)")
	{
		ZEString StringA = "Lorem";
		std::wstring StringB = L"a";

		StringA = StringB;

		ZETestCheck(StringA == "a");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "₭";
			wchar_t Temp[] = {8364, 0};
			std::wstring StringC = std::wstring(Temp);

			StringA = StringC;

			ZETestCheck(StringA == "€");
		}
	}

	ZETest("ZEString& ZEString::operator=(ZEInt8 Value)")
	{
		ZEInt8 Value = -20;

		ZEString String = Value;

		ZETestCheck(String == "-20");
	}

	ZETest("ZEString& ZEString::operator=(ZEInt16 Value)")
	{
		ZEInt32 Value = -8765;

		ZEString String = Value;

		ZETestCheck(String == "-8765");
	}

	ZETest("ZEString& ZEString::operator=(ZEInt32 Value)")
	{
		ZEInt32 Value = -87654321;

		ZEString String = Value;

		ZETestCheck(String == "-87654321");
	}

	ZETest("ZEString& ZEString::operator=(ZEInt64 Value)")
	{
		ZEInt64 Value = -87654321;

		ZEString String = Value;

		ZETestCheck(String == "-87654321");
	}

	ZETest("ZEString& ZEString::operator=(ZEUInt8 Value)")
	{
		ZEUInt8 Value = 20;

		ZEString String = Value;

		ZETestCheck(String == "20");
	}

	ZETest("ZEString& ZEString::operator=(ZEUInt16 Value)")
	{
		ZEUInt16 Value = 8765;

		ZEString String = Value;

		ZETestCheck(String == "8765");
	}

	ZETest("ZEString& ZEString::operator=(ZEUInt32 Value)")
	{
		ZEUInt32 Value = 87654321;

		ZEString String = Value;

		ZETestCheck(String == "87654321");
	}

	ZETest("ZEString& ZEString::operator=(ZEUInt64 Value)")
	{
		ZEUInt64 Value = 87654321;

		ZEString String = Value;

		ZETestCheck(String == "87654321");
	}

	ZETest("ZEString& ZEString::operator=(float Value)")
	{
		float Value = 400.5525252f;

		ZEString String = Value;

		ZETestCheck(String == "400.552521");
	}

	ZETest("ZEString& ZEString::operator=(double Value)")
	{
		double Value = 400.5525252;

		ZEString String = Value;

		ZETestCheck(String == "400.552521");
	}

	ZETest("bool ZEString::operator==(const ZEString & String) const")
	{
		ZETestCase("ZEStrings are not equal")
		{
			ZEString StringA = "Lorem";
			ZEString StringB = "Ipsum";

			ZETestCheck(!(StringA == StringB));
		}
		ZETestCase("ZEStrings are equal")
		{
			ZEString StringC = "Lorem Ipsum";
			ZEString StringD = "Lorem Ipsum";

			ZETestCheck(StringC == StringD);
		}

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringE = "Işık Ilık";
			ZEString StringF = "Süt İç";
			ZEString StringG = "Işık Ilık";

			ZETestCheck(StringE == StringG);
			ZETestCheck(!(StringE == StringF));
		}

		ZETestCase("Strings are not equal (Wide Character)")
		{
			ZEString StringA = "Lorem";
			const wchar_t* StringB = L"Ipsum";

			ZETestCheck(!(StringA == StringB));
		}

		ZETestCase("Strings are equal (Wide Character)")
		{
			ZEString StringA = "Lorem Ipsum";
			const wchar_t* StringB = L"Lorem Ipsum";

			ZETestCheck(StringA == StringB);
		}

		ZETestCase("UTF-8 encoding compatibility test (Wide Character)")
		{
			ZEString StringE = "€";
			wchar_t StringF[] = {8365, 0};
			wchar_t StringG[] = {8364, 0};

			ZETestCheck(StringE == StringG);
			ZETestCheck(!(StringE == StringF));
		}

		ZETestCase("Strings are not equal (Wide Standard String)")
		{
			ZEString StringA = "Lorem";
			std::wstring StringB = L"Ipsum";

			ZETestCheck(!(StringA == StringB));
		}

		ZETestCase("Strings are equal (Wide Standard String)")
		{
			ZEString StringA = "Lorem Ipsum";
			std::wstring StringB = L"Lorem Ipsum";

			ZETestCheck(StringA == StringB);
		}

		ZETestCase("UTF-8 encoding compatibility test (Wide Standard String)")
		{
			ZEString StringE = "€";
			wchar_t Temp1[] = {8365, 0};
			std::wstring StringF = std::wstring(Temp1);
			wchar_t Temp2[] = {8364, 0};
			std::wstring StringG = std::wstring(Temp2);

			ZETestCheck(StringE == StringG);
			ZETestCheck(!(StringE == StringF));
		}
	}

	ZETest("bool ZEString::operator==(const char * String) const")
	{
		ZETestCase("Strings are not equal")
		{
			ZEString StringA = "Lorem";
			const char* StringB = "Ipsum";

			ZETestCheck(!(StringA == StringB));
		}

		ZETestCase("Strings are equal")
		{
			ZEString StringA = "Lorem Ipsum";
			const char* StringB = "Lorem Ipsum";

			ZETestCheck(StringA == StringB);
		}

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringE = "Işık Ilık";
			const char* StringF = "Süt İç";
			const char* StringG = "Işık Ilık";

			ZETestCheck(StringE == StringG);
			ZETestCheck(!(StringE == StringF));
		}
    }

	ZETest("bool ZEString::operator==(const std::string& String) const")
	{
		ZETestCase("Strings are not equal")
		{
			ZEString StringA = "Lorem";
			std::string StringB = "Ipsum";

			ZETestCheck(!(StringA == StringB));
		}

		ZETestCase("Strings are equal")
		{
			ZEString StringA = "Lorem Ipsum";
			std::string StringB = "Lorem Ipsum";

			ZETestCheck(StringA == StringB);
		}

		ZETestCase("UTF-8 encoding compatibility test")
		{
			ZEString StringE = "Işık Ilık";
			std::string StringF = "Süt İç";
			std::string StringG = "Işık Ilık";

			ZETestCheck(StringE == StringG);
			ZETestCheck(!(StringE == StringF));
		}
	}

	ZETest("void ZEString::Remove(ZESize Position, ZEUInt Count = 1)")
	{
		ZEString String = "1234567890";
		ZESize Position = 3;
		ZEUInt Count = 5;

		String.Remove(Position, Count);
		ZETestCheck(String == "12390");
		ZETestCheck(String.GetLength() == 5);

		ZETestCase("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";
			Position = 9;
			Count = 7;

			String.Remove(Position, Count);
			ZETestCheck(String == "Işık Ilık");
			ZETestCheck(String.GetLength() == 9);
		}
	}

	ZETest("ZEString ZEString::Right(ZESize Count) const")
	{
		ZEString String;

		ZETestCase("ZEString is empty")
		{
			ZETestCheck(String.Right(0) == "");
		}
		
		ZETestCase("ZEString has a value")
		{
			String = "1234Test5678";

			ZETestCheck(String.Right(0) == "");
			ZETestCheck(String.Right(1) == "8");
			ZETestCheck(String.Right(4) == "5678");
			ZETestCheck(String.Right(12) == "1234Test5678");
		}

		ZETestCase("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";

			ZETestCheck(String.Right(0) == "");
			ZETestCheck(String.Right(1) == "ç");
			ZETestCheck(String.Right(6) == "Süt İç");
			ZETestCheck(String.Right(11) == "Ilık Süt İç");
		}
	}

	ZETest("void ZEString::SetBuffer(void* Buffer, ZESize Size)")
	{
		void* Buffer = new unsigned char[1];

		ZEString String("String");
		ZETestCheck(String.GetSize() == 7);

		String.SetBuffer(Buffer, 1);
		ZETestCheck(String == "");
		ZETestCheck(String.GetSize() == 1);
	}

	ZETest("void ZEString::SetCharacter(ZESize Position, char Value)")
	{
		ZEString String = "0123456789";
		char Value = 'X';
		ZESize Position = 5;

		String.SetCharacter(Position, Value);
		ZETestCheck(String == "01234X6789");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";
			const char* Value2 = "€";

			String.SetCharacter(15, Value2);
			ZETestCheck(String[15] == "€");
			ZETestCheck(String == "Işık Ilık Süt İ€");
		}
	}

	ZETest("void ZEString::SetValue(const char* String)")
	{
		ZEString StringA;
		const char* StringB = "Lorem";

		StringA.SetValue(StringB);

		ZETestCheck(StringA == "Lorem");
	}

	ZETest("void ZEString::SetValue(const wchar_t* String)")
	{
		ZEString StringA;
		wchar_t StringB[] = {8364, 0};

		StringA.SetValue(StringB);

		ZETestCheck(StringA == "€");
	}

	ZETest("void ZEString::SetValue(const ZEString& String)")
	{
		ZEString StringA("Test");
		ZETestCheck(StringA.GetSize() == 5);
		ZEString StringB("String");
		ZETestCheck(StringB.GetSize() == 7);

		StringA.SetValue(StringB);
		ZETestCheck(StringA == "String");
		ZETestCheck(StringA.GetSize() == 7);
	}

	ZETest("void ZEString::SetValue(const ZECharacter& Character)")
	{
		ZEString String;
		const char* Temp = "€";
		ZECharacter Character = "€";

		String.SetValue(Character);

		ZETestCheck(String == "€");
	}

	ZETest("void ZEString::SetValue(const std::string& String)")
	{
		ZEString StringA;
		std::string StringB = "Lorem";

		StringA.SetValue(StringB);

		ZETestCheck(StringA == "Lorem");
	}

	ZETest("void ZEString::SetValue(const std::wstring& String)")
	{
		ZEString StringA;
		wchar_t Temp[] = {8364, 0};
		std::wstring StringB = std::wstring(Temp);

		StringA.SetValue(StringB);

		ZETestCheck(StringA == "€");
	}

	ZETest("void ZEString::SetValue(wchar_t Character)")
	{
		ZEString StringA;
		wchar_t Character = 8364;

		StringA.SetValue(Character);

		ZETestCheck(StringA == "€");
	}

	ZETest("void ZEString::SetValue(float Value, const char* Format)")
	{
		float Value = 400.5525252f;
		ZEString String;

		String.SetValue(Value);
		ZETestCheck(String == "400.552521");

		ZETestCase("for Format != NULL")
		{
			String.SetValue(Value, "-8.1");
			ZETestCheck(String == "400.6   ");
		}
	}

	ZETest("void ZEString::SetValue(double Value, const char* Format)")
	{
		double Value = 400.5525252;
		ZEString String;

		String.SetValue(Value);
		ZETestCheck(String == "400.552521");

		ZETestCase("for Format != NULL")
		{
			String.SetValue(Value, "10.3");
			ZETestCheck(String == "   400.553");
		}
	}

	ZETest("void ZEString::SetValue(ZEInt8 Value, const char* Format)")
	{
		ZEInt8 Value = -20;

		ZEString String;
		String.SetValue(Value);

		ZETestCheck(String == "-20");

		ZETestCase("for Format != NULL")
		{
			String.SetValue(Value, "x");
			ZETestCheck(String == "ffffffec");
		}
	}

	ZETest("void ZEString::SetValue(ZEInt16 Value, const char* Format)")
	{
		ZEInt16 Value = -8765;

		ZEString String;
		String.SetValue(Value);

		ZETestCheck(String == "-8765");

		ZETestCase("for Format != NULL")
		{
			String.SetValue(Value, "d:8");
			ZETestCheck(String == "   -8765");
		}
	}

	ZETest("void ZEString::SetValue(ZEInt32 Value, const char* Format)")
	{
		ZEInt32 Value = -87654321;

		ZEString String;
		String.SetValue(Value);

		ZETestCheck(String == "-87654321");

		ZETestCase("for Format != NULL")
		{
			String.SetValue(Value, ":");
			ZETestCheck(String == "-87654321");
		}
	}

	ZETest("void ZEString::SetValue(ZEInt64 Value, const char* Format)")
	{
		ZEInt64 Value = -87654321;

		ZEString String;
		String.SetValue(Value);

		ZETestCheck(String == "-87654321");

		ZETestCase("for Format != NULL")
		{
			String.SetValue(Value, "d:.12");
			ZETestCheck(String == "-000087654321");
		}
	}

	ZETest("void ZEString::SetValue(ZEUInt8 Value, const char* Format)")
	{
		ZEUInt8 Value = 90;

		ZEString String;
		String.SetValue(Value);

		ZETestCheck(String == "90");

		ZETestCase("for Format != NULL")
		{
			String.SetValue(Value, "c:4");
			ZETestCheck(String == "   Z");
		}
	}

	ZETest("void ZEString::SetValue(ZEUInt16 Value, const char* Format)")
	{
		ZEUInt16 Value = 8765;

		ZEString String;
		String.SetValue(Value);

		ZETestCheck(String == "8765");

		ZETestCase("for Format != NULL")
		{
			String.SetValue(Value, "X:-6");
			ZETestCheck(String == "223D  ");
		}
	}

	ZETest("void ZEString::SetValue(ZEUInt32 Value, const char* Format)")
	{
		ZEUInt32 Value = 987654;

		ZEString String;
		String.SetValue(Value);

		ZETestCheck(String == "987654");

		ZETestCase("for Format != NULL")
		{
			String.SetValue(Value, "d:.8");
			ZETestCheck(String == "00987654");
		}
	}

	ZETest("void ZEString::SetValue(ZEUInt64 Value, const char* Format)")
	{
		ZEUInt64 Value = 987654321;

		ZEString String;
		String.SetValue(Value);

		ZETestCheck(String == "987654321");

		ZETestCase("for Format != NULL")
		{
			String.SetValue(Value, "X:-10");
			ZETestCheck(String == "3ADE68B1  ");
		}
	}

	ZETest("void ZEString::SetValue(bool Value, const char* Format)")
	{
		ZETestCase("ZEString value is set to True")
		{
			ZEString StringA;

			bool Value = 1;

			StringA.SetValue(Value);

			ZETestCheck(StringA == "true");
		}
		ZETestCase("ZEString value is set to False")
		{
			ZEString StringB;

			bool Value = 0;

			StringB.SetValue(Value);

			ZETestCheck(StringB == "false");
		}

		ZETestCase("for Format != NULL")
		{
			ZEString StringC;

			bool Value = 1;

			StringC.SetValue(Value, "false");
			ZETestCheck(StringC == "false");
		}
	}

	ZETest("void ZEString::SetValue(void* Value, ZESize Size)")
	{
		void* Value = "Value";
		ZEString String("String");
		ZETestCheck(String.GetSize() == 7);

		String.SetValue(Value, 0);
		ZETestCheck(String == "");
		ZETestCheck(String.GetSize() == 1);

		String.SetValue(Value, 1);
		ZETestCheck(String == "V");
		ZETestCheck(String.GetSize() == 2);

		String.SetValue(Value, 5);
		ZETestCheck(String == "Value");
		ZETestCheck(String.GetSize() == 6);
	}

	ZETest("ZEString ZEString::SubString(ZESize StartPosition, ZESize EndPosition) const")
	{
		ZEString String;

		ZETestCase("ZEString is empty")
		{
			ZETestCheck(String.SubString(0, 0) == "");
		}

		ZETestCase("ZEString has a value")
		{
			String = "1234Test5678";
			ZETestCheck(String.SubString(0, 0) == "1");
			ZETestCheck(String.SubString(0, 1) == "12");
			ZETestCheck(String.SubString(0, 3) == "1234");

			ZETestCheck(String.SubString(4, 4) == "T");
			ZETestCheck(String.SubString(4, 5) == "Te");
			ZETestCheck(String.SubString(4, 7) == "Test");

			ZETestCheck(String.SubString(8, 8) == "5");
			ZETestCheck(String.SubString(8, 9) == "56");
			ZETestCheck(String.SubString(8, 11) == "5678");
		}

		ZETestCase("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";

			ZETestCheck(String.SubString(0, 0) == "I");
			ZETestCheck(String.SubString(0, 1) == "Iş");
			ZETestCheck(String.SubString(0, 3) == "Işık");

			ZETestCheck(String.SubString(5, 5) == "I");
			ZETestCheck(String.SubString(5, 8) == "Ilık");
			ZETestCheck(String.SubString(5, 12) == "Ilık Süt");

			ZETestCheck(String.SubString(9, 12) == " Süt");
			ZETestCheck(String.SubString(10, 13) == "Süt ");
			ZETestCheck(String.SubString(10, 15) == "Süt İç");
		}
	}

	ZETest("const char* ZEString::ToCString() const")
	{
		ZEString StringA = "Lorem Ipsum";

		const char* StringB = StringA.ToCString();

		ZETestCheck(StringA == StringB);

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık Süt İç";

			StringB = StringA.ToCString();

			ZETestCheck(StringA == StringB);
		}
	}

	ZETest("std::string ZEString::ToStdString() const")
	{
		ZEString StringA = "Lorem Ipsum";

		std::string StringB = StringA.ToStdString();

		ZETestCheck(StringA == StringB);

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık Süt İç";

			StringB = StringA.ToStdString();

			ZETestCheck(StringA == StringB);
		}
	}

	ZETest("const wchar_t* ZEString::ToWCString()")
	{
		ZEString StringA = "Lorem Ipsum";

		const wchar_t* StringB = StringA.ToWCString();

		ZETestCheck(StringA == StringB);

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık Süt İç";

			StringB = StringA.ToWCString();

			ZETestCheck(StringA == StringB);
		}
	}

	ZETest("std::string ZEString::ToStdString() const")
	{
		ZEString StringA = "Lorem Ipsum";

		std::wstring StringB = StringA.ToWStdString();

		ZETestCheck(StringA == StringB);

		ZETestCase("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık Süt İç";

			StringB = StringA.ToWStdString();

			ZETestCheck(StringA == StringB);
		}
	}

	ZETest("float ZEString::ToFloat()")
	{
		ZEString String = "-255.046";

		float Value = String.ToFloat();

		ZETestCheckClose(Value, -255.04601f);
	}

	ZETest("double ZEString::ToDouble() const")
	{
		ZEString String = "-255.046";

		double Value = String.ToDouble();

		ZETestCheckClose(Value, -255.04601f);
	}

	ZETest("ZEInt8 ZEString::ToInt8() const")
	{
		ZEString StringA = "120";
		ZEString StringB = "-120.043";

		ZEInt8 ValueA = StringA.ToInt8();
		ZEInt8 ValueB = StringB.ToInt8();

		ZETestCheckEqual(ValueA, 120);
		ZETestCheckEqual(ValueB, -120);
	}

	ZETest("ZEInt16 ZEString::ToInt16() const")
	{
		ZEString StringA = "255";
		ZEString StringB = "-255.043";

		ZEInt16 ValueA = StringA.ToInt16();
		ZEInt16 ValueB = StringB.ToInt16();

		ZETestCheckEqual(ValueA, 255);
		ZETestCheckEqual(ValueB, -255);
	}

	ZETest("ZEInt32 ZEString::ToInt32() const")
	{
		ZEString StringA = "255";
		ZEString StringB = "-255.043";

		ZEInt32 ValueA = StringA.ToInt32();
		ZEInt32 ValueB = StringB.ToInt32();

		ZETestCheckEqual(ValueA, 255);
		ZETestCheckEqual(ValueB, -255);
	}

	ZETest("ZEInt64 ZEString::ToInt64() const")
	{
		ZEString StringA = "255";
		ZEString StringB = "-255.043";

		ZEInt64 ValueA = StringA.ToInt64();
		ZEInt64 ValueB = StringB.ToInt64();

		ZETestCheckEqual(ValueA, 255);
		ZETestCheckEqual(ValueB, -255);
	}

	ZETest("ZEUInt8 ZEString::ToUInt8() const")
	{
		ZEString String = "123";
		ZEUInt8 ExpectedValue = 123;

		ZEUInt8 Value = String.ToUInt8();

		ZETestCheckEqual(Value, ExpectedValue);
	}

	ZETest("ZEUInt16 ZEString::ToUInt16() const")
	{
		ZEString String = "123";
		ZEUInt16 ExpectedValue = 123;

		ZEUInt16 Value = String.ToUInt16();

		ZETestCheckEqual(Value, ExpectedValue);
	}

	ZETest("ZEUInt32 ZEString::ToUInt32() const")
	{
		ZEString String = "123456";
		ZEUInt32 ExpectedValue = 123456;

		ZEUInt32 Value = String.ToUInt32();

		ZETestCheckEqual(Value, ExpectedValue);
	}

	ZETest("ZEUInt64 ZEString::ToUInt64() const")
	{
		ZEString String = "123456";
		ZEUInt64 ExpectedValue = 123456;

		ZEUInt64 Value = String.ToUInt64();

		ZETestCheckEqual(Value, ExpectedValue);
	}

	ZETest("ZEString ZEString::Trim() const")
	{
		ZEString String;
		ZETestCheck(String.Trim() == "");

		String = "    ";
		ZETestCheck(String.Trim() == "");

		String = "Lorem Ipsum";
		ZETestCheck(String.Trim() == "Lorem Ipsum");

		String = "   Lorem Ipsum   ";
		ZETestCheck(String.Trim() == "Lorem Ipsum");

		String = "\t\tLorem Ipsum\t\t";
		ZETestCheck(String.Trim() == "Lorem Ipsum");

		String = " \t \tLorem Ipsum \t \t";
		ZETestCheck(String.Trim() == "Lorem Ipsum");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";
			ZETestCheck(String.Trim() == "Işık Ilık Süt İç");

			String = "   Işık Ilık Süt İç   ";
			ZETestCheck(String.Trim() == "Işık Ilık Süt İç");

			String = "\t\tIşık Ilık Süt İç\t\t";
			ZETestCheck(String.Trim() == "Işık Ilık Süt İç");

			String = " \t \tIşık Ilık Süt İç \t \t";
			ZETestCheck(String.Trim() == "Işık Ilık Süt İç");
		}
	}

	ZETest("void ZEString::TrimSelf()")
	{
		ZEString String;

		String.TrimSelf();
		ZETestCheck(String == "");

		String = "    ";
		String.TrimSelf();
		ZETestCheck(String == "");

		String = "Lorem Ipsum";
		String.TrimSelf();
		ZETestCheck(String == "Lorem Ipsum");

		String = "   Lorem Ipsum   ";
		String.TrimSelf();
		ZETestCheck(String == "Lorem Ipsum");

		String = "\t\tLorem Ipsum\t\t";
		String.TrimSelf();
		ZETestCheck(String == "Lorem Ipsum");

		String = " \t \tLorem Ipsum \t \t";
		String.TrimSelf();
		ZETestCheck(String == "Lorem Ipsum");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";
			String.TrimSelf();
			ZETestCheck(String == "Işık Ilık Süt İç");

			String = "   Işık Ilık Süt İç   ";
			String.TrimSelf();
			ZETestCheck(String == "Işık Ilık Süt İç");

			String = "\t\tIşık Ilık Süt İç\t\t";
			String.TrimSelf();
			ZETestCheck(String == "Işık Ilık Süt İç");

			String = " \t \tIşık Ilık Süt İç \t \t";
			String.TrimSelf();
			ZETestCheck(String == "Işık Ilık Süt İç");
		}
	}

	ZETest("ZEString ZEString::TrimLeft() const")
	{
		ZEString String;
		ZETestCheck(String.TrimLeft() == "");

		String = "    ";
		ZETestCheck(String.TrimLeft() == "");

		String = "Lorem Ipsum";
		ZETestCheck(String.TrimLeft() == "Lorem Ipsum");

		String = "   Lorem Ipsum   ";
		ZETestCheck(String.TrimLeft() == "Lorem Ipsum   ");

		String = "\t\tLorem Ipsum\t\t";
		ZETestCheck(String.TrimLeft() == "Lorem Ipsum\t\t");

		String = " \t \tLorem Ipsum \t \t";
		ZETestCheck(String.TrimLeft() == "Lorem Ipsum \t \t");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";
			ZETestCheck(String.TrimLeft() == "Işık Ilık Süt İç");

			String = "   Işık Ilık Süt İç   ";
			ZETestCheck(String.TrimLeft() == "Işık Ilık Süt İç   ");

			String = "\t\tIşık Ilık Süt İç\t\t";
			ZETestCheck(String.TrimLeft() == "Işık Ilık Süt İç\t\t");

			String = " \t \tIşık Ilık Süt İç \t \t";
			ZETestCheck(String.TrimLeft() == "Işık Ilık Süt İç \t \t");
		}
	}

	ZETest("void ZEString::TrimLeftSelf()")
	{
		ZEString String;

		String.TrimLeftSelf();
		ZETestCheck(String == "");

		String = "    ";
		String.TrimLeftSelf();
		ZETestCheck(String == "");

		String = "Lorem Ipsum";
		String.TrimLeftSelf();
		ZETestCheck(String == "Lorem Ipsum");

		String = "   Lorem Ipsum   ";
		String.TrimLeftSelf();
		ZETestCheck(String == "Lorem Ipsum   ");

		String = "\t\tLorem Ipsum\t\t";
		String.TrimLeftSelf();
		ZETestCheck(String == "Lorem Ipsum\t\t");

		String = " \t \tLorem Ipsum \t \t";
		String.TrimLeftSelf();
		ZETestCheck(String == "Lorem Ipsum \t \t");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";
			String.TrimLeftSelf();
			ZETestCheck(String == "Işık Ilık Süt İç");

			String = "   Işık Ilık Süt İç   ";
			String.TrimLeftSelf();
			ZETestCheck(String == "Işık Ilık Süt İç   ");

			String = "\t\tIşık Ilık Süt İç\t\t";
			String.TrimLeftSelf();
			ZETestCheck(String == "Işık Ilık Süt İç\t\t");

			String = " \t \tIşık Ilık Süt İç \t \t";
			String.TrimLeftSelf();
			ZETestCheck(String == "Işık Ilık Süt İç \t \t");
		}
	}

	ZETest("ZEString ZEString::TrimRight() const")
	{
		ZEString String;
		ZETestCheck(String.TrimRight() == "");

		String = "    ";
		ZETestCheck(String.TrimRight() == "");

		String = "Lorem Ipsum";
		ZETestCheck(String.TrimRight() == "Lorem Ipsum");

		String = "   Lorem Ipsum   ";
		ZETestCheck(String.TrimRight() == "   Lorem Ipsum");

		String = "\t\tLorem Ipsum\t\t";
		ZETestCheck(String.TrimRight() == "\t\tLorem Ipsum");

		String = " \t \tLorem Ipsum \t \t";
		ZETestCheck(String.TrimRight() == " \t \tLorem Ipsum");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";
			ZETestCheck(String.TrimRight() == "Işık Ilık Süt İç");

			String = "   Işık Ilık Süt İç   ";
			ZETestCheck(String.TrimRight() == "   Işık Ilık Süt İç");

			String = "\t\tIşık Ilık Süt İç\t\t";
			ZETestCheck(String.TrimRight() == "\t\tIşık Ilık Süt İç");

			String = " \t \tIşık Ilık Süt İç \t \t";
			ZETestCheck(String.TrimRight() == " \t \tIşık Ilık Süt İç");
		}
	}

	ZETest("void ZEString::TrimRightSelf()")
	{
		ZEString String;

		String.TrimRightSelf();
		ZETestCheck(String == "");

		String = "    ";
		String.TrimRightSelf();
		ZETestCheck(String == "");

		String = "Lorem Ipsum";
		String.TrimRightSelf();
		ZETestCheck(String == "Lorem Ipsum");

		String = "   Lorem Ipsum   ";
		String.TrimRightSelf();
		ZETestCheck(String == "   Lorem Ipsum");

		String = "\t\tLorem Ipsum\t\t";
		String.TrimRightSelf();
		ZETestCheck(String == "\t\tLorem Ipsum");

		String = " \t \tLorem Ipsum \t \t";
		String.TrimRightSelf();
		ZETestCheck(String == " \t \tLorem Ipsum");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";
			String.TrimRightSelf();
			ZETestCheck(String == "Işık Ilık Süt İç");

			String = "   Işık Ilık Süt İç   ";
			String.TrimRightSelf();
			ZETestCheck(String == "   Işık Ilık Süt İç");

			String = "\t\tIşık Ilık Süt İç\t\t";
			String.TrimRightSelf();
			ZETestCheck(String == "\t\tIşık Ilık Süt İç");

			String = " \t \tIşık Ilık Süt İç \t \t";
			String.TrimRightSelf();
			ZETestCheck(String == " \t \tIşık Ilık Süt İç");
		}
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

		ZETestCheck(StringA.Upper() == "LOREM");
		ZETestCheck(StringB.Upper() == "IPSUM");
		ZETestCheck(StringC.Upper() == "DOLOR");
		ZETestCheck(StringD.Upper() == "");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			//NO NON-ASCII Character Support at the moment.
			ZETestCheck(false);
		}
	}

	ZETest("void ZEString::UpperSelf()")
	{
		ZEString StringA = "lorem";
		ZEString StringB = "IPSUM";
		ZEString StringC = "DoLoR";

		StringA.UpperSelf();
		StringB.UpperSelf();
		StringC.UpperSelf();

		ZETestCheck(StringA == "LOREM");
		ZETestCheck(StringB == "IPSUM");
		ZETestCheck(StringC == "DOLOR");

		ZETestCase("UTF-8 encoding compatibility test")
		{
			//NO NON-ASCII Character Support at the moment.
			ZETestCheck(false);
		}
	}

	ZETest("ZEString::ZEString(const char * String)")
	{
		const char* StringA = "Lorem Ipsum";

		ZEString StringB(StringA);

		ZETestCheck(StringB == "Lorem Ipsum");
	}

	ZETest("ZEString::ZEString(const wchar_t* String)")
	{
		const wchar_t* StringA = L"Lorem Ipsum";

		ZEString StringB(StringA);

		ZETestCheck(StringB == L"Lorem Ipsum");
	}

	ZETest("ZEString::ZEString(const std::string& String)")
	{
		std::string StringA = "Lorem Ipsum";

		ZEString StringB(StringA);

		ZETestCheck(StringB == "Lorem Ipsum");
	}

	ZETest("ZEString::ZEString(const std::wstring& String)")
	{
		std::wstring StringA = L"Lorem Ipsum";

		ZEString StringB(StringA);

		ZETestCheck(StringB == L"Lorem Ipsum");
	}

	ZETest("ZEString::ZEString(const ZEString & String)")
	{
		ZEString StringA = "Lorem Ipsum";

		ZEString StringB(StringA);

		ZETestCheck(StringB == "Lorem Ipsum");
	}

	ZETest("ZEString::ZEString(ZEInt8 Value)")
	{
		ZEInt8 Value = -20;

		ZEString StringB(Value);

		ZETestCheck(StringB == "-20");
	}

	ZETest("ZEString::ZEString(ZEInt16 Value)")
	{
		ZEInt16 Value = -8765;

		ZEString StringB(Value);

		ZETestCheck(StringB == "-8765");
	}

	ZETest("ZEString::ZEString(ZEInt32 Value)")
	{
		ZEInt32 Value = -87654;

		ZEString StringB(Value);

		ZETestCheck(StringB == "-87654");
	}

	ZETest("ZEString::ZEString(ZEInt64 Value)")
	{
		ZEInt64 Value = -87654321;

		ZEString StringB(Value);

		ZETestCheck(StringB == "-87654321");
	}

	ZETest("ZEString::ZEString(ZEUInt8 Value)")
	{
		ZEUInt8 Value = 20;

		ZEString StringB(Value);

		ZETestCheck(StringB == "20");
	}

	ZETest("ZEString::ZEString(ZEUInt16 Value)")
	{
		ZEUInt16 Value = 8765;

		ZEString StringB(Value);

		ZETestCheck(StringB == "8765");
	}

	ZETest("ZEString::ZEString(ZEUInt32 Value)")
	{
		ZEUInt32 Value = 87654;

		ZEString StringB(Value);

		ZETestCheck(StringB == "87654");
	}

	ZETest("ZEString::ZEString(ZEUInt64 Value)")
	{
		ZEUInt64 Value = 87654321;

		ZEString StringB(Value);

		ZETestCheck(StringB == "87654321");
	}

	ZETest("ZEString::ZEString(float Value)")
	{
		float Value = 400.5525252f;

		ZEString String(Value);

		ZETestCheck(String == "400.552521");
	}

	ZETest("ZEString::ZEString(double Value)")
	{
		double Value = 400.5525252;

		ZEString String(Value);

		ZETestCheck(String == "400.552521");
	}

	ZETest("ZEString operator+(const char* String1, const ZEString& String2)")
	{
		ZEString StringA = "Ipsum";
		const char* StringB = "Lorem ";
		ZEString Result;

		Result = StringB + StringA;

		ZETestCheck(Result == "Lorem Ipsum");
	}

	ZETest("ZEString operator+(const wchar_t* String1, const ZEString& String2)")
	{
		ZEString StringA = "Ipsum";
		const wchar_t* StringB = L"Lorem ";
		ZEString Result;

		Result = StringB + StringA;

		ZETestCheck(Result == "Lorem Ipsum");
	}

	ZETest("ZEString operator+(ZEInt8 Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		ZEInt8 StringB = -20;
		ZEString Result;

		Result = StringB + StringA;

		ZETestCheck(Result == "-20 €");
	}

	ZETest("ZEString operator+(ZEInt16 Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		ZEInt16 StringB = -8765;
		ZEString Result;

		Result = StringB + StringA;

		ZETestCheck(Result == "-8765 €");
	}

	ZETest("ZEString operator+(ZEInt32 Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		ZEInt32 StringB = -87654;
		ZEString Result;

		Result = StringB + StringA;

		ZETestCheck(Result == "-87654 €");
	}

	ZETest("ZEString operator+(ZEInt64 Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		ZEInt64 StringB = -8765432;
		ZEString Result;

		Result = StringB + StringA;

		ZETestCheck(Result == "-8765432 €");
	}

	ZETest("ZEString operator+(ZEUInt8 Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		ZEUInt8 StringB = 20;
		ZEString Result;

		Result = StringB + StringA;

		ZETestCheck(Result == "20 €");
	}

	ZETest("ZEString operator+(ZEUInt16 Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		ZEUInt16 StringB = 8765;
		ZEString Result;

		Result = StringB + StringA;

		ZETestCheck(Result == "8765 €");
	}

	ZETest("ZEString operator+(ZEUInt32 Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		ZEUInt32 StringB = 87654;
		ZEString Result;

		Result = StringB + StringA;

		ZETestCheck(Result == "87654 €");
	}

	ZETest("ZEString operator+(ZEUInt64 Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		ZEUInt64 StringB = 8765432;
		ZEString Result;

		Result = StringB + StringA;

		ZETestCheck(Result == "8765432 €");
	}

	ZETest("ZEString operator+(float Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		float StringB = 400.5525252f;
		ZEString Result;

		Result = StringB + StringA;

		ZETestCheck(Result == "400.552521 €");
	}

	ZETest("ZEString operator+(double Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		double StringB = 400.5525252;
		ZEString Result;

		Result = StringB + StringA;

		ZETestCheck(Result == "400.552521 €");
	}

	ZETest("bool operator==(const char* String1, const ZEString& String2)")
	{
		ZETestCase("Strings are not equal")
		{
			const ZEString StringA = "Lorem";
			const char* StringB = "Ipsum";

			ZETestCheck(!(StringB == StringA));
		}

		ZETestCase("Strings are equal")
		{
			const ZEString StringA = "Lorem Ipsum";
			const char* StringB = "Lorem Ipsum";

			ZETestCheck(StringB == StringA);
		}
	}

	ZETest("bool operator==(const wchar_t* String1, const ZEString& String2)")
	{
		ZETestCase("Strings are not equal")
		{
			const ZEString StringA = "Lorem";
			const wchar_t* StringB = L"Ipsum";

			ZETestCheck(!(StringB == StringA));
		}

		ZETestCase("Strings are equal")
		{
			const ZEString StringA = "Lorem Ipsum";
			const wchar_t* StringB = L"Lorem Ipsum";

			ZETestCheck(StringB == StringA);
		}
	}

	ZETest("bool operator!=(const char* String1, const ZEString& String2)")
	{
		ZETestCase("Strings are not equal")
		{
			const ZEString StringA = "Lorem";
			const char* StringB = "Ipsum";

			ZETestCheck(StringB != StringA);
		}

		ZETestCase("Strings are equal")
		{
			const ZEString StringC = "Lorem";
			const char* StringD = "Lorem";

			ZETestCheck(!(StringD != StringC));
		}
	}

	ZETest("bool operator!=(const wchar_t* String1, const ZEString& String2)")
	{
		ZETestCase("Strings are not equal")
		{
			const ZEString StringA = "Lorem";
			const wchar_t* StringB = L"Ipsum";

			ZETestCheck(StringB != StringA);
		}

		ZETestCase("Strings are equal")
		{
			const ZEString StringC = "Lorem";
			const wchar_t* StringD = L"Lorem";

			ZETestCheck(!(StringD != StringC));
		}
	}

	ZETest("bool operator!=(const wchar_t* String1, const ZEString& String2)")
	{
		ZETestCase("Strings are not equal")
		{
			const ZEString StringA = "Lorem";
			const wchar_t* StringB = L"Ipsum";

			ZETestCheck(StringB != StringA);
		}

		ZETestCase("Strings are equal")
		{
			const ZEString StringC = "Lorem";
			const wchar_t* StringD = L"Lorem";

			ZETestCheck(!(StringD != StringC));
		}
	}
}
